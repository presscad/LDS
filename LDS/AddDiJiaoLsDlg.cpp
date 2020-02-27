// AddDiJiaoLsDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "AddDiJiaoLsDlg.h"
#include "math.h"
#include "f_ent.h"
#include "XhCharString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddDiJiaoLsDlg dialog


CAddDiJiaoLsDlg::CAddDiJiaoLsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddDiJiaoLsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddDiJiaoLsDlg)
	m_sLsGuiGe = _T("20");
	m_fHoleD = 25.0;
	m_fX = 80.0;
	m_fY = 80.0;
	m_nMirN = 4;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCaption = _T("Add bolt hole");
#else
	m_sCaption = _T("添加螺栓孔");
#endif
	m_bAddLs=TRUE;
	m_bEnableRayAngleMode=false;
	//}}AFX_DATA_INIT
}


void CAddDiJiaoLsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddDiJiaoLsDlg)
	DDX_CBString(pDX, IDC_CMB_LS_GUIGE, m_sLsGuiGe);
	DDX_Text(pDX, IDC_E_HOLE_D, m_fHoleD);
	DDX_Text(pDX, IDC_E_X, m_fX);
	DDX_Text(pDX, IDC_E_Y, m_fY);
	DDX_Text(pDX, IDC_E_MIR_N, m_nMirN);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddDiJiaoLsDlg, CDialog)
	//{{AFX_MSG_MAP(CAddDiJiaoLsDlg)
	ON_CBN_SELCHANGE(IDC_CMB_LS_GUIGE, OnSelchangeCmbLsGuige)
	ON_CBN_EDITCHANGE(IDC_CMB_LS_GUIGE, OnEditchangeCmbLsGuige)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddDiJiaoLsDlg message handlers

void CAddDiJiaoLsDlg::OnSelchangeCmbLsGuige() 
{
	CComboBox *pCMB = (	CComboBox*)GetDlgItem(IDC_CMB_LS_GUIGE);
	int ii=pCMB->GetCurSel();
	pCMB->GetLBText(ii,m_sLsGuiGe);
	if(m_sLsGuiGe.GetLength()>3)
	{
#ifdef AFX_TARG_ENU_ENGLISH
#if defined(__LDS_FILE_)||defined(__LDS_)
		MessageBox("Unreasonable bolt specification!","LDS",MB_OK);
#elif defined(__LMA_FILE_)||defined(__LMA_)
		MessageBox("Unreasonable bolt specification!","LMA",MB_OK);
#elif defined(__TMA_FILE_)||defined(__TMA_)
		MessageBox("Unreasonable bolt specification!","TMA",MB_OK);
#endif
#else
#if defined(__LDS_FILE_)||defined(__LDS_)
		MessageBox("不合理的螺栓规格!","LDS",MB_OK);
#elif defined(__LMA_FILE_)||defined(__LMA_)
		MessageBox("不合理的螺栓规格!","LMA",MB_OK);
#elif defined(__TMA_FILE_)||defined(__TMA_)
		MessageBox("不合理的螺栓规格!","TMA",MB_OK);
#endif
#endif
		UpdateData();
		UpdateData(FALSE);
	}
	else
	{
		CXhChar16 ss;
		ss.Printf("%s",m_sLsGuiGe);

		if(ss.Equal("12"))
		{
			m_fHoleD = 13.5;
			m_nDefaultS=80;
		}
		else if(ss.Equal("16"))
		{
			m_fHoleD = 17.5;
			m_nDefaultS=80;
		}
		else if(ss.Equal("20"))
		{
			m_fHoleD = 25;
			m_nDefaultS=80;
		}
		else if(ss.Equal("24"))
		{
			m_fHoleD = 30;
			m_nDefaultS=80;
		}
		else if(ss.Equal("27"))
		{
			m_fHoleD = 35;
			m_nDefaultS=100;
		}
		else if(ss.Equal("30"))
		{
			m_fHoleD = 40;
			m_nDefaultS=100;
		}
		else if(ss.Equal("36"))
		{
			m_fHoleD = 45;
			m_nDefaultS=120;
		}
		else if(ss.Equal("42"))
		{
			m_fHoleD = 55;
			m_nDefaultS=130;
		}
		else if(ss.Equal("45"))
		{
			m_fHoleD = 60;
			m_nDefaultS=140;
		}
		else if(ss.Equal("48"))
		{
			m_fHoleD = 60;
			m_nDefaultS=140;
		}
		else if(ss.Equal("52"))
		{
			m_fHoleD = 65;
			m_nDefaultS=140;
		}
		else if(ss.Equal("56"))
		{
			m_fHoleD = 70;
			m_nDefaultS=160;
		}
		else if(ss.Equal("60"))
		{
			m_fHoleD = 75;
			m_nDefaultS=160;
		}
		else if(ss.Equal("64"))
		{
			m_fHoleD = 80;
			m_nDefaultS=170;
		}
		else if(ss.Equal("68"))
		{
			m_fHoleD = 85;
			m_nDefaultS=185;
		}
		else if(ss.Equal("72"))
		{
			m_fHoleD = 90;
			m_nDefaultS=190;
		}
		m_fX=m_nDefaultS;
		m_fY=m_nDefaultS;
		UpdateData(FALSE);
	}
}

void CAddDiJiaoLsDlg::OnEditchangeCmbLsGuige() 
{
	CComboBox *pCMB = (	CComboBox*)GetDlgItem(IDC_CMB_LS_GUIGE);
	CString ss;
	pCMB->GetWindowText(ss);
	if(ss.GetLength()>3)
	{
#ifdef AFX_TARG_ENU_ENGLISH
#if defined(__LDS_FILE_)||defined(__LDS_)
		MessageBox("Unreasonable bolt specification!","LDS",MB_OK);
#elif defined(__LMA_FILE_)||defined(__LMA_)
		MessageBox("Unreasonable bolt specification!","LMA",MB_OK);
#elif defined(__TMA_FILE_)||defined(__TMA_)
		MessageBox("Unreasonable bolt specification!","TMA",MB_OK);
#endif
#else
#if defined(__LDS_FILE_)||defined(__LDS_)
		MessageBox("不合理的螺栓规格!","LDS",MB_OK);
#elif defined(__LMA_FILE_)||defined(__LMA_)
		MessageBox("不合理的螺栓规格!","LMA",MB_OK);
#elif defined(__TMA_FILE_)||defined(__TMA_)
		MessageBox("不合理的螺栓规格!","TMA",MB_OK);
#endif
#endif
		UpdateData(FALSE);
	}
}

BOOL CAddDiJiaoLsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_E_MIR_N)->EnableWindow(m_bAddLs);
	if(m_bEnableRayAngleMode)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_1ST_PARAM)->SetWindowText("Arrange circle radius:");
		GetDlgItem(IDC_S_2ND_PARAM)->SetWindowText("Initial ray angle,°:");
#else
		GetDlgItem(IDC_S_1ST_PARAM)->SetWindowText("布置圆半径:");
		GetDlgItem(IDC_S_2ND_PARAM)->SetWindowText("初始射线角,°:");
#endif
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_1ST_PARAM)->SetWindowText("X coordinates:");
		GetDlgItem(IDC_S_2ND_PARAM)->SetWindowText("Y coordinates:");
#else
		GetDlgItem(IDC_S_1ST_PARAM)->SetWindowText("X坐标:");
		GetDlgItem(IDC_S_2ND_PARAM)->SetWindowText("Y坐标:");
#endif
	}
	SetWindowText(m_sCaption);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
