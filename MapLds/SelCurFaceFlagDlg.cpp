// SelCurFaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SelCurFaceFlagDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelCurFaceFlagDlg dialog


CSelCurFaceFlagDlg::CSelCurFaceFlagDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelCurFaceFlagDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelCurFaceFlagDlg)
	m_iCurFace = 2;
	//}}AFX_DATA_INIT
}


void CSelCurFaceFlagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelCurFaceFlagDlg)
	DDX_Radio(pDX, IDC_RDO_CUR_FACE, m_iCurFace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelCurFaceFlagDlg, CDialog)
	//{{AFX_MSG_MAP(CSelCurFaceFlagDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelCurFaceFlagDlg message handlers
