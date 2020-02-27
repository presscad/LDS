// CalWindBetaZDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "CalWindBetaZDlg.h"
#include ".\\LdsModel\\WindLoad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if defined(__TSA_)||defined(__LDS_)

/////////////////////////////////////////////////////////////////////////////
// CCalWindBetaZDlg dialog


CCalWindBetaZDlg::CCalWindBetaZDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCalWindBetaZDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCalWindBetaZDlg)
	m_bSpecWindComp = FALSE;
	m_fBaseVibrationPeriod = 0.1;
	m_fBottomWidth = 7.0;
	m_fBaseWindComp = 0.1;
	m_fSegHighPos = 0.1;
	m_fSegWidth = 1.5;
	m_fTopWidth = 1.4;
	m_fTotalHigh = 40.0;
	m_fWindBetaZ = 0.1;
	m_fWindVelocity = 30;
	m_iTowerSupportType = 1;
	m_iBetaZCalType = 0;
	m_fWindBetaZForFoundation = 0.0;
	//}}AFX_DATA_INIT
}


void CCalWindBetaZDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalWindBetaZDlg)
	DDX_Check(pDX, IDC_CHK_SPEC_WIND_COMP, m_bSpecWindComp);
	DDX_Text(pDX, IDC_E_BASE_VIBRATION_PERIOD, m_fBaseVibrationPeriod);
	DDX_Text(pDX, IDC_E_BOTTOM_WIDTH, m_fBottomWidth);
	DDV_MinMaxDouble(pDX, m_fBottomWidth, 1.e-003, 1000.);
	DDX_Text(pDX, IDC_E_BASE_WIND_COMP, m_fBaseWindComp);
	DDX_Text(pDX, IDC_E_SEG_HIGH_POS, m_fSegHighPos);
	DDV_MinMaxDouble(pDX, m_fSegHighPos, 1., 1000.);
	DDX_Text(pDX, IDC_E_SEG_WIDTH, m_fSegWidth);
	DDV_MinMaxDouble(pDX, m_fSegWidth, 1.e-003, 1000.);
	DDX_Text(pDX, IDC_E_TOP_WIDTH, m_fTopWidth);
	DDV_MinMaxDouble(pDX, m_fTopWidth, 1.e-003, 1000.);
	DDX_Text(pDX, IDC_E_TOTAL_HIGH, m_fTotalHigh);
	DDV_MinMaxDouble(pDX, m_fTotalHigh, 1., 1000.);
	DDX_Text(pDX, IDC_E_WIND_BETA_Z, m_fWindBetaZ);
	DDX_Text(pDX, IDC_E_WIND_VELOCITY, m_fWindVelocity);
	DDX_Radio(pDX, IDC_RDO_TOWER_TYPE, m_iTowerSupportType);
	DDX_Radio(pDX, IDC_RDO_BETA_Z_CAL_TYPE, m_iBetaZCalType);
	DDX_Text(pDX, IDC_E_WIND_BETA_Z_FOR_FOUNDATION, m_fWindBetaZForFoundation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalWindBetaZDlg, CDialog)
	//{{AFX_MSG_MAP(CCalWindBetaZDlg)
	ON_EN_CHANGE(IDC_E_TOTAL_HIGH, OnChangeETotalHigh)
	ON_EN_CHANGE(IDC_E_BASE_VIBRATION_PERIOD, OnChangeEBaseVibrationPeriod)
	ON_EN_CHANGE(IDC_E_SEG_HIGH_POS, OnChangeESegHighPos)
	ON_EN_CHANGE(IDC_E_SEG_WIDTH, OnChangeESegWidth)
	ON_EN_CHANGE(IDC_E_TOP_WIDTH, OnChangeETopWidth)
	ON_EN_CHANGE(IDC_E_BOTTOM_WIDTH, OnChangeEBottomWidth)
	ON_EN_CHANGE(IDC_E_WIND_VELOCITY, OnChangeEWindVelocity)
	ON_EN_CHANGE(IDC_E_BASE_WIND_COMP, OnChangeEBaseWindComp)
	ON_BN_CLICKED(IDC_CHK_SPEC_WIND_COMP, OnChkSpecWindComp)
	ON_BN_CLICKED(IDC_RDO_TOWER_TYPE, OnRdoTowerType)
	ON_BN_CLICKED(IDC_RDO_BETA_Z_CAL_TYPE2, OnRdoBetaZCalType)
	ON_BN_CLICKED(IDC_RDO_TOWER_TYPE2, OnRdoTowerType)
	ON_BN_CLICKED(IDC_RDO_BETA_Z_CAL_TYPE3, OnRdoBetaZCalType)
	ON_BN_CLICKED(IDC_RDO_BETA_Z_CAL_TYPE, OnRdoBetaZCalType)
	ON_BN_CLICKED(IDC_CHK_SPEC_WIND_VELOCITY, OnChkSpecWindVelocity)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalWindBetaZDlg message handlers

BOOL CCalWindBetaZDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(!m_bSpecWindComp)
	{
		m_fBaseWindComp=m_fWindVelocity*m_fWindVelocity/1600;
		if(Ta.volt_grade!=0)
			m_fBaseWindComp*=Find_MuZ(10,Ta.m_cLandscape);
	}
	
	RefreshDispStatus(true);
	if(Ta.m_fVibrationT1>0)
		m_fBaseVibrationPeriod=Ta.m_fVibrationT1;
	else
		m_fBaseVibrationPeriod=PERIOD_COEF*m_fTotalHigh;
	CalBetaZ();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCalWindBetaZDlg::RefreshDispStatus(bool bUpdateChkBox/*=false*/)
{
	if(bUpdateChkBox)
	{
		CButton *pChkWindType=(CButton*)GetDlgItem(IDC_CHK_SPEC_WIND_VELOCITY);
		if(m_bSpecWindComp)
			pChkWindType->SetCheck(BST_UNCHECKED);
		else
			pChkWindType->SetCheck(BST_CHECKED);
	}
	GetDlgItem(IDC_E_TOTAL_HIGH)->EnableWindow(m_iBetaZCalType!=0);
	GetDlgItem(IDC_E_WIND_VELOCITY)->EnableWindow(!m_bSpecWindComp&&m_iBetaZCalType==1);
	GetDlgItem(IDC_E_BASE_WIND_COMP)->EnableWindow(m_bSpecWindComp&&m_iBetaZCalType==1);
	GetDlgItem(IDC_E_BASE_VIBRATION_PERIOD)->EnableWindow(m_iBetaZCalType==1);
	GetDlgItem(IDC_E_SEG_HIGH_POS)->EnableWindow(m_iBetaZCalType!=2);
	GetDlgItem(IDC_E_SEG_WIDTH)->EnableWindow(m_iBetaZCalType==1);
	GetDlgItem(IDC_E_TOP_WIDTH)->EnableWindow(m_iBetaZCalType==1);
	GetDlgItem(IDC_E_BOTTOM_WIDTH)->EnableWindow(m_iBetaZCalType==1);
	GetDlgItem(IDC_CHK_SPEC_WIND_VELOCITY)->EnableWindow(m_iBetaZCalType==1);
	GetDlgItem(IDC_CHK_SPEC_WIND_COMP)->EnableWindow(m_iBetaZCalType==1);
	GetDlgItem(IDC_RDO_TOWER_TYPE)->EnableWindow(m_iBetaZCalType==2);
	GetDlgItem(IDC_RDO_TOWER_TYPE2)->EnableWindow(m_iBetaZCalType==2);
	UpdateData(FALSE);
}

void CCalWindBetaZDlg::OnChangeETotalHigh() 
{
	double fBefore=m_fTotalHigh;
	UpdateData();
	if(fBefore==m_fTotalHigh)
		return;
	if(m_iBetaZCalType==1&&Ta.m_fVibrationT1<=0)
		m_fBaseVibrationPeriod=PERIOD_COEF*m_fTotalHigh;
	CalBetaZ();
}

void CCalWindBetaZDlg::OnChangeEBaseVibrationPeriod() 
{
	double fBefore=m_fBaseVibrationPeriod;
	UpdateData();
	if(fBefore==m_fBaseVibrationPeriod)
		return;
	CalBetaZ();
}

void CCalWindBetaZDlg::OnChangeESegHighPos() 
{
	double fBefore=m_fSegHighPos;
	UpdateData();
	if(fBefore==m_fSegHighPos)
		return;
	CalBetaZ();
}

void CCalWindBetaZDlg::OnChangeESegWidth() 
{
	double fBefore=m_fSegWidth;
	UpdateData();
	if(fBefore==m_fSegWidth)
		return;
	CalBetaZ();
}

void CCalWindBetaZDlg::OnChangeETopWidth() 
{
	double fBefore=m_fTopWidth;
	UpdateData();
	if(fBefore==m_fTopWidth)
		return;
	CalBetaZ();
}

void CCalWindBetaZDlg::OnChangeEBottomWidth() 
{
	double fBefore=m_fBottomWidth;
	UpdateData();
	if(fBefore==m_fBottomWidth)
		return;
	CalBetaZ();
}

void CCalWindBetaZDlg::OnChangeEWindVelocity() 
{
	double fBefore=m_fWindVelocity;
	UpdateData();
	if(fBefore==m_fWindVelocity)
		return;
	if(!m_bSpecWindComp)
	{
		m_fBaseWindComp=m_fWindVelocity*m_fWindVelocity/1600;
		if(Ta.volt_grade!=0)
			m_fBaseWindComp*=Find_MuZ(10,Ta.m_cLandscape);
		CalBetaZ();
	}
	UpdateData(FALSE);
}

void CCalWindBetaZDlg::OnChangeEBaseWindComp() 
{
	double fBefore=m_fBaseWindComp;
	UpdateData();
	if(fBefore==m_fBaseWindComp)
		return;
	CalBetaZ();
}

void CCalWindBetaZDlg::OnChkSpecWindVelocity() 
{
	CButton *pChkWindType=(CButton*)GetDlgItem(IDC_CHK_SPEC_WIND_VELOCITY);
	if(pChkWindType->GetCheck()==BST_CHECKED)
		m_bSpecWindComp=FALSE;
	else
		m_bSpecWindComp=TRUE;
	RefreshDispStatus();
}

void CCalWindBetaZDlg::OnChkSpecWindComp() 
{
	UpdateData();
	RefreshDispStatus(true);
}

void CCalWindBetaZDlg::OnRdoTowerType() 
{
	if(m_iBetaZCalType==2)
	{
		UpdateData();
		m_fWindBetaZ=Find_BetaZ(m_fTotalHigh,m_iTowerSupportType);
		if(m_fTotalHigh<-60)
			m_fWindBetaZForFoundation=1.0;
		else
			m_fWindBetaZForFoundation=1.3;
		UpdateData(FALSE);
	}
}

void CCalWindBetaZDlg::OnRdoBetaZCalType() 
{
	UpdateData();
	RefreshDispStatus(true);
}

void CCalWindBetaZDlg::CalBetaZ()
{
	if(m_iBetaZCalType==0)
	{	//根据风压段高插值取值
		m_fWindBetaZ=g_sysPara.Beta_z_InterpByHigh(m_fSegHighPos,false);
		m_fWindBetaZForFoundation=g_sysPara.Beta_z_InterpByHigh(m_fSegHighPos,true);
	}
	else if(m_iBetaZCalType==1)
	{	//根据风压段计算取值
		double epsilon=QuerySteelEpsilon(m_fBaseWindComp*m_fBaseVibrationPeriod*m_fBaseVibrationPeriod);
		double V=QueryVAffectCoef(m_fTotalHigh,Ta.m_cLandscape);
		double BH_to_B0=m_fTopWidth/m_fBottomWidth;
		double Bz_to_B0=m_fSegWidth/m_fBottomWidth;
		double v_r=QueryReviseVCoef(BH_to_B0)*Bz_to_B0;
		double fai_z=QueryVibrationType1Coef(m_fSegHighPos/m_fTotalHigh,BH_to_B0);
		double mu_z=Find_MuZ(m_fSegHighPos,Ta.m_cLandscape);
		m_fWindBetaZ=1+epsilon*V*v_r*fai_z/mu_z;
		m_fWindBetaZForFoundation=g_sysPara.Beta_z_InterpByHigh(m_fSegHighPos,true);
	}
	else	//根据全塔高取统一值
	{
		m_fWindBetaZ=Find_BetaZ(m_fTotalHigh,m_iTowerSupportType);
		if(m_fTotalHigh<-60)
			m_fWindBetaZForFoundation=1.0;
		else
			m_fWindBetaZForFoundation=1.3;
	}
	UpdateData(FALSE);
}
#endif
