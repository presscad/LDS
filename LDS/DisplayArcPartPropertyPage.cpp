//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "CfgPartNoDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "LmaDlg.h"
#include "Query.h"
#include "AddNewBoltDlg.h"
#include "RelativeObjectDlg.h"
#include "dlg.h"
#include "ReportPointDlg.h"
#include "ModifyRelativeObjPartNoDlg.h"
#include "DefPlaneDlg.h"
#include "DesignArcPartBaseLineDlg.h"
#include "ConfirmArcPartStyleDlg.h"
#include "BaseLineStartEndPosDlg.h"
#include "XhCharString.h"
#include "DatumPointPropertyDlg.h"
#include "DesignVectorPropDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//from DisplayPropertyPage.cpp
BOOL SetPartMutiObjsProp(CLDSObject *pObj, char *propName,long idClassType=0);

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL ModifyArcPartItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSArcPart::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL ModifyArcAngleItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSArcAngle::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL ModifyArcFlatItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSArcFlat::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL ModifyArcSlotItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSArcSlot::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL ArcPartButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSArcPart *pFirstPart=NULL, *pPart=NULL;
	CCfgPartNoDlg cfgdlg;

	if(pView->selectObjs.GetNodeNum()>0)
		pFirstPart = (CLDSArcPart*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	if(pItem->m_idProp==CLDSArcPart::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstPart->cfgword; 
		if(pFirstPart->layer(0)!='L')
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
			pFirstPart->cfgword = cfgdlg.cfgword;		//在此处更新第一个节点的配材料号
		char sText[MAX_TEMP_CHAR_100+1]="";
		if(pFirstPart->GetPropValueStr(pItem->m_idProp,sText))	//更新配材号
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else
		return FALSE;
	return TRUE;
}

BOOL ArcAngleButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSArcAngle *pFirstArcAngle=NULL, *pArcAngle=NULL;
	CLDSArcPart* pMirArcPart=NULL;
	CReportVectorDlg vecDlg;
	CReportPointDlg pointDlg;
	CCfgPartNoDlg cfgdlg;
	CString valueStr="";

	if(pView->selectObjs.GetNodeNum()>0)
		pFirstArcAngle = (CLDSArcAngle*)pView->selectObjs.GetFirst();
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
	if(pItem->m_idProp==CLDSArcAngle::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstArcAngle->cfgword; 
		if(pFirstArcAngle->layer(0)!='L')
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
			pFirstArcAngle->cfgword = cfgdlg.cfgword;	//在此处更新第一个节点的配材料号
		char sText[MAX_TEMP_CHAR_100+1]="";
		if(pFirstArcAngle->GetPropValueStr(pItem->m_idProp,sText))	//更新配材号
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("sPartNo"))
	{
		CModifyRelativeObjPartNoDlg modify_partno_dlg;
		modify_partno_dlg.m_pWorkPart=pFirstArcAngle;
		if(modify_partno_dlg.DoModal()==IDOK)
		{
			pFirstArcAngle->SetPartNo(modify_partno_dlg.m_sCurObjPartNo.GetBuffer(0));
			pPropList->SetItemPropValue(CLDSArcAngle::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("m_siLayoutStyle"))
	{
		CConfirmArcPartStyleDlg confirm_style_dlg;
		confirm_style_dlg.m_iArcPartType=0;	//环形角钢
		confirm_style_dlg.m_iArcPartLayStyle=pFirstArcAngle->m_siLayoutStyle;
		if(confirm_style_dlg.DoModal()==IDOK)
		{
			for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
				pArcAngle->m_siLayoutStyle=confirm_style_dlg.m_iArcPartLayStyle;
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("startPos"))
	{
		CBaseLineStartEndPosDlg start_pos_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
		start_pos_dlg.m_sCaption="Basic line start coord.";
#else
		start_pos_dlg.m_sCaption="基准线起点坐标";
#endif
		start_pos_dlg.m_pWorkPart=pFirstArcAngle;
		start_pos_dlg.m_fPointX=pFirstArcAngle->baseline.Start().x;
		start_pos_dlg.m_fPointY=pFirstArcAngle->baseline.Start().y;
		start_pos_dlg.m_fPointZ=pFirstArcAngle->baseline.Start().z;
		start_pos_dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("endPos"))
	{
		CBaseLineStartEndPosDlg end_pos_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
		end_pos_dlg.m_sCaption="Basic line end coord.";
#else
		end_pos_dlg.m_sCaption="基准线终点坐标";
#endif
		end_pos_dlg.m_pWorkPart=pFirstArcAngle;
		end_pos_dlg.m_fPointX=pFirstArcAngle->baseline.End().x;
		end_pos_dlg.m_fPointY=pFirstArcAngle->baseline.End().y;
		end_pos_dlg.m_fPointZ=pFirstArcAngle->baseline.End().z;
		end_pos_dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("des_arc_center"))
	{
		CDatumPointPropertyDlg datumPointDlg;
		datumPointDlg.m_pCurEditObj=pFirstArcAngle;
		datumPointDlg.m_xPoint=pFirstArcAngle->des_arc_center;
		if(datumPointDlg.DoModal()==IDOK)
		{
			pFirstArcAngle->des_arc_center = datumPointDlg.m_xPoint;
			pFirstArcAngle->DesignArcBaseLine();
			CXhChar100 sItemValue;
			if(pFirstArcAngle->GetPropValueStr(pItem->m_idProp,sItemValue)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,sItemValue);
			long idProp = CLDSArcAngle::GetPropID("arc_radius");
			if(pFirstArcAngle->GetPropValueStr(idProp,sItemValue)>0)
				pPropList->SetItemPropValue(idProp,sItemValue);
			//更新中性层半径
			pFirstArcAngle->radius = pFirstArcAngle->baseline.Radius();
			idProp = CLDSArcAngle::GetPropID("neutro_radius");
			if(pFirstArcAngle->GetPropValueStr(idProp,sItemValue)>0)
				pPropList->SetItemPropValue(idProp,sItemValue);
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("des_arc_norm"))
	{
		CDesignVectorPropDlg vectorPropDlg;
		vectorPropDlg.m_hCurEditPart=pFirstArcAngle->handle;
		vectorPropDlg.m_xVector=pFirstArcAngle->des_arc_norm;
		if(vectorPropDlg.DoModal()==IDOK)
		{
			pFirstArcAngle->des_arc_norm = vectorPropDlg.m_xVector;
			pFirstArcAngle->DesignArcBaseLine();
			CXhChar100 sPoint;
			if(pFirstArcAngle->GetPropValueStr(pItem->m_idProp,sPoint)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,sPoint);
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("des_arc_start"))
	{
		CDatumPointPropertyDlg datumPointDlg;
		datumPointDlg.m_pCurEditObj=pFirstArcAngle;
		datumPointDlg.m_xPoint=pFirstArcAngle->des_arc_start;
		if(datumPointDlg.DoModal()==IDOK)
		{
			pFirstArcAngle->des_arc_start = datumPointDlg.m_xPoint;
			pFirstArcAngle->DesignArcBaseLine();
			CXhChar100 sItemValue;
			if(pFirstArcAngle->GetPropValueStr(pItem->m_idProp,sItemValue)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,sItemValue);
			long idProp = CLDSArcAngle::GetPropID("arc_radius");
			if(pFirstArcAngle->GetPropValueStr(idProp,sItemValue)>0)
				pPropList->SetItemPropValue(idProp,sItemValue);
			//更新中性层半径
			pFirstArcAngle->radius = pFirstArcAngle->baseline.Radius();
			idProp = CLDSArcAngle::GetPropID("neutro_radius");
			if(pFirstArcAngle->GetPropValueStr(idProp,sItemValue)>0)
				pPropList->SetItemPropValue(idProp,sItemValue);
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("m_hPartWeldParent"))
	{
		//通过选择构件修改焊接父构件，现在仅可以修改单个构件的焊接父构件
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify welding parent part of arc angle";
		modifyhdlg.m_sTitle="Weld parent part:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstArcAngle->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//display node choice button
		modifyhdlg.SetCmdPickPromptStr("Please select welding parent part:");
#else
		modifyhdlg.m_sCaption="修改环形角钢焊接父构件";
		modifyhdlg.m_sTitle="焊接父构件:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstArcAngle->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
		modifyhdlg.SetCmdPickPromptStr("请选择焊接父构件:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
			{
#if defined(__LDS_FILE_)||defined(__LDS_)
				MessageBox(NULL,"输入句柄有误，找不到相应构件！","LDS",MB_OK);
#elif defined(__LMA_FILE_)||defined(__LMA_)
				MessageBox(NULL,"输入句柄有误，找不到相应构件！","LMA",MB_OK);
#elif defined(__TMA_FILE_)||defined(__TMA_)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"handle input is wrong, corresponding part can't be found!!","TMA",MB_OK);
#else
				MessageBox(NULL,"输入句柄有误，找不到相应构件！","TMA",MB_OK);
#endif
#endif
			}
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSArcAngle::GetPropID("m_hPartWeldParent"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLinePart* pMirLinePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirArcPart=(CLDSArcAngle*)pArcAngle->GetMirPart(msg);
						if(pMirArcPart)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//查找所选父构件的对称构件 wht 11-05-07
								pMirArcPart->m_hPartWeldParent=pMirPart->handle;
						}
					}
					pArcAngle->m_hPartWeldParent=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pArcAngle->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("hStart"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Rod Start Node";
		modifyhdlg.m_sTitle="Start Node:";
#else 
		modifyhdlg.m_sCaption="修改杆件始端节点";
		modifyhdlg.m_sTitle="始端节点:";
#endif
		if(pFirstArcAngle->pStart)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstArcAngle->pStart->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
		modifyhdlg.m_bEnableMir=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the datum node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("请选择基准节点:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind nodes！",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"输入句柄有误，找不到相应节点！",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSArcAngle::GetPropID("hStart"),valueStr);
				for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
					pArcAngle->pStart=pNode;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("hEnd"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Rod End Node";
		modifyhdlg.m_sTitle="End Node:";
#else 
		modifyhdlg.m_sCaption="修改杆件终端节点";
		modifyhdlg.m_sTitle="终端节点:";
#endif
		if(pFirstArcAngle->pEnd)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstArcAngle->pEnd->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
		modifyhdlg.m_bEnableMir=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the datum node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("请选择基准节点:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind nodes！",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"输入句柄有误，找不到相应节点！",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSArcAngle::GetPropID("hEnd"),valueStr);
				for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
					pArcAngle->pEnd=pNode;
			}
		}
	}
	else 
		return FALSE;
	for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
	{
		pArcAngle->SetModified();
		pArcAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pArcAngle->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL ArcFlatButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSArcFlat *pFirstArcFlat=NULL, *pArcFlat=NULL;
	CReportVectorDlg vecDlg;
	CReportPointDlg pointDlg;
	CCfgPartNoDlg cfgdlg;
	CString valueStr="";

	if(pView->selectObjs.GetNodeNum()>0)
		pFirstArcFlat = (CLDSArcFlat*)pView->selectObjs.GetFirst();
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
	if(pItem->m_idProp==CLDSArcFlat::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstArcFlat->cfgword; 
		if(pFirstArcFlat->layer(0)!='L')
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
			pFirstArcFlat->cfgword = cfgdlg.cfgword;		//在此处更新第一个节点的配材料号
		char sText[MAX_TEMP_CHAR_100+1]="";
		if(pFirstArcFlat->GetPropValueStr(pItem->m_idProp,sText))	//更新配材号
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("sPartNo"))
	{
		CModifyRelativeObjPartNoDlg modify_partno_dlg;
		modify_partno_dlg.m_pWorkPart=pFirstArcFlat;
		if(modify_partno_dlg.DoModal()==IDOK)
		{
			pFirstArcFlat->SetPartNo(modify_partno_dlg.m_sCurObjPartNo.GetBuffer(0));
			pPropList->SetItemPropValue(CLDSArcFlat::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("startPos"))
	{
		CBaseLineStartEndPosDlg start_pos_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
		start_pos_dlg.m_sCaption="Basic line start coord.";
#else
		start_pos_dlg.m_sCaption="基准线起点坐标";
#endif
		start_pos_dlg.m_pWorkPart=pFirstArcFlat;
		start_pos_dlg.m_fPointX=pFirstArcFlat->baseline.Start().x;
		start_pos_dlg.m_fPointY=pFirstArcFlat->baseline.Start().y;
		start_pos_dlg.m_fPointZ=pFirstArcFlat->baseline.Start().z;
		start_pos_dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("endPos"))
	{
		CBaseLineStartEndPosDlg end_pos_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
		end_pos_dlg.m_sCaption="Basic line end coord.";
#else
		end_pos_dlg.m_sCaption="基准线终点坐标";
#endif
		end_pos_dlg.m_pWorkPart=pFirstArcFlat;
		end_pos_dlg.m_fPointX=pFirstArcFlat->baseline.End().x;
		end_pos_dlg.m_fPointY=pFirstArcFlat->baseline.End().y;
		end_pos_dlg.m_fPointZ=pFirstArcFlat->baseline.End().z;
		end_pos_dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("des_arc_center"))
	{
		CDatumPointPropertyDlg datumPointDlg;
		datumPointDlg.m_pCurEditObj=pFirstArcFlat;
		datumPointDlg.m_xPoint=pFirstArcFlat->des_arc_center;
		if(datumPointDlg.DoModal()==IDOK)
		{
			pFirstArcFlat->des_arc_center = datumPointDlg.m_xPoint;
			pFirstArcFlat->DesignArcBaseLine();
			CXhChar100 sItemValue;
			if(pFirstArcFlat->GetPropValueStr(pItem->m_idProp,sItemValue)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,sItemValue);
			long idProp = CLDSArcAngle::GetPropID("arc_radius");
			if(pFirstArcFlat->GetPropValueStr(idProp,sItemValue)>0)
				pPropList->SetItemPropValue(idProp,sItemValue);
			//更新中性层半径
			pFirstArcFlat->radius = pFirstArcFlat->baseline.Radius();
			idProp = CLDSArcAngle::GetPropID("neutro_radius");
			if(pFirstArcFlat->GetPropValueStr(idProp,sItemValue)>0)
				pPropList->SetItemPropValue(idProp,sItemValue);
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("des_arc_norm"))
	{
		CDesignVectorPropDlg vectorPropDlg;
		vectorPropDlg.m_hCurEditPart=pFirstArcFlat->handle;
		vectorPropDlg.m_xVector=pFirstArcFlat->des_arc_norm;
		if(vectorPropDlg.DoModal()==IDOK)
		{
			pFirstArcFlat->des_arc_norm = vectorPropDlg.m_xVector;
			pFirstArcFlat->DesignArcBaseLine();
			CXhChar100 sPoint;
			if(pFirstArcFlat->GetPropValueStr(pItem->m_idProp,sPoint)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,sPoint);
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("des_arc_start"))
	{
		CDatumPointPropertyDlg datumPointDlg;
		datumPointDlg.m_pCurEditObj=pFirstArcFlat;
		datumPointDlg.m_xPoint=pFirstArcFlat->des_arc_start;
		if(datumPointDlg.DoModal()==IDOK)
		{
			pFirstArcFlat->des_arc_start = datumPointDlg.m_xPoint;
			pFirstArcFlat->DesignArcBaseLine();
			CXhChar100 sItemValue;
			if(pFirstArcFlat->GetPropValueStr(pItem->m_idProp,sItemValue)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,sItemValue);
			long idProp = CLDSArcAngle::GetPropID("arc_radius");
			if(pFirstArcFlat->GetPropValueStr(idProp,sItemValue)>0)
				pPropList->SetItemPropValue(idProp,sItemValue);
			//更新中性层半径
			pFirstArcFlat->radius = pFirstArcFlat->baseline.Radius();
			idProp = CLDSArcAngle::GetPropID("neutro_radius");
			if(pFirstArcFlat->GetPropValueStr(idProp,sItemValue)>0)
				pPropList->SetItemPropValue(idProp,sItemValue);
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("m_hPartWeldParent"))
	{
		//通过选择构件修改焊接父构件，现在仅可以修改单个构件的焊接父构件
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify welding parent part of arc flat steel";
		modifyhdlg.m_sTitle="Weld parent part:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstArcFlat->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//display node choice button
		modifyhdlg.SetCmdPickPromptStr("Please select welding parent part:");
#else
		modifyhdlg.m_sCaption="修改环形扁钢焊接父构件";
		modifyhdlg.m_sTitle="焊接父构件:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstArcFlat->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
		modifyhdlg.SetCmdPickPromptStr("请选择焊接父构件:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
			{
#if defined(__LDS_FILE_)||defined(__LDS_)
				MessageBox(NULL,"输入句柄有误，找不到相应构件！","LDS",MB_OK);
#elif defined(__LMA_FILE_)||defined(__LMA_)
				MessageBox(NULL,"输入句柄有误，找不到相应构件！","LMA",MB_OK);
#elif defined(__TMA_FILE_)||defined(__TMA_)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"handle input is wrong, corresponding part can't be found!","TMA",MB_OK);
#else
				MessageBox(NULL,"输入句柄有误，找不到相应构件！","TMA",MB_OK);
#endif
#endif
			}
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSArcFlat::GetPropID("m_hPartWeldParent"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSArcPart* pMirArcPart=NULL; 
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirArcPart=(CLDSArcPart*)pArcFlat->GetMirPart(msg);
						if(pMirArcPart)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//查找所选父构件的对称构件 wht 11-05-07
								pMirArcPart->m_hPartWeldParent=pMirPart->handle;
						}
					}
					pArcFlat->m_hPartWeldParent=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pArcFlat->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("hStart"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Rod Start Node";
		modifyhdlg.m_sTitle="Start Node:";
#else 
		modifyhdlg.m_sCaption="修改杆件始端节点";
		modifyhdlg.m_sTitle="始端节点:";
#endif
		if(pFirstArcFlat->pStart)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstArcFlat->pStart->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
		modifyhdlg.m_bEnableMir=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the datum node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("请选择基准节点:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind nodes！",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"输入句柄有误，找不到相应节点！",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSArcFlat::GetPropID("hStart"),valueStr);
				for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
					pArcFlat->pStart=pNode;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("hEnd"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Rod End Node";
		modifyhdlg.m_sTitle="End Node:";
#else 
		modifyhdlg.m_sCaption="修改杆件终端节点";
		modifyhdlg.m_sTitle="终端节点:";
#endif
		if(pFirstArcFlat->pEnd)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstArcFlat->pEnd->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
		modifyhdlg.m_bEnableMir=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the datum node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("请选择基准节点:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind nodes！",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"输入句柄有误，找不到相应节点！",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSArcFlat::GetPropID("hEnd"),valueStr);
				for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
					pArcFlat->pEnd=pNode;
			}
		}
	}
	else 
		return FALSE;
	for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
	{
		pArcFlat->SetModified();
		pArcFlat->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pArcFlat->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL ArcSlotButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSArcSlot *pFirstArcSlot=NULL, *pArcSlot=NULL;
	CLDSArcPart* pMirArcPart=NULL;
	CReportVectorDlg vecDlg;
	CReportPointDlg pointDlg;
	CCfgPartNoDlg cfgdlg;
	CString valueStr="";

	if(pView->selectObjs.GetNodeNum()>0)
		pFirstArcSlot = (CLDSArcSlot*)pView->selectObjs.GetFirst();
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
	if(pItem->m_idProp==CLDSArcSlot::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstArcSlot->cfgword; 
		if(pFirstArcSlot->layer(0)!='L')
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
			pFirstArcSlot->cfgword = cfgdlg.cfgword;	//在此处更新第一个节点的配材料号
		char sText[MAX_TEMP_CHAR_100+1]="";
		if(pFirstArcSlot->GetPropValueStr(pItem->m_idProp,sText))	//更新配材号
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("sPartNo"))
	{
		CModifyRelativeObjPartNoDlg modify_partno_dlg;
		modify_partno_dlg.m_pWorkPart=pFirstArcSlot;
		if(modify_partno_dlg.DoModal()==IDOK)
		{
			pFirstArcSlot->SetPartNo(modify_partno_dlg.m_sCurObjPartNo.GetBuffer(0));
			pPropList->SetItemPropValue(CLDSArcSlot::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("m_siLayoutStyle"))
	{
		CConfirmArcPartStyleDlg confirm_style_dlg;
		confirm_style_dlg.m_iArcPartType=1;	//环形槽钢
		confirm_style_dlg.m_iArcPartLayStyle=pFirstArcSlot->m_siLayoutStyle;
		if(confirm_style_dlg.DoModal()==IDOK)
		{
			for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
				pArcSlot->m_siLayoutStyle=confirm_style_dlg.m_iArcPartLayStyle;
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("startPos"))
	{
		CBaseLineStartEndPosDlg start_pos_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
		start_pos_dlg.m_sCaption="Basic line start coord.";
#else
		start_pos_dlg.m_sCaption="基准线起点坐标";
#endif
		start_pos_dlg.m_pWorkPart=pFirstArcSlot;
		start_pos_dlg.m_fPointX=pFirstArcSlot->baseline.Start().x;
		start_pos_dlg.m_fPointY=pFirstArcSlot->baseline.Start().y;
		start_pos_dlg.m_fPointZ=pFirstArcSlot->baseline.Start().z;
		start_pos_dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("endPos"))
	{
		CBaseLineStartEndPosDlg end_pos_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
		end_pos_dlg.m_sCaption="Basic line end coord.";
#else
		end_pos_dlg.m_sCaption="基准线终点坐标";
#endif
		end_pos_dlg.m_pWorkPart=pFirstArcSlot;
		end_pos_dlg.m_fPointX=pFirstArcSlot->baseline.End().x;
		end_pos_dlg.m_fPointY=pFirstArcSlot->baseline.End().y;
		end_pos_dlg.m_fPointZ=pFirstArcSlot->baseline.End().z;
		end_pos_dlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("des_arc_center"))
	{
		CDatumPointPropertyDlg datumPointDlg;
		datumPointDlg.m_pCurEditObj=pFirstArcSlot;
		datumPointDlg.m_xPoint=pFirstArcSlot->des_arc_center;
		if(datumPointDlg.DoModal()==IDOK)
		{
			pFirstArcSlot->des_arc_center = datumPointDlg.m_xPoint;
			pFirstArcSlot->DesignArcBaseLine();
			CXhChar100 sItemValue;
			if(pFirstArcSlot->GetPropValueStr(pItem->m_idProp,sItemValue)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,sItemValue);
			long idProp = CLDSArcAngle::GetPropID("arc_radius");
			if(pFirstArcSlot->GetPropValueStr(idProp,sItemValue)>0)
				pPropList->SetItemPropValue(idProp,sItemValue);
			//更新中性层半径
			pFirstArcSlot->radius = pFirstArcSlot->baseline.Radius();
			idProp = CLDSArcAngle::GetPropID("neutro_radius");
			if(pFirstArcSlot->GetPropValueStr(idProp,sItemValue)>0)
				pPropList->SetItemPropValue(idProp,sItemValue);
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("des_arc_norm"))
	{
		CDesignVectorPropDlg vectorPropDlg;
		vectorPropDlg.m_hCurEditPart=pFirstArcSlot->handle;
		vectorPropDlg.m_xVector=pFirstArcSlot->des_arc_norm;
		if(vectorPropDlg.DoModal()==IDOK)
		{
			pFirstArcSlot->des_arc_norm = vectorPropDlg.m_xVector;
			pFirstArcSlot->DesignArcBaseLine();
			CXhChar100 sPoint;
			if(pFirstArcSlot->GetPropValueStr(pItem->m_idProp,sPoint)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,sPoint);
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("des_arc_start"))
	{
		CDatumPointPropertyDlg datumPointDlg;
		datumPointDlg.m_pCurEditObj=pFirstArcSlot;
		datumPointDlg.m_xPoint=pFirstArcSlot->des_arc_start;
		if(datumPointDlg.DoModal()==IDOK)
		{
			pFirstArcSlot->des_arc_start = datumPointDlg.m_xPoint;
			pFirstArcSlot->DesignArcBaseLine();
			CXhChar100 sItemValue;
			if(pFirstArcSlot->GetPropValueStr(pItem->m_idProp,sItemValue)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,sItemValue);
			long idProp = CLDSArcAngle::GetPropID("arc_radius");
			if(pFirstArcSlot->GetPropValueStr(idProp,sItemValue)>0)
				pPropList->SetItemPropValue(idProp,sItemValue);
			//更新中性层半径
			pFirstArcSlot->radius = pFirstArcSlot->baseline.Radius();
			idProp = CLDSArcAngle::GetPropID("neutro_radius");
			if(pFirstArcSlot->GetPropValueStr(idProp,sItemValue)>0)
				pPropList->SetItemPropValue(idProp,sItemValue);
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("m_hPartWeldParent"))
	{
		//通过选择构件修改焊接父构件，现在仅可以修改单个构件的焊接父构件
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify welding parent part of arc slot";
		modifyhdlg.m_sTitle="Weld parent parts:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstArcSlot->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//display node choice button
		modifyhdlg.SetCmdPickPromptStr("Please select welding parent part:");
#else
		modifyhdlg.m_sCaption="修改环形槽钢焊接父构件";
		modifyhdlg.m_sTitle="焊接父构件:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstArcSlot->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
		modifyhdlg.SetCmdPickPromptStr("请选择焊接父构件:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
			{
#if defined(__LDS_FILE_)||defined(__LDS_)
				MessageBox(NULL,"输入句柄有误，找不到相应构件！","LDS",MB_OK);
#elif defined(__LMA_FILE_)||defined(__LMA_)
				MessageBox(NULL,"输入句柄有误，找不到相应构件！","LMA",MB_OK);
#elif defined(__TMA_FILE_)||defined(__TMA_)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"handle input is wrong, corresponding part can't be found!","TMA",MB_OK);
#else
				MessageBox(NULL,"输入句柄有误，找不到相应构件！","TMA",MB_OK);
#endif
#endif
			}
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSArcSlot::GetPropID("m_hPartWeldParent"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirArcPart=(CLDSArcPart*)pArcSlot->GetMirPart(msg);
						if(pMirArcPart)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//查找所选父构件的对称构件 wht 11-05-07
								pMirArcPart->m_hPartWeldParent=pMirPart->handle;
						}
					}
					pArcSlot->m_hPartWeldParent=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pArcSlot->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("hStart"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Rod Start Node";
		modifyhdlg.m_sTitle="Start Node:";
#else 
		modifyhdlg.m_sCaption="修改杆件始端节点";
		modifyhdlg.m_sTitle="始端节点:";
#endif
		if(pFirstArcSlot->pStart)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstArcSlot->pStart->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
		modifyhdlg.m_bEnableMir=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the datum node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("请选择基准节点:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind nodes！",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"输入句柄有误，找不到相应节点！",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSArcSlot::GetPropID("hStart"),valueStr);
				for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
					pArcSlot->pStart=pNode;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("hEnd"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Rod End Node";
		modifyhdlg.m_sTitle="End Node:";
#else 
		modifyhdlg.m_sCaption="修改杆件终端节点";
		modifyhdlg.m_sTitle="终端节点:";
#endif
		if(pFirstArcSlot->pEnd)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstArcSlot->pEnd->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//显示选择节点的按钮
		modifyhdlg.m_bEnableMir=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the datum node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("请选择基准节点:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind nodes！",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"输入句柄有误，找不到相应节点！",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSArcSlot::GetPropID("hEnd"),valueStr);
				for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
					pArcSlot->pEnd=pNode;
			}
		}
	}
	else 
		return FALSE;
	for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
	{
		pArcSlot->SetModified();
		pArcSlot->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pArcSlot->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}
BOOL ModifyArcPartProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSArcPart *pFirstPart = (CLDSArcPart*)pView->selectObjs.GetFirst();
	CLDSArcPart *pPart = NULL;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	if(pItem->m_idProp==CLDSArcPart::GetPropID("layer"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->SetLayer(tem_str);	
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("cMaterial"))
	{	//材质
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->cMaterial = QuerySteelBriefMatMark(tem_str);
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("m_cQualityLevel"))
	{	//质量等级
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->m_cQualityLevel = valueStr[0];
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("iSeg"))
	{
		SEGI iSeg = SEGI(valueStr.GetBuffer());
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->iSeg = iSeg;
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("sPartNo"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->SetPartNo(tem_str);
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("cfgword"))
	{	//更新其他杆件的配材号
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->cfgword = pFirstPart->cfgword;
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("m_uStatMatCoef"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->m_uStatMatCoef = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("m_fZoomLenCoef"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->m_fZoomLenCoef = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("m_hPartWeldParent"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->m_hPartWeldParent=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("m_bDisplayHole"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pPart->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
				pPart->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("否")==0)
#endif
				pPart->m_bDisplayHole = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("m_bVirtualPart"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pPart->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
				pPart->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("否")==0)
#endif
				pPart->m_bVirtualPart = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("m_uDisplaySlices"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->m_uDisplaySlices = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("neutro_radius"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->radius = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("offset_h"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->offset_h = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("offset_v"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			pPart->offset_v = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("m_bUserSpecColor"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pPart->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
				pPart->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("否")==0)
#endif
				pPart->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pPart->relativeObjs.GetFirst();pMirObj;pMirObj=pPart->relativeObjs.GetNext())
			{
				CLDSArcPart *pMirArcPart=(CLDSArcPart*)pPart->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCPART);
				if(pMirArcPart)
					pMirArcPart->m_bUserSpecColor=pPart->m_bUserSpecColor;
			}
		}
		if(pFirstPart->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSArcPart::GetPropID("crMaterial"),NULL);
			if(pFindItem==NULL)
			{
				char sText[MAX_TEMP_CHAR_100+1]="";
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "part's color";
				lpInfo->m_strPropHelp = "part's color";
#else
				lpInfo->m_strPropName = "构件颜色";
				lpInfo->m_strPropHelp = "构件颜色";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = CLDSArcPart::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if(pFirstPart->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSArcPart::GetPropID("crMaterial"));
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//跳过RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
		{
			pPart->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pPart->relativeObjs.GetFirst();pMirObj;pMirObj=pPart->relativeObjs.GetNext())
			{
				CLDSArcPart *pMirArcPart=(CLDSArcPart*)pPart->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCPART);
				if(pMirArcPart)
					pMirArcPart->crMaterial=pPart->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcPart::GetPropID("sNotes"))
	{
		for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
			_snprintf(pPart->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
	}
	else
		return FALSE;
	for(pPart=(CLDSArcPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSArcPart*)pView->selectObjs.GetNext())
	{
		pPart->SetModified();
		pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL ModifyArcAngleProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSArcAngle *pFirstArcAngle = (CLDSArcAngle*)pView->selectObjs.GetFirst();
	CLDSArcAngle *pArcAngle = NULL;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	const int GROUP_GENERAL=1,GROUP_GEOMETRY=2;
	if(pItem->m_idProp==CLDSArcAngle::GetPropID("layer"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->SetLayer(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//旋转对称
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					CalMirAtomLayer(pArcAngle->layer(),pMirArcAngle->layer(),pMirObj->mirInfo);
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("cMaterial"))
	{	//材质
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->cMaterial = QuerySteelBriefMatMark(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->cMaterial=pArcAngle->cMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("m_cQualityLevel"))
	{	//质量等级
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->m_cQualityLevel = valueStr[0];
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->m_cQualityLevel=pArcAngle->m_cQualityLevel;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("iSeg"))
	{
		SEGI iSeg = SEGI(valueStr.GetBuffer());
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->iSeg = iSeg;
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->iSeg=pArcAngle->iSeg;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("sPartNo"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
			pArcAngle->SetPartNo(tem_str);
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("cfgword"))
	{	//更新其他杆件的配材号
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->cfgword = pFirstArcAngle->cfgword;
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->cfgword=pArcAngle->cfgword;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("m_uStatMatCoef"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
			pArcAngle->m_uStatMatCoef = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("m_fZoomLenCoef"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
			pArcAngle->m_fZoomLenCoef = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("m_hPartWeldParent"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
			pArcAngle->m_hPartWeldParent=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("m_bDisplayHole"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pArcAngle->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
				pArcAngle->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("否")==0)
#endif
				pArcAngle->m_bDisplayHole = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->m_bDisplayHole=pArcAngle->m_bDisplayHole;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("m_bVirtualPart"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pArcAngle->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
				pArcAngle->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("否")==0)
#endif
				pArcAngle->m_bVirtualPart = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->m_bVirtualPart=pArcAngle->m_bVirtualPart;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("m_bUserSpecColor"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pArcAngle->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
				pArcAngle->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("否")==0)
#endif
				pArcAngle->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->m_bUserSpecColor=pArcAngle->m_bUserSpecColor;
			}
		}
		if(pFirstArcAngle->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSArcAngle::GetPropID("crMaterial"),NULL);
			if(pFindItem==NULL)
			{
				char sText[MAX_TEMP_CHAR_100+1]="";
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "part's color";
				lpInfo->m_strPropHelp = "part's color";
#else
				lpInfo->m_strPropName = "构件颜色";
				lpInfo->m_strPropHelp = "构件颜色";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = CLDSArcAngle::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if(pFirstArcAngle->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSArcAngle::GetPropID("crMaterial"));
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//跳过RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->crMaterial=pArcAngle->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("m_uDisplaySlices"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
			pArcAngle->m_uDisplaySlices = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("specification")) 
	{	//规格
		double wing_wide,wing_thick;
		restore_JG_guige(tem_str,wing_wide,wing_thick);
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->wing_thick=wing_thick;
			pArcAngle->wing_wide=wing_wide;
			if(pArcAngle->m_siLayoutStyle==0)
			{
				pArcAngle->offset_h=0.5*wing_wide;	//横向位移
				pArcAngle->offset_v=-0.5*wing_wide;	//竖向位移
			}
			else if(pArcAngle->m_siLayoutStyle==1)
			{
				pArcAngle->offset_h=0.5*wing_wide;	//横向位移
				pArcAngle->offset_v=0.5*wing_wide;	//竖向位移
			}
			else if(pArcAngle->m_siLayoutStyle==2)
			{
				pArcAngle->offset_h=-0.5*wing_wide;	//横向位移
				pArcAngle->offset_v=0.5*wing_wide;	//竖向位移
			}
			else if(pArcAngle->m_siLayoutStyle==3)
			{
				pArcAngle->offset_h=-0.5*wing_wide;	//横向位移
				pArcAngle->offset_v=-0.5*wing_wide;	//竖向位移
			}
		}
		if(pFirstArcAngle->GetPropValueStr(CLDSArcAngle::GetPropID("wing_wide"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcAngle::GetPropID("wing_wide"),CString(tem_str));
		if(pFirstArcAngle->GetPropValueStr(CLDSArcAngle::GetPropID("wing_thick"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcAngle::GetPropID("wing_thick"),CString(tem_str));
		if(pFirstArcAngle->GetPropValueStr(CLDSArcAngle::GetPropID("offset_v"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcAngle::GetPropID("offset_v"),CString(tem_str));
		if(pFirstArcAngle->GetPropValueStr(CLDSArcAngle::GetPropID("offset_h"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcAngle::GetPropID("offset_h"),CString(tem_str));
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("start_rot_angle"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->start_rot_angle = atof(valueStr)*RADTODEG_COEF;
			pArcAngle->DesignArcBaseLine();
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
				{
					pMirArcAngle->start_rot_angle=pArcAngle->start_rot_angle;
					pMirArcAngle->DesignArcBaseLine();
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("sector_angle"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->sector_angle=atof(valueStr)*RADTODEG_COEF;
			pArcAngle->DesignArcBaseLine();
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
				{
					pMirArcAngle->sector_angle = pArcAngle->sector_angle;
					pMirArcAngle->DesignArcBaseLine();
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("start_oddment"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->start_oddment = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->start_oddment=pArcAngle->start_oddment;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("end_oddment"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->end_oddment = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->end_oddment=pArcAngle->end_oddment;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("arc_radius"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->baseline.SetRadius(atof(valueStr));
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->baseline.SetRadius(atof(valueStr));
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("neutro_radius"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->radius = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->radius=atof(valueStr);
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("offset_h"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->offset_h = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->offset_h=pArcAngle->offset_h;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("offset_v"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->offset_v = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->offset_v=pArcAngle->offset_v;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("wing_thick"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->wing_thick = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->wing_thick=pArcAngle->wing_thick;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("wing_wide"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->wing_wide = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					pMirArcAngle->wing_wide=pArcAngle->wing_wide;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("sNotes"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			_snprintf(pArcAngle->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
			//备注不用关联修改 wht 10-10-22
			/*for(RELATIVE_OBJECT *pMirObj=pArcAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pArcAngle->relativeObjs.GetNext())
			{
				CLDSArcAngle *pMirArcAngle=(CLDSArcAngle*)pArcAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCANGLE);
				if(pMirArcAngle)
					strcpy(pMirArcAngle->sNotes,pArcAngle->sNotes);
			}*/
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("ucs.origin.x"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
			pArcAngle->ucs.origin.x = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("ucs.origin.y"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
			pArcAngle->ucs.origin.y = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("ucs.origin.z"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
			pArcAngle->ucs.origin.z = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("ucs.axis_x.x"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->ucs.axis_x.x = atof(valueStr);
			rotate_ucs_axis(pArcAngle->ucs,pArcAngle->ucs.origin+pArcAngle->ucs.axis_x*1,0);
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("ucs.axis_x.y"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->ucs.axis_x.y = atof(valueStr);
			rotate_ucs_axis(pArcAngle->ucs,pArcAngle->ucs.origin+pArcAngle->ucs.axis_x*1,0);
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("ucs.axis_x.z"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->ucs.axis_x.z = atof(valueStr);
			rotate_ucs_axis(pArcAngle->ucs,pArcAngle->ucs.origin+pArcAngle->ucs.axis_x*1,0);
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("ucs.axis_y.x"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->ucs.axis_y.x = atof(valueStr);
			rotate_ucs_axis(pArcAngle->ucs,pArcAngle->ucs.origin+pArcAngle->ucs.axis_y*1,1);
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("ucs.axis_y.y"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->ucs.axis_y.y = atof(valueStr);
			rotate_ucs_axis(pArcAngle->ucs,pArcAngle->ucs.origin+pArcAngle->ucs.axis_y*1,1);
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("ucs.axis_y.z"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->ucs.axis_y.z = atof(valueStr);
			rotate_ucs_axis(pArcAngle->ucs,pArcAngle->ucs.origin+pArcAngle->ucs.axis_y*1,1);
		}	
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("ucs.axis_z.x"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->ucs.axis_z.x = atof(valueStr);
			rotate_ucs_axis(pArcAngle->ucs,pArcAngle->ucs.origin+pArcAngle->ucs.axis_z*1,2);
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("ucs.axis_z.y"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->ucs.axis_z.y = atof(valueStr);
			rotate_ucs_axis(pArcAngle->ucs,pArcAngle->ucs.origin+pArcAngle->ucs.axis_z*1,2);
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("ucs.axis_z.z"))
	{
		for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
		{
			pArcAngle->ucs.axis_z.z = atof(valueStr);
			rotate_ucs_axis(pArcAngle->ucs,pArcAngle->ucs.origin+pArcAngle->ucs.axis_z*1,2);
		}
	}
	else 
		return FALSE;
	for(pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetFirst();pArcAngle;pArcAngle=(CLDSArcAngle*)pView->selectObjs.GetNext())
	{
		pArcAngle->SetModified();
		pArcAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pArcAngle->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL ModifyArcSlotProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSArcSlot *pFirstArcSlot = (CLDSArcSlot*)pView->selectObjs.GetFirst();
	CLDSArcSlot *pArcSlot = NULL;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	const int GROUP_GENERAL=1,GROUP_GEOMETRY=2;
	if(pItem->m_idProp==CLDSArcSlot::GetPropID("layer"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->SetLayer(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//旋转对称
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					CalMirAtomLayer(pArcSlot->layer(),pMirArcSlot->layer(),pMirObj->mirInfo);
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("cMaterial"))
	{	//材质
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->cMaterial = QuerySteelBriefMatMark(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->cMaterial=pArcSlot->cMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("m_cQualityLevel"))
	{	//质量等级
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->m_cQualityLevel = valueStr[0];
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->m_cQualityLevel=pArcSlot->m_cQualityLevel;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("iSeg"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->iSeg = SEGI(valueStr.GetBuffer());
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->iSeg=pArcSlot->iSeg;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("sPartNo"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
			pArcSlot->SetPartNo(tem_str);
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("cfgword"))
	{	//更新其他杆件的配材号
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->cfgword = pFirstArcSlot->cfgword;
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->cfgword=pArcSlot->cfgword;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("m_uStatMatCoef"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
			pArcSlot->m_uStatMatCoef = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("m_fZoomLenCoef"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
			pArcSlot->m_fZoomLenCoef = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("m_hPartWeldParent"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
			pArcSlot->m_hPartWeldParent=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("m_bDisplayHole"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pArcSlot->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
				pArcSlot->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("否")==0)
#endif
				pArcSlot->m_bDisplayHole = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->m_bDisplayHole=pArcSlot->m_bDisplayHole;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("m_bVirtualPart"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pArcSlot->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
				pArcSlot->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("否")==0)
#endif
				pArcSlot->m_bVirtualPart = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->m_bVirtualPart=pArcSlot->m_bVirtualPart;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("m_bUserSpecColor"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pArcSlot->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
				pArcSlot->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("否")==0)
#endif
				pArcSlot->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->m_bUserSpecColor=pArcSlot->m_bUserSpecColor;
			}
		}
		if(pFirstArcSlot->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSArcSlot::GetPropID("crMaterial"),NULL);
			if(pFindItem==NULL)
			{
				char sText[MAX_TEMP_CHAR_100+1]="";
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "part's color";
				lpInfo->m_strPropHelp = "Tpart's color";
#else
				lpInfo->m_strPropName = "构件颜色";
				lpInfo->m_strPropHelp = "构件颜色";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = CLDSArcSlot::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if(pFirstArcSlot->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSArcSlot::GetPropID("crMaterial"));
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//跳过RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->crMaterial=pArcSlot->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("m_uDisplaySlices"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->m_uDisplaySlices = atoi(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->m_uDisplaySlices=pArcSlot->m_uDisplaySlices;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("specification")) 
	{	//规格
		SLOT_PARA_TYPE *pSlotPara=FindSlotType(tem_str);
		if(pSlotPara==NULL)
			return FALSE;
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			_snprintf(pArcSlot->spec,MAX_CHAR_GUIGE_16,"%s",tem_str);
			pArcSlot->wide=pSlotPara->b;
			pArcSlot->thick=pSlotPara->d;
			pArcSlot->height=pSlotPara->h;
			if(pArcSlot->m_siLayoutStyle==0)
			{
				pArcSlot->offset_h=0.5*pArcSlot->wide;	//横向位移
				pArcSlot->offset_v=0.5*pArcSlot->height;//竖向位移
			}
			else if(pArcSlot->m_siLayoutStyle==1)
			{
				pArcSlot->offset_h=-0.5*pArcSlot->wide;	//横向位移
				pArcSlot->offset_v=0.5*pArcSlot->height;//竖向位移
			}
			else if(pArcSlot->m_siLayoutStyle==2)
			{
				pArcSlot->offset_h=-0.5*pArcSlot->wide;	//横向位移
				pArcSlot->offset_v=-0.5*pArcSlot->height;//竖向位移
			}
			else if(pArcSlot->m_siLayoutStyle==3)
			{
				pArcSlot->offset_h=0.5*pArcSlot->wide;	//横向位移
				pArcSlot->offset_v=-0.5*pArcSlot->height;//竖向位移
			}
		}
		if(pFirstArcSlot->GetPropValueStr(CLDSArcSlot::GetPropID("wide"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcSlot::GetPropID("wide"),CString(tem_str));
		if(pFirstArcSlot->GetPropValueStr(CLDSArcSlot::GetPropID("thick"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcSlot::GetPropID("thick"),CString(tem_str));
		if(pFirstArcSlot->GetPropValueStr(CLDSArcSlot::GetPropID("height"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcSlot::GetPropID("height"),CString(tem_str));
		if(pFirstArcSlot->GetPropValueStr(CLDSArcSlot::GetPropID("offset_v"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcSlot::GetPropID("offset_v"),CString(tem_str));
		if(pFirstArcSlot->GetPropValueStr(CLDSArcSlot::GetPropID("offset_h"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcSlot::GetPropID("offset_h"),CString(tem_str));
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("thick"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->thick = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->thick=pArcSlot->thick;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("wide"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->wide = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->wide=pArcSlot->wide;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("height"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->height = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->height=pArcSlot->height;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("arc_radius"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->baseline.SetRadius(atof(valueStr));
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->baseline.SetRadius(atof(valueStr));
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("neutro_radius"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->radius = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->radius=pArcSlot->radius;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("offset_h"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->offset_h = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->offset_h=pArcSlot->offset_h;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("offset_v"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->offset_v = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->offset_v=pArcSlot->offset_v;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("start_rot_angle"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->start_rot_angle = atof(valueStr)*RADTODEG_COEF;
			pArcSlot->DesignArcBaseLine();
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
				{
					pMirArcSlot->start_rot_angle = pArcSlot->start_rot_angle;
					pMirArcSlot->DesignArcBaseLine();
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("sector_angle"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->sector_angle=atof(valueStr)*RADTODEG_COEF;
			pArcSlot->DesignArcBaseLine();
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
				{
					pMirArcSlot->sector_angle = pArcSlot->sector_angle;
					pMirArcSlot->DesignArcBaseLine();
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("start_oddment"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->start_oddment = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->start_oddment=pArcSlot->start_oddment;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("end_oddment"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->end_oddment = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					pMirArcSlot->end_oddment=pArcSlot->end_oddment;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("sNotes"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			_snprintf(pArcSlot->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
			//备注不用关联修改 wht 10-10-22
			/*for(RELATIVE_OBJECT *pMirObj=pArcSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pArcSlot->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcSlot=(CLDSArcSlot*)pArcSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCSLOT);
				if(pMirArcSlot)
					strcpy(pMirArcSlot->sNotes,pArcSlot->sNotes);
			}*/
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("ucs.origin.x"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
			pArcSlot->ucs.origin.x = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("ucs.origin.y"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
			pArcSlot->ucs.origin.y = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("ucs.origin.z"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
			pArcSlot->ucs.origin.z = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("ucs.axis_x.x"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->ucs.axis_x.x = atof(valueStr);
			rotate_ucs_axis(pArcSlot->ucs,pArcSlot->ucs.origin+pArcSlot->ucs.axis_x*1,0);
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("ucs.axis_x.y"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->ucs.axis_x.y = atof(valueStr);
			rotate_ucs_axis(pArcSlot->ucs,pArcSlot->ucs.origin+pArcSlot->ucs.axis_x*1,0);
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("ucs.axis_x.z"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->ucs.axis_x.z = atof(valueStr);
			rotate_ucs_axis(pArcSlot->ucs,pArcSlot->ucs.origin+pArcSlot->ucs.axis_x*1,0);
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("ucs.axis_y.x"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->ucs.axis_y.x = atof(valueStr);
			rotate_ucs_axis(pArcSlot->ucs,pArcSlot->ucs.origin+pArcSlot->ucs.axis_y*1,1);
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("ucs.axis_y.y"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->ucs.axis_y.y = atof(valueStr);
			rotate_ucs_axis(pArcSlot->ucs,pArcSlot->ucs.origin+pArcSlot->ucs.axis_y*1,1);
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("ucs.axis_y.z"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->ucs.axis_y.z = atof(valueStr);
			rotate_ucs_axis(pArcSlot->ucs,pArcSlot->ucs.origin+pArcSlot->ucs.axis_y*1,1);
		}	
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("ucs.axis_z.x"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->ucs.axis_z.x = atof(valueStr);
			rotate_ucs_axis(pArcSlot->ucs,pArcSlot->ucs.origin+pArcSlot->ucs.axis_z*1,2);
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("ucs.axis_z.y"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->ucs.axis_z.y = atof(valueStr);
			rotate_ucs_axis(pArcSlot->ucs,pArcSlot->ucs.origin+pArcSlot->ucs.axis_z*1,2);
		}
	}
	else if(pItem->m_idProp==CLDSArcSlot::GetPropID("ucs.axis_z.z"))
	{
		for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
		{
			pArcSlot->ucs.axis_z.z = atof(valueStr);
			rotate_ucs_axis(pArcSlot->ucs,pArcSlot->ucs.origin+pArcSlot->ucs.axis_z*1,2);
		}
	}
	else 
		return FALSE;
	for(pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetFirst();pArcSlot;pArcSlot=(CLDSArcSlot*)pView->selectObjs.GetNext())
	{
		pArcSlot->SetModified();
		pArcSlot->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pArcSlot->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL ModifyArcFlatProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSArcFlat *pFirstArcFlat = (CLDSArcFlat*)pView->selectObjs.GetFirst();
	CLDSArcFlat *pArcFlat = NULL;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	
	const int GROUP_GENERAL=1,GROUP_GEOMETRY=2;
	if(pItem->m_idProp==CLDSArcFlat::GetPropID("layer"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->SetLayer(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//旋转对称
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					CalMirAtomLayer(pArcFlat->layer(),pMirArcFlat->layer(),pMirObj->mirInfo);
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("cMaterial"))
	{	//材质
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->cMaterial = QuerySteelBriefMatMark(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->cMaterial=pArcFlat->cMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("m_cQualityLevel"))
	{	//质量等级
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->m_cQualityLevel = valueStr[0];
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->m_cQualityLevel=pArcFlat->m_cQualityLevel;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("iSeg"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->iSeg = SEGI(valueStr.GetBuffer());
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->iSeg=pArcFlat->iSeg;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("sPartNo"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
			pArcFlat->SetPartNo(tem_str);
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("cfgword"))
	{	//更新其他杆件的配材号
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->cfgword = pFirstArcFlat->cfgword;
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->cfgword=pArcFlat->cfgword;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("m_uStatMatCoef"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
			pArcFlat->m_uStatMatCoef = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("m_fZoomLenCoef"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
			pArcFlat->m_fZoomLenCoef = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("m_hPartWeldParent"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
			pArcFlat->m_hPartWeldParent=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("m_bDisplayHole"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pArcFlat->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
				pArcFlat->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("否")==0)
#endif
				pArcFlat->m_bDisplayHole = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->m_bDisplayHole=pArcFlat->m_bDisplayHole;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("m_bVirtualPart"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pArcFlat->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
				pArcFlat->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("否")==0)
#endif
				pArcFlat->m_bVirtualPart = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->m_bVirtualPart=pArcFlat->m_bVirtualPart;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("m_uDisplaySlices"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->m_uDisplaySlices = atoi(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->m_uDisplaySlices=pArcFlat->m_uDisplaySlices;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcAngle::GetPropID("specification")) 
	{	//规格
		double wide,thick;
		restore_JG_guige(tem_str,wide,thick);
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->thick=thick;
			pArcFlat->wide=wide;
			pArcFlat->offset_v=-0.5*wide;	//竖向位移
			pArcFlat->offset_h=0;
		}
		if(pFirstArcFlat->GetPropValueStr(CLDSArcFlat::GetPropID("wide"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcFlat::GetPropID("wide"),CString(tem_str));
		if(pFirstArcFlat->GetPropValueStr(CLDSArcFlat::GetPropID("thick"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcFlat::GetPropID("thick"),CString(tem_str));
		if(pFirstArcFlat->GetPropValueStr(CLDSArcFlat::GetPropID("offset_v"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcFlat::GetPropID("offset_v"),CString(tem_str));
		if(pFirstArcFlat->GetPropValueStr(CLDSArcFlat::GetPropID("offset_h"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSArcFlat::GetPropID("offset_h"),CString(tem_str));
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("start_rot_angle"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->start_rot_angle = atof(valueStr)*RADTODEG_COEF;
			pArcFlat->DesignArcBaseLine();
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcFlat *pMirArcFlat=(CLDSArcFlat*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
				{
					pMirArcFlat->start_rot_angle=pArcFlat->start_rot_angle;
					pMirArcFlat->DesignArcBaseLine();
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("sector_angle"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->sector_angle=atof(valueStr)*RADTODEG_COEF;
			pArcFlat->DesignArcBaseLine();
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcFlat *pMirArcFlat=(CLDSArcFlat*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
				{
					pMirArcFlat->sector_angle = pArcFlat->sector_angle;
					pMirArcFlat->DesignArcBaseLine();
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("start_oddment"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->start_oddment = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcFlat *pMirArcFlat=(CLDSArcFlat*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->start_oddment=pArcFlat->start_oddment;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("end_oddment"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->end_oddment = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcFlat *pMirArcFlat=(CLDSArcFlat*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->end_oddment=pArcFlat->end_oddment;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("arc_radius"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->baseline.SetRadius(atof(valueStr));
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->baseline.SetRadius(atof(valueStr));
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("neutro_radius"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->radius = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->radius=pArcFlat->radius;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("offset_h"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->offset_h = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->offset_h=pArcFlat->offset_h;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("offset_v"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->offset_v = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->offset_v=pArcFlat->offset_v;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("thick"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->thick = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->thick=pArcFlat->thick;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("wide"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->wide = atof(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->wide=pArcFlat->wide;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("sNotes"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			_snprintf(pArcFlat->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
			//备注不用关联修改 wht 10-10-22
			/*for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcSlot *pMirArcFlat=(CLDSArcSlot*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					strcpy(pMirArcFlat->sNotes,pArcFlat->sNotes);
			}*/
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("m_bUserSpecColor"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pArcFlat->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
				pArcFlat->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("否")==0)
#endif
				pArcFlat->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcFlat *pMirArcFlat=(CLDSArcFlat*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->m_bUserSpecColor=pArcFlat->m_bUserSpecColor;
			}
		}
		if(pFirstArcFlat->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSArcFlat::GetPropID("crMaterial"),NULL);
			if(pFindItem==NULL)
			{
				char sText[MAX_TEMP_CHAR_100+1]="";
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "part's color";
				lpInfo->m_strPropHelp = "part's color";
#else
				lpInfo->m_strPropName = "构件颜色";
				lpInfo->m_strPropHelp = "构件颜色";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = CLDSArcFlat::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if(pFirstArcFlat->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSArcFlat::GetPropID("crMaterial"));
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//跳过RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pArcFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pArcFlat->relativeObjs.GetNext())
			{
				CLDSArcFlat *pMirArcFlat=(CLDSArcFlat*)pArcFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_ARCFLAT);
				if(pMirArcFlat)
					pMirArcFlat->crMaterial=pArcFlat->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("ucs.origin.x"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
			pArcFlat->ucs.origin.x = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("ucs.origin.y"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
			pArcFlat->ucs.origin.y = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("ucs.origin.z"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
			pArcFlat->ucs.origin.z = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("ucs.axis_x.x"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->ucs.axis_x.x = atof(valueStr);
			rotate_ucs_axis(pArcFlat->ucs,pArcFlat->ucs.origin+pArcFlat->ucs.axis_x*1,0);
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("ucs.axis_x.y"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->ucs.axis_x.y = atof(valueStr);
			rotate_ucs_axis(pArcFlat->ucs,pArcFlat->ucs.origin+pArcFlat->ucs.axis_x*1,0);
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("ucs.axis_x.z"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->ucs.axis_x.z = atof(valueStr);
			rotate_ucs_axis(pArcFlat->ucs,pArcFlat->ucs.origin+pArcFlat->ucs.axis_x*1,0);
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("ucs.axis_y.x"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->ucs.axis_y.x = atof(valueStr);
			rotate_ucs_axis(pArcFlat->ucs,pArcFlat->ucs.origin+pArcFlat->ucs.axis_y*1,1);
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("ucs.axis_y.y"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->ucs.axis_y.y = atof(valueStr);
			rotate_ucs_axis(pArcFlat->ucs,pArcFlat->ucs.origin+pArcFlat->ucs.axis_y*1,1);
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("ucs.axis_y.z"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->ucs.axis_y.z = atof(valueStr);
			rotate_ucs_axis(pArcFlat->ucs,pArcFlat->ucs.origin+pArcFlat->ucs.axis_y*1,1);
		}	
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("ucs.axis_z.x"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->ucs.axis_z.x = atof(valueStr);
			rotate_ucs_axis(pArcFlat->ucs,pArcFlat->ucs.origin+pArcFlat->ucs.axis_z*1,2);
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("ucs.axis_z.y"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->ucs.axis_z.y = atof(valueStr);
			rotate_ucs_axis(pArcFlat->ucs,pArcFlat->ucs.origin+pArcFlat->ucs.axis_z*1,2);
		}
	}
	else if(pItem->m_idProp==CLDSArcFlat::GetPropID("ucs.axis_z.z"))
	{
		for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
		{
			pArcFlat->ucs.axis_z.z = atof(valueStr);
			rotate_ucs_axis(pArcFlat->ucs,pArcFlat->ucs.origin+pArcFlat->ucs.axis_z*1,2);
		}
	}
	else 
		return FALSE;
	for(pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetFirst();pArcFlat;pArcFlat=(CLDSArcFlat*)pView->selectObjs.GetNext())
	{
		pArcFlat->SetModified();
		pArcFlat->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pArcFlat->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

void CLDSView::DisplayArcPartProperty(BOOL bRebuild)
{
	CLDSArcPart *pFirstPart = (CLDSArcPart*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	if(bRebuild)
	{	//需要重建对象属性页
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(0);
		pPropDlg->RefreshTabCtrl(0);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_ARCPART;
		//设置属性栏回调函数
		pPropList->SetModifyStatusFunc(ModifyArcPartItemStatus);
		pPropList->SetModifyValueFunc(ModifyArcPartProperty);
		pPropList->SetButtonClickFunc(ArcPartButtonClick);
		//初始化属性项只读状态初始值,块内构件或映射构件属性不允许编辑 wht 12-10-16
		if(!pFirstPart->BelongModel()->IsTowerModel()||pFirstPart->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//基本信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Arc part basic info";
		lpInfo->m_strPropHelp = "Basic info of arc parts";
#else
		lpInfo->m_strPropName = "环形构件基本信息";
		lpInfo->m_strPropHelp = "环形构件基本信息";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcPart::GetPropID("basicInfo");
		if(CLDSArcPart::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcPart::GetPropStatus(pGroupItem->m_idProp);
			//句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "Handle is the only mark of part in LDS system";
#else
		lpInfo->m_strPropName = "句柄";
		lpInfo->m_strPropHelp = "句柄是LDS系统中对构件的全局唯一标识";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("handle");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
			//图层
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "layer";
		lpInfo->m_strPropHelp = "part's layer";
#else
		lpInfo->m_strPropName = "图层";
		lpInfo->m_strPropHelp = "构件所在图层";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("layer");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//材质	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "material";
		lpInfo->m_strPropHelp = "steel material";
#else
		lpInfo->m_strPropName = "材质";
		lpInfo->m_strPropHelp = "材质";
#endif
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390|Q420|Q460";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("cMaterial");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//质量等级	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Quality Grade";
		lpInfo->m_strPropHelp = "steel quality grade";
#else
		lpInfo->m_strPropName = "质量等级";
		lpInfo->m_strPropHelp = "质量等级";
#endif
		lpInfo->m_cmbItems = "A|B|C|D|";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("m_cQualityLevel");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//分段号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Segment No.";
		lpInfo->m_strPropHelp = "Segment No.";
#else
		lpInfo->m_strPropName = "分段号";
		lpInfo->m_strPropHelp = "分段号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("iSeg");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//构件编号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part No.";
		lpInfo->m_strPropHelp = "Part No.";
#else
		lpInfo->m_strPropName = "构件编号";
		lpInfo->m_strPropHelp = "构件编号"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("sPartNo");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//配材号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "CFG word";
		lpInfo->m_strPropHelp = "CFG word";
#else
		lpInfo->m_strPropName = "配材号";
		lpInfo->m_strPropHelp = "配材号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("cfgword");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//统材系数
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Stat. Material Coef";
		lpInfo->m_strPropHelp = "Statistics material coefficient";
#else
		lpInfo->m_strPropName = "统材系数";
		lpInfo->m_strPropHelp = "统材系数";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("m_uStatMatCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		//下料系数
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Len Zoom Coef";
		lpInfo->m_strPropHelp = "len zoom coefficient";
#else
		lpInfo->m_strPropName = "下料系数";
		lpInfo->m_strPropHelp = "加工时相对于圆弧基线的长度缩放系数";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("m_fZoomLenCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		  //焊接父构件
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Welding parent part";
		lpInfo->m_strPropHelp = "If the part needs to be welded onto other part, its property is the weld part's handle, if the part does not need welding, the value is0X0.";
#else
		lpInfo->m_strPropName = "焊接父构件";
		lpInfo->m_strPropHelp = "如果此构件需要焊接到其它构件上,那么此属性就是本构件所依附的焊接构件的句柄;如果此构件不需要焊接,此值为0X0。";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("m_hPartWeldParent");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//显示孔
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "show hole";
		lpInfo->m_strPropHelp = "is show hole or not";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "显示孔";
		lpInfo->m_strPropHelp = "是否显示孔";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("m_bDisplayHole");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//虚拟辅助构件
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual auxiliary parts";
		lpInfo->m_strPropHelp = "Virtual auxiliary parts are only used to assist other parts positioning, not real BOM part, so not included in material statistics.";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "虚拟辅助构件";
		lpInfo->m_strPropHelp = "虚拟辅助构件只用于辅助其它构件定位，并不是结构体真实具有的统材构件，所以不参预材料统计。";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("m_bVirtualPart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//用户指定构件颜色
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specified part color";
		lpInfo->m_strPropHelp = "is part color specified by the users";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "指定构件颜色";
		lpInfo->m_strPropHelp = "构件颜色是否为用户指定";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstPart->m_bUserSpecColor)
		{	//构件颜色
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "part color";
			lpInfo->m_strPropHelp = "part color";
#else
			lpInfo->m_strPropName = "构件颜色";
			lpInfo->m_strPropHelp = "构件颜色";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSArcPart::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstPart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		/*	//显示片数
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "显示片数";
		lpInfo->m_strPropHelp = "圆柱形回转体的显示片数";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("m_uDisplaySlices");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;*/
			//圆弧半径
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "neutrosphere arc radius";
		lpInfo->m_strPropHelp = "curved neutrosphere arc radius";
#else
		lpInfo->m_strPropName = "中性层圆弧半径";
		lpInfo->m_strPropHelp = "弯曲中性层的圆弧半径";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("neutro_radius");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//横向位移
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "radial offset";
		lpInfo->m_strPropHelp = "radial offset is lateral displacement from generatrix to section shape features origin (calibration)";
#else
		lpInfo->m_strPropName = "横向位移";
		lpInfo->m_strPropHelp = "基准母线距离截面形状特征(标定)原点的横向位移";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("offset_h");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//竖向位移
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "normal offset";
		lpInfo->m_strPropHelp = "normal offset is vertical displacement from generatrix to section shape features origin (calibration)";
#else
		lpInfo->m_strPropName = "竖向位移";
		lpInfo->m_strPropHelp = "基准母线距离截面形状特征(标定)原点的竖向位移";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("offset_v");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//备注
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "notes";
		lpInfo->m_strPropHelp = "notes"; 
#else
		lpInfo->m_strPropName = "备注";
		lpInfo->m_strPropHelp = "备注"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcPart::GetPropID("sNotes");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
	}
	pPropList->Redraw();
}
void UpdateArcAngleStart_EndPositionProperty(CPropertyList *pPropList,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSArcAngle *pFirstArcAngle = (CLDSArcAngle*)pView->selectObjs.GetFirst();
	if(pFirstArcAngle==NULL)
		return;
	CPropTreeItem* pParentItem=NULL,*pPropItem=NULL,*pSonPropItem=NULL;
	if(bStart)
		pParentItem=pPropList->FindItemByPropId(CLDSArcAngle::GetPropID("startInfo"),NULL);
	else
		pParentItem=pPropList->FindItemByPropId(CLDSArcAngle::GetPropID("endInfo"),NULL);
	if(pParentItem==NULL)
		return;
	long propId=0;
	CString sPropName,sPropHelp;
	CItemInfo* lpInfo=NULL;
	char sText[MAX_TEMP_CHAR_100+1]="";
	//节点句柄
	if(bStart)
	{
		propId=CLDSArcAngle::GetPropID("hStart");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName = "start handle";
#else
		sPropName = "起点句柄";
#endif
	}
	else
	{
		propId=CLDSArcAngle::GetPropID("hEnd");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName = "start handle";
#else
		sPropName = "终点句柄";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_strPropName =sPropName;
	lpInfo->m_strPropHelp =sPropName;
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
	pPropItem->m_idProp = propId;
	if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//正负头
	if(bStart)
	{
		propId=CLDSArcAngle::GetPropID("start_oddment");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName = "start Oddment";
#else
		sPropName = "始端正负头";
#endif
	}
	else
	{
		propId=CLDSArcAngle::GetPropID("end_oddment");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName = "end Oddment";
#else
		sPropName = "终端正负头";
#endif		
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName =sPropName;
	lpInfo->m_strPropHelp =sPropName;
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
	pPropItem->m_idProp = propId;
	if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//始终端坐标
	if(bStart)
	{
		propId=CLDSArcAngle::GetPropID("startPos");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName="Start Design Coordinate";
#else
		sPropName="始端设计坐标";
#endif
	}
	else
	{
		propId=CLDSArcAngle::GetPropID("endPos");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName="End Design Coordinate";
#else
		sPropName="终端设计坐标";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_strPropName = sPropName;
	lpInfo->m_strPropHelp = sPropName;
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
	pPropItem->m_idProp = propId;
	if(CLDSArcAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSArcAngle::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//X
	if(bStart)
	{
		propId=CLDSArcAngle::GetPropID("startPos.x");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="Start Design Coordinate X";
#else
		sPropHelp="环形角钢始端X轴方向的坐标";
#endif
	}
	else
	{
		propId=CLDSArcAngle::GetPropID("endPos.x");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="End Design Coordinate X";
#else
		sPropHelp="环形角钢终端X轴方向的坐标";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
#else 
	lpInfo->m_strPropName = "X坐标";
#endif
	lpInfo->m_strPropHelp=sPropHelp;
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
	pSonPropItem->m_idProp = propId;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	if(pFirstArcAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//Y
	if(bStart)
	{
		propId=CLDSArcAngle::GetPropID("startPos.y");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="Start Design Coordinate Y";
#else
		sPropHelp="环形角钢始端Y轴方向的坐标";
#endif
	}
	else
	{
		propId=CLDSArcAngle::GetPropID("endPos.y");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="End Design Coordinate Y";
#else
		sPropHelp="环形角钢终端Y轴方向的坐标";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
#else 
	lpInfo->m_strPropName = "Y坐标";
#endif
	lpInfo->m_strPropHelp = sPropHelp;
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
	pSonPropItem->m_idProp = propId;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	if(pFirstArcAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//Z
	if(bStart)
	{
		propId=CLDSArcAngle::GetPropID("startPos.z");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="Start Design Coordinate Z";
#else
		sPropHelp="环形角钢始端Z轴方向的坐标";
#endif
	}
	else
	{
		propId=CLDSArcAngle::GetPropID("endPos.z");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="End Design Coordinate Z";
#else
		sPropHelp="环形角钢终端Z轴方向的坐标";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
#else 
	lpInfo->m_strPropName = "Z坐标";
#endif
	lpInfo->m_strPropHelp = sPropHelp;
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
	pSonPropItem->m_idProp = propId;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	if(pFirstArcAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
//	//终端实际坐标
//	if(bStart)
//	{
//		propId=CLDSArcAngle::GetPropID("actualStartPos");
//#ifdef AFX_TARG_ENU_ENGLISH
//		sPropName="Start Actual Coordinate"
//		sPropHelp="The start coordinate of arc angle after considering oddment";
//#else
//		sPropName="始端实际坐标";
//		sPropHelp="环形角钢始端考虑正负头后的坐标位置";
//#endif
//	}
//	else
//	{
//		propId=CLDSArcAngle::GetPropID("actualEndPos");
//#ifdef AFX_TARG_ENU_ENGLISH
//		sPropName="End Actual Coordinate"
//		sPropHelp="End Design Coordinate Z";
//#else
//		sPropName="终端实际坐标";
//		sPropHelp="环形角钢终端考虑正负头后的坐标位置";
//#endif
//	}
//	lpInfo = new CItemInfo();
//	lpInfo->m_controlType = PIT_BUTTON;
//	lpInfo->m_strPropName = sPropName;
//	lpInfo->m_strPropHelp = sPropHelp;
//	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
//	pPropItem->m_idProp = propId;
//	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
}
void CLDSView::DisplayArcAngleProperty(BOOL bRebuild)
{
	CLDSArcAngle *pFirstArcAngle = (CLDSArcAngle*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	
	const int GROUP_GENERAL=1,GROUP_GEOMETRY=2,GROUP_START=3,GROUP_END=4;
	if(bRebuild)
	{	//需要重建对象属性页
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		
		pPropDlg->m_arrPropGroupLabel.SetSize(4);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GEOMETRY-1,"Geometry");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"Start");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"End");
#else
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"常规");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GEOMETRY-1,"形位");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"始端");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"终端");
#endif
		pPropDlg->RefreshTabCtrl(CLDSArcAngle::m_iCurDisplayPropGroup);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_ARCANGLE;
		//设置属性栏回调函数
		pPropList->SetModifyStatusFunc(ModifyArcAngleItemStatus);
		pPropList->SetModifyValueFunc(ModifyArcAngleProperty);
		pPropList->SetButtonClickFunc(ArcAngleButtonClick);
		//初始化属性项只读状态初始值,块内构件或映射构件属性不允许编辑 wht 12-10-16
		if(!pFirstArcAngle->BelongModel()->IsTowerModel()||pFirstArcAngle->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//基本信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "arc angle basic info";
		lpInfo->m_strPropHelp = "basic info of arc angle";
#else
		lpInfo->m_strPropName = "环形角钢基本信息";
		lpInfo->m_strPropHelp = "环形角钢基本信息";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcAngle::GetPropID("basicInfo");
		if(CLDSArcAngle::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcAngle::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_GEOMETRY)
									|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
			//句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle ";
		lpInfo->m_strPropHelp = "Handle is the only mark of part in LDS system";
#else
		lpInfo->m_strPropName = "句柄";
		lpInfo->m_strPropHelp = "句柄是LDS系统中对构件的全局唯一标识";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("handle");
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_GEOMETRY)
									|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
		pPropItem->SetReadOnly();
			//图层
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "layer";
		lpInfo->m_strPropHelp = "part layer";
#else
		lpInfo->m_strPropName = "图层";
		lpInfo->m_strPropHelp = "构件所在图层";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("layer");
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//材质	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "material";
		lpInfo->m_strPropHelp = "steel material";
#else
		lpInfo->m_strPropName = "材质";
		lpInfo->m_strPropHelp = "材质";
#endif
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390|Q420|Q460";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("cMaterial");
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//质量等级	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Quality Grade";
		lpInfo->m_strPropHelp = "steel quality grade";
#else
		lpInfo->m_strPropName = "质量等级";
		lpInfo->m_strPropHelp = "质量等级";
#endif
		lpInfo->m_cmbItems = "A|B|C|D|";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("m_cQualityLevel");
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//分段号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "segment No.";
		lpInfo->m_strPropHelp = "segment No.";
#else
		lpInfo->m_strPropName = "分段号";
		lpInfo->m_strPropHelp = "分段号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("iSeg");
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//构件编号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part No.";
		lpInfo->m_strPropHelp = "Part No."; 
#else
		lpInfo->m_strPropName = "构件编号";
		lpInfo->m_strPropHelp = "构件编号"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("sPartNo");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//配材号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "CFG word";
		lpInfo->m_strPropHelp = "CFG word";
#else
		lpInfo->m_strPropName = "配材号";
		lpInfo->m_strPropHelp = "配材号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("cfgword");
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//统材系数
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Stat. Material Coef";
		lpInfo->m_strPropHelp = "Statistics material coefficient";
#else
		lpInfo->m_strPropName = "统材系数";
		lpInfo->m_strPropHelp = "统材系数";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("m_uStatMatCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//下料系数
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Len Zoom Coef";
		lpInfo->m_strPropHelp = "len zoom coefficient";
#else
		lpInfo->m_strPropName = "下料系数";
		lpInfo->m_strPropHelp = "加工时相对于圆弧基线的长度缩放系数";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("m_fZoomLenCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		  //焊接父构件
		lpInfo = new CItemInfo();
		lpInfo->m_controlType =	PIT_BUTTON;	//PIT_EDIT
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weld parent part";
		lpInfo->m_strPropHelp = "If the part needs to be welded onto other part, its property is the weld part's handle, if the part does not need welding, the value is0X0.";
#else
		lpInfo->m_strPropName = "焊接父构件";
		lpInfo->m_strPropHelp = "如果此构件需要焊接到其它构件上,那么此属性就是本构件所依附的焊接构件的句柄;如果此构件不需要焊接,此值为0X0。";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("m_hPartWeldParent");
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//显示孔
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "show hole";
		lpInfo->m_strPropHelp = "is show hole or not";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "显示孔";
		lpInfo->m_strPropHelp = "是否显示孔";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("m_bDisplayHole");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//虚拟辅助构件
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual auxiliary parts";
		lpInfo->m_strPropHelp = "Virtual auxiliary parts are only used to assist other parts positioning, not real BOM part, so not included in material statistics.";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "虚拟辅助构件";
		lpInfo->m_strPropHelp = "虚拟辅助构件只用于辅助其它构件定位，并不是结构体真实具有的统材构件，所以不参预材料统计。";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("m_bVirtualPart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//用户指定构件颜色
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specify part's color";
		lpInfo->m_strPropHelp = "is part's color specified by the users";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "指定构件颜色";
		lpInfo->m_strPropHelp = "构件颜色是否为用户指定";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstArcAngle->m_bUserSpecColor)
		{	//构件颜色
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "part's color";
			lpInfo->m_strPropHelp = "part's color";
#else
			lpInfo->m_strPropName = "构件颜色";
			lpInfo->m_strPropHelp = "构件颜色";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSArcAngle::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstArcAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
			//显示片数
		/*lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "显示片数";
		lpInfo->m_strPropHelp = "圆柱形回转体的显示片数";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("m_uDisplaySlices");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);*/
			//备注
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "notes";
		lpInfo->m_strPropHelp = "notes"; 
#else
		lpInfo->m_strPropName = "备注";
		lpInfo->m_strPropHelp = "备注"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("sNotes");
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		
		//基本信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "geometry info";
		lpInfo->m_strPropHelp = "geometry information";
#else
		lpInfo->m_strPropName = "形位信息";
		lpInfo->m_strPropHelp = "形位信息";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcAngle::GetPropID("geometryInfo");
		if(CLDSArcAngle::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcAngle::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//规格
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "spec";
		lpInfo->m_strPropHelp = "specifications";
#else
		lpInfo->m_strPropName = "规格型号";
		lpInfo->m_strPropHelp = "规格型号";
#endif
		lpInfo->m_cmbItems = MakeAngleSetString();
		lpInfo->m_cmbType = CDT_EDIT;
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("specification");
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//肢宽
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "width";
		lpInfo->m_strPropHelp = "leg width";
#else
		lpInfo->m_strPropName = "肢宽";
		lpInfo->m_strPropHelp = "肢宽";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("wing_wide");
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		pPropItem->SetReadOnly();
			//肢厚
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "thickness";
		lpInfo->m_strPropHelp = "leg thickness";
#else
		lpInfo->m_strPropName = "肢厚";
		lpInfo->m_strPropHelp = "肢厚";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("wing_thick");
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		pPropItem->SetReadOnly();
			//圆弧半径
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "arc radius ";
		lpInfo->m_strPropHelp = "arc radius ";
#else
		lpInfo->m_strPropName = "圆弧半径";
		lpInfo->m_strPropHelp = "圆弧半径";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("arc_radius");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		pPropItem->SetReadOnly();
			//中性层圆弧半径
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "neutrosphere arc radius";
		lpInfo->m_strPropHelp = "curved neutrosphere arc radius";
#else
		lpInfo->m_strPropName = "中性层圆弧半径";
		lpInfo->m_strPropHelp = "弯曲中性层的圆弧半径";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("neutro_radius");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//横向位移
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "radial offset";
		lpInfo->m_strPropHelp = "radial offset is lateral displacement from generaltrix to section shape features origin (calibration)";
#else
		lpInfo->m_strPropName = "横向位移";
		lpInfo->m_strPropHelp = "基准母线距离截面形状特征(标定)原点的横向位移";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("offset_h");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//竖向位移
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "normal offset";
		lpInfo->m_strPropHelp = "normal offset is vertical displacement from generaltrix to section shape features origin (calibration)";
#else
		lpInfo->m_strPropName = "竖向位移";
		lpInfo->m_strPropHelp = "基准母线距离截面形状特征(标定)原点的竖向位移";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("offset_v");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//摆放方式
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "layout style";
		lpInfo->m_strPropHelp = "part space layout style";
#else
		lpInfo->m_strPropName = "摆放方式";
		lpInfo->m_strPropHelp = "构件空间摆放方式";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("m_siLayoutStyle");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		//装配方位
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "assemble position";
		lpInfo->m_strPropHelp = "part's assembling position and assembling direction.";
#else
		lpInfo->m_strPropName = "装配方位";
		lpInfo->m_strPropHelp = "构件的装配位置及装配时的装配方向。";
#endif
		CPropTreeItem *pLeafGroupItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pLeafGroupItem->m_bHideChildren = FALSE;
		pLeafGroupItem->m_idProp = CLDSArcAngle::GetPropID("assemblePos");
		if(CLDSArcAngle::GetPropStatus(pLeafGroupItem->m_idProp)!=-1)
			pLeafGroupItem->m_bHideChildren = CLDSArcAngle::GetPropStatus(pLeafGroupItem->m_idProp);
		pLeafGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//起始偏转角
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "start offset angle";
		lpInfo->m_strPropHelp = "start offset angle";
#else
		lpInfo->m_strPropName = "起始偏转角";
		lpInfo->m_strPropHelp = "起始偏转角";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("start_rot_angle");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//圆弧扇形角
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Arc sector Angle";
		lpInfo->m_strPropHelp = "Arc sector Angle";
#else
		lpInfo->m_strPropName = "圆弧扇形角";
		lpInfo->m_strPropHelp = "圆弧扇形角";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("sector_angle");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//工作面法线
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "work plane normal";
		lpInfo->m_strPropHelp = "part's work plane normal.";
#else
		lpInfo->m_strPropName = "工作面法线";
		lpInfo->m_strPropHelp = "构件工作面法线方向。";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("des_arc_norm");
		if(CLDSArcAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSArcAngle::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//圆弧起始点
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "starting point of arc";
		lpInfo->m_strPropHelp = "Ring part's arc starting point.";
#else
		lpInfo->m_strPropName = "圆弧起点";
		lpInfo->m_strPropHelp = "环形构件圆弧起点。";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("des_arc_start");
		if(CLDSArcAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSArcAngle::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//圆弧圆心位置
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "circle center position ";
		lpInfo->m_strPropHelp = "part basic line arc center position.";
#else
		lpInfo->m_strPropName = "圆心位置";
		lpInfo->m_strPropHelp = "构件基准线圆弧圆心位置。";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_idProp = CLDSArcAngle::GetPropID("des_arc_center");
		if(CLDSArcAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSArcAngle::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		//始端信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Information";
		lpInfo->m_strPropHelp = "arc angle start information";
#else 
		lpInfo->m_strPropName = "始端信息";
		lpInfo->m_strPropHelp = "环形角钢始端信息";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcAngle::GetPropID("startInfo");
		if(CLDSArcAngle::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcAngle::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		UpdateArcAngleStart_EndPositionProperty(pPropList,TRUE);
		//终端信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Information";
		lpInfo->m_strPropHelp = "arc angle end information";
#else 
		lpInfo->m_strPropName = "终端信息";
		lpInfo->m_strPropHelp = "环形角钢终端信息";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcAngle::GetPropID("endInfo");
		if(CLDSArcAngle::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcAngle::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		UpdateArcAngleStart_EndPositionProperty(pPropList,FALSE);
	}
	else
	{
		SetPartMutiObjsProp(pFirstArcAngle,"handle");
		SetPartMutiObjsProp(pFirstArcAngle,"layer");
		SetPartMutiObjsProp(pFirstArcAngle,"cMaterial");
		SetPartMutiObjsProp(pFirstArcAngle,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstArcAngle,"iSeg");
		SetPartMutiObjsProp(pFirstArcAngle,"sPartNo");
		SetPartMutiObjsProp(pFirstArcAngle,"cfgword");
		SetPartMutiObjsProp(pFirstArcAngle,"wing_thick");
		SetPartMutiObjsProp(pFirstArcAngle,"wing_wide");
		SetPartMutiObjsProp(pFirstArcAngle,"neutro_radius");
		SetPartMutiObjsProp(pFirstArcAngle,"arc_radius");
		SetPartMutiObjsProp(pFirstArcAngle,"offset_h");
		SetPartMutiObjsProp(pFirstArcAngle,"offset_v");
		SetPartMutiObjsProp(pFirstArcAngle,"start_rot_angle");
		SetPartMutiObjsProp(pFirstArcAngle,"sector_angle");
		SetPartMutiObjsProp(pFirstArcAngle,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstArcAngle,"m_hPartWeldParent");
		SetPartMutiObjsProp(pFirstArcAngle,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstArcAngle,"m_bUserSpecColor");
		if(pFirstArcAngle->m_bUserSpecColor)
		{	
			if(!SetPartMutiObjsProp(pFirstArcAngle,"crMaterial"))
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSArcAngle::GetPropID("m_bUserSpecColor"),NULL);
				if(pFindItem)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
					lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "part's color";
					lpInfo->m_strPropHelp = "part's color";
#else
					lpInfo->m_strPropName = "构件颜色";
					lpInfo->m_strPropHelp = "构件颜色";
#endif
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSArcAngle::GetPropID("crMaterial");
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
					if(pFirstArcAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				}
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSArcAngle::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstArcAngle,"sNotes");
		SetPartMutiObjsProp(pFirstArcAngle,"arc_start.x");
		SetPartMutiObjsProp(pFirstArcAngle,"arc_start.y");
		SetPartMutiObjsProp(pFirstArcAngle,"arc_start.z");
		SetPartMutiObjsProp(pFirstArcAngle,"center.x");
		SetPartMutiObjsProp(pFirstArcAngle,"center.y");
		SetPartMutiObjsProp(pFirstArcAngle,"center.z");
		SetPartMutiObjsProp(pFirstArcAngle,"workNorm.x");
		SetPartMutiObjsProp(pFirstArcAngle,"workNorm.y");
		SetPartMutiObjsProp(pFirstArcAngle,"workNorm.z");
		SetPartMutiObjsProp(pFirstArcAngle,"ucs.origin.x");
		SetPartMutiObjsProp(pFirstArcAngle,"ucs.origin.y");
		SetPartMutiObjsProp(pFirstArcAngle,"ucs.origin.z");
		SetPartMutiObjsProp(pFirstArcAngle,"ucs.axis_x.x");
		SetPartMutiObjsProp(pFirstArcAngle,"ucs.axis_x.y");
		SetPartMutiObjsProp(pFirstArcAngle,"ucs.axis_x.z");
		SetPartMutiObjsProp(pFirstArcAngle,"ucs.axis_y.x");
		SetPartMutiObjsProp(pFirstArcAngle,"ucs.axis_y.y");
		SetPartMutiObjsProp(pFirstArcAngle,"ucs.axis_y.z");
		SetPartMutiObjsProp(pFirstArcAngle,"ucs.axis_z.x");
		SetPartMutiObjsProp(pFirstArcAngle,"ucs.axis_z.y");
		SetPartMutiObjsProp(pFirstArcAngle,"ucs.axis_z.z");
		SetPartMutiObjsProp(pFirstArcAngle,"hStart");
		SetPartMutiObjsProp(pFirstArcAngle,"start_oddment");
		SetPartMutiObjsProp(pFirstArcAngle,"startPos.x");
		SetPartMutiObjsProp(pFirstArcAngle,"startPos.y");
		SetPartMutiObjsProp(pFirstArcAngle,"startPos.z");
		SetPartMutiObjsProp(pFirstArcAngle,"hEnd");
		SetPartMutiObjsProp(pFirstArcAngle,"end_oddment");
		SetPartMutiObjsProp(pFirstArcAngle,"endPos.x");
		SetPartMutiObjsProp(pFirstArcAngle,"endPos.y");
		SetPartMutiObjsProp(pFirstArcAngle,"endPos.z");
	}
	pPropList->Redraw();
}

void UpdateArcFlatStart_EndPositionProperty(CPropertyList *pPropList,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSArcFlat *pFirstArcFlat = (CLDSArcFlat*)pView->selectObjs.GetFirst();
	if(pFirstArcFlat==NULL)
		return;
	CPropTreeItem* pParentItem=NULL,*pPropItem=NULL,*pSonPropItem=NULL;
	if(bStart)
		pParentItem=pPropList->FindItemByPropId(CLDSArcFlat::GetPropID("startInfo"),NULL);
	else
		pParentItem=pPropList->FindItemByPropId(CLDSArcFlat::GetPropID("endInfo"),NULL);
	if(pParentItem==NULL)
		return;
	long propId=0;
	CString sPropName,sPropHelp;
	CItemInfo* lpInfo=NULL;
	char sText[MAX_TEMP_CHAR_100+1]="";
	//节点句柄
	if(bStart)
	{
		propId=CLDSArcFlat::GetPropID("hStart");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName = "start handle";
#else
		sPropName = "起点句柄";
#endif
	}
	else
	{
		propId=CLDSArcFlat::GetPropID("hEnd");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName = "start handle";
#else
		sPropName = "终点句柄";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_strPropName =sPropName;
	lpInfo->m_strPropHelp =sPropName;
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
	pPropItem->m_idProp = propId;
	if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//正负头
	if(bStart)
	{
		propId=CLDSArcFlat::GetPropID("start_oddment");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName = "start Oddment";
#else
		sPropName = "始端正负头";
#endif
	}
	else
	{
		propId=CLDSArcFlat::GetPropID("end_oddment");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName = "end Oddment";
#else
		sPropName = "终端正负头";
#endif		
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName =sPropName;
	lpInfo->m_strPropHelp =sPropName;
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
	pPropItem->m_idProp = propId;
	if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//始终端坐标
	if(bStart)
	{
		propId=CLDSArcFlat::GetPropID("startPos");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName="Start Design Coordinate";
#else
		sPropName="始端设计坐标";
#endif
	}
	else
	{
		propId=CLDSArcFlat::GetPropID("endPos");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName="End Design Coordinate";
#else
		sPropName="终端设计坐标";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_strPropName = sPropName;
	lpInfo->m_strPropHelp = sPropName;
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
	pPropItem->m_idProp = propId;
	if(CLDSArcFlat::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSArcFlat::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//X
	if(bStart)
	{
		propId=CLDSArcFlat::GetPropID("startPos.x");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="Start Design Coordinate X";
#else
		sPropHelp="环形角钢始端X轴方向的坐标";
#endif
	}
	else
	{
		propId=CLDSArcFlat::GetPropID("endPos.x");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="End Design Coordinate X";
#else
		sPropHelp="环形角钢终端X轴方向的坐标";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
#else 
	lpInfo->m_strPropName = "X坐标";
#endif
	lpInfo->m_strPropHelp=sPropHelp;
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
	pSonPropItem->m_idProp = propId;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	if(pFirstArcFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//Y
	if(bStart)
	{
		propId=CLDSArcFlat::GetPropID("startPos.y");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="Start Design Coordinate Y";
#else
		sPropHelp="环形角钢始端Y轴方向的坐标";
#endif
	}
	else
	{
		propId=CLDSArcFlat::GetPropID("endPos.y");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="End Design Coordinate Y";
#else
		sPropHelp="环形角钢终端Y轴方向的坐标";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
#else 
	lpInfo->m_strPropName = "Y坐标";
#endif
	lpInfo->m_strPropHelp = sPropHelp;
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
	pSonPropItem->m_idProp = propId;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	if(pFirstArcFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//Z
	if(bStart)
	{
		propId=CLDSArcFlat::GetPropID("startPos.z");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="Start Design Coordinate Z";
#else
		sPropHelp="环形角钢始端Z轴方向的坐标";
#endif
	}
	else
	{
		propId=CLDSArcFlat::GetPropID("endPos.z");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="End Design Coordinate Z";
#else
		sPropHelp="环形角钢终端Z轴方向的坐标";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
#else 
	lpInfo->m_strPropName = "Z坐标";
#endif
	lpInfo->m_strPropHelp = sPropHelp;
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
	pSonPropItem->m_idProp = propId;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	if(pFirstArcFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
//	//终端实际坐标
//	if(bStart)
//	{
//		propId=CLDSArcFlat::GetPropID("actualStartPos");
//#ifdef AFX_TARG_ENU_ENGLISH
//		sPropName="Start Actual Coordinate"
//		sPropHelp="The start coordinate of arc angle after considering oddment";
//#else
//		sPropName="始端实际坐标";
//		sPropHelp="环形角钢始端考虑正负头后的坐标位置";
//#endif
//	}
//	else
//	{
//		propId=CLDSArcFlat::GetPropID("actualEndPos");
//#ifdef AFX_TARG_ENU_ENGLISH
//		sPropName="End Actual Coordinate"
//		sPropHelp="End Design Coordinate Z";
//#else
//		sPropName="终端实际坐标";
//		sPropHelp="环形角钢终端考虑正负头后的坐标位置";
//#endif
//	}
//	lpInfo = new CItemInfo();
//	lpInfo->m_controlType = PIT_BUTTON;
//	lpInfo->m_strPropName = sPropName;
//	lpInfo->m_strPropHelp = sPropHelp;
//	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
//	pPropItem->m_idProp = propId;
//	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
}
void CLDSView::DisplayArcFlatProperty(BOOL bRebuild)
{
	CLDSArcFlat *pFirstArcFlat = (CLDSArcFlat*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";

	const int GROUP_GENERAL=1,GROUP_GEOMETRY=2,GROUP_START=3,GROUP_END=4;
	if(bRebuild)
	{	//需要重建对象属性页
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(4);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GEOMETRY-1,"Geometry");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"Start");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"End");
#else
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"常规");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GEOMETRY-1,"形位");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"始端");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"终端");
#endif
		pPropDlg->RefreshTabCtrl(CLDSArcFlat::m_iCurDisplayPropGroup);
		
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_ARCFLAT;
		//设置属性栏回调函数
		pPropList->SetModifyStatusFunc(ModifyArcFlatItemStatus);
		pPropList->SetModifyValueFunc(ModifyArcFlatProperty);
		pPropList->SetButtonClickFunc(ArcFlatButtonClick);
		//初始化属性项只读状态初始值,块内构件或映射构件属性不允许编辑 wht 12-10-16
		if(!pFirstArcFlat->BelongModel()->IsTowerModel()||pFirstArcFlat->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//基本信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "arc flat basic info";
		lpInfo->m_strPropHelp = "basic information arc flat steel";
#else
		lpInfo->m_strPropName = "环形扁钢基本信息";
		lpInfo->m_strPropHelp = "环形扁钢基本信息";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcFlat::GetPropID("basicInfo");
		if(CLDSArcFlat::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcFlat::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_GEOMETRY);
			//句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "Handle is the only mark of part in LDS system";
#else
		lpInfo->m_strPropName = "句柄";
		lpInfo->m_strPropHelp = "句柄是LDS系统中对构件的全局唯一标识";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("handle");
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_GEOMETRY);
			//图层
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "layer";
		lpInfo->m_strPropHelp = "part's layer";
#else
		lpInfo->m_strPropName = "图层";
		lpInfo->m_strPropHelp = "构件所在图层";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("layer");
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//材质	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "material";
		lpInfo->m_strPropHelp = "steel material";
#else
		lpInfo->m_strPropName = "材质";
		lpInfo->m_strPropHelp = "材质";
#endif
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390|Q420|Q460";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("cMaterial");
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//质量等级	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Quality Grade";
		lpInfo->m_strPropHelp = "steel quality grade";
#else
		lpInfo->m_strPropName = "质量等级";
		lpInfo->m_strPropHelp = "质量等级";
#endif
		lpInfo->m_cmbItems = "A|B|C|D|";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("m_cQualityLevel");
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//分段号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "segment No.";
		lpInfo->m_strPropHelp = "segment No.";
#else
		lpInfo->m_strPropName = "分段号";
		lpInfo->m_strPropHelp = "分段号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("iSeg");
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//构件编号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "part No.";
		lpInfo->m_strPropHelp = "part No.";
#else
		lpInfo->m_strPropName = "构件编号";
		lpInfo->m_strPropHelp = "构件编号"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("sPartNo");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//配材号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "CFG word";
		lpInfo->m_strPropHelp = "CFG word";
#else
		lpInfo->m_strPropName = "配材号";
		lpInfo->m_strPropHelp = "配材号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("cfgword");
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//统材系数
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Stat. Material Coef";
		lpInfo->m_strPropHelp = "Statistics material coefficient";
#else
		lpInfo->m_strPropName = "统材系数";
		lpInfo->m_strPropHelp = "统材系数";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("m_uStatMatCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//下料系数
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Len Zoom Coef";
		lpInfo->m_strPropHelp = "len zoom coefficient";
#else
		lpInfo->m_strPropName = "下料系数";
		lpInfo->m_strPropHelp = "加工时相对于圆弧基线的长度缩放系数";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("m_fZoomLenCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		  //焊接父构件
		lpInfo = new CItemInfo();
		lpInfo->m_controlType =	PIT_BUTTON;	//PIT_EDIT
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weld parent part";
		lpInfo->m_strPropHelp = "If the part needs to be welded onto other part, its property is the weld part's handle, if the part does not need welding, the value is0X0.";
#else
		lpInfo->m_strPropName = "焊接父构件";
		lpInfo->m_strPropHelp = "如果此构件需要焊接到其它构件上,那么此属性就是本构件所依附的焊接构件的句柄;如果此构件不需要焊接,此值为0X0。";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("m_hPartWeldParent");
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//显示孔
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "show hole";
		lpInfo->m_strPropHelp = "is show hole or not";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "显示孔";
		lpInfo->m_strPropHelp = "是否显示孔";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("m_bDisplayHole");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//虚拟辅助构件
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual auxiliary part";
		lpInfo->m_strPropHelp = "Virtual auxiliary parts are only used to assist other parts positioning, not real BOM part, so not included in material statistics.";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "虚拟辅助构件";
		lpInfo->m_strPropHelp = "虚拟辅助构件只用于辅助其它构件定位，并不是结构体真实具有的统材构件，所以不参预材料统计。";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("m_bVirtualPart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);	
			//用户指定构件颜色
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specify part color";
		lpInfo->m_strPropHelp = "is part color specified by the users";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "指定构件颜色";
		lpInfo->m_strPropHelp = "构件颜色是否为用户指定";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstArcFlat->m_bUserSpecColor)
		{	//构件颜色
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "part color";
			lpInfo->m_strPropHelp = "part color";
#else
			lpInfo->m_strPropName = "构件颜色";
			lpInfo->m_strPropHelp = "构件颜色";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSArcFlat::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstArcFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
			//显示片数
		/*lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "显示片数";
		lpInfo->m_strPropHelp = "圆柱形回转体的显示片数";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("m_uDisplaySlices");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);*/
			//备注
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "notes";
		lpInfo->m_strPropHelp = "notes"; 
#else
		lpInfo->m_strPropName = "备注";
		lpInfo->m_strPropHelp = "备注"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("sNotes");
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		
		//形位信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "geometry Info";
		lpInfo->m_strPropHelp = "geometry information";
#else
		lpInfo->m_strPropName = "形位信息";
		lpInfo->m_strPropHelp = "形位信息";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcAngle::GetPropID("geometryInfo");
		if(CLDSArcAngle::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcAngle::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//规格
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "spec";
		lpInfo->m_strPropHelp = "specifications";
#else
		lpInfo->m_strPropName = "规格型号";
		lpInfo->m_strPropHelp = "规格型号";
#endif
		lpInfo->m_cmbItems = MakeFlatSetString();
		lpInfo->m_cmbType = CDT_EDIT;
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("specification");
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//宽度
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "width";
		lpInfo->m_strPropHelp = "width";
#else
		lpInfo->m_strPropName = "宽度";
		lpInfo->m_strPropHelp = "宽度";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("wide");
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		pPropItem->SetReadOnly();
			//厚度
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Thickness ";
		lpInfo->m_strPropHelp = "Thickness ";
#else
		lpInfo->m_strPropName = "厚度";
		lpInfo->m_strPropHelp = "厚度";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("thick");
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		pPropItem->SetReadOnly();
			//圆弧半径
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "arc Radius ";
		lpInfo->m_strPropHelp = "arc Radius ";
#else
		lpInfo->m_strPropName = "圆弧半径";
		lpInfo->m_strPropHelp = "圆弧半径";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("arc_radius");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		pPropItem->SetReadOnly();
			//中性层圆弧半径
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "neutrosphere arc radius";
		lpInfo->m_strPropHelp = "curved neutrosphere arc radius";
#else
		lpInfo->m_strPropName = "中性层圆弧半径";
		lpInfo->m_strPropHelp = "弯曲中性层的圆弧半径";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("neutro_radius");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//横向位移
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "radial offset";
		lpInfo->m_strPropHelp = "radial offset is lateral displacement from generaltrix to section shape features origin (calibration)";
#else
		lpInfo->m_strPropName = "横向位移";
		lpInfo->m_strPropHelp = "基准母线距离截面形状特征(标定)原点的横向位移";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("offset_h");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//竖向位移
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "normal offset";
		lpInfo->m_strPropHelp = "normal offset is vertical displacement from generaltrix to section shape features origin (calibration)";
#else
		lpInfo->m_strPropName = "竖向位移";
		lpInfo->m_strPropHelp = "基准母线距离截面形状特征(标定)原点的竖向位移";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("offset_v");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		//装配方位
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Assemble position";
		lpInfo->m_strPropHelp = "part's assembling position and assembling direction.";
#else
		lpInfo->m_strPropName = "装配方位";
		lpInfo->m_strPropHelp = "构件的装配位置及装配时的装配方向。";
#endif
		CPropTreeItem *pLeafGroupItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pLeafGroupItem->m_bHideChildren = FALSE;
		pLeafGroupItem->m_idProp = CLDSArcFlat::GetPropID("assemblePos");
		if(CLDSArcFlat::GetPropStatus(pLeafGroupItem->m_idProp)!=-1)
			pLeafGroupItem->m_bHideChildren = CLDSArcFlat::GetPropStatus(pLeafGroupItem->m_idProp);
		pLeafGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//起始偏转角
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "start offset angle";
		lpInfo->m_strPropHelp = "start offset angle";
#else
		lpInfo->m_strPropName = "起始偏转角";
		lpInfo->m_strPropHelp = "起始偏转角";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("start_rot_angle");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//圆弧扇形角
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Arc sector Angle";
		lpInfo->m_strPropHelp = "Arc sector Angle";
#else
		lpInfo->m_strPropName = "圆弧扇形角";
		lpInfo->m_strPropHelp = "圆弧扇形角";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("sector_angle");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//工作面法线
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "work plane normal";
		lpInfo->m_strPropHelp = "part work plane normal direction.";
#else
		lpInfo->m_strPropName = "工作面法线";
		lpInfo->m_strPropHelp = "构件工作面法线方向。";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("des_arc_norm");
		if(CLDSArcFlat::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSArcFlat::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//圆弧起始点
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Arc starting point";
		lpInfo->m_strPropHelp = "Ring part's arc starting point.";
#else
		lpInfo->m_strPropName = "圆弧起点";
		lpInfo->m_strPropHelp = "环形构件圆弧起点。";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("des_arc_start");
		if(CLDSArcFlat::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSArcFlat::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//圆弧圆心位置
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "circle center position ";
		lpInfo->m_strPropHelp = "part basic line arc center position.";
#else
		lpInfo->m_strPropName = "圆心位置";
		lpInfo->m_strPropHelp = "构件基准线圆弧圆心位置。";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_idProp = CLDSArcFlat::GetPropID("des_arc_center");
		if(CLDSArcFlat::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSArcFlat::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		//始端信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Information";
		lpInfo->m_strPropHelp = "arc angle start information";
#else 
		lpInfo->m_strPropName = "始端信息";
		lpInfo->m_strPropHelp = "环形扁铁始端信息";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcFlat::GetPropID("startInfo");
		if(CLDSArcFlat::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcFlat::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		UpdateArcFlatStart_EndPositionProperty(pPropList,TRUE);
		//终端信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Information";
		lpInfo->m_strPropHelp = "arc angle end information";
#else 
		lpInfo->m_strPropName = "终端信息";
		lpInfo->m_strPropHelp = "环形扁铁终端信息";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcFlat::GetPropID("endInfo");
		if(CLDSArcFlat::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcFlat::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		UpdateArcFlatStart_EndPositionProperty(pPropList,FALSE);
	}
	else
	{
		SetPartMutiObjsProp(pFirstArcFlat,"handle");
		SetPartMutiObjsProp(pFirstArcFlat,"layer");
		SetPartMutiObjsProp(pFirstArcFlat,"cMaterial");
		SetPartMutiObjsProp(pFirstArcFlat,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstArcFlat,"iSeg");
		SetPartMutiObjsProp(pFirstArcFlat,"sPartNo");
		SetPartMutiObjsProp(pFirstArcFlat,"cfgword");
		SetPartMutiObjsProp(pFirstArcFlat,"thick");
		SetPartMutiObjsProp(pFirstArcFlat,"wide");
		SetPartMutiObjsProp(pFirstArcFlat,"neutro_radius");
		SetPartMutiObjsProp(pFirstArcFlat,"arc_radius");
		SetPartMutiObjsProp(pFirstArcFlat,"offset_h");
		SetPartMutiObjsProp(pFirstArcFlat,"offset_v");
		SetPartMutiObjsProp(pFirstArcFlat,"start_rot_angle");
		SetPartMutiObjsProp(pFirstArcFlat,"sector_angle");
		SetPartMutiObjsProp(pFirstArcFlat,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstArcFlat,"m_hPartWeldParent");
		SetPartMutiObjsProp(pFirstArcFlat,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstArcFlat,"m_bUserSpecColor");
		if(pFirstArcFlat->m_bUserSpecColor)
		{	
			if(!SetPartMutiObjsProp(pFirstArcFlat,"crMaterial"))
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSArcFlat::GetPropID("m_bUserSpecColor"),NULL);
				if(pFindItem)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
					lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "part color";
					lpInfo->m_strPropHelp = "part color";
#else
					lpInfo->m_strPropName = "构件颜色";
					lpInfo->m_strPropHelp = "构件颜色";
#endif
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSArcFlat::GetPropID("crMaterial");
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
					if(pFirstArcFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				}
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSArcFlat::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstArcFlat,"sNotes");
		SetPartMutiObjsProp(pFirstArcFlat,"arc_start.x");
		SetPartMutiObjsProp(pFirstArcFlat,"arc_start.y");
		SetPartMutiObjsProp(pFirstArcFlat,"arc_start.z");
		SetPartMutiObjsProp(pFirstArcFlat,"center.x");
		SetPartMutiObjsProp(pFirstArcFlat,"center.y");
		SetPartMutiObjsProp(pFirstArcFlat,"center.z");
		SetPartMutiObjsProp(pFirstArcFlat,"workNorm.x");
		SetPartMutiObjsProp(pFirstArcFlat,"workNorm.y");
		SetPartMutiObjsProp(pFirstArcFlat,"workNorm.z");
		SetPartMutiObjsProp(pFirstArcFlat,"ucs.origin.x");
		SetPartMutiObjsProp(pFirstArcFlat,"ucs.origin.y");
		SetPartMutiObjsProp(pFirstArcFlat,"ucs.origin.z");
		SetPartMutiObjsProp(pFirstArcFlat,"ucs.axis_x.x");
		SetPartMutiObjsProp(pFirstArcFlat,"ucs.axis_x.y");
		SetPartMutiObjsProp(pFirstArcFlat,"ucs.axis_x.z");
		SetPartMutiObjsProp(pFirstArcFlat,"ucs.axis_y.x");
		SetPartMutiObjsProp(pFirstArcFlat,"ucs.axis_y.y");
		SetPartMutiObjsProp(pFirstArcFlat,"ucs.axis_y.z");
		SetPartMutiObjsProp(pFirstArcFlat,"ucs.axis_z.x");
		SetPartMutiObjsProp(pFirstArcFlat,"ucs.axis_z.y");
		SetPartMutiObjsProp(pFirstArcFlat,"ucs.axis_z.z");
		SetPartMutiObjsProp(pFirstArcFlat,"hStart");
		SetPartMutiObjsProp(pFirstArcFlat,"start_oddment");
		SetPartMutiObjsProp(pFirstArcFlat,"startPos.x");
		SetPartMutiObjsProp(pFirstArcFlat,"startPos.y");
		SetPartMutiObjsProp(pFirstArcFlat,"startPos.z");
		SetPartMutiObjsProp(pFirstArcFlat,"hEnd");
		SetPartMutiObjsProp(pFirstArcFlat,"end_oddment");
		SetPartMutiObjsProp(pFirstArcFlat,"endPos.x");
		SetPartMutiObjsProp(pFirstArcFlat,"endPos.y");
		SetPartMutiObjsProp(pFirstArcFlat,"endPos.z");
	}
	
	pPropList->Redraw();
}

void UpdateArcSlotStart_EndPositionProperty(CPropertyList *pPropList,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSArcSlot *pFirstArcSlot = (CLDSArcSlot*)pView->selectObjs.GetFirst();
	if(pFirstArcSlot==NULL)
		return;
	CPropTreeItem* pParentItem=NULL,*pPropItem=NULL,*pSonPropItem=NULL;
	if(bStart)
		pParentItem=pPropList->FindItemByPropId(CLDSArcSlot::GetPropID("startInfo"),NULL);
	else
		pParentItem=pPropList->FindItemByPropId(CLDSArcSlot::GetPropID("endInfo"),NULL);
	if(pParentItem==NULL)
		return;
	long propId=0;
	CString sPropName,sPropHelp;
	CItemInfo* lpInfo=NULL;
	char sText[MAX_TEMP_CHAR_100+1]="";
	//节点句柄
	if(bStart)
	{
		propId=CLDSArcSlot::GetPropID("hStart");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName = "start handle";
#else
		sPropName = "起点句柄";
#endif
	}
	else
	{
		propId=CLDSArcSlot::GetPropID("hEnd");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName = "start handle";
#else
		sPropName = "终点句柄";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_strPropName =sPropName;
	lpInfo->m_strPropHelp =sPropName;
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
	pPropItem->m_idProp = propId;
	if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//正负头
	if(bStart)
	{
		propId=CLDSArcSlot::GetPropID("start_oddment");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName = "start Oddment";
#else
		sPropName = "始端正负头";
#endif
	}
	else
	{
		propId=CLDSArcSlot::GetPropID("end_oddment");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName= "end Oddment";
#else
		sPropName= "终端正负头";
#endif		
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName =sPropName;
	lpInfo->m_strPropHelp =sPropName;
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
	pPropItem->m_idProp = propId;
	if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//始终端坐标
	if(bStart)
	{
		propId=CLDSArcSlot::GetPropID("startPos");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName="Start Design Coordinate";
#else
		sPropName="始端设计坐标";
#endif
	}
	else
	{
		propId=CLDSArcSlot::GetPropID("endPos");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropName="End Design Coordinate";
#else
		sPropName="终端设计坐标";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_strPropName = sPropName;
	lpInfo->m_strPropHelp = sPropName;
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
	pPropItem->m_idProp = propId;
	if(CLDSArcSlot::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSArcSlot::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//X
	if(bStart)
	{
		propId=CLDSArcSlot::GetPropID("startPos.x");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="Start Design Coordinate X";
#else
		sPropHelp="环形角钢始端X轴方向的坐标";
#endif
	}
	else
	{
		propId=CLDSArcSlot::GetPropID("endPos.x");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="End Design Coordinate X";
#else
		sPropHelp="环形角钢终端X轴方向的坐标";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
#else 
	lpInfo->m_strPropName = "X坐标";
#endif
	lpInfo->m_strPropHelp=sPropHelp;
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
	pSonPropItem->m_idProp = propId;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	if(pFirstArcSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//Y
	if(bStart)
	{
		propId=CLDSArcSlot::GetPropID("startPos.y");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="Start Design Coordinate Y";
#else
		sPropHelp="环形角钢始端Y轴方向的坐标";
#endif
	}
	else
	{
		propId=CLDSArcSlot::GetPropID("endPos.y");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="End Design Coordinate Y";
#else
		sPropHelp="环形角钢终端Y轴方向的坐标";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
#else 
	lpInfo->m_strPropName = "Y坐标";
#endif
	lpInfo->m_strPropHelp = sPropHelp;
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
	pSonPropItem->m_idProp = propId;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	if(pFirstArcSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//Z
	if(bStart)
	{
		propId=CLDSArcSlot::GetPropID("startPos.z");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="Start Design Coordinate Z";
#else
		sPropHelp="环形角钢始端Z轴方向的坐标";
#endif
	}
	else
	{
		propId=CLDSArcSlot::GetPropID("endPos.z");
#ifdef AFX_TARG_ENU_ENGLISH
		sPropHelp="End Design Coordinate Z";
#else
		sPropHelp="环形角钢终端Z轴方向的坐标";
#endif
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
#else 
	lpInfo->m_strPropName = "Z坐标";
#endif
	lpInfo->m_strPropHelp = sPropHelp;
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
	pSonPropItem->m_idProp = propId;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	if(pFirstArcSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
//	//终端实际坐标
//	if(bStart)
//	{
//		propId=CLDSArcSlot::GetPropID("actualStartPos");
//#ifdef AFX_TARG_ENU_ENGLISH
//		sPropName="Start Actual Coordinate"
//			sPropHelp="The start coordinate of arc angle after considering oddment";
//#else
//		sPropName="始端实际坐标";
//		sPropHelp="环形角钢始端考虑正负头后的坐标位置";
//#endif
//	}
//	else
//	{
//		propId=CLDSArcSlot::GetPropID("actualEndPos");
//#ifdef AFX_TARG_ENU_ENGLISH
//		sPropName="End Actual Coordinate"
//			sPropHelp="End Design Coordinate Z";
//#else
//		sPropName="终端实际坐标";
//		sPropHelp="环形角钢终端考虑正负头后的坐标位置";
//#endif
//	}
//	lpInfo = new CItemInfo();
//	lpInfo->m_controlType = PIT_BUTTON;
//	lpInfo->m_strPropName = sPropName;
//	lpInfo->m_strPropHelp = sPropHelp;
//	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
//	pPropItem->m_idProp = propId;
//	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
}
void CLDSView::DisplayArcSlotProperty(BOOL bRebuild)
{
	CLDSArcSlot *pFirstArcSlot = (CLDSArcSlot*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";

	const int GROUP_GENERAL=1,GROUP_GEOMETRY=2,GROUP_START=3,GROUP_END=4;
	if(bRebuild)
	{	//需要重建对象属性页
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(4);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GEOMETRY-1,"Geometry");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"Start");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"End");
#else
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"常规");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GEOMETRY-1,"形位");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"始端");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"终端");
#endif
		pPropDlg->RefreshTabCtrl(CLDSArcSlot::m_iCurDisplayPropGroup);
		
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_ARCSLOT;
		//设置属性栏回调函数
		pPropList->SetModifyStatusFunc(ModifyArcSlotItemStatus);
		pPropList->SetModifyValueFunc(ModifyArcSlotProperty);
		pPropList->SetButtonClickFunc(ArcSlotButtonClick);
		//初始化属性项只读状态初始值,块内构件或映射构件属性不允许编辑 wht 12-10-16
		if(!pFirstArcSlot->BelongModel()->IsTowerModel()||pFirstArcSlot->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//基本信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "arc Slot basic info";
		lpInfo->m_strPropHelp = "arc Slot basic information";
#else
		lpInfo->m_strPropName = "环形槽钢基本信息";
		lpInfo->m_strPropHelp = "环形槽钢基本信息";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcSlot::GetPropID("basicInfo");
		if(CLDSArcSlot::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcSlot::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_GEOMETRY);
			//句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle ";
		lpInfo->m_strPropHelp = "Handle is the only mark of part in LDS system";
#else
		lpInfo->m_strPropName = "句柄";
		lpInfo->m_strPropHelp = "句柄是LDS系统中对构件的全局唯一标识";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("handle");
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_GEOMETRY);
		pPropItem->SetReadOnly();
			//图层
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "layer";
		lpInfo->m_strPropHelp = "part's layer";
#else
		lpInfo->m_strPropName = "图层";
		lpInfo->m_strPropHelp = "构件所在图层";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("layer");
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//材质	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Material";
		lpInfo->m_strPropHelp = "steel Material";
#else
		lpInfo->m_strPropName = "材质";
		lpInfo->m_strPropHelp = "材质";
#endif
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390|Q420|Q460";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("cMaterial");
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//质量等级	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Quality Grade";
		lpInfo->m_strPropHelp = "steel quality grade";
#else
		lpInfo->m_strPropName = "质量等级";
		lpInfo->m_strPropHelp = "质量等级";
#endif
		lpInfo->m_cmbItems = "A|B|C|D|";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("m_cQualityLevel");
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//分段号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "segment No.";
		lpInfo->m_strPropHelp = "segment No.";
#else
		lpInfo->m_strPropName = "分段号";
		lpInfo->m_strPropHelp = "分段号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("iSeg");
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//构件编号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part No.";
		lpInfo->m_strPropHelp = "Part No.";
#else
		lpInfo->m_strPropName = "构件编号";
		lpInfo->m_strPropHelp = "构件编号"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("sPartNo");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//配材号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "CFG word";
		lpInfo->m_strPropHelp = "CFG word";
#else
		lpInfo->m_strPropName = "配材号";
		lpInfo->m_strPropHelp = "配材号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("cfgword");
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//统材系数
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Stat. Material Coef";
		lpInfo->m_strPropHelp = "Statistics material coefficient";
#else
		lpInfo->m_strPropName = "统材系数";
		lpInfo->m_strPropHelp = "统材系数";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("m_uStatMatCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//下料系数
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Len Zoom Coef";
		lpInfo->m_strPropHelp = "len zoom coefficient";
#else
		lpInfo->m_strPropName = "下料系数";
		lpInfo->m_strPropHelp = "加工时相对于圆弧基线的长度缩放系数";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("m_fZoomLenCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		  //焊接父构件
		lpInfo = new CItemInfo();
		lpInfo->m_controlType =	PIT_BUTTON;	//PIT_EDIT
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weld parent part";
		lpInfo->m_strPropHelp = "If the part needs to be welded onto other part, its property is the weld part's handle, if the part does not need welding, the value is0X0.";
#else
		lpInfo->m_strPropName = "焊接父构件";
		lpInfo->m_strPropHelp = "如果此构件需要焊接到其它构件上,那么此属性就是本构件所依附的焊接构件的句柄;如果此构件不需要焊接,此值为0X0。";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("m_hPartWeldParent");
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//显示孔
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "show hole";
		lpInfo->m_strPropHelp = "is show hole or not";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "显示孔";
		lpInfo->m_strPropHelp = "是否显示孔";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("m_bDisplayHole");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//虚拟辅助构件
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual auxiliary part";
		lpInfo->m_strPropHelp = "Virtual auxiliary parts are only used to assist other parts positioning, not real BOM part, so not included in material statistics.";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "虚拟辅助构件";
		lpInfo->m_strPropHelp = "虚拟辅助构件只用于辅助其它构件定位，并不是结构体真实具有的统材构件，所以不参预材料统计。";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("m_bVirtualPart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//用户指定构件颜色
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specify part color";
		lpInfo->m_strPropHelp = "is part color specified by the users?";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "指定构件颜色";
		lpInfo->m_strPropHelp = "构件颜色是否为用户指定";
		lpInfo->m_cmbItems = "是|否";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstArcSlot->m_bUserSpecColor)
		{	//构件颜色
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "part color";
			lpInfo->m_strPropHelp = "part color";
#else
			lpInfo->m_strPropName = "构件颜色";
			lpInfo->m_strPropHelp = "构件颜色";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSArcSlot::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstArcSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
			//显示片数
		/*lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "显示片数";
		lpInfo->m_strPropHelp = "圆柱形回转体的显示片数";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("m_uDisplaySlices");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);*/
			//备注
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "notes";
		lpInfo->m_strPropHelp = "notes";
#else
		lpInfo->m_strPropName = "备注";
		lpInfo->m_strPropHelp = "备注"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("sNotes");
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		
		//形位信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "geometry Info";
		lpInfo->m_strPropHelp = "geometry Information";
#else
		lpInfo->m_strPropName = "形位信息";
		lpInfo->m_strPropHelp = "形位信息";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcSlot::GetPropID("geometryInfo");
		if(CLDSArcSlot::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcSlot::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//规格
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "spec";
		lpInfo->m_strPropHelp = "specifications";
#else
		lpInfo->m_strPropName = "规格型号";
		lpInfo->m_strPropHelp = "规格型号";
#endif
		lpInfo->m_cmbItems = MakeSlotSetString();
		lpInfo->m_cmbType = CDT_LIST;	//CDT_EDIT;
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("specification");
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//截面高
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "section height";
		lpInfo->m_strPropHelp = "section height";
#else
		lpInfo->m_strPropName = "截面高";
		lpInfo->m_strPropHelp = "截面高";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("height");
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		pPropItem->SetReadOnly();
			//肢宽
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "width";
		lpInfo->m_strPropHelp = "Leg width";
#else
		lpInfo->m_strPropName = "肢宽";
		lpInfo->m_strPropHelp = "肢宽";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("wide");
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		pPropItem->SetReadOnly();
			//肢厚
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "thickness";
		lpInfo->m_strPropHelp = "leg thickness";
#else
		lpInfo->m_strPropName = "肢厚";
		lpInfo->m_strPropHelp = "肢厚";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("thick");
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		pPropItem->SetReadOnly();
			//圆弧半径
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "arc radius";
		lpInfo->m_strPropHelp = "arc radius";
#else
		lpInfo->m_strPropName = "圆弧半径";
		lpInfo->m_strPropHelp = "圆弧半径";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("arc_radius");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		pPropItem->SetReadOnly();
			//中性层圆弧半径
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "neutrosphere arc radius";
		lpInfo->m_strPropHelp = "curved neutrosphere arc radius";
#else
		lpInfo->m_strPropName = "中性层圆弧半径";
		lpInfo->m_strPropHelp = "弯曲中性层的圆弧半径";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("neutro_radius");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//横向位移
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "radial offset";
		lpInfo->m_strPropHelp = "radial offset is lateral displacement from generaltrix to section shape features origin (calibration)";
#else
		lpInfo->m_strPropName = "横向位移";
		lpInfo->m_strPropHelp = "基准母线距离截面形状特征(标定)原点的横向位移";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("offset_h");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//竖向位移
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "normal offset";
		lpInfo->m_strPropHelp = "normal offset is vertical displacement from generaltrix to section shape features origin (calibration)";
#else
		lpInfo->m_strPropName = "竖向位移";
		lpInfo->m_strPropHelp = "基准母线距离截面形状特征(标定)原点的竖向位移";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("offset_v");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);//竖向位移
			//摆放方式
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "layout style";
		lpInfo->m_strPropHelp = "part space layout style";
#else
		lpInfo->m_strPropName = "摆放方式";
		lpInfo->m_strPropHelp = "构件空间摆放方式";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("m_siLayoutStyle");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		//装配方位
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "assemble position";
		lpInfo->m_strPropHelp = "part's assembling position and assembling direction.";
#else
		lpInfo->m_strPropName = "装配方位";
		lpInfo->m_strPropHelp = "构件的装配位置及装配时的装配方向。";
#endif
		CPropTreeItem *pLeafGroupItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pLeafGroupItem->m_bHideChildren = FALSE;
		pLeafGroupItem->m_idProp = CLDSArcSlot::GetPropID("assemblePos");
		if(CLDSArcSlot::GetPropStatus(pLeafGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcSlot::GetPropStatus(pLeafGroupItem->m_idProp);
		pLeafGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//起始偏转角
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "start offset angle";
		lpInfo->m_strPropHelp = "start offset angle";
#else
		lpInfo->m_strPropName = "起始偏转角";
		lpInfo->m_strPropHelp = "起始偏转角";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("start_rot_angle");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//圆弧扇形角
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Arc sector Angle";
		lpInfo->m_strPropHelp = "Arc sector Angle";
#else
		lpInfo->m_strPropName = "圆弧扇形角";
		lpInfo->m_strPropHelp = "圆弧扇形角";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("sector_angle");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//工作面法线
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "work plane normal";
		lpInfo->m_strPropHelp = "part work plane normal direction.";
#else
		lpInfo->m_strPropName = "工作面法线";
		lpInfo->m_strPropHelp = "构件工作面法线方向。";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("des_arc_norm");
		if(CLDSArcSlot::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSArcSlot::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//圆弧起始点
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Arc starting point";
		lpInfo->m_strPropHelp = "Ring part arc starting point.";
#else
		lpInfo->m_strPropName = "圆弧起点";
		lpInfo->m_strPropHelp = "环形构件圆弧起点。";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("des_arc_start");
		if(CLDSArcSlot::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSArcSlot::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
			//圆弧圆心位置
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "circle center position ";
		lpInfo->m_strPropHelp = "part basic line arc center position.";
#else
		lpInfo->m_strPropName = "圆心位置";
		lpInfo->m_strPropHelp = "构件基准线圆弧圆心位置。";
#endif
		pPropItem = pPropList->InsertItem(pLeafGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_idProp = CLDSArcSlot::GetPropID("des_arc_center");
		if(CLDSArcSlot::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSArcSlot::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GEOMETRY);
		//始端信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Information";
		lpInfo->m_strPropHelp = "arc angle start information";
#else 
		lpInfo->m_strPropName = "始端信息";
		lpInfo->m_strPropHelp = "环形槽钢始端信息";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcSlot::GetPropID("startInfo");
		if(CLDSArcSlot::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcSlot::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		UpdateArcSlotStart_EndPositionProperty(pPropList,TRUE);
		//终端信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Information";
		lpInfo->m_strPropHelp = "arc angle end information";
#else 
		lpInfo->m_strPropName = "终端信息";
		lpInfo->m_strPropHelp = "环形槽钢终端信息";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSArcSlot::GetPropID("endInfo");
		if(CLDSArcSlot::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSArcSlot::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		UpdateArcSlotStart_EndPositionProperty(pPropList,FALSE);
		
	}
	else
	{
		SetPartMutiObjsProp(pFirstArcSlot,"handle");
		SetPartMutiObjsProp(pFirstArcSlot,"layer");
		SetPartMutiObjsProp(pFirstArcSlot,"cMaterial");
		SetPartMutiObjsProp(pFirstArcSlot,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstArcSlot,"specification");
		SetPartMutiObjsProp(pFirstArcSlot,"iSeg");
		SetPartMutiObjsProp(pFirstArcSlot,"sPartNo");
		SetPartMutiObjsProp(pFirstArcSlot,"cfgword");
		SetPartMutiObjsProp(pFirstArcSlot,"thick");
		SetPartMutiObjsProp(pFirstArcSlot,"wide");
		SetPartMutiObjsProp(pFirstArcSlot,"height");
		SetPartMutiObjsProp(pFirstArcSlot,"neutro_radius");
		SetPartMutiObjsProp(pFirstArcSlot,"arc_radius");
		SetPartMutiObjsProp(pFirstArcSlot,"offset_h");
		SetPartMutiObjsProp(pFirstArcSlot,"offset_v");
		SetPartMutiObjsProp(pFirstArcSlot,"start_rot_angle");
		SetPartMutiObjsProp(pFirstArcSlot,"sector_angle");
		SetPartMutiObjsProp(pFirstArcSlot,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstArcSlot,"m_hPartWeldParent");
		SetPartMutiObjsProp(pFirstArcSlot,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstArcSlot,"m_bUserSpecColor");
		if(pFirstArcSlot->m_bUserSpecColor)
		{	
			if(!SetPartMutiObjsProp(pFirstArcSlot,"crMaterial"))
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSArcSlot::GetPropID("m_bUserSpecColor"),NULL);
				if(pFindItem)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
					lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "part color";
					lpInfo->m_strPropHelp = "part color";
#else
					lpInfo->m_strPropName = "构件颜色";
					lpInfo->m_strPropHelp = "构件颜色";
#endif
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSArcSlot::GetPropID("crMaterial");
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
					if(pFirstArcSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				}
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSArcSlot::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstArcSlot,"sNotes");
		SetPartMutiObjsProp(pFirstArcSlot,"arc_start.x");
		SetPartMutiObjsProp(pFirstArcSlot,"arc_start.y");
		SetPartMutiObjsProp(pFirstArcSlot,"arc_start.z");
		SetPartMutiObjsProp(pFirstArcSlot,"center.x");
		SetPartMutiObjsProp(pFirstArcSlot,"center.y");
		SetPartMutiObjsProp(pFirstArcSlot,"center.z");
		SetPartMutiObjsProp(pFirstArcSlot,"workNorm.x");
		SetPartMutiObjsProp(pFirstArcSlot,"workNorm.y");
		SetPartMutiObjsProp(pFirstArcSlot,"workNorm.z");
		SetPartMutiObjsProp(pFirstArcSlot,"ucs.origin.x");
		SetPartMutiObjsProp(pFirstArcSlot,"ucs.origin.y");
		SetPartMutiObjsProp(pFirstArcSlot,"ucs.origin.z");
		SetPartMutiObjsProp(pFirstArcSlot,"ucs.axis_x.x");
		SetPartMutiObjsProp(pFirstArcSlot,"ucs.axis_x.y");
		SetPartMutiObjsProp(pFirstArcSlot,"ucs.axis_x.z");
		SetPartMutiObjsProp(pFirstArcSlot,"ucs.axis_y.x");
		SetPartMutiObjsProp(pFirstArcSlot,"ucs.axis_y.y");
		SetPartMutiObjsProp(pFirstArcSlot,"ucs.axis_y.z");
		SetPartMutiObjsProp(pFirstArcSlot,"ucs.axis_z.x");
		SetPartMutiObjsProp(pFirstArcSlot,"ucs.axis_z.y");
		SetPartMutiObjsProp(pFirstArcSlot,"ucs.axis_z.z");
		SetPartMutiObjsProp(pFirstArcSlot,"hStart");
		SetPartMutiObjsProp(pFirstArcSlot,"start_oddment");
		SetPartMutiObjsProp(pFirstArcSlot,"startPos.x");
		SetPartMutiObjsProp(pFirstArcSlot,"startPos.y");
		SetPartMutiObjsProp(pFirstArcSlot,"startPos.z");
		SetPartMutiObjsProp(pFirstArcSlot,"hEnd");
		SetPartMutiObjsProp(pFirstArcSlot,"end_oddment");
		SetPartMutiObjsProp(pFirstArcSlot,"endPos.x");
		SetPartMutiObjsProp(pFirstArcSlot,"endPos.y");
		SetPartMutiObjsProp(pFirstArcSlot,"endPos.z");
	}
	pPropList->Redraw();
}
#endif
