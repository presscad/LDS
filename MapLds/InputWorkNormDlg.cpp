// InputWorkNormDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InputWorkNormDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputWorkNormDlg dialog


CInputWorkNormDlg::CInputWorkNormDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputWorkNormDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputWorkNormDlg)
	m_bDefaultWorkNorm = TRUE;
	m_fNormX = 0.0;
	m_fNormY = 1.0;
	m_fNormZ = 0.0;
	//}}AFX_DATA_INIT
}


void CInputWorkNormDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputWorkNormDlg)
	DDX_Check(pDX, IDC_CHK_DEFAULT_WORK_NORM, m_bDefaultWorkNorm);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputWorkNormDlg, CDialog)
	//{{AFX_MSG_MAP(CInputWorkNormDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_WORK_NORM, OnBnPasteWorkNorm)
	ON_BN_CLICKED(IDC_CHK_DEFAULT_WORK_NORM, OnChkDefaultWorkNorm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputWorkNormDlg message handlers

BOOL CInputWorkNormDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CEdit *pEdit;
	pEdit = (CEdit*)GetDlgItem(IDC_E_NORM_X);
	pEdit->EnableWindow(FALSE);
	pEdit = (CEdit*)GetDlgItem(IDC_E_NORM_Y);
	pEdit->EnableWindow(FALSE);
	pEdit = (CEdit*)GetDlgItem(IDC_E_NORM_Z);
	pEdit->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputWorkNormDlg::OnChkDefaultWorkNorm() 
{
	UpdateData();
	CEdit *pEdit;
	if(m_bDefaultWorkNorm)
	{
		pEdit = (CEdit*)GetDlgItem(IDC_E_NORM_X);
		pEdit->EnableWindow(FALSE);
		pEdit = (CEdit*)GetDlgItem(IDC_E_NORM_Y);
		pEdit->EnableWindow(FALSE);
		pEdit = (CEdit*)GetDlgItem(IDC_E_NORM_Z);
		pEdit->EnableWindow(FALSE);
	}
	else
	{
		pEdit = (CEdit*)GetDlgItem(IDC_E_NORM_X);
		pEdit->EnableWindow(TRUE);
		pEdit = (CEdit*)GetDlgItem(IDC_E_NORM_Y);
		pEdit->EnableWindow(TRUE);
		pEdit = (CEdit*)GetDlgItem(IDC_E_NORM_Z);
		pEdit->EnableWindow(TRUE);
	}
}

void CInputWorkNormDlg::OnBnPasteWorkNorm() 
{
	f3dPoint point;
	ReadClipPoint(point);
	m_fNormX = point.x;
	m_fNormY = point.y;
	m_fNormZ = point.z;
	UpdateData(FALSE);
}
