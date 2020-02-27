// AddDianBanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "AddDianBanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddDianBanDlg dialog


CAddDianBanDlg::CAddDianBanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddDianBanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddDianBanDlg)
	m_nDianBanThick = 0;
	m_sDianBanNo = _T("");
	//}}AFX_DATA_INIT
}


void CAddDianBanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddDianBanDlg)
	DDX_Text(pDX, IDC_E_DIANBAN_THICK, m_nDianBanThick);
	DDX_Text(pDX, IDC_E_DIANBAN_NO, m_sDianBanNo);
	DDV_MaxChars(pDX, m_sDianBanNo, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddDianBanDlg, CDialog)
	//{{AFX_MSG_MAP(CAddDianBanDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddDianBanDlg message handlers
