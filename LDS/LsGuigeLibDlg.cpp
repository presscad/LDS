// LsGuigeLibDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "LsGuigeLibDlg.h"
#include "afxdialogex.h"
#include "image.h"
#include "GlobalFunc.h"
#include "PropertyListOper.h"
#include "database.h"
#include "SysLsFamilyDlg.h"
//////////////////////////////////////////////////////////////////////////
//回调函数处理
static BOOL ModifyPropValue(CPropertyList *pPropList,CPropTreeItem *pItem,CString &valueStr)
{
	CLsGuigeLibDlg* pDlg=(CLsGuigeLibDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	DWORD dwData=0;
	if(pDlg->GetCurSelItmeType(&dwData)!=LS_GUIGE_ITEM)
		return FALSE;
	CLsFamily* pCurLsFamily=(CLsFamily*)dwData;
	CPropertyListOper<CLsGuigeLibDlg> oper(pPropList,pDlg);
	BOOL bUpdateTreeNode=FALSE;
	if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("name"))
	{
		strcpy(pCurLsFamily->name,valueStr);
		bUpdateTreeNode=TRUE;
	}
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("descr"))
		strcpy(pCurLsFamily->descr,valueStr);
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("FootNail"))
	{
		if(valueStr.Compare("是")==0)
			pCurLsFamily->m_bFootNail=TRUE;
		else
			pCurLsFamily->m_bFootNail=FALSE;
	}
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("Safety"))
	{	//0.普通|1.双帽|2.防盗|3.防松
		pCurLsFamily->m_bDualCap=pCurLsFamily->m_bBurglarproof=pCurLsFamily->m_bAntiLoose=false;
		BYTE ciSafeMeasure=valueStr[0]-'0';
		if(ciSafeMeasure==1)
			pCurLsFamily->m_bDualCap=true;
		else if(ciSafeMeasure==2)
			pCurLsFamily->m_bBurglarproof=true;
		else if(ciSafeMeasure==3)
			pCurLsFamily->m_bAntiLoose=true;
		//
		pPropList->DeleteAllSonItems(pItem);
		if(ciSafeMeasure==0)
		{
			oper.InsertCmbEditPropItem(pItem,"MappingAntiLooseFamily",pDlg->MakeAttachItemStr(1,pCurLsFamily->m_bFootNail),"","",-1,TRUE);
			oper.InsertCmbEditPropItem(pItem,"MappingAntiTheftFamily",pDlg->MakeAttachItemStr(0,pCurLsFamily->m_bFootNail),"","",-1,TRUE);
		}	
	}
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("MappingAntiLooseFamily"))
	{
		if(stricmp(valueStr,"无")==0)
			pCurLsFamily->m_hMappingAntiLooseFamily=0;
		else
			pCurLsFamily->m_hMappingAntiLooseFamily=valueStr[0]-'0';
	}
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("MappingAntiTheftFamily"))
	{
		if(stricmp(valueStr,"无")==0)
			pCurLsFamily->m_hMappingAntiTheftFamily=0;
		else
			pCurLsFamily->m_hMappingAntiTheftFamily=valueStr[0]-'0';
	}
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("comm_nut"))
		pCurLsFamily->SetLsSubPartNum(CLsSubPartFamily::TYPE_COMMON_NUT,atoi(valueStr));
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("fast_nut"))
		pCurLsFamily->SetLsSubPartNum(CLsSubPartFamily::TYPE_FASTEN_NUT,atoi(valueStr));
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("fd_nut"))
		pCurLsFamily->SetLsSubPartNum(CLsSubPartFamily::TYPE_ANTITHEFT_NUT,atoi(valueStr));
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("thin_nut"))
		pCurLsFamily->SetLsSubPartNum(CLsSubPartFamily::TYPE_THIN_NUT,atoi(valueStr));
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("comm_gasket"))
		pCurLsFamily->SetLsSubPartNum(CLsSubPartFamily::TYPE_COMMON_GASKET,atoi(valueStr));
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("spring_gasket"))
		pCurLsFamily->SetLsSubPartNum(CLsSubPartFamily::TYPE_SPRING_GASKET,atoi(valueStr));
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("lock_clasp"))
		pCurLsFamily->SetLsSubPartNum(CLsSubPartFamily::TYPE_LOCK_CLASP,atoi(valueStr));
	//
	if(bUpdateTreeNode)
	{
		HTREEITEM hSelItem=pDlg->m_xTreeCtrl.GetSelectedItem();
		if(hSelItem!=NULL)
			pDlg->m_xTreeCtrl.SetItemText(hSelItem,CXhChar50("%s(%d)",pCurLsFamily->name,pCurLsFamily->GetCount()));
	}
	return TRUE;
}
static BOOL ButtonClickProp(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	return FALSE;
}
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
	CLsGuigeLibDlg *pDlg=(CLsGuigeLibDlg*)pListCtrl->GetParent();
	if(sTextValue.GetLength()<=0)
		return FALSE;
	CString oldText = pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	if(oldText.CompareNoCase(sTextValue)==0)
		return FALSE;
	if(iSubItem==0)
	{	//螺栓直径必须在螺栓规格表范围内（当前规格是12—48）
		if (atoi(sTextValue)<12||atoi(sTextValue)>48)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox(NULL,"bolt's diameter is not in the specified range!","ERROR",MB_OK);
#else
			MessageBox(NULL,"螺栓直径不在规定范围内!","ERROR",MB_OK);
#endif
			return FALSE;
		}
	}
	pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	//
	int num = pListCtrl->GetItemCount();
	long nIndex=pSelItem->m_idProp-1;
	if(pSelItem->m_idProp==num && sTextValue!="")
	{
		CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
		lpRoot->AddSubItemText("");
		lpRoot->AddSubItemText("");
		lpRoot->AddSubItemText("");
		CSuperGridCtrl::CTreeItem* pItem=pListCtrl->InsertRootItem(lpRoot);
		pItem->m_idProp=num+1;	
	}
	return TRUE;
}
extern void DeleteAllSubItems(CTreeCtrl* pTreeCtrl,HTREEITEM hParentItem);
//////////////////////////////////////////////////////////////////////////
// CLsGuigeLibDlg 对话框
IMPLEMENT_DYNAMIC(CLsGuigeLibDlg, CRememberLocationDlg)

CLsGuigeLibDlg::CLsGuigeLibDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CLsGuigeLibDlg::IDD, pParent)
{
	m_xListCtrl.AddColumnHeader("级别");
	m_xListCtrl.AddColumnHeader("直径");
	m_xListCtrl.AddColumnHeader("有效长");
	m_xListCtrl.AddColumnHeader("无扣长");
	m_xListCtrl.AddColumnHeader("通厚下限");
	m_xListCtrl.AddColumnHeader("通厚上限");
	m_xListCtrl.AddColumnHeader("重量");
	//
	m_xLsComptListCtrl.AddColumnHeader("螺栓直径(mm)");
	m_xLsComptListCtrl.AddColumnHeader("零件厚度(mm)");
	m_xLsComptListCtrl.AddColumnHeader("零件宽度(mm)");
	m_xLsComptListCtrl.AddColumnHeader("零件重量(kg)");
	//
	m_xListAnchorBoltCtrl.AddColumnHeader("直径");
	m_xListAnchorBoltCtrl.AddColumnHeader("出露长");
	m_xListAnchorBoltCtrl.AddColumnHeader("无扣长");
	//m_xListAnchorBoltCtrl.AddColumnHeader("丝扣长(mm)");
	m_xListAnchorBoltCtrl.AddColumnHeader("螺母高");
	m_xListAnchorBoltCtrl.AddColumnHeader("螺母宽");//(螺母边到边距离)
	m_xListAnchorBoltCtrl.AddColumnHeader("座板孔径");
	m_xListAnchorBoltCtrl.AddColumnHeader("垫板孔径");
	m_xListAnchorBoltCtrl.AddColumnHeader("垫板厚");
	m_xListAnchorBoltCtrl.AddColumnHeader("垫板宽");
	//m_xListAnchorBoltCtrl.AddColumnHeader("座板间距(mm)");
	//
	m_pBoltLib=NULL;		//用于记录匹配好的螺栓系列库
	m_pSrcBoltLib=NULL;		//用于记录当前模型使用的螺栓系列库
}

CLsGuigeLibDlg::~CLsGuigeLibDlg()
{
}

void CLsGuigeLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CRememberLocationDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CTRL, m_xListCtrl);
	DDX_Control(pDX, IDC_LIST_ANCHOR_BOLT_CTRL, m_xListAnchorBoltCtrl);
	DDX_Control(pDX, IDC_TREE_CTRL, m_xTreeCtrl);
	DDX_Control(pDX, IDC_LIST_BOX,  m_xPropList);
	DDX_Control(pDX, IDC_LS_SUBPART_LIST_CTRL,m_xLsComptListCtrl);
}


BEGIN_MESSAGE_MAP(CLsGuigeLibDlg, CRememberLocationDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CTRL, &CLsGuigeLibDlg::OnSelchangedTreeCtrl)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_CTRL, &CLsGuigeLibDlg::OnKeydownTreeCtrl)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_CTRL, &CLsGuigeLibDlg::OnRclickTreeCtrl)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CLsGuigeLibDlg::OnBnSaveLsComponent)
	ON_COMMAND(ID_DELETE_ITEM, &CLsGuigeLibDlg::OnDeleteItem)
	ON_COMMAND(ID_IMPORT_XML_FILE, &CLsGuigeLibDlg::OnImportLsFamilyFile)
	ON_COMMAND(ID_EXPORT_XML_FILE, &CLsGuigeLibDlg::OnExportLsFamilyFile)
	ON_COMMAND(ID_IMPORT_LOAD_FILE, &CLsGuigeLibDlg::OnImportSysLsFamily)
END_MESSAGE_MAP()


// CLsGuigeLibDlg 消息处理程序
BOOL CLsGuigeLibDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitBoltLibrary();
	//初始化属性栏
	m_xPropList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xPropList.SetModifyValueFunc(ModifyPropValue);
	m_xPropList.SetButtonClickFunc(ButtonClickProp);
	InitPropHashtable();	//初始化属性哈希表
	//初始化树控件
	m_imageList.Create(IDB_IL_PROJECT, 16, 1, RGB(0,255,0));
	m_xTreeCtrl.SetImageList(&m_imageList,TVSIL_NORMAL);
	m_xTreeCtrl.ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS);
	//初始化列表控件
	long col_wide_arr1[7]={40,40,52,52,65,65,70};
	m_xListCtrl.InitListCtrl(col_wide_arr1);
	long col_wide_arr2[4]={100,100,100,100};
	m_xLsComptListCtrl.InitListCtrl(col_wide_arr2);
	m_xLsComptListCtrl.SetModifyValueFunc(FireValueModify);
	long col_wide_arr3[9]={40,60,60,60,60,80,80,60,60};
	m_xListAnchorBoltCtrl.InitListCtrl(col_wide_arr3);
	//
	RefreshTreeCtrl();
	MoveWndToLocation();
	return TRUE;
}
IMPLEMENT_PROP_FUNC(CLsGuigeLibDlg)
const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
void CLsGuigeLibDlg::InitPropHashtable()
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	//
	AddPropItem("General",PROPLIST_ITEM(id++,"螺栓系列基本信息"));
	AddPropItem("ID",PROPLIST_ITEM(id++,"系列号"));
	AddPropItem("name",PROPLIST_ITEM(id++,"名称"));
	AddPropItem("descr",PROPLIST_ITEM(id++,"描述"));
	AddPropItem("FootNail",PROPLIST_ITEM(id++,"脚钉功用","该螺栓系列是否作为脚钉使用","是|否"));
	AddPropItem("Safety",PROPLIST_ITEM(id++,"安全措施","该螺栓系列的安全措施","0.普通|1.双帽|2.防盗|3.防松"));
	AddPropItem("MappingAntiTheftFamily",PROPLIST_ITEM(id++,"配套防盗系列","配套的防盗系列"));
	AddPropItem("MappingAntiLooseFamily",PROPLIST_ITEM(id++,"配套放松系列","配套的放松系列"));
	AddPropItem("LsSubPart",PROPLIST_ITEM(id++,"螺栓零件信息"));
	AddPropItem("comm_nut",PROPLIST_ITEM(id++,"普通螺母数"));
	AddPropItem("fast_nut",PROPLIST_ITEM(id++,"扣紧螺母数"));
	AddPropItem("fd_nut",PROPLIST_ITEM(id++,"防盗螺母数"));
	AddPropItem("thin_nut",PROPLIST_ITEM(id++,"薄螺母数"));
	AddPropItem("comm_gasket",PROPLIST_ITEM(id++,"平垫圈数"));
	AddPropItem("spring_gasket",PROPLIST_ITEM(id++,"弹簧垫圈数"));
	AddPropItem("lock_clasp",PROPLIST_ITEM(id++,"防松扣数"));
}
int CLsGuigeLibDlg::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	DWORD dwData=0;
	if(GetCurSelItmeType(&dwData)!=LS_GUIGE_ITEM)
		return 0;
	CLsFamily* pCurLsFamily=(CLsFamily*)dwData;
	CXhChar100 sText;
	if(GetPropID("ID")==id)
		sText.Printf("%d",pCurLsFamily->GetFamilyHandle());
	else if(GetPropID("name")==id)
		sText.Copy(pCurLsFamily->name);
	else if(GetPropID("descr")==id)
		sText.Copy(pCurLsFamily->descr);
	else if(GetPropID("FootNail")==id)
	{
		if(pCurLsFamily->m_bFootNail)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("Safety")==id)
	{	
		if(pCurLsFamily->m_bBurglarproof)
			sText.Copy("2.防盗");
		else if(pCurLsFamily->m_bAntiLoose)
			sText.Copy("3.防松");
		else if(pCurLsFamily->m_bDualCap)
			sText.Copy("1.双帽");
		else
			sText.Copy("0.普通");
	}
	else if(GetPropID("MappingAntiTheftFamily")==id)
	{
		CLsFamily* pLsFamily=m_pBoltLib->FromHandle(pCurLsFamily->m_hMappingAntiTheftFamily);
		if(pLsFamily)
			sText.Printf("%d-%s",pLsFamily->GetFamilyHandle(),pLsFamily->name);
		else
			sText.Copy("无");
	}
	else if(GetPropID("MappingAntiLooseFamily")==id)
	{	
		CLsFamily* pLsFamily=m_pBoltLib->FromHandle(pCurLsFamily->m_hMappingAntiLooseFamily);
		if(pLsFamily)
			sText.Printf("%d-%s",pLsFamily->GetFamilyHandle(),pLsFamily->name);
		else
			sText.Copy("无");
	}
	else if(GetPropID("comm_nut")==id)
		sText.Printf("%d",pCurLsFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_COMMON_NUT));
	else if(GetPropID("fast_nut")==id)
		sText.Printf("%d",pCurLsFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_FASTEN_NUT));
	else if(GetPropID("fd_nut")==id)
		sText.Printf("%d",pCurLsFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_ANTITHEFT_NUT));
	else if(GetPropID("thin_nut")==id)
		sText.Printf("%d",pCurLsFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_THIN_NUT));
	else if(GetPropID("comm_gasket")==id)
		sText.Printf("%d",pCurLsFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_COMMON_GASKET));
	else if(GetPropID("spring_gasket")==id)
		sText.Printf("%d",pCurLsFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_SPRING_GASKET));
	else if(GetPropID("lock_clasp")==id)
		sText.Printf("%d",pCurLsFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_LOCK_CLASP));
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
CString CLsGuigeLibDlg::MakeAttachItemStr(BYTE theft0_loose1/*=0*/,bool bFootNail/*=false*/)
{
	CString sAttachItemValue("无");
	for(CLsFamily* pLsFamily=m_pBoltLib->GetFirstFamily();pLsFamily;pLsFamily=m_pBoltLib->GetNextFamily())
	{
		if(bFootNail!=pLsFamily->m_bFootNail)
			continue;
		CXhChar50 sName;
		if((pLsFamily->m_bAntiLoose && theft0_loose1==1)||(pLsFamily->m_bBurglarproof && theft0_loose1==0))
			sName.Printf("%d-%s",pLsFamily->GetFamilyHandle(),pLsFamily->name);
		if(sName.GetLength()>0)
		{
			sAttachItemValue+="|";
			sAttachItemValue+=sName;
		}
	}
	return sAttachItemValue;
}
void CLsGuigeLibDlg::DisplayProperty()
{
	DWORD dwData=0;
	if(GetCurSelItmeType(&dwData)!=LS_GUIGE_ITEM)
		return;
	CLsFamily* pCurLsFamily=(CLsFamily*)dwData;
	m_xPropList.CleanTree();
	CPropertyListOper<CLsGuigeLibDlg> oper(&m_xPropList,this);
	//基本信息
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pRootItem=m_xPropList.GetRootItem();
	pGroupItem=oper.InsertPropItem(pRootItem,"General");
	pGroupItem->m_bHideChildren=FALSE;
	pPropItem=oper.InsertEditPropItem(pGroupItem,"ID");
	pPropItem->SetReadOnly();
	oper.InsertEditPropItem(pGroupItem,"name");
	oper.InsertEditPropItem(pGroupItem,"descr");
	oper.InsertCmbListPropItem(pGroupItem,"FootNail");
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"Safety");
	if(pCurLsFamily&&!pCurLsFamily->m_bAntiLoose&&!pCurLsFamily->m_bBurglarproof&&!pCurLsFamily->m_bDualCap)
	{
		oper.InsertCmbListPropItem(pPropItem,"MappingAntiLooseFamily",MakeAttachItemStr(1,pCurLsFamily->m_bFootNail));
		oper.InsertCmbListPropItem(pPropItem,"MappingAntiTheftFamily",MakeAttachItemStr(0,pCurLsFamily->m_bFootNail));
	}	
	//零件信息
	pGroupItem=oper.InsertPropItem(pRootItem,"LsSubPart");
	pGroupItem->m_bHideChildren=FALSE;
	oper.InsertEditPropItem(pGroupItem,"comm_nut");
	oper.InsertEditPropItem(pGroupItem,"fast_nut");
	oper.InsertEditPropItem(pGroupItem,"fd_nut");
	oper.InsertEditPropItem(pGroupItem,"thin_nut");
	oper.InsertEditPropItem(pGroupItem,"comm_gasket");
	oper.InsertEditPropItem(pGroupItem,"spring_gasket");
	oper.InsertEditPropItem(pGroupItem,"lock_clasp");
	//
	m_xPropList.Redraw();
}
void CLsGuigeLibDlg::InitBoltLibrary()
{
	//获取当前模型使用的螺栓规格库
	m_pSrcBoltLib=IBoltLibrary::CreateBoltLibrary();
	m_pBoltLib=IBoltLibrary::CreateBoltLibrary();
	CLsFamily *pNewFamily=NULL,*pLsFamily=NULL;
	for(pLsFamily=CLsLibrary::GetFirstFamily();pLsFamily;pLsFamily=CLsLibrary::GetNextFamily())
	{
		pNewFamily=m_pSrcBoltLib->AppendFamily(pLsFamily->GetFamilyHandle());
		m_pSrcBoltLib->CopyLsFamily(pNewFamily,pLsFamily);
		//
		pNewFamily=m_pBoltLib->AppendFamily(pLsFamily->GetFamilyHandle());
		m_pBoltLib->CopyLsFamily(pNewFamily,pLsFamily);
	}
}
int CLsGuigeLibDlg::GetCurSelItmeType(DWORD* dwRefData/*=NULL*/)
{
	HTREEITEM hSelItem=m_xTreeCtrl.GetSelectedItem();
	if(hSelItem==NULL)
		return 0;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hSelItem);
	if(pItemInfo==0)
		return 0;
	if(dwRefData)
		*dwRefData=pItemInfo->dwRefData;
	return pItemInfo->itemType;
}
void CLsGuigeLibDlg::RelayoutWnd()
{
	GetDlgItem(IDC_WORK_PANEL)->ShowWindow(FALSE);
	GetDlgItem(IDC_LIST_BOX)->ShowWindow(FALSE);
	GetDlgItem(IDC_LIST_CTRL)->ShowWindow(FALSE);
	GetDlgItem(IDC_LIST_ANCHOR_BOLT_CTRL)->ShowWindow(FALSE);
	GetDlgItem(IDC_LS_SUBPART_LIST_CTRL)->ShowWindow(FALSE);
	GetDlgItem(IDC_BTN_SAVE)->ShowWindow(FALSE);
	GetDlgItem(IDC_E_PROP_HELP_STR)->ShowWindow(FALSE);
	//根据选中数节点更新界面显示
	CRect panel_rc,panel_rc2;
	GetDlgItem(IDC_WORK_PANEL)->GetWindowRect(panel_rc);
	ScreenToClient(panel_rc);
	GetDlgItem(IDC_WORK_PANEL2)->GetWindowRect(panel_rc2);
	ScreenToClient(panel_rc2);
	int iSelItemType=GetCurSelItmeType();
	if(iSelItemType==LS_COMPONENT_GROUP||iSelItemType==LS_COMPONENT_ITEM)
	{
		GetDlgItem(IDC_BTN_SAVE)->ShowWindow(TRUE);
		m_xLsComptListCtrl.ShowWindow(TRUE);
		m_xLsComptListCtrl.MoveWindow(panel_rc2);
		m_xListAnchorBoltCtrl.ShowWindow(FALSE);
	}
	else if(iSelItemType==LS_GUIGE_ITEM||iSelItemType==LS_GUIGE_GROUP)
	{
		GetDlgItem(IDC_E_PROP_HELP_STR)->ShowWindow(TRUE);
		m_xListCtrl.ShowWindow(TRUE);
		m_xPropList.ShowWindow(TRUE);
		m_xListAnchorBoltCtrl.ShowWindow(FALSE);
		m_xPropList.MoveWindow(panel_rc);
	}
	else if(iSelItemType==ANCHOR_BOLTS_GROUP)
	{
		GetDlgItem(IDC_E_PROP_HELP_STR)->ShowWindow(TRUE);
		m_xListCtrl.ShowWindow(FALSE);
		m_xPropList.ShowWindow(FALSE);
		m_xListAnchorBoltCtrl.ShowWindow(TRUE);
		m_xListAnchorBoltCtrl.MoveWindow(panel_rc2);
	}
}
void CLsGuigeLibDlg::RefreshTreeCtrl()
{
	itemInfoList.Empty();
	m_xTreeCtrl.DeleteAllItems();
	TREEITEM_INFO *pItemInfo=NULL;
	//
	HTREEITEM hGroupItem1=NULL,hGroupItem2=NULL,hGroupItem3=NULL;
	hGroupItem1=m_xTreeCtrl.InsertItem("螺栓零件库",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(LS_COMPONENT_GROUP,0));
	m_xTreeCtrl.SetItemData(hGroupItem1,(DWORD)pItemInfo);
	RefreshLsCompnTreeCtrl(hGroupItem1);
	//
	hGroupItem2=m_xTreeCtrl.InsertItem("螺栓规格库",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(LS_GUIGE_GROUP,(DWORD)m_pBoltLib));
	m_xTreeCtrl.SetItemData(hGroupItem2,(DWORD)pItemInfo);
	RefreshLsGuigeTreeCtrl(hGroupItem2);
	//
	hGroupItem3=m_xTreeCtrl.InsertItem("地脚螺栓库",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(ANCHOR_BOLTS_GROUP,0));
	m_xTreeCtrl.SetItemData(hGroupItem3,(DWORD)pItemInfo);
	RefreshAnchorBoltsTreeCtrl();
	//
	m_xTreeCtrl.Expand(hGroupItem1,TVE_EXPAND);
	m_xTreeCtrl.Expand(hGroupItem2,TVE_EXPAND);
	m_xTreeCtrl.SelectItem(hGroupItem1);
}
void CLsGuigeLibDlg::RefreshLsCompnTreeCtrl(HTREEITEM hGroup)
{
	DeleteAllSubItems(&m_xTreeCtrl,hGroup);
	TREEITEM_INFO *pItemInfo=NULL;
	HTREEITEM hTreeItem=NULL;
	for(CLsSubPartFamily* pLsSubPartFamily=CLsLibrary::GetFirstLsSubPartFamily();pLsSubPartFamily;pLsSubPartFamily=CLsLibrary::GetNextLsSubPartFamily())
	{
		CXhChar50 sName("%s(%d)",pLsSubPartFamily->name,pLsSubPartFamily->GetCount());
		hTreeItem=m_xTreeCtrl.InsertItem(sName,PRJ_IMG_FL,PRJ_IMG_FL,hGroup);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(LS_COMPONENT_ITEM,(DWORD)pLsSubPartFamily));
		m_xTreeCtrl.SetItemData(hTreeItem,(DWORD)pItemInfo);
	}
}
void CLsGuigeLibDlg::RefreshLsGuigeTreeCtrl(HTREEITEM hGroup)
{
	DeleteAllSubItems(&m_xTreeCtrl,hGroup);
	TREEITEM_INFO *pItemInfo=NULL;
	HTREEITEM hTreeItem=NULL;
	for(CLsFamily* pLsFamily=m_pBoltLib->GetFirstFamily();pLsFamily;pLsFamily=m_pBoltLib->GetNextFamily())
	{
		CXhChar50 sName("%s(%d)",pLsFamily->name,pLsFamily->GetCount());
		hTreeItem=m_xTreeCtrl.InsertItem(sName,PRJ_IMG_FL,PRJ_IMG_FL,hGroup);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(LS_GUIGE_ITEM,(DWORD)pLsFamily));
		m_xTreeCtrl.SetItemData(hTreeItem,(DWORD)pItemInfo);
	}
}
void CLsGuigeLibDlg::RefreshAnchorBoltsTreeCtrl()
{
	m_xListAnchorBoltCtrl.DeleteAllItems();
	CStringArray str_arr;
	str_arr.SetSize(9);
	int i=0,count=ANCHOR_BOLT::GetCount();
	for(int i=0;i<count;i++)
	{
		const ANCHOR_BOLT* pAnchor=ANCHOR_BOLT::RecordAt(i);
		str_arr[0].Format("%d",pAnchor->d);	//"直径(mm)";
		//m_fLo(丝扣长)=出露长-无扣长
		str_arr[1].Format("%g",pAnchor->m_fLe);	//"出露长(mm)"
		str_arr[2].Format("%g",pAnchor->m_fLa);	//"无扣长(mm)"
		str_arr[3].Format("%g",pAnchor->m_fNutH);	//"螺母高度(mm)"
		CXhChar16 szDs("%.1f",pAnchor->m_fNutDs);
		SimplifiedNumString(szDs);
		str_arr[4]=szDs;			//"螺母宽边到边距离(mm)"
		//str_arr[5].Format("%f",pAnchor->m_fNutTaper);	//"无扣长(mm)"
		//float m_fNutTaper;	//螺母端面过渡圆锥面的坡度h/w;β=15°~30°
		str_arr[5].Format("%d",pAnchor->wiBasePlateHoleD);			//"座板孔径(mm)"
		str_arr[6].Format("%d",pAnchor->d+pAnchor->wiHoleIncrement);//"垫板孔径(mm)"
		str_arr[7].Format("%d",pAnchor->wiWidth);	//"垫板宽度(mm)"
		str_arr[8].Format("%d",pAnchor->wiThick);	//"垫板厚度(mm)"
		//str_arr[9].Format("%d",pAnchor->S);	//"座板间距(mm)"
		m_xListAnchorBoltCtrl.InsertItemRecord(-1,str_arr);
	}
}
void CLsGuigeLibDlg::RefreshListCtrl()
{
	DWORD dwData=0;
	if(GetCurSelItmeType(&dwData)!=LS_GUIGE_ITEM)
		return;
	CLsFamily* pLsFamily=(CLsFamily*)dwData;
	m_xListCtrl.DeleteAllItems();
	CStringArray str_arr;
	str_arr.SetSize(7);
	for(int i=0;i<pLsFamily->GetCount();i++)
	{
		LS_XING_HAO *pXingHao=pLsFamily->RecordAt(i);
		str_arr[0].Format("%f",pXingHao->grade);
		SimplifiedNumString(str_arr[0]);
		str_arr[1].Format("%d",pXingHao->d);
		str_arr[2].Format("%d",pXingHao->valid_len);
		str_arr[3].Format("%d",pXingHao->no_thread_len);
		str_arr[4].Format("%d",pXingHao->down_lim);		
		str_arr[5].Format("%d",pXingHao->up_lim);
		str_arr[6].Format("%f",pXingHao->weight);
		m_xListCtrl.InsertItemRecord(-1,str_arr);
	}
}
void CLsGuigeLibDlg::RefreshLsCompntListCtrl()
{
	DWORD dwData=0;
	if(GetCurSelItmeType(&dwData)!=LS_COMPONENT_ITEM)
		return;
	CLsSubPartFamily* pLsSubPartFamliy=(CLsSubPartFamily*)dwData;
	m_xLsComptListCtrl.DeleteAllItems();
	CSuperGridCtrl::CTreeItem* pItem=NULL;
	int i=0;
	for(i=0;i<pLsSubPartFamliy->GetCount();i++)
	{
		LS_SUB_PART* pLsSubPart=pLsSubPartFamliy->RecordAt(i);
		if(pLsSubPart)
		{
			CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
			CXhChar50 sTextValue("%d",pLsSubPart->wiLsD);
			lpInfo->AddSubItemText(sTextValue);	//直径
			sTextValue.Printf("%f",pLsSubPart->thick);		
			SimplifiedNumString(sTextValue);
			lpInfo->AddSubItemText(sTextValue);	//厚度
			sTextValue.Printf("%g",pLsSubPart->dfShapeDs);		
			lpInfo->AddSubItemText(sTextValue);	//宽度
			sTextValue.Printf("%f",pLsSubPart->weight);
			SimplifiedNumString(sTextValue);
			lpInfo->AddSubItemText(sTextValue);	//重量
			//
			pItem=m_xLsComptListCtrl.InsertRootItem(lpInfo);
			pItem->m_idProp=i+1;
		}
	}
	//末尾追加一条空记录，便于用户添加记录
	CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
	lpInfo->AddSubItemText("");
	lpInfo->AddSubItemText("");
	lpInfo->AddSubItemText("");
	pItem=m_xLsComptListCtrl.InsertRootItem(lpInfo);
	pItem->m_idProp=i+1;
}
void CLsGuigeLibDlg::ContextMenu(CWnd* pWnd, CPoint point)
{
	int iCurItemType=GetCurSelItmeType();
	if(iCurItemType==0)
		return;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	//
	if(iCurItemType==LS_COMPONENT_GROUP)
	{
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"导入螺栓零件库文件");
	}
	else if(iCurItemType==LS_COMPONENT_ITEM)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"清空螺栓零件库");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"导出螺栓零件库文件");
	}
	else if(iCurItemType==LS_GUIGE_GROUP)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"清空螺栓库");	
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"导入螺栓系列文件");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_LOAD_FILE,"导入系统螺栓系列");
	}
	else if(iCurItemType==LS_GUIGE_ITEM)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除螺栓系列");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"导出螺栓系列文件");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"导入螺栓系列文件");
	}
	CPoint scr_point = point;
	m_xTreeCtrl.ClientToScreen(&scr_point);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}
void CLsGuigeLibDlg::OnOK()
{
	//保存匹配好的螺栓系列库
	CLsLibrary::ClearLibrary();
	for(CLsFamily* pLsFamily=m_pBoltLib->GetFirstFamily();pLsFamily;pLsFamily=m_pBoltLib->GetNextFamily())
	{
		CLsFamily* pNewFamily=CLsLibrary::AppendFamily(pLsFamily->GetFamilyHandle());
		m_pBoltLib->CopyLsFamily(pNewFamily,pLsFamily);
	}
	//销毁库指针
	IBoltLibrary::DestroyBoltLibrary(m_pSrcBoltLib->GetSerialId());
	IBoltLibrary::DestroyBoltLibrary(m_pBoltLib->GetSerialId());
	//
	return CDialog::OnOK();
}
void CLsGuigeLibDlg::OnCancel()
{
	//保存原有的螺栓系列库
	CLsLibrary::ClearLibrary();
	for(CLsFamily* pLsFamily=m_pSrcBoltLib->GetFirstFamily();pLsFamily;pLsFamily=m_pSrcBoltLib->GetNextFamily())
	{
		CLsFamily* pNewFamily=CLsLibrary::AppendFamily(pLsFamily->GetFamilyHandle());
		m_pSrcBoltLib->CopyLsFamily(pNewFamily,pLsFamily);
	}
	//销毁库指针
	IBoltLibrary::DestroyBoltLibrary(m_pSrcBoltLib->GetSerialId());
	IBoltLibrary::DestroyBoltLibrary(m_pBoltLib->GetSerialId());
	//
	return CDialog::OnCancel();
}
//////////////////////////////////////////////////////////////////////////
//消息事件处理
void CLsGuigeLibDlg::OnSelchangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem=m_xTreeCtrl.GetSelectedItem();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	RelayoutWnd();
	if(pItemInfo->itemType==LS_COMPONENT_ITEM)
		RefreshLsCompntListCtrl();
	else if(pItemInfo->itemType==LS_GUIGE_ITEM)
	{
		DisplayProperty();
		RefreshListCtrl();
	}
	*pResult = 0;
}

void CLsGuigeLibDlg::OnKeydownTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;

	*pResult = 0;
}

void CLsGuigeLibDlg::OnRclickTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	TVHITTESTINFO HitTestInfo;
	GetCursorPos(&HitTestInfo.pt);
	m_xTreeCtrl.ScreenToClient(&HitTestInfo.pt);
	m_xTreeCtrl.HitTest(&HitTestInfo);
	m_xTreeCtrl.Select(HitTestInfo.hItem,TVGN_CARET);
	ContextMenu(this,HitTestInfo.pt);
	*pResult = 0;
}

void CLsGuigeLibDlg::OnImportLsFamilyFile()
{
	DWORD dwData=0;
	int iSelItemType=GetCurSelItmeType(&dwData);
	if(iSelItemType==LS_COMPONENT_GROUP)
	{

	}
	else if(iSelItemType==LS_COMPONENT_ITEM)
	{
		CLsSubPartFamily* pFamily=(CLsSubPartFamily*)dwData;
		if(pFamily==NULL)
			return;
#ifdef AFX_TARG_ENU_ENGLISH
		CFileDialog dlg(TRUE,"lssp","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"bolt specification file(*.lssp)|*.lssp||");
#else
		CFileDialog dlg(TRUE,"lssp","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"螺栓零件文件(*.lssp)|*.lssp||");

#endif
		if(dlg.DoModal()!=IDOK)
			return;
		CString sFileName=dlg.GetPathName();
		LsSubPartSerialize(pFamily,sFileName,FALSE);
		//导入以后需要刷新列表内容
		CXhChar50 sPartType("%s(%d)",pFamily->name,pFamily->GetCount());
		m_xTreeCtrl.SetItemText(m_xTreeCtrl.GetSelectedItem(),sPartType);
		RefreshLsCompntListCtrl();
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		CFileDialog dlg(TRUE,"bfs","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"bolt specification file(*.bfs)|*.bfs||");
#else
		CFileDialog dlg(TRUE,"bfs","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"螺栓系列文件(*.bfs)|*.bfs||");

#endif
		if(dlg.DoModal()!=IDOK)
			return;
		CLsFamily temLsFamily;
		BoltFamilySerialize(&temLsFamily,dlg.GetPathName(),FALSE);
		//更新树节点
		HTREEITEM hSelItem=m_xTreeCtrl.GetSelectedItem();
		if(iSelItemType==LS_GUIGE_GROUP)
		{	//添加一种新的螺栓系列
			CLsFamily* pNewFamily=m_pBoltLib->AppendFamily();
			m_pBoltLib->CopyLsFamily(pNewFamily,&temLsFamily);
			//
			CXhChar50 sName("%s(%d)",pNewFamily->name,pNewFamily->GetCount());
			HTREEITEM hTreeItem=m_xTreeCtrl.InsertItem(sName,PRJ_IMG_FL,PRJ_IMG_FL,hSelItem);
			TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(LS_GUIGE_ITEM,(DWORD)pNewFamily));
			m_xTreeCtrl.SetItemData(hTreeItem,(DWORD)pItemInfo);
		}
		else if(iSelItemType==LS_GUIGE_ITEM)
		{	//覆盖选中的螺栓系列
			CLsFamily* pCurFamily=(CLsFamily*)dwData;
			m_pBoltLib->CopyLsFamily(pCurFamily,&temLsFamily);
			//
			CXhChar50 sType("%s(%d)",pCurFamily->name,pCurFamily->GetCount());
			m_xTreeCtrl.SetItemText(hSelItem,sType);
			RefreshListCtrl();
		}
	}
}

void CLsGuigeLibDlg::OnExportLsFamilyFile()
{
	DWORD dwData=0;
	int iSelItemType=GetCurSelItmeType(&dwData);
	if(iSelItemType==LS_GUIGE_ITEM)
	{
		CLsFamily* pFamily=(CLsFamily*)dwData;
		if(pFamily==NULL)
			return;
	#ifdef AFX_TARG_ENU_ENGLISH
		CFileDialog dlg(FALSE,"bfs","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"bolt series file(*.bfs)|*.bfs||");	
	#else
		CFileDialog dlg(FALSE,"bfs","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"螺栓系列文件(*.bfs)|*.bfs||");

	#endif
		if(dlg.DoModal()!=IDOK)
			return;
		BoltFamilySerialize(pFamily,dlg.GetPathName(),TRUE);
	}
	else if(iSelItemType==LS_COMPONENT_ITEM)
	{
		CLsSubPartFamily* pFamily=(CLsSubPartFamily*)dwData;
		if(pFamily==NULL)
			return;
#ifdef AFX_TARG_ENU_ENGLISH
		CFileDialog dlg(FALSE,"lssp","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"bolt specification file(*.lssp)|*.lssp||");	
#else
		CFileDialog dlg(FALSE,"lssp","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"螺栓零件文件(*.lssp)|*.lssp||");

#endif
		if(dlg.DoModal()!=IDOK)
			return;
		CString sFileName = dlg.GetPathName();
		LsSubPartSerialize(pFamily,sFileName,TRUE);
	}
}

void CLsGuigeLibDlg::OnImportSysLsFamily()
{
	DWORD dwData=0;
	int iSelItemType=GetCurSelItmeType(&dwData);
	if(iSelItemType!=LS_GUIGE_GROUP)
		return;
	IBoltLibrary* pBoltLib=IBoltLibrary::CreateBoltLibrary();
	char default_file[MAX_PATH];
	strcpy(default_file,theApp.m_sLicFile);
	char* separator=SearchChar(default_file,'.',true);
	strcpy(separator,".plb");
	InitPartLibrary(default_file,pBoltLib);
	CSysLsFamilyDlg dlg;
	dlg.m_pBoltLib=pBoltLib;
	if(dlg.DoModal()==IDOK)
	{
		HTREEITEM hSelItem=m_xTreeCtrl.GetSelectedItem();
		for(CLsFamily* pLsFamily=pBoltLib->GetFirstFamily();pLsFamily;pLsFamily=pBoltLib->GetNextFamily())
		{
			CLsFamily* pNewFamily=m_pBoltLib->AppendFamily();
			m_pBoltLib->CopyLsFamily(pNewFamily,pLsFamily);
			//
			CXhChar50 sName("%s(%d)",pNewFamily->name,pNewFamily->GetCount());
			HTREEITEM hTreeItem=m_xTreeCtrl.InsertItem(sName,PRJ_IMG_FL,PRJ_IMG_FL,hSelItem);
			TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(LS_GUIGE_ITEM,(DWORD)pNewFamily));
			m_xTreeCtrl.SetItemData(hTreeItem,(DWORD)pItemInfo);
		}
	}
	IBoltLibrary::DestroyBoltLibrary(pBoltLib->GetSerialId());
}

void CLsGuigeLibDlg::OnDeleteItem()
{
	DWORD dwData=0;
	int iSelItemType=GetCurSelItmeType(&dwData);
	if(iSelItemType==LS_COMPONENT_ITEM)
	{
		CLsSubPartFamily* pFamily=(CLsSubPartFamily*)dwData;
		pFamily->CleanSubParts();
		m_xTreeCtrl.SetItemText(m_xTreeCtrl.GetSelectedItem(),CXhChar50("%s(0)",pFamily->name));
		RefreshLsCompntListCtrl();
	}
	else if(iSelItemType==LS_GUIGE_GROUP)
	{
		m_pBoltLib->ClearLibrary();
		RefreshLsGuigeTreeCtrl(m_xTreeCtrl.GetSelectedItem());
	}
	else if(iSelItemType==LS_GUIGE_ITEM)
	{
		CLsFamily* pCurFamily=(CLsFamily*)dwData;
		m_pBoltLib->RemoveFamily(pCurFamily->GetFamilyHandle());
		//
		HTREEITEM hSelItem=m_xTreeCtrl.GetSelectedItem();
		m_xTreeCtrl.DeleteItem(hSelItem);
		m_xTreeCtrl.SelectItem(m_xTreeCtrl.GetRootItem());
	}
}

void CLsGuigeLibDlg::OnBnSaveLsComponent()
{
	DWORD dwData=0;
	if(GetCurSelItmeType(&dwData)!=LS_COMPONENT_ITEM)
		return;
	CLsSubPartFamily* pLsSubPartFamily=(CLsSubPartFamily*)dwData;
	if(pLsSubPartFamily==NULL)
		return;
	int nNum = m_xLsComptListCtrl.GetItemCount();
	if(nNum-1<=0)
		return;
	CXhChar100 sText;
	pLsSubPartFamily->CreateLsSubPartFamily(nNum-1);
	for(int i=0;i<nNum-1;i++)
	{
		LS_SUB_PART *pSubPart=pLsSubPartFamily->RecordAt(i);
		m_xLsComptListCtrl.GetItemText(i,0,sText,100);
		pSubPart->wiLsD=atoi(sText);
		m_xLsComptListCtrl.GetItemText(i,1,sText,100);
		pSubPart->thick=atof(sText);
		m_xLsComptListCtrl.GetItemText(i,2,sText,100);
		pSubPart->weight=atof(sText);
	}
	//
	CXhChar50 sPartType("%s(%d)",pLsSubPartFamily->name,pLsSubPartFamily->GetCount());
	m_xTreeCtrl.SetItemText(m_xTreeCtrl.GetSelectedItem(),sPartType);
}
