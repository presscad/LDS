// ReusePartsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "ReusePartsDlg.h"
#include "afxdialogex.h"
#include "CfgPartNoDlg.h"
#include "env_def.h"
#include "LDSView.h"
#include "MainFrm.h"
//
int CALLBACK CompareItemsProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	return 0;
}
CXhChar16 REUSE_PART::GetReuseTypeName()
{
	CXhChar16 sName;
#ifdef AFX_TARG_ENU_ENGLISH
	if(m_cReuseType==REUSE_PART::REUSE_TYPE_COPY)
		sName.Copy("Copy");
	else if(m_cReuseType==REUSE_PART::REUSE_TYPE_SHARE)
		sName.Copy("Share");
	else if (m_cReuseType==REUSE_PART::REUSE_TYPE_SHADOW)
		sName.Copy("Mapping");
#else
	if(m_cReuseType==REUSE_PART::REUSE_TYPE_COPY)
		sName.Copy("复制");
	else if(m_cReuseType==REUSE_PART::REUSE_TYPE_SHARE)
		sName.Copy("共用");
	else if (m_cReuseType==REUSE_PART::REUSE_TYPE_SHADOW)
		sName.Copy("影射");
#endif
	return sName;
}
CXhChar50 REUSE_PART::GetCfgWordStr(CFGWORD cfgword,bool bLegObj)
{
	CXhChar50 sCfgWord;
	if(bLegObj)	//接腿构件
		cfgword.GetLegScopeStr(sCfgWord,50,true);
	else
		cfgword.GetBodyScopeStr(sCfgWord);
	return sCfgWord;
}
// CReusePartsDlg 对话框

IMPLEMENT_DYNAMIC(CReusePartsDlg, CDialog)

CReusePartsDlg::CReusePartsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReusePartsDlg::IDD, pParent)
{
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrl.AddColumnHeader("Handle",65);
	m_listCtrl.AddColumnHeader("Type",75);
	m_listCtrl.AddColumnHeader("Configure Word",115);
	m_listCtrl.AddColumnHeader("Reuse Type",90);
	m_listCtrl.AddColumnHeader("Destination Configure Word",200);
	m_listCtrl.AddColumnHeader("Z Position",90);
#else
	m_listCtrl.AddColumnHeader("句柄",61);
	m_listCtrl.AddColumnHeader("类型",71);
	m_listCtrl.AddColumnHeader("配材号",82);
	m_listCtrl.AddColumnHeader("复用类型",82);
	m_listCtrl.AddColumnHeader("目标配材号",82);
	m_listCtrl.AddColumnHeader("Z坐标",80);
#endif
}
BEGIN_MESSAGE_MAP(CReusePartsDlg,CDialog)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_REUSE_PARTS, &CReusePartsDlg::OnRclickListReuseParts)
	ON_COMMAND(ID_SET_TO_SHARE_TYPE,OnSetToShareType)
	ON_COMMAND(ID_SET_TO_COPY_TYPE,OnSetToCopyType)
	ON_COMMAND(ID_SET_TO_SHADOW_TYPE,OnSetToShadowType)
	ON_COMMAND(ID_SET_CFGWORD,OnSetCfgword)
	ON_NOTIFY(NM_CLICK, IDC_LIST_REUSE_PARTS, &CReusePartsDlg::OnNMClickListReuseParts)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_REUSE_PARTS, &CReusePartsDlg::OnLvnKeydownListReuseParts)
END_MESSAGE_MAP()

CReusePartsDlg::~CReusePartsDlg()
{
}

void CReusePartsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_REUSE_PARTS,m_listCtrl);
}
 BOOL CReusePartsDlg::OnInitDialog()
 {
     CDialog::OnInitDialog();
	 m_listCtrl.InitListCtrl();
	 m_listCtrl.m_bEnableSortItems=TRUE;
	 m_listCtrl.SetExtendedStyle(m_listCtrl.GetExtendedStyle()&~LVS_EX_HEADERDRAGDROP);
	 m_listCtrl.SetColumnSortType(0,SORTTYPE_HEX_NUM);
	 m_listCtrl.SetColumnSortType(1,SORTTYPE_STRING);
	 m_listCtrl.SetColumnSortType(5,SORTTYPE_INTEGER);
	 UpdateReusePartList();
	 return TRUE;
 }
BOOL CReusePartsDlg::UpdateReusePartList()
{
	m_listCtrl.DeleteAllItems();
	CStringArray str_arr;
	str_arr.SetSize(6);
	for(REUSE_PART *pReusePart=m_reusePartList.GetFirst();
		pReusePart;pReusePart=m_reusePartList.GetNext())
	{
		str_arr[0].Format("0x%X",pReusePart->m_pPart->handle);
		str_arr[1]=pReusePart->m_pPart->GetPartTypeName();
		str_arr[2]=REUSE_PART::GetCfgWordStr(pReusePart->m_pPart->cfgword,pReusePart->m_pPart->IsLegObj());
		str_arr[3]=pReusePart->GetReuseTypeName();
		str_arr[4]=REUSE_PART::GetCfgWordStr(pReusePart->m_destCfgword,pReusePart->m_pPart->IsLegObj());
		str_arr[5].Format("%.0f",pReusePart->m_pPart->ucs.origin.z);
		int iItem=m_listCtrl.InsertItemRecord(-1,str_arr);
		m_listCtrl.SetItemData(iItem,(DWORD)pReusePart);
	}
	return TRUE;
}
void CReusePartsDlg::OnRclickListReuseParts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(m_listCtrl.GetSelectedCount()==0)
		return;
	LVHITTESTINFO lvHitTestInfo;
	lvHitTestInfo.pt = pNMLV->ptAction;
	m_listCtrl.SubItemHitTest(&lvHitTestInfo);
	if(lvHitTestInfo.iSubItem<0)
		return;
	CPoint scr_point = pNMLV->ptAction;
	m_listCtrl.ClientToScreen(&scr_point);
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	while(pMenu->GetMenuItemCount()>0)
		pMenu->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
	if(lvHitTestInfo.iSubItem==3)
	{
		pMenu->AppendMenu(MF_STRING,ID_SET_TO_SHARE_TYPE,"Share");
		pMenu->AppendMenu(MF_STRING, ID_SET_TO_SHADOW_TYPE,"Mapping");
		pMenu->AppendMenu(MF_STRING,ID_SET_TO_COPY_TYPE,"Copy");
	}
	if(lvHitTestInfo.iSubItem==4)
		pMenu->AppendMenu(MF_STRING,ID_SET_CFGWORD,"Set Destination Configure Word");
#else
	if(lvHitTestInfo.iSubItem==3)
	{
		pMenu->AppendMenu(MF_STRING,ID_SET_TO_SHARE_TYPE,"共用");
		pMenu->AppendMenu(MF_STRING, ID_SET_TO_SHADOW_TYPE,"影射");
		pMenu->AppendMenu(MF_STRING,ID_SET_TO_COPY_TYPE,"复制");
	}
	if(lvHitTestInfo.iSubItem==4)
		pMenu->AppendMenu(MF_STRING,ID_SET_CFGWORD,"设置目标配材号");
#endif
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
	*pResult = 0;
}
void CReusePartsDlg::SetSelectedItemsReuseType(BYTE cType)
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	while(pos)
	{
		int iItem=m_listCtrl.GetNextSelectedItem(pos);
		REUSE_PART *pReusePart=(REUSE_PART*)m_listCtrl.GetItemData(iItem);
		pReusePart->m_cReuseType=cType;
		m_listCtrl.SetItemText(iItem,3,pReusePart->GetReuseTypeName());
	}
}
void CReusePartsDlg::OnSetToShareType()
{
	SetSelectedItemsReuseType(REUSE_PART::REUSE_TYPE_SHARE);
}
void CReusePartsDlg::OnSetToCopyType()
{
	SetSelectedItemsReuseType(REUSE_PART::REUSE_TYPE_COPY);
}
void CReusePartsDlg::OnSetToShadowType()
{
	SetSelectedItemsReuseType(REUSE_PART::REUSE_TYPE_SHADOW);
}
void CReusePartsDlg::OnSetCfgword()
{
	if(m_listCtrl.GetSelectedCount()<0)
		return;
	CCfgPartNoDlg cfgdlg;
	int iItem=m_listCtrl.GetSelectedItem();
	REUSE_PART *pFirstReusePart=(REUSE_PART*)m_listCtrl.GetItemData(iItem);
	if(pFirstReusePart&&pFirstReusePart->m_pPart&&pFirstReusePart->m_pPart->IsLegObj())
		cfgdlg.cfg_style=CFG_LEG_NO;
	else
		cfgdlg.cfg_style=CFG_BODY_NO;
	if(pFirstReusePart&&pFirstReusePart->m_pPart)
		cfgdlg.cfgword=pFirstReusePart->m_destCfgword;
	if(cfgdlg.DoModal()!=IDOK)
		return;
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iItem=m_listCtrl.GetNextSelectedItem(pos);
		REUSE_PART *pReusePart=(REUSE_PART*)m_listCtrl.GetItemData(iItem);
		pReusePart->m_destCfgword= cfgdlg.cfgword;
		m_listCtrl.SetItemText(iItem,4,REUSE_PART::GetCfgWordStr(pReusePart->m_destCfgword,pReusePart->m_pPart->IsLegObj()));
	}
}

void CReusePartsDlg::OnNMClickListReuseParts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	g_pSolidDraw->ReleaseSnapStatus();
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iItem=m_listCtrl.GetNextSelectedItem(pos);
		REUSE_PART *pReusePart=(REUSE_PART*)m_listCtrl.GetItemData(iItem);
		if(pReusePart&&pReusePart->m_pPart)
			g_pSolidDraw->SetEntSnapStatus(pReusePart->m_pPart->handle);
	}
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(pLDSView)
		pLDSView->UpdatePropertyPage(NULL,FALSE,m_hWnd);
	*pResult = 0;
}


void CReusePartsDlg::OnLvnKeydownListReuseParts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if(pLVKeyDow->wVKey==VK_DELETE)
	{
		if(m_listCtrl.GetSelectedCount()<=0)
			return;
#ifdef AFX_TARG_ENU_ENGLISH
		if(IDOK==AfxMessageBox("Are you sure to delete the selected items? ", MB_YESNO))
#else
		if(IDOK==AfxMessageBox("确定要删除选中项吗？",MB_YESNO))
#endif
			return;
		POSITION pos=m_listCtrl.GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iCurSel = m_listCtrl.GetNextSelectedItem(pos);
			REUSE_PART *pReusePart=(REUSE_PART*)m_listCtrl.GetItemData(iCurSel);
			if(pReusePart)
			{
				for(REUSE_PART *pPart=m_reusePartList.GetFirst();pPart;pPart=m_reusePartList.GetNext())
				{
					if(pPart==pReusePart)
					{
						m_reusePartList.DeleteCursor();
						break;
					}
				}
			}
			m_listCtrl.DeleteItem(iCurSel);
			pos=m_listCtrl.GetFirstSelectedItemPosition();
		}
	}
	*pResult = 0;
}
