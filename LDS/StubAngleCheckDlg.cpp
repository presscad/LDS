// StubAngleCheckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "StubAngleCheckDlg.h"
#include "Query.h"
#include "LibraryQuery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __PART_DESIGN_INC_
/////////////////////////////////////////////////////////////////////////////
// CStubAngleCheckDlg dialog

CStubAngleCheckDlg::CStubAngleCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStubAngleCheckDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStubAngleCheckDlg)
	m_iBoltD = 1;
	m_iBoltGrade = 1;
	m_fBoltCanCompForce = 0.0;
	m_fBoltCanShearForce = 0.0;
	m_nBoltN = 0;
	m_fMinStubDepth = 0.0;
	m_fMinStubLength = 0.0;
	m_fForceN = 0.0;
	m_fShearForce = 0.0;
	m_sCalResult = _T("");
	m_fStubAngleHoleOut = 2.0;
	m_sStubAngleSize = _T("140X12");
	m_sAnchorAngleSize = _T("140X12");
	m_fAnchorAngleLength = 0.0;
	m_nAnchorAngleN = 0;
	m_iConcreteType = 2;
	m_iAnchorMat = 0;
	m_iStubMat = 1;
	m_fConcreteFc = 0.0;
	//}}AFX_DATA_INIT
}


void CStubAngleCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStubAngleCheckDlg)
	DDX_CBIndex(pDX, IDC_CMB_BOLT_D, m_iBoltD);
	DDX_CBIndex(pDX, IDC_CMB_BOLT_GRADE, m_iBoltGrade);
	DDX_Text(pDX, IDC_E_BOLT_CAN_COMP_FORCE, m_fBoltCanCompForce);
	DDX_Text(pDX, IDC_E_BOLT_CAN_SHEAR_FORCE, m_fBoltCanShearForce);
	DDX_Text(pDX, IDC_E_BOLT_N, m_nBoltN);
	DDX_Text(pDX, IDC_E_MIN_STUB_DEPTH, m_fMinStubDepth);
	DDX_Text(pDX, IDC_E_MIN_STUB_LENGTH, m_fMinStubLength);
	DDX_Text(pDX, IDC_E_N_FORCE, m_fForceN);
	DDX_Text(pDX, IDC_E_SHREAR_FORCE, m_fShearForce);
	DDX_Text(pDX, IDC_S_CAL_RESULT, m_sCalResult);
	DDX_Text(pDX, IDC_E_STUB_ANGLE_HOLE_OUT, m_fStubAngleHoleOut);
	DDX_CBString(pDX, IDC_CMB_STUB_ANGLE_SIZE, m_sStubAngleSize);
	DDX_CBString(pDX, IDC_CMB_ANCHOR_ANGLE_SIZE, m_sAnchorAngleSize);
	DDX_Text(pDX, IDC_E_ANCHOR_ANGLE_LENGTH, m_fAnchorAngleLength);
	DDX_Text(pDX, IDC_E_ANCHOR_ANGLE_N, m_nAnchorAngleN);
	DDX_CBIndex(pDX, IDC_CMB_CONCRETE_TYPE, m_iConcreteType);
	DDX_CBIndex(pDX, IDC_CMB_ANCHOR_MAT, m_iAnchorMat);
	DDX_CBIndex(pDX, IDC_CMB_STUB_MAT, m_iStubMat);
	DDX_Text(pDX, IDC_E_CONCRETE_FC, m_fConcreteFc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStubAngleCheckDlg, CDialog)
	//{{AFX_MSG_MAP(CStubAngleCheckDlg)
	ON_CBN_SELCHANGE(IDC_CMB_BOLT_GRADE, OnSelchangeCmbBoltGrade)
	ON_CBN_SELCHANGE(IDC_CMB_BOLT_D, OnSelchangeCmbBoltD)
	ON_BN_CLICKED(ID_BN_CAL_CHECK, OnBnCalCheck)
	ON_CBN_SELCHANGE(IDC_CMB_ANCHOR_ANGLE_SIZE, OnSelchangeCmbAngleSize)
	ON_CBN_SELCHANGE(IDC_CMB_STUB_ANGLE_SIZE, OnSelchangeCmbStubAngleSize)
	ON_CBN_SELCHANGE(IDC_CMB_CONCRETE_TYPE, OnSelchangeCmbConcreteType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStubAngleCheckDlg message handlers

BOOL CStubAngleCheckDlg::OnInitDialog() 
{
	CComboBox* pCMBJg = (CComboBox*)GetDlgItem(IDC_CMB_STUB_ANGLE_SIZE);
	AddJgRecord(pCMBJg);
	pCMBJg = (CComboBox*)GetDlgItem(IDC_CMB_ANCHOR_ANGLE_SIZE);
	AddJgRecord(pCMBJg);
	CDialog::OnInitDialog();
	
	OnSelchangeCmbAngleSize();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStubAngleCheckDlg::OnSelchangeCmbAngleSize() 
{
	UpdateData();
	CComboBox* pCMBJg = (CComboBox*)GetDlgItem(IDC_CMB_STUB_ANGLE_SIZE);
	int iCurSel=pCMBJg->GetCurSel();
	pCMBJg->GetLBText(iCurSel,m_sStubAngleSize);
	pCMBJg = (CComboBox*)GetDlgItem(IDC_CMB_ANCHOR_ANGLE_SIZE);
	iCurSel=pCMBJg->GetCurSel();
	pCMBJg->GetLBText(iCurSel,m_sAnchorAngleSize);
	double wing_wide,wing_thick_stub,wing_thick_anchor;
	char cSep;
	sscanf(m_sStubAngleSize,"%lf%C%lf",&wing_wide,&cSep,&wing_thick_stub);
	sscanf(m_sAnchorAngleSize,"%lf%C%lf",&wing_wide,&cSep,&wing_thick_anchor);
	char grade[8]="";
	if(m_iBoltGrade==0)
		strcpy(grade,"4.8");
	else if(m_iBoltGrade==1)
		strcpy(grade,"6.8");
	else if(m_iBoltGrade==2)
		strcpy(grade,"8.8");
	else //if(m_iBoltGrade==3)
		strcpy(grade,"10.9");
	LS_GRADE *pLsType=FindLsMatType(grade);
	double bolt_d=0;
	if(m_iBoltD==0)
		bolt_d=16;
	else if(m_iBoltD==1)
		bolt_d=20;
	else
		bolt_d=24;
	double wing_thick=min(wing_thick_stub,wing_thick_anchor);
	m_fBoltCanShearForce=Pi*bolt_d*bolt_d*0.25*pLsType->fv*0.001;
	m_fBoltCanCompForce=bolt_d*wing_thick*pLsType->fc*0.001;
	UpdateData(FALSE);
}

void CStubAngleCheckDlg::OnSelchangeCmbStubAngleSize() 
{
	OnSelchangeCmbAngleSize();
	CComboBox* pCMBJg = (CComboBox*)GetDlgItem(IDC_CMB_STUB_ANGLE_SIZE);
	int iCurSel=pCMBJg->GetCurSel();
	pCMBJg->GetLBText(iCurSel,m_sStubAngleSize);
	double wing_wide,wing_thick_stub;
	char cSep;
	sscanf(m_sStubAngleSize,"%lf%C%lf",&wing_wide,&cSep,&wing_thick_stub);
	m_fAnchorAngleLength=wing_wide-wing_thick_stub;
	UpdateData(FALSE);
}

void CStubAngleCheckDlg::OnSelchangeCmbBoltGrade() 
{
	OnSelchangeCmbAngleSize();
}

void CStubAngleCheckDlg::OnSelchangeCmbBoltD() 
{
	OnSelchangeCmbAngleSize();
}

struct CONCRETE_FC{
	char mark[4];
	double fc;
};
CONCRETE_FC concrete_tbl[14]=
{
	"C15", 7.2,
	"C20", 9.6,
	"C25",11.9,
	"C30",14.3,
	"C35",16.7,
	"C40",19.1,
	"C45",21.2,
	"C50",23.1,
	"C55",25.3,
	"C60",27.5,
	"C65",29.7,
	"C70",31.8,
	"C75",33.9,
	"C80",35.9,
};

void CStubAngleCheckDlg::OnBnCalCheck() 
{
	UpdateData();
	CComboBox* pCMBJg = (CComboBox*)GetDlgItem(IDC_CMB_STUB_ANGLE_SIZE);
	int iCurSel=pCMBJg->GetCurSel();
	pCMBJg->GetLBText(iCurSel,m_sStubAngleSize);
	pCMBJg = (CComboBox*)GetDlgItem(IDC_CMB_ANCHOR_ANGLE_SIZE);
	iCurSel=pCMBJg->GetCurSel();
	pCMBJg->GetLBText(iCurSel,m_sAnchorAngleSize);
	double wing_wide_stub,wing_wide_anchor,wing_thick_stub,wing_thick_anchor;
	char cSep;
	sscanf(m_sStubAngleSize,"%lf%C%lf",&wing_wide_stub,&cSep,&wing_thick_stub);
	sscanf(m_sAnchorAngleSize,"%lf%C%lf",&wing_wide_anchor,&cSep,&wing_thick_anchor);
	char grade[8]="";
	if(m_iBoltGrade==0)
		strcpy(grade,"4.8");
	else if(m_iBoltGrade==1)
		strcpy(grade,"6.8");
	else if(m_iBoltGrade==2)
		strcpy(grade,"8.8");
	else //if(m_iBoltGrade==3)
		strcpy(grade,"10.9");
	LS_GRADE *pLsType=FindLsMatType(grade);
	double bolt_d=0;
	if(m_iBoltD==0)
		bolt_d=16;
	else if(m_iBoltD==1)
		bolt_d=20;
	else
		bolt_d=24;
	double wing_thick=min(wing_thick_stub,wing_thick_anchor);
	m_fBoltCanShearForce=Pi*bolt_d*bolt_d*0.25*pLsType->fv*0.001;
	m_fBoltCanCompForce=bolt_d*wing_thick*pLsType->fc*0.001;
	STEELMAT *pStubMat,*pAnchorMat;
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_STUB_MAT);
	double fyk;
	if(pCMB->GetCurSel()==0)
	{
		pStubMat=QuerySteelMatByBriefMark(CSteelMatLibrary::Q235BriefMark());//'S');
		fyk=375;
	}
	else if(pCMB->GetCurSel()==1)
	{
		pStubMat=QuerySteelMatByBriefMark(CSteelMatLibrary::Q345BriefMark());//'H');
		fyk=470;
	}
	else if(pCMB->GetCurSel()==2)
	{
		pStubMat=QuerySteelMatByBriefMark(CSteelMatLibrary::Q390BriefMark());//'G');
		fyk=490;
	}
	else if(pCMB->GetCurSel()==3)
	{
		pStubMat=QuerySteelMatByBriefMark(CSteelMatLibrary::Q420BriefMark());//'T');
		fyk=490;
	}
	else //if(pCMB->GetCurSel()==4)
	{
		pStubMat=QuerySteelMatByBriefMark(CSteelMatLibrary::Q460BriefMark());//'U');
		fyk=490;
	}
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_ANCHOR_MAT);
	if(pCMB->GetCurSel()==0)
		pAnchorMat=QuerySteelMatByBriefMark(CSteelMatLibrary::Q235BriefMark());//'S');
	else if(pCMB->GetCurSel()==1)
		pAnchorMat=QuerySteelMatByBriefMark(CSteelMatLibrary::Q345BriefMark());//'H');
	else if(pCMB->GetCurSel()==2)
		pAnchorMat=QuerySteelMatByBriefMark(CSteelMatLibrary::Q390BriefMark());//'G');
	else if(pCMB->GetCurSel()==3)
		pAnchorMat=QuerySteelMatByBriefMark(CSteelMatLibrary::Q420BriefMark());//'T');
	else //if(pCMB->GetCurSel()==4)
		pAnchorMat=QuerySteelMatByBriefMark(CSteelMatLibrary::Q460BriefMark());//'U');
	//一、插入角钢规格验算
	double area=globalLibrary.GetPartArea(wing_wide_stub,wing_thick_stub,0,CLS_LINEANGLE,'L');
	double An=area-m_fStubAngleHoleOut*bolt_d*wing_thick_stub;
	if(An<(m_fForceN*1000/pStubMat->fy(wing_thick_stub)+m_fShearForce*1000/(0.75*pStubMat->fy(wing_thick_stub))))
		m_sCalResult="插入角钢规格偏小不满足强度设计！";
	else
		m_sCalResult="";
	//二、锚固角钢规格数量验算
	double fc=concrete_tbl[m_iConcreteType].fc;
	double fy=pAnchorMat->fy(wing_thick_anchor);
	double x=wing_thick_anchor*sqrt(fy/(1.19*fc));
	double r=FindJg_r(wing_wide_anchor);
	double sub=wing_thick_anchor+r+x-wing_wide_anchor;
	if(sub>0)
		x-=sub;
	double Vp=0.00119*fc*m_fAnchorAngleLength*(wing_thick_anchor+r+0.5*x);
	m_nAnchorAngleN=(int)ceil(m_fForceN/Vp);
	double shearForce=m_fForceN/m_nAnchorAngleN;
	//三、锚固螺栓验算
	m_nBoltN=(int)ceil(shearForce/min(m_fBoltCanCompForce,m_fBoltCanShearForce));
	//四、最小插入深度计算
	double b=x+r+wing_thick_anchor;
	m_fMinStubLength=(m_nAnchorAngleN-1)*3*b+8*b;
	double d=sqrt(area/Pi)*2;	//角钢换算成圆钢的直径mm
	m_fMinStubDepth=0.6*d*sqrt(fyk);
	UpdateData(FALSE);
}

void CStubAngleCheckDlg::OnSelchangeCmbConcreteType() 
{
	UpdateData();
	m_fConcreteFc=concrete_tbl[m_iConcreteType].fc;
	UpdateData(FALSE);
}
#endif