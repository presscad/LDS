// LsGuigeLibDlg.cpp : ʵ���ļ�
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
//�ص���������
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
		if(valueStr.Compare("��")==0)
			pCurLsFamily->m_bFootNail=TRUE;
		else
			pCurLsFamily->m_bFootNail=FALSE;
	}
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("Safety"))
	{	//0.��ͨ|1.˫ñ|2.����|3.����
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
		if(stricmp(valueStr,"��")==0)
			pCurLsFamily->m_hMappingAntiLooseFamily=0;
		else
			pCurLsFamily->m_hMappingAntiLooseFamily=valueStr[0]-'0';
	}
	else if(pItem->m_idProp==CLsGuigeLibDlg::GetPropID("MappingAntiTheftFamily"))
	{
		if(stricmp(valueStr,"��")==0)
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
	{	//��˨ֱ����������˨����Χ�ڣ���ǰ�����12��48��
		if (atoi(sTextValue)<12||atoi(sTextValue)>48)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox(NULL,"bolt's diameter is not in the specified range!","ERROR",MB_OK);
#else
			MessageBox(NULL,"��˨ֱ�����ڹ涨��Χ��!","ERROR",MB_OK);
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
// CLsGuigeLibDlg �Ի���
IMPLEMENT_DYNAMIC(CLsGuigeLibDlg, CRememberLocationDlg)

CLsGuigeLibDlg::CLsGuigeLibDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CLsGuigeLibDlg::IDD, pParent)
{
	m_xListCtrl.AddColumnHeader("����");
	m_xListCtrl.AddColumnHeader("ֱ��");
	m_xListCtrl.AddColumnHeader("��Ч��");
	m_xListCtrl.AddColumnHeader("�޿۳�");
	m_xListCtrl.AddColumnHeader("ͨ������");
	m_xListCtrl.AddColumnHeader("ͨ������");
	m_xListCtrl.AddColumnHeader("����");
	//
	m_xLsComptListCtrl.AddColumnHeader("��˨ֱ��(mm)");
	m_xLsComptListCtrl.AddColumnHeader("������(mm)");
	m_xLsComptListCtrl.AddColumnHeader("������(mm)");
	m_xLsComptListCtrl.AddColumnHeader("�������(kg)");
	//
	m_xListAnchorBoltCtrl.AddColumnHeader("ֱ��");
	m_xListAnchorBoltCtrl.AddColumnHeader("��¶��");
	m_xListAnchorBoltCtrl.AddColumnHeader("�޿۳�");
	//m_xListAnchorBoltCtrl.AddColumnHeader("˿�۳�(mm)");
	m_xListAnchorBoltCtrl.AddColumnHeader("��ĸ��");
	m_xListAnchorBoltCtrl.AddColumnHeader("��ĸ��");//(��ĸ�ߵ��߾���)
	m_xListAnchorBoltCtrl.AddColumnHeader("����׾�");
	m_xListAnchorBoltCtrl.AddColumnHeader("���׾�");
	m_xListAnchorBoltCtrl.AddColumnHeader("����");
	m_xListAnchorBoltCtrl.AddColumnHeader("����");
	//m_xListAnchorBoltCtrl.AddColumnHeader("������(mm)");
	//
	m_pBoltLib=NULL;		//���ڼ�¼ƥ��õ���˨ϵ�п�
	m_pSrcBoltLib=NULL;		//���ڼ�¼��ǰģ��ʹ�õ���˨ϵ�п�
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


// CLsGuigeLibDlg ��Ϣ�������
BOOL CLsGuigeLibDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitBoltLibrary();
	//��ʼ��������
	m_xPropList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xPropList.SetModifyValueFunc(ModifyPropValue);
	m_xPropList.SetButtonClickFunc(ButtonClickProp);
	InitPropHashtable();	//��ʼ�����Թ�ϣ��
	//��ʼ�����ؼ�
	m_imageList.Create(IDB_IL_PROJECT, 16, 1, RGB(0,255,0));
	m_xTreeCtrl.SetImageList(&m_imageList,TVSIL_NORMAL);
	m_xTreeCtrl.ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS);
	//��ʼ���б�ؼ�
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
	AddPropItem("General",PROPLIST_ITEM(id++,"��˨ϵ�л�����Ϣ"));
	AddPropItem("ID",PROPLIST_ITEM(id++,"ϵ�к�"));
	AddPropItem("name",PROPLIST_ITEM(id++,"����"));
	AddPropItem("descr",PROPLIST_ITEM(id++,"����"));
	AddPropItem("FootNail",PROPLIST_ITEM(id++,"�Ŷ�����","����˨ϵ���Ƿ���Ϊ�Ŷ�ʹ��","��|��"));
	AddPropItem("Safety",PROPLIST_ITEM(id++,"��ȫ��ʩ","����˨ϵ�еİ�ȫ��ʩ","0.��ͨ|1.˫ñ|2.����|3.����"));
	AddPropItem("MappingAntiTheftFamily",PROPLIST_ITEM(id++,"���׷���ϵ��","���׵ķ���ϵ��"));
	AddPropItem("MappingAntiLooseFamily",PROPLIST_ITEM(id++,"���׷���ϵ��","���׵ķ���ϵ��"));
	AddPropItem("LsSubPart",PROPLIST_ITEM(id++,"��˨�����Ϣ"));
	AddPropItem("comm_nut",PROPLIST_ITEM(id++,"��ͨ��ĸ��"));
	AddPropItem("fast_nut",PROPLIST_ITEM(id++,"�۽���ĸ��"));
	AddPropItem("fd_nut",PROPLIST_ITEM(id++,"������ĸ��"));
	AddPropItem("thin_nut",PROPLIST_ITEM(id++,"����ĸ��"));
	AddPropItem("comm_gasket",PROPLIST_ITEM(id++,"ƽ��Ȧ��"));
	AddPropItem("spring_gasket",PROPLIST_ITEM(id++,"���ɵ�Ȧ��"));
	AddPropItem("lock_clasp",PROPLIST_ITEM(id++,"���ɿ���"));
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
			sText.Copy("��");
		else
			sText.Copy("��");
	}
	else if(GetPropID("Safety")==id)
	{	
		if(pCurLsFamily->m_bBurglarproof)
			sText.Copy("2.����");
		else if(pCurLsFamily->m_bAntiLoose)
			sText.Copy("3.����");
		else if(pCurLsFamily->m_bDualCap)
			sText.Copy("1.˫ñ");
		else
			sText.Copy("0.��ͨ");
	}
	else if(GetPropID("MappingAntiTheftFamily")==id)
	{
		CLsFamily* pLsFamily=m_pBoltLib->FromHandle(pCurLsFamily->m_hMappingAntiTheftFamily);
		if(pLsFamily)
			sText.Printf("%d-%s",pLsFamily->GetFamilyHandle(),pLsFamily->name);
		else
			sText.Copy("��");
	}
	else if(GetPropID("MappingAntiLooseFamily")==id)
	{	
		CLsFamily* pLsFamily=m_pBoltLib->FromHandle(pCurLsFamily->m_hMappingAntiLooseFamily);
		if(pLsFamily)
			sText.Printf("%d-%s",pLsFamily->GetFamilyHandle(),pLsFamily->name);
		else
			sText.Copy("��");
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
	CString sAttachItemValue("��");
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
	//������Ϣ
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
	//�����Ϣ
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
	//��ȡ��ǰģ��ʹ�õ���˨����
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
	//����ѡ�����ڵ���½�����ʾ
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
	hGroupItem1=m_xTreeCtrl.InsertItem("��˨�����",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(LS_COMPONENT_GROUP,0));
	m_xTreeCtrl.SetItemData(hGroupItem1,(DWORD)pItemInfo);
	RefreshLsCompnTreeCtrl(hGroupItem1);
	//
	hGroupItem2=m_xTreeCtrl.InsertItem("��˨����",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(LS_GUIGE_GROUP,(DWORD)m_pBoltLib));
	m_xTreeCtrl.SetItemData(hGroupItem2,(DWORD)pItemInfo);
	RefreshLsGuigeTreeCtrl(hGroupItem2);
	//
	hGroupItem3=m_xTreeCtrl.InsertItem("�ؽ���˨��",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
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
		str_arr[0].Format("%d",pAnchor->d);	//"ֱ��(mm)";
		//m_fLo(˿�۳�)=��¶��-�޿۳�
		str_arr[1].Format("%g",pAnchor->m_fLe);	//"��¶��(mm)"
		str_arr[2].Format("%g",pAnchor->m_fLa);	//"�޿۳�(mm)"
		str_arr[3].Format("%g",pAnchor->m_fNutH);	//"��ĸ�߶�(mm)"
		CXhChar16 szDs("%.1f",pAnchor->m_fNutDs);
		SimplifiedNumString(szDs);
		str_arr[4]=szDs;			//"��ĸ��ߵ��߾���(mm)"
		//str_arr[5].Format("%f",pAnchor->m_fNutTaper);	//"�޿۳�(mm)"
		//float m_fNutTaper;	//��ĸ�������Բ׶����¶�h/w;��=15��~30��
		str_arr[5].Format("%d",pAnchor->wiBasePlateHoleD);			//"����׾�(mm)"
		str_arr[6].Format("%d",pAnchor->d+pAnchor->wiHoleIncrement);//"���׾�(mm)"
		str_arr[7].Format("%d",pAnchor->wiWidth);	//"�����(mm)"
		str_arr[8].Format("%d",pAnchor->wiThick);	//"�����(mm)"
		//str_arr[9].Format("%d",pAnchor->S);	//"������(mm)"
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
			lpInfo->AddSubItemText(sTextValue);	//ֱ��
			sTextValue.Printf("%f",pLsSubPart->thick);		
			SimplifiedNumString(sTextValue);
			lpInfo->AddSubItemText(sTextValue);	//���
			sTextValue.Printf("%g",pLsSubPart->dfShapeDs);		
			lpInfo->AddSubItemText(sTextValue);	//���
			sTextValue.Printf("%f",pLsSubPart->weight);
			SimplifiedNumString(sTextValue);
			lpInfo->AddSubItemText(sTextValue);	//����
			//
			pItem=m_xLsComptListCtrl.InsertRootItem(lpInfo);
			pItem->m_idProp=i+1;
		}
	}
	//ĩβ׷��һ���ռ�¼�������û���Ӽ�¼
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
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"������˨������ļ�");
	}
	else if(iCurItemType==LS_COMPONENT_ITEM)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"�����˨�����");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"������˨������ļ�");
	}
	else if(iCurItemType==LS_GUIGE_GROUP)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"�����˨��");	
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"������˨ϵ���ļ�");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_LOAD_FILE,"����ϵͳ��˨ϵ��");
	}
	else if(iCurItemType==LS_GUIGE_ITEM)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ����˨ϵ��");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"������˨ϵ���ļ�");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"������˨ϵ���ļ�");
	}
	CPoint scr_point = point;
	m_xTreeCtrl.ClientToScreen(&scr_point);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}
void CLsGuigeLibDlg::OnOK()
{
	//����ƥ��õ���˨ϵ�п�
	CLsLibrary::ClearLibrary();
	for(CLsFamily* pLsFamily=m_pBoltLib->GetFirstFamily();pLsFamily;pLsFamily=m_pBoltLib->GetNextFamily())
	{
		CLsFamily* pNewFamily=CLsLibrary::AppendFamily(pLsFamily->GetFamilyHandle());
		m_pBoltLib->CopyLsFamily(pNewFamily,pLsFamily);
	}
	//���ٿ�ָ��
	IBoltLibrary::DestroyBoltLibrary(m_pSrcBoltLib->GetSerialId());
	IBoltLibrary::DestroyBoltLibrary(m_pBoltLib->GetSerialId());
	//
	return CDialog::OnOK();
}
void CLsGuigeLibDlg::OnCancel()
{
	//����ԭ�е���˨ϵ�п�
	CLsLibrary::ClearLibrary();
	for(CLsFamily* pLsFamily=m_pSrcBoltLib->GetFirstFamily();pLsFamily;pLsFamily=m_pSrcBoltLib->GetNextFamily())
	{
		CLsFamily* pNewFamily=CLsLibrary::AppendFamily(pLsFamily->GetFamilyHandle());
		m_pSrcBoltLib->CopyLsFamily(pNewFamily,pLsFamily);
	}
	//���ٿ�ָ��
	IBoltLibrary::DestroyBoltLibrary(m_pSrcBoltLib->GetSerialId());
	IBoltLibrary::DestroyBoltLibrary(m_pBoltLib->GetSerialId());
	//
	return CDialog::OnCancel();
}
//////////////////////////////////////////////////////////////////////////
//��Ϣ�¼�����
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
		CFileDialog dlg(TRUE,"lssp","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"��˨����ļ�(*.lssp)|*.lssp||");

#endif
		if(dlg.DoModal()!=IDOK)
			return;
		CString sFileName=dlg.GetPathName();
		LsSubPartSerialize(pFamily,sFileName,FALSE);
		//�����Ժ���Ҫˢ���б�����
		CXhChar50 sPartType("%s(%d)",pFamily->name,pFamily->GetCount());
		m_xTreeCtrl.SetItemText(m_xTreeCtrl.GetSelectedItem(),sPartType);
		RefreshLsCompntListCtrl();
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		CFileDialog dlg(TRUE,"bfs","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"bolt specification file(*.bfs)|*.bfs||");
#else
		CFileDialog dlg(TRUE,"bfs","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"��˨ϵ���ļ�(*.bfs)|*.bfs||");

#endif
		if(dlg.DoModal()!=IDOK)
			return;
		CLsFamily temLsFamily;
		BoltFamilySerialize(&temLsFamily,dlg.GetPathName(),FALSE);
		//�������ڵ�
		HTREEITEM hSelItem=m_xTreeCtrl.GetSelectedItem();
		if(iSelItemType==LS_GUIGE_GROUP)
		{	//���һ���µ���˨ϵ��
			CLsFamily* pNewFamily=m_pBoltLib->AppendFamily();
			m_pBoltLib->CopyLsFamily(pNewFamily,&temLsFamily);
			//
			CXhChar50 sName("%s(%d)",pNewFamily->name,pNewFamily->GetCount());
			HTREEITEM hTreeItem=m_xTreeCtrl.InsertItem(sName,PRJ_IMG_FL,PRJ_IMG_FL,hSelItem);
			TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(LS_GUIGE_ITEM,(DWORD)pNewFamily));
			m_xTreeCtrl.SetItemData(hTreeItem,(DWORD)pItemInfo);
		}
		else if(iSelItemType==LS_GUIGE_ITEM)
		{	//����ѡ�е���˨ϵ��
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
		CFileDialog dlg(FALSE,"bfs","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"��˨ϵ���ļ�(*.bfs)|*.bfs||");

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
		CFileDialog dlg(FALSE,"lssp","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"��˨����ļ�(*.lssp)|*.lssp||");

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
