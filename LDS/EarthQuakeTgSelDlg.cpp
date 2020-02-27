// EarthQuakeTgSelDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "EarthQuakeTgSelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEarthQuakeTgSelDlg dialog


CEarthQuakeTgSelDlg::CEarthQuakeTgSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEarthQuakeTgSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEarthQuakeTgSelDlg)
	m_iDesignGroup = 0;
	m_iEnvType = 1;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sTg = _T("Periodic proper value Tg= 0.35s");
#else 
	m_sTg = _T("特征周期值Tg= 0.35s");
#endif
	//}}AFX_DATA_INIT
	m_fTg=0.35;
}


void CEarthQuakeTgSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEarthQuakeTgSelDlg)
	DDX_CBIndex(pDX, IDC_CMB_DESIGN_GROUP, m_iDesignGroup);
	DDX_CBIndex(pDX, IDC_CMB_ENV_TYPE, m_iEnvType);
	DDX_Text(pDX, IDC_S_TG, m_sTg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEarthQuakeTgSelDlg, CDialog)
	//{{AFX_MSG_MAP(CEarthQuakeTgSelDlg)
	ON_CBN_SELCHANGE(IDC_CMB_DESIGN_GROUP, OnSelchangeCmbInput)
	ON_CBN_SELCHANGE(IDC_CMB_ENV_TYPE, OnSelchangeCmbInput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEarthQuakeTgSelDlg message handlers

void CEarthQuakeTgSelDlg::OnSelchangeCmbInput() 
{
	UpdateData();
	double tg_arr[3][4]={
		{0.25,0.35,0.45,0.65},
		{0.30,0.40,0.55,0.75},
		{0.35,0.45,0.65,0.90},};
	m_fTg=tg_arr[m_iDesignGroup][m_iEnvType];
#ifdef AFX_TARG_ENU_ENGLISH
		m_sTg.Format("Periodic proper value Tg= %.2fs",m_fTg);
#else 
	m_sTg.Format("特征周期值Tg= %.2fs",m_fTg);
#endif
	UpdateData(FALSE);
}
