// PolyStyleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "PolyStyleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPolyStyleDlg dialog


CPolyStyleDlg::CPolyStyleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPolyStyleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPolyStyleDlg)
	m_iPolyStyle = 0;
	//}}AFX_DATA_INIT
}


void CPolyStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPolyStyleDlg)
	DDX_Radio(pDX, IDC_RDO_POLY_STYLE, m_iPolyStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPolyStyleDlg, CDialog)
	//{{AFX_MSG_MAP(CPolyStyleDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPolyStyleDlg message handlers
