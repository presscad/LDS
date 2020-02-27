// 3dPtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "3dPtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C3dPtDlg dialog


C3dPtDlg::C3dPtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C3dPtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(C3dPtDlg)
	m_fPtX = 0.0;
	m_fPtY = 0.0;
	m_fPtZ = 0.0;
	//}}AFX_DATA_INIT
	m_bCanModify = TRUE;
}


void C3dPtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C3dPtDlg)
	DDX_Text(pDX, IDC_E_PT_X, m_fPtX);
	DDX_Text(pDX, IDC_E_PT_Y, m_fPtY);
	DDX_Text(pDX, IDC_E_PT_Z, m_fPtZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(C3dPtDlg, CDialog)
	//{{AFX_MSG_MAP(C3dPtDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3dPtDlg message handlers

BOOL C3dPtDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_E_PT_X)->EnableWindow(m_bCanModify);
	GetDlgItem(IDC_E_PT_Y)->EnableWindow(m_bCanModify);
	GetDlgItem(IDC_E_PT_Z)->EnableWindow(m_bCanModify);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
