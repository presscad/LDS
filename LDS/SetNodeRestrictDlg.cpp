// SetNodeRestrictDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "SetNodeRestrictDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetNodeRestrictDlg dialog


CSetNodeRestrictDlg::CSetNodeRestrictDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetNodeRestrictDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetNodeRestrictDlg)
	m_bRestrictX = FALSE;
	m_bRestrictY = FALSE;
	m_bRestrictZ = FALSE;
	m_hNode = _T("");
	m_nNodeI = 0;
	//}}AFX_DATA_INIT
	m_bNewRestrictNode=FALSE;
}


void CSetNodeRestrictDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetNodeRestrictDlg)
	DDX_Check(pDX, IDC_CHK_X_RESTRICT, m_bRestrictX);
	DDX_Check(pDX, IDC_CHK_Y_RESTRICT, m_bRestrictY);
	DDX_Check(pDX, IDC_CHK_Z_RESTRICT, m_bRestrictZ);
	DDX_Text(pDX, IDC_E_NODE_HANDLE, m_hNode);
	DDX_Text(pDX, IDC_E_NODE_I, m_nNodeI);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetNodeRestrictDlg, CDialog)
	//{{AFX_MSG_MAP(CSetNodeRestrictDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetNodeRestrictDlg message handlers

BOOL CSetNodeRestrictDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CEdit *pEdit=(CEdit*)GetDlgItem(IDC_E_NODE_HANDLE);
	pEdit->SetReadOnly(!m_bNewRestrictNode);
	pEdit=(CEdit*)GetDlgItem(IDC_E_NODE_I);
	pEdit->SetReadOnly(!m_bNewRestrictNode);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
