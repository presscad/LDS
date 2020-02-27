// JgNodeDaoWeiDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "JgNodePrjDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJgNodePrjDlg dialog


CJgNodePrjDlg::CJgNodePrjDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJgNodePrjDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJgNodePrjDlg)
	m_iPosStyle = 0;
	//}}AFX_DATA_INIT
}


void CJgNodePrjDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJgNodePrjDlg)
	DDX_Radio(pDX, IDC_RDO_POS_STYLE, m_iPosStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJgNodePrjDlg, CDialog)
	//{{AFX_MSG_MAP(CJgNodePrjDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJgNodePrjDlg message handlers
