// EditJointJgZhunJuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "EditJointJgZhunJuDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditJointJgZhunJuDlg dialog


CEditJointJgZhunJuDlg::CEditJointJgZhunJuDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CEditJointJgZhunJuDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditJointJgZhunJuDlg)
	m_nJg1G = 0;
	m_nJg1G1 = 0;
	m_nJg1G2 = 0;
	m_nJg1G3 = 0;
	m_nJg2G = 0;
	m_nJg2G1 = 0;
	m_nJg2G2 = 0;
	m_nJg2G3 = 0;
	m_sJg1GuiGe = _T("");
	m_sJg2GuiGe = _T("");
	//}}AFX_DATA_INIT
}


void CEditJointJgZhunJuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditJointJgZhunJuDlg)
	DDX_Text(pDX, IDC_E_JG1_G, m_nJg1G);
	DDX_Text(pDX, IDC_E_JG1_G1, m_nJg1G1);
	DDX_Text(pDX, IDC_E_JG1_G2, m_nJg1G2);
	DDX_Text(pDX, IDC_E_JG1_G3, m_nJg1G3);
	DDX_Text(pDX, IDC_E_JG2_G, m_nJg2G);
	DDX_Text(pDX, IDC_E_JG2_G1, m_nJg2G1);
	DDX_Text(pDX, IDC_E_JG2_G2, m_nJg2G2);
	DDX_Text(pDX, IDC_E_JG2_G3, m_nJg2G3);
	DDX_Text(pDX, IDC_S_JG1_GUIGE, m_sJg1GuiGe);
	DDX_Text(pDX, IDC_S_JG2_GUIGE, m_sJg2GuiGe);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditJointJgZhunJuDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CEditJointJgZhunJuDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditJointJgZhunJuDlg message handlers

BOOL CEditJointJgZhunJuDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_nJg1G  = jgzj1.g;
	m_nJg1G1 = jgzj1.g1;
	m_nJg1G2 = jgzj1.g2;
	m_nJg1G3 = jgzj1.g3;
	m_nJg2G  = jgzj2.g;
	m_nJg2G1 = jgzj2.g1;
	m_nJg2G2 = jgzj2.g2;
	m_nJg2G3 = jgzj2.g3;
	UpdateData(FALSE);
	MoveWndToLocation();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditJointJgZhunJuDlg::OnOK() 
{
	CDialog::OnOK();
	jgzj1.g  = m_nJg1G;
	jgzj1.g1 = m_nJg1G1;
	jgzj1.g2 = m_nJg1G2;
	jgzj1.g3 = m_nJg1G3;
	jgzj2.g  = m_nJg2G;
	jgzj2.g1 = m_nJg2G1;
	jgzj2.g2 = m_nJg2G2;
	jgzj2.g3 = m_nJg2G3;
}
