// InputJgGDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "InputJgGDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputJgGDlg dialog


CInputJgGDlg::CInputJgGDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputJgGDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputJgGDlg)
	m_nXG = 0;
	m_nXG1 = 0;
	m_nXG2 = 0;
	m_nXG3 = 0;
	m_nYG = 0;
	m_nYG1 = 0;
	m_nYG2 = 0;
	m_nYG3 = 0;
	//}}AFX_DATA_INIT
}


void CInputJgGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputJgGDlg)
	DDX_Text(pDX, IDC_E_WING_X_G, m_nXG);
	DDX_Text(pDX, IDC_E_WING_X_G1, m_nXG1);
	DDX_Text(pDX, IDC_E_WING_X_G2, m_nXG2);
	DDX_Text(pDX, IDC_E_WING_X_G3, m_nXG3);
	DDX_Text(pDX, IDC_E_WING_Y_G, m_nYG);
	DDX_Text(pDX, IDC_E_WING_Y_G1, m_nYG1);
	DDX_Text(pDX, IDC_E_WING_Y_G2, m_nYG2);
	DDX_Text(pDX, IDC_E_WING_Y_G3, m_nYG3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputJgGDlg, CDialog)
	//{{AFX_MSG_MAP(CInputJgGDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputJgGDlg message handlers
