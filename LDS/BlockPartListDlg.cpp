// BlockPartListDLg.cpp : 实现文件
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "BlockPartListDLg.h"
#include "Tower.h"
#include "TowerPropertyDlg.h"
#include "MainFrm.h"
#include ".\blockpartlistdlg.h"

// CBlockPartListDlg 对话框

IMPLEMENT_DYNAMIC(CBlockPartListDlg, CDialog)
CBlockPartListDlg::CBlockPartListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBlockPartListDlg::IDD, pParent)
{
#ifdef AFX_TARG_ENU_ENGLISH
	m_xPartList.AddColumnHeader("Handle");
	m_xPartList.AddColumnHeader("Style");
#else 
	m_xPartList.AddColumnHeader("句柄");
	m_xPartList.AddColumnHeader("类型");
#endif
	m_xPartList.m_bEnableSortItems=TRUE;//排序
}

CBlockPartListDlg::~CBlockPartListDlg()
{
}

void CBlockPartListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BLOCK_PART,m_xPartList);
}


BEGIN_MESSAGE_MAP(CBlockPartListDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_LIST_BLOCK_PART, OnClickListPart)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_BLOCK_PART,OnKeydownListPart)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_BLOCK_PART, OnNMRclickListPart)
	ON_COMMAND(ID_DELETE_SELECT_PART, DeleteSelectItems)
END_MESSAGE_MAP()

static void InsertPartToList(CXhListCtrl *pListCtrl,CLDSPart *pPart)
{
	CString tempStr;
	CStringArray str_arr;
	tempStr.Format("0x%X",pPart->handle);
	str_arr.Add(tempStr);
	str_arr.Add(pPart->GetPartTypeName());
	int iCur=pListCtrl->InsertItemRecord(-1,str_arr);
	pListCtrl->SetItemData(iCur,(DWORD)pPart);
}

// CBlockPartListDlg 消息处理程序
BOOL CBlockPartListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(m_pBlock==NULL)
		return FALSE;
	long col_wide_arr[2]={73,80};
	m_xPartList.InitListCtrl(col_wide_arr);
	
	if(m_pBlock->IsEmbeded())
	{
		for(CLDSPart* pPart = console.EnumPartFirst();pPart;pPart=console.EnumPartNext())
			InsertPartToList(&m_xPartList,pPart);
	}
	else 
	{
		for(CLDSPart* pPart = m_pBlock->Parts.GetFirst();pPart;pPart=m_pBlock->Parts.GetNext())
			InsertPartToList(&m_xPartList,pPart);
	}
	//改变窗口位置
	CRect rect;
	GetWindowRect(rect);
	rect.right = rect.Width();
	rect.left = 0;
	rect.bottom = rect.Height();
	rect.top = 0;
	MoveWindow(rect,TRUE);
	return TRUE;
}
void CBlockPartListDlg::SelectPart(int nCurSel) 
{
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	char tem_str[100];
	m_xPartList.GetItemText(nCurSel,0,tem_str,100);
	long hCurPart=0;
	sscanf(tem_str,"%X",&hCurPart);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(hCurPart);
	pLDSView->UpdatePropertyPage();
	
}
void CBlockPartListDlg::OnClickListPart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(!m_pBlock->IsEmbeded())
		return;
	int iCurSel = m_xPartList.GetSelectedItem();
	if(iCurSel>=0)
		SelectPart(iCurSel);
	*pResult = 0;
}

void CBlockPartListDlg::DeleteSelectItems()
{
	if(!m_pBlock->IsEmbeded())
		return;
	POSITION pos = m_xPartList.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel=m_xPartList.GetNextSelectedItem(pos);
		if(iCurSel>=0)
		{
			CLDSPart* pPart=(CLDSPart*)m_xPartList.GetItemData(iCurSel);
			m_pBlock->Parts.DeleteNode(pPart->handle);
			g_pSolidDraw->DelEnt(pPart->handle);//删除实体图中的构件
			m_xPartList.DeleteItem(iCurSel);
		}
		pos = m_xPartList.GetFirstSelectedItemPosition();
	}
	g_pSolidDraw->Draw();
}
void CBlockPartListDlg::OnKeydownListPart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(!m_pBlock->IsEmbeded())
		return;
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_xPartList.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_xPartList.GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_UP)
		{	
			if(iCurSel>=1)
				iCurSel--;	
			if(iCurSel>=0)
				SelectPart(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_DOWN)
		{	
			if(iCurSel<m_xPartList.GetItemCount())
				iCurSel++;
			if(iCurSel>=0)
				SelectPart(iCurSel);
		}
		//为保证部件内构件的完整性，不允许随意删除部件中的构件
		/*
		else if(pLVKeyDow->wVKey==VK_DELETE)
			DeleteSelectItems();*/
	}
	*pResult = 0;
}
void CBlockPartListDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect rectItem;
	m_xPartList.GetWindowRect(rectItem);
	if(!rectItem.PtInRect(point))
		return;
	/*
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	pMenu->AppendMenu(MF_STRING,ID_DELETE_SELECT_PART,"删除选中构件");
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);*/
}
void CBlockPartListDlg::OnNMRclickListPart(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iCurSel = m_xPartList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); 
	if(iCurSel<0)
		return;
	CPoint point;
	GetCursorPos(&point);
	CRect rect;
	m_xPartList.GetWindowRect(&rect);
	if(rect.PtInRect(point))
		OnContextMenu(this,point);
	*pResult = 0;
}