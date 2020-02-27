// OpenSizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "OpenSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenSizeDlg dialog


COpenSizeDlg::COpenSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenSizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenSizeDlg)
	m_fFrontOpen = 0.0;
	m_fSideOpen = 0.0;
	m_fVerticalHigh = 0.0;
	//}}AFX_DATA_INIT
}


void COpenSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenSizeDlg)
	DDX_Text(pDX, IDC_E_FRONT_OPEN, m_fFrontOpen);
	DDV_MinMaxDouble(pDX, m_fFrontOpen, 0., 100000.);
	DDX_Text(pDX, IDC_E_SIDE_OPEN, m_fSideOpen);
	DDV_MinMaxDouble(pDX, m_fSideOpen, 0., 100000.);
	DDX_Text(pDX, IDC_E_VETICAL_HIGH, m_fVerticalHigh);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenSizeDlg, CDialog)
	//{{AFX_MSG_MAP(COpenSizeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenSizeDlg message handlers
