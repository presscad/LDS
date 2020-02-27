// CircularPlateDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "CircularPlateDlg.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "DataCompare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCircularPlateDlg dialog


CCircularPlateDlg::CCircularPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCircularPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCircularPlateDlg)
	m_nPlateThick = 6;
	m_sPartNo = _T("");
	m_fStartAngle = 0.0;
	m_fSectorAngle = 360.0;
	m_iMaterial = 0;
	m_fExternalDiameter = 0.0;
	m_fInternalDiameter = 0.0;
	m_fNormalOffset = 0.0;
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	m_sSegI = _T("");
	//}}AFX_DATA_INIT
	m_iDlgType = 0;
}


void CCircularPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCircularPlateDlg)
	DDX_Text(pDX, IDC_E_PLATE_THICK, m_nPlateThick);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDV_MaxChars(pDX, m_sPartNo, 16);
	DDX_Text(pDX, IDC_E_START_ANGLE, m_fStartAngle);
	DDX_Text(pDX, IDC_E_SECTOR_ANGLE, m_fSectorAngle);
	DDV_MinMaxDouble(pDX, m_fSectorAngle, 0., 360.);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_EXTERNAL_DIAMETER, m_fExternalDiameter);
	DDX_Text(pDX, IDC_E_INTERNAL_DIAMETER, m_fInternalDiameter);
	DDX_Text(pDX, IDC_E_NORMAL_OFFSET, m_fNormalOffset);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_Text(pDX, IDC_E_PLATE_SEG_I, m_sSegI);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCircularPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CCircularPlateDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_NORM, OnBnPasteNorm)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangeEPartNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCircularPlateDlg message handlers

BOOL CCircularPlateDlg::OnInitDialog() 
{
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	AddSteelMatRecord(pCMB);
	RefreshDlgCtrlState();
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCircularPlateDlg::RefreshDlgCtrlState()
{
	if(m_iDlgType==1)	//封头板
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Tube Head Plate");
#else 
		SetWindowText("钢管封头板");
#endif
		GetDlgItem(IDC_E_START_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_SECTOR_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_INTERNAL_DIAMETER)->EnableWindow(FALSE);
	}
	else if(m_iDlgType==2)//流水板 
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Water Plate");
#else 
		SetWindowText("流水板");
#endif
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Circular Plate");
#else 
		SetWindowText("环向板");
#endif
		GetDlgItem(IDC_E_START_ANGLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_SECTOR_ANGLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_INTERNAL_DIAMETER)->EnableWindow(TRUE);
	}
}

void CCircularPlateDlg::OnBnPasteNorm() 
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

void CCircularPlateDlg::OnChangeEPartNo() 
{
	UpdateData();
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo.GetBuffer(),&iSeg,NULL))
		m_sSegI=iSeg.ToString();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if(::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0)
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNo);
	if(pPart)
	{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
		if(pPart->cPartType==BOMPART::PLATE)
		{
			m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
			m_nPlateThick=ftoi(pPart->thick);
		}
		//else
		//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	UpdateData(FALSE);
}
