// ExportExcelOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "ExportExcelOptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportExcelOptionDlg dialog


CExportExcelOptionDlg::CExportExcelOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExportExcelOptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExportExcelOptionDlg)
	m_bBoltSumMatTbl = TRUE;
	m_bLineAngleSumMatTbl = TRUE;
	m_bLineFlatSumMatTbl = FALSE;
	m_bLineSlotSumMatTbl = FALSE;
	m_bLineTubeSumMatTbl = TRUE;
	m_bPlateSumMatTbl = TRUE;
	//}}AFX_DATA_INIT
}


void CExportExcelOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportExcelOptionDlg)
	DDX_Check(pDX, IDC_CHK_BOLT_SUM_MAT_TBL, m_bBoltSumMatTbl);
	DDX_Check(pDX, IDC_CHK_LINEANGLE_SUM_MAT_TBL, m_bLineAngleSumMatTbl);
	DDX_Check(pDX, IDC_CHK_LINEFLAT_SUM_MAT_TBL, m_bLineFlatSumMatTbl);
	DDX_Check(pDX, IDC_CHK_LINESLOT_SUM_MAT_TBL, m_bLineSlotSumMatTbl);
	DDX_Check(pDX, IDC_CHK_LINETUBE_SUM_MAT_TBL, m_bLineTubeSumMatTbl);
	DDX_Check(pDX, IDC_CHK_PLATE_SUM_MAT_TBL, m_bPlateSumMatTbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportExcelOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CExportExcelOptionDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportExcelOptionDlg message handlers
