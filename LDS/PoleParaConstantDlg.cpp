// PoleParaConstantDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "PoleParaConstantDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPoleParaConstantDlg dialog


CPoleParaConstantDlg::CPoleParaConstantDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPoleParaConstantDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPoleParaConstantDlg)
	m_fArea = 0.0;
	m_fPara1 = 0.0;
	m_fPara2 = 0.0;
	m_fUnitWeight = 0.0;
	m_fYangModulus = 206000.0;
	m_fDesCompStrength = 0.0;
	m_fDesTensStrength = 0.0;
	m_fPreStress = 0.0;
	//}}AFX_DATA_INIT
}


void CPoleParaConstantDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPoleParaConstantDlg)
	DDX_Text(pDX, IDC_E_AREA, m_fArea);
	DDX_Text(pDX, IDC_E_PARA1, m_fPara1);
	DDX_Text(pDX, IDC_E_PARA2, m_fPara2);
	DDX_Text(pDX, IDC_E_UNIT_WEIGHT, m_fUnitWeight);
	DDX_Text(pDX, IDC_E_YANG_MODULUS, m_fYangModulus);
	DDX_Text(pDX, IDC_E_DES_COMP_STRENGTH, m_fDesCompStrength);
	DDX_Text(pDX, IDC_E_DES_TENS_STRENGTH, m_fDesTensStrength);
	DDX_Text(pDX, IDC_E_PRE_STRESS, m_fPreStress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPoleParaConstantDlg, CDialog)
	//{{AFX_MSG_MAP(CPoleParaConstantDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPoleParaConstantDlg message handlers
