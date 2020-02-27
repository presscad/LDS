// BreakLineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BreakLineDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBreakLineDlg dialog


CBreakLineDlg::CBreakLineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBreakLineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBreakLineDlg)
	m_fBreakLineLen = 1.4;
	m_fLenThreshold = 8;
	//}}AFX_DATA_INIT
}


void CBreakLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBreakLineDlg)
	DDX_Text(pDX, IDC_E_BREAK_LINE_LEN, m_fBreakLineLen);
	DDX_Text(pDX, IDC_E_LEN_THRESHOLD, m_fLenThreshold);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBreakLineDlg, CDialog)
	//{{AFX_MSG_MAP(CBreakLineDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBreakLineDlg message handlers
