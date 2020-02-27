// FoundationPropDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "FoundationPropDlg.h"
#include "afxdialogex.h"
#include "PropertyListOper.h"
struct ANCHOR_BOLT_S{WORD d,S;};
static ANCHOR_BOLT_S _localBoltSArr[12]={
	{20, 80},{24, 80},{27,100},{30,100},{36,120},{42,130},
	{45,140},{48,140},{52,140},{56,160},{60,160},{64,170}};
static int FindPreferAnchorBoltS(WORD wiBoltD)
{
	for(int i=0;i<12;i++)
	{
		if(_localBoltSArr[i].d==wiBoltD)
			return _localBoltSArr[i].S;
	}
	return 0;
}
static BOOL ModifyFoundationPropValue(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{	
	CFoundationPropDlg *pDlg=(CFoundationPropDlg*)pPropList->GetParent();
	if(pDlg==NULL||pDlg->m_pFoundation==NULL)
		return FALSE;
	//
	if(FOUNDATION_PARA::GetPropID("biBasePlateType")==pItem->m_idProp)
		pDlg->m_pFoundation->biBasePlateType=valueStr[0]-'0';
	else if(FOUNDATION_PARA::GetPropID("wiBasePlateThick")==pItem->m_idProp)
		pDlg->m_pFoundation->wiBasePlateThick=atoi(valueStr);
	else if(FOUNDATION_PARA::GetPropID("wiBasePlateWidth")==pItem->m_idProp)
		pDlg->m_pFoundation->wiBasePlateWidth=atoi(valueStr);
	else if(FOUNDATION_PARA::GetPropID("wiBasePlateHoleD")==pItem->m_idProp)
		pDlg->m_pFoundation->wiBasePlateHoleD=atoi(valueStr);
	else if(FOUNDATION_PARA::GetPropID("biLayoutType")==pItem->m_idProp)
	{
		if(valueStr[0]=='8')
			pDlg->m_pFoundation->biLayoutType=1;
		else
			pDlg->m_pFoundation->biLayoutType=0;
	}
	else if(FOUNDATION_PARA::GetPropID("wiBoltD")==pItem->m_idProp)
	{
		pDlg->m_pFoundation->wiBoltD=atoi(valueStr);
		const ANCHOR_BOLT* pAnchorBolt=ANCHOR_BOLT::FromBoltD(pDlg->m_pFoundation->wiBoltD);
		if(pAnchorBolt)
		{
			pDlg->m_pFoundation->wiBasePlateHoleD=pAnchorBolt->wiBasePlateHoleD;
			pPropList->SetItemPropValue(FOUNDATION_PARA::GetPropID("wiBasePlateHoleD"),CXhChar16(pAnchorBolt->wiBasePlateHoleD));
			WORD S=FindPreferAnchorBoltS(pAnchorBolt->d);
			if(S>0)
			{
				pDlg->m_pFoundation->wiBoltSapce=S;
				pPropList->SetItemPropValue(FOUNDATION_PARA::GetPropID("wiBoltSapce"),CXhChar16(S));
			}
		}
	}
	else if(FOUNDATION_PARA::GetPropID("wiBoltSapce")==pItem->m_idProp)
		pDlg->m_pFoundation->wiBoltSapce=atoi(valueStr);
	else if(FOUNDATION_PARA::GetPropID("wiFoundationWidth")==pItem->m_idProp)
		pDlg->m_pFoundation->wiFoundationWidth=atoi(valueStr);
	else if (FOUNDATION_PARA::GetPropID("ConnBoltCountOfPrimaryAngle") == pItem->m_idProp)
		pDlg->m_pFoundation->cnMainAngleBoltNum = atoi(valueStr);
	else if (FOUNDATION_PARA::GetPropID("ConnBoltRowsOfPrimaryAngle") == pItem->m_idProp)
		pDlg->m_pFoundation->cnRows = valueStr[0]-'0';
	else if (FOUNDATION_PARA::GetPropID("wiShoePlateThick") == pItem->m_idProp)
		pDlg->m_pFoundation->wiShoeThick = atoi(valueStr);
	else if (FOUNDATION_PARA::GetPropID("wiRibPlateThick") == pItem->m_idProp)
		pDlg->m_pFoundation->wiRibThick = atoi(valueStr);
	else if (FOUNDATION_PARA::GetPropID("wiRibPlateWidth") == pItem->m_idProp)
		pDlg->m_pFoundation->wRibWidth = atoi(valueStr);
	else if (FOUNDATION_PARA::GetPropID("wiRibPlateHeightOuter") == pItem->m_idProp)
		pDlg->m_pFoundation->wRibOuterHeight = atoi(valueStr);
	else if (FOUNDATION_PARA::GetPropID("wiRibPlateHeightInner") == pItem->m_idProp)
		pDlg->m_pFoundation->wRibInnerHeight = atoi(valueStr);
	else if (FOUNDATION_PARA::GetPropID("wiRibPlateMargin") == pItem->m_idProp)
		pDlg->m_pFoundation->wiRibMargin = atoi(valueStr);
	return true;
}

// CFoundationPropDlg 对话框

IMPLEMENT_DYNAMIC(CFoundationPropDlg, CDialog)

CFoundationPropDlg::CFoundationPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFoundationPropDlg::IDD, pParent)
{
	m_pFoundation=NULL;
}

CFoundationPropDlg::~CFoundationPropDlg()
{
}

void CFoundationPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_LIST, m_propList);
}


BEGIN_MESSAGE_MAP(CFoundationPropDlg, CDialog)
END_MESSAGE_MAP()


BOOL CFoundationPropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_propList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.m_iPropGroup=0;
	m_propList.SetDividerScale(0.5);
	//
	FOUNDATION_PARA::InitPropHashtable();
	DisplayFoundationPropList();
	UpdateData(FALSE);
	return TRUE;
}

void CFoundationPropDlg::OnOK() 
{
	CDialog::OnOK();
}
void CFoundationPropDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CFoundationPropDlg::DisplayFoundationPropList()
{
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pSonPropItem=NULL,*pRootItem=m_propList.GetRootItem();
	//设置回调函数
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(ModifyFoundationPropValue);
	FOUNDATION_PARA *pFoundation=m_pFoundation;
	if(pFoundation==NULL)
		pFoundation=&defaultPara;
	CPropertyListOper<FOUNDATION_PARA> oper(&m_propList,pFoundation);
	pGroupItem=oper.InsertPropItem(pRootItem,"FoundationPara");
	pGroupItem->m_bHideChildren=FALSE;
	oper.InsertEditPropItem(pGroupItem,"wiFoundationWidth");
	oper.InsertCmbListPropItem(pGroupItem,"biLayoutType","4地脚螺栓|8地脚螺栓");
	oper.InsertCmbListPropItem(pGroupItem,"wiBoltD");
	oper.InsertEditPropItem(pGroupItem,"wiBoltSapce");
	pGroupItem=oper.InsertPropItem(pRootItem,"PrimaryAngle");
	pGroupItem->m_bHideChildren=FALSE;
	oper.InsertEditPropItem(pGroupItem,"ConnBoltCountOfPrimaryAngle");
	oper.InsertCmbListPropItem(pGroupItem,"ConnBoltRowsOfPrimaryAngle","1.单排|2.双排");
	pGroupItem=oper.InsertPropItem(pRootItem,"BasePlate");
	pGroupItem->m_bHideChildren=FALSE;
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"biBasePlateType","0.交于上平面|1.交于下平面|2.交于中间|");
	oper.InsertEditPropItem(pPropItem,"wiBasePlateHoleD");
	oper.InsertEditPropItem(pPropItem,"wiBasePlateThick");
	oper.InsertEditPropItem(pPropItem,"wiBasePlateWidth");
	pPropItem=oper.InsertEditPropItem(pGroupItem,"wiShoePlateThick");
	pPropItem = oper.InsertEditPropItem(pGroupItem, "wiRibPlateThick");
	oper.InsertEditPropItem(pPropItem,"wiRibPlateWidth");
	oper.InsertEditPropItem(pPropItem,"wiRibPlateHeightOuter");
	oper.InsertEditPropItem(pPropItem,"wiRibPlateHeightInner");
	oper.InsertEditPropItem(pPropItem,"wiRibPlateMargin");
	m_propList.Redraw();
}