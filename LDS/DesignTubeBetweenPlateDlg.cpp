// DesignTubeBetweenPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignTubeBetweenPlateDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CDesignTubeBetweenPlateDlg dialog


CDesignTubeBetweenPlateDlg::CDesignTubeBetweenPlateDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignTubeBetweenPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignTubeBetweenPlateDlg)
	m_bAutoCalcFirst = TRUE;
	m_bHasInterPos = TRUE;
	m_bLinkLinePart = FALSE;
	m_bMirCreate = FALSE;
	m_fFirstEdgeLen = 200.0;
	m_fFirstEdgeWidth = 80.0;
	m_fSecondEdgeLen = 200.0;
	m_fSecondEdgeWidth = 80.0;
	m_fTangentWidth = 60.0;
	m_fPlateThick = 10.0;
	m_iOnlyOnePlate = 0;	//0.生成一块钢管  1.生成两小块钢板
	m_sPartName = _T("");
	m_sSegI = _T("");
	m_sPartNo = _T("");
	m_iMaterial = 0;
	m_sTransectPartHandle = _T("0X0");
	m_bUseTangWidth = TRUE;
	m_bAutoCalcSecond = TRUE;
	//}}AFX_DATA_INIT
}


void CDesignTubeBetweenPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignTubeBetweenPlateDlg)
	DDX_Check(pDX, IDC_CHK_AUTO_CALC, m_bAutoCalcFirst);
	DDX_Check(pDX, IDC_CHK_HAS_INTER_POS, m_bHasInterPos);
	DDX_Check(pDX, IDC_CHK_LINK_LINEPART, m_bLinkLinePart);
	DDX_Check(pDX, IDC_CHK_MIR_CREATE, m_bMirCreate);
	DDX_Text(pDX, IDC_E_FIRST_EDGE_LEN, m_fFirstEdgeLen);
	DDX_Text(pDX, IDC_E_FIRST_EDGE_WIDTH, m_fFirstEdgeWidth);
	DDX_Text(pDX, IDC_E_SECOND_EDGE_LEN, m_fSecondEdgeLen);
	DDX_Text(pDX, IDC_E_SECOND_EDGE_WIDTH, m_fSecondEdgeWidth);
	DDX_Text(pDX, IDC_E_TANGENT_WIDTH, m_fTangentWidth);
	DDX_Text(pDX, IDC_E_THICK, m_fPlateThick);
	DDX_Radio(pDX, IDC_RAD_ONLY_ONE_PLATE, m_iOnlyOnePlate);
	DDX_Text(pDX, IDC_S_PART_NAME, m_sPartName);
	DDX_Text(pDX, IDC_E_SEG_I, m_sSegI);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_TRANSECT_PART_HANDLE, m_sTransectPartHandle);
	DDX_Check(pDX, IDC_CHK_USE_TANG_WIDTH, m_bUseTangWidth);
	DDX_Check(pDX, IDC_CHK_AUTO_CALC2, m_bAutoCalcSecond);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignTubeBetweenPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignTubeBetweenPlateDlg)
	ON_BN_CLICKED(IDC_BTN_SELECT_PART, OnSelectPart)
	ON_BN_CLICKED(IDC_CHK_HAS_INTER_POS, OnHasInterPos)
	ON_BN_CLICKED(IDC_CHK_AUTO_CALC, OnAutoCalc)
	ON_BN_CLICKED(IDC_RAD_ONLY_ONE_PLATE, OnOnlyOnePlate)
	ON_EN_CHANGE(IDC_E_TRANSECT_PART_HANDLE, OnChangeTransectPartHandle)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangePartNo)
	ON_BN_CLICKED(IDC_CHK_AUTO_CALC2, OnAutoCalc)
	ON_BN_CLICKED(IDC_RAD_TWO_PLATE, OnOnlyOnePlate)
	ON_BN_CLICKED(IDC_CHK_USE_TANG_WIDTH, OnUseTangWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignTubeBetweenPlateDlg message handlers

BOOL CDesignTubeBetweenPlateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CComboBox*pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	AddSteelMatRecord(pCMB);
	RefreshDlgCtrlState();
	if(!m_bInernalStart)
		pCMB->SetCurSel(m_iMaterial);
	else if(m_bInernalStart&&m_hPickObj>0)
		m_sTransectPartHandle.Format("0X%X",m_hPickObj);
	UpdateData(FALSE);
	return TRUE; 
}

void CDesignTubeBetweenPlateDlg::OnSelectPart() 
{	
	UpdateData();
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;	
	m_dwSelectPartsType=GetSingleWord(SELECTINDEX_LINETUBE)|GetSingleWord(SELECTINDEX_SPHERE);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCmdPickPrompt="Please select two tubes or spheres whose datum tube are consistent";
#else 
	m_sCmdPickPrompt="请选择相贯的钢管或球 ";
#endif
	m_iPickObjType=1;
	CDialog::OnOK();
}

void CDesignTubeBetweenPlateDlg::OnHasInterPos() 
{
	UpdateData();
	RefreshDlgCtrlState();
	UpdateData(FALSE);
}

void CDesignTubeBetweenPlateDlg::OnAutoCalc() 
{
	UpdateData();
	RefreshDlgCtrlState();
	UpdateData(FALSE);
}

void CDesignTubeBetweenPlateDlg::OnOnlyOnePlate() 
{
	UpdateData();
	RefreshDlgCtrlState();
	UpdateData(FALSE);
}

void CDesignTubeBetweenPlateDlg::RefreshDlgCtrlState()
{	//刷新空间显示状态
	GetDlgItem(IDC_E_TANGENT_WIDTH)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_USE_TANG_WIDTH)->EnableWindow(FALSE);
	if(!m_bHasInterPos)
	{	//两钢管实体无交点 需要选择相贯构件
		GetDlgItem(IDC_BTN_SELECT_PART)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_TRANSECT_PART_HANDLE)->EnableWindow(TRUE);
		long h=0;
		sscanf(m_sTransectPartHandle,"%X",&h);
		CLDSPart *pPart=console.FromPartHandle(h);
		if(pPart&&pPart->GetClassTypeId()==CLS_SPHERE)
		{	//两根基准钢管相贯的构件为球体
			GetDlgItem(IDC_RAD_ONLY_ONE_PLATE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RAD_TWO_PLATE)->EnableWindow(TRUE);
			if(m_iOnlyOnePlate==0)
				GetDlgItem(IDC_CHK_MIR_CREATE)->EnableWindow(FALSE);
			else if(m_iOnlyOnePlate==1)
			{
				GetDlgItem(IDC_CHK_MIR_CREATE)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHK_USE_TANG_WIDTH)->EnableWindow(TRUE);
				GetDlgItem(IDC_E_TANGENT_WIDTH)->EnableWindow(!m_bUseTangWidth);
			}
		}
		else
		{
			GetDlgItem(IDC_RAD_ONLY_ONE_PLATE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RAD_TWO_PLATE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHK_MIR_CREATE)->EnableWindow(FALSE);
			GetDlgItem(IDC_E_TANGENT_WIDTH)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHK_USE_TANG_WIDTH)->EnableWindow(FALSE);
		}
	}
	else
	{
		GetDlgItem(IDC_BTN_SELECT_PART)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_TRANSECT_PART_HANDLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RAD_ONLY_ONE_PLATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RAD_TWO_PLATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_MIR_CREATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_TANGENT_WIDTH)->EnableWindow(FALSE);
	}
	GetDlgItem(IDC_E_FIRST_EDGE_LEN)->EnableWindow(!m_bAutoCalcFirst);
	GetDlgItem(IDC_E_FIRST_EDGE_WIDTH)->EnableWindow(!m_bAutoCalcFirst);
	GetDlgItem(IDC_E_SECOND_EDGE_LEN)->EnableWindow(!m_bAutoCalcSecond);
	GetDlgItem(IDC_E_SECOND_EDGE_WIDTH)->EnableWindow(!m_bAutoCalcSecond);
}

void CDesignTubeBetweenPlateDlg::OnChangeTransectPartHandle() 
{
	UpdateData();
	long h=0;
	sscanf(m_sTransectPartHandle,"%X",&h);
	CLDSPart *pPart=console.FromPartHandle(h);
	if(pPart&&pPart->GetClassTypeId()==CLS_SPHERE)
		m_sPartName="球体";
	else if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
		m_sPartName="钢管";
	else 
		AfxMessageBox("选择了错误的构件，相贯基准构件只能是钢管或者球体！");
	RefreshDlgCtrlState();
	UpdateData(FALSE);
}

void CDesignTubeBetweenPlateDlg::OnChangePartNo() 
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
		m_fPlateThick=pPart->thick;
		m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
	}
#endif
	UpdateData(FALSE);
}

void CDesignTubeBetweenPlateDlg::OnUseTangWidth() 
{
	UpdateData();
	RefreshDlgCtrlState();
	UpdateData(FALSE);
}

void CDesignTubeBetweenPlateDlg::OnOK()
{
	CDialog::OnOK();
}
void CDesignTubeBetweenPlateDlg::OnCancel()
{
	CCallBackDialog::OnCancel();
}
#endif