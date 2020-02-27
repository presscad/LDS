// SolidCollideOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "SolidCollideOptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSolidCollideOptionDlg dialog


CSolidCollideOptionDlg::CSolidCollideOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSolidCollideOptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSolidCollideOptionDlg)
	m_bJgVsJg = FALSE;
	m_bJgVsLjb = FALSE;
	m_bJgVsLs = FALSE;
	m_bLjbVsLjb = FALSE;
	m_bLjbVsLs = FALSE;
	m_bLsVsLs = FALSE;
	//}}AFX_DATA_INIT
}


void CSolidCollideOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSolidCollideOptionDlg)
	DDX_Check(pDX, IDC_CHK_JG_VS_JG, m_bJgVsJg);
	DDX_Check(pDX, IDC_CHK_JG_VS_LJB, m_bJgVsLjb);
	DDX_Check(pDX, IDC_CHK_JG_VS_LS, m_bJgVsLs);
	DDX_Check(pDX, IDC_CHK_LJB_VS_LJB, m_bLjbVsLjb);
	DDX_Check(pDX, IDC_CHK_LJB_VS_LS, m_bLjbVsLs);
	DDX_Check(pDX, IDC_CHK_LS_VS_LS, m_bLsVsLs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSolidCollideOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CSolidCollideOptionDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSolidCollideOptionDlg message handlers
