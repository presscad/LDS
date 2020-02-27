// OffsetPartDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "OffsetPartDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COffsetPartDlg dialog


COffsetPartDlg::COffsetPartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COffsetPartDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COffsetPartDlg)
	m_fBaseOffsetX = 0.0;
	m_fBaseOffsetY = 0.0;
	m_fBaseOffsetZ = 0.0;
	m_fEndOffsetX = 0.0;
	m_fEndOffsetY = 0.0;
	m_fEndOffsetZ = 0.0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sParaName = _T("Datum Point Offset:");
#else 
	m_sParaName = _T("基准点偏移:");
#endif
	m_bRelativeUcs = TRUE;
	m_bAbsoluteUcs = FALSE;
	//}}AFX_DATA_INIT
	part_type = 0;	//板
}


void COffsetPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COffsetPartDlg)
	DDX_Text(pDX, IDC_E_BASE_OFFSET_X, m_fBaseOffsetX);
	DDX_Text(pDX, IDC_E_BASE_OFFSET_Y, m_fBaseOffsetY);
	DDX_Text(pDX, IDC_E_BASE_OFFSET_Z, m_fBaseOffsetZ);
	DDX_Text(pDX, IDC_E_END_OFFSET_X, m_fEndOffsetX);
	DDX_Text(pDX, IDC_E_END_OFFSET_Y, m_fEndOffsetY);
	DDX_Text(pDX, IDC_E_END_OFFSET_Z, m_fEndOffsetZ);
	DDX_Text(pDX, IDC_S_TITLE, m_sParaName);
	DDX_Check(pDX, IDC_CHK_RELATIVE_UCS, m_bRelativeUcs);
	DDX_Check(pDX, IDC_CHK_ABSOLUTE_UCS, m_bAbsoluteUcs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COffsetPartDlg, CDialog)
	//{{AFX_MSG_MAP(COffsetPartDlg)
	ON_BN_CLICKED(IDC_CHK_RELATIVE_UCS, OnChkRelativeUcs)
	ON_BN_CLICKED(IDC_CHK_ABSOLUTE_UCS, OnChkAbsoluteUcs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COffsetPartDlg message handlers

BOOL COffsetPartDlg::OnInitDialog() 
{
	
	if(part_type==0)	//板
	{
		GetDlgItem(IDC_E_END_OFFSET_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_END_OFFSET_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_END_OFFSET_Z)->EnableWindow(FALSE);
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		m_sParaName.Format("Start Ridge Vertice Offset:");
#else 
		m_sParaName.Format("始端楞点偏移:");
#endif
	CDialog::OnInitDialog();
	
	return TRUE;
}

void COffsetPartDlg::OnChkRelativeUcs() 
{
	UpdateData();
	m_bAbsoluteUcs=!m_bRelativeUcs;
	UpdateData(FALSE);
}

void COffsetPartDlg::OnChkAbsoluteUcs() 
{
	UpdateData();
	m_bRelativeUcs=!m_bAbsoluteUcs;
	UpdateData(FALSE);
}
