// InsertMapFrmDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "tma.h"
#include "InsertMapFrmDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInsertMapFrmDlg dialog


CInsertMapFrmDlg::CInsertMapFrmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInsertMapFrmDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInsertMapFrmDlg)
	m_iMapFrmStyle = 0;
	//}}AFX_DATA_INIT
}


void CInsertMapFrmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertMapFrmDlg)
	DDX_Radio(pDX, IDC_RDO_MAP_FRM, m_iMapFrmStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInsertMapFrmDlg, CDialog)
	//{{AFX_MSG_MAP(CInsertMapFrmDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsertMapFrmDlg message handlers
