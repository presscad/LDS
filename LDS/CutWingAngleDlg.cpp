// CutWingAngleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "CutWingAngleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CCutWingAngleDlg dialog


CCutWingAngleDlg::CCutWingAngleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCutWingAngleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCutWingAngleDlg)
	m_fCutAngleXLen = 0.0;
	m_fCutAngleXWide = 0.0;
	m_fCutAngleYLen = 0.0;
	m_fCutAngleYWide = 0.0;
	m_iCutXWingAngle = 0;
	m_fCutWingLen = 0.0;
	m_fCutWingLen2 = 0.0;
	m_fCutWingWide = 0.0;
	m_bUserSpecCutInfo = FALSE;
	//}}AFX_DATA_INIT
}


void CCutWingAngleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCutWingAngleDlg)
	DDX_Text(pDX, IDC_E_CUT_ANGLE_X_LEN, m_fCutAngleXLen);
	DDX_Text(pDX, IDC_E_CUT_ANGLE_X_WIDE, m_fCutAngleXWide);
	DDX_Text(pDX, IDC_E_CUT_ANGLE_Y_LEN, m_fCutAngleYLen);
	DDX_Text(pDX, IDC_E_CUT_ANGLE_Y_WIDE, m_fCutAngleYWide);
	DDX_Radio(pDX, IDC_RDO_X_WING, m_iCutXWingAngle);
	DDX_Text(pDX, IDC_E_CUT_WING_LEN, m_fCutWingLen);
	DDX_Text(pDX, IDC_E_CUT_WING_LEN2, m_fCutWingLen2);
	DDX_Text(pDX, IDC_E_CUT_WING_WIDE, m_fCutWingWide);
	DDX_Check(pDX, IDC_CHK_USER_SPEC_CUT_INFO, m_bUserSpecCutInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCutWingAngleDlg, CDialog)
	//{{AFX_MSG_MAP(CCutWingAngleDlg)
	ON_BN_CLICKED(IDC_CHK_USER_SPEC_CUT_INFO, OnChkUserSpecCutInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CCutWingAngleDlg message handlers

BOOL CCutWingAngleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	RefreshReadOnlyState();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCutWingAngleDlg::RefreshReadOnlyState()
{
	GetDlgItem(IDC_RDO_X_WING)->EnableWindow(m_bUserSpecCutInfo);
	GetDlgItem(IDC_RDO_X_WING_X)->EnableWindow(m_bUserSpecCutInfo);
	GetDlgItem(IDC_RDO_X_WING_Y)->EnableWindow(m_bUserSpecCutInfo);
	((CEdit*)GetDlgItem(IDC_E_CUT_WING_WIDE))->SetReadOnly(!m_bUserSpecCutInfo);
	((CEdit*)GetDlgItem(IDC_E_CUT_WING_LEN))->SetReadOnly(!m_bUserSpecCutInfo);
	((CEdit*)GetDlgItem(IDC_E_CUT_WING_LEN2))->SetReadOnly(!m_bUserSpecCutInfo);
	((CEdit*)GetDlgItem(IDC_E_CUT_ANGLE_X_WIDE))->SetReadOnly(!m_bUserSpecCutInfo);
	((CEdit*)GetDlgItem(IDC_E_CUT_ANGLE_X_LEN))->SetReadOnly(!m_bUserSpecCutInfo);
	((CEdit*)GetDlgItem(IDC_E_CUT_ANGLE_Y_WIDE))->SetReadOnly(!m_bUserSpecCutInfo);
	((CEdit*)GetDlgItem(IDC_E_CUT_ANGLE_Y_LEN))->SetReadOnly(!m_bUserSpecCutInfo);
}

void CCutWingAngleDlg::OnChkUserSpecCutInfo() 
{
	UpdateData();
	RefreshReadOnlyState();
}
#endif
