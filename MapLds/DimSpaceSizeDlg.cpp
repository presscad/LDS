// DimSpaceSizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DimSpaceSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDimSpaceSizeDlg dialog


CDimSpaceSizeDlg::CDimSpaceSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDimSpaceSizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDimSpaceSizeDlg)
	m_iDimDirection = 0;
	//}}AFX_DATA_INIT
}


void CDimSpaceSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDimSpaceSizeDlg)
	DDX_Radio(pDX, IDC_DIMENSION_DRIECTION, m_iDimDirection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDimSpaceSizeDlg, CDialog)
	//{{AFX_MSG_MAP(CDimSpaceSizeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDimSpaceSizeDlg message handlers
