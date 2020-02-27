// LsCheckParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "LsCheckParaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLsCheckParaDlg dialog


CLsCheckParaDlg::CLsCheckParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLsCheckParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLsCheckParaDlg)
	m_bCutAngleDist = TRUE;
	m_bLsEndSpace = TRUE;
	m_bRollingSpace = TRUE;
	m_fScope = 0.5;
	m_bBoltGuiGe = TRUE;
	m_bBoltL0 = TRUE;
	m_bLsPlateEdgeSpace = TRUE;
	m_sSegStr = _T("*");
	//}}AFX_DATA_INIT
}


void CLsCheckParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLsCheckParaDlg)
	DDX_Check(pDX, IDC_CHK_CUT_ANGLE_DIST, m_bCutAngleDist);
	DDX_Check(pDX, IDC_CHK_LS_END_SPACE, m_bLsEndSpace);
	DDX_Check(pDX, IDC_CHK_ROLLINGSPACE, m_bRollingSpace);
	DDX_Text(pDX, IDC_E_SCOPE, m_fScope);
	DDV_MinMaxDouble(pDX, m_fScope, 0., 100.);
	DDX_Check(pDX, IDC_CHK_BOLT_GUIGE, m_bBoltGuiGe);
	DDX_Check(pDX, IDC_CHK_BOLT_L0, m_bBoltL0);
	DDX_Check(pDX, IDC_CHK_LS_PLATE_EDGESPACE, m_bLsPlateEdgeSpace);
	DDX_Text(pDX, IDC_E_SEG_STR, m_sSegStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLsCheckParaDlg, CDialog)
	//{{AFX_MSG_MAP(CLsCheckParaDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLsCheckParaDlg message handlers
