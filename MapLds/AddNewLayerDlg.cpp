//<LOCALE_TRANSLATE BY hxr /> 2015-04-24
// AddNewLayerDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "maplds.h"
#include "AddNewLayerDlg.h"
//#include　"SystemSettingDlg.h"

#include "SysPara.h"
#include "XhLdsLm.h"
#include "XhCharString.h"
#include "MenuFunc.h"
#include "RxTools.h"
#include "dbmain.h"
#include "SpecialSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddNewLayerDlg dialog


CAddNewLayerDlg::CAddNewLayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddNewLayerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddNewLayerDlg)
	m_sLayerName = _T("");
	//}}AFX_DATA_INIT
	m_pColorCmbBox = NULL;
	m_pLineTypeCmbBox = NULL;
	m_xLayer.lineWeight=AcDb::kLnWt013;
}


void CAddNewLayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddNewLayerDlg)
	DDX_Control(pDX, IDC_CMB_COLOR, m_cmbColor);
	DDX_Text(pDX, IDC_E_LAYER_NAME, m_sLayerName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddNewLayerDlg, CDialog)
	//{{AFX_MSG_MAP(CAddNewLayerDlg)
//	ON_BN_CLICKED(IDOK, OnAddNewLayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddNewLayerDlg message handlers

BOOL CAddNewLayerDlg::OnInitDialog() 
{
	m_pLineTypeCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_LINETYPE);	
	m_pLineWeightCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_LINE_WEIGHT);
	m_pColorCmbBox = (CColorSelectComboBox*)GetDlgItem(IDC_CMB_COLOR);
	ASSERT(m_pLineTypeCmbBox);
	ASSERT(m_pColorCmbBox);
	ASSERT(m_pLineWeightCmbBox);
	//
	m_pColorCmbBox->ResetContent();
	m_pColorCmbBox->InitBox(RGB(255,0,0));
	//
	m_pLineTypeCmbBox->AddString("CONTINUOUS");
	m_pLineTypeCmbBox->AddString("HIDDEN");
	m_pLineTypeCmbBox->AddString("DASHDOT2X");
	m_pLineTypeCmbBox->AddString("DIVIDE");
	m_pLineTypeCmbBox->SetCurSel(0);
	//
	m_pLineWeightCmbBox->AddString("0.00mm");
	m_pLineWeightCmbBox->AddString("0.05mm");
    m_pLineWeightCmbBox->AddString("0.09mm");
	m_pLineWeightCmbBox->AddString("0.13mm");
	m_pLineWeightCmbBox->AddString("0.15mm");
    m_pLineWeightCmbBox->AddString("0.18mm");
	m_pLineWeightCmbBox->AddString("0.20mm");
	m_pLineWeightCmbBox->AddString("0.25mm");
	m_pLineWeightCmbBox->AddString("0.30mm");
	m_pLineWeightCmbBox->AddString("0.35mm");
	m_pLineWeightCmbBox->AddString("0.40mm");
	m_pLineWeightCmbBox->AddString("0.50mm");
	m_pLineWeightCmbBox->AddString("0.53mm");
	m_pLineWeightCmbBox->AddString("0.60mm");
	m_pLineWeightCmbBox->AddString("0.70mm");
	m_pLineWeightCmbBox->AddString("0.80mm");
	m_pLineWeightCmbBox->AddString("0.90mm");
	m_pLineWeightCmbBox->AddString("1.00mm");
	m_pLineWeightCmbBox->AddString("1.06mm");
	m_pLineWeightCmbBox->AddString("1.20mm");
	m_pLineWeightCmbBox->AddString("1.40mm");
	m_pLineWeightCmbBox->AddString("1.58mm");
	m_pLineWeightCmbBox->AddString("2.00mm");
	m_pLineWeightCmbBox->AddString("2.11mm");
	m_pLineWeightCmbBox->SetCurSel(7);   //0.25mm
	CDialog::OnInitDialog();
	return TRUE;
}
void CAddNewLayerDlg::OnOK()
{
	UpdateData();
	int iCurSel = m_pColorCmbBox->GetCurSel();//得到当前选中索引
	if(iCurSel>-1)
	{	//得到当前颜色
		long curColor = (DWORD)m_pColorCmbBox->GetItemData(iCurSel);
		m_xLayer.colorIndex = GetNearestACI(curColor);		
	}
	//线型
	iCurSel=m_pLineTypeCmbBox->GetCurSel();
	if(iCurSel>-1)
		m_pLineTypeCmbBox->GetLBText(iCurSel,m_xLayer.lineTypeName);
	//图层名
	m_xLayer.layerName.Copy(m_sLayerName);
	//线宽
	iCurSel=m_pLineWeightCmbBox->GetCurSel();
	if(iCurSel>-1)
	{
		CString str;
		m_pLineWeightCmbBox->GetLBText(iCurSel,str);
		str.Replace("mm","");
		int nLineWeight=(int)(atof(str)*100);
		m_xLayer.lineWeight=(AcDb::LineWeight)nLineWeight;
	}
	//判断哈希表中是否存在相同的图层名
	if(LayerTable::layerHashList.GetValue(m_xLayer.layerName))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Exist same layer name already,please input again!");
#else
		AfxMessageBox("已存在相同名称的图层名,请重新输入!");
#endif
		GetDlgItem(IDC_E_LAYER_NAME)->SetFocus();
		return;
	}
	CDialog::OnOK();
}