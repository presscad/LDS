//<LOCALE_TRANSLATE BY hxr /> 2015-1-10
// DesignVectorPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "DesignVectorPropDlg.h"
#include "XhCharString.h"
#include "env_def.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignVectorPropDlg dialog

CDesignVectorPropDlg::CDesignVectorPropDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignVectorPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignVectorPropDlg)
	m_iNormCalStyle=0;
	m_fVectorX = 0.0;
	m_fVectorY = 0.0;
	m_fVectorZ = 0.0;
	//}}AFX_DATA_INIT
	m_hCurEditPart=0;
	m_bSpecAngleWingNormStyle=FALSE;
}


void CDesignVectorPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignVectorPropDlg)
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_CBIndex(pDX, IDC_CMB_NORM_CAL_STYLE, m_iNormCalStyle);
	DDX_Text(pDX, IDC_E_VECTOR_X, m_fVectorX);
	DDX_Text(pDX, IDC_E_VECTOR_Y, m_fVectorY);
	DDX_Text(pDX, IDC_E_VECTOR_Z, m_fVectorZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignVectorPropDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignVectorPropDlg)
	ON_CBN_SELCHANGE(IDC_CMB_NORM_CAL_STYLE, OnSelchangeCmbNormCalStyle)
	ON_BN_CLICKED(IDC_BN_COPY, OnCopyVector)
	ON_BN_CLICKED(IDC_BN_PASTE, OnPasteVector)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignVectorPropDlg message handlers

const DWORD HASHTABLESIZE = 500;
CStringKeyHashTable<long> CDesignVectorPropDlg::propHashtable;
void CDesignVectorPropDlg::InitPropHashtable()
{
	int id = 1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	//常规
	propHashtable.SetValueAt("hVicePart",id++);
	propHashtable.SetValueAt("hCrossPart",id++);
	propHashtable.SetValueAt("norm_wing",id++);
	propHashtable.SetValueAt("direction",id++);
	propHashtable.SetValueAt("rot_ang",id++);
	propHashtable.SetValueAt("VectorType",id++);
	propHashtable.SetValueAt("nearVectorType",id++);
	propHashtable.SetValueAt("nearVector.x",id++);
	propHashtable.SetValueAt("nearVector.y",id++);
	propHashtable.SetValueAt("nearVector.z",id++);
}

void CDesignVectorPropDlg::OnSelchangeCmbNormCalStyle() 
{
	UpdateData();
	m_xVector.norm_style=m_iNormCalStyle;
	m_xVector.UpdateVector(console.GetActiveModel());
	m_fVectorX=m_xVector.vector.x;
	m_fVectorY=m_xVector.vector.y;
	m_fVectorZ=m_xVector.vector.z;
	UpdateData(FALSE);
	int vectorType = CStdVector::GetVectorType(m_xVector.vector);
	((CEdit*)GetDlgItem(IDC_E_VECTOR_X))->SetReadOnly(m_iNormCalStyle!=0||vectorType!=0);
	((CEdit*)GetDlgItem(IDC_E_VECTOR_Y))->SetReadOnly(m_iNormCalStyle!=0||vectorType!=0);
	((CEdit*)GetDlgItem(IDC_E_VECTOR_Z))->SetReadOnly(m_iNormCalStyle!=0||vectorType!=0);
	GetDlgItem(IDC_BN_PASTE)->EnableWindow(m_iNormCalStyle==0&&vectorType==0);
	UpdateDataPropList();
}

static BOOL FireSelChange(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDesignVectorPropDlg *pDesignVectorPropDlg=(CDesignVectorPropDlg*)pPropList->GetParent();
	ASSERT(pDesignVectorPropDlg);
	if(CDesignVectorPropDlg::GetPropID("direction")==pItem->m_idProp)
		pItem->m_lpNodeInfo->m_strPropHelp = pDesignVectorPropDlg->m_xVector.GetDirectionHelpStr();
	if(pDesignVectorPropDlg->m_hCurEditPart<=0x20)
		return FALSE;	//未指定当前编辑构件时不启用选择句柄的同时选中响应构件的功能 wht 11-05-25
	//选中定位构件
	long hCurPart=0;
	if(CDesignVectorPropDlg::GetPropID("hVicePart")==pItem->m_idProp)
		hCurPart=pDesignVectorPropDlg->m_xVector.hVicePart;
	else if(CDesignVectorPropDlg::GetPropID("hCrossPart")==pItem->m_idProp)
		hCurPart=pDesignVectorPropDlg->m_xVector.hCrossPart;
	if(hCurPart<=0x20)
		hCurPart=pDesignVectorPropDlg->m_hCurEditPart;
	long *id_arr=NULL;
	int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	//需要设置选中状态的构件已处于选中状态时不用重新设置选中状态
	if(!(n==1&&id_arr[0]==hCurPart)&&hCurPart>0x20)
	{	//
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(hCurPart);
	}
	return TRUE;
}

static BOOL ModifyDesignVectorProperty(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CDesignVectorPropDlg *pDesignVectorPropDlg=(CDesignVectorPropDlg*)pPropList->GetParent();
	if(CDesignVectorPropDlg::GetPropID("hVicePart")==pItem->m_idProp)//基准杆件句柄
		pDesignVectorPropDlg->m_xVector.hVicePart=HexStrToLong(valueStr);
	else if(CDesignVectorPropDlg::GetPropID("hCrossPart")==pItem->m_idProp)
		pDesignVectorPropDlg->m_xVector.hCrossPart=HexStrToLong(valueStr);
	else if(CDesignVectorPropDlg::GetPropID("norm_wing")==pItem->m_idProp)
		pDesignVectorPropDlg->m_xVector.norm_wing=valueStr[0]-'0';
	else if(CDesignVectorPropDlg::GetPropID("direction")==pItem->m_idProp)
	{
		pDesignVectorPropDlg->m_xVector.direction=valueStr[0]-'0';
		if(pDesignVectorPropDlg->m_xVector.norm_style==5)
		{
			CSuperSmartPtr<CLDSPart>pDatumPart=console.FromPartHandle(pDesignVectorPropDlg->m_xVector.hVicePart);
			if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			{
				if(pDesignVectorPropDlg->m_xVector.direction<2||pDesignVectorPropDlg->m_xVector.direction>3)
					pDesignVectorPropDlg->m_propList.DeleteItemByPropId(CDesignVectorPropDlg::GetPropID("rot_ang"));
				else
				{
					int idProp=CDesignVectorPropDlg::GetPropID("rot_ang");
					if(pDesignVectorPropDlg->m_propList.FindItemByPropId(idProp,NULL)==NULL)
					{
						CItemInfo* lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_COMBO;
						lpInfo->m_cmbType=CDT_EDIT;
						lpInfo->m_cmbItems = "0|90|180|270";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Rotate Angle";
						if(pDesignVectorPropDlg->m_xVector.direction==2)
							lpInfo->m_strPropHelp="Rotate angle since the plane normal of Tube datum line around the axis(°)";
						else 
							lpInfo->m_strPropHelp="Rotate angle since the plane normal of Tube welding line around the axis(°)";
#else 
						lpInfo->m_strPropName = "旋转角度";
						if(pDesignVectorPropDlg->m_xVector.direction==2)
							lpInfo->m_strPropHelp="自钢管基准线平面法向绕轴线的旋转角度(°)";
						else 
							lpInfo->m_strPropHelp="自钢管焊道线平面法向绕轴线的旋转角度(°)";
#endif
						CPropTreeItem* pSonItem = pDesignVectorPropDlg->m_propList.InsertItem(pItem,lpInfo, -1,TRUE);
						pSonItem->m_idProp = idProp;
						char item_text[100]="";
						if(pDesignVectorPropDlg->GetPropValueStr(pSonItem->m_idProp,item_text)>0)
							pSonItem->m_lpNodeInfo->m_strPropValue = item_text;
					}
				}
			}
			else if(pDatumPart.IsHasPtr()&&pDatumPart->IsLinePart()&&pDesignVectorPropDlg->m_xVector.direction<2)
			{
				GEPOINT linevec=pDatumPart.LinePartPointer()->End()-pDatumPart.LinePartPointer()->Start();
				if(pDesignVectorPropDlg->m_xVector.direction==1)	//始->终
					linevec*=-1.0;
				normalize(linevec);
				pDesignVectorPropDlg->m_fVectorX=linevec.x;
				pDesignVectorPropDlg->m_fVectorY=linevec.y;
				pDesignVectorPropDlg->m_fVectorZ=linevec.z;
				pDesignVectorPropDlg->UpdateData(FALSE);
			}
		}
	}
	else if(CDesignVectorPropDlg::GetPropID("rot_ang")==pItem->m_idProp)
	{
		pDesignVectorPropDlg->m_xVector.rot_ang = atof(valueStr);
	}
	else if(CDesignVectorPropDlg::GetPropID("VectorType")==pItem->m_idProp)
	{
		if(valueStr[0]=='0')	//自定义方向
		{
			((CEdit*)pDesignVectorPropDlg->GetDlgItem(IDC_E_VECTOR_X))->SetReadOnly(pDesignVectorPropDlg->m_iNormCalStyle!=0);
			((CEdit*)pDesignVectorPropDlg->GetDlgItem(IDC_E_VECTOR_Y))->SetReadOnly(pDesignVectorPropDlg->m_iNormCalStyle!=0);
			((CEdit*)pDesignVectorPropDlg->GetDlgItem(IDC_E_VECTOR_Z))->SetReadOnly(pDesignVectorPropDlg->m_iNormCalStyle!=0);
			pDesignVectorPropDlg->GetDlgItem(IDC_BN_PASTE)->EnableWindow(TRUE);
		}
		else
		{
			((CEdit*)pDesignVectorPropDlg->GetDlgItem(IDC_E_VECTOR_X))->SetReadOnly(TRUE);
			((CEdit*)pDesignVectorPropDlg->GetDlgItem(IDC_E_VECTOR_Y))->SetReadOnly(TRUE);
			((CEdit*)pDesignVectorPropDlg->GetDlgItem(IDC_E_VECTOR_Z))->SetReadOnly(TRUE);
			pDesignVectorPropDlg->GetDlgItem(IDC_BN_PASTE)->EnableWindow(FALSE);
			f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
			pDesignVectorPropDlg->m_xVector.vector=vector;
			CString sText;
			sText.Format("%.f",vector.x);
			pDesignVectorPropDlg->GetDlgItem(IDC_E_VECTOR_X)->SetWindowText(sText);
			sText.Format("%.f",vector.y);
			pDesignVectorPropDlg->GetDlgItem(IDC_E_VECTOR_Y)->SetWindowText(sText);
			sText.Format("%.f",vector.z);
			pDesignVectorPropDlg->GetDlgItem(IDC_E_VECTOR_Z)->SetWindowText(sText);
		}
	}
	else if(CDesignVectorPropDlg::GetPropID("nearVectorType")==pItem->m_idProp)
	{
		if(valueStr[0]-'0'==0)	//自定义方向
		{
			CXhChar100 sText;
			CItemInfo *lpInfo;
			CPropTreeItem *pSonItem,*pFindItem=pDesignVectorPropDlg->m_propList.FindItemByPropId(CDesignVectorPropDlg::GetPropID("nearVector.x"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "X";
				lpInfo->m_strPropHelp = "X coordinate";
#else 
				lpInfo->m_strPropName = "X分量";
				lpInfo->m_strPropHelp = "X分量";
#endif
				pSonItem = pDesignVectorPropDlg->m_propList.InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CDesignVectorPropDlg::GetPropID("nearVector.x");
				if(pDesignVectorPropDlg->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			
			pFindItem=pDesignVectorPropDlg->m_propList.FindItemByPropId(CDesignVectorPropDlg::GetPropID("nearVector.y"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Y";
				lpInfo->m_strPropHelp = "Y coordinate";
#else 
				lpInfo->m_strPropName = "Y分量";
				lpInfo->m_strPropHelp = "Y分量";
#endif
				pSonItem = pDesignVectorPropDlg->m_propList.InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CDesignVectorPropDlg::GetPropID("nearVector.y");
				if(pDesignVectorPropDlg->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			
			pFindItem=pDesignVectorPropDlg->m_propList.FindItemByPropId(CDesignVectorPropDlg::GetPropID("nearVector.z"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Z";
				lpInfo->m_strPropHelp = "Z coordinate";
#else 
				lpInfo->m_strPropName = "Z分量";
				lpInfo->m_strPropHelp = "Z分量";
#endif
				pSonItem = pDesignVectorPropDlg->m_propList.InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CDesignVectorPropDlg::GetPropID("nearVector.z");
				if(pDesignVectorPropDlg->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
		{
			pDesignVectorPropDlg->m_propList.DeleteItemByPropId(CDesignVectorPropDlg::GetPropID("nearVector.x"));
			pDesignVectorPropDlg->m_propList.DeleteItemByPropId(CDesignVectorPropDlg::GetPropID("nearVector.y"));
			pDesignVectorPropDlg->m_propList.DeleteItemByPropId(CDesignVectorPropDlg::GetPropID("nearVector.z"));
			pDesignVectorPropDlg->m_xVector.nearVector=CStdVector::GetVector(valueStr[0]-'0');
		}
	}
	else if(CDesignVectorPropDlg::GetPropID("nearVector.x")==pItem->m_idProp)
		pDesignVectorPropDlg->m_xVector.nearVector.x=atof(valueStr);
	else if(CDesignVectorPropDlg::GetPropID("nearVector.y")==pItem->m_idProp)
		pDesignVectorPropDlg->m_xVector.nearVector.y=atof(valueStr);
	else if(CDesignVectorPropDlg::GetPropID("nearVector.z")==pItem->m_idProp)
		pDesignVectorPropDlg->m_xVector.nearVector.z=atof(valueStr);
	else
		return FALSE;
	return TRUE;
}
int CDesignVectorPropDlg::GetPropValueStr(long id,char* valueStr)
{
	long handle=0;
	CXhChar100 sText;
	if(GetPropID("hVicePart")==id)
		sText.Printf("0X%X",m_xVector.hVicePart);
	else if(GetPropID("hCrossPart")==id)
		sText.Printf("0X%X",m_xVector.hCrossPart);
	else if(GetPropID("norm_wing")==id)
	{
		if(m_xVector.norm_wing==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("%s","0.X Leg Normal");
		else
			sText.Printf("%s","1.Y Leg Normal");
#else 
			sText.Printf("%s","0.X肢法线");
		else
			sText.Printf("%s","1.Y肢法线");
#endif
	}
	else if(GetPropID("direction")==id)
		sText.Printf("%s",m_xVector.GetDirectionDes());
	else if(GetPropID("rot_ang")==id)
	{
		sText.Printf("%f",m_xVector.rot_ang);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("VectorType")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(m_xVector.vector));
	else if(GetPropID("nearVectorType")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(m_xVector.nearVector));
	else if(GetPropID("nearVector.x")==id)
	{
		sText.Printf("%lf",m_xVector.nearVector.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearVector.y")==id)
	{
		sText.Printf("%lf",m_xVector.nearVector.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearVector.z")==id)
	{
		sText.Printf("%lf",m_xVector.nearVector.z);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}

long CDesignVectorPropDlg::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}
/*void UpdateVectorPropByNormStyle(CPropertyList *pPropList,CPropTreeItem *pParentItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem,*pSonPropItem;
	int idProp;
	CItemInfo *lpInfo;
	if(pFirstLineAngle==NULL||pParentItem==NULL)
		return;
}*/
void CDesignVectorPropDlg::UpdateDataPropList()
{
	m_propList.DeleteItemByPropId(GetPropID("hVicePart"));
	m_propList.DeleteItemByPropId(GetPropID("hCrossPart"));
	m_propList.DeleteItemByPropId(GetPropID("norm_wing"));
	m_propList.DeleteItemByPropId(GetPropID("direction"));
	m_propList.DeleteItemByPropId(GetPropID("VectorType"));
	m_propList.DeleteItemByPropId(GetPropID("nearVectorType"));
	m_propList.DeleteItemByPropId(GetPropID("nearVector.x"));
	m_propList.DeleteItemByPropId(GetPropID("nearVector.y"));
	m_propList.DeleteItemByPropId(GetPropID("nearVector.z"));
	CPropTreeItem *pPropItem,*pSonItem,*pRoot=m_propList.GetRootItem();
	char sText[MAX_TEMP_CHAR_100+1]="";
	CItemInfo* lpInfo=NULL;
	//指定法线
	if(m_xVector.norm_style==0)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specify";
		lpInfo->m_strPropHelp = "Specify vector";
#else 
		lpInfo->m_strPropName = "指定方向";
		lpInfo->m_strPropHelp = "指定方向";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("VectorType");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	//单角钢肢法线
	else if(m_xVector.norm_style==1)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType  = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Angle";
		lpInfo->m_strPropHelp = "Handle of datum angle";
#else 
		lpInfo->m_strPropName = "基准角钢";
		lpInfo->m_strPropHelp = "基准角钢句柄";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems ="0.X Leg Normal|1.Y Leg Normal";
		lpInfo->m_strPropName = "Datum Leg";
		lpInfo->m_strPropHelp = "Datum leg";
#else 
		lpInfo->m_cmbItems ="0.X肢法线|1.Y肢法线";
		lpInfo->m_strPropName = "基准肢";
		lpInfo->m_strPropHelp = "基准肢";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("norm_wing");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = m_xVector.GetDirectionCmbItems();
		lpInfo->m_strPropHelp = m_xVector.GetDirectionHelpStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Direction";
#else 
		lpInfo->m_strPropName = "朝向";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("direction");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else if(m_xVector.norm_style==2)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType  = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Rod";
		lpInfo->m_strPropHelp = "Handle of datum rod";
#else 
		lpInfo->m_strPropName = "基准杆件";
		lpInfo->m_strPropHelp = "基准杆件句柄";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		if(m_bSpecAngleWingNormStyle)
		{
			m_xVector.hVicePart=m_hCurEditPart;
			pPropItem->SetReadOnly();
		}
		pPropItem->m_idProp =GetPropID("hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType  = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Cross Rod";
		lpInfo->m_strPropHelp = "Handle of crossing reference rod";
#else 
		lpInfo->m_strPropName = "交叉参照杆件";
		lpInfo->m_strPropHelp = "交叉参照杆件句柄";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo,-1);
		pPropItem->m_idProp =GetPropID("hCrossPart");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Near Vector";
		lpInfo->m_strPropHelp = "Near vector used to differentiate the calculation vector is the positive of negative normal direction of cross plane";
#else 
		lpInfo->m_strPropName = "近似方向";
		lpInfo->m_strPropHelp = "近似方向用于区别计算方向为交叉面的正法线还是负法线方向";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("nearVectorType");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		int nearVectorType=CStdVector::GetVectorType(m_xVector.nearVector);
		if(nearVectorType==0)	//自定义
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X";
			lpInfo->m_strPropHelp = "X coordinate";
#else 
			lpInfo->m_strPropName = "X分量";
			lpInfo->m_strPropHelp = "X分量";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonItem->m_idProp =GetPropID("nearVector.x");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y";
			lpInfo->m_strPropHelp = "Y coordinate";
#else 
			lpInfo->m_strPropName = "Y分量";
			lpInfo->m_strPropHelp = "Y分量";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonItem->m_idProp =GetPropID("nearVector.y");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z";
			lpInfo->m_strPropHelp = "Z coordinate";
#else 
			lpInfo->m_strPropName = "Z分量";
			lpInfo->m_strPropHelp = "Z分量";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonItem->m_idProp =GetPropID("nearVector.z");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
	else if(m_xVector.norm_style==3)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType  = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Part";
		lpInfo->m_strPropHelp = "Datum part";
#else 
		lpInfo->m_strPropName = "基准构件";
		lpInfo->m_strPropHelp = "基准构件";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = m_xVector.GetDirectionCmbItems();
		lpInfo->m_strPropHelp = m_xVector.GetDirectionHelpStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Direction";
#else 
		lpInfo->m_strPropName = "朝向";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("direction");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else if(m_xVector.norm_style==4)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType  = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Part";
		lpInfo->m_strPropHelp = "Datum part";
#else 
		lpInfo->m_strPropName = "基准构件";
		lpInfo->m_strPropHelp = "基准构件";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Relative Vector";
		lpInfo->m_strPropHelp = "Relative vector under datum part's assemble CS";
#else 
		lpInfo->m_strPropName = "相对方向";
		lpInfo->m_strPropHelp = "基准构件装配坐标系下的相对方向";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("nearVectorType");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		int nearVectorType=CStdVector::GetVectorType(m_xVector.nearVector);
		if(nearVectorType==0)	//自定义
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X";
			lpInfo->m_strPropHelp = "X coordinate";
#else 
			lpInfo->m_strPropName = "X分量";
			lpInfo->m_strPropHelp = "X分量";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonItem->m_idProp =GetPropID("nearVector.x");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y";
			lpInfo->m_strPropHelp = "Y coordinate";
#else 
			lpInfo->m_strPropName = "Y分量";
			lpInfo->m_strPropHelp = "Y分量";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonItem->m_idProp =GetPropID("nearVector.y");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z";
			lpInfo->m_strPropHelp = "Z coordinate";
#else 
			lpInfo->m_strPropName = "Z分量";
			lpInfo->m_strPropHelp = "Z分量";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonItem->m_idProp =GetPropID("nearVector.z");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
	else if(m_xVector.norm_style==5)
	{	//5.基准杆件上的特征方向
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType  = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Rod";
		lpInfo->m_strPropHelp = "Handle of datum rod";
#else 
		lpInfo->m_strPropName = "基准杆件";
		lpInfo->m_strPropHelp = "基准杆件句柄";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("hVicePart");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		
		//方向类型
		bool bNeedRotAnglePara=false;
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_xVector.hVicePart,CLS_LINEPART);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Direction";
#else 
		lpInfo->m_strPropName = "方向类型";
#endif
		lpInfo->m_strPropHelp = m_xVector.GetDirectionHelpStr();
		lpInfo->m_cmbItems =m_xVector.GetDirectionCmbItems();
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("direction");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		CSuperSmartPtr<CLDSPart>pDatumPart=console.FromPartHandle(m_xVector.hVicePart);
		if((pDatumPart.IsHasPtr()&&pDatumPart->IsTube()))
		{
			if(m_xVector.direction==2||m_xVector.direction==3)
				bNeedRotAnglePara=true;
		}
		if(bNeedRotAnglePara)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=CDT_EDIT;
			lpInfo->m_cmbItems = "0|90|180|270";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rotate Angle";
			if(m_xVector.direction==2)
				lpInfo->m_strPropHelp="Rotate angle since the plane normal of Tube datum line around the axis °";
			else 
				lpInfo->m_strPropHelp="Rotate angle since the plane normal of Tube welding line around the axis °";
#else 
			lpInfo->m_strPropName = "旋转角度";
			if(m_xVector.direction==2)
				lpInfo->m_strPropHelp="自钢管基准线平面法向绕轴线的旋转角度°";
			else 
				lpInfo->m_strPropHelp="自钢管焊道线平面法向绕轴线的旋转角度°";
#endif
			pSonItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonItem->m_idProp = GetPropID("rot_ang");
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
	//单角钢肢法线
	else if(m_xVector.norm_style==6)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType  = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Angle";
		lpInfo->m_strPropHelp = "Handle of datum angle";
#else 
		lpInfo->m_strPropName = "基准角钢";
		lpInfo->m_strPropHelp = "基准角钢句柄";
#endif
		pPropItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("hVicePart");
		pPropItem->m_lpNodeInfo->m_strPropValue.Format("0X%X",m_hCurEditPart);
		pPropItem->SetReadOnly();
	}
	m_propList.Redraw();
}
static BOOL ButtonClickDesignVector(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CDesignVectorPropDlg *pVecDlg=(CDesignVectorPropDlg*)pPropList->GetParent();
	if(CDesignVectorPropDlg::GetPropID("hVicePart")==pItem->m_idProp
		||CDesignVectorPropDlg::GetPropID("hCrossPart")==pItem->m_idProp)
	{
		if(pVecDlg->m_xVector.norm_style==1)
			pVecDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);
		else if(pVecDlg->m_xVector.norm_style==2
				||pVecDlg->m_xVector.norm_style==5)
			pVecDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);
		else 
			pVecDlg->SelectObject(CLS_PART,pItem->m_idProp);
	}
	return TRUE;
}
BOOL CDesignVectorPropDlg::OnInitDialog() 
{
	m_iNormCalStyle=m_xVector.norm_style;
	if(m_xVector.vector.IsZero())
		m_xVector.UpdateVector(console.GetActiveModel());
	m_xVector.vector.Fillet();
	m_xVector.nearVector.Fillet();
	m_fVectorX = m_xVector.vector.x;
	m_fVectorY = m_xVector.vector.y;
	m_fVectorZ = m_xVector.vector.z;
	int vectorType=CStdVector::GetVectorType(m_xVector.vector);
	((CEdit*)GetDlgItem(IDC_E_VECTOR_X))->SetReadOnly(m_iNormCalStyle!=0||vectorType!=0);
	((CEdit*)GetDlgItem(IDC_E_VECTOR_Y))->SetReadOnly(m_iNormCalStyle!=0||vectorType!=0);
	((CEdit*)GetDlgItem(IDC_E_VECTOR_Z))->SetReadOnly(m_iNormCalStyle!=0||vectorType!=0);
	GetDlgItem(IDC_BN_PASTE)->EnableWindow(m_iNormCalStyle==0&&vectorType==0);
	CDialog::OnInitDialog();
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.5);
	m_propList.SetModifyValueFunc(ModifyDesignVectorProperty);
	m_propList.SetButtonClickFunc(ButtonClickDesignVector);
	m_propList.SetPropHelpPromptFunc(FireSelChange);
	InitPropHashtable();
	//内部启动
	if(m_bInernalStart)	
		FinishSelectObjOper();	//完成选择对象的后续操作
	if(m_bSpecAngleWingNormStyle)
	{
		CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_NORM_CAL_STYLE);
#ifdef AFX_TARG_ENU_ENGLISH
		pCMB->AddString("6.Confirm By Another Leg");
#else 
		pCMB->AddString("6.由另一肢确定");
#endif
		if(m_iNormCalStyle==6)
			pCMB->SetCurSel(m_iNormCalStyle);
	}
	UpdateDataPropList();
	if(m_bInernalStart)	
	{	//恢复选中项
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();	//属性栏获得焦点
		if(pItem)
			m_propList.SetCurSel(pItem->m_iIndex);	//设置选中项
		Invalidate(FALSE);
	}
	return TRUE;  
}

//完成选择对象的后续操作
void CDesignVectorPropDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	if(CDesignVectorPropDlg::GetPropID("hVicePart")==GetEventPropId())
	{
		CLDSPart *pPart=NULL;
		if(m_xVector.norm_style==1)
			pPart=console.FromPartHandle(m_hPickObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		else if(m_xVector.norm_style==2
				||m_xVector.norm_style==5)
			pPart=console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		else 
			pPart=console.FromPartHandle(m_hPickObj);
		if(pPart)
			m_xVector.hVicePart=pPart->handle;
	}
	else if(m_xVector.norm_style==2&&
		CDesignVectorPropDlg::GetPropID("hCrossPart")==GetEventPropId())
	{
		CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
		if(pPart)
			m_xVector.hCrossPart=pPart->handle;
	}
	if( m_xVector.norm_style!=2||(
		m_xVector.norm_style==2&&m_xVector.hVicePart>0x20&&m_xVector.hCrossPart>0x20))
	{
		m_xVector.UpdateVector(console.GetActiveModel());
		m_fVectorX=m_xVector.vector.x;
		m_fVectorY=m_xVector.vector.y;
		m_fVectorZ=m_xVector.vector.z;
		UpdateData(FALSE);
	}
}

void CDesignVectorPropDlg::OnOK() 
{
	UpdateData();
	if(m_iNormCalStyle==0)	//指定法线，点击确定时需保存法向方向 wht 11-05-31
		m_xVector.vector.Set(m_fVectorX,m_fVectorY,m_fVectorZ);
	char sError[200]="";
	if(m_iNormCalStyle!=6&&!m_xVector.AssertValid(console.GetActiveModel(),sError))
	{
		AfxMessageBox(sError);
		return;
	}
	CDialog::OnOK();
}

void CDesignVectorPropDlg::OnCopyVector() 
{
	f3dPoint point;
	UpdateData(TRUE);
	point.x = m_fVectorX;
	point.y = m_fVectorY;
	point.z = m_fVectorZ;
	WritePointToClip(point);
}

void CDesignVectorPropDlg::OnPasteVector() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fVectorX = point.x;
		m_fVectorY = point.y;
		m_fVectorZ = point.z;
		UpdateData(FALSE);
	}
}
