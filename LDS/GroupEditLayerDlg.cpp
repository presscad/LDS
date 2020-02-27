// GroupEditLayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "GroupEditLayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupEditLayerDlg dialog


CGroupEditLayerDlg::CGroupEditLayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGroupEditLayerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGroupEditLayerDlg)
	m_bFirstChar = FALSE;
	m_bSecChar = FALSE;
	m_bThirdChar = FALSE;
	m_iFirstChar = 0;
	m_iSecChar = 0;
	m_iThirdChar = 0;
	//}}AFX_DATA_INIT
	strcpy(layer,"SXQ");
}


void CGroupEditLayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupEditLayerDlg)
	DDX_Check(pDX, IDC_CHK_FIRST_CHAR, m_bFirstChar);
	DDX_Check(pDX, IDC_CHK_SEC_CHAR, m_bSecChar);
	DDX_Check(pDX, IDC_CHK_THIRD_CHAR, m_bThirdChar);
	DDX_CBIndex(pDX, IDC_CMB_FIRST_CHAR, m_iFirstChar);
	DDX_CBIndex(pDX, IDC_CMB_SEC_CHAR, m_iSecChar);
	DDX_CBIndex(pDX, IDC_CMB_THIRD_CHAR, m_iThirdChar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupEditLayerDlg, CDialog)
	//{{AFX_MSG_MAP(CGroupEditLayerDlg)
	ON_BN_CLICKED(IDC_CHK_FIRST_CHAR, OnChkFirstChar)
	ON_BN_CLICKED(IDC_CHK_SEC_CHAR, OnChkSecChar)
	ON_BN_CLICKED(IDC_CHK_THIRD_CHAR, OnChkThirdChar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupEditLayerDlg message handlers

BOOL CGroupEditLayerDlg::OnInitDialog() 
{
	switch(toupper(layer[0]))
	{
	case 'T':
		m_iFirstChar=0;
		break;
	case 'S':
		m_iFirstChar=1;
		break;
	case 'L':
		m_iFirstChar=2;
		break;
	default:
		m_iFirstChar=0;
		break;
	}
	switch(toupper(layer[1]))
	{
	case 'Z':
		m_iSecChar=0;
		break;
	case 'X':
		m_iSecChar=1;
		break;
	case 'H':
		m_iSecChar=2;
		break;
	case 'B':
		m_iSecChar=3;
		break;
	default:
		m_iSecChar=0;
		break;
	}
	switch(toupper(layer[2]))
	{
	case 'Q':
		m_iThirdChar=0;
		break;
	case 'H':
		m_iThirdChar=1;
		break;
	case 'Z':
		m_iThirdChar=2;
		break;
	case 'Y':
		m_iThirdChar=3;
		break;
	case '1':
		m_iThirdChar=4;
		break;
	case '2':
		m_iThirdChar=5;
		break;
	case '3':
		m_iThirdChar=6;
		break;
	case '4':
		m_iThirdChar=7;
		break;
	case 'T':
		m_iThirdChar=8;
		break;
	default:
		m_iThirdChar=0;
		break;
	}
	GetDlgItem(IDC_CMB_FIRST_CHAR)->EnableWindow(m_bFirstChar);
	GetDlgItem(IDC_CMB_SEC_CHAR)->EnableWindow(m_bSecChar);
	GetDlgItem(IDC_CMB_THIRD_CHAR)->EnableWindow(m_bThirdChar);
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGroupEditLayerDlg::OnChkFirstChar() 
{
	UpdateData();
	GetDlgItem(IDC_CMB_FIRST_CHAR)->EnableWindow(m_bFirstChar);
}

void CGroupEditLayerDlg::OnChkSecChar() 
{
	UpdateData();
	GetDlgItem(IDC_CMB_SEC_CHAR)->EnableWindow(m_bSecChar);
}

void CGroupEditLayerDlg::OnChkThirdChar() 
{
	UpdateData();
	GetDlgItem(IDC_CMB_THIRD_CHAR)->EnableWindow(m_bThirdChar);
}

void CGroupEditLayerDlg::OnOK() 
{
	UpdateData();
	switch(m_iFirstChar)
	{
	case 0:
		layer[0]='T';
		break;
	case 1:
		layer[0]='S';
		break;
	case 2:
		layer[0]='L';
		break;
	default:
		layer[0]='T';
		break;
	}
	switch(m_iSecChar)
	{
	case 0:
		layer[1]='Z';
		break;
	case 1:
		layer[1]='X';
		break;
	case 2:
		layer[1]='H';
		break;
	case 3:
		layer[1]='F';
		break;
	default:
		layer[1]='Z';
		break;
	}
	switch(m_iThirdChar)
	{
	case 0:
		layer[2]='Q';
		break;
	case 1:
		layer[2]='H';
		break;
	case 2:
		layer[2]='Z';
		break;
	case 3:
		layer[2]='Y';
		break;
	case 4:
		layer[2]='1';
		break;
	case 5:
		layer[2]='2';
		break;
	case 6:
		layer[2]='3';
		break;
	case 7:
		layer[2]='4';
		break;
	case 8:
		layer[2]='G';
		break;
	default:
		layer[2]='Q';
		break;
	}
	CDialog::OnOK();
}
