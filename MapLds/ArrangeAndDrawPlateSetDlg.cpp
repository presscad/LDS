// ArrangeAndDrawPlateSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ArrangeAndDrawPlateSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArrangeAndDrawPlateSetDlg dialog


CArrangeAndDrawPlateSetDlg::CArrangeAndDrawPlateSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CArrangeAndDrawPlateSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CArrangeAndDrawPlateSetDlg)
	m_nMinDistance = 5;
	m_nPaperW = 1500;
	m_nPaperL = 0;
	m_bAutoArrange=TRUE;
	//}}AFX_DATA_INIT
}


void CArrangeAndDrawPlateSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArrangeAndDrawPlateSetDlg)
	DDX_Text(pDX, IDC_E_MINDISTANCE, m_nMinDistance);
	DDX_Text(pDX, IDC_E_MAP_W, m_nPaperW);
	DDX_Text(pDX, IDC_E_MAP_L, m_nPaperL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArrangeAndDrawPlateSetDlg, CDialog)
	//{{AFX_MSG_MAP(CArrangeAndDrawPlateSetDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArrangeAndDrawPlateSetDlg message handlers
BOOL CArrangeAndDrawPlateSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(!m_bAutoArrange)
	{
		m_nPaperL=m_nPaperW=0;
		CEdit* pDlg1=(CEdit*)GetDlgItem(IDC_E_MAP_W);
		pDlg1->SetReadOnly(TRUE);
		CEdit* pDlg2=(CEdit*)GetDlgItem(IDC_E_MAP_L);
		pDlg2->SetReadOnly(TRUE);
	}
	UpdateData(FALSE);
	return TRUE;
}