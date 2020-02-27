// JointBaseJg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "JointBaseJg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJointBaseJg dialog


CJointBaseJg::CJointBaseJg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CJointBaseJg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJointBaseJg)
	m_sLsGuiGe = _T("");
	m_nLsN = 0;
	m_nLsRows = 0;
	m_sJgGuiGe = _T("");
	m_nLsEndSpace = 0;
	m_nLsSpace = 0;
	//}}AFX_DATA_INIT
}


void CJointBaseJg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJointBaseJg)
	DDX_CBString(pDX, IDC_CMB_LS_GUIGE, m_sLsGuiGe);
	DDX_Text(pDX, IDC_LS_N, m_nLsN);
	DDV_MinMaxInt(pDX, m_nLsN, 1, 100);
	DDX_Radio(pDX, IDC_LS_ROWS, m_nLsRows);
	DDX_Text(pDX, IDC_JG_GUIGE, m_sJgGuiGe);
	DDX_Text(pDX, IDC_E_LS_END_SPACE, m_nLsEndSpace);
	DDX_Text(pDX, IDC_E_LS_SPACE, m_nLsSpace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJointBaseJg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CJointBaseJg)
	ON_CBN_SELCHANGE(IDC_CMB_LS_GUIGE, OnSelchangeLsGuige)
	ON_BN_CLICKED(IDC_LS_ROWS, OnLsRows)
	ON_BN_CLICKED(IDC_LS_ROWS2, OnLsRows)
	ON_EN_CHANGE(IDC_LS_N, &CJointBaseJg::OnChangeLsN)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJointBaseJg message handlers

BOOL CJointBaseJg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	MoveWndToLocation();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CJointBaseJg::OnSelchangeLsGuige() 
{
	CString sText;
	CComboBox *pCmb = (CComboBox*)GetDlgItem(IDC_CMB_LS_GUIGE);
	int iCur = pCmb->GetCurSel();
	UpdateData();
	pCmb->GetLBText(iCur,m_sLsGuiGe);
	int ls_d = atoi(m_sLsGuiGe);
	LSSPACE_STRU LsSpace;
	if(GetLsSpace(LsSpace,ls_d))
	{
		if(m_nLsRows==0)
			m_nLsSpace = LsSpace.SingleRowSpace;
		else
			m_nLsSpace = LsSpace.doubleRowSpace;
		m_nLsEndSpace = LsSpace.EndSpace;
		UpdateData(FALSE);
	}
}

void CJointBaseJg::OnLsRows() 
{
	UpdateData();
	int ls_d = atoi(m_sLsGuiGe);
	LSSPACE_STRU LsSpace;
	if(GetLsSpace(LsSpace,ls_d))
	{
		if(m_nLsRows==0)
			m_nLsSpace = LsSpace.SingleRowSpace;
		else
			m_nLsSpace = LsSpace.doubleRowSpace;
		m_nLsEndSpace = LsSpace.EndSpace;
	}
	UpdateData(FALSE);
}

void CJointBaseJg::OnChangeLsN()
{
	UpdateData();
	if(m_nLsN>=5)
		m_nLsRows=1;
	else
		m_nLsRows=0;
	int ls_d = atoi(m_sLsGuiGe);
	LSSPACE_STRU LsSpace;
	if(GetLsSpace(LsSpace,ls_d))
	{
		if(m_nLsRows==0)
			m_nLsSpace = LsSpace.SingleRowSpace;
		else
			m_nLsSpace = LsSpace.doubleRowSpace;
		m_nLsEndSpace = LsSpace.EndSpace;
	}
	UpdateData(FALSE);
}
