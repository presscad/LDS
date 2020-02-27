// WorkStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "WorkStatusDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if defined(__LDS_)||defined(__TSA_)
/////////////////////////////////////////////////////////////////////////////
// CWorkStatusDlg dialog

static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	CWorkStatusDlg *pDlg=(CWorkStatusDlg*)pListCtrl->GetParent();
	double coef=atof(sTextValue);
	if(pSelItem->m_idProp==1)		//线条荷载
	{
		if(iSubItem==1)
			pDlg->m_pWorkStatus->SetKesai(coef);
		else if(iSubItem==2)
			pDlg->m_pWorkStatus->SetGamaQ(coef);
	}
	else if(pSelItem->m_idProp==2)	//风荷载
	{
		if(iSubItem==1)
			pDlg->m_pWorkStatus->SetKesai_W(coef);
		else if(iSubItem==2)
			pDlg->m_pWorkStatus->SetGamaQ_W(coef);
	}
	else if(pSelItem->m_idProp==3)	//覆冰荷载
	{
		if(iSubItem==1)
			pDlg->m_pWorkStatus->SetKesai_I(coef);
		else if(iSubItem==2)
			pDlg->m_pWorkStatus->SetGamaQ_I(coef);
	}
	else if(pSelItem->m_idProp==4)	//温度荷载
	{
		if(iSubItem==1)
			pDlg->m_pWorkStatus->SetKesai_T(coef);
		else if(iSubItem==2)
			pDlg->m_pWorkStatus->SetGamaQ_T(coef);
	}
	else if(pSelItem->m_idProp==5)	//断线荷载
	{
		if(iSubItem==1)
			pDlg->m_pWorkStatus->SetKesai_U(coef);
		else if(iSubItem==2)
			pDlg->m_pWorkStatus->SetGamaQ_U(coef);
	}
	else if(pSelItem->m_idProp==6)	//安装荷载
	{
		if(iSubItem==1)
			pDlg->m_pWorkStatus->SetKesai_A(coef);
		else if(iSubItem==2)
			pDlg->m_pWorkStatus->SetGamaQ_A(coef);
	}
	else if(pSelItem->m_idProp==7)	//水平向地震荷载
	{
		if(iSubItem==1)
			;//pDlg->m_pWorkStatus->SetKesai_W(coef);
		else if(iSubItem==2)
			pDlg->m_pWorkStatus->SetGamaQ_Eh(coef);
	}
	else if(pSelItem->m_idProp==8)	//竖向地震荷载
	{
		if(iSubItem==1)
			;//pDlg->m_pWorkStatus->SetKesai_W(coef);
		else if(iSubItem==2)
			pDlg->m_pWorkStatus->SetGamaQ_Ev(coef);
	}
	return TRUE;
}

CWorkStatusDlg::CWorkStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkStatusDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWorkStatusDlg)
	m_sModuleScope = _T("");
	m_nWindAzimuth = 0;
	m_sStatusName = _T("");
	m_fIceThick = 0.0;
	m_fWindVelocity = 0.0;
	m_fWindLoadGrowCoefByIce = 1.0;
	m_iMainLoadType = 0;
	m_bCheckWorkCond = FALSE;
	m_sVibrationModeStr = _T("1-3");
	m_cHoriEqAxisAzimuth = 90;
	//}}AFX_DATA_INIT
	m_pWorkStatus=NULL;
	m_listCtrlLoadCoef.AddColumnHeader("荷载类型");
	m_listCtrlLoadCoef.AddColumnHeader("组合系数");
	m_listCtrlLoadCoef.AddColumnHeader("分项系数");
}


void CWorkStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkStatusDlg)
	DDX_Control(pDX, IDC_LIST_LOAD_DESIGN_COEF, m_listCtrlLoadCoef);
	DDX_Text(pDX, IDC_E_MODULE_SCOPE, m_sModuleScope);
	DDV_MaxChars(pDX, m_sModuleScope, 99);
	DDX_Text(pDX, IDC_E_WIND_AZIMUTH, m_nWindAzimuth);
	DDX_Text(pDX, IDC_E_STATUS_NAME, m_sStatusName);
	DDV_MaxChars(pDX, m_sStatusName, 99);
	DDX_Text(pDX, IDC_E_ICE_THICK, m_fIceThick);
	DDX_Text(pDX, IDC_E_WIND_VECLOCITY, m_fWindVelocity);
	DDX_Text(pDX, IDC_E_WINDLOAD_COEF_GROW_BY_ICE, m_fWindLoadGrowCoefByIce);
	DDX_CBIndex(pDX, IDC_CMB_MAIN_LOAD_STYLE, m_iMainLoadType);
	DDX_Check(pDX, IDC_CHK_CHECK_WORK_COND, m_bCheckWorkCond);
	DDX_Text(pDX, IDC_E_VIBRATION_MODE_STR, m_sVibrationModeStr);
	DDV_MaxChars(pDX, m_sVibrationModeStr, 50);
	DDX_Text(pDX, IDC_E_HORI_EQ_AXIS_AZIMUTH, m_cHoriEqAxisAzimuth);
	DDV_MinMaxByte(pDX, m_cHoriEqAxisAzimuth, 0, 180);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorkStatusDlg, CDialog)
	//{{AFX_MSG_MAP(CWorkStatusDlg)
	ON_CBN_SELCHANGE(IDC_CMB_MAIN_LOAD_STYLE, OnSelchangeCmbMainLoadStyle)
	ON_BN_CLICKED(IDC_CHK_CHECK_WORK_COND, OnChkCheckWorkCond)
	ON_EN_CHANGE(IDC_E_ICE_THICK, OnChangeEIceThick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkStatusDlg message handlers

BOOL CWorkStatusDlg::OnInitDialog() 
{
	if(m_pWorkStatus==NULL)
		return FALSE;
	CDialog::OnInitDialog();
	long col_wide_arr[]={100,80,80};
	m_listCtrlLoadCoef.InitListCtrl(col_wide_arr);
	m_listCtrlLoadCoef.SetImageWidth();

	m_bCheckWorkCond=m_pWorkStatus->m_bCheckWorkCond;
	m_iMainLoadType=m_pWorkStatus->m_cMainLoadType;
	m_fWindLoadGrowCoefByIce=m_pWorkStatus->Bw_I();
	GetDlgItem(IDC_CMB_MAIN_LOAD_STYLE)->EnableWindow(Ta.m_iLoadExecuteCode!=3);
	GetDlgItem(IDC_CHK_CHECK_WORK_COND)->EnableWindow(m_iMainLoadType>2);
	GetDlgItem(IDC_E_VIBRATION_MODE_STR)->EnableWindow(m_iMainLoadType>=5);
	GetDlgItem(IDC_E_HORI_EQ_AXIS_AZIMUTH)->EnableWindow(m_iMainLoadType>=5);
	m_listCtrlLoadCoef.SetModifyValueFunc(FireValueModify);
	if(Ta.m_iLoadExecuteCode==3)
		((CEdit*)GetDlgItem(IDC_E_WINDLOAD_COEF_GROW_BY_ICE))->SetReadOnly(FALSE);
	else
		((CEdit*)GetDlgItem(IDC_E_WINDLOAD_COEF_GROW_BY_ICE))->SetReadOnly();
	m_cHoriEqAxisAzimuth=m_pWorkStatus->m_cHoriEarthQuakeAxis;
	m_sVibrationModeStr=m_pWorkStatus->vibration_mode_str;
	UpdateData(FALSE);
	InitListCtrlItems();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWorkStatusDlg::OnOK() 
{
	CDialog::OnOK();
	sprintf(m_pWorkStatus->description,"%s",m_sStatusName);
	sprintf(m_pWorkStatus->limit_str,"%s",m_sModuleScope);
	m_pWorkStatus->m_bCheckWorkCond=m_bCheckWorkCond!=0;
	m_pWorkStatus->m_cMainLoadType=(BYTE)m_iMainLoadType;
	m_pWorkStatus->WindLoad.azimuth=m_nWindAzimuth;
	m_pWorkStatus->WindLoad.velocity=m_fWindVelocity;
	m_pWorkStatus->ice_thick=m_fIceThick;
	m_pWorkStatus->SetBw_I(m_fWindLoadGrowCoefByIce);
	if(m_iMainLoadType>=5)
	{
		m_pWorkStatus->m_cHoriEarthQuakeAxis=m_cHoriEqAxisAzimuth;
		_snprintf(m_pWorkStatus->vibration_mode_str,50,"%s",m_sVibrationModeStr);
	}
	if(m_sStatusName.IsEmpty())
		sprintf(m_pWorkStatus->description,"%d",m_pWorkStatus->iNo);
}

void CWorkStatusDlg::OnSelchangeCmbMainLoadStyle() 
{
	UpdateData();
	m_pWorkStatus->SetGamaQ(1.4);
	m_pWorkStatus->SetGamaQ_A(1.4);
	m_pWorkStatus->SetGamaQ_I(1.4);
	m_pWorkStatus->SetGamaQ_W(1.4);
	m_pWorkStatus->SetGamaQ_U(1.4);
	m_pWorkStatus->SetGamaQ_T(1.4);
	GetDlgItem(IDC_CHK_CHECK_WORK_COND)->EnableWindow(m_iMainLoadType>2);
	GetDlgItem(IDC_E_VIBRATION_MODE_STR)->EnableWindow(m_iMainLoadType>=5);
	GetDlgItem(IDC_E_HORI_EQ_AXIS_AZIMUTH)->EnableWindow(m_iMainLoadType>=5);
	if(m_iMainLoadType==0)		//大风
	{
		m_pWorkStatus->SetKesai(1.0);
		m_pWorkStatus->SetKesai_W(1.0);
		m_pWorkStatus->SetKesai_I(1.0);
		m_bCheckWorkCond=FALSE;
	}
	else if(m_iMainLoadType==1)	//覆冰
	{
		m_pWorkStatus->SetKesai(1.0);
		m_pWorkStatus->SetKesai_W(1.0);
		m_pWorkStatus->SetKesai_I(1.0);
		m_pWorkStatus->SetKesai_T(1.0);
		m_bCheckWorkCond=FALSE;
	}
	else if(m_iMainLoadType==2)	//低温
	{
		m_pWorkStatus->SetKesai(1.0);
		m_pWorkStatus->SetKesai_W(1.0);
		m_pWorkStatus->SetKesai_I(1.0);
		m_pWorkStatus->SetKesai_T(1.0);
		m_bCheckWorkCond=FALSE;
	}
	else if(m_iMainLoadType==3)	//断线及不均匀覆冰
	{
		m_pWorkStatus->SetKesai(0.9);
		m_pWorkStatus->SetKesai_W(0.9);
		m_pWorkStatus->SetKesai_U(0.9);
		m_pWorkStatus->SetKesai_I(0.9);
		m_pWorkStatus->SetKesai_T(0.9);
	}
	else if(m_iMainLoadType==4)	//安装
	{
		m_pWorkStatus->SetKesai(0.9);
		m_pWorkStatus->SetKesai_W(0.9);
		m_pWorkStatus->SetKesai_A(0.9);
		m_pWorkStatus->SetKesai_U(0.9);
		m_pWorkStatus->SetKesai_I(0.9);
		m_pWorkStatus->SetKesai_T(0.9);
	}
	else if(m_iMainLoadType==5)	//水平地震
	{
		m_pWorkStatus->SetGamaQ_Eh(1.3);
		m_pWorkStatus->SetGamaQ_Ev(0.5);
		m_pWorkStatus->SetKesai(1.0);	//导地线荷载组合系数
		m_pWorkStatus->SetGamaQ(0.5);	//导地线荷载分项系数
		m_pWorkStatus->SetKesai_W(0.3);	//风荷载组合系数
		m_pWorkStatus->SetGamaQ_W(1.0);	//风荷载分项系数
	}
	else if(m_iMainLoadType==6)	//竖向地震
	{
		m_pWorkStatus->SetGamaQ_Eh(0.5);
		m_pWorkStatus->SetGamaQ_Ev(1.3);
		m_pWorkStatus->SetKesai(1.0);	//导地线荷载组合系数
		m_pWorkStatus->SetGamaQ(0.5);	//导地线荷载分项系数
		m_pWorkStatus->SetKesai_W(0.3);	//风荷载组合系数
		m_pWorkStatus->SetGamaQ_W(1.0);	//风荷载分项系数
	}
	else if(m_iMainLoadType==7)	//仅水平地震
	{
		m_pWorkStatus->SetGamaQ_Eh(1.3);
		m_pWorkStatus->SetGamaQ_Ev(0.0);
		m_pWorkStatus->SetKesai(1.0);	//导地线荷载组合系数
		m_pWorkStatus->SetGamaQ(0.5);	//导地线荷载分项系数
		m_pWorkStatus->SetKesai_W(0.3);	//风荷载组合系数
		m_pWorkStatus->SetGamaQ_W(1.0);	//风荷载分项系数
	}
	else if(m_iMainLoadType==8)	//仅竖向地震
	{
		m_pWorkStatus->SetGamaQ_Eh(0.0);
		m_pWorkStatus->SetGamaQ_Ev(1.3);
		m_pWorkStatus->SetKesai(1.0);	//导地线荷载组合系数
		m_pWorkStatus->SetGamaQ(0.5);	//导地线荷载分项系数
		m_pWorkStatus->SetKesai_W(0.3);	//风荷载组合系数
		m_pWorkStatus->SetGamaQ_W(1.0);	//风荷载分项系数
	}
	if(m_iMainLoadType<5&&m_bCheckWorkCond)
	{
		m_pWorkStatus->SetKesai(0.75);
		m_pWorkStatus->SetKesai_W(0.75);
		m_pWorkStatus->SetKesai_U(0.75);
		m_pWorkStatus->SetKesai_I(0.75);
		m_pWorkStatus->SetKesai_T(0.75);
	}
	InitListCtrlItems();
}

void CWorkStatusDlg::OnChkCheckWorkCond() 
{
	UpdateData();
	if(m_bCheckWorkCond&&m_iMainLoadType<5)	//验算工况
	{
		m_pWorkStatus->SetKesai(0.75);
		m_pWorkStatus->SetKesai_W(0.75);
		m_pWorkStatus->SetKesai_U(0.75);
		m_pWorkStatus->SetKesai_I(0.75);
		m_pWorkStatus->SetKesai_T(0.75);
	}
	InitListCtrlItems();
}

void CWorkStatusDlg::OnChangeEIceThick() 
{
	double oldThick=m_fIceThick;
	UpdateData();
	if(oldThick==m_fIceThick)
		return;
	CWorkStatus status;
	status.ice_thick=m_fIceThick;
	m_fWindLoadGrowCoefByIce=status.Bw_I();
	UpdateData(FALSE);
}

void CWorkStatusDlg::InitListCtrlItems()
{
	BYTE cOldMainLoadType=m_pWorkStatus->m_cMainLoadType;
	bool bCheckWorkCond = m_pWorkStatus->m_bCheckWorkCond;
	m_pWorkStatus->m_cMainLoadType=m_iMainLoadType;
	m_pWorkStatus->m_bCheckWorkCond=m_bCheckWorkCond!=0;
	m_listCtrlLoadCoef.DeleteAllItems();
	CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("线条荷载"),TRUE);
	CString sText;
	sText.Format("%f",m_pWorkStatus->Kesai());
	SimplifiedNumString(sText);
	lpInfo->AddSubItemText(sText);
	if(m_iMainLoadType>=5||Ta.m_iLoadExecuteCode!=3)	//抗震验算工况
		lpInfo->SetSubItemReadOnly(1,TRUE);
	sText.Format("%f",m_pWorkStatus->GamaQ());
	SimplifiedNumString(sText);
	lpInfo->AddSubItemText(sText);
	if(Ta.m_iLoadExecuteCode!=3)	//非用户指定
		lpInfo->SetSubItemReadOnly(2,TRUE);
	CSuperGridCtrl::CTreeItem* pItem = m_listCtrlLoadCoef.InsertRootItem(lpInfo);
	pItem->m_idProp=1;

	lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("杆塔风荷载"),TRUE);
	sText.Format("%f",m_pWorkStatus->Kesai_W());
	SimplifiedNumString(sText);
	lpInfo->AddSubItemText(sText);
	if(m_iMainLoadType>=5||Ta.m_iLoadExecuteCode!=3)	//抗震验算工况
		lpInfo->SetSubItemReadOnly(1,TRUE);
	sText.Format("%f",m_pWorkStatus->GamaQ_W());
	SimplifiedNumString(sText);
	lpInfo->AddSubItemText(sText);
	if(Ta.m_iLoadExecuteCode!=3)	//非用户指定
		lpInfo->SetSubItemReadOnly(2,TRUE);
	pItem = m_listCtrlLoadCoef.InsertRootItem(lpInfo);
	pItem->m_idProp=2;
	if(m_iMainLoadType==1||m_iMainLoadType==3)	//覆冰工况及断线工况
	{
		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,_T("杆塔覆冰荷载"),TRUE);
		sText.Format("%f",m_pWorkStatus->Kesai_I());
		SimplifiedNumString(sText);
		lpInfo->AddSubItemText(sText);
		if(Ta.m_iLoadExecuteCode!=3)	//非用户指定
			lpInfo->SetSubItemReadOnly(1,TRUE);
		sText.Format("%f",m_pWorkStatus->GamaQ_I());
		SimplifiedNumString(sText);
		lpInfo->AddSubItemText(sText);
		if(Ta.m_iLoadExecuteCode!=3)	//非用户指定
			lpInfo->SetSubItemReadOnly(2,TRUE);
		pItem = m_listCtrlLoadCoef.InsertRootItem(lpInfo);
		pItem->m_idProp=3;
	}
	if(m_iMainLoadType>=5)	//抗震验算工况
	{
		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,_T("水平向地震荷载"),TRUE);
		sText.Format("%f",m_pWorkStatus->Kesai());
		SimplifiedNumString(sText);
		lpInfo->AddSubItemText(sText);
		if(Ta.m_iLoadExecuteCode!=3)	//非用户指定
			lpInfo->SetSubItemReadOnly(1,TRUE);
		sText.Format("%f",m_pWorkStatus->GamaQ_Eh());
		SimplifiedNumString(sText);
		lpInfo->AddSubItemText(sText);
		if(Ta.m_iLoadExecuteCode!=3)	//非用户指定
			lpInfo->SetSubItemReadOnly(2,TRUE);
		pItem = m_listCtrlLoadCoef.InsertRootItem(lpInfo);
		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,_T("竖向地震荷载"),TRUE);
		sText.Format("%f",m_pWorkStatus->Kesai());
		SimplifiedNumString(sText);
		lpInfo->AddSubItemText(sText);
		if(Ta.m_iLoadExecuteCode!=3)	//非用户指定
			lpInfo->SetSubItemReadOnly(1,TRUE);
		sText.Format("%f",m_pWorkStatus->GamaQ_Ev());
		SimplifiedNumString(sText);
		lpInfo->AddSubItemText(sText);
		if(Ta.m_iLoadExecuteCode!=3)	//非用户指定
			lpInfo->SetSubItemReadOnly(2,TRUE);
		pItem = m_listCtrlLoadCoef.InsertRootItem(lpInfo);
		pItem->m_idProp=7;
	}
	m_pWorkStatus->m_cMainLoadType=cOldMainLoadType;
	m_pWorkStatus->m_bCheckWorkCond=bCheckWorkCond;
	m_listCtrlLoadCoef.Redraw();
	/*if(m_iMainLoadType<5)	//非地震工况
	{
		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,_T("温度荷载"),TRUE);
		sText.Format("%f",m_pWorkStatus->Kesai_T());
		SimplifiedNumString(sText);
		lpInfo->AddSubItemText(sText);
		sText.Format("%f",m_pWorkStatus->GamaQ_T());
		SimplifiedNumString(sText);
		lpInfo->AddSubItemText(sText);
		pItem = m_listCtrlLoadCoef.InsertRootItem(lpInfo);
		pItem->m_idProp=4;
	}
	if(m_iMainLoadType==3)
	{
		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,_T("断线荷载"),TRUE);
		sText.Format("%f",m_pWorkStatus->Kesai_U());
		SimplifiedNumString(sText);
		lpInfo->AddSubItemText(sText);
		sText.Format("%f",m_pWorkStatus->GamaQ_U());
		SimplifiedNumString(sText);
		lpInfo->AddSubItemText(sText);
		pItem = m_listCtrlLoadCoef.InsertRootItem(lpInfo);
		pItem->m_idProp=5;
	}
	if(m_iMainLoadType==4)
	{
		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,_T("安装荷载"),TRUE);
		sText.Format("%f",m_pWorkStatus->Kesai_A());
		SimplifiedNumString(sText);
		lpInfo->AddSubItemText(sText);
		sText.Format("%f",m_pWorkStatus->GamaQ_A());
		SimplifiedNumString(sText);
		lpInfo->AddSubItemText(sText);
		pItem = m_listCtrlLoadCoef.InsertRootItem(lpInfo);
		pItem->m_idProp=6;
	}*/
}
#endif
