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
		if(valueStr.CompareNoCase("���ļ�����")==0)
			g_sysPara.iAutoSaveType = 0;
		else if(valueStr.CompareNoCase("���ļ�����")==0)
#endif
			g_sysPara.iAutoSaveType = 1;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.uFileSaveVersion"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Program Version")==0)
#else 
		if(valueStr.CompareNoCase("����汾")==0)
#endif
			g_sysPara.uFileSaveVersion=0;
		else
		{	//��ȡ�汾��
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
		UINT FileRW_BUFFSIZE=atoi(valueStr);	//*0x100000,MBΪ��λ��Ĭ��50M����ռ�
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
		if(valueStr.Compare("����")==0)
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
		if(valueStr.Compare("����")==0)
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
		if(valueStr.Compare("����")==0)
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
		if(valueStr.CompareNoCase("V1 ���㷨")==0)
			Ta.m_ciRepositionAlgor=1;
		else if(valueStr.CompareNoCase("V2 ���㷨")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_NODE);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_NODE)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINEANGLE);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINEANGLE)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINETUBE);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINETUBE)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_PLATE);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_PLATE)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_PARAMPLATE);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_PARAMPLATE)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_BOLT);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_BOLT)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_SPHERE);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_SPHERE)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINESLOT);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINESLOT)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_LINEFLAT);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_LINEFLAT)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_ARCANGLE);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_ARCANGLE)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_ARCFLAT);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_ARCFLAT)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_ARCSLOT);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_ARCSLOT)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_dwSelectPartsType |= GetSingleWord(SELECTINDEX_STDPART);
		else if(valueStr.CompareNoCase("��")==0&&(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_STDPART)))
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_bCanSelectEdgeLine = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			g_sysPara.m_bCanSelectEdgeLine = FALSE;
	}
	//��ʽˢ����ѡ��
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.brush.bMaterial"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			g_sysPara.brush.bMaterial = TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.brush.bMaterial = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.brush.bLayer = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.brush.bGuige = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.brush.bSeg = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.brush.bCfgword = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.brush.bDisplayHole = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.brush.bVirtualPart = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.brush.bColour = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bPartSketch = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bNodePnt = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
				lpInfo->m_strPropName = "�ڵ���ʾ��С";
				lpInfo->m_strPropHelp = "�ڵ���ʾ��С";
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
				lpInfo->m_strPropName = "�ڵ���ʾ����";
				lpInfo->m_strPropHelp = "�ڵ���ʾ����";
				lpInfo->m_cmbItems = "0.����|1.Բ��";
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bPoleLine = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			CLDSLinePart::bDisplayLineVecMark = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("Ԥ��ǰ")==0)
			CLDSNode::m_bDisplayBeforeArcLift = TRUE;
		else if(valueStr.CompareNoCase("Ԥ����")==0)
#endif
			CLDSNode::m_bDisplayBeforeArcLift = FALSE;
		g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.bWirePoint"))
	{	//���ؼ�ͷ
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else 
		if(valueStr.Compare("��")==0)
#endif
			g_sysPara.display.bWirePoint=TRUE;
		else
			g_sysPara.display.bWirePoint=FALSE;
		g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.bLoadArrow"))
	{	//���ؼ�ͷ
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else 
		if(valueStr.Compare("��")==0)
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
				lpInfo->m_strPropName = "���ؼ�ͷ��С";
				lpInfo->m_strPropHelp = "���ؼ�ͷ��С";
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
	{	//�ڵ��ű�ע
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else 
		if(valueStr.Compare("��")==0)
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
				lpInfo->m_strPropName = "�ڵ����ָ�";
				lpInfo->m_strPropHelp = "�ڵ����ָ�";
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
	{	//�ڵ��ű�ע
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else 
		if(valueStr.Compare("��")==0)
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
				lpInfo->m_strPropName = "�˼������Ϣ�ָ�";
				lpInfo->m_strPropHelp = "�˼������Ϣ�ָ�";
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
	{	//�ֲ��Ŵ�λʱ�����������ʾ����
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bDispVirtualPart = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidJg = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidTube = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidLjb = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidJoint = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidLs = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidSlot = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidSphere = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidFlat = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidArcAngle = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidArcFlat = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidArcSlot = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidStdPart = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidFoundation = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			CLDSLineTube::bDisplaySolidTransectFace=TRUE;	//g_sysPara.display.bSolidTransectFace = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			CLDSLineTube::bDisplaySolidTransectFace=FALSE;	//g_sysPara.display.bSolidTransectFace = FALSE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("CLDSLineAngle::m_bDisplaySolidAxisLine"))
	{	//��ʾ�Ǹ�����
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
			CLDSLineAngle::m_bDisplaySolidAxisLine=TRUE;
		else if(valueStr.CompareNoCase("No")==0)
#else 
		if(valueStr.CompareNoCase("��")==0)
			CLDSLineAngle::m_bDisplaySolidAxisLine=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.display.bSolidLine = TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("�ڵ�")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crNode);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Angle(Tube) Single Line")==0)
#else 
		else if(valueStr.CompareNoCase("�Ǹ�(�ֹ�)����")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crLine);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Angle(Tube)")==0)
#else 
		else if(valueStr.CompareNoCase("�Ǹ�(�ֹ�)")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crPole);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Angle X Leg")==0)
#else 
		else if(valueStr.CompareNoCase("�Ǹ�X֫")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crAngleWingX);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Angle Y Leg")==0)
#else 
		else if(valueStr.CompareNoCase("�Ǹ�Y֫")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crAngleWingY);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Angle Leg")==0)
#else 
		else if(valueStr.CompareNoCase("�Ǹ�֫��")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crAngleEdge);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("No.1 Child Angle X Leg")==0)
#else 
		else if(valueStr.CompareNoCase("1���ӽǸ�X֫")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crSonAngle1WingX);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("No.1 Child Angle Y Leg")==0)
#else 
		else if(valueStr.CompareNoCase("1���ӽǸ�Y֫")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crSonAngle1WingY);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("No.1 Child Angle Leg")==0)
#else 
		else if(valueStr.CompareNoCase("1���ӽǸ�֫��")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crSonAngle1Edge);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Other Bolts")==0)
#else 
		else if(valueStr.CompareNoCase("������˨")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crOtherBolt);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M12")==0)
#else 
		else if(valueStr.CompareNoCase("��˨M12")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt12);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M16")==0)
#else 
		else if(valueStr.CompareNoCase("��˨M16")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt16);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M20")==0)
#else 
		else if(valueStr.CompareNoCase("��˨M20")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt20);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M24")==0)
#else 
		else if(valueStr.CompareNoCase("��˨M24")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt24);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M27")==0)
#else 
		else if(valueStr.CompareNoCase("��˨M27")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt27);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M30")==0)
#else 
		else if(valueStr.CompareNoCase("��˨M30")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt30);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M36")==0)
#else 
		else if(valueStr.CompareNoCase("��˨M36")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt36);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M39")==0)
#else 
		else if(valueStr.CompareNoCase("��˨M39")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt39);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M42")==0)
#else 
		else if(valueStr.CompareNoCase("��˨M42")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt42);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bolt M48")==0)
#else 
		else if(valueStr.CompareNoCase("��˨M48")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBolt48);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Connecting Plate")==0)
#else 
		else if(valueStr.CompareNoCase("���Ӱ�")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crPlate);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Standard Part")==0)
#else 
		else if(valueStr.CompareNoCase("��׼����")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crPlate);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Bend Segment/Cut Segment")==0)
#else 
		else if(valueStr.CompareNoCase("����/�и��")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crCutFaceOrPolyJoint);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Main Background")==0)
#else 
		else if(valueStr.CompareNoCase("������")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crBackground);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Supporting Background")==0)
#else 
		else if(valueStr.CompareNoCase("������")==0)
#endif
		{
			clrStr.Format("RGB%X", g_sysPara.crPartMode.crPartBackground);
			pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"), clrStr);
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("partColorName"))
	{
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&curClr);
		/*if(curClr==0xFFFFFFFF)
		{	
		CColorDialog dlg(RGB(0,0,0));
		dlg.DoModal();
		curClr = dlg.GetColor();

		int iCur = m_CurObjColor.IsHaveThisColor(curClr);
		if(iCur==-1)
		{
		iCur = m_CurObjColor.AddColor(curClr,"�Զ���",FALSE);
		m_CurObjColor.SetCurSel(iCur);
		}
		else
		m_CurObjColor.SetCurSel(iCur);

		}*/
		pPropList->GetItemPropValue(CLDSApp::GetPropID("partName"), partName);
#ifdef AFX_TARG_ENU_ENGLISH
		if(partName.CompareNoCase("Node")==0)
#else 
		if(partName.CompareNoCase("�ڵ�")==0)
#endif
		{
			if(g_sysPara.crPartMode.crNode!=curClr)
				g_sysPara.crPartMode.crNode=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Angle(Tube) Single Line")==0)
#else 
		else if(partName.CompareNoCase("�Ǹ�(�ֹ�)����")==0)
#endif
		{
			if(g_sysPara.crPartMode.crLine!=curClr)
				g_sysPara.crPartMode.crLine=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Angle(Tube)")==0)
#else 
		else if(partName.CompareNoCase("�Ǹ�(�ֹ�)")==0)
#endif
		{
			if(g_sysPara.crPartMode.crPole!=curClr)
				g_sysPara.crPartMode.crPole=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Angle X Leg")==0)
#else 
		else if(partName.CompareNoCase("�Ǹ�X֫")==0)
#endif
		{
			if(g_sysPara.crPartMode.crAngleWingX!=curClr)
				g_sysPara.crPartMode.crAngleWingX=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Angle Y Leg")==0)
#else 
		else if(partName.CompareNoCase("�Ǹ�Y֫")==0)
#endif
		{
			if(g_sysPara.crPartMode.crAngleWingY!=curClr)
				g_sysPara.crPartMode.crAngleWingY=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Angle Leg")==0)
#else 
		else if(partName.CompareNoCase("�Ǹ�֫��")==0)
#endif
		{
			if(g_sysPara.crPartMode.crAngleEdge!=curClr)
				g_sysPara.crPartMode.crAngleEdge=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("No.1 Child Angle X Leg")==0)
#else 
		else if(partName.CompareNoCase("1���ӽǸ�X֫")==0)
#endif
		{
			if(g_sysPara.crPartMode.crSonAngle1WingX!=curClr)
				g_sysPara.crPartMode.crSonAngle1WingX=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("No.1 Child Angle Y Leg")==0)
#else 
		else if(partName.CompareNoCase("1���ӽǸ�Y֫")==0)
#endif
		{
			if(g_sysPara.crPartMode.crSonAngle1WingY!=curClr)
				g_sysPara.crPartMode.crSonAngle1WingY=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("No.1 Child Angle Leg")==0)
#else 
		else if(partName.CompareNoCase("1���ӽǸ�֫��")==0)
#endif
		{
			if(g_sysPara.crPartMode.crSonAngle1Edge!=curClr)
				g_sysPara.crPartMode.crSonAngle1Edge=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Other Bolts")==0)
#else 
		else if(partName.CompareNoCase("������˨")==0)
#endif
		{
			if(g_sysPara.crPartMode.crOtherBolt!=curClr)
				g_sysPara.crPartMode.crOtherBolt=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M12")==0)
#else 
		else if(partName.CompareNoCase("��˨M12")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt12!=curClr)
				g_sysPara.crPartMode.crBolt12=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M16")==0)
#else 
		else if(partName.CompareNoCase("��˨M16")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt16!=curClr)
				g_sysPara.crPartMode.crBolt16=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M20")==0)
#else 
		else if(partName.CompareNoCase("��˨M20")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt20!=curClr)
				g_sysPara.crPartMode.crBolt20=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M24")==0)
#else 
		else if(partName.CompareNoCase("��˨M24")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt24!=curClr)
				g_sysPara.crPartMode.crBolt24=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M27")==0)
#else 
		else if(partName.CompareNoCase("��˨M27")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt27!=curClr)
				g_sysPara.crPartMode.crBolt27=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M30")==0)
#else 
		else if(partName.CompareNoCase("��˨M30")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt30!=curClr)
				g_sysPara.crPartMode.crBolt30=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M36")==0)
#else 
		else if(partName.CompareNoCase("��˨M36")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt36!=curClr)
				g_sysPara.crPartMode.crBolt36=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M39")==0)
#else 
		else if(partName.CompareNoCase("��˨M39")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt39!=curClr)
				g_sysPara.crPartMode.crBolt39=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M42")==0)
#else 
		else if(partName.CompareNoCase("��˨M42")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt42!=curClr)
				g_sysPara.crPartMode.crBolt42=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bolt M48")==0)
#else 
		else if(partName.CompareNoCase("��˨M48")==0)
#endif
		{
			if(g_sysPara.crPartMode.crBolt48!=curClr)
				g_sysPara.crPartMode.crBolt48=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Connecting Plate")==0)
#else 
		else if(partName.CompareNoCase("���Ӱ�")==0)
#endif
		{
			if(g_sysPara.crPartMode.crPlate!=curClr)
				g_sysPara.crPartMode.crPlate=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Bend Segment/Cut Segment")==0)
#else 
		else if(partName.CompareNoCase("����/�и��")==0)
#endif
		{
			if(g_sysPara.crPartMode.crCutFaceOrPolyJoint!=curClr)
				g_sysPara.crPartMode.crCutFaceOrPolyJoint=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Standard Part")==0)
#else 
		else if(partName.CompareNoCase("��׼����")==0)
#endif
		{
			if(g_sysPara.crPartMode.crStdPart!=curClr)
				g_sysPara.crPartMode.crStdPart=curClr;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(partName.CompareNoCase("Main Background")==0)
#else 
		else if(partName.CompareNoCase("������")==0)
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
		else if(partName.CompareNoCase("������")==0)
#endif
		{
			if(g_sysPara.crPartMode.crPartBackground!=curClr)
			{
				g_sysPara.crPartMode.crPartBackground=curClr;
				g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crPartBackground);
			}
		}
		//�����ʱˢ����֧����껬�ֹ��� wht 08-06-26
		//pView->RedrawAll();	//��ʱ�����Ҳ���ʾ����ɫ
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs"))
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Visible")==0)
			theApp.env.m_bDisplayWorldUcs = TRUE;
		else if(valueStr.CompareNoCase("Invisible")==0)
#else 
		if(valueStr.CompareNoCase("��ʾ")==0)
			theApp.env.m_bDisplayWorldUcs = TRUE;
		else if(valueStr.CompareNoCase("����")==0)
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
		if(valueStr.CompareNoCase("��ʾ")==0)
			g_sysPara.m_bDispPartACS = TRUE;
		else if(valueStr.CompareNoCase("����")==0)
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
		if(valueStr.CompareNoCase("��ʾ")==0)
			g_sysPara.m_bDispWorkUcs = TRUE;
		else if(valueStr.CompareNoCase("����")==0)
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
		if(valueStr.CompareNoCase("��ʾ")==0)
			g_sysPara.m_bDispBlockLCS = TRUE;
		else if(valueStr.CompareNoCase("����")==0)
#endif
			g_sysPara.m_bDispBlockLCS = FALSE;
		if(!console.GetActiveModel()->IsTowerModel())
		{	//��������λ����ϵid=3
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
		if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("�Զ��趨")==0)
			g_sysPara.ciLinkAngleDlgFocusCtrlMode=0;
		else if(valueStr.CompareNoCase("��������")==0)
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
		if(valueStr.CompareNoCase("���ƫ��")==0)
			g_sysPara.nAutoAdjustTubePos = 1;
		else if(valueStr.CompareNoCase("�ֹ�ƫ��")==0)
#endif
			g_sysPara.nAutoAdjustTubePos = 2;
		else //if(valueStr.CompareNoCase("������")==0)
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
		g_sysPara.m_fKaiHeJiaoThreshold = atof(valueStr);	//���Ͻ���ֵ Ĭ��Ϊ2��
		//����ע������� wht 11-06-13
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
		//����ע�������
		AfxGetApp()->WriteProfileInt(_T("Settings"),_T("MatCharPosType"),g_sysPara.m_nMatCharPosType);
		//
		pPropList->DeleteAllSonItems(pItem);
		if(g_sysPara.m_nMatCharPosType>0)
			oper.InsertCmbEditPropItem(pItem,"g_sysPara.m_cMatCharSeparator","","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_cMatCharSeparator"))
	{
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.m_cMatCharSeparator=0;
		else if(valueStr.CompareNoCase("�ո�")==0)
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
		{	//�����޷�ֹ���ֵ��Ӧ�������ɸֹ�ʵ�� wht 11-08-29
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
		if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��������λ")==0)
			g_sysPara.m_bAnglePreferSimplePosMode=FALSE;	
		else //if(valueStr.CompareNoCase("�򵥶�λ")==0)
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
		if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.intelligentConnect.m_bOnlyDesignCurView=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.intelligentConnect.m_bDesignCommonConnect=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
				lpInfo->m_cmbItems = "��|��";
				lpInfo->m_strPropName = "����������������";
				lpInfo->m_strPropHelp = "�������ʱ������׼�����߲�������������";
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.intelligentConnect.m_bDesignBendConnect=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.intelligentConnect.m_bDesignAngleJoint=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.intelligentConnect.m_bDesignFL=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
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
		if(valueStr.CompareNoCase("��")==0)
			g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate=TRUE;
		else if(valueStr.CompareNoCase("��")==0)
#endif
			g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate=FALSE;
	}
	else
	{
		bContinueFind=TRUE;
		if(Ta.hashParams.GetNodeNum()>0)   //��Ʋ���
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
	//if else��ι����ڴ˴��
	if(!bContinueFind)
		return TRUE;
	else	//if(bContinueFind)
	{	//���
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
			if(valueStr.Compare("ƽ����·")==0)
				Ta.m_cLineRoadType=0;
			else if(valueStr.Compare("������·")==0)
#endif
				Ta.m_cLineRoadType=1;
			else//if(valueStr.Compare("ɽ����·")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("0.���ֻ�")==0)
				g_sysPara.iLegShiftType=0;
			else if(valueStr.CompareNoCase("1.�ȸ����ֻ�")==0)
#endif
				g_sysPara.iLegShiftType=1;
			else// if(valueStr.CompareNoCase("2.�ߵ����ֻ�")==0)
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
#ifndef __LDS_TDA_	//����LDS�Ľ���ͼ��ģʽ
		else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.iShowDeformedMapPlan"))
		{
			g_sysPara.iShowDeformedMapPlan=valueStr[0]-'0';
			if(g_sysPara.iShowDeformedMapPlan>0)
			{
				((CMainFrame*)AfxGetMainWnd())->m_bDisplayDeformedTower=TRUE;
				theApp.m_eEnvType=FEMELEM_ENV;	//ת����������Ԥ����ʽʱ�Զ�ת������Ԫģʽ
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
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
			if(valueStr.CompareNoCase("��")==0)
#endif
			{
				g_sysPara.bCreateDetailRptFile=TRUE;
				pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iDetailDocType"),&pParentItem);
				if(pFindItem==NULL)
				{	//��ϸ��������ĵ�����
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "document type";
					lpInfo->m_strPropHelp = "document type";
					lpInfo->m_cmbItems = "0.Text Document|1.EXCEL Form";
#else
					lpInfo->m_strPropName = "�ĵ�����";
					lpInfo->m_strPropHelp = "�ĵ�����";
					lpInfo->m_cmbItems = "0.�ı��ĵ�|1.EXCEL���";
#endif
					pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
					pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iDetailDocType");
					pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

				pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iLanguage"),&pParentItem);
				if(pFindItem==NULL)
				{	//��ϸ��������ĵ�����
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Output language";
					lpInfo->m_strPropHelp = "Computer output language";
					lpInfo->m_cmbItems = "0.Chinese|1.English";
#else
					lpInfo->m_strPropName = "�������";
					lpInfo->m_strPropHelp = "�������������";
					lpInfo->m_cmbItems = "0.����|1.Ӣ��";
#endif
					pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
					pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iLanguage");
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

				pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.bMergeRptFileMirObj"),&pParentItem);
				if(pFindItem==NULL)
				{	//��ϸ��������ĵ�����
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Merge symmetrical items";
                    lpInfo->m_strPropHelp = "Merge symmetrical nodes and the information output of rods";
                    lpInfo->m_cmbItems = "Yes|No";
#else
					lpInfo->m_strPropName = "�ϲ��Գ���";
					lpInfo->m_strPropHelp = "�ϲ��Գƽڵ�͸˼�����Ϣ���";
					lpInfo->m_cmbItems = "��|��";
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
					lpInfo->m_strPropName = "ԣ��ϵ������";
					lpInfo->m_strPropHelp = "����ϵ����ֻ������йص�ԣ��ϵ���������ԣ����������˳�ϸ������ԣ�ȡ�";
					lpInfo->m_cmbItems = "0.����+��ϸ��ϵ��|1.����ϵ��";
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
				lpInfo->m_strPropName = "��������";
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
					lpInfo->m_strPropName = "M��ֵ";
					lpInfo->m_strPropHelp = "��ؾ���ֵ";
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
					lpInfo->m_strPropName = "��M threshold value";
					lpInfo->m_strPropHelp = "Bending stress from bending moment";
#else
					lpInfo->m_strPropName = "��M��ֵ";
					lpInfo->m_strPropHelp = "����ز���������Ӧ��";
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
					lpInfo->m_strPropName = "��M %threshold value";
					lpInfo->m_strPropHelp = "percentage for the bending stress from bending moment in stress produced by axial force";
#else
					lpInfo->m_strPropName = "��M %��ֵ";
					lpInfo->m_strPropHelp = "����ز���������Ӧ��ռ��������Ӧ���İٷֱ�";
#endif
					pFindItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
					pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nPrecentSigemaM");
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_ANALYSIS);
				}
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

				pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.reactionExportType"),&pParentItem);
				if(pFindItem==NULL)
				{	//��ϸ��������ĵ�����
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "basic forces filtrating";
					lpInfo->m_strPropHelp = "When filtrating the level of high-low legs��only output the maximum pull-up force and down force from a module;when filtrating in a working condition level,only output all the pull-up force and down force from a moudle in  working condition ";
					lpInfo->m_cmbItems = "0.Full output|1.High-low legs level|2.Working condition level";
#else
					lpInfo->m_strPropName = "����������ɸѡ";
					lpInfo->m_strPropHelp = "�ߵ��ȼ�ɸѡʱ��ֻ���һ�������鹤�������иߵ����е�����ϲ�������ѹ��;������ɸѡʱֻ���һ�������������й������ϲ�������ѹ����";
					lpInfo->m_cmbItems = "0.ȫ�����|1.�ߵ��ȼ�|2.������";
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
			if(valueStr.Compare("��")==0)
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
					lpInfo->m_strPropName = "��������";
					lpInfo->m_strPropHelp = "ÿ�������������������������˳�����";
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
					lpInfo->m_strPropName = "������ֵ";
					lpInfo->m_strPropHelp = "��ѧ��������ʱ��ƽ����������ֵ,��λN";
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
			if(valueStr.CompareNoCase("��")==0)
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
				continue;	//������׼��λ�˼�
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
	sPrompt.Format("�޶���ɣ����޶�%d��ְ������!",i);
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
			pView->RenderAll();		//��ɫ�����޸ĺ����й����������»���
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("defaultColor"))
	{
		CString partName, clrStr;
		pPropList->GetItemPropValue(CLDSApp::GetPropID("partName"), partName);
		g_sysPara.crPartMode.crNode = RGB(255,255,255);
		g_sysPara.crPartMode.crControlNode = RGB(255,255,255);		  //���ƽڵ�
		g_sysPara.crPartMode.crDivideScaleNode = RGB(255,255,0);      //�ȷֽڵ�
		g_sysPara.crPartMode.crOffsetNode = RGB(128,128,0);   	      //ƫ�ƽڵ�
		g_sysPara.crPartMode.crAxisValFixNode= RGB(255,255,0);        //ָ����������ڵ�
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
		//��ʹ��Ĭ����ɫ������ʱ�������Կ���ʾ����Ӧ��������ɫ
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
		if(partName.CompareNoCase("�ڵ�")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crNode);
		else if(partName.CompareNoCase("���ƽڵ�")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crControlNode);
		else if(partName.CompareNoCase("�ȷֽڵ�")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crDivideScaleNode);
		else if(partName.CompareNoCase("ƫ�ƽڵ�")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crOffsetNode);
		else if(partName.CompareNoCase("ָ����������ڵ�")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAxisValFixNode);
		else if(partName.CompareNoCase("�Ǹ�(�ֹ�)����")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crLine);
		else if(partName.CompareNoCase("�Ǹ�(�ֹ�)")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crPole);
		else if(partName.CompareNoCase("�Ǹ�X֫")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAngleWingX);
		else if(partName.CompareNoCase("�Ǹ�Y֫")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAngleWingY);
		else if(partName.CompareNoCase("�Ǹ�֫��")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAngleEdge);
		else if(partName.CompareNoCase("1���ӽǸ�X֫")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1WingX);
		else if(partName.CompareNoCase("1���ӽǸ�Y֫")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1WingY);
		else if(partName.CompareNoCase("1���ӽǸ�֫��")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1Edge);
		else if(partName.CompareNoCase("������˨")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crOtherBolt);
		else if(partName.CompareNoCase("��˨M12")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt12);
		else if(partName.CompareNoCase("��˨M16")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt16);
		else if(partName.CompareNoCase("��˨M20")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt20);
		else if(partName.CompareNoCase("��˨M24")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt24);
		else if(partName.CompareNoCase("��˨M27")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt27);
		else if(partName.CompareNoCase("��˨M30")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt30);
		else if(partName.CompareNoCase("��˨M36")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt36);
		else if(partName.CompareNoCase("��˨M39")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt39);
		else if(partName.CompareNoCase("��˨M42")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt42);
		else if(partName.CompareNoCase("��˨M48")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt48);
		else if(partName.CompareNoCase("���Ӱ�")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crPlate);
		else if(partName.CompareNoCase("����/�и��")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crCutFaceOrPolyJoint);
		else if(partName.CompareNoCase("������")==0)
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBackground);
		else if(partName.CompareNoCase("������")==0)
#endif
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crPartBackground);
		pPropList->SetItemPropValue(CLDSApp::GetPropID("partColorName"),clrStr);
		pView->RedrawAll();	//��ʱ�����Ҳ���ʾ����ɫ
		return TRUE;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.mcs"))
	{
		CPartUcsDlg ucs_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
		ucs_dlg.m_sCaption="Model Coordinate System";
#else 
		ucs_dlg.m_sCaption="ģ������ϵ";
#endif
		ucs_dlg.m_bCanModifyOrigin=FALSE;
		ucs_dlg.ucs=theApp.env.mcs;
		if(ucs_dlg.DoModal()==IDOK)
			theApp.env.mcs=ucs_dlg.ucs;
	}
	/*else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDispPartACS"))
	{
		CPartUcsDlg ucs_dlg;
		ucs_dlg.m_sCaption="����װ������ϵ";
		ucs_dlg.m_bCanModifyOrigin=FALSE;
		ucs_dlg.ucs=theApp.env.ucs;
		if(ucs_dlg.DoModal()==IDOK)
			theApp.env.ucs=ucs_dlg.ucs;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDispWorkUcs"))
	{
		CPartUcsDlg ucs_dlg;
		ucs_dlg.m_sCaption="��������ϵ";
		ucs_dlg.m_bCanModifyOrigin=FALSE;
		ucs_dlg.ucs=theApp.env.ucs;
		if(ucs_dlg.DoModal()==IDOK)
			theApp.env.ucs=ucs_dlg.ucs;
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.m_bDispBlockLCS"))
	{
		CPartUcsDlg ucs_dlg;
		ucs_dlg.m_sCaption="������λ����ϵ";
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
		vecDlg.m_sCaption = "��׼����ϵ-X��";
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
		vecDlg.m_sCaption = "��׼����ϵ-Y��";
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
		vecDlg.m_sCaption = "��׼����ϵ-Z��";
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
	{	//�������������ز���
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
			{	//��˨���ϵ��
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMON;
				lpInfo->m_strPropName = pLsFamily->name;
				CPropTreeItem* pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp = CLDSApp::MAX_USER_PROPID+pLsFamily->GetFamilyHandle();
				pSonItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			//�ؽ���˨��
			oper.InsertButtonPropItem(pItem,"ANCHOR_BOLT_LIB","","",-1,TRUE);
		}
	}
	else if(pItem->m_idProp>CLDSApp::MAX_USER_PROPID)	//��˨���ϵ��
	{
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=4;
		long h=pItem->m_idProp-CLDSApp::MAX_USER_PROPID;
		dlg.m_pLsFamily=CLsLibrary::FromHandle(h);
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("ANCHOR_BOLT_LIB"))	//�ؽ���˨��
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
	{	//EB�Ұ��
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=10;
		dlg.m_iPartSubType=0;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sGDGuiGeTbl"))
	{	//GD�Ұ��
		CPartGuiGeLibDlg dlg;
		dlg.m_iPartType=10;
		dlg.m_iPartSubType=1;
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sURGuiGeTbl"))
	{	//U�ͻ���
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
		dlg.m_iContentsType=1;	//��ʾ����ϵ����ֵ��
		dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sInterpIceLoadCoefTbl"))
	{
		CSteelMatLibDlg dlg;
		dlg.m_iContentsType=3;	//��ʾ����������������ϵ����
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
	{	//��׼������
		CTubeEndLjStdPartLibDlg stdPartDlg;
		stdPartDlg.DoModal();
	}
	//��ʱ�����ɵĲ�弰�������¼����� wht 10-07-06
	else if(pItem->m_idProp==CLDSApp::GetPropID("InsertPlateLib"))
	{	//�ֹܹ�ͷ����
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
			Filter = "��ִ���ļ�(*.exe)|*.exe||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgZhunJuTblPath"))
			Filter = "�Ǹ�׼���ļ�(*.jgg)|*.jgg||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgGuiGeTblPath"))
			Filter = "�Ǹֹ���ļ�(*.jgt)|*.jgt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sJgJointParaTblPath"))
			Filter = "�Ǹֽ�ͷ���ļ�(*.jjp)|*.jjp||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sTubeGuiGeTblPath"))
			Filter = "�ֹܹ���ļ�(*.pgt)|*.pgt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sFlatGuiGeTblPath"))
			Filter = "��������ļ�(*.bia)|*.bia||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sSlotGuiGeTblPath"))
			Filter = "�۸ֹ���ļ�(*.cgt)|*.cgt||";
#ifdef __H_SHAPE_STEEL_
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHWGuiGeTblPath"))
			Filter = "HW�͸ֹ���ļ�(*.hwgt)|*.hwgt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHMGuiGeTblPath"))
			Filter = "HM�͸ֹ���ļ�(*.hmgt)|*.hmgt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHNGuiGeTblPath"))
			Filter = "HN�͸ֹ���ļ�(*.hngt)|*.hngt||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sHPGuiGeTblPath"))
			Filter = "HP�͸ֹ���ļ�(*.hpgt)|*.hpgt||";
#endif
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sLsSpaceTblPath"))
			Filter = "��˨����ļ�(*.lss)|*.lss||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sCInsertPlateLibPath"))
			Filter = "���Ͳ����ļ�(*.cipl)|*.cipl||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sUInsertPlateLibPath"))
			Filter = "U�Ͳ����ļ�(*.uipl)|*.uipl||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sXInsertPlateLibPath"))
			Filter = "ʮ���Ͳ����ļ�(*.xipl)|*.xipl||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlDLibPath"))
			Filter = "�Ժ����������ļ�(*.fld)|*.fld||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlPLibPath"))
			Filter = "ƽ�����������ļ�(*.flp)|*.flp||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlGLibPath"))
			Filter = "���Է��������ļ�(*.flg)|*.flg||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sStdPartFlRLibPath"))
			Filter = "���Է��������ļ�(*.flr)|*.flr||";
		else if(pItem->m_idProp==CLDSApp::GetPropID("env.m_sFlCoupleLevelPath"))
			Filter = "�Խӷ����������ļ�(*.fll)|*.fll||";
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
	//��ʽˢ����ѡ��
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
				lpInfo->m_strPropName = "�ڵ���ʾ��С";
				lpInfo->m_strPropHelp = "�ڵ���ʾ��С";
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
				lpInfo->m_strPropName = "�ڵ���ʾ����";
				lpInfo->m_strPropHelp = "�ڵ���ʾ����";
				lpInfo->m_cmbItems = "0.����|1.Բ��";
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
		g_sysPara.display.bWirePoint=bCheck;	//�ҵ��ͷ
	else if(pItem->m_idProp == CLDSApp::GetPropID("g_sysPara.display.bLoadArrow"))
	{	//���ؼ�ͷ
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
				lpInfo->m_strPropName = "���ؼ�ͷ��С";
				lpInfo->m_strPropHelp = "���ؼ�ͷ��С";
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
		CLDSLineAngle::m_bDisplaySolidAxisLine=bCheck; //��ʾ�Ǹ�����
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
	//���
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
			{	//��ϸ��������ĵ�����
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "document type";
				lpInfo->m_strPropHelp = "document type";
				lpInfo->m_cmbItems = "0.Text Document|1.EXCEL Form"
#else
				lpInfo->m_strPropName = "�ĵ�����";
				lpInfo->m_strPropHelp = "�ĵ�����";
				lpInfo->m_cmbItems = "0.�ı��ĵ�|1.EXCEL���";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iDetailDocType");
				pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

			pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.iLanguage"),&pParentItem);
			if(pFindItem==NULL)
			{	//��ϸ��������ĵ�����
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "output language";
				lpInfo->m_strPropHelp = "Output language of computer";
				lpInfo->m_cmbItems = "0.Chinese|1.English";
#else
				lpInfo->m_strPropName = "�������";
				lpInfo->m_strPropHelp = "�������������";
				lpInfo->m_cmbItems = "0.����|1.Ӣ��";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iLanguage");
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

			pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.bMergeRptFileMirObj"),&pParentItem);
			if(pFindItem==NULL)
			{	//��ϸ��������ĵ�����
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
    				lpInfo->m_strPropName = "Merge symmetric items";
				lpInfo->m_strPropHelp = "Merge symmetric nodes and the output information of rods";
				lpInfo->m_cmbItems = "Yes|No";
#else
				lpInfo->m_strPropName = "�ϲ��Գ���";
				lpInfo->m_strPropHelp = "�ϲ��Գƽڵ�͸˼�����Ϣ���";
				lpInfo->m_cmbItems = "��|��";
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
				lpInfo->m_strPropName = "ԣ��ϵ������";
				lpInfo->m_strPropHelp = "����ϵ����ֻ������йص�ԣ��ϵ���������ԣ����������˳�ϸ������ԣ�ȡ�";
				lpInfo->m_cmbItems = "0.����+��ϸ��ϵ��|1.����ϵ��";
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
			lpInfo->m_strPropName = "��������";
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
				lpInfo->m_strPropName = "M��ֵ";
				lpInfo->m_strPropHelp = "��ؾ���ֵ";
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
    				lpInfo->m_strPropName = "��M threshold value";
				lpInfo->m_strPropHelp = "bending stress from bending moment";
#else
				lpInfo->m_strPropName = "��M��ֵ";
				lpInfo->m_strPropHelp = "����ز���������Ӧ��";
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
    			        lpInfo->m_strPropName = "��M % threshold value";
				lpInfo->m_strPropHelp = "percentage for the bending stress from bending moment in stress produced by axial force";
#else
				lpInfo->m_strPropName = "��M %��ֵ";
				lpInfo->m_strPropHelp = "����ز���������Ӧ��ռ��������Ӧ���İٷֱ�";
#endif
				pFindItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nPrecentSigemaM");
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pFindItem->m_idProp);

			pFindItem = pPropList->FindItemByPropId(CLDSApp::GetPropID("g_sysPara.reactionExportType"),&pParentItem);
			if(pFindItem==NULL)
			{	//��ϸ��������ĵ�����
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
    				lpInfo->m_strPropName = "basic forces filtrating";
				lpInfo->m_strPropHelp = "When filtrating the level of high-low legs��only output the maximum pull-up force and down force from a module;when filtrating in a working condition level,only output all the pull-up force and down force from a moudle.";
				lpInfo->m_cmbItems = "0.Full output|1.High-low legs level|2.Working condition level";
#else
				lpInfo->m_strPropName = "����������ɸѡ";
				lpInfo->m_strPropHelp = "�ߵ��ȼ�ɸѡʱ��ֻ���һ�������鹤�������иߵ����е�����ϲ�������ѹ��;������ɸѡʱֻ���һ�������������й������ϲ�������ѹ����";
				lpInfo->m_cmbItems = "0.ȫ�����|1.�ߵ��ȼ�|2.������";
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
		sMenuName.Replace("���Ϊ","");
		CLDSDoc* pDoc=pView->GetDocument();
		if(toupper(sMenuName[0])=='V')
		{
			UINT uFileSaveVersion=FromStringVersion(sMenuName.Mid(2));
			BatchSaveAsFile(uFileSaveVersion);
		}
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("g_sysPara.uFileSaveVersion"))
	{
		sMenuName.Replace("���Ϊ","");
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
				"������������(*.tma)|*.tma|�����ļ�(*.*)|*.*||");
#endif
#elif defined(__LMA_FILE_)
			CFileDialog dlg(FALSE,"lma",filename,
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				"��������������(*.lma)|*.lma|�����ļ�(*.*)|*.*||");
			//"��������������(*.lma)|*.lma|�����������ļ�(*.sgt)|*.sgt|�����ļ�(*.*)|*.*||");
#elif defined(__TDA_FILE_)
			CFileDialog dlg(FALSE,"tda",filename,
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				"������ͼ����(*.tda)|*.tda|�����ļ�(*.*)|*.*||");
#elif defined(__LDS_FILE_)
			CFileDialog dlg(FALSE,"lds",filename,
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				"������ƻ�ͼһ�廯���(*.lds)|*.lds|�����ļ�(*.*)|*.*||");
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
		sMenuName.Replace("���Ϊ","");
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
				filter="������������(*.tma)|*.tma|�����ļ�(*.*)|*.*||";
			}
			else if(extension.Equal("lma"))
			{
				doc_type=2;
				filter="��������������(*.lma)|*.lma|�����ļ�(*.*)|*.*||";
			}
			else if(extension.Equal("tda"))
			{
				doc_type=5;
				filter="������ͼ����(*.tda)|*.tda|�����ļ�(*.*)|*.*||";
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
		if(iMenu==0)//sMenuName.Compare("�Զ��趨")==0)
		{
			g_sysPara.ciLinkAngleDlgFocusCtrlMode=0;
			pItem->m_lpNodeInfo->m_bMutiObjsProp=true;
			pPropList->SetItemPropValue(pItem->m_idProp,theApp.GetPropValueStr(pItem->m_idProp));
		}
		else if (iMenu==1)
		{	//ָ����������
			g_sysPara.ciLinkAngleDlgFocusCtrlMode=1;
			pPropList->SetItemPropValue(pItem->m_idProp,theApp.GetPropValueStr(pItem->m_idProp));
			pItem->m_lpNodeInfo->m_bMutiObjsProp=false;
		}
		else //if (iMenu==2)
		{	//ָ����ſ�����
			g_sysPara.ciLinkAngleDlgFocusCtrlMode=2;
			pPropList->SetItemPropValue(pItem->m_idProp,theApp.GetPropValueStr(pItem->m_idProp));
			pItem->m_lpNodeInfo->m_bMutiObjsProp=false;
		}
		pItem->m_lpNodeInfo->SetCheck(iMenu+1);
	}
	else if(pItem->m_idProp==CLDSApp::GetPropID("Ta.m_fNamedHeightZeroZ"))
	{
		if(iMenu==0)//sMenuName.Compare("���ܼ���")==0)
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
		sMenuName.CompareNoCase("�ų��κ�");
#endif
*/
		CString segExcludeStr;
		if(Input("����Ҫ�ų��Ķκŷ�Χ",&segExcludeStr,'s')>0)
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
			g_pSolidDraw->BuildDisplayList(pView);	//ֻˢ�µ�ǰ������ʾ�����ݶ���
		}
	}
	else if(pItem->m_idProp>CLDSApp::MAX_USER_PROPID && (pLsFamily=CLsLibrary::FromHandle(pItem->m_idProp-CLDSApp::MAX_USER_PROPID)))
	{
		CLsFamily xLocalLsFamily;
		if(iMenu!=0 && GetLocalLsFamily(pLsFamily->Id,sMenuName.GetBuffer(),&xLocalLsFamily))
		{	//ѡ�񱾵���˨����ļ�
			pItem->m_lpNodeInfo->SetCheck(iMenu+1);
			if(pLsFamily->m_uiCodeSerial!=xLocalLsFamily.m_uiCodeSerial )
				pLsFamily->CopyLsFamily(&xLocalLsFamily);	//ͬһ���ϵ��������淶�����ͬ������Ϊ��ͬһ��˨�淶
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
	CString sLsSeriesStr="�༭��ǰ�淶|-";
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
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DISPLAY-1,"��ʾ");
#endif
#if defined(__LDS_)||defined(__TSA_)
		if(!theApp.m_bDrawingWorkEnvOnly)
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_ANALYSIS-1,"����");
		//pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DESIGNCODE-1,"���");
#endif
#ifndef __TSA_
		if(!theApp.m_bAnalysisWorkEnvOnly)
#ifdef AFX_TARG_ENU_ENGLISH
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DRAWING-1,"Modeling");
#else 
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DRAWING-1,"��ģ");
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
		lpInfo->m_strPropName = "������Ϣ";
#endif
		pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
		pGroupItem->m_idProp = CLDSApp::GetPropID("basicInfo");
		pGroupItem->m_bHideChildren=FALSE;
		if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL); 
		//�û���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "User Name";
#else 
		lpInfo->m_strPropName = "�û���";
#endif
		lpInfo->m_strPropHelp = "";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("env.m_sUserName");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�û�����д
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "User Name Abbreviation";
#else 
		lpInfo->m_strPropName = "�û�����д";
#endif
		lpInfo->m_strPropHelp = "";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("env.m_sShortUserName");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�Զ����̼��ʱ��		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Auto Save Time";
#else 
		lpInfo->m_strPropName = "�Զ����̼��ʱ��";
#endif
		lpInfo->m_strPropHelp = "";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nAutoSaveTime");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�Զ����̷�ʽ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Auto Save Type";
		lpInfo->m_strPropHelp = "Single File :Back up only one file; Multiple File :Back up multiple file ,they distinguish by n��";
		lpInfo->m_cmbItems = "Single File Back-Up|Multiple File Back-Up";
#else 
		lpInfo->m_strPropName = "�Զ����̷�ʽ";
		lpInfo->m_strPropHelp = "���ļ�����:ֻ����һ���ļ�;���ļ�����:���ݶ���ļ�����(n)���֡�";
		lpInfo->m_cmbItems = "���ļ�����|���ļ�����";
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
		{	//�ļ��汾��ʽ
			lpInfo = new CItemInfo();
			//lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Save as previous version";
			lpInfo->m_strPropHelp = "Save as a file by previous version";
#else 
			lpInfo->m_strPropName = "���ɰ汾";
			lpInfo->m_strPropHelp = "���Ϊ�ɰ汾�ļ�";
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
				cmbItems = "V 5.1.8.0|V 5.1.6.0|V 5.1.2.0|V 5.0.2.0|V 4.1.2.0|V 4.1.1.2";//��Ϳɵ�V4.1.1.2
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
			lpInfo->m_strPropName = "�������ɰ汾";
			lpInfo->m_strPropHelp = "ѡ���ļ������ļ��У��������Ϊ�ɰ汾�ļ�";
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
		//��������Ʒ���ļ���ʽ wjh-2017.7.26
#if defined(__LDS_)
		if(true)	//���ݺ���Ժ��ӽ��Ҫ��ֱ�Ӹ����Ժ���Ž���洢��������ģ���ļ��Ĺ��ܸ��������г��ƹ� wjh-2019.5.25
#else
		if(szCanSaveDocTypes!=NULL&&VerifyValidFunction(LICFUNC::FUNC_IDENTITY_INTERNAL_TEST))
#endif
		{	//�ļ��汾��ʽ
			lpInfo = new CItemInfo();
			//lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENU;
			lpInfo->m_cmbItems=szCanSaveDocTypes;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Save as file type";
			lpInfo->m_strPropHelp = "Save as a file type";
#else 
			lpInfo->m_strPropName = "��������ʽ";
			lpInfo->m_strPropHelp = "��������ļ���ʽ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iFileSaveDocType");
			pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
#ifdef __TSA_
		//��λ��ͼ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_FILEPATH;
		lpInfo->m_strPropName = "��λ��ͼ����";
		lpInfo->m_strPropHelp = "��λ��ͼ����";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("env.m_sAcadPath");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
#endif
		//��ǰͼ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "Current layer";
#else 
		lpInfo->m_strPropName = "��ǰͼ��";
		lpInfo->m_strPropHelp = "��ǰͼ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("env.layer");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���ϵͳ��ʾ��Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems = "0.Close warnings|1.Important warnings|2.General warnings|3.Unimportant warnings|4.All warnings output";
		lpInfo->m_strPropName = "Warning Level";
		lpInfo->m_strPropHelp = "When the system running abnormal situation,output warning information";
#else
		lpInfo->m_cmbItems = "0.�ر����о�ʾ|1.��Ҫ��ʾ��Ϣ|2.һ�㾯ʾ��Ϣ|3.��Ҫ��ʾ��Ϣ|4.���ȫ����ʾ";
		lpInfo->m_strPropName = "����ȼ�";
		lpInfo->m_strPropHelp = "��ϵͳ���й������������������ʱ�������ʾ��Ϣ";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.m_ciLowestWarningLevel");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�ļ���д���������С
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "File read-write buffer size(MB)";
		lpInfo->m_strPropHelp = "";
#else
		lpInfo->m_strPropName = "�ļ���д��������(MB)";
		lpInfo->m_strPropHelp = "�ļ���д������������������д�ٶȣ�����������趨������ܵ��¡��ĵ�����ʧ�ܡ�����";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("FileRW.BUFFSIZE");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�ļ�����
		pGroupItem=oper.InsertPropItem(pRootItem,"env.flieProp");
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		pGroupItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
		//�汾��
		pPropItem=oper.InsertEditPropItem(pGroupItem,"Ta.version");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"Ta.m_uiOriginalDogKey");
		pPropItem->SetReadOnly();
		if(Ta.m_uiOriginalDogKey==Ta.m_uiLastSaveDogKey)
			pPropItem->m_lpNodeInfo->m_strPropName="��������";
		else
		{
			pPropItem=oper.InsertEditPropItem(pGroupItem,"Ta.m_uiLastSaveDogKey");
			pPropItem->SetReadOnly();
		}
		CPropTreeItem* pClassItem=oper.InsertEditPropItem(pGroupItem,"ModelInfo");
		pClassItem->SetReadOnly();
		//������
		pPropItem=oper.InsertEditPropItem(pClassItem,"Ta.Parts.GetNodeNum");
		pPropItem->SetReadOnly();
		//�ڵ���
		pPropItem=oper.InsertEditPropItem(pClassItem,"Ta.Node.GetNodeNum");
		pPropItem->SetReadOnly();
		//���߻���Zֵ����LTMA�п��ź�������
		pPropItem=oper.InsertBtnPopMenuEditPropItem(pClassItem,"Ta.m_fNamedHeightZeroZ","","","���ܼ���");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp=!Ta.m_bUserSpecifyNamedHeightZeroZ;
		//���̵�����
		oper.InsertEditPropItem(pGroupItem,"Ta.m_sPrjName");
		//����
		oper.InsertEditPropItem(pGroupItem,"Ta.m_sTaType");
		//����
		oper.InsertEditPropItem(pGroupItem,"Ta.m_sTaAlias");
		//��ӡ��
		oper.InsertEditPropItem(pGroupItem,"Ta.m_sTaStampNo");
		//����
		oper.InsertEditPropItem(pGroupItem,"Ta.m_nTaNum");
		//����Ա
		oper.InsertEditPropItem(pGroupItem,"Ta.m_sOperator");
		// ���Ա
		oper.InsertEditPropItem(pGroupItem,"Ta.m_sAuditor");
		// ��λ�����㷨
		oper.InsertCmbListPropItem(pGroupItem,"Ta.m_bRepositionByModeBeforeV51");
		//�������ͳ�Ʒ�ʽ
		oper.InsertCmbListPropItem(pGroupItem,"m_iPlankWeightStatType");
		//�������ͳ�Ʒ�ʽ
		oper.InsertCmbListPropItem(pGroupItem,"m_iCirclePlankAreaType");
		//��ߴ���ʽ
		oper.InsertCmbListPropItem(pGroupItem,"Ta.m_cRollProcessType");
		//����߼��㷽ʽ
		oper.InsertCmbListPropItem(pGroupItem,"Ta.m_cRollOffsetType");
		//�������δ���ʽ
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"Ta.m_cDeformType");
		if(Ta.m_ciDeformType==1)
			oper.InsertEditPropItem(pPropItem,"Ta.m_fDeformCoef");
		//��������Ʋ���
		if(Ta.hashParams.GetNodeNum()>0)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Model parameters";
			lpInfo->m_strPropHelp = "Model design parameters";
#else 
			lpInfo->m_strPropName = "ģ�Ͳ���";
			lpInfo->m_strPropHelp = "ģ����Ʋ���";
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
		//�����ļ�������־
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems = "0.None|1.Save";
		lpInfo->m_strPropName = "Save modification log";
		lpInfo->m_strPropHelp = "";
#else 
		lpInfo->m_cmbItems = "0.������|1.����";
		lpInfo->m_strPropName = "���������־";
		lpInfo->m_strPropHelp = "�����ļ�ʱ����ģ�͸�����ʷ��¼";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.m_bSaveLogVersion");
		pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
#endif
		if(!theApp.m_bAnalysisWorkEnvOnly)
			oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_iModFileCodeType");
		//������
		pGroupItem=oper.InsertPropItem(pRootItem,"PartLibrary");
		pGroupItem->m_bHideChildren=FALSE;
		if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//�Ǹ�׼���(���ļ�����)
		oper.InsertButtonPropItem(pGroupItem,"env.m_sJgZhunJuTbl");
		//�Ǹֹ���
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sJgGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//�Ǹֹ����ļ�
		oper.InsertFilePathPropItem(pPropItem,"env.m_sJgGuiGeTblPath");
#if defined(__COMPLEX_PART_INC_)||defined(__TSA_)//defined(__LMA_)||defined(__LDS_)
		//�ֹܹ���
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sTubeGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//�ֹܹ����ļ�
		oper.InsertFilePathPropItem(pPropItem,"env.m_sTubeGuiGeTblPath");
		//��������	
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sFlatGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//���������ļ�
		oper.InsertFilePathPropItem(pPropItem,"env.m_sFlatGuiGeTblPath");
		//�۸ֹ���
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sSlotGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//�۸ֹ����ļ�
		oper.InsertFilePathPropItem(pPropItem,"env.m_sSlotGuiGeTblPath");
		//�Ǹֽ�ͷ��
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sJgJointParaTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//�Ǹֽ�ͷ���ļ�
		oper.InsertFilePathPropItem(pPropItem,"env.m_sJgJointParaTblPath");
#ifdef __H_SHAPE_STEEL_
		//HW�͸ֹ���
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sHWGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//HW�͸ֹ����ļ�
		oper.InsertFilePathPropItem(pPropItem,"env.m_sHWGuiGeTblPath");
		//HM�͸ֹ���
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sHMGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//HM�͸ֹ����ļ�
		oper.InsertFilePathPropItem(pPropItem,"env.m_sHMGuiGeTblPath");
		//HN�͸ֹ���
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sHNGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//HN�͸ֹ����ļ�
		oper.InsertFilePathPropItem(pPropItem,"env.m_sHNGuiGeTblPath");
		//HP�͸ֹ���
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sHPGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
		//HP�͸ֹ����ļ�
		oper.InsertFilePathPropItem(pPropItem,"env.m_sHPGuiGeTblPath");
#endif
#endif

#if defined(__LDS_)||defined(__TSA_)
		//���¹���
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sWireGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
#endif
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
		//��׼������(���ļ�����)
		oper.InsertButtonPropItem(pGroupItem,"StdPartLib");
#endif
#ifndef __TSA_
		//��˨���ϵ��(���ļ�����)
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sLsGuiGeTbl");
		pPropItem->m_bHideChildren=FALSE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
		for(CLsFamily *pLsFamily=CLsLibrary::GetFirstFamily();pLsFamily;pLsFamily=CLsLibrary::GetNextFamily())//,i++)
		{	//��˨���ϵ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMON;
			if(GetLocalBoltSeries(pLsFamily->Id,NULL)>0)
			{	//�����иù��ϵ�е���˨��׼�ļ���ͨ���˵���ť������ʾ
				lpInfo->m_buttonType = BDT_POPMENU;
				lpInfo->m_cmbItems=MakeLocalLsSeriesString(pLsFamily->Id);
			}
#ifdef AFX_TARG_ENU_ENGLISH
			if(stricmp(pLsFamily->name,"��׼��˨")==0)
				lpInfo->m_strPropName = "Standard Bolts";
			else if(stricmp(pLsFamily->name,"˫ñ��˨")==0)
				lpInfo->m_strPropName = "Double-nut Bolts";
			else if(stricmp(pLsFamily->name,"�Ŷ�")==0)
				lpInfo->m_strPropName = "Climbing Nails";
			else if(stricmp(pLsFamily->name,"������˨")==0)
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
		//�ؽ���˨��
		oper.InsertButtonPropItem(pPropItem,"ANCHOR_BOLT_LIB");
		//��˨����(���ļ�����)
		oper.InsertButtonPropItem(pGroupItem,"env.m_sLsSpaceTbl");
		//��߹���(���ļ�����)
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"env.m_sFittingGuiGeTbl");
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
		oper.InsertButtonPropItem(pPropItem,"env.m_sEBGuiGeTbl");
		oper.InsertButtonPropItem(pPropItem,"env.m_sGDGuiGeTbl");
		//oper.InsertButtonPropItem(pPropItem,"env.m_sURGuiGeTbl");
#endif
		//�Ǹ���˨ѡ���
		oper.InsertButtonPropItem(pGroupItem,"Ta.listSelBoltCond");
		//���ʿ�
		oper.InsertButtonPropItem(pGroupItem,"env.m_sSteelMatTbl");
#if defined(__LDS_)||defined(__TSA_)
		//��˨ѡ����ϱ�
		oper.InsertButtonPropItem(pGroupItem,"env.m_sLsSelectTbl");
		//����ϵ����ֵ��
		oper.InsertButtonPropItem(pGroupItem,"env.m_sInterpolateBetaZTbl");
		//�������������������ϵ����
		oper.InsertButtonPropItem(pGroupItem,"env.m_sInterpIceLoadCoefTbl");
#endif
		//��ʾ������
		DisplaySysSettingDisplaySubSectionProperty(GROUP_DISPLAY);
#if defined(__LDS_)||defined(__TSA_)
		//DisplaySysSettingDesignCodeSubSectionProperty(GROUP_DESIGNCODE);
		if(!theApp.m_bDrawingWorkEnvOnly)
			DisplaySysSettingDesignSubSectionProperty(GROUP_ANALYSIS);
#endif
		//��ͼ������
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
		//���ؼ�ͷ
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
		//Ӧ��
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
	g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);	//��ʱ�ڴ˴�����
	pPropList->Redraw();
}
static void UpdateWorkUcsPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem)
{
	CPropertyListOper<CLDSApp> oper(pPropList,&theApp);
	CPropTreeItem *pPropItem=NULL,*pLeafItem=NULL;
	//��ʾ��Χ
	//װ��ƽ��
	oper.InsertCmbListPropItem(pParentItem,"work_ucs.m_eDatumPlane",CParaCS::GetDatumPlaneCmbItems());
	//װ��λ��
	pPropItem=oper.InsertButtonPropItem(pParentItem,"work_ucs.origin");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//װ������ϵԭ��λ��X�������
	oper.InsertEditPropItem(pPropItem,"work_ucs.origin.x");
	//װ������ϵԭ��λ��Y�������
	oper.InsertEditPropItem(pPropItem,"work_ucs.origin.y");
	//��������ϵԭ��λ��Z�������
	oper.InsertEditPropItem(pPropItem,"work_ucs.origin.z");
	//װ��ʱX�᷽��
	pPropItem=oper.InsertButtonPropItem(pParentItem,"work_ucs.axis_x");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//��������ϵX��X�������
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_x.x");
	pLeafItem->SetReadOnly();
	//��������ϵX��Y�������
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_x.y");
	pLeafItem->SetReadOnly();
	//��������ϵX��Z�������
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_x.z");
	pLeafItem->SetReadOnly();
	//װ��ʱY�᷽��
	pPropItem=oper.InsertButtonPropItem(pParentItem,"work_ucs.axis_y");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//��������ϵY��X�������
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_y.x");
	pLeafItem->SetReadOnly();
	//��������ϵY��Y�������
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_y.y");
	pLeafItem->SetReadOnly();
	//��������ϵY��Z�������
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_y.z");
	pLeafItem->SetReadOnly();
	//װ��ʱZ�᷽��
	pPropItem=oper.InsertButtonPropItem(pParentItem,"work_ucs.axis_z");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//��������ϵZ��X�������
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_z.x");
	pLeafItem->SetReadOnly();
	//��������ϵZ��Y�������
	pLeafItem=oper.InsertEditPropItem(pPropItem,"work_ucs.axis_z.y");
	pLeafItem->SetReadOnly();
	//��������ϵZ��Z�������
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
	//��ʾ��Χ
	pGroupItem=oper.InsertPropItem(pRootItem,"DisplayScope");
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(iSectionType);
#ifndef __TSA_
	//��ǰ��ʾ�κŷ�Χ
	oper.InsertBtnPopMenuEditPropItem(pGroupItem,"Ta.segstr");
#endif
	//��ʾ���⸨������
	oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.display.bDispVirtualPart");
	//������ʾ
	pPropItem=oper.InsertEditPropItem(pGroupItem,"lineDisplay");
	//��ѧ����ģʽ�£��������õ�[������ʾ]�ڵ���ѡ���� wjh-2015.3.5
	pPropItem->m_bHideChildren = !theApp.m_bAnalysisWorkEnvOnly;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//�ڵ�
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
	//�˼�����
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bPoleLine");
	//�˼���ʼ�˱��
	oper.InsertCmbListPropItem(pPropItem,"CLDSLinePart::bDisplayLineVecMark");
#ifdef __COMMON_PART_INC_
	//Ԥ����ʾ״̬
	oper.InsertCmbListPropItem(pPropItem,"CLDSNode::m_bDisplayBeforeArcLift");
#endif
#ifndef __TSA_
	//�����������Լ�����
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
	pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bDimRodDesignInfo","��|��","�˼������Ϣ","�����˼���񼰵�Ԫ�����ԣ����Ϣ",-1);
#else
	pSonPropItem=oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bDimRodDesignInfo","��|��","�˼������Ϣ","�����˼������ʼ������Ϣ",-1);
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
#if !defined(__LDS_TSA_) && !defined(__TSA_)	//��������ģ��
	//ʵ����ʾ
	pPropItem=oper.InsertEditPropItem(pGroupItem,"solidDisplay");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//�Ǹ�
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidJg");
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
	//�ֹ�
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidTube");
#endif
	//���Ӱ�
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidLjb");
	//��˨
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidLs");
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
	//�۸�
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidSlot");
	//����
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidSphere");
	//���
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidFlat");
	//���νǸ�
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidArcAngle");
	//���α��
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidArcFlat");
	//���β۸�
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidArcSlot");
	//��׼����
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidStdPart");
	//����/�ؽ���˨
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidFoundation");
	//ʵ�������
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidTransectFace");
#endif
	//����
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.bSolidLine");
	//�Ǹ����� wht 10-12-27
	oper.InsertCmbListPropItem(pPropItem,"CLDSLineAngle::m_bDisplaySolidAxisLine");
	//�ְ�������
	oper.InsertCmbListPropItem(pPropItem,"CLDSGeneralPlate::m_ciDisplayVertexType");
	//ʵ����ʾʱ�Ĺ⻬��
	oper.InsertEditPropItem(pPropItem,"g_sysPara.display.nSmoothness");
	//�ױ���ʾ��ʽ
	oper.InsertCmbListPropItem(pPropItem,"g_sysPara.display.cHoleWallMode");
	//����ѡ�й���
	pGroupItem=oper.InsertPropItem(pRootItem,"m_dwSelectPartsType");
	pGroupItem->m_bHideChildren=TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//�Ƿ��ѡ�нڵ�
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectNode");
	//�Ƿ��ѡ�нǸ�
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectLineAngle");
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
	//�Ƿ��ѡ�иֹ�
	const char* itemname="�ֹ�";
#else
	const char* itemname="�׹�";
#endif
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectLineTube","��|��",itemname);
	//�Ƿ��ѡ�иְ�
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectPlate");
	//�Ƿ��ѡ�в������ְ�
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectParamPlate");
	//�Ƿ��ѡ����˨
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectBolt");
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
	//�Ƿ��ѡ������
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectSphere");
	//�Ƿ��ѡ�в۸�
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectLineSlot");
	//�Ƿ��ѡ�б��
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectLineFlat");
	//�Ƿ��ѡ�л��νǸ�
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectArcAngle");
	//�Ƿ��ѡ�л��α��
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectArcFlat");
	//�Ƿ��ѡ�л��β۸�
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectArcSlot");
	//�Ƿ��ѡ�б�׼����
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectStdPart");
	//
	oper.InsertCmbListPropItem(pGroupItem,"CanSelectEdgeLine");
#endif
#endif
	//��ʾģʽ
	pGroupItem=oper.InsertPropItem(pRootItem,"DisplayMode");
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//��ת�ٶ�
	oper.InsertEditPropItem(pGroupItem,"g_sysPara.fRotateVelocity");
	//����ģ���Զ�����
	oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_bLineDepthTest");
	//��ק�������ܿ�����
	oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_bDragOpenWndSel");
	//��ק�������ܿ�����
	oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_bDblClkStartRotateOper");
	//��ɫ����
	oper.InsertButtonPropItem(pGroupItem,"ColorScheme");
	//��׼����ϵ
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"env.mcs");
	pPropItem->m_bHideChildren=TRUE;
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pPropItem->m_idProp);
	//��׼����ϵX�᷽��
	pSonPropItem=oper.InsertButtonPropItem(pPropItem,"env.mcs.axis_x");
	pSonPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	//��׼����ϵX��X�������
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_x.x");
	pLeafItem->SetReadOnly();
	//��׼����ϵX��Y�������
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_x.y");
	pLeafItem->SetReadOnly();
	//��׼����ϵX��Z�������
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_x.z");
	pLeafItem->SetReadOnly();
	//װ��ʱY�᷽��
	pSonPropItem=oper.InsertButtonPropItem(pPropItem,"env.mcs.axis_y");
	pSonPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	//��׼����ϵY��X�������
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_y.x");
	pLeafItem->SetReadOnly();
	//��׼����ϵY��Y�������
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_y.y");
	pLeafItem->SetReadOnly();
	//��׼����ϵY��Z�������
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_y.z");
	pLeafItem->SetReadOnly();
	//װ��ʱZ�᷽��
	pSonPropItem=oper.InsertButtonPropItem(pPropItem,"env.mcs.axis_z");
	pSonPropItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren = CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	//��׼����ϵZ��X�������
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_z.x");
	pLeafItem->SetReadOnly();
	//��׼����ϵZ��Y�������
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_z.y");
	pLeafItem->SetReadOnly();
	//��׼����ϵZ��Z�������
	pLeafItem=oper.InsertEditPropItem(pSonPropItem,"env.mcs.axis_z.z");
	pLeafItem->SetReadOnly();
	//��������ϵ
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_bDispWorkUcs");
	UpdateWorkUcsPropItem(pPropList,pPropItem);
#if !defined(__LDS_TSA_) && !defined(__TSA_)	//LDS����������ģ��
	//����װ������ϵ
	oper.InsertCmbListPropItem(pGroupItem,"g_sysPara.m_bDispPartACS");
	//������λ����ϵ
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
	//�����
	CItemInfo* lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Load Calculation";
#else
	lpInfo->m_strPropName = "���ؼ���";
#endif
	pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CLDSApp::GetPropID("WireLoadCal");
	pGroupItem->m_bHideChildren=TRUE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(iSectionType); 

	/*lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = "��ѹ�ȼ�(kV)";
	lpInfo->m_strPropHelp="500kV�����ϵ�ѹ�ȼ��������߷���ؼ����迼�ǵ����߷���ص���ϵ����c��";
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
	lpInfo->m_strPropName = "��λ��ò";
	lpInfo->m_strPropHelp="��λ��òӰ���ѹ�߶ȱ仯ϵ��ȡֵ��";
	lpInfo->m_cmbItems="A:������ɳĮƽ̹����|B:��½������ϡ�����|C:���ܼ�����Ⱥ����|D:���ܼ�����Ⱥ�͸߲㽨������";
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
	lpInfo->m_strPropName = "��··��";
	lpInfo->m_strPropHelp="��··����ϢӰ�쵽���߹���ʱ�����߶���������ȡֵ��";
	lpInfo->m_cmbItems="ƽ����·|������·|ɽ����·";
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
	lpInfo->m_strPropName = "��������";
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
	lpInfo->m_strPropName = "Maximum air temperature(��C)";
	lpInfo->m_strPropHelp = "Maximum air temperature(��C)";
#else
	lpInfo->m_strPropName = "�������(��C)";
	lpInfo->m_strPropHelp = "�������(��C)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_nHighestTemperature");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Minimum air temperature(��C)";
	lpInfo->m_strPropHelp = "Minimum air temperature(��C)";
#else
	lpInfo->m_strPropName = "�������(��C)";
	lpInfo->m_strPropHelp = "�������(��C)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_nLowestTemperature");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "average temperature(��C)";
	lpInfo->m_strPropHelp = "average temperature(��C)";
#else
	lpInfo->m_strPropName = "ƽ������(��C)";
	lpInfo->m_strPropHelp = "ƽ������(��C)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_nAverageTemperature");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	//��繤��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "termperature in maximum wind velocity(��C)";
	lpInfo->m_strPropHelp = "termperature in maximum wind velocity(��C)";
#else
	lpInfo->m_strPropName = "������ʱ�¶�(��C)";
	lpInfo->m_strPropHelp = "������ʱ�¶�(��C)";
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
	lpInfo->m_strPropName = "������(m/s)";
	lpInfo->m_strPropHelp = "������(m/s)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_fMaxWindVelocity");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	//��������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "temperature in maximum icing(��C)";
	lpInfo->m_strPropHelp = "temperature in maximum icing(��C)";
#else
	lpInfo->m_strPropName = "��󸲱�ʱ�¶�(��C)";
	lpInfo->m_strPropHelp = "��󸲱�ʱ�¶�(��C)";
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
	lpInfo->m_strPropName = "��󸲱�ʱ����(m/s)";
	lpInfo->m_strPropHelp = "��󸲱�ʱ����(m/s)";
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
	lpInfo->m_strPropName = "�������(mm)";
	lpInfo->m_strPropHelp = "�������(mm)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_fMaxIceThick");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pSonPropItem->m_idProp);
	//��װ����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "temperature while installing(��C)";
	lpInfo->m_strPropHelp = "The temperature while installing(��C)";
#else
	lpInfo->m_strPropName = "��װʱ�¶�(��C)";
	lpInfo->m_strPropHelp = "��װʱ�¶�(��C)";
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
	lpInfo->m_strPropName = "��װʱ����(m/s)";
	lpInfo->m_strPropHelp = "��װʱ����(m/s)";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_fSetupWindVelocity");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	//�����
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "finite element solver";
#else
	lpInfo->m_strPropName = "����Ԫ�����";
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
	lpInfo->m_strPropName = "�������ʽѡ��";
	lpInfo->m_strPropHelp = "�������ʽѡ��";
	lpInfo->m_cmbItems = "0.��˹��|1.��ǰ��|2.��������";
	lpInfo->m_strPropValue = "0.��˹��";
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
	lpInfo->m_strPropName = "�ߵ����ֻ�����";
	lpInfo->m_strPropHelp = "�ߵ����ֻ�����";
	lpInfo->m_cmbItems = "0.���ֻ�|1.�ȸ����ֻ�|2.�ߵ����ֻ�";
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
	lpInfo->m_strPropName = "��¼����������Ϣ";
	lpInfo->m_strPropHelp = "���ڴ�����ʱ��¼ÿ���������̵Ľڵ�λ�ơ����ؼ���Ԫ�������Ա�鿴����ͼ��";
	lpInfo->m_cmbItems = "��|��";
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
	lpInfo->m_strPropName = "����ANSYS�������ļ�";
	lpInfo->m_strPropHelp = "���ɷ������̵�ANSYS�����нӿ��ļ�";
	lpInfo->m_cmbItems = "��|��";
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
	lpInfo->m_strPropName = "������ն��ۼ�ϵ��";
	lpInfo->m_strPropHelp = "����ʵ����Ҫ�ɶ�����Ԫ����նȽ���һ���̶��ϵ��ۼ���";
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
	lpInfo->m_strPropName = "�����Ĳ�Ԥ��ѧ����";
	lpInfo->m_strPropHelp = "������Ҳ��Ϊ�����Ĳ�Ԥ����Ԫ��ѧ�����������ڶ������Է����������Խṹ����ʱ�����Է���ʱһ�㸨���Ĳ���Ԥ����Ԫ��ѧ������";
	lpInfo->m_cmbItems = "��|��";
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
	lpInfo->m_strPropName = "ģ̬������������";
	lpInfo->m_strPropHelp = "�ṹģ̬�������Է���ʱ������������";
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
	lpInfo->m_strPropName = "����Ƶ���������";
	lpInfo->m_strPropHelp = "�������Է���ʱ,�ṹ����Ƶ�ʵ�����������";
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
	lpInfo->m_strPropName = "Ӧ���������㷽ʽ";
	lpInfo->m_strPropHelp = "Ӧ���������㷽ʽ";
	lpInfo->m_cmbItems = "0.���Է���|1.�����Է���";
	lpInfo->m_strPropValue = "0.���Է���";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_iNonlinearStyle");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	pPropItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
	if(g_sysPara.AnalysisPara.m_iNonlinearStyle==1)
	{	//�����Է���
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "times of iteration";
		lpInfo->m_strPropHelp = "maximum times of iteration in each increment, if beyond, quit the iteration"
#else
		lpInfo->m_strPropName = "��������";
		lpInfo->m_strPropHelp = "ÿ�������������������������˳�����";
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
		lpInfo->m_strPropHelp = "dynamic��N times reach precision and quit the iteration";
#else
		lpInfo->m_strPropName = "������ֵ";
		lpInfo->m_strPropHelp = "Ӧ����������������ֵ��N�δﵽ�����˳�����";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.AnalysisPara.m_fConvergeLim");
		pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType); 
	}
	//��Ԫ����
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Unit calculation";
#else
	lpInfo->m_strPropName = "��Ԫ����";
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
	lpInfo->m_strPropHelp = "singular Major structure refers to normal single tower structure such as tower power��microwave communication tower, Eiffel wind pressure effect should be considered when desiging; composite structure does not need to consider the Eiffel wind pressure effect";
	lpInfo->m_cmbItems = "0.Single major structure|1.Composite structure";
#else
	lpInfo->m_strPropName = "�ṹ����";
	lpInfo->m_strPropHelp = "������ṹָ��ͨ������ṹ�糣����������΢��ͨ���������ʱ�迼�ǰ��ƶ���ѹЧӦ�����Ͻṹ�����ǰ��ƶ���ѹЧӦ";
	lpInfo->m_cmbItems = "0.������ṹ|1.���Ͻṹ";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("Ta.m_cStructureType");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//������
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName =	 "result output";
#else
	lpInfo->m_strPropName = "������";
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
	lpInfo->m_strPropName = "��������anl�ļ�";
	lpInfo->m_strPropHelp = "���ɷ��������ļ�";
	lpInfo->m_cmbItems = "��|��";
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
	lpInfo->m_strPropName = "������Ԥ������";
	lpInfo->m_strPropHelp = "������Ԥ������";
	lpInfo->m_cmbItems = "0.λ�Ʊ���|1.����+��ϸ��ϵ��|2.����ϵ��|3.������";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.iShowDeformedMapPlan");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_strPropName = "��ԪЧ��ϵ������";
	lpInfo->m_strPropHelp = "��Ԫ����ϵ����ֻ������йص�Ч��ϵ���������ϵ����������˳�ϸ���������ء�";
	lpInfo->m_cmbItems = "0.����+��ϸ��ϵ��|1.����ϵ��";
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
	lpInfo->m_strPropName = "���¸˼����������ֵ";
	lpInfo->m_strPropHelp = "��������Զ����ܸ��¸��˼���ͷ������������ֵ���Ա��ڽ�����˨�����������Ӽ����У��";
	lpInfo->m_cmbItems = "��|��";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.bUpdateRodDesignForce");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//
	{	//��ϸ��������ĵ�����
		/*
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�ϲ��Գ��������";
		lpInfo->m_strPropHelp = "�ϲ��Գƽڵ�͸˼�����Ϣ���";
		lpInfo->m_cmbItems = "��|��";
		pSonPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);	//InsertItem(pPropItem
		pSonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.bMergeRptFileMirObj");
		pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);*/
		//
		/*lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��������";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.putOutM");
		pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
		pSonPropItem->m_bHideChildren=TRUE;
		if(CLDSApp::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pSonPropItem->m_idProp);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "M��ֵ";
		lpInfo->m_strPropHelp = "M��ֵ";
		CPropTreeItem* pGrandsonPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
		pGrandsonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nM");
		pGrandsonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pGrandsonPropItem->m_idProp);
		pGrandsonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��M��ֵ";
		lpInfo->m_strPropHelp = "��M��ֵ";
		pGrandsonPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
		pGrandsonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nSigemaM");
		pGrandsonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pGrandsonPropItem->m_idProp);
		pGrandsonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��M%��ֵ";
		lpInfo->m_strPropHelp = "��M%��ֵ";
		pGrandsonPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
		pGrandsonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.nPrecentSigemaM");
		pGrandsonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pGrandsonPropItem->m_idProp);
		pGrandsonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);*/

		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Filtrating basic forces";
		lpInfo->m_strPropHelp = "When filtrating the level of high-low legs��only output the maximum pull-up force and down force from a module; When filtrating in a working condition level, only output all the pull-up force and down force from a moudle.";
		lpInfo->m_cmbItems = "0.Full output|1.High-low legs level|2.Working condition level";
#else
		lpInfo->m_strPropName = "����������ɸѡ";
		lpInfo->m_strPropHelp = "�ߵ��ȼ�ɸѡʱ��ֻ���һ�������鹤�������иߵ����е�����ϲ�������ѹ��;������ɸѡʱֻ���һ�������������й������ϲ�������ѹ����";
		lpInfo->m_cmbItems = "0.ȫ�����|1.�ߵ��ȼ�|2.������";
#endif
		pSonPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);	//InsertItem(pPropItem
		pSonPropItem->m_idProp = CLDSApp::GetPropID("g_sysPara.reactionExportType");
		pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	}
	//ѡ��
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "select steel material";
#else
	lpInfo->m_strPropName = "ѡ��";
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
	lpInfo->m_strPropName = "�Ǹֹ������Ǹ�����";
	lpInfo->m_strPropHelp = "�Ǹֹ���ѡ��ʱ��ѡ���Ǹ�";
	lpInfo->m_cmbItems = "��|��";
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
	lpInfo->m_strPropName = "�����ѡ�������";
	lpInfo->m_strPropHelp = "����ѡ��ʱ�������������";
	lpInfo->m_cmbItems = "0.���������|1.��ת�뾶����|2.��Ⱥ������";
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
	lpInfo->m_strPropName = "ѡ�ľ���";
	lpInfo->m_strPropHelp = "ѡ�ľ���";
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
	lpInfo->m_strPropName = "ѡ�Ĵ���";
	lpInfo->m_strPropHelp = "ѡ�Ĵ���";
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
	//������ƹ涨
	//��0�ṹ��Ҫ��ϵ��
	//��G�������ط���ϵ��
	//�׿ɱ�������ϵ��
	//��Qi��i��ɱ���ط���ϵ��
	//����ؼ���涨
	//������������
	//����ϵ��
	//��c���߷���ص���ϵ��
	//��z��������ص���ϵ��
	//��s���������ϵ��
	//�Ի�������ص���ϵ��
	//�������ؼ���涨
	//����������أ������ڷ���ص���ϵ����
	//�������
	//Ig������������
	//ǿ����ƹ涨
	//m������������ǿ���ۼ�ϵ��
	//mM���乹���ȶ�ǿ���ۼ�ϵ��
	//������ѹ�����ȶ�ϵ��
	//mNѹ���ȶ�ǿ���ۼ�ϵ��
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem* pRootItem=pPropList->GetRootItem();
	CPropTreeItem* pGroupItem,*pPropItem,*pSonPropItem,*pLeafItem;

	CItemInfo* lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "attribute of tower";
	lpInfo->m_strPropHelp = "attribute of tower";
#else
	lpInfo->m_strPropName = "��������";
	lpInfo->m_strPropHelp = "��������";
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
	lpInfo->m_strPropName = "������������";
	lpInfo->m_strPropHelp = "������������(s)";
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
	lpInfo->m_strPropName = "��λ����";
	lpInfo->m_strPropHelp = "��λ����";
	lpInfo->m_cmbItems="A.������ɳĮƽ̹����|B.��½������ϡ�����|C.���ܼ�����Ⱥ����|D.���ܼ�����Ⱥ�͸߲㽨������";
	lpInfo->m_strPropValue = "��½������ϡ�����";
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
	lpInfo->m_strPropName = "��ȫ�ȼ�";
	lpInfo->m_strPropHelp = "��ȫ�ȼ�";
	lpInfo->m_cmbItems="һ��:�ر���Ҫ�ĸ���|����:һ��ĸ������|����:��ʱʹ�õĸ���";
	lpInfo->m_strPropValue = "����:һ��ĸ������";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("Ta.m_siSafetyGrade");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=TRUE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//�ṹ��Ҫ��ϵ����o
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "strcucture importance coefficient";
	lpInfo->m_strPropHelp = "strcucture importance coefficient��o";
#else
	lpInfo->m_strPropName = "�ṹ��Ҫ��ϵ��";
	lpInfo->m_strPropHelp = "�ṹ��Ҫ��ϵ����o";
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
	lpInfo->m_strPropName = "�����涨";
	lpInfo->m_strPropHelp = "�����涨";
#endif
	pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CLDSApp::GetPropID("BasicRegulations");
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CLDSApp::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//����ϵ��������ֵ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "maximum design value of loading coefficient";
	lpInfo->m_strPropHelp = "maximum design value of loading coefficient which parts can take, ranging from 0~1.0";
#else
	lpInfo->m_strPropName = "����ϵ��������ֵ";
	lpInfo->m_strPropHelp = "�������Դﵽ��������ϵ��ֵ��ȡֵ��ΧΪ0~1.0";
#endif
	lpInfo->m_strPropValue = "1.0";
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("m_fLimitBearCoef");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//������ƹ涨
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "rules for carrying out load calculation";
	lpInfo->m_strPropHelp = "rule of load calculation��design specification is executed currently";
	lpInfo->m_cmbItems = "design rules for 110~750kV transmission line|Towering structure design rules GB50135-2006|Loading rules for building structures design GB50009-2001";
	lpInfo->m_strPropValue = "design rules for 110~750kV transmission line"
#else
	lpInfo->m_strPropName = "���ؼ���ִ�й淶";
	lpInfo->m_strPropHelp = "���ؼ���涨����ǰִ����ƹ淶";
	lpInfo->m_cmbItems = "110~750kV�����·��ƹ涨|���ʽṹ��ƹ涨 GB50135-2006|�����ṹ���ع淶 GB50009-2001";
	lpInfo->m_strPropValue = "110~750kV�����·��ƹ涨";
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
	lpInfo->m_strPropName = "basic calculation ��z";
	lpInfo->m_strPropHelp = "basic calculating wind load adjustment coefficient";
#else
	lpInfo->m_strPropName = "���������z";
	lpInfo->m_strPropHelp = "�����������ص���ϵ��";
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
	lpInfo->m_strPropName = "Optimize gravity load ��G when selecting material";
	lpInfo->m_strPropHelp = "Optimize the gravity loads subentry coefficient when selecting material";
#else
	lpInfo->m_strPropName = "�Ż�ѡ��ʱ�������ئ�G";
	lpInfo->m_strPropHelp = "�Ż�ѡ��ʱ�������ط���ϵ��";
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
	lpInfo->m_strPropName = "Gravity load��G during calculation";
	lpInfo->m_strPropHelp = "Gravity load subentry coefficient during calculation";
#else
	lpInfo->m_strPropName = "��������ʱ�������ئ�G";
	lpInfo->m_strPropHelp = "��������ʱ�������ط���ϵ��";
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
	lpInfo->m_strPropName = "Tower wind load subentry coefficient��Q";
	lpInfo->m_strPropHelp = "variable loads subentry coefficient ��Q of Tower wind load"
#else
	lpInfo->m_strPropName = "�������ط���ϵ����Q";
	lpInfo->m_strPropHelp = "����������Ϊ�ɱ���صķ���ϵ����Q";
#endif
	lpInfo->m_strPropValue = "1.4";
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CLDSApp::GetPropID("m_fGamaQForTowerWindLoad");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//������ƹ涨
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "rules for parts material selection";
	lpInfo->m_strPropHelp = "basic design rules, design rules are executed currently";
	lpInfo->m_cmbItems = "design rules for 110~750kV transmission line|towering structures design rules GB50135-2006|America ASCE 10-97";
	lpInfo->m_strPropValue = "110~750kV ransmission line design rules";
#else
	lpInfo->m_strPropName = "����ѡ�ļ���ִ�й淶";
	lpInfo->m_strPropHelp = "������ƹ涨����ǰִ����ƹ淶";
	lpInfo->m_cmbItems = "110~750kV�����·��ƹ涨|���ʽṹ��ƹ涨 GB50135-2006|���� ASCE 10-97";
	lpInfo->m_strPropValue = "110~750kV�����·��ƹ涨";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSApp::GetPropID("m_iMemberSizeCheckCode");
	pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSApp::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);

	//
	//ѡ�Ĺ�����ƣ��ְ�����Сֵ���Ǹֽ�����Сֵ���ֹܱں���Сֵ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "limit for  angle specification material selection";
	lpInfo->m_strPropHelp = "limit for  angle specification material selection";
#else
	lpInfo->m_strPropName = "�Ǹֹ��ѡ������";
	lpInfo->m_strPropHelp = "�Ǹֹ��ѡ������";
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
	lpInfo->m_strPropName = "������С�����";
	lpInfo->m_strPropHelp = "������С�����";
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
	lpInfo->m_strPropName = "��������С�����";
	lpInfo->m_strPropHelp = "��������С�����";
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
	lpInfo->m_strPropName = "��������С�����";
	lpInfo->m_strPropHelp = "��������С�����";
#endif
	lpInfo->m_cmbItems = MakeAngleSetString();
	lpInfo->m_cmbType = CDT_EDIT;
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_csMinAngleSize");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//�ֹܹ�����ϸ������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "proportion limit for tube's length and slenderness";
	lpInfo->m_strPropHelp = "proportion limit for tube's length and slenderness��used for considering the limit for the critical wind speed start oscillation from equal diameter steel tube";
#else
	lpInfo->m_strPropName = "�ֹܳ�ϸ������";
	lpInfo->m_strPropHelp = "�ֹܳ�ϸ�����ƣ����ڿ��ǵ�ֱ���ֹ������ٽ���ٵ�����";
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
	lpInfo->m_strPropName = "��ѹ���ĸֹܳ�ϸ����ֵ";
	lpInfo->m_strPropHelp = "��ѹ���ĸֹܳ�ϸ����ֵ";
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
	lpInfo->m_strPropName = "��ѹб�ĸֹܳ�ϸ����ֵ";
	lpInfo->m_strPropHelp = "��ѹб�ĸֹܳ�ϸ����ֵ";
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
	lpInfo->m_strPropName = "��ѹ�����ֹܳ�ϸ����ֵ";
	lpInfo->m_strPropHelp = "��ѹ�����ֹܳ�ϸ����ֵ";
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
	lpInfo->m_strPropName = "�����ֹܳ�ϸ����ֵ";
	lpInfo->m_strPropHelp = "�����ֹܳ�ϸ����ֵ";
#endif
	lpInfo->m_strPropValue = "400";
	pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1);
	pLeafItem->m_idProp = CLDSApp::GetPropID("Ta.SelMatPara.m_nTensiveTubeSlendernessLimit");
	pLeafItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
	//����˼���ϸ������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "length and slenderness ration limit  for the other rods";
	lpInfo->m_strPropHelp = "length and slenderness ration limit  for the other rods except tubes��current design specifications";
#else
	lpInfo->m_strPropName = "����˼���ϸ������";
	lpInfo->m_strPropHelp = "���ֹ�������˼���ϸ�����ƣ���ǰ��ƹ淶";
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
	lpInfo->m_strPropName = "��ѹ���ĳ�ϸ����ֵ";
	lpInfo->m_strPropHelp = "��ѹ���ĳ�ϸ����ֵ";
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
	lpInfo->m_strPropName = "��ѹб�ĳ�ϸ����ֵ";
	lpInfo->m_strPropHelp = "��ѹб�ĳ�ϸ����ֵ";
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
	lpInfo->m_strPropName = "��ѹ�����ĳ�ϸ����ֵ";
	lpInfo->m_strPropHelp = "��ѹ�����ĳ�ϸ����ֵ";
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
	lpInfo->m_strPropName = "�����ĳ�ϸ����ֵ";
	lpInfo->m_strPropHelp = "�����ĳ�ϸ����ֵ";
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
	//������Ϣ
	pBasicInfoItem=oper.InsertPropItem(pRootItem,"DrawingInfo");
	pBasicInfoItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pBasicInfoItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pBasicInfoItem->m_idProp)!=-1)
		pBasicInfoItem->m_bHideChildren=CLDSApp::GetPropStatus(pBasicInfoItem->m_idProp);
	//��λ��ͼ����
	oper.InsertFilePathPropItem(pBasicInfoItem,"env.m_sAcadPath");
	//������϶
	oper.InsertEditPropItem(pBasicInfoItem,"g_sysPara.HuoQuDist");
	//�ǸֶԽӼ�϶
	oper.InsertEditPropItem(pBasicInfoItem,"g_sysPara.JgEndDist");
	//��ͷ����϶
	oper.InsertEditPropItem(pBasicInfoItem,"g_sysPara.JointPlankDist");
	//�����϶
	oper.InsertEditPropItem(pBasicInfoItem,"g_sysPara.VertexDist");
	//�����ַ������λ��
	pPropItem=oper.InsertCmbListPropItem(pBasicInfoItem,"g_sysPara.m_nMatCharPosType");
	if(g_sysPara.m_nMatCharPosType>0)	//�����ַ������
		oper.InsertCmbEditPropItem(pPropItem,"g_sysPara.m_cMatCharSeparator");
	//�����κű��ͬ������
	oper.InsertCmbListPropItem(pBasicInfoItem,"CLDSPart::UPDATE_PARTNO_AND_SEG");
	//��ʾ�򿪹�
	oper.InsertCmbListPropItem(pBasicInfoItem,"g_sysPara.m_bEnablePrompt");
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
	//�ֹ��������
	pParentItem=oper.InsertPropItem(pRootItem,"g_sysPara.TubeParam");
	pParentItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pParentItem->m_bHideChildren = TRUE;
	if(CLDSApp::GetPropStatus(pParentItem->m_idProp)!=-1)
		pParentItem->m_bHideChildren=CLDSApp::GetPropStatus(pParentItem->m_idProp);
	//�޸ĶԽӷ����ֹ�����ͷ
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.m_bModifyCoupledFLTubeOddment");
	//������ֹܼ�϶
	oper.InsertEditPropItem(pParentItem,"g_sysPara.TubeFlDist");
	//������ֹܼ�϶
	oper.InsertCmbListPropItem(pParentItem,"CLDSLineTube::ciFlTubeOddment");
	//�ֹܴ��ں�������Сֱ��
	oper.InsertEditPropItem(pParentItem,"g_sysPara.fTubeWeldRoadMinD");
	//�Զ������ֹ�λ��
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.nAutoAdjustTubePos");
#endif
	//�ְ����
	pParentItem=oper.InsertPropItem(pRootItem,"g_sysPara.PlateParam");
	pParentItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pParentItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pParentItem->m_idProp)!=-1)
		pParentItem->m_bHideChildren=CLDSApp::GetPropStatus(pParentItem->m_idProp);
	//���Ӱ�Ӧ����ɢ��
	oper.InsertEditPropItem(pParentItem,"g_sysPara.fStressDiffuseAngle");
	//���߾�
	oper.InsertEditPropItem(pParentItem,"g_sysPara.fInsertPlateEdgeSpace");
	//���Ӱ��ڱ���С�Ƕ�
	oper.InsertEditPropItem(pParentItem,"g_sysPara.m_nPlankEdgeAngleThreshold");
	//�ְ����������㷨
	oper.InsertCmbListPropItem(pParentItem,"m_iProfileAlgorithm");
	//ͬ��ɾ���ְ���˨
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.m_bDelBoltsResideOnPlate");
	//�Ǹֲ���
	pParentItem=oper.InsertPropItem(pRootItem,"g_sysPara.AngleParam");
	pParentItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pParentItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pParentItem->m_idProp)!=-1)
		pParentItem->m_bHideChildren=CLDSApp::GetPropStatus(pParentItem->m_idProp);
	//�Ǹ����ĶԻ���Ĭ�Ϲ��λ��ѡ��
	pPropItem=oper.InsertPopMenuItem(pParentItem,"g_sysPara.ciLinkAngleDlgFocusCtrlMode");
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp=(g_sysPara.ciLinkAngleDlgFocusCtrlMode==0);//�����ж�
	pPropItem->m_lpNodeInfo->SetCheck(g_sysPara.ciLinkAngleDlgFocusCtrlMode+1);
	//���Ͻ���ֵ wht 11-05-06
	oper.InsertEditPropItem(pParentItem,"g_sysPara.m_fKaiHeJiaoThreshold");
	//���Ͻ������С��϶
	oper.InsertEditPropItem(pParentItem,"g_sysPara.m_fKaiHeJiaoMinDeita");
	//��ϽǸֵļӹ���϶
	oper.InsertEditPropItem(pParentItem,"g_sysPara.m_nGroupJgMachiningGap");
	//��С�н�ֵ
	oper.InsertEditPropItem(pParentItem,"g_sysPara.m_fMinCutAngleLen");
	//�������н�ֵ
	oper.InsertEditPropItem(pParentItem,"g_sysPara.m_fMaxIgnoreCutAngleLen");
	//�н��Ƿ���Բ��ռ��ϵ��
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.m_b45CutAngleNeedCoefR");
	//�нǼ�϶��Բ��ռ��ϵ��
	oper.InsertCmbEditPropItem(pParentItem,"g_sysPara.m_fCutAngleCoefR");
	//�Ǹ�Ĭ�϶�λ��ʽ
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.m_bAnglePreferSimplePosMode");
	//��˨���Ŷ�����
	pParentItem=oper.InsertPropItem(pRootItem,"g_sysPara.BoltParam");
	pParentItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pParentItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pParentItem->m_idProp)!=-1)
		pParentItem->m_bHideChildren=CLDSApp::GetPropStatus(pParentItem->m_idProp);
	//�Ŷ����
	oper.InsertEditPropItem(pParentItem,"g_sysPara.FootNailDist");
	//�Ŷ����׷�Χ
	oper.InsertEditPropItem(pParentItem,"g_sysPara.fNailRepScope");
	//����˨��ƹ�����ʾ��ӵ��Ŀ�
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.b1BoltDesignPrompt");
	//��˨ͨ�����ִ�й��� wht 19-08-23
	oper.InsertCmbListPropItem(pParentItem, "CLDSBolt::BOLTL0_CALMODE");
	//��˨ͨ���������ʼֵ wht 19-08-23
	oper.InsertCmbListPropItem(pParentItem, "CLDSBolt::BOLTL0_PREFER_MODE");
	//��Ȧ����������(��ʱδ���� ����һ���汾��������߼���������� wjh-2019.9.3)
	//pPropItem=oper.InsertEditPropItem(pParentItem,"MaxWasherN");
	//����˨��Ȧ��С���
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
	//����������Ʋ���
	pParentItem=oper.InsertGrayButtonPropItem(pRootItem,"g_sysPara.intelligentConnect");
	pParentItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pParentItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pParentItem->m_idProp)!=-1)
		pParentItem->m_bHideChildren=CLDSApp::GetPropStatus(pParentItem->m_idProp);
	//��������Ƶ�ǰ��ͼ������
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.intelligentConnect.m_bOnlyDesignCurView");
	//����������ͨ������Ʋ���
	pPropItem=oper.InsertCmbListPropItem(pParentItem,"g_sysPara.intelligentConnect.m_bDesignCommonConnect");
	pPropItem->m_bHideChildren=FALSE;
	if(g_sysPara.intelligentConnect.m_bDesignCommonConnect)	//����������ͨ������Ʋ���
		oper.InsertCmbListPropItem(pPropItem,"g_sysPara.intelligentConnect.m_bDesignBendConnect");
	//�������ܽǸֽ�ͷ��Ʋ���
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.intelligentConnect.m_bDesignAngleJoint");
	//�������ֹܸܷ�����Ʋ���
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.intelligentConnect.m_bDesignFL");
	//�������ܽǸֽ�ͷ��Ʋ���
	oper.InsertCmbListPropItem(pParentItem,"g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate");
#endif
	//��������ͬ������
	pParentItem=oper.InsertPropItem(pRootItem,"SyncProp");
	pParentItem->m_dwPropGroup=GetSingleWord(iSectionType);
	pParentItem->m_bHideChildren=FALSE;
	if(CLDSApp::GetPropStatus(pParentItem->m_idProp)!=-1)
		pParentItem->m_bHideChildren=CLDSApp::GetPropStatus(pParentItem->m_idProp);
	//�ڵ�
	oper.InsertButtonPropItem(pParentItem,"SyncProp.Node");
	//�Ǹ�
	oper.InsertButtonPropItem(pParentItem,"SyncProp.Angle");
#ifdef __COMPLEX_PART_INC_	
	//�ֹ�
	oper.InsertButtonPropItem(pParentItem,"SyncProp.Tube");
#endif
	//�ְ�
	oper.InsertButtonPropItem(pParentItem,"SyncProp.Plate");
}