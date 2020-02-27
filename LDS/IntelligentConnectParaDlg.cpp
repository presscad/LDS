// IntelligentConnectParaDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "IntelligentConnectParaDlg.h"
#include "Query.h"
#include "env_def.h"
#include "XhCharString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIntelligentConnectParaDlg dialog


CIntelligentConnectParaDlg::CIntelligentConnectParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIntelligentConnectParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIntelligentConnectParaDlg)
	//}}AFX_DATA_INIT
	m_nCurItemData=-1;
	m_nRayPoleNum=0;
	m_pDatumLinePart=NULL;
}


void CIntelligentConnectParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIntelligentConnectParaDlg)
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_LIST_LJ, m_listLj);
	DDX_Control(pDX, IDC_STYPE_TAB, m_tabDesignType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIntelligentConnectParaDlg, CDialog)
	//{{AFX_MSG_MAP(CIntelligentConnectParaDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LJ, OnItemchangedListLj)
	ON_NOTIFY(TCN_SELCHANGE, IDC_STYPE_TAB, OnSelchangeStypeTab)
	ON_NOTIFY(HDN_ENDDRAG, IDC_LIST_LJ, OnEnddragList)
	//}}AFX_MSG_MAP
	ON_WM_HELPINFO()
//	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIntelligentConnectParaDlg message handlers

void CIntelligentConnectParaDlg::RefreshTabCtrl(int iCurSel)
{
	CStringArray sDesignType;
	sDesignType.SetSize(5);
#ifdef AFX_TARG_ENU_ENGLISH
	sDesignType[0]="Single Ray Line";
	sDesignType[1]="Double Ray Line";
	sDesignType[2]="Multiply Ray Line";
	sDesignType[3]="Tube Board";
	sDesignType[4]="Flange";
#else 
	sDesignType[0]="������";
	sDesignType[1]="˫����";
	sDesignType[2]="������";
	sDesignType[3]="�ֹܲ��";
	sDesignType[4]="����";
#endif
	m_tabDesignType.DeleteAllItems();
	for(int i=0;i<sDesignType.GetSize();i++)
		m_tabDesignType.InsertItem(i,sDesignType[i]);
	m_tabDesignType.SetCurSel(iCurSel);
}
//������
const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
CHashStrTable<long> CIntelligentConnectParaDlg::propHashtable;
CHashTable<DWORD> CIntelligentConnectParaDlg::propStatusHashtable;
void CIntelligentConnectParaDlg::InitSysSettingPropHashtable()
{
	int id = 1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	if (propStatusHashtable.GetHashTableSize()<=0)
		propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	//����
	propHashtable.SetValueAt("generalSet",id++);
	propHashtable.SetValueAt("generalSet.iBoltLayoutStyle",id++);
	propHashtable.SetValueAt("generalSet.jgSpace",id++);
	propHashtable.SetValueAt("generalSet.nOffset",id++);
	propHashtable.SetValueAt("generalSet.nOffsetAdd",id++);
	propHashtable.SetValueAt("generalSet.bPriorOffsetGradientRod",id++);
	propHashtable.SetValueAt("generalSet.nFirstLsJgSpaceHasPlate",id++);
	propHashtable.SetValueAt("generalSet.n12LsOutterSpaceHasWasher",id++);
	propHashtable.SetValueAt("generalSet.n16LsOutterSpaceHasWasher",id++);
	propHashtable.SetValueAt("generalSet.n20LsOutterSpaceHasWasher",id++);
	propHashtable.SetValueAt("generalSet.n24LsOutterSpaceHasWasher",id++);
	propHashtable.SetValueAt("generalSet.nFirstLsJgSpaceNoPlate",id++);
	propHashtable.SetValueAt("generalSet.n12LsOutterSpace",id++);
	propHashtable.SetValueAt("generalSet.n16LsOutterSpace",id++);
	propHashtable.SetValueAt("generalSet.n20LsOutterSpace",id++);
	propHashtable.SetValueAt("generalSet.n24LsOutterSpace",id++);
	propHashtable.SetValueAt("generalSet.nFirstLsJgAngleEdgeSpace",id++);
	propHashtable.SetValueAt("generalSet.n12LsAngleEdgeSpace",id++);
	propHashtable.SetValueAt("generalSet.n16LsAngleEdgeSpace",id++);
	propHashtable.SetValueAt("generalSet.n20LsAngleEdgeSpace",id++);
	propHashtable.SetValueAt("generalSet.n24LsAngleEdgeSpace",id++);
	propHashtable.SetValueAt("generalSet.nFirstLsJgOutterStaggerSpace",id++);
	propHashtable.SetValueAt("generalSet.n12LsOutterStaggerSpace",id++);
	propHashtable.SetValueAt("generalSet.n16LsOutterStaggerSpace",id++);
	propHashtable.SetValueAt("generalSet.n20LsOutterStaggerSpace",id++);
	propHashtable.SetValueAt("generalSet.n24LsOutterStaggerSpace",id++);
	propHashtable.SetValueAt("generalSet.nFirstLsJgShareStaggerSpace",id++);
	propHashtable.SetValueAt("generalSet.n12LsShareStaggerSpace",id++);
	propHashtable.SetValueAt("generalSet.n16LsShareStaggerSpace",id++);
	propHashtable.SetValueAt("generalSet.n20LsShareStaggerSpace",id++);
	propHashtable.SetValueAt("generalSet.n24LsShareStaggerSpace",id++);

	
	propHashtable.SetValueAt("designSet",id++);
	propHashtable.SetValueAt("designSet.iProfileStyle",id++);
	propHashtable.SetValueAt("designSet.iSingleAngleJointStyle",id++);
	propHashtable.SetValueAt("designSet.iTwoAngleJointStyle",id++);
	propHashtable.SetValueAt("designSet.iFourAngleJointStyle",id++);
}

static BOOL ModifySystemSettingItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CIntelligentConnectParaDlg::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

static BOOL ModifySystemSettingValue(CPropertyList	*pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.iBoltLayoutStyle"))
		g_sysPara.intelligentConnect.m_iBoltLayoutStyle=valueStr[0]-'0';
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.nOffset"))
		g_sysPara.intelligentConnect.m_nOffset=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.nOffsetAdd"))
		g_sysPara.intelligentConnect.m_nOffsetAdd=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.bPriorOffsetGradientRod"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.intelligentConnect.m_bPriorOffsetGradientRod = true;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.intelligentConnect.m_bPriorOffsetGradientRod = true;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			g_sysPara.intelligentConnect.m_bPriorOffsetGradientRod = false;
	}
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n12LsOutterSpaceHasWasher"))
		g_sysPara.intelligentConnect.m_n12LsOutterSpaceHasWasher=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n16LsOutterSpaceHasWasher"))
		g_sysPara.intelligentConnect.m_n16LsOutterSpaceHasWasher=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n20LsOutterSpaceHasWasher"))
		g_sysPara.intelligentConnect.m_n20LsOutterSpaceHasWasher=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n24LsOutterSpaceHasWasher"))
		g_sysPara.intelligentConnect.m_n24LsOutterSpaceHasWasher=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n12LsOutterSpace"))
		g_sysPara.intelligentConnect.m_n12LsOutterSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n16LsOutterSpace"))
		g_sysPara.intelligentConnect.m_n16LsOutterSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n20LsOutterSpace"))
		g_sysPara.intelligentConnect.m_n20LsOutterSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n24LsOutterSpace"))
		g_sysPara.intelligentConnect.m_n24LsOutterSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n12LsAngleEdgeSpace"))
		g_sysPara.intelligentConnect.m_n12LsAngleEdgeSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n16LsAngleEdgeSpace"))
		g_sysPara.intelligentConnect.m_n16LsAngleEdgeSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n20LsAngleEdgeSpace"))
		g_sysPara.intelligentConnect.m_n20LsAngleEdgeSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n24LsAngleEdgeSpace"))
		g_sysPara.intelligentConnect.m_n24LsAngleEdgeSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n12LsOutterStaggerSpace"))
		g_sysPara.intelligentConnect.m_n12LsOutterStaggerSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n16LsOutterStaggerSpace"))
		g_sysPara.intelligentConnect.m_n16LsOutterStaggerSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n20LsOutterStaggerSpace"))
		g_sysPara.intelligentConnect.m_n20LsOutterStaggerSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n24LsOutterStaggerSpace"))
		g_sysPara.intelligentConnect.m_n24LsOutterStaggerSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n12LsShareStaggerSpace"))
		g_sysPara.intelligentConnect.m_n12LsShareStaggerSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n16LsShareStaggerSpace"))
		g_sysPara.intelligentConnect.m_n16LsShareStaggerSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n20LsShareStaggerSpace"))
		g_sysPara.intelligentConnect.m_n20LsShareStaggerSpace=atoi(valueStr);
	else if(pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("generalSet.n24LsShareStaggerSpace"))
		g_sysPara.intelligentConnect.m_n24LsShareStaggerSpace=atoi(valueStr);
	else if (pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("designSet.iProfileStyle"))
		g_sysPara.intelligentConnect.m_iPlateProfileStyle=valueStr[0]-'0';
	else if (pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("designSet.iSingleAngleJointStyle"))
		g_sysPara.intelligentConnect.m_iSingleAngleJointStyle=valueStr[0]-'0';
	else if (pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("designSet.iTwoAngleJointStyle"))
		g_sysPara.intelligentConnect.m_iTwoAngleJointStyle=valueStr[0]-'0';
	else if (pItem->m_idProp==CIntelligentConnectParaDlg::GetPropID("designSet.iFourAngleJointStyle"))
		g_sysPara.intelligentConnect.m_iFourAngleJointStyle=valueStr[0]-'0';
	else
		return FALSE;
	return TRUE;
}
BOOL ButtonClickSystemSetting(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	return TRUE;
}
long CIntelligentConnectParaDlg::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}

int CIntelligentConnectParaDlg::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CIntelligentConnectParaDlg::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}

BOOL CIntelligentConnectParaDlg::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CIntelligentConnectParaDlg::propStatusHashtable.SetValueAt(id,dwStatus);
}
void CIntelligentConnectParaDlg::DisplaySystemSetting()
{
	CItemInfo* lpInfo=NULL;
	CXhChar100 valueStr;
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pSonItem=NULL,*pRootItem=m_propList.GetRootItem();
	const int GROUP_GENERAL=1;
	
	m_propList.CleanTree();
	m_propList.m_nObjClassTypeId = 0;
	
	//���ûص�����
	m_propList.SetModifyValueFunc(ModifySystemSettingValue);
	m_propList.SetButtonClickFunc(ButtonClickSystemSetting);
	m_propList.SetModifyStatusFunc(ModifySystemSettingItemStatus);
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "General";
#else 
	lpInfo->m_strPropName = "����";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = GetPropID("generalSet");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	if(CIntelligentConnectParaDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CIntelligentConnectParaDlg::GetPropStatus(pGroupItem->m_idProp);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbType = CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems = "0.Left High Right Low|1.Right High Left Low";
	lpInfo->m_strPropName = "Two Row Bolt Set";
	lpInfo->m_strPropHelp = "Set the layout mode of bolt's double row";
#else 
	lpInfo->m_cmbItems = "0.����ҵ�|1.�Ҹ����";
	lpInfo->m_strPropName = "˫����˨���÷�ʽ";
	lpInfo->m_strPropHelp = "����˫����˨���÷�ʽ";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.iBoltLayoutStyle");
	if(GetPropValueStr(GetPropID("generalSet.iBoltLayoutStyle"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbType = CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems = "0.Standard Profile|1.Customized Profile|2.Envelope Profile";
	lpInfo->m_strPropName = "Create Profile Style";
	lpInfo->m_strPropHelp = "The style of generating profile";
#else 
	lpInfo->m_cmbItems = "0.��׼����|1.��������|2.��������";
	lpInfo->m_strPropName = "�������ɷ�ʽ";
	lpInfo->m_strPropHelp = "�������������õķ�ʽ";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("designSet.iProfileStyle");
	if(GetPropValueStr(GetPropID("designSet.iProfileStyle"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Exist Plate Hole Clearance";
#else 
	lpInfo->m_strPropName = "�е����׼�϶";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_bHideChildren=TRUE;
	pPropItem->m_idProp = GetPropID("generalSet.nFirstLsJgSpaceHasPlate");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	if(CIntelligentConnectParaDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CIntelligentConnectParaDlg::GetPropStatus(pPropItem->m_idProp);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M12";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 12mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ12mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n12LsOutterSpaceHasWasher");
	if(GetPropValueStr(GetPropID("generalSet.n12LsOutterSpaceHasWasher"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M16";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 16mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ16mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n16LsOutterSpaceHasWasher");
	if(GetPropValueStr(GetPropID("generalSet.n16LsOutterSpaceHasWasher"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M20";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 20mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ20mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n20LsOutterSpaceHasWasher");
	if(GetPropValueStr(GetPropID("generalSet.n20LsOutterSpaceHasWasher"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Others";
	lpInfo->m_strPropHelp = "Bolt's diameter is bigger than 20mm";
#else 
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "��˨ֱ������20mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n24LsOutterSpaceHasWasher");
	if(GetPropValueStr(GetPropID("generalSet.n24LsOutterSpaceHasWasher"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "No Plate Hole Clearance";
#else 
	lpInfo->m_strPropName = "�޵����׼�϶";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_bHideChildren=TRUE;
	pPropItem->m_idProp = GetPropID("generalSet.nFirstLsJgSpaceNoPlate");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	if(CIntelligentConnectParaDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CIntelligentConnectParaDlg::GetPropStatus(pPropItem->m_idProp);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M12";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 12mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ12mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n12LsOutterSpace");
	if(GetPropValueStr(GetPropID("generalSet.n12LsOutterSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M16";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 16mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ16mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n16LsOutterSpace");
	if(GetPropValueStr(GetPropID("generalSet.n16LsOutterSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M20";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 20mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ20mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n20LsOutterSpace");
	if(GetPropValueStr(GetPropID("generalSet.n20LsOutterSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Others";
	lpInfo->m_strPropHelp = "Bolt's diameter is bigger than 20mm";
#else 
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "��˨ֱ������20mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n24LsOutterSpace");
	if(GetPropValueStr(GetPropID("generalSet.n24LsOutterSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Bolt Wing Margin";
#else 
	lpInfo->m_strPropName = "��˨֫��߾�";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_bHideChildren=TRUE;
	pPropItem->m_idProp = GetPropID("generalSet.nFirstLsJgAngleEdgeSpace");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	if(CIntelligentConnectParaDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CIntelligentConnectParaDlg::GetPropStatus(pPropItem->m_idProp);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "M12";
	lpInfo->m_strPropHelp = "Bolt's diameter is 12mm";
#else 
	lpInfo->m_strPropName = "M12";
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ12mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n12LsAngleEdgeSpace");
	if(GetPropValueStr(GetPropID("generalSet.n12LsAngleEdgeSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M16";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 16mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ16mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n16LsAngleEdgeSpace");
	if(GetPropValueStr(GetPropID("generalSet.n16LsAngleEdgeSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M20";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 20mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ20mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n20LsAngleEdgeSpace");
	if(GetPropValueStr(GetPropID("generalSet.n20LsAngleEdgeSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Others";
	lpInfo->m_strPropHelp = "Bolt's diameter is bigger than 20mm";
#else 
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "��˨ֱ������20mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n24LsAngleEdgeSpace");
	if(GetPropValueStr(GetPropID("generalSet.n24LsAngleEdgeSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Outside Bolt Stagger Hole Space";
#else 
	lpInfo->m_strPropName = "�����˨��׼��";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_bHideChildren=TRUE;
	pPropItem->m_idProp = GetPropID("generalSet.nFirstLsJgOutterStaggerSpace");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	if(CIntelligentConnectParaDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CIntelligentConnectParaDlg::GetPropStatus(pPropItem->m_idProp);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M12";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 12mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ12mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n12LsOutterStaggerSpace");
	if(GetPropValueStr(GetPropID("generalSet.n12LsOutterStaggerSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M16";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 16mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ16mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n16LsOutterStaggerSpace");
	if(GetPropValueStr(GetPropID("generalSet.n16LsOutterStaggerSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M20";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 20mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ20mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n20LsOutterStaggerSpace");
	if(GetPropValueStr(GetPropID("generalSet.n20LsOutterStaggerSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Others";
	lpInfo->m_strPropHelp = "Bolt's diameter is bigger than 20mm";
#else 
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "��˨ֱ������20mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n24LsOutterStaggerSpace");
	if(GetPropValueStr(GetPropID("generalSet.n24LsOutterStaggerSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Common Bolt Stagger Hole Space";
#else 
	lpInfo->m_strPropName = "������˨��׼��";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_bHideChildren=TRUE;
	pPropItem->m_idProp = GetPropID("generalSet.nFirstLsJgShareStaggerSpace");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	if(CIntelligentConnectParaDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CIntelligentConnectParaDlg::GetPropStatus(pPropItem->m_idProp);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M12";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 12mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ12mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n12LsShareStaggerSpace");
	if(GetPropValueStr(GetPropID("generalSet.n12LsShareStaggerSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M16";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 16mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ16mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n16LsShareStaggerSpace");
	if(GetPropValueStr(GetPropID("generalSet.n16LsShareStaggerSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "M20";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp = "Bolt's diameter is 20mm";
#else 
	lpInfo->m_strPropHelp = "��˨ֱ��Ϊ20mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n20LsShareStaggerSpace");
	if(GetPropValueStr(GetPropID("generalSet.n20LsShareStaggerSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Others";
	lpInfo->m_strPropHelp = "Bolt's diameter is bigger than 20mm";
#else 
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "��˨ֱ������20mm";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.n24LsShareStaggerSpace");
	if(GetPropValueStr(GetPropID("generalSet.n24LsShareStaggerSpace"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "End Connect Angle Space";
#else 
	lpInfo->m_strPropName = "�����ӽǸּ��";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.jgSpace");
	pPropItem->m_bHideChildren=TRUE;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	if(CIntelligentConnectParaDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CIntelligentConnectParaDlg::GetPropStatus(pPropItem->m_idProp);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Initial Value";
	lpInfo->m_strPropHelp = "The initial space value of multiply isolated rods' end connective set";
#else 
	lpInfo->m_strPropName = "��ʼֵ";
	lpInfo->m_strPropHelp = "��������˼�������ʱ���õĳ�ʼ�����ֵ";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.nOffset");
	if(GetPropValueStr(GetPropID("generalSet.nOffset"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Grow Steps";
	lpInfo->m_strPropHelp = "multiply isolated rod set the growth step value,It shows to automatically adjust angle space directly when the grow-step value is 0��";
#else 
	lpInfo->m_strPropName = "��������";
	lpInfo->m_strPropHelp = "��������˼�������ʱ���õ�ƫ��ֵ��������,������������0ʱ��ʾ���Զ������Ǹּ��ֱ��ȡ����ʼֵ��";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.nOffsetAdd");
	if(GetPropValueStr(GetPropID("generalSet.nOffsetAdd"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Priority Diagonal Deviation";
	lpInfo->m_strPropHelp = "��Three components are not zero��Isolated ray connected to datum offset";
	lpInfo->m_cmbItems = "Yes|No";
	lpInfo->m_strPropValue = "Yes";
#else 
	lpInfo->m_strPropName = "���ȶ�б��ƫ��";
	lpInfo->m_strPropHelp = "���ȶ�б��������������Ϊ�㣩�Ĺ������߶����ӽ��л�׼ƫ��";
	lpInfo->m_cmbItems = "��|��";
	lpInfo->m_strPropValue = "��";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("generalSet.bPriorOffsetGradientRod");
	if(GetPropValueStr(GetPropID("generalSet.bPriorOffsetGradientRod"),valueStr)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pSonItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Joint";
#else 
	lpInfo->m_strPropName = "��ͷ";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = GetPropID("designSet");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	if(CIntelligentConnectParaDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CIntelligentConnectParaDlg::GetPropStatus(pGroupItem->m_idProp);
		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbType = CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems = "0.Outside Iron|1.Inside Or Outside Iron|2.Inside Iron Outside Plate|3.Inside Plate Outside Iron";
	lpInfo->m_strPropName = "Single Angle Joint Type";
	lpInfo->m_strPropHelp = "The single angle's joint type";
#else 
	lpInfo->m_cmbItems = "0.�����|1.�������|2.�ڰ�������ְ�|3.�ڰ��ְ������";
	lpInfo->m_strPropName = "���Ǹֽ�ͷ��ʽ";
	lpInfo->m_strPropHelp = "���Ǹֽ�ͷ��ʽ";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("designSet.iSingleAngleJointStyle");
	if(GetPropValueStr(GetPropID("designSet.iSingleAngleJointStyle"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbType = CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems = "0.Diagonal Combination Joint A|1.Diagonal Combination Joint B|2.Diagonal Combination Joint C|3.Diagonal Combination Joint D";
	lpInfo->m_strPropName = "Double Main Material Joint Method";
	lpInfo->m_strPropHelp = "Double main material's joint method";
#else 
	lpInfo->m_cmbItems = "0.�Խ���Ͻ�ͷA|1.�Խ���Ͻ�ͷB|2.�Խ���Ͻ�ͷC|3.�Խ���Ͻ�ͷD";
	lpInfo->m_strPropName = "˫���Ľ�ͷ��ʽ";
	lpInfo->m_strPropHelp = "˫���Ľ�ͷ��ʽ";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("designSet.iTwoAngleJointStyle");
	if(GetPropValueStr(GetPropID("designSet.iTwoAngleJointStyle"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbType = CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems = "0.Cross Combine Joint A|1.Cross Combine Joint B";
	lpInfo->m_strPropName = "Four Main Material Joint Method";
	lpInfo->m_strPropHelp = "Four main material's joint method";
#else 
	lpInfo->m_cmbItems = "0.ʮ����Ͻ�ͷA|1.ʮ����Ͻ�ͷB";
	lpInfo->m_strPropName = "�����Ľ�ͷ��ʽ";
	lpInfo->m_strPropHelp = "�����Ľ�ͷ��ʽ";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CIntelligentConnectParaDlg::GetPropID("designSet.iFourAngleJointStyle");
	if(GetPropValueStr(GetPropID("designSet.iFourAngleJointStyle"),valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	m_propList.Redraw();
}
int CIntelligentConnectParaDlg::GetPropValueStr(long id,char* valueStr)
{
	CXhChar100 sText;
	if(GetPropID("generalSet.iBoltLayoutStyle")==id)
	{
		if(g_sysPara.intelligentConnect.m_iBoltLayoutStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","0.Left High Right Low");
#else 
			sText.Printf("%s","0.����ҵ�");
#endif
		else if(g_sysPara.intelligentConnect.m_iBoltLayoutStyle==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","1.Right High Left Low");
#else 
			sText.Printf("%s","1.�Ҹ����");
#endif
	}
	else if(GetPropID("generalSet.nOffset")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_nOffset);
	else if(GetPropID("generalSet.nOffsetAdd")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_nOffsetAdd);
	else if(GetPropID("generalSet.bPriorOffsetGradientRod")==id)
	{
		if(g_sysPara.intelligentConnect.m_bPriorOffsetGradientRod)
#ifdef AFX_TARG_ENU_ENGLISH
			sText = "Yes";
		else
			sText = "No";
#else 
			sText = "��";
		else
			sText = "��";
#endif
	}
	else if(GetPropID("generalSet.n12LsOutterSpaceHasWasher")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n12LsOutterSpaceHasWasher);
	else if(GetPropID("generalSet.n16LsOutterSpaceHasWasher")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n16LsOutterSpaceHasWasher);
	else if(GetPropID("generalSet.n20LsOutterSpaceHasWasher")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n20LsOutterSpaceHasWasher);
	else if(GetPropID("generalSet.n24LsOutterSpaceHasWasher")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n24LsOutterSpaceHasWasher);
	else if(GetPropID("generalSet.n12LsOutterSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n12LsOutterSpace);
	else if(GetPropID("generalSet.n16LsOutterSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n16LsOutterSpace);
	else if(GetPropID("generalSet.n20LsOutterSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n20LsOutterSpace);
	else if(GetPropID("generalSet.n24LsOutterSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n24LsOutterSpace);
	else if(GetPropID("generalSet.n12LsAngleEdgeSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n12LsAngleEdgeSpace);
	else if(GetPropID("generalSet.n16LsAngleEdgeSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n16LsAngleEdgeSpace);
	else if(GetPropID("generalSet.n20LsAngleEdgeSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n20LsAngleEdgeSpace);
	else if(GetPropID("generalSet.n24LsAngleEdgeSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n24LsAngleEdgeSpace);
	else if(GetPropID("generalSet.n12LsOutterStaggerSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n12LsOutterStaggerSpace);
	else if(GetPropID("generalSet.n16LsOutterStaggerSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n16LsOutterStaggerSpace);
	else if(GetPropID("generalSet.n20LsOutterStaggerSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n20LsOutterStaggerSpace);
	else if(GetPropID("generalSet.n24LsOutterStaggerSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n24LsOutterStaggerSpace);
	else if(GetPropID("generalSet.n12LsShareStaggerSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n12LsShareStaggerSpace);
	else if(GetPropID("generalSet.n16LsShareStaggerSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n16LsShareStaggerSpace);
	else if(GetPropID("generalSet.n20LsShareStaggerSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n20LsShareStaggerSpace);
	else if(GetPropID("generalSet.n24LsShareStaggerSpace")==id)
		sText.Printf("%d",g_sysPara.intelligentConnect.m_n24LsShareStaggerSpace);
	else if(GetPropID("designSet.iProfileStyle")==id)
	{
		if(g_sysPara.intelligentConnect.m_iPlateProfileStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","0.Stand Profile");
#else 
			sText.Printf("%s","0.��׼����");
#endif
		else if(g_sysPara.intelligentConnect.m_iPlateProfileStyle==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","1.Custum Profile");
#else 
			sText.Printf("%s","1.��������");
#endif
		if(g_sysPara.intelligentConnect.m_iPlateProfileStyle==2)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","2.Envelope Profile");
#else 
			sText.Printf("%s","2.��������");
#endif
	}
	else if(GetPropID("designSet.iSingleAngleJointStyle")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(g_sysPara.intelligentConnect.m_iSingleAngleJointStyle==0)
			sText.Printf("%s","0.Outside Iron");
		else if(g_sysPara.intelligentConnect.m_iSingleAngleJointStyle==1)
			sText.Printf("%s","1.Inside Or Outside Iron");
		if(g_sysPara.intelligentConnect.m_iSingleAngleJointStyle==2)
			sText.Printf("%s","2.Inside Iron Outside Plate");
		if(g_sysPara.intelligentConnect.m_iSingleAngleJointStyle==3)
			sText.Printf("%s","3.Inside Plate Outside Iron");
#else 
		if(g_sysPara.intelligentConnect.m_iSingleAngleJointStyle==0)
			sText.Printf("%s","0.�����");
		else if(g_sysPara.intelligentConnect.m_iSingleAngleJointStyle==1)
			sText.Printf("%s","1.�������");
		if(g_sysPara.intelligentConnect.m_iSingleAngleJointStyle==2)
			sText.Printf("%s","2.�ڰ�������ְ�");
		if(g_sysPara.intelligentConnect.m_iSingleAngleJointStyle==3)
			sText.Printf("%s","3.�ڰ��ְ������");
#endif
	}
	else if(GetPropID("designSet.iTwoAngleJointStyle")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(g_sysPara.intelligentConnect.m_iTwoAngleJointStyle==0)
			sText.Printf("%s","0.Diagonal Combination Joint A");
		else if(g_sysPara.intelligentConnect.m_iTwoAngleJointStyle==1)
			sText.Printf("%s","1.Diagonal Combination Joint B");
		if(g_sysPara.intelligentConnect.m_iTwoAngleJointStyle==2)
			sText.Printf("%s","2.Diagonal Combination Joint C");
		if(g_sysPara.intelligentConnect.m_iTwoAngleJointStyle==3)
			sText.Printf("%s","3.Diagonal Combination Joint D");
#else 
		if(g_sysPara.intelligentConnect.m_iTwoAngleJointStyle==0)
			sText.Printf("%s","0.�Խ���Ͻ�ͷA");
		else if(g_sysPara.intelligentConnect.m_iTwoAngleJointStyle==1)
			sText.Printf("%s","1.�Խ���Ͻ�ͷB");
		if(g_sysPara.intelligentConnect.m_iTwoAngleJointStyle==2)
			sText.Printf("%s","2.�Խ���Ͻ�ͷC");
		if(g_sysPara.intelligentConnect.m_iTwoAngleJointStyle==3)
			sText.Printf("%s","3.�Խ���Ͻ�ͷD");
#endif
	}
	else if(GetPropID("designSet.iFourAngleJointStyle")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(g_sysPara.intelligentConnect.m_iFourAngleJointStyle==0)
			sText.Printf("%s","0.Cross Combine Joint A");
		else if(g_sysPara.intelligentConnect.m_iFourAngleJointStyle==1)
			sText.Printf("%s","1.Cross Combine Joint B");
#else 
		if(g_sysPara.intelligentConnect.m_iFourAngleJointStyle==0)
			sText.Printf("%s","0.ʮ����Ͻ�ͷA");
		else if(g_sysPara.intelligentConnect.m_iFourAngleJointStyle==1)
			sText.Printf("%s","1.ʮ����Ͻ�ͷB");
#endif
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}

BOOL CIntelligentConnectParaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//��ʼ��ͼ���б�
	m_ImageList.Create(69,62,ILC_COLORDDB|ILC_MASK,0,30);
	//
	m_arrBitemapInfo[0].bmp.LoadBitmap(IDB_BMP_L1LB1_MODAL);
	m_arrBitemapInfo[0].sName.Format("L_1LB1");
	m_arrBitemapInfo[1].bmp.LoadBitmap(IDB_BMP_L1LB2_MODAL);
	m_arrBitemapInfo[1].sName.Format("L_1LB2");
	m_arrBitemapInfo[2].bmp.LoadBitmap(IDB_BMP_L1LO1_MODAL);
	m_arrBitemapInfo[2].sName.Format("L_1LO1");
	m_arrBitemapInfo[3].bmp.LoadBitmap(IDB_BMP_L1LE1_MODAL);
	m_arrBitemapInfo[3].sName.Format("L_1LE1");
	m_arrBitemapInfo[4].bmp.LoadBitmap(IDB_BMP_L1LE2_MODAL);
	m_arrBitemapInfo[4].sName.Format("L_1LE2");
	m_arrBitemapInfo[5].bmp.LoadBitmap(IDB_BMP_L2LB1_MODAL);
	m_arrBitemapInfo[5].sName.Format("L_2LB1");
	m_arrBitemapInfo[6].bmp.LoadBitmap(IDB_BMP_L2LB2_MODAL);
	m_arrBitemapInfo[6].sName.Format("L_2LB2");
	m_arrBitemapInfo[7].bmp.LoadBitmap(IDB_BMP_L2LB3_MODAL);
	m_arrBitemapInfo[7].sName.Format("L_2LB3");
	m_arrBitemapInfo[8].bmp.LoadBitmap(IDB_BMP_L2LB4_MODAL);
	m_arrBitemapInfo[8].sName.Format("L_2LB4");
	m_arrBitemapInfo[9].bmp.LoadBitmap(IDB_BMP_L2LO1_MODAL);
	m_arrBitemapInfo[9].sName.Format("L_2LO1");
	m_arrBitemapInfo[10].bmp.LoadBitmap(IDB_BMP_L2LE1_MODAL);
	m_arrBitemapInfo[10].sName.Format("L_2LE1");
	m_arrBitemapInfo[11].bmp.LoadBitmap(IDB_BMP_L2LE2_MODAL);
	m_arrBitemapInfo[11].sName.Format("L_2LE2");
	m_arrBitemapInfo[12].bmp.LoadBitmap(IDB_BMP_L2LC1_MODAL);
	m_arrBitemapInfo[12].sName.Format("L_2LC1");
	m_arrBitemapInfo[13].bmp.LoadBitmap(IDB_BMP_L2LC2_MODAL);
	m_arrBitemapInfo[13].sName.Format("L_2LC2");
	m_arrBitemapInfo[14].bmp.LoadBitmap(IDB_BMP_L3LO1_MODAL);
	m_arrBitemapInfo[14].sName.Format("L_3LO1");
	m_arrBitemapInfo[15].bmp.LoadBitmap(IDB_BMP_L3LC1_MODAL);
	m_arrBitemapInfo[15].sName.Format("L_3LC1");
	m_arrBitemapInfo[16].bmp.LoadBitmap(IDB_BMP_L3LC2_MODAL);
	m_arrBitemapInfo[16].sName.Format("L_3LC2");
	m_arrBitemapInfo[17].bmp.LoadBitmap(IDB_BMP_L3LC3_MODAL);
	m_arrBitemapInfo[17].sName.Format("L_3LC3");
	m_arrBitemapInfo[18].bmp.LoadBitmap(IDB_BMP_L3LB1_MODAL);
	m_arrBitemapInfo[18].sName.Format("L_3LB1");
	m_arrBitemapInfo[19].bmp.LoadBitmap(IDB_BMP_L3LB2_MODAL);
	m_arrBitemapInfo[19].sName.Format("L_3LB2");
	m_arrBitemapInfo[20].bmp.LoadBitmap(IDB_BMP_XEND_MODAL);
	m_arrBitemapInfo[20].sName.Format("X_END");
	m_arrBitemapInfo[21].bmp.LoadBitmap(IDB_BMP_UEND_MODAL);
	m_arrBitemapInfo[21].sName.Format("U_END");
	m_arrBitemapInfo[22].bmp.LoadBitmap(IDB_BMP_CEND_MODAL);
	m_arrBitemapInfo[22].sName.Format("C_END");
	m_arrBitemapInfo[23].bmp.LoadBitmap(IDB_BMP_FLP_MODAL);
	m_arrBitemapInfo[23].sName.Format("FLP");
	m_arrBitemapInfo[24].bmp.LoadBitmap(IDB_BMP_FLD_MODAL);
	m_arrBitemapInfo[24].sName.Format("FLD");
	int i;
	for(i=0;i<25;i++)
		m_ImageList.Add(&m_arrBitemapInfo[i].bmp,RGB(0,128,128));
	m_listLj.SetImageList(&m_ImageList,TVSIL_NORMAL);
	m_listLj.SetTextColor(RGB(0,0,255));
	m_listLj.SetIconSpacing(CSize(92,15));
	//
	RefreshTabCtrl(0);
	//
	m_arrL1RayPole.RemoveAll();
	for(i=0;i<g_sysPara.m_arrL1RayPole.GetSize();i++)
		m_arrL1RayPole.Add(g_sysPara.m_arrL1RayPole[i]);
	m_arrL2RayPole.RemoveAll();
	for(i=0;i<g_sysPara.m_arrL2RayPole.GetSize();i++)
		m_arrL2RayPole.Add(g_sysPara.m_arrL2RayPole[i]);
	m_arrL3RayPole.RemoveAll();
	for(i=0;i<g_sysPara.m_arrL3RayPole.GetSize();i++)
		m_arrL3RayPole.Add(g_sysPara.m_arrL3RayPole[i]);
	m_arrLnRayPole.RemoveAll();
	for(i=0;i<g_sysPara.m_arrLnRayPole.GetSize();i++)
		m_arrLnRayPole.Add(g_sysPara.m_arrLnRayPole[i]);
	//
	m_arrInsertPlateType.RemoveAll();
	for(i=0;i<g_sysPara.m_arrInsertPlateType.GetSize();i++)
		m_arrInsertPlateType.Add(g_sysPara.m_arrInsertPlateType[i]);
	m_arrFlType.RemoveAll();
	for(i=0;i<g_sysPara.m_arrFlType.GetSize();i++)
		m_arrFlType.Add(g_sysPara.m_arrFlType[i]);
	RefreshListView(0);
	//������
	/*RECT rc,rcHeader,rcTop,rcBottom;
	GetClientRect(&rc);
	rc.bottom-=40;
	m_rcClient = rc;
	CWnd *pTopWnd = GetDlgItem(IDC_LIST_BOX);
	CWnd *pBtmWnd = GetDlgItem(IDC_E_PROP_HELP_STR);
	if(pTopWnd)
		pTopWnd->GetWindowRect(&rcTop);
	if(pBtmWnd)
		pBtmWnd->GetWindowRect(&rcBottom);
	ScreenToClient(&rcTop);
	ScreenToClient(&rcBottom);
	int btmHeight = rcBottom.bottom - rcBottom.top;
	rcHeader.left = rcTop.left = rcBottom.left = 0;
	rcHeader.right = rcTop.right = rcBottom.right = rc.right;
	rcHeader.top=0;
	rcBottom.bottom = rc.bottom;
	//���ݷ�������������λ��
	if(m_arrPropGroupLabel.GetSize()<=0)
		rcHeader.bottom=rcTop.top=0;
	else
		rcHeader.bottom = rcTop.top = 21;
	
	rcTop.bottom=rcBottom.bottom-btmHeight-m_nSplitterWidth-1;
	rcBottom.top=rcTop.bottom+m_nSplitterWidth+1;
	m_nOldHorzY = rcBottom.top-m_nSplitterWidth/2;
	
	RefreshTabCtrl(0);
	m_ctrlPropGroup.MoveWindow(&rcHeader);
	if(pTopWnd)
		pTopWnd->MoveWindow(&rcTop);
	if(pBtmWnd)
		pBtmWnd->MoveWindow(&rcBottom);
	
	m_bTracking = FALSE;
	m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZENS);
	m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);*/
	//��ʼ������ID
	CWnd *pBtmWnd = GetDlgItem(IDC_E_PROP_HELP_STR);
	m_propList.m_hPromptWnd = pBtmWnd->GetSafeHwnd();
	m_propList.SetDividerScale(0.55);
	InitSysSettingPropHashtable();
	//��ʾϵͳ����
	DisplaySystemSetting();
	return TRUE;  
}

void CIntelligentConnectParaDlg::OnOK() 
{
	UpdateData();
	g_sysPara.m_arrL1RayPole.RemoveAll();
	for(int i=0;i<m_arrL1RayPole.GetSize();i++)
		g_sysPara.m_arrL1RayPole.Add(m_arrL1RayPole[i]);
	g_sysPara.m_arrL2RayPole.RemoveAll();
	for(i=0;i<m_arrL2RayPole.GetSize();i++)
		g_sysPara.m_arrL2RayPole.Add(m_arrL2RayPole[i]);
	g_sysPara.m_arrL3RayPole.RemoveAll();
	for(i=0;i<m_arrL3RayPole.GetSize();i++)
		g_sysPara.m_arrL3RayPole.Add(m_arrL3RayPole[i]);
	g_sysPara.m_arrLnRayPole.RemoveAll();
	for(i=0;i<m_arrLnRayPole.GetSize();i++)
		g_sysPara.m_arrLnRayPole.Add(m_arrLnRayPole[i]);
	//
	g_sysPara.m_arrInsertPlateType.RemoveAll();
	for(i=0;i<m_arrInsertPlateType.GetSize();i++)
		g_sysPara.m_arrInsertPlateType.Add(m_arrInsertPlateType[i]);
	g_sysPara.m_arrFlType.RemoveAll();
	for(i=0;i<m_arrFlType.GetSize();i++)
		g_sysPara.m_arrFlType.Add(m_arrFlType[i]);
	CDialog::OnOK();
}

void CIntelligentConnectParaDlg::RefreshListView(int iCurSel)
{
	m_listLj.DeleteAllItems();
	m_listLj.SetIconSpacing(80,90);
	CStringArray m_arrRayPole;
	if (iCurSel==0)
	{
		for (int i=0;i<m_arrL1RayPole.GetSize();i++)
		{
			char sType[10];
			sprintf(sType,"%s",m_arrL1RayPole[i].GetBuffer(1));
			for (int j=0;j<25;j++)
			{
				if (m_arrBitemapInfo[j].sName.CompareNoCase(sType)==0)
					break;
			}
			m_listLj.InsertItem(i,m_arrL1RayPole[i].GetBuffer(1),j);
		}
		
	}
	else if (iCurSel==1)
	{
		for (int i=0;i<m_arrL2RayPole.GetSize();i++)
		{
			char sType[10];
			sprintf(sType,"%s",m_arrL2RayPole[i].GetBuffer(1));
			for (int j=0;j<25;j++)
			{
				if (m_arrBitemapInfo[j].sName.CompareNoCase(sType)==0)
					break;
			}
			m_listLj.InsertItem(i,m_arrL2RayPole[i].GetBuffer(1),j);
		}
		
	}
	else if (iCurSel==2)
	{
		for (int i=0;i<m_arrL3RayPole.GetSize();i++)
		{
			char sType[10];
			sprintf(sType,"%s",m_arrL3RayPole[i].GetBuffer(1));
			for (int j=0;j<25;j++)
			{
				if (m_arrBitemapInfo[j].sName.CompareNoCase(sType)==0)
					break;
			}
			m_listLj.InsertItem(i,m_arrL3RayPole[i].GetBuffer(1),j);
		}
		
	}
	else if (iCurSel==3)
	{
		for (int i=0;i<m_arrInsertPlateType.GetSize();i++)
		{
			char sType[10];
			sprintf(sType,"%s",m_arrInsertPlateType[i].GetBuffer(1));
			if (strcmp(sType,"X")==0)
				sprintf(sType,"%s","X_END");
			if (strcmp(sType,"U")==0)
				sprintf(sType,"%s","U_END");
			if (strcmp(sType,"C")==0)
				sprintf(sType,"%s","C_END");
			for (int j=0;j<25;j++)
			{
				if (m_arrBitemapInfo[j].sName.CompareNoCase(sType)==0)
					break;
			}
			m_listLj.InsertItem(i,sType,j);
		}
		
	}
	else if (iCurSel==4)
	{
		for (int i=0;i<m_arrFlType.GetSize();i++)
		{
			char sType[10];
			sprintf(sType,"%s",m_arrFlType[i].GetBuffer(1));
			for (int j=0;j<25;j++)
			{
				if (m_arrBitemapInfo[j].sName.CompareNoCase(sType)==0)
					break;
			}
			m_listLj.InsertItem(i,m_arrFlType[i].GetBuffer(1),j);
		}
		
	}
	UpdateData(FALSE);
}

void CIntelligentConnectParaDlg::OnItemchangedListLj(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POSITION pos = m_listLj.GetFirstSelectedItemPosition();
	int i = -1;
	if (pos)
		i = m_listLj.GetNextSelectedItem(pos);
	else 
		return;
	char sTemp[100];
	m_listLj.GetItemText(i,0,sTemp,100);
	*pResult = 0;
}

void CIntelligentConnectParaDlg::OnSelchangeStypeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iIndex = m_tabDesignType.GetCurSel();
	RefreshListView(iIndex);
	*pResult = 0;
}

void CIntelligentConnectParaDlg::OnEnddragList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNotify = (NM_LISTVIEW *)pNMHDR;  
	int* pBegin = (int*)pNotify->lParam;  
	int nEnd = pNotify->iItem; 
	CString strTmp;
	int i=0,j=0,k=0;
	CStringArray m_arrTemp;//�洢ѡ����
	CStringArray *pArr=NULL;//�洢��ק���
	BOOL bAhead = pBegin[0] > nEnd ? TRUE : FALSE;
	int iIndex = m_tabDesignType.GetCurSel(); 
	if(iIndex==0)
		pArr = &m_arrL1RayPole;
	else if(iIndex==1)
		pArr = &m_arrL2RayPole;
	else if(iIndex==2)
		pArr = &m_arrL3RayPole;
	else if(iIndex==3)
		pArr = &m_arrInsertPlateType;
	else if(iIndex==4)
		pArr = &m_arrFlType;
	for ( i = 0; i < MAX_DRAG_SIZE && pBegin[i] >= 0; i++ )  
	{  
		if (pBegin[i] == nEnd)  
			return;
		if(iIndex==0)
			m_arrTemp.Add(m_arrL1RayPole[pBegin[i]]);
		else if(iIndex==1)
			m_arrTemp.Add(m_arrL2RayPole[pBegin[i]]);
		else if(iIndex==2)
			m_arrTemp.Add(m_arrL3RayPole[pBegin[i]]);
		else if(iIndex==3)
			m_arrTemp.Add(m_arrInsertPlateType[pBegin[i]]);
		else if(iIndex==4)
			m_arrTemp.Add(m_arrFlType[pBegin[i]]);
	}
	if (m_arrTemp.GetSize()>0)
	{
		CStringArray arrRayPoleTemp;
		if (bAhead)
		{
			for(j=0;j<nEnd;j++)
				arrRayPoleTemp.Add((*pArr)[j]);
			for(j=0;j<m_arrTemp.GetSize();j++)
				arrRayPoleTemp.Add(m_arrTemp[j]);
			for(j=nEnd;j<pArr->GetSize();j++)
			{
				for(k=0;k<i;k++)
				{
					if (j==pBegin[k])
						break;
				}
				if(k==i)//�����Ų������϶�������
					arrRayPoleTemp.Add((*pArr)[j]);
			}	
		}	
		else
		{
			for(j=-1;j<nEnd;j++)
			{
				for(k=0;k<i;k++)
				{
					if (j+1==pBegin[k])
						break;
				}
				if(k==i)//�����Ų������϶�������
					arrRayPoleTemp.Add((*pArr)[j+1]);
			}
			for(j=0;j<m_arrTemp.GetSize();j++)
				arrRayPoleTemp.Add(m_arrTemp[j]);
			for(j=nEnd+1;j<pArr->GetSize();j++)
				arrRayPoleTemp.Add((*pArr)[j]);
		}
		pArr->RemoveAll();
		pArr->SetSize(arrRayPoleTemp.GetSize());
		for(i=0;i<arrRayPoleTemp.GetSize();i++)
			(*pArr)[i] = arrRayPoleTemp[i];
		RefreshListView(iIndex);
	}
	*pResult = 0;  
}
static DWORD myarray[] = {
   IDC_LIST_LJ, 1,
   IDC_LIST_BOX, 2,
   IDOK, -1,
   0,0
};

BOOL CIntelligentConnectParaDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

#ifdef AFX_TARG_ENU_ENGLISH
	CXhChar200 helpStr("%s::/Design Principle/Draw Layout/Connect Design/Intelligent Connection Design Parameters.htm",theApp.m_pszHelpFilePath);
#else 
	CXhChar200 helpStr("%s::/���ԭ��/��ͼ����/�������/����������Ʋ���.htm",theApp.m_pszHelpFilePath);
#endif
	HtmlHelp((DWORD_PTR)(char*)helpStr,HH_DISPLAY_TOPIC);
	//HtmlHelp(1,HH_DISPLAY_TEXT_POPUP);
  /* if (pHelpInfo->iContextType == HELPINFO_WINDOW)
   {
	   helpStr.Append("::/ctrlhlp.txt");
      return ::HtmlHelp(
         (HWND)pHelpInfo->hItemHandle,
         helpStr,
         HH_TP_HELP_WM_HELP,
         (DWORD)(LPVOID)myarray) 
         != NULL;
	}*/
	return TRUE;//CDialog::OnHelpInfo(pHelpInfo);HH_TP_HELP_WM_HELP
}
