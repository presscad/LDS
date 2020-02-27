// DatumPointParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "DatumPointPropertyDlg.h"
#include "LmaDlg.h"
#include "XhCharString.h"
#include "env_def.h"
#include "ScrTools.h"
#include "ReportPointDlg.h"
#include "GlobalFunc.h"
//#include "DisplayPointProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDatumPointPropertyDlg dialog

int CDatumPointPropertyDlg::nScrLocationX=0;
int CDatumPointPropertyDlg::nScrLocationY=0;
CDatumPointPropertyDlg::CDatumPointPropertyDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDatumPointPropertyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDatumPointPropertyDlg)
	m_iDatumPointStyle = 0;
	m_fPointX = 0.0;
	m_fPointY = 0.0;
	m_fPointZ = 0.0;
	//}}AFX_DATA_INIT
	m_idEventProp=0;
	m_bEnableUpdateEditPoint=TRUE;
	m_pCurEditObj=NULL;
	m_bEnableWindowMoveListen=false;
}
void CDatumPointPropertyDlg::UpdateEditPoint()
{
	m_xPoint.UpdatePos(console.GetActiveModel());
	m_fPointX=m_xPoint.Position().x;
	m_fPointY=m_xPoint.Position().y;
	m_fPointZ=m_xPoint.Position().z;
	UpdateData(FALSE);
}

void CDatumPointPropertyDlg::ModifyPositionFromPropList(double coord,char cCoordType)
{
	if(toupper(cCoordType)=='X')
	{
		m_fPointX=coord;
		m_xPoint.SetPositionX(coord);
	}
	else if(toupper(cCoordType)=='Y')
	{
		m_fPointY=coord;
		m_xPoint.SetPositionY(coord);
	}
	else if(toupper(cCoordType)=='Z')
	{
		m_fPointZ=coord;
		m_xPoint.SetPositionZ(coord);
	}
	UpdateData(FALSE);
}


void CDatumPointPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDatumPointPropertyDlg)
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_CBIndex(pDX, IDC_CMB_DATUM_POINT_STYLE, m_iDatumPointStyle);
	DDX_Text(pDX, IDC_E_POINT_X, m_fPointX);
	DDX_Text(pDX, IDC_E_POINT_Y, m_fPointY);
	DDX_Text(pDX, IDC_E_POINT_Z, m_fPointZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDatumPointPropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CDatumPointPropertyDlg)
	ON_WM_MOVE()
	ON_CBN_SELCHANGE(IDC_CMB_DATUM_POINT_STYLE, OnSelchangeDatumPointStyle)
	ON_BN_CLICKED(IDC_BN_COPY_POINT, OnCopyPoint)
	ON_BN_CLICKED(IDC_BN_PASTE_POINT, OnPastePoint)
	ON_EN_CHANGE(IDC_E_POINT_X, OnEnChangePointCoord)
	ON_EN_CHANGE(IDC_E_POINT_Y, OnEnChangePointCoord)
	ON_EN_CHANGE(IDC_E_POINT_Z, OnEnChangePointCoord)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatumPointPropertyDlg message handlers
static bool ReflectPointPropertyModifyValueFunc(CNativePropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CDatumPointPropertyDlg* pDlg=(CDatumPointPropertyDlg*)pPropList->GetParent();
	CXhChar50 sText;
	if(pPropList->GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==pItem->m_idProp)
	{
		if(pPropList->GetPropValueStr(pItem->m_idProp,sText))
		{
			((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_X))->SetReadOnly(TRUE);
			((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(TRUE);
			((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(TRUE);
			if(sText[0]=='0')
				((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_X))->SetReadOnly(FALSE);
			else if(sText[0]=='1')
				((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(FALSE);
			else if(sText[0]=='2')
				((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(FALSE);
			return true;
		}
	}
	else if(pPropList->GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==pItem->m_idProp)
	{	
		if(pPropList->GetPropValueStr(pItem->m_idProp,sText))
		{
			((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_X))->SetReadOnly(TRUE);
			((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(TRUE);
			((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(TRUE);
			if(sText[0]=='0')		//0.X-Z分量(Y轴与面求交)
			{	//DATUMPLANE.fix_axis_style_x0_y1_z2=1
				((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_X))->SetReadOnly(FALSE);
				((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(FALSE);
			}
			else if(sText[0]=='1')	//1.Y-Z分量(X轴与面求交)
			{	//DATUMPLANE.fix_axis_style_x0_y1_z2=0
				((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(FALSE);
				((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(FALSE);
			}
			else if(sText[0]=='2')	//2.X-Y分量(Z轴与面求交)
			{	//DATUMPLANE.fix_axis_style_x0_y1_z2=2
				((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_X))->SetReadOnly(FALSE);
				((CEdit*)pDlg->GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(FALSE);
			}
			return true;
		}
	}
	else if(pPropList->GetPropID("pos.x")==pItem->m_idProp)
		pDlg->ModifyPositionFromPropList(atof(valueStr),'X');
	else if(pPropList->GetPropID("pos.y")==pItem->m_idProp)
		pDlg->ModifyPositionFromPropList(atof(valueStr),'Y');
	else if(pPropList->GetPropID("pos.z")==pItem->m_idProp)
		pDlg->ModifyPositionFromPropList(atof(valueStr),'Z');
	//更新编辑框X\Y\Z坐标值 
	pDlg->m_propList.CopyTo(&pDlg->m_xPoint);
	if(pDlg->m_bEnableUpdateEditPoint)
		pDlg->UpdateEditPoint();	//防止用户直接在坐标编辑框中更改坐标后来回反射，导致编辑框编辑不方便 wjh-2016.5.06
	return false;
}
static bool ReflectPointPropertyButtonClickFunc(CNativePropertyList* pPropList,CPropTreeItem* pItem)
{   
	CDatumPointPropertyDlg* pDlg=(CDatumPointPropertyDlg*)pPropList->GetParent();
	pDlg->m_xPoint.UpdatePos();
	pDlg->UpdateEditPoint();
	return TRUE;
}
static BOOL FireSelChangeItemFunc(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDatumPointPropertyDlg* pDlg=(CDatumPointPropertyDlg*)pPropList->GetParent();
	if(pDlg==NULL||pDlg->m_pCurEditObj==NULL)
		return FALSE;
	CParaPointPropertyList *pNativePropList=&pDlg->m_propList;
	long hObj=0;
	if(pItem->m_idProp==pNativePropList->GetPropID("des_para.LINEPARTNODE.hLinePart"))
		hObj=pDlg->m_xPoint.des_para.RODNODE.hRod;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(pItem->m_idProp==pNativePropList->GetPropID("des_para.RODEND.hRod"))
		hObj=pDlg->m_xPoint.des_para.RODEND.hRod;
#endif
	else if(pItem->m_idProp==pNativePropList->GetPropID("des_para.LINEPARTNODE.hNode"))
		hObj=pDlg->m_xPoint.des_para.RODNODE.hNode;
	else if(pItem->m_idProp==pNativePropList->GetPropID("des_para.AXIS_INTERS.hDatum1"))
		hObj=pDlg->m_xPoint.des_para.AXIS_INTERS.hDatum1;
	else if(pItem->m_idProp==pNativePropList->GetPropID("des_para.AXIS_INTERS.hDatum2"))
		hObj=pDlg->m_xPoint.des_para.AXIS_INTERS.hDatum2;
	else if(pItem->m_idProp==pNativePropList->GetPropID("des_para.hNode"))
		hObj=pDlg->m_xPoint.des_para.hNode;
	else if(pItem->m_idProp==pNativePropList->GetPropID("des_para.hPart"))
		hObj=pDlg->m_xPoint.des_para.hPart;
	else if(pItem->m_idProp==pNativePropList->GetPropID("des_para.COLUMN_INTERS.hLineTube"))
		hObj=pDlg->m_xPoint.des_para.COLUMN_INTERS.hLineTube;
	else if(pItem->m_idProp==pNativePropList->GetPropID("des_para.COLUMN_INTERS.SectCenter.hNode"))
		hObj=pDlg->m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode;
	else if(pItem->m_idProp==pNativePropList->GetPropID("des_para.SCALE_NODE.hLinePart"))
		hObj=pDlg->m_xPoint.des_para.SCALE_NODE.hLinePart;
	else if(pItem->m_idProp==pNativePropList->GetPropID("des_para.SCALE_NODE.hStartNode"))
		hObj=pDlg->m_xPoint.des_para.SCALE_NODE.hStartNode;
	else if(pItem->m_idProp==pNativePropList->GetPropID("des_para.SCALE_NODE.hEndNode"))
		hObj=pDlg->m_xPoint.des_para.SCALE_NODE.hEndNode;
	long *id_arr=NULL;
	int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	//需要设置选中状态的构件已处于选中状态时不用重新设置选中状态
	CLDSDbObject *pDbObj=(CLDSDbObject*)Ta.FromHandle(hObj,TRUE);
	if(pDbObj==NULL)
		pDbObj=pDlg->m_pCurEditObj;
	if(!(n==1&&id_arr[0]==hObj)&&pDbObj)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pDbObj->handle);
	}
	return TRUE;
}

BOOL CDatumPointPropertyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_propList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.InitPropHashtable();
	if(!m_bInernalStart)
		m_propList.CopyFrom(&m_xPoint);
	else if(m_bInernalStart)
	{	//内部启动时也需要拷贝, 否则不更新界面显示　wjh-2015.1.7
		m_propList.FinishSelectObjOper(m_hPickObj,GetEventPropId());
		m_propList.CopyTo(&m_xPoint);
	}
	UpdateEditPoint();
	//
	m_iDatumPointStyle=m_xPoint.datum_pos_style;
	((CEdit*)GetDlgItem(IDC_E_POINT_X))->SetReadOnly(m_iDatumPointStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(m_iDatumPointStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(m_iDatumPointStyle!=0);
	if(m_iDatumPointStyle==5)
	{
		if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			((CEdit*)GetDlgItem(IDC_E_POINT_X))->SetReadOnly(FALSE);
		else if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			((CEdit*)GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(FALSE);
		else if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2)
			((CEdit*)GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(FALSE);
	}
	else if(m_iDatumPointStyle==6)
	{
		if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
		{
			((CEdit*)GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(FALSE);
			((CEdit*)GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(FALSE);
		}
		else if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
		{
			((CEdit*)GetDlgItem(IDC_E_POINT_X))->SetReadOnly(FALSE);
			((CEdit*)GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(FALSE);
		}
		else if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==2)
		{
			((CEdit*)GetDlgItem(IDC_E_POINT_X))->SetReadOnly(FALSE);
			((CEdit*)GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(FALSE);
		}
	}
	GetDlgItem(IDC_BN_PASTE_POINT)->EnableWindow(m_iDatumPointStyle==0);
	//显示系统设置
	m_propList.DisplayPropertyList();
	m_propList.SetReflectModifyValueFunc(ReflectPointPropertyModifyValueFunc);
	m_propList.SetReflectButtonClickFunc(ReflectPointPropertyButtonClickFunc);
	m_propList.SetPropHelpPromptFunc(FireSelChangeItemFunc);
	if(m_bInernalStart)	
	{	//恢复选中项
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();
		if(pItem)
			m_propList.SetCurSel(pItem->m_iIndex);	//设置选中项	
	}
	UpdateData(FALSE);
	//移动窗口到合适位置
	CRect rect;
	CWnd::GetWindowRect(rect);
	int width = rect.Width();
	int height=rect.Height();
	rect.left=nScrLocationX;
	rect.top=nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect, TRUE);
	m_bEnableWindowMoveListen=true;
	return TRUE;  
}
void CDatumPointPropertyDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
void InitDatumPointDesPara(CLDSPoint *pPoint);
void CDatumPointPropertyDlg::OnSelchangeDatumPointStyle() 
{
	UpdateData();
	int liOldDatumPointStyle=m_iDatumPointStyle;
	m_propList.CopyTo(&m_xPoint);
	if(liOldDatumPointStyle==0||liOldDatumPointStyle==5||liOldDatumPointStyle==6)
		m_xPoint.datum_pos.Set(m_fPointX,m_fPointY,m_fPointZ);
	m_xPoint.datum_pos_style=m_iDatumPointStyle;
	InitDatumPointDesPara(&m_xPoint);
	UpdateEditPoint();
	((CEdit*)GetDlgItem(IDC_E_POINT_X))->SetReadOnly(m_iDatumPointStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(m_iDatumPointStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(m_iDatumPointStyle!=0);
	if(m_iDatumPointStyle==5)
	{
		if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			((CEdit*)GetDlgItem(IDC_E_POINT_X))->SetReadOnly(FALSE);
		else if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			((CEdit*)GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(FALSE);
		else if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2)
			((CEdit*)GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(FALSE);
	}
	else if(m_iDatumPointStyle==6)
	{
		if(m_xPoint.datum_pos_style==6&&m_xPoint.des_para.DATUMPLANE.hDatumPlane==0)	//基准面上指定坐标分量点，默认为指定X-Z坐标
			m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=1;
		if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
		{
			((CEdit*)GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(FALSE);
			((CEdit*)GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(FALSE);
		}
		else if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
		{
			((CEdit*)GetDlgItem(IDC_E_POINT_X))->SetReadOnly(FALSE);
			((CEdit*)GetDlgItem(IDC_E_POINT_Z))->SetReadOnly(FALSE);
		}
		else if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==2)
		{
			((CEdit*)GetDlgItem(IDC_E_POINT_X))->SetReadOnly(FALSE);
			((CEdit*)GetDlgItem(IDC_E_POINT_Y))->SetReadOnly(FALSE);
		}
	}
	GetDlgItem(IDC_BN_PASTE_POINT)->EnableWindow(m_iDatumPointStyle==0);
	//显示系统设置
	m_propList.CopyFrom(&m_xPoint);
	m_propList.UpdateDatumPointParaProperty();
	UpdateData(FALSE);
}

void CDatumPointPropertyDlg::OnOK() 
{
	UpdateData();
	//手动输入基准点，点击确定时需保存基准点坐标 wht 11-05-31
	//基准线或基准面定位 wjh 12-7-15
	m_propList.CopyTo(&m_xPoint);
	if(m_iDatumPointStyle==0||m_iDatumPointStyle==5||m_iDatumPointStyle==6)
	{
		m_xPoint.datum_pos.Set(m_fPointX,m_fPointY,m_fPointZ);
		//m_xPoint.pos.Set(m_fPointX,m_fPointY,m_fPointZ);
	}
	char sError[200]="";
	if(!m_xPoint.AssertValid(console.GetActiveModel(),sError))
	{
		AfxMessageBox(sError);
		return;
	}
	CDialog::OnOK();
}

void CDatumPointPropertyDlg::OnCopyPoint() 
{
	f3dPoint point;
	UpdateData();
	point.x=m_fPointX;
	point.y=m_fPointY;
	point.z=m_fPointZ;
	WritePointToClip(point);
}

void CDatumPointPropertyDlg::OnPastePoint() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fPointX=point.x;
		m_fPointY=point.y;
		m_fPointZ=point.z;
		UpdateData(FALSE);
	}
}

void CDatumPointPropertyDlg::OnEnChangePointCoord()
{
	CString sTextX,sTextY,sTextZ;
	GetDlgItem(IDC_E_POINT_X)->GetWindowText(sTextX);
	GetDlgItem(IDC_E_POINT_Y)->GetWindowText(sTextY);
	GetDlgItem(IDC_E_POINT_Z)->GetWindowText(sTextZ);
	if( (sTextX.CompareNoCase("-")==0&&sTextX.GetLength()==1)||
		(sTextY.CompareNoCase("-")==0&&sTextY.GetLength()==1)||
		(sTextZ.CompareNoCase("-")==0&&sTextZ.GetLength()==1))
		return;
	UpdateData();
	//更新m_propList.m_xPoint
	CPropTreeItem* pItem=NULL;
	CString sCoordX,sCoordY,sCoordZ;
	sCoordX.Format("%g",m_fPointX);
	sCoordY.Format("%g",m_fPointY);
	sCoordZ.Format("%g",m_fPointZ);
	m_bEnableUpdateEditPoint=FALSE;
	pItem=m_propList.FindItemByPropId(m_propList.GetPropID("pos.x"),NULL);
	if(pItem&&m_propList.ModifyValueFunc(pItem,sCoordX,NULL))
		m_propList.SetItemPropValue(pItem->m_idProp,sCoordX);
	pItem=m_propList.FindItemByPropId(m_propList.GetPropID("pos.y"),NULL);
	if(pItem&&m_propList.ModifyValueFunc(pItem,sCoordY,NULL))
		m_propList.SetItemPropValue(pItem->m_idProp,sCoordY);
	pItem=m_propList.FindItemByPropId(m_propList.GetPropID("pos.z"),NULL);
	if(pItem&&m_propList.ModifyValueFunc(pItem,sCoordZ,NULL))
		m_propList.SetItemPropValue(pItem->m_idProp,sCoordZ);
	m_bEnableUpdateEditPoint=TRUE;
}

BOOL CDatumPointPropertyDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	PROPLIST_ITEM* pPropHelpItem=NULL;
	CPropTreeItem* pPropItem=m_propList.GetSelectedItem();
	if(pPropItem)
		pPropHelpItem=m_propList.GetPropListItemByPropId(pPropItem->m_idProp);
	if(pPropHelpItem==NULL)
		pPropHelpItem=theApp.hashDialogHelpItemById.GetValue(m_nIDHelp);
	CXhChar200 helpStr=theApp.m_pszHelpFilePath;
	if(pPropHelpItem!=NULL&&pPropHelpItem->Url.GetLength()>0)
		helpStr.Printf("%s::/%s",theApp.m_pszHelpFilePath,(char*)pPropHelpItem->Url);
	else
	{
		//sprintf(helpStr,"%s::/命令参考/属性栏.htm",theApp.m_pszHelpFilePath);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Lacking of the help top of this property,please contact the software vendor directly!");
#else
		AfxMessageBox("暂缺少该属性的帮助主题，请直接与软件供应商联系！");
#endif
	}
	HtmlHelp((DWORD_PTR)(char*)helpStr,HH_DISPLAY_TOPIC);
	return TRUE;//CCallBackDialog::OnHelpInfo(pHelpInfo);
}
