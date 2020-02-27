// DesignOverlappendPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignOverlappendPlateDlg.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignOverlappendPlateDlg dialog


CDesignOverlappendPlateDlg::CDesignOverlappendPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignOverlappendPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignOverlappendPlateDlg)
	m_iMaterial=0;
	m_sSegI=_T("");
	m_nThick=8;
	m_sPartNo = _T("");
	m_fBaseOffsetX = 0.0;
	m_fBaseOffsetY = 0.0;
	m_fBaseOffsetZ = 0.0;
	//}}AFX_DATA_INIT
}


void CDesignOverlappendPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignOverlappendPlateDlg)
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_THICK, m_nThick);
	DDX_Text(pDX, IDC_E_BASE_OFFSET_X, m_fBaseOffsetX);
	DDX_Text(pDX, IDC_E_BASE_OFFSET_Y, m_fBaseOffsetY);
	DDX_Text(pDX, IDC_E_BASE_OFFSET_Z, m_fBaseOffsetZ);
	DDX_Check(pDX, IDC_CHK_WCS, m_bChooseWCS);
	DDX_Check(pDX, IDC_CHK_UCS, m_bChooseUCS);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignOverlappendPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignOverlappendPlateDlg)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangeEPartNo)
	ON_BN_CLICKED(IDC_CHK_WCS, OnChkEnableWCS)
	ON_BN_CLICKED(IDC_CHK_UCS, OnChkEnableUCS)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignOverlappendPlateDlg message handlers

BOOL CDesignOverlappendPlateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CComboBox*pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iMaterial);

	m_bChooseWCS=FALSE;
	m_bChooseUCS=TRUE;
	UpdateData(FALSE);
	return TRUE;
}

void CDesignOverlappendPlateDlg::OnChangeEPartNo() 
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
	if(pPart)
	{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
		if(pPart->cPartType==BOMPART::PLATE)
		{
			m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
			m_nThick=ftoi(pPart->thick);
		}
		//else
		//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	UpdateData(FALSE);
}
void CDesignOverlappendPlateDlg::OnChkEnableWCS()
{
	UpdateData();
	m_bChooseUCS=!m_bChooseWCS;
	UpdateData(FALSE);
}
void CDesignOverlappendPlateDlg::OnChkEnableUCS()
{
	UpdateData();
	m_bChooseWCS=!m_bChooseUCS;
	UpdateData(FALSE);
}