// FootViewSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FootViewSelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFootViewSelDlg dialog


CFootViewSelDlg::CFootViewSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFootViewSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFootViewSelDlg)
	m_iViewStyle = 0;
	m_hFootHandle = _T("");
	m_nScale = 5;
	//}}AFX_DATA_INIT
}


void CFootViewSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFootViewSelDlg)
	DDX_Radio(pDX, IDC_RDO_VIEW_STYLE, m_iViewStyle);
	DDX_Text(pDX, IDC_E_FOOT_HANDLE, m_hFootHandle);
	DDX_Text(pDX, IDC_E_DRAW_SCALE, m_nScale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFootViewSelDlg, CDialog)
	//{{AFX_MSG_MAP(CFootViewSelDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFootViewSelDlg message handlers
