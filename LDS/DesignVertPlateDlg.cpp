// DesignVertPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignVertPlateDlg.h"
#include "Query.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignVertPlateDlg dialog


CDesignVertPlateDlg::CDesignVertPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignVertPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignVertPlateDlg)
	m_iMaterial = 0;
	m_sSegI = _T("");
	m_nThick = 8;
	m_fWidth = 0;
	m_fLength = 0;
	m_sPartNo = _T("");
	//}}AFX_DATA_INIT
}


void CDesignVertPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignVertPlateDlg)
	DDX_Control(pDX, IDC_CMB_PLANK_MATERIAL, m_listMaterial);
	DDX_CBIndex(pDX, IDC_CMB_PLANK_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_THICK, m_nThick);
	DDX_Text(pDX, IDC_E_WIDTH, m_fWidth);
	DDX_Text(pDX, IDC_E_LENGTH, m_fLength);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignVertPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignVertPlateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignVertPlateDlg message handlers

BOOL CDesignVertPlateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CComboBox* pCMB=(CComboBox*)GetDlgItem(IDC_CMB_PLANK_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iMaterial);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
