// DesCtrlPtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesCtrlPtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesCtrlPtDlg dialog


CDesCtrlPtDlg::CDesCtrlPtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesCtrlPtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesCtrlPtDlg)
	m_iPosSpecStyle = 0;
	m_hBaseNode = _T("");
	m_iJgOffsetVec = 0;
	m_iOffsetWing = 0;
	m_hBaseJg = _T("");
	m_fLenOffset = 0.0;
	m_fWingOffset = 0.0;
	//}}AFX_DATA_INIT
}


void CDesCtrlPtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesCtrlPtDlg)
	DDX_CBIndex(pDX, IDC_CMB_POS_SPEC_STYLE, m_iPosSpecStyle);
	DDX_Text(pDX, IDC_E_BASE_NODE, m_hBaseNode);
	DDX_Radio(pDX, IDC_RDO_JG_OFFSET_VEC, m_iJgOffsetVec);
	DDX_Radio(pDX, IDC_RDO_OFFSET_WING, m_iOffsetWing);
	DDX_Text(pDX, IDC_E_BASE_JG, m_hBaseJg);
	DDX_Text(pDX, IDC_E_LEN_OFFSET_DIST, m_fLenOffset);
	DDX_Text(pDX, IDC_E_WING_OFFSET_DIST, m_fWingOffset);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesCtrlPtDlg, CDialog)
	//{{AFX_MSG_MAP(CDesCtrlPtDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesCtrlPtDlg message handlers
