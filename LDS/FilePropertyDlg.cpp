// FilePropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tower.h"
#include "FilePropertyDlg.h"
#include "resource.h"       // main symbols
#include ".\filepropertydlg.h"
#include "env_def.h"

#if defined(__PART_DESIGN_INC_)
#include "..\Log\XhLog.h"
#endif

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
	m_iPlankAreaStatType = 1;
	m_iCirclePlankAreaType= 0;
	m_nPartNum = 0;
	m_sTabMaker = _T("");
	m_sAuditor = _T("");
	m_bSaveLogVersion=TRUE;
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
	DDX_CBIndex(pDX, IDC_CMB_PLANK_AREA_STAT_TYPE, m_iPlankAreaStatType);
	DDX_CBIndex(pDX, IDC_CMB_PLANK_AREA_STAT_TYPE2,m_iCirclePlankAreaType);
	DDX_Text(pDX, IDC_JG_NUM, m_nPartNum);
	DDX_Text(pDX, IDC_E_TAB_MAKER, m_sTabMaker);
	DDX_Text(pDX, IDC_E_AUDITOR, m_sAuditor);
	DDX_Check(pDX, IDC_CHK_SAVE_LOG_VERSION, m_bSaveLogVersion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilePropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CFilePropertyDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilePropertyDlg message handlers

void CFilePropertyDlg::OnOK()
{
	UpdateData();
#if defined(__PART_DESIGN_INC_)
	g_sysPara.m_bSaveLogVersion=m_bSaveLogVersion;
#endif
	_snprintf(Ta.m_sPrjName,MAX_TEMP_CHAR_50,"%s",m_sPrjName);
	_snprintf(Ta.m_sTaType,MAX_TEMP_CHAR_50,"%s",m_sTaType);
	_snprintf(Ta.m_sTaAlias,MAX_TEMP_CHAR_50,"%s",m_sTaAlias);
	_snprintf(Ta.m_sTaStampNo,MAX_TEMP_CHAR_50,"%s",m_sTaStampNo);
	_snprintf(Ta.m_sOperator,MAX_TEMP_CHAR_50,"%s",m_sTabMaker);
	_snprintf(Ta.m_sAuditor,MAX_TEMP_CHAR_50,"%s",m_sAuditor);
	Ta.m_nTaNum = m_nTaNum;
#ifdef __COMMON_PART_INC_
	CLDSPlate::m_iPlankWeightStatType = m_iPlankAreaStatType+1;
	CLDSParamPlate::m_iCirclePlankAreaType=m_iCirclePlankAreaType+1;
#endif
	CDialog::OnOK();
}

BOOL CFilePropertyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_sFileVersion = Ta.version;
	m_nNodeNum = Ta.Node.GetNodeNum();
	m_nPartNum = Ta.Parts.GetNodeNum();
	m_sPrjName = Ta.m_sPrjName;
	m_sTaType = Ta.m_sTaType;
	m_nTaNum = Ta.m_nTaNum;
	m_sTaAlias = Ta.m_sTaAlias;
	m_sTaStampNo = Ta.m_sTaStampNo;
	m_sTabMaker= Ta.m_sOperator;
	m_sAuditor = Ta.m_sAuditor;
#ifdef __COMMON_PART_INC_
	m_iPlankAreaStatType = CLDSPlate::m_iPlankWeightStatType-1;
	m_iCirclePlankAreaType= CLDSParamPlate::m_iCirclePlankAreaType-1;
#endif
#if defined(__PART_DESIGN_INC_)
	GetDlgItem(IDC_CHK_SAVE_LOG_VERSION)->ShowWindow(SW_SHOW);
	m_bSaveLogVersion = g_sysPara.m_bSaveLogVersion;
#else
	GetDlgItem(IDC_CHK_SAVE_LOG_VERSION)->ShowWindow(SW_HIDE);
	m_bSaveLogVersion = FALSE;
#endif
	UpdateData(FALSE);
	return TRUE;
}
