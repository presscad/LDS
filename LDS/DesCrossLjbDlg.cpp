// DesCrossLjbDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesCrossLjbDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesCrossLjbDlg dialog


CDesCrossLjbDlg::CDesCrossLjbDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesCrossLjbDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesCrossLjbDlg)
	m_nThick = 0;
	m_nVertexDist = 5;
	m_sPartSegI = _T("");
	m_sPartNo = _T("");
	m_iProfileType = 0;
	m_iMaterial = 0;
	//}}AFX_DATA_INIT
}


void CDesCrossLjbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesCrossLjbDlg)
	DDX_Text(pDX, IDC_PLATE_THICK, m_nThick);
	DDX_Text(pDX, IDC_E_VERTEX_DIST, m_nVertexDist);
	DDX_Text(pDX, IDC_E_PART_SEG_I, m_sPartSegI);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_CBIndex(pDX, IDC_CMB_PROFILE_TYPE, m_iProfileType);
	DDX_CBIndex(pDX, IDC_CMB_PLANK_MATERIAL, m_iMaterial);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesCrossLjbDlg, CDialog)
	//{{AFX_MSG_MAP(CDesCrossLjbDlg)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangePartNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesCrossLjbDlg message handlers

BOOL CDesCrossLjbDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CComboBox*pCMB = (CComboBox*)GetDlgItem(IDC_CMB_PLANK_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iMaterial);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDesCrossLjbDlg::OnChangePartNo() 
{
	UpdateData();
	SEGI iSeg;
	if(ParsePartNo(m_sPartNo.GetBuffer(),&iSeg,NULL))
		m_sPartSegI=iSeg.ToString();
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
