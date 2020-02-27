//<LOCALE_TRANSLATE Confirm by hxr/>
//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "TowerPropertyDlg.h"
#include "XhCharString.h"
#include "ColorSchemaDlg.h"
#include "LmaDlg.h"
#include "SteelMatLibDlg.h"
#include "IntelligentConnectParaDlg.h"
#include "PartGuiGeLibDlg.h"
#include "JgZhunJuLibDlg.h"
#include "WireGuiGeTblDlg.h"
#include "TubeEndLjStdPartLibDlg.h"
#include "TubeEndLjGuiGeTblDlg.h"
#include "WeatherRegionDlg.h"
#include "PartUcsDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "database.h"
#include "Expression.h"
#include "LicFuncDef.h"
#include "LocalFeatureDef.h"
#include "ObjectSyncPropDetailsDlg.h"
#include "PropertyListOper.h"
#include "folder_dialog.h"
#include "LsGuigeLibDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL ModifySyssettingItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSApp::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL ModifyGeneralProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.nAutoSaveTime"))
		g_sysPara.nAutoSaveTime = atoi(valueStr)*60000;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.iAutoSaveType"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Single File Back-Up")==0)
			g_sysPara.iAutoSaveType = 0;
		else if(valueStr.CompareNoCase("Multiple File Back-Up")==0)
#else 
		if(valueStr.CompareNoCase("单文件备份")==0)
			g_sysPara.iAutoSaveType = 0;
		else if(valueStr.CompareNoCase("多文件备份")==0)
#endif
			g_sysPara.iAutoSaveType = 1;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.uFileSaveVersion"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Program Version")==0)
#else 
		if(valueStr.CompareNoCase("程序版本")==0)
#endif
			g_sysPara.uFileSaveVersion=0;
		else
		{	//获取版本号
			CString versionStr=valueStr;
			if(toupper(valueStr[0])=='V')
				versionStr=valueStr.Mid(2);
			g_sysPara.uFileSaveVersion=FromStringVersion(versionStr.GetBuffer());
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sAcadPath"))
	{
		theApp.env.m_sAcadPath = valueStr;
		theApp.WriteProfileString("ACAD","ACAD_PATH",theApp.env.m_sAcadPath);
	}	
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_ciLowestWarningLevel"))
	{
		if(valueStr[0]-'0'==0)
			g_sysPara.m_ciLowestWarningLevel=ILog2File::WARNING_LEVEL0_NONE;
		else if(valueStr[0]-'1'==0)
			g_sysPara.m_ciLowestWarningLevel=ILog2File::WARNING_LEVEL1_IMPORTANT;
		else if(valueStr[0]-'2'==0)
			g_sysPara.m_ciLowestWarningLevel=ILog2File::WARNING_LEVEL2_COMMON;
		else if(valueStr[0]-'3'==0)
			g_sysPara.m_ciLowestWarningLevel=ILog2File::WARNING_LEVEL3_UNIMPORTANT;
		else //if(valueStr[0]-'4'==0)
			g_sysPara.m_ciLowestWarningLevel=ILog2File::WARNING_LEVEL_ALL;
		logerr.SetWarningLevel(g_sysPara.m_ciLowestWarningLevel);
	}	
	else if(pItem->m_idProp==CLDSApp::GetPropID("FileRW.BUFFSIZE"))
	{
		UINT FileRW_BUFFSIZE=atoi(valueStr);	//*0x100000,MB为单位，默认50M缓存空间
		theApp.WriteProfileInt("Settings","FileRW_BUFFSIZE",FileRW_BUFFSIZE);
	}	
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgZhunJuTblPath"))
	{
		theApp.env.m_sJgZhunJuTblPath = valueStr;
		theApp.WriteProfileString("Settings","JgZhunJuTblPath",theApp.env.m_sJgZhunJuTblPath);
	}	
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgGuiGeTblPath"))
	{
		theApp.env.m_sJgGuiGeTblPath = valueStr;
		theApp.WriteProfileString("Settings","JgGuiGeTblPath",theApp.env.m_sJgGuiGeTblPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgJointParaTblPath"))
	{
		theApp.env.m_sJgJointParaTblPath = valueStr;
		theApp.WriteProfileString("Settings","JgJointParaTblPath",theApp.env.m_sJgJointParaTblPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sTubeGuiGeTblPath"))
	{
		theApp.env.m_sTubeGuiGeTblPath = valueStr;
		theApp.WriteProfileString("Settings","TubeGuiGeTblPath",theApp.env.m_sTubeGuiGeTblPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sFlatGuiGeTblPath"))
	{
		theApp.env.m_sFlatGuiGeTblPath = valueStr;
		theApp.WriteProfileString("Settings","FlatGuiGeTblPath",theApp.env.m_sFlatGuiGeTblPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sSlotGuiGeTblPath"))
	{
		theApp.env.m_sSlotGuiGeTblPath = valueStr;
		theApp.WriteProfileString("Settings","SlotGuiGeTblPath",theApp.env.m_sSlotGuiGeTblPath);
	}
#ifdef __H_SHAPE_STEEL_
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHWGuiGeTblPath"))
	{
		theApp.env.m_sHWGuiGeTblPath = valueStr;
		theApp.WriteProfileString("Settings","HWGuiGeTblPath",theApp.env.m_sHWGuiGeTblPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHMGuiGeTblPath"))
	{
		theApp.env.m_sHMGuiGeTblPath = valueStr;
		theApp.WriteProfileString("Settings","HMGuiGeTblPath",theApp.env.m_sHMGuiGeTblPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHNGuiGeTblPath"))
	{
		theApp.env.m_sHNGuiGeTblPath = valueStr;
		theApp.WriteProfileString("Settings","HNGuiGeTblPath",theApp.env.m_sHNGuiGeTblPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHPGuiGeTblPath"))
	{
		theApp.env.m_sHPGuiGeTblPath = valueStr;
		theApp.WriteProfileString("Settings","HPGuiGeTblPath",theApp.env.m_sHPGuiGeTblPath);
	}
#endif
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sLsSpaceTblPath"))
	{
		theApp.env.m_sLsSpaceTblPath = valueStr;
		theApp.WriteProfileString("Settings","LsSpaceTblPath",theApp.env.m_sLsSpaceTblPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sCInsertPlateLibPath"))
	{
		theApp.env.m_sCInsertPlateLibPath = valueStr;
		theApp.WriteProfileString("Settings","CInsertPlateLibPath",theApp.env.m_sCInsertPlateLibPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sUInsertPlateLibPath"))
	{
		theApp.env.m_sUInsertPlateLibPath = valueStr;
		theApp.WriteProfileString("Settings","UInsertPlateLibPath",theApp.env.m_sUInsertPlateLibPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sXInsertPlateLibPath"))
	{
		theApp.env.m_sXInsertPlateLibPath = valueStr;
		theApp.WriteProfileString("Settings","XInsertPlateLibPath",theApp.env.m_sXInsertPlateLibPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlDLibPath"))
	{
		theApp.env.m_sStdPartFlDLibPath = valueStr;
		theApp.WriteProfileString("Settings","StdPartFlDLibPath",theApp.env.m_sStdPartFlDLibPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlPLibPath"))
	{
		theApp.env.m_sStdPartFlPLibPath = valueStr;
		theApp.WriteProfileString("Settings","StdPartFlPLibPath",theApp.env.m_sStdPartFlPLibPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlGLibPath"))
	{
		theApp.env.m_sStdPartFlGLibPath = valueStr;
		theApp.WriteProfileString("Settings","StdPartFlGLibPath",theApp.env.m_sStdPartFlGLibPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlRLibPath"))
	{
		theApp.env.m_sStdPartFlRlibPath = valueStr;
		theApp.WriteProfileString("Settings","StdPartFlRLibPath",theApp.env.m_sStdPartFlRlibPath);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sFlCoupleLevelPath"))
	{
		theApp.env.m_sFlCoupleLevelPath = valueStr;
		theApp.WriteProfileString("Settings","FlCoupleLevelPath",theApp.env.m_sFlCoupleLevelPath);
	}
	else
		return FALSE;
	return TRUE;
}

BOOL ModifySyssettingProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	BOOL bContinueFind=FALSE;
	CPropTreeItem *pFindItem,*pParentItem=NULL;
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CString clrStr, partName, isDefault;
	COLORREF curClr = 0;
	CPropertyListOper<CLDSApp> oper(pPropList,&theApp);
#if defined(__TSA_)||defined(__LDS_)
	const int GROUP_GENERAL=1,GROUP_DISPLAY=2,GROUP_ANALYSIS=3,GROUP_DRAWING=4;
#else 
	const int GROUP_GENERAL=1,GROUP_DISPLAY=2,GROUP_DRAWING=3;
#endif
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(ModifyGeneralProperty(pPropList,pItem,valueStr))
		return TRUE;
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sUserName"))
	{
		theApp.env.m_sUserName = valueStr;
		theApp.WriteProfileString("Settings","UserName",theApp.env.m_sUserName);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sShortUserName"))
	{
		theApp.env.m_sShortUserName = valueStr;
		theApp.WriteProfileString("Settings","ShortUserName",theApp.env.m_sShortUserName);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.fRotateVelocity"))
	{
		g_sysPara.fRotateVelocity = atof(valueStr);
		g_pSolidSet->SetRotRadius(g_sysPara.fRotateVelocity*30);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bLineDepthTest"))
	{
	#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Enable")==0)
	#else
		if(valueStr.Compare("开启")==0)
	#endif
		{
			g_sysPara.m_bLineDepthTest = true;
			g_pSolidSet->EnableLineModelDepthTest();
		}
		else
		{
			g_sysPara.m_bLineDepthTest = false;
			g_pSolidSet->DisableLineModelDepthTest();
		}
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDragOpenWndSel"))
	{
	#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Enable")==0)
	#else
		if(valueStr.Compare("开启")==0)
	#endif
			g_sysPara.m_bDragOpenWndSel = true;
		else
			g_sysPara.m_bDragOpenWndSel = false;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDblClkStartRotateOper"))
	{
	#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Enable")==0)
	#else
		if(valueStr.Compare("开启")==0)
	#endif
			g_sysPara.m_bDblClkStartRotateOper = true;
		else
			g_sysPara.m_bDblClkStartRotateOper = false;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_fTowerHeight"))
	{
		Ta.m_fTowerHeight = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.segstr"))
	{
		if(valueStr.CollateNoCase(Ta.segstr)!=0)
		{
			_snprintf(Ta.segstr,200,"%s",valueStr);
			pView->RedrawAll();
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.layer"))
		_snprintf(theApp.env.layer,3,"%s",valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_fNamedHeightZeroZ"))
	{
		pItem->m_lpNodeInfo->m_bMutiObjsProp=false;
		Ta.m_bUserSpecifyNamedHeightZeroZ=true;
		Ta.NamedHeightZeroZ=atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_sPrjName"))
		sprintf(Ta.m_sPrjName,"%s",valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_sTaType"))
		sprintf(Ta.m_sTaType,"%s",valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_nTaNum"))
		Ta.m_nTaNum=atoi(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_sTaAlias"))
		sprintf(Ta.m_sTaAlias,"%s",valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_sTaStampNo"))
		sprintf(Ta.m_sTaStampNo,"%s",valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_sOperator"))
		sprintf(Ta.m_sOperator,"%s",valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("m_iPlankWeightStatType"))
       CLDSPlate::m_iPlankWeightStatType=valueStr[0]-'/';	// '/'='0'-1
	else if(pItem->m_idProp==CLDSApp::GetPropID("m_iCirclePlankAreaType"))
       CLDSParamPlate::m_iCirclePlankAreaType=valueStr[0]-'/';	// '/'='0'-1
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_cRollProcessType"))
	{
		BYTE cOldType=Ta.m_ciRollProcessType;
		Ta.m_ciRollProcessType = valueStr[0]-'0';
		if(cOldType!=Ta.m_ciRollProcessType)
		{
			CLDSParamPlate *pParamPlate=NULL;
			for(CLDSPlate* pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;
				pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
			{
				if(pPlate->GetRollEdgeHeight()!=0)
					pPlate->SetSolidModified();
			}
			for(pParamPlate=(CLDSParamPlate*)Ta.Parts.GetFirst(CLS_PARAMPLATE);pParamPlate;
				pParamPlate=(CLDSParamPlate*)Ta.Parts.GetNext(CLS_PARAMPLATE))
			{
				if(pParamPlate->m_iParamType==TYPE_ROLLEND)
					pParamPlate->SetModified();
			}
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_cRollOffsetType"))
		Ta.m_ciRollOffsetType=valueStr[0]-'0';
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_cDeformType"))
	{
		Ta.m_ciDeformType=valueStr[0]-'0';
		pPropList->DeleteAllSonItems(pItem);
		if(Ta.m_ciDeformType==1)
			oper.InsertEditPropItem(pItem,"Ta.m_fDeformCoef","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_fDeformCoef"))
		Ta.m_fDeformCoef=atof(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bSaveLogVersion"))
       g_sysPara.m_bSaveLogVersion=valueStr[0]-'0';
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_iModFileCodeType"))
		g_sysPara.m_iModFileCodeType=valueStr[0]-'0';
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_sAuditor"))
	   sprintf(Ta.m_sAuditor,"%s",valueStr);		 
	//else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_ciModelArchitecture"))
	//	Ta.m_cStructureType=valueStr[0]-'0';
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_bRepositionByModeBeforeV51"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("V1 deprecated mode")==0)
			Ta.m_ciRepositionAlgor=1;
		else if(valueStr.CompareNoCase("V2 deprecated mode")==0)
			Ta.m_ciRepositionAlgor=2;
#else
		if(valueStr.CompareNoCase("V1 旧算法")==0)
			Ta.m_ciRepositionAlgor=1;
		else if(valueStr.CompareNoCase("V2 旧算法")==0)
			Ta.m_ciRepositionAlgor=2;
#endif
		else
			Ta.m_ciRepositionAlgor=0;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectNode"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_NODE);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_NODE)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_NODE);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_NODE)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_NODE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectLineAngle"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINEANGLE);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINEANGLE)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINEANGLE);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINEANGLE)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_LINEANGLE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectLineTube"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINETUBE);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINETUBE)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINETUBE);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINETUBE)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_LINETUBE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectPlate"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_PLATE);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_PLATE)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_PLATE);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_PLATE)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_PLATE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectParamPlate"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_PARAMPLATE);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_PARAMPLATE)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_PARAMPLATE);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_PARAMPLATE)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_PARAMPLATE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectBolt"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_BOLT);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_BOLT)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_BOLT);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_BOLT)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_BOLT);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectSphere"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_SPHERE);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_SPHERE)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_SPHERE);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_SPHERE)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_SPHERE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectLineSlot"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINESLOT);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINESLOT)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINESLOT);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINESLOT)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_LINESLOT);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectLineFlat"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINEFLAT);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINEFLAT)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINEFLAT);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINEFLAT)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_LINEFLAT);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectArcAngle"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_ARCANGLE);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_ARCANGLE)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_ARCANGLE);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_ARCANGLE)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_ARCANGLE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectArcFlat"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_ARCFLAT);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_ARCFLAT)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_ARCFLAT);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_ARCFLAT)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_ARCFLAT);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectArcSlot"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_ARCSLOT);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_ARCSLOT)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_ARCSLOT);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_ARCSLOT)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_ARCSLOT);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectStdPart"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_STDPART);
		else if(valueStr.CompareNoCase("No")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_STDPART)))
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_STDPART);
		else if(valueStr.CompareNoCase("否")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_STDPART)))
#endif
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_STDPART);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectEdgeLine"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.m_bCanSelectEdgeLine = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.m_bCanSelectEdgeLine = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.m_bCanSelectEdgeLine = FALSE;
	}
	//格式刷属性选择
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bMaterial"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.brush.bMaterial = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.brush.bMaterial = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.brush.bMaterial = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bLayer"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.brush.bLayer = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.brush.bLayer = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.brush.bLayer = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bGuige"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.brush.bGuige = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.brush.bGuige = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.brush.bGuige = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bSeg"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.brush.bSeg = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.brush.bSeg = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.brush.bSeg = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bCfgword"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.brush.bCfgword = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.brush.bCfgword = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.brush.bCfgword = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bDisplayHole"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.brush.bDisplayHole = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.brush.bDisplayHole = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.brush.bDisplayHole = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bVirtualPart"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.brush.bVirtualPart = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.brush.bVirtualPart = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.brush.bVirtualPart = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bColour"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.brush.bColour = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.brush.bColour = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.brush.bColour = FALSE;
	}
	else 
		bContinueFind=TRUE;
	if(!bContinueFind)
		return TRUE;
	else //if(bContinueFind)
	{
		bContinueFind=FALSE;
	if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bPartSketch"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bPartSketch = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bPartSketch = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bPartSketch = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bNodePnt"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bNodePnt = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bNodePnt = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bNodePnt = FALSE;
		int idProp=CLDSApp::GetPropID("g_sysPara.display.xPointStyle.cbPointSize");
		if(g_sysPara.display.bNodePnt)
		{
			pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pFindItem==NULL)
			{
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Node Display Size";
				lpInfo->m_strPropHelp = "Display the node's size";
#else 
				lpInfo->m_strPropName = "节点显示大小";
				lpInfo->m_strPropHelp = "节点显示大小";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
				pFindItem->m_idProp=idProp;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_DISPLAY);
			
				idProp=CLDSApp::GetPropID("g_sysPara.display.xPointStyle.ciPointStyle");
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Node Display Style";
				lpInfo->m_strPropHelp = "Display the node's style";
				lpInfo->m_cmbItems = "0.Square Point|1.Circular Point";
#else 
				lpInfo->m_strPropName = "节点显示类型";
				lpInfo->m_strPropHelp = "节点显示类型";
				lpInfo->m_cmbItems = "0.方点|1.圆点";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,1,TRUE);
				pFindItem->m_idProp=idProp;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_DISPLAY);
			}
			idProp=CLDSApp::GetPropID("g_sysPara.display.xPointStyle.cbPointSize");
			pFindItem = pPropList->FindItemByPropId(idProp,NULL);
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
			
			idProp=CLDSApp::GetPropID("g_sysPara.display.xPointStyle.ciPointStyle");
			pFindItem = pPropList->FindItemByPropId(idProp,NULL);
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
		}
		else 
		{
			idProp=CLDSApp::GetPropID("g_sysPara.display.xPointStyle.cbPointSize");
			pPropList->DeleteItemByPropId(idProp);
			idProp=CLDSApp::GetPropID("g_sysPara.display.xPointStyle.ciPointStyle");
			pPropList->DeleteItemByPropId(idProp);
		}
		g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bPoleLine"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bPoleLine = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bPoleLine = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bPoleLine = FALSE;
		g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CLDSLinePart::bDisplayLineVecMark"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			CLDSLinePart::bDisplayLineVecMark = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			CLDSLinePart::bDisplayLineVecMark = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			CLDSLinePart::bDisplayLineVecMark = FALSE;
		g_pSolidSet->SetDisplayLineVecMark(CLDSLinePart::bDisplayLineVecMark);
		DISPLAY_TYPE disp_type;
		g_pSolidSet->GetDisplayType(&disp_type);
		if(disp_type==DISP_SOLID)
		{
			BOOL pushed = Ta.Parts.push_stack();
			for(CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)Ta.Parts.GetNext())
				pAngle->SetModified();
			Ta.Parts.pop_stack(pushed);
			g_pSolidDraw->BuildDisplayList(pView);
		}
		else
			g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CLDSNode::m_bDisplayBeforeArcLift"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Before Arc Lift")==0)
			CLDSNode::m_bDisplayBeforeArcLift = TRUE;
		else if(valueStr.CompareNoCase("After Arc Lift")==0)
#else 
		if(valueStr.CompareNoCase("预拱前")==0)
			CLDSNode::m_bDisplayBeforeArcLift = TRUE;
		else if(valueStr.CompareNoCase("预拱后")==0)
#endif
			CLDSNode::m_bDisplayBeforeArcLift = FALSE;
		g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.bWirePoint"))
	{	//荷载箭头
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else 
		if(valueStr.Compare("是")==0)
#endif
			g_sysPara.display.bWirePoint=TRUE;
		else
			g_sysPara.display.bWirePoint=FALSE;
		g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.bLoadArrow"))
	{	//荷载箭头
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else 
		if(valueStr.Compare("是")==0)
#endif
			g_sysPara.display.bLoadArrow=TRUE;
		else
			g_sysPara.display.bLoadArrow=FALSE;
		int idProp=CLDSApp::GetPropID("g_sysPara.display.nLoadArrowScale");
		if(g_sysPara.display.bLoadArrow)
		{
			pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pFindItem==NULL)
			{
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Load Arrow Size";
				lpInfo->m_strPropHelp = "The load arrow's size";
#else 
				lpInfo->m_strPropName = "荷载箭头大小";
				lpInfo->m_strPropHelp = "荷载箭头大小";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
				pFindItem->m_idProp=idProp;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_DISPLAY);
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
		}
		else 
			pPropList->DeleteItemByPropId(idProp);
		g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.xPointStyle.cbPointSize"))
	{
		g_sysPara.display.xPointStyle.cbPointSize = atoi(valueStr);
		g_pSolidSet->SetPointSize(g_sysPara.display.xPointStyle.cbPointSize);
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.xPointStyle.ciPointStyle"))
	{
		g_sysPara.display.xPointStyle.ciPointStyle = valueStr[0]-'0';
		g_pSolidSet->SetPointStyle(g_sysPara.display.xPointStyle.ciPointStyle);
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.nLoadArrowScale"))
	{
		g_sysPara.display.nLoadArrowScale = atoi(valueStr);
		g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.bDimNodeLabel"))
	{	//节点编号标注
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else 
		if(valueStr.Compare("是")==0)
#endif
			g_sysPara.display.bDimNodeLabel=TRUE;
		else
			g_sysPara.display.bDimNodeLabel=FALSE;
		int idProp=CLDSApp::GetPropID("g_sysPara.display.nNodeLabelTextHeight");
		if(g_sysPara.display.bDimNodeLabel)
		{
			pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pFindItem==NULL)
			{
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Node No. Word Height";
				lpInfo->m_strPropHelp = "The word height of node's no.";
#else 
				lpInfo->m_strPropName = "节点编号字高";
				lpInfo->m_strPropHelp = "节点编号字高";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
				pFindItem->m_idProp=idProp;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_DISPLAY);
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
		}
		else 
			pPropList->DeleteItemByPropId(idProp);
		if(theApp.m_eEnvType==FEMELEM_ENV||theApp.m_eEnvType==FEMROD_ENV)
		{
			g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
			g_pSolidDraw->Draw();
		}
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.bDimRodDesignInfo"))
	{	//节点编号标注
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else 
		if(valueStr.Compare("是")==0)
#endif
			g_sysPara.display.bDimRodDesignInfo=TRUE;
		else
			g_sysPara.display.bDimRodDesignInfo=FALSE;
		int idProp=CLDSApp::GetPropID("g_sysPara.display.nRodDesignInfoTextHeight");
		if(g_sysPara.display.bDimRodDesignInfo)
		{
			pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pFindItem==NULL)
			{
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Rod Info Word Height";
				lpInfo->m_strPropHelp = "The word height of rod's info";
#else 
				lpInfo->m_strPropName = "杆件设计信息字高";
				lpInfo->m_strPropHelp = "杆件设计信息字高";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
				pFindItem->m_idProp=idProp;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_DISPLAY);
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
		}
		else 
			pPropList->DeleteItemByPropId(idProp);
		g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.nNodeLabelTextHeight"))
	{
		g_sysPara.display.nNodeLabelTextHeight = atoi(valueStr);
		//g_pSolidSet->SetPointSize(g_sysPara.display.nPointSize);
		if(theApp.m_eEnvType==FEMELEM_ENV||theApp.m_eEnvType==FEMROD_ENV)
		{
			g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
			g_pSolidDraw->Draw();
		}
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.nRodDesignInfoTextHeight"))
	{
		g_sysPara.display.nRodDesignInfoTextHeight = atoi(valueStr);
		g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.m_fMaxScaleScr2User"))
	{	//局部放大定位时的最大缩放显示比例
		g_sysPara.m_fMaxScaleScr2User = atof(valueStr);
		g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bDispVirtualPart"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bDispVirtualPart = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bDispVirtualPart = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bDispVirtualPart = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidJg"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidJg = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidJg = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidJg = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidTube"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidTube = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidTube = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidTube = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidLjb"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidLjb = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidLjb = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidLjb = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidJoint"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidJoint = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidJoint = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidJoint = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidLs"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidLs = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidLs = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidLs = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidSlot"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidSlot = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidSlot = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidSlot = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidSphere"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidSphere = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidSphere = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidSphere = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidFlat"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidFlat = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidFlat = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidFlat = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidArcAngle"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidArcAngle = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidArcAngle = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidArcAngle = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidArcFlat"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidArcFlat = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidArcFlat = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidArcFlat = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidArcSlot"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidArcSlot = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidArcSlot = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidArcSlot = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidStdPart"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidStdPart = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidStdPart = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidStdPart = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidFoundation"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidFoundation = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidFoundation = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidFoundation = FALSE;
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidTransectFace"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			CLDSLineTube::bDisplaySolidTransectFace=TRUE;	//g_sysPara.display.bSolidTransectFace = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			CLDSLineTube::bDisplaySolidTransectFace=TRUE;	//g_sysPara.display.bSolidTransectFace = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			CLDSLineTube::bDisplaySolidTransectFace=FALSE;	//g_sysPara.display.bSolidTransectFace = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CLDSLineAngle::m_bDisplaySolidAxisLine"))
	{	//显示角钢心线
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			CLDSLineAngle::m_bDisplaySolidAxisLine=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			CLDSLineAngle::m_bDisplaySolidAxisLine=TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			CLDSLineAngle::m_bDisplaySolidAxisLine=FALSE;
		g_pSolidDraw->BuildDisplayList(pView);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CLDSGeneralPlate::m_ciDisplayVertexType"))
	{
		CLDSGeneralPlate::m_ciDisplayVertexType=valueStr[0]-'0';
		g_pSolidDraw->BuildDisplayList(pView);
	}
#endif
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidLine"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.display.bSolidLine = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.display.bSolidLine = TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.display.bSolidLine = FALSE;
		g_pSolidSet->SetSolidAndLineStatus(g_sysPara.display.bSolidLine);
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.nSmoothness"))
	{
		g_sysPara.display.nSmoothness = atoi(valueStr);
		g_pSolidSet->SetSmoothness(g_sysPara.display.nSmoothness);
	}
	else 
		bContinueFind=TRUE;
	}
	if(!bContinueFind)
		return TRUE;
	else	//if(bContinueFind)
	{
		bContinueFind=FALSE;
	if(pItem->m_idProp==CLDSApp::GetPropID("partName"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Node")==0)
#else 
		if(valueStr.CompareNoCase("节点")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crNode);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Angle(Tube) Single Line")==0)
#else 
		else if(valueStr.CompareNoCase("角钢(钢管)单线")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crLine);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Angle(Tube)")==0)
#else 
		else if(valueStr.CompareNoCase("角钢(钢管)")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crPole);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Angle X Leg")==0)
#else 
		else if(valueStr.CompareNoCase("角钢X肢")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crAngleWingX);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Angle Y Leg")==0)
#else 
		else if(valueStr.CompareNoCase("角钢Y肢")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crAngleWingY);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Angle Leg")==0)
#else 
		else if(valueStr.CompareNoCase("角钢肢边")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crAngleEdge);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("No.1 Child Angle X Leg")==0)
#else 
		else if(valueStr.CompareNoCase("1号子角钢X肢")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crSonAngle1WingX);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("No.1 Child Angle Y Leg")==0)
#else 
		else if(valueStr.CompareNoCase("1号子角钢Y肢")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crSonAngle1WingY);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("No.1 Child Angle Leg")==0)
#else 
		else if(valueStr.CompareNoCase("1号子角钢肢边")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crSonAngle1Edge);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Other Bolts")==0)
#else 
		else if(valueStr.CompareNoCase("其它螺栓")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crOtherBolt);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M12")==0)
#else 
		else if(valueStr.CompareNoCase("螺栓M12")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt12);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M16")==0)
#else 
		else if(valueStr.CompareNoCase("螺栓M16")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt16);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M20")==0)
#else 
		else if(valueStr.CompareNoCase("螺栓M20")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt20);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M24")==0)
#else 
		else if(valueStr.CompareNoCase("螺栓M24")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt24);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M27")==0)
#else 
		else if(valueStr.CompareNoCase("螺栓M27")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt27);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M30")==0)
#else 
		else if(valueStr.CompareNoCase("螺栓M30")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt30);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M36")==0)
#else 
		else if(valueStr.CompareNoCase("螺栓M36")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt36);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M39")==0)
#else 
		else if(valueStr.CompareNoCase("螺栓M39")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt39);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M42")==0)
#else 
		else if(valueStr.CompareNoCase("螺栓M42")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt42);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M48")==0)
#else 
		else if(valueStr.CompareNoCase("螺栓M48")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt48);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Connecting Plate")==0)
#else 
		else if(valueStr.CompareNoCase("连接板")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crPlate);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Standard Part")==0)
#else 
		else if(valueStr.CompareNoCase("标准构件")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crPlate);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bend Segment/Cut Segment")==0)
#else 
		else if(valueStr.CompareNoCase("火曲/切割段")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crCutFaceOrPolyJoint);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Main Background")==0)
#else 
		else if(valueStr.CompareNoCase("主背景")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBackground);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Supporting Background")==0)
#else 
		else if(valueStr.CompareNoCase("辅背景")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crPartBackground);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("partColorName"))
	{
		memmove(tem_str, tem_str+3, 97);	//跳过RGB
		sscanf(tem_str,"%X",&curClr);
		/*if(curClr==0xFFFFFFFF)
		{	
		CColorDialog dlg(RGB(0,0,0));
		dlg.DoModal();
		curClr = dlg.GetColor();

		int iCur = m_CurObjColor.IsHaveThisColor(curClr);
		if(iCur==-1)
		{
		iCur = m_CurObjColor.AddColor(curClr,"自定义",FALSE);
		m_CurObjColor.SetCurSel(iCur);
		}
		else
		m_CurObjColor.SetCurSel(iCur);

		}*/
		pPropList->GetItemPropValue(CLDSApp::GetPropID("partName"), partName);
#ifdef AFX_TARG_ENU_ENGLISH
		if(partName.CompareNoCase("Node")==0)
#else 
		if(partName.CompareNoCase("节点")==0)
#endif
		{
			if(g_sysPara.crPartMode.crNode!=curClr)
				g_sysPara.crPartMode.crNode=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Angle(Tube) Single Line")==0)
#else 
		else if(partName.CompareNoCase("角钢(钢管)单线")==0)
#endif
		{
			if(g_sysPara.crPartMode.crLine!=curClr)
				g_sysPara.crPartMode.crLine=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Angle(Tube)")==0)
#else 
		else if(partName.CompareNoCase("角钢(钢管)")==0)
#endif
		{
			if(g_sysPara.crPartMode.crPole!=curClr)
				g_sysPara.crPartMode.crPole=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Angle X Leg")==0)
#else 
		else if(partName.CompareNoCase("角钢X肢")==0)
#endif
		{
			if(g_sysPara.crPartMode.crAngleWingX!=curClr)
				g_sysPara.crPartMode.crAngleWingX=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Angle Y Leg")==0)
#else 
		else if(partName.CompareNoCase("角钢Y肢")==0)
#endif
		{
			if(g_sysPara.crPartMode.crAngleWingY!=curClr)
				g_sysPara.crPartMode.crAngleWingY=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Angle Leg")==0)
#else 
		else if(partName.CompareNoCase("角钢肢边")==0)
#endif
		{
			if(g_sysPara.crPartMode.crAngleEdge!=curClr)
				g_sysPara.crPartMode.crAngleEdge=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("No.1 Child Angle X Leg")==0)
#else 
		else if(partName.CompareNoCase("1号子角钢X肢")==0)
#endif
		{
			if(g_sysPara.crPartMode.crSonAngle1WingX!=curClr)
				g_sysPara.crPartMode.crSonAngle1WingX=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("No.1 Child Angle Y Leg")==0)
#else 
		else if(partName.CompareNoCase("1号子角钢Y肢")==0)
#endif
		{
			if(g_sysPara.crPartMode.crSonAngle1WingY!=curClr)
				g_sysPara.crPartMode.crSonAngle1WingY=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("No.1 Child Angle Leg")==0)
#else 
		else if(partName.CompareNoCase("1号子角钢肢边")==0)
#endif
		{
			if(g_sysPara.crPartMode.crSonAngle1Edge!=curClr)
				g_sysPara.crPartMode.crSonAngle1Edge=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Other Bolts")==0)
#else 
		else if(partName.CompareNoCase("其它螺栓")==0)
#endif
		{
			if(g_sysPara.crPartMode.crOtherBolt!=curClr)
				g_sysPara.crPartMode.crOtherBolt=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M12")==0)
#else 
		else if(partName.CompareNoCase("螺栓M12")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt12!=curClr)
				g_sysPara.crPartMode.crBolt12=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M16")==0)
#else 
		else if(partName.CompareNoCase("螺栓M16")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt16!=curClr)
				g_sysPara.crPartMode.crBolt16=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M20")==0)
#else 
		else if(partName.CompareNoCase("螺栓M20")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt20!=curClr)
				g_sysPara.crPartMode.crBolt20=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M24")==0)
#else 
		else if(partName.CompareNoCase("螺栓M24")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt24!=curClr)
				g_sysPara.crPartMode.crBolt24=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M27")==0)
#else 
		else if(partName.CompareNoCase("螺栓M27")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt27!=curClr)
				g_sysPara.crPartMode.crBolt27=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M30")==0)
#else 
		else if(partName.CompareNoCase("螺栓M30")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt30!=curClr)
				g_sysPara.crPartMode.crBolt30=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M36")==0)
#else 
		else if(partName.CompareNoCase("螺栓M36")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt36!=curClr)
				g_sysPara.crPartMode.crBolt36=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M39")==0)
#else 
		else if(partName.CompareNoCase("螺栓M39")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt39!=curClr)
				g_sysPara.crPartMode.crBolt39=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M42")==0)
#else 
		else if(partName.CompareNoCase("螺栓M42")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt42!=curClr)
				g_sysPara.crPartMode.crBolt42=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M48")==0)
#else 
		else if(partName.CompareNoCase("螺栓M48")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt48!=curClr)
				g_sysPara.crPartMode.crBolt48=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Connecting Plate")==0)
#else 
		else if(partName.CompareNoCase("连接板")==0)
#endif
		{
			if(g_sysPara.crPartMode.crPlate!=curClr)
				g_sysPara.crPartMode.crPlate=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bend Segment/Cut Segment")==0)
#else 
		else if(partName.CompareNoCase("火曲/切割段")==0)
#endif
		{
			if(g_sysPara.crPartMode.crCutFaceOrPolyJoint!=curClr)
				g_sysPara.crPartMode.crCutFaceOrPolyJoint=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Standard Part")==0)
#else 
		else if(partName.CompareNoCase("标准构件")==0)
#endif
		{
			if(g_sysPara.crPartMode.crStdPart!=curClr)
				g_sysPara.crPartMode.crStdPart=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Main Background")==0)
#else 
		else if(partName.CompareNoCase("主背景")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBackground!=curClr)
			{
				g_sysPara.crPartMode.crBackground=curClr;
				g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crBackground);
			}
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Supporting Background")==0)
#else 
		else if(partName.CompareNoCase("辅背景")==0)
#endif
		{
			if(g_sysPara.crPartMode.crPartBackground!=curClr)
			{
				g_sysPara.crPartMode.crPartBackground=curClr;
				g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crPartBackground);
			}
		}
		//如果即时刷新则不支持鼠标滑轮滚动 wht 08-06-26
		//pView->RedrawAll();	//即时更新右侧显示区颜色
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs"))
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Visible")==0)
			theApp.env.m_bDisplayWorldUcs = TRUE;
		else if(valueStr.CompareNoCase("Invisible")==0)
#else 
		if(valueStr.CompareNoCase("显示")==0)
			theApp.env.m_bDisplayWorldUcs = TRUE;
		else if(valueStr.CompareNoCase("隐藏")==0)
#endif
			theApp.env.m_bDisplayWorldUcs = FALSE;
		g_pSolidSet->SetDispObjectUcs(theApp.env.m_bDisplayWorldUcs);
		pView->Invalidate(FALSE);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDispPartACS"))
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Visible")==0)
			g_sysPara.m_bDispPartACS = TRUE;
		else if(valueStr.CompareNoCase("Invisible")==0)
#else 
		if(valueStr.CompareNoCase("显示")==0)
			g_sysPara.m_bDispPartACS = TRUE;
		else if(valueStr.CompareNoCase("隐藏")==0)
#endif
			g_sysPara.m_bDispPartACS = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDispWorkUcs"))
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Visible")==0)
			g_sysPara.m_bDispWorkUcs = TRUE;
		else if(valueStr.CompareNoCase("Invisible")==0)
#else 
		if(valueStr.CompareNoCase("显示")==0)
			g_sysPara.m_bDispWorkUcs = TRUE;
		else if(valueStr.CompareNoCase("隐藏")==0)
#endif
			g_sysPara.m_bDispWorkUcs = FALSE;
		if(!g_sysPara.m_bDispWorkUcs)
			g_pSolidDraw->DelCS(ISolidDraw::WORK_CS);
		else
			g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,console.m_curWCS,"Xw","Yw",NULL,NULL,200);
		//g_pSolidSet->SetDispWorkUcs(g_sysPara.m_bDispWorkUcs);
		pView->Invalidate(FALSE);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDispBlockLCS"))
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Visible")==0)
			g_sysPara.m_bDispBlockLCS = TRUE;
		else if(valueStr.CompareNoCase("Invisible")==0)
#else 
		if(valueStr.CompareNoCase("显示")==0)
			g_sysPara.m_bDispBlockLCS = TRUE;
		else if(valueStr.CompareNoCase("隐藏")==0)
#endif
			g_sysPara.m_bDispBlockLCS = FALSE;
		if(!console.GetActiveModel()->IsTowerModel())
		{	//部件自身定位坐标系id=3
			CBlockModel *pBlock=(CBlockModel*)console.GetActiveModel();
			if(g_sysPara.m_bDispBlockLCS)
				g_pSolidDraw->AddCS(ISolidDraw::BLK_LOCATE_CS,pBlock->GetLCS(),"Xa","Ya","Za");
			else
				g_pSolidDraw->DelCS(ISolidDraw::BLK_LOCATE_CS);
			pView->Invalidate(FALSE);
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs.axis_x.x"))
		theApp.env.mcs.axis_x.x = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs.axis_x.y"))
		theApp.env.mcs.axis_x.y = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs.axis_x.z"))
		theApp.env.mcs.axis_x.z = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs.axis_y.x"))
		theApp.env.mcs.axis_y.x = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs.axis_y.y"))
		theApp.env.mcs.axis_y.y = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs.axis_y.z"))
		theApp.env.mcs.axis_y.z = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs.axis_z.x"))
		theApp.env.mcs.axis_z.x = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs.axis_z.y"))
		theApp.env.mcs.axis_z.y = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs.axis_z.z"))
		theApp.env.mcs.axis_z.z = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.HuoQuDist"))
		g_sysPara.HuoQuDist = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.JgEndDist"))
		g_sysPara.JgEndDist = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.JointPlankDist"))
		g_sysPara.JointPlankDist = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.VertexDist"))
		g_sysPara.VertexDist = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.CollideDist"))
		g_sysPara.CollideDist = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.FootNailDist"))
		g_sysPara.FootNailDist = atol(valueStr);
	else if (pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bModifyCoupledFLTubeOddment"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("是")==0)
#endif
			g_sysPara.m_bModifyCoupledFLTubeOddment=TRUE;
		else
			g_sysPara.m_bModifyCoupledFLTubeOddment=FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDelBoltsResideOnPlate"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("是")==0)
#endif
			g_sysPara.m_bDelBoltsResideOnPlate=TRUE;
		else
			g_sysPara.m_bDelBoltsResideOnPlate=FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.ciLinkAngleDlgFocusCtrlMode"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Auto")==0)
			g_sysPara.ciLinkAngleDlgFocusCtrlMode=0;
		else if(valueStr.CompareNoCase("Size control prefer")==0)
			g_sysPara.ciLinkAngleDlgFocusCtrlMode=1;
#else
		if(valueStr.CompareNoCase("自动设定")==0)
			g_sysPara.ciLinkAngleDlgFocusCtrlMode=0;
		else if(valueStr.CompareNoCase("规格框优先")==0)
			g_sysPara.ciLinkAngleDlgFocusCtrlMode=1;
#endif
		else
			g_sysPara.ciLinkAngleDlgFocusCtrlMode=2;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.TubeFlDist"))
		g_sysPara.TubeFlDist = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("CLDSLineTube::ciFlTubeOddment"))
		CLDSLineTube::ciFlTubeOddment = valueStr[0]-'0';
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.fStressDiffuseAngle"))
		g_sysPara.fStressDiffuseAngle = atof(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.fInsertPlateEdgeSpace"))
		g_sysPara.fInsertPlateEdgeSpace = atof(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.fNailRepScope"))
		g_sysPara.fNailRepScope = atof(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_nPlankEdgeAngleThreshold"))
		g_sysPara.m_nPlankEdgeAngleThreshold = atol(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("m_iProfileAlgorithm"))
		CLDSPlate::m_siProfileAlgor=valueStr[0]-'0';
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.nAutoAdjustTubePos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Board Offset")==0)
			g_sysPara.nAutoAdjustTubePos = 1;
		else if(valueStr.CompareNoCase("Tube Offset")==0)
#else 
		if(valueStr.CompareNoCase("插板偏移")==0)
			g_sysPara.nAutoAdjustTubePos = 1;
		else if(valueStr.CompareNoCase("钢管偏移")==0)
#endif
			g_sysPara.nAutoAdjustTubePos = 2;
		else //if(valueStr.CompareNoCase("不启用")==0)
			g_sysPara.nAutoAdjustTubePos = 0;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("MaxWasherN"))
		g_sysPara.BoltPadThick.cnMaxWasherN=(BYTE)atoi(valueStr);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.BoltPadThick.m_nM12"))
		g_sysPara.BoltPadThick.FromThickStr(valueStr,12);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.BoltPadThick.m_nM16"))
		g_sysPara.BoltPadThick.FromThickStr(valueStr,16);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.BoltPadThick.m_nM20"))
		g_sysPara.BoltPadThick.FromThickStr(valueStr,20);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.BoltPadThick.m_nM24"))
		g_sysPara.BoltPadThick.FromThickStr(valueStr,24);
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_fKaiHeJiaoThreshold"))
	{
		g_sysPara.m_fKaiHeJiaoThreshold = atof(valueStr);	//开合角阈值 默认为2°
		//更新注册表内容 wht 11-06-13
		AfxGetApp()->WriteProfileString(_T("Settings"),_T("KaiHeJiaoThreshold"),valueStr);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_fKaiHeJiaoMinDeita"))
	{
		g_sysPara.m_fKaiHeJiaoMinDeita = atof(valueStr);
		AfxGetApp()->WriteProfileString(_T("Settings"),_T("KaiHeJiaoMinDeita"),valueStr);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_nGroupJgMachiningGap"))
	{
		g_sysPara.m_nGroupJgMachiningGap=atoi(valueStr);
		AfxGetApp()->WriteProfileInt(_T("Settings"),_T("GroupJgMachiningGap"),g_sysPara.m_nGroupJgMachiningGap);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_nMatCharPosType"))
	{
		g_sysPara.m_nMatCharPosType = valueStr[0]-'0';
		//更新注册表内容
		AfxGetApp()->WriteProfileInt(_T("Settings"),_T("MatCharPosType"),g_sysPara.m_nMatCharPosType);
		//
		pPropList->DeleteAllSonItems(pItem);
		if(g_sysPara.m_nMatCharPosType>0)
			oper.InsertCmbEditPropItem(pItem,"g_sysPara.m_cMatCharSeparator","","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_cMatCharSeparator"))
	{
		if(valueStr.CompareNoCase("无")==0)
			g_sysPara.m_cMatCharSeparator=0;
		else if(valueStr.CompareNoCase("空格")==0)
			g_sysPara.m_cMatCharSeparator=' ';
		else
			g_sysPara.m_cMatCharSeparator=valueStr[0];
		AfxGetApp()->WriteProfileInt(_T("Settings"),_T("MatSeparator"),BYTE(g_sysPara.m_cMatCharSeparator));
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_fMinCutAngleLen"))
		g_sysPara.m_fMinCutAngleLen=atof(valueStr);
	else if (pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_fMaxIgnoreCutAngleLen"))
		g_sysPara.m_fMaxIgnoreCutAngleLen=atof(valueStr);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.fTubeWeldRoadMinD"))
	{
		double fOldR=g_sysPara.fTubeWeldRoadMinD;
		g_sysPara.fTubeWeldRoadMinD = atof(valueStr);
		if(fabs(fOldR-g_sysPara.fTubeWeldRoadMinD)>EPS)
		{	//调整无缝钢管阈值后应重新生成钢管实体 wht 11-08-29
			CLDSLineTube *pLineTube=NULL;
			for(pLineTube=(CLDSLineTube*)Ta.Parts.GetFirst(CLS_LINETUBE);pLineTube;
				pLineTube=(CLDSLineTube*)Ta.Parts.GetNext(CLS_LINETUBE))
			{
				if(pLineTube->GetDiameter()>g_sysPara.fTubeWeldRoadMinD)
					pLineTube->m_bHasWeldRoad=TRUE;
				else 
					pLineTube->m_bHasWeldRoad=FALSE;
				pLineTube->SetModified();
			}
		}
	}
#endif
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDisplayConnectDlg"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
#endif
			g_sysPara.m_bDisplayConnectDlg=TRUE;
		else
			g_sysPara.m_bDisplayConnectDlg=FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.b1BoltDesignPrompt"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
#endif
			g_sysPara.b1BoltDesignPrompt=TRUE;
		else
			g_sysPara.b1BoltDesignPrompt=FALSE;
	}
	else if (pItem->m_idProp == CLDSApp::GetPropID("CLDSBolt::BOLTL0_CALMODE"))
	{
		CLDSBolt::BOLTL0_CALMODE = (valueStr[0] - '0');
	}
	else if (pItem->m_idProp == CLDSApp::GetPropID("CLDSBolt::BOLTL0_PREFER_MODE"))
	{
		CLDSBolt::BOLTL0_PREFER_MODE = (valueStr[0] - '0');
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_b45CutAngleNeedCoefR"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
#endif
			g_sysPara.m_b45CutAngleNeedCoefR=TRUE;
		else
			g_sysPara.m_b45CutAngleNeedCoefR=FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_fCutAngleCoefR"))
	{
		CExpression expression;
		g_sysPara.m_fCutAngleCoefR=expression.SolveExpression(valueStr.GetBuffer());	
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bAnglePreferSimplePosMode"))
	{
		if(valueStr.CompareNoCase("参数化定位")==0)
			g_sysPara.m_bAnglePreferSimplePosMode=FALSE;	
		else //if(valueStr.CompareNoCase("简单定位")==0)
			g_sysPara.m_bAnglePreferSimplePosMode=TRUE;	
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.cHoleWallMode"))
	{
		g_sysPara.display.cHoleWallMode=valueStr[0]-'0';
		g_pSolidDraw->BuildDisplayList(pView);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CLDSPart::UPDATE_PARTNO_AND_SEG"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
#endif
			CLDSPart::UPDATE_PARTNO_AND_SEG=true;
		else
			CLDSPart::UPDATE_PARTNO_AND_SEG=false;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bEnablePrompt"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
#endif
			g_sysPara.m_bEnablePrompt=TRUE;
		else
			g_sysPara.m_bEnablePrompt=FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bOnlyDesignCurView"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.intelligentConnect.m_bOnlyDesignCurView=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.intelligentConnect.m_bOnlyDesignCurView=TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.intelligentConnect.m_bOnlyDesignCurView=FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignCommonConnect"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.intelligentConnect.m_bDesignCommonConnect=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.intelligentConnect.m_bDesignCommonConnect=TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.intelligentConnect.m_bDesignCommonConnect=FALSE;
		if(!g_sysPara.intelligentConnect.m_bDesignCommonConnect)
			pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignBendConnect"));
		else
		{
			int idProp=CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignBendConnect");
			pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pFindItem==NULL)
			{
				CItemInfo* lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
		#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems = "Yes|No";
				lpInfo->m_strPropName = "Desing Connect With Two Connect Planes";
				lpInfo->m_strPropHelp = "Include designing common datum rod and ray rod connecting with two connect planes";
		#else
				lpInfo->m_cmbItems = "是|否";
				lpInfo->m_strPropName = "智能设计制弯板连接";
				lpInfo->m_strPropHelp = "智能设计时包括基准材射线材制弯板连接设计";
		#endif
				pItem->m_bHideChildren=FALSE;
				CPropTreeItem* pLeafItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pLeafItem->m_idProp = CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignBendConnect");
				pLeafItem->m_dwPropGroup=pItem->m_dwPropGroup;//etSingleWord(iSectionType);
				pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
			}
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignBendConnect"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.intelligentConnect.m_bDesignBendConnect=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.intelligentConnect.m_bDesignBendConnect=TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.intelligentConnect.m_bDesignBendConnect=FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignAngleJoint"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.intelligentConnect.m_bDesignAngleJoint=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.intelligentConnect.m_bDesignAngleJoint=TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.intelligentConnect.m_bDesignAngleJoint=FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignFL"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.intelligentConnect.m_bDesignFL=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.intelligentConnect.m_bDesignFL=TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.intelligentConnect.m_bDesignFL=FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("是")==0)
			g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate=TRUE;
		else if(valueStr.CompareNoCase("否")==0)
#endif
			g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate=FALSE;
	}
	else
	{
		bContinueFind=TRUE;
		if(Ta.hashParams.GetNodeNum()>0)   //设计参数
		{
			for (DESIGN_PARAM_ITEM* pParam=Ta.hashParams.GetFirst();pParam;pParam=Ta.hashParams.GetNext())
			{
				if(pItem->m_idProp==CLDSApp::GetPropID(CXhChar50("model.%s",(char*)pParam->sLabel)))
				{
					if(pParam->dataType==DESIGN_PARAM_ITEM::TYPE_DOUBLE)
						pParam->value.fValue=atof(valueStr);
					else if(pParam->dataType==DESIGN_PARAM_ITEM::TYPE_INTEGER)
						pParam->value.iType=atoi(valueStr);
					else if(pParam->dataType==DESIGN_PARAM_ITEM::TYPE_HANDLE)
						pParam->value.hPart=HexStrToLong(valueStr);
					else
						continue;
					bContinueFind=FALSE;
					break;
				}
			}
		}
	}
	}
	//if else层次过深在此打断
	if(!bContinueFind)
		return TRUE;
	else	//if(bContinueFind)
	{	//设计
		bContinueFind=FALSE;
#if defined(__TSA_)||defined(__LDS_)
		if(pItem->m_idProp==CLDSApp::GetPropID("m_cLandscape"))
		{
			Ta.m_cLandscape=valueStr[0];
		}
		else if(pItem->m_idProp==CLDSApp::GetPropID("m_cLineRoadType"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.Compare("Flat line")==0)
				Ta.m_cLineRoadType=0;
			else if(valueStr.Compare("Hills line")==0)
#else
			if(valueStr.Compare("平地线路")==0)
				Ta.m_cLineRoadType=0;
			else if(valueStr.Compare("丘陵线路")==0)
#endif
				Ta.m_cLineRoadType=1;
			else//if(valueStr.Compare("山地线路")==0)
				Ta.m_cLineRoadType=2;
		}
		else if(pItem->m_idProp==CLDSApp::GetPropID("m_nHighestTemperature"))
			Ta.weather.m_nHighestT=atoi(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("m_nLowestTemperature"))
			Ta.weather.m_nLowestT=atoi(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("m_nAverageTemperature"))
			Ta.weather.m_nAverageT=atoi(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("m_fMaxWindVelocity"))
			Ta.weather.m_fMaxWindV=atof(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("m_nMaxWindVelocityTemperature"))
			Ta.weather.m_nMaxWindT=atoi(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("m_fMaxIceThick"))
			Ta.weather.m_fMaxIceThick=atof(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("m_nMaxIceTemperature"))
			Ta.weather.m_nMaxIceT=atoi(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("m_fMaxIceWindVelocity"))
			Ta.weather.m_fMaxIceWindV=atof(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("m_nSetupTemperature"))
			Ta.weather.m_nSetupT=atoi(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("m_fSetupWindVelocity"))
			Ta.weather.m_fSetupWindV=atof(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_fIceDensity"))
			Ta.m_fIceDensity=atof(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.iLanguage"))
		{
			g_sysPara.iLanguage=valueStr[0]-'0';
		}
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.iDetailDocType"))
		{
			g_sysPara.iDetailDocType=valueStr[0]-'0';
		}
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bMergeRptFileMirObj"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				g_sysPara.bMergeRptFileMirObj=TRUE;
			else
				g_sysPara.bMergeRptFileMirObj=FALSE;
		}
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.iLegShiftType"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("0.None rotate")==0)
				g_sysPara.iLegShiftType=0;
			else if(valueStr.CompareNoCase("1.Rotation of accordant legs")==0)
#else
			if(valueStr.CompareNoCase("0.不轮换")==0)
				g_sysPara.iLegShiftType=0;
			else if(valueStr.CompareNoCase("1.等高腿轮换")==0)
#endif
				g_sysPara.iLegShiftType=1;
			else// if(valueStr.CompareNoCase("2.高低腿轮换")==0)
				g_sysPara.iLegShiftType=2;
		}
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.nM"))
			g_sysPara.nM=atoi(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.nSigemaM"))
			g_sysPara.nSigemaM=atoi(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.nPrecentSigemaM"))
			g_sysPara.nPrecentSigemaM=atoi(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_cStructureType"))
			Ta.m_cStructureType=valueStr[0]-'0';
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.iEfficacyCeofType"))
		{
			g_sysPara.iEfficacyCeofType=valueStr[0]-'0';
		}
#ifndef __LDS_TDA_	//不是LDS的仅绘图子模式
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.iShowDeformedMapPlan"))
		{
			g_sysPara.iShowDeformedMapPlan=valueStr[0]-'0';
			if(g_sysPara.iShowDeformedMapPlan>0)
			{
				((CMainFrame*)AfxGetMainWnd())->m_bDisplayDeformedTower=TRUE;
				theApp.m_eEnvType=FEMELEM_ENV;	//转换到计算结果预览形式时自动转换到单元模式
			}
			g_pSolidDraw->BuildDisplayList(pView);
		}
#endif
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.reactionExportType"))
		{
			g_sysPara.reactionExportType=valueStr[0]-'0';
		}
		/*
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bCalStdFoundationForce"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				g_sysPara.bCalStdFoundationForce=TRUE;
			else
				g_sysPara.bCalStdFoundationForce=FALSE;
		}
		*/
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bUpdateRodDesignForce"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				g_sysPara.bUpdateRodDesignForce=TRUE;
			else
				g_sysPara.bUpdateRodDesignForce=FALSE;
		}
		/*else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bCreateAnalFile"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				g_sysPara.bCreateAnalFile=TRUE;
			else
				g_sysPara.bCreateAnalFile=FALSE;
		}*/
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bRecNodeOffset"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				g_sysPara.bRecNodeOffset=TRUE;
			else
				g_sysPara.bRecNodeOffset=FALSE;
		}
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bCreateAnsysCmdFile"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				g_sysPara.bCreateAnsysCmdFile=TRUE;
			else
				g_sysPara.bCreateAnsysCmdFile=FALSE;
		}
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bCreateDetailRptFile"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
			{
				g_sysPara.bCreateDetailRptFile=TRUE;
				pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iDetailDocType"),&pParentItem);
				if(pFindItem==NULL)
				{	//详细报告输出文档类型
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "document type";
					lpInfo->m_strPropHelp = "document type";
					lpInfo->m_cmbItems = "0.Text Document|1.EXCEL Form";
#else
					lpInfo->m_strPropName = "文档类型";
					lpInfo->m_strPropHelp = "文档类型";
					lpInfo->m_cmbItems = "0.文本文档|1.EXCEL表格";
#endif
					pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
					pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iDetailDocType");
					pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

				pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iLanguage"),&pParentItem);
				if(pFindItem==NULL)
				{	//详细报告输出文档类型
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Output language";
					lpInfo->m_strPropHelp = "Computer output language";
					lpInfo->m_cmbItems = "0.Chinese|1.English";
#else
					lpInfo->m_strPropName = "输出语言";
					lpInfo->m_strPropHelp = "计算书输出语言";
					lpInfo->m_cmbItems = "0.中文|1.英文";
#endif
					pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
					pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iLanguage");
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

				pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.bMergeRptFileMirObj"),&pParentItem);
				if(pFindItem==NULL)
				{	//详细报告输出文档类型
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Merge symmetrical items";
                    lpInfo->m_strPropHelp = "Merge symmetrical nodes and the information output of rods";
                    lpInfo->m_cmbItems = "Yes|No";
#else
					lpInfo->m_strPropName = "合并对称项";
					lpInfo->m_strPropHelp = "合并对称节点和杆件的信息输出";
					lpInfo->m_cmbItems = "是|否";
#endif
					pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
					pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.bMergeRptFileMirObj");
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

				pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iEfficacyCeofType"),&pParentItem);
				if (pFindItem==NULL)
				{
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Type of margin coefficient";
					lpInfo->m_strPropHelp = "Bearing capacity factor is margin coefficient only relative to load,but the design margin includes limited margin of slenderness ratio.";
					lpInfo->m_cmbItems = "0.Design margin|1.Bearing capacity factor";
#else
					lpInfo->m_strPropName = "裕度系数类型";
					lpInfo->m_strPropHelp = "承载系数是只与荷载有关的裕度系数，而设计裕度中则包含了长细比限制裕度。";
					lpInfo->m_cmbItems = "0.承载+长细比系数|1.承载系数";
#endif
					pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
					pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iEfficacyCeofType");
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

				CPropTreeItem *pSonPropItem;
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Output of bending moment";
#else
				lpInfo->m_strPropName = "弯矩项输出";
#endif
				pSonPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.putOutM");
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				pSonPropItem->m_bHideChildren=TRUE;
				if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
					pSonPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pSonPropItem->m_idProp);

				pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.nM"),&pParentItem);
				if (pFindItem==NULL)
				{
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "M threshold value";
					lpInfo->m_strPropHelp = "Absolute value of bending moment";
#else
					lpInfo->m_strPropName = "M阈值";
					lpInfo->m_strPropHelp = "弯矩绝对值";
#endif
					pFindItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
					pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nM");
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

				pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.nSigemaM"),&pParentItem);
				if(pFindItem==NULL)
				{
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "σM threshold value";
					lpInfo->m_strPropHelp = "Bending stress from bending moment";
#else
					lpInfo->m_strPropName = "σM阈值";
					lpInfo->m_strPropHelp = "由弯矩产生的弯曲应力";
#endif
					pFindItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
					pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nSigemaM");
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

				pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.nPrecentSigemaM"),&pParentItem);
				if(pFindItem==NULL)
				{
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "σM %threshold value";
					lpInfo->m_strPropHelp = "percentage for the bending stress from bending moment in stress produced by axial force";
#else
					lpInfo->m_strPropName = "σM %阈值";
					lpInfo->m_strPropHelp = "由弯矩产生的弯曲应力占轴力产生应力的百分比";
#endif
					pFindItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
					pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nPrecentSigemaM");
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

				pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.reactionExportType"),&pParentItem);
				if(pFindItem==NULL)
				{	//详细报告输出文档类型
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "basic forces filtrating";
					lpInfo->m_strPropHelp = "When filtrating the level of high-low legs，only output the maximum pull-up force and down force from a module;when filtrating in a working condition level,only output all the pull-up force and down force from a moudle in  working condition ";
					lpInfo->m_cmbItems = "0.Full output|1.High-low legs level|2.Working condition level";
#else
					lpInfo->m_strPropName = "基础作用力筛选";
					lpInfo->m_strPropHelp = "高低腿级筛选时，只输出一个呼高组工况中所有高低腿中的最大上拨力和下压力;工况级筛选时只输出一个呼高组中所有工况的上拨力和下压力。";
					lpInfo->m_cmbItems = "0.全部输出|1.高低腿级|2.工况级";
#endif
					pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
					pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.reactionExportType");
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
			}
			else
			{
				g_sysPara.bCreateDetailRptFile=FALSE;
				pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.iDetailDocType"));
				pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.iLanguage"));
				pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.bMergeRptFileMirObj"));
				pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.putOutM"));
				pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.iEfficacyCeofType"));	
				pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.reactionExportType"));
			}
		}
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_bAuxPoleAnalysisInc"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.Compare("Yes")==0)
#else
			if(valueStr.Compare("是")==0)
#endif
				g_sysPara.m_bAuxPoleAnalysisInc=TRUE;
			else
				g_sysPara.m_bAuxPoleAnalysisInc=FALSE;
		}
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_iResolverStyle"))
			g_sysPara.AnalysisPara.m_iResolverStyle = atol(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_fBeamLateralStifReductCoef"))
			g_sysPara.AnalysisPara.m_fBeamLateralStifReductCoef = atof(valueStr);
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_iNonlinearStyle"))
		{
			g_sysPara.AnalysisPara.m_iNonlinearStyle = atol(valueStr);
			if(g_sysPara.AnalysisPara.m_iNonlinearStyle == 0)
			{
				pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_nIterationCounts"));
				pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_fConvergeLim"));
			}
			else if(g_sysPara.AnalysisPara.m_iNonlinearStyle == 1)
			{
				CItemInfo *lpInfo=NULL;
				int idProp=CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_nIterationCounts");
				pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
				if(pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
    					lpInfo->m_strPropName = "Iterations";
					lpInfo->m_strPropHelp = "The most iterations times at every increments, quit from the iterations if the number is over";
#else
					lpInfo->m_strPropName = "迭代次数";
					lpInfo->m_strPropHelp = "每增量内最多迭代次数，超过则退出迭代";
#endif
					pFindItem = pPropList->InsertItem(pItem,lpInfo,4,TRUE);
					pFindItem->m_idProp=idProp;
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

				idProp=CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_fConvergeLim");
				pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
				if(pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
    					lpInfo->m_strPropName = "The value of convergence";
					lpInfo->m_strPropHelp = "Unbalancing force convergence value during mechanics analysis and calculation,unit N";
#else
					lpInfo->m_strPropName = "收敛阈值";
					lpInfo->m_strPropHelp = "力学分析计算时不平衡力收敛阈值,单位N";
#endif
					pFindItem = pPropList->InsertItem(pItem,lpInfo,5,TRUE);
					pFindItem->m_idProp=idProp;
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
			}
			long prop_id=CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_iResolverStyle");
			pFindItem = pPropList->FindItemByPropId(prop_id,&pParentItem);
			if(pFindItem!=NULL)
			{
				g_sysPara.AnalysisPara.m_iResolverStyle=0;
				pPropList->SetItemPropValue(prop_id,theApp.GetPropValueStr(prop_id));
				pFindItem->SetReadOnly(g_sysPara.AnalysisPara.m_iNonlinearStyle==1);
			}
		}
		else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_nSubSpaceIterCounts"))
			g_sysPara.AnalysisPara.m_nSubSpaceIterCounts = atol(valueStr);
		else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_fConvergeLim"))
			g_sysPara.AnalysisPara.m_fConvergeLim = atof(valueStr);
		else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_fFreqsTolerance"))
			g_sysPara.AnalysisPara.m_fFreqsTolerance = atof(valueStr);
		else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_nIterationCounts"))
			g_sysPara.AnalysisPara.m_nIterationCounts = atol(valueStr);
		else if(pItem->m_idProp == CLDSApp::GetPropID("Ta.SelMatPara.m_bPreferSingleAngle"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				Ta.SelMatPara.m_bPreferSingleAngle = TRUE;
			else
				Ta.SelMatPara.m_bPreferSingleAngle = FALSE;
		}
		else if(pItem->m_idProp == CLDSApp::GetPropID("Ta.SelMatPara.m_iSizeOrderRule"))
			Ta.SelMatPara.m_iSizeOrderRule = atoi(valueStr);
		else if(pItem->m_idProp == CLDSApp::GetPropID("Ta.SelMatPara.m_nConvergeLim"))
			Ta.SelMatPara.m_nConvergeLim = atol(valueStr);
		else if(pItem->m_idProp == CLDSApp::GetPropID("Ta.SelMatPara.m_nMaxSelMatCount"))
			Ta.SelMatPara.m_nMaxSelMatCount = atol(valueStr);
		else
			return FALSE;
#endif
	}
	return TRUE;
}
#ifdef __COMMON_PART_INC_
static void ModifyInsertPlateEdgeSpace()
{
	int i=0;
	g_pSolidDraw->ReleaseSnapStatus();
	for(CLDSPlate *pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
	{
		BOOL bValue=FALSE;
		for(CDesignLjPartPara *pLjPart=pPlate->designInfo.partList.GetFirst();pLjPart;pLjPart=pPlate->designInfo.partList.GetNext())
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pLjPart->hPart,CLS_LINEPART);
			if(pLinePart==NULL||(pLinePart&&!pLinePart->IsLinePart()))
				continue;
			if(pLinePart->handle==pPlate->designInfo.m_hBasePart)
				continue;	//跳过基准定位杆件
			if(pLinePart->GetClassTypeId()==CLS_LINETUBE&&(pLjPart->start0_end1==0||pLjPart->start0_end1==1))
			{
				pLjPart->ber_space=g_sysPara.fInsertPlateEdgeSpace;
				bValue=TRUE;
			}	
		}
		if(bValue)
		{
			pPlate->DesignPlate();
			pPlate->SetModified();
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
			i++;
		}
	}
	g_pSolidDraw->Draw();
	CString sPrompt;
#ifdef AFX_TARG_ENU_ENGLISH
	sPrompt.Format("Revision is completed, the number of revised plate is  %d !",i);
#else 
	sPrompt.Format("修订完成，共修订%d块钢板的外形!",i);
#endif
	AfxMessageBox(sPrompt);
}
#endif
BOOL SyssettingButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem, *pParentItem;
	CReportVectorDlg vecDlg;
	CString SelectedFile="", valueStr=""; 
	CString Filter = "";
	CPropertyListOper<CLDSApp> oper(pPropList,&theApp);
	if(pItem->m_idProp==CLDSApp::GetPropID("partColorName")/*(COLOR)*/)
	{
		COLORREF initClr;
		char temp_clr[MAX_TEMP_CHAR_100+1];
		CString currClr = pItem->m_lpNodeInfo->m_strPropValue;
		//parse the property's current color value
		if (currClr.Find("RGB") > -1)
		{
			_snprintf(temp_clr,MAX_TEMP_CHAR_100,"%s",currClr);
			memmove(temp_clr,temp_clr+3,MAX_TEMP_CHAR_100-3);
			sscanf(temp_clr,"%X",&initClr);
		}
		else
			initClr = 0;
		CColorDialog ClrDlg(initClr);
		if (IDOK!=ClrDlg.DoModal())
			return FALSE;
		COLORREF selClr = ClrDlg.GetColor();
		CString clrStr;
		clrStr.Format("RGB%X",selClr);
		pItem->m_lpNodeInfo->m_strPropValue = clrStr;
		return TRUE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("ColorScheme"))
	{
		CColorSchemeDlg colorSchemeDlg;
		if(colorSchemeDlg.DoModal()==IDOK)
		{
			CLDSView* pView=((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
			pView->RenderAll();		//颜色方案修改后，所有构件进行重新绘制
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("defaultColor"))
	{
		CString partName, clrStr;
		pPropList->GetItemPropValue(CLDSApp::GetPropID("partName"), partName);
		g_sysPara.crPartMode.crNode = RGB(255,255,255);
		g_sysPara.crPartMode.crControlNode = RGB(255,255,255);		  //控制节点
		g_sysPara.crPartMode.crDivideScaleNode = RGB(255,255,0);      //等分节点
		g_sysPara.crPartMode.crOffsetNode = RGB(128,128,0);   	      //偏移节点
		g_sysPara.crPartMode.crAxisValFixNode= RGB(255,255,0);        //指定坐标分量节点
		g_sysPara.crPartMode.crAngleEdge = RGB(0,100,255);
		g_sysPara.crPartMode.crPole   = RGB(150,150,255);
		g_sysPara.crPartMode.crLine   = RGB(150,150,255);
		g_sysPara.crPartMode.crBolt12 = RGB(255,0,255);
		g_sysPara.crPartMode.crBolt16 = RGB(128,0, 64);
		g_sysPara.crPartMode.crBolt20 = RGB(128,0,255);
		g_sysPara.crPartMode.crBolt24 = RGB( 46,0, 91);
		g_sysPara.crPartMode.crBolt27 = RGB(200,160,88);
		g_sysPara.crPartMode.crBolt30 = RGB(130,20,130);
		g_sysPara.crPartMode.crBolt36 = RGB(233,13,133);
		g_sysPara.crPartMode.crBolt39 = RGB(45,195,195);
		g_sysPara.crPartMode.crBolt42 = RGB(255,160,0);
		g_sysPara.crPartMode.crBolt48 = RGB(180,180,60);
		g_sysPara.crPartMode.crOtherBolt = RGB(128,0,255);
		g_sysPara.crPartMode.crPlate = RGB(0,255,128);
		g_sysPara.crPartMode.crCutFaceOrPolyJoint = RGB(255,128,128);
		if(g_sysPara.crPartMode.crBackground!=RGB(0,64,160))
			g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crBackground);
		if(g_sysPara.crPartMode.crPartBackground!=RGB(150,150,255))
			g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crPartBackground);
		g_sysPara.crPartMode.crBackground = RGB(0,64,160);
		g_sysPara.crPartMode.crPartBackground = RGB(150,150,255);
		//在使用默认颜色方案后及时更新属性框显示的相应构件的颜色
#ifdef AFX_TARG_ENU_ENGLISH
		if(partName.CompareNoCase("Node")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crNode);
		else if(partName.CompareNoCase("Control Node")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crControlNode);
		else if(partName.CompareNoCase("Divide Scale Node")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crDivideScaleNode);
		else if(partName.CompareNoCase("Offset Node")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crOffsetNode);
		else if(partName.CompareNoCase("Axis Val Fix Node")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAxisValFixNode);
		else if(partName.CompareNoCase("Angle(Tube) Single Line")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crLine);
		else if(partName.CompareNoCase("Angle(Tube)")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crPole);
		else if(partName.CompareNoCase("Angle X Leg")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAngleWingX);
		else if(partName.CompareNoCase("Angle Y Leg")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAngleWingY);
		else if(partName.CompareNoCase("Angle Leg")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAngleEdge);
		else if(partName.CompareNoCase("No.1 Child Angle X Leg")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1WingX);
		else if(partName.CompareNoCase("No.1 Child Angle Y Leg")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1WingY);
		else if(partName.CompareNoCase("No.1 Child Angle Leg")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1Edge);
		else if(partName.CompareNoCase("Other Bolts")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crOtherBolt);
		else if(partName.CompareNoCase("Bolt M12")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt12);
		else if(partName.CompareNoCase("Bolt M16")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt16);
		else if(partName.CompareNoCase("Bolt M20")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt20);
		else if(partName.CompareNoCase("Bolt M24")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt24);
		else if(partName.CompareNoCase("Bolt M27")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt27);
		else if(partName.CompareNoCase("Bolt M30")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt30);
		else if(partName.CompareNoCase("Bolt M36")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt36);
		else if(partName.CompareNoCase("Bolt M39")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt39);
		else if(partName.CompareNoCase("Bolt M42")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt42);
		else if(partName.CompareNoCase("Bolt M48")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt48);
		else if(partName.CompareNoCase("Connecting Plate")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crPlate);
		else if(partName.CompareNoCase("Bend Segment/Cut Segment")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crCutFaceOrPolyJoint);
		else if(partName.CompareNoCase("Main Background")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBackground);
		else if(partName.CompareNoCase("Supporting Background")==0)
#else 
		if(partName.CompareNoCase("节点")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crNode);
		else if(partName.CompareNoCase("控制节点")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crControlNode);
		else if(partName.CompareNoCase("等分节点")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crDivideScaleNode);
		else if(partName.CompareNoCase("偏移节点")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crOffsetNode);
		else if(partName.CompareNoCase("指定坐标分量节点")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAxisValFixNode);
		else if(partName.CompareNoCase("角钢(钢管)单线")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crLine);
		else if(partName.CompareNoCase("角钢(钢管)")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crPole);
		else if(partName.CompareNoCase("角钢X肢")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAngleWingX);
		else if(partName.CompareNoCase("角钢Y肢")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAngleWingY);
		else if(partName.CompareNoCase("角钢肢边")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAngleEdge);
		else if(partName.CompareNoCase("1号子角钢X肢")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1WingX);
		else if(partName.CompareNoCase("1号子角钢Y肢")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1WingY);
		else if(partName.CompareNoCase("1号子角钢肢边")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1Edge);
		else if(partName.CompareNoCase("其它螺栓")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crOtherBolt);
		else if(partName.CompareNoCase("螺栓M12")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt12);
		else if(partName.CompareNoCase("螺栓M16")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt16);
		else if(partName.CompareNoCase("螺栓M20")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt20);
		else if(partName.CompareNoCase("螺栓M24")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt24);
		else if(partName.CompareNoCase("螺栓M27")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt27);
		else if(partName.CompareNoCase("螺栓M30")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt30);
		else if(partName.CompareNoCase("螺栓M36")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt36);
		else if(partName.CompareNoCase("螺栓M39")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt39);
		else if(partName.CompareNoCase("螺栓M42")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt42);
		else if(partName.CompareNoCase("螺栓M48")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt48);
		else if(partName.CompareNoCase("连接板")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crPlate);
		else if(partName.CompareNoCase("火曲/切割段")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crCutFaceOrPolyJoint);
		else if(partName.CompareNoCase("主背景")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBackground);
		else if(partName.CompareNoCase("辅背景")==0)
#endif
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crPartBackground);
		pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"),clrStr);
		pView->RedrawAll();	//即时更新右侧显示区颜色
		return TRUE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs"))
	{
		CPartUcsDlg ucs_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
		ucs_dlg.m_sCaption="Model Coordinate System";
#else 
		ucs_dlg.m_sCaption="模型坐标系";
#endif
		ucs_dlg.m_bCanModifyOrigin=FALSE;
		ucs_dlg.ucs=theApp.env.mcs;
		if(ucs_dlg.DoModal()==IDOK)
			theApp.env.mcs=ucs_dlg.ucs;
	}
	/*else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDispPartACS"))
	{
		CPartUcsDlg ucs_dlg;
		ucs_dlg.m_sCaption="构件装配坐标系";
		ucs_dlg.m_bCanModifyOrigin=FALSE;
		ucs_dlg.ucs=theApp.env.ucs;
		if(ucs_dlg.DoModal()==IDOK)
			theApp.env.ucs=ucs_dlg.ucs;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDispWorkUcs"))
	{
		CPartUcsDlg ucs_dlg;
		ucs_dlg.m_sCaption="工作坐标系";
		ucs_dlg.m_bCanModifyOrigin=FALSE;
		ucs_dlg.ucs=theApp.env.ucs;
		if(ucs_dlg.DoModal()==IDOK)
			theApp.env.ucs=ucs_dlg.ucs;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDispBlockLCS"))
	{
		CPartUcsDlg ucs_dlg;
		ucs_dlg.m_sCaption="部件定位坐标系";
		ucs_dlg.m_bCanModifyOrigin=FALSE;
		ucs_dlg.ucs=theApp.env.ucs;
		if(ucs_dlg.DoModal()==IDOK)
			theApp.env.ucs=ucs_dlg.ucs;
	}*/
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs.axis_x"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Datum CS-X Axis";
#else 
		vecDlg.m_sCaption = "基准坐标系-X轴";
#endif
		vecDlg.m_fVectorX = theApp.env.mcs.axis_x.x;
		vecDlg.m_fVectorY = theApp.env.mcs.axis_x.y;
		vecDlg.m_fVectorZ = theApp.env.mcs.axis_x.z;
		pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_x.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_x.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_x.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("env.mcs.axis_x.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("env.mcs.axis_x.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("env.mcs.axis_x.z"),CString(sText));
			theApp.env.mcs.axis_x.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			return TRUE;
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs.axis_y"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Datum CS-Y Axis";
#else 
		vecDlg.m_sCaption = "基准坐标系-Y轴";
#endif
		vecDlg.m_fVectorX = theApp.env.mcs.axis_y.x;
		vecDlg.m_fVectorY = theApp.env.mcs.axis_y.y;
		vecDlg.m_fVectorZ = theApp.env.mcs.axis_y.z;
		pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_y.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_y.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_y.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("env.mcs.axis_y.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("env.mcs.axis_y.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("env.mcs.axis_y.z"),CString(sText));
			theApp.env.mcs.axis_y.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			return TRUE;
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs.axis_z"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Datum CS-Z Axis";
#else 
		vecDlg.m_sCaption = "基准坐标系-Z轴";
#endif
		vecDlg.m_fVectorX = theApp.env.mcs.axis_z.x;
		vecDlg.m_fVectorY = theApp.env.mcs.axis_z.y;
		vecDlg.m_fVectorZ = theApp.env.mcs.axis_z.z;
		pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_z.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_z.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_z.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("env.mcs.axis_z.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("env.mcs.axis_z.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("env.mcs.axis_z.z"),CString(sText));
			theApp.env.mcs.axis_z.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			return TRUE;
		}
	}
	if(pItem->m_idProp==CLDSApp::GetPropID("work_ucs.origin"))
		pView->SpecWcsOrgin();
	else if(pItem->m_idProp==CLDSApp::GetPropID("work_ucs.axis_x"))
		pView->SpecAxisXVertex();
	else if(pItem->m_idProp==CLDSApp::GetPropID("work_ucs.axis_y"))
		pView->SpecAxisYVertex();
#ifdef __SMART_DRAW_INC_
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.intelligentConnect"))
	{	//智能连接设计相关参数
		CIntelligentConnectParaDlg connectParaDlg;
		connectParaDlg.DoModal();
	}
#endif
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgZhunJuTbl"))
	{
		CJgZhunJuLibDlg dlg;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgGuiGeTbl"))
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=1;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sTubeGuiGeTbl"))
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=0;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sFlatGuiGeTbl"))
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=2;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sSlotGuiGeTbl"))
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=3;
		dlg.DoModal();
	}
#ifdef __H_SHAPE_STEEL_
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHWGuiGeTbl"))
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=8;
		dlg.m_iPartSubType=0;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHMGuiGeTbl"))
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=8;
		dlg.m_iPartSubType=1;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHNGuiGeTbl"))
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=8;
		dlg.m_iPartSubType=2;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHPGuiGeTbl"))
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=8;
		dlg.m_iPartSubType=3;
		dlg.DoModal();
	}
#endif
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sLsSpaceTbl"))
	{
		CPartGuiGeLibDlg LsSpaceDlg;
		LsSpaceDlg.m_iPartType=5;
		LsSpaceDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sLsGuiGeTbl"))
	{
		CLsGuigeLibDlg dlg;
		if(dlg.DoModal()==IDOK)
		{
			pPropList->DeleteAllSonItems(pItem);
			for(CLsFamily *pLsFamily=CLsLibrary::GetFirstFamily();pLsFamily;pLsFamily=CLsLibrary::GetNextFamily())//,i++)
			{	//螺栓规格系列
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMON;
				lpInfo->m_strPropName = pLsFamily->name;
				CPropTreeItem* pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp = CLDSApp::MAX_USER_PROPID+pLsFamily->GetFamilyHandle();
				pSonItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			//地脚螺栓库
			oper.InsertButtonPropItem(pItem,"ANCHOR_BOLT_LIB","","",-1,TRUE);
		}
	}
	else if(pItem->m_idProp>CLDSApp::MAX_USER_PROPID)	//螺栓规格系列
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=4;
		long h=pItem->m_idProp-CLDSApp::MAX_USER_PROPID;
		dlg.m_pLsFamily=CLsLibrary::FromHandle(h);
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("ANCHOR_BOLT_LIB"))	//地脚螺栓库
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=9;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sLsSelectTbl"))
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=6;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgJointParaTbl"))
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=7;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sEBGuiGeTbl"))
	{	//EB挂板库
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=10;
		dlg.m_iPartSubType=0;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sGDGuiGeTbl"))
	{	//GD挂板库
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=10;
		dlg.m_iPartSubType=1;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sURGuiGeTbl"))
	{	//U型环库
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=10;
		dlg.m_iPartSubType=2;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.listSelBoltCond"))
	{
		CSteelMatLibDlg dlg;
		dlg.m_iContentsType=2;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sSteelMatTbl"))
	{
		CSteelMatLibDlg dlg;
		if(dlg.DoModal()==IDOK)
		{
			CTowerTreeDlg *pTreeDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
			if(pTreeDlg)
				pTreeDlg->RefreshMatTypeItem();
		}
	}
#if defined(__LDS_)||defined(__TSA_)
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sInterpolateBetaZTbl"))
	{
		CSteelMatLibDlg dlg;
		dlg.m_iContentsType=1;	//显示风振系数插值表
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sInterpIceLoadCoefTbl"))
	{
		CSteelMatLibDlg dlg;
		dlg.m_iContentsType=3;	//显示杆塔覆冰重力荷载系数表
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sWireGuiGeTbl"))
	{
		CWireGuiGeTblDlg dlg;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("WeatherCondition"))
	{
		CWeatherRegionDlg dlg;
		//strcpy(dlg.weatherRegion.weatherCode,Ta.weatherCode);
		dlg.weatherRegion=Ta.weather;
		/*dlg.weatherRegion.m_nLowestT=Ta.m_nLowestT;
		dlg.weatherRegion.m_nAverageT=Ta.m_nAverageT;
		dlg.weatherRegion.m_fMaxWindV=Ta.m_fMaxWindV;
		dlg.weatherRegion.m_nMaxWindT=Ta.m_nMaxWindT;
		dlg.weatherRegion.m_fMaxIceThick=Ta.m_fMaxIceThick;
		dlg.weatherRegion.m_fMaxIceWindV=Ta.m_fMaxIceWindV;
		dlg.weatherRegion.m_nMaxIceT=Ta.m_nMaxIceT;
		dlg.weatherRegion.m_nSetupT=Ta.m_nSetupT;
		dlg.weatherRegion.m_fSetupWindV=Ta.m_fSetupWindV;*/
		if(dlg.DoModal()==IDOK)
		{
			Ta.weather=dlg.weatherRegion;
			/*strcpy(Ta.weatherCode,dlg.weatherRegion.weatherCode);
			Ta.m_nHighestT=dlg.weatherRegion.m_nHighestT;
			Ta.m_nLowestT=dlg.weatherRegion.m_nLowestT;
			Ta.m_nAverageT=dlg.weatherRegion.m_nAverageT;
			Ta.m_fMaxWindV=dlg.weatherRegion.m_fMaxWindV;
			Ta.m_nMaxWindT=dlg.weatherRegion.m_nMaxWindT;
			Ta.m_fMaxIceThick=dlg.weatherRegion.m_fMaxIceThick;
			Ta.m_fMaxIceWindV=dlg.weatherRegion.m_fMaxIceWindV;
			Ta.m_nMaxIceT=dlg.weatherRegion.m_nMaxIceT;
			Ta.m_nSetupT=dlg.weatherRegion.m_nSetupT;
			Ta.m_fSetupWindV=dlg.weatherRegion.m_fSetupWindV;*/
			CString temStr;
			temStr=Ta.weatherCode;
			pPropList->SetItemPropValue(CLDSApp::GetPropID("WeatherCondition"),temStr);
			temStr.Format("%d",Ta.weather.m_nHighestT);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("m_nHighestTemperature"),temStr);
			temStr.Format("%d",Ta.weather.m_nLowestT);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("m_nLowestTemperature"),temStr);
			temStr.Format("%d",Ta.weather.m_nAverageT);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("m_nAverageTemperature"),temStr);
			temStr.Format("%f",Ta.weather.m_fMaxWindV);
			SimplifiedNumString(temStr);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("m_fMaxWindVelocity"),temStr);
			temStr.Format("%d",Ta.weather.m_nMaxWindT);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("m_nMaxWindVelocityTemperature"),temStr);
			temStr.Format("%f",Ta.weather.m_fMaxIceThick);
			SimplifiedNumString(temStr);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("m_fMaxIceThick"),temStr);
			temStr.Format("%f",Ta.weather.m_fMaxIceWindV);
			SimplifiedNumString(temStr);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("m_fMaxIceWindVelocity"),temStr);
			temStr.Format("%d",Ta.weather.m_nMaxIceT);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("m_nMaxIceTemperature"),temStr);
			temStr.Format("%f",Ta.weather.m_fSetupWindV);
			SimplifiedNumString(temStr);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("m_fSetupWindVelocity"),temStr);
			temStr.Format("%d",Ta.weather.m_nSetupT);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("m_nSetupTemperature"),temStr);
		}
	}
#endif
#ifndef __TSA_
	else if(pItem->m_idProp==CLDSApp::GetPropID("StdPartLib"))
	{	//标准构件库
		CTubeEndLjStdPartLibDlg stdPartDlg;
		stdPartDlg.DoModal();
	}
	//暂时保留旧的插板及法兰库的录入界面 wht 10-07-06
	else if(pItem->m_idProp==CLDSApp::GetPropID("InsertPlateLib"))
	{	//钢管管头插板库
		CTubeEndLjGuiGeTblDlg dlg;
		dlg.DoModal();
	}
#endif
	else if(pItem->m_idProp==CLDSApp::GetPropID("SyncProp.Node"))
	{
		CObjectSyncPropDetailsDlg dlg;
		dlg.m_pMPCL=&CLDSNode::MPCL;
		dlg.m_pSyncPropList=&CLDSNode::propMirSync;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("SyncProp.Angle"))
	{
		CObjectSyncPropDetailsDlg dlg;
		dlg.m_pMPCL=&CLDSLineAngle::MPCL;
		dlg.m_pSyncPropList=&CLDSLineAngle::propMirSync;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("SyncProp.Plate"))
	{
		CObjectSyncPropDetailsDlg dlg;
		dlg.m_pMPCL=&CLDSPlate::MPCL;
		dlg.m_pSyncPropList=&CLDSPlate::propMirSync;
		dlg.DoModal();
	}
#ifdef __COMPLEX_PART_INC_
	else if(pItem->m_idProp==CLDSApp::GetPropID("SyncProp.Tube"))
	{
		CObjectSyncPropDetailsDlg dlg;
		dlg.m_pMPCL=&CLDSLineTube::MPCL;
		dlg.m_pSyncPropList=&CLDSLineTube::propMirSync;
		dlg.DoModal();
	}
#endif
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sAcadPath"))
			Filter = "Executable File(*.exe)|*.exe||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgZhunJuTblPath"))
			Filter = "Angle Stadia File(*.jgg)|*.jgg||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgGuiGeTblPath"))
			Filter = "Angle Specification File(*.jgt)|*.jgt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgJointParaTblPath"))
			Filter = "Angle Joint File(*.jjp)|*.jjp||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sTubeGuiGeTblPath"))
			Filter = "Tube Specification File(*.pgt)|*.pgt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sFlatGuiGeTblPath"))
			Filter = "Flat Iron Specification File(*.bia)|*.bia||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sSlotGuiGeTblPath"))
			Filter = "U-steel Specification File(*.cgt)|*.cgt||";
#ifdef __H_SHAPE_STEEL_
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHWGuiGeTblPath"))
			Filter = "HW Specification File(*.hwgt)|*.hwgt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHMGuiGeTblPath"))
			Filter = "HM Specification File(*.hmgt)|*.hmgt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHNGuiGeTblPath"))
			Filter = "HN Specification File(*.hngt)|*.hngt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHPGuiGeTblPath"))
			Filter = "HP Specification File(*.hpgt)|*.hpgt||";
#endif
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sLsSpaceTblPath"))
			Filter = "Bolt Interval File(*.lss)|*.lss||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sCInsertPlateLibPath"))
			Filter = "Trough Board File (*.cipl)|*.cipl||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sUInsertPlateLibPath"))
			Filter = "U Board File(*.uipl)|*.uipl||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sXInsertPlateLibPath"))
			Filter = "Cross Board File(*.xipl)|*.xipl||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlDLibPath"))
			Filter = "Weld Neck Flange Specification File(*.fld)|*.fld||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlPLibPath"))
			Filter = "Flat Weld Flange Specification File(*.flp)|*.flp||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlGLibPath"))
			Filter = "Rigid Flange Specification File(*.flg)|*.flg||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlRLibPath"))
			Filter = "flexible flange Specification File(*.flr)|*.flr||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sFlCoupleLevelPath"))
			Filter = "Bott Joint Flange Define File(*.fll)|*.fll||";
#else 
		if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sAcadPath"))
			Filter = "可执行文件(*.exe)|*.exe||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgZhunJuTblPath"))
			Filter = "角钢准距文件(*.jgg)|*.jgg||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgGuiGeTblPath"))
			Filter = "角钢规格文件(*.jgt)|*.jgt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgJointParaTblPath"))
			Filter = "角钢接头库文件(*.jjp)|*.jjp||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sTubeGuiGeTblPath"))
			Filter = "钢管规格文件(*.pgt)|*.pgt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sFlatGuiGeTblPath"))
			Filter = "扁铁规格文件(*.bia)|*.bia||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sSlotGuiGeTblPath"))
			Filter = "槽钢规格文件(*.cgt)|*.cgt||";
#ifdef __H_SHAPE_STEEL_
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHWGuiGeTblPath"))
			Filter = "HW型钢规格文件(*.hwgt)|*.hwgt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHMGuiGeTblPath"))
			Filter = "HM型钢规格文件(*.hmgt)|*.hmgt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHNGuiGeTblPath"))
			Filter = "HN型钢规格文件(*.hngt)|*.hngt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHPGuiGeTblPath"))
			Filter = "HP型钢规格文件(*.hpgt)|*.hpgt||";
#endif
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sLsSpaceTblPath"))
			Filter = "螺栓间距文件(*.lss)|*.lss||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sCInsertPlateLibPath"))
			Filter = "槽型插板库文件(*.cipl)|*.cipl||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sUInsertPlateLibPath"))
			Filter = "U型插板库文件(*.uipl)|*.uipl||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sXInsertPlateLibPath"))
			Filter = "十字型插板库文件(*.xipl)|*.xipl||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlDLibPath"))
			Filter = "对焊法兰规格库文件(*.fld)|*.fld||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlPLibPath"))
			Filter = "平焊法兰规格库文件(*.flp)|*.flp||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlGLibPath"))
			Filter = "刚性法兰规格库文件(*.flg)|*.flg||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlRLibPath"))
			Filter = "柔性法兰规格库文件(*.flr)|*.flr||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sFlCoupleLevelPath"))
			Filter = "对接法兰级别定义文件(*.fll)|*.fll||";
#endif
		if(!Filter.IsEmpty())
		{
			CFileDialog FileDlg(TRUE, NULL, NULL, NULL,	Filter);
			CString currPath = pItem->m_lpNodeInfo->m_strPropValue;
			FileDlg.m_ofn.lpstrTitle = "Select file";
			if (currPath.GetLength() > 0)
				FileDlg.m_ofn.lpstrInitialDir = currPath.Left(
				currPath.GetLength() - currPath.ReverseFind('\\'));
			if(IDOK!=FileDlg.DoModal())
				return FALSE;
			SelectedFile = FileDlg.GetPathName();
			if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sAcadPath"))
				theApp.WriteProfileString("ACAD","ACAD_PATH",SelectedFile);
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgZhunJuTblPath"))
				theApp.WriteProfileString("Settings","JgZhunJuTblPath",SelectedFile);
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgGuiGeTblPath"))
			{
				theApp.WriteProfileString("Settings","JgGuiGeTblPath",SelectedFile);
				JgGuiGeSerialize(SelectedFile,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sTubeGuiGeTblPath"))
			{
				theApp.WriteProfileString("Settings","TubeGuiGeTblPath",SelectedFile);
				TubeGuiGeSerialize(SelectedFile,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sFlatGuiGeTblPath"))
			{
				theApp.WriteProfileString("Settings","FlatGuiGeTblPath",SelectedFile);
				FlatGuiGeSerialize(SelectedFile,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgJointParaTblPath"))
			{
				theApp.WriteProfileString("Settings","JgJointParaTblPath",SelectedFile);
				JgJointParaSerialize(SelectedFile,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sSlotGuiGeTblPath"))
			{
				theApp.WriteProfileString("Settings","SlotGuiGeTblPath",SelectedFile);
				SlotGuiGeSerialize(SelectedFile,FALSE);
			}
#ifdef __H_SHAPE_STEEL_
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHWGuiGeTblPath"))
			{
				theApp.WriteProfileString("Settings","HWGuiGeTblPath",SelectedFile);
				HShapeGuiGeSerialize(SelectedFile,0,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHMGuiGeTblPath"))
			{
				theApp.WriteProfileString("Settings","HMGuiGeTblPath",SelectedFile);
				HShapeGuiGeSerialize(SelectedFile,1,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHNGuiGeTblPath"))
			{
				theApp.WriteProfileString("Settings","HNGuiGeTblPath",SelectedFile);
				HShapeGuiGeSerialize(SelectedFile,2,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHPGuiGeTblPath"))
			{
				theApp.WriteProfileString("Settings","HPGuiGeTblPath",SelectedFile);
				HShapeGuiGeSerialize(SelectedFile,3,FALSE);
			}
#endif
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sLsSpaceTblPath"))
			{
				theApp.WriteProfileString("Settings","LsSpaceTblPath",SelectedFile);
				LsSpaceSerialize(SelectedFile,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sCInsertPlateLibPath"))
			{
				theApp.WriteProfileString("Settings","CInsertPlateLibPath",SelectedFile);
				CInsertPlateSerialize(SelectedFile,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sUInsertPlateLibPath"))
			{
				theApp.WriteProfileString("Settings","UInsertPlateLibPath",SelectedFile);
				UInsertPlateSerialize(SelectedFile,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sXInsertPlateLibPath"))
			{
				theApp.WriteProfileString("Settings","XInsertPlateLibPath",SelectedFile);
				XInsertPlateSerialize(SelectedFile,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlDLibPath"))
			{
				theApp.WriteProfileString("Settings","StdPartFlDLibPath",SelectedFile);
				FlDSerialize(SelectedFile,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlPLibPath"))
			{
				theApp.WriteProfileString("Settings","StdPartFlPLibPath",SelectedFile);
				FlPSerialize(SelectedFile,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlGLibPath"))
			{
				theApp.WriteProfileString("Settings","StdPartFlGLibPath",SelectedFile);
				FlGSerialize(SelectedFile,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlRLibPath"))
			{
				theApp.WriteProfileString("Settings","StdPartFlRLibPath",SelectedFile);
				FlRSerialize(SelectedFile,FALSE);
			}
			else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sFlCoupleLevelPath"))
			{
				theApp.WriteProfileString("Settings","FlCoupleLevelPath",SelectedFile);
				FlCoupleLevelSerialize(SelectedFile,FALSE);
			}
			pItem->m_lpNodeInfo->m_strPropValue = SelectedFile;
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL SyssettingCheckBoxClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem=NULL, *pParentItem=NULL;
	BOOL bCheck=pItem->m_lpNodeInfo->GetCheck();
	if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectNode"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_NODE);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_NODE)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_NODE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectLineAngle"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINEANGLE);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINEANGLE)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_LINEANGLE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectLineTube"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINETUBE);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINETUBE)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_LINETUBE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectPlate"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_PLATE);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_PLATE)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_PLATE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectParamPlate"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_PARAMPLATE);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_PARAMPLATE)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_PARAMPLATE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectBolt"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_BOLT);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_BOLT)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_BOLT);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectSphere"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_SPHERE);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_SPHERE)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_SPHERE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectLineSlot"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINESLOT);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINESLOT)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_LINESLOT);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectLineFlat"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINEFLAT);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINEFLAT)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_LINEFLAT);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectArcAngle"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_ARCANGLE);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_ARCANGLE)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_ARCANGLE);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectArcFlat"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_ARCFLAT);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_ARCFLAT)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_ARCFLAT);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectArcSlot"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_ARCSLOT);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_ARCSLOT)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_ARCSLOT);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectStdPart"))
	{
		if(bCheck)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_STDPART);
		else if(!bCheck&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_STDPART)))
			g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_STDPART);
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CanSelectEdgeLine"))
		g_sysPara.m_bCanSelectEdgeLine = bCheck;
	//格式刷属性选择
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bMaterial"))
		g_sysPara.brush.bMaterial = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bLayer"))
		g_sysPara.brush.bLayer = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bGuige"))
		g_sysPara.brush.bGuige = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bSeg"))
		g_sysPara.brush.bSeg = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bCfgword"))
		g_sysPara.brush.bCfgword = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bDisplayHole"))
		g_sysPara.brush.bDisplayHole = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bVirtualPart"))
		g_sysPara.brush.bVirtualPart = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bColour"))
		g_sysPara.brush.bColour = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bPartSketch"))
		g_sysPara.display.bPartSketch = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bNodePnt"))
	{
		g_sysPara.display.bNodePnt = bCheck;
		int idProp=CLDSApp::GetPropID("g_sysPara.display.xPointStyle.cbPointSize");
		if(g_sysPara.display.bNodePnt)
		{
			pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pFindItem==NULL)
			{
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Node Display Size";
				lpInfo->m_strPropHelp = "Display the node's size";
#else 
				lpInfo->m_strPropName = "节点显示大小";
				lpInfo->m_strPropHelp = "节点显示大小";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
				pFindItem->m_idProp=idProp;
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;

				idProp=CLDSApp::GetPropID("g_sysPara.display.xPointStyle.ciPointStyle");
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Node Display Style";
				lpInfo->m_strPropHelp = "Display the node's style";
				lpInfo->m_cmbItems = "0.Square Point|1.Circular Point";
#else 
				lpInfo->m_strPropName = "节点显示类型";
				lpInfo->m_strPropHelp = "节点显示类型";
				lpInfo->m_cmbItems = "0.方点|1.圆点";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
				pFindItem->m_idProp=idProp;
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			idProp=CLDSApp::GetPropID("g_sysPara.display.xPointStyle.cbPointSize");
			pFindItem = pPropList->FindItemByPropId(idProp,NULL);
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

			idProp=CLDSApp::GetPropID("g_sysPara.display.xPointStyle.ciPointStyle");
			pFindItem = pPropList->FindItemByPropId(idProp,NULL);
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
		}
		else 
		{
			idProp=CLDSApp::GetPropID("g_sysPara.display.xPointStyle.cbPointSize");
			pPropList->DeleteItemByPropId(idProp);
			idProp=CLDSApp::GetPropID("g_sysPara.display.xPointStyle.ciPointStyle");
			pPropList->DeleteItemByPropId(idProp);
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bPoleLine"))
		g_sysPara.display.bPoleLine = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("CLDSLinePart::bDisplayLineVecMark"))
		CLDSLinePart::bDisplayLineVecMark = bCheck;
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.bWirePoint"))
		g_sysPara.display.bWirePoint=bCheck;	//挂点箭头
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.bLoadArrow"))
	{	//荷载箭头
		g_sysPara.display.bLoadArrow=bCheck;
		int idProp=CLDSApp::GetPropID("g_sysPara.display.nLoadArrowScale");
		if(g_sysPara.display.bLoadArrow)
		{
			pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pFindItem==NULL)
			{
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Load Arrow Size";
				lpInfo->m_strPropHelp = "The load arrow's size";
#else 
				lpInfo->m_strPropName = "荷载箭头大小";
				lpInfo->m_strPropHelp = "荷载箭头大小";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
				pFindItem->m_idProp=idProp;
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
		}
		else 
			pPropList->DeleteItemByPropId(idProp);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bDispVirtualPart"))
		g_sysPara.display.bDispVirtualPart = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidJg"))
		g_sysPara.display.bSolidJg = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidTube"))
		g_sysPara.display.bSolidTube = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidLjb"))
		g_sysPara.display.bSolidLjb = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidJoint"))
		g_sysPara.display.bSolidJoint = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidLs"))
		g_sysPara.display.bSolidLs = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidSlot"))
		g_sysPara.display.bSolidSlot = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidSphere"))
		g_sysPara.display.bSolidSphere = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidFlat"))
		g_sysPara.display.bSolidFlat = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidArcAngle"))
		g_sysPara.display.bSolidArcAngle = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidArcFlat"))
		g_sysPara.display.bSolidArcFlat = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidArcSlot"))
		g_sysPara.display.bSolidArcSlot = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidStdPart"))
		g_sysPara.display.bSolidStdPart = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidFoundation"))
		g_sysPara.display.bSolidFoundation = bCheck;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidTransectFace"))
		CLDSLineTube::bDisplaySolidTransectFace=bCheck;	//g_sysPara.display.bSolidTransectFace = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("CLDSLineAngle::m_bDisplaySolidAxisLine"))
		CLDSLineAngle::m_bDisplaySolidAxisLine=bCheck; //显示角钢心线
#endif
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.display.bSolidLine"))
	{
		g_sysPara.display.bSolidLine = bCheck;
		g_pSolidSet->SetSolidAndLineStatus(g_sysPara.display.bSolidLine);
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs"))
	{	
		theApp.env.m_bDisplayWorldUcs = bCheck;
		g_pSolidSet->SetDispObjectUcs(theApp.env.m_bDisplayWorldUcs);
		pView->Invalidate(FALSE);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDispPartACS"))
		g_sysPara.m_bDispPartACS = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDispWorkUcs"))
	{	
		g_sysPara.m_bDispWorkUcs = bCheck;
		//g_pSolidSet->SetDispWorkUcs(g_sysPara.m_bDispWorkUcs);
		pView->Invalidate(FALSE);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDispBlockLCS"))
		g_sysPara.m_bDispBlockLCS = bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDisplayConnectDlg"))
		g_sysPara.m_bDisplayConnectDlg=bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.b1BoltDesignPrompt"))
		g_sysPara.b1BoltDesignPrompt=bCheck;
	//设计
#if defined(__TSA_)||defined(__LDS_)
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bMergeRptFileMirObj"))
		g_sysPara.bMergeRptFileMirObj=bCheck;
	//else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bCalStdFoundationForce"))
	//	g_sysPara.bCalStdFoundationForce=bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bUpdateRodDesignForce"))
		g_sysPara.bCalStdFoundationForce=bCheck;
	//else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bCreateAnalFile"))
	//	g_sysPara.bCreateAnalFile=bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bRecNodeOffset"))
		g_sysPara.bRecNodeOffset=bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bCreateAnsysCmdFile"))
		g_sysPara.bCreateAnsysCmdFile=bCheck;
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.bCreateDetailRptFile"))
	{
		g_sysPara.bCreateDetailRptFile=bCheck;
		if(g_sysPara.bCreateDetailRptFile)
		{
			pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iDetailDocType"),&pParentItem);
			if(pFindItem==NULL)
			{	//详细报告输出文档类型
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "document type";
				lpInfo->m_strPropHelp = "document type";
				lpInfo->m_cmbItems = "0.Text Document|1.EXCEL Form"
#else
				lpInfo->m_strPropName = "文档类型";
				lpInfo->m_strPropHelp = "文档类型";
				lpInfo->m_cmbItems = "0.文本文档|1.EXCEL表格";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iDetailDocType");
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

			pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iLanguage"),&pParentItem);
			if(pFindItem==NULL)
			{	//详细报告输出文档类型
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "output language";
				lpInfo->m_strPropHelp = "Output language of computer";
				lpInfo->m_cmbItems = "0.Chinese|1.English";
#else
				lpInfo->m_strPropName = "输出语言";
				lpInfo->m_strPropHelp = "计算书输出语言";
				lpInfo->m_cmbItems = "0.中文|1.英文";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iLanguage");
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

			pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.bMergeRptFileMirObj"),&pParentItem);
			if(pFindItem==NULL)
			{	//详细报告输出文档类型
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
    				lpInfo->m_strPropName = "Merge symmetric items";
				lpInfo->m_strPropHelp = "Merge symmetric nodes and the output information of rods";
				lpInfo->m_cmbItems = "Yes|No";
#else
				lpInfo->m_strPropName = "合并对称项";
				lpInfo->m_strPropHelp = "合并对称节点和杆件的信息输出";
				lpInfo->m_cmbItems = "是|否";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.bMergeRptFileMirObj");
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

			pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iEfficacyCeofType"),&pParentItem);
			if (pFindItem==NULL)
			{
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
    				lpInfo->m_strPropName = "Type of margin coefficient";
				lpInfo->m_strPropHelp = "Bearing capacity factor is margin coefficient only relative to load,but the design margin,but the design margin includes limited margin of slenderness ratio.";
				lpInfo->m_cmbItems = "0.Design margin|1.Bearing capacity factor";
#else
				lpInfo->m_strPropName = "裕度系数类型";
				lpInfo->m_strPropHelp = "承载系数是只与荷载有关的裕度系数，而设计裕度中则包含了长细比限制裕度。";
				lpInfo->m_cmbItems = "0.承载+长细比系数|1.承载系数";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iEfficacyCeofType");
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

			CPropTreeItem *pSonPropItem;
			CItemInfo *lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "output of bending moment item";
#else
			lpInfo->m_strPropName = "弯矩项输出";
#endif
			pSonPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.putOutM");
			pSonPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
			pSonPropItem->m_bHideChildren=TRUE;
			if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pSonPropItem->m_idProp);

			pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.nM"),&pParentItem);
			if (pFindItem==NULL)
			{
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
    				lpInfo->m_strPropName = "M threshold value";
				lpInfo->m_strPropHelp = "absolute value of bending moment";
#else
				lpInfo->m_strPropName = "M阈值";
				lpInfo->m_strPropHelp = "弯矩绝对值";
#endif
				pFindItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nM");
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

			pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.nSigemaM"),&pParentItem);
			if(pFindItem==NULL)
			{
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
    				lpInfo->m_strPropName = "σM threshold value";
				lpInfo->m_strPropHelp = "bending stress from bending moment";
#else
				lpInfo->m_strPropName = "σM阈值";
				lpInfo->m_strPropHelp = "由弯矩产生的弯曲应力";
#endif
				pFindItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nSigemaM");
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

			pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.nPrecentSigemaM"),&pParentItem);
			if(pFindItem==NULL)
			{
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
    			        lpInfo->m_strPropName = "σM % threshold value";
				lpInfo->m_strPropHelp = "percentage for the bending stress from bending moment in stress produced by axial force";
#else
				lpInfo->m_strPropName = "σM %阈值";
				lpInfo->m_strPropHelp = "由弯矩产生的弯曲应力占轴力产生应力的百分比";
#endif
				pFindItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nPrecentSigemaM");
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

			pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.reactionExportType"),&pParentItem);
			if(pFindItem==NULL)
			{	//详细报告输出文档类型
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
    				lpInfo->m_strPropName = "basic forces filtrating";
				lpInfo->m_strPropHelp = "When filtrating the level of high-low legs，only output the maximum pull-up force and down force from a module;when filtrating in a working condition level,only output all the pull-up force and down force from a moudle.";
				lpInfo->m_cmbItems = "0.Full output|1.High-low legs level|2.Working condition level";
#else
				lpInfo->m_strPropName = "基础作用力筛选";
				lpInfo->m_strPropHelp = "高低腿级筛选时，只输出一个呼高组工况中所有高低腿中的最大上拨力和下压力;工况级筛选时只输出一个呼高组中所有工况的上拨力和下压力。";
				lpInfo->m_cmbItems = "0.全部输出|1.高低腿级|2.工况级";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.reactionExportType");
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.iDetailDocType"));
			pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.iLanguage"));
			pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.bMergeRptFileMirObj"));
			pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.putOutM"));
			pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.iEfficacyCeofType"));	
			pPropList->DeleteItemByPropId(CLDSApp::GetPropID("g_sysPara.reactionExportType"));
		}
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("Ta.SelMatPara.m_bPreferSingleAngle"))
		Ta.SelMatPara.m_bPreferSingleAngle = bCheck;
#endif
	else
		return FALSE;
	return TRUE;
}
void BatchSaveAsFile(UINT uFileSaveVersion)
{
	CString sFolder("D:\\");
	if(!InvokeFolderPickerDlg(sFolder))
		return;
	CXhChar200 sFolderDest("%s(V4)",sFolder);
	MakeDirectory(sFolderDest);
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSDoc* pDoc=pView->GetDocument();
	if(pDoc==NULL)
		return;
	CXhChar200 sFilename("%s\\*.tma",sFolder),sFileName2;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile = FindFirstFile(sFilename, &FindFileData);
	if(hFindFile==INVALID_HANDLE_VALUE)
		return;
	CBuffer buffer;
	do{
		sFilename.Printf("%s\\%s",sFolder, FindFileData.cFileName);
		sFileName2.Printf("%s\\%s",(char*)sFolderDest, FindFileData.cFileName);
		pDoc->OnOpenDocument(sFilename);
		g_sysPara.uFileSaveVersion=uFileSaveVersion;
		pDoc->OnSaveDocument(sFileName2);
	}while(FindNextFile(hFindFile,&FindFileData));
	FindClose(hFindFile);
}
BOOL FireSyssettingPopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLsFamily* pLsFamily=NULL;
	if(pItem->m_idProp==CLDSApp::GetPropID("BatchSaveAsOldFile"))
	{
		sMenuName.Replace("另存为","");
		CLDSDoc* pDoc=pView->GetDocument();
		if(toupper(sMenuName[0])=='V')
		{
			UINT uFileSaveVersion=FromStringVersion(sMenuName.Mid(2));
			BatchSaveAsFile(uFileSaveVersion);
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.uFileSaveVersion"))
	{
		sMenuName.Replace("另存为","");
		CLDSDoc* pDoc=pView->GetDocument();
		if(toupper(sMenuName[0])=='V')
		{
			UINT uFileSaveVersion=FromStringVersion(sMenuName.Mid(2));
			CXhChar200 filename=pDoc->GetTitle();
			char* separator=SearchChar(filename,'.',true);
			if(separator)
				*separator=0;
			//BatchSaveAsFile(uFileSaveVersion);
			//return FALSE;
#ifdef __TMA_FILE_
			CFileDialog dlg(FALSE,"tma",filename,
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
				"Tower Manufacturing Assistant(*.tma)|*.tma|all files(*.*)|*.*||");
#else 
				"铁塔制造助手(*.tma)|*.tma|所有文件(*.*)|*.*||");
#endif
#elif defined(__LMA_FILE_)
			CFileDialog dlg(FALSE,"lma",filename,
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				"格构铁塔制造助手(*.lma)|*.lma|所有文件(*.*)|*.*||");
			//"格构铁塔制造助手(*.lma)|*.lma|铁塔部件段文件(*.sgt)|*.sgt|所有文件(*.*)|*.*||");
#elif defined(__TDA_FILE_)
			CFileDialog dlg(FALSE,"tda",filename,
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				"铁塔绘图助手(*.tda)|*.tda|所有文件(*.*)|*.*||");
#elif defined(__LDS_FILE_)
			CFileDialog dlg(FALSE,"lds",filename,
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				"铁塔设计绘图一体化软件(*.lds)|*.lds|所有文件(*.*)|*.*||");
#else
			return FALSE;
#endif
			if(dlg.DoModal()!=IDOK)
				return FALSE;
			g_sysPara.uFileSaveVersion=uFileSaveVersion;
			pDoc->OnSaveDocument(dlg.GetPathName());
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.iFileSaveDocType"))
	{
		sMenuName.Replace("另存为","");
		CLDSDoc* pDoc=pView->GetDocument();
		if(toupper(sMenuName[0])=='*')
		{
			CXhChar200 filter,filename=pDoc->GetTitle();
			char* separator=SearchChar(filename,'.',true);
			if(separator)
				*separator=0;
			int doc_type=0;
			CXhChar16 extension=sMenuName.TrimLeft("*.");
			if(extension.Equal("tma"))
			{
				doc_type=1;
				//g_sysPara.uFileSaveVersion=5010200;
				filter="铁塔制造助手(*.tma)|*.tma|所有文件(*.*)|*.*||";
			}
			else if(extension.Equal("lma"))
			{
				doc_type=2;
				filter="格构铁塔制造助手(*.lma)|*.lma|所有文件(*.*)|*.*||";
			}
			else if(extension.Equal("tda"))
			{
				doc_type=5;
				filter="铁塔绘图助手(*.tda)|*.tda|所有文件(*.*)|*.*||";
			}
			else
				return FALSE;

			CFileDialog dlg(FALSE,extension,filename,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
			if(dlg.DoModal()!=IDOK)
				return FALSE;
			g_sysPara.iFileSaveDocType=doc_type;
			pDoc->OnSaveDocument(dlg.GetPathName());
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.ciLinkAngleDlgFocusCtrlMode"))
	{
		if(iMenu==0)//sMenuName.Compare("自动设定")==0)
		{
			g_sysPara.ciLinkAngleDlgFocusCtrlMode=0;
			pItem->m_lpNodeInfo->m_bMutiObjsProp=true;
			pPropList->SetItemPropValue(pItem->m_idProp,theApp.GetPropValueStr(pItem->m_idProp));
		}
		else if (iMenu==1)
		{	//指定规格框优先
			g_sysPara.ciLinkAngleDlgFocusCtrlMode=1;
			pPropList->SetItemPropValue(pItem->m_idProp,theApp.GetPropValueStr(pItem->m_idProp));
			pItem->m_lpNodeInfo->m_bMutiObjsProp=false;
		}
		else //if (iMenu==2)
		{	//指定编号框优先
			g_sysPara.ciLinkAngleDlgFocusCtrlMode=2;
			pPropList->SetItemPropValue(pItem->m_idProp,theApp.GetPropValueStr(pItem->m_idProp));
			pItem->m_lpNodeInfo->m_bMutiObjsProp=false;
		}
		pItem->m_lpNodeInfo->SetCheck(iMenu+1);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_fNamedHeightZeroZ"))
	{
		if(iMenu==0)//sMenuName.Compare("智能计算")==0)
		{
			Ta.m_bUserSpecifyNamedHeightZeroZ=false;
			pItem->m_lpNodeInfo->m_bMutiObjsProp=true;
			CXhChar16 szNamedHeightZeroZ(Ta.NamedHeightZeroZ);
			pPropList->SetItemPropValue(pItem->m_idProp,szNamedHeightZeroZ);
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.segstr"))
	{
/*
#ifdef AFX_TARG_ENU_ENGLISH
		sMenuName.CompareNoCase("Exclude Segments");
#else
		sMenuName.CompareNoCase("排除段号");
#endif
*/
		CString segExcludeStr;
		if(Input("输入要排除的段号范围",&segExcludeStr,'s')>0)
		{
			CHashList<SEGI> hashExcludeSegs;
			GetSegNoHashTblBySegStr(segExcludeStr,hashExcludeSegs);
			for(CLDSNode* pNode=console.DispNodeSet.GetFirst();pNode;pNode=console.DispNodeSet.GetNext())
			{
				DWORD dwCurSegI=pNode->iSeg>0?pNode->iSeg:-1;
				if(hashExcludeSegs.GetValue(dwCurSegI))
					pNode->is_visible=false;
			}
			for(CLDSPart* pPart=console.DispPartSet.GetFirst();pPart;pPart=console.DispPartSet.GetNext())
			{
				DWORD dwCurSegI=pPart->iSeg>0?pPart->iSeg:-1;
				if(hashExcludeSegs.GetValue(dwCurSegI))
					pPart->is_visible=false;
			}
			for(CBlockReference* pAssmBlk=console.DispBlockRefSet.GetFirst();pAssmBlk;pAssmBlk=console.DispBlockRefSet.GetNext())
			{
				DWORD dwCurSegI=pAssmBlk->iSeg>0?pAssmBlk->iSeg:-1;
				if(hashExcludeSegs.GetValue(dwCurSegI))
					pAssmBlk->is_visible=false;
			}
			g_pSolidDraw->BuildDisplayList(pView);	//只刷新当前允许显示的数据对象
		}
	}
	else if(pItem->m_idProp>CLDSApp::MAX_USER_PROPID && (pLsFamily=CLsLibrary::FromHandle(pItem->m_idProp-CLDSApp::MAX_USER_PROPID)))
	{
		CLsFamily xLocalLsFamily;
		if(iMenu!=0 && GetLocalLsFamily(pLsFamily->Id,sMenuName.GetBuffer(),&xLocalLsFamily))
		{	//选择本地螺栓规格文件
			pItem->m_lpNodeInfo->SetCheck(iMenu+1);
			if(pLsFamily->m_uiCodeSerial!=xLocalLsFamily.m_uiCodeSerial )
				pLsFamily->CopyLsFamily(&xLocalLsFamily);	//同一规格系列内如果规范序号相同，则认为是同一螺栓规范
		}
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=4;
		dlg.m_pLsFamily=pLsFamily;
		dlg.DoModal();
	}
	return FALSE;
}
CString MakeLocalLsSeriesString(long hLsFamily)
{
	CString sLsSeriesStr="编辑当前规范|-";
	int nNum=GetLocalBoltSeries(hLsFamily,NULL);
	if(nNum>0)
	{
		ARRAY_LIST<LOCAL_BOLT_CODESERIES> localLsSeriesArr;
		localLsSeriesArr.SetSize(nNum);
		GetLocalBoltSeries(hLsFamily,localLsSeriesArr.m_pData);
		for(int i=0;i<nNum;i++)
		{
			sLsSeriesStr+='|';
			sLsSeriesStr+=localLsSeriesArr[i].m_sCodeName;
		}
	}
	return sLsSeriesStr;
}
void CLDSView::DisplaySysSettingProperty(BOOL bRebuild)
{
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	if(pPropDlg==NULL)
		return;
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem* pRootItem=pPropList->GetRootItem();
	CPropTreeItem* pGroupItem, *pPropItem,*pSonPropItem,*pParentItem;
	CPropertyListOper<CLDSApp> oper(pPropList,&theApp);
#ifdef __TSA_
	const int GROUP_GENERAL=1,GROUP_DISPLAY=2,GROUP_ANALYSIS=3;
#elif defined(__LDS_)
	const int GROUP_GENERAL=1,GROUP_DISPLAY=2,GROUP_ANALYSIS=3;
	int GROUP_DRAWING=4;
	if(theApp.m_bDrawingWorkEnvOnly)
		GROUP_DRAWING=3;
#else 
	const int GROUP_GENERAL=1,GROUP_DISPLAY=2,GROUP_DRAWING=3;
#endif
	if(bRebuild)
	{
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
#ifdef __TSA_
		pPropDlg->m_arrPropGroupLabel.SetSize(3);
#elif defined(__LDS_)
		if(theApp.m_bAnalysisWorkEnvOnly||theApp.m_bDrawingWorkEnvOnly)
			pPropDlg->m_arrPropGroupLabel.SetSize(3);
		else
			pPropDlg->m_arrPropGroupLabel.SetSize(4);
#else
		pPropDlg->m_arrPropGroupLabel.SetSize(3);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DISPLAY-1,"Display");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"常规");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DISPLAY-1,"显示");
#endif
#if defined(__LDS_)||defined(__TSA_)
		if(!theApp.m_bDrawingWorkEnvOnly)
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_ANALYSIS-1,"计算");
		//pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DESIGNCODE-1,"设计");
#endif
#ifndef __TSA_
		if(!theApp.m_bAnalysisWorkEnvOnly)
#ifdef AFX_TARG_ENU_ENGLISH
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DRAWING-1,"Modeling");
#else 
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DRAWING-1,"建模");
#endif
#endif
		pPropDlg->RefreshTabCtrl(CLDSApp::m_iCurDisplayPropGroup);
		pPropList->CleanCallBackFunc();
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = 0;
		pPropList->SetModifyValueFunc(ModifySyssettingProperty);
		pPropList->SetButtonClickFunc(SyssettingButtonClick);
		pPropList->SetModifyStatusFunc(ModifySyssettingItemStatus);
		pPropList->SetCheckBoxClickFunc(SyssettingCheckBoxClick);
		pPropList->SetPopMenuClickFunc(FireSyssettingPopMenuClick);
		//
		CItemInfo* lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Basic Info";
#else 
		lpInfo->m_strPropName = "基本信息";
#endif
		pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
		pGroupItem->m_idProp = CLDSApp::GetPropID("basicInfo");
		pGroupItem->m_bHideChildren=FALSE;
		if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL); 
		//用户名
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "User Name";
#else 
		lpInfo->m_strPropName = "用户名";
#endif
		lpInfo->m_strPropHelp = "";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("env.m_sUserName");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//用户名缩写
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "User Name Abbreviation";
#else 
		lpInfo->m_strPropName = "用户名缩写";
#endif
		lpInfo->m_strPropHelp = "";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("env.m_sShortUserName");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//自动存盘间隔时间		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Auto Save Time";
#else 
		lpInfo->m_strPropName = "自动存盘间隔时间";
#endif
		lpInfo->m_strPropHelp = "";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nAutoSaveTime");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//自动存盘方式
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Auto Save Type";
		lpInfo->m_strPropHelp = "Single File :Back up only one file; Multiple File :Back up multiple file ,they distinguish by n。";
		lpInfo->m_cmbItems = "Single File Back-Up|Multiple File Back-Up";
#else 
		lpInfo->m_strPropName = "自动存盘方式";
		lpInfo->m_strPropHelp = "单文件备份:只备份一个文件;多文件备份:备份多个文件并用(n)区分。";
		lpInfo->m_cmbItems = "单文件备份|多文件备份";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iAutoSaveType");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
#ifndef __TMA_
		DWORD dwSaveFileVersion=0;
		if(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_TIDMODEL_EXPORT))
#else
		DWORD dwSaveFileVersion=ValidateLocalizeFeature(FEATURE::SAVEAS_FILE_VERSION);
		if(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_TIDMODEL_EXPORT)||dwSaveFileVersion>0)
#endif
		{	//文件版本格式
			lpInfo = new CItemInfo();
			//lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Save as previous version";
			lpInfo->m_strPropHelp = "Save as a file by previous version";
#else 
			lpInfo->m_strPropName = "另存旧版本";
			lpInfo->m_strPropHelp = "另存为旧版本文件";
#endif
			CString cmbItems;
#ifdef __TMA_
			if(dwSaveFileVersion>0)
			{
				XHVERSION xVer;
				xVer.cMajorVer=(BYTE)(dwSaveFileVersion/1000000);
				dwSaveFileVersion%=1000000;
				xVer.cMinorVer=(BYTE)(dwSaveFileVersion/10000);
				dwSaveFileVersion%=10000;
				xVer.cRevision=(BYTE)(dwSaveFileVersion/100);
				dwSaveFileVersion%=100;
				xVer.cBuild=(BYTE)dwSaveFileVersion;
				char verstr[24]={0};
				xVer.ToString(verstr);
				cmbItems = CXhChar50("V %s",verstr);
			}
			else
				cmbItems = "V 5.1.8.0|V 5.1.6.0|V 5.1.2.0|V 5.0.2.0|V 4.1.2.0|V 4.1.1.2";//最低可到V4.1.1.2
#elif defined(__LMA_)
			cmbItems = "V 1.1.1.2|V 1.1.2.0";
#elif defined(__LDS_)
			cmbItems = "V 1.2.1.0";
#elif defined(__TDA_)
			cmbItems = "V 1.3.7.0|V 1.3.0.3|V 1.2.1.0";
#else
			cmbItems = "V 1.3.0.3";
#endif
			lpInfo->m_cmbItems=cmbItems;
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.uFileSaveVersion");
			pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Save as previous version";
			lpInfo->m_strPropHelp = "Save as a file by previous version";
#else 
			lpInfo->m_strPropName = "批量另存旧版本";
			lpInfo->m_strPropHelp = "选择文件所在文件夹，批量另存为旧版本文件";
#endif
			lpInfo->m_cmbItems=cmbItems;
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSApp::GetPropID("BatchSaveAsOldFile");
			pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		char* szCanSaveDocTypes=NULL;
#ifdef __LMA_
		szCanSaveDocTypes = "*.tma";
#elif defined(__LDS_)
		szCanSaveDocTypes = "*.tma|*.lma|*.tda";
#elif defined(__TDA_)
		szCanSaveDocTypes = "*.tma|*.lma";
//#else
//		szCanSaveDocTypes = "";
#endif
		//另存其余产品的文件格式 wjh-2017.7.26
#if defined(__LDS_)
		if(true)	//根据河南院郭咏华要求，直接给设计院开放降版存储塔厂放样模型文件的功能更有利于市场推广 wjh-2019.5.25
#else
		if(szCanSaveDocTypes!=NULL&&VerifyValidFunction(LICFUNC::FUNC_IDENTITY_INTERNAL_TEST))
#endif
		{	//文件版本格式
			lpInfo = new CItemInfo();
			//lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENU;
			lpInfo->m_cmbItems=szCanSaveDocTypes;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Save as file type";
			lpInfo->m_strPropHelp = "Save as a file type";
#else 
			lpInfo->m_strPropName = "另存其余格式";
			lpInfo->m_strPropHelp = "另存其余文件格式";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iFileSaveDocType");
			pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
#ifdef __TSA_
		//定位绘图程序
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_FILEPATH;
		lpInfo->m_strPropName = "定位绘图程序";
		lpInfo->m_strPropHelp = "定位绘图程序";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("env.m_sAcadPath");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
#endif
		//当前图层名
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "Current layer";
#else 
		lpInfo->m_strPropName = "当前图层";
		lpInfo->m_strPropHelp = "当前图层";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("env.layer");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//输出系统警示信息
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems = "0.Close warnings|1.Important warnings|2.General warnings|3.Unimportant warnings|4.All warnings output";
		lpInfo->m_strPropName = "Warning Level";
		lpInfo->m_strPropHelp = "When the system running abnormal situation,output warning information";
#else
		lpInfo->m_cmbItems = "0.关闭所有警示|1.重要警示信息|2.一般警示信息|3.次要警示信息|4.输出全部警示";
		lpInfo->m_strPropName = "警告等级";
		lpInfo->m_strPropHelp = "当系统运行过程中遇到非正常情况时，输出警示信息";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.m_ciLowestWarningLevel");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//文件读写工作缓存大小
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "File read-write buffer size(MB)";
		lpInfo->m_strPropHelp = "";
#else
		lpInfo->m_strPropName = "文件读写工作缓存(MB)";
		lpInfo->m_strPropHelp = "文件读写工作缓存用于提升读写速度，极端情况下设定过大可能导致‘文档保存失败’错误";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("FileRW.BUFFSIZE");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//文件属性
		pGroupItem=oper.InsertPropItem(pRootItem,"env.flieProp");
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		pGroupItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
		//版本号
		pPropItem=oper.InsertEditPropItem(pGroupItem,"Ta.version");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"Ta.m_uiOriginalDogKey");
		pPropItem->SetReadOnly();
		if(Ta.m_uiOriginalDogKey==Ta.m_uiLastSaveDogKey)
			pPropItem->m_lpNodeInfo->m_strPropName="加密锁号";
		else
		{
			pPropItem=oper.InsertEditPropItem(pGroupItem,"Ta.m_uiLastSaveDogKey");
			pPropItem->SetReadOnly();
		}
		CPropTreeItem* pClassItem=oper.InsertEditPropItem(pGroupItem,"ModelInfo");
		pClassItem->SetReadOnly();
		//构件数
		pPropItem=oper.InsertEditPropItem(pClassItem,"Ta.Parts.GetNodeNum");
		pPropItem->SetReadOnly();
		//节点数
		pPropItem=oper.InsertEditPropItem(pClassItem,"Ta.Node.GetNodeNum");
		pPropItem->SetReadOnly();
		//呼高基点Z值（在LTMA中开放呼高排序）
		pPropItem=oper.InsertBtnPopMenuEditPropItem(pClassItem,"Ta.m_fNamedHeightZeroZ","","","智能计算");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp=!Ta.m_bUserSpecifyNamedHeightZeroZ;
		//工程的名称
		oper.InsertEditPropItem(pGroupItem,"Ta.m_sPrjName");
		//塔型
		oper.InsertEditPropItem(pGroupItem,"Ta.m_sTaType");
		//代号
		oper.InsertEditPropItem(pGroupItem,"Ta.m_sTaAlias");
		//钢印号
		oper.InsertEditPropItem(pGroupItem,"Ta.m_sTaStampNo");
		//基数
		oper.InsertEditPropItem(pGroupItem,"Ta.m_nTaNum");
		//操作员
		oper.InsertEditPropItem(pGroupItem,"Ta.m_sOperator");
		// 审查员
		oper.InsertEditPropItem(pGroupItem,"Ta.m_sAuditor");
		// 到位计算算法
		oper.InsertCmbListPropItem(pGroupItem,"Ta.m_bRepositionByModeBeforeV51");
		//连板面积统计方式
		oper.InsertCmbListPropItem(pGroupItem,"m_iPlankWeightStatType");
		//环板面积统计方式
		oper.InsertCmbListPropItem(pGroupItem,"m_iCirclePlankAreaType");
		//卷边处理方式
		oper.InsertCmbListPropItem(pGroupItem,"Ta.m_cRollProcessType");
		//卷边线计算方式
		oper.InsertCmbListPropItem(pGroupItem,"Ta.m_cRollOffsetType");
		//火曲变形处理方式
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"Ta.m_cDeformType");
		if(Ta.m_ciDeformType==1)
			oper.InsertEditPropItem(pPropItem,"Ta.m_fDeformCoef");
		//构架梁设计参数
		if(Ta.hashParams.GetNodeNum()>0)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Model parameters";
			lpInfo->m_strPropHelp = "Model design parameters";
#else 
			lpInfo->m_strPropName = "模型参数";
			lpInfo->m_strPropHelp = "模型设计参数";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSApp::GetPropID("Ta.hashParams");
			pPropItem->m_bHideChildren=FALSE;
			if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL); 
			CHashSet<CPropTreeItem*>hashInserts;
			CPropTreeItem* pParentItem=NULL;
			long idPropIdle=CLDSApp::MIN_IDLE_PROPID;
			for(CATEGORY* pCat=Ta.listParamCategory.EnumObjectFirst();pCat;pCat=Ta.listParamCategory.EnumObjectNext())
			{
				if((pParentItem=hashInserts.GetValue(pCat->idUpper))==NULL)
					pParentItem=pPropItem;
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = pCat->name;
				//lpInfo->m_strPropHelp = "";
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
				pSonPropItem->m_idProp = idPropIdle++;
				pSonPropItem->SetReadOnly();
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				hashInserts.SetValue(pCat->id,pSonPropItem);
			}
			for(DESIGN_PARAM_ITEM* pParam=Ta.hashParams.GetFirst();pParam;pParam=Ta.hashParams.GetNext())
			{
				if(pParam->uiCategoryId==0||(pParentItem=hashInserts.GetValue(pParam->uiCategoryId))==NULL)
					pParentItem=pPropItem;
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = pParam->sLabel;
				lpInfo->m_strPropHelp = pParam->sLabel;
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
				pSonPropItem->m_idProp = CLDSApp::GetPropID(CXhChar50("model.%s",(char*)pParam->sLabel));	
				pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
#ifdef __PART_DESIGN_INC_
		//保存文件更改日志
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems = "0.None|1.Save";
		lpInfo->m_strPropName = "Save modification log";
		lpInfo->m_strPropHelp = "";
#else 
		lpInfo->m_cmbItems = "0.不保存|1.保存";
		lpInfo->m_strPropName = "保存更改日志";
		lpInfo->m_strPropHelp = "保存文件时包含模型更改历史记录";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.m_bSaveLogVersion");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
#endif
		if(!theApp.m_bAnalysisWorkEnvOnly)
			oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_iModFileCodeType");
		//构件库
		pGroupItem=oper.InsertPropItem(pRootItem,"PartLibrary");
		pGroupItem->m_bHideChildren=FALSE;
		if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//角钢准距库(随文件保存)
		oper.InsertButtonPropItem(pGroupItem,"env.m_sJgZhunJuTbl");
		//角钢规格库
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sJgGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//角钢规格库文件
		oper.InsertFilePathPropItem(pPropItem,"env.m_sJgGuiGeTblPath");
#if defined(__COMPLEX_PART_INC_)||defined(__TSA_)//defined(__LMA_)||defined(__LDS_)
		//钢管规格库
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sTubeGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//钢管规格库文件
		oper.InsertFilePathPropItem(pPropItem,"env.m_sTubeGuiGeTblPath");
		//扁铁规格库	
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sFlatGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//扁铁规格库文件
		oper.InsertFilePathPropItem(pPropItem,"env.m_sFlatGuiGeTblPath");
		//槽钢规格库
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sSlotGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//槽钢规格库文件
		oper.InsertFilePathPropItem(pPropItem,"env.m_sSlotGuiGeTblPath");
		//角钢接头库
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sJgJointParaTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//角钢接头库文件
		oper.InsertFilePathPropItem(pPropItem,"env.m_sJgJointParaTblPath");
#ifdef __H_SHAPE_STEEL_
		//HW型钢规格库
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sHWGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//HW型钢规格库文件
		oper.InsertFilePathPropItem(pPropItem,"env.m_sHWGuiGeTblPath");
		//HM型钢规格库
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sHMGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//HM型钢规格库文件
		oper.InsertFilePathPropItem(pPropItem,"env.m_sHMGuiGeTblPath");
		//HN型钢规格库
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sHNGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//HN型钢规格库文件
		oper.InsertFilePathPropItem(pPropItem,"env.m_sHNGuiGeTblPath");
		//HP型钢规格库
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sHPGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//HP型钢规格库文件
		oper.InsertFilePathPropItem(pPropItem,"env.m_sHPGuiGeTblPath");
#endif
#endif

#if defined(__LDS_)||defined(__TSA_)
		//线缆规格库
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sWireGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
#endif
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
		//标准构件库(随文件保存)
		oper.InsertButtonPropItem(pGroupItem,"StdPartLib");
#endif
#ifndef __TSA_
		//螺栓规格系列(随文件保存)
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sLsGuiGeTbl");
		pPropItem->m_bHideChildren=FALSE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
		for(CLsFamily *pLsFamily=CLsLibrary::GetFirstFamily();pLsFamily;pLsFamily=CLsLibrary::GetNextFamily())//,i++)
		{	//螺栓规格系列
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMON;
			if(GetLocalBoltSeries(pLsFamily->Id,NULL)>0)
			{	//本地有该规格系列的螺栓标准文件，通过菜单按钮进行显示
				lpInfo->m_buttonType = BDT_POPMENU;
				lpInfo->m_cmbItems=MakeLocalLsSeriesString(pLsFamily->Id);
			}
#ifdef AFX_TARG_ENU_ENGLISH
			if(stricmp(pLsFamily->name,"标准螺栓")==0)
				lpInfo->m_strPropName = "Standard Bolts";
			else if(stricmp(pLsFamily->name,"双帽螺栓")==0)
				lpInfo->m_strPropName = "Double-nut Bolts";
			else if(stricmp(pLsFamily->name,"脚钉")==0)
				lpInfo->m_strPropName = "Climbing Nails";
			else if(stricmp(pLsFamily->name,"法兰螺栓")==0)
				lpInfo->m_strPropName = "Flange Bolts";
			else
				lpInfo->m_strPropName = pLsFamily->name;
#else
			lpInfo->m_strPropName = pLsFamily->name;
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSApp::MAX_USER_PROPID+pLsFamily->GetFamilyHandle();
			//pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//地脚螺栓库
		oper.InsertButtonPropItem(pPropItem,"ANCHOR_BOLT_LIB");
		//螺栓间距库(随文件保存)
		oper.InsertButtonPropItem(pGroupItem,"env.m_sLsSpaceTbl");
		//金具规格库(随文件保存)
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sFittingGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
		oper.InsertButtonPropItem(pPropItem,"env.m_sEBGuiGeTbl");
		oper.InsertButtonPropItem(pPropItem,"env.m_sGDGuiGeTbl");
		//oper.InsertButtonPropItem(pPropItem,"env.m_sURGuiGeTbl");
#endif
		//角钢螺栓选择表
		oper.InsertButtonPropItem(pGroupItem,"Ta.listSelBoltCond");
		//材质库
		oper.InsertButtonPropItem(pGroupItem,"env.m_sSteelMatTbl");
#if defined(__LDS_)||defined(__TSA_)
		//螺栓选择材料表
		oper.InsertButtonPropItem(pGroupItem,"env.m_sLsSelectTbl");
		//风振系数插值表
		oper.InsertButtonPropItem(pGroupItem,"env.m_sInterpolateBetaZTbl");
		//风振杆塔覆冰重力荷载系数表
		oper.InsertButtonPropItem(pGroupItem,"env.m_sInterpIceLoadCoefTbl");
#endif
		//显示属性组
		DisplaySysSettingDisplaySubSectionProperty(GROUP_DISPLAY);
#if defined(__LDS_)||defined(__TSA_)
		//DisplaySysSettingDesignCodeSubSectionProperty(GROUP_DESIGNCODE);
		if(!theApp.m_bDrawingWorkEnvOnly)
			DisplaySysSettingDesignSubSectionProperty(GROUP_ANALYSIS);
#endif
		//绘图属性组
#ifndef __TSA_
		if(!theApp.m_bAnalysisWorkEnvOnly)
			DisplaySysSettingDrawingSubSectionProperty(GROUP_DRAWING);
#endif
	}
	else
	{
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.nAutoSaveTime"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iMaxBakSerial"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sUserName"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sShortUserName"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sJgZhunJuTblPath"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sJgGuiGeTblPath"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sJgJointParaTblPath"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sTubeGuiGeTblPath"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sFlatGuiGeTblPath"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sSlotGuiGeTblPath"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
#ifdef __H_SHAPE_STEEL_
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sHWGuiGeTblPath"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sHMGuiGeTblPath"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sHNGuiGeTblPath"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sHPGuiGeTblPath"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
#endif
#ifndef __TSA_
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sLsSpaceTblPath"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.m_sLsSpaceTblPath"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("Ta.segstr"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
#endif
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.layer"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.fRotateVelocity"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bPartSketch"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bNodePnt"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bPoleLine"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("CLDSLinePart::bDisplayLineVecMark"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
#ifdef __COMPLEX_PART_INC_
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("CLDSNode::m_bDisplayBeforeArcLift"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
#endif
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.xPointStyle.cbPointSize"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.xPointStyle.ciPointStyle"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
#if defined(__LDS_)||defined(__TSA_)
#ifdef __RAPID_ROUGH_3DMODEL_
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bWirePoint"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
#endif
		//荷载箭头
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bLoadArrow"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.nLoadArrowScale"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bDimNodeLabel"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.nNodeLabelTextHeight"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bDimRodDesignInfo"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.nRodDesignInfoTextHeight"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
#endif
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_fMaxScaleScr2User"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
#ifndef __TSA_
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidJg"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidTube"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidLjb"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidJoint"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidLs"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidSlot"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidSphere"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidTransectFace"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("CLDSLineAngle::m_bDisplaySolidAxisLine"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("CLDSGeneralPlate::m_ciDisplayVertexType"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidFlat"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidArcAngle"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidArcSlot"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidStdPart"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidFoundation"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidArcFlat"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.bSolidLine"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.nSmoothness"),&pParentItem);
#endif
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_x.x"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_x.y"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_x.z"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_y.x"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_y.y"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_y.z"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_z.x"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_z.y"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("env.mcs.axis_z.z"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_bDispPartACS"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_bDispWorkUcs"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_bDispBlockLCS"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
#ifndef __TSA_
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.HuoQuDist"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.JgEndDist"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.JointPlankDist"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.TubeFlDist"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("CLDSLineTube::ciFlTubeOddment"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.VertexDist"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.CollideDist"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.fStressDiffuseAngle"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.fInsertPlateEdgeSpace"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.FootNailDist"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.fNailRepScope"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.fTubeWeldRoadMinD"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.nAutoAdjustTubePos"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_fKaiHeJiaoThreshold"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_fKaiHeJiaoMinDeita"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_nGroupJgMachiningGap"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_nMatCharPosType"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_cMatCharSeparator"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_fMinCutAngleLen"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_bDisplayConnectDlg"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_b45CutAngleNeedCoefR"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_fCutAngleCoefR"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_bAnglePreferSimplePosMode"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.display.cHoleWallMode"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("CLDSPart::UPDATE_PARTNO_AND_SEG"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_bEnablePrompt"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.b1BoltDesignPrompt"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("CLDSBolt::BOLTL0_CALMODE"), &pParentItem);
		if (pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("CLDSBolt::BOLTL0_PREFER_MODE"), &pParentItem);
		if (pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bOnlyDesignCurView"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignCommonConnect"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignBendConnect"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignAngleJoint"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignFL"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.BoltPadThick.m_nM12"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.BoltPadThick.m_nM16"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.BoltPadThick.m_nM20"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.BoltPadThick.m_nM24"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.m_nPlankEdgeAngleThreshold"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("m_iProfileAlgorithm"),&pParentItem);
		if(pPropItem)
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
#endif
		//应力
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_bAuxPoleAnalysisInc"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_iResolverStyle"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_fBeamLateralStifReductCoef"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_iNonlinearStyle"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_nIterationCounts"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("Ta.m_cStructureType"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iShowDeformedMapPlan"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		//pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.bCalStdFoundationForce"),&pParentItem);
		//if(pPropItem)	
		//	pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.bUpdateRodDesignForce"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		//pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.bCreateAnalFile"),&pParentItem);
		//if(pPropItem)	
		//	pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.bRecNodeOffset"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.bCreateDetailRptFile"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iLanguage"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.g_sysPara.nM"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.nSigemaM"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.nPrecentSigemaM"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iEfficacyCeofType"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iDocType"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("Ta.SelMatPara.m_bPreferSingleAngle"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("Ta.SelMatPara.m_iSizeOrderRule"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("Ta.SelMatPara.m_nConvergeLim"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("Ta.SelMatPara.m_nMaxSelMatCount"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iDesignCode"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("Ta.SelMatPara.m_nMainPoleSlendernessLimit"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("Ta.SelMatPara.m_nXiePoleSlendernessLimit"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("Ta.SelMatPara.m_nAuxPoleSlendernessLimit"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("Ta.SelMatPara.m_nTensivePoleSlendernessLimit"),&pParentItem);
		if(pPropItem)	
			pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
	}
	g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);	//暂时在此处设置
	pPropList->Redraw();
}
static void UpdateWorkUcsPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem)
{
	CPropertyListOper<CLDSApp> oper(pPropList,&theApp);
	CPropTreeItem *pPropItem=NULL,*pLeafItem=NULL;
	//显示范围
	//装配平面
	oper.InsertCmbListPropItem(pParentItem,"work_ucs.m_eDatumPlane",CParaCS::GetDatumPlaneCmbItems());
	//装配位置
	pPropItem=oper.InsertButtonPropItem(pParentItem,"work_ucs.origin");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//装配坐标系原点位置X坐标分量
	oper.InsertEditPropItem(pPropItem,"work_ucs.origin.x");
	//装配坐标系原点位置Y坐标分量
	oper.InsertEditPropItem(pPropItem,"work_ucs.origin.y");
	//工作坐标系原点位置Z坐标分量
	oper.InsertEditPropItem(pPropItem,"work_ucs.origin.z");
	//装配时X轴方向
	pPropItem=oper.InsertButtonPropItem(pParentItem,"work_ucs.axis_x");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//工作坐标系X轴X坐标分量
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_x.x");
	pLeafItem->SetReadOnly();
	//工作坐标系X轴Y坐标分量
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_x.y");
	pLeafItem->SetReadOnly();
	//工作坐标系X轴Z坐标分量
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_x.z");
	pLeafItem->SetReadOnly();
	//装配时Y轴方向
	pPropItem=oper.InsertButtonPropItem(pParentItem,"work_ucs.axis_y");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//工作坐标系Y轴X坐标分量
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_y.x");
	pLeafItem->SetReadOnly();
	//工作坐标系Y轴Y坐标分量
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_y.y");
	pLeafItem->SetReadOnly();
	//工作坐标系Y轴Z坐标分量
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_y.z");
	pLeafItem->SetReadOnly();
	//装配时Z轴方向
	pPropItem=oper.InsertButtonPropItem(pParentItem,"work_ucs.axis_z");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//工作坐标系Z轴X坐标分量
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_z.x");
	pLeafItem->SetReadOnly();
	//工作坐标系Z轴Y坐标分量
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_z.y");
	pLeafItem->SetReadOnly();
	//工作坐标系Z轴Z坐标分量
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_z.z");
	pLeafItem->SetReadOnly();
}
void CLDSView::DisplaySysSettingDisplaySubSectionProperty(int iSectionType)
{
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem* pRootItem=pPropList->GetRootItem();
	CPropTreeItem* pGroupItem,*pPropItem,*pSonPropItem,*pLeafItem;
	CPropertyListOper<CLDSApp> oper(pPropList,&theApp);
	//显示范围
	pGroupItem=oper.InsertPropItem(pRootItem,"DisplayScope");
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(iSectionType);
#ifndef __TSA_
	//当前显示段号范围
	oper.InsertBtnPopMenuEditPropItem(pGroupItem,"Ta.segstr");
#endif
	//显示虚拟辅助构件
	oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.display.bDispVirtualPart");
	//单线显示
	pPropItem=oper.InsertEditPropItem(pGroupItem,"lineDisplay");
	//力学分析模式下，经常会用到[单线显示]内的子选择项 wjh-2015.3.5
	pPropItem->m_bHideChildren = !theApp.m_bAnalysisWorkEnvOnly;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//节点
	pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bNodePnt");
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	//
	if(g_sysPara.display.bNodePnt)
	{
		oper.InsertEditPropItem(pSonPropItem,"g_sysPara.display.xPointStyle.cbPointSize");
		//
		oper.InsertCmbListPropItem(pSonPropItem,"g_sysPara.display.xPointStyle.ciPointStyle");
	}
	//杆件简化线
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bPoleLine");
	//杆件起始端标记
	oper.InsertCmbListPropItem(pPropItem,"CLDSLinePart::bDisplayLineVecMark");
#ifdef __COMMON_PART_INC_
	//预拱显示状态
	oper.InsertCmbListPropItem(pPropItem,"CLDSNode::m_bDisplayBeforeArcLift");
#endif
#ifndef __TSA_
	//构件轮廓边以及顶点
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bPartSketch");
#endif 

#ifdef __PART_DESIGN_INC_
	//
#ifdef __RAPID_ROUGH_3DMODEL_
	pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bWirePoint");
#endif
	pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bLoadArrow");
	pSonPropItem->m_bHideChildren = FALSE;
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	//
	if(g_sysPara.display.bLoadArrow)
		oper.InsertEditPropItem(pSonPropItem,"g_sysPara.display.nLoadArrowScale");
	//
	pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bDimNodeLabel");
	pSonPropItem->m_bHideChildren = FALSE;
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	if(g_sysPara.display.bDimNodeLabel)
		oper.InsertEditPropItem(pSonPropItem,"g_sysPara.display.nNodeLabelTextHeight");
#endif
	//
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef __PART_DESIGN_INC_
	pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bDimRodDesignInfo","Yes|No","Rod Design Info","Rod's size and finite element design information",-1);
#else
	pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bDimRodDesignInfo","Yes|No","Rod Size Info","Rod's size and label information",-1);
#endif
#else 
#ifdef __PART_DESIGN_INC_
	pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bDimRodDesignInfo","是|否","杆件设计信息","包括杆件规格及单元的设计裕度信息",-1);
#else
	pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bDimRodDesignInfo","是|否","杆件规格信息","包括杆件规格材质及编号信息",-1);
#endif
#endif
	pSonPropItem->m_bHideChildren = FALSE;
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
#ifdef __PART_DESIGN_INC_
	//
	if(g_sysPara.display.bDimRodDesignInfo)
		oper.InsertEditPropItem(pSonPropItem,"g_sysPara.display.nRodDesignInfoTextHeight");
#endif
	oper.InsertEditPropItem(pPropItem,"g_sysPara.m_fMaxScaleScr2User");
#if !defined(__LDS_TSA_) && !defined(__TSA_)	//内力分析模块
	//实体显示
	pPropItem=oper.InsertEditPropItem(pGroupItem,"solidDisplay");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//角钢
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidJg");
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
	//钢管
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidTube");
#endif
	//连接板
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidLjb");
	//螺栓
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidLs");
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
	//槽钢
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidSlot");
	//球体
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidSphere");
	//扁钢
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidFlat");
	//环形角钢
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidArcAngle");
	//环形扁钢
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidArcFlat");
	//环形槽钢
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidArcSlot");
	//标准构件
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidStdPart");
	//基础/地脚螺栓
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidFoundation");
	//实体相贯面
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidTransectFace");
#endif
	//单线
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidLine");
	//角钢心线 wht 10-12-27
	oper.InsertCmbListPropItem(pPropItem,"CLDSLineAngle::m_bDisplaySolidAxisLine");
	//钢板轮廓点
	oper.InsertCmbListPropItem(pPropItem,"CLDSGeneralPlate::m_ciDisplayVertexType");
	//实体显示时的光滑度
	oper.InsertEditPropItem(pPropItem,"g_sysPara.display.nSmoothness");
	//孔壁显示方式
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.cHoleWallMode");
	//过滤选中构件
	pGroupItem=oper.InsertPropItem(pRootItem,"m_dwSelectPartsType");
	pGroupItem->m_bHideChildren=TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//是否可选中节点
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectNode");
	//是否可选中角钢
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectLineAngle");
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
	//是否可选中钢管
	const char* itemname="钢管";
#else
	const char* itemname="套管";
#endif
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectLineTube","是|否",itemname);
	//是否可选中钢板
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectPlate");
	//是否可选中参数化钢板
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectParamPlate");
	//是否可选中螺栓
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectBolt");
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
	//是否可选中球体
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectSphere");
	//是否可选中槽钢
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectLineSlot");
	//是否可选中扁钢
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectLineFlat");
	//是否可选中环形角钢
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectArcAngle");
	//是否可选中环形扁钢
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectArcFlat");
	//是否可选中环形槽钢
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectArcSlot");
	//是否可选中标准构件
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectStdPart");
	//
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectEdgeLine");
#endif
#endif
	//显示模式
	pGroupItem=oper.InsertPropItem(pRootItem,"DisplayMode");
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//旋转速度
	oper.InsertEditPropItem(pGroupItem,"g_sysPara.fRotateVelocity");
	//单线模型自动消隐
	oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_bLineDepthTest");
	//拖拽开窗功能开关项
	oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_bDragOpenWndSel");
	//拖拽开窗功能开关项
	oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_bDblClkStartRotateOper");
	//颜色方案
	oper.InsertButtonPropItem(pGroupItem,"ColorScheme");
	//基准坐标系
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"env.mcs");
	pPropItem->m_bHideChildren=TRUE;
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//基准坐标系X轴方向
	pSonPropItem=oper.InsertButtonPropItem(pPropItem,"env.mcs.axis_x");
	pSonPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	//基准坐标系X轴X坐标分量
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_x.x");
	pLeafItem->SetReadOnly();
	//基准坐标系X轴Y坐标分量
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_x.y");
	pLeafItem->SetReadOnly();
	//基准坐标系X轴Z坐标分量
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_x.z");
	pLeafItem->SetReadOnly();
	//装配时Y轴方向
	pSonPropItem=oper.InsertButtonPropItem(pPropItem,"env.mcs.axis_y");
	pSonPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	//基准坐标系Y轴X坐标分量
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_y.x");
	pLeafItem->SetReadOnly();
	//基准坐标系Y轴Y坐标分量
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_y.y");
	pLeafItem->SetReadOnly();
	//基准坐标系Y轴Z坐标分量
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_y.z");
	pLeafItem->SetReadOnly();
	//装配时Z轴方向
	pSonPropItem=oper.InsertButtonPropItem(pPropItem,"env.mcs.axis_z");
	pSonPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	//基准坐标系Z轴X坐标分量
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_z.x");
	pLeafItem->SetReadOnly();
	//基准坐标系Z轴Y坐标分量
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_z.y");
	pLeafItem->SetReadOnly();
	//基准坐标系Z轴Z坐标分量
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_z.z");
	pLeafItem->SetReadOnly();
	//工作坐标系
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_bDispWorkUcs");
	UpdateWorkUcsPropItem(pPropList,pPropItem);
#if !defined(__LDS_TSA_) && !defined(__TSA_)	//LDS的内力分析模块
	//构件装配坐标系
	oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_bDispPartACS");
	//部件定位坐标系
	oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_bDispBlockLCS");
#endif
}

void CLDSView::DisplaySysSettingDesignSubSectionProperty(int iSectionType)
{
#if defined(__LDS_)||defined(__TSA_)
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem* pRootItem=pPropList->GetRootItem();
	CPropTreeItem* pGroupItem,*pPropItem,*pSonPropItem;
	//求解器
	CItemInfo* lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Load Calculation";
#else
	lpInfo->m_strPropName = "荷载计算";
#endif
	pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CLDSApp::GetPropID("WireLoadCal");
	pGroupItem->m_bHideChildren=TRUE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(iSectionType); 

	/*lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = "电压等级(kV)";
	lpInfo->m_strPropHelp="500kV及以上电压等级，导地线风荷载计算需考虑导地线风荷载调整系数βc。";
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("volt_grade");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(iSectionType); */
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "landform of tower location";
	lpInfo->m_strPropHelp="landform of tower location affects the changing of wind pressure in coefficient values.";
	lpInfo->m_cmbItems="A:Offshore and desert flat region|B:Inland sparse-constructed area|C:A town with dense-constructed area|D:A town with dense-constructed and high-rise buildings";
#else
	lpInfo->m_strPropName = "塔位地貌";
	lpInfo->m_strPropHelp="塔位地貌影响风压高度变化系数取值。";
	lpInfo->m_cmbItems="A:近海或沙漠平坦地区|B:内陆建筑物稀疏地区|C:有密集建筑群市区|D:有密集建筑群和高层建筑市区";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("m_cLandscape");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "line road condition";
	lpInfo->m_strPropHelp="road condition information affects ground line's tension value in broken line working condition.";
	lpInfo->m_cmbItems="Flat line|Hills line|Mountainous region";
#else
	lpInfo->m_strPropName = "线路路况";
	lpInfo->m_strPropHelp="线路路况信息影响到断线工况时导地线断线张力的取值。";
	lpInfo->m_cmbItems="平地线路|丘陵线路|山地线路";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("m_cLineRoadType");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 

	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Weather condition";
#else
	lpInfo->m_strPropName = "气象条件";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("WeatherCondition");
	pPropItem->m_lpNodeInfo->m_strPropValue=theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 

	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Maximum air temperature(°C)";
	lpInfo->m_strPropHelp = "Maximum air temperature(°C)";
#else
	lpInfo->m_strPropName = "最高气温(°C)";
	lpInfo->m_strPropHelp = "最高气温(°C)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_nHighestTemperature");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Minimum air temperature(°C)";
	lpInfo->m_strPropHelp = "Minimum air temperature(°C)";
#else
	lpInfo->m_strPropName = "最低气温(°C)";
	lpInfo->m_strPropHelp = "最低气温(°C)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_nLowestTemperature");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "average temperature(°C)";
	lpInfo->m_strPropHelp = "average temperature(°C)";
#else
	lpInfo->m_strPropName = "平均气温(°C)";
	lpInfo->m_strPropHelp = "平均气温(°C)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_nAverageTemperature");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	//大风工况
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "termperature in maximum wind velocity(°C)";
	lpInfo->m_strPropHelp = "termperature in maximum wind velocity(°C)";
#else
	lpInfo->m_strPropName = "最大风速时温度(°C)";
	lpInfo->m_strPropHelp = "最大风速时温度(°C)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_nMaxWindVelocityTemperature");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "maximum wind velocity(m/s)";
	lpInfo->m_strPropHelp = "maximum wind velocity(m/s)";
#else
	lpInfo->m_strPropName = "最大风速(m/s)";
	lpInfo->m_strPropHelp = "最大风速(m/s)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_fMaxWindVelocity");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	//覆冰工况
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "temperature in maximum icing(°C)";
	lpInfo->m_strPropHelp = "temperature in maximum icing(°C)";
#else
	lpInfo->m_strPropName = "最大覆冰时温度(°C)";
	lpInfo->m_strPropHelp = "最大覆冰时温度(°C)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_nMaxIceTemperature");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "wind speed in maximum icing(m/s)";
	lpInfo->m_strPropHelp = "wind speed in maximum icing(m/s)";
#else
	lpInfo->m_strPropName = "最大覆冰时风速(m/s)";
	lpInfo->m_strPropHelp = "最大覆冰时风速(m/s)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_fMaxIceWindVelocity");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "thickness of icing(mm)";
	lpInfo->m_strPropHelp = "thickness of icing(mm)";
#else
	lpInfo->m_strPropName = "覆冰厚度(mm)";
	lpInfo->m_strPropHelp = "覆冰厚度(mm)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_fMaxIceThick");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	//安装工况
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "temperature while installing(°C)";
	lpInfo->m_strPropHelp = "The temperature while installing(°C)";
#else
	lpInfo->m_strPropName = "安装时温度(°C)";
	lpInfo->m_strPropHelp = "安装时温度(°C)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_nSetupTemperature");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "wind speed while installing(m/s)";
	lpInfo->m_strPropHelp = "wind speed while installing(m/s)";
#else
	lpInfo->m_strPropName = "安装时风速(m/s)";
	lpInfo->m_strPropHelp = "安装时风速(m/s)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_fSetupWindVelocity");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	//求解器
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "finite element solver";
#else
	lpInfo->m_strPropName = "有限元求解器";
#endif
	pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CLDSApp::GetPropID("EquationSolver");
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "choose solver mode";
	lpInfo->m_strPropHelp = "choose solver mode";
	lpInfo->m_cmbItems = "0.Gauss method|1.Frontal method|2.electric power pylon";
	lpInfo->m_strPropValue = "0.Gauss method";
#else
	lpInfo->m_strPropName = "求解器方式选择";
	lpInfo->m_strPropHelp = "求解器方式选择";
	lpInfo->m_cmbItems = "0.高斯法|1.波前法|2.电力塔架";
	lpInfo->m_strPropValue = "0.高斯法";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_iResolverStyle");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	pPropItem->SetReadOnly(g_sysPara.AnalysisPara.m_iNonlinearStyle==1);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "high-low legs rotating calculation";
	lpInfo->m_strPropHelp = "high-low legs rotating calculation";
	lpInfo->m_cmbItems = "0.None rotate|1.Rotation of accordant legs|2.Rotation of high-low legs";
#else
	lpInfo->m_strPropName = "高低腿轮换计算";
	lpInfo->m_strPropHelp = "高低腿轮换计算";
	lpInfo->m_cmbItems = "0.不轮换|1.等高腿轮换|2.高低腿轮换";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iLegShiftType");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Recording the information of analysis procedure";
	lpInfo->m_strPropHelp = "Recording each node's displacement,load and element force of analysis processure in memory, in order to check the deformation pattern";
	lpInfo->m_cmbItems = "Yes|No";
#else
	lpInfo->m_strPropName = "记录分析过程信息";
	lpInfo->m_strPropHelp = "在内存中临时记录每个分析过程的节点位移、荷载及单元内力，以便查看变形图。";
	lpInfo->m_cmbItems = "是|否";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.bRecNodeOffset");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Generating ANSYS command line files";
	lpInfo->m_strPropHelp = "Generating command line interface files of analysis procedure ";
	lpInfo->m_cmbItems = "Yes|No";
#else
	lpInfo->m_strPropName = "生成ANSYS命令行文件";
	lpInfo->m_strPropHelp = "生成分析过程的ANSYS命令行接口文件";
	lpInfo->m_cmbItems = "是|否";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.bCreateAnsysCmdFile");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Reduction factor of beam transverse stiffness";
	lpInfo->m_strPropHelp = "According to practical requirements, reducing beam element's transverse stiffness is permitted";
#else
	lpInfo->m_strPropName = "梁横向刚度折减系数";
	lpInfo->m_strPropHelp = "根据实际需要可对梁单元横向刚度进行一定程度上的折减。";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_fBeamLateralStifReductCoef");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Bracing rod participates in mechanical analysis";
	lpInfo->m_strPropHelp = "Bracing rod can also participate in finite element mechanical analysis as stress material, and often used in dynamic analysis and nonelinear structure analysis. When doing the liner analysis,the supplementary material often does not participate in finite element mechancial analysis";
	lpInfo->m_cmbItems = "Yes|No";
#else
	lpInfo->m_strPropName = "辅助材参预力学分析";
	lpInfo->m_strPropHelp = "辅助材也作为受力材参预有限元力学分析，常用于动力特性分析及非线性结构分析时。线性分析时一般辅助材不参预有限元力学分析。";
	lpInfo->m_cmbItems = "是|否";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_bAuxPoleAnalysisInc");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "iterations of modal analysis";
	lpInfo->m_strPropHelp = " maximum number of literations of the structural mode dynamic analysis ";
#else
	lpInfo->m_strPropName = "模态分析迭代次数";
	lpInfo->m_strPropHelp = "结构模态动力特性分析时的最大迭代次数";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_nSubSpaceIterCounts");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	pPropItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName =	"convergence error for natural vibration frequency";
	lpInfo->m_strPropHelp = "during dynamic characteristics analysis, convergence relative error of strucuture natural vibration frequencies";
#else
	lpInfo->m_strPropName = "自振频率收敛误差";
	lpInfo->m_strPropHelp = "动力特性分析时,结构自振频率的收敛相对误差";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_fFreqsTolerance");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "stress analysis and calculation method";
	lpInfo->m_strPropHelp = "stress analysis and calculation method";
	lpInfo->m_cmbItems = "0.Linear analysis |1.Nonlinear analysis";
	lpInfo->m_strPropValue = "0.Linear analysis";
#else
	lpInfo->m_strPropName = "应力分析计算方式";
	lpInfo->m_strPropHelp = "应力分析计算方式";
	lpInfo->m_cmbItems = "0.线性分析|1.非线性分析";
	lpInfo->m_strPropValue = "0.线性分析";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_iNonlinearStyle");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	pPropItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
	if(g_sysPara.AnalysisPara.m_iNonlinearStyle==1)
	{	//非线性分析
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "times of iteration";
		lpInfo->m_strPropHelp = "maximum times of iteration in each increment, if beyond, quit the iteration"
#else
		lpInfo->m_strPropName = "迭代次数";
		lpInfo->m_strPropHelp = "每增量内最多迭代次数，超过则退出迭代";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_nIterationCounts");
		pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "astringe value";
		lpInfo->m_strPropHelp = "dynamic，N times reach precision and quit the iteration";
#else
		lpInfo->m_strPropName = "收敛阈值";
		lpInfo->m_strPropHelp = "应力分析计算收敛阈值，N次达到精度退出迭代";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_fConvergeLim");
		pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	}
	//单元验算
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Unit calculation";
#else
	lpInfo->m_strPropName = "单元验算";
#endif
	pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CLDSApp::GetPropID("MemberCheck");
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "The type of structure";
	lpInfo->m_strPropHelp = "singular Major structure refers to normal single tower structure such as tower power、microwave communication tower, Eiffel wind pressure effect should be considered when desiging; composite structure does not need to consider the Eiffel wind pressure effect";
	lpInfo->m_cmbItems = "0.Single major structure|1.Composite structure";
#else
	lpInfo->m_strPropName = "结构类型";
	lpInfo->m_strPropHelp = "单主体结构指普通单塔身结构如常见电力塔、微波通信塔，设计时需考虑埃菲尔风压效应；复合结构不考虑埃菲尔风压效应";
	lpInfo->m_cmbItems = "0.单主体结构|1.复合结构";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("Ta.m_cStructureType");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//结果输出
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName =	 "result output";
#else
	lpInfo->m_strPropName = "结果输出";
#endif
	pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CLDSApp::GetPropID("DocumentExport");
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	/*
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_strPropName = "分析过程anl文件";
	lpInfo->m_strPropHelp = "生成分析过程文件";
	lpInfo->m_cmbItems = "是|否";
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.bCreateAnalFile");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	*/
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "calculation result preview plan";
	lpInfo->m_strPropHelp = "calculation result preview plan";
	lpInfo->m_cmbItems = "0.Displacement deformation|1.Efficiency coefficient method|2.bearing capacity factor|3.Internal force proportion";
#else
	lpInfo->m_strPropName = "计算结果预览方案";
	lpInfo->m_strPropHelp = "计算结果预览方案";
	lpInfo->m_cmbItems = "0.位移变形|1.承载+长细比系数|2.承载系数|3.内力比";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iShowDeformedMapPlan");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_strPropName = "单元效率系数类型";
	lpInfo->m_strPropHelp = "单元承载系数是只与荷载有关的效率系数，而设计系数中则包含了长细比限制因素。";
	lpInfo->m_cmbItems = "0.承载+长细比系数|1.承载系数";
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iEfficacyCeofType");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Update rod force design value";
	lpInfo->m_strPropHelp = "Calculating,ending, automatically adding and updating the maximum force design value of each rod's end, in order to check the connection design of bolts and flanges";
	lpInfo->m_cmbItems = "Yes|No";
#else
	lpInfo->m_strPropName = "更新杆件作用力设计值";
	lpInfo->m_strPropHelp = "计算结束自动汇总更新各杆件端头最大作用力设计值，以便于进行螺栓、法兰等连接件设计校核";
	lpInfo->m_cmbItems = "是|否";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.bUpdateRodDesignForce");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	{	//详细报告输出文档类型
		/*
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "合并对称项计算结果";
		lpInfo->m_strPropHelp = "合并对称节点和杆件的信息输出";
		lpInfo->m_cmbItems = "是|否";
		pSonPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);	//InsertItem(pPropItem
		pSonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.bMergeRptFileMirObj");
		pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);*/
		//
		/*lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "弯矩项输出";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.putOutM");
		pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
		pSonPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pSonPropItem->m_idProp);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "M阈值";
		lpInfo->m_strPropHelp = "M阈值";
		CPropTreeItem* pGrandsonPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
		pGrandsonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nM");
		pGrandsonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pGrandsonPropItem->m_idProp);
		pGrandsonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "σM阈值";
		lpInfo->m_strPropHelp = "σM阈值";
		pGrandsonPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
		pGrandsonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nSigemaM");
		pGrandsonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pGrandsonPropItem->m_idProp);
		pGrandsonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "σM%阈值";
		lpInfo->m_strPropHelp = "σM%阈值";
		pGrandsonPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
		pGrandsonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nPrecentSigemaM");
		pGrandsonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pGrandsonPropItem->m_idProp);
		pGrandsonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);*/

		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Filtrating basic forces";
		lpInfo->m_strPropHelp = "When filtrating the level of high-low legs，only output the maximum pull-up force and down force from a module; When filtrating in a working condition level, only output all the pull-up force and down force from a moudle.";
		lpInfo->m_cmbItems = "0.Full output|1.High-low legs level|2.Working condition level";
#else
		lpInfo->m_strPropName = "基础作用力筛选";
		lpInfo->m_strPropHelp = "高低腿级筛选时，只输出一个呼高组工况中所有高低腿中的最大上拨力和下压力;工况级筛选时只输出一个呼高组中所有工况的上拨力和下压力。";
		lpInfo->m_cmbItems = "0.全部输出|1.高低腿级|2.工况级";
#endif
		pSonPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);	//InsertItem(pPropItem
		pSonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.reactionExportType");
		pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	}
	//选材
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "select steel material";
#else
	lpInfo->m_strPropName = "选材";
#endif
	pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CLDSApp::GetPropID("selectMaterial");
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Priority given to singular-angle in angle parts";
	lpInfo->m_strPropHelp = "Priority given to singular-angle in angle parts when selecting steel material";
	lpInfo->m_cmbItems = "Yes|No";
#else
	lpInfo->m_strPropName = "角钢构件单角钢优先";
	lpInfo->m_strPropHelp = "角钢构件选材时优选单角钢";
	lpInfo->m_cmbItems = "是|否";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_bPreferSingleAngle");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "rules for specification prioirity order";
	lpInfo->m_strPropHelp = "ordering basis for specification warehouse when choosing parts material";
	lpInfo->m_cmbItems = "0.cross-section Order|1.turning radius order|2. width and thickness Order";
#else
	lpInfo->m_strPropName = "规格优选排序规则";
	lpInfo->m_strPropHelp = "构件选材时规格库的排序依据";
	lpInfo->m_cmbItems = "0.截面积排序|1.回转半径排序|2.宽度厚度排序";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_iSizeOrderRule");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "accuracy of material selection";
	lpInfo->m_strPropHelp = "accuracy of material selection";
#else
	lpInfo->m_strPropName = "选材精度";
	lpInfo->m_strPropHelp = "选材精度";
#endif
	lpInfo->m_strPropValue = "4";
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_nConvergeLim");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Times of material selection";
	lpInfo->m_strPropHelp = "Times of material selection";
#else
	lpInfo->m_strPropName = "选材次数";
	lpInfo->m_strPropHelp = "选材次数";
#endif
	lpInfo->m_strPropValue = "4";
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_nMaxSelMatCount");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
#endif
}

void CLDSView::DisplaySysSettingDesignCodeSubSectionProperty(int iSectionType)
{
#if defined(__LDS_)||defined(__TSA_)
	//基本设计规定
	//γ0结构重要性系数
	//γG重力荷载分项系数
	//ψ可变荷载组合系数
	//γQi第i项可变荷载分项系数
	//风荷载计算规定
	//基本自振周期
	//风振系数
	//βc导线风荷载调整系数
	//βz杆塔风荷载调整系数
	//μs风荷载体型系数
	//对基础风荷载调整系数
	//覆冰荷载计算规定
	//覆冰挡风荷载（体现在风荷载调整系数）
	//覆冰厚度
	//Ig覆冰重力荷载
	//强度设计规定
	//m构件轴心受力强度折减系数
	//mM受弯构件稳定强度折减系数
	//轴心受压构件稳定系数
	//mN压杆稳定强度折减系数
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem* pRootItem=pPropList->GetRootItem();
	CPropTreeItem* pGroupItem,*pPropItem,*pSonPropItem,*pLeafItem;

	CItemInfo* lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "attribute of tower";
	lpInfo->m_strPropHelp = "attribute of tower";
#else
	lpInfo->m_strPropName = "杆塔属性";
	lpInfo->m_strPropHelp = "杆塔属性";
#endif
	pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CLDSApp::GetPropID("TowerProperty");
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Fundamental natural vibration period";
	lpInfo->m_strPropHelp = "Fundamental natural vibration period(s)";
#else
	lpInfo->m_strPropName = "基本自振周期";
	lpInfo->m_strPropHelp = "基本自振周期(s)";
#endif
	lpInfo->m_strPropValue = "0.25";
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("Ta.m_fVibrationT1");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "landform of tower location";
	lpInfo->m_strPropHelp = "landform of tower location";
	lpInfo->m_cmbItems="A:Offshore and desert flat region|B:Inland sparse-constructed area|C:A town with dense-constructed area|D:A town with dense-constructed and high-rise buildings";
	lpInfo->m_strPropValue = "Inland sparsely-constructed area";
#else
	lpInfo->m_strPropName = "塔位地形";
	lpInfo->m_strPropHelp = "塔位地形";
	lpInfo->m_cmbItems="A.近海或沙漠平坦地区|B.内陆建筑物稀疏地区|C.有密集建筑群市区|D.有密集建筑群和高层建筑市区";
	lpInfo->m_strPropValue = "内陆建筑物稀疏地区";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("Ta.m_cLandscape");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Security level";
	lpInfo->m_strPropHelp = "Security level";
	lpInfo->m_cmbItems="First level:very important tower|Second level: normal towers of different types|Third level: temporary tower";
	lpInfo->m_strPropValue = "Second level: normal towers of different types";
#else
	lpInfo->m_strPropName = "安全等级";
	lpInfo->m_strPropHelp = "安全等级";
	lpInfo->m_cmbItems="一级:特别重要的杆塔|二级:一般的各类杆塔|三级:临时使用的杆塔";
	lpInfo->m_strPropValue = "二级:一般的各类杆塔";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("Ta.m_siSafetyGrade");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//结构重要性系数γo
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "strcucture importance coefficient";
	lpInfo->m_strPropHelp = "strcucture importance coefficientγo";
#else
	lpInfo->m_strPropName = "结构重要性系数";
	lpInfo->m_strPropHelp = "结构重要性系数γo";
#endif
	lpInfo->m_strPropValue = "1.0";
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("Ta.m_fSafetyCoef");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Basic rules";
	lpInfo->m_strPropHelp = "Basic rules";
#else
	lpInfo->m_strPropName = "基本规定";
	lpInfo->m_strPropHelp = "基本规定";
#endif
	pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CLDSApp::GetPropID("BasicRegulations");
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//承载系数最大设计值
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "maximum design value of loading coefficient";
	lpInfo->m_strPropHelp = "maximum design value of loading coefficient which parts can take, ranging from 0~1.0";
#else
	lpInfo->m_strPropName = "承载系数最大设计值";
	lpInfo->m_strPropHelp = "构件可以达到的最大承载系数值，取值范围为0~1.0";
#endif
	lpInfo->m_strPropValue = "1.0";
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("m_fLimitBearCoef");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//基本设计规定
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "rules for carrying out load calculation";
	lpInfo->m_strPropHelp = "rule of load calculation，design specification is executed currently";
	lpInfo->m_cmbItems = "design rules for 110~750kV transmission line|Towering structure design rules GB50135-2006|Loading rules for building structures design GB50009-2001";
	lpInfo->m_strPropValue = "design rules for 110~750kV transmission line"
#else
	lpInfo->m_strPropName = "荷载计算执行规范";
	lpInfo->m_strPropHelp = "荷载计算规定，当前执行设计规范";
	lpInfo->m_cmbItems = "110~750kV输电线路设计规定|高耸结构设计规定 GB50135-2006|建筑结构荷载规范 GB50009-2001";
	lpInfo->m_strPropValue = "110~750kV输电线路设计规定";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("m_iLoadExecuteCode");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "basic calculation βz";
	lpInfo->m_strPropHelp = "basic calculating wind load adjustment coefficient";
#else
	lpInfo->m_strPropName = "基础验算βz";
	lpInfo->m_strPropHelp = "基础验算风荷载调整系数";
#endif
	lpInfo->m_strPropValue = "0.25";
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_fWindBetaZForFoundation");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Optimize gravity load γG when selecting material";
	lpInfo->m_strPropHelp = "Optimize the gravity loads subentry coefficient when selecting material";
#else
	lpInfo->m_strPropName = "优化选材时重力荷载γG";
	lpInfo->m_strPropHelp = "优化选材时重力荷载分项系数";
#endif
	lpInfo->m_strPropValue = "1.2";
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_fGamaGForDesign");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Gravity loadγG during calculation";
	lpInfo->m_strPropHelp = "Gravity load subentry coefficient during calculation";
#else
	lpInfo->m_strPropName = "基础验算时重力荷载γG";
	lpInfo->m_strPropHelp = "基础验算时重力荷载分项系数";
#endif
	lpInfo->m_strPropValue = "1.0";
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_fGamaGForFoundation");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Tower wind load subentry coefficientγQ";
	lpInfo->m_strPropHelp = "variable loads subentry coefficient γQ of Tower wind load"
#else
	lpInfo->m_strPropName = "塔身风荷载分项系数γQ";
	lpInfo->m_strPropHelp = "塔身风荷载作为可变荷载的分项系数γQ";
#endif
	lpInfo->m_strPropValue = "1.4";
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_fGamaQForTowerWindLoad");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//基本设计规定
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "rules for parts material selection";
	lpInfo->m_strPropHelp = "basic design rules, design rules are executed currently";
	lpInfo->m_cmbItems = "design rules for 110~750kV transmission line|towering structures design rules GB50135-2006|America ASCE 10-97";
	lpInfo->m_strPropValue = "110~750kV ransmission line design rules";
#else
	lpInfo->m_strPropName = "构件选材计算执行规范";
	lpInfo->m_strPropHelp = "基本设计规定，当前执行设计规范";
	lpInfo->m_cmbItems = "110~750kV输电线路设计规定|高耸结构设计规定 GB50135-2006|美国 ASCE 10-97";
	lpInfo->m_strPropValue = "110~750kV输电线路设计规定";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("m_iMemberSizeCheckCode");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);

	//
	//选材规格限制：钢板厚度最小值、角钢截面最小值、钢管壁厚最小值
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "limit for  angle specification material selection";
	lpInfo->m_strPropHelp = "limit for  angle specification material selection";
#else
	lpInfo->m_strPropName = "角钢规格选材限制";
	lpInfo->m_strPropHelp = "角钢规格选材限制";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("setMatLim");
	pSonPropItem->m_bHideChildren=TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "minimun cross-section of steel material";
	lpInfo->m_strPropHelp = "minimun cross-section of steel material";
#else
	lpInfo->m_strPropName = "主材最小截面积";
	lpInfo->m_strPropHelp = "主材最小截面积";
#endif
	lpInfo->m_cmbItems = MakeAngleSetString();
	lpInfo->m_cmbType = CDT_EDIT;
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_csMinMainAngleSize");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "minimum cross-section of the forced steel material";
	lpInfo->m_strPropHelp = "minimum cross-section of the forced steel material";
#else
	lpInfo->m_strPropName = "受力材最小截面积";
	lpInfo->m_strPropHelp = "受力材最小截面积";
#endif
	lpInfo->m_cmbItems = MakeAngleSetString();
	lpInfo->m_cmbType = CDT_EDIT;
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_csMinForceAngleSize");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "minimum cross-section for the other steel materials";
	lpInfo->m_strPropHelp = "minimum cross-section for the othersteel materials";
#else
	lpInfo->m_strPropName = "其它材最小截面积";
	lpInfo->m_strPropHelp = "其它材最小截面积";
#endif
	lpInfo->m_cmbItems = MakeAngleSetString();
	lpInfo->m_cmbType = CDT_EDIT;
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_csMinAngleSize");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//钢管构件长细比限制
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "proportion limit for tube's length and slenderness";
	lpInfo->m_strPropHelp = "proportion limit for tube's length and slenderness，used for considering the limit for the critical wind speed start oscillation from equal diameter steel tube";
#else
	lpInfo->m_strPropName = "钢管长细比限制";
	lpInfo->m_strPropHelp = "钢管长细比限制，用于考虑等直径钢管起振临界风速的限制";
#endif
	lpInfo->m_strPropValue = "";
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("TubeSlendernessLimit");
	//pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "length and slenderness ration threshold  of pressure legs' tube";
	lpInfo->m_strPropHelp = "length and slenderness ration threshold  of pressure legs' tube";
#else
	lpInfo->m_strPropName = "受压主材钢管长细比阈值";
	lpInfo->m_strPropHelp = "受压主材钢管长细比阈值";
#endif
	lpInfo->m_strPropValue = "150";
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_nMainTubeSlendernessLimit");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "length and slenderness ration threshold  of pressure tilted material";
	lpInfo->m_strPropHelp = "length and slenderness ration threshold  of pressure tilted material";
#else
	lpInfo->m_strPropName = "受压斜材钢管长细比阈值";
	lpInfo->m_strPropHelp = "受压斜材钢管长细比阈值";
#endif
	lpInfo->m_strPropValue = "220";
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_nXieTubeSlendernessLimit");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "length and slenderness ration threshold  of pressure auxiliary tube";
	lpInfo->m_strPropHelp = "length and slenderness ration threshold  of pressure auxiliary tube";
#else
	lpInfo->m_strPropName = "受压辅助钢管长细比阈值";
	lpInfo->m_strPropHelp = "受压辅助钢管长细比阈值";
#endif
	lpInfo->m_strPropValue = "250";
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_nAuxTubeSlendernessLimit");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "length and slenderness ration threshold  of the pulled tube";
	lpInfo->m_strPropHelp = "length and slenderness ration threshold  of the pulled tube";
#else
	lpInfo->m_strPropName = "受拉钢管长细比阈值";
	lpInfo->m_strPropHelp = "受拉钢管长细比阈值";
#endif
	lpInfo->m_strPropValue = "400";
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_nTensiveTubeSlendernessLimit");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//其余杆件长细比限制
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "length and slenderness ration limit  for the other rods";
	lpInfo->m_strPropHelp = "length and slenderness ration limit  for the other rods except tubes，current design specifications";
#else
	lpInfo->m_strPropName = "其余杆件长细比限制";
	lpInfo->m_strPropHelp = "除钢管外其余杆件长细比限制，当前设计规范";
#endif
	lpInfo->m_strPropValue = "";
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("SlendernessLimit");
	//pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "length and slenderness ration threshold  of pressure legs' tube";
	lpInfo->m_strPropHelp = "length and slenderness ration threshold  of pressure legs' tube";
#else
	lpInfo->m_strPropName = "受压主材长细比阈值";
	lpInfo->m_strPropHelp = "受压主材长细比阈值";
#endif
	lpInfo->m_strPropValue = "150";
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_nMainPoleSlendernessLimit");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "length and slenderness ration threshold  of pressure tilted material";
	lpInfo->m_strPropHelp = "length and slenderness ration threshold  of pressure tilted material";
#else
	lpInfo->m_strPropName = "受压斜材长细比阈值";
	lpInfo->m_strPropHelp = "受压斜材长细比阈值";
#endif
	lpInfo->m_strPropValue = "220";
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_nXiePoleSlendernessLimit");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "length and slenderness ration threshold  of pressure auxiliary steel";
	lpInfo->m_strPropHelp = "length and slenderness ration threshold  of pressure auxiliary steel";
#else
	lpInfo->m_strPropName = "受压辅助材长细比阈值";
	lpInfo->m_strPropHelp = "受压辅助材长细比阈值";
#endif
	lpInfo->m_strPropValue = "250";
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_nAuxPoleSlendernessLimit");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "length and slenderness ration threshold  of the pulled steel";
	lpInfo->m_strPropHelp = "length and slenderness ration threshold  of the pulled steel";
#else
	lpInfo->m_strPropName = "受拉材长细比阈值";
	lpInfo->m_strPropHelp = "受拉材长细比阈值";
#endif
	lpInfo->m_strPropValue = "400";
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_nTensivePoleSlendernessLimit");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
#endif
}

void CLDSView::DisplaySysSettingDrawingSubSectionProperty(int iSectionType)
{
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem* pRootItem=pPropList->GetRootItem();
	CPropTreeItem* pPropItem=NULL,*pParentItem=NULL,*pBasicInfoItem=NULL;
	CPropertyListOper<CLDSApp> oper(pPropList,&theApp);
	//基本信息
	pBasicInfoItem=oper.InsertPropItem(pRootItem,"DrawingInfo");
	pBasicInfoItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pBasicInfoItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pBasicInfoItem->m_idProp)!=-1)
		pBasicInfoItem->m_bHideChildren=CLDSApp::GetPropStatus(pBasicInfoItem->m_idProp);
	//定位绘图程序
	oper.InsertFilePathPropItem(pBasicInfoItem,"env.m_sAcadPath");
	//火曲间隙
	oper.InsertEditPropItem(pBasicInfoItem,"g_sysPara.HuoQuDist");
	//角钢对接间隙
	oper.InsertEditPropItem(pBasicInfoItem,"g_sysPara.JgEndDist");
	//接头与板间隙
	oper.InsertEditPropItem(pBasicInfoItem,"g_sysPara.JointPlankDist");
	//顶点间隙
	oper.InsertEditPropItem(pBasicInfoItem,"g_sysPara.VertexDist");
	//材质字符的添加位置
	pPropItem=oper.InsertCmbListPropItem(pBasicInfoItem,"g_sysPara.m_nMatCharPosType");
	if(g_sysPara.m_nMatCharPosType>0)	//材质字符间隔符
		oper.InsertCmbEditPropItem(pPropItem,"g_sysPara.m_cMatCharSeparator");
	//构件段号编号同步更新
	oper.InsertCmbListPropItem(pBasicInfoItem,"CLDSPart::UPDATE_PARTNO_AND_SEG");
	//提示框开关
	oper.InsertCmbListPropItem(pBasicInfoItem,"g_sysPara.m_bEnablePrompt");
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
	//钢管与插板参数
	pParentItem=oper.InsertPropItem(pRootItem,"g_sysPara.TubeParam");
	pParentItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pParentItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pParentItem->m_idProp)!=-1)
		pParentItem->m_bHideChildren=CLDSApp::GetPropStatus(pParentItem->m_idProp);
	//修改对接法兰钢管正负头
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.m_bModifyCoupledFLTubeOddment");
	//法兰与钢管间隙
	oper.InsertEditPropItem(pParentItem,"g_sysPara.TubeFlDist");
	//法兰与钢管间隙
	oper.InsertCmbListPropItem(pParentItem,"CLDSLineTube::ciFlTubeOddment");
	//钢管存在焊道的最小直径
	oper.InsertEditPropItem(pParentItem,"g_sysPara.fTubeWeldRoadMinD");
	//自动调整钢管位置
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.nAutoAdjustTubePos");
#endif
	//钢板参数
	pParentItem=oper.InsertPropItem(pRootItem,"g_sysPara.PlateParam");
	pParentItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pParentItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pParentItem->m_idProp)!=-1)
		pParentItem->m_bHideChildren=CLDSApp::GetPropStatus(pParentItem->m_idProp);
	//连接板应力扩散角
	oper.InsertEditPropItem(pParentItem,"g_sysPara.fStressDiffuseAngle");
	//插板边距
	oper.InsertEditPropItem(pParentItem,"g_sysPara.fInsertPlateEdgeSpace");
	//连接板邻边最小角度
	oper.InsertEditPropItem(pParentItem,"g_sysPara.m_nPlankEdgeAngleThreshold");
	//钢板外形生成算法
	oper.InsertCmbListPropItem(pParentItem,"m_iProfileAlgorithm");
	//同步删除钢板螺栓
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.m_bDelBoltsResideOnPlate");
	//角钢参数
	pParentItem=oper.InsertPropItem(pRootItem,"g_sysPara.AngleParam");
	pParentItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pParentItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pParentItem->m_idProp)!=-1)
		pParentItem->m_bHideChildren=CLDSApp::GetPropStatus(pParentItem->m_idProp);
	//角钢连材对话框，默认光标位置选项
	pPropItem=oper.InsertPopMenuItem(pParentItem,"g_sysPara.ciLinkAngleDlgFocusCtrlMode");
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp=(g_sysPara.ciLinkAngleDlgFocusCtrlMode==0);//智能判断
	pPropItem->m_lpNodeInfo->SetCheck(g_sysPara.ciLinkAngleDlgFocusCtrlMode+1);
	//开合角阈值 wht 11-05-06
	oper.InsertEditPropItem(pParentItem,"g_sysPara.m_fKaiHeJiaoThreshold");
	//开合角外侧最小间隙
	oper.InsertEditPropItem(pParentItem,"g_sysPara.m_fKaiHeJiaoMinDeita");
	//组合角钢的加工间隙
	oper.InsertEditPropItem(pParentItem,"g_sysPara.m_nGroupJgMachiningGap");
	//最小切角值
	oper.InsertEditPropItem(pParentItem,"g_sysPara.m_fMinCutAngleLen");
	//最大忽略切角值
	oper.InsertEditPropItem(pParentItem,"g_sysPara.m_fMaxIgnoreCutAngleLen");
	//切角是否考虑圆弧占比系数
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.m_b45CutAngleNeedCoefR");
	//切角间隙内圆弧占比系数
	oper.InsertCmbEditPropItem(pParentItem,"g_sysPara.m_fCutAngleCoefR");
	//角钢默认定位方式
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.m_bAnglePreferSimplePosMode");
	//螺栓及脚钉参数
	pParentItem=oper.InsertPropItem(pRootItem,"g_sysPara.BoltParam");
	pParentItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pParentItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pParentItem->m_idProp)!=-1)
		pParentItem->m_bHideChildren=CLDSApp::GetPropStatus(pParentItem->m_idProp);
	//脚钉间距
	oper.InsertEditPropItem(pParentItem,"g_sysPara.FootNailDist");
	//脚钉代孔范围
	oper.InsertEditPropItem(pParentItem,"g_sysPara.fNailRepScope");
	//单螺栓设计过程显示添加垫板的框
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.b1BoltDesignPrompt");
	//螺栓通厚计算执行规则 wht 19-08-23
	oper.InsertCmbListPropItem(pParentItem, "CLDSBolt::BOLTL0_CALMODE");
	//螺栓通厚计算规则初始值 wht 19-08-23
	oper.InsertCmbListPropItem(pParentItem, "CLDSBolt::BOLTL0_PREFER_MODE");
	//垫圈最大叠加数量(暂时未启用 等下一个版本完善相关逻辑代码后启用 wjh-2019.9.3)
	//pPropItem=oper.InsertEditPropItem(pParentItem,"MaxWasherN");
	//单螺栓垫圈最小厚度
	pPropItem=oper.InsertEditPropItem(pParentItem,"BoltPadThick");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//M12
	oper.InsertEditPropItem(pPropItem,"g_sysPara.BoltPadThick.m_nM12");
	//M16
	oper.InsertEditPropItem(pPropItem,"g_sysPara.BoltPadThick.m_nM16");
	//M20
	oper.InsertEditPropItem(pPropItem,"g_sysPara.BoltPadThick.m_nM20");
	//M24
	oper.InsertEditPropItem(pPropItem,"g_sysPara.BoltPadThick.m_nM24");	
#ifdef __SMART_DRAW_INC_
	//智能连接设计参数
	pParentItem=oper.InsertGrayButtonPropItem(pRootItem,"g_sysPara.intelligentConnect");
	pParentItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pParentItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pParentItem->m_idProp)!=-1)
		pParentItem->m_bHideChildren=CLDSApp::GetPropStatus(pParentItem->m_idProp);
	//仅进行设计当前视图内连接
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.intelligentConnect.m_bOnlyDesignCurView");
	//进行智能普通连接设计参数
	pPropItem=oper.InsertCmbListPropItem(pParentItem,"g_sysPara.intelligentConnect.m_bDesignCommonConnect");
	pPropItem->m_bHideChildren=FALSE;
	if(g_sysPara.intelligentConnect.m_bDesignCommonConnect)	//进行智能普通连接设计参数
		oper.InsertCmbListPropItem(pPropItem,"g_sysPara.intelligentConnect.m_bDesignBendConnect");
	//进行智能角钢接头设计参数
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.intelligentConnect.m_bDesignAngleJoint");
	//进行智能钢管法兰设计参数
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.intelligentConnect.m_bDesignFL");
	//进行智能角钢接头设计参数
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate");
#endif
	//关联对象同步属性
	pParentItem=oper.InsertPropItem(pRootItem,"SyncProp");
	pParentItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pParentItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pParentItem->m_idProp)!=-1)
		pParentItem->m_bHideChildren=CLDSApp::GetPropStatus(pParentItem->m_idProp);
	//节点
	oper.InsertButtonPropItem(pParentItem,"SyncProp.Node");
	//角钢
	oper.InsertButtonPropItem(pParentItem,"SyncProp.Angle");
#ifdef __COMPLEX_PART_INC_	
	//钢管
	oper.InsertButtonPropItem(pParentItem,"SyncProp.Tube");
#endif
	//钢板
	oper.InsertButtonPropItem(pParentItem,"SyncProp.Plate");
}