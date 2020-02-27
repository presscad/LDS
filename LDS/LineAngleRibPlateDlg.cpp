//<LOCALE_TRANSLATE Confirm by hxr/>
// LineAngleRibPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "LineAngleRibPlateDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "LocalFeatureDef.h"
#include "DataCompare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CLineAngleRibPlateDlg dialog
CLineAngleRibPlateDlg::CLineAngleRibPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLineAngleRibPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLineAngleRibPlateDlg)
	m_iMaterial = 0;
	m_fParamD = 0.0;
	m_fParamW = 20.0;
	m_fParamS = 10.0;
	m_fParamL = 80.0;
	m_fPlateT = 8.0;
	m_sPartNo = _T("");
	m_sSegI = _T("1");
	m_bInside = TRUE;
	m_bOutSide = FALSE;
	m_fParamH = 0.0;
	//}}AFX_DATA_INIT
	m_pDatumLineAngle = NULL;
	m_fParamLr = 10.0;
	m_fParamR = 0.0;
	m_fParamd = 0.0;
	m_fParamP = 0.0;
	m_bCirEdge = FALSE;
}


void CLineAngleRibPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLineAngleRibPlateDlg)
	DDX_CBIndex(pDX, IDC_CMB_MATER, m_iMaterial);
	DDX_Text(pDX, IDC_E_PARAMER_D, m_fParamD);
	DDX_Text(pDX, IDC_E_PARAMER_W, m_fParamW);
	DDX_Text(pDX, IDC_E_PARAMER_S, m_fParamS);
	DDX_Text(pDX, IDC_E_PARAMER_L, m_fParamL);
	DDX_Text(pDX, IDC_E_T, m_fPlateT);
	DDX_Text(pDX, IDC_E_PARTNO, m_sPartNo);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_JG_GUI_GE, m_sJgGuiGe);
	DDX_Text(pDX, IDC_E_JG_INNER_R, m_fJgInnerR);
	DDX_Check(pDX, IDC_CHK_INSIDE, m_bInside);
	DDX_Check(pDX, IDC_CHK_OUTSIDE, m_bOutSide);
	DDX_Text(pDX, IDC_E_PARAMER_H, m_fParamH);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_E_PARAMER_Lr, m_fParamLr);
	DDX_Text(pDX, IDC_E_PARAMER_R, m_fParamR);
	DDX_Text(pDX, IDC_E_PARAMER_D2, m_fParamd);
	DDX_Text(pDX, IDC_E_PARAMER_P, m_fParamP);
	DDX_Check(pDX, IDC_CHK_CIR_EDGE, m_bCirEdge);
}


BEGIN_MESSAGE_MAP(CLineAngleRibPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CLineAngleRibPlateDlg)
	ON_EN_CHANGE(IDC_E_PARTNO, OnChangePartNo)
	ON_BN_CLICKED(IDC_CHK_INSIDE, OnChkInside)
	ON_BN_CLICKED(IDC_CHK_OUTSIDE, OnChkOutside)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_E_PARAMER_L, &CLineAngleRibPlateDlg::OnEnChangeEParamerL)
	ON_EN_CHANGE(IDC_E_PARAMER_Lr, &CLineAngleRibPlateDlg::OnEnChangeEParamerL2)
	ON_BN_CLICKED(IDOK, &CLineAngleRibPlateDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHK_CIR_EDGE, &CLineAngleRibPlateDlg::OnChkCirEdge)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineAngleRibPlateDlg message handlers

BOOL CLineAngleRibPlateDlg::OnInitDialog() 
{
	if(m_pDatumLineAngle==NULL)
		return FALSE;
	CComboBox*pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MATER);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iMaterial);
	((CEdit*)GetDlgItem(IDC_E_JG_GUI_GE))->SetReadOnly(TRUE);
	//((CEdit*)GetDlgItem(IDC_E_JG_INNER_R))->SetReadOnly(TRUE);
	m_sJgGuiGe.Format("%.0fX%.0f",m_pDatumLineAngle->GetWidth(),m_pDatumLineAngle->GetThick());
	m_fJgInnerR=FindJg_r(m_pDatumLineAngle->GetWidth());
	
	//加载示意图
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_MAP);
	m_curBitmap.Detach();
	if(m_bInside)
	{
		if (m_bCirEdge)
			m_curBitmap.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE1);
		else
			m_curBitmap.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE);
	}
	else 
		m_curBitmap.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE2);
	pPic->SetBitmap(m_curBitmap);
	GetDlgItem(IDC_S_PARAM_H)->ShowWindow(m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_H)->ShowWindow(m_bOutSide);
	GetDlgItem(IDC_CHK_CIR_EDGE)->ShowWindow(!m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_H)->EnableWindow(m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_L)->EnableWindow(!m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_Lr)->EnableWindow(!m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_R)->EnableWindow(m_bInside&&m_bCirEdge);
	GetDlgItem(IDC_E_PARAMER_D2)->EnableWindow(m_bInside);
	GetDlgItem(IDC_E_PARAMER_P)->EnableWindow(m_bInside);
	if(m_bOutSide)
	{	//外侧肋板
		m_fParamL=m_pDatumLineAngle->GetWidth()*2;
		m_fParamH=m_pDatumLineAngle->GetWidth();
		m_fParamW=m_pDatumLineAngle->GetWidth();
	}
	else	//内侧肋板
	{
		m_fParamL=m_pDatumLineAngle->GetWidth()-m_pDatumLineAngle->GetThick();
		m_fParamLr = m_pDatumLineAngle->GetThick();
	}
	UpdateData(FALSE);
	CDialog::OnInitDialog();
	return TRUE;  
}

void CLineAngleRibPlateDlg::OnChangePartNo() 
{
	UpdateData();
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo.GetBuffer(),&iSeg,NULL))
		m_sSegI=iSeg.ToString();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNo);
	if(pPart&&pPart->cPartType==BOMPART::PLATE)
	{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
		m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		m_fPlateT=pPart->thick;
	}
#endif
	UpdateData(FALSE);
}
void CLineAngleRibPlateDlg::OnChkInside() 
{
	UpdateData();
	m_bOutSide=!m_bInside;
	//加载示意图
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_MAP);
	m_curBitmap.Detach();
	if(m_bInside)
	{
		if (m_bCirEdge)
			m_curBitmap.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE1);
		else
			m_curBitmap.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE);
	}
	else 
		m_curBitmap.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE2);
	pPic->SetBitmap(m_curBitmap);
	GetDlgItem(IDC_S_PARAM_H)->ShowWindow(m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_H)->ShowWindow(m_bOutSide);
	GetDlgItem(IDC_CHK_CIR_EDGE)->ShowWindow(!m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_H)->EnableWindow(m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_Lr)->EnableWindow(!m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_R)->EnableWindow(m_bInside&&m_bCirEdge);
	GetDlgItem(IDC_E_PARAMER_D2)->EnableWindow(m_bInside);
	GetDlgItem(IDC_E_PARAMER_P)->EnableWindow(m_bInside);
	if(m_bOutSide)
	{	//外侧肋板
		m_fParamL=m_pDatumLineAngle->GetWidth()*2;
		m_fParamW=m_pDatumLineAngle->GetWidth();
		m_fParamH=m_pDatumLineAngle->GetWidth();
	}
	else
	{	//内侧肋板
		m_fParamL=m_pDatumLineAngle->GetWidth()-m_pDatumLineAngle->GetThick();
		m_fParamW=20;
	}
	UpdateData(FALSE);
}

void CLineAngleRibPlateDlg::OnChkOutside() 
{
	UpdateData();
	m_bInside=!m_bOutSide;
	//加载示意图
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_MAP);
	m_curBitmap.Detach();
	if(m_bInside)
	{
		if (m_bCirEdge)
			m_curBitmap.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE1);
		else
			m_curBitmap.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE);
	}
	else 
		m_curBitmap.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE2);
	pPic->SetBitmap(m_curBitmap);
	GetDlgItem(IDC_S_PARAM_H)->ShowWindow(m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_H)->ShowWindow(m_bOutSide);
	GetDlgItem(IDC_CHK_CIR_EDGE)->ShowWindow(!m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_H)->EnableWindow(m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_Lr)->EnableWindow(!m_bOutSide);
	GetDlgItem(IDC_E_PARAMER_R)->EnableWindow(m_bInside&&m_bCirEdge);
	GetDlgItem(IDC_E_PARAMER_D2)->EnableWindow(m_bInside);
	GetDlgItem(IDC_E_PARAMER_P)->EnableWindow(m_bInside);
	if(m_bOutSide)
	{	//外侧肋板
		m_fParamL=m_pDatumLineAngle->GetWidth()*2;
		m_fParamW=m_pDatumLineAngle->GetWidth();
		m_fParamH=m_pDatumLineAngle->GetWidth();
	}
	else
	{	//内侧肋板
		m_fParamL=m_pDatumLineAngle->GetWidth()-m_pDatumLineAngle->GetThick();
		m_fParamW=20;
	}
	UpdateData(FALSE);
}
#endif

void CLineAngleRibPlateDlg::OnEnChangeEParamerL()
{
	UpdateData();
	if (m_pDatumLineAngle->GetWidth()<m_fParamLr)
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("This parameter is not larger than the width of datum angle wing!");
#else
		MessageBox("该参数不能大于基准角钢肢宽！");
#endif
	m_fParamLr = m_pDatumLineAngle->GetWidth()-m_fParamL;
	UpdateData(FALSE);
}

void CLineAngleRibPlateDlg::OnEnChangeEParamerL2()
{
	UpdateData();
	if (m_pDatumLineAngle->GetWidth()<m_fParamL)
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("This parameter is not larger than the width of datum angle wing!");
#else
		MessageBox("该参数不能大于基准角钢肢宽！");
#endif
	m_fParamL = m_pDatumLineAngle->GetWidth()-m_fParamLr;
	UpdateData(FALSE);
}


void CLineAngleRibPlateDlg::OnBnClickedOk()
{
	if (!m_bCirEdge||m_fParamR <=0||m_fParamP<=0)
	{
		CDialog::OnOK();
		return;
	}
	UpdateData();
	UCS_STRU ucs;
	f3dPoint axis_z=m_pDatumLineAngle->End()-m_pDatumLineAngle->Start();
	normalize(axis_z);
	ucs.axis_z=axis_z;
	ucs.axis_x=-m_pDatumLineAngle->get_norm_y_wing();
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_y);

	f3dPoint x_wing_vec,y_wing_vec,norm_x_wing,norm_y_wing;
	double wing_thick = m_pDatumLineAngle->GetThick();
	x_wing_vec=m_pDatumLineAngle->GetWingVecX();
	y_wing_vec=m_pDatumLineAngle->GetWingVecY();
	norm_x_wing=m_pDatumLineAngle->get_norm_x_wing();
	norm_y_wing=m_pDatumLineAngle->get_norm_y_wing();
	vector_trans(x_wing_vec,ucs,FALSE);
	vector_trans(y_wing_vec,ucs,FALSE);
	vector_trans(norm_x_wing,ucs,FALSE);
	vector_trans(norm_y_wing,ucs,FALSE);
	wing_thick=m_pDatumLineAngle->GetThick();
	f3dPoint startVertice;
	startVertice=f3dPoint(0,0,0)+x_wing_vec*(m_fParamL+wing_thick)-norm_x_wing*wing_thick;
	f3dPoint center;
	center = f3dPoint(0,0,0)-norm_x_wing*(m_fParamP+wing_thick)-norm_y_wing*(m_fParamP+wing_thick);
	double maxR = DISTANCE(center,startVertice);
	if (m_fParamR>maxR)
	{//始终端顶点在挂线孔圆弧内部
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox(CXhChar50("The vertex of starting and ending are inside the arc of hanging hole，parameter R can not be larger than %f",maxR));
#else
		MessageBox(CXhChar50("始终端顶点在挂线孔圆弧内部，参数R不能大于%f",maxR));
#endif
		return;
	}
	UpdateData(FALSE);
	//
	CDialog::OnOK();
}


void CLineAngleRibPlateDlg::OnChkCirEdge()
{
	UpdateData();
	//加载示意图
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_MAP);
	m_curBitmap.Detach();
	if(m_bCirEdge)
		m_curBitmap.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE1);
	else 
		m_curBitmap.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE);
	pPic->SetBitmap(m_curBitmap);
	GetDlgItem(IDC_E_PARAMER_R)->EnableWindow(m_bInside&&m_bCirEdge);
	GetDlgItem(IDC_E_PARAMER_D2)->EnableWindow(m_bInside);
	GetDlgItem(IDC_E_PARAMER_P)->EnableWindow(m_bInside);
	UpdateData(FALSE);
}
