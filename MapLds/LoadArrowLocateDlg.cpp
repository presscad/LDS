// LoadArrowLocateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LoadArrowLocateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoadArrowLocateDlg dialog


CLoadArrowLocateDlg::CLoadArrowLocateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadArrowLocateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadArrowLocateDlg)
	m_iXArrowLocate = 1;
	m_iYArrowLocate = 1;
	m_iZArrowLocate = 1;
	//}}AFX_DATA_INIT
}


void CLoadArrowLocateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadArrowLocateDlg)
	DDX_Radio(pDX, IDC_RDO_X_ARROW, m_iXArrowLocate);
	DDX_Radio(pDX, IDC_RDO_Y_ARROW, m_iYArrowLocate);
	DDX_Radio(pDX, IDC_RDO_Z_ARROW, m_iZArrowLocate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadArrowLocateDlg, CDialog)
	//{{AFX_MSG_MAP(CLoadArrowLocateDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadArrowLocateDlg message handlers
