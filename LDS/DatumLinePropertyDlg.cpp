// DatumLinePropertyDlg.cpp : 实现文件
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "DatumLinePropertyDlg.h"
#include "LmaDlg.h"
#include "env_def.h"
#include "ScrTools.h"
#include "ReportPointDlg.h"
#include "DisplayPointProperty.h"

// CDatumLinePropertyDlg 对话框

IMPLEMENT_DYNAMIC(CDatumLinePropertyDlg, CDialog)
CDatumLinePropertyDlg::CDatumLinePropertyDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDatumLinePropertyDlg::IDD, pParent)
	, m_fPointX(0)
	, m_fPointY(0)
	, m_fPointZ(0)
{
	m_idEventProp=0;
	m_pActivePt=NULL;
	m_bNewStartPoint=FALSE;
	m_bNewEndPoint=FALSE;
}

CDatumLinePropertyDlg::~CDatumLinePropertyDlg()
{
}
void SetEditState(CPropertyList *pPropList,CLDSPoint* pDatumPoint);
void CDatumLinePropertyDlg::UpdateEditPoint()
{
	if(m_pActivePt==NULL)
		return;
	if(m_propList.m_iPropGroup>0)
		SetEditState(&m_propList,m_pActivePt);
	m_pActivePt->UpdatePos();
	m_fPointY=m_pActivePt->Position().y;
	m_fPointX=m_pActivePt->Position().x;
	m_fPointZ=m_pActivePt->Position().z;
	UpdateData(FALSE);
}
void CDatumLinePropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Text(pDX, IDC_EDT_X, m_fPointX);
	DDX_Text(pDX, IDC_EDT_Y, m_fPointY);
	DDX_Text(pDX, IDC_EDT_Z, m_fPointZ);
}

BEGIN_MESSAGE_MAP(CDatumLinePropertyDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnTcnSelchangeTabGroup)
	ON_EN_CHANGE(IDC_EDT_X, OnEnChangeEdtX)
	ON_EN_CHANGE(IDC_EDT_Y, OnEnChangeEdtY)
	ON_EN_CHANGE(IDC_EDT_Z, OnEnChangeEdtZ)
END_MESSAGE_MAP()


// CDatumLinePropertyDlg 消息处理程序
static void SetEditState(CPropertyList *pPropList,CLDSPoint* pDatumPoint)
{
	CDatumLinePropertyDlg* pLinePropDlg=(CDatumLinePropertyDlg*)pPropList->GetParent();
	((CEdit*)pLinePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(pDatumPoint->datum_pos_style!=0);
	((CEdit*)pLinePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(pDatumPoint->datum_pos_style!=0);
	((CEdit*)pLinePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(pDatumPoint->datum_pos_style!=0);
	if(pDatumPoint->datum_pos_style==5)
	{
		if(pDatumPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			((CEdit*)pLinePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(FALSE);
		else if(pDatumPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			((CEdit*)pLinePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(FALSE);
		else if(pDatumPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2)
			((CEdit*)pLinePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(FALSE);
	}
	else if(pDatumPoint->datum_pos_style==6)
	{
		if(pDatumPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
		{
			((CEdit*)pLinePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(FALSE);
			((CEdit*)pLinePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(FALSE);
		}
		else if(pDatumPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
		{
			((CEdit*)pLinePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(FALSE);
			((CEdit*)pLinePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(FALSE);
		}
		else if(pDatumPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==2)
		{
			((CEdit*)pLinePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(FALSE);
			((CEdit*)pLinePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(FALSE);
		}
	}
}
static bool ReflectPointPropertyModifyValueFunc(CNativePropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CDatumLinePropertyDlg* pDlg=(CDatumLinePropertyDlg*)pPropList->GetParent();
	if(pDlg->m_pActivePt)
	{
		pDlg->m_propList.CopyTo(pDlg->m_pActivePt);
		pDlg->UpdateEditPoint();
	}
	else if(pItem->m_idProp==pDlg->GetPropID("description"))
		StrCopy(pDlg->m_datumLine.description,valueStr,101);
	return true;
}
static bool ReflectPointPropertyButtonClickFunc(CNativePropertyList* pPropList,CPropTreeItem* pItem)
{   
	CDatumLinePropertyDlg* pDlg=(CDatumLinePropertyDlg*)pPropList->GetParent();
	if(pDlg->m_pActivePt)
	{
		pDlg->m_propList.CopyTo(pDlg->m_pActivePt);
		pDlg->UpdateEditPoint();
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
BOOL CDatumLinePropertyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//
	int id=InitPropHashtable();
#ifdef AFX_TARG_ENU_ENGLISH
	m_ctrlPropGroup.InsertItem(0,"General information");
	m_ctrlPropGroup.InsertItem(1,"Start Datum Point");
	m_ctrlPropGroup.InsertItem(2,"End Datum Point");
#else 
	m_ctrlPropGroup.InsertItem(0,"常规信息");
	m_ctrlPropGroup.InsertItem(1,"始端基准点");
	m_ctrlPropGroup.InsertItem(2,"终端基准点");
#endif
	m_ctrlPropGroup.SetCurSel(m_propList.m_iPropGroup);
	if(m_propList.m_iPropGroup==1)
		m_pActivePt=&m_startPoint;
	else if(m_propList.m_iPropGroup==2)
		m_pActivePt=&m_endPoint;
	//
	m_propList.m_hPromptWnd = GetDlgItem(IDC_S_DATUM_LINE_HELP_STR)->GetSafeHwnd();
	m_propList.InitPropHashtable(id);
	m_propList.SetDividerScale(0.4);
	m_propList.m_bDisPosStyle=TRUE;
	m_propList.SetReflectButtonClickFunc(ReflectPointPropertyButtonClickFunc);
	m_propList.SetReflectModifyValueFunc(ReflectPointPropertyModifyValueFunc);
	//
	((CEdit*)GetDlgItem(IDC_EDT_X))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_EDT_Y))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_EDT_Z))->SetReadOnly(TRUE);
	if(!m_bInernalStart)
	{
		if(m_datumLine.handle>0x20)
		{
			CLDSPoint* pPoint=NULL;
			//始端
			if(m_datumLine.hPointArr[0].bSimpleNodePoint)
			{
				m_startPoint.datum_pos_style=8;
				m_startPoint.des_para.hNode=m_datumLine.hPointArr[0].handle;
				m_startPoint.UpdatePos();
			}
			else if(m_datumLine.hPointArr[0].bRodTipStart)
			{
				m_startPoint.datum_pos_style=1;	//杆件端点定位
				m_startPoint.des_para.RODEND.hRod=m_datumLine.hPointArr[0].handle;
				m_startPoint.des_para.RODEND.direction=0;
				m_startPoint.des_para.RODEND.wing_offset_style=4;
				m_startPoint.des_para.RODEND.wing_offset_dist=0;
				m_startPoint.des_para.RODEND.len_offset_dist=0;
				m_startPoint.UpdatePos();
			}
			else if(m_datumLine.hPointArr[0].bRodTipEnd)
			{
				m_startPoint.datum_pos_style=1;	//杆件端点定位
				m_startPoint.des_para.RODEND.hRod=m_datumLine.hPointArr[0].handle;
				m_startPoint.des_para.RODEND.direction=1;
				m_startPoint.des_para.RODEND.wing_offset_style=4;
				m_startPoint.des_para.RODEND.wing_offset_dist=0;
				m_startPoint.des_para.RODEND.len_offset_dist=0;
				m_startPoint.UpdatePos();
			}
			else
			{
				pPoint=Ta.Point.FromHandle(m_datumLine.hPointArr[0]);
				if(pPoint)
					m_startPoint=*pPoint;
			}
			//终端
			if(m_datumLine.hPointArr[1].bSimpleNodePoint)
			{
				m_endPoint.datum_pos_style=8;
				m_endPoint.des_para.hNode=m_datumLine.hPointArr[1].handle;
				m_endPoint.UpdatePos();
			}
			else if(m_datumLine.hPointArr[1].bRodTipStart)
			{
				m_endPoint.datum_pos_style=1;	//杆件端点定位
				m_endPoint.des_para.RODEND.hRod=m_datumLine.hPointArr[1].handle;
				m_endPoint.des_para.RODEND.direction=0;
				m_endPoint.des_para.RODEND.wing_offset_style=4;
				m_endPoint.des_para.RODEND.wing_offset_dist=0;
				m_endPoint.des_para.RODEND.len_offset_dist=0;
				m_endPoint.UpdatePos();
			}
			else if(m_datumLine.hPointArr[1].bRodTipEnd)
			{
				m_endPoint.datum_pos_style=1;	//杆件端点定位
				m_endPoint.des_para.RODEND.hRod=m_datumLine.hPointArr[1].handle;
				m_endPoint.des_para.RODEND.direction=1;
				m_endPoint.des_para.RODEND.wing_offset_style=4;
				m_endPoint.des_para.RODEND.wing_offset_dist=0;
				m_endPoint.des_para.RODEND.len_offset_dist=0;
				m_endPoint.UpdatePos();
			}
			else
			{
				pPoint=Ta.Point.FromHandle(m_datumLine.hPointArr[1]);
				if(pPoint)
					m_endPoint=*pPoint;
			}
		}
		if(m_pActivePt)
			m_propList.CopyFrom(m_pActivePt);
	}
	else if(m_bInernalStart)	
	{
		m_propList.FinishSelectObjOper(m_hPickObj,GetEventPropId());//完成选择对象的后续操作
		if(m_pActivePt)
			m_propList.CopyTo(m_pActivePt);
	}
	UpdateEditPoint();
	//显示基准线信息
	DisplayDatumLineProperty();
	if(m_bInernalStart)	
	{	//恢复选中项
		m_propList.Redraw();
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();
		m_propList.SetCurSel(pItem->m_iIndex);	//设置选中项	
	}
	UpdateData(FALSE);
	return TRUE;
}

void CDatumLinePropertyDlg::OnTcnSelchangeTabGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_propList.m_iPropGroup==1)
		m_propList.CopyTo(&m_startPoint);
	else if(m_propList.m_iPropGroup==2)
		m_propList.CopyTo(&m_endPoint);
	m_propList.m_iPropGroup = m_ctrlPropGroup.GetCurSel();
	if(m_propList.m_iPropGroup==0)
	{
		m_fPointX=m_fPointY=m_fPointZ=0;
		((CEdit*)GetDlgItem(IDC_EDT_X))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDT_Y))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDT_Z))->SetReadOnly(TRUE);
	}
	else if(m_propList.m_iPropGroup==1)
	{
		m_fPointX=m_startPoint.Position().x;
		m_fPointY=m_startPoint.Position().y;
		m_fPointZ=m_startPoint.Position().z;
		SetEditState(&m_propList,&m_startPoint);
		m_pActivePt=&m_startPoint;
	}
	else if(m_propList.m_iPropGroup==2)
	{	
		m_fPointX=m_endPoint.Position().x;
		m_fPointY=m_endPoint.Position().y;
		m_fPointZ=m_endPoint.Position().z;
		SetEditState(&m_propList,&m_endPoint);
		m_pActivePt=&m_endPoint;
	}
	UpdateData(FALSE);
	DisplayDatumLineProperty();
	*pResult = 0;
}
void CDatumLinePropertyDlg::OnOK()
{
	UpdateData();
	if(m_propList.m_iPropGroup==1)
		m_propList.CopyTo(&m_startPoint);
	else if(m_propList.m_iPropGroup==2)
		m_propList.CopyTo(&m_endPoint);
	char sError[200]="";
	if(!m_startPoint.AssertValid(console.GetActiveModel(),sError))
	{
		AfxMessageBox(CXhChar200("基准线始端参数不完整,%s",sError));
		return;
	}
	if(!m_endPoint.AssertValid(console.GetActiveModel(),sError))
	{
		AfxMessageBox(CXhChar200("基准线终端参数不完整,%s",sError));
		return;
	}
	CLDSPoint* pDatumPoint=NULL;
	if(m_startPoint.handle<0X20)
	{		//新增基准点
		m_bNewStartPoint=TRUE;
		pDatumPoint=console.AppendPoint();
		m_startPoint.handle=pDatumPoint->handle;
	}
	else	//更新数据
		pDatumPoint=Ta.Point.FromHandle(m_startPoint.handle);
	*pDatumPoint=m_startPoint;
	pDatumPoint->UpdatePos(console.GetActiveModel());
	//
	if(m_endPoint.handle<0X20)
	{		//新增基准点
		m_bNewEndPoint=TRUE;
		pDatumPoint=console.AppendPoint();
		m_endPoint.handle=pDatumPoint->handle;
	}
	else	//更新数据
		pDatumPoint=Ta.Point.FromHandle(m_endPoint.handle);
	*pDatumPoint=m_endPoint;
	pDatumPoint->UpdatePos(console.GetActiveModel());
	if(m_startPoint.handle==m_endPoint.handle)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The start or end of datum line is overlap！");
#else 
		AfxMessageBox("基准线的始终端基准点重合！");
#endif
		return;
	}
	m_datumLine.hPointArr[0]=m_startPoint.handle;
	m_datumLine.hPointArr[1]=m_endPoint.handle;
	CDialog::OnOK();
}
const DWORD HASHTABLESIZE1 = 500;
CStringKeyHashTable<long> CDatumLinePropertyDlg::propHashtable;
int CDatumLinePropertyDlg::InitPropHashtable()
{
	int id=1;
	propHashtable.CreateHashTable(HASHTABLESIZE1);
	//基准线基本信息
	propHashtable.SetValueAt("basic",id++);
	propHashtable.SetValueAt("handle",id++);
	propHashtable.SetValueAt("description",id++);
	propHashtable.SetValueAt("start_pos",id++);
	propHashtable.SetValueAt("start_pos.x",id++);
	propHashtable.SetValueAt("start_pos.y",id++);
	propHashtable.SetValueAt("start_pos.z",id++);
	propHashtable.SetValueAt("end_pos",id++);
	propHashtable.SetValueAt("end_pos.x",id++);
	propHashtable.SetValueAt("end_pos.y",id++);
	propHashtable.SetValueAt("end_pos.z",id++);
	return id;
}
void CDatumLinePropertyDlg::DisplayDatumLineProperty()
{
	m_propList.CleanTree();
	m_propList.m_nObjClassTypeId = 0;
	CPropTreeItem *pGroupItem,*pPropItem,*pSonItem=NULL,*pRoot=m_propList.GetRootItem();
	CXhChar100 sText;
	const int GROUP_DATUMLINE=1,GROUP_DATUMPT1=2,GROUP_DATUMPT2=3;
	
	if(m_propList.m_iPropGroup>0)
	{
		m_propList.CopyFrom(m_pActivePt);
		m_propList.DisplayPropertyList(0,GetSingleWord(m_propList.m_iPropGroup+1));
	}
	else
	{	//基本信息
		CItemInfo*lpInfo = new CItemInfo();
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Basic Information";
	#else 
		lpInfo->m_strPropName = "基本信息";
	#endif
		pGroupItem = m_propList.InsertItem(pRoot,lpInfo,-1);
		pGroupItem->m_idProp = CDatumLinePropertyDlg::GetPropID("basic");
		pGroupItem->m_bHideChildren=FALSE;
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMLINE); 
		//句柄
		lpInfo= new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "Datum line's handle";
	#else 
		lpInfo->m_strPropName = "句柄";
		lpInfo->m_strPropHelp = "基准线句柄";
	#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CDatumLinePropertyDlg::GetPropID("handle");
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMLINE);
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
		//描述
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Description";
		lpInfo->m_strPropHelp = "The description of datum line";
	#else 
		lpInfo->m_strPropName = "描述";
		lpInfo->m_strPropHelp = "对基准线进行描述";
	#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CDatumLinePropertyDlg::GetPropID("description");
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMLINE);
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		//始端坐标
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Start Coordinate";
		lpInfo->m_strPropHelp="Start coordinate of datum line ";
	#else 
		lpInfo->m_strPropName="始端坐标";
		lpInfo->m_strPropHelp="基准线始端坐标";
	#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CDatumLinePropertyDlg::GetPropID("start_pos");
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMLINE);
		pPropItem->m_bHideChildren=FALSE;
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="X";
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Start X coordinate of datum line ";
	#else 
		lpInfo->m_strPropHelp="基准线始端X坐标";
	#endif
		pSonItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
		pSonItem->m_idProp = CDatumLinePropertyDlg::GetPropID("start_pos.x");
		pSonItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMLINE);
		if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonItem->SetReadOnly();
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Start Y coordinate of datum line";
	#else 
		lpInfo->m_strPropHelp="基准线始端Y坐标";
	#endif
		pSonItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
		pSonItem->m_idProp = CDatumLinePropertyDlg::GetPropID("start_pos.y");
		pSonItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMLINE);
		if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonItem->SetReadOnly();
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Start Z coordinate of datum line";
	#else 
		lpInfo->m_strPropHelp="基准线始端Z坐标";
	#endif
		pSonItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
		pSonItem->m_idProp = CDatumLinePropertyDlg::GetPropID("start_pos.z");
		pSonItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMLINE);
		if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonItem->SetReadOnly();
		//终端坐标
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="End Coordinate";
		lpInfo->m_strPropHelp="End coordinate of datum line ";
	#else 
		lpInfo->m_strPropName="终端坐标";
		lpInfo->m_strPropHelp="基准线终端坐标";
	#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CDatumLinePropertyDlg::GetPropID("end_pos");
		pPropItem->m_bHideChildren=FALSE;
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMLINE);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="X";
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="End X coordinate of datum line ";
	#else 
		lpInfo->m_strPropHelp="基准线终端X坐标";
	#endif
		pSonItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
		pSonItem->m_idProp = CDatumLinePropertyDlg::GetPropID("end_pos.x");
		if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonItem->SetReadOnly();
		pSonItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMLINE);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="End Y coordinate of datum line ";
	#else 
		lpInfo->m_strPropHelp="基准线终端Y坐标";
	#endif
		pSonItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
		pSonItem->m_idProp = CDatumLinePropertyDlg::GetPropID("end_pos.y");
		if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonItem->SetReadOnly();
		pSonItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMLINE);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
	#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="End Z coordinate of datum line ";
	#else 
		lpInfo->m_strPropHelp="基准线终端Z坐标";
	#endif
		pSonItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
		pSonItem->m_idProp = CDatumLinePropertyDlg::GetPropID("end_pos.z");
		if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonItem->SetReadOnly();
		pSonItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMLINE);
		m_propList.Redraw();
	}
}
int CDatumLinePropertyDlg::GetPropValueStr(long id,char *valueStr)
{
	CXhChar100 sText;
	if(m_datumLine.handle>0x20)
		m_datumLine.UpdateLine(console.GetActiveModel());
	else
	{
		m_startPoint.UpdatePos();
		m_endPoint.UpdatePos();
		m_datumLine.start_pos=m_startPoint.Position();
		m_datumLine.end_pos=m_endPoint.Position();
	}
	if(GetPropID("handle")==id)
		sText.Printf("0X%X",m_datumLine.handle);
	else if(GetPropID("description")==id)
		sText.Printf("%s",m_datumLine.description);
	else if(GetPropID("start_pos.x")==id)
	{
		sText.Printf("%f",m_datumLine.start_pos.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("start_pos.y")==id)
	{
		sText.Printf("%f",m_datumLine.start_pos.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("start_pos.z")==id)
	{
		sText.Printf("%f",m_datumLine.start_pos.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("end_pos.x")==id)
	{
		sText.Printf("%f",m_datumLine.end_pos.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("end_pos.y")==id)
	{
		sText.Printf("%f",m_datumLine.end_pos.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("end_pos.z")==id)
	{
		sText.Printf("%f",m_datumLine.end_pos.z);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
long CDatumLinePropertyDlg::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}

void CDatumLinePropertyDlg::OnEnChangeEdtX()
{
	UpdateData();
	CString sCoordX;
	sCoordX.Format("%g",m_fPointX);
	CPropTreeItem* pItem=m_propList.FindItemByPropId(m_propList.GetPropID("pos.x"),NULL);
	if(pItem&&m_propList.ModifyValueFunc(pItem,sCoordX,NULL))
		m_propList.SetItemPropValue(pItem->m_idProp,sCoordX);
}
void CDatumLinePropertyDlg::OnEnChangeEdtY()
{
	UpdateData();
	CString sCoordY;
	sCoordY.Format("%g",m_fPointY);
	CPropTreeItem* pItem=m_propList.FindItemByPropId(m_propList.GetPropID("pos.y"),NULL);
	if(pItem&&m_propList.ModifyValueFunc(pItem,sCoordY,NULL))
		m_propList.SetItemPropValue(pItem->m_idProp,sCoordY);
}
void CDatumLinePropertyDlg::OnEnChangeEdtZ()
{
	UpdateData();
	CString sCoordZ;
	sCoordZ.Format("%g",m_fPointZ);
	CPropTreeItem* pItem=m_propList.FindItemByPropId(m_propList.GetPropID("pos.z"),NULL);
	if(pItem&&m_propList.ModifyValueFunc(pItem,sCoordZ,NULL))
		m_propList.SetItemPropValue(pItem->m_idProp,sCoordZ);
}
