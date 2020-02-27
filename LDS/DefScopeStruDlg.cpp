// DefScopeStruDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DefScopeStruDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefScopeStruDlg dialog


CDefScopeStruDlg::CDefScopeStruDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefScopeStruDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefScopeStruDlg)
	m_fMaxX = 5000.0;
	m_fMaxY = 3000.0;
	m_fMaxZ = 25000.0;
	m_fMinX = -5000.0;
	m_fMinY = -3000.0;
	m_fMinZ = 0.0;
	//}}AFX_DATA_INIT
}


void CDefScopeStruDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefScopeStruDlg)
	DDX_Text(pDX, IDC_E_MAX_X, m_fMaxX);
	DDX_Text(pDX, IDC_E_MAX_Y, m_fMaxY);
	DDX_Text(pDX, IDC_E_MAX_Z, m_fMaxZ);
	DDX_Text(pDX, IDC_E_MIN_X, m_fMinX);
	DDX_Text(pDX, IDC_E_MIN_Y, m_fMinY);
	DDX_Text(pDX, IDC_E_MIN_Z, m_fMinZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefScopeStruDlg, CDialog)
	//{{AFX_MSG_MAP(CDefScopeStruDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefScopeStruDlg message handlers
