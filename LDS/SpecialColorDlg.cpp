//<LOCALE_TRANSLATE Confirm by hxr/>
// CSpecialColoeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SpecialColorDlg.h"
#include "afxdialogex.h"
#include "env_def.h"

// CSpecialColoeDlg 对话框

IMPLEMENT_DYNAMIC(CSpecialColorDlg, CDialog)

CSpecialColorDlg::CSpecialColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecialColorDlg::IDD, pParent)
{
	m_arrIsCanUse[0]=FALSE;
	m_arrIsCanUse[1]=FALSE;
	m_arrIsCanUse[2]=FALSE;
	m_arrIsCanUse[3]=FALSE;
	m_arrIsCanUse[4]=FALSE;
	m_arrValue[0]=RGB(0,0,0);
	m_arrValue[1]=RGB(0,0,0);
	m_arrValue[2]=RGB(0,0,0);
	m_arrValue[3]=RGB(0,0,0);
	m_arrValue[4]=RGB(0,0,0);
}

CSpecialColorDlg::~CSpecialColorDlg()
{
}

void CSpecialColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_MATERIAL_S, m_arrIsCanUse[0]);
	DDX_Check(pDX, IDC_CHK_MATERIAL_H, m_arrIsCanUse[1]);
	DDX_Check(pDX, IDC_CHK_MATERIAL_G, m_arrIsCanUse[2]);
	DDX_Check(pDX, IDC_CHK_MATERIAL_P, m_arrIsCanUse[3]);
	DDX_Check(pDX, IDC_CHK_MATERIAL_T, m_arrIsCanUse[4]);
	DDX_Control(pDX, IDC_CMB_COLOR_S, m_ComBoxS);
	DDX_Control(pDX, IDC_CMB_COLOR_H, m_ComBoxH);
	DDX_Control(pDX, IDC_CMB_COLOR_G, m_ComBoxG);
	DDX_Control(pDX, IDC_CMB_COLOR_P, m_ComBoxP);
	DDX_Control(pDX, IDC_CMB_COLOR_T, m_ComBoxT);
}


BEGIN_MESSAGE_MAP(CSpecialColorDlg, CDialog)
	ON_BN_CLICKED(IDC_CHK_MATERIAL_S, OnMaterialS)
	ON_BN_CLICKED(IDC_CHK_MATERIAL_H, OnMaterialH)
	ON_BN_CLICKED(IDC_CHK_MATERIAL_G, OnMaterialG)
	ON_BN_CLICKED(IDC_CHK_MATERIAL_P, OnMaterialP)
	ON_BN_CLICKED(IDC_CHK_MATERIAL_T, OnMaterialT)
	ON_CBN_SELCHANGE(IDC_CMB_COLOR_S, OnCbnSelchangeCmbColorS)
	ON_CBN_SELCHANGE(IDC_CMB_COLOR_H, OnCbnSelchangeCmbColorH)
	ON_CBN_SELCHANGE(IDC_CMB_COLOR_G, OnCbnSelchangeCmbColorG)
	ON_CBN_SELCHANGE(IDC_CMB_COLOR_P, OnCbnSelchangeCmbColorP)
	ON_CBN_SELCHANGE(IDC_CMB_COLOR_T, OnCbnSelchangeCmbColorT)
END_MESSAGE_MAP()


// CSpecialColoeDlg 消息处理程序
BOOL CSpecialColorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CColorSelectComboBox* pSelBox=NULL;
	CXhChar50 spropStr;
	COLORREF crDefalut,crValue;
	crDefalut=g_sysPara.plateColRef.GetPlateColor(m_sProp);
	for(int i=0;i<5;i++)
		m_arrValue[i]=crDefalut;
	//S
	spropStr.Printf("%s_S",m_sProp);
	crValue=g_sysPara.plateColRef.GetPlateColor(spropStr);
	if(crValue!=crDefalut)
	{
		m_arrIsCanUse[0]=TRUE;
		m_arrValue[0]=crValue;
	}
	m_ComBoxS.InitBox(m_arrValue[0]);
	m_ComBoxS.EnableWindow(m_arrIsCanUse[0]);
	//H
	spropStr.Printf("%s_H",m_sProp);
	crValue=g_sysPara.plateColRef.GetPlateColor(spropStr);
	if(crValue!=crDefalut)
	{
		m_arrIsCanUse[1]=TRUE;
		m_arrValue[1]=crValue;
	}
	m_ComBoxH.InitBox(m_arrValue[1]);
	m_ComBoxH.EnableWindow(m_arrIsCanUse[1]);
	//G
	spropStr.Printf("%s_G",m_sProp);
	crValue=g_sysPara.plateColRef.GetPlateColor(spropStr);
	if(crValue!=crDefalut)
	{
		m_arrIsCanUse[2]=TRUE;
		m_arrValue[2]=crValue;
	}
	m_ComBoxG.InitBox(m_arrValue[2]);
	m_ComBoxG.EnableWindow(m_arrIsCanUse[2]);
	//P
	spropStr.Printf("%s_P",m_sProp);
	crValue=g_sysPara.plateColRef.GetPlateColor(spropStr);
	if(crValue!=crDefalut)
	{
		m_arrIsCanUse[3]=TRUE;
		m_arrValue[3]=crValue;
	}
	m_ComBoxP.InitBox(m_arrValue[3]);
	m_ComBoxP.EnableWindow(m_arrIsCanUse[3]);
	//T
	spropStr.Printf("%s_T",m_sProp);
	crValue=g_sysPara.plateColRef.GetPlateColor(spropStr);
	if(crValue!=crDefalut)
	{
		m_arrIsCanUse[4]=TRUE;
		m_arrValue[4]=crValue;
	}
	m_ComBoxT.InitBox(m_arrValue[4]);
	m_ComBoxT.EnableWindow(m_arrIsCanUse[4]);
	UpdateData(FALSE);
	return TRUE;
}

void CSpecialColorDlg::OnMaterialS()
{
	m_arrIsCanUse[0]=!m_arrIsCanUse[0];
	m_ComBoxS.EnableWindow(m_arrIsCanUse[0]);
	if(!m_arrIsCanUse[0])
	{
		COLORREF crDefalut=g_sysPara.plateColRef.GetPlateColor(m_sProp);
		int iCur = m_ComBoxS.IsHaveThisColor(crDefalut);
		if(iCur==-1)
#ifdef AFX_TARG_ENU_ENGLISH
			iCur = m_ComBoxS.AddColor(crDefalut,"user defined",FALSE);
#else
			iCur = m_ComBoxS.AddColor(crDefalut,"自定义",FALSE);
#endif
		m_ComBoxS.SetCurSel(iCur);
	}
}
void CSpecialColorDlg::OnMaterialH()
{
	m_arrIsCanUse[1]=!m_arrIsCanUse[1];
	m_ComBoxH.EnableWindow(m_arrIsCanUse[1]);
	if(!m_arrIsCanUse[1])
	{
		COLORREF crDefalut=g_sysPara.plateColRef.GetPlateColor(m_sProp);
		int iCur = m_ComBoxH.IsHaveThisColor(crDefalut);
		if(iCur==-1)
#ifdef AFX_TARG_ENU_ENGLISH
			iCur = m_ComBoxH.AddColor(crDefalut,"user defined",FALSE);
#else
			iCur = m_ComBoxH.AddColor(crDefalut,"自定义",FALSE);
#endif
		m_ComBoxH.SetCurSel(iCur);
	}
}
void CSpecialColorDlg::OnMaterialG()
{
	m_arrIsCanUse[2]=!m_arrIsCanUse[2];
	m_ComBoxG.EnableWindow(m_arrIsCanUse[2]);
	if(!m_arrIsCanUse[2])
	{
		COLORREF crDefalut=g_sysPara.plateColRef.GetPlateColor(m_sProp);
		int iCur = m_ComBoxG.IsHaveThisColor(crDefalut);
		if(iCur==-1)
#ifdef AFX_TARG_ENU_ENGLISH
			iCur = m_ComBoxG.AddColor(crDefalut,"user defined",FALSE);
#else
			iCur = m_ComBoxG.AddColor(crDefalut,"自定义",FALSE);
#endif
		m_ComBoxG.SetCurSel(iCur);
	}
}
void CSpecialColorDlg::OnMaterialP()
{
	m_arrIsCanUse[3]=!m_arrIsCanUse[3];
	m_ComBoxP.EnableWindow(m_arrIsCanUse[3]);
	if(!m_arrIsCanUse[3])
	{
		COLORREF crDefalut=g_sysPara.plateColRef.GetPlateColor(m_sProp);
		int iCur = m_ComBoxP.IsHaveThisColor(crDefalut);
		if(iCur==-1)
#ifdef AFX_TARG_ENU_ENGLISH
			iCur = m_ComBoxP.AddColor(crDefalut,"user defined",FALSE);
#else
			iCur = m_ComBoxP.AddColor(crDefalut,"自定义",FALSE);
#endif
		m_ComBoxP.SetCurSel(iCur);
	}
}
void CSpecialColorDlg::OnMaterialT()
{
	m_arrIsCanUse[4]=!m_arrIsCanUse[4];
	m_ComBoxT.EnableWindow(m_arrIsCanUse[4]);
	if(!m_arrIsCanUse[4])
	{
		COLORREF crDefalut=g_sysPara.plateColRef.GetPlateColor(m_sProp);
		int iCur = m_ComBoxT.IsHaveThisColor(crDefalut);
		if(iCur==-1)
#ifdef AFX_TARG_ENU_ENGLISH
			iCur = m_ComBoxT.AddColor(crDefalut,"user defined",FALSE);
#else
			iCur = m_ComBoxT.AddColor(crDefalut,"自定义",FALSE);
#endif
		m_ComBoxT.SetCurSel(iCur);
	}
}
void CSpecialColorDlg::OnCbnSelchangeCmbColorS()
{
	if(!m_arrIsCanUse[0])
		return;
	COLORREF color = m_ComBoxS.GetItemData(m_ComBoxS.GetCurSel());
	if(color==0XFFFFFFFF)
	{	
		CColorDialog dlg(color);
		if(dlg.DoModal()==IDOK)
		{
			color = dlg.GetColor();
			int iCur = m_ComBoxS.IsHaveThisColor(color);
			if(iCur==-1)
#ifdef AFX_TARG_ENU_ENGLISH
				iCur = m_ComBoxS.AddColor(color,"user defined",FALSE);
#else
				iCur = m_ComBoxS.AddColor(color,"自定义",FALSE);
#endif
			m_ComBoxS.SetCurSel(iCur);
		}
	}
	CString itemString;
	int index=m_ComBoxS.GetCurSel();
	m_ComBoxS.GetLBText(index,itemString );
	m_arrValue[0]=color;
}
void CSpecialColorDlg::OnCbnSelchangeCmbColorH()
{
	if(!m_arrIsCanUse[1])
		return;
	COLORREF color = m_ComBoxH.GetItemData(m_ComBoxH.GetCurSel());
	if(color==0XFFFFFFFF)
	{	
		CColorDialog dlg(color);
		if(dlg.DoModal()==IDOK)
		{
			color = dlg.GetColor();
			int iCur = m_ComBoxH.IsHaveThisColor(color);
			if(iCur==-1)
#ifdef AFX_TARG_ENU_ENGLISH
				iCur = m_ComBoxH.AddColor(color,"user defined",FALSE);
#else
				iCur = m_ComBoxH.AddColor(color,"自定义",FALSE);
#endif
			m_ComBoxH.SetCurSel(iCur);
		}
	}
	m_arrValue[1]=color;
}
void CSpecialColorDlg::OnCbnSelchangeCmbColorG()
{
	if(!m_arrIsCanUse[2])
		return;
	COLORREF color = m_ComBoxG.GetItemData(m_ComBoxG.GetCurSel());
	if(color==0XFFFFFFFF)
	{	
		CColorDialog dlg(color);
		if(dlg.DoModal()==IDOK)
		{
			color = dlg.GetColor();
			int iCur = m_ComBoxG.IsHaveThisColor(color);
			if(iCur==-1)
#ifdef AFX_TARG_ENU_ENGLISH
				iCur = m_ComBoxG.AddColor(color,"user defined",FALSE);
#else
				iCur = m_ComBoxG.AddColor(color,"自定义",FALSE);
#endif
			m_ComBoxG.SetCurSel(iCur);
		}
	}
	m_arrValue[2]=color;
}
void CSpecialColorDlg::OnCbnSelchangeCmbColorP()
{
	if(!m_arrIsCanUse[3])
		return;
	COLORREF color = m_ComBoxP.GetItemData(m_ComBoxP.GetCurSel());
	if(color==0XFFFFFFFF)
	{	
		CColorDialog dlg(color);
		if(dlg.DoModal()==IDOK)
		{
			color = dlg.GetColor();
			int iCur = m_ComBoxP.IsHaveThisColor(color);
			if(iCur==-1)
#ifdef AFX_TARG_ENU_ENGLISH
				iCur = m_ComBoxP.AddColor(color,"user defined",FALSE);
#else
				iCur = m_ComBoxP.AddColor(color,"自定义",FALSE);
#endif
			m_ComBoxP.SetCurSel(iCur);
		}
	}
	m_arrValue[3]=color;
}
void CSpecialColorDlg::OnCbnSelchangeCmbColorT()
{
	if(!m_arrIsCanUse[4])
		return;
	COLORREF color = m_ComBoxT.GetItemData(m_ComBoxT.GetCurSel());
	if(color==0XFFFFFFFF)
	{	
		CColorDialog dlg(color);
		if(dlg.DoModal()==IDOK)
		{
			color = dlg.GetColor();
			int iCur = m_ComBoxT.IsHaveThisColor(color);
			if(iCur==-1)
#ifdef AFX_TARG_ENU_ENGLISH
				iCur = m_ComBoxT.AddColor(color,"user defined",FALSE);
#else
				iCur = m_ComBoxT.AddColor(color,"自定义",FALSE);
#endif
			m_ComBoxT.SetCurSel(iCur);
		}
	}
	m_arrValue[4]=color;
}
