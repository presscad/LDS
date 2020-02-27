//<LOCALE_TRANSLATE Confirm by hxr/>
// UserDefCmdGroupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "UserDefCmdGroupDlg.h"
#include ".\userdefcmdgroupdlg.h"
#include "DefScopeStruDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CUserDefCmdGroupDlg 对话框
IMPLEMENT_DYNAMIC(CUserDefCmdGroupDlg, CDialog)

CUserDefCmdGroupDlg::CUserDefCmdGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserDefCmdGroupDlg::IDD, pParent)
	, m_nMaxWarnings(0)
{
	//{{AFX_DATA_INIT(CUserDefCmdGroupDlg)
	//}}AFX_DATA_INIT
	m_pCmdListCtrl = NULL;
	m_pCmdGroup = NULL;
	m_pCmdItems = NULL;
	m_bOutputToNotepad = FALSE;
	m_sSegStr = "*";
	m_scope.SetCubeScope(10000);
	m_bSelectScope=FALSE;
}

CUserDefCmdGroupDlg::~CUserDefCmdGroupDlg()
{
}

void CUserDefCmdGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserDefCmdGroupDlg)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_E_MAX_WARNINGS, m_nMaxWarnings);
	DDX_Control(pDX, IDC_LIST_CMD_PARA, m_propList);
	DDX_Check(pDX, IDC_CHK_OUTPUT_TO_NOTEPAD, m_bOutputToNotepad);
}

BEGIN_MESSAGE_MAP(CUserDefCmdGroupDlg, CDialog)
	//{{AFX_MSG_MAP(CUserDefCmdGroupDlg)
	ON_BN_CLICKED(IDC_BNT_MOVE_TO_BOTTOM, OnBnMoveToBottom)
	ON_BN_CLICKED(IDC_BNT_MOVE_DOWN, OnBnMoveDown)
	ON_BN_CLICKED(IDC_BNT_MOVE_TO_TOP, OnBnMoveToTop)
	ON_BN_CLICKED(IDC_BNT_MOVE_UP, OnBnMoveUp)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CMD_ITEM, OnDblclkPartnoList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CMD_ITEM, OnRclickPartnoList)
	ON_COMMAND(ID_CHECK_ITEMS,OnCheckItems)
	ON_COMMAND(ID_UNCHECK_ITEMS,OnUncheckItems)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CMD_ITEM, OnLvnItemchangedListCmdItem)
END_MESSAGE_MAP()


// CUserDefCmdGroupDlg 消息处理程序
//属性项
CStringKeyHashTable<long> CUserDefCmdGroupDlg::propHashtable;
void CUserDefCmdGroupDlg::InitCmdGroupPropHashtable()
{
	int id=-1;	//避免与使用地址当做id的属性项发生冲突,此处id均取负数
	propHashtable.CreateHashTable(50);
	propHashtable.SetValueAt("cmdGroup",id--);
	propHashtable.SetValueAt("segStr",id--);
	propHashtable.SetValueAt("scope",id--);
	propHashtable.SetValueAt("scope.x",id--);
	propHashtable.SetValueAt("scope.y",id--);
	propHashtable.SetValueAt("scope.z",id--);
}
long CUserDefCmdGroupDlg::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
	{
		char prop_str[500];
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(prop_str,"Fail to find the name of\"");
		strcat(prop_str,propStr);
		strcat(prop_str,"\"property ID！");
#else
		strcpy(prop_str,"没找到名为\"");
		strcat(prop_str,propStr);
		strcat(prop_str,"\"的属性ID！");
#endif
		AfxMessageBox(prop_str);
		return 0;
	}
}
BOOL CUserDefCmdGroupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_pCmdListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_CMD_ITEM);
	if(m_pCmdListCtrl==NULL||m_pCmdGroup==NULL||m_pCmdItems==NULL)
		return FALSE;
	
	m_pCmdListCtrl->SetExtendedStyle(LVS_EX_CHECKBOXES);
	for(bool* pbActive=m_pCmdGroup->cmdItems.GetFirst();pbActive;pbActive=m_pCmdGroup->cmdItems.GetNext())
	{
		CMDCFG_ITEM *pCmdItem=m_pCmdItems->GetValue(m_pCmdGroup->cmdItems.GetCursorKey());
		if(pCmdItem==NULL || pCmdItem->group!=m_pCmdGroup->serial)
			continue;
		AppendRecord(m_pCmdListCtrl,CString(pCmdItem->name),(DWORD)pCmdItem,pCmdItem->bActive);
	}
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
#ifdef AFX_TARG_ENU_ENGLISH
	m_propList.SetDividerScale(0.85);//调整分割比例
#else 
	m_propList.SetDividerScale(0.45);//调整分割比例
#endif
	InitCmdGroupPropHashtable();
	UpdateCmdProperties(NULL);
	return TRUE;  
}

void CUserDefCmdGroupDlg::OnOK()
{	//更新命令组中命令顺序及激活状态
	UpdateData();
	m_pCmdGroup->nMaxWarnings=m_nMaxWarnings;
	m_pCmdGroup->bOutputToNotepad=m_bOutputToNotepad;
	int cmdGroupIndex = m_pCmdGroup->serial-1;
	for(int i=0;i<m_pCmdListCtrl->GetItemCount();i++)
	{
		CMDCFG_ITEM *pCmdItem=(CMDCFG_ITEM*)m_pCmdListCtrl->GetItemData(i);
		pCmdItem->bActive=(m_pCmdListCtrl->GetCheck(i)!=FALSE);
		m_pCmdGroup->cmdItems.SetValue(pCmdItem->serial,pCmdItem->bActive!=FALSE);
		if(cmdGroupIndex>=0&&cmdGroupIndex<4)
		{
			g_sysPara.cmdGroupParaArr[cmdGroupIndex].orders[i]=pCmdItem->serial;
			g_sysPara.cmdGroupParaArr[cmdGroupIndex].actives[i]=pCmdItem->bActive;
		}
	}
	if(cmdGroupIndex>=0&&cmdGroupIndex<4)
	{
		g_sysPara.cmdGroupParaArr[cmdGroupIndex].nCmdCount=m_pCmdGroup->cmdItems.GetNodeNum();
		g_sysPara.cmdGroupParaArr[cmdGroupIndex].nMaxWarnings=m_pCmdGroup->nMaxWarnings;
		g_sysPara.cmdGroupParaArr[cmdGroupIndex].bOutputToNotepad=m_pCmdGroup->bOutputToNotepad;
	}
	CDialog::OnOK();
}


void CUserDefCmdGroupDlg::OnBnMoveToBottom() 
{
	int iCurSel=GetSelectItemIndex();
	if(iCurSel<0)
		return;
	CMDCFG_ITEM *pCmdItem = (CMDCFG_ITEM*)m_pCmdListCtrl->GetItemData(iCurSel);
	BOOL bCheck = m_pCmdListCtrl->GetCheck(iCurSel);
	m_pCmdListCtrl->DeleteItem(iCurSel);
	AppendRecord(m_pCmdListCtrl,CString(pCmdItem->name),(DWORD)pCmdItem,bCheck);
	//选中插入项
	SelectItem(m_pCmdListCtrl->GetItemCount()-1);
}

void CUserDefCmdGroupDlg::OnBnMoveDown() 
{
	int iCurSel=GetSelectItemIndex();
	if(iCurSel<0||iCurSel>=m_pCmdListCtrl->GetItemCount()-1)
		return;
	CMDCFG_ITEM *pCmdItem = (CMDCFG_ITEM*)m_pCmdListCtrl->GetItemData(iCurSel+1);
	BOOL bCheck = m_pCmdListCtrl->GetCheck(iCurSel+1);
	m_pCmdListCtrl->DeleteItem(iCurSel+1);
	int iItem=InsertRecord(m_pCmdListCtrl,CString(pCmdItem->name),(DWORD)pCmdItem,iCurSel,bCheck);
	//选中插入项
	SelectItem(iItem+1);
}

void CUserDefCmdGroupDlg::OnBnMoveToTop() 
{
	int iCurSel=GetSelectItemIndex();
	if(iCurSel<=0)
		return;
	CMDCFG_ITEM *pCmdItem = (CMDCFG_ITEM*)m_pCmdListCtrl->GetItemData(iCurSel);
	BOOL bCheck = m_pCmdListCtrl->GetCheck(iCurSel);
	m_pCmdListCtrl->DeleteItem(iCurSel);
	InsertRecord(m_pCmdListCtrl,CString(pCmdItem->name),(DWORD)pCmdItem,-1,bCheck);
	//选中插入项
	SelectItem(0);
}

void CUserDefCmdGroupDlg::OnBnMoveUp() 
{
	int iCurSel=GetSelectItemIndex();
	if(iCurSel<=0)
		return;
	CMDCFG_ITEM *pCmdItem = (CMDCFG_ITEM*)m_pCmdListCtrl->GetItemData(iCurSel-1);
	BOOL bCheck = m_pCmdListCtrl->GetCheck(iCurSel-1);
	m_pCmdListCtrl->DeleteItem(iCurSel-1);
	int iItem=InsertRecord(m_pCmdListCtrl,CString(pCmdItem->name),(DWORD)pCmdItem,iCurSel,bCheck);
	//选中插入项
	SelectItem(iItem-1);
}

int CUserDefCmdGroupDlg::GetSelectItemIndex()
{
	int iCurSel=-1;
	POSITION pos = m_pCmdListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
		iCurSel = m_pCmdListCtrl->GetNextSelectedItem(pos);
	return iCurSel;
}

//选中插入项
void CUserDefCmdGroupDlg::SelectItem(int iItem)
{
	//取消所有选中项
	m_pCmdListCtrl->SetItemState(-1,0,LVIS_SELECTED);
	//选中指定项
	UINT uFlag=LVIS_SELECTED | LVIS_FOCUSED;
	m_pCmdListCtrl->SetItemState(iItem,uFlag,uFlag);
}
int CUserDefCmdGroupDlg::AppendRecord(CListCtrl *pListCtrl,CString sItem,
								 DWORD iParam,BOOL bCheck)
{
	LV_ITEM lvi;
	int iItem=pListCtrl->GetItemCount();
	
	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = sItem.GetBuffer(1);//_gszItem[i][0];
	lvi.lParam = iParam;
	iItem = pListCtrl->InsertItem(&lvi);
	pListCtrl->SetCheck(iItem,bCheck);
	pListCtrl->RedrawItems(iItem,iItem);
	return iItem;
}

int CUserDefCmdGroupDlg::InsertRecord(CListCtrl *pListCtrl, CString sItem, 
								 DWORD iParam,int iInsertPos,BOOL bCheck)
{
	LV_ITEM lvi;
	int nRows = pListCtrl->GetItemCount();
	if(iInsertPos<0)
		iInsertPos = 0;
	else if(iInsertPos>=nRows)
		iInsertPos = nRows;
	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = iInsertPos;
	lvi.iSubItem = 0;
	lvi.pszText = sItem.GetBuffer(1);//_gszItem[i][0];
	lvi.lParam = iParam;
	iInsertPos = pListCtrl->InsertItem(&lvi);
	pListCtrl->SetCheck(iInsertPos,bCheck);
	pListCtrl->RedrawItems(iInsertPos,iInsertPos);
	return iInsertPos;
}

void CUserDefCmdGroupDlg::OnRclickPartnoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_pCmdListCtrl->GetSelectedCount()<=0)
		return;
	CPoint point;
	GetCursorPos(&point);
	CRect rect;
	m_pCmdListCtrl->GetWindowRect(&rect);
	if(rect.PtInRect(point))
		ContextMenu(this,point);
	*pResult = 0;
}

void CUserDefCmdGroupDlg::ContextMenu(CWnd *pWnd, CPoint point)
{
	CPoint scr_point = point;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
	pMenu->AppendMenu(MF_STRING,ID_CHECK_ITEMS,"selected");
	pMenu->AppendMenu(MF_STRING,ID_UNCHECK_ITEMS,"cancel selected");
#else
	pMenu->AppendMenu(MF_STRING,ID_CHECK_ITEMS,"选中");
	pMenu->AppendMenu(MF_STRING,ID_UNCHECK_ITEMS,"取消选中");
#endif
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}

void CUserDefCmdGroupDlg::OnDblclkPartnoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iCurSel=GetSelectItemIndex();
	if(iCurSel<0)
		return;
	BOOL bCheck=m_pCmdListCtrl->GetCheck(iCurSel);
	m_pCmdListCtrl->SetCheck(iCurSel,!bCheck);
	*pResult = 0;
}

void CUserDefCmdGroupDlg::SetCheckState(BOOL bCheck)
{
	POSITION pos = m_pCmdListCtrl->GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_pCmdListCtrl->GetNextSelectedItem(pos);
		m_pCmdListCtrl->SetCheck(iCurSel,bCheck);
	}
}

void CUserDefCmdGroupDlg::OnCheckItems()
{
	SetCheckState(TRUE);
}

void CUserDefCmdGroupDlg::OnUncheckItems()
{
	SetCheckState(FALSE);
}
void CUserDefCmdGroupDlg::OnLvnItemchangedListCmdItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(pNMLV->uNewState&LVIS_SELECTED)
	{
		m_pCmdListCtrl->SetFocus();	//添加此行用于更改右侧属性栏后直接点击左侧属性栏情况更新属性用 wjh-2015.6.25
		CMDCFG_ITEM *pCmdItem = (CMDCFG_ITEM*)m_pCmdListCtrl->GetItemData(pNMLV->iItem);
		UpdateCmdProperties(pCmdItem);
	}
	*pResult = 0;
}
CXhChar50 GetPropertyItemValueStr(PROPERTY_ITEM &prop)
{
	if(prop.cType=='b')
	{
		if(prop.val.bVal)
#ifdef AFX_TARG_ENU_ENGLISH
			return CXhChar50("Yes");
		else
			return CXhChar50("No");
#else
			return CXhChar50("是");
		else
			return CXhChar50("否");
#endif
	}
	else if(prop.cType=='w')
		return CXhChar50("%d",prop.val.siVal);
	else if(prop.cType=='i')
		return CXhChar50("%d",prop.val.iVal);
	else if(prop.cType=='d')
		return CXhChar50(prop.val.dblVal);
	else
		return "";
}
BOOL UpdateCollisonScopeProp(CPropertyList* pPropList,CPropTreeItem* pItem,CString &valueStr)
{
	CUserDefCmdGroupDlg *pCmdGroupDlg=(CUserDefCmdGroupDlg*)pPropList->GetParent();
#ifdef AFX_TARG_ENU_ENGLISH
	if(valueStr.Compare("Yes")==0)
#else
	if(valueStr.Compare("是")==0)
#endif
	{
		CDefScopeStruDlg scopeDlg;
		scopeDlg.m_fMinX=pCmdGroupDlg->m_scope.fMinX;
		scopeDlg.m_fMinY=pCmdGroupDlg->m_scope.fMinY;
		scopeDlg.m_fMinZ=pCmdGroupDlg->m_scope.fMinZ;
		scopeDlg.m_fMaxX=pCmdGroupDlg->m_scope.fMaxX;
		scopeDlg.m_fMaxY=pCmdGroupDlg->m_scope.fMaxY;
		scopeDlg.m_fMaxZ=pCmdGroupDlg->m_scope.fMaxZ;
		if(scopeDlg.DoModal()==IDOK)
		{
			pCmdGroupDlg->m_bSelectScope = TRUE;
			pCmdGroupDlg->m_scope.fMinX = scopeDlg.m_fMinX;
			pCmdGroupDlg->m_scope.fMinY = scopeDlg.m_fMinY;
			pCmdGroupDlg->m_scope.fMinZ = scopeDlg.m_fMinZ;
			pCmdGroupDlg->m_scope.fMaxX = scopeDlg.m_fMaxX;
			pCmdGroupDlg->m_scope.fMaxY = scopeDlg.m_fMaxY;
			pCmdGroupDlg->m_scope.fMaxZ = scopeDlg.m_fMaxZ;
			CString sScopeStr;
			CItemInfo* lpInfo=NULL;
			CPropTreeItem *pFindItem=pPropList->FindItemByPropId(CUserDefCmdGroupDlg::GetPropID("scope.x"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "X range";
#else
				lpInfo->m_strPropName = "X范围";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = CUserDefCmdGroupDlg::GetPropID("scope.x");
				pFindItem->SetReadOnly();
			}
			sScopeStr.Format("%0.f~%0.f",scopeDlg.m_fMinX,scopeDlg.m_fMaxX);
			pPropList->SetItemPropValue(CUserDefCmdGroupDlg::GetPropID("scope.x"),sScopeStr);

			pFindItem=pPropList->FindItemByPropId(CUserDefCmdGroupDlg::GetPropID("scope.y"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Y range"; 
#else
				lpInfo->m_strPropName = "Y范围"; 
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = CUserDefCmdGroupDlg::GetPropID("scope.y");
				pFindItem->SetReadOnly();
			}
			sScopeStr.Format("%0.f~%0.f",scopeDlg.m_fMinY,scopeDlg.m_fMaxY);
			pPropList->SetItemPropValue(CUserDefCmdGroupDlg::GetPropID("scope.y"),sScopeStr);

			pFindItem=pPropList->FindItemByPropId(CUserDefCmdGroupDlg::GetPropID("scope.z"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Z range"; 
#else
				lpInfo->m_strPropName = "Z范围"; 
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = CUserDefCmdGroupDlg::GetPropID("scope.z");
				pFindItem->SetReadOnly();
			}
			sScopeStr.Format("%0.f~%0.f",scopeDlg.m_fMinZ,scopeDlg.m_fMaxZ);
			pPropList->SetItemPropValue(CUserDefCmdGroupDlg::GetPropID("scope.z"),sScopeStr);
		}
		else
			return FALSE;
	}
	else
	{
		pCmdGroupDlg->m_bSelectScope = FALSE;
		pPropList->DeleteAllSonItems(pItem);
	}
	return TRUE;
}
BOOL ModifyCmdItemProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{	
	CUserDefCmdGroupDlg *pCmdGroupDlg=(CUserDefCmdGroupDlg*)pPropList->GetParent();
	if(pItem->m_idProp==CUserDefCmdGroupDlg::GetPropID("segStr"))
		pCmdGroupDlg->m_sSegStr=valueStr;
	else if(pItem->m_idProp==CUserDefCmdGroupDlg::GetPropID("scope"))
	{	//碰撞区域
		return UpdateCollisonScopeProp(pPropList,pItem,valueStr);
	}
	else 
	{
		PROPERTY_ITEM* pProperty=(PROPERTY_ITEM*)pItem->m_idProp;
		if(pProperty->cType=='b')
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.Compare("Yes")==0)
#else
			if(valueStr.Compare("是")==0)
#endif
				pProperty->val.bVal=true;
			else
				pProperty->val.bVal=false;
		}
		else if(pProperty->cType=='w')
			pProperty->val.siVal=(short)atoi(valueStr);
		else if(pProperty->cType=='i')
			pProperty->val.iVal=atoi(valueStr);
		else if(pProperty->cType=='d')
			pProperty->val.dblVal=atof(valueStr);
		else
			return FALSE;
	}
	return TRUE;
}

void CUserDefCmdGroupDlg::UpdateCmdProperties(CMDCFG_ITEM *pCmdItem)
{
	CPropTreeItem *pPropItem,*pGroupItem,*pRoot=m_propList.GetRootItem();
	CItemInfo* lpInfo;
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(ModifyCmdItemProperty);
	BOOL bNeedGroupItem=FALSE;
	//命令组属性
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "command group property";
#else
	lpInfo->m_strPropName = "命令组属性";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	if(m_pCmdGroup->serial==2 || m_pCmdGroup->serial==3)
	{	//段号范围
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "segment number range";
		lpInfo->m_strPropHelp = "part number check segment number range"; 
#else
		lpInfo->m_strPropName = "段号范围";
		lpInfo->m_strPropHelp = "构件号检查段号范围"; 
#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("segStr");
		pPropItem->m_lpNodeInfo->m_strPropValue = m_sSegStr;
		//
		bNeedGroupItem=TRUE;
	}
	if(m_pCmdGroup->serial==4)
	{	//碰撞区域
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "specified area";
		lpInfo->m_strPropHelp = "the scope of part collision check"; 
		lpInfo->m_cmbItems="Yes|No";
#else
		lpInfo->m_strPropName = "指定区域";
		lpInfo->m_strPropHelp = "构件碰撞检查的碰撞范围"; 
		lpInfo->m_cmbItems="是|否";
#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("scope");
		if(m_bSelectScope)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropValue = "Yes";
#else
			pPropItem->m_lpNodeInfo->m_strPropValue = "是";
#endif
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X range";
#else
			lpInfo->m_strPropName = "X范围";
#endif
			CPropTreeItem *pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CUserDefCmdGroupDlg::GetPropID("scope.x");
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%0.f~%0.f",m_scope.fMinX,m_scope.fMaxX);
			
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y range";
#else
			lpInfo->m_strPropName = "Y范围";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CUserDefCmdGroupDlg::GetPropID("scope.y");
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%0.f~%0.f",m_scope.fMinY,m_scope.fMaxY);
			
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z range";
#else
			lpInfo->m_strPropName = "Z范围";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CUserDefCmdGroupDlg::GetPropID("scope.z");
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%0.f~%0.f",m_scope.fMinZ,m_scope.fMaxZ);
		}
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropValue = "No";
#else
			pPropItem->m_lpNodeInfo->m_strPropValue = "否";
#endif

		bNeedGroupItem=TRUE;
	}
	if(pCmdItem)
	{
		pGroupItem=pRoot;
		//命令属性
		for(int i=0;i<4;i++)
		{
			if(!pCmdItem->properties[i].bActive)
				break;		//已无有效属性
			else if(pCmdItem->properties[i].name.GetLength()==0)
				continue;	//内置属性
			if(bNeedGroupItem)
			{	//命令属性
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "command property";
#else
				lpInfo->m_strPropName = "命令属性";
#endif
				pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
				bNeedGroupItem=FALSE;
			}
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = (char*)pCmdItem->properties[i].name;
			lpInfo->m_strPropHelp = ""; 
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = (long)&pCmdItem->properties[i];
			pPropItem->m_lpNodeInfo->m_strPropValue = (char*)GetPropertyItemValueStr(pCmdItem->properties[i]);
		}
	}
	m_propList.Redraw();
}