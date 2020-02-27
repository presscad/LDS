// SpecialSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SpecialSettingDlg.h"
#include "SysPara.h"
#include "f_ent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpecialSettingDlg dialog


CSpecialSettingDlg::CSpecialSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecialSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecialSettingDlg)
	//}}AFX_DATA_INIT
	m_arrIsCanUse[0]=FALSE;
	m_arrIsCanUse[1]=FALSE;
	m_arrIsCanUse[2]=FALSE;
	m_arrIsCanUse[3]=FALSE;
	m_arrIsCanUse[4]=FALSE;
	m_arrValue[0]=0;
	m_arrValue[1]=0;
	m_arrValue[2]=0;
	m_arrValue[3]=0;
	m_arrValue[4]=0;
	txt_size0_wtoh1=0;
	strcpy(m_sProp,"");
}


void CSpecialSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecialSettingDlg)
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHK_SUM_MAP, m_arrIsCanUse[0]);
	DDX_Check(pDX, IDC_CHK_CHIEF_MAP, m_arrIsCanUse[1]);
	DDX_Check(pDX, IDC_CHK_STRU_MAP, m_arrIsCanUse[2]);
	DDX_Check(pDX, IDC_CHK_PART_MAP, m_arrIsCanUse[3]);
	DDX_Check(pDX, IDC_CHK_WELD_MAP, m_arrIsCanUse[4]);
	DDX_Text(pDX, IDC_E_SUM_VALUE, m_arrValue[0]);
	DDX_Text(pDX, IDC_E_CHIEF_VALUE, m_arrValue[1]);
	DDX_Text(pDX, IDC_E_STRU_VALUE, m_arrValue[2]);
	DDX_Text(pDX, IDC_E_PART_VALUE, m_arrValue[3]);
	DDX_Text(pDX, IDC_E_WELD_VALUE, m_arrValue[4]);
}


BEGIN_MESSAGE_MAP(CSpecialSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CSpecialSettingDlg)
	ON_BN_CLICKED(IDC_CHK_SUM_MAP, OnSumMap)
	ON_BN_CLICKED(IDC_CHK_STRU_MAP, OnStruMap)
	ON_BN_CLICKED(IDC_CHK_PART_MAP, OnPartMap)
	ON_BN_CLICKED(IDC_CHK_CHIEF_MAP, OnChiefMap)
	ON_BN_CLICKED(IDC_CHK_WELD_MAP, OnWeldMap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecialSettingDlg message handlers

BOOL CSpecialSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	double fDatumValue = 0;
	if(txt_size0_wtoh1==0)		//字高
		fDatumValue = sys.fonts.TextSize(m_sProp);
	else if(txt_size0_wtoh1==1)	//字体宽高比
		fDatumValue = sys.fonts.TextWtoH(m_sProp);
	for(int j=1;j<6;j++)
	{
		double fValue=0;
		if(txt_size0_wtoh1==0)
			fValue = sys.fonts.TextSize(m_sProp,j);
		else
			fValue = sys.fonts.TextWtoH(m_sProp,j);
		if(fabs(fValue)<EPS||fabs(fValue-fDatumValue)<EPS)
			continue;
		m_arrIsCanUse[j-1]=TRUE;
		m_arrValue[j-1]=fValue;
	}
	GetDlgItem(IDC_E_SUM_VALUE)->EnableWindow(m_arrIsCanUse[0]);
	GetDlgItem(IDC_E_CHIEF_VALUE)->EnableWindow(m_arrIsCanUse[1]);
	GetDlgItem(IDC_E_STRU_VALUE)->EnableWindow(m_arrIsCanUse[2]);
	GetDlgItem(IDC_E_PART_VALUE)->EnableWindow(m_arrIsCanUse[3]);
	GetDlgItem(IDC_E_WELD_VALUE)->EnableWindow(m_arrIsCanUse[4]);
	UpdateData(FALSE);
	return TRUE;  
}

void CSpecialSettingDlg::OnSumMap() 
{
	m_arrIsCanUse[0]=!m_arrIsCanUse[0];
	CEdit *pEdit=(CEdit*)GetDlgItem(IDC_E_SUM_VALUE);
	pEdit->EnableWindow(m_arrIsCanUse[0]);
	if(m_arrIsCanUse[0])
	{	//选中属性值
		pEdit->SetFocus();
		int len = pEdit->GetWindowTextLength();
		pEdit->SetSel(0, len);
	}
}

void CSpecialSettingDlg::OnChiefMap() 
{
	m_arrIsCanUse[1]=!m_arrIsCanUse[1];
	CEdit *pEdit=(CEdit*)GetDlgItem(IDC_E_CHIEF_VALUE);
	pEdit->EnableWindow(m_arrIsCanUse[1]);
	if(m_arrIsCanUse[1])
	{	//选中属性值
		pEdit->SetFocus();
		int len = pEdit->GetWindowTextLength();
		pEdit->SetSel(0, len);
	}
}

void CSpecialSettingDlg::OnStruMap() 
{
	m_arrIsCanUse[2]=!m_arrIsCanUse[2];
	CEdit *pEdit=(CEdit*)GetDlgItem(IDC_E_STRU_VALUE);
	pEdit->EnableWindow(m_arrIsCanUse[2]);
	if(m_arrIsCanUse[2])
	{	//选中属性值
		pEdit->SetFocus();
		int len = pEdit->GetWindowTextLength();
		pEdit->SetSel(0, len);
	}
}

void CSpecialSettingDlg::OnPartMap() 
{
	m_arrIsCanUse[3]=!m_arrIsCanUse[3];
	CEdit *pEdit=(CEdit*)GetDlgItem(IDC_E_PART_VALUE);
	pEdit->EnableWindow(m_arrIsCanUse[3]);
	if(m_arrIsCanUse[3])
	{	//选中属性值
		pEdit->SetFocus();
		int len = pEdit->GetWindowTextLength();
		pEdit->SetSel(0, len);
	}
}

void CSpecialSettingDlg::OnWeldMap() 
{
	m_arrIsCanUse[4]=!m_arrIsCanUse[4];
	CEdit *pEdit=(CEdit*)GetDlgItem(IDC_E_WELD_VALUE);
	pEdit->EnableWindow(m_arrIsCanUse[4]);
	if(m_arrIsCanUse[4])
	{	//选中属性值
		pEdit->SetFocus();
		int len = pEdit->GetWindowTextLength();
		pEdit->SetSel(0, len);
	}
}
