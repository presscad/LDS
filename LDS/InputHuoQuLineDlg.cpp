// InputHuoQuLineDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "InputHuoQuLineDlg.h"
#include "afxdialogex.h"
#include "env_def.h"
#include "Console.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CInputHuoQuLineDlg 对话框

IMPLEMENT_DYNAMIC(CInputHuoQuLineDlg, CDialog)

CInputHuoQuLineDlg::CInputHuoQuLineDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CInputHuoQuLineDlg::IDD, pParent)
{
	m_idEventProp=0;	//恢复默认拾取类型
	m_fWingOffsetDist=0;
	m_iWingOffsetType=0;		
	m_iOffsetWing=0;
	m_sJgHandle="0X0";
	m_iCurSel=0;
	m_pCrossRod=NULL;
}

CInputHuoQuLineDlg::~CInputHuoQuLineDlg()
{
}

void CInputHuoQuLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_propStartPtList);
	DDX_Control(pDX, IDC_LIST_BOX2,m_propEndPtList);
	DDX_Control(pDX, IDC_LIST_BOX3,m_propVectorList);
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	DDX_Text(pDX, IDC_E_JG_HANDLE, m_sJgHandle);
	DDX_Text(pDX, IDC_E_WING_OFFSET,m_fWingOffsetDist);
	DDX_CBIndex(pDX, IDC_CMB_WING, m_iOffsetWing);
	DDX_CBIndex(pDX, IDC_CMB_WING_OFFSET, m_iWingOffsetType);
}


BEGIN_MESSAGE_MAP(CInputHuoQuLineDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnTcnSelchangeTabGroup)
	ON_CBN_SELCHANGE(IDC_CMB_WING_OFFSET, OnInitHuoQuLineInfo)
	ON_CBN_SELCHANGE(IDC_CMB_WING, OnInitHuoQuLineInfo)
	ON_EN_CHANGE(IDC_E_JG_HANDLE, OnInitHuoQuLineInfo)
	ON_EN_CHANGE(IDC_E_WING_OFFSET, OnInitHuoQuLineInfo)
	ON_BN_CLICKED(IDC_BTN_SELECT_PART, OnBnClickedBtnSelectPart)
END_MESSAGE_MAP()


// CInputHuoQuLineDlg 消息处理程序

BOOL CInputHuoQuLineDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ctrlPropGroup.InsertItem(0,"火曲线起点");
	m_ctrlPropGroup.InsertItem(1,"火曲线终点");
	m_ctrlPropGroup.InsertItem(2,"火曲法线");
	m_ctrlPropGroup.SetCurSel(m_iCurSel);
	//设定火曲线属性栏
	m_propStartPtList.SetDividerScale(0.5);
	m_propStartPtList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propEndPtList.SetDividerScale(0.5);
	m_propEndPtList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propVectorList.SetDividerScale(0.5);
	m_propVectorList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propStartPtList.InitPropHashtable();
	m_propStartPtList.m_bDisPosStyle=TRUE;
	m_propStartPtList.m_bDisBaseInfo=FALSE;
	m_propEndPtList.InitPropHashtable();
	m_propEndPtList.m_bDisPosStyle=TRUE;
	m_propEndPtList.m_bDisBaseInfo=FALSE;
	m_propVectorList.InitPropHashtable();
	if(!m_bInernalStart)
	{
		InitHuoquLineInfo();
		UpdateUIStatus();
	}
	else if(m_bInernalStart)
	{	
		if(m_idEventProp==-1)
		{
			m_sJgHandle.Format("0X%X",m_hPickObj);
			InitHuoquLineInfo();
			UpdateUIStatus();
		}
		if(m_iCurSel==0)
			m_propStartPtList.FinishSelectObjOper(m_hPickObj,GetEventPropId());
		else if(m_iCurSel==1)
			m_propEndPtList.FinishSelectObjOper(m_hPickObj,GetEventPropId());
		else //if(m_iCurSel==2)
			m_propVectorList.FinishSelectObjOper(m_hPickObj,GetEventPropId());
		m_propStartPtList.CopyTo(&m_xStartPt);
		m_propEndPtList.CopyTo(&m_xEndPt);
		m_propVectorList.CopyTo(&m_xVector);
	}
	RefreshPropertyListCtrl();
	if(m_bInernalStart)	
	{	//恢复选中项
		if(m_iCurSel==0)
		{
			CPropTreeItem *pItem=m_propStartPtList.FindItemByPropId(m_idEventProp,NULL);
			m_propStartPtList.SetFocus();
			if(pItem)
				m_propStartPtList.SetCurSel(pItem->m_iIndex);	//设置选中项	
		}
		else if(m_iCurSel==1)
		{
			CPropTreeItem *pItem=m_propEndPtList.FindItemByPropId(m_idEventProp,NULL);
			m_propEndPtList.SetFocus();
			if(pItem)
				m_propEndPtList.SetCurSel(pItem->m_iIndex);	//设置选中项	
		}
		else //if(m_iCurSel==2)
		{
			CPropTreeItem *pItem=m_propVectorList.FindItemByPropId(m_idEventProp,NULL);
			m_propVectorList.SetFocus();
			if(pItem)
				m_propVectorList.SetCurSel(pItem->m_iIndex);	//设置选中项	
		}
	}
	UpdateData(FALSE);
	return TRUE;
}

void CInputHuoQuLineDlg::OnTcnSelchangeTabGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_iCurSel=m_ctrlPropGroup.GetCurSel();
	RefreshPropertyListCtrl();
	UpdateData(FALSE);
	*pResult = 0;
}

void CInputHuoQuLineDlg::OnInitHuoQuLineInfo()
{
	CString ss;
	GetDlgItem(IDC_E_WING_OFFSET)->GetWindowText(ss);
	if(ss.GetLength()==1&&ss[0]=='-')
		return;
	UpdateData(TRUE);
	InitHuoquLineInfo(FALSE);	//不设为FALSE，会导致界面无法修改肢偏移方式及偏移值 wjh-2015.5.17
	UpdateUIStatus();
	RefreshPropertyListCtrl();
	UpdateData(FALSE);
}

void CInputHuoQuLineDlg::OnBnClickedBtnSelectPart()
{
	SelectObject(CLS_LINEPART,-1);
}
//////////////////////////////////////////////////////////////////////////
void CInputHuoQuLineDlg::RefreshPropertyListCtrl()
{
	CRect rect;
	GetDlgItem(IDC_WORK_PANEL)->GetWindowRect(&rect);	//屏幕坐标系
	ScreenToClient(&rect);        
	if(m_iCurSel==0)
	{
		m_propStartPtList.MoveWindow(&rect);
		m_propStartPtList.ShowWindow(true);
		m_propEndPtList.ShowWindow(false);
		m_propVectorList.ShowWindow(false);
		m_propStartPtList.DisplayPropertyList();
	}
	else if(m_iCurSel==1)
	{
		m_propEndPtList.MoveWindow(&rect);
		m_propEndPtList.ShowWindow(true);
		m_propStartPtList.ShowWindow(false);
		m_propVectorList.ShowWindow(false);
		m_propEndPtList.DisplayPropertyList();
	}
	else //if(m_iCurSel==2)
	{
		m_propVectorList.MoveWindow(&rect);
		m_propVectorList.ShowWindow(true);
		m_propStartPtList.ShowWindow(false);
		m_propEndPtList.ShowWindow(false);
		m_propVectorList.DisplayPropertyList();
	}
}

void CInputHuoQuLineDlg::InitHuoquLineInfo(BOOL bInitDefaultInfo/*=TRUE*/)
{
	long hPart=HexStrToLong(m_sJgHandle);
	CLDSPart* pPart=console.FromRodHandle(hPart);
	if(pPart&&bInitDefaultInfo)
	{
		m_iWingOffsetType=4;
		m_fWingOffsetDist=-g_sysPara.HuoQuDist;
		if(m_pCrossRod&&pPart->IsAngle())			
		{	//肢翼侧制弯
			CLDSLineAngle* pAngle=(CLDSLineAngle*) pPart;
			f3dPoint interPos;
			int res=Int3dDifferPlaneLL(pAngle->Start(),pAngle->End(),m_pCrossRod->Start(),m_pCrossRod->End(),interPos);
			if(res!=0)	//计算到有效交点
			{	//计算射线杆件延伸方向，取离交叉点较远一端为参照点
				f3dPoint ray_vec=(DISTANCE(interPos,m_pCrossRod->Start())>DISTANCE(interPos,m_pCrossRod->End()))?
								 (m_pCrossRod->Start()-interPos):(m_pCrossRod->End()-interPos);
				normalize(ray_vec);
				double ddx=pAngle->GetWingVecX()*ray_vec;
				double ddy=pAngle->GetWingVecY()*ray_vec;
				if((fabs(ddx)>fabs(ddy)&&ddx>0)||(fabs(ddx)<fabs(ddy)&&ddy>0))	//工作肢延伸方向与杆件射线方向一致
					m_fWingOffsetDist=g_sysPara.HuoQuDist+pAngle->GetWidth();
			}
		}
	}
	m_xStartPt.datum_pos_style=1;
	m_xStartPt.des_para.RODEND.hRod=hPart;
	m_xStartPt.des_para.RODEND.offset_wing=m_iOffsetWing;
	m_xStartPt.des_para.RODEND.wing_offset_style=4;
	m_xStartPt.des_para.RODEND.wing_offset_dist=m_fWingOffsetDist;
	m_xStartPt.des_para.RODEND.len_offset_dist=0;
	m_xEndPt.datum_pos_style=1;
	m_xEndPt.des_para.RODEND.hRod=hPart;
	m_xEndPt.des_para.RODEND.offset_wing=m_iOffsetWing;
	m_xEndPt.des_para.RODEND.wing_offset_style=m_iWingOffsetType;
	m_xEndPt.des_para.RODEND.wing_offset_dist=m_fWingOffsetDist;
	m_xEndPt.des_para.RODEND.len_offset_dist=200;
	m_xVector.norm_style=2;			//两杆件交叉
	m_xVector.hVicePart=hPart;
	if(m_pCrossRod)
		m_xVector.hCrossPart = m_pCrossRod->handle;
	if(pPart&&pPart->IsAngle())
	{
		if(m_iOffsetWing==0)	//X肢为当前工作肢
			m_xVector.nearVector=((CLDSLineAngle*)pPart)->get_norm_x_wing();
		else
			m_xVector.nearVector=((CLDSLineAngle*)pPart)->get_norm_y_wing();
	}
	m_propStartPtList.CopyFrom(&m_xStartPt);
	m_propEndPtList.CopyFrom(&m_xEndPt);
	m_propVectorList.CopyFrom(&m_xVector);
}

void CInputHuoQuLineDlg::UpdateUIStatus()
{
	long hPart;
	sscanf(m_sJgHandle,"%X",&hPart);
	CLDSPart* pPart=console.FromPartHandle(hPart);
	if(pPart==NULL || pPart->GetClassTypeId()!=CLS_LINEANGLE ||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		((CEdit*)GetDlgItem(IDC_E_JG_HANDLE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_E_WING_OFFSET))->SetReadOnly(TRUE);
	}
	else
	{
		((CEdit*)GetDlgItem(IDC_E_JG_HANDLE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_E_WING_OFFSET))->SetReadOnly(FALSE);
	}
	if(m_iWingOffsetType==4)
		((CEdit*)GetDlgItem(IDC_E_WING_OFFSET))->SetReadOnly(FALSE);
	else
		((CEdit*)GetDlgItem(IDC_E_WING_OFFSET))->SetReadOnly(TRUE);
}

