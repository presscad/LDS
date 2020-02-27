// WaterShieldedPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "WaterShieldedPlateDlg.h"
#include "GlobalFunc.h"
#include "Query.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaterShieldedPlateDlg dialog


CWaterShieldedPlateDlg::CWaterShieldedPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaterShieldedPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaterShieldedPlateDlg)
	m_nPlateThick = 6;
	m_sPartNo = _T("");
	m_iMaterial = 0;
	m_fExternalDiameter = 0.0;
	m_fInternalDiameter = 0.0;
	m_fNormalOffset = 0.0;
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	m_sSegI = _T("");
	m_fWaterSlotWidth = 15;
	m_fWaterSlotDepth = 12;
	m_fWaterSlotLen = 100;
	m_fWaterSlotAngle = 60;
	m_iDatumLine = 1;
	m_fOffsetAngle = 0;
	//}}AFX_DATA_INIT
}


void CWaterShieldedPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaterShieldedPlateDlg)
	DDX_Text(pDX, IDC_E_PLATE_THICK, m_nPlateThick);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDV_MaxChars(pDX, m_sPartNo, 16);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_EXTERNAL_DIAMETER, m_fExternalDiameter);
	DDX_Text(pDX, IDC_E_INTERNAL_DIAMETER, m_fInternalDiameter);
	DDX_Text(pDX, IDC_E_NORMAL_OFFSET, m_fNormalOffset);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_Text(pDX, IDC_E_PLATE_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_WATER_SLOT_WIDTH, m_fWaterSlotWidth);
	DDX_Text(pDX, IDC_E_WATER_SLOT_DEPTH, m_fWaterSlotDepth);
	DDX_Text(pDX, IDC_E_WATER_SLOT_LEN, m_fWaterSlotLen);
	DDX_Text(pDX, IDC_E_WATER_SLOT_ANGLE, m_fWaterSlotAngle);
	DDX_Text(pDX, IDC_E_OFFSET_ANGLE, m_fOffsetAngle);
	DDX_Radio(pDX, IDC_RDO_DATUM_LINE, m_iDatumLine);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaterShieldedPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CWaterShieldedPlateDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_NORM, OnBnPasteNorm)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangePartNo)
	ON_EN_CHANGE(IDC_E_EXTERNAL_DIAMETER, OnChangeExternalDiameter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaterShieldedPlateDlg message handlers

BOOL CWaterShieldedPlateDlg::OnInitDialog() 
{
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	AddSteelMatRecord(pCMB);
	CDialog::OnInitDialog();
	return TRUE;
}

void CWaterShieldedPlateDlg::OnBnPasteNorm() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fNormX = point.x;
		m_fNormY = point.y;
		m_fNormZ = point.z;
		UpdateData(FALSE);
	}
}

void CWaterShieldedPlateDlg::OnChangePartNo() 
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
		m_nPlateThick=ftoi(pPart->thick);
		m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
	}
#endif
	UpdateData(FALSE);
}

void CWaterShieldedPlateDlg::OnChangeExternalDiameter() 
{
	UpdateData();
	m_fWaterSlotLen=0.5*m_fExternalDiameter;
	UpdateData(FALSE);
}
