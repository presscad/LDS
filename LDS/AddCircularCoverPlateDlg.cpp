// AddCircularCoverPlate.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "AddCircularCoverPlateDlg.h"
#include "GlobalFunc.h"
#include "PartsDbDlg.h"
#include "query.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef __ANGLE_PART_INC_

/////////////////////////////////////////////////////////////////////////////
// CAddCircularCoverPlateDlg dialog


CAddCircularCoverPlateDlg::CAddCircularCoverPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddCircularCoverPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddCircularCoverPlateDlg)
	m_fParaD = 30.0;
	m_fParaW = 60.0;
	m_sPartNo = _T("");
	m_sSegI = _T("");
	m_nThick = 8;
	m_iMaterial = 0;
	m_fParaB = 180.0;
	m_bIncDownPlate = TRUE;
	m_bIncUpPlate = TRUE;
	m_bMirSide = FALSE;
	m_fParaA = 0.0;
	//}}AFX_DATA_INIT
	pDatumPlate1 = NULL;
	pDatumPlate2 = NULL;
}


void CAddCircularCoverPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddCircularCoverPlateDlg)
	DDX_Text(pDX, IDC_E_PARA_D, m_fParaD);
	DDX_Text(pDX, IDC_E_PARA_W, m_fParaW);
	DDX_Text(pDX, IDC_E_PLATE_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_PLATE_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_PLATE_T, m_nThick);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_PARA_B, m_fParaB);
	DDX_Check(pDX, IDC_CHK_INC_DOWN, m_bIncDownPlate);
	DDX_Check(pDX, IDC_CHK_INC_UP, m_bIncUpPlate);
	DDX_Check(pDX, IDC_CHK_MIR_SIDE, m_bMirSide);
	DDX_Text(pDX, IDC_E_PARA_A, m_fParaA);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddCircularCoverPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CAddCircularCoverPlateDlg)
	ON_BN_CLICKED(IDC_BN_PARTS_DB, OnBnPartsDb)
	ON_EN_CHANGE(IDC_E_PLATE_PART_NO, OnChangeEPlatePartNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddCircularCoverPlateDlg message handlers

void CAddCircularCoverPlateDlg::OnBnPartsDb() 
{
	CPartsDbDlg partsDlg;
	partsDlg.m_bRefreshDlgCtrl = TRUE;
	partsDlg.m_bIncParamCirLid = TRUE;
	if(partsDlg.DoModal()==IDOK)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)partsDlg.m_pSelectPart;
		if(!pParamPlate)
			return;
		if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{
			pParamPlate->GetDesignItemValue('B',&m_fParaB);
			pParamPlate->GetDesignItemValue('D',&m_fParaD);
			pParamPlate->GetDesignItemValue('W',&m_fParaW);
			m_nThick = (int)pParamPlate->thick;
			m_sSegI = pParamPlate->iSeg.ToString();
			m_sPartNo.Format("%s",(char*)pParamPlate->GetPartNo());
			m_iMaterial=QuerySteelMatIndex(pParamPlate->cMaterial);
			UpdateData(FALSE);
		}
	}
}

void CAddCircularCoverPlateDlg::OnChangeEPlatePartNo() 
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
			m_nThick=(int)pPart->thick;
		}
		//else
		//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	UpdateData(FALSE);
}

BOOL CAddCircularCoverPlateDlg::OnInitDialog() 
{
	if(pDatumPlate1==NULL&&pDatumPlate2==NULL)
		return FALSE;
	if(pDatumPlate1&&pDatumPlate2==NULL)
		GetDlgItem(IDC_CHK_MIR_SIDE)->EnableWindow(TRUE);
	else
	{
		GetDlgItem(IDC_CHK_MIR_SIDE)->EnableWindow(FALSE);
		m_bMirSide=FALSE;
	}
	CDialog::OnInitDialog();
	return TRUE;
}
#endif