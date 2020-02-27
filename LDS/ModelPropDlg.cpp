// ModelPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "ModelPropDlg.h"
#include "CfgPartNoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelPropDlg dialog


CModelPropDlg::CModelPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModelPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelPropDlg)
	m_sLegCfgNo = _T("");
	m_sModelName = _T("");
	m_fBurglarLsEndZ = 0.0;
	m_fBurglarLsStartZ = 0.0;
	m_iBodyLegNo = 0;
	m_sIncPartSegStr = _T("");
	m_fLandHigh = 0.0;
	m_fLockPadStartZ = 0.0;
	m_fLockPadEndZ = 0.0;
	m_fLegLevelHeightDiff = 1.0;
	//}}AFX_DATA_INIT
	m_dwLegCfgNo=1;
	m_pRelaHeight =NULL;
}


void CModelPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelPropDlg)
	DDX_Text(pDX, IDC_E_LEG_CFGNO, m_sLegCfgNo);
	DDX_Text(pDX, IDC_E_MODEL_NAME, m_sModelName);
	DDV_MaxChars(pDX, m_sModelName, 49);
	DDX_Text(pDX, IDC_E_MODEL_BURGLAR_LS_END_Z, m_fBurglarLsEndZ);
	DDX_Text(pDX, IDC_E_MODEL_BURGLAR_LS_START_Z, m_fBurglarLsStartZ);
	DDX_Text(pDX, IDC_E_BODY_LEGNO, m_iBodyLegNo);
	DDV_MinMaxInt(pDX, m_iBodyLegNo, 1, 24);
	DDX_Text(pDX, IDC_E_MODEL_INC_PART_SEG_STR, m_sIncPartSegStr);
	DDV_MaxChars(pDX, m_sIncPartSegStr, 101);
	DDX_Text(pDX, IDC_E_TA_LAND_HIGH, m_fLandHigh);
	DDX_Text(pDX, IDC_E_MODEL_LOCK_PAD_START_Z, m_fLockPadStartZ);
	DDX_Text(pDX, IDC_E_MODEL_LOCK_PAD_END_Z, m_fLockPadEndZ);
#ifndef AFX_TARG_ENU_ENGLISH
	DDX_Text(pDX, IDC_E_LEG_LEVEL_HEIGHT_DIFF, m_fLegLevelHeightDiff);
	DDV_MinMaxDouble(pDX, m_fLegLevelHeightDiff, 0.5, 3);
#endif
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelPropDlg, CDialog)
	//{{AFX_MSG_MAP(CModelPropDlg)
	ON_BN_CLICKED(IDC_BN_LEG_CFGNO, OnBnLegCfgno)
	ON_BN_CLICKED(IDC_BN_AUTO_GET_SEG_STR, OnBnAutoGetSegStr)
	ON_BN_CLICKED(IDOK, &CModelPropDlg::OnOK)
	ON_BN_CLICKED(IDC_BTN_INTELLI_RETRIEVE, &CModelPropDlg::OnBnIntelliRetrieve)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelPropDlg message handlers

BOOL CModelPropDlg::OnInitDialog() 
{
#ifndef	__LDS_
	GetDlgItem(IDC_E_TA_LAND_HIGH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_S_TA_LAND_HIGH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_S_MODEL_BURGLAR_LS)->ShowWindow(SW_SHOW);
#endif
	char cfg_str[500]="";
	if(m_dwLegCfgNo.GetLegScopeStr(cfg_str)>0)
		m_sLegCfgNo = cfg_str;
	if(m_pRelaHeight)
		m_fLegLevelHeightDiff = m_pRelaHeight->uiLevelHeight*0.001;
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModelPropDlg::OnBnLegCfgno() 
{
	CCfgPartNoDlg dlg;
	UpdateData();
	CFGLEG legword=m_dwLegCfgNo.ToCfgLeg();
	legword.SetBodyFlag(GetSingleWord(m_iBodyLegNo));
	dlg.cfgword=legword.ToCfgWord();
	dlg.cfg_style=CFG_LEG_NO;
	if(dlg.DoModal()==IDOK)
	{
		m_dwLegCfgNo=dlg.cfgword;
		char cfg_str[500]="";
		if(m_dwLegCfgNo.GetLegScopeStr(cfg_str)>0)
			m_sLegCfgNo = cfg_str;
		UpdateData(FALSE);
	}
}

void CModelPropDlg::OnBnAutoGetSegStr() 
{
	CLDSModule module;
	module.SetBodyNo(m_iBodyLegNo);
	module.m_dwLegCfgWord = m_dwLegCfgNo;
	//存在大于200的段号时下列算法会出现数组越界 wht 10-11-04
	/*bool seg_flag_arr[201];
	memset(seg_flag_arr,0,sizeof(bool)*201);
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(module.IsSonPart(pPart)&&pPart->iSeg>=0)
			seg_flag_arr[pPart->iSeg]=true;
	}
	int start_i=0,end_i=0;
	m_sIncPartSegStr.Empty();	//清空原字符串
	for(int i=0;i<201;i++)
	{
		if(!seg_flag_arr[i])	//此段不属于本统计模型
		{
			CString seg;
			if(start_i<end_i)		//存在区间段
			{
				seg.Format("%d-%d",start_i,end_i);
				if(m_sIncPartSegStr.GetLength()==0)
					m_sIncPartSegStr=seg;
				else
					m_sIncPartSegStr+=","+seg;
			}
			else if(start_i>=0)
			{
				seg.Format("%d",start_i);
				if(m_sIncPartSegStr.GetLength()==0)
					m_sIncPartSegStr=seg;
				else
					m_sIncPartSegStr+=","+seg;
			}
			start_i=-1;
			end_i=-1;
		}
		else if(start_i==-1)
			start_i=i;
		else
			end_i=i;
	}*/
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	char seg_str[500]="";
	//获得指定模型的段号字符串
	Ta.GetSegStr(seg_str,&module);
	m_sIncPartSegStr=seg_str;
	GetDlgItem(IDC_E_MODEL_INC_PART_SEG_STR)->SetWindowText(m_sIncPartSegStr);
#endif
}

void CModelPropDlg::OnOK()
{
	UpdateData();
	CFGLEG cfgleg=m_dwLegCfgNo.ToCfgLeg();
	cfgleg.SetBodyFlag(GetSingleWord(m_iBodyLegNo));
	m_dwLegCfgNo=cfgleg.ToCfgWord();
	CDialog::OnOK();
}

void CModelPropDlg::OnBnIntelliRetrieve()
{
	if (m_pRelaHeight != NULL)
	{
		bool specified = m_pRelaHeight->m_bSpecifyLevelheightDiff;
		m_pRelaHeight->m_bSpecifyLevelheightDiff = false;
		m_fLegLevelHeightDiff = m_pRelaHeight->uiLevelHeight*0.001;
		m_pRelaHeight->m_bSpecifyLevelheightDiff = specified;
		UpdateData(FALSE);
	}
}
