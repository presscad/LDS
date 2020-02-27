// DesignConnectionPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignConnectionPlateDlg.h"
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
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CDesignConnectionPlateDlg dialog
CDesignConnectionPlateDlg::CDesignConnectionPlateDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CDesignConnectionPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignConnectionPlateDlg)
	m_bAdjustWeldLen = TRUE;
	m_bSpecWeldScope = FALSE;
	m_sPartNo = _T("");
	m_iPlateMaterial = 0;
	m_iFaceN = 0;
	m_sSegI = _T("");
	m_nPlateThick = 0;
	m_fWeldEndHigh = 0.0;
	m_fWeldStartHigh = 0.0;
	m_fWeldWidth = 60.0;
	m_fEdgeSpace = g_sysPara.fInsertPlateEdgeSpace;
	//}}AFX_DATA_INIT
	m_pLinePart=NULL;
}

void CDesignConnectionPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignConnectionPlateDlg)
	DDX_Check(pDX, IDC_CHK_ADJUST_WELD_LEN, m_bAdjustWeldLen);
	DDX_Check(pDX, IDC_CHK_SPEC_WELD_SCOPE, m_bSpecWeldScope);
	DDX_CBIndex(pDX, IDC_CMB_PLATE_MATERIAL, m_iPlateMaterial);
	DDX_CBIndex(pDX, IDC_CMB_FACE_N, m_iFaceN);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_PLATE_THICK, m_nPlateThick);
	DDX_Text(pDX, IDC_E_WELD_END_HIGH, m_fWeldEndHigh);
	DDX_Text(pDX, IDC_E_WELD_START_HIGH, m_fWeldStartHigh);
	DDX_Text(pDX, IDC_E_WELD_WIDTH, m_fWeldWidth);
	DDX_Text(pDX,IDC_E_EDGE_SPACE,m_fEdgeSpace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignConnectionPlateDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CDesignConnectionPlateDlg)
	ON_BN_CLICKED(IDC_CHK_SPEC_WELD_SCOPE, OnChkSpecWeldScope)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangePartNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignConnectionPlateDlg message handlers

BOOL CDesignConnectionPlateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//连接板命令暂时只支持做单面板
	GetDlgItem(IDC_CMB_FACE_N)->EnableWindow(FALSE);
	if(m_pLinePart==NULL||m_pLinePart->GetClassTypeId()!=CLS_LINETUBE)
	{	//基准杆件不是钢管
		m_bSpecWeldScope=FALSE;
		GetDlgItem(IDC_E_WELD_WIDTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_S_WELD_START_HIGH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_WELD_END_HIGH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_WELD_START_HIGH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_WELD_END_HIGH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_SPEC_WELD_SCOPE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_ADJUST_WELD_LEN)->ShowWindow(SW_HIDE);
		m_bAdjustWeldLen=m_bSpecWeldScope=FALSE;
		//调整对话框大小及确定取消按钮的位置
		CRect rc;
		const int SPACE_WIDTH=80;
		GetWindowRect(rc);
		rc.bottom-=SPACE_WIDTH;
		MoveWindow(rc,TRUE);
		for(int i=0;i<2;i++)
		{
			CWnd *pSonWnd=GetDlgItem(i==0?IDOK:IDCANCEL);
			pSonWnd->GetWindowRect(rc);
			ScreenToClient(rc);
			rc.top-=SPACE_WIDTH;
			rc.bottom-=SPACE_WIDTH;
			pSonWnd->MoveWindow(rc);
		}
	}
	GetDlgItem(IDC_E_WELD_START_HIGH)->EnableWindow(m_bSpecWeldScope);
	GetDlgItem(IDC_E_WELD_END_HIGH)->EnableWindow(m_bSpecWeldScope);
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_PLATE_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iPlateMaterial);
	MoveWndToLocation();
	return TRUE;
}

void CDesignConnectionPlateDlg::OnChkSpecWeldScope() 
{
	UpdateData();
	GetDlgItem(IDC_E_WELD_START_HIGH)->EnableWindow(m_bSpecWeldScope);
	GetDlgItem(IDC_E_WELD_END_HIGH)->EnableWindow(m_bSpecWeldScope);
	UpdateData(FALSE);
}

void CDesignConnectionPlateDlg::OnChangePartNo() 
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
			m_nPlateThick=(UINT)pPart->thick;
			m_iPlateMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
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
#endif