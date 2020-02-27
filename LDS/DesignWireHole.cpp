// DesignWireHole.cpp: implementation of the CDesignWireHole class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DesignWireHole.h"
#include "tower.h"
#include "LDS.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "SortFunc.h"
#include "LogFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static int compare_func1(const GUAXIANHOLE_BASE_INFO &lsPos1,const GUAXIANHOLE_BASE_INFO &lsPos2)
{	
	return int(lsPos1.lengthPosToEdge-lsPos2.lengthPosToEdge); 
}
//////////////////////////////////////////////////////////////////////////
//CDesignWireHole
//////////////////////////////////////////////////////////////////////////
CDesignWireHole::CDesignWireHole(CLDSPlate* pPlate/*=NULL*/)
{
	m_pPlate=pPlate;
	m_bWaveShape=TRUE;
	m_pVertexS=NULL;
	m_pVertexS=NULL;
	m_fInnerWaveArcRadius=0.0;
}
CDesignWireHole::~CDesignWireHole()
{
	m_pPlate=NULL;
	m_pVertexS=NULL;
	m_pVertexS=NULL;
}
//计算距挂线孔距离最近的轮廓边，记录始终端顶点
BOOL CDesignWireHole::CalNearestEdgeToHole()
{
	if(m_pPlate==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("No wire plate is selected!");
#else
		logerr.Log("没有选中进行设计的钢板!");
#endif
		return FALSE;
	}
	if(guaXianHoleInfoArr.GetSize()<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("No wire hole is selected!");
#else
		logerr.Log("没有选中进行设计的挂线孔!");
#endif
		return FALSE;
	}
	//计算与挂线孔距离最近的轮廓边
	double fMinDist=10000,fDist=0,fSumDist=0;
	f3dPoint inters;
	PROFILE_VERTEX *pPreVertex=m_pPlate->vertex_list.GetTail();
	for(PROFILE_VERTEX *pCurVertex=m_pPlate->vertex_list.GetFirst();pCurVertex;pCurVertex=m_pPlate->vertex_list.GetNext())
	{
		BOOL bValid=TRUE;
		fSumDist=0;
		for(GUAXIANHOLE_BASE_INFO* pHoleInfo=EnumFirstHole();pHoleInfo;pHoleInfo=EnumNextHole())
		{	
			SnapPerp(&inters,pPreVertex->vertex,pCurVertex->vertex,pHoleInfo->ls_pos,&fDist);
			fSumDist+=fDist;
			if(f3dPoint(inters-pPreVertex->vertex)*f3dPoint(pCurVertex->vertex-inters)<0)	//挂线孔投影点在此轮廓边外面
			{
				bValid=FALSE;
				break;
			}
		}
		if(bValid&&fSumDist<fMinDist)
		{	//距挂线孔最短距离
			fMinDist=fSumDist;
			m_pVertexS=pPreVertex;
			m_pVertexE=pCurVertex;
		}
		pPreVertex=pCurVertex;
	}
	if(m_pVertexS==NULL || m_pVertexE==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("can not find the nearest outline edge from wire hole，check whether the outline edge is selected correctly!");
#else
		logerr.Log("找不到距挂线孔最近的轮廓边，查看轮廓边是否选择正确!");
#endif
		return FALSE;
	}
	//计算挂孔向最近边垂足点到最近边起点距离，并按由近及远排序 wjh-2013.8.15
	for(GUAXIANHOLE_BASE_INFO *pHoleInfo=EnumFirstHole();pHoleInfo;pHoleInfo=EnumNextHole())
	{
		SnapPerp(&inters,m_pVertexS->vertex,m_pVertexE->vertex,pHoleInfo->ls_pos);
		pHoleInfo->lengthPosToEdge=DISTANCE(m_pVertexS->vertex,inters);
	}
	CQuickSort<GUAXIANHOLE_BASE_INFO>::QuickSort(guaXianHoleInfoArr.m_pData,GetHoleNum(),compare_func1);
	return TRUE;
}
//重新检测轮廓边的始终端顶点是否合理，选取合适的轮廓顶点
void CDesignWireHole::RefreshStartAndEndVertex()
{
	GUAXIANHOLE_BASE_INFO* pFirstHoleInfo=guaXianHoleInfoArr.GetFirst();
	GUAXIANHOLE_BASE_INFO* pEndHoleInfo=guaXianHoleInfoArr.GetTail();
	f3dPoint ptS(m_pVertexS->vertex),ptE(m_pVertexE->vertex);
	if(DISTANCE(pFirstHoleInfo->ls_pos,ptS)+EPS<=pFirstHoleInfo->fRadius)
	{	//始端顶点在挂线点圆弧内,删除此顶点，取前一个顶点
		int startIndex=0;
		for(PROFILE_VERTEX* pVertex=m_pPlate->vertex_list.GetFirst();pVertex;pVertex=m_pPlate->vertex_list.GetNext())
		{
			if(pVertex==m_pVertexS)
			{
				startIndex=m_pPlate->vertex_list.GetCurrentIndex();
				break;
			}
		}
		int nVertexNum=m_pPlate->vertex_list.GetNodeNum();
		int index=(nVertexNum+startIndex-1)%nVertexNum;	//查找前一个顶点
		m_pVertexS=m_pPlate->vertex_list.GetByIndex(index);
		m_pPlate->vertex_list.DeleteAt(startIndex);//删除之前顶点
	}
	if(DISTANCE(pEndHoleInfo->ls_pos,ptE)<=pEndHoleInfo->fRadius)
	{	//终端顶点在挂线点圆弧内,删除此顶点，取下一个顶点
		int endIndex=0;
		for(PROFILE_VERTEX* pVertex=m_pPlate->vertex_list.GetFirst();pVertex;pVertex=m_pPlate->vertex_list.GetNext())
		{
			if(pVertex==m_pVertexE)
			{
				endIndex=m_pPlate->vertex_list.GetCurrentIndex();
				break;
			}
		}
		int nVertexNum=m_pPlate->vertex_list.GetNodeNum();
		int index=(endIndex+1)%nVertexNum;	//查找后一个顶点
		m_pVertexE=m_pPlate->vertex_list.GetByIndex(index);
		m_pPlate->vertex_list.DeleteAt(endIndex);	//删除之前顶点
	}
}
//去除不合理的挂线孔
BOOL CDesignWireHole::RemoveErrorHoles()
{
	int nNum=GetHoleNum();
	f3dPoint ptS(m_pVertexS->vertex),ptE(m_pVertexE->vertex);
	for(GUAXIANHOLE_BASE_INFO* pHoleInfo=EnumFirstHole();pHoleInfo;pHoleInfo=EnumNextHole())
	{
		double fDistToLine=-DistOf2dPtLine(pHoleInfo->ls_pos,ptS,ptE);
		if((pHoleInfo==&guaXianHoleInfoArr[0] ||pHoleInfo==&guaXianHoleInfoArr[nNum-1])
			&&fDistToLine<=-pHoleInfo->fRadius)
		{	//始端或终端挂线孔所在的圆在轮廓边的左侧（板内）
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("wire hole selected is unreasonable，design failed!");
#else
			logerr.Log("所选挂线孔不合理，设计失败!");
#endif
			return FALSE;
		}
	}
	if(nNum<=2)
		return TRUE;
	f3dPoint vec=(ptE-ptS).normalized();
	f3dPoint offset_vec(vec.y,-vec.x,0);
	for(int i=1;i<nNum-1;i++)
	{
		GUAXIANHOLE_BASE_INFO holeInfo=guaXianHoleInfoArr[i];
		if(!m_bWaveShape)
		{	//凸弧形要求中间孔距离前后孔弧公切线左侧垂距<圆弧半径
			f2dCircle circle1,circle2;
			f3dPoint pick_pt1=guaXianHoleInfoArr[0].ls_pos+offset_vec*10;
			circle1.radius=guaXianHoleInfoArr[0].fRadius;
			circle1.centre.Set(guaXianHoleInfoArr[0].ls_pos.x,guaXianHoleInfoArr[0].ls_pos.y);
			f3dPoint pick_pt2=guaXianHoleInfoArr[nNum-1].ls_pos+offset_vec*10;
			circle2.radius=guaXianHoleInfoArr[nNum-1].fRadius;
			circle2.centre.Set(guaXianHoleInfoArr[nNum-1].ls_pos.x,guaXianHoleInfoArr[nNum-1].ls_pos.y);
			f2dLine tan_line;
			if(TangLine2dcc(circle1,pick_pt1,circle2,pick_pt2,tan_line)!=1)	//始末挂线孔的公切线
				return FALSE;
			vec=f3dPoint(tan_line.endPt.x,tan_line.endPt.y,0)-f3dPoint(tan_line.startPt.x,tan_line.startPt.y,0);
			normalize(vec);
			double dist=DistOf2dPtLine(holeInfo.ls_pos,f3dPoint(tan_line.startPt.x,tan_line.startPt.y,0),f3dPoint(tan_line.endPt.x,tan_line.endPt.y,0));
			if(dist>holeInfo.fRadius || fabs(dist-holeInfo.fRadius)<EPS2)
			{
				guaXianHoleInfoArr.RemoveAt(i);
				nNum--;
				i--;
			}
		}
		//else{//波浪形要求挂孔间间距<两凸弧半径加一凹弧半径}
	}
	return TRUE;
}
//计算轮廓边始终端到始末挂线孔圆弧的公切线
BOOL CDesignWireHole::CalStartAndEndTangentPt(PROFILE_VERTEX& startTangPt,PROFILE_VERTEX& endTangPt)
{
	f3dPoint ptS(m_pVertexS->vertex),ptE(m_pVertexE->vertex);
	f3dPoint vec=(ptE-ptS).normalized();
	f3dPoint pick_vec(vec.y,-vec.x,0);
	//始端顶点与始端处挂线孔所在圆弧求切点
	f2dPoint tan_pt;
	f2dCircle circle;
	GUAXIANHOLE_BASE_INFO* pStartHoleInfo=EnumFirstHole();
	circle.radius=pStartHoleInfo->fRadius;
	circle.centre.Set(pStartHoleInfo->ls_pos.x,pStartHoleInfo->ls_pos.y);
	f3dPoint pick_pos=pStartHoleInfo->ls_pos+pick_vec*10;
	if(Tang2dpc(f2dPoint(ptS.x,ptS.y),circle,pick_pos,tan_pt))
	{
		startTangPt.vertex.Set(tan_pt.x,tan_pt.y,0);
		startTangPt.type=2;
		startTangPt.center=pStartHoleInfo->ls_pos;
		startTangPt.radius=pStartHoleInfo->fRadius;
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("vertex of starting end is inside the wire hole arc, design failed!");
#else
		logerr.Log("始端顶点在挂线孔圆弧内部,设计失败！");
#endif
		return FALSE;
	}
	//终端顶点与终端处挂线孔所在圆弧求切点
	GUAXIANHOLE_BASE_INFO* pEndHoleInfo=EnumTailHole();
	circle.radius=pEndHoleInfo->fRadius;
	circle.centre.Set(pEndHoleInfo->ls_pos.x,pEndHoleInfo->ls_pos.y);
	pick_pos=pEndHoleInfo->ls_pos+pick_vec*10;
	if(Tang2dpc(f2dPoint(ptE.x,ptE.y),circle,pick_pos,tan_pt))
		endTangPt.vertex.Set(tan_pt.x,tan_pt.y,0);
	else
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("vertex of ending is inside the wire hole arc, design failed!");
#else
		logerr.Log("终端顶点在挂线孔圆弧内部,设计失败！");
#endif
		return FALSE;
	}
	return TRUE;
}
//计算多个挂线孔之间的公切点
BOOL CDesignWireHole::CalMultiHoleTangentPt(ATOM_LIST<PROFILE_VERTEX> &vertexList)
{
	f3dPoint offset_vec,rayvec;
	f3dPoint vec=(m_pVertexE->vertex-m_pVertexS->vertex).normalized();
	if(m_bWaveShape)	
	{	//波浪圆弧形设计
		double angle,cosa,start_radius,end_radius,tan_cir_r,dis;
		f3dPoint axis(0,0,1);
		PROFILE_VERTEX startVertex,midVertex,endVertex;
		GUAXIANHOLE_BASE_INFO* pPreHoleInfo=EnumFirstHole();
		for(GUAXIANHOLE_BASE_INFO* pCurHoleInfo=EnumNextHole();pCurHoleInfo;pCurHoleInfo=EnumNextHole())
		{
			dis=DISTANCE(pPreHoleInfo->ls_pos,pCurHoleInfo->ls_pos);
			if(dis<=fabs(pPreHoleInfo->fRadius-pCurHoleInfo->fRadius))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Two circles intersect or nest, legal outside common tangent circle does not exist !");
#else
				logerr.Log("两圆相套,合法外公切圆不存在!");
#endif
				return FALSE;
			}
			bool processed=false;
			tan_cir_r=(dis/2)*-1;	//负半径表示内凹弧
			offset_vec=pCurHoleInfo->ls_pos-pPreHoleInfo->ls_pos;
			normalize(offset_vec);
			if(m_fInnerWaveArcRadius>0)	//设定了内圆弧半径
			{
				double r=m_fInnerWaveArcRadius;
				double len_offset1=SQRT(SQR(pPreHoleInfo->fRadius+r)-SQR(r));
				double len_offset2=SQRT(SQR(pCurHoleInfo->fRadius+r)-SQR(r));
				double cosa1=len_offset1/(pPreHoleInfo->fRadius+r);
				double cosa2=len_offset2/(pCurHoleInfo->fRadius+r);
				double sina1=r/(pPreHoleInfo->fRadius+r);
				double sina2=r/(pCurHoleInfo->fRadius+r);
				if(len_offset1+len_offset2<dis)
				{
					rayvec=offset_vec;
					RotateVectorAroundVector(rayvec,sina1,cosa1,-axis);
					startVertex.vertex=pPreHoleInfo->ls_pos+rayvec*pPreHoleInfo->fRadius;
					startVertex.vertex.z=0;
					startVertex.type=2;
					startVertex.center=pPreHoleInfo->ls_pos+rayvec*(pPreHoleInfo->fRadius+r);
					startVertex.radius=-r;
					vertexList.append(startVertex);
					midVertex.vertex=pPreHoleInfo->ls_pos+offset_vec*len_offset1;
					midVertex.type=0;
					vertexList.append(midVertex);

					rayvec=-offset_vec;
					RotateVectorAroundVector(rayvec,sina2,cosa2,axis);
					midVertex.vertex=pCurHoleInfo->ls_pos-offset_vec*len_offset2;
					midVertex.type=2;
					midVertex.center=pCurHoleInfo->ls_pos+rayvec*(pCurHoleInfo->fRadius+r);
					midVertex.radius=-r;
					vertexList.append(midVertex);
					endVertex.vertex=pCurHoleInfo->ls_pos+rayvec*pCurHoleInfo->fRadius;
					endVertex.type=2;
					endVertex.center=pCurHoleInfo->ls_pos;
					endVertex.radius=pCurHoleInfo->fRadius;
					vertexList.append(endVertex);
					processed=true;
				}
				else
					tan_cir_r=-m_fInnerWaveArcRadius;
			}
			if(!processed)
			{
				start_radius=pPreHoleInfo->fRadius+(-tan_cir_r);
				end_radius=pCurHoleInfo->fRadius+(-tan_cir_r);
				cosa=(SQR(start_radius)+SQR(dis)-SQR(end_radius))*0.5/(start_radius*dis);
				angle=ACOS(cosa);
				rayvec=offset_vec;
				RotateVectorAroundVector(rayvec,angle,-axis);
				startVertex.vertex=pPreHoleInfo->ls_pos+rayvec*pPreHoleInfo->fRadius;
				startVertex.vertex.z=0;
				startVertex.type=2;
				startVertex.center=pPreHoleInfo->ls_pos+rayvec*start_radius;
				startVertex.radius=tan_cir_r;
				vertexList.append(startVertex);
				//
				cosa=(SQR(end_radius)+SQR(dis)-SQR(start_radius))*0.5/(end_radius*dis);
				angle=ACOS(cosa);
				rayvec=-offset_vec;
				RotateVectorAroundVector(rayvec,angle,axis);
				endVertex.vertex=pCurHoleInfo->ls_pos+rayvec*pCurHoleInfo->fRadius;
				endVertex.vertex.z=0;
				endVertex.type=2;
				endVertex.center=pCurHoleInfo->ls_pos;
				endVertex.radius=pCurHoleInfo->fRadius;
				vertexList.append(endVertex);
			}
			pPreHoleInfo=pCurHoleInfo;
		}
	}
	else	
	{	//外凸圆弧形设计,计算相邻挂线孔所在圆的公切点
		f3dPoint pick_pt1,pick_pt2;
		offset_vec.Set(vec.y,-vec.x,0);
		f2dCircle circle1,circle2;
		PROFILE_VERTEX startVertex,endVertex;
		GUAXIANHOLE_BASE_INFO* pPreHoleInfo=EnumFirstHole();
		for(GUAXIANHOLE_BASE_INFO* pCurHoleInfo=EnumNextHole();pCurHoleInfo;pCurHoleInfo=EnumNextHole())
		{
			pick_pt1=pPreHoleInfo->ls_pos+offset_vec*10;
			circle1.radius=pPreHoleInfo->fRadius;
			circle1.centre.Set(pPreHoleInfo->ls_pos.x,pPreHoleInfo->ls_pos.y);
			pick_pt2=pCurHoleInfo->ls_pos+offset_vec*10;
			circle2.radius=pCurHoleInfo->fRadius;
			circle2.centre.Set(pCurHoleInfo->ls_pos.x,pCurHoleInfo->ls_pos.y);
			f2dLine tan_line;
			if(TangLine2dcc(circle1,pick_pt1,circle2,pick_pt2,tan_line)==1)
			{
				startVertex.vertex.Set(tan_line.startPt.x,tan_line.startPt.y,0);
				vertexList.append(startVertex);
				//
				endVertex.vertex.Set(tan_line.endPt.x,tan_line.endPt.y,0);
				endVertex.type=2;
				endVertex.radius=pCurHoleInfo->fRadius;
				endVertex.center=pCurHoleInfo->ls_pos;
				vertexList.append(endVertex);
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Two circles intersect or nest，legal common tangent doesn't exist!");
#else
				logerr.Log("两圆相交或相套，合法公切线不存在!");
#endif
				return FALSE;
			}
			pPreHoleInfo=pCurHoleInfo;
		}
	}
	return TRUE;
}
void CDesignWireHole::DesignWirePlate()
{
	//计算最近轮廓边
	if(m_pVertexS==NULL||m_pVertexE==NULL)
		CalNearestEdgeToHole();
	//更新始终端顶点
	RefreshStartAndEndVertex();
	//删除不合理的挂线孔
	if(!RemoveErrorHoles())
	{
		logerr.ShowToScreen();
		return;
	}
	//计算轮廓边始终顶点与相应挂线孔所在圆的公切点
	PROFILE_VERTEX startTangPt,endTangPt;
	if(!CalStartAndEndTangentPt(startTangPt,endTangPt))
	{
		logerr.ShowToScreen();
		return ;
	}
	//多个挂线孔的公切点计算
	ATOM_LIST<PROFILE_VERTEX> vertexList;
	if(GetHoleNum()>1)		
	{
		if(!CalMultiHoleTangentPt(vertexList))
		{
			logerr.ShowToScreen();
			return;
		}
	}
	//检查切点之间的合理性
	vertexList.append(endTangPt);
	vertexList.insert(startTangPt,0);
	f3dPoint vec=(m_pVertexE->vertex-m_pVertexS->vertex).normalized();
	PROFILE_VERTEX curVertex,nextVertex;
	for(int i=0;i<vertexList.GetNodeNum()-1;i++)
	{
		curVertex=vertexList[i];
		nextVertex=vertexList[(i+1)%vertexList.GetNodeNum()];
		f3dPoint offset_vec=nextVertex.vertex-curVertex.vertex;
		normalize(offset_vec);
		if(vec*offset_vec<0)
		{
			vertexList.DeleteAt(i);
			i--;
			vertexList.DeleteAt(i+1);
			continue;
		}
	}
	//插入新增顶点,更新板外形
	PROFILE_VERTEX* pVertex=NULL;
	int index=0;
	for(pVertex=m_pPlate->vertex_list.GetFirst();pVertex;pVertex=m_pPlate->vertex_list.GetNext())
	{
		if(pVertex==m_pVertexE)
		{
			index=m_pPlate->vertex_list.GetCurrentIndex();
			break;
		}
	}
	for(pVertex=vertexList.GetFirst();pVertex;pVertex=vertexList.GetNext())
	{	
		pVertex->vertex.feature=m_pVertexS->vertex.feature;
		if(pVertex->vertex.feature>10)	//pStartVertex为火曲点时，新增点被错误设置为火曲点，导致实体显示错误 wht 16-06-23
			pVertex->vertex.feature=m_pVertexE->vertex.feature;
		m_pPlate->vertex_list.insert(*pVertex,index);
		index++;
	}
	//钢板外形定义方式修改为自定义
	m_pPlate->designInfo.iProfileStyle0123=3;
	m_pPlate->SetModified();
	g_pSolidDraw->BuildDisplayList();
}
//////////////////////////////////////////////////////////////////////////
//测试函数
#ifdef __ALFA_TEST_VERSION_
#include "DxfFile.h"
void Test_DesignGuaXianHole()
{
	logerr.ClearContents();
	f3dPoint startPt,endPt;
	PROFILE_VERTEX* pVertex=NULL;
	CDxfFile dxf;
	dxf.OpenFile("d:\\testDesignHole.dxf");
	ATOM_LIST<PROFILE_VERTEX> vertexList;
	double ls_r=10;
	for(int i=0;i<13;i++)
	{
		CDesignWireHole designHoleInfo;
		GUAXIANHOLE_BASE_INFO holeInfo;
		vertexList.Empty();
		startPt.Set(0,0,0);
		endPt.Set(200,0,0);
		designHoleInfo.m_pVertexS=&PROFILE_VERTEX(startPt);
		designHoleInfo.m_pVertexE=&PROFILE_VERTEX(endPt);
		if(i==0)
		{	//单个挂线孔，挂线孔在直线上
			holeInfo.ls_pos.Set(100,0,0);
			holeInfo.fRadius=30;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=FALSE;
		}
		else if(i==1)
		{	//单个挂线孔，挂线孔在直线外
			holeInfo.ls_pos.Set(150,-50,0);
			holeInfo.fRadius=60;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=FALSE;
		}
		else if(i==2)
		{	//单个挂线孔，挂线孔在直线内，距直线距离较近
			holeInfo.ls_pos.Set(150,10,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=FALSE;
		}
		else if(i==3)
		{	//单个挂线孔，顶点在圆弧内部
			holeInfo.ls_pos.Set(180,0,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=FALSE;
		}
		else if(i==4)
		{	//两个挂线孔，两圆上切点之间没有出现凹面(外凸)
			holeInfo.ls_pos.Set(50,-60,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(120,-70,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=FALSE;
		}
		else if(i==5)
		{	//两个挂线孔，两圆上切点之间没有出现凹面(波浪)
			holeInfo.ls_pos.Set(50,-60,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(120,-70,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=TRUE;
		}
		else if(i==6)
		{	//两个挂线孔，两圆上切点之间出现凹面(外凸)
			holeInfo.ls_pos.Set(100,-40,0);
			holeInfo.fRadius=30;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(120,-160,0);
			holeInfo.fRadius=60;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=FALSE;
		}
		else if(i==7)
		{	//两个挂线孔，两圆上切点之间出现凹面(波浪)
			holeInfo.ls_pos.Set(100,-40,0);
			holeInfo.fRadius=30;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(120,-160,0);
			holeInfo.fRadius=60;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=TRUE;
		}	
		else if(i==8)
		{	//三个挂线孔，凸形摆放，切点件没有出现凹面（外凸）
			holeInfo.ls_pos.Set(50,-50,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(100,-100,0);
			holeInfo.fRadius=60;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(150,-50,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=FALSE;
		}
		else if(i==9)
		{	//三个挂线孔，凸形摆放，切点件没有出现凹面（波浪）
			holeInfo.ls_pos.Set(50,-50,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(100,-100,0);
			holeInfo.fRadius=60;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(150,-50,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=TRUE;
		}
		else if(i==10)
		{	//三个挂线孔，凹形摆放，切点件没有出现凹面
			holeInfo.ls_pos.Set(50,-100,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(100,-50,0);
			holeInfo.fRadius=60;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(150,-100,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=FALSE;
		}
		else if(i==11)
		{	//三个挂线孔，凸形摆放，切点件出现凹面（外凸）
			holeInfo.ls_pos.Set(70,-40,0);
			holeInfo.fRadius=40;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(100,-100,0);
			holeInfo.fRadius=70;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(130,-40,0);
			holeInfo.fRadius=40;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=FALSE;
		}
		else if(i==12)
		{	//三个挂线孔，凸形摆放，切点件有出现凹面（波浪）
			holeInfo.ls_pos.Set(70,-40,0);
			holeInfo.fRadius=40;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(100,-100,0);
			holeInfo.fRadius=50;
			designHoleInfo.AppendHoleInfo(holeInfo);
			holeInfo.ls_pos.Set(130,-40,0);
			holeInfo.fRadius=40;
			designHoleInfo.AppendHoleInfo(holeInfo);
			designHoleInfo.m_bWaveShape=TRUE;
		}
		//切割前后图形绘制
		f3dPoint plumb_offset,level_offset;
		plumb_offset=f3dPoint(0,-1,0)*200*i;
		dxf.NewLine(startPt+plumb_offset,endPt+plumb_offset);
		for(GUAXIANHOLE_BASE_INFO* pHoleInfo=designHoleInfo.EnumFirstHole();pHoleInfo;pHoleInfo=designHoleInfo.EnumNextHole())
			dxf.NewCircle(pHoleInfo->ls_pos+plumb_offset,ls_r);
		//
		level_offset=f3dPoint(1,0,0)*250;
		for(GUAXIANHOLE_BASE_INFO* pHoleInfo=designHoleInfo.EnumFirstHole();pHoleInfo;pHoleInfo=designHoleInfo.EnumNextHole())
			dxf.NewCircle(pHoleInfo->ls_pos+plumb_offset+level_offset,ls_r);
		PROFILE_VERTEX startTangPt,endTangPt;
		if(!designHoleInfo.CalStartAndEndTangentPt(startTangPt,endTangPt))
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("in %d, vertex is inside the arc, Common tangent point can not be calculated!",i+1);
#else
			logerr.Log("第%d中情况,顶点在圆弧内,不能计算共切点!",i+1);
#endif
		if(designHoleInfo.GetHoleNum()>1)
		{
			f3dPoint vec,axis;
			vec.Set(1,0,0);
			axis.Set(0,0,1);
			CString valueStr="";
			designHoleInfo.CalMultiHoleTangentPt(vertexList);
			if(valueStr.GetLength()>0)
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("in %d,%s",i+1,valueStr);
#else
				logerr.Log("第%d情况,%s",i+1,valueStr);
#endif
		}
		vertexList.append(endTangPt);
		vertexList.insert(startTangPt,0);
		vertexList.append(PROFILE_VERTEX(endPt));
		vertexList.insert(PROFILE_VERTEX(startPt),0);
		f3dPoint vec=(designHoleInfo.m_pVertexE->vertex-designHoleInfo.m_pVertexS->vertex).normalized();
		PROFILE_VERTEX curVertex,nextVertex;
		for(int i=0;i<vertexList.GetNodeNum()-1;i++)
		{
			curVertex=vertexList[i];
			nextVertex=vertexList[(i+1)%vertexList.GetNodeNum()];
			f3dPoint offset_vec=nextVertex.vertex-curVertex.vertex;
			normalize(offset_vec);
			if(vec*offset_vec<0)
			{
				vertexList.DeleteAt(i);
				i--;
				vertexList.DeleteAt(i+1);
				continue;
			}
		}
		PROFILE_VERTEX* pPrevVertex=vertexList.GetFirst();
		for(PROFILE_VERTEX* pCurVertex=vertexList.GetNext();pCurVertex;pCurVertex=vertexList.GetNext())
		{
			if(pPrevVertex->type==0)
				dxf.NewLine(pPrevVertex->vertex+plumb_offset+level_offset,pCurVertex->vertex+plumb_offset+level_offset);
			else
				dxf.NewCircle(pPrevVertex->center+plumb_offset+level_offset,fabs(pPrevVertex->radius));
			pPrevVertex=pCurVertex;
		}
	}
	dxf.CloseFile();
	logerr.ShowToScreen();
}
#endif