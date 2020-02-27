// AddLeiBanDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "AddLeiBanDlg.h"
#include "Query.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddLeiBanDlg dialog


CAddLeiBanDlg::CAddLeiBanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddLeiBanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddLeiBanDlg)
	m_iHigh = 100;
	m_iThick = 16;
	m_sPartNo = _T("");
	m_nParaA = 20;
	m_nParaB = 20;
	m_iMaterial = 0;
	m_fGradientAngle = 90.0;
	m_fLeiBanWide = 0.0;
	m_fLeiBanRayDist = 0.0;
	m_fWaterSlotWide = 50.0;
	m_sDatumXueBan = _T("0");
	m_bEnableSecondXueBan = FALSE;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCaption = "Add Rib Plate";
#else 
	m_sCaption = "Ìí¼ÓÀß°å";
#endif
	m_bCircleDiBan=FALSE;
}


void CAddLeiBanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddLeiBanDlg)
	DDX_Text(pDX, IDC_E_HIGH, m_iHigh);
	DDX_Text(pDX, IDC_E_THICK, m_iThick);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDV_MaxChars(pDX, m_sPartNo, 6);
	DDX_Text(pDX, IDC_E_DES_PARA_A, m_nParaA);
	DDX_Text(pDX, IDC_E_DES_PARA_B, m_nParaB);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_DES_GRADIENT_ANGLE, m_fGradientAngle);
	DDV_MinMaxDouble(pDX, m_fGradientAngle, 0., 180.);
	DDX_Text(pDX, IDC_E_LEIBAN_WIDE, m_fLeiBanWide);
	DDX_Text(pDX, IDC_E_RAY_DIST, m_fLeiBanRayDist);
	DDX_Text(pDX, IDC_E_WATER_SLOT_WIDE, m_fWaterSlotWide);
	DDX_CBString(pDX, IDC_CMB_DATUM_XUEBAN, m_sDatumXueBan);
	DDX_Check(pDX, IDC_CHK_ENABLE_SEC_XUEBAN, m_bEnableSecondXueBan);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddLeiBanDlg, CDialog)
	//{{AFX_MSG_MAP(CAddLeiBanDlg)
	ON_BN_CLICKED(IDC_CHK_ENABLE_SEC_XUEBAN, OnChkEnableSecXueban)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddLeiBanDlg message handlers

BOOL CAddLeiBanDlg::OnInitDialog() 
{
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	AddSteelMatRecord(pCMB);
	CDialog::OnInitDialog();
	SetWindowText(m_sCaption);
	GetDlgItem(IDC_E_LEIBAN_WIDE)->EnableWindow(!m_bEnableSecondXueBan);
	OnChkEnableSecXueban();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddLeiBanDlg::OnChkEnableSecXueban() 
{
	UpdateData();
	GetDlgItem(IDC_E_LEIBAN_WIDE)->EnableWindow(!m_bEnableSecondXueBan);
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_DATUM_XUEBAN);
	if(m_bEnableSecondXueBan)
	{
		while(pCMB->GetCount()>0)
			pCMB->DeleteString(0);
		pCMB->AddString(" 22.5");
		pCMB->AddString(" 67.5");
		pCMB->AddString("112.5");
		pCMB->AddString("157.5");
		pCMB->AddString("202.5");
		pCMB->AddString("247.5");
		pCMB->AddString("292.5");
		pCMB->AddString("337.5");
	}
	else if(m_bCircleDiBan)
	{
		while(pCMB->GetCount()>0)
			pCMB->DeleteString(0);
		pCMB->AddString("  0");
		pCMB->AddString(" 45");
		pCMB->AddString(" 90");
		pCMB->AddString("135");
		pCMB->AddString("180");
		pCMB->AddString("225");
		pCMB->AddString("270");
		pCMB->AddString("315");
	}
	else
	{
		while(pCMB->GetCount()>0)
			pCMB->DeleteString(0);
		pCMB->AddString("  0");
		pCMB->AddString(" 90");
		pCMB->AddString("180");
		pCMB->AddString("270");
	}
}
