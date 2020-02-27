// SelectWindLoadDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "LDS.h"
#include "SelectWindLoadDlg.h"
#include "afxdialogex.h"
#include "PropertyListOper.h"
#include "ReportPointDlg.h"

//回调函数
static BOOL WindLoadPropertyModifyValueFunc(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CSelectWindLoadDlg* pDlg=(CSelectWindLoadDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	PROPLIST_ITEM* pPropItem=CWindSegment::GetPropItem(pItem->m_idProp);
	pDlg->m_checkList.SetValue(pPropItem->propKeyStr,TRUE);
	//
	CWindSegment* pFirWindSegment=pDlg->m_selectList.GetFirst();
	CPropertyListOper<CWindSegment> oper(pPropList,pFirWindSegment,&(pDlg->m_selectList));
	CXhChar100 tem_str;
	tem_str.Printf("%s",valueStr);
	if(pItem->m_idProp==CWindSegment::GetPropID("type"))
	{
		pDlg->m_xDefaultWindSeg.cType=tem_str[0]-'0';
		if(pDlg->m_xDefaultWindSeg.cType==1)
		{
			pPropList->SetItemReadOnly(CWindSegment::GetPropID("cMirType"),FALSE);
			pPropList->SetItemReadOnly(CWindSegment::GetPropID("cCalStyle"),FALSE);
			if(pDlg->m_xDefaultWindSeg.cCalStyle==0)
			{
				pDlg->m_xDefaultWindSeg.cCalStyle=1;
				pPropList->SetItemPropValue(CWindSegment::GetPropID("cCalStyle"),"X向横担");
			}
		}
		else
		{
			pDlg->m_xDefaultWindSeg.cMirType=0;
			pPropList->SetItemReadOnly(CWindSegment::GetPropID("cMirType"),TRUE);
			pPropList->SetItemPropValue(CWindSegment::GetPropID("cMirType"),"无对称");
			pDlg->m_xDefaultWindSeg.cCalStyle=0;
			pPropList->SetItemReadOnly(CWindSegment::GetPropID("cCalStyle"),TRUE);
			pPropList->SetItemPropValue(CWindSegment::GetPropID("cCalStyle"),"塔身");
		}
		if(pDlg->m_xDefaultWindSeg.cType!=0)
		{
			pDlg->m_xDefaultWindSeg.K=1;
			pPropList->SetItemReadOnly(CWindSegment::GetPropID("K"),TRUE);
			pPropList->SetItemPropValue(CWindSegment::GetPropID("K"),"1");
		}
		else
			pPropList->SetItemReadOnly(CWindSegment::GetPropID("K"),FALSE);
	}
	else if(pItem->m_idProp==CWindSegment::GetPropID("cMirType"))
	{
		if(valueStr.Compare("无对称")==0)
			pDlg->m_xDefaultWindSeg.cMirType=0;
		else if(valueStr.Compare("X轴对称")==0)
			pDlg->m_xDefaultWindSeg.cMirType=1;
		else if(valueStr.Compare("Y轴对称")==0)
			pDlg->m_xDefaultWindSeg.cMirType=2;
	}
	else if(pItem->m_idProp==CWindSegment::GetPropID("cCalStyle"))
	{
		if(valueStr.Compare("塔身")==0)
			pDlg->m_xDefaultWindSeg.cCalStyle=0;
		else if(valueStr.Compare("X向横担")==0)
			pDlg->m_xDefaultWindSeg.cCalStyle=1;
		else if(valueStr.Compare("Y向横担")==0)
			pDlg->m_xDefaultWindSeg.cCalStyle=2;
	}
	else if(pItem->m_idProp==CWindSegment::GetPropID("BetaZ"))
		pDlg->m_xDefaultWindSeg.BetaZ=atof(tem_str);
	else if(pItem->m_idProp==CWindSegment::GetPropID("BetaZForFoundation"))
		pDlg->m_xDefaultWindSeg.BetaZForFoundation=atof(tem_str);
	else if(pItem->m_idProp==CWindSegment::GetPropID("CqiFront"))
		pDlg->m_xDefaultWindSeg.CqiFront=atof(tem_str);
	else if(pItem->m_idProp==CWindSegment::GetPropID("CqiSide"))
		pDlg->m_xDefaultWindSeg.CqiSide=atof(tem_str);
	else if(pItem->m_idProp==CWindSegment::GetPropID("m_fProfileModeCoef"))
		pDlg->m_xDefaultWindSeg.m_fProfileModeCoef=atof(tem_str);
	else if(pItem->m_idProp==CWindSegment::GetPropID("m_bAutoCalProfileCoef"))
	{
		if(valueStr.Compare("自动计算")==0)
			pDlg->m_xDefaultWindSeg.m_bAutoCalProfileCoef=TRUE;
		else if(valueStr.Compare("手动指定")==0)
			pDlg->m_xDefaultWindSeg.m_bAutoCalProfileCoef=FALSE;
		//
		pPropList->SetItemReadOnly(CWindSegment::GetPropID("m_fProfileModeCoef"),pDlg->m_xDefaultWindSeg.m_bAutoCalProfileCoef);
		pPropList->SetItemReadOnly(CWindSegment::GetPropID("m_bSpecB_A"),!pDlg->m_xDefaultWindSeg.m_bAutoCalProfileCoef);
		pPropList->SetItemReadOnly(CWindSegment::GetPropID("B_AFront"),!pDlg->m_xDefaultWindSeg.m_bAutoCalProfileCoef||!pDlg->m_xDefaultWindSeg.m_bSpecB_A);
	}
	else if(pItem->m_idProp==CWindSegment::GetPropID("B_AFront"))
		pDlg->m_xDefaultWindSeg.B_AFront=atof(valueStr);
	else if(pItem->m_idProp==CWindSegment::GetPropID("m_bSpecB_A"))
	{	
		if(valueStr.Compare("手动指定")==0)
			pDlg->m_xDefaultWindSeg.m_bSpecB_A=TRUE;
		else if(valueStr.Compare("自动计算")==0)
			pDlg->m_xDefaultWindSeg.m_bSpecB_A=FALSE;
		//
		pPropList->SetItemReadOnly(CWindSegment::GetPropID("B_AFront"),!pDlg->m_xDefaultWindSeg.m_bSpecB_A);
	}
	else if(pItem->m_idProp==CWindSegment::GetPropID("K"))
		pDlg->m_xDefaultWindSeg.K=atof(valueStr);
	return TRUE;
}
static BOOL WindLoadPropertyModifyBtnFunc(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CSelectWindLoadDlg* pDlg=(CSelectWindLoadDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	PROPLIST_ITEM* pPropItem=CWindSegment::GetPropItem(pItem->m_idProp);
	pDlg->m_checkList.SetValue(pPropItem->propKeyStr,TRUE);
	//
	CWindSegment* pFirWindSegment=pDlg->m_selectList.GetFirst();
	if(pItem->m_idProp==CWindSegment::GetPropID("basis_points"))
	{
		CReportPointDlg dlg;
		if(dlg.DoModal()==IDOK)
		{
			pDlg->m_xDefaultWindSeg.xProjPlaneOrg.x=dlg.m_fPointX;
			pDlg->m_xDefaultWindSeg.xProjPlaneOrg.y=dlg.m_fPointY;
			pDlg->m_xDefaultWindSeg.xProjPlaneOrg.z=dlg.m_fPointZ;
		}
	}
	return TRUE;
}
// CSelectWindLoadDlg 对话框
IMPLEMENT_DYNAMIC(CSelectWindLoadDlg, CDialogEx)
CSelectWindLoadDlg::CSelectWindLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelectWindLoadDlg::IDD, pParent)
{	
	m_checkList.Empty();
}

CSelectWindLoadDlg::~CSelectWindLoadDlg()
{
}

void CSelectWindLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WIND_LOAD_LIST, m_propList);
}
BEGIN_MESSAGE_MAP(CSelectWindLoadDlg, CDialogEx)
END_MESSAGE_MAP()
// CSelectWindLoadDlg 消息处理程序
BOOL CSelectWindLoadDlg::OnInitDialog()
{
	if(m_selectList.GetNodeNum()<=0)
		return FALSE;
	CDialog::OnInitDialog();
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.5);
	m_propList.SetModifyValueFunc(WindLoadPropertyModifyValueFunc);
	m_propList.SetButtonClickFunc(WindLoadPropertyModifyBtnFunc);
	//显示风压段属性
	CWindSegment::InitPropHashtable();
	DisplayWindLoadProperty();
	return TRUE;
}
void CSelectWindLoadDlg::DisplayWindLoadProperty()
{
	m_propList.CleanTree();
	CWindSegment* pFirWindSegment=m_selectList.GetFirst();
	CPropertyListOper<CWindSegment> oper(&m_propList,pFirWindSegment,&m_selectList);
	CPropTreeItem *pRoot=m_propList.GetRootItem(),*pPropItem,*pSonItem=NULL;
	CPropTreeItem *pGroupItem=oper.InsertPropItem(pRoot,"attribute");	//风压段属性
	pGroupItem->m_bHideChildren=FALSE;
	oper.InsertCmbListPropItem(pGroupItem,"type");					//风压段类型
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"cMirType");	//对称方式
	pPropItem->SetReadOnly(pFirWindSegment->cType!=1);
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"cCalStyle");	//计算方式
	pPropItem->SetReadOnly(pFirWindSegment->cType!=1);
	pPropItem=oper.InsertEditPropItem(pGroupItem,"K");	//塔身断面形状系数
	pPropItem->SetReadOnly(pFirWindSegment->cType!=0);
	oper.InsertEditPropItem(pGroupItem,"BetaZ");					//风振系数(杆塔)
	oper.InsertEditPropItem(pGroupItem,"BetaZForFoundation");		//风振系数(基础)
	oper.InsertEditPropItem(pGroupItem,"CqiFront");					//正面效应系数
	oper.InsertEditPropItem(pGroupItem,"CqiSide");					//侧面效应系数
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bAutoCalProfileCoef");		//是否自动计算
	pPropItem->m_bHideChildren=FALSE;
	pSonItem=oper.InsertEditPropItem(pPropItem,"m_fProfileModeCoef");	//塔架体型系数
	pSonItem->SetReadOnly(pFirWindSegment->m_bAutoCalProfileCoef);
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bSpecB_A");	//是否自动计算
	pPropItem->SetReadOnly(!pFirWindSegment->m_bAutoCalProfileCoef);
	pPropItem->m_bHideChildren=FALSE;
	pSonItem=oper.InsertEditPropItem(pPropItem,"B_AFront");		//正面深度/宽度
	pSonItem->SetReadOnly(!pFirWindSegment->m_bAutoCalProfileCoef||!pFirWindSegment->m_bSpecB_A);
	pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"basis_points");	//投影面基点
	m_propList.Redraw();
}
void CSelectWindLoadDlg::OnOK()
{
	for(CWindSegment* m_pSelectStatus=m_selectList.GetFirst();m_pSelectStatus;m_pSelectStatus=m_selectList.GetNext())
	{
		if(m_checkList.GetValue("type"))
			m_pSelectStatus->cType=m_xDefaultWindSeg.cType;
		if(m_checkList.GetValue("cMirType"))
			m_pSelectStatus->cMirType=m_xDefaultWindSeg.cMirType;
		if(m_checkList.GetValue("cCalStyle"))
			m_pSelectStatus->cCalStyle=m_xDefaultWindSeg.cCalStyle;
		if(m_checkList.GetValue("BetaZ"))
			m_pSelectStatus->BetaZ=m_xDefaultWindSeg.BetaZ;
		if(m_checkList.GetValue("BetaZForFoundation"))
			m_pSelectStatus->BetaZForFoundation=m_xDefaultWindSeg.BetaZForFoundation;
		if(m_checkList.GetValue("CqiFront"))
			m_pSelectStatus->CqiFront=m_xDefaultWindSeg.CqiFront;
		if(m_checkList.GetValue("CqiSide"))
			m_pSelectStatus->CqiSide=m_xDefaultWindSeg.CqiSide;
		if(m_checkList.GetValue("m_bAutoCalProfileCoef"))
			m_pSelectStatus->m_bAutoCalProfileCoef=m_xDefaultWindSeg.m_bAutoCalProfileCoef;
		if(m_xDefaultWindSeg.m_bAutoCalProfileCoef&&m_checkList.GetValue("m_fProfileModeCoef"))
			m_pSelectStatus->m_fProfileModeCoef=m_xDefaultWindSeg.m_fProfileModeCoef;
		if(m_checkList.GetValue("m_bSpecB_A"))
			m_pSelectStatus->m_bSpecB_A=m_xDefaultWindSeg.m_bSpecB_A;
		if(m_xDefaultWindSeg.m_bSpecB_A&&m_checkList.GetValue("B_AFront"))
			m_pSelectStatus->B_AFront=m_xDefaultWindSeg.B_AFront;
		if(m_checkList.GetValue("K"))
			m_pSelectStatus->K=m_xDefaultWindSeg.K;
		if(m_checkList.GetValue("basis_points"))
		{
			m_pSelectStatus->xProjPlaneOrg.x=m_xDefaultWindSeg.xProjPlaneOrg.x;
			m_pSelectStatus->xProjPlaneOrg.y=m_xDefaultWindSeg.xProjPlaneOrg.y;
			m_pSelectStatus->xProjPlaneOrg.z=m_xDefaultWindSeg.xProjPlaneOrg.z;
		}
	}
	CDialog::OnOK();
}
