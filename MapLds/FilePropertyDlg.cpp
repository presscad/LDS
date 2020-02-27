// FilePropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tower.h"
#include "FilePropertyDlg.h"
#include "resource.h"       // main symbols

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilePropertyDlg dialog


CFilePropertyDlg::CFilePropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilePropertyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilePropertyDlg)
	m_nNodeNum = 0;
	m_sFileVersion = _T("");
	m_sPrjName = _T("");
	m_sTaType = _T("");
	m_nTaNum = 1;
	m_sTaAlias = _T("");
	m_sTaStampNo = _T("");
	m_sTblMaker = _T("");
	m_sCritic = _T("");
	m_sAuditor = _T("");
	m_sCompanyName = _T("");
	m_sTaskNo= _T("");
	m_sTolerance= _T("ЙњБъ");
	m_iPlankAreaStatType = 1;
	m_iCirclePlankAreaType= 0;
	m_nPartNum = 0;
	//}}AFX_DATA_INIT
}


void CFilePropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilePropertyDlg)
	DDX_Text(pDX, IDC_NODE_NUM, m_nNodeNum);
	DDX_Text(pDX, IDC_VERSION, m_sFileVersion);
	DDX_Text(pDX, IDC_E_PRJ_NAME, m_sPrjName);
	DDV_MaxChars(pDX, m_sPrjName, 49);
	DDX_Text(pDX, IDC_E_TA_TYPE, m_sTaType);
	DDV_MaxChars(pDX, m_sTaType, 49);
	DDX_Text(pDX, IDC_E_TA_NUM, m_nTaNum);
	DDX_Text(pDX, IDC_E_TA_ALIAS, m_sTaAlias);
	DDV_MaxChars(pDX, m_sTaAlias, 49);
	DDX_Text(pDX, IDC_E_TA_STAMP_NO, m_sTaStampNo);
	DDV_MaxChars(pDX, m_sTaStampNo, 49);
	DDX_Text(pDX, IDC_E_TABLE_MAKER, m_sTblMaker);
	DDV_MaxChars(pDX, m_sTblMaker, 49);
	DDX_Text(pDX, IDC_E_CRITIC, m_sCritic);
	DDV_MaxChars(pDX, m_sCritic, 49);
	DDX_Text(pDX, IDC_E_AUDITOR, m_sAuditor);
	DDV_MaxChars(pDX, m_sAuditor, 49);
	DDX_Text(pDX, IDC_E_COMPANY_NAME, m_sCompanyName);
	DDV_MaxChars(pDX, m_sCompanyName, 100);
	DDX_CBIndex(pDX, IDC_CMB_PLANK_AREA_STAT_TYPE, m_iPlankAreaStatType);
	DDX_CBIndex(pDX, IDC_CMB_PLANK_AREA_STAT_TYPE2,m_iCirclePlankAreaType);
	DDX_Text(pDX, IDC_CMB_TOLERANCE, m_sTolerance);
	DDX_Text(pDX, IDC_E_TASK_NO, m_sTaskNo);
	DDX_Text(pDX, IDC_PART_NUM, m_nPartNum);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilePropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CFilePropertyDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilePropertyDlg message handlers
