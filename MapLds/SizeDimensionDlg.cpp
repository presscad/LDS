// SizeDimensionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SizeDimensionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSizeDimensionDlg dialog


CSizeDimensionDlg::CSizeDimensionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSizeDimensionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSizeDimensionDlg)
	m_nDimDirection = 0;
	//}}AFX_DATA_INIT
}


void CSizeDimensionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSizeDimensionDlg)
	DDX_Radio(pDX, IDC_DIMENSION_DRIECTION, m_nDimDirection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSizeDimensionDlg, CDialog)
	//{{AFX_MSG_MAP(CSizeDimensionDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSizeDimensionDlg message handlers
