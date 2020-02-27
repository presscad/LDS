// ImportScriptOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImportScriptOptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportScriptOptionDlg dialog


CImportScriptOptionDlg::CImportScriptOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImportScriptOptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportScriptOptionDlg)
	m_bStruMap = FALSE;
	m_bAnalysisMap = FALSE;
	m_bSumMap = FALSE;
	m_sSegStr = _T("*");
	//}}AFX_DATA_INIT
}


void CImportScriptOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportScriptOptionDlg)
	DDX_Check(pDX, IDC_CHK_STRU_DRAWING_MAP, m_bStruMap);
	DDX_Check(pDX, IDC_CHK_ANALYSIS_MAP, m_bAnalysisMap);
	DDX_Check(pDX, IDC_CHK_SUM_DRAWING_MAP, m_bSumMap);
	DDX_Text(pDX, IDC_E_SEG_SCOPE, m_sSegStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportScriptOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CImportScriptOptionDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportScriptOptionDlg message handlers
