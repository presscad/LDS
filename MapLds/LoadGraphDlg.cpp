// LoadGraphDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LoadGraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoadGraphDlg dialog


CLoadGraphDlg::CLoadGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadGraphDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadGraphDlg)
	m_nRowsOrColumns = 3;
	m_nRowGap = 1;
	m_nColumnGap = 1;
	m_iRowFirst = 0;
	//}}AFX_DATA_INIT
}


void CLoadGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadGraphDlg)
	DDX_Text(pDX, IDC_E_ROWS_OR_COLUMNS, m_nRowsOrColumns);
	DDV_MinMaxInt(pDX, m_nRowsOrColumns, 1, 10);
	DDX_Text(pDX, IDC_E_ROW_GAP, m_nRowGap);
	DDX_Text(pDX, IDC_E_COLUMN_GAP, m_nColumnGap);
	DDX_Radio(pDX, IDC_RDO_ROW_FIRST, m_iRowFirst);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadGraphDlg, CDialog)
	//{{AFX_MSG_MAP(CLoadGraphDlg)
	ON_BN_CLICKED(IDC_RDO_ROW_FIRST, OnRdoRowFirst)
	ON_BN_CLICKED(IDC_RADIO5, OnRdoRowFirst)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadGraphDlg message handlers

void CLoadGraphDlg::OnRdoRowFirst() 
{
	UpdateData();
	if(m_iRowFirst==0)
		GetDlgItem(IDC_S_PROMPT)->SetWindowText("每行列数:");
	else
		GetDlgItem(IDC_S_PROMPT)->SetWindowText("每列行数:");
}
