// DesignXEndRibPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "DesignXEndRibPlateDlg.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignXEndRibPlateDlg dialog


CDesignXEndRibPlateDlg::CDesignXEndRibPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignXEndRibPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignXEndRibPlateDlg)
	m_bMirCreate = TRUE;
	m_bUseAngleParam = FALSE;
	m_iMaterial = 0;
	m_fAngle = 45.0;
	m_fBolEndSpace = 0.0;
	m_fCutAngleWidth = 0.0;
	m_fParaS = 30.0;
	m_sPartNo = _T("");
	m_fPlateLen = 0.0;
	m_fPlateWidth = 0.0;
	m_sSegI = _T("");
	m_nThick = 0;
	//}}AFX_DATA_INIT
}


void CDesignXEndRibPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignXEndRibPlateDlg)
	DDX_Check(pDX, IDC_CHK_MIR_CREATE, m_bMirCreate);
	DDX_Check(pDX, IDC_CHK_USE_ANGLE_PARAM, m_bUseAngleParam);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_ANGLE, m_fAngle);
	DDV_MinMaxDouble(pDX, m_fAngle, 0., 90.);
	DDX_Text(pDX, IDC_E_END_SPACE, m_fBolEndSpace);
	DDX_Text(pDX, IDC_E_CUT_ANGLE_WIDTH, m_fCutAngleWidth);
	DDX_Text(pDX, IDC_E_PARA_S, m_fParaS);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_PLATE_LEN, m_fPlateLen);
	DDX_Text(pDX, IDC_E_PLATE_WIDTH, m_fPlateWidth);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_THICK, m_nThick);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignXEndRibPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignXEndRibPlateDlg)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangePartNo)
	ON_BN_CLICKED(IDC_CHK_USE_ANGLE_PARAM, OnUseAngleParam)
	ON_EN_CHANGE(IDC_E_ANGLE, OnChanegAngle)
	ON_EN_CHANGE(IDC_E_CUT_ANGLE_WIDTH, OnChanegCutAngleWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignXEndRibPlateDlg message handlers

void CDesignXEndRibPlateDlg::OnChangePartNo() 
{
	UpdateData();
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo.GetBuffer(),&iSeg,NULL))
		m_sSegI=iSeg.ToString();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNo,BOMPART::PLATE);
	if(pPart)
	{
		m_nThick=(int)pPart->thick;
		m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
	}
#endif
	UpdateData(FALSE);
}


BOOL CDesignXEndRibPlateDlg::OnInitDialog() 
{
	CComboBox* pCMBMate = (CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	AddSteelMatRecord(pCMBMate);
	GetDlgItem(IDC_E_ANGLE)->EnableWindow(m_bUseAngleParam);
	GetDlgItem(IDC_E_CUT_ANGLE_WIDTH)->EnableWindow(!m_bUseAngleParam);
	m_fCutAngleWidth=(m_fPlateWidth-m_fParaS)*tan(m_fAngle*RADTODEG_COEF);
	UpdateData(FALSE);
	CDialog::OnInitDialog();
	return TRUE;
}

void CDesignXEndRibPlateDlg::OnUseAngleParam() 
{
	UpdateData();
	GetDlgItem(IDC_E_ANGLE)->EnableWindow(m_bUseAngleParam);
	GetDlgItem(IDC_E_CUT_ANGLE_WIDTH)->EnableWindow(!m_bUseAngleParam);
	UpdateData(FALSE);
}

void CDesignXEndRibPlateDlg::OnChanegAngle() 
{
	UpdateData();
	m_fCutAngleWidth=(m_fPlateWidth-m_fParaS)*tan(m_fAngle*RADTODEG_COEF);
	m_fCutAngleWidth=ftol(m_fCutAngleWidth*10)/10.0;	//保留小数点后一位
	UpdateData(FALSE);
}

void CDesignXEndRibPlateDlg::OnChanegCutAngleWidth() 
{
	UpdateData();
	double tana=m_fCutAngleWidth/(m_fPlateWidth-m_fParaS);
	m_fAngle=atan(tana)*DEGTORAD_COEF;
	UpdateData(FALSE);
}
