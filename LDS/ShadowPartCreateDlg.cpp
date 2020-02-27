//<LOCALE_TRANSLATE Confirm by hxr/>
// ShadowPartCreateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "ShadowPartCreateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShadowPartCreateDlg dialog


const DWORD HASHTABLESIZE = 500;
CStringKeyHashTable<long> CShadowPartCreateDlg::propHashtable;
CShadowPartCreateDlg::CShadowPartCreateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShadowPartCreateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShadowPartCreateDlg)
	m_iShadowStyle = 0;
	//}}AFX_DATA_INIT
}


void CShadowPartCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShadowPartCreateDlg)
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_CBIndex(pDX, IDC_CMB_SHADOW_STYLE, m_iShadowStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShadowPartCreateDlg, CDialog)
	//{{AFX_MSG_MAP(CShadowPartCreateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShadowPartCreateDlg message handlers
static BOOL ModifyShadowPartProperty(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CShadowPartCreateDlg *pShadowDlg=(CShadowPartCreateDlg*)pPropList->GetParent();
	return TRUE;
}
static BOOL ButtonClickShadowPart(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	return TRUE;
}
BOOL CShadowPartCreateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.5);
	m_propList.SetModifyValueFunc(ModifyShadowPartProperty);
	m_propList.SetButtonClickFunc(ButtonClickShadowPart);
	InitPropHashtable();
	UpdatePropList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//属性管理
long CShadowPartCreateDlg::GetPropID(char* propStr)	//得到属性ID
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}
void CShadowPartCreateDlg::InitPropHashtable()
{
	int id = 1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	propHashtable.SetValueAt("hVicePart",id++);
	propHashtable.SetValueAt("hCrossPart",id++);
	propHashtable.SetValueAt("norm_wing",id++);
	propHashtable.SetValueAt("direction",id++);
}
//根据属性ID得到属性值
int CShadowPartCreateDlg::GetPropValueStr(long id, char *valueStr)
{
	return TRUE;
}
void CShadowPartCreateDlg::UpdatePropList()
{
	m_propList.CleanTree();
	m_propList.DeleteItemByPropId(GetPropID("hVicePart"));
	CPropTreeItem *pPropItem,*pSonItem,*pRoot=m_propList.GetRootItem();
	char sText[MAX_TEMP_CHAR_100+1]="";
	CItemInfo* lpInfo = new CItemInfo();
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "config word";
	lpInfo->m_strPropHelp = "specified direction";
#else
	lpInfo->m_strPropName = "配材号";
	lpInfo->m_strPropHelp = "指定方向";
#endif
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("VectorType");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "datum point position";
	lpInfo->m_strPropHelp = "specified direction";
#else
	lpInfo->m_strPropName = "基点位置";
	lpInfo->m_strPropHelp = "指定方向";
#endif
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("VectorType");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "target position";
	lpInfo->m_strPropHelp = "specified direction";
#else
	lpInfo->m_strPropName = "目标位置";
	lpInfo->m_strPropHelp = "指定方向";
#endif
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("VectorType");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "rotation axis";
	lpInfo->m_strPropHelp = "specified direction";
#else
	lpInfo->m_strPropName = "旋转轴";
	lpInfo->m_strPropHelp = "指定方向";
#endif
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("VectorType");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "datum point";
	lpInfo->m_strPropHelp = "rotation axis datum point";
#else
	lpInfo->m_strPropName = "基点";
	lpInfo->m_strPropHelp = "旋转轴基点";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp =GetPropID("VectorType");
	if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = sText;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "direction";
	lpInfo->m_strPropHelp = "rotation axis direction";
#else
	lpInfo->m_strPropName = "方向";
	lpInfo->m_strPropHelp = "旋转轴方向";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp =GetPropID("VectorType");
	if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = sText;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "rotation angle";
	lpInfo->m_strPropHelp = "specified direction";
#else
	lpInfo->m_strPropName = "旋转角度";
	lpInfo->m_strPropHelp = "指定方向";
#endif
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("VectorType");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "mirror plane";
	lpInfo->m_strPropHelp = "specified direction";
#else
	lpInfo->m_strPropName = "镜像平面";
	lpInfo->m_strPropHelp = "指定方向";
#endif
	pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("VectorType");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "datum point";
	lpInfo->m_strPropHelp = "specified direction";
#else
	lpInfo->m_strPropName = "基点";
	lpInfo->m_strPropHelp = "指定方向";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp =GetPropID("VectorType");
	if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = sText;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "normal line";
	lpInfo->m_strPropHelp = "specified direction";
#else
	lpInfo->m_strPropName = "法线";
	lpInfo->m_strPropHelp = "指定方向";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
	pSonItem->m_idProp =GetPropID("VectorType");
	if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue = sText;
	m_propList.Redraw();
}
