// PartDrawingArraySpaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PartDrawingArraySpaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartDrawingArraySpaceDlg dialog


CPartDrawingArraySpaceDlg::CPartDrawingArraySpaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartDrawingArraySpaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartDrawingArraySpaceDlg)
	m_nRowHigh = 0;
	m_nColumnWidth = 0;
	m_iSpaceMode=0;
	//}}AFX_DATA_INIT
}


void CPartDrawingArraySpaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartDrawingArraySpaceDlg)
	DDX_Text(pDX, IDC_E_ROW_HIGH, m_nRowHigh);
	DDX_Text(pDX, IDC_E_COLUMN_WIDTH, m_nColumnWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartDrawingArraySpaceDlg, CDialog)
	//{{AFX_MSG_MAP(CPartDrawingArraySpaceDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartDrawingArraySpaceDlg message handlers


BOOL CPartDrawingArraySpaceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWnd* pLabelWnd;
	if((pLabelWnd=GetDlgItem(IDC_S_ROW_OR_HORI))!=NULL)
		pLabelWnd->SetWindowText(m_iSpaceMode==0?"行高:":"横向:");
	if((pLabelWnd=GetDlgItem(IDC_S_COL_OR_VERT))!=NULL)
		pLabelWnd->SetWindowText(m_iSpaceMode==0?"列宽:":"竖向:");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
