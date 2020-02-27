// ExcelExportOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "ExcelExportOptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExcelExportOptionDlg dialog


CExcelExportOptionDlg::CExcelExportOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExcelExportOptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExcelExportOptionDlg)
	m_bDataInterface = FALSE;
	m_bLsSumMatTbl = FALSE;
	m_bOrgMatTbl = FALSE;
	m_bPartTbl = FALSE;
	m_iLsTblStyle = 0;
	m_iWeldPartTblStyle = 0;
	m_bWeldPartTbl = FALSE;
	m_iOrgmatTblStyle = 0;
	m_bPoleExcelFile = FALSE;
	m_iPoleSortType = 0;
	m_sSegStr = _T("");
	m_bSelectAll = FALSE;
	//}}AFX_DATA_INIT
}


void CExcelExportOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExcelExportOptionDlg)
	DDX_Check(pDX, IDC_CHK_DATA_INTERFACE, m_bDataInterface);
	DDX_Check(pDX, IDC_CHK_CUSTOM_TBL, m_bCustomTbl);
	DDX_Check(pDX, IDC_CHK_LS_SUM_MAT_TBL, m_bLsSumMatTbl);
	DDX_Check(pDX, IDC_CHK_ORG_MAT_TBL, m_bOrgMatTbl);
	DDX_Check(pDX, IDC_CHK_PART_TBL, m_bPartTbl);
	DDX_CBIndex(pDX, IDC_CMB_LS_TBL_STYLE, m_iLsTblStyle);
	DDX_CBIndex(pDX, IDC_CMB_WELD_PART_TBL_STYLE, m_iWeldPartTblStyle);
	DDX_Check(pDX, IDC_CHK_WELD_PART_TBL, m_bWeldPartTbl);
	DDX_CBIndex(pDX, IDC_CMB_ORG_MAT_TBL_STYLE, m_iOrgmatTblStyle);
	DDX_CBIndex(pDX, IDC_CMB_PARTS_SORT_TYPE, m_iPartsSortType);
	DDX_Check(pDX, ID_CHK_POLE_EXCEL_FILE, m_bPoleExcelFile);
	DDX_CBIndex(pDX, IDC_CMB_POLE_SORT_TYPE, m_iPoleSortType);
	DDX_Check(pDX, ID_CHK_SELECT_ALL, m_bSelectAll);
	DDX_Text(pDX, IDC_E_SEG_STR, m_sSegStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExcelExportOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CExcelExportOptionDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_CHK_SELECT_ALL, &CExcelExportOptionDlg::OnBnClickedChkSelectAll)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExcelExportOptionDlg message handlers


void CExcelExportOptionDlg::OnBnClickedChkSelectAll()
{
	m_bSelectAll = !m_bSelectAll;
	m_bDataInterface = m_bSelectAll;
	m_bLsSumMatTbl = m_bSelectAll;
	m_bOrgMatTbl = m_bSelectAll;
	m_bPartTbl = m_bSelectAll;
	m_bWeldPartTbl = m_bSelectAll;
	m_bPoleExcelFile = m_bSelectAll;
	UpdateData(FALSE);
}


BOOL CExcelExportOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_bDataInterface = FALSE;
	m_bLsSumMatTbl = FALSE;
	m_bOrgMatTbl = FALSE;
	m_bPartTbl = FALSE;
	m_bWeldPartTbl = FALSE;
	m_bPoleExcelFile = FALSE;
	m_bSelectAll = FALSE;
	const int BOM_SHEET_PART = 0x01;		//构件明细表
	const int BOM_SHEET_BOLT = 0x02;		//螺栓明细表
	const int BOM_SHEET_WELDPART = 0x04;	//组焊件明细表
	if (CLDSApp::CreateExcelBomFile == NULL || CLDSApp::GetSupportDataBufferVersion == NULL)
		GetDlgItem(IDC_CHK_CUSTOM_TBL)->ShowWindow(SW_HIDE);
	return TRUE;
}