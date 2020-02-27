// MouseStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "MouseStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMouseStatusBar

CMouseStatusBar::CMouseStatusBar()
{
	m_crTxtColor=0;
}

CMouseStatusBar::~CMouseStatusBar()
{
}


BEGIN_MESSAGE_MAP(CMouseStatusBar, CMFCStatusBar)
	//{{AFX_MSG_MAP(CMouseStatusBar)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_DRAWITEM()
	ON_COMMAND(ID_UI_OF_MECHANICS, OnShiftUiOfMechanics)
	ON_COMMAND(ID_UI_OF_RAPID_3D_MODEL, OnShiftUiOfRapid3dModel)
	ON_COMMAND(ID_UI_OF_FINAL_3D_MODEL, OnShiftUiOfFinal3dModel)
	ON_COMMAND(ID_UI_OF_DRAWING_MODEL, OnShiftUiOfDrawingModel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMouseStatusBar message handlers

bool CMouseStatusBar::SetPaneTextById(UINT idPaneItemId,const char* szPaneText,BOOL bUpdate)
{
	for(int i=0;i<m_nCount;i++)
	{
		UINT idItem=this->GetItemID(i);
		if(idItem==idPaneItemId)
		{
			SetPaneText(i,szPaneText,bUpdate);
			return true;
		}
	}
	return false;
}

void CMouseStatusBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRect rect;
	for (int i = 0; i < m_nCount; i++)
	{
		UINT idItem = this->GetItemID(i);
		if (idItem == ID_INDICATOR_MESSAGE)
		{
			GetItemRect(i, &rect);
			break;
		}
	}
	if(rect.PtInRect(point))	//处理未处理信息包（如阅读消息或更新数据等）
		theApp.ProcessInfoPackage();
	CMFCStatusBar::OnLButtonDblClk(nFlags, point);
}

void CMouseStatusBar::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef __LDS_
	CRect rect;
	int index = this->CommandToIndex(ID_INDICATOR_WORKENV);
	GetItemRect(index, &rect);
	if (rect.PtInRect(point))
		ContextMenu(point);
#endif
	CMFCStatusBar::OnLButtonDown(nFlags, point);
}
void CMouseStatusBar::OnRButtonDown(UINT nFlags, CPoint point)
{
#ifdef __LDS_
	CRect rect;
	int index = this->CommandToIndex(ID_INDICATOR_WORKENV);
	GetItemRect(index, &rect);
	if (rect.PtInRect(point))
		ContextMenu(point);
#endif
	CMFCStatusBar::OnRButtonDown(nFlags, point);
}

void CMouseStatusBar::ContextMenu(CPoint point)
{
	CPoint scr_point = point;
	ClientToScreen(&scr_point);
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu = popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0, MF_BYPOSITION);
	pMenu->AppendMenu(MF_STRING, ID_UI_OF_MECHANICS, "力学计算");
	pMenu->AppendMenu(MF_STRING, ID_UI_OF_RAPID_3D_MODEL, "快速建模");
	pMenu->AppendMenu(MF_STRING, ID_UI_OF_DRAWING_MODEL, "建模绘图");
	pMenu->AppendMenu(MF_STRING,ID_UI_OF_FINAL_3D_MODEL,"建模放样");
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, scr_point.x, scr_point.y, this);
}

void CMouseStatusBar::OnShiftUiOfMechanics()
{
	CMainFrame* pMainFrm = (CMainFrame*)theApp.GetMainWnd();
	if (pMainFrm)
		pMainFrm->ShiftUiOfMechanics();
}

void CMouseStatusBar::OnShiftUiOfRapid3dModel()
{
	CMainFrame* pMainFrm = (CMainFrame*)theApp.GetMainWnd();
	if (pMainFrm)
		pMainFrm->ShiftUiOfRapid3dModel();
}

void CMouseStatusBar::OnShiftUiOfFinal3dModel()
{
	CMainFrame* pMainFrm = (CMainFrame*)theApp.GetMainWnd();
	if (pMainFrm)
		pMainFrm->ShiftUiOfFinal3dModel();
}

void CMouseStatusBar::OnShiftUiOfDrawingModel()
{
	CMainFrame* pMainFrm = (CMainFrame*)theApp.GetMainWnd();
	if (pMainFrm)
		pMainFrm->ShiftUiOfDrawingModel();
}

void CMouseStatusBar::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	dc.SetTextColor(m_crTxtColor);
	CMFCStatusBar::OnDrawItem(nIDCtl, lpDrawItemStruct);
	dc.Detach();
}
