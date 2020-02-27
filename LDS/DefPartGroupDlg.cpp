// DefPartGroupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DefPartGroupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefPartGroupDlg dialog


CDefPartGroupDlg::CDefPartGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefPartGroupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefPartGroupDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDefPartGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefPartGroupDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefPartGroupDlg, CDialog)
	//{{AFX_MSG_MAP(CDefPartGroupDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefPartGroupDlg message handlers
