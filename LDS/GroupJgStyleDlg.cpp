// GroupJgStyleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "GroupJgStyleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupJgStyleDlg dialog


CGroupJgStyleDlg::CGroupJgStyleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGroupJgStyleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGroupJgStyleDlg)
	m_iJgGroupStyle = 0;
	m_fFillThick = 12.0;
	//}}AFX_DATA_INIT
}


void CGroupJgStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupJgStyleDlg)
	DDX_Radio(pDX, IDC_RDO_GROUP_STYLE, m_iJgGroupStyle);
	DDX_Text(pDX, IDC_E_FILL_PLANK_THICK, m_fFillThick);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupJgStyleDlg, CDialog)
	//{{AFX_MSG_MAP(CGroupJgStyleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupJgStyleDlg message handlers
IMPLEMENT_GET(CGroupJgStyleDlg,wSectStyle)
{
	if(m_iJgGroupStyle<=1)
		return 0;	//对角组合
	else if(m_iJgGroupStyle<=3)
		return 1;	//T型组合
	else //if(m_iJgGroupStyle==4)
		return 2;	//T型组合
}
IMPLEMENT_GET(CGroupJgStyleDlg,ciMasterIndex)
{
	if(m_iJgGroupStyle==1||m_iJgGroupStyle==3)
		return 1;
	else //if(m_iJgGroupStyle==0||m_iJgGroupStyle==2||m_iJgGroupStyle==4)
		return 0;
}

BOOL CGroupJgStyleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
