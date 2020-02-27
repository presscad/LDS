// DefBmpParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DefBmpParaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefBmpParaDlg dialog


CDefBmpParaDlg::CDefBmpParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefBmpParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefBmpParaDlg)
	m_nBmpHeight = 0;
	m_nBmpWidth = 0;
	//}}AFX_DATA_INIT
}


void CDefBmpParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefBmpParaDlg)
	DDX_Text(pDX, IDC_E_BMP_HEIGHT, m_nBmpHeight);
	DDX_Text(pDX, IDC_E_BMP_WIDTH, m_nBmpWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefBmpParaDlg, CDialog)
	//{{AFX_MSG_MAP(CDefBmpParaDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefBmpParaDlg message handlers
