// SelPartTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "SelPartTypeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelPartTypeDlg dialog


CSelPartTypeDlg::CSelPartTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelPartTypeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelPartTypeDlg)
	m_iPartType = 0;
	//}}AFX_DATA_INIT
}


void CSelPartTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelPartTypeDlg)
	DDX_Radio(pDX, IDC_RDO_PART_TYPE, m_iPartType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelPartTypeDlg, CDialog)
	//{{AFX_MSG_MAP(CSelPartTypeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelPartTypeDlg message handlers
