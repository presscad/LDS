//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"

#include "LDSDoc.h"
#include "LDSView.h"
#include "env_def.h"
#include "MainFrm.h"
#include "Tower.h"
#include "PromptDlg.h"
#include "OperCtrlCenterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CLDSView::OnRedrawAll() 
{
	m_nPrevCommandID=ID_REDRAW_ALL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to refresh";
#else
	m_sPrevCommandName="�ظ�ˢ��";
#endif
	RedrawAll();
}
double CLDSView::GetPickPosScaleOnRodS2E(CLDSLinePart* pRod)
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	GEPOINT posOnClient(point.x,point.y);
	DISPLAY_TYPE disp;
	g_pSolidSet->GetDisplayType(&disp);

	GEPOINT lineStart,lineEnd;
	if(disp==DISP_LINE&&pRod->pStart!=NULL&&pRod->pEnd!=NULL)
	{
		lineStart=pRod->pStart->xActivePos;
		lineEnd=pRod->pEnd->xActivePos;
	}
	else
	{
		lineStart=pRod->Start();
		lineEnd=pRod->End();
	}
	//���˼�����ת������Ļ��������ϵ��
	lineStart=g_pSolidOper->TransPToScr(lineStart);
	lineEnd=g_pSolidOper->TransPToScr(lineEnd);
	lineStart.z=lineEnd.z=0;
	GEPOINT lineStdVec=lineEnd-lineStart;
	double length=lineStdVec.mod();
	if(length<EPS)
		return 0;
	lineStdVec/=length;
	double scale=(lineStdVec*(posOnClient-lineStart))/length;
	return scale;
}

int CLDSView::RenderAll()
{
	DISPLAY_TYPE type;
	g_pSolidSet->GetDisplayType(&type);
	if(type==DISP_SOLID)
	{
		for(CLDSPart *pPart=console.DispPartSet.GetFirst();pPart;pPart=console.DispPartSet.GetNext())
			pPart->SetModified();
	}
	return RedrawAll();
}
int CLDSView::RedrawAll() 
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Refresh screen:","");
#else
	pCmdLine->FillCmdLine("Refresh ˢ����Ļ:","");
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(m_curAppType==TA_PART_EDITOR&&console.m_pWorkPart)
		console.m_pWorkPart->SetModified();
	else if(m_curAppType==TA_ASSEMBLE_ENV)
#endif
	{
		for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			pNode->is_visible = TRUE;
		//�����Ƿ���ʾ�ߵ���ʼ�˱�� ������ʾ�Ƕȱ�ע��ʱ�����˲���ʾ�ñ�ǣ�ˢ��ʱ��ָ�ԭ�������ã�
		g_pSolidSet->SetDisplayLineVecMark(CLDSLinePart::bDisplayLineVecMark);
		for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
			pPart->is_visible = TRUE;
	}
	g_pSolidDraw->BuildDisplayList(this);
	g_pSolidDraw->Draw();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("����:","");
#endif
	if(m_curAppType==TA_ASSEMBLE_ENV)
	{
		console.m_pWorkPart=NULL;	//ˢ�º���յ�ǰ�������� wht 10-09-03
		if(!g_sysPara.m_bDispWorkUcs)
		{	//ϵͳ����Ϊ����ʾ��������ϵʱ,����ʾ������ʱӦ��ʱ��ȥ��������ϵ wht 10-09-15
			//g_pSolidSet->SetDispWorkUcs(FALSE);
			g_pSolidDraw->BatchClearCS(2);
			Invalidate(FALSE);
		}
	}
	//ˢ����Ļ��������ԶԻ���
	UpdatePropertyPage();
	return 0;
}

void CLDSView::OperOther()
{
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	g_pSolidSet->SetOperType(OPER_OTHER);
	m_curTask = TASK_OTHER;
}

void CLDSView::OnOperOther() 
{
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	g_pSolidSet->SetOperType(OPER_OTHER);
	m_curTask = TASK_OTHER;
	m_setSnapObjects.Empty();
	if(g_pPromptMsg->GetSafeHwnd()!=0)
		g_pPromptMsg->Destroy();
}

void CLDSView::OnGlOpenWindow() 
{	//����
	m_nPrevCommandID=ID_GL_OPEN_WINDOW;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to open window";
#else
	m_sPrevCommandName="�ظ�����";
#endif
	g_pSolidSet->SetOperType(OPER_ZOOMWND);
}

void CLDSView::OnUpdateGlOpenWindow(CCmdUI* pCmdUI) 
{
	OPER_TYPE oper_type;
	oper_type = g_pSolidSet->GetOperType();
	if(oper_type==OPER_ZOOMWND)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CLDSView::OnGlAllZoom() 
{	//ȫ��
	m_nPrevCommandID=ID_GL_ALL_ZOOM;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to zoom to extent";
#else
	m_sPrevCommandName="�ظ�ȫ��";
#endif
	g_pSolidOper->ZoomAll(0.95);
}

void CLDSView::OnGlZoom() 
{	//����
	m_nPrevCommandID=ID_GL_ZOOM;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to zoom";
#else
	m_sPrevCommandName="�ظ�����";
#endif
	g_pSolidSet->SetOperType(OPER_ZOOM);	
}

void CLDSView::OnUpdateGlZoom(CCmdUI* pCmdUI) 
{
	OPER_TYPE oper_type;
	oper_type = g_pSolidSet->GetOperType();
	if(oper_type==OPER_ZOOM)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CLDSView::OnGlPan() 
{	//ƽ��
	m_nPrevCommandID=ID_GL_PAN;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to plane offset";
#else
	m_sPrevCommandName="�ظ�ƽ��";
#endif
	g_pSolidSet->SetOperType(OPER_PAN);	
}

void CLDSView::OnUpdateGlPan(CCmdUI* pCmdUI) 
{
	OPER_TYPE oper_type;
	oper_type = g_pSolidSet->GetOperType();
	if(oper_type==OPER_PAN)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CLDSView::OnGlRotated() 
{	//��ת
	m_nPrevCommandID=ID_GL_ROTATED;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to rotate";
#else
	m_sPrevCommandName="�ظ���ת";
#endif
	g_pSolidSet->SetOperType(OPER_ROTATE);	
}

void CLDSView::OnUpdateGlRotated(CCmdUI* pCmdUI) 
{
	OPER_TYPE oper_type;
	oper_type = g_pSolidSet->GetOperType();
	if(oper_type==OPER_ROTATE)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CLDSView::OnResetView() 
{	//��λ
	m_nPrevCommandID=ID_RESET_VIEW;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to reset";
#else
	m_sPrevCommandName="�ظ���λ";
#endif
	g_pSolidOper->ReSetView();
}

void CLDSView::OnOperControlCenter() 
{	//��������
	m_nPrevCommandID=ID_OPER_CONTROL_CENTER;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat control center";
#else
	m_sPrevCommandName="�ظ���������";
#endif
	f3dPoint org;
	COperCtrlCenterDLg dlg;
	ZOOM_CENTER_STYLE zoom_style;
	g_pSolidSet->GetRotOrg(&org);
	g_pSolidSet->GetZoomStyle(&zoom_style);
	dlg.m_fRotOrgX=org.x; dlg.m_fRotOrgY=org.y; dlg.m_fRotOrgZ=org.z;
	dlg.m_iZoomCenterStyle = zoom_style;
	if(dlg.DoModal()==IDOK)
	{
		org.Set(dlg.m_fRotOrgX,dlg.m_fRotOrgY,dlg.m_fRotOrgZ);
		g_pSolidSet->SetRotOrg(org);

		if(dlg.m_iZoomCenterStyle==MOUSE_CENTER)
			g_pSolidSet->SetZoomStyle(MOUSE_CENTER);
		else if(dlg.m_iZoomCenterStyle==SCR_CENTER)
			g_pSolidSet->SetZoomStyle(SCR_CENTER);
		else
			g_pSolidSet->SetZoomStyle(ROT_CENTER);
		Invalidate(FALSE);
	}
}

void CLDSView::OnNodeCenter() 
{	//�ڵ�����
	m_nPrevCommandID=ID_NODE_CENTER;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat node center";
#else
	m_sPrevCommandName="�ظ��ڵ�����";
#endif
	f3dPoint org;
	f3dPoint *center;
	DISPLAY_TYPE mode;
	g_pSolidSet->GetDisplayType(&mode);
	bool disabled=false;
	if(g_sysPara.m_bLineDepthTest&&mode==DISP_SOLID)
	{
		g_pSolidSet->DisableLineModelDepthTest();
		g_pSolidDraw->Draw();
		disabled=true;
	}
	if(g_pSolidSnap->SnapPoint(center,TRUE)>0)
	{
		CLDSNode *pNode = console.GetActiveModel()->FromNodeHandle(center->ID);
		if(pNode)
		{
			org = pNode->ActivePosition();
			g_pSolidSet->SetRotOrg(org);
			Invalidate(FALSE);
		}
	}
	if(disabled)
	{
		g_pSolidSet->EnableLineModelDepthTest();
		g_pSolidDraw->Draw();
	}
}

void CLDSView::OnPartPermSwitch() 
{	//��ʾ���й���
	theApp.m_bDisplayAllPart=!theApp.m_bDisplayAllPart;
	RedrawAll();
}

void CLDSView::OnUpdatePartPermSwitch(CCmdUI* pCmdUI) 
{
	if(theApp.m_bDisplayAllPart)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CLDSView::OnRefreshBurglarLs() 
{	//ˢ�·�����˨
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	m_nPrevCommandID=ID_REFRESH_BURGLAR_LS;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to refresh ant-theft bolt";
#else
	m_sPrevCommandName="�ظ�ˢ�·�����˨";
#endif
	Ta.RefreshBurglarLs();
	RedrawAll();	
#endif
}