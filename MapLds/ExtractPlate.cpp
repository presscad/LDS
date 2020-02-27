#include "stdafx.h"
#include "ExtractPlate.h"
#include "SortFunc.h"
#include "DragEntSet.h"

CExtractPlate::CExtractPlate(void)
{
}


CExtractPlate::~CExtractPlate(void)
{
}

void CExtractPlate::Select()
{	//通过选择实体获取实体的ID
	hashSelectedCurves.Empty();
	AcDbObjectId entId;
	ads_name entname,ent_sel_set;
	if(acedSSGet(NULL,NULL,NULL,NULL,ent_sel_set)==RTNORM)
	{
		long ll;
		acedSSLength(ent_sel_set,&ll);
		for(long i=0;i<ll;i++)
		{
			acedSSName(ent_sel_set,i,entname);
			acdbGetObjectId(entId,entname);
			AcDbEntity *pEnt=NULL;
			if(acdbOpenObject(pEnt,entId,AcDb::kForRead)!=Acad::eOk)
				continue;
			CAcDbObjLife entLife(pEnt);
			if(pEnt->isKindOf(AcDbLine::desc())||pEnt->isKindOf(AcDbArc::desc()))
			{
				ACDB_CURVE *pCurve=hashSelectedCurves.append();
				pCurve->pCurve=(AcDbCurve*)pEnt;
				pCurve->curveId=entId;
				if(pEnt->isKindOf(AcDbLine::desc()))
				{
					AcDbLine *pLine=(AcDbLine*)pEnt;
					Cpy_Pnt(pCurve->startPt,pLine->startPoint());
					Cpy_Pnt(pCurve->endPt,pLine->endPoint());
				}
				else 
				{
					AcDbArc *pArc=(AcDbArc*)pEnt;
					AcGePoint3d  pointTemp;
					pArc->getStartPoint(pointTemp);
					Cpy_Pnt(pCurve->startPt,pointTemp);
					pArc->getEndPoint(pointTemp);
					Cpy_Pnt(pCurve->endPt,pointTemp);
				}
			}
		}
	}
}

//0.错误需结束循环，1.继续递规，2.查找到封闭图形
int AppendEdgeList(CXhPtrSet<ACDB_CURVE> &edgeList,CURVE_VERTEX *pVertex,ACDB_CURVE *pStartCurve,
				   ACDB_CURVE *pNextCurve,CMapList<CURVE_VERTEX> &hashMapVertexList)
{
	f3dPoint curPt=pNextCurve->startPtNo==pVertex->vertexNo?pNextCurve->endPt:pNextCurve->startPt;
	CURVE_VERTEX *pNextVertex=hashMapVertexList.GetValue((DWORD)curPt.x,(DWORD)curPt.y);
	if(pNextVertex==NULL||pNextVertex->bUsed)
		return 0;
	for(ACDB_CURVE *pCurve=pNextVertex->curveList.GetFirst();pCurve;pCurve=pNextVertex->curveList.GetNext())
	{
		if(pCurve==pNextCurve)
			continue;
		if(pCurve!=pStartCurve)
			edgeList.append(pCurve);
		if(pNextVertex->curveList.GetNodeNum()==2)
			pNextVertex->bUsed=true;
		if(pCurve==pStartCurve)
			return 2;
		else 
			return AppendEdgeList(edgeList,pNextVertex,pStartCurve,pCurve,hashMapVertexList);
	}
	return 0;
}
//判断两个点之间距离是否满足合为一个点
bool JudgeIsMergeOnePoint(CXhPtrSet<ACDB_CURVE> &curveList,f3dPoint point)
{
	for (ACDB_CURVE *pCure=curveList.GetFirst();pCure;pCure=curveList.GetNext())
	{
		if ((DWORD)pCure->startPt.x==(DWORD)point.x&&(DWORD)pCure->startPt.y==(DWORD)point.y)
		{
			double fReceive=DISTANCE(point,pCure->startPt);
			if(fReceive<0.1)
				return true;
			else
				return false;
		}
	}
	return true;
}
f3dPoint centerPoint;
int ComparePoint(const f3dPoint &point1,const f3dPoint &point2)
{
	double angle1=Cal2dLineAng(f2dPoint(centerPoint.x,centerPoint.y),f2dPoint(point1.x,point1.y));
	double angle2=Cal2dLineAng(f2dPoint(centerPoint.x,centerPoint.y),f2dPoint(point2.x,point2.y));
	if(angle1>angle2)
		return 1;
	else if(angle1<angle2)
		return -1;
	else 
		return 0;
}
//提取板
void CExtractPlate::Extract()
{	//1.将线段起点终点编号(方便以端点编号查找关联边)
	CMapList<CURVE_VERTEX>hashMapVertexList;
	int ptNo=1;
	for(ACDB_CURVE *pCurVe=hashSelectedCurves.GetFirst();pCurVe;pCurVe=hashSelectedCurves.GetNext())
	{
		f3dPoint pt[2]={pCurVe->startPt,pCurVe->endPt};
		for (int i=0;i<2;i++)
		{
			CURVE_VERTEX *pVertex=hashMapVertexList.GetValue((DWORD)pt[i].x,(DWORD)pt[i].y);
			//判断两个点之间距离是否满足合为一个点(暂时一放)
			if(pVertex==NULL)
			{
				pVertex=hashMapVertexList.Add((DWORD)pt[i].x,(DWORD)pt[i].y);
				pVertex->vertexNo=ptNo++;
			}
			if(i==0)
				pCurVe->startPtNo=pVertex->vertexNo;
			else
				pCurVe->endPtNo=pVertex->vertexNo;
			pVertex->curveList.append(pCurVe);
		}
	}
	//2.根据点关联边查找封闭图形(仅通过连接两条边的顶点遍历查找封闭图形)
	CXhPtrSet<ACDB_CURVE> edgeList;
	for(CURVE_VERTEX *pVertex=hashMapVertexList.GetFirst();pVertex;pVertex=hashMapVertexList.GetNext())
	{
		if(pVertex->curveList.GetNodeNum()!=2||pVertex->bUsed)
			continue;
		edgeList.Empty();
		ACDB_CURVE *pStartCurve=pVertex->curveList.GetFirst();
		ACDB_CURVE *pNextCurve=pVertex->curveList.GetNext();
		edgeList.append(pStartCurve);
		edgeList.append(pNextCurve);
		pVertex->bUsed=true;
		int nRetCode=AppendEdgeList(edgeList,pVertex,pStartCurve,pNextCurve,hashMapVertexList);
		if(nRetCode==2)
		{	//1.初始化钢板轮廓点数组
			int iPoint=0;
			centerPoint.Set();
			int nVertexCount=edgeList.GetNodeNum();
			DYN_ARRAY<f3dPoint> plateVertexArr(nVertexCount);
			CHashList<long> hashPtNoByPtNo;
			for(ACDB_CURVE *pCurve=edgeList.GetFirst();pCurve;pCurve=edgeList.GetNext())
			{
				int ptNoArr[2]={pCurve->startPtNo,pCurve->endPtNo};
				f3dPoint ptArr[2]={pCurve->startPt,pCurve->endPt};
				for(int i=0;i<2;i++)
				{
					if(hashPtNoByPtNo.GetValue(ptNoArr[i]))
						continue;
					hashPtNoByPtNo.SetValue(ptNoArr[i],ptNoArr[i]);
					plateVertexArr[i]=ptArr[i];
					centerPoint+=ptArr[i];
				}
			}
			centerPoint.Set(centerPoint.x/nVertexCount,centerPoint.y/nVertexCount,0);//求出轮廓的中心点
			CQuickSort<f3dPoint>::QuickSort(plateVertexArr,nVertexCount,ComparePoint);//得到有序的轮廓点数组ptArr，注意：如果不有序则调用acedSSGet出错
			//2.使用acedSSGet函数获取与钢板封闭区域相关的实体数组
			//2.1 根据钢板轮廓点初始化多边形区域
			ads_name ss_name,ent_name;	
			struct resbuf *pList=NULL,*pPoly=NULL;
			for (int i=0;i<nVertexCount;i++)
			{
				if(pList==NULL)
					pPoly=pList=acutNewRb(RTPOINT);	
				else
				{
					pList->rbnext=acutNewRb(RTPOINT);
					pList=pList->rbnext;
				}
				pList->restype=RTPOINT;
				pList->resval.rpoint[X]=plateVertexArr[i].x;
				pList->resval.rpoint[Y]=plateVertexArr[i].y;
				pList->resval.rpoint[Z]=0;
			}
			pList->rbnext=NULL;
			//2.2 选择封闭区域相关的实体并保存至AcDbPlate中
	#ifdef _ARX_2007
			int retCode=acedSSGet(L"cp",pPoly,NULL,NULL,ss_name);
	#else
			int retCode=acedSSGet("cp",pPoly,NULL,NULL,ss_name);
	#endif
			acutRelRb(pList);
			if(retCode==RTNORM)
			{
				CAcDbPlate *pPlate=plateList.append();
				for(ACDB_CURVE *pCurve=edgeList.GetFirst();pCurve;pCurve=edgeList.GetNext())
					pPlate->edgeList.append(pCurve);
				long ll;
				AcDbObjectId entId;
				acedSSLength(ss_name,&ll);
				for(int i=0;i<ll;i++)
				{
					AcDbEntity *pEnt;
					AcDbLine *pLine=NULL;
					AcDbPolyline *pPolyline=NULL;
					acedSSName(ss_name,i,ent_name);
					if(!DRAGSET.IsInDragSet(ent_name))
						continue;
					acdbGetObjectId(entId,ent_name);
					acdbOpenObject(pEnt,entId,AcDb::kForWrite);
					AcDbObjectId *pEntId=pPlate->hashAttachEnts.append();
					*pEntId=entId;
				}
			}
		}
	}
}

