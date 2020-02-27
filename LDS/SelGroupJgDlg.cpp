// SelGroupJgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "SelGroupJgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSel2CrossGroupJgDlg dialog


CSel2CrossGroupJgDlg::CSel2CrossGroupJgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSel2CrossGroupJgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSel2CrossGroupJgDlg)
	m_iCurOperJg = 0;
	//}}AFX_DATA_INIT
}


void CSel2CrossGroupJgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSel2CrossGroupJgDlg)
	DDX_Radio(pDX, IDC_RDO_CUR_OPER_SEL, m_iCurOperJg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSel2CrossGroupJgDlg, CDialog)
	//{{AFX_MSG_MAP(CSel2CrossGroupJgDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSel2CrossGroupJgDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CSel4CrossGroupJgDlg dialog


CSel4CrossGroupJgDlg::CSel4CrossGroupJgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSel4CrossGroupJgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSel4CrossGroupJgDlg)
	m_iCurOperJg = 0;
	//}}AFX_DATA_INIT
}


void CSel4CrossGroupJgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSel4CrossGroupJgDlg)
	DDX_Radio(pDX, IDC_RDO_CUR_OPER_SEL, m_iCurOperJg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSel4CrossGroupJgDlg, CDialog)
	//{{AFX_MSG_MAP(CSel4CrossGroupJgDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSel4CrossGroupJgDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CSelTGroupJgDlg dialog


CSelTGroupJgDlg::CSelTGroupJgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelTGroupJgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelTGroupJgDlg)
	m_iCurOperJg = 0;
	//}}AFX_DATA_INIT
}


void CSelTGroupJgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelTGroupJgDlg)
	DDX_Radio(pDX, IDC_RDO_CUR_OPER_SEL, m_iCurOperJg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelTGroupJgDlg, CDialog)
	//{{AFX_MSG_MAP(CSelTGroupJgDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelTGroupJgDlg message handlers
