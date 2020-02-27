// HuoQuJg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "HuoQuJgPropDlg.h"
#include "CfgPartNoDlg.h"
#include "Query.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHuoQuJgPropDlg dialog


CHuoQuJgPropDlg::CHuoQuJgPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHuoQuJgPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHuoQuJgPropDlg)
	m_fHuoQuR = 0.0;
	m_iSegI = 0;
	m_sJgGuige = _T("");
	m_iJgMat = 0;
	m_sCfgPartNo = _T("");
	m_sNotes = _T("");
	m_sPartNo = _T("");
	m_fRealLen = 0.0;
	m_uStatMatCoef = 0;
	m_iStatMatNo = 0;
	m_hNextJg = _T("");
	m_hStartJg = _T("");
	m_hHuoQuJgHandle = _T("");
	//}}AFX_DATA_INIT
}


void CHuoQuJgPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHuoQuJgPropDlg)
	DDX_Text(pDX, IDC_E_SEG_JG_HUOQU_R, m_fHuoQuR);
	DDX_Text(pDX, IDC_BELONG_SEG_I, m_iSegI);
	DDX_CBString(pDX, IDC_CMB_JG_GUIEGE, m_sJgGuige);
	DDX_CBIndex(pDX, IDC_CMB_JG_MATERIAL, m_iJgMat);
	DDX_Text(pDX, IDC_E_CFG_PART_NO, m_sCfgPartNo);
	DDX_Text(pDX, IDC_E_NOTES, m_sNotes);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_REAL_LEN, m_fRealLen);
	DDX_Text(pDX, IDC_E_STAT_MAT_COEF, m_uStatMatCoef);
	DDX_Text(pDX, IDC_E_STAT_MAT_NO, m_iStatMatNo);
	DDX_Text(pDX, IDC_S_END_JG_HANDLE, m_hNextJg);
	DDX_Text(pDX, IDC_S_START_JG_HANDLE, m_hStartJg);
	DDX_Text(pDX, IDC_S_HUOQU_JG_HANDLE, m_hHuoQuJgHandle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHuoQuJgPropDlg, CDialog)
	//{{AFX_MSG_MAP(CHuoQuJgPropDlg)
	ON_BN_CLICKED(IDC_BN_CFG_PART_NO, OnBnCfgPartNo)
	ON_BN_CLICKED(IDC_BN_HEAD_HUOQU_JOINT, OnBnHeadHuoquJoint)
	ON_BN_CLICKED(IDC_BN_NEXT_HUOQU_JOINT, OnBnNextHuoquJoint)
	ON_EN_CHANGE(IDC_E_SEG_JG_HUOQU_R, OnChangeESegJgHuoquR)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHuoQuJgPropDlg message handlers

BOOL CHuoQuJgPropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CLDSLineAngle *pFirst = m_pHuoQuJg->seg_arr.GetFirst();
	if(pFirst)
	{
		m_sJgGuige.Format("%.0fX%.0f",pFirst->GetWidth(),pFirst->GetThick());
		m_iSegI=pFirst->iSeg;
		if(pFirst->cMaterial=='S')
			m_iJgMat=0;
		else if(pFirst->cMaterial=='H')
			m_iJgMat=1;
		else
			m_iJgMat=2;
	}
	m_sPartNo=m_pHuoQuJg->sPartNo;
	m_fRealLen = m_pHuoQuJg->GetLength();
	m_uStatMatCoef = m_pHuoQuJg->m_uStatMatCoef;
	//m_iStatMatNo = m_pHuoQuJg->m_nStatMatNo;
	if(m_pHuoQuJg->seg_arr.GetNodeNum()==2)	//已到最后一个火曲点
		GetDlgItem(IDC_BN_NEXT_HUOQU_JOINT)->EnableWindow(FALSE);
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_JG_GUIEGE);
	AddJgRecord(pCMB);
	OnBnHeadHuoquJoint();
	UpdateCfgPartNoString();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHuoQuJgPropDlg::OnBnCfgPartNo() 
{
	CCfgPartNoDlg dlg;
	dlg.cfgword = m_pHuoQuJg->cfgword;
	if(dlg.DoModal()==IDOK)
	{
		m_pHuoQuJg->cfgword = dlg.cfgword;
		UpdateCfgPartNoString();
	}
}

void CHuoQuJgPropDlg::OnBnHeadHuoquJoint() 
{
	if(m_pHuoQuJg->seg_arr.GetNodeNum()>2)
		GetDlgItem(IDC_BN_NEXT_HUOQU_JOINT)->EnableWindow(TRUE);
	m_pStartJg=m_pHuoQuJg->seg_arr.GetFirst();
	m_pNextJg=m_pHuoQuJg->seg_arr.GetNext();
	m_hStartJg.Format("始端角钢:0X%X",m_pStartJg->handle);
	m_hNextJg.Format("终端角钢:0X%X",m_pNextJg->handle);
	m_fHuoQuR=m_pStartJg->huoqu_r;
	UpdateData(FALSE);
}

void CHuoQuJgPropDlg::OnBnNextHuoquJoint() 
{
	m_pStartJg=m_pNextJg;
	m_pNextJg=m_pHuoQuJg->seg_arr.GetNext();
	m_pHuoQuJg->seg_arr.push_stack();
	if(m_pHuoQuJg->seg_arr.GetTail()==m_pNextJg)	//已到最后一个火曲点
		GetDlgItem(IDC_BN_NEXT_HUOQU_JOINT)->EnableWindow(FALSE);
	m_hStartJg.Format("始端角钢:0X%X",m_pStartJg->handle);
	m_hNextJg.Format("终端角钢:0X%X",m_pNextJg->handle);
	m_fHuoQuR=m_pStartJg->huoqu_r;
	UpdateData(FALSE);
}

void CHuoQuJgPropDlg::UpdateCfgPartNoString()
{
	m_sCfgPartNo.Empty();
	for(int i=0;i<160;i++)
	{
		if(m_pHuoQuJg->cfgword.IsHasNo(i+1))
		{
			CString sText;
			sText.Format("%d",i+1);
			if(m_sCfgPartNo.GetLength()<=0)
				m_sCfgPartNo=sText;
			else
				m_sCfgPartNo+=","+sText;
		}
	}
	UpdateData(FALSE);
}

void CHuoQuJgPropDlg::OnOK() 
{
	CDialog::OnOK();
	double wing_wide,wing_thick;
	restore_JG_guige(m_sJgGuige,wing_wide,wing_thick);
	_snprintf(m_pHuoQuJg->sPartNo,MAX_CHAR_GUIGE_16,"%s",m_sPartNo);
	if(m_iJgMat==0)
		m_pHuoQuJg->cMaterial='S';
	else if(m_iJgMat==1)
		m_pHuoQuJg->cMaterial='H';
	else
		m_pHuoQuJg->cMaterial='G';
	m_pHuoQuJg->m_uStatMatCoef = m_uStatMatCoef;
	//m_pHuoQuJg->m_nStatMatNo = m_iStatMatNo;
	for(CLDSLineAngle *pSeg=m_pHuoQuJg->seg_arr.GetFirst();pSeg;pSeg=m_pHuoQuJg->seg_arr.GetNext())
	{
		pSeg->iSeg=m_iSegI;
		pSeg->set_wing_wide(wing_wide);
		pSeg->set_wing_thick(wing_thick);
		pSeg->cMaterial=m_pHuoQuJg->cMaterial;
		pSeg->m_uStatMatCoef=m_uStatMatCoef;
		//pSeg->m_nStatMatNo=m_iStatMatNo;
		pSeg->cfgword=m_pHuoQuJg->cfgword;
	}
}

void CHuoQuJgPropDlg::OnChangeESegJgHuoquR() 
{
	UpdateData();
	m_pStartJg->huoqu_r=m_fHuoQuR;
}
