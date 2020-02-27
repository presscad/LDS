// ActiveHeightLegsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "ActiveHeightLegsDlg.h"
#include "afxdialogex.h"


// CActiveHeightLegsDlg 对话框

IMPLEMENT_DYNAMIC(CActiveHeightLegsDlg, CDialogEx)

CActiveHeightLegsDlg::CActiveHeightLegsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CActiveHeightLegsDlg::IDD, pParent)
{

	m_iOrderOfLegQuadA2 = 0;
	m_iOrderOfLegQuadB4 = 0;
	m_iOrderOfLegQuadC3 = 0;
	m_iOrderOfLegQuadD1 = 0;
	m_sHeightName = _T("");
	m_sLegItems = _T("");
	m_sLegLevelDiff = _T("");
	m_siInitLegHeight=0;
}

CActiveHeightLegsDlg::~CActiveHeightLegsDlg()
{
}

void CActiveHeightLegsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_S_MASTER_LEG_LABEL, m_ctrlMasterLegLabel);
	DDX_CBIndex(pDX, IDC_CMB_LEG_QUAD_A2, m_iOrderOfLegQuadA2);
	DDX_CBIndex(pDX, IDC_CMB_LEG_QUAD_B4, m_iOrderOfLegQuadB4);
	DDX_CBIndex(pDX, IDC_CMB_LEG_QUAD_C3, m_iOrderOfLegQuadC3);
	DDX_CBIndex(pDX, IDC_CMB_LEG_QUAD_D1, m_iOrderOfLegQuadD1);
	DDX_Text(pDX, IDC_S_HEIGHT_NAME, m_sHeightName);
	DDX_Text(pDX, IDC_S_LEG_ITEMS, m_sLegItems);
	DDX_Text(pDX, IDC_S_LEG_LEVEL_DIFF, m_sLegLevelDiff);
}


BEGIN_MESSAGE_MAP(CActiveHeightLegsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ACTIVATE, &CActiveHeightLegsDlg::OnBnActivate)
	ON_CBN_SELCHANGE(IDC_CMB_LEG_QUAD_A2, &CActiveHeightLegsDlg::OnCbnSelchangeCmbLegQuadA2)
END_MESSAGE_MAP()


// CActiveHeightLegsDlg 消息处理程序


BOOL CActiveHeightLegsDlg::OnInitDialog()
{
	m_ctrlMasterLegLabel.SetTextColor(RGB(255,0,0));
	CClientDC dc(this);
	m_ctrlMasterLegLabel.SetBackgroundColour(dc.GetBkColor());
	CDialogEx::OnInitDialog();
	DWORD dwTmpValue=m_dwIncLegItemFlag;
	CString szLegItem;
	CComboBox* pCMBQuad1=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_D1);
	CComboBox* pCMBQuad2=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_A2);
	CComboBox* pCMBQuad3=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_C3);
	CComboBox* pCMBQuad4=(CComboBox*)GetDlgItem(IDC_CMB_LEG_QUAD_B4);
	pCMBQuad1->ResetContent();
	pCMBQuad2->ResetContent();
	pCMBQuad3->ResetContent();
	pCMBQuad4->ResetContent();
	CXhChar16 szLegItems;
	CFGWORD cfgword;
	cfgword.AddBodyLegs(1,this->m_dwIncLegItemFlag);
	cfgword.GetLegScopeStr(szLegItems);
	m_sLegItems.Format("接腿：%s",(char*)szLegItems);

	for(int i=0;i<32;i++)
	{
		szLegItem="";
		if(dwTmpValue%2==0)
			continue;
		dwTmpValue>>=1;
		szLegItem.AppendChar('A'+i);
		double heightdiff=(m_siInitLegHeight+i*this->m_siLegLevelDiff)*0.001;
		if(heightdiff!=0)
			szLegItem.AppendFormat("-%3.1fm",heightdiff);
		else
			szLegItem.AppendFormat("- 0 m");
		pCMBQuad1->AddString(szLegItem);
		pCMBQuad2->AddString(szLegItem);
		pCMBQuad3->AddString(szLegItem);
		pCMBQuad4->AddString(szLegItem);
	}
	this->m_sLegLevelDiff.Format("级差：%gm",0.001*m_siLegLevelDiff);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CActiveHeightLegsDlg::OnBnActivate()
{
	OnOK();
}

void CActiveHeightLegsDlg::OnCbnSelchangeCmbLegQuadA2()
{
	UpdateData();
	m_iOrderOfLegQuadB4=m_iOrderOfLegQuadC3=m_iOrderOfLegQuadD1=m_iOrderOfLegQuadA2;
	UpdateData(FALSE);
}
