// DisplayVectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "DisplayVectorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisplayVectorDlg dialog


CDisplayVectorDlg::CDisplayVectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplayVectorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDisplayVectorDlg)
	m_fVectorX = 0.0;
	m_fVectorY = 0.0;
	m_fVectorZ = 0.0;
	//}}AFX_DATA_INIT
}


void CDisplayVectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplayVectorDlg)
	DDX_Text(pDX, IDC_E_VECTOR_X, m_fVectorX);
	DDX_Text(pDX, IDC_E_VECTOR_Y, m_fVectorY);
	DDX_Text(pDX, IDC_E_VECTOR_Z, m_fVectorZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisplayVectorDlg, CDialog)
	//{{AFX_MSG_MAP(CDisplayVectorDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplayVectorDlg message handlers
