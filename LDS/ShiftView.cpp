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
{	//չ��������
	m_nPrevCommandID=ID_SPREAD_FACE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to spread work plane";
#else
	m_sPrevCommandName="�ظ�չ��������";
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
	{	//�����߱�ִ����������������趨ϵͳ��������
		m_curTask = TASK_SPREAD_FACE;
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select datum node or rod for locating spread work plane:","");
#else
		pCmdLine->FillCmdLine("��ѡ��ȷ��չ��������Ļ�׼�ڵ��˼�:","");
#endif
		g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_ALL);
		return -1;
	}
	else
		pFirstObj=m_setSnapObjects.GetFirst();
	if(pFirstObj&&((CLDSPart*)pFirstObj)->IsLinePart())	//���ݸ˼�ȷ��չ��������
		retCode=FinishSpreadFaceByLinePart((CLDSLinePart*)pFirstObj);
	else if(pFirstObj->GetClassTypeId()==CLS_NODE)	//���������ڵ�ȷ��չ��������
	{
		CLDSNode *pFirstNode=(CLDSNode*)pFirstObj;
		if(m_setSnapObjects.GetNodeNum()==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("please select ending datum node for locating spread work plane:","");
#else
			pCmdLine->FillCmdLine("��ѡ��ȷ��չ����������ն˻�׼�ڵ�:","");
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
	//����չ������������״̬
	/*
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("please select starting datum node or rod for locating spread work plane:");
#else
	cmdStr.Format("��ѡ��ȷ��չ���������ʼ�˻�׼�ڵ��˼�:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	g_pSolidSet->SetDisplayType(DISP_LINE);//�л���������ʾģʽ
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
	Ta.m_iCurSeg=pLinePart->iSeg;	//��¼��ǰ������ͼ��Ӧ�Ķκţ��½���ͼʱ������ʼ������ wht 10-12-27
	if(m_eViewFlag==FRONT_VIEW|| m_eViewFlag==BACK_VIEW||m_eViewFlag==PERSPECTIVE)//չ��ǰ��Գ���
	{
		pMirStart = pStart->GetMirNode(MIRMSG(1));	//console.FromNodeHandle(pStart->mir_x_node_handle);	//��������������ʱ�޷�չ����ԭ��ͷ���������ĶԳƲ�����������  WJH 2003.11.29
		pMirEnd   = pEnd->GetMirNode(MIRMSG(1));	//console.FromNodeHandle(pEnd->mir_x_node_handle);
		if(pMirStart==NULL||pMirEnd==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Does not meet the spread condition(lacking of symmetrical parts), it cannot be spread");
#else
			AfxMessageBox("������չ������(ȱ�ٶԳƹ���),����չ��");
#endif
			return FALSE;
		}
		Sub_Pnt(jg_vec,pStart->ActivePosition(),pEnd->ActivePosition());
		if(jg_vec.z>0)	//�Ǹַ�����ȡΪ����Ϊ��
			jg_vec = -1.0*jg_vec;
	}
	else if(m_eViewFlag==LEFT_VIEW|| m_eViewFlag==RIGHT_VIEW)//չ�����ҶԳ���
	{
		pMirStart = pStart->GetMirNode(MIRMSG(2));	//console.FromNodeHandle(pStart->mir_y_node_handle);
		pMirEnd   = pEnd->GetMirNode(MIRMSG(2));	//console.FromNodeHandle(pEnd->mir_y_node_handle);
		if(pMirStart==NULL||pMirEnd==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Does not meet the spread condition(lacking of symmetrical parts), it cannot be spread");
#else
			AfxMessageBox("������չ������(ȱ�ٶԳƹ���),����չ��");
#endif
			return FALSE;
		}
		Sub_Pnt(jg_vec,pStart->ActivePosition(),pEnd->ActivePosition());
		if(jg_vec.z>0)	//�Ǹַ�����ȡΪ����Ϊ��
			jg_vec = -1.0*jg_vec;
	}
	else 
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can not spread any plane in an unknown view or a spread view!");
#else
		AfxMessageBox("������δ֪��ͼ��չ����ͼ��չ��������!");
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
		AfxMessageBox("ʵ��ͼ�в���,չ��������!");
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
	//����������û��������ϵ(���ԭ������ϵ)
	if(fabs(jg_vec.z)>EPS2)	//��ͨб����ƽ��
	{
		if(pMirStart)
			Sub_Pnt(ucs.axis_x,pMirStart->ActivePosition(),pStart->ActivePosition());
		else
			Sub_Pnt(ucs.axis_x,pMirEnd->ActivePosition(),pEnd->ActivePosition());
		ucs.axis_z = cross_prod(jg_vec,ucs.axis_x);
		normalize(ucs.axis_z);
		if(ucs.axis_z.z>0)
			ucs.axis_z *= -1.0;
		//�������߷���
		dlg.vPlaneNormal=-ucs.axis_z;
		if(dlg.DoModal()!=IDOK)
			return FALSE;
		ucs.axis_z=-dlg.vPlaneNormal;
		//��������ʰȡ����б��չ����ͼ
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
	else//ˮƽ�����
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
		//�������߷���
		dlg.m_fVectorX = -ucs.axis_z.x;
		if(fabs(dlg.m_fVectorX)<EPS)
			dlg.m_fVectorX=0;	//��ֹ����ֵΪ-0  wht 10-11-09
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
	if(!InitDisplaySetByMirPoint(pLinePart,pMirStart,pMirEnd))	//���ݶԳƽڵ���ɵľ��������ʼ����ͼ�Ĺ�������
		return FALSE;
	console.SetActiveView(0);
	console.GetActiveView()->ucs=ucs;
	theApp.env.layer[2]='G';	//����ϵͳĬ��ͼ������2����ĸΪG ��ʾΪ����
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
	//ȷ������ι�������
	nCounts=4;
	pPoints[0] = pNode1->ActivePosition()+console.GetActiveView()->ucs.axis_z*100000;
	pPoints[1] = pNode2->ActivePosition()+console.GetActiveView()->ucs.axis_z*100000;
	pPoints[2] = pNode2->ActivePosition()-console.GetActiveView()->ucs.axis_z*100000;
	pPoints[3] = pNode1->ActivePosition()-console.GetActiveView()->ucs.axis_z*100000;
	if(!selRgn.CreatePolygonRgn(pPoints,nCounts))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "Nodes selection are illegal!";
#else
		throw "�ڵ�ѡ�񲻺Ϸ�!";
#endif
	//����������û��������ϵ(���ԭ������ϵ)
	Sub_Pnt(ucs.axis_x,pNode2->ActivePosition(),pNode1->ActivePosition());
	ucs.axis_y=-console.GetActiveView()->ucs.axis_z;
	ucs.axis_z = cross_prod(ucs.axis_x,ucs.axis_y);
	normalize(ucs.axis_z);
	if(ucs.axis_z.z>0)
		ucs.axis_z *= -1.0;
	CSpreadVectorDlg dlg;
	//��ʼ��ʾ����������
	pPoints[0] = pNode1->ActivePosition()+console.GetActiveView()->ucs.axis_z*1000;
	pPoints[1] = pNode2->ActivePosition()+console.GetActiveView()->ucs.axis_z*1000;
	pPoints[2] = pNode2->ActivePosition()-console.GetActiveView()->ucs.axis_z*1000;
	pPoints[3] = pNode1->ActivePosition()-console.GetActiveView()->ucs.axis_z*1000;
	dlg.InitPlaneVertex(pPoints,nCounts);
	//�������߷���
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
	if(ucs.axis_z*axis_z<0)	//Y����Z��ͬʱ����
		ucs.axis_y*=-1.0;
	
	ucs.axis_x = cross_prod(ucs.axis_y,ucs.axis_z);
	ucs.axis_y = cross_prod(ucs.axis_z,ucs.axis_x);
	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	ucs.origin = pNode1->ActivePosition();
	PushView();
	//�����趨�������ʼ����ʾ����
	if(InitDisplaySetByPolyRgn(selRgn)==FALSE)
		return FALSE;
	//�����ڶ��������Ľڵ㸸�˼��ŵ���ʾ����������
	for(CLDSNode* pNode=console.DispNodeSet.GetFirst();pNode;pNode=console.DispNodeSet.GetNext())
	{
		CLDSPart* pFatherPart=console.FromPartHandle(pNode->hFatherPart);
		CLDSPart* pPart=console.DispPartSet.FromHandle(pNode->hFatherPart);
		if(pPart==NULL&&pFatherPart)	//�ڵ㸸�˼������Ҳ��ڶ��������
			console.DispPartSet.append(pFatherPart);
	}
	console.SetActiveView(0);
	console.GetActiveView()->ucs=ucs;
	theApp.env.layer[2]='G';	//����ϵͳĬ��ͼ������2����ĸΪG ��ʾΪ����
	g_pSolidDraw->BuildDisplayList(&LDSVIEW_CONTEXT(this));
	g_pSolidOper->ZoomAll(0.95);
	g_pSolidOper->ReSetView();
	m_eViewFlag = RANDOM_VIEW;
	return TRUE;
}
void CLDSView::OnSpreadNodeFace() 
{	//չ��������
	m_nPrevCommandID=ID_SPREAD_NODE_FACE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to spread by profile nodes";
#else
	m_sPrevCommandName="�ظ�չ��������";
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
	//-----vvvvvvv-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	if(!LockFunc())
		return;
	try
	{
		DISPLAY_TYPE type;
		g_pSolidSet->GetDisplayType(&type);
		//��ʾ���߽ڵ�
		if(type==DISP_SOLID)
		{
			g_pSolidSet->SetDisplayType(DISP_LINE);
			g_pSolidDraw->Draw();
		}
		int nNode=1;
		CString sCmdstr;
		sCmdstr.Format("��ѡ���%d��<������հ״�����ѡ��>:",nNode);
		pCmdLine->FillCmdLine(sCmdstr,"");
		while(g_pSolidSnap->SnapPoint(pPoint)>0)
		{
			pNode = console.FromNodeHandle(pPoint->ID);
			g_pSolidDraw->SetEntSnapStatus(pPoint->ID);
			if(pNode)
			{
				nodeset.append(pNode);
				nNode++;
				sCmdstr.Format("��ѡ���%d��<������հ״�����ѡ��>:",nNode);
				pCmdLine->FillCmdLine(sCmdstr,"");
			}
			else
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Nodes have been deleted!";
#else
				throw "�ڵ��ѱ�ɾ��!";
#endif
		}
#ifdef AFX_TARG_ENU_ENGLISH
		node_n = nodeset.GetNodeNum();
		if(node_n<3)
			throw "Can not spread any node plane because of few selected nodes!";
#else
		node_n = nodeset.GetNodeNum();
		if(node_n<3)
			throw "ѡ��ڵ�̫�ٲ���չ������ڵ���!";
#endif
		Ta.m_iCurSeg=nodeset[0]->iSeg;//��¼��ǰ������ͼ��Ӧ�Ķκţ��½���ͼʱ������ʼ������ wht 10-12-27
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
			//����������û��������ϵ(���ԭ������ϵ)
			Sub_Pnt(ucs.axis_x,pVTopNode1->ActivePosition(),pVTopNode2->ActivePosition());
			ucs.axis_z = cross_prod(ucs.axis_x,ucs.axis_y);
			ucs.axis_y = ucs.axis_z^ucs.axis_x;
			if(ucs.axis_z.z<0)
			{
				ucs.axis_z*=-1.0;
				ucs.axis_y*=-1.0;
			}
			if(ucs.axis_y.z>0)
			{	//�Ǹַ�����ȡΪ����Ϊ��, ͬʱ����Y������X������
				ucs.axis_x*=-1.0;
				ucs.axis_y*=-1.0;
			}
		}
		else
		{
			Sub_Pnt(ucs.axis_y,nodeset[0]->ActivePosition(),nodeset[1]->ActivePosition());
			if(ucs.axis_y.z>0)	//�Ǹַ�����ȡΪ����Ϊ��
				ucs.axis_y *= -1.0;
			//����������û��������ϵ(���ԭ������ϵ)
			Sub_Pnt(ucs.axis_x,nodeset[0]->ActivePosition(),nodeset[2]->ActivePosition());
			ucs.axis_z = cross_prod(ucs.axis_x,ucs.axis_y);
			if(ucs.axis_z.z>0)
				ucs.axis_z *= -1.0;
		}
		normalize(ucs.axis_z);
		CSpreadVectorDlg dlg;
		dlg.InitPlaneVertex(pt_arr,node_n);
		//�������߷���
		dlg.m_fVectorX = -ucs.axis_z.x;
		if(fabs(dlg.m_fVectorX)<EPS)
			dlg.m_fVectorX=0;	//��ֹ����ֵΪ-0  wht 10-11-09
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
			throw "�ڵ�ѡ�񲻺Ϸ�!";
#endif
		PushView();
		//�����趨�������ʼ����ʾ����
		if(InitDisplaySetByPolyRgn(selRgn)==FALSE)
			return;
		console.SetActiveView(0);
		console.GetActiveView()->ucs=ucs;
		theApp.env.layer[2]='G';	//����ϵͳĬ��ͼ������2����ĸΪG ��ʾΪ����
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
	m_sPrevCommandName="�ظ�ѡ�������湹��";
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
			AfxMessageBox("�ڵ��ѱ�ɾ��!");
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
			AfxMessageBox("�ڵ���С��2!");
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
				continue;	//���˼����ص�
			if((scale_start>=0&&scale_start<=1)&&(scale_end>=0&&scale_end<=1))
				g_pSolidDraw->SetEntSnapStatus(pLinePart->handle);
			else
			{	//�̸˼��볤�˼����ص�
				double dd=lineCheck.LineStdVec()*lineCheck2.LineStdVec();
				if(fabs(dd)<EPS_COS)
					continue;	//�����򲻿��ܹ���
				if(dd<0)
				{	//����ʱ��תʼĩ��
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
	//������������ϵ
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
			if(fabs(ucs.axis_y*ucs.axis_x)>EPS_COS)	//��X�Ṳ��Ϊ��ЧY�᷽��,��ռ���Ѱ��
				ucs.axis_y.Set();
			else
			{	//�ҵ���������ϵ
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
	//��������
	int n=vertexList.GetSize();
	for(i=0;i<n;i++)
	{
		double result=DistOf2dPtLine(vertexList[(i+2)%n],vertexList[i],vertexList[(i+1)%n]);
		if(result<0)	// ��������Ҳ࣬�а��ǳ���
		{
			vertexList.RemoveAt((i+1)%n);
			n--;
		}
	}
	//ѡ����ʸ˼�
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
				//ȥ���߽���
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
						break;	//�����ĳ���ߵ��Ҳ�ʲ���͹����ε��ڵ�
					if(L3>EPS&&v1.x*v3.y-v1.y*v3.x<-EPS2*L1*L3) 
						break;	//�յ���ĳ���ߵ��Ҳ�ʲ���͹����ε��ڵ�
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
						break;	//�����ĳ���ߵ��Ҳ�ʲ���͹����ε��ڵ�
				}
				if(i==n) //ѡ�е�ǰ�ְ�
					g_pSolidDraw->SetEntSnapStatus(pPart->handle);
			}
		}
#endif
	}
	//ѡ����ʵ�
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
					break;	//�����ĳ���ߵ��Ҳ�ʲ���͹����ε��ڵ�
				if (!g_sysPara.m_bCanSelectEdgeLine)
				{
					normalize(v1);
					normalize(v2);
					if(v1*v2>EPS_COS)
						break;	
				}
			}
			if(i==n) //ѡ�е�ǰ�ְ�
				g_pSolidDraw->SetEntSnapStatus(pTempNode->handle);
		}
		
	}
	//����������,�����޸�ѡ�й��������� 
	UpdatePropertyPage();
}
/* ������ʹ��Ƶ�ʼ��ͣ���ȥ�����Ӻ�Ч��wjh-2015.3.7
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
 * iViewType��Ӧ 0����͸��ͼ 1����ǰ��ͼ 2��������ͼ 3�������ͼ 4��������ͼ 5������ͼ 
 */
BOOL IsMatchedView(CDisplayView *pView,int iViewType)
{
	if(iViewType<0||iViewType>=6||pView==NULL)
		return FALSE;
	UCS_STRU ucsArr[6]={ UCS_STRU(GECS(GEPOINT(0,0,0),GEPOINT(1,0,0),GEPOINT(0,0,-1),GEPOINT(0,1,0))),   //͸��ͼ
						 UCS_STRU(GECS(GEPOINT(0,0,0),GEPOINT(1,0,0),GEPOINT(0,0,-1),GEPOINT(0,1,0))),   //ǰ��ͼ
						 UCS_STRU(GECS(GEPOINT(0,0,0),GEPOINT(0,-1,0),GEPOINT(0,0,-1),GEPOINT(1,0,0))),  //����ͼ
						 UCS_STRU(GECS(GEPOINT(0,0,0),GEPOINT(-1,0,0),GEPOINT(0,0,-1),GEPOINT(0,-1,0))), //����ͼ
						 UCS_STRU(GECS(GEPOINT(0,0,0),GEPOINT(0,1,0),GEPOINT(0,0,-1),GEPOINT(-1,0,0))),  //����ͼ
						 UCS_STRU(GECS(GEPOINT(0,0,0),GEPOINT(1,0,0),GEPOINT(0,-1,0),GEPOINT(0,0,-1)))}; //����ͼ
	if(IsSameUcs(pView->ucs,ucsArr[iViewType]))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if((iViewType==0&&strcmp(pView->name,"Perspective View")!=0)||
			(iViewType==1&&strcmp(pView->name,"Front View")!=0))
#else
		if((iViewType==0&&strcmp(pView->name,"͸��ͼ")!=0)||
			(iViewType==1&&strcmp(pView->name,"ǰ��ͼ")!=0))
#endif
			return FALSE;
		else	
			return TRUE;
	}
	//�ڶ�ȡ�ɰ汾��TMA V4.1��ʽ��ʽ���´���ᵼ���� wjh-2017.1.20
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