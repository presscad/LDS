// DesignPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignPlateDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "DataCompare.h"
#include "LocalFeatureDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignPlateDlg dialog


CDesignPlateDlg::CDesignPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignPlateDlg)
	m_iMaterial=0;
	m_sPartNo = _T("");
	m_nThick=8;
	m_fNormOffset = 0.0;
	m_pDatumPart=NULL;
	m_iDesStyle=0;
	m_nWasherThick=3;
	m_nWasherNum=1;
	m_fDianQuanOffset=0;
	m_bDatumPartTop=TRUE;
	m_pBoltPad=NULL;
	//}}AFX_DATA_INIT
}


void CDesignPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignPlateDlg)
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Radio(pDX, IDC_RDO_DIAN_BAN, m_iDesStyle);
	DDX_Text(pDX, IDC_E_THICK, m_nThick);
	DDX_Text(pDX, IDC_E_NORM_OFFSET, m_fNormOffset);
	DDX_Check(pDX, IDC_CHK_DATUM_PART_TOP, m_bDatumPartTop);
	DDX_Text(pDX, IDC_E_NUM, m_nWasherNum);
	DDX_Text(pDX, IDC_E_THICK2, m_nWasherThick);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignPlateDlg)
	ON_BN_CLICKED(IDC_CHK_DATUM_PART_TOP, &CDesignPlateDlg::OnBnClickedDatumPartTop)
	ON_EN_CHANGE(IDC_E_NORM_OFFSET, &CDesignPlateDlg::OnEnChangeENormOffset)
	ON_EN_CHANGE(IDC_E_THICK, &CDesignPlateDlg::OnEnChangeEThick)
	ON_EN_CHANGE(IDC_E_THICK2, &CDesignPlateDlg::OnEnChangeEWasher)
	ON_EN_CHANGE(IDC_E_NUM,&CDesignPlateDlg::OnEnChangeEWasher)
	ON_BN_CLICKED(IDC_RDO_DIAN_BAN, &CDesignPlateDlg::OnBnClickedRdoDianBan)
	ON_BN_CLICKED(IDC_RDO_DIAN_QUAN, &CDesignPlateDlg::OnBnClickedRdoDianQuan)
	ON_EN_CHANGE(IDC_E_PART_NO, &CDesignPlateDlg::OnChangePartNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignPlateDlg message handlers

BOOL CDesignPlateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CComboBox*pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iMaterial);
	m_bSpreadState=TRUE;
	m_bAutoCalWash=true;
	CalNormOffset();
	RelayoutWnd();
	PreviewBoltPad();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CDesignPlateDlg::CalNormOffset()
{
	if(m_iDesStyle==0)
	{
		if(m_pDatumPart->IsPlate())
			m_fNormOffset=m_bDatumPartTop?0:-m_pDatumPart->GetThick()-m_nThick;
		else//if(m_pDatumPart->IsAngle())
			m_fNormOffset=m_bDatumPartTop?0:-(m_pDatumPart->GetThick()+m_nThick);
	}
	else
	{
		CLDSBolt* pBolt=m_pBoltPad->GetFirstLsRef()->GetLsPtr();
		if(m_bAutoCalWash)
			m_nWasherThick=CLDSPart::library->GetBestMatchWasherThick(pBolt->get_d(),m_nThick);
		m_nWasherNum=(m_nThick>m_nWasherThick)?m_nThick/m_nWasherThick:1;
		pBolt->DianQuan.N=m_nWasherNum;
		pBolt->DianQuan.thick=m_nWasherThick;
		PARTSET partset;
		CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
		pView->GetBoltActiveRelaPartSet(pBolt,partset,NULL);
		pBolt->CalGasketOffset(partset);
		CXhChar100 sValue;
		sprintf(sValue,"%f",pBolt->DianQuan.offset);
		SimplifiedNumString(sValue);
		m_fDianQuanOffset=atof(sValue);
	}
}
void CDesignPlateDlg::RelayoutWnd()
{
	GetDlgItem(IDC_WORK_PANEL)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHK_DATUM_PART_TOP)->ShowWindow(m_iDesStyle==0);
	GetDlgItem(IDC_S_NORM_OFFSET)->ShowWindow(m_iDesStyle==0);
	GetDlgItem(IDC_E_NORM_OFFSET)->ShowWindow(m_iDesStyle==0);
	GetDlgItem(IDC_S_THICK)->ShowWindow(m_iDesStyle==0);
	GetDlgItem(IDC_E_THICK)->ShowWindow(m_iDesStyle==0);
	GetDlgItem(IDC_S_PART_NO)->ShowWindow(m_iDesStyle==0);
	GetDlgItem(IDC_E_PART_NO)->ShowWindow(m_iDesStyle==0);
	GetDlgItem(IDC_S_MATERIAL)->ShowWindow(m_iDesStyle==0);
	GetDlgItem(IDC_CMB_MATERIAL)->ShowWindow(m_iDesStyle==0);
	GetDlgItem(IDC_E_THICK2)->ShowWindow(m_iDesStyle==1);
	GetDlgItem(IDC_S_THICK2)->ShowWindow(m_iDesStyle==1);
	GetDlgItem(IDC_E_NUM)->ShowWindow(m_iDesStyle==1);
	GetDlgItem(IDC_S_NUM)->ShowWindow(m_iDesStyle==1);
	//
	CRect panel_rc;
	GetDlgItem(IDC_WORK_PANEL)->GetWindowRect(panel_rc);
	long nOffset=0;
	if(m_iDesStyle==0 && !m_bSpreadState)
	{
		nOffset=panel_rc.Height();
		m_bSpreadState=TRUE;
	}
	else if(m_iDesStyle==1 && m_bSpreadState)
	{	
		nOffset=-panel_rc.Height();
		m_bSpreadState=FALSE;
	}
	//调整对话框大小
	CRect full_rc;
	GetWindowRect(full_rc);
	full_rc.bottom+=nOffset;
	MoveWindow(full_rc);
	//调整按钮位置
	CWnd *pBtn=NULL;
	for(int i=0;i<2;i++)
	{
		if(i==0)
			pBtn=GetDlgItem(IDOK);
		else
			pBtn=GetDlgItem(IDCANCEL);
		CRect rcBtn;
		pBtn->GetWindowRect(rcBtn);
		ScreenToClient(rcBtn);
		rcBtn.top+=nOffset;
		rcBtn.bottom+=nOffset;
		pBtn->MoveWindow(rcBtn);
	}
}
void CDesignPlateDlg::PreviewBoltPad()
{
	if(m_pBoltPad==NULL)
		return;
	if(m_iDesStyle==0)
	{
		CLDSPlate* pPad=m_pBoltPad;
		pPad->m_fNormOffset=m_fNormOffset;
		pPad->Thick=m_nThick;
		pPad->DesignSetupUcs();
		pPad->CalStdProfile();
		pPad->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pPad->GetSolidPartObject());
		g_pSolidDraw->SetEntSnapStatus(pPad->handle);
	}
	else
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->DelEnt(m_pBoltPad->handle);
	}
	for(CLsRef *pLsRef=m_pBoltPad->GetFirstLsRef();pLsRef;pLsRef=m_pBoltPad->GetNextLsRef())
	{	
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		pBolt->DianQuan.N=(m_iDesStyle==1?m_nWasherNum:0);
		pBolt->DianQuan.thick=(m_iDesStyle==1?m_nWasherThick:0);
		pBolt->DianQuan.offset=(m_iDesStyle==1?m_fDianQuanOffset:0);
		pBolt->DianQuan.AutoMatchThick=(m_iDesStyle==1?m_bAutoCalWash:false);
		pBolt->CalGuigeAuto();
		pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
}
//
void CDesignPlateDlg::OnBnClickedDatumPartTop()
{
	UpdateData(TRUE);
	CalNormOffset();
	UpdateData(FALSE);
	PreviewBoltPad();
}
void CDesignPlateDlg::OnEnChangeENormOffset()
{
	CString sText;
	GetDlgItem(IDC_E_NORM_OFFSET)->GetWindowText(sText);
	if(sText.CompareNoCase("-")==0&&sText.GetLength()==1)
		return;
	UpdateData(TRUE);
	PreviewBoltPad();
}
void CDesignPlateDlg::OnEnChangeEThick()
{
	UpdateData(TRUE);
	PreviewBoltPad();
}
void CDesignPlateDlg::OnEnChangeEWasher()
{
	UpdateData();
	m_bAutoCalWash=false;
	//CalNormOffset();
	PreviewBoltPad();
}
void CDesignPlateDlg::OnBnClickedRdoDianBan()
{
	UpdateData(TRUE);
	RelayoutWnd();
	PreviewBoltPad();
}
void CDesignPlateDlg::OnBnClickedRdoDianQuan()
{
	UpdateData(TRUE);
	m_bAutoCalWash=true;
	CalNormOffset();
	UpdateData(FALSE);
	RelayoutWnd();
	PreviewBoltPad();
}
void CDesignPlateDlg::OnChangePartNo()
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
			m_nThick=(UINT)pPart->thick;
			m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
		}
		//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
	}
#endif
	UpdateData(FALSE);
}
void CDesignPlateDlg::OnCancel()
{	//取消设计，清空螺栓的垫圈信息，并重新设计
	for(CLsRef *pLsRef=m_pBoltPad->GetFirstLsRef();pLsRef;pLsRef=m_pBoltPad->GetNextLsRef())
	{	
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		pBolt->DianQuan.N=0;
		pBolt->DianQuan.thick=0;
		pBolt->DianQuan.offset=0;
		pBolt->CalGuigeAuto();
		pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return CDialog::OnCancel();
}