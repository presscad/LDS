// PlateBasicParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "PlateBasicParaDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "DataCompare.h"
#include "LocalFeatureDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlateBasicParaDlg dialog


CPlateBasicParaDlg::CPlateBasicParaDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CPlateBasicParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlateBasicParaDlg)
	m_sPartNo = _T("");
	m_nPlankThick = 8;
	m_iMaterial = 0;
	m_sSegI = _T("");
	m_iProfileType = 0;
	m_nWeldWidth = 60;
	m_bAdjustWeldLen = TRUE;
	m_fEdgeSpace=g_sysPara.fInsertPlateEdgeSpace;
	//}}AFX_DATA_INIT
	m_bEnableWeld=TRUE;
}


void CPlateBasicParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlateBasicParaDlg)
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDV_MaxChars(pDX, m_sPartNo, 8);
	DDX_Text(pDX, IDC_PLANK_THICK, m_nPlankThick);
	DDX_CBIndex(pDX, IDC_CMB_PLANK_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDV_MaxChars(pDX, m_sSegI, 10);
	DDX_CBIndex(pDX, IDC_CMB_PROFILE_TYPE, m_iProfileType);
	DDX_Text(pDX, IDC_E_WELD_WIDTH, m_nWeldWidth);
	DDX_Check(pDX, IDC_CHK_ADJUST_WELD_LEN, m_bAdjustWeldLen);
	DDX_Text(pDX, IDC_E_EDGE_DISTANCE, m_fEdgeSpace);
	//}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(CPlateBasicParaDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CPlateBasicParaDlg)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangeEPartNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlateBasicParaDlg message handlers

BOOL CPlateBasicParaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CComboBox* pCMB=(CComboBox*)GetDlgItem(IDC_CMB_PLANK_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iMaterial);
	GetDlgItem(IDC_E_WELD_WIDTH)->EnableWindow(m_bEnableWeld);
	GetDlgItem(IDC_CHK_ADJUST_WELD_LEN)->EnableWindow(m_bEnableWeld);
	m_bAdjustWeldLen=m_bEnableWeld;
	MoveWndToLocation();
	return TRUE;
}

void CPlateBasicParaDlg::OnChangeEPartNo() 
{
	UpdateData();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNo);
	if(pPart)
	{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
		if(pPart->cPartType==BOMPART::PLATE)
		{
			m_nPlankThick=(UINT)pPart->thick;
			m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		}
		//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo,&iSeg,NULL))
		m_sSegI=iSeg.ToString();
	UpdateData(FALSE);
}

