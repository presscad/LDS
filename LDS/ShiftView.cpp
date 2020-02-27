//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "Tower.h"
#include "GUID.h"
#include "env_def.h"
#include "MainFrm.h"
#include "GlobalFunc.h"
#include "PromptDlg.h"
#include "CoordDefDlg.h"
#include "LmaDlg.h"
#include "ArrayList.h"
#include "SpreadVectorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL g_bObjectDeleted;

void CLDSView::OnSpreadFace() 
{	//展开工作面
	m_nPrevCommandID=ID_SPREAD_FACE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to spread work plane";
#else
	m_sPrevCommandName="重复展开工作面";
#endif
	if(!LockFunc())
		return;
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
	m_setSnapObjects.Empty();
	FinishSpreadFace();
}
int CLDSView::FinishSpreadFace()
{
	int retCode=0;
	CString cmdStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSDbObject *pFirstObj=NULL,*pSecondObj=NULL;
	if(m_setSnapObjects.GetNodeNum()==0)
	{	//还不具备执行命令的条件，先设定系统操作环境
		m_curTask = TASK_SPREAD_FACE;
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select datum node or rod for locating spread work plane:","");
#else
		pCmdLine->FillCmdLine("请选择确定展开工作面的基准节点或杆件:","");
#endif
		g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_ALL);
		return -1;
	}
	else
		pFirstObj=m_setSnapObjects.GetFirst();
	if(pFirstObj&&((CLDSPart*)pFirstObj)->IsLinePart())	//根据杆件确定展开工作面
		retCode=FinishSpreadFaceByLinePart((CLDSLinePart*)pFirstObj);
	else if(pFirstObj->GetClassTypeId()==CLS_NODE)	//根据两个节点确定展开工作面
	{
		CLDSNode *pFirstNode=(CLDSNode*)pFirstObj;
		if(m_setSnapObjects.GetNodeNum()==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("please select ending datum node for locating spread work plane:","");
#else
			pCmdLine->FillCmdLine("请选择确定展开工作面的终端基准节点:","");
#endif
			g_pSolidSnap->SetSnapType(SNAP_POINT);
			return -1;
		}
		else
		{
			pSecondObj=m_setSnapObjects.GetNext();
			if(pSecondObj->GetClassTypeId()==CLS_NODE)
				retCode= FinishSpreadFaceByNodes(pFirstNode,(CLDSNode*)pSecondObj);
			else
				retCode= -2;
		}
	}
	else
		retCode= -2;
	ReleaseFunc();
	OperOther();
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->FinishCmdLine();
	//继续展开工作的命令状态
	/*
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("please select starting datum node or rod for locating spread work plane:");
#else
	cmdStr.Format("请选择确定展开工作面的始端基准节点或杆件:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	g_pSolidSet->SetDisplayType(DISP_LINE);//切换到单线显示模式
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	m_setSnapObjects.Empty();*/
	return retCode;
}
BOOL CLDSView::FinishSpreadFaceByLinePart(CLDSLinePart* pLinePart)
{
	if(pLinePart==NULL)
		return FALSE;
	CLDSNode *pStart, *pEnd,*pMirStart=NULL,*pMirEnd=NULL;
	f3dPoint jg_vec;
	UCS_STRU ucs;

	pStart = pLinePart->pStart;
	pEnd   = pLinePart->pEnd;
	Ta.m_iCurSeg=pLinePart->iSeg;	//记录当前定制视图对应的段号，新建视图时用来初始化组名 wht 10-12-27
	if(m_eViewFlag==FRONT_VIEW|| m_eViewFlag==BACK_VIEW||m_eViewFlag==PERSPECTIVE)//展开前后对称面
	{
		pMirStart = pStart->GetMirNode(MIRMSG(1));	//console.FromNodeHandle(pStart->mir_x_node_handle);	//武晓画塔发现有时无法展开，原因就发生在这里的对称不是依据坐标  WJH 2003.11.29
		pMirEnd   = pEnd->GetMirNode(MIRMSG(1));	//console.FromNodeHandle(pEnd->mir_x_node_handle);
		if(pMirStart==NULL||pMirEnd==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Does not meet the spread condition(lacking of symmetrical parts), it cannot be spread");
#else
			AfxMessageBox("不满足展开条件(缺少对称构件),不能展开");
#endif
			return FALSE;
		}
		Sub_Pnt(jg_vec,pStart->ActivePosition(),pEnd->ActivePosition());
		if(jg_vec.z>0)	//角钢方向暂取为向上为正
			jg_vec = -1.0*jg_vec;
	}
	else if(m_eViewFlag==LEFT_VIEW|| m_eViewFlag==RIGHT_VIEW)//展开左右对称面
	{
		pMirStart = pStart->GetMirNode(MIRMSG(2));	//console.FromNodeHandle(pStart->mir_y_node_handle);
		pMirEnd   = pEnd->GetMirNode(MIRMSG(2));	//console.FromNodeHandle(pEnd->mir_y_node_handle);
		if(pMirStart==NULL||pMirEnd==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Does not meet the spread condition(lacking of symmetrical parts), it cannot be spread");
#else
			AfxMessageBox("不满足展开条件(缺少对称构件),不能展开");
#endif
			return FALSE;
		}
		Sub_Pnt(jg_vec,pStart->ActivePosition(),pEnd->ActivePosition());
		if(jg_vec.z>0)	//角钢方向暂取为向上为正
			jg_vec = -1.0*jg_vec;
	}
	else 
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can not spread any plane in an unknown view or a spread view!");
#else
		AfxMessageBox("不能在未知视图或展开视图下展开任意面!");
#endif
		return FALSE;
	}
	PushView();
	DISPLAY_TYPE type;
	g_pSolidSet->GetDisplayType(&type);
	if(type==DISP_LINE)
	{
		console.DispPartSet.Empty();
		console.DispNodeSet.Empty();
	}
	else 
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can not spread any plane in a solid model map!");
#else
		AfxMessageBox("实体图中不能,展开任意面!");
#endif
		return FALSE;
	}

	CSpreadVectorDlg dlg;
	GEPOINT vertexArr[4];
	vertexArr[0]=pEnd->ActivePosition();
	vertexArr[1]=pStart->ActivePosition();
	vertexArr[2]=pMirStart->ActivePosition();
	vertexArr[3]=pMirEnd->ActivePosition();
	dlg.InitPlaneVertex(vertexArr,4);
	//求任意面的用户相对坐标系(相对原塔坐标系)
	if(fabs(jg_vec.z)>EPS2)	//普通斜构件平面
	{
		if(pMirStart)
			Sub_Pnt(ucs.axis_x,pMirStart->ActivePosition(),pStart->ActivePosition());
		else
			Sub_Pnt(ucs.axis_x,pMirEnd->ActivePosition(),pEnd->ActivePosition());
		ucs.axis_z = cross_prod(jg_vec,ucs.axis_x);
		normalize(ucs.axis_z);
		if(ucs.axis_z.z>0)
			ucs.axis_z *= -1.0;
		//设置视线方向
		dlg.vPlaneNormal=-ucs.axis_z;
		if(dlg.DoModal()!=IDOK)
			return FALSE;
		ucs.axis_z=-dlg.vPlaneNormal;
		//尽量按照拾取的倾斜材展开视图
		double rot_ang=ACOS(console.GetActiveView()->ucs.axis_z,ucs.axis_z);
		f3dPoint rot_axis=console.GetActiveView()->ucs.axis_z^ucs.axis_z;
		f3dPoint axis_x=console.GetActiveView()->ucs.axis_x;
		if(RotateVectorAroundVector(axis_x,rot_ang,rot_axis))
			ucs.axis_x=axis_x;
		
		ucs.axis_y = cross_prod(ucs.axis_z,ucs.axis_x);
		if(ucs.axis_y.z>0)
		{
			ucs.axis_y*=-1;
			ucs.axis_x*=-1;
		}
	}
	else//水平横隔面
	{
		if(pMirStart)
			Sub_Pnt(ucs.axis_y,pMirStart->ActivePosition(),pStart->ActivePosition());
		else
			Sub_Pnt(ucs.axis_y,pMirEnd->ActivePosition(),pEnd->ActivePosition());
		ucs.axis_z = cross_prod(jg_vec,ucs.axis_y);
		normalize(ucs.axis_z);
		if(ucs.axis_z.z>0)
		{
			ucs.axis_z *= -1.0;
		}
		//设置视线方向
		dlg.m_fVectorX = -ucs.axis_z.x;
		if(fabs(dlg.m_fVectorX)<EPS)
			dlg.m_fVectorX=0;	//防止输入值为-0  wht 10-11-09
		dlg.m_fVectorY = -ucs.axis_z.y;
		if(fabs(dlg.m_fVectorY)<EPS)
			dlg.m_fVectorY=0;
		dlg.m_fVectorZ = -ucs.axis_z.z;
		if(fabs(dlg.m_fVectorZ)<EPS)
			dlg.m_fVectorZ=0;
		if(dlg.DoModal()!=IDOK)
			return FALSE;
		ucs.axis_z.x = -dlg.m_fVectorX;
		ucs.axis_z.y = -dlg.m_fVectorY;
		ucs.axis_z.z = -dlg.m_fVectorZ;
		if(ucs.axis_z.z>0)
			ucs.axis_y *= -1.0;
		ucs.axis_x = cross_prod(ucs.axis_y,ucs.axis_z);
	}
	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	normalize(ucs.axis_z);
	ucs.origin = pStart->ActivePosition();
	if(!InitDisplaySetByMirPoint(pLinePart,pMirStart,pMirEnd))	//根据对称节点组成的矩形区域初始化视图的构件集合
		return FALSE;
	console.SetActiveView(0);
	console.GetActiveView()->ucs=ucs;
	theApp.env.layer[2]='G';	//设置系统默认图层名第2个字母为G 表示为隔面
	g_pSolidDraw->BuildDisplayList(&LDSVIEW_CONTEXT(this));

	g_pSolidOper->ZoomAll(0.95);
	g_pSolidOper->ReSetView();
	m_eViewFlag = RANDOM_VIEW;
	return TRUE;
}
BOOL CLDSView::FinishSpreadFaceByNodes(CLDSNode* pNode1,CLDSNode* pNode2)
{
	if(pNode1==NULL || pNode2==NULL)
		return FALSE;
	int nCounts = 4;
	f3dPoint pPoints[4];
	UCS_STRU ucs;
	POLYGON selRgn;
	//确定多边形工作区域
	nCounts=4;
	pPoints[0] = pNode1->ActivePosition()+console.GetActiveView()->ucs.axis_z*100000;
	pPoints[1] = pNode2->ActivePosition()+console.GetActiveView()->ucs.axis_z*100000;
	pPoints[2] = pNode2->ActivePosition()-console.GetActiveView()->ucs.axis_z*100000;
	pPoints[3] = pNode1->ActivePosition()-console.GetActiveView()->ucs.axis_z*100000;
	if(!selRgn.CreatePolygonRgn(pPoints,nCounts))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "Nodes selection are illegal!";
#else
		throw "节点选择不合法!";
#endif
	//求任意面的用户相对坐标系(相对原塔坐标系)
	Sub_Pnt(ucs.axis_x,pNode2->ActivePosition(),pNode1->ActivePosition());
	ucs.axis_y=-console.GetActiveView()->ucs.axis_z;
	ucs.axis_z = cross_prod(ucs.axis_x,ucs.axis_y);
	normalize(ucs.axis_z);
	if(ucs.axis_z.z>0)
		ucs.axis_z *= -1.0;
	CSpreadVectorDlg dlg;
	//初始化示意面轮廓点
	pPoints[0] = pNode1->ActivePosition()+console.GetActiveView()->ucs.axis_z*1000;
	pPoints[1] = pNode2->ActivePosition()+console.GetActiveView()->ucs.axis_z*1000;
	pPoints[2] = pNode2->ActivePosition()-console.GetActiveView()->ucs.axis_z*1000;
	pPoints[3] = pNode1->ActivePosition()-console.GetActiveView()->ucs.axis_z*1000;
	dlg.InitPlaneVertex(pPoints,nCounts);
	//设置视线方向
	dlg.m_fVectorX = -ucs.axis_z.x;
	if(fabs(dlg.m_fVectorX)<EPS)
		dlg.m_fVectorX=0;
	dlg.m_fVectorY = -ucs.axis_z.y;
	if(fabs(dlg.m_fVectorY)<EPS)
		dlg.m_fVectorY=0;
	dlg.m_fVectorZ = -ucs.axis_z.z;
	if(fabs(dlg.m_fVectorZ)<EPS)
		dlg.m_fVectorZ=0;
	if(dlg.DoModal()!=IDOK)
		return FALSE;
	f3dPoint axis_z=ucs.axis_z;
	ucs.axis_z.x = -dlg.m_fVectorX;
	ucs.axis_z.y = -dlg.m_fVectorY;
	ucs.axis_z.z = -dlg.m_fVectorZ;
	if(ucs.axis_z*axis_z<0)	//Y轴与Z轴同时反向
		ucs.axis_y*=-1.0;
	
	ucs.axis_x = cross_prod(ucs.axis_y,ucs.axis_z);
	ucs.axis_y = cross_prod(ucs.axis_z,ucs.axis_x);
	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	ucs.origin = pNode1->ActivePosition();
	PushView();
	//根据设定的区域初始化显示集合
	if(InitDisplaySetByPolyRgn(selRgn)==FALSE)
		return FALSE;
	//将不在多边形区域的节点父杆件放到显示构件集合中
	for(CLDSNode* pNode=console.DispNodeSet.GetFirst();pNode;pNode=console.DispNodeSet.GetNext())
	{
		CLDSPart* pFatherPart=console.FromPartHandle(pNode->hFatherPart);
		CLDSPart* pPart=console.DispPartSet.FromHandle(pNode->hFatherPart);
		if(pPart==NULL&&pFatherPart)	//节点父杆件存在且不在多边形区域
			console.DispPartSet.append(pFatherPart);
	}
	console.SetActiveView(0);
	console.GetActiveView()->ucs=ucs;
	theApp.env.layer[2]='G';	//设置系统默认图层名第2个字母为G 表示为隔面
	g_pSolidDraw->BuildDisplayList(&LDSVIEW_CONTEXT(this));
	g_pSolidOper->ZoomAll(0.95);
	g_pSolidOper->ReSetView();
	m_eViewFlag = RANDOM_VIEW;
	return TRUE;
}
void CLDSView::OnSpreadNodeFace() 
{	//展开轮廓面
	m_nPrevCommandID=ID_SPREAD_NODE_FACE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to spread by profile nodes";
#else
	m_sPrevCommandName="重复展开轮廓面";
#endif
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSLinePart *pLinePart=NULL;
	CLDSNode *pNode,*pStart=NULL, *pEnd=NULL;
	f3dLine line;
	int i,node_n;
	f3dPoint origin,*pPoint;
	POLYGON selRgn;
	UCS_STRU ucs;
	NODESET nodeset;
	//-----vvvvvvv-标识函数运行状态为真，即同一时刻只能有一个塔创建函数运行---------
	if(!LockFunc())
		return;
	try
	{
		DISPLAY_TYPE type;
		g_pSolidSet->GetDisplayType(&type);
		//显示单线节点
		if(type==DISP_SOLID)
		{
			g_pSolidSet->SetDisplayType(DISP_LINE);
			g_pSolidDraw->Draw();
		}
		int nNode=1;
		CString sCmdstr;
		sCmdstr.Format("请选择第%d点<鼠标点击空白处结束选择>:",nNode);
		pCmdLine->FillCmdLine(sCmdstr,"");
		while(g_pSolidSnap->SnapPoint(pPoint)>0)
		{
			pNode = console.FromNodeHandle(pPoint->ID);
			g_pSolidDraw->SetEntSnapStatus(pPoint->ID);
			if(pNode)
			{
				nodeset.append(pNode);
				nNode++;
				sCmdstr.Format("请选择第%d点<鼠标点击空白处结束选择>:",nNode);
				pCmdLine->FillCmdLine(sCmdstr,"");
			}
			else
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Nodes have been deleted!";
#else
				throw "节点已被删除!";
#endif
		}
#ifdef AFX_TARG_ENU_ENGLISH
		node_n = nodeset.GetNodeNum();
		if(node_n<3)
			throw "Can not spread any node plane because of few selected nodes!";
#else
		node_n = nodeset.GetNodeNum();
		if(node_n<3)
			throw "选择节点太少不能展开任意节点面!";
#endif
		Ta.m_iCurSeg=nodeset[0]->iSeg;//记录当前定制视图对应的段号，新建视图时用来初始化组名 wht 10-12-27
		DYN_ARRAY<f3dPoint> pt_arr(node_n);
		CLDSNode *pVFaceNode=NULL,*pVTopNode1=NULL,*pVTopNode2=NULL;
		for(pNode=nodeset.GetFirst(),i=0;pNode;pNode=nodeset.GetNext(),i++)
		{
			pt_arr[i] = pNode->ActivePosition();
			pLinePart=Ta.FromRodHandle(pNode->hFatherPart);
			if(pVFaceNode==0&&isdigit(pNode->layer(2))&&pLinePart&&pLinePart->layer(2)==pNode->layer(2)&&
				pLinePart->layer(1)=='Z'&&(pNode->IsLegObj()||pLinePart->IsLegObj()))
				pVFaceNode=pNode;
			else if(pVTopNode1==NULL)
				pVTopNode1=pNode;
			else if(pVTopNode2==NULL)
				pVTopNode2=pNode;
		}
		
		if(node_n==3&&pVFaceNode!=NULL&&fabs(pVTopNode1->ActivePosition().z-pVTopNode2->ActivePosition().z)<=100)
		{
			Sub_Pnt(ucs.axis_y,pVTopNode1->ActivePosition(),pVFaceNode->ActivePosition());
			//求任意面的用户相对坐标系(相对原塔坐标系)
			Sub_Pnt(ucs.axis_x,pVTopNode1->ActivePosition(),pVTopNode2->ActivePosition());
			ucs.axis_z = cross_prod(ucs.axis_x,ucs.axis_y);
			ucs.axis_y = ucs.axis_z^ucs.axis_x;
			if(ucs.axis_z.z<0)
			{
				ucs.axis_z*=-1.0;
				ucs.axis_y*=-1.0;
			}
			if(ucs.axis_y.z>0)
			{	//角钢方向暂取为向上为正, 同时根据Y轴修正X轴正向
				ucs.axis_x*=-1.0;
				ucs.axis_y*=-1.0;
			}
		}
		else
		{
			Sub_Pnt(ucs.axis_y,nodeset[0]->ActivePosition(),nodeset[1]->ActivePosition());
			if(ucs.axis_y.z>0)	//角钢方向暂取为向上为正
				ucs.axis_y *= -1.0;
			//求任意面的用户相对坐标系(相对原塔坐标系)
			Sub_Pnt(ucs.axis_x,nodeset[0]->ActivePosition(),nodeset[2]->ActivePosition());
			ucs.axis_z = cross_prod(ucs.axis_x,ucs.axis_y);
			if(ucs.axis_z.z>0)
				ucs.axis_z *= -1.0;
		}
		normalize(ucs.axis_z);
		CSpreadVectorDlg dlg;
		dlg.InitPlaneVertex(pt_arr,node_n);
		//设置视线方向
		dlg.m_fVectorX = -ucs.axis_z.x;
		if(fabs(dlg.m_fVectorX)<EPS)
			dlg.m_fVectorX=0;	//防止输入值为-0  wht 10-11-09
		dlg.m_fVectorY = -ucs.axis_z.y;
		if(fabs(dlg.m_fVectorY)<EPS)
			dlg.m_fVectorY=0;
		dlg.m_fVectorZ = -ucs.axis_z.z;
		if(fabs(dlg.m_fVectorZ)<EPS)
			dlg.m_fVectorZ=0;
		if(dlg.DoModal()!=IDOK)
		{
			g_pSolidSet->SetDisplayType(type);
			g_pSolidDraw->Draw();
			g_pSolidDraw->ReleaseSnapStatus();
			pCmdLine->CancelCmdLine();
			ReleaseFunc();
			return;
		}
		ucs.axis_z.x = -dlg.m_fVectorX;
		ucs.axis_z.y = -dlg.m_fVectorY;
		ucs.axis_z.z = -dlg.m_fVectorZ;

		ucs.axis_x = cross_prod(ucs.axis_y,ucs.axis_z);
		ucs.axis_y = cross_prod(ucs.axis_z,ucs.axis_x);
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		ucs.origin = nodeset[0]->ActivePosition();
		
		if(!selRgn.CreatePolygonRgn(pt_arr, node_n))
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Nodes selection are illegal!";
#else
			throw "节点选择不合法!";
#endif
		PushView();
		//根据设定的区域初始化显示集合
		if(InitDisplaySetByPolyRgn(selRgn)==FALSE)
			return;
		console.SetActiveView(0);
		console.GetActiveView()->ucs=ucs;
		theApp.env.layer[2]='G';	//设置系统默认图层名第2个字母为G 表示为隔面
		g_pSolidDraw->BuildDisplayList(&LDSVIEW_CONTEXT(this));

		g_pSolidOper->ZoomAll(0.95);
		g_pSolidOper->ReSetView();
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		pCmdLine->CancelCmdLine();
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		g_pSolidDraw->Draw();
		ReleaseFunc();
		g_pSolidDraw->ReleaseSnapStatus();
		return;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	ReleaseFunc();
	m_eViewFlag = RANDOM_VIEW;
}


void CLDSView::OnSelectNodeFace()
{
	m_nPrevCommandID=ID_SELECT_NODE_FACE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to select by profile nodes";
#else
	m_sPrevCommandName="重复选择轮廓面构件";
#endif
	f3dPoint *pPoint;
	CLDSNode *pNode;
	NODESET nodeset;
	int i=0;
	UCS_STRU ucs;
	ARRAY_LIST<f3dPoint>vertexList;
	while(g_pSolidSnap->SnapPoint(pPoint)>0)
	{
		pNode = console.FromNodeHandle(pPoint->ID);
		g_pSolidDraw->SetEntSnapStatus(pPoint->ID);
		if(pNode)
			nodeset.append(pNode);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Nodes have been deleted!");
#else
			AfxMessageBox("节点已被删除!");
#endif
	}
	g_pSolidDraw->ReleaseSnapStatus();
	if(nodeset.GetNodeNum()<3)
	{
		if (nodeset.GetNodeNum()!=2)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The number of nodes is less than 2!");
#else
			AfxMessageBox("节点数小于2!");
#endif
			return;
		}
		f3dPoint start = nodeset.GetFirst()->xPreliftPos;
		f3dPoint end = nodeset.GetNext()->xPreliftPos;
		f3dLine pLine(start,end);
		CPtInLineCheck lineCheck(start,end);
		for (CLDSNode* pTempNode = Ta.Node.GetFirst();pTempNode;pTempNode = Ta.Node.GetNext())
		{
			f3dPoint pPt = pTempNode->xPreliftPos;
			double fCheck = lineCheck.CheckPoint(pPt);
			
			if (fCheck<1 && fCheck>0)
			{
				g_pSolidDraw->SetEntSnapStatus(pTempNode->handle);
			}
		}
		for (CLDSLinePart* pLinePart = Ta.Parts.GetFirstLinePart();pLinePart;pLinePart = Ta.Parts.GetNextLinePart())
		{
			if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
				continue;
			CPtInLineCheck lineCheck2(pLinePart->pStart->xPreliftPos,pLinePart->pEnd->xPreliftPos);
			double scale_start,scale_end;
			scale_start=lineCheck.CheckPoint(pLinePart->pStart->xPreliftPos);
			scale_end=lineCheck.CheckPoint(pLinePart->pEnd->xPreliftPos);
			if((scale_start<0||scale_start>1)&&(scale_end<0||scale_end>1))
				continue;	//两杆件不重叠
			if((scale_start>=0&&scale_start<=1)&&(scale_end>=0&&scale_end<=1))
				g_pSolidDraw->SetEntSnapStatus(pLinePart->handle);
			else
			{	//短杆件与长杆件有重叠
				double dd=lineCheck.LineStdVec()*lineCheck2.LineStdVec();
				if(fabs(dd)<EPS_COS)
					continue;	//不共向不可能共线
				if(dd<0)
				{	//反向时调转始末端
					double overlapp=scale_start;
					scale_start=scale_end;
					scale_end=overlapp;
				}
				if(scale_start<0)
					scale_start=0;
				if(scale_end>1)
					scale_end=1;
				if(scale_start<scale_end)
					g_pSolidDraw->SetEntSnapStatus(pLinePart->handle);
			}
		}
		return;
	}
	//建立参照坐标系
	vertexList.SetSize(nodeset.GetNodeNum());
	for(pNode=nodeset.GetFirst(),i=0;pNode;pNode=nodeset.GetNext(),i++)
	{
		vertexList[i] = pNode->xPreliftPos;
		if(i==0)
			ucs.origin=vertexList[i];
		else if(ucs.axis_x.IsZero())
		{
			ucs.axis_x=vertexList[i]-vertexList[i-1];
			normalize(ucs.axis_x);
		}
		else if(ucs.axis_y.IsZero())
		{
			ucs.axis_y=vertexList[i]-vertexList[i-1];
			normalize(ucs.axis_y);
			if(fabs(ucs.axis_y*ucs.axis_x)>EPS_COS)	//与X轴共线为无效Y轴方向,清空继续寻找
				ucs.axis_y.Set();
			else
			{	//找到合理坐标系
				ucs.axis_z = ucs.axis_x^ucs.axis_y;
				ucs.axis_y = ucs.axis_z^ucs.axis_x;
				normalize(ucs.axis_y);
				normalize(ucs.axis_z);
			}
		}
	}
	double minZ=0,maxZ=0;
	for(i=0;i<nodeset.GetNodeNum();i++)
	{
		coord_trans(vertexList[i],ucs,FALSE);
		if(minZ>vertexList[i].z)
			minZ=vertexList[i].z;
		if(maxZ<vertexList[i].z)
			maxZ=vertexList[i].z;
		vertexList[i].z=0;
	}
	//消除凹角
	int n=vertexList.GetSize();
	for(i=0;i<n;i++)
	{
		double result=DistOf2dPtLine(vertexList[(i+2)%n],vertexList[i],vertexList[(i+1)%n]);
		if(result<0)	// 后点在线右侧，有凹角出现
		{
			vertexList.RemoveAt((i+1)%n);
			n--;
		}
	}
	//选择合适杆件
//	ARRAY_LIST<long>vertexCutList;
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if (pPart->IsLinePart())
		{
			CLDSLinePart *pLinePart = (CLDSLinePart *)pPart;
			if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
				continue;
			f3dPoint start=pLinePart->pStart->xPreliftPos;
			f3dPoint end=pLinePart->pEnd->xPreliftPos;
			coord_trans(start,ucs,FALSE,TRUE);
			coord_trans(end,ucs,FALSE,TRUE);
			bool bInternal=false;
			if(start.z>=minZ-EPS&&start.z<=maxZ+EPS&&end.z>=minZ-EPS&&end.z<=maxZ+EPS)
			{
				//去除边界线
				BOOL bContiue = TRUE;
				if (!g_sysPara.m_bCanSelectEdgeLine)
				{
					for(i=0;i<n;i++)
					{
						CPtInLineCheck lineCheck(vertexList[(i+1)%n],vertexList[i]);
						double fCheckStart = lineCheck.CheckPoint(start);
						double fCheckEnd = lineCheck.CheckPoint(end);
						if (fCheckStart<=1 && fCheckStart>=0 && fCheckEnd<=1 && fCheckEnd>=0)
						{
							bContiue = FALSE;
							break;
						}
					}
				}
				if (!bContiue)
					continue;
				for(i=0;i<n;i++)
				{
					f3dPoint v1=vertexList[(i+1)%n]-vertexList[i];
					f3dPoint v2=start-vertexList[i];
					f3dPoint v3=end-vertexList[i];
					double L1=v1.mod();
					double L2=v2.mod();
					double L3=v3.mod();
					if(L2>EPS&&v1.x*v2.y-v1.y*v2.x<-EPS2*L1*L2) 
						break;	//起点在某条边的右侧故不是凸多边形的内点
					if(L3>EPS&&v1.x*v3.y-v1.y*v3.x<-EPS2*L1*L3) 
						break;	//终点在某条边的右侧故不是凸多边形的内点
				}
				if(i==n)
					g_pSolidDraw->SetEntSnapStatus(pLinePart->handle);
			}
		}
#ifndef __TSA_
		else if(pPart->GetClassTypeId() == CLS_PLATE || pPart->GetClassTypeId() == CLS_PARAMPLATE)
		{
			f3dLine baseLine;
			CLDSNode *pBaseNode=NULL;
			if (pPart->GetClassTypeId() == CLS_PLATE)
			{
				CLDSPlate *pPlate = (CLDSPlate *)pPart;
				pBaseNode=pPlate->GetBaseNode();
			}
			else if (pPart->GetClassTypeId() == CLS_PARAMPLATE)
			{
				CLDSParamPlate *pParaPlate = (CLDSParamPlate *)pPart;
				pBaseNode=pParaPlate->GetBaseNode();
			}
			if(pBaseNode==NULL)
				continue;
			f3dPoint basePt=pBaseNode->xPreliftPos;
			coord_trans(basePt,ucs,FALSE,TRUE);
			if(basePt.z>=minZ-EPS&&basePt.z<=maxZ+EPS)
			{
				int i;
				for(i=0;i<n;i++)
				{
					f3dPoint v1=vertexList[(i+1)%n]-vertexList[i];
					f3dPoint v2=basePt-vertexList[i];
					double L1=v1.mod();
					double L2=v2.mod();
					if(L2>EPS&&v1.x*v2.y-v1.y*v2.x<-EPS2*L1*L2) 
						break;	//起点在某条边的右侧故不是凸多边形的内点
				}
				if(i==n) //选中当前钢板
					g_pSolidDraw->SetEntSnapStatus(pPart->handle);
			}
		}
#endif
	}
	//选择合适点
	for (CLDSNode* pTempNode = Ta.Node.GetFirst();pTempNode;pTempNode = Ta.Node.GetNext())
	{
		f3dPoint pPt = pTempNode->xPreliftPos;
		coord_trans(pPt,ucs,FALSE,TRUE);
		if(pPt.z>=minZ-EPS&&pPt.z<=maxZ+EPS)
		{
			for(i=0;i<n;i++)
			{
				f3dPoint v1=vertexList[(i+1)%n]-vertexList[i];
				f3dPoint v2=pPt-vertexList[i];
				double L1=v1.mod();
				double L2=v2.mod();
				if(L2>EPS&&v1.x*v2.y-v1.y*v2.x<-EPS2*L1*L2) 
					break;	//起点在某条边的右侧故不是凸多边形的内点
				if (!g_sysPara.m_bCanSelectEdgeLine)
				{
					normalize(v1);
					normalize(v2);
					if(v1*v2>EPS_COS)
						break;	
				}
			}
			if(i==n) //选中当前钢板
				g_pSolidDraw->SetEntSnapStatus(pTempNode->handle);
		}
		
	}
	//更新属性栏,便于修改选中构件的属性 
	UpdatePropertyPage();
}
/* 该命令使用频率极低，暂去掉以视后效　wjh-2015.3.7
void CLDSView::OnNewWorkFace() 
{
	static CCoordDefDlg coord_dlg;
	UCS_STRU ucs;

	if(coord_dlg.DoModal()==IDOK)
	{
		ucs = coord_dlg.ucs;
		console.DispNodeSet.Empty();
		console.DispPartSet.Empty();
		g_pSolidDraw->BuildDisplayList();
		g_pSolidOper->ZoomAll(0.95);
		g_pSolidOper->ReSetView();
		
		m_eViewFlag = RANDOM_VIEW;
	}
}
*/

void CLDSView::OnShiftView(UINT cmdID)
{
	CTowerTreeDlg *pTowerTreeDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
	if(pTowerTreeDlg==NULL)
		return;
	int idArr[6]={ID_PERSPECTIVE_VIEW,ID_FRONT_VIEW,ID_RIGHT_VIEW,ID_BACK_VIEW,ID_LEFT_VIEW,ID_DOWNWARD_VIEW};
	for(int i=0;i<6;i++)
	{
		if(cmdID==idArr[i])
			pTowerTreeDlg->ActivateLayerViewItem(i);
	}
}
static BOOL IsSameUcs(UCS_STRU srcUcs,UCS_STRU destUcs)
{
	if(srcUcs.axis_x.IsEqual(destUcs.axis_x)&&
		srcUcs.axis_y.IsEqual(destUcs.axis_y)&&
		srcUcs.axis_z.IsEqual(destUcs.axis_z))
		return TRUE;
	else
		return FALSE;
}
/*
 * iViewType对应 0代表透视图 1代表前视图 2代表右面图 3代表后面图 4代表左面图 5代表俯视图 
 */
BOOL IsMatchedView(CDisplayView *pView,int iViewType)
{
	if(iViewType<0||iViewType>=6||pView==NULL)
		return FALSE;
	UCS_STRU ucsArr[6]={ UCS_STRU(GECS(GEPOINT(0,0,0),GEPOINT(1,0,0),GEPOINT(0,0,-1),GEPOINT(0,1,0))),   //透视图
						 UCS_STRU(GECS(GEPOINT(0,0,0),GEPOINT(1,0,0),GEPOINT(0,0,-1),GEPOINT(0,1,0))),   //前视图
						 UCS_STRU(GECS(GEPOINT(0,0,0),GEPOINT(0,-1,0),GEPOINT(0,0,-1),GEPOINT(1,0,0))),  //右面图
						 UCS_STRU(GECS(GEPOINT(0,0,0),GEPOINT(-1,0,0),GEPOINT(0,0,-1),GEPOINT(0,-1,0))), //后面图
						 UCS_STRU(GECS(GEPOINT(0,0,0),GEPOINT(0,1,0),GEPOINT(0,0,-1),GEPOINT(-1,0,0))),  //左面图
						 UCS_STRU(GECS(GEPOINT(0,0,0),GEPOINT(1,0,0),GEPOINT(0,-1,0),GEPOINT(0,0,-1)))}; //俯视图
	if(IsSameUcs(pView->ucs,ucsArr[iViewType]))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if((iViewType==0&&strcmp(pView->name,"Perspective View")!=0)||
			(iViewType==1&&strcmp(pView->name,"Front View")!=0))
#else
		if((iViewType==0&&strcmp(pView->name,"透视图")!=0)||
			(iViewType==1&&strcmp(pView->name,"前面图")!=0))
#endif
			return FALSE;
		else	
			return TRUE;
	}
	//在读取旧版本（TMA V4.1格式）式以下代码会导致误报 wjh-2017.1.20
	//else if( pView->m_iViewType==0&&
	//		((pView->handle==33&&iViewType==0)||
	//		(pView->handle==34&&iViewType==1)||
	//		(pView->handle==35&&iViewType==2)||
	//		(pView->handle==36&&iViewType==3)||
	//		(pView->handle==37&&iViewType==4)||
	//		(pView->handle==38&&iViewType==5)))
	//	return TRUE;
	else
		return FALSE;
}

void CLDSView::OnUpdateShiftView(CCmdUI *pCmdUI)
{
	int idArr[6]={ID_PERSPECTIVE_VIEW,ID_FRONT_VIEW,ID_RIGHT_VIEW,ID_BACK_VIEW,ID_LEFT_VIEW,ID_DOWNWARD_VIEW};
	CDisplayView *pView=console.GetActiveView();
	for(int i=0;i<6;i++)
	{
		if(pCmdUI->m_nID==idArr[i])
		{
			pCmdUI->SetRadio(IsMatchedView(pView,i));
			return;
		}
	}
}