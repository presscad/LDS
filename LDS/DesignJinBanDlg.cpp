// DesignJinBanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignJinBanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignJinBanDlg dialog


CDesignJinBanDlg::CDesignJinBanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignJinBanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignJinBanDlg)
	m_fParaA = 200;
	m_fParaB = 100;
	m_fParaC = 50;
	m_fParaD = 40;
	m_fParaE = 20;
	m_fParaT = 10;
	//}}AFX_DATA_INIT
}


void CDesignJinBanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignJinBanDlg)
	DDX_Text(pDX, IDC_E_PARA_A, m_fParaA);
	DDX_Text(pDX, IDC_E_PARA_B, m_fParaB);
	DDX_Text(pDX, IDC_E_PARA_C, m_fParaC);
	DDX_Text(pDX, IDC_E_PARA_D, m_fParaD);
	DDX_Text(pDX, IDC_E_PARA_E, m_fParaE);
	DDX_Text(pDX, IDC_E_PARA_T, m_fParaT);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignJinBanDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignJinBanDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignJinBanDlg message handlers
