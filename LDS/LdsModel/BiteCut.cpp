// BiteCut.cpp: implementation of the CFlatBiteCut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f_ent.h"
#include "f_ent_list.h"
#include "f_alg_fun.h"
#include "BiteCut.h"
#include "ArrayList.h"
#include "LogFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//通过一个不封闭的折线（逆时针遍历时左侧为待切割侧），在一个指定平面的侧边上咬切掉一个缺口
bool CFlatBiteCut::Cut(GEPOINT* xarrCutPolyVertexes,int count,
	f3dPolyFace *pTopFace, f3dPolyFace *pBtmFace,fBody *pBody)
{
	if(pTopFace==NULL||pBtmFace==NULL||pBody==NULL)
		return FALSE;
	f3dAtomLine* pEdgeLine;
	//1.提取割口平面法线
	GEPOINT vPlaneNormal=pTopFace->poly_norm;
	if(vPlaneNormal.IsZero())
	{
		GEPOINT vValidEdgeVec1,vValidEdgeVec2;
		double dfSquareLen1=0,dfSquareLen2=0,MINVALID_LENGTH_SQUARE=4;
		for(pEdgeLine=pTopFace->outer_edge.GetFirst();pEdgeLine;pEdgeLine=pTopFace->outer_edge.GetNext())
		{
			if(vValidEdgeVec1.IsZero()||dfSquareLen1<MINVALID_LENGTH_SQUARE)
			{
				vValidEdgeVec1=(*pEdgeLine->pEnd)-(*pEdgeLine->pStart);
				dfSquareLen1=vValidEdgeVec1.mod2();
			}
			else if(vValidEdgeVec2.IsZero()||dfSquareLen2<MINVALID_LENGTH_SQUARE)
			{
				vValidEdgeVec2=(*pEdgeLine->pEnd)-(*pEdgeLine->pStart);
				dfSquareLen2=vValidEdgeVec2.mod2();
			}
			else
			{
				normalize(vValidEdgeVec1);
				normalize(vValidEdgeVec2);
				if(fabs(vValidEdgeVec1*vValidEdgeVec2)>0.98)
				{	//两边夹角不能过小
					vValidEdgeVec2.Set();
					dfSquareLen2=0;
					continue;
				}
				break;
			}
		}
		vPlaneNormal=vValidEdgeVec1^vValidEdgeVec2;
		if(!normalize(vPlaneNormal))
			return false;	//无法找到割口平面法线
	}
	//2.以割口顶面为基准生成平面坐标系
	f3dLine xBaseLine,xCurrLine;
	pEdgeLine=pTopFace->outer_edge.GetFirst();
	xBaseLine.startPt=*pEdgeLine->pStart;
	xBaseLine.endPt=*pEdgeLine->pEnd;
	GECS acs;
	acs.origin=xBaseLine.startPt;
	acs.axis_x=xBaseLine.endPt-xBaseLine.startPt;
	normalize(acs.axis_x);
	acs.axis_z=vPlaneNormal;
	acs.axis_y=acs.axis_z^acs.axis_x;
	//3.计算顶面切割交点，生成切割壁折线顶点表
	PRESET_ARRAY32<GEPOINT> xarrLocalCutPolyVertexes;	//切割多边形顶点在平板坐标系下的局部坐标
	PRESET_ARRAY32<INTERS_POINT> xarrCutWallTopVertexes;//切割壁在顶面的折线顶点（原坐标系）
	PRESET_ARRAY32<INTERS_POINT> xarrCutWallBtmVertexes;//切割壁在底面的折线顶点（原坐标系）
	ARRAY_LIST<f3dPoint*> xarrFinalVertexPtrs;
	int i,retcode;
	for(i=0;i<count;i++)
	{
		ProjectPoint(xarrCutPolyVertexes[i],acs.origin,acs.axis_z);
		GEPOINT point=acs.TransPToCS(xarrCutPolyVertexes[i]);
		xarrLocalCutPolyVertexes.Append(point);
	}
	f3dAtomLine atomline;
	f3dAtomLine xTopCutEdge,xBtmCutEdge;	//切割口所在的顶平面边线和底平面边线
	INTERS_POINT xStartInters,xEndInters,*pWallVertex;
	for(pEdgeLine=pTopFace->outer_edge.GetFirst();pEdgeLine;pEdgeLine=pTopFace->outer_edge.GetNext())
	{
		xCurrLine.startPt=acs.TransPToCS(*pEdgeLine->pStart);
		xCurrLine.endPt=acs.TransPToCS(*pEdgeLine->pEnd);
		GEPOINT inters;
		xStartInters.index=xEndInters.index=-1;	//重置为未找到状态码#-1
		for(i=1;i<count;i++)
		{
			f3dLine xCutLine(xarrLocalCutPolyVertexes.At(i-1),xarrLocalCutPolyVertexes.At(i));
			if((retcode=Int2dll(f2dLine(xCutLine.startPt,xCutLine.endPt),f2dLine(xCurrLine.startPt,xCurrLine.endPt),
				inters.x,inters.y))!=1)
				continue;
			if(xStartInters.index<0)
			{
				xStartInters.index=i-1;
				xStartInters.position=inters;
			}
			else
			{
				xEndInters.index=i-1;
				xEndInters.position=inters;
				break;
			}
		}
		if(xStartInters.index>=0&&xEndInters.index>=0)
		{	//生成切割壁在顶/底面的多边形顶点（原坐标系）
			//////////////////////////////////////////////////////////
			xTopCutEdge=*pEdgeLine;	//记录顶面上的割口原始切割边信息以便后续查找切割侧壁面
			//////////////////////////////////////////////////////////
			GEPOINT vertice,vertex;
			//添加顶面切割折线顶点
			vertex=acs.TransPFromCS(xStartInters.position);
			xarrCutWallTopVertexes.Append(INTERS_POINT(vertex));
			//添加底面切割折线顶点
			f3dAtomLine* pBtmEdgeLine=pBtmFace->outer_edge.GetFirst();
			GEPOINT xBtmOrg=acs.TransPToCS(*pBtmEdgeLine->pStart);
			vertex=acs.TransPFromCS(GEPOINT(xStartInters.position.x,xStartInters.position.y,xBtmOrg.z));
			xarrCutWallBtmVertexes.Append(INTERS_POINT(vertex));
			
			for(i=xStartInters.index+1;i<=xEndInters.index;i++)
			{
				vertice=xarrLocalCutPolyVertexes.At(i);
				//添加顶面切割折线顶点
				GEPOINT vertex=acs.TransPFromCS(vertice);
				xarrCutWallTopVertexes.Append(INTERS_POINT(vertex));
				//添加底面切割折线顶点
				vertex=acs.TransPFromCS(GEPOINT(vertice.x,vertice.y,xBtmOrg.z));
				xarrCutWallBtmVertexes.Append(INTERS_POINT(vertex));
			}
			//添加顶面切割折线末顶点
			vertex=acs.TransPFromCS(xEndInters.position);
			xarrCutWallTopVertexes.Append(INTERS_POINT(vertex));
			//添加底面切割折线末顶点
			vertex=acs.TransPFromCS(GEPOINT(xEndInters.position.x,xEndInters.position.y,xBtmOrg.z));
			xarrCutWallBtmVertexes.Append(INTERS_POINT(vertex));
			//补全整个实体的顶点表
			int iStartIndex=pBody->vertex.GetNodeNum();
			int iInitIndex=iStartIndex;
			for(i=0;i<(int)xarrCutWallTopVertexes.Count;i++,iStartIndex++)
			{
				pWallVertex=xarrCutWallTopVertexes.GetAt(i);
				pBody->vertex.append(pWallVertex->position);
				pWallVertex->index=iStartIndex;
			}
			for(i=0;i<(int)xarrCutWallBtmVertexes.Count;i++,iStartIndex++)
			{
				pWallVertex=xarrCutWallBtmVertexes.GetAt(i);
				pBody->vertex.append(pWallVertex->position);
				pWallVertex->index=iStartIndex;
			}
			xarrFinalVertexPtrs.SetSize(0,pBody->vertex.GetNodeNum());
			for(f3dPoint* pVertex=pBody->vertex.GetFirst();pVertex;pVertex=pBody->vertex.GetNext())
				xarrFinalVertexPtrs.append(pVertex);
			break;
		}
	}
	if(xStartInters.index<0||xEndInters.index<0)
		return false;
	//4.确认切割折线在底面上的切割交点是否与顶面一致
	int indexOfStartInters=-1,indexOfEndInters=-1;
	for(pEdgeLine=pBtmFace->outer_edge.GetFirst();pEdgeLine;pEdgeLine=pBtmFace->outer_edge.GetNext())
	{
		xCurrLine.startPt=acs.TransPToCS(*pEdgeLine->pStart);
		xCurrLine.endPt=acs.TransPToCS(*pEdgeLine->pEnd);
		GEPOINT inters;
		indexOfStartInters=-1,indexOfEndInters=-1;
		for(i=1;i<count;i++)
		{
			f3dLine xCutLine(xarrLocalCutPolyVertexes.At(i-1),xarrLocalCutPolyVertexes.At(i));
			if((retcode=Int2dll(f2dLine(xCutLine.startPt,xCutLine.endPt),f2dLine(xCurrLine.startPt,xCurrLine.endPt),
				inters.x,inters.y))!=1)
				continue;
			if(indexOfStartInters<0)
				indexOfStartInters=i-1;
			else
			{
				indexOfEndInters=i-1;
				//////////////////////////////////////////////////////////
				xBtmCutEdge=*pEdgeLine;	//记录底面上的割口原始切割边信息以便后续查找切割侧壁面
				//////////////////////////////////////////////////////////
				break;
			}
		}
		if(indexOfStartInters>=0&&indexOfEndInters>=0)
			break;
	}
	if(indexOfStartInters!=xStartInters.index||indexOfEndInters!=xEndInters.index)
	{
		logerr.Log("实体割口操作时，顶面与底面切割结果不一致");
		return false;
	}
	//5.更新顶面的外环顶点链表
	for(pEdgeLine=pTopFace->outer_edge.GetFirst();pEdgeLine;pEdgeLine=pTopFace->outer_edge.GetNext())
	{
		if(pEdgeLine->pStart==xTopCutEdge.pStart&&pEdgeLine->pEnd==xTopCutEdge.pEnd)
			break;
	}
	pEdgeLine->pEnd=xarrFinalVertexPtrs[xarrCutWallTopVertexes.At(xarrCutWallTopVertexes.Count-1).index];
	for(i=xarrCutWallTopVertexes.Count-2;i>=0;i--)
	{
		pWallVertex=xarrCutWallTopVertexes.GetAt(i);
		atomline.pStart=pEdgeLine->pEnd;
		atomline.pEnd=xarrFinalVertexPtrs[pWallVertex->index];
		pEdgeLine=pTopFace->outer_edge.insert(atomline,fLineList::INSERT_AFTER);
	}
	atomline.pStart=pEdgeLine->pEnd;
	atomline.pEnd=xTopCutEdge.pEnd;
	pEdgeLine=pTopFace->outer_edge.insert(atomline,fLineList::INSERT_AFTER);
	//6.更新底面的外环顶点链表
	for(pEdgeLine=pBtmFace->outer_edge.GetFirst();pEdgeLine;pEdgeLine=pBtmFace->outer_edge.GetNext())
	{
		if(pEdgeLine->pStart==xBtmCutEdge.pStart&&pEdgeLine->pEnd==xBtmCutEdge.pEnd)
			break;
	}
	pEdgeLine->pEnd=xarrFinalVertexPtrs[xarrCutWallBtmVertexes[0].index];
	for(i=1;i<(int)xarrCutWallBtmVertexes.Count;i++)
	{
		pWallVertex=xarrCutWallBtmVertexes.GetAt(i);
		atomline.pStart=pEdgeLine->pEnd;
		atomline.pEnd=xarrFinalVertexPtrs[pWallVertex->index];
		pEdgeLine=pBtmFace->outer_edge.insert(atomline,fLineList::INSERT_AFTER);
	}
	atomline.pStart=pEdgeLine->pEnd;
	atomline.pEnd=xBtmCutEdge.pEnd;
	pEdgeLine=pBtmFace->outer_edge.insert(atomline,fLineList::INSERT_AFTER);
	//7.查找连接顶面与底面的侧壁面
	f3dPolyFace *pFace=NULL, *pSideEdgeFace=NULL,*pNewSideEdgeFace=NULL;
	int indexOfTopCutEdge=-1;	//侧壁平面中顶部被切割边索引位置
	int indexOfBtmCutEdge=-1;	//侧壁平面中底部被切割边索引位置
	for(pFace=pBody->faceList.GetFirst();pFace;pFace=pBody->faceList.GetNext())
	{
		if(pFace==pTopFace||pFace==pBtmFace)
			continue;
		indexOfTopCutEdge=indexOfBtmCutEdge=-1;
		for(pEdgeLine=pFace->outer_edge.GetFirst(),i=0;pEdgeLine;pEdgeLine=pFace->outer_edge.GetNext(),i++)
		{	//目前仅考虑角钢肢这类的普通板面割口（即顶/底部平面与连接侧壁面有共同边情况） wjh-2019.3.26
			if( indexOfTopCutEdge<0&&(
				pEdgeLine->pStart==xTopCutEdge.pStart&&pEdgeLine->pEnd==xTopCutEdge.pEnd||
				pEdgeLine->pStart==xTopCutEdge.pEnd  &&pEdgeLine->pEnd==xTopCutEdge.pStart))
			{
				indexOfTopCutEdge=i;
				xTopCutEdge=*pEdgeLine;
			}
			if( indexOfBtmCutEdge<0&&(
				pEdgeLine->pStart==xBtmCutEdge.pStart&&pEdgeLine->pEnd==xBtmCutEdge.pEnd||
				pEdgeLine->pStart==xBtmCutEdge.pEnd  &&pEdgeLine->pEnd==xBtmCutEdge.pStart))
			{
				indexOfBtmCutEdge=i;
				xBtmCutEdge=*pEdgeLine;
			}
			if(indexOfTopCutEdge>=0&&indexOfBtmCutEdge>=0)
				break;
		}
		if(indexOfTopCutEdge>=0&&indexOfBtmCutEdge>=0)
		{	
			pSideEdgeFace=pFace;
			break;
		}
	}
	if(pSideEdgeFace==NULL)
	{
		logerr.Log("error");
		return false;
	}
	//重新拆分构建切割口侧面
	DYN_ARRAY<f3dAtomLine> xarrEdges(pSideEdgeFace->outer_edge.GetNodeNum());
	for(pEdgeLine=pSideEdgeFace->outer_edge.GetFirst(),i=0;pEdgeLine;pEdgeLine=pSideEdgeFace->outer_edge.GetNext(),i++)
		xarrEdges[i]=*pEdgeLine;
	pSideEdgeFace->outer_edge.Empty();
	pNewSideEdgeFace=pBody->faceList.append();
	pNewSideEdgeFace->material=pSideEdgeFace->material;
	if(indexOfTopCutEdge<indexOfBtmCutEdge)
	{
		//插入侧壁保留区域头边
		pWallVertex=xarrCutWallTopVertexes.GetAt(xarrCutWallTopVertexes.Count-1);
		atomline.pStart=xarrFinalVertexPtrs[pWallVertex->index];
		atomline.pEnd=xTopCutEdge.pEnd;
		pSideEdgeFace->outer_edge.append(atomline);
		
		for(i=0;i<(int)xarrEdges.Size();i++)
		{
			if(i>indexOfTopCutEdge&&i<indexOfBtmCutEdge)
			{	//indexOfTopCutEdge～indexOfBtmCutEdge区域为左侧保留区
				pSideEdgeFace->outer_edge.append(xarrEdges[i]);
			}
			if(i==indexOfTopCutEdge)
			{	//插入侧壁分拆区域顶部打断边
				pWallVertex=xarrCutWallTopVertexes.GetAt(0);
				atomline.pStart=xTopCutEdge.pStart;
				atomline.pEnd=xarrFinalVertexPtrs[pWallVertex->index];;
				pNewSideEdgeFace->outer_edge.append(atomline);
			}
			else if(i==indexOfBtmCutEdge)
			{
				//插入接续边
				pWallVertex=xarrCutWallBtmVertexes.GetAt(0);
				atomline.pStart=xarrFinalVertexPtrs[xarrCutWallTopVertexes[0].index];
				atomline.pEnd=xarrFinalVertexPtrs[xarrCutWallBtmVertexes[0].index];
				pNewSideEdgeFace->outer_edge.append(atomline);
				//插入侧壁分拆区域底部打断边
				atomline.pStart=atomline.pEnd;
				atomline.pEnd=xBtmCutEdge.pEnd;
				pNewSideEdgeFace->outer_edge.append(atomline);
			}
			else if(i<indexOfTopCutEdge||i>indexOfBtmCutEdge)
				pNewSideEdgeFace->outer_edge.append(xarrEdges[i]);
		}
		//插入侧壁保留区域尾边
		atomline.pStart=xBtmCutEdge.pStart;
		pWallVertex=xarrCutWallBtmVertexes.GetAt(xarrCutWallBtmVertexes.Count-1);
		atomline.pEnd=xarrFinalVertexPtrs[pWallVertex->index];
		pSideEdgeFace->outer_edge.append(atomline);
		//插入接续边
		pWallVertex=xarrCutWallTopVertexes.GetAt(xarrCutWallTopVertexes.Count-1);
		atomline.pStart=atomline.pEnd;
		atomline.pEnd=xarrFinalVertexPtrs[pWallVertex->index];;
		pSideEdgeFace->outer_edge.append(atomline);
	}
	else //if(indexOfTopCutEdge>indexOfBtmCutEdge)
	{
		//插入侧壁分拆区域顶部打断边
		atomline.pStart=xTopCutEdge.pStart;
		atomline.pEnd=xarrFinalVertexPtrs[xarrCutWallTopVertexes[0].index];
		pNewSideEdgeFace->outer_edge.append(atomline);
		atomline.pStart=atomline.pEnd;
		atomline.pEnd=xarrFinalVertexPtrs[xarrCutWallBtmVertexes[0].index];
		pNewSideEdgeFace->outer_edge.append(atomline);
		//插入侧壁分拆区域底部打断边
		atomline.pStart=atomline.pEnd;
		atomline.pEnd=xBtmCutEdge.pEnd;
		pNewSideEdgeFace->outer_edge.append(atomline);

		for(i=0;i<(int)xarrEdges.Size();i++)
		{
			if(i>indexOfBtmCutEdge&&i<indexOfTopCutEdge)
			{	//indexOfTopCutEdge～indexOfBtmCutEdge区域为左侧保留区
				pNewSideEdgeFace->outer_edge.append(xarrEdges[i]);
			}
			if(i==indexOfTopCutEdge)
			{	//插入侧壁分拆区域顶部打断边
				pWallVertex=xarrCutWallTopVertexes.GetAt(xarrCutWallTopVertexes.Count-1);
				atomline.pStart=xarrFinalVertexPtrs[pWallVertex->index];
				atomline.pEnd=xTopCutEdge.pEnd;
				pSideEdgeFace->outer_edge.append(atomline);
			}
			else if(i==indexOfBtmCutEdge)
			{	//插入侧壁分拆区域底部打断边
				pWallVertex=xarrCutWallBtmVertexes.GetAt(xarrCutWallBtmVertexes.Count-1);
				atomline.pStart=xBtmCutEdge.pStart;
				atomline.pEnd=xarrFinalVertexPtrs[pWallVertex->index];
				pSideEdgeFace->outer_edge.append(atomline);
				//插入接续边
				atomline.pStart=atomline.pEnd;
				pWallVertex=xarrCutWallTopVertexes.GetAt(xarrCutWallTopVertexes.Count-1);
				atomline.pEnd=xarrFinalVertexPtrs[pWallVertex->index];
				pSideEdgeFace->outer_edge.append(atomline);
			}
			else if(i<indexOfBtmCutEdge||i>indexOfTopCutEdge)
				pSideEdgeFace->outer_edge.append(xarrEdges[i]);
		}
	}
	//8.切割口侧壁面生成
	for(i=1;i<(int)xarrCutWallTopVertexes.Count;i++)
	{
		pNewSideEdgeFace=pBody->faceList.append();
		pNewSideEdgeFace->material=pSideEdgeFace->material;
		atomline.pStart=xarrFinalVertexPtrs[xarrCutWallTopVertexes[i-1].index];
		atomline.pEnd  =xarrFinalVertexPtrs[xarrCutWallTopVertexes[i].index];
		pNewSideEdgeFace->outer_edge.append(atomline);
		atomline.pStart=atomline.pEnd;
		atomline.pEnd=xarrFinalVertexPtrs[xarrCutWallBtmVertexes[i].index];
		pNewSideEdgeFace->outer_edge.append(atomline);
		atomline.pStart=atomline.pEnd;
		atomline.pEnd=xarrFinalVertexPtrs[xarrCutWallBtmVertexes[i-1].index];
		pNewSideEdgeFace->outer_edge.append(atomline);
		atomline.pStart=atomline.pEnd;
		atomline.pEnd=xarrFinalVertexPtrs[xarrCutWallTopVertexes[i-1].index];
		pNewSideEdgeFace->outer_edge.append(atomline);
	}

	return true;	
}
