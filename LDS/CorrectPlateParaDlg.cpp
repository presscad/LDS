// CorrectPlateParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "CorrectPlateParaDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCorrectPlateParaDlg dialog


CCorrectPlateParaDlg::CCorrectPlateParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCorrectPlateParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCorrectPlateParaDlg)
	m_bCorrectAngleProfilePara = FALSE;
	m_bCorrectLsInfo = FALSE;
	m_iPlateMaterial = 0;
	m_iProfileStyle = 0;
	m_sPlatePartNo = _T("");
	m_sPlateSegI = _T("1");
	m_nPlateThick = 0;
	m_bCalLinePartOddment = FALSE;
	//}}AFX_DATA_INIT
	m_bGuiGeMatch=FALSE;
}


void CCorrectPlateParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCorrectPlateParaDlg)
	DDX_Check(pDX, IDC_CHK_CORRECT_ANGLE_PROFILE_PARA, m_bCorrectAngleProfilePara);
	DDX_Check(pDX, IDC_CHK_CORRECT_LS_INFO, m_bCorrectLsInfo);
	DDX_CBIndex(pDX, IDC_CMB_PLATE_MATERIAL, m_iPlateMaterial);
	DDX_CBIndex(pDX, IDC_CMB_PROFILE_STYLE, m_iProfileStyle);
	DDX_Text(pDX, IDC_E_PLATE_PART_NO, m_sPlatePartNo);
	DDX_Text(pDX, IDC_E_PLATE_SEG_I, m_sPlateSegI);
	DDX_Text(pDX, IDC_E_PLATE_THICK, m_nPlateThick);
	DDX_Check(pDX, IDC_CHK_CAL_LINEPART_ODDMENT, m_bCalLinePartOddment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCorrectPlateParaDlg, CDialog)
	//{{AFX_MSG_MAP(CCorrectPlateParaDlg)
	ON_EN_CHANGE(IDC_E_PLATE_PART_NO, OnChangePlatePartNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCorrectPlateParaDlg message handlers

void CCorrectPlateParaDlg::OnChangePlatePartNo() 
{
	UpdateData();
	SEGI iSeg;
	if(ParsePartNo(m_sPlatePartNo.GetBuffer(),&iSeg,NULL))
		m_sPlateSegI=iSeg.ToString();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPlatePartNo);
	if(pPart)
	{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
		if(pPart->cPartType==BOMPART::PLATE)
		{
			m_iPlateMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
			m_nPlateThick=ftoi(pPart->thick);
		}
		//else
		//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	UpdateData(FALSE);
}

BOOL CCorrectPlateParaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CComboBox*pCMB = (CComboBox*)GetDlgItem(IDC_CMB_PLATE_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iPlateMaterial);
	GetDlgItem(IDC_CHK_CORRECT_ANGLE_PROFILE_PARA)->EnableWindow(!m_bGuiGeMatch);
	GetDlgItem(IDC_CHK_CORRECT_LS_INFO)->EnableWindow(!m_bGuiGeMatch);
	return TRUE;
}
