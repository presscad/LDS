// JointDesParaDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "JointDesParaDlg.h"
#include "Query.h"
#include "XhCharString.h"
#include "GlobalFunc.h"
#include "DataCompare.h"
#include "LocalFeatureDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJointDesParaDlg dialog
#ifndef __TSA_

CJointDesParaDlg::CJointDesParaDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CJointDesParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJointDesParaDlg)
	m_iMirMsg = 4;
	m_iLsLayStyle = 1;
	m_iLsOffsetStyle = 0;
	m_nUpOffset = 0;
	m_iJointStyle = 0;
	m_sJgAGuige = _T("90X7");
	m_iJgAMat = 0;
	m_sJgBGuige = _T("90X7");
	m_iJgBMat = 0;
	m_sJgCGuige = _T("90X7");
	m_iJgCMat = 0;
	m_sJgDGuige = _T("90X7");
	m_iJgDMat = 0;
	m_iPlankEMat = 0;
	m_iPlankFMat = 0;
	m_iPlankGMat = 0;
	m_iPlankHMat = 0;
	m_fJgANormOffsetX = 0.0;
	m_fJgANormOffsetY = 0.0;
	m_sPartNoA = _T("");
	m_sPartNoB = _T("");
	m_sPartNoC = _T("");
	m_sPartNoD = _T("");
	m_sPartNoE = _T("");
	m_sPartNoF = _T("");
	m_sPartNoG = _T("");
	m_sPartNoH = _T("");
	m_fJgBNormOffsetX = 0.0;
	m_fJgBNormOffsetY = 0.0;
	m_fJgCNormOffsetX = 0.0;
	m_fJgCNormOffsetY = 0.0;
	m_fJgDNormOffsetX = 0.0;
	m_fJgDNormOffsetY = 0.0;
	m_nPlankThickE = 0;
	m_nPlankWidthE = 0;
	m_nPlankThickF = 0;
	m_nPlankWidthF = 0;
	m_nPlankThickG = 0;
	m_nPlankWidthG = 0;
	m_nPlankThickH = 0;
	m_nPlankWidthH = 0;
	m_bAdjustWrapPlateLen = FALSE;
	m_bWeldingVertPlate = FALSE;
	m_sASegI = _T("");
	m_sBSegI = _T("");
	m_sCSegI = _T("");
	m_sDSegI = _T("");
	m_sESegI = _T("");
	m_sFSegI = _T("");
	m_sGSegI = _T("");
	m_sHSegI = _T("");
	//}}AFX_DATA_INIT
	m_iBaseJgStyle=0;	//0.单角钢1.对角组合角钢2.十字组合角钢
	m_pMainJg1=NULL;
	m_pMainJg2=NULL;
	m_fPlankEFBerWidth=m_fPlankEBerWidth=m_fPlankFBerWidth=0;
}


void CJointDesParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJointDesParaDlg)
	DDX_CBIndex(pDX, IDC_CMB_MIR_MSG, m_iMirMsg);
	DDX_Radio(pDX, IDC_LS_LAYOUT_STYLE, m_iLsLayStyle);
	DDX_Radio(pDX, IDC_RDO_LS_OFFSET_STYLE, m_iLsOffsetStyle);
	DDX_Text(pDX, IDC_UPOFFSET, m_nUpOffset);
	DDX_CBIndex(pDX, IDC_CMB_JOINT_STYLE, m_iJointStyle);
	DDX_CBString(pDX, IDC_CMB_A_JG_GUIGE, m_sJgAGuige);
	DDX_CBIndex(pDX, IDC_CMB_A_JG_MAT, m_iJgAMat);
	DDX_CBString(pDX, IDC_CMB_B_JG_GUIGE, m_sJgBGuige);
	DDX_CBIndex(pDX, IDC_CMB_B_JG_MAT, m_iJgBMat);
	DDX_CBString(pDX, IDC_CMB_C_JG_GUIGE, m_sJgCGuige);
	DDX_CBIndex(pDX, IDC_CMB_C_JG_MAT, m_iJgCMat);
	DDX_CBString(pDX, IDC_CMB_D_JG_GUIGE, m_sJgDGuige);
	DDX_CBIndex(pDX, IDC_CMB_D_JG_MAT, m_iJgDMat);
	DDX_CBIndex(pDX, IDC_CMB_E_PLANK_MAT, m_iPlankEMat);
	DDX_CBIndex(pDX, IDC_CMB_F_PLANK_MAT, m_iPlankFMat);
	DDX_CBIndex(pDX, IDC_CMB_G_PLANK_MAT, m_iPlankGMat);
	DDX_CBIndex(pDX, IDC_CMB_H_PLANK_MAT, m_iPlankHMat);
	DDX_Text(pDX, IDC_E_A_JG_NORM_X_OFFSET, m_fJgANormOffsetX);
	DDX_Text(pDX, IDC_E_A_JG_NORM_Y_OFFSET, m_fJgANormOffsetY);
	DDX_Text(pDX, IDC_E_A_PART_NO, m_sPartNoA);
	DDX_Text(pDX, IDC_E_B_PART_NO, m_sPartNoB);
	DDX_Text(pDX, IDC_E_C_PART_NO, m_sPartNoC);
	DDX_Text(pDX, IDC_E_D_PART_NO, m_sPartNoD);
	DDX_Text(pDX, IDC_E_E_PART_NO, m_sPartNoE);
	DDX_Text(pDX, IDC_E_F_PART_NO, m_sPartNoF);
	DDX_Text(pDX, IDC_E_G_PART_NO, m_sPartNoG);
	DDX_Text(pDX, IDC_E_H_PART_NO, m_sPartNoH);
	DDX_Text(pDX, IDC_E_B_JG_NORM_X_OFFSET, m_fJgBNormOffsetX);
	DDX_Text(pDX, IDC_E_B_JG_NORM_Y_OFFSET, m_fJgBNormOffsetY);
	DDX_Text(pDX, IDC_E_C_JG_NORM_X_OFFSET, m_fJgCNormOffsetX);
	DDX_Text(pDX, IDC_E_C_JG_NORM_Y_OFFSET, m_fJgCNormOffsetY);
	DDX_Text(pDX, IDC_E_D_JG_NORM_X_OFFSET, m_fJgDNormOffsetX);
	DDX_Text(pDX, IDC_E_D_JG_NORM_Y_OFFSET, m_fJgDNormOffsetY);
	DDX_Text(pDX, IDC_E_E_PLANK_THICK, m_nPlankThickE);
	DDX_Text(pDX, IDC_E_E_PLANK_WIDE, m_nPlankWidthE);
	DDX_Text(pDX, IDC_E_F_PLANK_THICK, m_nPlankThickF);
	DDX_Text(pDX, IDC_E_F_PLANK_WIDE, m_nPlankWidthF);
	DDX_Text(pDX, IDC_E_EF_BER_WIDE, m_fPlankEFBerWidth);
	DDX_Text(pDX, IDC_E_G_PLANK_THICK, m_nPlankThickG);
	DDX_Text(pDX, IDC_E_G_PLANK_WIDE, m_nPlankWidthG);
	DDX_Text(pDX, IDC_E_H_PLANK_THICK, m_nPlankThickH);
	DDX_Text(pDX, IDC_E_H_PLANK_WIDE, m_nPlankWidthH);
	DDX_Check(pDX, IDC_CHK_ADJUST_WRAP_PLATE_LEN, m_bAdjustWrapPlateLen);
	DDX_Check(pDX, IDC_CHK_T_WELD_TO_BASE_PLATE, m_bWeldingVertPlate);
	DDX_Text(pDX, IDC_E_A_SEG_I, m_sASegI);
	DDX_Text(pDX, IDC_E_B_SEG_I, m_sBSegI);
	DDX_Text(pDX, IDC_E_C_SEG_I, m_sCSegI);
	DDX_Text(pDX, IDC_E_D_SEG_I, m_sDSegI);
	DDX_Text(pDX, IDC_E_E_SEG_I, m_sESegI);
	DDX_Text(pDX, IDC_E_F_SEG_I, m_sFSegI);
	DDX_Text(pDX, IDC_E_G_SEG_I, m_sGSegI);
	DDX_Text(pDX, IDC_E_H_SEG_I, m_sHSegI);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJointDesParaDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CJointDesParaDlg)
	ON_CBN_SELCHANGE(IDC_CMB_JOINT_STYLE, OnSelchangeCmbJointStyle)
	ON_CBN_SELCHANGE(IDC_CMB_A_JG_MAT, OnSelchangeCmbAJgMat)
	ON_CBN_SELCHANGE(IDC_CMB_B_JG_MAT, OnSelchangeCmbBJgMat)
	ON_CBN_SELCHANGE(IDC_CMB_C_JG_MAT, OnSelchangeCmbCJgMat)
	ON_CBN_SELCHANGE(IDC_CMB_A_JG_GUIGE, OnSelchangeCmbAJgGuige)
	ON_CBN_SELCHANGE(IDC_CMB_B_JG_GUIGE, OnSelchangeCmbBJgGuige)
	ON_CBN_SELCHANGE(IDC_CMB_C_JG_GUIGE, OnSelchangeCmbCJgGuige)
	ON_CBN_SELCHANGE(IDC_CMB_E_PLANK_MAT, OnSelchangeCmbEPlankMat)
	ON_CBN_SELCHANGE(IDC_CMB_G_PLANK_MAT, OnSelchangeCmbGPlankMat)
	ON_EN_CHANGE(IDC_E_E_PLANK_THICK, OnChangeEEPlankThick)
	ON_EN_CHANGE(IDC_E_E_PLANK_WIDE, OnChangeEEPlankWide)
	ON_EN_CHANGE(IDC_E_F_PLANK_WIDE, OnChangeEFPlankWide)
	ON_EN_CHANGE(IDC_E_EF_BER_WIDE, OnChangeEFBerWide)
	ON_EN_CHANGE(IDC_E_G_PLANK_WIDE, OnChangeEGPlankWide)
	ON_EN_CHANGE(IDC_E_G_PLANK_THICK, OnChangeEGPlankThick)
	ON_EN_CHANGE(IDC_E_A_PART_NO, OnChangeAPartNo)
	ON_EN_CHANGE(IDC_E_B_PART_NO, OnChangeBPartNo)
	ON_EN_CHANGE(IDC_E_C_PART_NO, OnChangeCPartNo)
	ON_EN_CHANGE(IDC_E_D_PART_NO, OnChangeDPartNo)
	ON_EN_CHANGE(IDC_E_E_PART_NO, OnChangeEPartNo)
	ON_EN_CHANGE(IDC_E_F_PART_NO, OnChangeFPartNo)
	ON_EN_CHANGE(IDC_E_G_PART_NO, OnChangeGPartNo)
	ON_EN_CHANGE(IDC_E_H_PART_NO, OnChangeHPartNo)
	ON_EN_CHANGE(IDC_UPOFFSET, &CJointDesParaDlg::OnEnChangeUpoffset)
	ON_BN_CLICKED(IDC_CHK_T_WELD_TO_BASE_PLATE, &CJointDesParaDlg::OnBnClickedChkTWeldToBasePlate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJointDesParaDlg message handlers

BOOL CJointDesParaDlg::OnInitDialog() 
{
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_A_JG_GUIGE);
	AddJgRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_B_JG_GUIGE);
	AddJgRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_C_JG_GUIGE);
	AddJgRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_D_JG_GUIGE);
	AddJgRecord(pCMB);

	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_A_JG_MAT);
	AddSteelMatRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_B_JG_MAT);
	AddSteelMatRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_C_JG_MAT);
	AddSteelMatRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_D_JG_MAT);
	AddSteelMatRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_E_PLANK_MAT);
	AddSteelMatRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_F_PLANK_MAT);
	AddSteelMatRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_G_PLANK_MAT);
	AddSteelMatRecord(pCMB);
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_H_PLANK_MAT);
	AddSteelMatRecord(pCMB);
	CDialog::OnInitDialog();
	if (m_pMainJg2)
	{	//包铁包板材质应该于主材材质一致
		m_iJgBMat = CSteelMatLibrary::GetMatIndex(m_pMainJg2->cMaterial);
		m_iJgDMat = m_iJgCMat = m_iJgAMat = m_iJgBMat;
		m_iPlankEMat = m_iPlankFMat = m_iJgBMat;
	}
	//保证给予不合法的接头型式合法化
	if(m_iBaseJgStyle==0&&m_iJointStyle>4)
		m_iJointStyle=2;	//单角钢接头型式不合法时默认为内包铁外包钢板
	else if(m_iBaseJgStyle==1&&(m_iJointStyle<4||m_iJointStyle>=8))
		m_iJointStyle=4;	//对角组合角钢型接头不合法时默认为'对角组合角钢接头A'
	else if(m_iBaseJgStyle==2&&m_iJointStyle<8)
		m_iJointStyle=8;	//十字组合角钢型接头
	if(m_pMainJg2&&strlen(m_pMainJg2->GetPartNo())>0)	//说明当前操作的角钢，方便用户直观查看
		((CStatic*)GetDlgItem(IDC_S_OFFSET_ODD))->SetWindowText(CXhChar50("向'%s'侧偏移:",m_pMainJg2->GetPartNo()));
	UpdateData(FALSE);
	OnSelchangeCmbJointStyle();
	MoveWndToLocation();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//From DesignJoint.cpp
BOOL CalJgJointPara(CLDSLineAngle *pMainJg1,CLDSLineAngle *pMainJg2,JG_JOINT_PARA *pJointPara,int &joint_type);
void CJointDesParaDlg::OnSelchangeCmbJointStyle() 
{
	int old_style=m_iJointStyle;
	UpdateData();
	if(IsValidJointStyle())
	{
		CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_JOINT_STYLE);
		m_CurJointBmp.Detach();
		GetDlgItem(IDC_CHK_T_WELD_TO_BASE_PLATE)->EnableWindow(FALSE);
		switch(m_iJointStyle)
		{
		case 0:	//外包铁
			m_CurJointBmp.LoadBitmap(IDB_BMP_JOINT_JGA);
			GetDlgItem(IDC_E_A_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_B_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_F_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_G_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_A_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_B_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_F_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_G_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_A_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_B_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_D_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PLANK_WIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_F_PLANK_WIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_EF_BER_WIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_G_PLANK_WIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_PLANK_WIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_F_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_G_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_A_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_B_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_D_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_E_PLANK_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_F_PLANK_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_G_PLANK_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_H_PLANK_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_A_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_B_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_A_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_B_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHK_ADJUST_WRAP_PLATE_LEN)->EnableWindow(FALSE);
			break;
		case 1:	//内外包铁
			m_CurJointBmp.LoadBitmap(IDB_BMP_JOINT_JGB);
			GetDlgItem(IDC_E_A_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_F_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_G_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_F_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_G_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_A_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_D_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PLANK_WIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_F_PLANK_WIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_EF_BER_WIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_G_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_E_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_F_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_G_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_A_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_D_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_E_PLANK_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_F_PLANK_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_G_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_H_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_A_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHK_ADJUST_WRAP_PLATE_LEN)->EnableWindow(FALSE);
			break;
		case 2:	//内包铁外包钢板
			m_CurJointBmp.LoadBitmap(IDB_BMP_JOINT_JGC);
			GetDlgItem(IDC_E_A_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_C_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_C_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_A_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_C_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_D_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_EF_BER_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_E_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_A_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_C_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_D_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_E_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_F_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_G_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_H_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_C_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_A_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_C_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHK_ADJUST_WRAP_PLATE_LEN)->EnableWindow(TRUE);
#ifdef AFX_TARG_ENU_ENGLISH
			GetDlgItem(IDC_CHK_ADJUST_WRAP_PLATE_LEN)->SetWindowText("Align Outside Plate");
#else 
			GetDlgItem(IDC_CHK_ADJUST_WRAP_PLATE_LEN)->SetWindowText("对齐外包钢板");
#endif
			break;
		case 3:	//外包铁内包钢板
			m_CurJointBmp.LoadBitmap(IDB_BMP_JOINT_JGD);
			GetDlgItem(IDC_E_A_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_B_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_B_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_A_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_B_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_D_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_EF_BER_WIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_G_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_E_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_A_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_B_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_D_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_E_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_F_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_G_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_H_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_B_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_A_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_B_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHK_ADJUST_WRAP_PLATE_LEN)->EnableWindow(TRUE);
#ifdef AFX_TARG_ENU_ENGLISH
			GetDlgItem(IDC_CHK_ADJUST_WRAP_PLATE_LEN)->SetWindowText("Align Inside Plate");
#else 
			GetDlgItem(IDC_CHK_ADJUST_WRAP_PLATE_LEN)->SetWindowText("对齐内包钢板");
#endif
			break;
		case 4:
			m_CurJointBmp.LoadBitmap(IDB_BMP_JOINT_GROUP_JG2A);
			m_bWeldingVertPlate = TRUE;
			GetDlgItem(IDC_CHK_T_WELD_TO_BASE_PLATE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_A_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_A_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_D_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_EF_BER_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_E_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_A_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_D_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_E_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_F_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_G_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_H_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_A_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			break;
		case 5:
			m_CurJointBmp.LoadBitmap(IDB_BMP_JOINT_GROUP_JG2B);
			m_bWeldingVertPlate = TRUE;
			GetDlgItem(IDC_CHK_T_WELD_TO_BASE_PLATE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_A_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_PART_NO)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_SEG_I)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_A_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_D_JG_GUIGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_E_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_EF_BER_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_E_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_A_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_D_JG_MAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_E_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_F_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_G_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_H_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_JG_NORM_X_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_A_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_D_JG_NORM_Y_OFFSET)->EnableWindow(FALSE);
			break;
		case 6:
			m_CurJointBmp.LoadBitmap(IDB_BMP_JOINT_GROUP_JG2C);
			GetDlgItem(IDC_E_A_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_E_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_E_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_A_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_D_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_E_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_EF_BER_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_E_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_A_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_D_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_E_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_F_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_G_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_H_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_A_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			break;
		case 7:
			m_CurJointBmp.LoadBitmap(IDB_BMP_JOINT_GROUP_JG2D);
			GetDlgItem(IDC_E_A_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_E_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_E_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_A_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_D_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_E_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_EF_BER_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_E_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_A_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_D_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_E_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_F_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_G_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_H_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_A_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			break;
		case 8:
			m_CurJointBmp.LoadBitmap(IDB_BMP_JOINT_GROUP_JG4A);
			GetDlgItem(IDC_E_A_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_E_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_E_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_A_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_D_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_E_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_EF_BER_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_E_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_A_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_D_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_E_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_F_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_G_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_H_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_A_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			break;
		case 9:
			m_CurJointBmp.LoadBitmap(IDB_BMP_JOINT_GROUP_JG4B);
			GetDlgItem(IDC_E_A_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_E_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PART_NO)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PART_NO)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_E_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_SEG_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_SEG_I)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_A_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_D_JG_GUIGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_E_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_EF_BER_WIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_H_PLANK_WIDE)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_E_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_F_PLANK_THICK)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_G_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_H_PLANK_THICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CMB_A_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_B_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_C_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_D_JG_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_E_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_F_PLANK_MAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMB_G_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_CMB_H_PLANK_MAT)->EnableWindow(m_bHavePadPlank);
			GetDlgItem(IDC_E_A_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_JG_NORM_X_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_A_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_B_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_C_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			GetDlgItem(IDC_E_D_JG_NORM_Y_OFFSET)->EnableWindow(TRUE);
			break;
		default:
			break;
		}
		pPic->SetBitmap(m_CurJointBmp);
		//改变角钢接头类型时自动判断角钢接头参数 wht 11-01-25
		JG_JOINT_PARA joint_para;
		if(CalJgJointPara(m_pMainJg1,m_pMainJg2,&joint_para,m_iJointStyle))
		{
			if(m_iJointStyle==0)//外包铁
				m_sJgBGuige.Format("%.0fX%.0f",joint_para.fOuterJgWidth,joint_para.fOuterJgThick);
			else if(m_iJointStyle==1)
			{	//内外包铁
				m_sJgAGuige.Format("%.0fX%.0f",joint_para.fInnerJgWidth,joint_para.fInnerJgThick);
				m_sJgBGuige.Format("%.0fX%.0f",joint_para.fOuterJgWidth,joint_para.fOuterJgThick);
			}
			else if(m_iJointStyle==2)
			{	//内包铁外包钢板
				m_sJgAGuige.Format("%.0fX%.0f",joint_para.fInnerJgWidth,joint_para.fInnerJgThick);
				m_nPlankWidthE=m_nPlankWidthF=ftol(joint_para.fOuterPlateWidth);
				m_nPlankThickE=m_nPlankThickF=ftol(joint_para.fOuterPlateThick);
			}
			else if(m_iJointStyle==3)
			{	//内包钢板外包铁
				m_nPlankWidthE=m_nPlankWidthF=ftol(joint_para.fOuterPlateWidth);
				m_nPlankThickE=m_nPlankThickF=ftol(joint_para.fOuterPlateThick);
				m_sJgBGuige.Format("%.0fX%.0f",joint_para.fOuterJgWidth,joint_para.fOuterJgThick);
			}
			else 
			{	//内包铁外包钢板
				m_sJgAGuige.Format("%.0fX%.0f",joint_para.fInnerJgWidth,joint_para.fInnerJgThick);
				m_sJgBGuige.Format("%.0fX%.0f",joint_para.fInnerJgWidth,joint_para.fInnerJgThick);
				m_nPlankWidthE=m_nPlankWidthF=ftol(joint_para.fOuterPlateWidth);
				m_nPlankThickE=m_nPlankThickF=ftol(joint_para.fOuterPlateThick);
				if(m_iJointStyle==6||m_iJointStyle==7)
				{
					m_sJgCGuige.Format("%.0fX%.0f",m_pMainJg1->GetWidth(),m_pMainJg2->GetThick());
					m_sJgDGuige=m_sJgCGuige;
				}
				else if(m_iJointStyle==8||m_iJointStyle==9)
					m_sJgCGuige=m_sJgDGuige=m_sJgAGuige;
			}
			if(m_iJointStyle==2||m_iJointStyle==4||m_iJointStyle==6||m_iJointStyle==8)
				UpdatePlankEFBerWidth(m_nPlankWidthF);
			else if(m_iJointStyle==2||m_iJointStyle==5||m_iJointStyle==7||m_iJointStyle==9)
				UpdatePlankEFBerWidth(m_nPlankWidthE);
			else
				m_fPlankEFBerWidth=m_fPlankEBerWidth=m_fPlankFBerWidth=0;
		}
	}
	else
	{
		m_iJointStyle=old_style;
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The selective joint form is not appropriate,joint form rolls back！");
#else 
		AfxMessageBox("选择了不适当的接头形式，接头形式回滚！");
#endif
	}
	UpdateData(FALSE);
}

BOOL CJointDesParaDlg::IsValidJointStyle()
{
	if(m_iBaseJgStyle==0&&m_iJointStyle<4)
		return TRUE;	//单角钢接头
	else if(m_iBaseJgStyle==1&&(m_iJointStyle>=4&&m_iJointStyle<8))
		return TRUE;	//对角组合角钢型接头
	else if(m_iBaseJgStyle==2&&m_iJointStyle>=8)
		return TRUE;	//十字组合角钢型接头
	else
		return FALSE;

}

void CJointDesParaDlg::OnSelchangeCmbAJgMat() 
{
	UpdateData();
	m_iJgBMat=m_iJgCMat=m_iJgDMat=m_iJgAMat;
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnSelchangeCmbBJgMat() 
{
	UpdateData();
	m_iJgCMat=m_iJgDMat=m_iJgBMat;
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnSelchangeCmbCJgMat() 
{
	UpdateData();
	m_iJgDMat=m_iJgCMat;
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnSelchangeCmbAJgGuige() 
{
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_A_JG_GUIGE);
	int iCurSel=pCMB->GetCurSel();
	pCMB->GetLBText(iCurSel,m_sJgAGuige);
	m_sJgBGuige=m_sJgCGuige=m_sJgDGuige=m_sJgAGuige;
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnSelchangeCmbBJgGuige() 
{
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_B_JG_GUIGE);
	int iCurSel=pCMB->GetCurSel();
	pCMB->GetLBText(iCurSel,m_sJgBGuige);
	m_sJgCGuige=m_sJgDGuige=m_sJgBGuige;
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnSelchangeCmbCJgGuige() 
{
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_C_JG_GUIGE);
	int iCurSel=pCMB->GetCurSel();
	pCMB->GetLBText(iCurSel,m_sJgCGuige);
	m_sJgDGuige=m_sJgCGuige;
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnSelchangeCmbEPlankMat() 
{
	UpdateData();
	m_iPlankFMat=m_iPlankEMat;
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnSelchangeCmbGPlankMat() 
{
	UpdateData();
	m_iPlankHMat=m_iPlankGMat;
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnChangeEEPlankThick() 
{
	UpdateData();
	m_nPlankThickF=m_nPlankThickE;
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnChangeEEPlankWide() 
{
	UpdateData();
	m_nPlankWidthF=m_nPlankWidthE;
	if(m_iJointStyle==2||m_iJointStyle==5||m_iJointStyle==7||m_iJointStyle==9)
		UpdatePlankEFBerWidth(m_nPlankWidthE);	//F板为外包钢板需要计算楞线侧宽度
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnChangeEGPlankWide() 
{
	UpdateData();
	m_nPlankWidthH=m_nPlankWidthG;
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnChangeEGPlankThick() 
{
	UpdateData();
	m_nPlankThickH=m_nPlankThickG;
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnChangeAPartNo() 
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNoA);
	if(pPart)
	{
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			m_sJgAGuige.Format("%.0fX%.0f",pPart->wide,pPart->thick);
			m_iJgAMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		}
		//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的角钢",(char*)pPart->sPartNo));
	}
#endif
	SEGI iSeg;
	if(ParsePartNo(m_sPartNoA.GetBuffer(),&iSeg,NULL))
		m_sASegI=iSeg.ToString();
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnChangeBPartNo() 
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNoB);
	if(pPart)
	{
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			m_sJgBGuige.Format("%.0fX%.0f",pPart->wide,pPart->thick);
			m_iJgBMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		}
		//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的角钢",(char*)pPart->sPartNo));
	}
#endif
	SEGI iSeg;
	if(ParsePartNo(m_sPartNoB.GetBuffer(),&iSeg,NULL))
		m_sBSegI=iSeg.ToString();
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnChangeCPartNo() 
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNoC);
	if(pPart)
	{
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			m_sJgCGuige.Format("%.0fX%.0f",pPart->wide,pPart->thick);
			m_iJgCMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		}
		//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的角钢",(char*)pPart->sPartNo));
	}
#endif
	SEGI iSeg;
	if(ParsePartNo(m_sPartNoC.GetBuffer(),&iSeg,NULL))
		m_sCSegI=iSeg.ToString();
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnChangeDPartNo() 
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNoD);
	if(pPart)
	{	
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			m_sJgDGuige.Format("%.0fX%.0f",pPart->wide,pPart->thick);
			m_iJgDMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		}
		//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的角钢",(char*)pPart->sPartNo));
	}
#endif
	SEGI iSeg;
	if(ParsePartNo(m_sPartNoD.GetBuffer(),&iSeg,NULL))
		m_sDSegI=iSeg.ToString();
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnChangeEPartNo() 
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNoE);
	if(pPart)
	{
		if(pPart->cPartType==BOMPART::PLATE)
		{
			m_nPlankThickE=ftoi(pPart->thick);
			m_nPlankWidthE=ftoi(pPart->wide);
			m_iPlankEMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		}
		//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	SEGI iSeg;
	if(ParsePartNo(m_sPartNoE.GetBuffer(),&iSeg,NULL))
		m_sESegI=iSeg.ToString();
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnChangeFPartNo() 
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNoF);
	if(pPart)
	{
		if(pPart->cPartType==BOMPART::PLATE)
		{
			m_nPlankThickF=ftoi(pPart->thick);
			m_nPlankWidthF=ftoi(pPart->wide);
			m_iPlankFMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		}
		//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	SEGI iSeg;
	if(ParsePartNo(m_sPartNoF.GetBuffer(),&iSeg,NULL))
		m_sFSegI=iSeg.ToString();
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnChangeGPartNo() 
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNoG);
	if(pPart)
	{
		if(pPart->cPartType==BOMPART::PLATE)
		{
			m_nPlankThickG=ftoi(pPart->thick);
			m_nPlankWidthG=ftoi(pPart->wide);
			m_iPlankGMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		}
		//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	SEGI iSeg;
	if(ParsePartNo(m_sPartNoG.GetBuffer(),&iSeg,NULL))
		m_sGSegI=iSeg.ToString();
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnChangeHPartNo() 
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNoH);
	if(pPart)
	{
		if(pPart->cPartType==BOMPART::PLATE)
		{
			m_nPlankThickH=ftoi(pPart->thick);
			m_nPlankWidthH=ftoi(pPart->wide);
			m_iPlankHMat=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		}
		//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	SEGI iSeg;
	if(ParsePartNo(m_sPartNoH.GetBuffer(),&iSeg,NULL))
		m_sHSegI=iSeg.ToString();
	UpdateData(FALSE);
}

void CJointDesParaDlg::OnEnChangeUpoffset()
{	//防止因改动其它界面参数导致冲掉之前输入的正负头 wjh-2015.2.28
	UpdateData();
}

void CJointDesParaDlg::UpdatePlankEFBerWidth(double fPlateWidth)
{	//以第一根角钢螺栓为基准计算楞线侧宽度
	int fDifferG1=0;	//两角钢g1准据差值
	int fMinG1=base_jgzj1.g1,fEndSpaceG1=LsSpace1.EndSpace;
	if(base_jgzj2.g1<base_jgzj1.g1)
	{
		fMinG1=base_jgzj2.g1;
		fDifferG1=base_jgzj1.g1-base_jgzj2.g1;
		fEndSpaceG1=LsSpace2.EndSpace;
	}
	int fMaxG2=base_jgzj1.g2,fEndSpaceG2=LsSpace1.EndSpace;
	if(base_jgzj2.g2>base_jgzj2.g2)
	{
		fMaxG2=base_jgzj2.g2;
		fEndSpaceG2=LsSpace2.EndSpace;
	}
	double fWidthMargin=fPlateWidth-((fMaxG2-fMinG1)+fEndSpaceG1+fEndSpaceG2);
	if(fWidthMargin>=0)
	{
		m_fPlankEFBerWidth=0.5*fWidthMargin+fEndSpaceG1;
		if(fDifferG1>0)
			m_fPlankEFBerWidth+=fDifferG1;
		if(m_bWeldingVertPlate)	//选择焊接时楞线侧宽度取g1,保证钢板与角钢楞线对齐 wht 17-05-18
			m_fPlankEFBerWidth=base_jgzj1.g1;
	}
	else
		m_fPlankEFBerWidth=0;
	m_fPlankFBerWidth=m_fPlankEBerWidth=m_fPlankEFBerWidth;
	if(m_iJointStyle==0||m_iJointStyle==1||m_iJointStyle==3)
		m_fPlankFBerWidth=m_fPlankEBerWidth=m_fPlankEFBerWidth=0;	//不支持设置楞线侧宽度
	//支持设置组合角钢包板楞线侧宽度 wht 16-11-04
	else if(m_iJointStyle==4||m_iJointStyle==6||m_iJointStyle==8)
		m_fPlankEBerWidth=0;	//E板为通板不需要设置楞线侧宽度
	else if(m_iJointStyle==5||m_iJointStyle==7||m_iJointStyle==9)
		m_fPlankFBerWidth=0;	//F板为通板不需要设置楞线侧宽度
	UpdateData(FALSE);
}
void CJointDesParaDlg::OnChangeEFPlankWide()
{
	UpdateData();
	if(m_iJointStyle==2||m_iJointStyle==4||m_iJointStyle==6||m_iJointStyle==8)
		UpdatePlankEFBerWidth(m_nPlankWidthF);	//F板为外包钢板需要计算楞线侧宽度
	UpdateData(FALSE);
}
void CJointDesParaDlg::OnChangeEFBerWide()
{
	UpdateData(TRUE);
	m_fPlankFBerWidth=m_fPlankEBerWidth=m_fPlankEFBerWidth;
	//支持设置组合角钢包板楞线侧宽度 wht 16-11-04
	if(m_iJointStyle==4||m_iJointStyle==6||m_iJointStyle==8)
		m_fPlankEBerWidth=0;	//E板为通板不需要设置楞线侧宽度
	else if(m_iJointStyle==5||m_iJointStyle==7||m_iJointStyle==9)
		m_fPlankFBerWidth=0;	//F板为通板不需要设置楞线侧宽度
	UpdateData(FALSE);
}
void CJointDesParaDlg::OnBnClickedChkTWeldToBasePlate()
{
	UpdateData();
	if(m_iJointStyle==2||m_iJointStyle==4||m_iJointStyle==6||m_iJointStyle==8)
		UpdatePlankEFBerWidth(m_nPlankWidthF);
	else if(m_iJointStyle==2||m_iJointStyle==5||m_iJointStyle==7||m_iJointStyle==9)
		UpdatePlankEFBerWidth(m_nPlankWidthE);
	UpdateData(FALSE);
}
#endif
