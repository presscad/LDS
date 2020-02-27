// DesignSphereDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "DesignSphereDlg.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignSphereDlg dialog


CDesignSphereDlg::CDesignSphereDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignSphereDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignSphereDlg)
	m_sSphereGuige = _T("60X8");
	m_iSphereMat = 0;
	m_sPartNo = _T("");
	m_sSegNo = _T("");
	//}}AFX_DATA_INIT
}


void CDesignSphereDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignSphereDlg)
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_SEG_NO, m_sSegNo);
	DDX_CBString(pDX, IDC_CMB_SPHERE_GUIGE, m_sSphereGuige);
	DDX_CBIndex(pDX, IDC_CMB_SPHERE_MATERIAL, m_iSphereMat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignSphereDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignSphereDlg)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangePartNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignSphereDlg message handlers

BOOL CDesignSphereDlg::OnInitDialog() 
{
	CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_SPHERE_GUIGE);
	AddTubeRecord(pCMB);
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_SPHERE_MATERIAL);
	AddSteelMatRecord(pCMB);
	CDialog::OnInitDialog();
	return TRUE;
}

void CDesignSphereDlg::OnChangePartNo() 
{
	UpdateData();
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo.GetBuffer(),&iSeg,NULL))
		m_sSegNo=iSeg.ToString();
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sPartNo);
	if(pPart)
	{
		m_sSphereGuige.Format("%.fx%.f",pPart->wide,pPart->thick);
		m_iSphereMat=pPart->cMaterial;
	}
#endif
	UpdateData(FALSE);
}