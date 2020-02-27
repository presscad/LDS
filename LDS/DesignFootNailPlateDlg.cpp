//<LOCALE_TRANSLATE Confirm by hxr/>
// DesignFootNailPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignFootNailPlateDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CDesignFootNailPlateDlg dialog


CDesignFootNailPlateDlg::CDesignFootNailPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignFootNailPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignFootNailPlateDlg)
	m_fParamH = 50;
	m_fParamL = 50;
	m_fParamT = 5;
	m_fParamW = 50;
	m_iMaterial = 0;
	m_sSegNo = _T("");
	m_sPartNo = _T("");
	m_sGuiGe = _T("");
	//}}AFX_DATA_INIT
	m_pLineTube = NULL;
	m_iBoltD = 20;
}


void CDesignFootNailPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignFootNailPlateDlg)
	DDX_Text(pDX, IDC_E_PARAM_HEIGHT, m_fParamH);
	DDX_Text(pDX, IDC_E_PARAM_LEN, m_fParamL);
	DDX_Text(pDX, IDC_E_PARAM_THICK, m_fParamT);
	DDX_Text(pDX, IDC_E_PARAM_WIDTH, m_fParamW);
	DDX_Text(pDX, IDC_E_LINESLOT_SEG_NO, m_sSegNo);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_CBString(pDX, IDC_CMB_LINESLOT_GUIGE, m_sGuiGe);
	DDX_CBIndex(pDX, IDC_CMB_LINESLOT_MATERIAL, m_iMaterial);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignFootNailPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignFootNailPlateDlg)
	ON_EN_CHANGE(IDC_E_PART_NO, OnChangePartNo)
	ON_CBN_SELCHANGE(IDC_CMB_LINESLOT_GUIGE, OnSelchangeGuige)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignFootNailPlateDlg message handlers

void CDesignFootNailPlateDlg::OnOK() 
{
	UpdateData();
	if(!m_pLineTube)
		return;
	if((m_fParamH-2*m_fParamT)>m_pLineTube->GetDiameter())
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("specification of slot is too big,please select reasonable specification again!");
#else
		AfxMessageBox("槽钢规格过大，请重新选择合理规格！");
#endif
	else
		CDialog::OnOK();
}

void CDesignFootNailPlateDlg::OnChangePartNo() 
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
	{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
		if( (part_type==1&&pPart->cPartType==BOMPART::ANGLE)||
			(part_type==0&&pPart->cPartType==BOMPART::SLOT))
		{
			m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
			if(part_type==1)
				m_sGuiGe.Format("%.fx%.f",pPart->wide,pPart->thick);
			else
				m_sGuiGe=pPart->GetSpec();
		}
		//else
		//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	UpdateData(FALSE);
}

BOOL CDesignFootNailPlateDlg::OnInitDialog() 
{
	if(m_pLineTube==NULL)
		return FALSE;
	CComboBox* pCMBMate = (CComboBox*)GetDlgItem(IDC_CMB_LINESLOT_MATERIAL);
	AddSteelMatRecord(pCMBMate);
	m_sSegNo = m_pLineTube->iSeg.ToString();
	m_sPartNo.Format("%s",(char*)m_pLineTube->GetPartNo());
	GetDlgItem(IDC_E_PARAM_HEIGHT)->EnableWindow(FALSE);
	GetDlgItem(IDC_E_PARAM_WIDTH)->EnableWindow(FALSE);
	GetDlgItem(IDC_E_PARAM_THICK)->EnableWindow(FALSE);
	if(part_type==1)
	{
		m_fParamL=200;
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("angle foot nail");
#else
		SetWindowText("角钢脚钉");
#endif
		CComboBox* pCMBJg=(CComboBox*)GetDlgItem(IDC_CMB_LINESLOT_GUIGE);
		AddJgRecord(pCMBJg);
		pCMBJg->SetCurSel(1);
		m_fParamL=150;	//临时给的一个默认长度值
	}
	else if(part_type==0)
	{
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,m_iBoltD);
		m_fParamL=LsSpace.EndSpace*2;
		/*m_fParamW=LsSpace.EndSpace*2;
		m_fParamH=LsSpace.EndSpace*2;*/
		CComboBox* pCMBSlot=(CComboBox*)GetDlgItem(IDC_CMB_LINESLOT_GUIGE);
		AddSlotRecord(pCMBSlot);
		if(m_sGuiGe.IsEmpty())
		{
			pCMBSlot->SetCurSel(1);
			pCMBSlot->GetLBText(1,m_sGuiGe);
		}
		SLOT_PARA_TYPE *pSlotPara=FindSlotType(m_sGuiGe.GetBuffer(16));
		if(pSlotPara)
		{
			m_fParamW=pSlotPara->b;
			m_fParamH=pSlotPara->h;
			m_fParamT=pSlotPara->d;
			m_fParamL=pSlotPara->h;
		}
	}
	CDialog::OnInitDialog();
	return TRUE;
}

void CDesignFootNailPlateDlg::OnSelchangeGuige() 
{
	UpdateData();
	SLOT_PARA_TYPE *pSlotPara=FindSlotType(m_sGuiGe.GetBuffer(16));
	if(pSlotPara)
	{
		m_fParamW=pSlotPara->b;
		m_fParamH=pSlotPara->h;
		m_fParamT=pSlotPara->d;
		m_fParamL=pSlotPara->h;
	}
	UpdateData(FALSE);
}
#endif