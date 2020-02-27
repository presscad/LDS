// SelectWorkCaseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SelectWorkCaseDlg.h"
#include "afxdialogex.h"
#include "env_def.h"
#include "PropertyListOper.h"

#ifdef __PART_DESIGN_INC_
//回调函数处理
static BOOL WorkCasePropertyModifyValueFunc(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CSelectWorkCaseDlg* pDlg=(CSelectWorkCaseDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	PROPLIST_ITEM* pPropItem=CWorkStatus::GetPropItem(pItem->m_idProp);
	pDlg->m_checkList.SetValue(pPropItem->propKeyStr,TRUE);
	//
	CWorkStatus* pFirstWork=pDlg->m_selectList.GetFirst();
	CPropertyListOper<CWorkStatus> oper(pPropList,pFirstWork,&(pDlg->m_selectList));
	CXhChar100 tem_str;
	tem_str.Printf("%s",valueStr);
	if(pItem->m_idProp==CWorkStatus::GetPropID("description"))
		strcpy(pDlg->m_xDefaultWork.description,tem_str);
	else if(pItem->m_idProp==CWorkStatus::GetPropID("main_load"))
	{
		pDlg->m_xDefaultWork.m_cMainLoadType=tem_str[0]-'0';
		//更新属性界面
		if(pDlg->m_xDefaultWork.m_cMainLoadType<5)
			pPropList->DeleteAllSonItems(pItem);
		else
		{
			oper.InsertEditPropItem(pItem,"vibration_mode","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"HoriEarthQuakeAxis","","",-1,TRUE);
		}
		CPropTreeItem* pPropItem=(CPropTreeItem*)pPropList->GetItemDataPtr(0);
		pDlg->SelchangeCmbMainLoadStyle(pPropItem);
	}
	else if(pItem->m_idProp==CWorkStatus::GetPropID("limit_str"))
		strcpy(pDlg->m_xDefaultWork.limit_str,tem_str);
	else if(pItem->m_idProp==CWorkStatus::GetPropID("wind_speed"))
		pDlg->m_xDefaultWork.WindLoad.velocity=atof(tem_str);
	else if(pItem->m_idProp==CWorkStatus::GetPropID("azimuth"))
		pDlg->m_xDefaultWork.WindLoad.azimuth=atoi(tem_str);
	else if(pItem->m_idProp==CWorkStatus::GetPropID("ice_thick"))
	{
		pDlg->m_xDefaultWork.ice_thick=atof(tem_str);
		//
		CXhChar100 value("%lg",pDlg->m_xDefaultWork.Bw_I());
		pPropList->SetItemPropValue(CWorkStatus::GetPropID("tower_ice_factor"),value);
	}
	else if(pItem->m_idProp==CWorkStatus::GetPropID("tower_ice_factor"))
		pDlg->m_xDefaultWork.SetBw_I(atof(tem_str));
	else if(pItem->m_idProp==CWorkStatus::GetPropID("vibration_mode"))
		strcpy(pDlg->m_xDefaultWork.vibration_mode_str,tem_str);
	else if(pItem->m_idProp==CWorkStatus::GetPropID("HoriEarthQuakeAxis"))
		pDlg->m_xDefaultWork.m_cHoriEarthQuakeAxis=atoi(tem_str);
	else if(pItem->m_idProp==CWorkStatus::GetPropID("WireKesai"))
		pDlg->m_xDefaultWork.SetKesai(atof(tem_str));
	else if(pItem->m_idProp==CWorkStatus::GetPropID("WireGamaQi"))
		pDlg->m_xDefaultWork.SetGamaQ(atof(tem_str));
	else if(pItem->m_idProp==CWorkStatus::GetPropID("TowerWindKesai"))
		pDlg->m_xDefaultWork.SetKesai_W(atof(tem_str));
	else if(pItem->m_idProp==CWorkStatus::GetPropID("TowerWindGamaQi"))
		pDlg->m_xDefaultWork.SetGamaQ_W(atof(tem_str));
	else if(pItem->m_idProp==CWorkStatus::GetPropID("TowerIceKesai"))
		pDlg->m_xDefaultWork.SetKesai_I(atof(tem_str));
	else if(pItem->m_idProp==CWorkStatus::GetPropID("TowerIceGamaQi"))
		pDlg->m_xDefaultWork.SetGamaQ_I(atof(tem_str));
// 	else if(pItem->m_idProp==CWorkStatus::GetPropID("HorizEarthQuakeKesai"))
// 		pDlg->m_xDefaultWork.gamaEv(atof(tem_str));
	else if(pItem->m_idProp==CWorkStatus::GetPropID("HorizEarthQuakeQamaQi"))
		pDlg->m_xDefaultWork.SetGamaQ_Eh(atof(tem_str));
// 	else if(pItem->m_idProp==CWorkStatus::GetPropID("VertEarthQuakeKesai"))
// 		pDlg->m_xDefaultWork.gamaEh(atof(tem_str));
	else if(pItem->m_idProp==CWorkStatus::GetPropID("VertEarthQuakeQamaQi"))
		pDlg->m_xDefaultWork.SetGamaQ_Ev(atof(tem_str));
	return true;
}

// CSelectWorkCaseDlg 对话框
IMPLEMENT_DYNAMIC(CSelectWorkCaseDlg, CDialogEx)

CSelectWorkCaseDlg::CSelectWorkCaseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelectWorkCaseDlg::IDD, pParent)
{
	m_checkList.Empty();
}

CSelectWorkCaseDlg::~CSelectWorkCaseDlg()
{
}

void CSelectWorkCaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WORK_CASE, m_propList);
}
BEGIN_MESSAGE_MAP(CSelectWorkCaseDlg, CDialogEx)
END_MESSAGE_MAP()
// CSelectWorkCaseDlg 消息处理程序
BOOL CSelectWorkCaseDlg::OnInitDialog()
{
	if(m_selectList.GetNodeNum()<=0)
		return FALSE;
	CDialog::OnInitDialog();
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.4);
	m_propList.SetModifyValueFunc(WorkCasePropertyModifyValueFunc);
	//显示工况属性
	CWorkStatus::InitPropHashtable();
	DisplayWorkCaseProperty();
	return TRUE;
}
void CSelectWorkCaseDlg::DisplayWorkCaseProperty()
{
	m_propList.CleanTree();
	CWorkStatus* pFirstWork=m_selectList.GetFirst();
	char pooldata[512]={0};
	CBuffer pool(pooldata,512);
	pFirstWork->ToBasicCoefBuffer(pool);
	pool.SeekToBegin();
	m_xDefaultWork.FromBasicCoefBuffer(pool);
	CPropertyListOper<CWorkStatus> oper(&m_propList,pFirstWork,&m_selectList);
	CPropTreeItem *pRoot=m_propList.GetRootItem(),*pGroupItem=NULL,*pPropItem=NULL,*pSonItem=NULL;
	pGroupItem=oper.InsertPropItem(pRoot,"attribute");
	pPropItem=oper.InsertEditPropItem(pGroupItem,"description");			//工况名称
	pPropItem->SetReadOnly(FALSE);
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"main_load");//主要荷载
	pPropItem->SetReadOnly(Ta.m_iLoadExecuteCode==3);
	if(pFirstWork->m_cMainLoadType>=5)
	{
		oper.InsertEditPropItem(pPropItem,"vibration_mode");	//适用振型范围
		oper.InsertEditPropItem(pPropItem,"HoriEarthQuakeAxis");//水平地震主轴方向角
	}
	oper.InsertEditPropItem(pGroupItem,"limit_str");				//适用范围
	oper.InsertEditPropItem(pGroupItem,"ice_thick");				//覆冰厚度
	pPropItem=oper.InsertEditPropItem(pGroupItem,"tower_ice_factor");	//杆塔覆冰风荷载最大系
	pPropItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);	//执行特定规范非用户指定系数时为只读状态
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"WindLoad");
	pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
	oper.InsertEditPropItem(pPropItem,"wind_speed");		//风速
	oper.InsertEditPropItem(pPropItem,"azimuth");			//方向角
	SelchangeCmbMainLoadStyle(pGroupItem,false);
	m_propList.Redraw();
}

void CSelectWorkCaseDlg::SelchangeCmbMainLoadStyle(CPropTreeItem* pGroupItem,bool initDefaultParams/*=true*/)
{
	CPropertyListOper<CWorkStatus> oper(&m_propList,&m_xDefaultWork);
	CPropTreeItem *pSonItem=NULL,*pPropItem=NULL;
	m_propList.DeleteItemByPropId(CWorkStatus::GetPropID("LineLoad"));
	m_propList.DeleteItemByPropId(CWorkStatus::GetPropID("TowerWindLoad"));
	m_propList.DeleteItemByPropId(CWorkStatus::GetPropID("TowerIceLoad"));
	m_propList.DeleteItemByPropId(CWorkStatus::GetPropID("HorizontalSeismicLoad"));
	m_propList.DeleteItemByPropId(CWorkStatus::GetPropID("VerticalSeismicLoad"));
	if(Ta.m_iLoadExecuteCode!=3)
		initDefaultParams=false;
	if(initDefaultParams)
	{
		m_xDefaultWork.SetGamaQ(1.4);
		m_xDefaultWork.SetGamaQ_A(1.4);
		m_xDefaultWork.SetGamaQ_I(1.4);
		m_xDefaultWork.SetGamaQ_W(1.4);
		m_xDefaultWork.SetGamaQ_U(1.4);
		m_xDefaultWork.SetGamaQ_T(1.4);
	}
	if(m_xDefaultWork.m_cMainLoadType==0)		//大风
	{
		if(initDefaultParams)
		{
			m_xDefaultWork.SetKesai(1.0);
			m_xDefaultWork.SetKesai_W(1.0);
			m_xDefaultWork.SetKesai_I(1.0);
		}
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"LineLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"TowerWindLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
	}
	else if(m_xDefaultWork.m_cMainLoadType==1)	//覆冰
	{
		if(initDefaultParams)
		{
			m_xDefaultWork.SetKesai(1.0);
			m_xDefaultWork.SetKesai_W(1.0);
			m_xDefaultWork.SetKesai_I(1.0);
			m_xDefaultWork.SetKesai_T(1.0);
		}
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"LineLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"TowerWindLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"TowerIceLoad");
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerIceKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerIceGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
	}
	else if(m_xDefaultWork.m_cMainLoadType==2)	//低温
	{
		if(initDefaultParams)
		{
			m_xDefaultWork.SetKesai(1.0);
			m_xDefaultWork.SetKesai_W(1.0);
			m_xDefaultWork.SetKesai_I(1.0);
			m_xDefaultWork.SetKesai_T(1.0);
		}
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"LineLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"TowerWindLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
	}
	else if(m_xDefaultWork.m_cMainLoadType==3)	//断线及不均匀覆冰
	{
		if(initDefaultParams)
		{
			m_xDefaultWork.SetKesai(0.9);
			m_xDefaultWork.SetKesai_W(0.9);
			m_xDefaultWork.SetKesai_U(0.9);
			m_xDefaultWork.SetKesai_I(0.9);
			m_xDefaultWork.SetKesai_T(0.9);
		}
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"LineLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"TowerWindLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"TowerIceLoad");
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerIceKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerIceGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
	}
	else if(m_xDefaultWork.m_cMainLoadType==4)	//安装
	{
		if(initDefaultParams)
		{
			m_xDefaultWork.SetKesai(0.9);
			m_xDefaultWork.SetKesai_W(0.9);
			m_xDefaultWork.SetKesai_A(0.9);
			m_xDefaultWork.SetKesai_U(0.9);
			m_xDefaultWork.SetKesai_I(0.9);
			m_xDefaultWork.SetKesai_T(0.9);
		}
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"LineLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"TowerWindLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
	}
	else if(m_xDefaultWork.m_cMainLoadType==5)	//水平地震
	{
		if(initDefaultParams)
		{
			m_xDefaultWork.SetGamaQ_Eh(1.3);
			m_xDefaultWork.SetGamaQ_Ev(0.5);
			m_xDefaultWork.SetKesai(1.0);	//导地线荷载组合系数
			m_xDefaultWork.SetGamaQ(0.5);	//导地线荷载分项系数
			m_xDefaultWork.SetKesai_W(0.3);	//风荷载组合系数
			m_xDefaultWork.SetGamaQ_W(1.0);	//风荷载分项系数
		}
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"LineLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"TowerWindLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"HorizontalSeismicLoad");
		pSonItem=oper.InsertEditPropItem(pPropItem,"HorizEarthQuakeQamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"HorizEarthQuakeKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"VerticalSeismicLoad");
		pSonItem=oper.InsertEditPropItem(pPropItem,"VertEarthQuakeQamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"VertEarthQuakeKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
	}
	else if(m_xDefaultWork.m_cMainLoadType==6)	//竖向地震
	{
		if(initDefaultParams)
		{
			m_xDefaultWork.SetGamaQ_Eh(0.5);
			m_xDefaultWork.SetGamaQ_Ev(1.3);
			m_xDefaultWork.SetKesai(1.0);	//导地线荷载组合系数
			m_xDefaultWork.SetGamaQ(0.5);	//导地线荷载分项系数
			m_xDefaultWork.SetKesai_W(0.3);	//风荷载组合系数
			m_xDefaultWork.SetGamaQ_W(1.0);	//风荷载分项系数
		}
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"LineLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"TowerWindLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"HorizontalSeismicLoad");
		pSonItem=oper.InsertEditPropItem(pPropItem,"HorizEarthQuakeQamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"HorizEarthQuakeKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"VerticalSeismicLoad");
		pSonItem=oper.InsertEditPropItem(pPropItem,"VertEarthQuakeQamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"VertEarthQuakeKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
	}
	else if(m_xDefaultWork.m_cMainLoadType==7)	//仅水平地震
	{
		m_xDefaultWork.SetGamaQ_Eh(1.3);
		m_xDefaultWork.SetGamaQ_Ev(0.0);
		m_xDefaultWork.SetKesai(1.0);	//导地线荷载组合系数
		m_xDefaultWork.SetGamaQ(0.5);	//导地线荷载分项系数
		m_xDefaultWork.SetKesai_W(0.3);	//风荷载组合系数
		m_xDefaultWork.SetGamaQ_W(1.0);	//风荷载分项系数
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"LineLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"TowerWindLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"HorizontalSeismicLoad");
		pSonItem=oper.InsertEditPropItem(pPropItem,"HorizEarthQuakeQamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"HorizEarthQuakeKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"VerticalSeismicLoad");
		pSonItem=oper.InsertEditPropItem(pPropItem,"VertEarthQuakeQamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"VertEarthQuakeKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
	}
	else if(m_xDefaultWork.m_cMainLoadType==8)	//仅竖向地震
	{
		m_xDefaultWork.SetGamaQ_Eh(0.0);
		m_xDefaultWork.SetGamaQ_Ev(1.3);
		m_xDefaultWork.SetKesai(1.0);	//导地线荷载组合系数
		m_xDefaultWork.SetGamaQ(0.5);	//导地线荷载分项系数
		m_xDefaultWork.SetKesai_W(0.3);	//风荷载组合系数
		m_xDefaultWork.SetGamaQ_W(1.0);	//风荷载分项系数
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"LineLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"WireGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"TowerWindLoad");
		pPropItem->m_lpNodeInfo->m_controlType=CItemInfo::TYPE_STATIC;
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"TowerWindGamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"HorizontalSeismicLoad");
		pSonItem=oper.InsertEditPropItem(pPropItem,"HorizEarthQuakeKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"HorizEarthQuakeQamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"VerticalSeismicLoad");
		pSonItem=oper.InsertEditPropItem(pPropItem,"VertEarthQuakeKesai");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
		pSonItem=oper.InsertEditPropItem(pPropItem,"VertEarthQuakeQamaQi");
		pSonItem->SetReadOnly(Ta.m_iLoadExecuteCode!=3);
	}
}

void CSelectWorkCaseDlg::OnOK()
{
	if(m_checkList.GetNodeNum()<=0)
		return CDialog::OnCancel();
	for(CWorkStatus* pSelStatus=m_selectList.GetFirst();pSelStatus;pSelStatus=m_selectList.GetNext())
	{
		if(m_checkList.GetValue("description"))
			strcpy(pSelStatus->description,m_xDefaultWork.description);
		if(m_checkList.GetValue("main_load"))
			pSelStatus->m_cMainLoadType=m_xDefaultWork.m_cMainLoadType;
		if(m_checkList.GetValue("limit_str"))
			strcpy(pSelStatus->limit_str,m_xDefaultWork.limit_str);
		if(m_checkList.GetValue("wind_speed"))
			pSelStatus->WindLoad.velocity=m_xDefaultWork.WindLoad.velocity;
		if(m_checkList.GetValue("azimuth"))
			pSelStatus->WindLoad.azimuth=m_xDefaultWork.WindLoad.azimuth;
		if(m_checkList.GetValue("ice_thick"))
		{
			pSelStatus->ice_thick=m_xDefaultWork.ice_thick;
			pSelStatus->SetBw_I(m_xDefaultWork.Bw_I());
		}
		else if(m_checkList.GetValue("tower_ice_factor"))
			pSelStatus->SetBw_I(m_xDefaultWork.Bw_I());
		if(m_checkList.GetValue("vibration_mode"))
			strcpy(pSelStatus->vibration_mode_str,m_xDefaultWork.vibration_mode_str);
		if(m_checkList.GetValue("HoriEarthQuakeAxis"))
			pSelStatus->m_cHoriEarthQuakeAxis=m_xDefaultWork.m_cHoriEarthQuakeAxis;
		if(m_checkList.GetValue("WireGamaQi"))
			pSelStatus->SetGamaQ(m_xDefaultWork.GamaQ());
		if(m_checkList.GetValue("WireKesai"))
			pSelStatus->SetKesai(m_xDefaultWork.Kesai());
		if(m_checkList.GetValue("TowerWindGamaQi"))
			pSelStatus->SetGamaQ_W(m_xDefaultWork.GamaQ_W());
		if(m_checkList.GetValue("TowerWindKesai"))
			pSelStatus->SetKesai_W(m_xDefaultWork.Kesai_W());
		if(m_checkList.GetValue("TowerIceGamaQi"))
			pSelStatus->SetGamaQ_I(m_xDefaultWork.GamaQ_I());
		if(m_checkList.GetValue("TowerIceKesai"))
			pSelStatus->SetKesai_I(m_xDefaultWork.Kesai_I());
		if(m_checkList.GetValue("HorizEarthQuakeQamaQi"))
			pSelStatus->SetGamaQ_Eh(m_xDefaultWork.GamaQ_Eh());
// 		if(m_checkList.GetValue("HorizEarthQuakeKesai"))
// 			pSelStatus->SetKesai_Eh(m_xDefaultWork.Kesai_Eh());
		if(m_checkList.GetValue("VertEarthQuakeQamaQi"))
			pSelStatus->SetGamaQ_Ev(m_xDefaultWork.GamaQ_Ev());
// 		if(m_checkList.GetValue("VertEarthQuakeKesai"))
// 			pSelStatus->SetKesai_Ev(m_xDefaultWork.Kesai_Ev());
	}
	CDialog::OnOK();
}
#endif
