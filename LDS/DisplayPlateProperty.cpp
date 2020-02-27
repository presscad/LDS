//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "TowerPropertyDlg.h"
#include "CfgPartNoDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "LmaDlg.h"
#include "Query.h"
#include "LinkLinePartDlg.h"
#include "AddNewBoltDlg.h"
#include "PlateFixedVertexDlg.h"
#include "PlateLsRefDlg.h"
#include "PlateDesignParamDlg.h"
#include "AddNewElbowPlateDlg.h"
#include "DatumPointDlg.h"
#include "DesignLjParaDlg.h"
#include "RelativeObjectDlg.h"
#include "dlg.h"
#include "PartUcsDlg.h"
#include "ModifyRelativeObjPartNoDlg.h"
#include "FaceOffsetDlg.h"
#include "ParamPlateParamListDlg.h"
#include "DefHuoquFaceDlg.h"
#include "EditPlateDatumFaceDlg.h"
#include "XhCharString.h"
#include "CutAngleDlg.h"
#include "BoltPropDlg.h"
#include "PartWorkPlaneDlg.h"
#include "DatumPointPropertyDlg.h"
#include "DesignVectorPropDlg.h"
#include "DesignRibbedPlateDlg.h"
#include "DesignCircularPlateDlg.h"
#include "DesignShadowDlg.h"
#include "LogFile.h"
#include "InputHuoQuLineDlg.h"
#include "DrawingToolKits.h"
#include "btc.h"
#include "DesLineAngleRibPlateDlg.h"
#include "DbObjPropId.h"
#include "PropertyListOper.h"
#include "SnapTypeVerify.h"
#include "DesignJdb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//from DisplayPropertyPage.cpp
BOOL SetPartMutiObjsProp(CLDSObject *pObj,char *propName,long idClassType=0);
void SetPropItemReadOnly(CLDSDbObject *pObj,char *propName,BOOL bReadOnly);

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL ModifyPlateProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr);
void Update_Plate_Norm_CalStyle_TreeItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,BOOL bFirst)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPlate *pFirstPlate = (CLDSPlate*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem,*pFindItem,*pItem;
	CItemInfo *lpInfo;
	long idProp=0;
	if (pFirstPlate==NULL||pParentItem==NULL)
		return;
	CXhChar100 sText;
	CLDSVector facenorm;
	const int GROUP_GENERAL=1,GROUP_POS=2,GROUP_PROFILE=3,GROUP_OTHER=4;
	DWORD dwPropGroup=GetSingleWord(GROUP_PROFILE);
	if (bFirst)
		facenorm = pFirstPlate->designInfo.huoqufacenorm[0];
	else
		facenorm = pFirstPlate->designInfo.huoqufacenorm[1];
	if (facenorm.norm_style!=0)
	{
		if (bFirst)
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart");
		else
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart");
		pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		if (pFindItem==NULL)
		{	//基准法线构件
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Normal Part Handle";
			lpInfo->m_strPropHelp = "The handle of datum normal part.";
#else 
			lpInfo->m_strPropName = "基准法线构件";
			lpInfo->m_strPropHelp = "基准法线构件";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if (bFirst)
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.x"),&pItem);
		else
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"),&pItem);
		if (pFindItem)
			pFindItem->SetReadOnly();
		if (bFirst)
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.y"),&pItem);
		else
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"),&pItem);
		if (pFindItem)
			pFindItem->SetReadOnly();
		if (bFirst)
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.z"),&pItem);
		else
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"),&pItem);
		if (pFindItem)
			pFindItem->SetReadOnly();
	}
	else
	{
		if (bFirst)
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].norm_wing"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].direction"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].norm_wing"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].direction"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"));
		}
		if (bFirst)
		{
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].vector.x");
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		}
		else
		{
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].vector.x");
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		}
		if (pFindItem)
			pFindItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(idProp));
		if (bFirst)
		{
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].vector.y");
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		}
		else
		{
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].vector.y");
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		}
		if (pFindItem)
			pFindItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(idProp));
		if (bFirst)
		{
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].vector.z");
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		}
		else
		{
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].vector.z");
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		}
		if (pFindItem)
			pFindItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(idProp));
	}
	if (facenorm.norm_style==0)
	{
		if (bFirst)
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].angle");
		else
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].angle");
		pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		if (pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "huo qu angle";
			lpInfo->m_strPropHelp = "huo qu angle";
#else 
			lpInfo->m_strPropName = "火曲角（°)";
			lpInfo->m_strPropHelp = "火曲角（°)";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
			if (bFirst)
				SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].angle");
			else
				SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].angle");
		}
	}
	if (facenorm.norm_style==2)
	{

		if (bFirst)
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].angle"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].norm_wing"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].direction"));
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart");
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].angle"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].norm_wing"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].direction"));
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart");
		}
		pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		if (pFindItem==NULL)
		{	//交叉参考杆件
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Cross Rod Handle";
			lpInfo->m_strPropHelp = "The handle of cross reference rod.";
#else 
			lpInfo->m_strPropName = "交叉参考杆件";
			lpInfo->m_strPropHelp = "交叉参考杆件";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
			if (bFirst)
				SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].hCrossPart");
			else
				SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].hCrossPart");
		}
	}
	else if (facenorm.norm_style!=0)
	{
		if (bFirst)
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].angle"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"));
			idProp = CLDSPlate::GetPropID("huoqufacenorm[0].norm_wing");
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].angle"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"));
			idProp = CLDSPlate::GetPropID("huoqufacenorm[1].norm_wing");
		}
		if (facenorm.norm_style==1)
		{
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
			if (pFindItem==NULL)
			{	//基准肢
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Datum Angle Leg";
				lpInfo->m_strPropHelp = "Datum angle leg";
				lpInfo->m_cmbItems = "X leg normal|Y leg normal";
#else 
				lpInfo->m_strPropName = "基准肢";
				lpInfo->m_strPropHelp = "基准肢";
				lpInfo->m_cmbItems = "X肢法线|Y肢法线";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
				pPropItem->m_idProp = idProp;
				if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			else
			{
				if (bFirst)
					SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].norm_wing");
				else
					SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].norm_wing");
			}
		}
		if (bFirst)
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].direction");
		else
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].direction");
		pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		if (pFindItem==NULL)
		{	//朝向
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Normal";
#else 
			lpInfo->m_strPropName = "法线朝向";
#endif
			if (bFirst)
			{
				lpInfo->m_strPropHelp = pFirstPlate->designInfo.huoqufacenorm[0].GetDirectionHelpStr();
				lpInfo->m_cmbItems = pFirstPlate->designInfo.huoqufacenorm[0].GetDirectionCmbItems();
			}
			else
			{
				lpInfo->m_strPropHelp = pFirstPlate->designInfo.huoqufacenorm[1].GetDirectionHelpStr();
				lpInfo->m_cmbItems = pFirstPlate->designInfo.huoqufacenorm[1].GetDirectionCmbItems();
			}
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
			if (bFirst)
				SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].direction");
			else
				SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].direction");
		}
	}
}

void Update_Plate_Face_N_TreeItem(CPropertyList *pPropList,CPropTreeItem *pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPlate *pFirstPlate = (CLDSPlate*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem,*pSonPropItem,*pNextLeafItem;
	CItemInfo *lpInfo;
	if (pFirstPlate==NULL||pParentItem==NULL)
		return;
	CXhChar100 sText;
	const int GROUP_GENERAL=1,GROUP_POS=2,GROUP_PROFILE=3,GROUP_OTHER=4;
	DWORD dwPropGroup=GetSingleWord(GROUP_PROFILE);
	//钢板基准面 暂时不显示钢板基准面法线定义 wht 11-01-24
	/*pPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("basicFace"),&pParentItem);
	if(pPropItem==NULL)
	{
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_strPropName = "基准面定义";
	lpInfo->m_strPropHelp = "基准面定义";
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
	pPropItem->m_idProp = CLDSPlate::GetPropID("basicFace");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
	pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=dwPropGroup;
	}*/
	if (pFirstPlate->face_N==1)
	{
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_bSelfCorrectBendline"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquFace1"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_start[0]"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_end[0]"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.x"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.y"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.z"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0]"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].norm_wing"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].direction"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquFace2"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_start[1]"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_end[1]"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1]"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].norm_wing"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].direction"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"));
	}
	if (pFirstPlate->face_N>1)
	{
		if (pFirstPlate->face_N==2)
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquFace2"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_start[1]"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_end[1]"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1]"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].norm_wing"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].direction"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("peakVertexOffsetY"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("extendVertexOffsetLen"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("peakPointMargin"));
		}
		//钢板火曲线计算类型
		pPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_bSelfCorrectBendline"),&pParentItem);
		if (pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Bending line defination";
			lpInfo->m_strPropHelp = "Bending line defination";
			lpInfo->m_cmbItems = "0.Line specified|1.Intersection";
#else 
			lpInfo->m_strPropName = "火曲线计算方式";
			lpInfo->m_strPropHelp = "火曲线计算方式";
			lpInfo->m_cmbItems = "0.指定火曲线|1.面面求交线";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSPlate::GetPropID("m_bSelfCorrectBendline");
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		//钢板第一火曲面定义
		pPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquFace1"),&pParentItem);
		if (pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "1st Bending Plane Defination";
			lpInfo->m_strPropHelp = "The defination of first bending plane";
#else 
			lpInfo->m_strPropName = "第一火曲面定义";
			lpInfo->m_strPropHelp = "第一火曲面定义";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSPlate::GetPropID("huoquFace1");
			pPropItem->m_bHideChildren = TRUE;
			if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		//起始点
		pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquline_start[0]"),&pPropItem);
		if (pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,true);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoquline_start[0]");
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		if (pFirstPlate->designInfo.m_bSelfCorrectBendline)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pSonPropItem->m_lpNodeInfo->m_strPropName = "Bending Plane Pick Point";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "The pick point of bending plane ";
#else 
			pSonPropItem->m_lpNodeInfo->m_strPropName = "火曲面拾取点";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "火曲面拾取点";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pSonPropItem->m_lpNodeInfo->m_strPropName = "Bending Line Start Point";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "The start point of bending line ";
#else 
			pSonPropItem->m_lpNodeInfo->m_strPropName = "火曲线起始点";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "火曲线起始点";
#endif
		}
		//终止点
		if (!pFirstPlate->designInfo.m_bSelfCorrectBendline)
		{
			pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquline_end[0]"),&pPropItem);
			if (pSonPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Bending Line End Point";
				lpInfo->m_strPropHelp = "The end point of bending line";
#else 
				lpInfo->m_strPropName = "火曲线终止点";
				lpInfo->m_strPropHelp = "火曲线终止点";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,true);
				pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoquline_end[0]");
				pSonPropItem->m_dwPropGroup=dwPropGroup;
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_end[0]"));
		pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector"),&pPropItem);
		if (pSonPropItem==NULL)
		{	//火曲法线
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Plane Normal";
#else 
			lpInfo->m_strPropName = "火曲法线";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,true);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[0].vector");
			pSonPropItem->m_bHideChildren = TRUE;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.x"),&pSonPropItem);
		if (pSonPropItem==NULL)
			return;
		if (pNextLeafItem==NULL)
		{	//火曲法线X轴方向坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Coordinate";
			lpInfo->m_strPropHelp = "The plane normal coordinates of X axis direction.";
#else 
			lpInfo->m_strPropName = "火曲法线X轴方向坐标";
			lpInfo->m_strPropHelp = "火曲法线X轴方向坐标";
#endif
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,0,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[0].vector.x");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			if (pFirstPlate->designInfo.huoqufacenorm[0].norm_style!=0)
				pNextLeafItem->SetReadOnly();
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.y"),&pSonPropItem);
		if (pNextLeafItem==NULL)
		{	//火曲法线Y轴方向坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Coordinate";
			lpInfo->m_strPropHelp = "The plane normal coordinates of Y axis direction.";
#else 
			lpInfo->m_strPropName = "火曲法线Y轴方向坐标";
			lpInfo->m_strPropHelp = "火曲法线Y轴方向坐标";
#endif
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,1,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[0].vector.y");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			if (pFirstPlate->designInfo.huoqufacenorm[0].norm_style!=0)
				pNextLeafItem->SetReadOnly();
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.z"),&pSonPropItem);
		if (pNextLeafItem==NULL)
		{	//火曲法线Z轴方向坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Coordinate";
			lpInfo->m_strPropHelp = "The plane normal coordinates of Z axis direction.";
#else 
			lpInfo->m_strPropName = "火曲法线Z轴方向坐标";
			lpInfo->m_strPropHelp = "火曲法线Z轴方向坐标";
#endif
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,2,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[0].vector.z");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			if (pFirstPlate->designInfo.huoqufacenorm[0].norm_style!=0)
				pNextLeafItem->SetReadOnly();
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		//钢板第一火曲面法线定义
		pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0]"),&pPropItem);
		if (pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Defination";
			lpInfo->m_strPropHelp = "The normal defination of bending plane.";
#else 
			lpInfo->m_strPropName = "火曲面法线定义";
			lpInfo->m_strPropHelp = "火曲面法线定义";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,3,true);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[0]");
			pSonPropItem->m_bHideChildren = TRUE;
			if (CLDSPlate::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].norm_style"),&pSonPropItem);
		if (pSonPropItem==NULL)
			return;
		if (pNextLeafItem==NULL)
		{
			//肢法线计算方式
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Calculative Type";
			lpInfo->m_strPropHelp = "The desgin parameter of work normal-- leg's normal calculative type";
#else 
			lpInfo->m_strPropName = "肢法线计算方式";
			lpInfo->m_strPropHelp = "工作法线设计参数--肢法线计算方式";
#endif
			lpInfo->m_cmbItems = CLDSVector::GetNormStyleCmbItems();
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,0,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[0].norm_style");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		Update_Plate_Norm_CalStyle_TreeItem(pPropList,pSonPropItem,TRUE);
	}
	if (pFirstPlate->face_N>2)
	{	//双面板到3面板切换过程如果双面板的子项目为展开状态则出错
		//pPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquFace1"),&pParentItem);
		//if(pPropItem)
		//	pPropItem->m_bHideChildren=TRUE;
		//钢板第二火曲面定义
		pPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquFace2"),&pParentItem);
		if (pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "2nd Bending Plane Defination";
			lpInfo->m_strPropHelp = "The defination of second bending plane";
#else 
			lpInfo->m_strPropName = "第二火曲面定义";
			lpInfo->m_strPropHelp = "第二火曲面定义";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSPlate::GetPropID("huoquFace2");
			pPropItem->m_bHideChildren = TRUE;
			if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		//起始点
		pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquline_start[1]"),&pPropItem);
		if (pPropItem==NULL)
			return;
		if (pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,true);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoquline_start[1]");
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		if (pFirstPlate->designInfo.m_bSelfCorrectBendline)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pSonPropItem->m_lpNodeInfo->m_strPropName = "Bending Plane Pick Point";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "The pick point of bending plane ";
#else 
			pSonPropItem->m_lpNodeInfo->m_strPropName = "火曲面拾取点";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "火曲面拾取点";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pSonPropItem->m_lpNodeInfo->m_strPropName = "Bending Line Start Point";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "The start point of bending line ";
#else 
			pSonPropItem->m_lpNodeInfo->m_strPropName = "火曲线起始点";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "火曲线起始点";
#endif
		}
		//终止点
		if (!pFirstPlate->designInfo.m_bSelfCorrectBendline)
		{
			pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquline_end[1]"),&pPropItem);
			if (pSonPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Bending Line End Point";
				lpInfo->m_strPropHelp = "The end point of bending line";
#else 
				lpInfo->m_strPropName = "火曲线终止点";
				lpInfo->m_strPropHelp = "火曲线终止点";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,true);
				pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoquline_end[1]");
				pSonPropItem->m_dwPropGroup=dwPropGroup;
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_end[1]"));
		pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector"),&pPropItem);
		if (pSonPropItem==NULL)
		{	//火曲法线
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Plane Normal";
#else 
			lpInfo->m_strPropName = "火曲法线";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,true);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[1].vector");
			pSonPropItem->m_bHideChildren = TRUE;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"),&pSonPropItem);
		if (pSonPropItem==NULL)
			return;
		if (pNextLeafItem==NULL)
		{	//火曲法线X轴方向坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Coordinate";
			lpInfo->m_strPropHelp = "The bending normal coordinates of X axis direction.";
#else 
			lpInfo->m_strPropName = "火曲法线X轴方向坐标";
			lpInfo->m_strPropHelp = "火曲法线X轴方向坐标";
#endif
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,0,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[1].vector.x");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			if (pFirstPlate->designInfo.huoqufacenorm[1].norm_style!=0)
				pNextLeafItem->SetReadOnly();
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"),&pSonPropItem);
		if (pNextLeafItem==NULL)
		{	//火曲法线Y轴方向坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Coordinate";
			lpInfo->m_strPropHelp = "The bending normal coordinates of Y axis direction.";
#else 
			lpInfo->m_strPropName = "火曲法线Y轴方向坐标";
			lpInfo->m_strPropHelp = "火曲法线Y轴方向坐标";
#endif
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,1,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[1].vector.y");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			if (pFirstPlate->designInfo.huoqufacenorm[1].norm_style!=0)
				pNextLeafItem->SetReadOnly();
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"),&pSonPropItem);
		if (pNextLeafItem==NULL)
		{	//火曲法线Z轴方向坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Coordinate";
			lpInfo->m_strPropHelp = "The bending normal coordinates of z axis direction.";
#else 
			lpInfo->m_strPropName = "火曲法线Z轴方向坐标";
			lpInfo->m_strPropHelp = "火曲法线Z轴方向坐标";
#endif
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,2,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[1].vector.z");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			if (pFirstPlate->designInfo.huoqufacenorm[1].norm_style!=0)
				pNextLeafItem->SetReadOnly();
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		//钢板第二火曲面法线定义
		pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1]"),&pPropItem);
		if (pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Plane Normal Defination";
			lpInfo->m_strPropHelp = "The normal defination of bending plane.";
#else 
			lpInfo->m_strPropName = "火曲面法线定义";
			lpInfo->m_strPropHelp = "火曲面法线定义";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,3,true);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[1]");
			pSonPropItem->m_bHideChildren = TRUE;
			if (CLDSPlate::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].norm_style"),&pSonPropItem);
		if (pSonPropItem==NULL)
			return;
		if (pNextLeafItem==NULL)
		{
			//肢法线计算方式
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Calculative Type";
			lpInfo->m_strPropHelp = "The desgin parameter of work normal-- leg's normal calculative type";
#else 
			lpInfo->m_strPropName = "肢法线计算方式";
			lpInfo->m_strPropHelp = "工作法线设计参数--肢法线计算方式";
#endif
			lpInfo->m_cmbItems = CLDSVector::GetNormStyleCmbItems();
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[1].norm_style");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		Update_Plate_Norm_CalStyle_TreeItem(pPropList,pSonPropItem,FALSE);
		//
		if (pFirstPlate->IsHuoQuLineCollinear3FacePlate())
		{
			CPropListOper<CLDSPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&pView->selectObjs);
			oper.InsertEditPropItem(pParentItem,"peakVertexOffsetY");
			oper.InsertEditPropItem(pParentItem,"extendVertexOffsetLen");
			oper.InsertEditPropItem(pParentItem,"peakPointMargin");
		}
	}
}

BOOL ModifyPlateItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSPlate::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL ModifyParamPlateItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSParamPlate::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL FirePlatePopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSPlate *pFirstPlate=(CLDSPlate*)pView->selectObjs.GetFirst();
	if (pFirstPlate==NULL)
		return FALSE;
	if (pItem->m_idProp==CLDSPlate::GetPropID("plateLinkPoles"))
	{
		if (0==iMenu)		//查看连接杆件项
		{	//显示引入的构件
			CLinkLinePartDlg linkdlg;
			linkdlg.m_pLjPara = &pFirstPlate->designInfo;
			linkdlg.DoModal();
		}
		else if (1==iMenu)	//引入连接杆件项
			pView->ImportLinePartToPlate(pFirstPlate);
	}
	else if (CLDSPlate::GetPropID("plateProfile")==pItem->m_idProp)
	{
		//面数类型，1-3，表示相应的面数的普通板，4表示折叠板，5表示螺栓垫板
		//"构架顶梁八字板|构架曲梁折叠板|螺栓垫板|接头包钢板|加强肋板|普通连接板";
		int iFaceType=pFirstPlate->designInfo.iFaceType;
		if (iMenu==0)
			iFaceType=7;
		else if (iMenu==1)
			iFaceType=6;
		else if (iMenu==2)
			iFaceType=5;
		/*else if(iMenu==3)
			;
		else if(iMenu==4)
			;*/
		else if (iMenu==5)
		{
			if (pFirstPlate->designInfo.iFaceType==7||pFirstPlate->designInfo.iFaceType==6)
				iFaceType=1;
			else
				iFaceType=pFirstPlate->face_N;
		}
		pPropList->SetItemPropValue(pItem->m_idProp,sMenuName);
		if (pFirstPlate->designInfo.iFaceType!=iFaceType)
		{
			CUndoOperObject undo(&Ta);
			pFirstPlate->designInfo.iFaceType=iFaceType;
			pFirstPlate->SetModified();
			pFirstPlate->DesignPlate();
			pFirstPlate->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pFirstPlate->GetSolidPartObject());
			Update_Plate_Face_N_TreeItem(pPropList,pItem);
			CXhChar100 sText;
			int idProp=CLDSPlate::GetPropID("face_N");
			if (pFirstPlate->GetPropValueStr(idProp,sText))	//更新配材号
				pPropList->SetItemPropValue(idProp,sText);

		}
	}
	return TRUE;
}
static UCS_STRU TransWCSFromLocal(CLDSGeneralPlate* pPlate,UCS_STRU& csLocal)
{
	UCS_STRU ucs=csLocal;
	coord_trans(ucs.origin,pPlate->ucs,TRUE,TRUE);
	vector_trans(ucs.axis_x,pPlate->ucs,TRUE,TRUE);
	vector_trans(ucs.axis_y,pPlate->ucs,TRUE,TRUE);
	ucs.axis_z=ucs.axis_x^ucs.axis_y;
	//vector_trans(ucs.axis_z,pPlate->ucs,TRUE,TRUE);
	return ucs;
}
BOOL PlateButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem=NULL,*pParentItem=NULL,*pPropItem=NULL,*pLeafItem=NULL;
	CLDSPlate *pFirstPlate=NULL,*pPlate=NULL;
	CLDSPart *pPart = NULL;
	CReportVectorDlg vecDlg;
	CCfgPartNoDlg cfgdlg;
	char sText[100]="";
	CString cmdStr,valueStr = _T("");

	if (pView->selectObjs.GetNodeNum()>0)
		pFirstPlate = (CLDSPlate*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	CXhChar16 sSoftName("LMA");
#if defined(__LDS_FILE_)
	sSoftName.Copy("LDS");
#elif defined(__LMA_FILE_)
	sSoftName.Copy("LMA");
#elif defined(__TMA_FILE_)
	sSoftName.Copy("TMA");
#elif defined(__TSA_FILE_)
	sSoftName.Copy("TSA");
#elif defined(__TDA_FILE_)
	sSoftName.Copy("TDA");
#endif
	BOOL bRetCode=TRUE;
	try
	{
		CPropListOper<CLDSPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&pView->selectObjs);
		if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.hNode"))
		{
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Dependant node";
			modifyhdlg.m_sTitle="Dependant node:";
#else
			modifyhdlg.m_sCaption="修改依赖节点";
			modifyhdlg.m_sTitle="依赖节点:";
#endif
			if (pFirstPlate==NULL)
				return FALSE;
			modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->designInfo.m_hBaseNode);
			modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
			modifyhdlg.m_iMirMsg=0;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select Dependant node:");
#else
			modifyhdlg.SetCmdPickPromptStr("请选择依赖节点:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if (IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if (pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related node！",sSoftName,MB_OK);
#else
					MessageBox(NULL,"输入句柄有误，找不到相应节点！",sSoftName,MB_OK);
#endif
				else
				{
					CUndoOperObject undo(&Ta);
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pFirstPlate->designInfo.m_hBaseNode=modifyhdlg.m_nHandle;
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("designInfo.hNode"),valueStr);
					g_pSolidDraw->ReleaseSnapStatus();
					for (CLDSObject* pObj=pView->selectObjs.GetFirst();pObj;pObj=pView->selectObjs.GetNext())
					{
						if (pObj->GetClassTypeId()==CLS_PLATE)
						{
							CLDSPlate* pPate=(CLDSPlate*)pObj;
							pPate->designInfo.m_hBaseNode = modifyhdlg.m_nHandle;
							g_pSolidDraw->SetEntSnapStatus(pObj->handle);
						}
					}
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.hRod"))
		{
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Rod Start Node";
			modifyhdlg.m_sTitle="Start Node:";
#else
			modifyhdlg.m_sCaption="修改依赖杆件";
			modifyhdlg.m_sTitle="依赖杆件:";
#endif
			if (pFirstPlate==NULL)
				return FALSE;
			modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->designInfo.m_hBasePart);
			modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
			modifyhdlg.m_iMirMsg=0;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select part:");
#else
			modifyhdlg.SetCmdPickPromptStr("请选择依赖杆件:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			if (IDOK==modifyhdlg.DoModal())
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if (pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part！",sSoftName,MB_OK);
#else
					MessageBox(NULL,"输入句柄有误，找不到相应杆件！",sSoftName,MB_OK);
#endif
				else
				{
					CUndoOperObject undo(&Ta);
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("designInfo.hRod"),valueStr);
					g_pSolidDraw->ReleaseSnapStatus();
					for (CLDSObject* pObj=pView->selectObjs.GetFirst();pObj;pObj=pView->selectObjs.GetNext())
					{
						if (pObj->GetClassTypeId()==CLS_PLATE)
						{
							CLDSPlate* pPate=(CLDSPlate*)pObj;
							pPate->designInfo.m_hBasePart= modifyhdlg.m_nHandle;
							g_pSolidDraw->SetEntSnapStatus(pObj->handle);
						}
					}
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("cfgword"))
		{
			cfgdlg.cfgword = pFirstPlate->cfgword;
			if (pFirstPlate->IsBodyObj())
				cfgdlg.cfg_style=CFG_BODY_NO;
			else
				cfgdlg.cfg_style=CFG_LEG_NO;
			if (cfgdlg.DoModal()==IDOK)
			{
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					pPlate->cfgword = cfgdlg.cfgword;		//在此处更新第一个节点的配材料号
			}
			CXhChar100 sText;
			if (pFirstPlate->GetPropValueStr(pItem->m_idProp,sText))	//更新配材号
				pItem->m_lpNodeInfo->m_strPropValue=sText;
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.origin")||
			pItem->m_idProp==CLDSPlate::GetPropID("wcs.axis_x")||pItem->m_idProp==CLDSPlate::GetPropID("wcs.axis_y"))
		{
			f3dPoint spec_point;
			CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
			int iMenu=0;	//pItem->m_idProp==CLDSPlate::GetPropID("wcs.origin")
			CString cmdStr="请选择当前工作坐标系原点所在的顶点或螺栓:";
			if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.axis_x"))
			{
				iMenu=1;
				cmdStr="请选择当前工作坐标系X轴轴向标定点所在的顶点或螺栓:";
			}
			else if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.axis_y"))
			{
				iMenu=2;
				cmdStr="请选择当前工作坐标系Y轴轴向标定点所在的顶点或螺栓:";
			}
			pCmdLine->FillCmdLine(cmdStr,"");
			CSnapTypeVerify verifier;
			DWORD dwhObj=0,dwExportFlag=0;
			verifier.SetVerifyFlag(OBJPROVIDER::DRAWINGSPACE,GetSingleWord(IDbEntity::DbPoint));
			verifier.AddVerifyType(OBJPROVIDER::SOLIDSPACE,CLS_BOLT);
			CLDSBolt* pPickBolt=NULL;
			PROFILE_VERTEX* pPickVertex=NULL;
			while (true)
			{
				if ((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				{
					pCmdLine->CancelCmdLine();
					return FALSE;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				if (obj.provider==OBJPROVIDER::SOLIDSPACE)
					pPickBolt=(CLDSBolt*)Ta.FromPartHandle(obj.hObj,CLS_BOLT);
				else if (obj.provider==OBJPROVIDER::DRAWINGSPACE)
				{
					IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
					IDbEntity* pEnt=pBuddyDrawing->GetDbEntity(obj.idEnt);
					if (pEnt==NULL || pEnt->GetDbEntType()!=IDbEntity::DbPoint)
						continue;	//非轮廓点
					if (pEnt->GetHiberId().masterId!=pFirstPlate->handle)
						continue;	//非当前钢板上的轮廓点
					DWORD dwObjectId=pEnt->GetHiberId().HiberDownId(1);
					for (PROFILE_VERTEX* pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
					{
						if (pVertex->vertex.ID==dwObjectId)
						{
							pPickVertex=pVertex;
							break;
						}
					}
				}
				else
					continue;
				if (pPickBolt!=NULL||pPickVertex!=NULL)
					break;
			}
			pCmdLine->FinishCmdLine();
			PLATEEDIT_STATE* pState=CLDSPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
			//计算工作坐标系
			f3dPoint xLocalPickPoint;
			if (pPickBolt)
			{
				pFirstPlate->GetBoltIntersPos(pPickBolt,xLocalPickPoint);
				coord_trans(xLocalPickPoint,pFirstPlate->ucs,FALSE,TRUE);
			}
			else
				xLocalPickPoint=pPickVertex->vertex;
			if (iMenu==0)		//捕捉原点
			{
				console.m_curWCS.origin = xLocalPickPoint;

				pState->m_fLocalWcsOrgX=xLocalPickPoint.x;
				pState->m_fLocalWcsOrgY=xLocalPickPoint.y;
			}
			else if (iMenu==1)	//捕捉X轴向标定点
			{	//捕捉轮廓点
				console.m_curWCS.axis_x = xLocalPickPoint-console.m_curWCS.origin;
				normalize(console.m_curWCS.axis_x);
				console.m_curWCS.axis_z.Set(0,0,1);
				console.m_curWCS.axis_y = console.m_curWCS.axis_z^console.m_curWCS.axis_x;

				pState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
				pState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
				pState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
				pState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
			}
			else if (iMenu==2)	//捕捉板上螺栓
			{
				console.m_curWCS.axis_y = xLocalPickPoint-console.m_curWCS.origin;
				normalize(console.m_curWCS.axis_y);
				console.m_curWCS.axis_z.Set(0,0,1);
				console.m_curWCS.axis_x = console.m_curWCS.axis_y^console.m_curWCS.axis_z;

				pState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
				pState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
				pState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
				pState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
			}
			//在钢板实体上显示工作坐标系
			UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
			g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
			g_pSolidDraw->Draw();
			//更新属性栏显示
			if (iMenu==0)
			{
				oper.UpdatePropItemValue("wcs.origin");
				oper.UpdatePropItemValue("wcs.origin.x");
				oper.UpdatePropItemValue("wcs.origin.y");
			}
			else
			{
				oper.UpdatePropItemValue("wcs.axis_x");
				oper.UpdatePropItemValue("wcs.axis_x.a");
				oper.UpdatePropItemValue("wcs.axis_y");
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("InsertProfileVertex"))
		{		//添加轮廓顶点
			return pView->InsertPlankVertex();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("AddNewHole"))
		{	//添加螺栓
			return pView->InsertPlankBolt();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("FilletVertex"))
		{	//切角切弧
			return pView->FilletPlateCorner();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("DeleteFeature"))
		{	//删除钢板特征属性
			return pView->DeletePlankFeature();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("sPartNo"))
		{
			CModifyRelativeObjPartNoDlg modify_partno_dlg;
			modify_partno_dlg.m_pWorkPart=pFirstPlate;
			if (modify_partno_dlg.DoModal()==IDOK)
			{
				pFirstPlate->SetPartNo(modify_partno_dlg.m_sCurObjPartNo.GetBuffer(0));
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
				SEGI iSeg;
				if (CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(modify_partno_dlg.m_sCurObjPartNo,&iSeg,NULL))
				{
					pFirstPlate->iSeg=iSeg;
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("iSeg"),iSeg.ToString());
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs"))
		{
			CPartUcsDlg part_ucs_dlg;
			part_ucs_dlg.ucs=pFirstPlate->ucs;
			if (part_ucs_dlg.DoModal()==IDOK)
			{
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					pPlate->ucs=part_ucs_dlg.ucs;
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.origin"))
		{	//在此处显示钢板定位基准点 wht 11-01-24
			if (pFirstPlate->designInfo.origin.datum_pos_style==10)
			{
				/*CDatumPointPropertyDlg desPointDlg;
				desPointDlg.m_pPoint=&pFirstPlate->designInfo.origin;
				desPointDlg.DoModal();
				pFirstPlate->ucs.origin=pFirstPlate->designInfo.origin.UpdatePos(&Ta);
				pFirstPlate->DesignSetupUcs();
				CLsRef *pLsRef=NULL;
				for(pLsRef=pFirstPlate->GetFirstLsRef();pLsRef;pLsRef=pFirstPlate->GetNextLsRef())
				{	//重新计算螺栓位置
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}*/
#ifdef AFX_TARG_ENU_ENGLISH
				vecDlg.m_sCaption = "Assembly CS origin";
#else 
				vecDlg.m_sCaption = "装配坐标系原点位置";
#endif
				vecDlg.m_fVectorX = pFirstPlate->ucs.origin.x;
				vecDlg.m_fVectorY = pFirstPlate->ucs.origin.y;
				vecDlg.m_fVectorZ = pFirstPlate->ucs.origin.z;
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.origin.x"),&pParentItem);
				vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.origin.y"),&pParentItem);
				vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.origin.z"),&pParentItem);
				vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
				if (vecDlg.DoModal()==IDOK)
				{
					CXhChar100 sText;
					sText.Printf("%f",vecDlg.m_fVectorX);
					SimplifiedNumString(sText);
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.origin.x"),CString(sText));
					sText.Printf("%f",vecDlg.m_fVectorY);
					SimplifiedNumString(sText);
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.origin.y"),CString(sText));
					sText.Printf("%f",vecDlg.m_fVectorZ);
					SimplifiedNumString(sText);
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.origin.z"),CString(sText));
					for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
						pPlate->ucs.origin.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				}
			}
			else
			{
				/*CDatumPointDlg des_point_dlg;
				des_point_dlg.m_pPoint=&pFirstPlate->designInfo.origin;
				des_point_dlg.m_shDatumPoint.Format("句柄:无");
				des_point_dlg.m_sPointDescription = "连接板定位基点";
				if(des_point_dlg.DoModal()==IDOK)
				pFirstPlate->designInfo.origin=*des_point_dlg.m_pPoint;*/
				//
				CDatumPointPropertyDlg desPointDlg;
				desPointDlg.m_pCurEditObj=pFirstPlate;
				desPointDlg.m_xPoint=pFirstPlate->designInfo.origin;
				if (desPointDlg.m_xPoint.datum_pos_style==0)
					desPointDlg.m_xPoint.SetPosition(pFirstPlate->ucs.origin);
				if (desPointDlg.DoModal()==IDOK)
				{
					pFirstPlate->designInfo.origin=desPointDlg.m_xPoint;
					pFirstPlate->designInfo.origin.UpdatePos(&Ta);
					pFirstPlate->ucs.origin=pFirstPlate->designInfo.origin.Position();
					CUndoOperObject undo(&Ta);
					pFirstPlate->DesignPlate();
					pFirstPlate->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pFirstPlate->GetSolidPartObject());
				}
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_x"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "X Axis Direction";
#else 
			vecDlg.m_sCaption = "装配时X轴方向";
#endif
			vecDlg.m_fVectorX = pFirstPlate->ucs.axis_x.x;
			vecDlg.m_fVectorY = pFirstPlate->ucs.axis_x.y;
			vecDlg.m_fVectorZ = pFirstPlate->ucs.axis_x.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.axis_x.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.axis_x.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.axis_x.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.axis_x.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.axis_x.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.axis_x.z"),CString(sText));
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->ucs.axis_x.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_x*1,0);
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_y"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Y Axis Direction";
#else 
			vecDlg.m_sCaption = "装配时Y轴方向";
#endif
			vecDlg.m_fVectorX = pFirstPlate->ucs.axis_y.x;
			vecDlg.m_fVectorY = pFirstPlate->ucs.axis_y.y;
			vecDlg.m_fVectorZ = pFirstPlate->ucs.axis_y.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.axis_y.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.axis_y.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.axis_y.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.axis_y.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.axis_y.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.axis_y.z"),CString(sText));
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->ucs.axis_y.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_y*1,1);
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_z"))
		{	//钢板法线定位方式 wht 11-01-24
			CDesignVectorPropDlg vector_dlg;
			vector_dlg.m_hCurEditPart=pFirstPlate->handle;
			if (vector_dlg.m_xVector.norm_style==0)
				pFirstPlate->designInfo.norm.vector=pFirstPlate->ucs.axis_z;
			vector_dlg.m_xVector = pFirstPlate->designInfo.norm;
			//为方便调整立板法线与所在坡面一致操作，十字插板附加板初始化时需要特殊处理 wht 11-05-25
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pFirstPlate->m_hPartWeldParent,CLS_PARAMPLATE);
			if (pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
			{	//钢板父构件为十字插板
				vector_dlg.m_hCurEditPart=pFirstPlate->handle;
				//1.当前定位方式未使用近似法线变量，使用钢板方向初始化近似法线
				if (vector_dlg.m_xVector.norm_style!=2&&vector_dlg.m_xVector.norm_style!=4)
					vector_dlg.m_xVector.nearVector=pFirstPlate->ucs.axis_z;
				//2.当前定位方式未使用交叉杆件变量，使用插板所在杆件初始化交叉杆件句柄
				if (vector_dlg.m_xVector.norm_style!=2)
					vector_dlg.m_xVector.hCrossPart=pParamPlate->m_hPartWeldParent;
			}
			//3.显示坐标系
			UCS_STRU object_ucs,ucs = pFirstPlate->ucs;
			g_pSolidSet->GetObjectUcs(object_ucs);
			coord_trans(ucs.origin,object_ucs,TRUE);
			vector_trans(ucs.axis_x,object_ucs,TRUE);
			vector_trans(ucs.axis_y,object_ucs,TRUE);
			vector_trans(ucs.axis_z,object_ucs,TRUE);
			g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
			//g_pSolidSet->SetDispWorkUcs();
			g_pSolidDraw->Draw();
			if (vector_dlg.DoModal()==IDOK)
			{
				CUndoOperObject undo(&Ta);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.norm=vector_dlg.m_xVector;
					pPlate->designInfo.norm.UpdateVector(&Ta);
					//调整法线设计参数后重新设计当前钢板
					pPlate->DesignSetupUcs();
					pPlate->DesignPlate();
					pPlate->SetModified();
					pPlate->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
					//更新Z轴方向的显示内容 wht 11-05-31
					long idProp=CLDSPlate::GetPropID("ucs.axis_z.x");
					if (pFirstPlate->GetPropValueStr(idProp,sText)>0)
						pPropList->SetItemPropValue(idProp,sText);
					idProp=CLDSPlate::GetPropID("ucs.axis_z.y");
					if (pFirstPlate->GetPropValueStr(idProp,sText)>0)
						pPropList->SetItemPropValue(idProp,sText);
					idProp=CLDSPlate::GetPropID("ucs.axis_z.z");
					if (pFirstPlate->GetPropValueStr(idProp,sText)>0)
						pPropList->SetItemPropValue(idProp,sText);
					if (pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
					{	//重新设计插板附加板中已当前钢板为定位构件的包板
						ATTACHPART_ITEM *pItem=NULL;
						for (pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
						{
							CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pItem->h,CLS_PLATE);
							if (pPlate==NULL)
								continue;
							if (pPlate->designInfo.norm.hVicePart==pFirstPlate->handle
								&&pPlate->designInfo.norm.norm_style>0)
							{
								pPlate->DesignSetupUcs();
								pPlate->DesignPlate();
								pPlate->SetModified();
								pPlate->Create3dSolidModel();
								g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
							}
						}
						//重新设计参数板时将其附带的螺栓重新设计
						for (CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
						{
							CLDSBolt *pBolt=pLsRef->GetLsPtr();
							pBolt->correct_worknorm();
							pBolt->correct_pos();
							pBolt->CalGuigeAuto();
							pBolt->SetModified();
							pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
							g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
						}
					}
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
			//
			//g_pSolidSet->SetDispWorkUcs(FALSE);
			g_pSolidDraw->BatchClearCS(2);
			g_pSolidDraw->Draw();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("plate_work_plane"))
		{
			CPartWorkPlaneDlg planeDlg;
			planeDlg.m_pDatumPart=pFirstPlate;
			planeDlg.DoModal();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerColumnNorm"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Plate Inner Circle Normal";
#else 
			vecDlg.m_sCaption = "钢板内环轴线方向";
#endif
			vecDlg.m_fVectorX = pFirstPlate->m_tInnerColumnNorm.x;
			vecDlg.m_fVectorY = pFirstPlate->m_tInnerColumnNorm.y;
			vecDlg.m_fVectorZ = pFirstPlate->m_tInnerColumnNorm.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_tInnerColumnNorm.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_tInnerColumnNorm.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_tInnerColumnNorm.z"),CString(sText));
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					pPlate->m_tInnerColumnNorm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerOrigin"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Inner Circle Position";
#else 
			vecDlg.m_sCaption = "钢板内环位置";
#endif
			vecDlg.m_fVectorX = pFirstPlate->m_tInnerOrigin.x;
			vecDlg.m_fVectorY = pFirstPlate->m_tInnerOrigin.y;
			vecDlg.m_fVectorZ = pFirstPlate->m_tInnerOrigin.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_tInnerOrigin.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_tInnerOrigin.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_tInnerOrigin.z"),CString(sText));
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					pPlate->m_tInnerOrigin.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_fInnerRadius"))
		{
			valueStr.Format("%lf",pFirstPlate->m_fInnerRadius);
			SimplifiedNumString(valueStr);
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the inner circle's radius",&valueStr,'s');
#else 
			Input("输入内环半径",&valueStr,'s');
#endif
			CString sOffset;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pFirstPlate->m_fInnerRadius=atof(valueStr);
			pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_fInnerRadius"),valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoquline_start[0]"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstPlate;
			GEPOINT pos=pFirstPlate->HuoQuLine[0].startPt;
			coord_trans(pos,pFirstPlate->ucs,TRUE,TRUE);
			pFirstPlate->designInfo.huoquline_start[0].SetPosition(pos);
			datumPointDlg.m_xPoint = pFirstPlate->designInfo.huoquline_start[0];
			if (IDOK==datumPointDlg.DoModal())
			{
				CXhChar100 sText;
				CLDSPoint datumPoint=datumPointDlg.m_xPoint;
				sText.Printf("%f",datumPoint.Position().x);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().y);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().z);
				SimplifiedNumString(sText);
				datumPoint.UpdatePos(&Ta);
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.huoquline_start[0]=datumPoint;
					pPlate->HuoQuLine[0].startPt=datumPoint.Position();
					coord_trans(pPlate->HuoQuLine[0].startPt,pPlate->ucs,FALSE,TRUE);
					pPlate->HuoQuLine[0].startPt.z=0;
					pPlate->DesignPlate();
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoquline_end[0]"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstPlate;
			GEPOINT pos=pFirstPlate->HuoQuLine[0].endPt;
			coord_trans(pos,pFirstPlate->ucs,TRUE,TRUE);
			pFirstPlate->designInfo.huoquline_end[0].SetPosition(pos);
			datumPointDlg.m_xPoint = pFirstPlate->designInfo.huoquline_end[0];
			if (IDOK==datumPointDlg.DoModal())
			{
				CXhChar100 sText;
				CLDSPoint datumPoint=datumPointDlg.m_xPoint;
				sText.Printf("%f",datumPoint.Position().x);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().y);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().z);
				SimplifiedNumString(sText);
				datumPoint.UpdatePos(&Ta);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.huoquline_end[0]=datumPoint;
					pPlate->HuoQuLine[0].endPt=datumPoint.Position();
					coord_trans(pPlate->HuoQuLine[0].endPt,pPlate->ucs,FALSE,TRUE);
					pPlate->HuoQuLine[0].endPt.z=0;
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].vector"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Plane Normal";
#else 
			vecDlg.m_sCaption = "火曲法线";
#endif
			vecDlg.m_fVectorX = pFirstPlate->huoqufacenorm[0].x;	//designInfo.huoqufacenorm[0].vector.x;
			vecDlg.m_fVectorY = pFirstPlate->huoqufacenorm[0].y;	//designInfo.huoqufacenorm[0].vector.y;
			vecDlg.m_fVectorZ = pFirstPlate->huoqufacenorm[0].z;	//designInfo.huoqufacenorm[0].vector.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("huoqufacenorm[0].vector.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("huoqufacenorm[0].vector.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("huoqufacenorm[0].vector.z"),CString(sText));
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{	//修正不能修改火曲面法线错误 wjh/2009-4-16
					pPlate->designInfo.huoqufacenorm[0].vector.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					pPlate->huoqufacenorm[0].Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					pPlate->DesignPlate();
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoquline_start[1]"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstPlate;
			GEPOINT pos=pFirstPlate->HuoQuLine[1].startPt;
			coord_trans(pos,pFirstPlate->ucs,TRUE,TRUE);
			pFirstPlate->designInfo.huoquline_start[1].SetPosition(pos);
			datumPointDlg.m_xPoint = pFirstPlate->designInfo.huoquline_start[1];
			if (IDOK==datumPointDlg.DoModal())
			{
				CXhChar100 sText;
				CLDSPoint datumPoint=datumPointDlg.m_xPoint;
				sText.Printf("%f",datumPoint.Position().x);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().y);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().z);
				SimplifiedNumString(sText);
				datumPoint.UpdatePos(&Ta);
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.huoquline_start[1]=datumPoint;
					pPlate->HuoQuLine[1].startPt=datumPoint.Position();
					coord_trans(pPlate->HuoQuLine[1].startPt,pPlate->ucs,FALSE,TRUE);
					pPlate->HuoQuLine[1].startPt.z=0;
					pPlate->DesignPlate();
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoquline_end[1]"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstPlate;
			GEPOINT pos=pFirstPlate->HuoQuLine[1].endPt;
			coord_trans(pos,pFirstPlate->ucs,TRUE,TRUE);
			pFirstPlate->designInfo.huoquline_end[1].SetPosition(pos);
			datumPointDlg.m_xPoint = pFirstPlate->designInfo.huoquline_end[1];
			if (IDOK==datumPointDlg.DoModal())
			{
				CXhChar100 sText;
				CLDSPoint datumPoint=datumPointDlg.m_xPoint;
				sText.Printf("%f",datumPoint.Position().x);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().y);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().z);
				SimplifiedNumString(sText);
				datumPoint.UpdatePos(&Ta);
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.huoquline_end[1]=datumPoint;
					pPlate->HuoQuLine[1].endPt=datumPoint.Position();
					coord_trans(pPlate->HuoQuLine[1].endPt,pPlate->ucs,FALSE,TRUE);
					pPlate->HuoQuLine[1].endPt.z=0;
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].vector"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Plane Normal";
#else 
			vecDlg.m_sCaption = "火曲法线";
#endif
			vecDlg.m_fVectorX = pFirstPlate->huoqufacenorm[1].x;	//designInfo.huoqufacenorm[1].vector.x;
			vecDlg.m_fVectorY = pFirstPlate->huoqufacenorm[1].y;	//designInfo.huoqufacenorm[1].vector.y;
			vecDlg.m_fVectorZ = pFirstPlate->huoqufacenorm[1].z;	//designInfo.huoqufacenorm[1].vector.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"),CString(sText));
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{	//修正不能修改火曲面法线错误 wjh/2009-4-16
					pPlate->designInfo.huoqufacenorm[1].vector.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					pPlate->huoqufacenorm[1].Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					pPlate->SetModified();
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0]"))
		{	//火曲面法线设计
			CDesignVectorPropDlg vector_dlg;
			vector_dlg.m_hCurEditPart=pFirstPlate->handle;
			if (pFirstPlate->designInfo.huoqufacenorm[0].norm_style==0
				&&pFirstPlate->designInfo.huoqufacenorm[0].vector.IsZero())
				pFirstPlate->designInfo.huoqufacenorm[0].vector=pFirstPlate->huoqufacenorm[0];
			vector_dlg.m_xVector=pFirstPlate->designInfo.huoqufacenorm[0];
			if (vector_dlg.DoModal()==IDOK)
			{
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.huoqufacenorm[0]=vector_dlg.m_xVector;
					pPlate->designInfo.huoqufacenorm[0].UpdateVector(&Ta);
					//更新钢板火曲面法线 wht 15-09-08
					pPlate->huoqufacenorm[0]=pPlate->designInfo.huoqufacenorm[0].vector;
					pPlate->SetModified();
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1]"))
		{	//火曲面法线设计
			CDesignVectorPropDlg vector_dlg;
			vector_dlg.m_hCurEditPart=pFirstPlate->handle;
			if (pFirstPlate->designInfo.huoqufacenorm[1].norm_style==0
				&&pFirstPlate->designInfo.huoqufacenorm[1].vector.IsZero())
				pFirstPlate->designInfo.huoqufacenorm[1].vector=pFirstPlate->huoqufacenorm[1];
			vector_dlg.m_xVector=pFirstPlate->designInfo.huoqufacenorm[1];
			if (vector_dlg.DoModal()==IDOK)
			{
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.huoqufacenorm[1]=vector_dlg.m_xVector;
					pPlate->designInfo.huoqufacenorm[1].UpdateVector(&Ta);
					//更新钢板火曲面法线 wht 15-09-08
					pPlate->huoqufacenorm[1]=pPlate->designInfo.huoqufacenorm[1].vector;
					pPlate->SetModified();
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("basicFace"))
		{	//基准面定义
			CEditPlateDatumFaceDlg datum_face_dlg;
			if (pFirstPlate->designInfo.origin.datum_pos_style==0
				&&pFirstPlate->designInfo.origin.Position().IsZero())
				pFirstPlate->designInfo.origin.SetPosition(pFirstPlate->ucs.origin);
			else
				pFirstPlate->designInfo.origin.UpdatePos(&Ta);
			if (pFirstPlate->designInfo.norm.norm_style==0
				&&pFirstPlate->designInfo.norm.vector.IsZero())
				pFirstPlate->designInfo.norm.vector=pFirstPlate->ucs.axis_z;
			else
				pFirstPlate->designInfo.norm.UpdateVector(&Ta);
			datum_face_dlg.m_pLjPara=&pFirstPlate->designInfo;
			datum_face_dlg.DoModal();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoquFace1"))
		{	//第一火曲面定义	
			CDefHuoquFaceDlg huoqu_face_dlg;
			pFirstPlate->designInfo.origin.UpdatePos(&Ta);
			//更新火曲线法线设计参数
			if (pFirstPlate->designInfo.huoqufacenorm[0].norm_style==0
				&&pFirstPlate->designInfo.huoqufacenorm[0].vector.IsZero())
				pFirstPlate->designInfo.huoqufacenorm[0].vector=pFirstPlate->huoqufacenorm[0];
			//更新火曲线终点设计参数
			if (pFirstPlate->designInfo.huoquline_start[0].datum_pos_style==0
				&&pFirstPlate->designInfo.huoquline_start[0].Position().IsZero())
				pFirstPlate->designInfo.huoquline_start[0].SetPosition(pFirstPlate->HuoQuLine[0].startPt);
			else
				pFirstPlate->designInfo.huoquline_start[0].UpdatePos(&Ta);
			//更新火曲线终点设计参数
			if (pFirstPlate->designInfo.huoquline_end[0].datum_pos_style==0
				&&pFirstPlate->designInfo.huoquline_end[0].Position().IsZero())
				pFirstPlate->designInfo.huoquline_end[0].SetPosition(pFirstPlate->HuoQuLine[0].endPt);
			else
				pFirstPlate->designInfo.huoquline_end[0].UpdatePos(&Ta);
			huoqu_face_dlg.m_pLjPara=&pFirstPlate->designInfo;
			huoqu_face_dlg.m_iHuoquFaceType=0;
			if (huoqu_face_dlg.DoModal()==IDOK)	//更新钢板火曲面法线 wjh 15-09-09
				pFirstPlate->huoqufacenorm[0].Set(huoqu_face_dlg.m_fHuoquNormX,huoqu_face_dlg.m_fHuoquNormY,huoqu_face_dlg.m_fHuoquNormZ);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoquFace2"))
		{	//第二火曲面定义	
			CDefHuoquFaceDlg huoqu_face_dlg;
			pFirstPlate->designInfo.origin.UpdatePos(&Ta);
			//更新火曲线法线设计参数
			if (pFirstPlate->designInfo.huoqufacenorm[1].norm_style==0
				&&pFirstPlate->designInfo.huoqufacenorm[1].vector.IsZero())
				pFirstPlate->designInfo.huoqufacenorm[1].vector=pFirstPlate->huoqufacenorm[1];
			//更新火曲线终点设计参数
			if (pFirstPlate->designInfo.huoquline_start[1].datum_pos_style==0
				&&pFirstPlate->designInfo.huoquline_start[1].Position().IsZero())
				pFirstPlate->designInfo.huoquline_start[1].SetPosition(pFirstPlate->HuoQuLine[1].startPt);
			else
				pFirstPlate->designInfo.huoquline_start[1].UpdatePos(&Ta);
			//更新火曲线终点设计参数
			if (pFirstPlate->designInfo.huoquline_end[1].datum_pos_style==0
				&&pFirstPlate->designInfo.huoquline_end[1].Position().IsZero())
				pFirstPlate->designInfo.huoquline_end[1].SetPosition(pFirstPlate->HuoQuLine[1].endPt);
			else
				pFirstPlate->designInfo.huoquline_end[1].UpdatePos(&Ta);
			huoqu_face_dlg.m_pLjPara=&pFirstPlate->designInfo;
			huoqu_face_dlg.m_iHuoquFaceType=1;
			if (huoqu_face_dlg.DoModal()==IDOK)	//更新钢板火曲面法线 wjh 15-09-09
				pFirstPlate->huoqufacenorm[1].Set(huoqu_face_dlg.m_fHuoquNormX,huoqu_face_dlg.m_fHuoquNormY,huoqu_face_dlg.m_fHuoquNormZ);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("plateParam"))
		{	//钢板参数
			CDesignLjParaDlg ljDlg;
			if (pFirstPlate->jdb_style==4)	//螺栓垫板
				pFirstPlate->designInfo.iFaceType=5;
			pFirstPlate->designInfo.origin.SetPosition(pFirstPlate->ucs.origin);
			pFirstPlate->designInfo.norm.vector=pFirstPlate->ucs.axis_z;
			ljDlg.m_pPlate = pFirstPlate;
			ljDlg.DoModal();
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("plateLinkPoles"))
		{	//查看连接杆件项
			CLinkLinePartDlg linkdlg;
			linkdlg.m_hPlate=pFirstPlate->handle;
			linkdlg.m_pLjPara = &pFirstPlate->designInfo;
			linkdlg.DoModal();
			CUndoOperObject undo(&Ta,true);
			pFirstPlate->DesignPlate();
			g_pSolidDraw->NewSolidPart(pFirstPlate->GetSolidPartObject());
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("relativeObjs"))
		{
			CRelativeObjectDlg relativeDlg;
			relativeDlg.m_pWorkObj = pFirstPlate;
			if (relativeDlg.DoModal()==IDOK)
			{
				CXhChar200 sText;
				pFirstPlate->GetPropValueStr(pItem->m_idProp,sText);
				pPropList->SetItemPropValue(pItem->m_idProp,sText);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("shadowInfo"))
		{
			CDesignShadowDlg shadowdlg;
			shadowdlg.SetShadowObj((CLDSObject*)pFirstPlate);
			shadowdlg.DoModal();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_hPartWeldParent"))
		{
			//通过选择构件修改焊接父构件，现在仅可以修改单个构件的焊接父构件
			CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Weld Parent Part";
			modifyhdlg.m_sTitle="Weld Parent Part:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->m_hPartWeldParent);
			modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
			modifyhdlg.SetCmdPickPromptStr("Please select weld parent part:");
#else 
			modifyhdlg.m_sCaption="修改钢板焊接父构件";
			modifyhdlg.m_sTitle="焊接父构件:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->m_hPartWeldParent);
			modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
			modifyhdlg.SetCmdPickPromptStr("请选择焊接父构件:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			if (IDOK==modifyhdlg.DoModal())
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if (pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"The input handle is wrong,so can't find the corresponding part！",sSoftName,MB_OK);
#else 
					MessageBox(NULL,"输入句柄有误，找不到相应构件！",sSoftName,MB_OK);
#endif
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_hPartWeldParent"),valueStr);
					g_pSolidDraw->ReleaseSnapStatus();
					MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
					for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					{
						int flagArr[3]={ 1,2,4 };
						CLDSPlate* pMirPlate=NULL;
						for (int i=0;i<3;i++)
						{
							if ((mirmsg.axis_flag&flagArr[i])==0)
								continue;
							MIRMSG msg=mirmsg.GetSubMirItem(i+1);
							//查找钢板的对称构件
							pMirPlate=(CLDSPlate*)pPlate->GetMirPart(msg);
							if (pMirPlate)
							{
								CLDSPart *pMirPart=pPart->GetMirPart(msg);
								if (pMirPart)	//查找所选父构件的对称构件 wht 11-05-07
									pMirPlate->m_hPartWeldParent=pMirPart->handle;
							}
						}
						pPlate->m_hPartWeldParent=pPart->handle;
						g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
					}
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart")||
			pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart"))
		{
			//通过选择构件修改定位基准构件,一般不需要对称功能故支持直接选择构件 wht 11-04-27
			//重复执行GetPartsFromScr，会导致多个线程同时执行但不能同时终止导致下一步selectObjs乱掉死机 wjh-2011.7.19
			CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify normal of the bending plane";
			modifyhdlg.m_sTitle=" Normal of the bending plane:";
#else
			modifyhdlg.m_sCaption="修改火曲面法线基准构件";
			modifyhdlg.m_sTitle="火曲面法线基准构件:";
#endif
			if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"))
				modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->designInfo.huoqufacenorm[0].hVicePart);
			else
				modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->designInfo.huoqufacenorm[1].hVicePart);
			modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Select part to determine normal：");
#else
			modifyhdlg.SetCmdPickPromptStr("选择法线基准构件:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			modifyhdlg.m_iMirMsg=0;
			if (IDOK==modifyhdlg.DoModal())
			{
				CLDSDbObject *pObj=console.FromPartHandle(modifyhdlg.m_nHandle);
				if (pObj&&pObj->IsPart())
				{
					pPropList->SetItemPropValue(pItem->m_idProp,"0X%X",pObj->handle);
					g_pSolidDraw->ReleaseSnapStatus();
					for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					{
						if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"))
							pPlate->designInfo.huoqufacenorm[0].hVicePart=pObj->handle;
						else
							pPlate->designInfo.huoqufacenorm[1].hVicePart=pObj->handle;
						g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
					}
					//选择基准构件后，根据构件类型更新方向下拉框内容、帮助内容及方向显示内容 wht 12-09-26
					long idProp=0;
					CLDSVector *pVector=NULL;
					if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"))
					{
						idProp=CLDSPlate::GetPropID("huoqufacenorm[0].direction");
						pVector=&(pFirstPlate->designInfo.huoqufacenorm[0]);
					}
					else
					{
						idProp=CLDSPlate::GetPropID("huoqufacenorm[1].direction");
						pVector=&(pFirstPlate->designInfo.huoqufacenorm[1]);
					}
					pFindItem=pPropList->FindItemByPropId(idProp,NULL);
					if (pFindItem)
					{
						pFindItem->m_lpNodeInfo->m_cmbItems=pVector->GetDirectionCmbItems();
						pFindItem->m_lpNodeInfo->m_strPropHelp=pVector->GetDirectionHelpStr();
						CXhChar100 sDirection;
						if (pFirstPlate->GetPropValueStr(idProp,sDirection)>0)
							pFindItem->m_lpNodeInfo->m_strPropValue=sDirection;
					}
				}
				pPropList->SetCurSel(pItem->m_iIndex);
				pPropList->SetFocus();
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart")||
			pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"))
		{
			//通过选择构件修改定位基准构件,一般不需要对称功能故支持直接选择构件 wht 11-04-27
			//重复执行GetPartsFromScr，会导致多个线程同时执行但不能同时终止导致下一步selectObjs乱掉死机 wjh-2011.7.19
			CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify normal of the bending plane";
			modifyhdlg.m_sTitle=" Normal of the bending plane:";
#else
			modifyhdlg.m_sCaption="修改火曲面法线基准构件";
			modifyhdlg.m_sTitle="火曲面法线基准构件:";
#endif
			if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"))
				modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->designInfo.huoqufacenorm[0].hCrossPart);
			else
				modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->designInfo.huoqufacenorm[1].hCrossPart);
			modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Select part to determine normal：");
#else
			modifyhdlg.SetCmdPickPromptStr("选择法线基准构件:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			modifyhdlg.m_iMirMsg=0;
			if (IDOK==modifyhdlg.DoModal())
			{
				CLDSDbObject *pObj=console.FromPartHandle(modifyhdlg.m_nHandle);
				if (pObj&&pObj->IsPart())
				{
					pPropList->SetItemPropValue(pItem->m_idProp,"0X%X",pObj->handle);
					g_pSolidDraw->ReleaseSnapStatus();
					for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					{
						if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"))
							pPlate->designInfo.huoqufacenorm[0].hCrossPart=pObj->handle;
						else
							pPlate->designInfo.huoqufacenorm[1].hCrossPart=pObj->handle;
						g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
					}
					//选择基准构件后，根据构件类型更新方向下拉框内容、帮助内容及方向显示内容 wht 12-09-26
					long idProp=0;
					CLDSVector *pVector=NULL;
					if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"))
					{
						idProp=CLDSPlate::GetPropID("huoqufacenorm[0].direction");
						pVector=&(pFirstPlate->designInfo.huoqufacenorm[0]);
					}
					else
					{
						idProp=CLDSPlate::GetPropID("huoqufacenorm[1].direction");
						pVector=&(pFirstPlate->designInfo.huoqufacenorm[1]);
					}
					pFindItem=pPropList->FindItemByPropId(idProp,NULL);
					if (pFindItem)
					{
						pFindItem->m_lpNodeInfo->m_cmbItems=pVector->GetDirectionCmbItems();
						pFindItem->m_lpNodeInfo->m_strPropHelp=pVector->GetDirectionHelpStr();
						CXhChar100 sDirection;
						if (pFirstPlate->GetPropValueStr(idProp,sDirection)>0)
							pFindItem->m_lpNodeInfo->m_strPropValue=sDirection;
					}
				}
				pPropList->SetCurSel(pItem->m_iIndex);
				pPropList->SetFocus();
			}
		}

		for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
		{
			pPlate->SetModified();
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		}
		g_pSolidDraw->Draw();
	}
	catch (char *sError)
	{
		AfxMessageBox(sError);
	}
	return bRetCode;
}
BOOL RedrawPlateParamDrawing(CLDSGeneralPlate* pPlate,const char *sPropKey,UCS_STRU *pObjectCS=NULL,bool removeOldEntities=true);
BOOL FirePlateHelpStr(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPlate *pCurPlate = (CLDSPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL||pItem==NULL)
		return FALSE;
	long *id_arr=NULL;
	long n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if (pItem->m_idProp==CLDSPlate::GetPropID("m_hPartWeldParent"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurPlate->m_hPartWeldParent);
	}
	else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurPlate->designInfo.huoqufacenorm[0].hVicePart);
	}
	else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurPlate->designInfo.huoqufacenorm[0].hCrossPart);
	}
	else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurPlate->designInfo.huoqufacenorm[1].hVicePart);
	}
	else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurPlate->designInfo.huoqufacenorm[1].hCrossPart);
	}
	else if (n==1&&id_arr[0]!=pCurPlate->handle)
	{	//上一次选中的是焊接父构件
		g_pSolidDraw->ReleaseSnapStatus();
		for (CLDSObject* pObj=pView->selectObjs.GetFirst();pObj;pObj=pView->selectObjs.GetNext())
			g_pSolidDraw->SetEntSnapStatus(pObj->handle);
	}
	else if (pItem->m_idProp==CLDSPlate::GetPropID("peakVertexOffsetY"))
		RedrawPlateParamDrawing(pCurPlate,"peakVertexOffsetY");
	else if (pItem->m_idProp==CLDSPlate::GetPropID("extendVertexOffsetLen"))
		RedrawPlateParamDrawing(pCurPlate,"extendVertexOffsetLen");
	else
		return FALSE;

	return TRUE;
}
long RetrieveBlockHandle(const char* blockIdName);
BOOL ModifyPlateProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPlate *pFirstPlate = (CLDSPlate*)pView->selectObjs.GetFirst();
	CPropTreeItem *pFindItem,*pParentItem,*pPropItem;
	CLDSPlate *pPlate = NULL;
	CItemInfo *lpInfo;
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	int iTemp;
	if (pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	try {
		BOOL bUpdateWcs=FALSE;
		CPropListOper<CLDSPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&pView->selectObjs);
		_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
		if (pItem->m_idProp==CLDSPlate::GetPropID("layer"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->SetLayer(tem_str);
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					if (pMirObj->mirInfo.axis_flag&8)
						continue;	//旋转对称
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						CalMirAtomLayer(pPlate->layer(),pMirPlate->layer(),pMirObj->mirInfo);
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("cMaterial"))
		{	//钢板材质
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->cMaterial = QuerySteelBriefMatMark(tem_str);
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->cMaterial=pPlate->cMaterial;
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_cQualityLevel"))
		{	//钢板质量等级
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Default")==0)
					pPlate->m_cQualityLevel=0;
				else
#else
				if (valueStr.CompareNoCase("默认")==0)
					pPlate->m_cQualityLevel=0;
				else
#endif
					pPlate->m_cQualityLevel = valueStr[0];
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->m_cQualityLevel=pPlate->m_cQualityLevel;
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.hNode"))
		{
			pFirstPlate->designInfo.m_hBaseNode=HexStrToLong(valueStr);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.m_hBaseNode=pFirstPlate->designInfo.m_hBaseNode;
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.hRod"))
		{
			pFirstPlate->designInfo.m_hBasePart=HexStrToLong(valueStr);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.m_hBasePart=pFirstPlate->designInfo.m_hBasePart;
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_hBlock"))
		{
			long hBlock=RetrieveBlockHandle(valueStr);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_hBlock=hBlock;
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("thick"))
		{	//钢板厚度
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->Thick=atoi(tem_str);
				pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->Thick=ftoi(pPlate->GetThick());
				}
			}
			g_pSolidDraw->Draw();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("iSeg"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->iSeg = SEGI(valueStr);
				pPlate->SyncMirProp("iSeg");
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("sPartNo"))
		{	//根据件号提取段号，并更新钢板段号
			bool parsed=false;
			SEGI iSeg;
			if (CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(tem_str,&iSeg,NULL))
			{
				parsed=true;
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("iSeg"),iSeg.ToString());
			}
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				if (parsed)
					pPlate->iSeg=iSeg;
				pPlate->SetPartNo(tem_str);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("cfgword"))
		{	//用更新过的第一个构件中的配材号更新其他构件的配材料号
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->cfgword = pFirstPlate->cfgword;
				pPlate->SetModified(TRUE,FALSE);
				pPlate->SyncMirProp("cfgword");
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_uStatMatCoef"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_uStatMatCoef = atoi(tem_str);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_hPartWeldParent"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_hPartWeldParent=HexStrToLong(tem_str);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bDisplayHole"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
					pPlate->m_bDisplayHole = TRUE;
				else if (valueStr.CompareNoCase("No")==0)
#else 
				if (valueStr.CompareNoCase("是")==0)
					pPlate->m_bDisplayHole = TRUE;
				else if (valueStr.CompareNoCase("否")==0)
#endif
					pPlate->m_bDisplayHole = FALSE;
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->m_bDisplayHole=pPlate->m_bDisplayHole;
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_cPickedDisplayMode"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("None")==0)
					pPlate->m_cPickedDisplayMode = 0;
				else if (valueStr.CompareNoCase("Actual Profile")==0)
					pPlate->m_cPickedDisplayMode = 1;
				else if (valueStr.CompareNoCase("Spread Profile")==0)
					pPlate->m_cPickedDisplayMode = 2;
#else 
				pPlate->m_cPickedDisplayMode=valueStr[0]-'0';
#endif
				IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
				if (pPlate->m_cPickedDisplayMode)
				{
					CLDSView::RedrawPlateProfileDrawing(pPlate);
					g_pSolidDraw->Draw();
					return TRUE;
				}
				else if (pBuddyDrawing)
					pBuddyDrawing->DeleteDbEntityByHiberMasterId(pPlate->handle);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bVirtualPart"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
					pPlate->m_bVirtualPart = TRUE;
				else if (valueStr.CompareNoCase("No")==0)
#else 
				if (valueStr.CompareNoCase("是")==0)
					pPlate->m_bVirtualPart = TRUE;
				else if (valueStr.CompareNoCase("否")==0)
#endif
					pPlate->m_bVirtualPart = FALSE;
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bAttachPartByWeldRoad"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
					pPlate->m_bAttachPartByWeldRoad = TRUE;
				else if (valueStr.CompareNoCase("No")==0)
#else 
				if (valueStr.CompareNoCase("是")==0)
					pPlate->m_bAttachPartByWeldRoad = TRUE;
				else if (valueStr.CompareNoCase("否")==0)
#endif
					pPlate->m_bAttachPartByWeldRoad = FALSE;
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.m_bEnableFlexibleDesign"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
					pPlate->designInfo.m_bEnableFlexibleDesign = TRUE;
				else if (valueStr.CompareNoCase("No")==0)
#else 
				if (valueStr.CompareNoCase("是")==0)
					pPlate->designInfo.m_bEnableFlexibleDesign = TRUE;
				else if (valueStr.CompareNoCase("否")==0)
#endif
					pPlate->designInfo.m_bEnableFlexibleDesign = FALSE;
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->designInfo.m_bEnableFlexibleDesign=pPlate->designInfo.m_bEnableFlexibleDesign;
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bUserSpecColor"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
					pPlate->m_bUserSpecColor = TRUE;
				else if (valueStr.CompareNoCase("No")==0)
#else 
				if (valueStr.CompareNoCase("是")==0)
					pPlate->m_bUserSpecColor = TRUE;
				else if (valueStr.CompareNoCase("否")==0)
#endif
					pPlate->m_bUserSpecColor = FALSE;
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->m_bUserSpecColor=pPlate->m_bUserSpecColor;
				}
			}
			if (pFirstPlate->m_bUserSpecColor)
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("crMaterial"),NULL);
				if (pFindItem==NULL)
				{
					CXhChar100 sText;
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
					lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Part Color";
					lpInfo->m_strPropHelp = "Part's color";
#else 
					lpInfo->m_strPropName = "构件颜色";
					lpInfo->m_strPropHelp = "构件颜色";
#endif
					pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
					pFindItem->m_idProp = CLDSPlate::GetPropID("crMaterial");
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
				}
			}
			else
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("crMaterial"));
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("crMaterial"))
		{
			COLORREF crCurSel=0;
			memmove(tem_str,tem_str+3,97);	//跳过RGB
			sscanf(tem_str,"%X",&crCurSel);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->crMaterial = crCurSel;
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->crMaterial=pPlate->crMaterial;
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("sNotes"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				_snprintf(pPlate->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
				//备注不用关联修改 wht 10-10-22
				/*for(RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
				CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
				if(pMirPlate)
				strcpy(pMirPlate->sNotes,pPlate->sNotes);
				}*/
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_fNormOffset"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->m_fNormOffset = atof(valueStr);
				pPlate->SetModified();
				pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
			g_pSolidDraw->Draw();
		}
		else if ((CLDSPlate::GetPropID("des_para.SCALE_NODE.hLinePart")==pItem->m_idProp)		//基准杆件句柄
			||(CLDSPlate::GetPropID("des_para.SCALE_NODE.hStartNode")==pItem->m_idProp)	//始端节点句柄
			||(CLDSPlate::GetPropID("des_para.SCALE_NODE.hEndNode")==pItem->m_idProp)		//终端节点句柄
			||(CLDSPlate::GetPropID("des_para.SCALE_NODE.start_offset_dist")==pItem->m_idProp) //始端偏移量
			||(CLDSPlate::GetPropID("des_para.SCALE_NODE.end_offset_dist")==pItem->m_idProp)	//终端偏移量
			||(CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_scale")==pItem->m_idProp)		//偏移比例
			||(CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_dist")==pItem->m_idProp))		//在等分基础上的偏移值
		{
			if (CLDSPlate::GetPropID("des_para.SCALE_NODE.hLinePart")==pItem->m_idProp)			//基准杆件句柄
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.hLinePart=HexStrToLong(tem_str);
			else if (CLDSPlate::GetPropID("des_para.SCALE_NODE.hStartNode")==pItem->m_idProp)	//始端节点句柄
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.hStartNode=HexStrToLong(tem_str);
			else if (CLDSPlate::GetPropID("des_para.SCALE_NODE.hEndNode")==pItem->m_idProp)		//终端节点句柄
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.hEndNode=HexStrToLong(tem_str);
			else if (CLDSPlate::GetPropID("des_para.SCALE_NODE.start_offset_dist")==pItem->m_idProp)//始端偏移量
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.start_offset_dist=atof(valueStr);
			else if (CLDSPlate::GetPropID("des_para.SCALE_NODE.end_offset_dist")==pItem->m_idProp)	//终端偏移量
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.end_offset_dist=atof(valueStr);
			else if (CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_scale")==pItem->m_idProp)		//偏移比例
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.offset_scale=atof(valueStr);
			else //if(CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_dist")==pItem->m_idProp)	//在等分基础上的偏移值
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.offset_dist=atof(valueStr);
			pFirstPlate->ucs.origin=pFirstPlate->designInfo.origin.UpdatePos(&Ta);
			pFirstPlate->DesignSetupUcs();
			CLsRef *pLsRef=NULL;
			for (pLsRef=pFirstPlate->GetFirstLsRef();pLsRef;pLsRef=pFirstPlate->GetNextLsRef())
			{	//重新计算螺栓位置
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.origin.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->ucs.origin.x = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.origin.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->ucs.origin.y = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.origin.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->ucs.origin.z = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_x.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_x(atof(valueStr),pPlate->ucs.axis_x.y,pPlate->ucs.axis_x.z);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_x,0);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_x.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_x(pPlate->ucs.axis_x.x,atof(valueStr),pPlate->ucs.axis_x.z);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_x,0);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_x.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_x(pPlate->ucs.axis_x.x,pPlate->ucs.axis_x.y,atof(valueStr));
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_x,0);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_y.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_y(atof(valueStr),pPlate->ucs.axis_y.y,pPlate->ucs.axis_y.z);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_y,1);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_y.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_y(pPlate->ucs.axis_y.x,atof(valueStr),pPlate->ucs.axis_y.z);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_y,1);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_y.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_y(pPlate->ucs.axis_y.x,pPlate->ucs.axis_y.y,atof(valueStr));
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_y,1);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_z.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_z(atof(valueStr),pPlate->ucs.axis_z.y,pPlate->ucs.axis_z.z);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_z,2);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_z.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_z(pPlate->ucs.axis_z.x,atof(valueStr),pPlate->ucs.axis_z.z);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_z,2);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_z.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_z(pPlate->ucs.axis_z.x,pPlate->ucs.axis_z.y,atof(valueStr));
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_z,2);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_fInnerRadius"))
		{
			CXhChar100 sText;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_fInnerRadius = atof(valueStr);
			if (pFirstPlate->m_fInnerRadius>0)
			{
				pParentItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_fInnerRadius"),&pFindItem);
				if (pParentItem)
				{
					pParentItem->m_bHideChildren=TRUE;
					pPropItem=NULL;
					long idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm");
					pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
					if (pFindItem==NULL)
					{
						lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Inner Circle Normal";
						lpInfo->m_strPropHelp = "The normal of inner circle.";
#else 
						lpInfo->m_strPropName = "内环轴线方向";
						lpInfo->m_strPropHelp = "内环轴线方向。";
#endif
						pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
						pPropItem->m_idProp = idProp;
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
						pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
					}
					if (pPropItem)
					{
						idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm.x");
						pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "X ";
							lpInfo->m_strPropHelp = "The X coordinate component of inner circle's normal.";
#else 
							lpInfo->m_strPropName = "内环轴向X分量";
							lpInfo->m_strPropHelp = "内环轴线方向的X分量值。";
#endif
							pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
							pFindItem->m_idProp = idProp;
							if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
								pFindItem->m_lpNodeInfo->m_strPropValue = sText;
							pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
							pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
						}
						idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm.y");
						pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Y ";
							lpInfo->m_strPropHelp = "The Y coordinate component of inner circle's normal.";
#else 
							lpInfo->m_strPropName = "内环轴向Y分量";
							lpInfo->m_strPropHelp = "内环轴线方向的Y分量值。";
#endif
							pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
							pFindItem->m_idProp = idProp;
							if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
								pFindItem->m_lpNodeInfo->m_strPropValue = sText;
							pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
							pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
						}
						idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm.z");
						pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Z ";
							lpInfo->m_strPropHelp = "The Z coordinate component of inner circle's normal.";
#else 
							lpInfo->m_strPropName = "内环轴向Z分量";
							lpInfo->m_strPropHelp = "内环轴线方向的Z分量值。";
#endif
							pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
							pFindItem->m_idProp = idProp;
							if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
								pFindItem->m_lpNodeInfo->m_strPropValue = sText;
							pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
							pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
						}
					}
					//内环位置
					pPropItem=NULL;
					idProp=CLDSPlate::GetPropID("m_tInnerOrigin");
					pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
					if (pFindItem==NULL)
					{
						lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Inner Circle Center Position";
						lpInfo->m_strPropHelp = "The inner circle position is the relative coordinates of plate coordinates.";
#else 
						lpInfo->m_strPropName = "内环位置";
						lpInfo->m_strPropHelp = "内环位置,该坐标为钢板坐标系下的相对坐标。";
#endif
						pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
						pPropItem->m_idProp = idProp;
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
						pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
					}
					if (pPropItem)
					{
						idProp=CLDSPlate::GetPropID("m_tInnerOrigin.x");
						pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Inner Center X Coordinate";
							lpInfo->m_strPropHelp = "The X coordinate component of inner circle's center.";
#else 
							lpInfo->m_strPropName = "内环位置X分量";
							lpInfo->m_strPropHelp = "内环位置的X分量值。";
#endif
							pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
							pFindItem->m_idProp = idProp;
							if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
								pFindItem->m_lpNodeInfo->m_strPropValue = sText;
							pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
							pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
						}
						idProp=CLDSPlate::GetPropID("m_tInnerOrigin.y");
						pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Inner Center Y Coordinate";
							lpInfo->m_strPropHelp = "The Y coordinate component of inner circle's center.";
#else 
							lpInfo->m_strPropName = "内环位置Y分量";
							lpInfo->m_strPropHelp = "内环位置的Y分量值。";
#endif
							pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
							pFindItem->m_idProp = idProp;
							if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
								pFindItem->m_lpNodeInfo->m_strPropValue = sText;
							pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
							pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
						}
						idProp=CLDSPlate::GetPropID("m_tInnerOrigin.z");
						pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Inner Center Z Coordinate";
							lpInfo->m_strPropHelp = "The Z coordinate component of inner circle's center.";
#else 
							lpInfo->m_strPropName = "内环位置Z分量";
							lpInfo->m_strPropHelp = "内环位置的Z分量值。";
#endif
							pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
							pFindItem->m_idProp = idProp;
							if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
								pFindItem->m_lpNodeInfo->m_strPropValue = sText;
							pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
							pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
						}
					}
				}
			}
			else
			{
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.x"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.y"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.z"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.x"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.y"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.z"));
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerColumnNorm.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_tInnerColumnNorm.x = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerColumnNorm.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_tInnerColumnNorm.y = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerColumnNorm.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_tInnerColumnNorm.z = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerOrigin.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_tInnerOrigin.x = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerOrigin.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_tInnerOrigin.y = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerOrigin.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_tInnerOrigin.z = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("iProfileStyle0123"))
		{
			sscanf(valueStr,"%d",&iTemp);
			CUndoOperObject undo(&Ta);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->designInfo.iProfileStyle0123 = iTemp;
				pPlate->DesignPlate();	//修改外形计算方式后重新设计钢板
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_cRollProcessType"))
		{
			BYTE cProcessType= valueStr[0]-'0';
			if (cProcessType==2)
				cProcessType=0xFF;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_cRollProcessType=cProcessType;
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_cRollOffsetType"))
		{
			BYTE cProcessType= valueStr[0]-'0';
			if (cProcessType==2)
				cProcessType=0xFF;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_ciRollOffsetType=cProcessType;
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_cDeformType"))
		{
			BYTE cProcessType= valueStr[0]-'0';
			if (cProcessType==4)
				cProcessType=0xFF;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_ciDeformType=cProcessType;
			//
			pPropList->DeleteAllSonItems(pItem);
			if (cProcessType==1&&pFirstPlate->GetFaceN()>1)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Deform Coef";
#else 
				lpInfo->m_strPropName = "中性层系数";
#endif
				pPropItem = pPropList->InsertItem(pItem,lpInfo,0,true);
				pPropItem->m_idProp = CLDSPlate::GetPropID("m_fDeformCoef");
				if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = tem_str;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
				if (pFirstPlate->GetFaceN()==3)	//三面板
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Deform Coef2";
#else
					lpInfo->m_strPropName = "中性层系数2";
#endif
					pPropItem = pPropList->InsertItem(pItem,lpInfo,-1);
					pPropItem->m_idProp = CLDSPlate::GetPropID("m_fDeformCoef2");
					if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = tem_str;
					pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_fDeformCoef"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_fDeformCoef=atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_fDeformCoef2"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_fDeformCoef2=atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("face_N"))
		{
			iTemp=valueStr[0]-'0';//sscanf(valueStr,"%d",&iTemp);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				long nOldFaceN=pPlate->face_N;
				if (iTemp==4)
				{
					pPlate->face_N = 1;
					pPlate->jdb_style=4;	//螺栓垫板
				}
				else if (iTemp==3)
				{
					pPlate->face_N = 3;
					pPlate->jdb_style=1;	//折叠板
				}
				else
				{
					pPlate->face_N=iTemp+1;
					if (pPlate->designInfo.iFaceType!=6&&pPlate->designInfo.iFaceType!=7)
						pPlate->designInfo.iFaceType=iTemp+1;
					pPlate->jdb_style=0;	//普通钢板
				}
				//面号变小之后,修改关联杆件面号，重新设计钢板外形
				//变电构件顶部八字板之后可能为三面板或双面板,可通过调整面数修改为单面板	wht 16-10-20
				if (pPlate->face_N<nOldFaceN)
				{
					for (CDesignLjPartPara *pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
					{
						if (pLjPara->iFaceNo>pPlate->face_N)
							pLjPara->iFaceNo=1;
					}
				}
				pPlate->DesignPlate();
				pPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("plateProfile"),&pParentItem);
			if (pFindItem)
				Update_Plate_Face_N_TreeItem(pPropList,pFindItem);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bSelfCorrectBendline"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->designInfo.m_bSelfCorrectBendline=(valueStr[0]!='0');
				if (pPlate->face_N>1)
				{
					pPlate->SetModified();
					pPlate->DesignPlate();
					pPlate->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].norm_style"))
		{	//第一火曲线法线定义
			sscanf(valueStr,"%d",&iTemp);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].norm_style = iTemp;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0]"),&pParentItem);
			if (pFindItem)
				Update_Plate_Norm_CalStyle_TreeItem(pPropList,pFindItem,TRUE);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].vector.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].vector.x = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].vector.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].vector.y = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].vector.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].vector.z = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].hVicePart=HexStrToLong(tem_str);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].norm_wing"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("X leg normal")==0)
					pPlate->designInfo.huoqufacenorm[0].norm_wing = 0;
				else if (valueStr.CompareNoCase("Y leg normal")==0)
#else 
				if (valueStr.CompareNoCase("X肢法线")==0)
					pPlate->designInfo.huoqufacenorm[0].norm_wing = 0;
				else if (valueStr.CompareNoCase("Y肢法线")==0)
#endif
					pPlate->designInfo.huoqufacenorm[0].norm_wing = 1;
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].angle"))
		{
			double angle =atof(valueStr)*RADTODEG_COEF;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dLine huoquline;
				pPlate->GetHuoQuAngle(1,&huoquline);
				GEPOINT rotaxis=huoquline.endPt-huoquline.startPt;
				rotaxis.z=0;
				vector_trans(rotaxis,pPlate->ucs,TRUE,TRUE);
				pPlate->huoqufacenorm[0]=pPlate->ucs.axis_z;
				RotateVectorAroundVector(pPlate->huoqufacenorm[0],angle,rotaxis);
				pPlate->designInfo.huoqufacenorm[0].vector=pPlate->huoqufacenorm[0];
				pPlate->SetModified();
				//更新挂线板上的套管及挂线孔壁的位置及实体显示 wjh-2016.11.04
				for (CLsRef* pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					CLDSBolt* pBolt=pLsRef->GetLsPtr();
					if (pBolt->IsLocalCoordLocation()&&pBolt->des_base_pos.reside_plane==1)
					{
						pBolt->correct_worknorm();
						pBolt->correct_pos();
						if (!pBolt->m_bVirtualPart)
						{
							pBolt->Create3dSolidModel();
							g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
						}
						else
						{
							CLdsPartListStack stack(&Ta.Parts);
							HOLE_WALL* pHoleWall = console.GetHoleWall(pBolt,pPlate->handle);
							if (pHoleWall)
							{
								pHoleWall->UpdateSolid();
								g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
							}
							if (pBolt->get_d()+pBolt->hole_d_increment>26)	//有可能是套管孔
							{
								for (CLDSLineTube* pBushTube=(CLDSLineTube*)Ta.Parts.GetFirst(CLS_LINETUBE);pBushTube;pBushTube=(CLDSLineTube*)Ta.Parts.GetNext(CLS_LINETUBE))
								{
									if (pBushTube->FuncType()==CLDSLineTube::FUNC_WIREBUSH&&pBushTube->m_hPartWeldParent==pPlate->handle)
									{
										pBushTube->CalPosition();
										pBushTube->Create3dSolidModel();
										g_pSolidDraw->NewSolidPart(pBushTube->GetSolidPartObject());
										break;
									}
								}
							}
						}
					}
				}
			}
		}
		else  if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].angle"))
		{
			double angle =atof(valueStr)*RADTODEG_COEF;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dLine huoquline;
				pPlate->GetHuoQuAngle(2,&huoquline);
				GEPOINT rotaxis=huoquline.endPt-huoquline.startPt;
				vector_trans(rotaxis,pPlate->ucs,TRUE,TRUE);
				pPlate->huoqufacenorm[1]=pPlate->ucs.axis_z;
				RotateVectorAroundVector(pPlate->huoqufacenorm[1],angle,rotaxis);
				pPlate->designInfo.huoqufacenorm[1].vector=pPlate->huoqufacenorm[1];
				pPlate->SetModified();
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].direction"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].direction = valueStr[0]-'0';
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].hCrossPart=HexStrToLong(tem_str);
		}
		//第二火曲线法线定义
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].norm_style"))
		{
			sscanf(valueStr,"%d",&iTemp);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].norm_style = iTemp;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1]"),&pParentItem);
			if (pFindItem)
				Update_Plate_Norm_CalStyle_TreeItem(pPropList,pFindItem,FALSE);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].vector.x = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].vector.y = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].vector.z = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].hVicePart=HexStrToLong(tem_str);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].norm_wing"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("X leg normal")==0)
					pPlate->designInfo.huoqufacenorm[1].norm_wing = 0;
				else if (valueStr.CompareNoCase("Y leg normal")==0)
#else 
				if (valueStr.CompareNoCase("X肢法线")==0)
					pPlate->designInfo.huoqufacenorm[1].norm_wing = 0;
				else if (valueStr.CompareNoCase("Y肢法线")==0)
#endif
					pPlate->designInfo.huoqufacenorm[1].norm_wing = 1;
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].direction"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].direction = valueStr[0]-'0';
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].hCrossPart=HexStrToLong(tem_str);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bWeldPart"))
		{
			CXhChar100 sText;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
#else 
				if (valueStr.CompareNoCase("是")==0)
#endif
					pPlate->m_bWeldPart=TRUE;
				else //if(valueStr.CompareNoCase("否")==0)
					pPlate->m_bWeldPart=FALSE;
			}
			if (pFirstPlate->m_bWeldPart)
			{
				//指定焊缝线
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("iWeldLineStyle"),&pParentItem);
					if (pFindItem==NULL)
					{
						lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Weld Line Type";
						lpInfo->m_strPropHelp = "Weld line size calculation type";
						lpInfo->m_cmbItems = "0.Calculate freely|1.Specified Weld Line|2.Extention Aligned";
#else 
						lpInfo->m_strPropName = "焊缝线类型";
						lpInfo->m_strPropHelp = "焊缝线尺寸计算类型";
						lpInfo->m_cmbItems = "0.自由计算|1.指定焊缝|2.延伸对齐";
#endif
						pPropItem = pPropList->InsertItem(pItem,lpInfo,0,true);
						pPropItem->m_idProp = CLDSPlate::GetPropID("iWeldLineStyle");
						if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
							pPropItem->m_lpNodeInfo->m_strPropValue = sText;
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
						pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
					}
					if (pPlate->designInfo.iWeldLineStyle==1)
					{
						pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"),&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Weld Line Start Height";
							lpInfo->m_strPropHelp = "The start height of weld line.";
#else 
							lpInfo->m_strPropName = "焊缝线起始高";
							lpInfo->m_strPropHelp = "焊缝线起始高";
#endif
							pPropItem = pPropList->InsertItem(pItem,lpInfo,1,true);
							pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_start_high");
							if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
								pPropItem->m_lpNodeInfo->m_strPropValue = sText;
							pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
						}
						pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"),&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Weld Line End Height";
							lpInfo->m_strPropHelp = "The end height of weld line.";
#else 
							lpInfo->m_strPropName = "焊缝线终止高";
							lpInfo->m_strPropHelp = "焊缝线终止高";
#endif
							pPropItem = pPropList->InsertItem(pItem,lpInfo,2,true);
							pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_end_high");
							if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
								pPropItem->m_lpNodeInfo->m_strPropValue = sText;
							pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
						}
						pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"),&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Weld Width";
							lpInfo->m_strPropHelp = "Weld width";
#else 
							lpInfo->m_strPropName = "焊区宽度";
							lpInfo->m_strPropHelp = "焊区宽度";
#endif
							pPropItem = pPropList->InsertItem(pItem,lpInfo,3,true);
							pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_width");
							if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
								pPropItem->m_lpNodeInfo->m_strPropValue = sText;
							pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
						}
					}
				}
			}
			else
			{
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("iWeldLineStyle"),&pParentItem);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("iWeldLineStyle"));
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"),&pParentItem);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"));
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"),&pParentItem);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"));
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"),&pParentItem);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"));
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("iWeldLineStyle"))
		{
			CXhChar100 sText;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->designInfo.iWeldLineStyle=valueStr[0]-'0';
				if (pPlate->designInfo.iWeldLineStyle==1)
				{
					CPropTreeItem *pFatherItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_bWeldPart"),&pParentItem);
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"),&pParentItem);
					if (pFindItem==NULL)
					{
						lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Weld Line Start Height";
						lpInfo->m_strPropHelp = "The start height of weld line.";
#else 
						lpInfo->m_strPropName = "焊缝线起始高";
						lpInfo->m_strPropHelp = "焊缝线起始高";
#endif
						pPropItem = pPropList->InsertItem(pFatherItem,lpInfo,1,true);
						pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_start_high");
						if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
							pPropItem->m_lpNodeInfo->m_strPropValue = sText;
						pPropItem->m_dwPropGroup=pFatherItem->m_dwPropGroup;
					}
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"),&pParentItem);
					if (pFindItem==NULL)
					{
						lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Weld Line End Height";
						lpInfo->m_strPropHelp = "The end height of weld line.";
#else 
						lpInfo->m_strPropName = "焊缝线终止高";
						lpInfo->m_strPropHelp = "焊缝线终止高";
#endif
						pPropItem = pPropList->InsertItem(pFatherItem,lpInfo,2,true);
						pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_end_high");
						if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
							pPropItem->m_lpNodeInfo->m_strPropValue = sText;
						pPropItem->m_dwPropGroup=pFatherItem->m_dwPropGroup;
					}
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"),&pParentItem);
					if (pFindItem==NULL)
					{
						lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Weld Width";
						lpInfo->m_strPropHelp = "Weld width";
#else 
						lpInfo->m_strPropName = "焊区宽度";
						lpInfo->m_strPropHelp = "焊区宽度";
#endif
						pPropItem = pPropList->InsertItem(pFatherItem,lpInfo,3,true);
						pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_width");
						if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
							pPropItem->m_lpNodeInfo->m_strPropValue = sText;
						pPropItem->m_dwPropGroup=pFatherItem->m_dwPropGroup;
					}
				}
				else
				{
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"),&pParentItem);
					if (pFindItem)
						pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"));
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"),&pParentItem);
					if (pFindItem)
						pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"));
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"),&pParentItem);
					if (pFindItem)
						pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"));
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bNeedFillet"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
#else 
				if (valueStr.CompareNoCase("是")==0)
#endif
					pPlate->m_bNeedFillet=TRUE;
				else //if(valueStr.CompareNoCase("否")==0)
					pPlate->m_bNeedFillet=FALSE;
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.weld_start_high"))
		{
			CUndoOperObject undo(&Ta);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->designInfo.weld_start_high = atof(valueStr);
				pPlate->DesignPlate();
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.weld_end_high"))
		{
			CUndoOperObject undo(&Ta);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->designInfo.weld_end_high = atof(valueStr);
				pPlate->DesignPlate();
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.weld_width"))
		{
			CUndoOperObject undo(&Ta);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->designInfo.weld_width = atof(valueStr);
				pPlate->DesignPlate();
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.origin.x"))
		{
			bUpdateWcs=TRUE;
			PLATEEDIT_STATE* pEditState=CLDSPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
			pEditState->m_fLocalWcsOrgX=console.m_curWCS.origin.x=atof(valueStr);
			//
			oper.UpdatePropItemValue("wcs.origin");
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.origin.y"))
		{
			bUpdateWcs=TRUE;
			PLATEEDIT_STATE* pEditState=CLDSPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
			pEditState->m_fLocalWcsOrgY=console.m_curWCS.origin.y=atof(valueStr);
			//
			oper.UpdatePropItemValue("wcs.origin");
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.axis_x.a"))
		{
			bUpdateWcs=TRUE;
			double alfa=atof(valueStr)*RADTODEG_COEF;	//由角度制转为弧度制
			PLATEEDIT_STATE* pEditState=CLDSPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
			console.m_curWCS.axis_x=GEPOINT(cos(alfa),sin(alfa));
			console.m_curWCS.axis_y.Set(-console.m_curWCS.axis_x.y,console.m_curWCS.axis_x.x);
			pEditState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
			pEditState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
			pEditState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
			pEditState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
			//
			oper.UpdatePropItemValue("wcs.axis_x");
			oper.UpdatePropItemValue("wcs.axis_y");
		}
		if (pItem->m_idProp==CLDSPlate::GetPropID("peakVertexOffsetY")||
			pItem->m_idProp==CLDSPlate::GetPropID("extendVertexOffsetLen")||
			pItem->m_idProp==CLDSPlate::GetPropID("peakPointMargin"))
		{
			if (pItem->m_idProp==CLDSPlate::GetPropID("peakVertexOffsetY"))
				pFirstPlate->peakVertexOffsetY=atof(valueStr);
			else if (pItem->m_idProp==CLDSPlate::GetPropID("extendVertexOffsetLen"))
				pFirstPlate->extendVertexOffsetLen=atof(valueStr);
			else if (pItem->m_idProp==CLDSPlate::GetPropID("peakPointMargin"))
				pFirstPlate->peakPointMargin=atof(valueStr);
			pFirstPlate->DesignPlate();
			CLDSView::RedrawPlateProfileDrawing(pFirstPlate);
		}
		if (bUpdateWcs==TRUE)
		{
			UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
			g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
			g_pSolidDraw->Draw();
		}
		for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
		{
			pPlate->SetModified();
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		}
	}
	catch (char *sError)
	{
		AfxMessageBox(sError);
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

CPropTreeItem* InsertOrUpdateItem(CPropertyList* pPropList,PROPLIST_ITEM* pPropItem,CPropTreeItem *pParent,int controlType/*=0*/,
	const char* valueText/*=NULL*/,int iInsertIndex/*=-1*/,BOOL bUpdate/*=0*/)
{
	if (pPropItem==NULL)
		return NULL;
	CPropTreeItem* pItem=pPropList->FindItemByPropId(pPropItem->id,NULL);
	if (pItem==NULL)
	{
		CItemInfo* lpInfo = new CItemInfo();
		lpInfo->m_strPropName=pPropItem->name;
		lpInfo->m_strPropHelp=pPropItem->propHelpStr;
		lpInfo->m_controlType=controlType;
		if (lpInfo->m_controlType==PIT_COMBO)
			lpInfo->m_cmbItems=pPropItem->propCmbItemsStr;
		if (valueText)
			lpInfo->m_strPropValue = valueText;
		pItem=pPropList->InsertItem(pParent,lpInfo,iInsertIndex,bUpdate);
		pItem->m_idProp=pPropItem->id;
		pItem->m_dwPropGroup=pParent->m_dwPropGroup;
	}
	else if (valueText!=NULL)
	{
		pItem->m_lpNodeInfo->m_strPropName = pPropItem->name;
		pItem->m_lpNodeInfo->m_strPropHelp = pPropItem->propHelpStr;
		pItem->m_lpNodeInfo->m_strPropValue = valueText;
		if (bUpdate&&!pItem->IsHideSelf())
		{
			RECT rc;
			pPropList->GetItemRect(pItem->m_iIndex,&rc);
			pPropList->InvalidateRect(&rc);
		}
	}
	return pItem;
}
CPropTreeItem* InsertOrUpdatePlateParamItem(CPropertyList *pPropList,CPropTreeItem *pParent,const char* propKey,CLDSParamPlate *pPlate,
	int controlType=0,BOOL bUpdateValueText=TRUE,int iInsertIndex=-1,BOOL bUpdate=0)
{
	PROPLIST_ITEM* pPropItem=CLDSParamPlate::propHashtable.GetValue(propKey);
	if (pPropItem==NULL)
	{
		DYNA_PROPITEM_GROUP *pGroup=CLDSParamPlate::hashParamItemPropGroup.GetValue(pPlate->m_iParamType);
		if (pGroup)
		{
			long id=pGroup->GetPropID(propKey);
			pPropItem=pGroup->GetPropItem(id);
		}
	}
	if (pPropItem==NULL)
		return NULL;
	CXhChar50 sText;
	CPropTreeItem* pItem;
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if (bUpdateValueText&&pPlate->GetPropValueStr(pPropItem->id,sText)>0)
		pItem=::InsertOrUpdateItem(pPropList,pPropItem,pParent,controlType,sText,iInsertIndex,bUpdate);
	else
		pItem=::InsertOrUpdateItem(pPropList,pPropItem,pParent,controlType,NULL,iInsertIndex,bUpdate);
	pItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(pView->selectObjs,pPropItem->id);
	return pItem;
}
//塔脚(或牛蹄板)底板
void RebuildOrUpdateBasePlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem *pSonPropItem=NULL,*pPropItem=NULL;
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"S",pCurPlate,PIT_COMBO);
	long shapeType=0;
	pCurPlate->GetDesignItemValue(KEY2C("S"),&shapeType);
	if (0==shapeType)
	{
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("R",pCurPlate->m_iParamType));
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"XA",pCurPlate,PIT_EDIT,TRUE,0,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"XB",pCurPlate,PIT_EDIT,TRUE,1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"YA",pCurPlate,PIT_EDIT,TRUE,2,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"YB",pCurPlate,PIT_EDIT,TRUE,3,TRUE);
		pSonPropItem=InsertOrUpdatePlateParamItem(pPropList,pPropItem,"X2",pCurPlate,PIT_EDIT,TRUE,4,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pSonPropItem,"Y2",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pSonPropItem=InsertOrUpdatePlateParamItem(pPropList,pPropItem,"X3",pCurPlate,PIT_EDIT,TRUE,5,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pSonPropItem,"Y3",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pSonPropItem=InsertOrUpdatePlateParamItem(pPropList,pPropItem,"X1",pCurPlate,PIT_EDIT,TRUE,6,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pSonPropItem,"Y1",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pSonPropItem=InsertOrUpdatePlateParamItem(pPropList,pPropItem,"X0",pCurPlate,PIT_EDIT,TRUE,7,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pSonPropItem,"Y0",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("XA",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("XB",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("YA",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("YB",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("X0",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("X1",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("X2",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("X3",pCurPlate->m_iParamType));
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"R",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
}
BOOL ModifyBasePlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	CXhChar100 tem_str("%s",valueStr);
	KEY2C paramKey=CLDSParamPlate::GetPropKey(pItem->m_idProp,pPlate->m_iParamType);
	if (KEY2C("S")==paramKey)
	{	//底板外形类型
		long iType=valueStr[0]-'0';
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey,iType);
		//更新属性栏
		RebuildOrUpdateBasePlateParamsPropertyList(pPropList,pItem->m_pParent);
	}
	else
		return FALSE;
	return TRUE;
}
//塔脚(或牛蹄板)靴板
void RebuildOrUpdateShoePlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	long iConnStyle=0,shapeType=0,hLinkXieAngle=0;
	pCurPlate->GetDesignItemValue(KEY2C("LX"),&hLinkXieAngle);
	pCurPlate->GetDesignItemValue(KEY2C("F"),&shapeType);	//外形类别 0.贯穿底板|1.肢翼单侧|2.楞线单侧
	pCurPlate->GetDesignItemValue(KEY2C("CS"),&iConnStyle);	//0.螺栓连接|1.对焊连接
	//
	CPropTreeItem* pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"F",pCurPlate,PIT_COMBO);
	pItem->SetReadOnly();
	if (shapeType!=1)
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
	}
	if (shapeType!=2)
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"IA",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"IB",pCurPlate,PIT_EDIT);
	}
	if (hLinkXieAngle>0X20)
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"IC",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"ID",pCurPlate,PIT_EDIT);
	}
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"M",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"AL",pCurPlate,PIT_EDIT);
	if (0==shapeType)
	{
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("CM",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("L",pCurPlate->m_iParamType));
	}
	else
	{
		long CW=0;
		pCurPlate->GetDesignItemValue(KEY2C("L"),&CW);
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"L",pCurPlate,PIT_EDIT);
		if (CW>0)
			InsertOrUpdatePlateParamItem(pPropList,pItem,"CM",pCurPlate,PIT_COMBO);
	}
	//对焊模式下的楞线侧靴板
	if (iConnStyle==1 && shapeType==2)
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"NL",pCurPlate,PIT_EDIT);
	else
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("NL",pCurPlate->m_iParamType));
}
BOOL ModifyShoePlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	KEY2C paramKey=CLDSParamPlate::GetPropKey(pItem->m_idProp,pPlate->m_iParamType);
	if (KEY2C("F")==paramKey||KEY2C("W")==paramKey||KEY2C("CS")==paramKey||KEY2C("CM")==paramKey)
	{	//底板外形类型
		long iType=valueStr[0]-'0';
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey,iType);
		//更新属性栏
		if (KEY2C("F")==paramKey)
			RebuildOrUpdateShoePlateParamsPropertyList(pPropList,pItem->m_pParent);
	}
	else if (KEY2C("L")==paramKey)
	{
		long CW=atoi(valueStr);
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey,CW);
		//更新属性栏
		CLDSParamPlate* pFirstParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();
		if (CW==0)
			pPropList->DeleteAllSonItems(pItem);
		else
			InsertOrUpdatePlateParamItem(pPropList,pItem,"e",pFirstParamPlate,PIT_COMBO,TRUE,-1,TRUE);
	}
	else
		return FALSE;
	return TRUE;
}
//角钢肋板
void RebuildOrUpdateAngleRibPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	long plateType=0,edgeType=0;
	pCurPlate->GetDesignItemValue(KEY2C("C"),&edgeType);
	pCurPlate->GetDesignItemValue(KEY2C("I"),&plateType);	//外形类别 0.内侧肋板|1.外侧肋板
	//InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_COMBO);
	//InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
	//InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
	//InsertOrUpdatePlateParamItem(pPropList,pParentItem,"S",pCurPlate,PIT_EDIT);
	//InsertOrUpdatePlateParamItem(pPropList,pParentItem,"L",pCurPlate,PIT_EDIT);
	if (plateType==0)
	{	//内侧角钢肋板
		double d=0;
		pCurPlate->GetDesignItemValue(KEY2C("d"),&d);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"L",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Ly",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Wy",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList, pParentItem,"E",pCurPlate,PIT_COMBO, TRUE, -1, TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"S",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"C",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		CPropTreeItem* pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"d",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		if (d>0)
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"P",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			if (edgeType>0)
				InsertOrUpdatePlateParamItem(pPropList,pItem,"R",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
	}
	else if (plateType==1)
	{	//外侧角钢肋板
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Hy",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Wy",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		CPropTreeItem* pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"S",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pItem->m_lpNodeInfo->m_strPropName="倒角圆弧R";
	}
	else if (plateType==2)
	{	//纵向角钢肋板
		CPropTreeItem* pItem=NULL;
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pItem->m_lpNodeInfo->m_strPropName="延伸偏移量";
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"L",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pItem->m_lpNodeInfo->m_strPropName="肋板长度L";
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pItem->m_lpNodeInfo->m_strPropName="肋板长度W";
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pItem->m_lpNodeInfo->m_strPropName="倒角圆弧R";
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"d",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
}
BOOL ModifyAngleRibPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CXhChar100 sText;
	CLDSParamPlate *pPlate=NULL,*pFirstPlate=(CLDSParamPlate*)selectObjs.GetFirst();
	KEY2C paramKey=CLDSParamPlate::GetPropKey(pItem->m_idProp,pFirstPlate->m_iParamType);
	if (KEY2C("Lr")==paramKey)
	{
		long hAngle=0;
		double fLr=atof(valueStr);
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			pPlate->GetDesignItemValue(KEY2C("A"),&hAngle);
			CLDSLineAngle *pAngle=(CLDSLineAngle*)console.FromPartHandle(hAngle,CLS_LINEANGLE);
			if (pAngle)
			{
				pPlate->SetDesignItemValue(KEY2C("L"),pAngle->GetWidth()-fLr);
				long idProp=CLDSParamPlate::GetPropID("L",pPlate->m_iParamType);
				if (pPlate->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,CString(sText));
			}
		}
	}
	else if (KEY2C("L")==paramKey)
	{
		double fValue=atof(valueStr);
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			//{
			pPlate->SetDesignItemValue(paramKey,fValue);
		//	long idProp=CLDSParamPlate::GetPropID("Lr",pPlate->m_iParamType);
		//	if(pPlate->GetPropValueStr(idProp,sText)>0)
		//		pPropList->SetItemPropValue(idProp,CString(sText));
		//}
	}
	/*else if(KEY2C("I")==paramKey)
	{	//底板外形类型
		long iOldType=0,iType=valueStr[0]-'0';
		pFirstPlate->GetDesignItemValue(KEY2C("I"),&iOldType);
		for(pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			pPlate->SetDesignItemValue(paramKey,iType);
			if(iOldType!=iType)
			{
				if(iType==0)
				{
					if(pPlate->GetDesignItem(KEY2C("C"))==NULL)
						pPlate->SetDesignItemValue(KEY2C("C"),0);
					if(pPlate->GetDesignItem(KEY2C("P"))==NULL)
						pPlate->SetDesignItemValue(KEY2C("P"),0);
					if(pPlate->GetDesignItem(KEY2C("d"))==NULL)
						pPlate->SetDesignItemValue(KEY2C("d"),0);
					if(pPlate->GetDesignItem(KEY2C("R"))==NULL)
						pPlate->SetDesignItemValue(KEY2C("R"),0);
				}
				else
				{
					if(pPlate->GetDesignItem(KEY2C("H"))==NULL)
						pPlate->SetDesignItemValue(KEY2C("H"),0);
				}
			}
		}
		//更新属性栏
		if(iOldType!=iType)
		{
			CPropTreeItem *pParentItem=pItem->m_pParent;
			if(iType==0)
			{
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("H",pFirstPlate->m_iParamType));
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Lr",pFirstPlate,PIT_EDIT,TRUE,5,TRUE);
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"C",pFirstPlate,PIT_COMBO,TRUE,6,TRUE);
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"P",pFirstPlate,PIT_EDIT,TRUE,7,TRUE);
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"d",pFirstPlate,PIT_EDIT,TRUE,8,TRUE);
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pFirstPlate,PIT_EDIT,TRUE,9,TRUE);
			}
			else
			{
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("Lr",pFirstPlate->m_iParamType));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("C",pFirstPlate->m_iParamType));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("P",pFirstPlate->m_iParamType));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("d",pFirstPlate->m_iParamType));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("R",pFirstPlate->m_iParamType));
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pFirstPlate,PIT_EDIT,TRUE,5,TRUE);
			}
		}
	}*/
	else if (KEY2C("C")==paramKey)
	{
		long type=valueStr[0]-'0';
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey,type,NULL,PARAM_SHAPE,1);
		//
		CPropTreeItem* pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("d",pFirstPlate->m_iParamType),NULL);
		if (pFindItem==NULL)
			return FALSE;
		pPropList->DeleteAllSonItems(pFindItem);
		double d=0;
		pFirstPlate->GetDesignItemValue(KEY2C("d"),&d);
		if (d>0)
		{
			InsertOrUpdatePlateParamItem(pPropList,pFindItem,"P",pFirstPlate,PIT_EDIT,TRUE,-1,TRUE);
			if (type>0)
				InsertOrUpdatePlateParamItem(pPropList,pFindItem,"R",pFirstPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
	}
	else if (KEY2C("E") == paramKey)
	{
		long type = valueStr[0] - '0';
		for (pPlate = (CLDSParamPlate*)selectObjs.GetFirst(); pPlate; pPlate = (CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey, type, NULL, PARAM_SHAPE, 1);
	}
	else if (KEY2C("d")==paramKey)
	{
		double d=atof(valueStr);
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey,d);
		//
		long edgeType=0,I=0;
		pFirstPlate->GetDesignItemValue(KEY2C("C"),&edgeType);
		pFirstPlate->GetDesignItemValue(KEY2C("I"),&I);
		pPropList->DeleteAllSonItems(pItem);
		if (d>0 && I==0)
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"P",pFirstPlate,PIT_EDIT,TRUE,-1,TRUE);
			if (edgeType>0)
				InsertOrUpdatePlateParamItem(pPropList,pItem,"R",pFirstPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
	}
	else
		return FALSE;
	return TRUE;
}
//钢管挂线板
void RebuildOrUpdateStayWirePlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"L",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"N",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"d",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"P",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
}
struct RIB_PLATE
{	//钢板基本属性
	static const BYTE TYPE_IN_CORNER = 0;
	static const BYTE TYPE_IN_SLOT  = 1;
	static const BYTE TYPE_ON_ANGLE = 2;
	static const BYTE TYPE_ON_PLATE = 3;
	static const BYTE TYPE_ON_TUBE  = 4;
	BYTE m_cPlateType;	//肋板类型 0.角肋板 1.槽肋板 2.角钢肋板 3.钢板肋板 4.钢管肋板 5.圆塔脚对角肋板 6.圆塔脚外侧环向布置的肋板
	CLDSNode *m_pDatumNode;
	CLDSBolt *m_pDatumBolt;
	CSuperSmartPtr<CLDSPart> m_pDatumPart,m_pRelaPart1,m_pRelaPart2,m_pVecPart;
	struct tagTUBE_PARA
	{	//基准构件为钢管时使用
		int iDatumEdge;				//0.基准边 1.焊道线 2.任意边(可调整工作法线)
		int iWorkNormStyle;			//0.垂直钢管延伸方向 1.钢管轴向 2.自定义
		BYTE cLeftTopCornerType;	//Y+顶部边角类型 0.倒角 1.圆弧 2.直角
		double fLeftTopCornerWidth;	//倒角时表示切角宽度，圆弧时表示切弧半径
		double fLeftTopCornerHeight;//倒角时表示切角高度，圆弧时表示切弧半径
		BYTE cRightTopCornerType;	//Y-顶部边角类型 0.倒角 1.圆弧 2.直角
		double fRightTopCornerWidth;//倒角时表示切角宽度，圆弧时表示切弧半径
		double fRightTopCornerHeight;//倒角时表示切角高度，圆弧时表示切弧半径
		tagTUBE_PARA() { memset(this,0,sizeof(tagTUBE_PARA)); }
	}tube;
	struct tagPLATE_PARA
	{	//基准构件为钢板时使用
		BOOL bSpecOrg;				//指定坐标原点与指定定位基准构件二选一
		BOOL bSpecAxisX;			//钢板肋板指定X轴方向与指定构件方向杆件二选一，
		BOOL bSpecAxisY;			//与底板倾斜角度（即Y轴与底板间绕X轴的旋转角与方向基准杆件二选一）
		BOOL bOnDatumPlateTopFace;	//基准构件为钢板时使用
		BOOL bAlongVecPart;			//钢板延伸方向与方向杆件同向
		BOOL bHashBendPlane;		//是否有制弯面
		tagPLATE_PARA() { memset(this,0,sizeof(tagPLATE_PARA)); }
	}plate;
	BOOL bOnRelaPlateTopFace;		//一块辅助钢板表示肋板位于钢板顶面一侧，无辅助构件表示与钢管延伸方向相同
	RIB_PLATE() { memset(this,0,sizeof(RIB_PLATE)); }
};
static BOOL InitRibPlatePara(RIB_PLATE &xRibPlate,CLDSParamPlate *pRibPlate)
{
	if (pRibPlate==NULL||pRibPlate->m_iParamType!=TYPE_RIBPLATE)
		return FALSE;
	//初始化定位参数
	long hDatumPart=0,hNode=0,hDatumBolt=0,hRelaPart1=0,hRelaPart2=0,hVecPart=0;
	pRibPlate->GetDesignItemValue('A',&hDatumPart);
	pRibPlate->GetDesignItemValue('B',&hNode);
	pRibPlate->GetDesignItemValue('C',&hRelaPart1);
	pRibPlate->GetDesignItemValue('D',&hRelaPart2);
	pRibPlate->GetDesignItemValue('O',&hDatumBolt);
	pRibPlate->GetDesignItemValue('V',&hVecPart);
	xRibPlate.m_pDatumPart=console.FromPartHandle(abs(hDatumPart));
	xRibPlate.m_pDatumNode=console.FromNodeHandle(hNode);
	xRibPlate.m_pDatumBolt=(CLDSBolt*)console.FromPartHandle(hDatumBolt,CLS_BOLT);
	xRibPlate.m_pRelaPart1=console.FromPartHandle(abs(hRelaPart1));
	xRibPlate.m_pRelaPart2=console.FromPartHandle(abs(hRelaPart2));
	xRibPlate.m_pVecPart=console.FromPartHandle(abs(hVecPart));
	if (xRibPlate.m_pRelaPart1.IsNULL()&&xRibPlate.m_pRelaPart1.IsNULL())
	{
		if (xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			xRibPlate.bOnRelaPlateTopFace=(hDatumPart>0);
		else
			xRibPlate.plate.bOnDatumPlateTopFace=(hDatumPart>0);
		//
		if (xRibPlate.m_pVecPart.IsHasPtr()&&xRibPlate.m_pVecPart->IsLinePart())
			xRibPlate.plate.bAlongVecPart=(hVecPart>0);
	}
	else if (xRibPlate.m_pRelaPart1.IsHasPtr())
	{
		xRibPlate.bOnRelaPlateTopFace=(hRelaPart1>0);
		xRibPlate.plate.bOnDatumPlateTopFace=(hDatumPart>0);
	}
	//初始化外形参数
	if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
	{	//基准构件为钢管
		long h=0;
		pRibPlate->GetDesignItemValue('J',&h);
		xRibPlate.tube.iDatumEdge=(int)h;
		pRibPlate->GetDesignItemValue(KEY2C("Nt"),&h);
		xRibPlate.tube.iWorkNormStyle=(int)h;
		pRibPlate->GetDesignItemValue(KEY2C("Lt"),(long*)&xRibPlate.tube.cLeftTopCornerType);
		pRibPlate->GetDesignItemValue(KEY2C("Lw"),&xRibPlate.tube.fLeftTopCornerWidth);
		pRibPlate->GetDesignItemValue(KEY2C("Lh"),&xRibPlate.tube.fLeftTopCornerHeight);
		pRibPlate->GetDesignItemValue(KEY2C("Rt"),(long*)&xRibPlate.tube.cRightTopCornerType);
		pRibPlate->GetDesignItemValue(KEY2C("Rw"),&xRibPlate.tube.fRightTopCornerWidth);
		pRibPlate->GetDesignItemValue(KEY2C("Rh"),&xRibPlate.tube.fRightTopCornerHeight);
	}
	else if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->IsPlate())
	{
		double fValue=0;
		BOOL bSpecOrg=TRUE,bSpecAxisX=TRUE,bSpecAxisY=TRUE;
		bSpecOrg=bSpecOrg&&pRibPlate->GetDesignItemValue(KEY2C("Ox"),&fValue);
		bSpecOrg=bSpecOrg&&pRibPlate->GetDesignItemValue(KEY2C("Oy"),&fValue);
		bSpecAxisX=bSpecAxisX&&pRibPlate->GetDesignItemValue(KEY2C("Xx"),&fValue);
		bSpecAxisX=bSpecAxisX&&pRibPlate->GetDesignItemValue(KEY2C("Xy"),&fValue);
		bSpecAxisY=bSpecAxisY&&pRibPlate->GetDesignItemValue(KEY2C("Ga"),&fValue);
		xRibPlate.plate.bSpecOrg=bSpecOrg;
		xRibPlate.plate.bSpecAxisX=bSpecAxisX;
		xRibPlate.plate.bSpecAxisY=bSpecAxisY;
		//----------VVVVVVVVVVVVVVVVVVVVV-----------扩展功能后的新增参数
		CLDSParamPlate::DESIGN_PARAM Ex,Ey,Bx,By,hTipBolt,Tx,Ty,hBendVecPart;
		pRibPlate->GetDesignItemValue(KEY2C("Ex"),&Ex);		//ZO->E为肋板基准面的走向始末基点
		pRibPlate->GetDesignItemValue(KEY2C("Ey"),&Ey);		//ZO->E为肋板基准面的走向始末基点
		pRibPlate->GetDesignItemValue(KEY2C("TB"),&hTipBolt); //制弯面末端T点的基准螺栓
		pRibPlate->GetDesignItemValue(KEY2C("Tx"),&Tx);		//制弯面末端T点的用户输入定义值
		pRibPlate->GetDesignItemValue(KEY2C("Ty"),&Ty);		//制弯面末端T点的用户输入定义值
		pRibPlate->GetDesignItemValue(KEY2C("BV"),&hBendVecPart);//'E'->T为带制弯肋板基面终端火曲点E'->制弯面末端T点
		pRibPlate->GetDesignItemValue(KEY2C("Bx"),&Bx);
		pRibPlate->GetDesignItemValue(KEY2C("By"),&By);
		if ((Tx.bExist&&Ty.bExist)||hTipBolt.bExist)
			xRibPlate.plate.bHashBendPlane=TRUE;
		else
			xRibPlate.plate.bHashBendPlane=FALSE;
	}
	if (xRibPlate.m_pDatumPart.IsNULL())
		return FALSE;
	if (xRibPlate.m_pDatumPart->IsLinePart()&&xRibPlate.m_pRelaPart1.IsNULL()
		&&xRibPlate.m_pRelaPart2.IsNULL()&&xRibPlate.m_pDatumNode==NULL)
	{
		//AfxMessageBox("缺少肋板定位基准节点！");
		return FALSE;
	}
	if (xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE
		&&(xRibPlate.m_pRelaPart1.IsNULL()||xRibPlate.m_pRelaPart1->GetClassTypeId()==CLS_LINETUBE)
		&&xRibPlate.m_pRelaPart2.IsNULL()&&xRibPlate.m_pDatumNode==NULL)
	{
		//AfxMessageBox("钢板夹板缺少定位基准节点！");
		return FALSE;
	}
	//初始化钢板类型
	if (xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsNULL())
		xRibPlate.m_cPlateType=0;			//0.角肋板
	else if (xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsHasPtr())
	{
		xRibPlate.m_cPlateType=1;			//1.槽肋板
		BOOL bSpecRayAngle=(pRibPlate->GetDesignItem(KEY2C("Ra"))!=NULL);
		BOOL bSpecRayDist=(pRibPlate->GetDesignItem(KEY2C("Rd"))!=NULL);
		if (bSpecRayAngle)
		{
			if (bSpecRayDist)
				xRibPlate.m_cPlateType=6;
			else
				xRibPlate.m_cPlateType=5;
		}
	}
	else if (xRibPlate.m_pRelaPart1.IsNULL()&&xRibPlate.m_pRelaPart2.IsNULL())
	{
		if (xRibPlate.m_pDatumPart->IsAngle())
			xRibPlate.m_cPlateType=2;		//2.角钢肋板
		else if (xRibPlate.m_pDatumPart->IsPlate())
			xRibPlate.m_cPlateType=3;		//3.钢板肋板
		else if (xRibPlate.m_pDatumPart->IsTube())
			xRibPlate.m_cPlateType=4;		//4.钢管肋板
	}
	return TRUE;
}
//

bool CorrectRibPlatePropItemName(RIB_PLATE &xRibPlate,const char* propKey,CItemInfo* lpInfo)
{
	if (stricmp(propKey,"RibPlate.plate.bSpecAxisX")==0)
	{
		if (xRibPlate.m_cPlateType==3)
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Along Rod|1.Direct to Bolt|2.Direct to Point|3.Input Directly";
		else
			lpInfo->m_cmbItems="0.Calculate Automatically|1.Input Directly";
#else
			lpInfo->m_cmbItems="0.沿杆件走向|1.指向螺栓|2.定向点|3.直接输入";
		else
			lpInfo->m_cmbItems="0.定向构件|1.直接输入";
#endif
	}
	else if (stricmp(propKey,"RibPlate.plate.bSpecAxisY")==0)
	{
		if (xRibPlate.m_cPlateType!=3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y axis";
			lpInfo->m_cmbItems="0.Direction Rod|1.Inclination Angle";
			lpInfo->m_strPropHelp = "determine axis Y direction, through axis Y direction rod or the inclination angle（degree measure)input.";
#else
			lpInfo->m_strPropName = "Y轴方向";
			lpInfo->m_cmbItems="0.方向杆件|1.倾斜角度";
			lpInfo->m_strPropHelp = "通过Y轴方向杆件或输入的倾斜角度(角度制)，确定Y轴方向。";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y axis";
			lpInfo->m_cmbItems="0.Vertical Basic Plane|1.Inclination Angle";
			lpInfo->m_strPropHelp = "determine axis Y direction, through inputting inclination angle（degree measure).";
#else
			lpInfo->m_strPropName = "Y轴方向";
			lpInfo->m_cmbItems="0.垂直基板|1.倾斜角度";
			lpInfo->m_strPropHelp = "通过输入的倾斜角度(角度制)，确定Y轴方向。";
#endif
		}
	}
	else if (stricmp(propKey,"RibPlate.plate.bOnRelaPlateTopFace")==0)
	{
		if (xRibPlate.m_pRelaPart1.IsNULL()&&xRibPlate.m_pRelaPart2.IsNULL())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName = "same direction with tube";
			lpInfo->m_strPropHelp = "rib plate's axis X has the same direction with tube extending direction";
#else
			lpInfo->m_cmbItems="是|否";
			lpInfo->m_strPropName = "与钢管同向";
			lpInfo->m_strPropHelp = "肋板X轴方向与钢管延伸方向同向。";
#endif
		}
		else if (xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsNULL()&&xRibPlate.m_pRelaPart1->IsPlate())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName = "At plate front side";
			lpInfo->m_strPropHelp = "rib plate is on the top side of the supplementary plate";
#else
			lpInfo->m_cmbItems="是|否";
			lpInfo->m_strPropName = "位于辅助钢板正侧";
			lpInfo->m_strPropHelp = "肋板在辅助钢板顶面一侧。";
#endif
		}
	}
	else if (stricmp(propKey,"RibPlate.fHeight")==0)
	{
		if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Parameter C";
			lpInfo->m_strPropHelp = "Heigth of step";//top width of rib plate, that is, the length of edge connected with supplementary plate 2, initial value automatically extends to the outer edge of supplementary plate 2.";
#else
			lpInfo->m_strPropName = "台阶高度C";
			lpInfo->m_strPropHelp = "台阶高度";
#endif

		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "top width D";
			lpInfo->m_strPropHelp = "Width of rib plate";
#else
			lpInfo->m_strPropName = "顶部宽度D";
			lpInfo->m_strPropHelp = "肋板顶部宽度";
#endif
		}
	}
	else if (stricmp(propKey,"RibPlate.fTopWidth")==0)
	{
		if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "top width D";
			lpInfo->m_strPropHelp = "Width of rib plate, that is, the length of edge connected with supplementary plate 2, initial value automatically extends to the outer edge of supplementary plate 2.";
#else
			lpInfo->m_strPropName = "顶部宽度D";
			lpInfo->m_strPropHelp = "肋板顶部宽度,即与辅助钢板2相连边的的边长,初始值自动延伸至辅助钢板2的外侧边缘。";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Parameter C";
			lpInfo->m_strPropHelp = "Heigth of step";
#else
			lpInfo->m_strPropName = "台阶高度C";
			lpInfo->m_strPropHelp = "台阶高度";
#endif
		}
	}
	else if (stricmp(propKey,"E")==0)
	{
		if (xRibPlate.m_pRelaPart1.IsNULL()&&xRibPlate.m_pRelaPart2.IsNULL())
		{
			if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "rib plate width";
				lpInfo->m_strPropHelp = "Rib plate width, that is, the length of edge which is vertical to datum part.";
#else
				lpInfo->m_strPropName = "肋板宽度";
				lpInfo->m_strPropHelp = "肋板宽度,即垂直于基准构件的边的长度。";
#endif
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "rib plate's height H";
				lpInfo->m_strPropHelp = "height of rib plate, that is, the length of edge which is vertical to datum part.";
#else
				lpInfo->m_strPropName = "肋板高度H";
				lpInfo->m_strPropHelp = "肋板高度,即垂直于基准构件的边的长度。";
#endif
			}
		}
		else
		{
			if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "bottom width B";
				lpInfo->m_strPropHelp = "bottom width,that is, length of edge connected with supplementary plate, initial value automatically extends to the outer edge of supplementary plate.";
#else
				lpInfo->m_strPropName = "底部宽度B";
				lpInfo->m_strPropHelp = "即与辅助钢板相连边的边长,初始值自动延伸到辅助钢板外侧边缘。";
#endif
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Parameter A";
				lpInfo->m_strPropHelp = "height of rib plate";
#else
				lpInfo->m_strPropName = "肋板长度A";
				lpInfo->m_strPropHelp = "与辅助构件相连边的边长,初始值自动延伸至辅助钢板的外侧边缘";
#endif
			}
		}
	}
	else if (stricmp(propKey,"G")==0)
	{
		if (xRibPlate.m_pRelaPart1.IsNULL()&&xRibPlate.m_pRelaPart2.IsNULL())
		{
			if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "rib plate height";
				lpInfo->m_strPropHelp = "minimum vertical distance in axis Z direction between datum plate's outer surface and rib plate's out edge.";
#else
				lpInfo->m_strPropName = "肋板高度";
				lpInfo->m_strPropHelp = "基准钢管外壁距肋板外边缘最小Z轴向垂直距离。";
#endif
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "rib plate width W";
				lpInfo->m_strPropHelp = "Parameter W, length of the edge connected with datum part.";
#else
				lpInfo->m_strPropName = "肋板宽度W";
				lpInfo->m_strPropHelp = "参数W, 与基准构件相连边的边长。";
#endif
			}
		}
		else
		{
			if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "rib plate length A";
				lpInfo->m_strPropHelp = "the length of edge connected with datum part, initial value automatically extends to the outer edge of datum plate.";
#else
				lpInfo->m_strPropName = "肋板长度A";
				lpInfo->m_strPropHelp = "与基准构件相连边的边长,初始值自动延伸至基准钢板的外侧边缘。";
#endif
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Parameter B";
				lpInfo->m_strPropHelp = "Width of bottom";//the length of edge connected with datum part, initial value automatically extends to the outer edge of datum plate.";
#else
				lpInfo->m_strPropName = "底部宽度B";
				lpInfo->m_strPropHelp = "与基准构件相连边的边长,初始值自动延伸至基准钢板的外侧边缘。";
#endif
			}
		}
	}
	else
		return false;
	return true;
}
//肋板
void RebuildOrUpdateRibPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	RIB_PLATE xRibPlate;
	if (!InitRibPlatePara(xRibPlate,pCurPlate))
		return;
	if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsNULL()&&
		((xRibPlate.m_pDatumPart->IsTube()&&xRibPlate.m_pRelaPart1->IsPlate())||
		(xRibPlate.m_pDatumPart->IsPlate()&&xRibPlate.m_pRelaPart1->IsTube())))
	{	//钢管肘板式角肋板
		CPropTreeItem* pPropItem=NULL;
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"J",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="基准线";
		pPropItem->m_lpNodeInfo->m_cmbItems="0.基准边|1.焊道线";
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"K",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"G",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="肋板高度A";
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="底部宽度B";
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="台阶高度C";
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"F",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="顶部宽度D";
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="倒角宽度E";
		long CW=0,MH=0;
		pCurPlate->GetDesignItemValue(KEY2C("I"),&CW);
		if (CW>0)
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"e",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"MH",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pCurPlate->GetDesignItemValue(KEY2C("MH"),&MH);
		if (MH>0)
		{
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"MF",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"MG",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		return;
	}
	else if (xRibPlate.m_pDatumPart->IsTube()&&xRibPlate.m_pDatumNode!=NULL&&xRibPlate.m_pRelaPart1.IsNULL())
	{
		CPropTreeItem* pPropItem=NULL;
		//定位参数
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"J",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		if (xRibPlate.tube.iDatumEdge==2)	//指定展开方向
		{
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"AxisY",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Wx",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Wy",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Wz",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		else
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"K",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		if (xRibPlate.tube.iWorkNormStyle!=0)
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"DX",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"DM",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="轴向偏移量M";
		//外形参数	
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"G",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="肋板高度H";
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="底部宽度W";
		//X+向顶部边角类型
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Rt",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		if (xRibPlate.tube.cLeftTopCornerType!=0)	//切角宽度/切弧半径
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Rw",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		if (xRibPlate.tube.cLeftTopCornerType==1)	//切角宽度/切弧半径
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Rh",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		//X-向顶部边角类型
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Lt",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		if (xRibPlate.tube.cRightTopCornerType!=0)	//切角宽度/切弧半径
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Lw",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		if (xRibPlate.tube.cRightTopCornerType==1)
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Lh",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		return;
	}
	else if (xRibPlate.m_pDatumPart->IsTube()&&xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsHasPtr())
	{
		CPropTreeItem* pPropItem=NULL;
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"J",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="基准线";
		pPropItem->m_lpNodeInfo->m_cmbItems="0.基准边|1.焊道线";
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"K",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		double R=0,r=0;
		pCurPlate->GetDesignItemValue('R',&R);
		pCurPlate->GetDesignItemValue('r',&r);
		if (R>0&&r>0)
		{
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"r",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		else
		{
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"F",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"G",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->SetReadOnly();
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"e",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="倒角宽度1";
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I2",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="倒角宽度2";
		return;
	}
	else if (xRibPlate.m_pDatumPart->IsTube())
		return;//属性栏中暂不支持显示基准构件为钢管的肋板参数
	//肋板类型 0.角肋板 1.槽肋板 2.角钢肋板 3.钢板肋板 4.钢管肋板  5.圆塔脚对角肋板 6.圆塔脚外侧环向布置的肋板
	BYTE cPlateType=xRibPlate.m_cPlateType;
	CPropTreeItem* pTempItem=NULL;
	if (cPlateType==5||cPlateType==6)
	{
		if (cPlateType==5)
		{
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"F",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		else
		{
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Rd",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		return;
	}
	pTempItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_BUTTON,TRUE,-1,TRUE);		//定位基准构件
	if (pTempItem!=NULL)	//TODO: 存在pTempItem==NULL的情况 wjh-2016.6.18
		pTempItem->SetReadOnly(TRUE);
	if (0==cPlateType||1==cPlateType)	//0.角肋板 //1.槽肋板
	{
		pTempItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"C",pCurPlate,PIT_BUTTON,TRUE,-1,TRUE);	//定位辅助构件1
		if (pTempItem!=NULL)	//TODO: 存在pTempItem==NULL的情况 wjh-2016.6.18
			pTempItem->m_lpNodeInfo->m_buttonType=BDT_COMMONEDIT;
		if (1==cPlateType)				//1.槽肋板
		{
			pTempItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_BUTTON,TRUE,-1,TRUE);//定位辅助构件2
			if (pTempItem!=NULL)	//TODO: 存在pTempItem==NULL的情况 wjh-2016.6.18
				pTempItem->m_lpNodeInfo->m_buttonType=BDT_COMMONEDIT;
		}
	}
	//2.角钢肋板 //4.钢管肋板
	if (2==cPlateType||4==cPlateType)
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_BUTTON,TRUE,-1,TRUE);	//定位基准节点
	if (xRibPlate.m_pDatumPart->IsPlate())
	{	//
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"RibPlate.plate.bOnDatumPlateTopFace",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		//if(!xRibPlate.plate.bSpecAxisX&&xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsNULL()&&xRibPlate.m_pRelaPart1->IsPlate())
		if (xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsNULL()&&xRibPlate.m_pRelaPart1->IsPlate())
		{
			pTempItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"RibPlate.plate.bOnRelaPlateTopFace",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
			CorrectRibPlatePropItemName(xRibPlate,"RibPlate.plate.bOnRelaPlateTopFace",pTempItem->m_lpNodeInfo);
		}
		CPropTreeItem *pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"RibPlate.plate.bSpecOrg",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		if (xRibPlate.plate.bSpecOrg&&pPropItem)
		{
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Ox",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Oy",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		else
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"O",pCurPlate,PIT_BUTTON,TRUE,-1,TRUE);	//定位螺栓
		if (0==cPlateType||1==cPlateType||3==cPlateType)
		{
			pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"RibPlate.plate.bSpecAxisY",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
			if (pPropItem!=NULL)
			{
				CorrectRibPlatePropItemName(xRibPlate,"RibPlate.plate.bSpecAxisY",pPropItem->m_lpNodeInfo);
				if (xRibPlate.plate.bSpecAxisY)
					InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Ga",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
				else if (0==cPlateType||1==cPlateType)
					InsertOrUpdatePlateParamItem(pPropList,pPropItem,"V",pCurPlate,PIT_BUTTON,TRUE,-1,TRUE);
			}
		}
		//
		if (xRibPlate.m_pRelaPart1.IsNULL()&&xRibPlate.m_pRelaPart2.IsNULL())
		{	//钢板肋板
			pTempItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			CorrectRibPlatePropItemName(xRibPlate,"E",pTempItem->m_lpNodeInfo);
			pTempItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"G",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			CorrectRibPlatePropItemName(xRibPlate,"G",pTempItem->m_lpNodeInfo);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"X",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);	//X方向偏移距离
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Z",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);	//Z方向偏移距离
			if (xRibPlate.plate.bHashBendPlane)
			{
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"dx",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);//X方向偏移距离
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"dz",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);//Z方向偏移距离
			}
			else
			{
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("dx",pCurPlate->m_iParamType));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("dz",pCurPlate->m_iParamType));
			}
		}
		//角肋板或槽肋板
		else if (xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart1->IsPlate())
		{
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"El",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
			pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			long liSideEdgeLength=0;
			if (pCurPlate->GetDesignItemValue(KEY2C("El"),&liSideEdgeLength)&&liSideEdgeLength!=0)
				pPropItem->m_lpNodeInfo->m_strPropName="侧边高度A";
			else
				pPropItem->m_lpNodeInfo->m_strPropName="侧边长度A";
			pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"G",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			pPropItem->SetReadOnly(1==xRibPlate.m_cPlateType);	//长度(为两钢板夹板时该长度自动计算)
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"F",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);	//定位辅助构件1
			if (1==cPlateType)
			{
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"e",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
				pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
				pPropItem->m_lpNodeInfo->m_strPropName="倒角宽度1";
				pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I2",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
				pPropItem->m_lpNodeInfo->m_strPropName="倒角宽度2";
			}
			else
			{
				long CW=0;
				pCurPlate->GetDesignItemValue(KEY2C("I"),&CW);
				pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);		//切角宽度
				if (CW>0)	//倒角宽度E>0
					InsertOrUpdatePlateParamItem(pPropList,pPropItem,"e",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
			}
			long MH=0;
			pCurPlate->GetDesignItemValue(KEY2C("MH"),&MH);
			CPropTreeItem *pMHItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"MH",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			if (MH>0)
			{
				InsertOrUpdatePlateParamItem(pPropList,pMHItem,"MF",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
				InsertOrUpdatePlateParamItem(pPropList,pMHItem,"MG",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			}
			else
			{
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("MF",pCurPlate->m_iParamType));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("MG",pCurPlate->m_iParamType));
			}
		}
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Z",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);		//Z方向偏移距离
	}
}
BOOL ModifyRibPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	long h=0;
	CLDSParamPlate* pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("RibPlate.plate.bOnDatumPlateTopFace",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			pParamPlate->GetDesignItemValue('A',&h);
			if (valueStr.CompareNoCase("是")==0)
				pParamPlate->SetDesignItemValue('A',abs(h),NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
			else
				pParamPlate->SetDesignItemValue('A',abs(h)*-1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("RibPlate.plate.bSpecOrg",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		BOOL bSpecOrg=valueStr[0]-'0';
		pPropList->DeleteAllSonItems(pItem);
		if (bSpecOrg)
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Ox",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Oy",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		else
			InsertOrUpdatePlateParamItem(pPropList,pItem,"O",pParamPlate,PIT_BUTTON,TRUE,-1,TRUE);
		//
		double fValue=0;
		long hBolt=0;
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			BOOL bSpecOrgX=pParamPlate->GetDesignItemValue(KEY2C("Ox"),&fValue);
			BOOL bSpecOrgY=pParamPlate->GetDesignItemValue(KEY2C("Oy"),&fValue);
			pParamPlate->GetDesignItemValue(KEY2C("O"),&hBolt);
			if (bSpecOrg)
			{
				if (!bSpecOrgX)
					pParamPlate->SetDesignItemValue(KEY2C("Ox"),fValue);
				if (!bSpecOrgY)
					pParamPlate->SetDesignItemValue(KEY2C("Oy"),fValue);
				pParamPlate->SetDesignItemValue(KEY2C("O"),0,NULL,0,1);
			}
			else
			{
				if (bSpecOrgX)
					pParamPlate->DeleteDesignItem(KEY2C("Ox"));
				if (bSpecOrgY)
					pParamPlate->DeleteDesignItem(KEY2C("Oy"));
				pParamPlate->SetDesignItemValue(KEY2C("O"),hBolt,NULL,0,1);
			}
		}
	}
	else if (CLDSParamPlate::GetPropID("Ox",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double fValue=atof(valueStr);
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pParamPlate->SetDesignItemValue(KEY2C("Ox"),fValue);
	}
	else if (CLDSParamPlate::GetPropID("Oy",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double fValue=atof(valueStr);
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pParamPlate->SetDesignItemValue(KEY2C("Oy"),fValue);
	}
	else if (CLDSParamPlate::GetPropID("Ga",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double fValue=atof(valueStr);
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pParamPlate->SetDesignItemValue(KEY2C("Ga"),fValue);
	}
	else if (CLDSParamPlate::GetPropID("RibPlate.plate.bSpecAxisX",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double fValue=0;
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			BOOL bSpecX=pParamPlate->GetDesignItemValue(KEY2C("Xx"),&fValue);
			BOOL bSpecY=pParamPlate->GetDesignItemValue(KEY2C("Xy"),&fValue);
			if (valueStr[0]-'0'==1)
			{
				if (!bSpecX)
					pParamPlate->SetDesignItemValue(KEY2C("Xx"),fValue);
				if (!bSpecY)
					pParamPlate->SetDesignItemValue(KEY2C("Xy"),fValue);
			}
			else
			{
				if (bSpecX)
					pParamPlate->DeleteDesignItem(KEY2C("Xx"));
				if (bSpecY)
					pParamPlate->DeleteDesignItem(KEY2C("Xy"));
			}
		}
	}
	else if (CLDSParamPlate::GetPropID("RibPlate.plate.bSpecAxisY",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		BOOL bSpecAxisY=valueStr[0]-'0';
		pPropList->DeleteAllSonItems(pItem);
		if (bSpecAxisY)
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Ga",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
		else
			InsertOrUpdatePlateParamItem(pPropList,pItem,"V",pParamPlate,PIT_BUTTON,TRUE,-1,TRUE);
		double fValue=0;
		long hVicePart=0;
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			BOOL bSpecY=pParamPlate->GetDesignItemValue(KEY2C("Ga"),&fValue);
			if (!bSpecY)	//若无初始值，则取默认值90度 wjh-2019.7.30
				fValue=90;
			pParamPlate->GetDesignItemValue(KEY2C("V"),&hVicePart);
			if (bSpecAxisY)
			{
				if (!bSpecY)
					pParamPlate->SetDesignItemValue(KEY2C("Ga"),fValue);
				pParamPlate->SetDesignItemValue(KEY2C("V"),0,NULL,0,1);
			}
			else if (bSpecY)
			{
				pParamPlate->DeleteDesignItem(KEY2C("Ga"));
				pParamPlate->SetDesignItemValue(KEY2C("V"),hVicePart,NULL,0,1);
			}
		}
	}
	else if (CLDSParamPlate::GetPropID("Ga",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double fValue=atof(valueStr);
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pParamPlate->GetDesignItemValue(KEY2C("Ga"),&fValue);
	}
	else if (CLDSParamPlate::GetPropID("RibPlate.plate.bAlongVecPart",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			pParamPlate->GetDesignItemValue('V',&h);
			if (valueStr.CompareNoCase("是")==0)
				pParamPlate->SetDesignItemValue('V',abs(h),NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
			else
				pParamPlate->SetDesignItemValue('V',abs(h)*-1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("RibPlate.plate.bOnRelaPlateTopFace",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		long hDatumPart=0,hRelaPart1=0,hRelaPart2=0;
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			pParamPlate->GetDesignItemValue('A',&hDatumPart);
			pParamPlate->GetDesignItemValue('C',&hRelaPart1);
			pParamPlate->GetDesignItemValue('D',&hRelaPart2);
			CLDSPart *pDatumPart=pParamPlate->BelongModel()->FromPartHandle(abs(hDatumPart));
			CLDSPart *pRelaPart1=pParamPlate->BelongModel()->FromPartHandle(abs(hRelaPart1));
			CLDSPart *pRelaPart2=pParamPlate->BelongModel()->FromPartHandle(abs(hRelaPart2));
			if (pRelaPart1==NULL&&pRelaPart2==NULL)
			{
				if (pDatumPart&&pDatumPart->GetClassTypeId()==CLS_LINETUBE)
				{
					if (valueStr.CompareNoCase("是")==0)
						pParamPlate->SetDesignItemValue('A',abs(hDatumPart),NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
					else
						pParamPlate->SetDesignItemValue('A',abs(hDatumPart)*-1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
				}
			}
			else if (pRelaPart1)
			{
				if (valueStr.CompareNoCase("是")==0)
					pParamPlate->SetDesignItemValue('C',abs(hRelaPart1),NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
				else
					pParamPlate->SetDesignItemValue('C',abs(hRelaPart1)*-1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
			}
		}
	}
	else if (CLDSParamPlate::GetPropID("MH",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		long MH=atoi(valueStr);
		pParamPlate->SetDesignItemValue(KEY2C("MH"),MH);
		if (MH==0)
			pPropList->DeleteAllSonItems(pItem);
		else
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"MF",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"MG",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("Lt",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		long Lt=valueStr[0]-'0';
		pParamPlate->SetDesignItemValue(KEY2C("Lt"),Lt,NULL,PARAM_SHAPE,1,TRUE);
		pPropList->DeleteAllSonItems(pItem);
		if (Lt!=0)
		{
			CPropTreeItem* pSonItem=InsertOrUpdatePlateParamItem(pPropList,pItem,"Lw",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			if (Lt==2)
				pSonItem->m_lpNodeInfo->m_strPropName="切弧半径";
		}
		if (Lt==1)
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Lh",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
	else if (CLDSParamPlate::GetPropID("Rt",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		long Rt=valueStr[0]-'0';
		pParamPlate->SetDesignItemValue(KEY2C("Rt"),Rt,NULL,PARAM_SHAPE,1,TRUE);
		pPropList->DeleteAllSonItems(pItem);
		if (Rt!=0)
		{
			CPropTreeItem* pSonItem=InsertOrUpdatePlateParamItem(pPropList,pItem,"Rw",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			if (Rt==2)
				pSonItem->m_lpNodeInfo->m_strPropName="切弧半径";
		}
		if (Rt==1)
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Rh",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
	else if (CLDSParamPlate::GetPropID("I",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//倒角宽度E
		int CW=atoi(valueStr);
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType==CLDSParamPlate::TYPE_RIB_ELBOW_PLATE||iSubType==CLDSParamPlate::TYPE_RIB_CORNER_PLATE||iSubType==CLDSParamPlate::TYPE_RIB_SLOT_PLATE)
				pParamPlate->SetDesignItemValue(KEY2C("I"),CW,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER,TRUE);
		}
		CLDSParamPlate* pFirstParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();
		if (pFirstParamPlate->GetRibPlateSubType()!=CLDSParamPlate::TYPE_RIB_SLOT_PLATE)
		{
			if (CW==0)
				pPropList->DeleteAllSonItems(pItem);
			else
				InsertOrUpdatePlateParamItem(pPropList,pItem,"e",pFirstParamPlate,PIT_COMBO,TRUE,-1,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("I2",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//倒角宽度2
		int CW=atoi(valueStr);
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType==CLDSParamPlate::TYPE_RIB_SLOT_PLATE)
				pParamPlate->SetDesignItemValue(KEY2C("I2"),CW,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("e",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//倒角模式
		int iType=valueStr[0]-'0';
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType==CLDSParamPlate::TYPE_RIB_ELBOW_PLATE||iSubType==CLDSParamPlate::TYPE_RIB_CORNER_PLATE||iSubType==CLDSParamPlate::TYPE_RIB_SLOT_PLATE)
				pParamPlate->SetDesignItemValue(KEY2C("e"),iType,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("J",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//肋板展开方向定义类型
		int iType=valueStr[0]-'0';
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType==CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE||iSubType==CLDSParamPlate::TYPE_RIB_SLOT_PLATE||
				iSubType==CLDSParamPlate::TYPE_RIB_ELBOW_PLATE)
				pParamPlate->SetDesignItemValue(KEY2C("J"),iType,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER,TRUE);
		}
		//
		CLDSParamPlate* pFirstParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();
		pPropList->DeleteAllSonItems(pItem);
		if (iType==2)
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"AxisY",pFirstParamPlate,PIT_COMBO,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Wx",pFirstParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Wy",pFirstParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Wz",pFirstParamPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		else
			InsertOrUpdatePlateParamItem(pPropList,pItem,"K",pFirstParamPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
	else if (CLDSParamPlate::GetPropID("AxisY",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//自定义肋板展开方向
		f3dPoint spread_vec=CStdVector::GetVector(valueStr[0]-'0');
		if (!spread_vec.IsZero())
		{
			for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			{
				int iSubType=pParamPlate->GetRibPlateSubType();
				if (iSubType!=CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE)	//钢管肋板
					continue;
				pParamPlate->SetDesignItemValue(KEY2C("Wx"),spread_vec.x,NULL,PARAM_POS);
				pParamPlate->SetDesignItemValue(KEY2C("Wy"),spread_vec.y,NULL,PARAM_POS);
				pParamPlate->SetDesignItemValue(KEY2C("Wz"),spread_vec.z,NULL,PARAM_POS);
			}
			//
			CXhChar16 sText;
			CLDSParamPlate* pFirstParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();
			sprintf(sText,"%f",spread_vec.x);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("Wx",pFirstParamPlate->m_iParamType),sText);
			sprintf(sText,"%f",spread_vec.y);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("Wy",pFirstParamPlate->m_iParamType),sText);
			sprintf(sText,"%f",spread_vec.z);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("Wz",pFirstParamPlate->m_iParamType),sText);
		}
	}
	else if (CLDSParamPlate::GetPropID("Wx",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double Wx=atof(valueStr);
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType!=CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE)	//钢管肋板
				pParamPlate->SetDesignItemValue(KEY2C("Wx"),Wx,NULL,PARAM_POS);
		}
	}
	else if (CLDSParamPlate::GetPropID("Wy",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double Wy=atof(valueStr);
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType!=CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE)	//钢管肋板
				pParamPlate->SetDesignItemValue(KEY2C("Wy"),Wy,NULL,PARAM_POS);
		}
	}
	else if (CLDSParamPlate::GetPropID("Wz",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double Wz=atof(valueStr);
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType!=CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE)	//钢管肋板
				pParamPlate->SetDesignItemValue(KEY2C("Wz"),Wz,NULL,PARAM_POS);
		}
	}
	else if (CLDSParamPlate::GetPropID("El",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		long liSideEdgeLengthByHeight=valueStr[0]-'0';
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType==CLDSParamPlate::TYPE_RIB_CORNER_PLATE)	//角肋板
				pParamPlate->SetDesignItemValue(KEY2C("El"),liSideEdgeLengthByHeight,NULL,PARAM_SHAPE,2);
		}
		CLDSParamPlate* pFirstParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
		CPropTreeItem* pFindItem=NULL;
		long idRelaItem=CLDSParamPlate::GetPropID("E",pFirstParamPlate->m_iParamType);
		if ((pFindItem=pPropList->FindItemByPropId(idRelaItem,NULL))!=NULL)
		{
			CString szPropName=(liSideEdgeLengthByHeight!=0)?"侧边高度A":"侧边长度A";
			pPropList->SetItemPropName(idRelaItem,szPropName);
		}
	}
	else
		return FALSE;
	return TRUE;
}
//端板
void RebuildOrUpdateStationConnPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem* pItem=NULL;
	pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"LP",pCurPlate,PIT_COMBO);
	pItem->SetReadOnly();
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_COMBO);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"C",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"r",pCurPlate,PIT_EDIT);
}
BOOL ModifyStationConnPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	CXhChar100 tem_str("%s",valueStr);
	KEY2C paramKey=CLDSParamPlate::GetPropKey(pItem->m_idProp,pPlate->m_iParamType);
	if (KEY2C("I")==paramKey)
	{	//底板外形类型
		long iType=valueStr[0]-'0';
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey,iType);
	}
	else
		return FALSE;
	return TRUE;
}
//支柱夹板
void RebuildOrUpdateStationShoePlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem *pPropItem=NULL;
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"LB",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly();
	long hAssistPlate=0;
	pCurPlate->GetDesignItemValue(KEY2C("LE"),&hAssistPlate);
	CLDSParamPlate* pEndPlate=(CLDSParamPlate*)console.FromPartHandle(hAssistPlate,CLS_PARAMPLATE);
	if (pEndPlate && pEndPlate->m_iParamType==TYPE_STATIONCONNPLATE)
	{
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("H",pCurPlate->m_iParamType));
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"LE",pCurPlate,PIT_EDIT);
		pPropItem->SetReadOnly();
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT);
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("LE",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("R",pCurPlate->m_iParamType));
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
	}
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"CR",pCurPlate,PIT_EDIT);
}
BOOL ModifyStationShoePlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	return FALSE;
}
//端撑连板
void RebuildOrUpdateStationLinkPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem *pPropItem=NULL;
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly();
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly();
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"C",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly();
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly();
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"OW",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"OB",pCurPlate,PIT_EDIT);
}
BOOL ModifyStationLinkPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	return FALSE;
}
//钢管肘板
void RebuildOrUpdateElbowPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem *pPropItem=NULL;
	long MH=0,CW=0;
	pCurPlate->GetDesignItemValue(KEY2C("H"),&MH);
	pCurPlate->GetDesignItemValue(KEY2C("E"),&CW);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"C",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	if (CW>0)
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"e",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	if (MH>0)
	{
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"F",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"G",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
}
BOOL ModifyElbowPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("H",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		long MH=atoi(valueStr);
		pParamPlate->SetDesignItemValue(KEY2C("H"),MH);
		if (MH==0)
			pPropList->DeleteAllSonItems(pItem);
		else
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"F",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"G",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("I",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//倒角宽度E
		int CW=atoi(valueStr);
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pParamPlate->SetDesignItemValue(KEY2C("I"),CW,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER,TRUE);
		//更新属性栏
		CLDSParamPlate* pFirstParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();
		if (CW==0)
			pPropList->DeleteAllSonItems(pItem);
		else
			InsertOrUpdatePlateParamItem(pPropList,pItem,"e",pFirstParamPlate,PIT_COMBO,TRUE,-1,TRUE);
	}
	else if (CLDSParamPlate::GetPropID("e",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//倒角模式
		int iType=valueStr[0]-'0';
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pParamPlate->SetDesignItemValue(KEY2C("e"),iType,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER,TRUE);
	}
	else
		return FALSE;
	return TRUE;
}
//端撑插板
void RebuildOrUpdateStationCrossPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	long I=0;
	pCurPlate->GetDesignItemValue(KEY2C("I"),&I);
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"LP",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly();
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_COMBO);
	if (I==0||I==3)
		pPropItem->m_lpNodeInfo->m_cmbItems="X轴通板|Y轴通板";
	else
	{
		pPropItem->m_lpNodeInfo->m_cmbItems="X+向板|X-向板|Y+向板|Y-向板";
		pSonPropItem=InsertOrUpdatePlateParamItem(pPropList,pPropItem,"LA",pCurPlate,PIT_EDIT);
		pSonPropItem->SetReadOnly();
	}
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
}
BOOL ModifyStationCrossPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("I",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		int iVecType=0;
		if (valueStr.CompareNoCase("X轴通板")==0)
			iVecType=0;
		else if (valueStr.CompareNoCase("Y轴通板")==0)
			iVecType=3;
		else if (valueStr.CompareNoCase("X+向板")==0)
			iVecType=1;
		else if (valueStr.CompareNoCase("X-向板")==0)
			iVecType=2;
		else if (valueStr.CompareNoCase("Y+向板")==0)
			iVecType=4;
		else if (valueStr.CompareNoCase("Y-向板")==0)
			iVecType=5;
		pParamPlate->SetDesignItemValue(KEY2C("I"),iVecType);
	}
	else
		return FALSE;
	return TRUE;
}
//遮水板
void RebuildOrUpdateWaterShieldedPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	double angle=0;
	pCurPlate->GetDesignItemValue(KEY2C("A"),&angle);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_COMBO);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"S",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
	CPropTreeItem *pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
	if (angle>0)
	{
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"O",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"L",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"K",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"H",pCurPlate,PIT_EDIT);
	}
}
BOOL ModifyWaterShieldedPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("E",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		int iType=valueStr[0]-'0';
		for (CLDSParamPlate* pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(KEY2C("E"),iType);
	}
	else if (CLDSParamPlate::GetPropID("A",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double angle=atof(valueStr);
		for (CLDSParamPlate* pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(KEY2C("A"),angle);
		//
		pPropList->DeleteAllSonItems(pItem);
		if (angle>0)
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"O",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"L",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"K",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"H",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
	}
	else
		return FALSE;
	return TRUE;
}
//环向封板
void RebuildOrUpdateCircularCoverPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	long iCoverPlateType=0;
	pCurPlate->GetDesignItemValue('Y',&iCoverPlateType);
	if (iCoverPlateType==3)
	{
		CPropTreeItem* pItem=NULL;
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		pItem->m_lpNodeInfo->m_strPropName="封板宽度W";
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"N",pCurPlate,PIT_EDIT);
		pItem->m_lpNodeInfo->m_strPropName="封板宽度H";
	}
	else
	{
		if (iCoverPlateType==0)
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
		if (iCoverPlateType!=0)
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"N",pCurPlate,PIT_EDIT);
	}
}
BOOL ModifyCircularCoverPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	return FALSE;
}
//环向肋板
void RebuildOrUpdateCircularRibPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem* pItem=NULL;
	int iSubType=pCurPlate->GetCircularPlateSubType();
	if (pCurPlate->m_iParamType==TYPE_TUBERIBPLATE)
	{	//钢管肋板（跨越连板）
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"N",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"h",pCurPlate,PIT_EDIT);
	}
	else if (iSubType==CLDSParamPlate::TYPE_SPECIAL_CIR_PLATE)			//异型法兰
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"r",pCurPlate,PIT_EDIT);
	}
	else if (iSubType==CLDSParamPlate::TYPE_COMMON_CIR_PLATE)	//普通环向肋板
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT);
	}
	else if (iSubType==CLDSParamPlate::TYPE_3WL_CIR_PLATE)
	{
		double D=0;
		pCurPlate->GetDesignItemValue('D',&D);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"N",pCurPlate,PIT_EDIT);
		if (D>0)
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
		else
		{
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"h",pCurPlate,PIT_EDIT);
		}
	}
	else if (iSubType==CLDSParamPlate::TYPE_3WA_CIR_PLATE)
	{
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
		pItem->m_lpNodeInfo->m_strPropName="环形带宽W";
	}
	else if (iSubType==CLDSParamPlate::TYPE_2WL_CIR_PLATE)
	{
		double angle=0;
		pCurPlate->GetDesignItemValue('B',&angle);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
		pItem->m_lpNodeInfo->m_strPropName="台口高度H";
		if (angle>0)
		{
			pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"h",pCurPlate,PIT_EDIT);
			pItem->m_lpNodeInfo->m_strPropName="台口高度h";
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
		}
		else
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
	}
	else if (iSubType==CLDSParamPlate::TYPE_2WA_CIR_PLATE)
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"y",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
	}
	else if (iSubType==CLDSParamPlate::TYPE_CIR_AISLE_PLATE)
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"ES",pCurPlate,PIT_EDIT);
	}
	if (iSubType!=CLDSParamPlate::TYPE_CIR_AISLE_PLATE)
	{
		double offsetDist=0;
		pCurPlate->GetDesignItemValue('T',&offsetDist);
		if (offsetDist>0)
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"T",pCurPlate,PIT_EDIT);
		else
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"S",pCurPlate,PIT_EDIT);
	}
}
BOOL ModifyCircularRibPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	bool processed=false;
	CLDSLinePart* pRod=NULL;
	for (CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate!=NULL;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
	{
		KEY2C key=CLDSParamPlate::GetPropKey(pItem->m_idProp,pParamPlate->m_iParamType);
		if (key=='S'&&(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)&&
			(pRod=pParamPlate->BelongModel()->FromRodHandle(pParamPlate->m_hBaseRod))!=NULL)
		{
			GEPOINT vDatumAxis=pRod->End()-pRod->Start();
			double S=atof(valueStr);
			if (vDatumAxis*pParamPlate->ucs.axis_z<0)
				S*=-1;
			pParamPlate->SetDesignItemValue(key,S,"轴向偏移量",PARAM_POS,0,TRUE);
			processed=true;
		}
		else if (key=='d'&& pParamPlate->GetCircularPlateSubType()==CLDSParamPlate::TYPE_SPECIAL_CIR_PLATE)
		{
			long hTube=0;
			pParamPlate->GetDesignItemValue('C',&hTube);
			CLDSLineTube* pTube=(CLDSLineTube*)console.FromPartHandle(hTube,CLS_LINETUBE);
			if (pTube)
			{
				double d=atof(valueStr);
				double r=pTube->GetDiameter()*0.5-pTube->GetThick()-d;
				pParamPlate->SetDesignItemValue('r',r,"",PARAM_POS,0,TRUE);
				processed=true;
			}
		}
	}
	return processed;
}
void RebuildOrUpdateFLPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem* pPropItem=NULL;
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly(TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT);
}
BOOL ModifyFLPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("H",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		for (CLDSParamPlate* pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			if (pPlate->m_iParamType!=TYPE_FL)
				continue;
			pPlate->SetDesignItemValue('H',atof(valueStr));
			//修改H参数后，需根据钢管端头设计重新计算法兰的位置
			CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
			if (pLineTube)
			{
				if (pLineTube->m_tJointEnd.hLinkObj==pPlate->handle)
					pLineTube->DesignEndJointUCS(pPlate);
				else if (pLineTube->m_tJointStart.hLinkObj==pPlate->handle)
					pLineTube->DesignStartJointUCS(pPlate);
			}
		}
		return TRUE;
	}
	return FALSE;
}
void RebuildOrUpdatePedalPlateParamsPropertyList(CPropertyList* pPropList, CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate == NULL)
		return;
	CPropTreeItem* pPropItem = NULL;
	pPropItem = InsertOrUpdatePlateParamItem(pPropList, pParentItem, "A", pCurPlate, PIT_EDIT);
	pPropItem->SetReadOnly(TRUE);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "V", pCurPlate, PIT_COMBO);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "C", pCurPlate, PIT_COMBO);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "S", pCurPlate, PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "L", pCurPlate, PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "W", pCurPlate, PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "H", pCurPlate, PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "D", pCurPlate, PIT_EDIT);
}
BOOL ModifyPedalPlateParamProperty(CPropertyList *pPropList, CPropTreeItem* pItem, CString &valueStr, HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum() < 0)
		return FALSE;
	long type = valueStr[0] - '0';
	CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("V", pParamPlate->m_iParamType) == pItem->m_idProp)
	{
		for (CLDSParamPlate* pPlate = (CLDSParamPlate*)selectObjs.GetFirst(); pPlate; pPlate = (CLDSParamPlate*)selectObjs.GetNext())
		{
			if (pPlate->m_iParamType != TYPE_PEDALPLATE)
				continue;
			pPlate->SetDesignItemValue('V', type);
		}
		return TRUE;
	}
	else if (CLDSParamPlate::GetPropID("C", pParamPlate->m_iParamType) == pItem->m_idProp)
	{
		for (CLDSParamPlate* pPlate = (CLDSParamPlate*)selectObjs.GetFirst(); pPlate; pPlate = (CLDSParamPlate*)selectObjs.GetNext())
		{
			if (pPlate->m_iParamType != TYPE_PEDALPLATE)
				continue;
			pPlate->SetDesignItemValue('C', type);
		}
		//
		CLDSView::PROMPT_LIFE lifeprompt(true);
		if (type == 0)
			lifeprompt.SetPromptPicture(IDB_BMP_PEDAL_PLATE1);
		else
			lifeprompt.SetPromptPicture(IDB_BMP_PEDAL_PLATE2);
		return TRUE;
	}
	return FALSE;
}
//
void RebuildOrUpdateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	pPropList->DeleteAllSonItems(pParentItem);
	if (pCurPlate->m_iParamType==TYPE_BASEPLATE)
		RebuildOrUpdateBasePlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_SHOEPLATE)
		RebuildOrUpdateShoePlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
		RebuildOrUpdateAngleRibPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_TUBESTAYWIREPLATE)
		RebuildOrUpdateStayWirePlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_RIBPLATE)
		RebuildOrUpdateRibPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_STATIONCONNPLATE)
		RebuildOrUpdateStationConnPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_STATIONSHOEPLATE)
		RebuildOrUpdateStationShoePlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_STATIONLINKPLATE)
		RebuildOrUpdateStationLinkPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_ELBOWPLATE)	//肘板
		RebuildOrUpdateElbowPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_STATIONCROSSPLATE)
		RebuildOrUpdateStationCrossPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
		RebuildOrUpdateWaterShieldedPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		RebuildOrUpdateCircularCoverPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_CIRRIBPLATE||pCurPlate->m_iParamType==TYPE_TUBERIBPLATE)
		RebuildOrUpdateCircularRibPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_FL)
		RebuildOrUpdateFLPlateParamsPropertyList(pPropList,pParentItem);
	else if(pCurPlate->m_iParamType==TYPE_PEDALPLATE)
		RebuildOrUpdatePedalPlateParamsPropertyList(pPropList, pParentItem);
}
BOOL ModifyParamPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	CLDSParamPlate *pFirstPlate=(CLDSParamPlate*)selectObjs.GetFirst();
	CLDSParamPlate *pPlate=pFirstPlate;
	if (pPlate==NULL)
		return FALSE;
	BOOL bRetCode=FALSE;
	if (pPlate->m_iParamType==TYPE_BASEPLATE)
		bRetCode=ModifyBasePlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_SHOEPLATE)
		bRetCode=ModifyShoePlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
		bRetCode=ModifyAngleRibPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_RIBPLATE)
		bRetCode=ModifyRibPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_STATIONCONNPLATE)
		bRetCode=ModifyStationConnPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_STATIONSHOEPLATE)
		bRetCode=ModifyStationShoePlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_STATIONLINKPLATE)
		bRetCode=ModifyStationLinkPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_ELBOWPLATE)
		bRetCode=ModifyElbowPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_STATIONCROSSPLATE)
		bRetCode=ModifyStationCrossPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
		bRetCode=ModifyWaterShieldedPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		bRetCode=ModifyCircularCoverPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_CIRRIBPLATE||pPlate->m_iParamType==TYPE_TUBERIBPLATE)
		bRetCode=ModifyCircularRibPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_FL)
		bRetCode=ModifyFLPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if(pPlate->m_iParamType==TYPE_PEDALPLATE)
		bRetCode = ModifyPedalPlateParamProperty(pPropList, pItem, valueStr, selectObjs);
	if (!bRetCode)
	{	//按参数类型修改参数值
		KEY2C paramKey=CLDSParamPlate::GetPropKey(pItem->m_idProp,pPlate->m_iParamType);
		DESIGN_PLATE_ITEM *pDesignItem=pPlate->GetDesignItem(paramKey);
		if (pDesignItem)
		{
			bRetCode=TRUE;
			if (0==pDesignItem->dataType)
			{	//1.浮点数
				pDesignItem->value.fValue=atof(valueStr);
				for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
					pPlate->SetDesignItemValue(paramKey,pDesignItem->value.fValue,pDesignItem->sNotes,pDesignItem->paramType,pDesignItem->dataType,TRUE);
			}
			else if (1==pDesignItem->dataType)
			{	//2.句柄
				pDesignItem->value.hPart=HexStrToLong(valueStr);
				for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
					pPlate->SetDesignItemValue(paramKey,pDesignItem->value.hPart,pDesignItem->sNotes,pDesignItem->paramType,pDesignItem->dataType,TRUE);
			}
			else if (2==pDesignItem->dataType)
			{	//3.整数
				pDesignItem->value.iType=atoi(valueStr);
				for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
					pPlate->SetDesignItemValue(paramKey,pDesignItem->value.iType,pDesignItem->sNotes,pDesignItem->paramType,pDesignItem->dataType,TRUE);
			}
			else
				bRetCode=FALSE;
		}
		else
		{	//根据PROPLIST_ITEM添加钢板中未保存的新增参数 wht 15-07-06
			PROPLIST_ITEM *pPropListItem=NULL;
			DYNA_PROPITEM_GROUP *pGroup=CLDSParamPlate::hashParamItemPropGroup.GetValue(pPlate->m_iParamType);
			if (pGroup)
				pPropListItem=pGroup->GetPropItem(pItem->m_idProp);
			bRetCode=TRUE;
			if (pPropListItem)
				pPlate->SetDesignItemValue(paramKey,atoi(valueStr));
			else
				bRetCode=FALSE;
		}
	}
	if (bRetCode)
	{	//修改参数之后重新设置参数化板
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->DesignPlate();
		KEY2C key=CLDSParamPlate::GetPropKey(pItem->m_idProp,pFirstPlate->m_iParamType);
		RedrawPlateParamDrawing(pFirstPlate,key.Label());//必须置于DesignPlate之后，否则生成初始基面示意图位置有错　wjh-2016.11.1
		CLDSView::RedrawPlateProfileDrawing(pFirstPlate);
		//更新相关的约束控制对象
		long idDbProp=CLDSParamPlate::GetDbPropId(key,pFirstPlate->m_iParamType);
		CHashSet<CLDSDbObject*> hashUpdateObjs;
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			Ta.ClearConstraintSyncStates();	//由于构件与构件间的约束关联成网状，必须进行全局重置同步状态
			pPlate->ReDesignConstraintSonObjs((WORD)idDbProp,&hashUpdateObjs);
			if (pPlate->IsModified())
			{
				pPlate->DesignPlate();
				pPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
		}
		for (CLDSDbObject* pDbObj=hashUpdateObjs.GetFirst();pDbObj;pDbObj=hashUpdateObjs.GetNext())
		{
			if (!pDbObj->IsPart()||!((CLDSPart*)pDbObj)->IsPlate())
				continue;
			CSuperSmartPtr<CLDSGeneralPlate>pPlate=(CLDSGeneralPlate*)pDbObj;
			if (pPlate->wiType>0)
			{
				pPlate.pParamPlate->DesignPlate();
				pPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
		}
	}
	return bRetCode;
}

void DrawParamArrowLine(IDrawing *pDrawing,GEPOINT base_pos,GEPOINT line_vec,
	GEPOINT work_norm,const char* param_name=NULL,double offset_dist=0,
	double line_len=250,long hPart=0,UCS_STRU *pObjectCS=NULL)
{
	UCS_STRU ocs;
	if (pObjectCS!=NULL)
		ocs=*pObjectCS;
	else
	{
		CDisplayView *pDispView=console.GetActiveView();
		if (pDispView==NULL)
			return;
		LoadDefaultUCS(&ocs);
		ocs.origin.Set();
		vector_trans(ocs.axis_x,pDispView->ucs,FALSE);
		vector_trans(ocs.axis_y,pDispView->ucs,FALSE);
		vector_trans(ocs.axis_z,pDispView->ucs,FALSE);
		coord_trans(ocs.origin,pDispView->ucs,FALSE);
	}
	HIBERID hiberId(hPart,HIBERARCHY(0,0,0,0));
	//1.绘制基准点位置
	GEPOINT real_base_pos=base_pos;
	coord_trans(real_base_pos,ocs,TRUE,TRUE);
	hiberId.SetHiberarchy(5,0,1,0);
	AppendDbPoint(pDrawing,real_base_pos,hiberId,PS_SOLID,RGB(0,0,255),100);
	//2.绘制箭头线(考虑箭头偏移量)
	GEPOINT real_line_vec=line_vec,real_offset_vec=line_vec,real_work_norm=work_norm;
	RotateVectorAroundVector(real_offset_vec,0.5*Pi,work_norm);
	vector_trans(real_line_vec,ocs,TRUE,TRUE);
	vector_trans(real_offset_vec,ocs,TRUE,TRUE);
	vector_trans(real_work_norm,ocs,TRUE,TRUE);
	GEPOINT line_start=real_base_pos;
	if (offset_dist>0&&!real_offset_vec.IsZero())
		line_start+=real_offset_vec*offset_dist;
	GEPOINT line_end=line_start+real_line_vec*line_len;
	//2.1 绘制直线
	hiberId.SetHiberarchy(5,0,2,0);
	BYTE line_width=3;
	COLORREF clrLine=RGB(255,0,0);
	AppendDbLine(pDrawing,line_start,line_end,hiberId,PS_SOLID,clrLine,line_width);
	//2.2 绘制箭头
	double fAngle=15*RADTODEG_COEF;
	double fEdgeLen=25/cos(fAngle);	//箭头长度设置为10
	GEPOINT arrowEndPtArr[2];
	for (int i=0;i<2;i++)
	{
		f3dPoint arrowVec=real_line_vec*-1;
		RotateVectorAroundVector(arrowVec,i==0?fAngle:-1*fAngle,real_work_norm);
		arrowEndPtArr[i]=line_end+arrowVec*fEdgeLen;
		AppendDbLine(pDrawing,line_end,arrowEndPtArr[i],hiberId,PS_SOLID,clrLine,line_width);
	}
	AppendDbLine(pDrawing,arrowEndPtArr[0],arrowEndPtArr[1],hiberId,PS_SOLID,clrLine,line_width);
	//3.绘制+\-号及参数名称
	GEPOINT plus_sign_pos=real_base_pos+real_line_vec*line_len*0.5;
	GEPOINT minus_sign_pos=real_base_pos-real_line_vec*line_len*0.5;
	IDbText* pText=AppendDbText(pDrawing,plus_sign_pos,"+",0,80,IDbText::AlignMiddleCenter,hiberId,PS_SOLID,RGB(255,0,255),5);
	pText->SetNormal(real_work_norm);
	pText=AppendDbText(pDrawing,minus_sign_pos,"-",0,80,IDbText::AlignMiddleCenter,hiberId,PS_SOLID,RGB(255,0,255),5);
	pText->SetNormal(real_work_norm);
}

//绘制参数化板参数示意图
//TODO:不支持普通角肋板,如塔脚肋板,且塔脚肋板的默认方位也不好,
bool CreateSketchPlaneByPlate(CLDSGeneralPlate* pPlate,btc::SKETCH_PLANE* pSketchPlane,
	int iFaceNo=0,const double* distFromSketch=NULL);
BOOL RedrawPlateParamDrawing(CLDSGeneralPlate* pGeneralPlate,const char *sPropKey,UCS_STRU *pObjectCS/*=NULL*/,bool removeOldEntities/*=true*/)
{
	if (pGeneralPlate==NULL)
		return FALSE;
	/*IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	if(pBuddyDrawing==NULL)
		return FALSE;
	DimStyleTable::InitDimStyle2(pBuddyDrawing->Database());
	long idDimStyle=DimStyleTable::dimStyle.m_idDimStyle;
	if(removeOldEntities)
		pBuddyDrawing->DeleteDbEntityByHiberMasterId(pPlate->handle);*/
	if (removeOldEntities)
	{
		g_pSolidDraw->DelWorkPlaneSketch(1);
		g_pSolidDraw->Draw();
	}
	HIBERID hiberId(pGeneralPlate->handle,HIBERARCHY(0,0,0,0));
	CLDSParamPlate *pParamPlate=NULL;
	CLDSPlate *pPlate=NULL;
	if (pGeneralPlate->GetClassTypeId()==CLS_PARAMPLATE)
		pParamPlate=(CLDSParamPlate*)pGeneralPlate;
	else if (pGeneralPlate->GetClassTypeId()==CLS_PLATE)
		pPlate=(CLDSPlate*)pGeneralPlate;
	else
		return FALSE;
	btc::SKETCH_PLANE sketch;
	if (pParamPlate)
	{
		if (pParamPlate->m_iParamType==TYPE_RIBPLATE)
		{
			long hAssPart1=0,hAssPart2=0,hDatumPart=0;
			pParamPlate->GetDesignItemValue('A',&hDatumPart);//基准构件
			pParamPlate->GetDesignItemValue('C',&hAssPart1);	//辅助构件1
			pParamPlate->GetDesignItemValue('D',&hAssPart2);	//辅助构件2
			CLDSPart* pDatumPart=console.FromPartHandle(abs(hDatumPart));
			if (pDatumPart==NULL)
				return FALSE;
			if (pDatumPart->IsPlate())
			{
				f3dPoint dimStart,dimEnd,dimPos;
				if (stricmp("X",sPropKey)==0||stricmp("Z",sPropKey)==0)
				{
					double fOffsetX=0,fOffsetZ=0;
					pParamPlate->GetDesignItemValue(KEY2C("X"),&fOffsetX);
					pParamPlate->GetDesignItemValue(KEY2C("Z"),&fOffsetZ);
					f3dPoint basePos=pParamPlate->ucs.origin-pParamPlate->ucs.axis_x*fOffsetX-pParamPlate->ucs.axis_z*fOffsetZ;
					basePos+=pParamPlate->ucs.axis_y*10;
					if (stricmp("X",sPropKey)==0)
					{
						double offset_dist=fOffsetZ>pParamPlate->Thick?0:pParamPlate->Thick*2;
						sketch.CreateStdPlane(basePos,pParamPlate->ucs.axis_x,pParamPlate->ucs.axis_z);
						g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),sketch.pVertexArr,sketch.VertexCount,pParamPlate->ucs.axis_x,"ΔX");
						//DrawParamArrowLine(pBuddyDrawing,basePos,pPlate->ucs.axis_x,pPlate->ucs.axis_y,"ΔX",offset_dist);
					}
					else
					{
						CreateSketchPlaneByPlate(pParamPlate,&sketch,1);
						g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),sketch.pVertexArr,sketch.VertexCount,pParamPlate->ucs.axis_z,"ΔZ",150,true,sketch.m_fUnderLength);
						//DrawParamArrowLine(pBuddyDrawing,basePos,pPlate->ucs.axis_z,pPlate->ucs.axis_y,"ΔZ");
					}
				}
				else if (stricmp("dx",sPropKey)==0||stricmp("dz",sPropKey)==0)
				{
					CLDSParamPlate::DESIGN_PARAM hDatumPlate,hTipBolt,Tx,Ty,TipOffsetX,TipOffsetZ;
					pParamPlate->GetDesignItemValue(KEY2C("TB"),&hTipBolt);	//制弯面末端T点的基准螺栓
					pParamPlate->GetDesignItemValue(KEY2C("Tx"),&Tx);		//制弯面末端T点的用户输入定义值
					pParamPlate->GetDesignItemValue(KEY2C("Ty"),&Ty);		//制弯面末端T点的用户输入定义值
					pParamPlate->GetDesignItemValue(KEY2C("A"),&hDatumPlate);//整数
					pParamPlate->GetDesignItemValue(KEY2C("dx"),&TipOffsetX);
					pParamPlate->GetDesignItemValue(KEY2C("dz"),&TipOffsetZ);
					CLDSGeneralPlate *pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(hDatumPlate.value.hValue,CLS_PARAMPLATE,CLS_PLATE);
					if (pDatumPlate==NULL)
						return FALSE;
					GEPOINT direction,vpTipPoint;
					if (hTipBolt.bExist)
					{
						CLDSBolt* pBendTipBolt=(CLDSBolt*)Ta.Parts.FromHandle(hTipBolt.value.hValue,CLS_BOLT);
						if (pBendTipBolt==NULL)
							return FALSE;
						if (!pDatumPlate->CalBoltIntersPos(pBendTipBolt,vpTipPoint,CLDSPlate::TOP_PLANE,NULL))
							return FALSE;
					}
					else if (Tx.bExist&&Ty.bExist)
					{	//自此行开始bendplane.vpTipPoint点为基准钢板pDatumPlate装配坐标系下局部展开坐标 wjh-2016.5.7
						PROFILE_VERTEX* pBendlineStart[2]={ NULL },*pBendlineEnd[2]={ NULL };
						int iResidePlane=-1;
						if (pDatumPlate->GetFaceN()>=2&&pBendlineStart[0]==NULL&&iResidePlane==-1)
							pDatumPlate->GetBendlineVertexies(&pBendlineStart[0],&pBendlineEnd[0],&pBendlineStart[1],&pBendlineEnd[1]);
						bool middlePick=true;
						GEPOINT vpLocalTipPoint(Tx.value.fValue,Ty.value.fValue,pDatumPlate->GetNormOffset());
						if (iResidePlane==-1&&pBendlineStart[0]&&pBendlineEnd[0])
						{
							double left_or_right=DistOf2dPtLine(vpLocalTipPoint,pBendlineStart[0]->vertex,pBendlineEnd[0]->vertex);
							//middlePick=(vpTipPoint-pBendlineStart[0]->vertex)*(vpTipPoint-pBendlineEnd[0]->vertex)<0;
							if (left_or_right<0&&middlePick)
								iResidePlane=1;
						}
						if (iResidePlane==-1&&iResidePlane==0&&pBendlineStart[1]!=NULL&&pBendlineEnd[1]!=NULL)
						{
							double left_or_right=DistOf2dPtLine(vpLocalTipPoint,pBendlineStart[1]->vertex,pBendlineEnd[1]->vertex)<0;
							//middlePick=(vpTipPoint-pBendlineStart[1]->vertex)*(vpTipPoint-pBendlineEnd[1]->vertex)<0;
							if (left_or_right<0&&middlePick)
								iResidePlane=2;
						}
						iResidePlane=max(0,iResidePlane);
						vpTipPoint=pDatumPlate->TransToActual(vpLocalTipPoint,iResidePlane+1,CLDSGeneralPlate::FROM_ACS|CLDSGeneralPlate::TO_MCS|CLDSGeneralPlate::IS_VERTEX);
						GEPOINT bendplanenorm=iResidePlane==0?pDatumPlate->ucs.axis_z:pDatumPlate->GetHuoquFaceNorm(iResidePlane-1);
						if (hDatumPlate.value.hValue>0)
							vpTipPoint+=pDatumPlate->GetThick()*bendplanenorm;
					}
					else
						return FALSE;
					direction=pParamPlate->HuoquAxisVec();
					RotateVectorAroundVector(direction,-0.5*Pi,pParamPlate->GetHuoquFaceNorm(0));
					double fOffsetX=0,fOffsetZ=0;
					pParamPlate->GetDesignItemValue(KEY2C("X"),&fOffsetX);
					pParamPlate->GetDesignItemValue(KEY2C("Z"),&fOffsetZ);
					GEPOINT basePos=vpTipPoint+pParamPlate->ucs.axis_y*10;
					if (stricmp("dx",sPropKey)==0)
					{
						double offset_dist=TipOffsetZ.value.fValue>pParamPlate->Thick?0:pParamPlate->Thick*2;
						//DrawParamArrowLine(pBuddyDrawing,basePos,direction,pPlate->ucs.axis_y,"δX",offset_dist);
						sketch.CreateStdPlane(basePos,direction,pParamPlate->ucs.axis_y);
						g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),sketch.pVertexArr,sketch.VertexCount,direction,"δX");
					}
					else
					{
						//DrawParamArrowLine(pBuddyDrawing,basePos,pPlate->GetHuoquFaceNorm(0),pPlate->ucs.axis_y,"δZ");
						CreateSketchPlaneByPlate(pParamPlate,&sketch,2);
						g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),sketch.pVertexArr,sketch.VertexCount,pParamPlate->GetHuoquFaceNorm(0),"δZ");
					}
				}
			}
			else if (pDatumPart->GetClassTypeId()==CLS_LINETUBE&&pParamPlate->GetRibPlateSubType()==CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE)
			{
				CLDSLineTube* pDatumTube=(CLDSLineTube*)pDatumPart;
				CLDSParamPlate::DESIGN_PARAM W,H;
				pParamPlate->GetDesignItemValue(KEY2C("E"),&W);
				pParamPlate->GetDesignItemValue(KEY2C("G"),&H);
				GEPOINT base_pos,work_norm,direction;
				base_pos=pParamPlate->ucs.origin;
				base_pos+=pParamPlate->ucs.axis_y*H.ToDouble()*0.5;
				if (stricmp("DX",sPropKey)==0)
				{
					direction=pParamPlate->ucs.axis_z;
					btc::SKETCH_PLANE sketch(base_pos,pParamPlate->ucs.axis_x,direction);
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),sketch.pVertexArr,sketch.VertexCount,sketch.normal,"ΔX",200);
				}
				else if (stricmp("DM",sPropKey)==0)
				{
					direction=pDatumTube->ucs.axis_y;
					btc::SKETCH_PLANE sketch(base_pos,pDatumTube->ucs.axis_z,direction);
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),sketch.pVertexArr,sketch.VertexCount,sketch.normal,"钢管轴向",200);
				}
				else if (stricmp("Rt",sPropKey)==0||stricmp("Rw",sPropKey)==0)
				{
					base_pos+=pParamPlate->ucs.axis_x*W.ToDouble()*(-0.5);
					direction=pParamPlate->ucs.axis_x;
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),&base_pos,1,direction,"X+");
				}
				else if (stricmp("Lt",sPropKey)==0||stricmp("Lw",sPropKey)==0)
				{
					base_pos+=pParamPlate->ucs.axis_x*W.ToDouble()*(-0.5);
					direction=-1*pParamPlate->ucs.axis_x;
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),&base_pos,1,direction,"X-");
				}
				else if (stricmp("Rh",sPropKey)==0||stricmp("Lh",sPropKey)==0)
				{
					base_pos+=pParamPlate->ucs.axis_x*W.ToDouble()*(-0.5);
					direction=pParamPlate->ucs.axis_y;
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),&base_pos,1,direction,"Y");
				}
			}
			else
				return FALSE;
		}
		else if (pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
		{
			long hDatumTube=0;
			pParamPlate->GetDesignItemValue('C',&hDatumTube);
			CLDSLineTube *pDatumTube=(CLDSLineTube*)console.FromPartHandle(hDatumTube,CLS_LINETUBE);
			if (pDatumTube==NULL)
				return FALSE;
			double fTubeR=pDatumTube->GetDiameter()*0.5;
			if (stricmp("T",sPropKey)==0||stricmp("S",sPropKey)==0)
			{
				GEPOINT basePos=pParamPlate->ucs.origin;
				if (pParamPlate->GetCircularPlateSubType()==1)
				{	//普通环形板
					double angle=0,R=0;
					pParamPlate->GetDesignItemValue('R',&R);
					pParamPlate->GetDesignItemValue('B',&angle);
					angle*=0.5*RADTODEG_COEF;
					basePos.Set(R,0,0);
					rotate_point_around_axis(basePos,angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
					coord_trans(basePos,pParamPlate->ucs,TRUE);
				}
				else
				{
					double W=0,R=0;
					pParamPlate->GetDesignItemValue('W',&W);
					pParamPlate->GetDesignItemValue('R',&R);
					if (W>0)
						basePos+=pParamPlate->ucs.axis_x*(fTubeR+W);
					else if (R>0)
						basePos+=pParamPlate->ucs.axis_x*R;
				}
				if (stricmp("T",sPropKey)==0)	//沿法线方向
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),&basePos,1,pParamPlate->ucs.axis_z,"ΔZ",200);
				else		//沿钢管轴向
				{	//根据环板的法线方向调整钢管轴向显示
					f3dPoint norm_vec=pDatumTube->ucs.axis_z;
					//实际操作中发现下列代码会导致环向肋板的提示方向与钢管轴向反向
					//推测应该是之前参数板计算偏移量以板法线为正向导致临时把方向调整的与真实偏移方向一致，
					//现在已在GetPropValueStr中偏移量显示正负值中进行相应调整　wjh-2018.2.1
					//if(pDatumTube->ucs.axis_z*pPlate->ucs.axis_z<0)
					//	norm_vec*=-1;
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),&basePos,1,norm_vec,"钢管轴向",200);
				}
			}
		}
		else
			return FALSE;
	}
	else if (pPlate)
	{
		if (pPlate->IsHuoQuLineCollinear3FacePlate())
		{
			if (stricmp("peakVertexOffsetY",sPropKey)==0||stricmp("extendVertexOffsetLen",sPropKey)==0)
			{
				f3dPoint basePos=pPlate->ucs.origin;
				if (stricmp("peakVertexOffsetY",sPropKey)==0)
				{
					sketch.CreateStdPlane(basePos,pPlate->ucs.axis_y);
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(0,255,0),sketch.pVertexArr,sketch.VertexCount,pPlate->ucs.axis_y,"");
				}
				else
				{
					sketch.CreateStdPlane(basePos,-1.0*pPlate->ucs.axis_x);
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(0,255,0),sketch.pVertexArr,sketch.VertexCount,-1.0*pPlate->ucs.axis_x,"");
				}
			}
		}
	}
	else
		return FALSE;
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL FireParamPlateHelpStr(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	CLDSView::PROMPT_LIFE lifeprompt(true);
	if (pCurPlate==NULL||pItem==NULL)
		return FALSE;
	long id=CLDSParamPlate::GetPropID("m_iParamType");
	CPropTreeItem *pParentItem=pItem->m_pParent;
	while (pParentItem)
	{
		if (id==pParentItem->m_idProp)
			break;
		pParentItem=pParentItem->m_pParent;
	}
	CTowerPropertyDlg *pPropDlg=(CTowerPropertyDlg*)pPropList->GetParent();
	if (pPropDlg==NULL)
		return FALSE;
	if (pParentItem&&pCurPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
	{
		long iType=0,iCirEdge=0;
		pCurPlate->GetDesignItemValue(KEY2C("I"),&iType);
		pCurPlate->GetDesignItemValue(KEY2C("C"),&iCirEdge);
		if (iType==1)
			lifeprompt.SetPromptPicture(IDB_BMP_LINEANGLE_RIB_PLATE2);
		else if (iType==2)
			lifeprompt.SetPromptPicture(IDB_BMP_LINEANGLE_RIB_PLATE3);
		else
		{
			if (iCirEdge==1)
				lifeprompt.SetPromptPicture(IDB_BMP_LINEANGLE_RIB_PLATE1);
			else
				lifeprompt.SetPromptPicture(IDB_BMP_LINEANGLE_RIB_PLATE);
		}
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_RIBPLATE)
	{
		long hDatumPart=0,hRelaPart1=0,hRelaPart2=0;
		pCurPlate->GetDesignItemValue('A',&hDatumPart);
		pCurPlate->GetDesignItemValue('C',&hRelaPart1);
		pCurPlate->GetDesignItemValue('D',&hRelaPart2);
		CLDSPart *pDatumPart=console.FromPartHandle(abs(hDatumPart));
		CLDSPart *pRelaPart1=console.FromPartHandle(abs(hRelaPart1));
		CLDSPart *pRelaPart2=console.FromPartHandle(abs(hRelaPart2));
		if (pDatumPart==NULL)
			return FALSE;
		if (pRelaPart1&&pRelaPart2==NULL)	//角肋板
		{
			if (pDatumPart->IsTube())
			{
				if (pRelaPart1->IsTube())
					lifeprompt.SetPromptPicture(IDB_BMP_TUBE_BETWEEN_PLATE);	//钢管夹板
				else
					lifeprompt.SetPromptPicture(IDB_BMP_CORNER_RIB_PLATE);	//钢管肘板
			}
			else //if(pDatumPart->IsPlate())
			{
				CXhChar200 imagefile("%simages\\prompt_images\\bmp_ribcorner.bmp",theApp.execute_path);
				lifeprompt.SetPromptPicture(imagefile);
			}
		}
		else if (pRelaPart1&&pRelaPart2)	//槽肋板
		{
			if (pDatumPart->IsTube())
				lifeprompt.SetPromptPicture(IDB_BMP_SLOT_RIB_PLATE);
			else
			{
				CXhChar200 imagefile("%simages\\prompt_images\\bmp_ribslot.bmp",theApp.execute_path);
				lifeprompt.SetPromptPicture(imagefile);
				//lifeprompt.SetPromptPicture(IDB_BMP_SLOT_RIB_PLATE2);
			}
		}
		else //if(pRelaPart1==NULL&&pRelaPart2==NULL)
		{
			if (pDatumPart->IsPlate())		//钢板肋板
				lifeprompt.SetPromptPicture(IDB_BMP_PLATE_RIB_PLATE);
			else if (pDatumPart->IsTube())	//钢管肋板
			{
				long iWorkNormStyle=0;	//肋板定位基准边:0.基准边 1.焊道线 2.指定方向
				pCurPlate->GetDesignItemValue(KEY2C("Nt"),&iWorkNormStyle);
				if (iWorkNormStyle!=0)
					lifeprompt.SetPromptPicture(IDB_BMP_TUBE_RIB_PLATE2);
				else
					lifeprompt.SetPromptPicture(IDB_BMP_TUBE_RIB_PLATE1);
			}
		}
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_SHOEPLATE)
	{
		long iConnStyle=0;
		pCurPlate->GetDesignItemValue(KEY2C("CS"),&iConnStyle);
		CXhChar200 imagefile;
		if (iConnStyle==0)	//螺栓连接
			imagefile.Printf("%simages\\prompt_images\\shoe_plate.bmp",theApp.execute_path);
		else				//对焊连接
			imagefile.Printf("%simages\\prompt_images\\shoe_plate2.bmp",theApp.execute_path);
		lifeprompt.SetPromptPicture(imagefile);
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_BASEPLATE)
	{
		long shapeType=0;
		pCurPlate->GetDesignItemValue('S',&shapeType);
		CXhChar200 imagefile("%simages\\prompt_images\\base_plate1.bmp",theApp.execute_path);
		if (shapeType==1)
			imagefile.Printf("%simages\\prompt_images\\base_plate2.bmp",theApp.execute_path);
		lifeprompt.SetPromptPicture(imagefile);
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_STATIONCONNPLATE)
	{	//端撑端板示意图
		CXhChar200 imagefile("%simages\\prompt_images\\station_conn_plate.bmp",theApp.execute_path);
		lifeprompt.SetPromptPicture(imagefile);
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_STATIONSHOEPLATE)
	{	//立柱夹板
		long hEndPlate=0;
		pCurPlate->GetDesignItemValue(KEY2C("LE"),&hEndPlate);
		CLDSParamPlate* pEndPlate=(CLDSParamPlate*)console.FromPartHandle(hEndPlate,CLS_PARAMPLATE);
		CXhChar200 imagefile("%simages\\prompt_images\\station_shoe_plate2.bmp",theApp.execute_path);
		if (pEndPlate&&pEndPlate->m_iParamType==TYPE_STATIONCONNPLATE)
			imagefile.Printf("%simages\\prompt_images\\station_shoe_plate1.bmp",theApp.execute_path);
		lifeprompt.SetPromptPicture(imagefile);
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
		lifeprompt.SetPromptPicture(IDB_BMP_SHIELDEDWATER_PLATE);
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{
		long Y=0;
		pCurPlate->GetDesignItemValue('Y',&Y);
		if (Y==0)
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_COVERPLATE2);
		else if (Y==3)
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_COVERPLATE4);
		else
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_COVERPLATE3);
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_CIRRIBPLATE)
	{
		int iSubType=pCurPlate->GetCircularPlateSubType();
		if (iSubType==CLDSParamPlate::TYPE_COMMON_CIR_PLATE)		//普通环向肋板
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_PLATE);
		else if (iSubType==CLDSParamPlate::TYPE_SPECIAL_CIR_PLATE)//异形法兰
			lifeprompt.SetPromptPicture(IDB_BMP_SPECIAL_PROFILE_FL);
		else if (iSubType==CLDSParamPlate::TYPE_3WL_CIR_PLATE)	//3WL肋板
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE1);
		else if (iSubType==CLDSParamPlate::TYPE_3WA_CIR_PLATE)	//3WA肋板
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE2);
		else if (iSubType==CLDSParamPlate::TYPE_2WL_CIR_PLATE)	//2WL肋板
		{
			double angle=0;
			pCurPlate->GetDesignItemValue('B',&angle);
			lifeprompt.SetPromptPicture((angle<=0)?IDB_BMP_CIRCULAR_RIBPLATE3:IDB_BMP_CIRCULAR_RIBPLATE7);
		}
		else if (iSubType==CLDSParamPlate::TYPE_2WA_CIR_PLATE)	//2WA肋板
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE4);
		else if (iSubType==CLDSParamPlate::TYPE_CIR_AISLE_PLATE)	//走道板
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE8);
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_TUBERIBPLATE)
		lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE5);
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_FL)
		lifeprompt.SetPromptPicture(IDB_BMP_PLATE_FL2);
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_TUBESTAYWIREPLATE)
		lifeprompt.SetPromptPicture(IDB_BMP_STAYWIRE_RIBPLATE);
	else if (pParentItem&&pCurPlate->m_iParamType == TYPE_PEDALPLATE)
	{
		long C = 0;
		pCurPlate->GetDesignItemValue('C', &C);
		if (C == 0)
			lifeprompt.SetPromptPicture(IDB_BMP_PEDAL_PLATE1);
		else
			lifeprompt.SetPromptPicture(IDB_BMP_PEDAL_PLATE2);
	}
	else
	{
		pPropDlg->SetPropHelpBmpID(NULL);
		::SetWindowText(pPropList->m_hPromptWnd,pItem->m_lpNodeInfo->m_strPropHelp);
		::UpdateWindow(pPropList->m_hPromptWnd);
	}
	KEY2C key=CLDSParamPlate::GetPropKey(pItem->m_idProp,pCurPlate->m_iParamType);
	RedrawPlateParamDrawing(pCurPlate,key.Label());
	return TRUE;
}
BOOL ParamRibPlateButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	long h=0;
	CString valueStr;
	CModifyPartHandleDlg modifyhdlg;
	modifyhdlg.m_iMirMsg=0;
	CLDSParamPlate* pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("A",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//定位基准构件
		pParamPlate->GetDesignItemValue(KEY2C("A"),&h);
		modifyhdlg.m_sCaption="肋板基准构件";
		modifyhdlg.m_sTitle="肋板基准构件";
		modifyhdlg.m_sHandle.Format("0X%X",h);
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
		modifyhdlg.SetCmdPickPromptStr("请选肋板基准构件:");
		modifyhdlg.SetPickObjTypeId(CLS_PLATE);
		if (modifyhdlg.DoModal()!=IDOK)
			return FALSE;
		CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
		if (pPart==NULL || !pPart->IsPart())
		{
			AfxMessageBox("输入句柄有误，找不到相应构件！");
			return FALSE;
		}
		valueStr.Format("0X%X",modifyhdlg.m_nHandle);
		pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("A",pParamPlate->m_iParamType),valueStr);
		g_pSolidDraw->ReleaseSnapStatus();
	}
	else if (CLDSParamPlate::GetPropID("C",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//辅助构件1
		pParamPlate->GetDesignItemValue(KEY2C("C"),&h);
		modifyhdlg.m_sCaption="肋板辅助构件1";
		modifyhdlg.m_sTitle="肋板辅助构件1";
		modifyhdlg.m_sHandle.Format("0X%X",abs(h));
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮_
		modifyhdlg.SetCmdPickPromptStr("请选辅助构件:");
		modifyhdlg.SetPickObjTypeId(CLS_PLATE);
		if (modifyhdlg.DoModal()!=IDOK)
			return FALSE;
		CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
		if (pPart==NULL || !pPart->IsPlate())
		{
			AfxMessageBox("输入句柄有误，找不到相应构件！");
			return FALSE;
		}
		valueStr.Format("0X%X",modifyhdlg.m_nHandle);
		pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("C",pParamPlate->m_iParamType),valueStr);
		g_pSolidDraw->ReleaseSnapStatus();
	}
	else if (CLDSParamPlate::GetPropID("O",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//定位螺栓
		pParamPlate->GetDesignItemValue(KEY2C("O"),&h);
		modifyhdlg.m_sCaption="肋板定位螺栓";
		modifyhdlg.m_sTitle="肋板定位螺栓";
		modifyhdlg.m_sHandle.Format("0X%X",h);
		modifyhdlg.m_bDispSelectBtn=TRUE;
		modifyhdlg.SetCmdPickPromptStr("请选定位螺栓:");
		modifyhdlg.SetPickObjTypeId(CLS_BOLT);
		if (modifyhdlg.DoModal()!=IDOK)
			return FALSE;
		CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
		if (pPart==NULL)
		{
			AfxMessageBox("输入句柄有误，找不到相应构件！");
			return FALSE;
		}
		valueStr.Format("0X%X",modifyhdlg.m_nHandle);
		pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("O",pParamPlate->m_iParamType),valueStr);
		g_pSolidDraw->ReleaseSnapStatus();
	}
	else if (CLDSParamPlate::GetPropID("V",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//方向杆件
		pParamPlate->GetDesignItemValue(KEY2C("V"),&h);
		modifyhdlg.m_sCaption="肋板方向杆件";
		modifyhdlg.m_sTitle="肋板方向杆件";
		modifyhdlg.m_sHandle.Format("0X%X",h);
		modifyhdlg.m_bDispSelectBtn=TRUE;
		modifyhdlg.SetCmdPickPromptStr("请选方向杆件:");
		modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
		if (modifyhdlg.DoModal()!=IDOK)
			return FALSE;
		CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
		if (pPart==NULL || !pPart->IsLinePart())
		{
			AfxMessageBox("输入句柄有误，找不到相应构件！");
			return FALSE;
		}
		valueStr.Format("0X%X",modifyhdlg.m_nHandle);
		pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("V",pParamPlate->m_iParamType),valueStr);
		g_pSolidDraw->ReleaseSnapStatus();
	}
	else
		return FALSE;
	for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
	{
		g_pSolidDraw->SetEntSnapStatus(pParamPlate->handle);
		if (CLDSParamPlate::GetPropID("A",pParamPlate->m_iParamType)==pItem->m_idProp)
			pParamPlate->SetDesignItemValue(KEY2C("A"),modifyhdlg.m_nHandle,NULL,0,1);
		else if (CLDSParamPlate::GetPropID("C",pParamPlate->m_iParamType)==pItem->m_idProp)
			pParamPlate->SetDesignItemValue(KEY2C("C"),modifyhdlg.m_nHandle,NULL,0,1);
		else if (CLDSParamPlate::GetPropID("O",pParamPlate->m_iParamType)==pItem->m_idProp)
			pParamPlate->SetDesignItemValue(KEY2C("O"),modifyhdlg.m_nHandle,NULL,0,1);
		else if (CLDSParamPlate::GetPropID("V",pParamPlate->m_iParamType)==pItem->m_idProp)
			pParamPlate->SetDesignItemValue(KEY2C("V"),modifyhdlg.m_nHandle,NULL,0,1);
	}
	return TRUE;
}
BOOL ParamPlateButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem,*pParentItem;
	CLDSParamPlate *pFirstPlate=NULL,*pPlate=NULL;
	CReportVectorDlg vecDlg;
	CCfgPartNoDlg cfgdlg;
	CString valueStr = _T("");

	if (pView->selectObjs.GetNodeNum()>0)
		pFirstPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	BOOL bRetCode=FALSE;
	CXhChar16 sSoftName("LMA");
#if defined(__LDS_FILE_)
	sSoftName.Copy("LDS");
#elif defined(__LMA_FILE_)
	sSoftName.Copy("LMA");
#elif defined(__TMA_FILE_)
	sSoftName.Copy("TMA");
#elif defined(__TSA_FILE_)
	sSoftName.Copy("TSA");
#elif defined(__TDA_FILE_)
	sSoftName.Copy("TDA");
#endif
	CPropListOper<CLDSParamPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&pView->selectObjs);
	if (pItem->m_idProp==CLDSParamPlate::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstPlate->cfgword;
		if (pFirstPlate->IsBodyObj())
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if (cfgdlg.DoModal()==IDOK)
			pFirstPlate->cfgword = cfgdlg.cfgword;		//在此处更新第一个节点的配材料号
		CXhChar100 sText;
		if (pFirstPlate->GetPropValueStr(pItem->m_idProp,sText))	//更新配材号
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("sPartNo"))
	{
		if (pFirstPlate->m_bStdPart)
		{	//在标准构件库中查找可使用的标准构件，修改标准构件编号
			//重新生成标准构件(使用旧句柄),更新厚度、材质、相关钢板外形
			CLDSStdPartModel stdPartModel;
			if (pFirstPlate->m_iParamType==TYPE_FLD)
			{
				CXhChar16 sPartNo(pFirstPlate->GetPartNo());
				int len=sPartNo.GetLength();
				char cMark=sPartNo.At(len-1);
				if (cMark<'0'||cMark>'9')	//存在壁厚分组标识字符
				{
					sPartNo.At(len-1)='?';
					pFirstPlate->SetPartNo(sPartNo);
				}
				pFirstPlate->GetStdPartModel(&stdPartModel);
				pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("sPartNo"),pFirstPlate->GetPartNo());
			}
		}
		else
		{
			CModifyRelativeObjPartNoDlg modify_partno_dlg;
			modify_partno_dlg.m_pWorkPart=pFirstPlate;
			if (modify_partno_dlg.DoModal()==IDOK)
			{
				pFirstPlate->SetPartNo(modify_partno_dlg.m_sCurObjPartNo.GetBuffer(0));
				pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
				SEGI iSeg;
				if (CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(modify_partno_dlg.m_sCurObjPartNo,&iSeg,NULL))
				{
					pFirstPlate->iSeg=iSeg;
					pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("iSeg"),iSeg.ToString());
				}
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs"))
	{
		CPartUcsDlg part_dlg_ucs;
		part_dlg_ucs.ucs=pFirstPlate->ucs;
		if (part_dlg_ucs.DoModal()==IDOK)
		{
			for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
				pPlate->ucs=part_dlg_ucs.ucs;
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.origin"))
	{
		if (pFirstPlate->design.origin.datum_pos_style==10)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Assembly CS origin";
#else 
			vecDlg.m_sCaption = "装配坐标系原点位置";
#endif
			vecDlg.m_fVectorX = pFirstPlate->ucs.origin.x;
			vecDlg.m_fVectorY = pFirstPlate->ucs.origin.y;
			vecDlg.m_fVectorZ = pFirstPlate->ucs.origin.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.origin.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.origin.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.origin.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.origin.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.origin.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.origin.z"),CString(sText));
				for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
					pPlate->ucs.origin.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
		else
		{
			CDatumPointPropertyDlg desPointDlg;
			desPointDlg.m_pCurEditObj=pFirstPlate;
			desPointDlg.m_xPoint=pFirstPlate->design.origin;
			if (desPointDlg.m_xPoint.datum_pos_style==0)
				desPointDlg.m_xPoint.SetPosition(pFirstPlate->ucs.origin);
			if (desPointDlg.DoModal()==IDOK)
			{
				pFirstPlate->design.origin=desPointDlg.m_xPoint;
				pFirstPlate->design.origin.UpdatePos(&Ta);
				pFirstPlate->ucs.origin=pFirstPlate->design.origin.Position();
				pFirstPlate->SetModified();
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_x"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "X Axis Direction";
#else 
		vecDlg.m_sCaption = "装配时X轴方向";
#endif
		vecDlg.m_fVectorX = pFirstPlate->ucs.axis_x.x;
		vecDlg.m_fVectorY = pFirstPlate->ucs.axis_x.y;
		vecDlg.m_fVectorZ = pFirstPlate->ucs.axis_x.z;
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_x.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_x.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_x.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if (vecDlg.DoModal()==IDOK)
		{
			CXhChar100 sText;
			sText.Printf("%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_x.x"),CString(sText));
			sText.Printf("%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_x.y"),CString(sText));
			sText.Printf("%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_x.z"),CString(sText));
			for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			{
				pPlate->ucs.axis_x.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_x*1,0);
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_y"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Y Axis Direction";
#else 
		vecDlg.m_sCaption = "装配时Y轴方向";
#endif
		vecDlg.m_fVectorX = pFirstPlate->ucs.axis_y.x;
		vecDlg.m_fVectorY = pFirstPlate->ucs.axis_y.y;
		vecDlg.m_fVectorZ = pFirstPlate->ucs.axis_y.z;
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_y.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_y.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_y.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if (vecDlg.DoModal()==IDOK)
		{
			CXhChar100 sText;
			sText.Printf("%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_y.x"),CString(sText));
			sText.Printf("%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_y.y"),CString(sText));
			sText.Printf("%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_y.z"),CString(sText));
			for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			{
				pPlate->ucs.axis_y.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_y*1,1);
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_z"))
	{
		if (pFirstPlate->m_iParamType!=TYPE_XEND)
		{	//非十字插板暂时不支持定制插板工作法线
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Z Axis Direction";
#else 
			vecDlg.m_sCaption = "装配时Z轴方向";
#endif
			vecDlg.m_fVectorX = pFirstPlate->ucs.axis_z.x;
			vecDlg.m_fVectorY = pFirstPlate->ucs.axis_z.y;
			vecDlg.m_fVectorZ = pFirstPlate->ucs.axis_z.z;
			pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_z.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_z.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_z.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_z.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_z.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_z.z"),CString(sText));
				for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
				{
					pPlate->ucs.axis_z.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_z*1,2);
				}
			}
		}
		else
		{	//十字插板支持定制插板工作法线
			CDesignVectorPropDlg vector_dlg;
			vector_dlg.m_hCurEditPart=pFirstPlate->handle;
			vector_dlg.m_xVector = pFirstPlate->workNorm;
			//显示坐标系
			UCS_STRU object_ucs,ucs = pFirstPlate->ucs;
			g_pSolidSet->GetObjectUcs(object_ucs);
			coord_trans(ucs.origin,object_ucs,TRUE);
			vector_trans(ucs.axis_x,object_ucs,TRUE);
			vector_trans(ucs.axis_y,object_ucs,TRUE);
			vector_trans(ucs.axis_z,object_ucs,TRUE);
			g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
			//g_pSolidSet->SetDispWorkUcs();
			g_pSolidDraw->Draw();
			if (vector_dlg.DoModal()==IDOK)
			{
				pFirstPlate->workNorm=vector_dlg.m_xVector;
				pFirstPlate->workNorm.UpdateVector(&Ta);
				//调整法线设计参数后重新设计当前钢板
				//pFirstPlate->DesignSetupUcs();
				pFirstPlate->DesignPlate();
				pFirstPlate->SetModified();
				pFirstPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pFirstPlate->GetSolidPartObject());
				//更新Z轴方向的显示内容
				CXhChar100 sText;
				long idProp=CLDSParamPlate::GetPropID("ucs.axis_z.x");
				if (pFirstPlate->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,sText);
				idProp=CLDSParamPlate::GetPropID("ucs.axis_z.y");
				if (pFirstPlate->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,sText);
				idProp=CLDSParamPlate::GetPropID("ucs.axis_z.z");
				if (pFirstPlate->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,sText);
				if (pFirstPlate->m_iParamType==TYPE_XEND)
				{	//重新设计插板附加板中已当前钢板为定位构件的包板
					ATTACHPART_ITEM *pItem=NULL;
					for (pItem=pFirstPlate->GetFirstAttachPart();pItem;pItem=pFirstPlate->GetNextAttachPart())
					{
						CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pItem->h,CLS_PLATE);
						if (pPlate==NULL)
							continue;
						if (pPlate->designInfo.norm.hVicePart==pFirstPlate->handle
							&&pPlate->designInfo.norm.norm_style>0)
						{
							pPlate->DesignSetupUcs();
							pPlate->DesignPlate();
							pPlate->SetModified();
							pPlate->Create3dSolidModel();
							g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
						}
					}
					//重新设计参数板时将其附带的螺栓重新设计
					for (CLsRef *pLsRef=pFirstPlate->GetFirstLsRef();pLsRef;pLsRef=pFirstPlate->GetNextLsRef())
					{
						CLDSBolt *pBolt=pLsRef->GetLsPtr();
						pBolt->correct_worknorm();
						pBolt->correct_pos();
						pBolt->CalGuigeAuto();
						pBolt->SetModified();
						pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
						g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
					}
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
			//
			//g_pSolidSet->SetDispWorkUcs(FALSE);
			g_pSolidDraw->BatchClearCS(2);
			g_pSolidDraw->Draw();
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("plate_work_plane"))
	{
		CPartWorkPlaneDlg planeDlg;
		planeDlg.m_pDatumPart=pFirstPlate;
		planeDlg.DoModal();
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_iParamType"))
	{	//钢板设计参数列表
		CUndoOperObject undo(&Ta,true);
		if (pFirstPlate->m_iParamType==TYPE_RIBPLATE)
		{
			CDesignRibbedPlateDlg ribPlateDlg;
			ribPlateDlg.SetRibPlate(pFirstPlate);
			ribPlateDlg.DoModal();
			RebuildOrUpdateParamsPropertyList(pPropList,pItem);
		}
		else if (pFirstPlate->m_iParamType==TYPE_ELBOWPLATE)
		{	//肘板按照肋板对话框进行显示
			CDesignRibbedPlateDlg ribPlateDlg;
			pFirstPlate->ShiftElbowAndRibPlateSchema(TYPE_RIBPLATE);
			ribPlateDlg.SetRibPlate(pFirstPlate);
			ribPlateDlg.DoModal();
			pFirstPlate->ShiftElbowAndRibPlateSchema(TYPE_ELBOWPLATE);
			RebuildOrUpdateParamsPropertyList(pPropList,pItem);
		}
		else if (pFirstPlate->m_iParamType==TYPE_LINEANGLERIBPLATE||
			pFirstPlate->m_iParamType==TYPE_TUBESTAYWIREPLATE)
		{
			CDesLineAngleRibPlateDlg lineAngleRibPlateDlg;
			lineAngleRibPlateDlg.m_bNewRibPlate=FALSE;
			lineAngleRibPlateDlg.m_pRibPlate=pFirstPlate;
			lineAngleRibPlateDlg.DoModal();
			RebuildOrUpdateParamsPropertyList(pPropList,pItem);
		}
#ifdef __COMPLEX_PART_INC_
		else if (pFirstPlate->m_iParamType==TYPE_CIRCOVERPLATE
			||pFirstPlate->m_iParamType==TYPE_TUBERIBPLATE
			||pFirstPlate->m_iParamType==TYPE_CIRRIBPLATE
			||pFirstPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
		{
			CDesignCircularPlateDlg cirPlateDlg;
			cirPlateDlg.m_bNewCirPlate=FALSE;
			cirPlateDlg.m_pCirPlate=pFirstPlate;
			cirPlateDlg.DoModal();
		}
#endif
		else
		{
			CParamPlateParamListDlg paramDlg;
			paramDlg.m_pParamPlate=pFirstPlate;
			paramDlg.DoModal();
		}
		/*
		if(pItem->m_lpNodeInfo->m_strPropValue[0]=='2')
		dlgDesginParam.m_bAddItem = TRUE;
		else
		dlgDesginParam.m_bAddItem = FALSE;
		dlgDesginParam.m_pWorkPart = pFirstPlate;
		if(IDOK==dlgDesginParam.DoModal())
		pFirstPlate = ((CLDSParamPlate*)dlgDesginParam.m_pWorkPart);
		CString sPartNo=pFirstPlate->sPartNo;
		for(CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{	//类型相同且编号相同的参数板的参数可以一起修改
		if(pParamPlate==pFirstPlate)
		continue;
		if(pParamPlate->m_iParamType!=pFirstPlate->m_iParamType)
		continue;
		if(sPartNo.CompareNoCase(pParamPlate->sPartNo)!=0)
		continue;
		for(DESIGN_PLATE_ITEM *pItem=pFirstPlate->designItemList.GetFirst();pItem;pItem=pFirstPlate->designItemList.GetNext())
		{
		//if(pItem->dataType==1)	//句柄类型的参数跳过
		//	continue;
		if(!pItem->bModify)
		continue;
		if(pItem->dataType==1)
		pParamPlate->SetDesignItemValue(pItem->cItem,pItem->value.fValue,pItem->sNotes,1);
		else
		pParamPlate->SetDesignItemValue(pItem->cItem,pItem->value.fValue,pItem->sNotes);
		}
		}*/
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("relativeObjs"))
	{
		CRelativeObjectDlg relativeDlg;
		relativeDlg.m_pWorkObj = pFirstPlate;
		if (relativeDlg.DoModal()==IDOK)
		{
			CXhChar200 sText;
			pFirstPlate->GetPropValueStr(pItem->m_idProp,sText);
			pPropList->SetItemPropValue(pItem->m_idProp,sText);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("shadowInfo"))
	{
		CDesignShadowDlg shadowdlg;
		shadowdlg.SetShadowObj((CLDSObject*)pFirstPlate);
		shadowdlg.DoModal();
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_hPartWeldParent"))
	{
		//通过选择构件修改焊接父构件，现在仅可以修改单个构件的焊接父构件
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Parametric Weld Parent Part";
		modifyhdlg.m_sTitle="Weld Parent Part:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
		modifyhdlg.SetCmdPickPromptStr("Please select weld parent part:");
#else 
		modifyhdlg.m_sCaption="修改参数化钢板焊接父构件";
		modifyhdlg.m_sTitle="焊接父构件:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
		modifyhdlg.SetCmdPickPromptStr("请选择焊接父构件:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if (IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if (pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong,so can't find the corresponding part！",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"输入句柄有误，找不到相应构件！",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("m_hPartWeldParent"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={ 1,2,4 };
					CLDSPlate* pMirPlate=NULL;
					for (int i=0;i<3;i++)
					{
						if ((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirPlate=(CLDSPlate*)pPlate->GetMirPart(msg);
						if (pMirPlate)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if (pMirPart)	//查找所选父构件的对称构件 wht 11-05-07
								pMirPlate->m_hPartWeldParent=pMirPart->handle;
						}
					}
					pPlate->m_hPartWeldParent=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
				}
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.origin")||
		pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.axis_x")||pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.axis_y"))
	{
		f3dPoint spec_point;
		CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
		CString cmdStr="请选择当前工作坐标系原点所在的顶点或螺栓:";
		int iMenu=0;
		if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.axis_x"))
		{
			iMenu=1;
			cmdStr="请选择当前工作坐标系X轴轴向标定点所在的顶点或螺栓:";
		}
		else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.axis_y"))
		{
			iMenu=2;
			cmdStr="请选择当前工作坐标系Y轴轴向标定点所在的顶点或螺栓:";
		}
		pCmdLine->FillCmdLine(cmdStr,"");
		CSnapTypeVerify verifier;
		DWORD dwhObj=0,dwExportFlag=0;
		verifier.SetVerifyFlag(OBJPROVIDER::DRAWINGSPACE,GetSingleWord(IDbEntity::DbPoint));
		verifier.AddVerifyType(OBJPROVIDER::SOLIDSPACE,CLS_BOLT);
		CLDSBolt* pPickBolt=NULL;
		PROFILE_VERTEX* pPickVertex=NULL;
		while (true)
		{
			if ((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			if (obj.provider==OBJPROVIDER::SOLIDSPACE)
				pPickBolt=(CLDSBolt*)Ta.FromPartHandle(obj.hObj,CLS_BOLT);
			else if (obj.provider==OBJPROVIDER::DRAWINGSPACE)
			{
				IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
				IDbEntity* pEnt=pBuddyDrawing->GetDbEntity(obj.idEnt);
				if (pEnt==NULL || pEnt->GetDbEntType()!=IDbEntity::DbPoint)
					continue;	//非轮廓点
				if (pEnt->GetHiberId().masterId!=pFirstPlate->handle)
					continue;	//非当前钢板上的轮廓点
				DWORD dwObjectId=pEnt->GetHiberId().HiberDownId(1);
				for (PROFILE_VERTEX* pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
				{
					if (pVertex->vertex.ID==dwObjectId)
					{
						pPickVertex=pVertex;
						break;
					}
				}
			}
			else
				continue;
			if (pPickBolt!=NULL||pPickVertex!=NULL)
				break;
		}
		pCmdLine->FinishCmdLine();
		PLATEEDIT_STATE* pState=CLDSParamPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		//计算工作坐标系
		f3dPoint xLocalPickPoint;
		if (pPickBolt)
		{
			pFirstPlate->GetBoltIntersPos(pPickBolt,xLocalPickPoint);
			coord_trans(xLocalPickPoint,pFirstPlate->ucs,FALSE,TRUE);
		}
		else
			xLocalPickPoint=pPickVertex->vertex;
		if (iMenu==0)		//捕捉原点
		{
			console.m_curWCS.origin = xLocalPickPoint;
			//
			pState->m_fLocalWcsOrgX=xLocalPickPoint.x;
			pState->m_fLocalWcsOrgY=xLocalPickPoint.y;
		}
		else if (iMenu==1)	//捕捉X轴向标定点
		{	//捕捉轮廓点
			console.m_curWCS.axis_x = xLocalPickPoint-console.m_curWCS.origin;
			normalize(console.m_curWCS.axis_x);
			console.m_curWCS.axis_z.Set(0,0,1);
			console.m_curWCS.axis_y = console.m_curWCS.axis_z^console.m_curWCS.axis_x;
			//
			pState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
			pState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
			pState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
			pState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
		}
		else if (iMenu==2)	//捕捉板上螺栓
		{
			console.m_curWCS.axis_y = xLocalPickPoint-console.m_curWCS.origin;
			normalize(console.m_curWCS.axis_y);
			console.m_curWCS.axis_z.Set(0,0,1);
			console.m_curWCS.axis_x = console.m_curWCS.axis_y^console.m_curWCS.axis_z;
			//
			pState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
			pState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
			pState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
			pState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
		}
		//在钢板实体上显示工作坐标系
		UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
		g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
		g_pSolidDraw->Draw();
		//更新属性栏显示
		if (iMenu==0)
		{
			oper.UpdatePropItemValue("wcs.origin");
			oper.UpdatePropItemValue("wcs.origin.x");
			oper.UpdatePropItemValue("wcs.origin.y");
		}
		else
		{
			oper.UpdatePropItemValue("wcs.axis_x");
			oper.UpdatePropItemValue("wcs.axis_x.a");
			oper.UpdatePropItemValue("wcs.axis_y");
		}
	}
	else if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.origin.x"))
	{
		PLATEEDIT_STATE* pEditState=CLDSPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		pEditState->m_fLocalWcsOrgX=console.m_curWCS.origin.x=atof(valueStr);
		UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
		g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
		g_pSolidDraw->Draw();
		return TRUE;
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.origin.y"))
	{
		PLATEEDIT_STATE* pEditState=CLDSParamPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		pEditState->m_fLocalWcsOrgY=console.m_curWCS.origin.y=atof(valueStr);
		UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
		g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
		g_pSolidDraw->Draw();
		return TRUE;
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.axis_x.a"))
	{
		double alfa=atof(valueStr)*RADTODEG_COEF;	//由角度制转为弧度制
		PLATEEDIT_STATE* pEditState=CLDSParamPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		console.m_curWCS.axis_x=GEPOINT(cos(alfa),sin(alfa));
		console.m_curWCS.axis_y.Set(-console.m_curWCS.axis_x.y,console.m_curWCS.axis_x.x);
		pEditState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
		pEditState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
		pEditState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
		pEditState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
		oper.UpdatePropItemValue("wcs.axis_x");
		oper.UpdatePropItemValue("wcs.axis_y");
		UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
		g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
		g_pSolidDraw->Draw();
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("InsertProfileVertex"))	//添加轮廓顶点
		return pView->InsertPlankVertex();
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("AddNewHole"))	//添加螺栓
		return pView->InsertPlankBolt();
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("DeleteFeature"))	//删除钢板特征属性
		return pView->DeletePlankFeature();
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("FilletVertex"))	//
		return pView->FilletPlateCorner();
	else if (pFirstPlate->m_iParamType==TYPE_RIBPLATE)
		bRetCode=ParamRibPlateButtonClick(pPropList,pItem,pView->selectObjs);
	else
		return FALSE;
	try {
		if (bRetCode)
		{	//参数修改后，肋板重新设计
			for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
				pPlate->DesignPlate();
		}
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->SetModified();
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		}
	}
	catch (char* error)
	{
		AfxMessageBox(error);
	}
	catch (...)
	{
		AfxMessageBox("ParamPlateButtonClick encountered an unexpected error");
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL FireParamPlatePopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pFirstPlate=NULL,*pPlate=NULL;
	if (pView->selectObjs.GetNodeNum()>0)
		pFirstPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pItem->m_idProp==CLDSParamPlate::GetPropID("plateAttachElbowPlate"))
	{
		CXhChar100 sText;
		if (0==iMenu)		//查看法兰关联肘板
		{
			g_pLsRefDlg->InitLsRefDlg(pFirstPlate,0,TRUE);
			/*
			g_pLsRefDlg->m_pWorkPart = pFirstPlate;
			g_pLsRefDlg->m_bPlateAttachPart = TRUE;
			if(g_pLsRefDlg->DoModal()==IDOK)
				pFirstPlate = (CLDSParamPlate*)(dlgLsRef.m_pWorkPart);
			*/
		}
		else if (1==iMenu)	//引入肘板
		{
			CLDSPart *pPart = NULL;
			CString cmdStr = _T("");
			//创建HashTable
			CHashTable<CLDSPart*>partsetTable;
			partsetTable.Empty();
			partsetTable.CreateHashTable(100);
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Select the importing elbow plate,or directly input handle(Seperate: ','), press enter key to confirm:","");
#else 
			pCmdLine->FillCmdLine("选择待引入的肘板,或直接输入句柄(可用','分隔),按回车键确认:","");
#endif
			if (pCmdLine->GetStrFromCmdLine(cmdStr))
			{
				//根据句柄字符串引入构件
				long *id_arr1=NULL,arr1_len=0;;
				if (cmdStr.GetLength()>0)
				{
					id_arr1=new long[cmdStr.GetLength()];
					sText.Printf("%s",cmdStr);
					for (char* key=strtok(sText,",");key;key=strtok(NULL,","))
					{
						long h=HexStrToLong(key);
						pPart=console.FromPartHandle(h);
						id_arr1[arr1_len]=h;
						arr1_len++;
					}
				}
				long *id_arr;
				int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
				for (int i=0;i<arr1_len+n;i++)
				{
					if (i<arr1_len)
						pPart=console.FromPartHandle(id_arr1[i]);
					else
						pPart=console.FromPartHandle(id_arr[i-arr1_len]);
					if (pPart&&pPart->GetClassTypeId()==CLS_PARAMPLATE)
					{
						if (((CLDSParamPlate*)pPart)->m_iParamType==TYPE_ELBOWPLATE)
						{
							if (!partsetTable.GetValueAt(pPart->handle,pPart))
							{
								partsetTable.SetValueAt(pPart->handle,pPart);
								pFirstPlate->AppendAttachPart(pPart);
							}
						}
					}
				}
				if (id_arr1)
					delete[]id_arr1;
				g_pSolidDraw->ReleaseSnapStatus();
			}
			else
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Command:","");
#else 
			pCmdLine->FillCmdLine("命令:","");
#endif
		}
		else if (2==iMenu)	//生成肘板
		{
			static CAddNewElbowPlateDlg dlgElbowPlate;
			dlgElbowPlate.m_pDatumTube=(CLDSLineTube*)console.FromPartHandle(pFirstPlate->m_hPartWeldParent);
			dlgElbowPlate.m_pDatumPlate = pFirstPlate;
			dlgElbowPlate.DoModal();
		}
	}
	return TRUE;
}

long RetrieveBlockHandle(const char* blockIdName);
BOOL ModifyParamPlateProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	CLDSParamPlate *pPlate = NULL;
	char tem_str[MAX_TEMP_CHAR_100+1]="";

	if (pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate* pFirstPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();
	BOOL bUpdateWcs=FALSE;
	CPropListOper<CLDSParamPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&pView->selectObjs);
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if (pItem->m_idProp==CLDSParamPlate::GetPropID("layer"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->SetLayer(tem_str);
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				if (pMirObj->mirInfo.axis_flag&8)
					continue;	//旋转对称
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					CalMirAtomLayer(pPlate->layer(),pMirPlate->layer(),pMirObj->mirInfo);
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("cMaterial"))
	{	//钢板材质
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->cMaterial = QuerySteelBriefMatMark(tem_str);
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					pMirPlate->cMaterial=pPlate->cMaterial;
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_cQualityLevel"))
	{	//钢板质量等级
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if (valueStr.CompareNoCase("Default")==0)
				pPlate->m_cQualityLevel=0;
			else
#else
			if (valueStr.CompareNoCase("默认")==0)
				pPlate->m_cQualityLevel=0;
			else
#endif
				pPlate->m_cQualityLevel = valueStr[0];
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					pMirPlate->m_cQualityLevel=pPlate->m_cQualityLevel;
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_hBlock"))
	{
		long hBlock=RetrieveBlockHandle(valueStr);
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			pPlate->m_hBlock=hBlock;
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("thick"))
	{	//钢板厚度
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->thick=atoi(tem_str);
			if (pPlate->m_iParamType==TYPE_BASEPLATE)
			{
				pPlate->Create3dSolidModel();
				if (pPlate->IsSolidModified())
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
			else if (pPlate->m_iParamType==TYPE_FL)
			{	//修改法兰板厚，需根据钢管端头设计重新计算法兰的位置
				CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
				if (pLineTube)
				{
					if (pLineTube->m_tJointEnd.hLinkObj==pPlate->handle)
						pLineTube->DesignEndJointUCS(pPlate);
					else if (pLineTube->m_tJointStart.hLinkObj==pPlate->handle)
						pLineTube->DesignStartJointUCS(pPlate);
				}
			}
			//pPlate->DesignPlate();		//对于基板(底座板)更改厚度时已集成了DesignPlate调用,不需重再设计钢板 wjh-2016.8.31
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
				{
					pMirPlate->thick=pPlate->thick;
					if (pPlate->m_iParamType==TYPE_BASEPLATE)
						pPlate->Create3dSolidModel();
				}
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("iSeg"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->iSeg = SEGI(valueStr.GetBuffer());
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					pMirPlate->iSeg=pPlate->iSeg;
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("sPartNo"))
	{	//根据件号提取段号，并更新钢板段号
		bool parsed=false;
		SEGI iSeg;
		if (CLDSPart::UPDATE_PARTNO_AND_SEG&&!pCurPlate->m_bStdPart&&ParsePartNo(tem_str,&iSeg,NULL))
		{
			parsed=true;
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("iSeg"),iSeg.ToString());
		}
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			if (parsed)
				pPlate->iSeg=iSeg;
			pPlate->SetPartNo(tem_str);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("cfgword"))
	{	//使用第一块钢板的配材号更新其他钢板的配材号
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->cfgword = pCurPlate->cfgword;
			pPlate->SetModified(TRUE,FALSE);
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
				{
					pPlate->SetModified(TRUE,FALSE);
					pMirPlate->cfgword=pPlate->cfgword;
				}
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_uStatMatCoef"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->m_uStatMatCoef = atoi(tem_str);
			pPlate->SetModified(TRUE,FALSE);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_hPartWeldParent"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->SetModified(TRUE,FALSE);
			pPlate->m_hPartWeldParent=HexStrToLong(tem_str);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_bDisplayHole"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if (valueStr.CompareNoCase("Yes")==0)
				pPlate->m_bDisplayHole = TRUE;
			else if (valueStr.CompareNoCase("No")==0)
#else 
			if (valueStr.CompareNoCase("是")==0)
				pPlate->m_bDisplayHole = TRUE;
			else if (valueStr.CompareNoCase("否")==0)
#endif
				pPlate->m_bDisplayHole = FALSE;
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					pMirPlate->m_bDisplayHole=pPlate->m_bDisplayHole;
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_bVirtualPart"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if (valueStr.CompareNoCase("Yes")==0)
				pPlate->m_bVirtualPart = TRUE;
			else if (valueStr.CompareNoCase("No")==0)
#else 
			if (valueStr.CompareNoCase("是")==0)
				pPlate->m_bVirtualPart = TRUE;
			else if (valueStr.CompareNoCase("否")==0)
#endif
				pPlate->m_bVirtualPart = FALSE;
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_bUserSpecColor"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if (valueStr.CompareNoCase("Yes")==0)
				pPlate->m_bUserSpecColor = TRUE;
			else if (valueStr.CompareNoCase("No")==0)
#else 
			if (valueStr.CompareNoCase("是")==0)
				pPlate->m_bUserSpecColor = TRUE;
			else if (valueStr.CompareNoCase("否")==0)
#endif
				pPlate->m_bUserSpecColor = FALSE;
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					pMirPlate->m_bUserSpecColor=pPlate->m_bUserSpecColor;
			}
		}
		if (pCurPlate->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("crMaterial"),NULL);
			if (pFindItem==NULL)
			{
				CXhChar100 sText;
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Part Color";
				lpInfo->m_strPropHelp = "Part's color";
#else 
				lpInfo->m_strPropName = "构件颜色";
				lpInfo->m_strPropHelp = "构件颜色";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = CLDSParamPlate::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if (pCurPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("crMaterial"));
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str,tem_str+3,97);	//跳过RGB
		sscanf(tem_str,"%X",&crCurSel);
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->crMaterial = crCurSel;
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					pMirPlate->crMaterial=pPlate->crMaterial;
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("sNotes"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			_snprintf(pPlate->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
			//备注不用关联修改 wht 10-10-22
			/*for(RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
			CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
			if(pMirPlate)
			strcpy(pMirPlate->sNotes,pPlate->sNotes);
			}*/
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_cRollProcessType"))
	{
		BYTE cProcessType = valueStr[0]-'0';
		if (cProcessType==3)
			cProcessType=0xFF;
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			pPlate->m_cRollProcessType=cProcessType;
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_cRollOffsetType"))
	{
		BYTE cProcessType = valueStr[0]-'0';
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			pPlate->m_ciRollOffsetType=cProcessType;
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_cDeformType"))
	{
		BYTE cProcessType = valueStr[0]-'0';
		if (cProcessType==4)
			cProcessType=0xFF;
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			pPlate->m_ciDeformType=cProcessType;
		//
		pPropList->DeleteAllSonItems(pItem);
		if (cProcessType==1)
		{
			CItemInfo* lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Deform Coef";
#else 
			lpInfo->m_strPropName = "中性层系数";
#endif
			CXhChar100 sText;
			CPropTreeItem *pPropItem = pPropList->InsertItem(pItem,lpInfo,0,true);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_fDeformCoef");
			if (pCurPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_fDeformCoef"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			pPlate->m_fDeformCoef=atof(valueStr);
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_fNormOffset"))
	{
		double norm_offset=atof(valueStr);
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			pPlate->SetNormOffset(norm_offset);
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.origin.x"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.origin.x = atof(valueStr);
			if (pPlate->design.origin.datum_pos_style==0)
				pPlate->design.origin.datum_pos.x=pPlate->ucs.origin.x;
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.origin.y"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.origin.y = atof(valueStr);
			if (pPlate->design.origin.datum_pos_style==0)
				pPlate->design.origin.datum_pos.y=pPlate->ucs.origin.y;
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.origin.z"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.origin.z = atof(valueStr);
			if (pPlate->design.origin.datum_pos_style==0)
				pPlate->design.origin.datum_pos.z=pPlate->ucs.origin.z;
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_x.x"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_x.x = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_x*1,0);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_x.y"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_x.y = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_x*1,0);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_x.z"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_x.z = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_x*1,0);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_y.x"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_y.x = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_y*1,1);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_y.y"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_y.y = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_y*1,1);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_y.z"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_y.z = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_y*1,1);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_z.x"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_z.x = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_z*1,2);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_z.y"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_z.y = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_z*1,2);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_z.z"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_z.z = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_z*1,2);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.origin.x"))
	{
		bUpdateWcs=TRUE;
		PLATEEDIT_STATE* pEditState=CLDSParamPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		pEditState->m_fLocalWcsOrgX=console.m_curWCS.origin.x=atof(valueStr);
		//
		oper.UpdatePropItemValue("wcs.origin");
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.origin.y"))
	{
		bUpdateWcs=TRUE;
		PLATEEDIT_STATE* pEditState=CLDSParamPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		pEditState->m_fLocalWcsOrgY=console.m_curWCS.origin.y=atof(valueStr);
		//
		oper.UpdatePropItemValue("wcs.origin");
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.axis_x.a"))
	{
		bUpdateWcs=TRUE;
		double alfa=atof(valueStr)*RADTODEG_COEF;	//由角度制转为弧度制
		PLATEEDIT_STATE* pEditState=CLDSParamPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		console.m_curWCS.axis_x=GEPOINT(cos(alfa),sin(alfa));
		console.m_curWCS.axis_y.Set(-console.m_curWCS.axis_x.y,console.m_curWCS.axis_x.x);
		pEditState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
		pEditState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
		pEditState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
		pEditState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
		//
		oper.UpdatePropItemValue("wcs.axis_x");
		oper.UpdatePropItemValue("wcs.axis_y");
	}
	else if (!ModifyParamPlateParamProperty(pPropList,pItem,valueStr,pView->selectObjs))
		return FALSE;
	if (bUpdateWcs==TRUE)
	{
		UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
		g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
	}
	for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
	{
		pPlate->SetModified();
		pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}
CString GetBlockListStr();
void CLDSView::DisplayPlateProperty(BOOL bRebuild)
{
	CLDSPlate *pFirstPlate = (CLDSPlate*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pLeafItem,*pSonLeafItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	CXhChar100 sText;

	CPropListOper<CLDSPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&selectObjs);
	const int GROUP_GENERAL=1,GROUP_POS=2,GROUP_PROFILE=3,GROUP_MODIFY=4,GROUP_OTHER=5;
	if (bRebuild)
	{	//需要重建对象属性页
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(5);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"Position");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"Profile");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_MODIFY-1,"Modify");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_OTHER-1,"Others");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"常规");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"位置");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"外形");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_MODIFY-1,"修改");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_OTHER-1,"其它");
#endif
		pPropDlg->RefreshTabCtrl(CLDSPlate::m_iCurDisplayPropGroup);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_PLATE;
		pPropList->SetModifyValueFunc(ModifyPlateProperty);
		pPropList->SetButtonClickFunc(PlateButtonClick);
		pPropList->SetModifyStatusFunc(ModifyPlateItemStatus);
		pPropList->SetPropHelpPromptFunc(FirePlateHelpStr);
		pPropList->SetPopMenuClickFunc(FirePlatePopMenuClick);
		//初始化属性项只读状态初始值,块内构件或映射构件属性不允许编辑 wht 12-10-16
		if (!pFirstPlate->BelongModel()->IsTowerModel()||pFirstPlate->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//钢板基本信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Plate Basic Information";
		lpInfo->m_strPropHelp = "The basic information of plate";
#else 
		lpInfo->m_strPropName = "钢板基本信息";
		lpInfo->m_strPropHelp = "钢板本信息";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pGroupItem->m_idProp = CLDSPlate::GetPropID("basicInfo");
		if (CLDSPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|
			GetSingleWord(GROUP_PROFILE)|GetSingleWord(GROUP_OTHER);
		//句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "Plate's handle is typical unique identifier string in the  LDS system.";
#else 
		lpInfo->m_strPropName = "句柄";
		lpInfo->m_strPropHelp = "钢板句柄是LDS系统中对钢板的全局唯一标识";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("handle");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|
			GetSingleWord(GROUP_PROFILE)|GetSingleWord(GROUP_OTHER);
		//依赖节点句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Dependant node";
		lpInfo->m_strPropHelp = "Dependant node";
#else 
		lpInfo->m_strPropName = "依赖节点句柄";
		lpInfo->m_strPropHelp = "设计钢板过程中所选择的连接节点";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.hNode");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//依赖杆件句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Dependant rod";
		lpInfo->m_strPropHelp = "Dependant rod";
#else 
		lpInfo->m_strPropName = "依赖杆件句柄";
		lpInfo->m_strPropHelp = "设计钢板过程中所选择的连接定位杆件";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.hRod");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//图层
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "Plate in the layer";
#else 
		lpInfo->m_strPropName = "图层";
		lpInfo->m_strPropHelp = "钢板所在图层";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("layer");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//配材号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Configure Word";
		lpInfo->m_strPropHelp = "Configure word";
#else 
		lpInfo->m_strPropName = "配材号";
		lpInfo->m_strPropHelp = "配材号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("cfgword");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//材质
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Material";
		lpInfo->m_strPropHelp = "The material of plate.";
#else 
		lpInfo->m_strPropName = "材质";
		lpInfo->m_strPropHelp = "钢板材质";
#endif
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("cMaterial");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|
			GetSingleWord(GROUP_PROFILE)|GetSingleWord(GROUP_OTHER);
		//质量等级
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Quality Grade";
		lpInfo->m_strPropHelp = "The quality grade of plate.";
		lpInfo->m_cmbItems = "Default|A|B|C|D|";
#else 
		lpInfo->m_strPropName = "质量等级";
		lpInfo->m_strPropHelp = "钢板质量等级";
		lpInfo->m_cmbItems = "默认|A|B|C|D|";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_cQualityLevel");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|
			GetSingleWord(GROUP_PROFILE)|GetSingleWord(GROUP_OTHER);
		//厚度
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Thickness";
		lpInfo->m_strPropHelp = "The thickness of plate(mm)。";
#else 
		lpInfo->m_strPropName = "厚度";
		lpInfo->m_strPropHelp = "钢板的厚度(mm)。";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("thick");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|
			GetSingleWord(GROUP_PROFILE)|GetSingleWord(GROUP_OTHER);
		//分段号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Segment Number";
		lpInfo->m_strPropHelp = "Segment number";
#else 
		lpInfo->m_strPropName = "分段号";
		lpInfo->m_strPropHelp = "分段号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("iSeg");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//构件编号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part Number";
		lpInfo->m_strPropHelp = "Part number";
#else 
		lpInfo->m_strPropName = "构件编号";
		lpInfo->m_strPropHelp = "构件编号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("sPartNo");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|
			GetSingleWord(GROUP_PROFILE)|GetSingleWord(GROUP_OTHER);
		//
		if (pFirstPlate->IsShadowObject())
		{
			//影射信息
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Information";
			lpInfo->m_strPropHelp = "Project information";
#else 
			lpInfo->m_strPropName = "影射信息";
			lpInfo->m_strPropHelp = "影射信息";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("shadowInfo");
			pPropItem->m_bHideChildren = FALSE;
			if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			pPropItem->SetReadOnly(FALSE);
			//影射母构件
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Mother Part";
			lpInfo->m_strPropHelp = "The current part is project mother part.The current part is original part when the handle is Ox0.";
#else 
			lpInfo->m_strPropName = "影射母构件";
			lpInfo->m_strPropHelp = "当前构件对应的影射母构件句柄，该句柄为0x0时表示当前构件为原始构件。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("m_hMotherObj");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//关联更新编号
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Synchornize part label";
			lpInfo->m_strPropHelp = "Synchornize part label and segment number with mother part.";
			lpInfo->m_cmbItems = "Yes|No";
#else
			lpInfo->m_strPropName = "关联更新编号";
			lpInfo->m_strPropHelp = "当前影射构件与其影射母构件同步更新构件编号、段号信息。";
			lpInfo->m_cmbItems = "是|否";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("m_bSyncShadowPartNo");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstPlate->IsBlockRefShadowObject())
			{
				//归属部件引用
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block Assembly";
				lpInfo->m_strPropHelp = "The reference handle of the vest in block.";
#else 
				lpInfo->m_strPropName = "归属部件引用";
				lpInfo->m_strPropHelp = "当前构件归属的部件引用句柄。";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("m_hBlockRef");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
				pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else
		{
			//关联构件
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Relative Part";
			lpInfo->m_strPropHelp = "The part associated with the current part and symmetrical type";
#else 
			lpInfo->m_strPropName = "关联构件";
			lpInfo->m_strPropHelp = "与当前构件相关联的构件以及对称方式";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("relativeObjs");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//归属部件
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Dependent Block Handle";
			lpInfo->m_strPropHelp = "The handle of the dependent block.";
#else 
			lpInfo->m_strPropName = "归属部件";
			lpInfo->m_strPropHelp = "当前构件归属的部件句柄。";
#endif
			lpInfo->m_cmbItems=GetBlockListStr();
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("m_hBlock");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//焊接父构件
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weld Parent Part";
		lpInfo->m_strPropHelp = "If the part need to weld to other part, then the property is the handle of the part's attached welded part;If the part doesn't need to be welded,the value is 0X0.";
#else 
		lpInfo->m_strPropName = "焊接父构件";
		lpInfo->m_strPropHelp = "如果此构件需要焊接到其它构件上,那么此属性就是本构件所依附的焊接构件的句柄;如果此构件不需要焊接,此值为0X0。";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_hPartWeldParent");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//统材系数
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "BOM Number";
		lpInfo->m_strPropHelp = "BOM number";
#else 
		lpInfo->m_strPropName = "统材系数";
		lpInfo->m_strPropHelp = "统材系数";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_uStatMatCoef");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//显示孔
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Hole Visible";
		lpInfo->m_strPropHelp = "Hole visible";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "显示孔";
		lpInfo->m_strPropHelp = "是否显示孔";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_bDisplayHole");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		/*显示轮廓边
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Profile Visible";
		lpInfo->m_strPropHelp = "Profile visible";
		lpInfo->m_cmbItems = "None|Actual Profile|Spread Profile";
#else
		lpInfo->m_strPropName = "显示轮廓边";
		lpInfo->m_strPropHelp = "实体模式下钢板以是否以轮廓边";
		if(pFirstPlate->face_N==1)
			lpInfo->m_cmbItems = "0.不显示|1.轮廓边";
		else
			lpInfo->m_cmbItems = "0.不显示|1.轮廓边|2.轮廓边(展开)";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_cPickedDisplayMode");
		if(pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);	//GetSingleWord(GROUP_POS)| 为与参数化板统一不显示在<位置>页中 wjh-2017.11.20
		*/
		//显示孔
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual Part";
		lpInfo->m_strPropHelp = "Virtual Part";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "虚拟钢板";
		lpInfo->m_strPropHelp = "";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_bVirtualPart");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//用户指定构件颜色
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specified Part Color";
		lpInfo->m_strPropHelp = "Whether the part's color is specified by the user.";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "指定构件颜色";
		lpInfo->m_strPropHelp = "构件颜色是否为用户指定";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if (pFirstPlate->m_bUserSpecColor)
		{	//构件颜色
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Part Color";
			lpInfo->m_strPropHelp = "Part's color";
#else 
			lpInfo->m_strPropName = "构件颜色";
			lpInfo->m_strPropHelp = "构件颜色";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if (pFirstPlate->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//备注
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Notes";
		lpInfo->m_strPropHelp = "Notes";
#else 
		lpInfo->m_strPropName = "备注";
		lpInfo->m_strPropHelp = "备注";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("sNotes");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//装配方位
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Assemble Position";
		lpInfo->m_strPropHelp = "The assembly location of part and assembly direction.";
#else 
		lpInfo->m_strPropName = "装配方位";
		lpInfo->m_strPropHelp = "构件的装配位置及装配时的装配方向。";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		//pGroupItem->m_bHideChildren = TRUE;
		pGroupItem->m_idProp = CLDSPlate::GetPropID("assemblePos");
		if (CLDSPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//装配基点法向位置
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Normal Offset";
		lpInfo->m_strPropHelp = "The offset distance of plane to the datum node in the normal .";
#else 
		lpInfo->m_strPropName = "基点法向位置";
		lpInfo->m_strPropHelp = "装配基点法向位置。";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_fNormOffset");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//钢板定位参数
		if (pFirstPlate->designInfo.origin.datum_pos_style==10)
		{	//两节点间的比例等分点
			//装配基点法向位置
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Plate Locating Parameter";
			lpInfo->m_strPropHelp = "Plate locating parameter。";
#else 
			lpInfo->m_strPropName = "钢板定位参数";
			lpInfo->m_strPropHelp = "钢板定位参数。";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("designinfo.des_para");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//组合角钢句柄
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Rod Handle";
			lpInfo->m_strPropHelp="The handle of datum rod;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
			lpInfo->m_strPropName="基准杆件句柄";
			lpInfo->m_strPropHelp="基准杆件句柄，基准节点为杆件基准杆件端点时会考虑杆件的正负头信息。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.hLinePart");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//始端基准节点
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Start Node Handle";
			lpInfo->m_strPropHelp="The handle of start node;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
			lpInfo->m_strPropName="始端基准节点句柄";
			lpInfo->m_strPropHelp="始端基准节点句柄，基准节点为杆件基准杆件端点时会考虑杆件的正负头信息。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.hStartNode");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//始端偏移量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Start Offset";
			lpInfo->m_strPropHelp="The offset of start node along the direction of start node to end node.";
#else 
			lpInfo->m_strPropName="始端偏移量";
			lpInfo->m_strPropHelp="始端节点沿着始端节点->终端节点方向的偏移量。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.start_offset_dist");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//终端基准节点
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="End Node Handle";
			lpInfo->m_strPropHelp="The handle of end node;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
			lpInfo->m_strPropName="终端基准节点句柄";
			lpInfo->m_strPropHelp="终端基准节点句柄，基准节点为杆件基准杆件端点时会考虑杆件的正负头信息。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.hEndNode");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//终端偏移量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="End Offset";
			lpInfo->m_strPropHelp="The offset of head node along the direction of end node to start node.";
#else 
			lpInfo->m_strPropName="终端偏移量";
			lpInfo->m_strPropHelp="终端节点沿着终端节点->始端节点方向的偏移量。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.end_offset_dist");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//偏移比例
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Scale";
			lpInfo->m_strPropHelp="Calculate data from start node's offset position.";
#else 
			lpInfo->m_strPropName="偏移比例";
			lpInfo->m_strPropHelp="从始端节点考虑始端偏移量以后的位置开始计算。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_scale");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//偏移距离
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Distance";
			lpInfo->m_strPropHelp="Calculated on offset ratio first and then get distance after offset.";
#else 
			lpInfo->m_strPropName="偏移距离";
			lpInfo->m_strPropHelp="按偏移比例计算以后再进行偏移的距离。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_dist");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		}
		//装配坐标系
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Assemble CS";
		lpInfo->m_strPropHelp = "Assemble CS";
#else 
		lpInfo->m_strPropName = "装配坐标系";
		lpInfo->m_strPropHelp = "装配坐标系。";
#endif
		CPropTreeItem *pAcsItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pAcsItem->m_idProp = CLDSPlate::GetPropID("ucs");
		if (CLDSPlate::GetPropStatus(pAcsItem->m_idProp)!=-1)
			pAcsItem->m_bHideChildren = CLDSPlate::GetPropStatus(pAcsItem->m_idProp);
		pAcsItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//装配位置
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Position";
		lpInfo->m_strPropHelp = "The position of assembling datum point.";
#else 
		lpInfo->m_strPropName = "位置";
		lpInfo->m_strPropHelp = "装配基点的位置。";
#endif
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("ucs.origin");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		/*
		//装配基点法向位置
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "基点法向位置";
		lpInfo->m_strPropHelp = "装配基点法向位置。";
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("m_fNormOffset");
		if(pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		*/
		//装配坐标系原点位置X坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS.";
#else 
		lpInfo->m_strPropName = "X坐标";
		lpInfo->m_strPropHelp = "装配坐标系原点位置X坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.origin.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//装配坐标系原点位置Y坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS.";
#else 
		lpInfo->m_strPropName = "Y坐标";
		lpInfo->m_strPropHelp = "装配坐标系原点位置Y坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.origin.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//装配坐标系原点位置Z坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS";
#else 
		lpInfo->m_strPropName = "Z坐标";
		lpInfo->m_strPropHelp = "装配坐标系原点位置Z坐标分量。";
#endif
		pLeafItem= pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.origin.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//装配时X轴方向
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Axis Direction";
		lpInfo->m_strPropHelp = "The assembling X axis direction";
#else 
		lpInfo->m_strPropName = "X轴方向";
		lpInfo->m_strPropHelp = "装配时X轴方向";
#endif
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_x");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系X轴X坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X ";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS's X axis.";
#else 
		lpInfo->m_strPropName = "X坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系X轴X坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_x.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系X轴Y坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y ";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS's X axis.";
#else 
		lpInfo->m_strPropName = "Y坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系X轴Y坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_x.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系X轴Z坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z ";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS's X axis.";
#else 
		lpInfo->m_strPropName = "Z坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系X轴Z坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_x.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配时Y轴方向
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Axis Direction";
		lpInfo->m_strPropHelp = "The assembling Y axis direction";
#else 
		lpInfo->m_strPropName = "Y轴方向";
		lpInfo->m_strPropHelp = "装配时Y轴方向";
#endif
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_y");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系Y轴X坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X ";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS's Y axis.";
#else 
		lpInfo->m_strPropName = "X坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系Y轴X坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_y.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系Y轴Y坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y ";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS's Y axis.";
#else 
		lpInfo->m_strPropName = "Y坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系Y轴Y坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_y.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系Y轴Z坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z ";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS's Y axis.";
#else 
		lpInfo->m_strPropName = "Z坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系Y轴Z坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_y.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配时Z轴方向
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Axis Direction";
		lpInfo->m_strPropHelp = "The assembling Z axis direction";
#else 
		lpInfo->m_strPropName = "Z轴方向";
		lpInfo->m_strPropHelp = "装配时Z轴方向";
#endif
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_z");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//装配坐标系Z轴X坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X ";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS's Z axis.";
#else 
		lpInfo->m_strPropName = "X坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系Z轴X坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_z.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//装配坐标系Z轴Y坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y ";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS's Z axis.";
#else 
		lpInfo->m_strPropName = "Y坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系Z轴Y坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_z.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//装配坐标系Z轴Z坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z ";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS's Z axis.";
#else 
		lpInfo->m_strPropName = "Z坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系Z轴Z坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_z.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//钢板工作面
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Work Plane";
		lpInfo->m_strPropHelp = "The datum point of plate's work-plane  is  origin point of assembly CS,the Z axis diretion of plate is the direction of work normal.";
#else 
		lpInfo->m_strPropName = "钢板工作面";
		lpInfo->m_strPropHelp = "钢板工作面基准点为装配坐标系原点，工作面法线为钢板Z轴方向。";
#endif
		pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("plate_work_plane");
		pPropItem->m_bHideChildren = TRUE;
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_POS);
		//钢板外形
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
		lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Plate Profile";
		lpInfo->m_strPropHelp = "The plate profile information";
		lpInfo->m_cmbItems="Splay Fold Plate|Curved Beam Fold Plate|Bolt Pad|Joint Plate|Rib Plate|Connect Plate";
#else 
		lpInfo->m_strPropName = "钢板外形";
		lpInfo->m_strPropHelp = "钢板外形信息";
#ifdef __STATION_
		lpInfo->m_cmbItems="构架顶梁八字板|构架曲梁折叠板|螺栓垫板|接头包钢板|加强肋板|普通连接板";
#else
		lpInfo->m_cmbItems="螺栓垫板|接头包钢板|加强肋板|普通连接板";
#endif
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pGroupItem->m_idProp = CLDSPlate::GetPropID("plateProfile");
		if (pFirstPlate->GetPropValueStr(pGroupItem->m_idProp,sText)>0)
			pGroupItem->m_lpNodeInfo->m_strPropValue = sText;
		if (CLDSPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		//pGroupItem->SetReadOnly();
		//基点内环半径
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Inner Circle Radius";
		lpInfo->m_strPropHelp = "If the plate exists inner circle in basis points,the property will be the radius of inner circle,otherwise should be set to 0.";
#else 
		lpInfo->m_strPropName = "基点内环半径";
		lpInfo->m_strPropHelp = "如果板在基点处存在内环,则为内环的半径，否则应设为0。";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_fInnerRadius");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_bHideChildren=TRUE;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		if (pFirstPlate->m_fInnerRadius>0)
		{
			//内环轴线方向
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Inner Circle Normal";
			lpInfo->m_strPropHelp = "The normal of inner circle.";
#else 
			lpInfo->m_strPropName = "内环轴线方向";
			lpInfo->m_strPropHelp = "内环轴线方向。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerColumnNorm");
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//内环轴向X分量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X ";
			lpInfo->m_strPropHelp = "The X coordinate component of inner circle's normal.";
#else 
			lpInfo->m_strPropName = "内环轴向X分量";
			lpInfo->m_strPropHelp = "内环轴线方向的X分量值。";
#endif
			pSonLeafItem = pPropList->InsertItem(pLeafItem,lpInfo,-1);
			pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerColumnNorm.x");
			if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
				pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonLeafItem->m_idProp);
			pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//内环轴向Y分量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y ";
			lpInfo->m_strPropHelp = "The Y coordinate component of inner circle's normal.";
#else 
			lpInfo->m_strPropName = "内环轴向Y分量";
			lpInfo->m_strPropHelp = "内环轴线方向的Y分量值。";
#endif
			pSonLeafItem = pPropList->InsertItem(pLeafItem,lpInfo,-1);
			pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerColumnNorm.y");
			if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
				pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonLeafItem->m_idProp);
			pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//内环轴向Z分量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z ";
			lpInfo->m_strPropHelp = "The Z coordinate component of inner circle's normal.";
#else 
			lpInfo->m_strPropName = "内环轴向Z分量";
			lpInfo->m_strPropHelp = "内环轴线方向的Z分量值。";
#endif
			pSonLeafItem = pPropList->InsertItem(pLeafItem,lpInfo,-1);
			pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerColumnNorm.z");
			if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
				pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonLeafItem->m_idProp);
			pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//内环位置
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Inner Circle Center Position";
			lpInfo->m_strPropHelp = "The inner circle position is the relative coordinates of plate coordinates.";
#else 
			lpInfo->m_strPropName = "内环位置";
			lpInfo->m_strPropHelp = "内环位置,该坐标为钢板坐标系下的相对坐标。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerOrigin");
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//内环位置X分量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Inner Circle X Coordinate";
			lpInfo->m_strPropHelp = "The X coordinate component of inner circle's center.";
#else 
			lpInfo->m_strPropName = "内环位置X分量";
			lpInfo->m_strPropHelp = "内环位置的X分量值。";
#endif
			pSonLeafItem = pPropList->InsertItem(pLeafItem,lpInfo,-1);
			pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerOrigin.x");
			if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
				pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonLeafItem->m_idProp);
			pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//内环位置Y分量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Inner Circle Y Coordinate";
			lpInfo->m_strPropHelp = "The Y coordinate component of inner circle's center.";
#else 
			lpInfo->m_strPropName = "内环位置Y分量";
			lpInfo->m_strPropHelp = "内环位置的Y分量值。";
#endif
			pSonLeafItem = pPropList->InsertItem(pLeafItem,lpInfo,-1);
			pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerOrigin.y");
			if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
				pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonLeafItem->m_idProp);
			pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//内环位置Z分量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Inner Circle Z Coordinate";
			lpInfo->m_strPropHelp = "The Z coordinate component of inner circle's center.";
#else 
			lpInfo->m_strPropName = "内环位置Z分量";
			lpInfo->m_strPropHelp = "内环位置的Z分量值。";
#endif
			pSonLeafItem = pPropList->InsertItem(pLeafItem,lpInfo,-1);
			pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerOrigin.z");
			if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
				pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonLeafItem->m_idProp);
			pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		}
		//是否进行柔性参数化设计
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "IF Flexible Design";
		lpInfo->m_strPropHelp = "Whethe to enable the flexible parametric design";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "参预柔性设计";
		lpInfo->m_strPropHelp = "是否进行柔性参数化设计";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.m_bEnableFlexibleDesign");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		//钢板外形生成方式
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems = "0.Standard Profile|1.Customized Profile|2.Envelope Profile|3.Just Update Coordinate";
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "Create Profile Style";
		lpInfo->m_strPropHelp = "The creating style of profile";
#else 
		lpInfo->m_cmbItems = "0.标准外形|1.定制外形|2.包络外形|3.仅更新坐标";
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "外形生成方式";
		lpInfo->m_strPropHelp = "外形生成方式";
#endif
		lpInfo->m_controlType = PIT_COMBO;
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("iProfileStyle0123");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		//钢板火曲类型
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Plate Type";
		lpInfo->m_strPropHelp = "plate type";
		lpInfo->m_cmbItems = "0. Planar Plate|1. 2 Planar Plate|2. 3 Planar Plate|3. Folded Plate|4. Bolt Gasket";
#else 
		lpInfo->m_strPropName = "钢板类型";
		lpInfo->m_strPropHelp = "钢板类型";
		lpInfo->m_cmbItems = "0.单面板|1.双面板|2.三面板|3.折叠板|4.螺栓垫板";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("face_N");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		Update_Plate_Face_N_TreeItem(pPropList,pGroupItem);
		//轮廓顶点
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Profile vertex";
		lpInfo->m_strPropName = "Profile vertex";
		lpInfo->m_strPropHelp = "Profile vertex";
#else 
		lpInfo->m_cmbItems="轮廓顶点";
		lpInfo->m_strPropName = "轮廓顶点";
		lpInfo->m_strPropHelp = "轮廓顶点";
#endif
		pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("plateParam");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		//钢板连接杆件
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMON;//BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="View|Import";
		lpInfo->m_strPropName = "Link Rod";
		lpInfo->m_strPropHelp = "Link rod";
#else 
		lpInfo->m_cmbItems="查看|引入";
		lpInfo->m_strPropName = "连接杆件";
		lpInfo->m_strPropHelp = "连接杆件";
#endif
		pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("plateLinkPoles");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		//钢板卷边处理方式
		PROFILE_VERTEX* pVertex=NULL;
		for (pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
		{
			if (pVertex->m_bRollEdge)
				break;
		}
		if (pVertex)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rolling Style of Plate Edge";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Mechanical Priority|1.Processing Priority|2.Default ";
#else
			lpInfo->m_strPropName = "钢板卷边处理方式";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.最佳受力|1.最少刀数|2.默认值";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("m_cRollProcessType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//钢板卷边线计算方式
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "RollLine Cal Type";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Outside|1.Inside";
#else
			lpInfo->m_strPropName = "卷边偏移方式";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.外侧|1.内侧|2.默认值";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("m_cRollOffsetType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		}
		if (pFirstPlate->GetFaceN()>1)
		{	//钢板火曲变形处理方式
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Deform Type";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Assembly Plane|1.Middle Plane|2.Raised Plane|3.Sunk Plane";
#else
			lpInfo->m_strPropName = "火曲变形处理方式";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.装配面|1.中性层|2.凸面|3.凹面|4.默认值";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("m_cDeformType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			if (sText[0]-'0'==1)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Deform Coef";
				lpInfo->m_strPropHelp = "";
#else
				lpInfo->m_strPropName = "中性层系数";
				lpInfo->m_strPropHelp = "进行火曲变形的中性层系数";
#endif
				pSonLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_fDeformCoef");
				if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
					pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
				if (pFirstPlate->GetFaceN()==3)	//三面板
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Deform Coef2";
					lpInfo->m_strPropHelp = "";
#else
					lpInfo->m_strPropName = "中性层系数2";
					lpInfo->m_strPropHelp = "进行火曲变形的第二条火曲线的中性层系数";
#endif
					pSonLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_fDeformCoef2");
					if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
						pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
				}
			}
		}
		//加工工艺
		pGroupItem=oper.InsertPropItem(pRoot,"manuProcess");
		if (CLDSPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_OTHER);
		//焊接工艺
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bWeldPart");
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_OTHER);
		if (pFirstPlate->m_bWeldPart)
		{	//用户指定焊缝线
			oper.InsertCmbListPropItem(pPropItem,"iWeldLineStyle");
			if (pFirstPlate->designInfo.iWeldLineStyle==1)
			{
				//焊缝起始高
				oper.InsertEditPropItem(pPropItem,"designInfo.weld_start_high");
				//焊缝终止高
				oper.InsertEditPropItem(pPropItem,"designInfo.weld_end_high");
				//焊区宽度
				oper.InsertEditPropItem(pPropItem,"designInfo.weld_width");
			}
		}
		//坡口工艺
		oper.InsertCmbListPropItem(pGroupItem,"m_bNeedFillet");
		if (selectObjs.GetNodeNum()==1&&pFirstPlate->face_N==1&&g_sysPara.display.bPartSketch)
		{	//暂时仅支持显示单面板轮廓点列表 wht 10-09-02
			pGroupItem=oper.InsertPropItem(pRoot,"VertexList");
			if (CLDSPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
				pGroupItem->m_bHideChildren = CLDSPlate::GetPropStatus(pGroupItem->m_idProp);
			pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_OTHER);
			int i=0;
			for (PROFILE_VERTEX *pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
			{
				CXhChar16 sPropId("%c",'A'+i);
				CXhChar16 sPosX("%s.x",(char*)sPropId);
				CXhChar16 sPosY("%s.y",(char*)sPropId);
				//顶点
				pPropItem=oper.InsertButtonPropItem(pGroupItem,sPropId);
				pPropItem->m_bHideChildren = FALSE;
				if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=GetSingleWord(GROUP_OTHER);
				//装配坐标系Y轴X坐标分量
				oper.InsertEditPropItem(pPropItem,sPosX);
				//装配坐标系Y轴Y坐标分量
				oper.InsertEditPropItem(pPropItem,sPosY);
				i++;
			}
		}
		//钢板编辑操作
		//oper.SetPreferPropGroup(GetSingleWord(GROUP_MODIFY));
		pGroupItem=oper.InsertPropItem(pRoot,"PlateEdit");
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_MODIFY);
		//工作坐标系
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"WorkUcs");
		pPropItem->m_bHideChildren=FALSE;
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		//坐标系原点
		pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"wcs.origin");
		pLeafItem->SetReadOnly();
		oper.InsertEditPropItem(pLeafItem,"wcs.origin.x");
		oper.InsertEditPropItem(pLeafItem,"wcs.origin.y");
		//坐标系X轴
		pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"wcs.axis_x");
		pLeafItem->SetReadOnly();
		oper.InsertEditPropItem(pLeafItem,"wcs.axis_x.a");
		//坐标系Y轴
		pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"wcs.axis_y");
		pLeafItem->SetReadOnly();
		//编辑项(插入顶点、插入螺栓、倒角)
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"InsertProfileVertex");		//
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"AddNewHole");	//
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"DeleteFeature");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"FilletVertex");
		pPropItem->SetReadOnly();
		//oper.SetPreferPropGroup(0);
	}
	else
		DisplayPlateNeedlessRebuildProperty();
	pPropList->Redraw();
}

void CLDSView::DisplayPlateNeedlessRebuildProperty()
{
	CLDSPlate *pFirstPlate;
	pFirstPlate = (CLDSPlate*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pLeafItem,*pRoot=pPropList->GetRootItem();
	const int GROUP_GENERAL=1,GROUP_POS=2,GROUP_PROFILE=3,GROUP_OTHER=4;
	CItemInfo* lpInfo;
	CXhChar100 sText;
	CPropTreeItem *pFindItem,*pParentItem;
	SetPartMutiObjsProp(pFirstPlate,"handle");
	SetPartMutiObjsProp(pFirstPlate,"layer");
	SetPartMutiObjsProp(pFirstPlate,"cMaterial");
	SetPartMutiObjsProp(pFirstPlate,"m_cQualityLevel");
	SetPartMutiObjsProp(pFirstPlate,"thick");
	SetPartMutiObjsProp(pFirstPlate,"iSeg");
	SetPartMutiObjsProp(pFirstPlate,"sPartNo");
	SetPartMutiObjsProp(pFirstPlate,"cfgword");
	SetPartMutiObjsProp(pFirstPlate,"m_bDisplayHole");
	SetPartMutiObjsProp(pFirstPlate,"m_cPickedDisplayMode");
	SetPartMutiObjsProp(pFirstPlate,"m_bVirtualPart");
	SetPartMutiObjsProp(pFirstPlate,"m_fInnerRadius");

	SetPartMutiObjsProp(pFirstPlate,"wcs.origin");
	SetPartMutiObjsProp(pFirstPlate,"wcs.origin.x");
	SetPartMutiObjsProp(pFirstPlate,"wcs.origin.y");
	SetPartMutiObjsProp(pFirstPlate,"wcs.axis_x");
	SetPartMutiObjsProp(pFirstPlate,"wcs.axis_x.a");
	SetPartMutiObjsProp(pFirstPlate,"wcs.axis_y");

	pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_hBlock"),&pParentItem);
	if (pFindItem)
	{
		if (pFirstPlate->IsShadowObject())
		{	//从普通构件切换到影射构件
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_hBlock"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("relativeObjs"));
			//影射信息
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Information";
			lpInfo->m_strPropHelp = "Project information";
#else 
			lpInfo->m_strPropName = "影射信息";
			lpInfo->m_strPropHelp = "影射信息";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,12,true);
			pPropItem->m_idProp = CLDSPlate::GetPropID("shadowInfo");
			if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
			pPropItem->SetReadOnly(FALSE);
			//影射母构件
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Mother Part";
			lpInfo->m_strPropHelp = "The current part is project mother part.The current part is original part when the handle is Ox0.";
#else 
			lpInfo->m_strPropName = "影射母构件";
			lpInfo->m_strPropHelp = "当前构件对应的影射母构件句柄，该句柄为0x0时表示当前构件为原始构件。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("m_hMotherObj");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			//关联更新编号
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Synchornize part label";
			lpInfo->m_strPropHelp = "Synchornize part label and segment number with mother part.";
			lpInfo->m_cmbItems = "Yes|No";
#else
			lpInfo->m_strPropName = "关联更新编号";
			lpInfo->m_strPropHelp = "当前影射构件与其影射母构件同步更新构件编号、段号信息。";
			lpInfo->m_cmbItems = "是|否";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("m_bSyncShadowPartNo");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstPlate->IsBlockRefShadowObject())
			{
				//归属部件引用
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block Assembly";
				lpInfo->m_strPropHelp = "The reference handle of the vest in block.";
#else 
				lpInfo->m_strPropName = "归属部件引用";
				lpInfo->m_strPropHelp = "当前构件归属的部件引用句柄。";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("m_hBlockRef");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			}
		}
		else
		{
			SetPartMutiObjsProp(pFirstPlate,"m_hBlock");
			SetPartMutiObjsProp(pFirstPlate,"relativeObjs");
		}
	}
	pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("shadowInfo"),&pParentItem);
	if (pFindItem)
	{
		if (!pFirstPlate->IsShadowObject())
		{	//从影射构件切换到普通构件
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("shadowInfo"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_hMotherObj"));
			pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_hBlockRef"),NULL);
			if (pFindItem)
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_hBlockRef"));
			pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_hBlock"),NULL);
			if (pFindItem!=NULL)
				SetPartMutiObjsProp(pFirstPlate,"m_hBlock");
			else
			{	//归属部件
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block Handle";
				lpInfo->m_strPropHelp = "The handle of the vest in block。";
#else 
				lpInfo->m_strPropName = "归属部件";
				lpInfo->m_strPropHelp = "当前构件归属的部件句柄。";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
				pPropItem->m_idProp = CLDSPlate::GetPropID("m_hBlock");
				if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
				pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			}
			pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("relativeObjs"),NULL);
			if (pFindItem!=NULL)
				SetPartMutiObjsProp(pFirstPlate,"relativeObjs");
			else
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Relative Part";
				lpInfo->m_strPropHelp = "The part associated with the current part and symmetrical type";
#else 
				lpInfo->m_strPropName = "关联构件";
				lpInfo->m_strPropHelp = "与当前构件相关联的构件以及对称方式";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,12,true);
				pPropItem->m_idProp = CLDSPlate::GetPropID("relativeObjs");
				if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
		{
			SetPartMutiObjsProp(pFirstPlate,"m_hMotherObj");
			pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_hBlockRef"),&pParentItem);
			if (pFindItem&&pFirstPlate->IsBlockRefShadowObject())
			{
				SetPartMutiObjsProp(pFirstPlate,"m_hBlockRef");
				SetPartMutiObjsProp(pFirstPlate,"m_hBlock");
			}
			else if (pFindItem&&!pFirstPlate->IsBlockRefShadowObject())
			{
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_hBlockRef"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_hBlock"));
			}
			else if (pFindItem==NULL&&pFirstPlate->IsBlockRefShadowObject())
			{
				pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_hMotherObj"),&pParentItem);
				//归属部件引用
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block Assembly";
				lpInfo->m_strPropHelp = "The reference handle of the vest in block.";
#else 
				lpInfo->m_strPropName = "归属部件引用";
				lpInfo->m_strPropHelp = "当前构件归属的部件引用句柄。";
#endif
				pLeafItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("m_hBlockRef");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			}
		}
	}
	SetPartMutiObjsProp(pFirstPlate,"designInfo.hNode");
	SetPartMutiObjsProp(pFirstPlate,"designInfo.hRod");
	SetPartMutiObjsProp(pFirstPlate,"plateProfile");
	if (pFirstPlate->m_fInnerRadius>0)
	{
		pParentItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_fInnerRadius"),&pFindItem);
		if (pParentItem)
		{
			pPropItem=NULL;
			long idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm");
			pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if (pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Inner Circle Normal";
				lpInfo->m_strPropHelp = "The normal of inner circle.";
#else 
				lpInfo->m_strPropName = "内环轴线方向";
				lpInfo->m_strPropHelp = "内环轴线方向。";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
				pPropItem->m_idProp = idProp;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
			else
				SetPartMutiObjsProp(pFirstPlate,"m_tInnerColumnNorm");
			if (pPropItem)
			{
				idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm.x");
				pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "X ";
					lpInfo->m_strPropHelp = "The X coordinate component of inner circle's normal.";
#else 
					lpInfo->m_strPropName = "内环轴向X分量";
					lpInfo->m_strPropHelp = "内环轴线方向的X分量值。";
#endif
					pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pFindItem->m_idProp = idProp;
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				}
				else
					SetPartMutiObjsProp(pFirstPlate,"m_tInnerColumnNorm.x");
				idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm.y");
				pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Y ";
					lpInfo->m_strPropHelp = "The Y coordinate component of inner circle's normal.";
#else 
					lpInfo->m_strPropName = "内环轴向Y分量";
					lpInfo->m_strPropHelp = "内环轴线方向的Y分量值。";
#endif
					pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pFindItem->m_idProp = idProp;
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				}
				else
					SetPartMutiObjsProp(pFirstPlate,"m_tInnerColumnNorm.y");
				idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm.z");
				pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Z ";
					lpInfo->m_strPropHelp = "The Z coordinate component of inner circle's normal.";
#else 
					lpInfo->m_strPropName = "内环轴向Z分量";
					lpInfo->m_strPropHelp = "内环轴线方向的Z分量值。";
#endif
					pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pFindItem->m_idProp = idProp;
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				}
				else
					SetPartMutiObjsProp(pFirstPlate,"m_tInnerColumnNorm.z");
			}
			//内环位置
			pPropItem=NULL;
			idProp=CLDSPlate::GetPropID("m_tInnerOrigin");
			pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if (pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Inner Circle Position";
				lpInfo->m_strPropHelp = "The inner circle position is the relative coordinates of plate CS.";
#else 
				lpInfo->m_strPropName = "内环位置";
				lpInfo->m_strPropHelp = "内环位置,该坐标为钢板坐标系下的相对坐标。";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
				pPropItem->m_idProp = idProp;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
			else
				SetPartMutiObjsProp(pFirstPlate,"m_tInnerOrigin");
			if (pPropItem)
			{
				idProp=CLDSPlate::GetPropID("m_tInnerOrigin.x");
				pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Inner Center X Coordinate";
					lpInfo->m_strPropHelp = "The X coordinate component of inner circle's center.";
#else 
					lpInfo->m_strPropName = "内环位置X分量";
					lpInfo->m_strPropHelp = "内环位置的X分量值。";
#endif
					pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pFindItem->m_idProp = idProp;
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				}
				else
					SetPartMutiObjsProp(pFirstPlate,"m_tInnerOrigin.x");
				idProp=CLDSPlate::GetPropID("m_tInnerOrigin.y");
				pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Inner Center Y Coordinate";
					lpInfo->m_strPropHelp = "The Y coordinate component of inner circle's center.";
#else 
					lpInfo->m_strPropName = "内环位置Y分量";
					lpInfo->m_strPropHelp = "内环位置的Y分量值。";
#endif
					pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pFindItem->m_idProp = idProp;
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				}
				else
					SetPartMutiObjsProp(pFirstPlate,"m_tInnerOrigin.y");
				idProp=CLDSPlate::GetPropID("m_tInnerOrigin.z");
				pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Inner Center Z Coordinate";
					lpInfo->m_strPropHelp = "The Z coordinate component of inner circle's center.";
#else 
					lpInfo->m_strPropName = "内环位置Z分量";
					lpInfo->m_strPropHelp = "内环位置的Z分量值。";
#endif
					pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pFindItem->m_idProp = idProp;
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				}
				else
					SetPartMutiObjsProp(pFirstPlate,"m_tInnerOrigin.z");
			}
		}
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.x"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.y"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.z"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.x"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.y"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.z"));
	}
	SetPartMutiObjsProp(pFirstPlate,"m_uStatMatCoef");
	SetPartMutiObjsProp(pFirstPlate,"m_hPartWeldParent");
	SetPartMutiObjsProp(pFirstPlate,"m_bUserSpecColor");
	if (pFirstPlate->m_bUserSpecColor)
	{
		if (!SetPartMutiObjsProp(pFirstPlate,"crMaterial"))
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_bUserSpecColor"),NULL);
			if (pFindItem)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
				lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Part Color";
				lpInfo->m_strPropHelp = "Part's color";
#else 
				lpInfo->m_strPropName = "构件颜色";
				lpInfo->m_strPropHelp = "构件颜色";
#endif
				pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1);
				pPropItem->m_idProp = CLDSPlate::GetPropID("crMaterial");
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
				if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			}
		}
	}
	else
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("crMaterial"));
	SetPartMutiObjsProp(pFirstPlate,"sNotes");
	SetPartMutiObjsProp(pFirstPlate,"m_fNormOffset");
	pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designinfo.des_para"));
	if (pFirstPlate->designInfo.origin.datum_pos_style==10)
	{	//两节点间的比例等分点
		CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("assemblePos"),NULL);
		if (pFindItem)
		{
			//装配基点法向位置
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Plate Locating Parameter";
			lpInfo->m_strPropHelp = "Plate locating parameter。";
#else 
			lpInfo->m_strPropName = "钢板定位参数";
			lpInfo->m_strPropHelp = "钢板定位参数。";
#endif
			pPropItem = pPropList->InsertItem(pFindItem,lpInfo,1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("designinfo.des_para");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//组合角钢句柄
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Rod Handle";
			lpInfo->m_strPropHelp="The handle of datum rod;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
			lpInfo->m_strPropName="基准杆件句柄";
			lpInfo->m_strPropHelp="基准杆件句柄，基准节点为杆件基准杆件端点时会考虑杆件的正负头信息。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.hLinePart");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//始端基准节点
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Start Node Handle";
			lpInfo->m_strPropHelp="The handle of start node;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
			lpInfo->m_strPropName="始端基准节点句柄";
			lpInfo->m_strPropHelp="始端基准节点句柄，基准节点为杆件基准杆件端点时会考虑杆件的正负头信息。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.hStartNode");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//始端偏移量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Start Offset";
			lpInfo->m_strPropHelp="The offset of head node along the direction of start node to end node.";
#else 
			lpInfo->m_strPropName="始端偏移量";
			lpInfo->m_strPropHelp="始端节点沿着始端节点->终端节点方向的偏移量。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.start_offset_dist");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//终端基准节点
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="End Node Handle";
			lpInfo->m_strPropHelp="The handle of end node;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
			lpInfo->m_strPropName="终端基准节点句柄";
			lpInfo->m_strPropHelp="终端基准节点句柄，基准节点为杆件基准杆件端点时会考虑杆件的正负头信息。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.hEndNode");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//终端偏移量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="End Offset";
			lpInfo->m_strPropHelp="The offset of head node along the direction of end node to start node.";
#else 
			lpInfo->m_strPropName="终端偏移量";
			lpInfo->m_strPropHelp="终端节点沿着终端节点->始端节点方向的偏移量。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.end_offset_dist");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//偏移比例
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Scale";
			lpInfo->m_strPropHelp="Calculate data from start node's offset position.";
#else 
			lpInfo->m_strPropName="偏移比例";
			lpInfo->m_strPropHelp="从始端节点考虑始端偏移量以后的位置开始计算。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_scale");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//偏移距离
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Distance";
			lpInfo->m_strPropHelp="Calculated on offset ratio first and then get distance after offset.";
#else 
			lpInfo->m_strPropName="偏移距离";
			lpInfo->m_strPropHelp="按偏移比例计算以后再进行偏移的距离。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_dist");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
		}
	}
	SetPartMutiObjsProp(pFirstPlate,"ucs.origin.x");
	SetPartMutiObjsProp(pFirstPlate,"ucs.origin.y");
	SetPartMutiObjsProp(pFirstPlate,"ucs.origin.z");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_x.x");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_x.y");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_x.z");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_y.x");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_y.y");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_y.z");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_z.x");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_z.y");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_z.z");
	SetPartMutiObjsProp(pFirstPlate,"iProfileStyle0123");
	SetPartMutiObjsProp(pFirstPlate,"face_N");
	pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("plateProfile"),&pParentItem);
	if (pFindItem)
		Update_Plate_Face_N_TreeItem(pPropList,pFindItem);
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].norm_style");
	pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0]"),&pParentItem);
	if (pFindItem)
		Update_Plate_Norm_CalStyle_TreeItem(pPropList,pFindItem,TRUE);
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].vector.x");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].vector.y");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].vector.z");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].hVicePart");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].norm_wing");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].direction");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].hCrossPart");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].norm_style");
	pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1]"),&pParentItem);
	if (pFindItem)
		Update_Plate_Norm_CalStyle_TreeItem(pPropList,pFindItem,FALSE);
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].vector.x");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].vector.y");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].vector.z");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].hVicePart");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].norm_wing");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].direction");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].hCrossPart");
	SetPartMutiObjsProp(pFirstPlate,"iWeldLineStyle");
	SetPartMutiObjsProp(pFirstPlate,"m_bNeedFillet");
	SetPartMutiObjsProp(pFirstPlate,"designInfo.m_bEnableFlexibleDesign");
	SetPartMutiObjsProp(pFirstPlate,"m_bWeldPart");
	SetPartMutiObjsProp(pFirstPlate,"designInfo.weld_start_high");
	SetPartMutiObjsProp(pFirstPlate,"designInfo.weld_end_high");
	SetPartMutiObjsProp(pFirstPlate,"designInfo.weld_width");
	//在同类构件间切换时，某些属性从是->否 否->是 不能动态添加删除属性项
	pPropItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_bWeldPart"),&pParentItem);
	if (pPropItem)
	{
		if (pFirstPlate->m_bWeldPart)
		{
			pLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("iWeldLineStyle"),&pParentItem);
			if (pLeafItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Weld Line Type";
				lpInfo->m_strPropHelp = "Weld line size calculation type";
				lpInfo->m_cmbItems = "0.Calculate freely|1.Specified Weld Line|2.Extention Aligned";
#else 
				lpInfo->m_strPropName = "焊缝线类型";
				lpInfo->m_strPropHelp = "焊缝线尺寸计算类型";
				lpInfo->m_cmbItems = "0.自由计算|1.指定焊缝|2.延伸对齐";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("iWeldLineStyle");
				pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_OTHER);
			}
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		else
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("iWeldLineStyle"));
		if (!pFirstPlate->m_bWeldPart||pFirstPlate->designInfo.iWeldLineStyle!=1)
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"));
		}
		else
		{	//指定焊缝线位置焊缝起始高
			pLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"),&pParentItem);
			if (pLeafItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Weld Line Start Height";
				lpInfo->m_strPropHelp = "The start height of weld line.";
#else 
				lpInfo->m_strPropName = "焊缝起始高";
				lpInfo->m_strPropHelp = "焊缝起始高";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_start_high");
				pLeafItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
			}
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			//焊缝起始高
			pLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"),&pParentItem);
			if (pLeafItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Weld Line Start Height";
				lpInfo->m_strPropHelp = "The start height of weld line.";
#else 
				lpInfo->m_strPropName = "焊缝起始高";
				lpInfo->m_strPropHelp = "焊缝起始高";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_start_high");
				pLeafItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
			}
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			//焊缝终止高
			pLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"),&pParentItem);
			if (pLeafItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Weld Line End Height";
				lpInfo->m_strPropHelp = "The end height of weld line.";
#else 
				lpInfo->m_strPropName = "焊缝终止高";
				lpInfo->m_strPropHelp = "焊缝终止高";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_end_high");
				pLeafItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
			}
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			//焊区宽度
			pLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"),&pParentItem);
			if (pLeafItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Weld Width";
				lpInfo->m_strPropHelp = "Weld width";
#else 
				lpInfo->m_strPropName = "焊区宽度";
				lpInfo->m_strPropHelp = "焊区宽度";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_width");
				pLeafItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
			}
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
	//
	//删除顶点列表
	pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("VertexList"));
	int i=0;
	for (i=0;i<26;i++)
	{
		CXhChar16 sPropId("%c",'A'+i);
		CXhChar16 sPosX("%s.x",(char*)sPropId);
		CXhChar16 sPosY("%s.y",(char*)sPropId);
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID(sPropId));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID(sPosX));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID(sPosY));
	}
	pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("manuProcess"),&pParentItem); //获得所属分组
	if (selectObjs.GetNodeNum()==1&&pFirstPlate->face_N==1&&g_sysPara.display.bPartSketch&&pFindItem)
	{	//暂时仅支持显示单面板轮廓点列表 wht 10-09-02
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Vertex List";
		lpInfo->m_strPropHelp = "The vertex list of plate";
#else 
		lpInfo->m_strPropName = "轮廓点";
		lpInfo->m_strPropHelp = "钢板轮廓点列表";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pGroupItem->m_idProp = CLDSPlate::GetPropID("VertexList");
		if (CLDSPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
		PROFILE_VERTEX *pVertex=NULL;
		int i=0;
		for (pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
		{
			CXhChar16 sPropId("%c",'A'+i);
			CXhChar16 sPosX("%s.x",(char*)sPropId);
			CXhChar16 sPosY("%s.y",(char*)sPropId);
			//顶点
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("Vertex %s",(char*)sPropId);
			lpInfo->m_strPropHelp.Format("Vertex %s",(char*)sPropId);
#else 
			lpInfo->m_strPropName.Format("顶点%s",(char*)sPropId);
			lpInfo->m_strPropHelp.Format("顶点%s",(char*)sPropId);
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID(sPropId);
			pPropItem->m_bHideChildren = FALSE;
			if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
			//装配坐标系Y轴X坐标分量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("X");
			lpInfo->m_strPropHelp.Format("The X coordinate of vertex %s",(char*)sPropId);
#else 
			lpInfo->m_strPropName.Format("X坐标");
			lpInfo->m_strPropHelp.Format("顶点%s的X坐标值",(char*)sPropId);
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID(sPosX);
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
			//装配坐标系Y轴Y坐标分量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("Y");
			lpInfo->m_strPropHelp.Format("The Y coordinate of vertex %s",(char*)sPropId);
#else 
			lpInfo->m_strPropName.Format("Y坐标");
			lpInfo->m_strPropHelp.Format("顶点%s的Y坐标值",(char*)sPropId);
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID(sPosY);
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
			i++;
		}
	}
	PROFILE_VERTEX* pVertex=NULL;
	for (pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
	{
		if (pVertex->m_bRollEdge)
			break;
	}
	if (pVertex)
	{
		pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_cRollProcessType"),&pParentItem);
		if (pFindItem)
			SetPartMutiObjsProp(pFirstPlate,"m_cRollProcessType");
		else
		{	//板卷边处理方式
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rolling Style of Plate Edge";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Mechanical Priority|1.Processing Priority|2.Default ";
#else
			lpInfo->m_strPropName = "钢板卷边处理方式";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.最佳受力|1.最少刀数|2.默认值";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = 	CLDSPlate::GetPropID("m_cRollProcessType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
	else
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_cRollProcessType"));
}

void CLDSView::DisplayParamPlateProperty(BOOL bRebuild)
{
	CLDSParamPlate *pFirstPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pLeafItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo=NULL;
	CXhChar100 sText;
	CPropListOper<CLDSParamPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&selectObjs);
	const int GROUP_GENERAL=1,GROUP_POS=2,GROUP_PROFILE=3,GROUP_MODIFY=4;
	if (bRebuild)
	{//需要重建对象属性页
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(4);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"Position");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"Profile");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_MODIFY-1,"Modify");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"常规");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"位置");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"外形");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_MODIFY-1,"修改");
#endif
		//pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"外形");
		pPropDlg->RefreshTabCtrl(CLDSParamPlate::m_iCurDisplayPropGroup);
		//pPropDlg->RefreshTabCtrl(0);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_PARAMPLATE;
		pPropList->SetModifyValueFunc(ModifyParamPlateProperty);
		pPropList->SetButtonClickFunc(ParamPlateButtonClick);
		pPropList->SetModifyStatusFunc(ModifyParamPlateItemStatus);
		pPropList->SetPropHelpPromptFunc(FireParamPlateHelpStr);
		pPropList->SetPopMenuClickFunc(FireParamPlatePopMenuClick);
		//初始化属性项只读状态初始值,块内构件或映射构件属性不允许编辑 wht 12-10-16
		if (!pFirstPlate->BelongModel()->IsTowerModel()||pFirstPlate->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//钢板基本信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Parametric Plate Basic Information";
		lpInfo->m_strPropHelp = "The basic information of parametric plate";
#else 
		lpInfo->m_strPropName = "参数化钢板基本信息";
		lpInfo->m_strPropHelp = "参数化钢板基本信息";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pGroupItem->m_idProp = CLDSParamPlate::GetPropID("basicInfo");
		if (CLDSParamPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "Parametric plate's handle is typical unique identifier string in the  LDS system.";
#else 
		lpInfo->m_strPropName = "句柄";
		lpInfo->m_strPropHelp = "参数化钢板句柄是LDS系统中对钢板的全局唯一标识";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("handle");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//依赖节点句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Dependant node";
		lpInfo->m_strPropHelp = "Dependant node";
#else 
		lpInfo->m_strPropName = "依赖节点句柄";
		lpInfo->m_strPropHelp = "设计钢板过程中所选择的连接节点";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("designInfo.hNode");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		pPropItem->SetReadOnly();
		//依赖杆件句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Dependant rod";
		lpInfo->m_strPropHelp = "Dependant rod";
#else 
		lpInfo->m_strPropName = "依赖杆件句柄";
		lpInfo->m_strPropHelp = "设计钢板过程中所选择的连接定位杆件";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("designInfo.hRod");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		pPropItem->SetReadOnly();
		//图层
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "Parametric plate in the layer";
#else 
		lpInfo->m_strPropName = "图层";
		lpInfo->m_strPropHelp = "钢板所在图层";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("layer");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		//配材号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Configure Word";
		lpInfo->m_strPropHelp = "Configure word";
#else 
		lpInfo->m_strPropName = "配材号";
		lpInfo->m_strPropHelp = "配材号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("cfgword");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		//材质
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Material";
		lpInfo->m_strPropHelp = "The material of plate.";
#else 
		lpInfo->m_strPropName = "材质";
		lpInfo->m_strPropHelp = "钢板材质";
#endif
		lpInfo->m_cmbItems=MakeMaterialMarkSetString();
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("cMaterial");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		if (pFirstPlate->m_bStdPart)	//标准构件
			pPropItem->SetReadOnly();
		//材质
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Quality Grade";
		lpInfo->m_strPropHelp = "The quality grade of plate.";
		lpInfo->m_cmbItems="Default|A|B|C|D|";
#else 
		lpInfo->m_strPropName = "质量等级";
		lpInfo->m_strPropHelp = "钢板质量等级";
		lpInfo->m_cmbItems="默认|A|B|C|D|";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_cQualityLevel");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		if (pFirstPlate->m_bStdPart)	//标准构件
			pPropItem->SetReadOnly();
		//厚度
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Thickness";
		lpInfo->m_strPropHelp = "The thickness of plate(mm)。";
#else 
		lpInfo->m_strPropName = "厚度";
		lpInfo->m_strPropHelp = "钢板的厚度(mm)。";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("thick");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		if (pFirstPlate->m_bStdPart)	//标准构件
			pPropItem->SetReadOnly();
		//分段号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Segment Number";
		lpInfo->m_strPropHelp = "Segment number";
#else 
		lpInfo->m_strPropName = "分段号";
		lpInfo->m_strPropHelp = "分段号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("iSeg");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);//|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE); 与普通钢板统一仅在<常规>页中显示 wjh-2017.11.20
		//构件编号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		if (pFirstPlate->m_bStdPart)	//标准构件
			lpInfo->m_buttonType = BDT_COMMON;
		else
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part Number";
		lpInfo->m_strPropHelp = "Part number";
#else 
		lpInfo->m_strPropName = "构件编号";
		lpInfo->m_strPropHelp = "构件编号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("sPartNo");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//
		if (pFirstPlate->IsShadowObject())
		{
			//影射信息
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Information";
			lpInfo->m_strPropHelp = "Project information";
#else 
			lpInfo->m_strPropName = "影射信息";
			lpInfo->m_strPropHelp = "影射信息";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("shadowInfo");
			pPropItem->m_bHideChildren = FALSE;
			if (CLDSParamPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pPropItem->m_idProp);
			pPropItem->SetReadOnly(FALSE);
			//影射母构件
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Mother Part";
			lpInfo->m_strPropHelp = "The current part is project mother part.The current part is original part when the handle is Ox0.";
#else 
			lpInfo->m_strPropName = "影射母构件";
			lpInfo->m_strPropHelp = "当前构件对应的影射母构件句柄，该句柄为0x0时表示当前构件为原始构件。";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_hMotherObj");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			//关联更新编号
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Synchornize part label";
			lpInfo->m_strPropHelp = "Synchornize part label and segment number with mother part.";
			lpInfo->m_cmbItems = "Yes|No";
#else
			lpInfo->m_strPropName = "关联更新编号";
			lpInfo->m_strPropHelp = "当前影射构件与其影射母构件同步更新构件编号、段号信息。";
			lpInfo->m_cmbItems = "是|否";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_bSyncShadowPartNo");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstPlate->IsBlockRefShadowObject())
			{
				//归属部件引用
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block Assembly";
				lpInfo->m_strPropHelp = "The reference handle of the vest in block.";
#else 
				lpInfo->m_strPropName = "归属部件引用";
				lpInfo->m_strPropHelp = "当前构件归属的部件引用句柄。";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_hBlockRef");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			}
		}
		else
		{
			//关联构件
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Relative Part";
			lpInfo->m_strPropHelp = "The part associated with the current part and symmetrical type";
#else 
			lpInfo->m_strPropName = "关联构件";
			lpInfo->m_strPropHelp = "与当前构件相关联的构件以及对称方式";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("relativeObjs");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			//归属部件
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Dependent Block Handle";
			lpInfo->m_strPropHelp = "The handle of the dependent block.";
#else 
			lpInfo->m_strPropName = "归属部件";
			lpInfo->m_strPropHelp = "当前构件归属的部件句柄。";
#endif
			lpInfo->m_cmbItems=GetBlockListStr();
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_hBlock");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
		//焊接父构件
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weld Parent Part";
		lpInfo->m_strPropHelp = "If the part need to weld to other part, then the property is the handle of the part's dependent welded part;If the part doesn't need to be welded,the value is 0X0.";
#else 
		lpInfo->m_strPropName = "焊接父构件";
		lpInfo->m_strPropHelp = "如果此构件需要焊接到其它构件上,那么此属性就是本构件所依附的焊接构件的句柄;如果此构件不需要焊接,此值为0X0。";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_hPartWeldParent");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		//统材系数
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "BOM Number";
		lpInfo->m_strPropHelp = "BOM number";
#else 
		lpInfo->m_strPropName = "统材系数";
		lpInfo->m_strPropHelp = "统材系数";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_uStatMatCoef");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		//显示孔
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Hole Visible";
		lpInfo->m_strPropHelp = "Hole visible";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "显示孔";
		lpInfo->m_strPropHelp = "是否显示孔";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_bDisplayHole");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		/*显示轮廓边
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Profile Visible";
		lpInfo->m_strPropHelp = "Profile visible";
		lpInfo->m_cmbItems = "None|Actual Profile|Spread Profile";
#else
		lpInfo->m_strPropName = "显示轮廓边";
		lpInfo->m_strPropHelp = "实体模式下钢板以是否以轮廓边";
		//if(pFirstPlate->face_N==1)
			lpInfo->m_cmbItems = "0.不显示|1.轮廓边";
		//else
		//	lpInfo->m_cmbItems = "0.不显示|1.轮廓边|2.轮廓边(展开)";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_cPickedDisplayMode");
		if(pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		pPropItem->SetReadOnly();
		*/
		//显示孔
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual Part";
		lpInfo->m_strPropHelp = "Virtual Part";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "虚拟钢板";
		lpInfo->m_strPropHelp = "";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_bVirtualPart");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//用户指定构件颜色
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specified Part Color";
		lpInfo->m_strPropHelp = "Whether the part's color is specified by the user.";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "指定构件颜色";
		lpInfo->m_strPropHelp = "构件颜色是否为用户指定";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if (pFirstPlate->m_bUserSpecColor)
		{	//构件颜色
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Part Color";
			lpInfo->m_strPropHelp = "Part's color";
#else 
			lpInfo->m_strPropName = "构件颜色";
			lpInfo->m_strPropHelp = "构件颜色";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSParamPlate::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if (pFirstPlate->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		//备注
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Notes";
		lpInfo->m_strPropHelp = "Notes";
#else 
		lpInfo->m_strPropName = "备注";
		lpInfo->m_strPropHelp = "备注";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("sNotes");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		//装配方位
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Assemble Position";
		lpInfo->m_strPropHelp = "The assembly location of part and assembly direction.";
#else 
		lpInfo->m_strPropName = "装配方位";
		lpInfo->m_strPropHelp = "构件的装配位置及装配时的装配方向。";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		//pGroupItem->m_bHideChildren = TRUE;
		pGroupItem->m_idProp = CLDSParamPlate::GetPropID("assemblePos");
		if (CLDSParamPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//基点法向位置
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Normal Offset";
		lpInfo->m_strPropHelp = "In the normal the offset distance of plane to the datum node.";
#else 
		lpInfo->m_strPropName = "基点法向位置";
		lpInfo->m_strPropHelp = "装配基点法向位置。";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_fNormOffset");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Assemble CS";
		lpInfo->m_strPropHelp = "Assemble CS.";
#else 
		lpInfo->m_strPropName = "装配坐标系";
		lpInfo->m_strPropHelp = "装配坐标系。";
#endif
		CPropTreeItem *pAcsItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pAcsItem->m_bHideChildren = FALSE;
		pAcsItem->m_idProp = CLDSParamPlate::GetPropID("ucs");
		if (CLDSParamPlate::GetPropStatus(pAcsItem->m_idProp)!=-1)
			pAcsItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pAcsItem->m_idProp);
		pAcsItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配位置
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Position";
		lpInfo->m_strPropHelp = "The position of assembling datum point.";
#else 
		lpInfo->m_strPropName = "位置";
		lpInfo->m_strPropHelp = "装配基点的位置。";
#endif
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("ucs.origin");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSParamPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		/*		//基点法向位置
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "基点法向位置";
		lpInfo->m_strPropHelp = "装配基点法向位置。";
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_fNormOffset");
		if(pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		*/
		//装配坐标系原点位置X坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS.";
#else 
		lpInfo->m_strPropName = "X坐标";
		lpInfo->m_strPropHelp = "装配坐标系原点位置X坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.origin.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系原点位置Y坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS.";
#else 
		lpInfo->m_strPropName = "Y坐标";
		lpInfo->m_strPropHelp = "装配坐标系原点位置Y坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.origin.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系原点位置Z坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS.";
#else 
		lpInfo->m_strPropName = "Z坐标";
		lpInfo->m_strPropHelp = "装配坐标系原点位置Z坐标分量。";
#endif
		pLeafItem= pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.origin.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配时X轴方向
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Axis Direction";
		lpInfo->m_strPropHelp = "The assembling X axis direction";
#else 
		lpInfo->m_strPropName = "X轴方向";
		lpInfo->m_strPropHelp = "装配时X轴方向";
#endif
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_x");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSParamPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系X轴X坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X ";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS's X axis.";
#else 
		lpInfo->m_strPropName = "X坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系X轴X坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_x.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系X轴Y坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y ";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS's X axis.";
#else 
		lpInfo->m_strPropName = "Y坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系X轴Y坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_x.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系X轴Z坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z ";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS's X axis.";
#else 
		lpInfo->m_strPropName = "Z坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系X轴Z坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_x.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配时Y轴方向
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Axis Direction";
		lpInfo->m_strPropHelp = "The assembling Y axis direction";
#else 
		lpInfo->m_strPropName = "Y轴方向";
		lpInfo->m_strPropHelp = "装配时Y轴方向";
#endif
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_y");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSParamPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系Y轴X坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X ";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS's Y axis.";
#else 
		lpInfo->m_strPropName = "X坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系Y轴X坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_y.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系Y轴Y坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y ";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS's Y axis.";
#else 
		lpInfo->m_strPropName = "Y坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系Y轴Y坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_y.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系Y轴Z坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z ";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS's Y axis.";
#else 
		lpInfo->m_strPropName = "Z坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系Y轴Z坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_y.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配时Z轴方向
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Axis Direction";
		lpInfo->m_strPropHelp = "The assembling Z axis direction";
#else 
		lpInfo->m_strPropName = "Z轴方向";
		lpInfo->m_strPropHelp = "装配时Z轴方向";
#endif
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_z");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSParamPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系Z轴X坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X ";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS's Z axis.";
#else 
		lpInfo->m_strPropName = "X坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系Z轴X坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_z.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系Z轴Y坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y ";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS's Z axis.";
#else 
		lpInfo->m_strPropName = "Y坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系Z轴Y坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_z.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//装配坐标系Z轴Z坐标分量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z ";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS's Z axis.";
#else 
		lpInfo->m_strPropName = "Z坐标分量";
		lpInfo->m_strPropHelp = "装配坐标系Z轴Z坐标分量。";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_z.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//钢板工作面
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Plate Work Plane";
		lpInfo->m_strPropHelp = "The datum point of plate's work-plane  is  origin point of assembly CS,the Z axis diretion of plane is the direction of work normal.";
#else 
		lpInfo->m_strPropName = "钢板工作面";
		lpInfo->m_strPropHelp = "钢板工作面基准点为装配坐标系原点，工作面法线为钢板Z轴方向。";
#endif
		pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("plate_work_plane");
		pPropItem->m_bHideChildren = TRUE;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//钢板类型
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Plate Type";
		lpInfo->m_strPropHelp = "Plate type";
		lpInfo->m_cmbItems = "0.Flange Plate|1.U Plugboard|2.Trough Board|3.Elbow Plate|4.Vertical Stiffener Plate|5.Head Plate|6.Circular Elbow Plate|7.Circular Sealing Plate|8.Tube Elbow Plate|9.Angle Elbow Plate|10.Cross Board|11.Flat Weld Flange|12.Weld Neck Flange|13.Water Plate";
#else 
		lpInfo->m_strPropName = "钢板类型";
		lpInfo->m_strPropHelp = "钢板类型";
		lpInfo->m_cmbItems = "0.法兰盘|1.U型插板|2.槽型插板|3.肘板|4.垂向加劲板|5.封头板|6.环向肋板|7.环向封板|8.钢管肋板|9.角钢肋板|10.十字插板|11.平焊法兰|12.对焊法兰|13.流水板|14.肋板";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pGroupItem->m_idProp = CLDSParamPlate::GetPropID("m_iParamType");
		if (pFirstPlate->GetPropValueStr(pGroupItem->m_idProp,sText)>0)
			pGroupItem->m_lpNodeInfo->m_strPropValue = sText;
		pGroupItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pGroupItem->m_idProp);
		pGroupItem->SetReadOnly(pGroupItem->m_lpNodeInfo->m_bMutiObjsProp);	//不同类型参数化板不允许一起修改参数
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		//
		RebuildOrUpdateParamsPropertyList(pPropList,pGroupItem);
		if (pFirstPlate->m_iParamType==TYPE_ROLLEND)
		{  //槽型插板卷边处理方式
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rolling Style of Plate Edge";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Mechanical Priority|1.Processing Priority 1|2.Processing Priority 2|3.Default ";
#else
			lpInfo->m_strPropName = "槽型插板卷边处理方式";
			lpInfo->m_strPropHelp = "最佳受力:展开后倒角处为两条直线, 最少刀数1:保证卷边外侧边长度为L-N,最少刀数2:保证卷边内侧边长度为L-N。";
			lpInfo->m_cmbItems = "0.最佳受力|1.最少刀数1|2.最少刀数2|3.默认值";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = 	CLDSParamPlate::GetPropID("m_cRollProcessType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//钢板卷边线计算方式
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "RollLine Cal Type";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Outside|1.Inside";
#else
			lpInfo->m_strPropName = "卷边偏移方式";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.外侧|1.内侧|3.默认值";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_cRollOffsetType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		}
		if (pFirstPlate->GetFaceN()>1)
		{
			//钢板火曲变形处理方式
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Deform Type";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Assembly Plane|1.Middle Plane|2.Raised Plane|3.Sunk Plane";
#else
			lpInfo->m_strPropName = "火曲变形处理方式";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.装配面|1.中性层|2.凸面|3.凹面|4.默认值";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_cDeformType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			if (sText[0]-'0'==1)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Deform Coef";
				lpInfo->m_strPropHelp = "Deform Coef";
#else
				lpInfo->m_strPropName = "中性层系数";
				lpInfo->m_strPropHelp = "进行火曲变形的中性层系数";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_fDeformCoef");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			}
		}
		//钢板设计参数列表
		if (pFirstPlate->m_iParamType==TYPE_FL||pFirstPlate->m_iParamType==TYPE_FLG||
			pFirstPlate->m_iParamType==TYPE_FLR)
		{
			//钢板关联肘板
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
			if (pFirstPlate->m_bStdPart)	//标准构件
				lpInfo->m_cmbItems="View";
			else
				lpInfo->m_cmbItems="View|Import|Create";
			lpInfo->m_strPropName = "Relative Elbow Plate ";
			lpInfo->m_strPropHelp = "Relative elbow plate";
#else 
			if (pFirstPlate->m_bStdPart)	//标准构件
				lpInfo->m_cmbItems="查看";
			else
				lpInfo->m_cmbItems="查看|引入|生成肘板";
			lpInfo->m_strPropName = "关联肘板";
			lpInfo->m_strPropHelp = "关联肘板";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("plateAttachElbowPlate");
		}
		//钢板编辑操作
		pGroupItem=oper.InsertPropItem(pRoot,"PlateEdit");
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_MODIFY);
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"WorkUcs");
		pPropItem->m_bHideChildren=FALSE;
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		//工作坐标系原点
		pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"wcs.origin");
		pLeafItem->SetReadOnly();
		oper.InsertEditPropItem(pLeafItem,"wcs.origin.x");
		oper.InsertEditPropItem(pLeafItem,"wcs.origin.y");
		//工作坐标系X轴
		pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"wcs.axis_x");
		pLeafItem->SetReadOnly();
		oper.InsertEditPropItem(pLeafItem,"wcs.axis_x.a");
		//工作坐标系X轴
		pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"wcs.axis_y");
		pLeafItem->SetReadOnly();
		//编辑项(插入顶点、插入螺栓)
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"InsertProfileVertex");	//
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"AddNewHole");	//
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"DeleteFeature");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"FilletVertex");
		pPropItem->SetReadOnly();
	}
	else
	{
		SetPartMutiObjsProp(pFirstPlate,"handle");
		SetPartMutiObjsProp(pFirstPlate,"designInfo.hNode");
		SetPartMutiObjsProp(pFirstPlate,"designInfo.hRod");
		SetPartMutiObjsProp(pFirstPlate,"layer");
		SetPartMutiObjsProp(pFirstPlate,"cMaterial");
		SetPartMutiObjsProp(pFirstPlate,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstPlate,"iSeg");
		SetPartMutiObjsProp(pFirstPlate,"thick");
		SetPartMutiObjsProp(pFirstPlate,"sPartNo");
		SetPartMutiObjsProp(pFirstPlate,"cfgword");
		SetPartMutiObjsProp(pFirstPlate,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstPlate,"m_hPartWeldParent");
		SetPartMutiObjsProp(pFirstPlate,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstPlate,"m_cPickedDisplayMode");
		SetPartMutiObjsProp(pFirstPlate,"m_bVirtualPart");
		SetPartMutiObjsProp(pFirstPlate,"m_bUserSpecColor");

		SetPartMutiObjsProp(pFirstPlate,"wcs.origin");
		SetPartMutiObjsProp(pFirstPlate,"wcs.origin.x");
		SetPartMutiObjsProp(pFirstPlate,"wcs.origin.y");
		SetPartMutiObjsProp(pFirstPlate,"wcs.axis_x");
		SetPartMutiObjsProp(pFirstPlate,"wcs.axis_x.a");
		SetPartMutiObjsProp(pFirstPlate,"wcs.axis_y");

		CPropTreeItem *pFindItem,*pParentItem;
		pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_hBlock"),&pParentItem);
		if (pFindItem)
		{
			if (pFirstPlate->IsShadowObject())
			{	//从普通构件切换到影射构件
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_hBlock"));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("relativeObjs"));
				//影射信息
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Project Information";
				lpInfo->m_strPropHelp = "Project information";
#else 
				lpInfo->m_strPropName = "影射信息";
				lpInfo->m_strPropHelp = "影射信息";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,12,true);
				pPropItem->m_idProp = CLDSParamPlate::GetPropID("shadowInfo");
				if (CLDSParamPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pPropItem->m_idProp);
				pPropItem->SetReadOnly(FALSE);
				//影射母构件
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Project Mother Part";
				lpInfo->m_strPropHelp = "The current part is project mother part.The current part is original part when the handle is Ox0.";
#else 
				lpInfo->m_strPropName = "影射母构件";
				lpInfo->m_strPropHelp = "当前构件对应的影射母构件句柄，该句柄为0x0时表示当前构件为原始构件。";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_hMotherObj");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
				//关联更新编号
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Synchornize part label";
				lpInfo->m_strPropHelp = "Synchornize part label and segment number with mother part.";
				lpInfo->m_cmbItems = "Yes|No";
#else
				lpInfo->m_strPropName = "关联更新编号";
				lpInfo->m_strPropHelp = "当前影射构件与其影射母构件同步更新构件编号、段号信息。";
				lpInfo->m_cmbItems = "是|否";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_bSyncShadowPartNo");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				if (pFirstPlate->IsBlockRefShadowObject())
				{
					//归属部件引用
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Assembly";
					lpInfo->m_strPropHelp = "The reference handle of the vest in block.";
#else 
					lpInfo->m_strPropName = "归属部件引用";
					lpInfo->m_strPropHelp = "当前构件归属的部件引用句柄。";
#endif
					pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_hBlockRef");
					if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
					pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
					pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstPlate,"m_hBlock");
				SetPartMutiObjsProp(pFirstPlate,"relativeObjs");
			}
		}
		pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("shadowInfo"),&pParentItem);
		if (pFindItem)
		{
			if (!pFirstPlate->IsShadowObject())
			{	//从影射构件切换到普通构件
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("shadowInfo"));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_hMotherObj"));
				pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_hBlockRef"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_hBlockRef"));
				pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_hBlock"),NULL);
				if (pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstPlate,"m_hBlock");
				else
				{	//归属部件
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Handle";
					lpInfo->m_strPropHelp = "The handle of the vest in block。";
#else 
					lpInfo->m_strPropName = "归属部件";
					lpInfo->m_strPropHelp = "当前构件归属的部件句柄。";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
					pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_hBlock");
					if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
					pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
				pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("relativeObjs"),NULL);
				if (pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstPlate,"relativeObjs");
				else
				{
					if (pFindItem)
						pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_hBlock"));
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Relative Part";
					lpInfo->m_strPropHelp = "The part associated with the current part and symmetrical type";
#else 
					lpInfo->m_strPropName = "关联构件";
					lpInfo->m_strPropHelp = "与当前构件相关联的构件以及对称方式";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,12,true);
					pPropItem->m_idProp = CLDSParamPlate::GetPropID("relativeObjs");
					if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstPlate,"m_hMotherObj");
				pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_hBlockRef"),&pParentItem);
				if (pFindItem&&pFirstPlate->IsBlockRefShadowObject())
				{
					SetPartMutiObjsProp(pFirstPlate,"m_hBlockRef");
					SetPartMutiObjsProp(pFirstPlate,"m_hBlock");
				}
				else if (pFindItem&&!pFirstPlate->IsBlockRefShadowObject())
				{
					pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_hBlockRef"));
					pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_hBlock"));
				}
				else if (pFindItem==NULL&&pFirstPlate->IsBlockRefShadowObject())
				{
					pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_hMotherObj"),&pParentItem);
					//归属部件引用
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Assembly";
					lpInfo->m_strPropHelp = "The reference handle of the vest in block.";
#else 
					lpInfo->m_strPropName = "归属部件引用";
					lpInfo->m_strPropHelp = "当前构件归属的部件引用句柄。";
#endif
					pLeafItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
					pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_hBlockRef");
					if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
					pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
					pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
				}
			}
		}
		if (pFirstPlate->m_bUserSpecColor)
		{
			if (!SetPartMutiObjsProp(pFirstPlate,"crMaterial"))
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_bUserSpecColor"),NULL);
				if (pFindItem)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
					lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Part Color";
					lpInfo->m_strPropHelp = "Part's color";
#else 
					lpInfo->m_strPropName = "构件颜色";
					lpInfo->m_strPropHelp = "构件颜色";
#endif
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1);
					pPropItem->m_idProp = CLDSParamPlate::GetPropID("crMaterial");
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
					if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				}
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstPlate,"sNotes");
		SetPartMutiObjsProp(pFirstPlate,"m_fNormOffset");
		SetPartMutiObjsProp(pFirstPlate,"ucs.origin.x");
		SetPartMutiObjsProp(pFirstPlate,"ucs.origin.y");
		SetPartMutiObjsProp(pFirstPlate,"ucs.origin.z");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_x.x");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_x.y");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_x.z");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_y.x");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_y.y");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_y.z");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_z.x");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_z.y");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_z.z");
		SetPartMutiObjsProp(pFirstPlate,"m_iParamType");
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_iParamType"),NULL);
		if (pFindItem)
		{
			pFindItem->SetReadOnly(pFindItem->m_lpNodeInfo->m_bMutiObjsProp);
			RebuildOrUpdateParamsPropertyList(pPropList,pFindItem);
			pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_cRollProcessType"),&pParentItem);
			if (pFindItem)
			{
				if (pFirstPlate->m_iParamType!=TYPE_ROLLEND)
					pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_cRollProcessType"));
				else
					SetPartMutiObjsProp(pFirstPlate,"m_cRollProcessType");
			}
			else if (pFindItem==NULL&&pFirstPlate->m_iParamType==TYPE_ROLLEND)
			{   //槽型插板卷边处理方式
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_strPropName = "槽型插板卷边处理方式";
				lpInfo->m_strPropHelp = "最佳受力:展开后倒角处为两条直线, 最少刀数1:保证卷边外侧边长度为L-N,最少刀数2:保证卷边内侧边长度为L-N。";
				lpInfo->m_cmbItems = "0.最佳受力|1.最少刀数1|2.最少刀数2|3.默认值";
				pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
				pPropItem->m_idProp = 	CLDSParamPlate::GetPropID("m_cRollProcessType");
				if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
	}
	pPropList->Redraw();
}
#endif
//
void CLDSView::OnCreateBoltInPart()
{
	/*
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(console.m_pWorkPart&&m_curAppType==TA_ASSEMBLE_ENV
		&&g_sysPara.display.bPartSketch	//选择显示构件简图 wht 11-05-16
		&&(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE
		||console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE
		||console.m_pWorkPart->IsLinePart()||console.m_pWorkPart->IsArcPart()))
	{
		Ta.BeginUndoListen();
		static CBoltPropDlg new_lsdlg;
		UCS_STRU object_ucs,wcs;
		wcs=console.m_curWCS;
		object_ucs.origin.Set();
		object_ucs.axis_x.Set(1,0,0);
		object_ucs.axis_y.Set(0,0,1);
		object_ucs.axis_z.Set(0,-1,0);
		coord_trans(wcs.origin,object_ucs,FALSE);
		vector_trans(wcs.axis_x,object_ucs,FALSE);
		vector_trans(wcs.axis_y,object_ucs,FALSE);
		vector_trans(wcs.axis_z,object_ucs,FALSE);
		new_lsdlg.m_pWorkPart=console.m_pWorkPart;
		new_lsdlg.work_ucs=wcs;
		CLDSBolt ls;
		if(new_lsdlg.DoModal()==IDOK)
		{
			if(theApp.m_bCooperativeWork)
			{	//协同工作时判断是否有相应权限
				BOOL bPermit=TRUE;
				if(console.m_pWorkPart->IsLinePart())
				{
					CLDSLinePart *pLinePart=(CLDSLinePart*)console.m_pWorkPart;
					bPermit=((theApp.GetPermDword()&(pLinePart->dwPermission|pLinePart->dwStartPermission|pLinePart->dwEndPermission))!=0);
				}
				else
					bPermit=((theApp.GetPermDword()&console.m_pWorkPart->dwPermission)!=0);
				if(!bPermit)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("No corresponding permissions！");
#else
					AfxMessageBox("没有相应的权限！");
#endif
					return;
				}
			}
			Ta.BeginUndoListen();
			CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pLs->iSeg=console.m_pWorkPart->iSeg;
			pLs->cfgword=console.m_pWorkPart->cfgword;	//调整螺栓配材号与基准构件配材号一致
			pLs->m_cFuncType=new_lsdlg.m_iHoleFuncType;
			pLs->m_bVirtualPart=(pLs->m_cFuncType>=2);
			if(!restore_Ls_guige(new_lsdlg.m_sLsGuiGe,ls))
			{
				console.DeletePart(pLs->handle);
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Input specification is illegal,Creat bolt failure!");
#else
				AfxMessageBox("输入螺栓规格非法,螺栓生成失败!");
#endif
				return ;
			}
			pLs->set_d(ls.get_d());
			pLs->set_L(ls.get_L());
			pLs->hole_d_increment = new_lsdlg.m_fHoleD-ls.get_d();
			pLs->SetGrade(ls.Grade());
			//
			if(new_lsdlg.m_nWaistLen>0)
			{	//更新腰圆孔方向
				vector_trans(new_lsdlg.waist_vec,wcs,TRUE);
				vector_trans(new_lsdlg.waist_vec,console.m_pWorkPart->ucs,TRUE);
				pLs->ucs.axis_x=new_lsdlg.waist_vec;
			}
			//螺栓位置
			f3dPoint ls_pos;
			ls_pos.Set(new_lsdlg.m_fPosX,new_lsdlg.m_fPosY,new_lsdlg.m_fPosZ);
			coord_trans(ls_pos,wcs,TRUE);
			pLs->ucs.origin=ls_pos;
			pLs->m_cFuncType=new_lsdlg.m_iHoleFuncType;
			//添加螺栓引用
			CLsRef LsRef=pLs->GetLsRef();
			LsRef.waistLen=new_lsdlg.m_nWaistLen;
			if(console.m_pWorkPart->IsLinePart())
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)console.m_pWorkPart;
				if(theApp.m_bCooperativeWork)
				{
					if(pLinePart->dwPermission&theApp.GetPermDword())
						pLinePart->AppendMidLsRef(LsRef);
					else if(pLinePart->dwStartPermission&theApp.GetPermDword())
						pLinePart->AppendStartLsRef(LsRef);
					else if(pLinePart->dwEndPermission&theApp.GetPermDword())
						pLinePart->AppendEndLsRef(LsRef);
					else
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("No corresponding permissions！");
#else
						AfxMessageBox("没有相应的权限！");
#endif
				}
				else
					pLinePart->AppendMidLsRef(LsRef);
			}
			else
			{
				if(console.m_pWorkPart->IsArcPart())
				{
					CLDSArcPart *pArcPart=(CLDSArcPart*)console.m_pWorkPart;
					new_lsdlg.bak_des_ls_pos.hPart=pArcPart->handle;
					new_lsdlg.bak_des_ls_pos.R=pArcPart->baseline.Radius();
					new_lsdlg.bak_des_ls_pos.offset_angle=new_lsdlg.m_fPosX/pArcPart->baseline.Radius();
					new_lsdlg.bak_des_ls_pos.wing_offset_dist=new_lsdlg.m_fPosY;
					new_lsdlg.bak_des_ls_pos.direction=0;
					pArcPart->AppendLsRef(LsRef);
				}
				else if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
				{
					CLDSPlate *pPlate=(CLDSPlate*)console.m_pWorkPart;
					//折叠板引入螺栓时不检查过近的螺栓,FindLsByPos()可能导致某些螺栓无法引入 wht 11-07-13
					if(pPlate->IsFoldPlate())
						pPlate->AppendLsRef(LsRef,FALSE);
					else
						pPlate->AppendLsRef(LsRef);
				}
				else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
					((CLDSParamPlate*)console.m_pWorkPart)->AppendLsRef(LsRef);
			}
			pLs->AddL0Thick(console.m_pWorkPart->handle,TRUE,TRUE);
			//螺栓法线
			pLs->ucs.axis_x.Set();
			pLs->ucs.axis_y.Set();
			pLs->des_work_norm=new_lsdlg.bak_des_ls_norm;
			f3dPoint ls_norm(new_lsdlg.m_fNormX,new_lsdlg.m_fNormY,new_lsdlg.m_fNormZ);
			normalize(ls_norm);
			vector_trans(ls_norm,wcs,TRUE);
			pLs->set_norm(ls_norm);
			pLs->des_work_norm.vector=pLs->ucs.axis_z;
			if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
			{	//设置角钢螺栓法线参数
				CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.m_pWorkPart;
				if(fabs(pLineAngle->get_norm_x_wing()*ls_norm)>fabs(pLineAngle->get_norm_y_wing()*ls_norm))
					pLs->des_work_norm.norm_wing=0;
				else
					pLs->des_work_norm.norm_wing=1;
			}
			pLs->des_base_pos=new_lsdlg.bak_des_ls_pos;
			pLs->des_base_pos.datumPoint.datum_pos_style=0;
			pLs->des_base_pos.datumPoint.SetPosition(pLs->ucs.origin);
			pLs->des_base_pos.hPart=console.m_pWorkPart->handle;
			pLs->correct_worknorm();
			pLs->correct_pos();
			pLs->CalGuigeAuto();
			g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
			Ta.EndUndoListen();
		}
	}
	else
	{
		long *id_arr=NULL;
		if(g_pSolidSnap->GetLastSelectEnts(id_arr)!=1)
			return;
		CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
		if(!pPart)
			return;
		if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			Ta.BeginUndoListen();
			static CAddNewBoltDlg boltdlg;
			boltdlg.m_pWorkPart = pPart;
			boltdlg.DoModal();
			m_pDoc->SetModifiedFlag();
			Ta.EndUndoListen();
		}
	}
#endif
	*/
}

void CLDSView::OnImportBoltRefToPart()
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	long *id_arr=NULL;
	if (g_pSolidSnap->GetLastSelectEnts(id_arr)!=1)
		return;
	CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
	if (pPart)
		ImportBoltRefToPart(pPart);
#endif
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
//判断螺栓是否为十字插板立板螺栓 是则返回螺栓定位构件 否返回NULL
static CLDSPlate* IsVerticalPlateBolt(CLDSBolt *pBolt)
{
	if (pBolt)
	{
		//1.螺栓定位构件为钢板
		CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pBolt->des_base_pos.hPart,CLS_PLATE);
		if (pPlate==NULL)
			return NULL;
		//2.螺栓定位钢板的焊接父构件为十字插板
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pPlate->m_hPartWeldParent,CLS_PARAMPLATE);
		if (pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
			return pPlate;
	}
	return NULL;
}
//<DEVELOP_PROCESS_MARK nameId="CLDSView.ImportBoltRefToPart">
bool CLDSView::AppendBoltRefToPart(CLDSPart *pBasePart,CLDSBolt* pBolt,int style/* =0 */)
{
	if (pBasePart==NULL || pBolt==NULL)
		return false;
	CSuperSmartPtr<CLDSPart> pDatumPart = pBasePart;
	if (pDatumPart.IsNULL())
		return false;
	CLsRef *pLsRef=NULL;
	if (pDatumPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate=pDatumPart.PlatePointer();
		//折叠板引入螺栓时不检查过近的螺栓,FindLsByPos()可能导致某些螺栓无法引入 wht 11-07-13
		if (pPlate->IsFoldPlate())
			pLsRef=pDatumPart.PlatePointer()->AppendLsRef(pBolt->GetLsRef(),FALSE);
		else
			pLsRef=pDatumPart.PlatePointer()->AppendLsRef(pBolt->GetLsRef());
		pDatumPart.PlatePointer()->GetLsRefList()->ResetBoltRefRayNo(pDatumPart);
	}
	else if (pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		pLsRef=pDatumPart.ParamPlatePointer()->AppendLsRef(pBolt->GetLsRef());
		pDatumPart.ParamPlatePointer()->GetLsRefList()->ResetBoltRefRayNo(pDatumPart);
	}
	else if (pDatumPart->IsLinePart())
	{	//将螺栓引入到指定杆件上 wht 12-03-19
		if (style==1)
			pLsRef=pDatumPart.LinePartPointer()->AppendStartLsRef(pBolt->GetLsRef());
		else if (style==2)
			pLsRef=pDatumPart.LinePartPointer()->AppendEndLsRef(pBolt->GetLsRef());
		else //if(style==0)
			pLsRef=pDatumPart.LinePartPointer()->AppendMidLsRef(pBolt->GetLsRef());
	}
	else if (pDatumPart->IsArcPart())
		pDatumPart.ArcPartPointer()->AppendLsRef(pBolt->GetLsRef());
	else
	{
		char class_name[51];
		pDatumPart->GetClassType(class_name);
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The part type 0X%X can't support {%s},perforate failed!",pDatumPart->handle,class_name);
#else 
		logerr.Log("0X%X构件类型{%s}不支持,螺栓穿孔构件失败!",pDatumPart->handle,class_name);
#endif
		return false;
	}
	pDatumPart->SetModified();
	pBolt->AddL0Thick(pDatumPart->handle,TRUE,TRUE);
	pBolt->CalGuigeAuto();
	pBolt->SetModified();
	if (!pBolt->m_bVirtualPart)
	{
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	else
	{	//添加挂线孔，不显示螺栓实体，显示孔壁  wxc-2019.7.24
		HOLE_WALL* pHoleWall = console.MakeHoleWall(pBolt,pDatumPart);
		if (pHoleWall==NULL)
		{
			pHoleWall = console.hashHoles.Add(DUALKEY(pBolt->handle,pDatumPart->handle));
			pHoleWall->pBolt = pBolt;
			for (int j = 0; j < 4; j++)
			{
				if (pHoleWall->items[j].hRelaPart == pDatumPart->handle)
					break;
				else if (pHoleWall->items[j].hRelaPart == 0)
				{
					pHoleWall->items[j].hRelaPart = pDatumPart->handle;
					break;
				}
			}
			pHoleWall->is_visible = TRUE;
			pHoleWall->Create3dSolidModel(g_pSolidOper->GetScaleUserToScreen(),g_pSolidOper->GetScaleUserToScreen(),g_sysPara.display.nSmoothness);
		}
		pBolt->is_visible = FALSE;
		g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
		pDatumPart->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pDatumPart->GetSolidPartObject());
	}
	return true;
	//计算十字插板立板法线方向 
	/*if(bFirstCalNorm&&(pDatumPart->GetClassTypeId()==CLS_PLATE
	||pDatumPart->GetClassTypeId()==CLS_PARAMPLATE))
	{
	CLDSPlate *pPlate=IsVerticalPlateBolt(pBolt);
	if(pPlate)
	bFirstCalNorm=!CalVerticalPlateNorm(pPlate,pDatumPart);
	}*/
}
void CLDSView::ImportBoltRefToPart(CLDSPart *pBasePart,int style/*=0*/)
{
	if (pBasePart==NULL)
		return;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	long *id_arr=NULL;
	CString cmdStr;
	CLDSPart *pPart=NULL;
	if (pCmdLine->m_bGettingStrFromCmdLine)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The command running, please follow new command after the termination of the current command!");
#else 
		AfxMessageBox("有命令在运行,请终止当前命令后再执行新命令!");
#endif
		return;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Select the importing bolts,or directly input handle(Seperate: ','), press enter key to confirm:","");
#else 
	pCmdLine->FillCmdLine("选择要引入的螺栓,或直接输入句柄(可用','分隔),按回车键确认:","");
#endif
	g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
	OpenWndSel();	//开窗选择螺栓
	//创建HashTable
	CHashTable<CLDSPart*>partsetTable;
	partsetTable.Empty();
	partsetTable.CreateHashTable(100);
	if (pCmdLine->GetStrFromCmdLine(cmdStr))
	{
		Ta.BeginUndoListen();
		try {
			//根据句柄字符串引入构件
			CArray<long,long> id_arr1;
			CXhChar100 sText;
			if (cmdStr.GetLength()>0)
			{
				sText.Printf("%s",cmdStr);
				for (char* key=strtok(sText,",");key;key=strtok(NULL,","))
				{
					long h=HexStrToLong(key);
					pPart=console.FromPartHandle(h);
					id_arr1.Add(h);
				}
			}
			//BOOL bFirstCalNorm=TRUE;	//保证仅计算一次十字插板立板法线
			int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
			for (int i=0;i<id_arr1.GetSize()+n;i++)
			{
				if (i<id_arr1.GetSize())
					pPart=console.FromPartHandle(id_arr1[i]);
				else
					pPart=console.FromPartHandle(id_arr[i-id_arr1.GetSize()]);
				if (!pPart || pPart->GetClassTypeId()!=CLS_BOLT)
					continue;
				if (partsetTable.GetValueAt(pPart->handle,pPart))
					continue;
				partsetTable.SetValueAt(pPart->handle,pPart);
				CLDSBolt* pBolt=(CLDSBolt*)pPart;
				AppendBoltRefToPart(pBasePart,pBolt,style);
			}
		}
		catch (char* sError)
		{
			AfxMessageBox(sError);
		}
		g_pSolidDraw->ReleaseSnapStatus();
		m_pDoc->SetModifiedFlag();
		Ta.EndUndoListen();
	}
	else
	{
		pCmdLine->CancelCmdLine();
		return;
	}
	g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	OnOperOther();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("命令:","");
#endif
}
#endif
//</DEVELOP_PROCESS_MARK>
/*
void CLDSView::OnLookBoltInPart()
{
#ifndef __TSA_
	long *id_arr=NULL;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)!=1)
		return;
	CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
	if(!pPart)
		return;
	if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE||
		pPart->IsLinePart()||pPart->IsArcPart())
	{
		int start1_end2_mid0=0;
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			start1_end2_mid0=3;//角钢上的所有螺栓
		g_pLsRefDlg->InitLsRefDlg(pPart,start1_end2_mid0);
	}
#endif
}
*/
void CLDSView::OnLookLinePartInPlate()
{
	/*
#ifndef __TSA_
	long *id_arr=NULL;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)!=1)
		return;
	CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
	if(pPart==NULL || pPart->GetClassTypeId()!=CLS_PLATE)
		return;
	CLDSPlate* pPlate=(CLDSPlate*)pPart;
	CLinkLinePartDlg linkdlg;
	linkdlg.m_pLjPara = &pPlate->designInfo;
	linkdlg.DoModal();
#endif
	*/
}
void CLDSView::OnLookDesignParam()
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	long *id_arr=NULL,nCount=0;
	nCount=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if (nCount<1)
		return;
	CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.FromPartHandle(id_arr[0],CLS_PARAMPLATE);
	if (pParamPlate==NULL)
		return;
	for (int i=1;i<nCount;i++)
	{
		CLDSParamPlate *pPart = (CLDSParamPlate*)console.FromPartHandle(id_arr[i],CLS_PARAMPLATE);
		if (pPart==NULL)
			return;
		if (pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			return;
		if (pPart->m_iParamType!=pParamPlate->m_iParamType)
			return;
	}
	//查看参数化板的设计参数 wht 11-05-18
	if (pParamPlate->m_iParamType==TYPE_RIBPLATE)
	{
		CDesignRibbedPlateDlg ribPlateDlg;
		ribPlateDlg.SetRibPlate(pParamPlate);
		ribPlateDlg.DoModal();
	}
#ifdef __COMPLEX_PART_INC_
	else if (pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE
		||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE
		||pParamPlate->m_iParamType==TYPE_CIRRIBPLATE
		||pParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
	{
		CDesignCircularPlateDlg cirPlateDlg;
		cirPlateDlg.m_bNewCirPlate=FALSE;
		cirPlateDlg.m_pCirPlate=pParamPlate;
		cirPlateDlg.DoModal();
	}
#endif
	else
	{
		CParamPlateParamListDlg paramDlg;
		paramDlg.m_pParamPlate=pParamPlate;
		paramDlg.DoModal();
	}
#endif
}

void CLDSView::OnImportLinePartToPlate()
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	long *id_arr=NULL;
	if (g_pSolidSnap->GetLastSelectEnts(id_arr)!=1)
		return;
	CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
	if (pPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate* pPlate=(CLDSPlate*)pPart;
		ImportLinePartToPlate(pPlate);
	}
#endif
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
void CLDSView::ImportLinePartToPlate(CLDSPlate *pPlate)
{
	if (pPlate==NULL)
		return;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr;
	CLDSPart *pPart=NULL;
	CXhChar100 sText;
	if (pCmdLine->m_bGettingStrFromCmdLine)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The command running, please follow new command after the termination of the current command!");
#else 
		AfxMessageBox("有命令在运行,请终止当前命令后再执行新命令!");
#endif
		return;
	}
	g_pSolidSnap->SetSelectPartsType(SELECT_LINEPART);
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Select the importing rod,or directly input handle(Seperate: ','), press enter key to confirm:","");
#else 
	pCmdLine->FillCmdLine("选择要引入的连接杆件,或直接输入句柄(可用','分隔),按回车键确认:","");
#endif
	CUndoOperObject undo(&Ta,true);
	if (!pCmdLine->GetStrFromCmdLine(cmdStr))
	{
		pCmdLine->CancelCmdLine();
		return;
	}
	//根据句柄字符串引入构件
	CArray<long,long>id_arr1;
	if (cmdStr.GetLength()>0)
	{
		sText.Printf("%s",cmdStr);
		for (char* key=strtok(sText,",");key;key=strtok(NULL,","))
		{
			long h=HexStrToLong(key);
			pPart=console.FromPartHandle(h);
			id_arr1.Add(h);
		}
	}
	long *id_arr=NULL;
	int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	DYN_ARRAY<long> idarr(n);	//为防止后续使用id_arr之前用户清除屏幕构件选中状态带来的内存提前释放，设立此临时缓存 wjh-2015.5.17
	memcpy(idarr,id_arr,sizeof(long)*n);
	char cDefaultPlane='1';
	if (pPlate->face_N > 1)
	{	//多面板时需选在杆件所在工作面
		if (pPlate->face_N == 3)
			cmdStr = "当前引入连接杆件位于基准连接面(1)/第一火曲面(2)/第二火曲面(3)<1>:";
		else if (pPlate->face_N == 2)
			cmdStr = "当前引入连接杆件位于基准连接面(1)/第一火曲面(2)<1>:";
		while (1)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if (pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"1|2|3"))
			{
				char cPlane = '1';
				if (cmdStr.GetLength() > 0)
					cPlane = cmdStr[0];
				if (cPlane<'1' || cPlane>pPlate->face_N + '1' || cPlane > '3')
				{
					pCmdLine->FillCmdLine("输入连接面号错误","");
					pCmdLine->FinishCmdLine();
					continue;
				}
				cDefaultPlane = cPlane;
			}
			break;
		}
	}
	LINEPARTSET newrods;
	for (int i=0;i<id_arr1.GetSize()+n;i++)
	{
		if (i<id_arr1.GetSize())
			pPart=console.FromPartHandle(id_arr1[i]);
		else
			pPart=console.FromPartHandle(idarr[i-id_arr1.GetSize()]);
		if (pPart==NULL||!pPart->IsLinePart())
			continue;
		if (pPlate->designInfo.partList.GetValue(pPart->handle) != NULL)
			continue;	//钢板连接杆件中已存在此杆件
		CLDSLinePart* pRod=(CLDSLinePart*)pPart;
		//if (pRod->pStart!=NULL&&pRod->pEnd!=NULL)
		{//连接杆件中支持加入短角钢 wxc-2019.11.5
			CLDSNode*  pBaseNode = console.FromNodeHandle(pPlate->designInfo.m_hBaseNode);
			CLDSLinePart* pBaseRod = (CLDSLinePart*)console.FromPartHandle(pPlate->designInfo.m_hBasePart);
			CDesignLjPartPara desLjPartPara;
			desLjPartPara.hPart = pPart->handle;
			desLjPartPara.iFaceNo = cDefaultPlane - '0';
			desLjPartPara.m_nClassTypeId = pPart->GetClassTypeId();
			desLjPartPara.start0_end1 = CDesignJdb::LjPosInPart(pBaseRod, pBaseNode, pRod);
			if (pPart->GetClassTypeId() == CLS_LINEANGLE || pPart->GetClassTypeId() == CLS_GROUPLINEANGLE)
			{
				CLDSLineAngle* pLineAngle = (CLDSLineAngle*)pPart;
				double justify_x = pLineAngle->get_norm_x_wing()*pPlate->ucs.axis_z;
				double justify_y = pLineAngle->get_norm_y_wing()*pPlate->ucs.axis_z;
				if (fabs(justify_x) > fabs(justify_y))//&&justify_x>EPS_COS2)
					desLjPartPara.angle.cur_wing_x0_y1 = 0;//将来应考虑引入杆件在制弯面情况　wjh-2015.1.05
				else //if(fabs(justify_x)<fabs(justify_y))//&&justify_y>EPS_COS2)
					desLjPartPara.angle.cur_wing_x0_y1 = 1;
				double max_d = 0;
				for (CLsRef* pLsRef = pPlate->GetFirstLsRef(); pLsRef; pLsRef = pPlate->GetNextLsRef())
				{
					CLDSBolt* pBolt = pLsRef->GetLsPtr();
					if (pLineAngle->FindLsByHandle(pBolt->handle) && pBolt->get_d() > max_d)
						max_d = pBolt->get_d();
				}
				LSSPACE_STRU LsSpace;
				if (!GetLsSpace(LsSpace, ftoi(max_d)))
				{	//防止未引入螺栓先加入射线杆件的情况
					if (pLineAngle->GetWidth() < 63)
						LsSpace.EndSpace = 25;
					else if (pLineAngle->GetWidth() <= 110)
						LsSpace.EndSpace = 30;
					else
						LsSpace.EndSpace = 40;
				}
				desLjPartPara.ber_space = desLjPartPara.wing_space = desLjPartPara.end_space = LsSpace.EndSpace;
				desLjPartPara.angle.cbSpaceFlag.SetEndSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
			}
			pPlate->designInfo.partList.SetValue(desLjPartPara.hPart, desLjPartPara);
			newrods.append(pRod);
		}
	}
	if (newrods.GetNodeNum()>0)
	{
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Select connection bolts in new imported rods, press enter key to confirm:","");
#else 
		pCmdLine->FillCmdLine("选择新引入连接杆件上的连接螺栓,按回车键确认:","");
#endif
		if (!pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			pCmdLine->CancelCmdLine();
			return;
		}
		n=g_pSolidSnap->GetLastSelectEnts(id_arr);
		for (i=0;i<n;i++)
		{
			CLDSBolt* pBolt=(CLDSBolt*)Ta.Parts.FromHandle(id_arr[i],CLS_BOLT);
			if (pBolt==NULL)
				continue;
			for (CLDSLinePart* pRod =newrods.GetFirst(); pRod; pRod =newrods.GetNext())
			{
				if (pRod->FindLsByHandle(pBolt->handle)==NULL)
					continue;
				pPlate->AppendLsRef(pBolt->GetLsRef());
			}
		}
	}
	if (cDefaultPlane-'0'>pPlate->face_N)
	{
		pPlate->designInfo.m_bEnableFlexibleDesign=TRUE;//防止之前未开启柔性设计直接升为火曲板导致外形设计失败　wjh-2015.9.9
		pPlate->face_N++;
		long hDatumPart=pPlate->designInfo.m_hBasePart;
		CInputHuoQuLineDlg dlg;
		dlg.m_pCrossRod=newrods.GetFirst(); //给交叉材赋值
		dlg.m_sJgHandle.Format("0X%X",hDatumPart);
		dlg.m_iOffsetWing=0;
		dlg.m_iWingOffsetType=4;			//自定义
		pPart=console.FromPartHandle(pPlate->designInfo.m_hBasePart);
		if (pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pPart;
			double justify_x=pLineAngle->get_norm_x_wing()*pPlate->ucs.axis_z;
			double justify_y=pLineAngle->get_norm_y_wing()*pPlate->ucs.axis_z;
			if (fabs(justify_x)>fabs(justify_y))
				dlg.m_iOffsetWing=0;
			else
				dlg.m_iOffsetWing=1;
		}
		if (dlg.DoModal()!=IDOK)
		{
			cmdStr.Format("%d面板的火曲线初始化失败",pPlate->face_N);
			AfxMessageBox(cmdStr);
			pPlate->face_N--;
			return;
		}
		pPlate->designInfo.iFaceType=(int)pPlate->face_N;
		if (pPlate->face_N==3)
		{	//暂时仅支持折叠板式的三面板，iFaceType属性将来应由wiLegacyStyle或其它属性统一取代 wjh-2017.8.30
			pPlate->wiLegacyStyle=1;
			pPlate->designInfo.iFaceType=4;
		}
		pPlate->designInfo.iProfileStyle0123=1;
		pPlate->designInfo.huoquline_start[pPlate->face_N-2]=dlg.m_xStartPt;
		pPlate->designInfo.huoquline_end[pPlate->face_N-2]=dlg.m_xEndPt;
		pPlate->designInfo.huoqufacenorm[pPlate->face_N-2]=dlg.m_xVector;
		dlg.m_xVector.UpdateVector(console.GetActiveModel());
		pPlate->huoqufacenorm[pPlate->face_N-2]=dlg.m_xVector.vector;
	}
	pPlate->DesignPlate();
	pPlate->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	OnOperOther();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("命令:","");
#endif
}
#endif