// WorkStatusListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "WorkStatusListDlg.h"
#include "GlobalFunc.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkStatusListDlg dialog

#if defined(__TSA_)||defined(__LDS_)
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CWorkStatusListDlg *pWorkStatusListDlg=(CWorkStatusListDlg*)pListCtrl->GetParent();
	if(pWorkStatusListDlg==NULL||pSelItem==NULL)
		return FALSE;
	CRect rectItem;
	pListCtrl->GetItemRect(pSelItem->GetIndex(),rectItem,LVIR_BOUNDS);
	if(!rectItem.PtInRect(point))
		return FALSE;	//鼠标不在当前选中项上
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"新建工况");
	pMenu->AppendMenu(MF_STRING,ID_PASTE_ITEM,"粘贴工况");
	//pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除工况");
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pWorkStatusListDlg);
	return TRUE;
}
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	return TRUE;
}
CWorkStatusListDlg::CWorkStatusListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkStatusListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWorkStatusListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_listCtrl.AddColumnHeader("工况名称");
	m_listCtrl.AddColumnHeader("风速m/s");
	m_listCtrl.AddColumnHeader("风向角°");
	m_listCtrl.AddColumnHeader("覆冰厚度mm");
}


void CWorkStatusListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkStatusListDlg)
	DDX_Control(pDX, IDC_LIST_WORK_STATUS, m_listCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorkStatusListDlg, CDialog)
	//{{AFX_MSG_MAP(CWorkStatusListDlg)
	ON_COMMAND(ID_NEW_ITEM, OnNewItem)
	ON_COMMAND(ID_PASTE_ITEM, OnPasteItem)
	ON_COMMAND(ID_DELETE_ITEM, OnDeleteItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkStatusListDlg message handlers

BOOL CWorkStatusListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	long col_wide_arr[4]={300,70,70,80};
	m_listCtrl.InitListCtrl(col_wide_arr);
	m_listCtrl.SetModifyValueFunc(FireValueModify);
	m_listCtrl.SetContextMenuFunc(FireContextMenu);
	//初始化工况列表
	CString sTextValue=_T("");
	CWorkStatus *pWorkStatus=NULL;
	for(pWorkStatus=Ta.WorkStatus.GetFirst();pWorkStatus;pWorkStatus=Ta.WorkStatus.GetNext())
	{
		CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
		sTextValue.Format("%s",pWorkStatus->description);
		lpInfo->AddSubItemText(sTextValue);
		sTextValue.Format("%f",pWorkStatus->WindLoad.velocity);
		SimplifiedNumString(sTextValue);
		lpInfo->AddSubItemText(sTextValue);
		sTextValue.Format("%d",pWorkStatus->WindLoad.azimuth);
		lpInfo->AddSubItemText(sTextValue);
		sTextValue.Format("%f",pWorkStatus->ice_thick);
		SimplifiedNumString(sTextValue);
		lpInfo->AddSubItemText(sTextValue);
		m_listCtrl.InsertRootItem(lpInfo);
	}
	return TRUE;
}

void CWorkStatusListDlg::OnNewItem()
{
	CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
	CString sName;
	sName.Format("%d",m_listCtrl.GetItemCount()+1);
	lpInfo->AddSubItemText(sName);
	lpInfo->AddSubItemText(_T(""));
	lpInfo->AddSubItemText(_T(""));
	lpInfo->AddSubItemText(_T(""));
	CSuperGridCtrl::CTreeItem * pRoot = m_listCtrl.InsertRootItem(lpInfo);
	if( pRoot == NULL )
		return;	
	m_listCtrl.SetItemState(pRoot->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
}

void CWorkStatusListDlg::OnPasteItem()
{
	m_listCtrl.PasteDataFromClipboard(4);
}

void CWorkStatusListDlg::OnDeleteItem()
{
	int iCurSel=m_listCtrl.GetSelectedItem();
	CSuperGridCtrl::CTreeItem *pCurItem=m_listCtrl.GetTreeItem(iCurSel);
	if(pCurItem)
		m_listCtrl.DeleteItemEx(pCurItem,iCurSel);
}
void CWorkStatusListDlg::OnOK()
{
	int i=0;
	for(i=0;i<m_listCtrl.GetItemCount();i++)
	{
		CWorkStatus *pWorkStatus=NULL;
		if(i==0)
			pWorkStatus=Ta.WorkStatus.GetFirst();
		else
			pWorkStatus=Ta.WorkStatus.GetNext();
		if(pWorkStatus==NULL)
		{
			pWorkStatus=Ta.WorkStatus.append();
			CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_GROUP);
			pWorkStatus->iNo=pNoGroup->EnumIdleNo();
			pNoGroup->SetNoState(pWorkStatus->iNo);
		}
		CString sTextValue=_T("");
		sTextValue=m_listCtrl.GetItemText(i,0);
		sprintf(pWorkStatus->description,"%s",sTextValue);
		sTextValue=m_listCtrl.GetItemText(i,1);
		pWorkStatus->WindLoad.velocity=atof(sTextValue.GetBuffer(1));
		sTextValue=m_listCtrl.GetItemText(i,2);
		pWorkStatus->WindLoad.azimuth=atoi(sTextValue.GetBuffer(1));
		sTextValue=m_listCtrl.GetItemText(i,3);
		pWorkStatus->ice_thick=atof(sTextValue.GetBuffer(1));
	}
	CDialog::OnOK();
}
#endif