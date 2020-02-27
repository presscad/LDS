// BeamPartSelMatDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "BeamPartSelMatDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "./Codes/Codes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBeamPartSelMatDlg dialog

#ifdef __PART_DESIGN_INC_
CBeamPartSelMatDlg::CBeamPartSelMatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBeamPartSelMatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBeamPartSelMatDlg)
	m_sLsHoleDiameter = _T("17.5");
	m_iMat = 0;
	m_sPartSpec = _T("40X3");
	m_fAxisLoadForce = 0.0;
	m_fDeductLsHoleCoef = 1.0;
	m_fSimpleBeamLength = 0.0;
	m_fVerticalLoadForce = 1000.0;
	m_fVerticalLoadPosition = 0.0;
	m_iGyrationStyle = 0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCheckResult = _T("Check Result:");
#else 
	m_sCheckResult = _T("验算结果:");
#endif
	//}}AFX_DATA_INIT
}


void CBeamPartSelMatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBeamPartSelMatDlg)
	DDX_CBString(pDX, IDC_CMB_LS_HOLE_DIAMETER, m_sLsHoleDiameter);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMat);
	DDX_CBString(pDX, IDC_CMB_PART_SPEC, m_sPartSpec);
	DDX_Text(pDX, IDC_E_AXIS_LOAD_FORCE, m_fAxisLoadForce);
	DDX_Text(pDX, IDC_E_DEDUCT_LS_COEF, m_fDeductLsHoleCoef);
	DDX_Text(pDX, IDC_E_SIMPLE_BEAM_LENGTH, m_fSimpleBeamLength);
	DDX_Text(pDX, IDC_E_VERTICAL_LOAD_FORCE, m_fVerticalLoadForce);
	DDX_Text(pDX, IDC_E_VERTICAL_LOAD_POSITION, m_fVerticalLoadPosition);
	DDX_Radio(pDX, IDC_RDO_GYRATION_STYLE, m_iGyrationStyle);
	DDX_Text(pDX, IDC_S_CHECK_RESULT, m_sCheckResult);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBeamPartSelMatDlg, CDialog)
	//{{AFX_MSG_MAP(CBeamPartSelMatDlg)
	ON_BN_CLICKED(IDC_BN_CHECK_PART, OnBnCheckPart)
	ON_EN_CHANGE(IDC_E_AXIS_LOAD_FORCE, OnChangeEAxisLoadForce)
	ON_EN_CHANGE(IDC_E_SIMPLE_BEAM_LENGTH, OnChangeESimpleBeamLength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBeamPartSelMatDlg message handlers

BOOL CBeamPartSelMatDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_PART_SPEC);
	AddJgRecord(pCMB);
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	AddSteelMatRecord(pCMB);
	int iSel = pCMB->FindString(0,m_sPartSpec);
	if(iSel>=0)
		pCMB->SetCurSel(iSel);
	OnChangeEAxisLoadForce();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBeamPartSelMatDlg::OnBnCheckPart() 
{
	UpdateData();
	IMemberDesignCode* pDesignCode=Ta.DesignCode();
	double wing_wide,wing_thick;
	restore_JG_guige(m_sPartSpec,wing_wide,wing_thick);
	JG_PARA_TYPE *pJgType = FindJgType(wing_wide,wing_thick);
	if(pJgType==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
		m_sCheckResult = "Check result:The input specification is wrong！";
#else 
		m_sCheckResult = "验算结果:输入规格有误！";
#endif
	STEELMAT matType=CSteelMatLibrary::RecordAt(m_iMat);
	double ft=matType.f(wing_thick);
	double fy=matType.fy(wing_thick);
	//弯矩单位 N*m
	double M = 0.001*m_fVerticalLoadForce*m_fVerticalLoadPosition*(m_fSimpleBeamLength-m_fVerticalLoadPosition)/m_fSimpleBeamLength;
	double W=pJgType->Wx;	// 默认为角钢平行轴(cm^3)
	if(m_iGyrationStyle==1)	// 最小轴
		W= pJgType->Wy0;
	double bear_coef=0;
	if(m_fAxisLoadForce<EPS)	//单位,N
	{	//轴向受压
		double lamda;
		if(m_iGyrationStyle==0)
			lamda = m_fSimpleBeamLength/pJgType->Rx/10;
		else
			lamda = m_fSimpleBeamLength/pJgType->Ry0/10;
		double fai=pDesignCode->CompStableCoef('B',lamda,&matType,wing_thick);
		//角钢类
		double t=wing_thick;
		double b=pJgType->wing_wide-pJgType->r-t;
		double mN=pDesignCode->CompReduceCoef(b,t,ft,fy,lamda,false,true);	//压杆稳定强度折减系数
		bear_coef=pDesignCode->BendingCompRodCheck(m_fAxisLoadForce,pJgType->area*100,mN,fai,M,W,lamda,ft);
	}
	else if(m_fAxisLoadForce>0)
	{	//轴向受拉
		double m=0.7;	//肢宽40以上规格角钢单肢连接
		if(wing_wide<=40)
			m=0.55;
		double d = atof(m_sLsHoleDiameter)/10;
		double An = pJgType->area*100-m_fDeductLsHoleCoef*(d*wing_thick);
		bear_coef=pDesignCode->BendingTensionRodCheck(m,m_fAxisLoadForce,An,M,W,ft);
	}
	double max_sigema=bear_coef*ft;	// 单位为MPa
#ifdef AFX_TARG_ENU_ENGLISH
	if(bear_coef>1.0)
		m_sCheckResult.Format("Check result:The max stress of physical theory is %.0fMPa.Beyond the material strength is(%.0fMPa)！",max_sigema,ft);
	else
		m_sCheckResult.Format("Check result:The max stress of physical theory is %.0fMPa,Beyond the material strength is(%.0fMPa)！",max_sigema,ft);
#else 
	if(bear_coef>1.0)
		m_sCheckResult.Format("验算结果:实际理论最大应力为%.0fMPa,超出材料强度值(%.0fMPa)！",max_sigema,ft);
	else
		m_sCheckResult.Format("验算结果:实际理论最大应力为%.0fMPa,符合材料强度值(%.0fMPa)！",max_sigema,ft);
#endif
	UpdateData(FALSE);
}
/*{
	UpdateData();
	IMemberDesignCode* pDesignCode=Ta.DesignCode();
	double wing_wide,wing_thick;
	restore_JG_guige(m_sPartSpec.GetBuffer(),wing_wide,wing_thick);
	JG_PARA_TYPE *pJgType = FindJgType(wing_wide,wing_thick);
	if(pJgType==NULL)
		m_sCheckResult = "验算结果:输入规格有误！";
	STEELMAT matType=CSteelMatLibrary::RecordAt(m_iMat);
	double M = 0.001*m_fVerticalLoadForce*m_fVerticalLoadPosition*(m_fSimpleBeamLength-m_fVerticalLoadPosition)/m_fSimpleBeamLength;
	double bend_sigema,max_sigema;	// 单位为MPa
	if(m_iGyrationStyle==0)	//平行轴
		bend_sigema= fabs(M)/pJgType->Wx;
	else	//最小轴
		bend_sigema= fabs(M)/pJgType->Wy0;
	max_sigema = bend_sigema;
	if(m_fAxisLoadForce<0)
	{	//轴向受压
		//pDesignCode->BendingCompRodCheck(m_fAxisLoadForce,pJgType->area*100,mN,fai,M,W,lamda,matType.f(t));
		double lamda;
		if(m_iGyrationStyle==0)
			lamda = m_fSimpleBeamLength/pJgType->Rx/10;
		else
			lamda = m_fSimpleBeamLength/pJgType->Ry0/10;
		int index=min(ftoi(lamda),250);
		double fai;
		if(m_iMat==0)	//Q235
			fai=Q235B_STABLE_COEF[index];
		else if(m_iMat==1)	//Q345
			fai=Q345B_STABLE_COEF[index];
		else	//Q390
			fai=Q390B_STABLE_COEF[index];
		double mN;	//压杆稳定强度折减系数
		//角钢类
		double t=wing_thick;
		double b=pJgType->wing_wide-pJgType->r-t;
		double bt_lim=202/sqrt(matType.f(t));
		if(b/t<=bt_lim)
			mN=1.0;
		else
			mN=1.677-0.677*(b/t)/bt_lim;
		max_sigema -= m_fAxisLoadForce/(fai*mN*pJgType->area*100);
	}
	else if(m_fAxisLoadForce>0)
	{	//轴向受拉
		double m=0.7;	//肢宽40以上规格角钢单肢连接
		if(wing_wide<=40)
			m=0.55;
		double d = atof(m_sLsHoleDiameter)/10;
		double An = pJgType->area-m_fDeductLsHoleCoef*d;
		max_sigema += m_fAxisLoadForce/(m*An*100);
	}
	if(max_sigema>matType.f(wing_thick))
		m_sCheckResult.Format("验算结果:实际理论最大应力为%.0fMPa,超出材料强度值(%.0fMPa)！",max_sigema,matType.f(wing_thick));
	else
		m_sCheckResult.Format("验算结果:实际理论最大应力为%.0fMPa,符合材料强度值(%.0fMPa)！",max_sigema,matType.f(wing_thick));
	UpdateData(FALSE);
}*/
void CBeamPartSelMatDlg::OnChangeEAxisLoadForce() 
{
	UpdateData();
	if(m_fAxisLoadForce>0)
	{
		GetDlgItem(IDC_E_DEDUCT_LS_COEF)->EnableWindow(TRUE);
		GetDlgItem(IDC_CMB_LS_HOLE_DIAMETER)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_E_DEDUCT_LS_COEF)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMB_LS_HOLE_DIAMETER)->EnableWindow(FALSE);
	}
}

void CBeamPartSelMatDlg::OnChangeESimpleBeamLength() 
{
	UpdateData();
	m_fVerticalLoadPosition = m_fSimpleBeamLength/2.0;
	UpdateData(FALSE);
}
#endif