// FootPlankWrapLenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "FootPlankWrapLenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFootPlankWrapLenDlg dialog


CFootPlankWrapLenDlg::CFootPlankWrapLenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFootPlankWrapLenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(300.0;
	m_fXWingBackWrapWidth = 0.0;
	m_fXWingFrontOffset = 0.0;
	m_fYWingBackWrapWidth = 0.0;
	m_fYWingFrontOffset = 0.0;
	//}}AFX_DATA_INIT
	weld_style=1;
	m_bBreakWingX = m_bBreakWingY = TRUE;
}


void CFootPlankWrapLenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFootPlankWrapLenDlg)
	DDX_Text(pDX, IDC_E_RIDGE_WRAP_LEN, m_fRidgeWrapLen);
	DDX_Text(pDX, IDC_E_X_WING_BACK_WRAP_WIDTH, m_fXWingBackWrapWidth);
	DDX_Text(pDX, IDC_E_X_WING_FRONT_OFFSET, m_fXWingFrontOffset);
	DDX_Text(pDX, IDC_E_Y_WING_BACK_WRAP_WIDTH, m_fYWingBackWrapWidth);
	DDX_Text(pDX, IDC_E_Y_WING_FRONT_OFFSET, m_fYWingFrontOffset);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFootPlankWrapLenDlg, CDialog)
	//{{AFX_MSG_MAP(CFootPlankWrapLenDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFootPlankWrapLenDlg message handlers

BOOL CFootPlankWrapLenDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(weld_style==1)	//¶Ôº¸
	{
		GetDlgItem(IDC_E_X_WING_FRONT_OFFSET)->EnableWindow(m_bBreakWingX);
		GetDlgItem(IDC_E_Y_WING_FRONT_OFFSET)->EnableWindow(m_bBreakWingY);
		GetDlgItem(IDC_E_X_WING_BACK_WRAP_WIDTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_Y_WING_BACK_WRAP_WIDTH)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_E_X_WING_FRONT_OFFSET)->EnableWindow(m_bBreakWingX);
		GetDlgItem(IDC_E_Y_WING_FRONT_OFFSET)->EnableWindow(m_bBreakWingY);
		GetDlgItem(IDC_E_X_WING_BACK_WRAP_WIDTH)->EnableWindow(m_bBreakWingX);
		GetDlgItem(IDC_E_Y_WING_BACK_WRAP_WIDTH)->EnableWindow(m_bBreakWingY);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
