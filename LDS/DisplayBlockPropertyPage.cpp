//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "DesignVectorPropDlg.h"
#include "DatumPointPropertyDlg.h"
#include "BlockPartListDLg.h"
#include "LmaDlg.h"
#include "PartUcsDlg.h"

//from DisplayPropertyPage.cpp
BOOL SetPartMutiObjsProp(CLDSObject *pObj, char *propName,long idClassType=0);
void SetPropItemReadOnly(CLDSDbObject *pObj,char *propName,BOOL bReadOnly);
#ifdef __COMMON_PART_INC_
BOOL ModifyBlockReferenceProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CBlockReference* pFirstBlockRef = (CBlockReference*)pView->selectObjs.GetFirst();
	CPropTreeItem *pFindItem=NULL, *pParentItem=NULL, *pPropItem=NULL;
	CBlockReference *pBlockRef = NULL;
	CItemInfo *lpInfo=NULL;
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	int iTemp=0;
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	try
	{
		BOOL bRedrawSolid=FALSE;
		if(pItem->m_idProp==CBlockReference::GetPropID("descript"))
		{
			strcpy(pFirstBlockRef->descript,valueStr);
			CTowerTreeDlg *pTreeView = (CTowerTreeDlg*)((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
			char sText[100]="";
			sprintf(sText,"%s(0X%X)",valueStr,pFirstBlockRef->handle);
			pTreeView->RenameSelectedItem(sText);
		}
		else if(pItem->m_idProp==CBlockReference::GetPropID("iSeg"))
		{
			pFirstBlockRef->iSeg=SEGI(valueStr.GetBuffer());
			bRedrawSolid=TRUE;
		}
		else if(pItem->m_idProp==CBlockReference::GetPropID("cfgword"))
		{	//更新其他杆件的配材号
			for(CBlockReference *pBlockRef=(CBlockReference*)pView->selectObjs.GetFirst();pBlockRef;pBlockRef=(CBlockReference*)pView->selectObjs.GetNext())
				pBlockRef->cfgword = pFirstBlockRef->cfgword;
		}
		else if(pItem->m_idProp==CBlockReference::GetPropID("layer"))
		{
			char tem_str[MAX_TEMP_CHAR_100+1];
			_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
			for(CBlockReference *pBlockRef=(CBlockReference*)pView->selectObjs.GetFirst();pBlockRef;pBlockRef=(CBlockReference*)pView->selectObjs.GetNext())
				pBlockRef->SetLayer(tem_str);
		}
		else if(pItem->m_idProp==CBlockReference::GetPropID("des_acs.m_eDatumPlane"))
		{
			CParaCS::PARAMETRIC_INFO param=pFirstBlockRef->des_acs.GetParamInfo();
			param.m_eDatumPlane=valueStr[0]-'0';
			pFirstBlockRef->des_acs.SetParamInfo(param);
			char sText[100]="";
			long idProp=CBlockReference::GetPropID("acs.axis_x");
			if(pFirstBlockRef->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CBlockReference::GetPropID("acs.axis_y");
			if(pFirstBlockRef->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CBlockReference::GetPropID("acs.axis_z");
			if(pFirstBlockRef->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
		}
		else if(pItem->m_idProp==CBlockReference::GetPropID("acs.origin.x"))
		{
			CParaCS::PARAMETRIC_INFO param=pFirstBlockRef->des_acs.GetParamInfo();
			param.desOrg.datum_pos.x=atof(valueStr);
			pFirstBlockRef->des_acs.SetParamInfo(param);
			pFirstBlockRef->UpdatePos();
			bRedrawSolid=TRUE;
		}
		else if(pItem->m_idProp==CBlockReference::GetPropID("acs.origin.y"))
		{
			CParaCS::PARAMETRIC_INFO param=pFirstBlockRef->des_acs.GetParamInfo();
			param.desOrg.datum_pos.y=atof(valueStr);
			pFirstBlockRef->des_acs.SetParamInfo(param);
			pFirstBlockRef->UpdatePos();
			bRedrawSolid=TRUE;
		}
		else if(pItem->m_idProp==CBlockReference::GetPropID("acs.origin.z"))
		{
			CParaCS::PARAMETRIC_INFO param=pFirstBlockRef->des_acs.GetParamInfo();
			param.desOrg.datum_pos.z=atof(valueStr);
			pFirstBlockRef->des_acs.SetParamInfo(param);
			pFirstBlockRef->UpdatePos();
			bRedrawSolid=TRUE;
		}

		if(bRedrawSolid)
		{
			for(CBlockReference *pBlockRef=(CBlockReference*)pView->selectObjs.GetFirst();pBlockRef;pBlockRef=(CBlockReference*)pView->selectObjs.GetNext())
			{
				pBlockRef->SetModified();
				pBlockRef->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBlockRef->GetSolidPartObject());
				if(pBlockRef->IsExploded())
				{	//更新打碎构件的位置
					for(CLDSPart *pPart=pBlockRef->BelongModel()->EnumPartFirst();pPart;pPart=pBlockRef->BelongModel()->EnumPartNext())
					{
						if(!pPart->IsMemberOfBlockRef(pBlockRef))
							continue;
						if(pPart->IsLinePart())
							((CLDSLinePart*)pPart)->ClearFlag();
						pPart->ModifyPartNoSegI(pBlockRef->iSeg);
						pPart->iSeg=pBlockRef->iSeg;
						pPart->cfgword=pBlockRef->cfgword;
						pPart->ShadowDesign();
						pPart->SetModified();
						pPart->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
					}
					for(CLDSNode *pNode=pBlockRef->BelongModel()->EnumNodeFirst();pNode;pNode=pBlockRef->BelongModel()->EnumNodeNext())
					{
						if(!pNode->IsMemberOfBlockRef(pBlockRef))
							continue;
						pNode->Set();
						pNode->iSeg=pBlockRef->iSeg;
						pNode->cfgword=pBlockRef->cfgword;
						f3dPoint point = pNode->ActivePosition();
						point.ID = pNode->handle;
						g_pSolidDraw->NewPoint(point);
					}
				}
			}
			g_pSolidDraw->Draw();
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	return TRUE;
}
static void RefreshUcsAxisItemValue(CPropertyList *pPropList,CBlockReference *pBlockRef)
{
	char sText[MAX_TEMP_CHAR_100+1]="";
	CPropTreeItem* pFindItem;
	//部件引用
	CParaCS::PARAMETRIC_INFO param=pBlockRef->des_acs.GetParamInfo();
	if(pBlockRef->GetPropValueStr(CBlockReference::GetPropID("acs.origin.x"),sText)>0)
	{	pPropList->SetItemPropValue(CBlockReference::GetPropID("acs.origin.x"),sText);
		pFindItem = pPropList->FindItemByPropId(CBlockReference::GetPropID("acs.origin.x"),NULL);
		if(pFindItem)
			pFindItem->SetReadOnly(param.desOrg.datum_pos_style!=0);   
	}
	if(pBlockRef->GetPropValueStr(CBlockReference::GetPropID("acs.origin.y"),sText)>0)
	{	
		pPropList->SetItemPropValue(CBlockReference::GetPropID("acs.origin.y"),sText);
		pFindItem = pPropList->FindItemByPropId(CBlockReference::GetPropID("acs.origin.y"),NULL);
		if(pFindItem)
			pFindItem->SetReadOnly(param.desOrg.datum_pos_style!=0);
	}
	if(pBlockRef->GetPropValueStr(CBlockReference::GetPropID("acs.origin.z"),sText)>0)
	{
		pPropList->SetItemPropValue(CBlockReference::GetPropID("acs.origin.z"),sText);
		pFindItem = pPropList->FindItemByPropId(CBlockReference::GetPropID("acs.origin.z"),NULL);
		if(pFindItem)
			pFindItem->SetReadOnly(param.desOrg.datum_pos_style!=0);
	}
	//
	if(pBlockRef->GetPropValueStr(CBlockReference::GetPropID("acs.axis_x.x"),sText)>0)
		pPropList->SetItemPropValue(CBlockReference::GetPropID("acs.axis_x.x"),sText);
	if(pBlockRef->GetPropValueStr(CBlockReference::GetPropID("acs.axis_x.y"),sText)>0)
		pPropList->SetItemPropValue(CBlockReference::GetPropID("acs.axis_x.y"),sText);
	if(pBlockRef->GetPropValueStr(CBlockReference::GetPropID("acs.axis_x.z"),sText)>0)
		pPropList->SetItemPropValue(CBlockReference::GetPropID("acs.axis_x.z"),sText);
	//
	if(pBlockRef->GetPropValueStr(CBlockReference::GetPropID("acs.axis_y.x"),sText)>0)
		pPropList->SetItemPropValue(CBlockReference::GetPropID("acs.axis_y.x"),sText);
	if(pBlockRef->GetPropValueStr(CBlockReference::GetPropID("acs.axis_y.y"),sText)>0)
		pPropList->SetItemPropValue(CBlockReference::GetPropID("acs.axis_y.y"),sText);
	if(pBlockRef->GetPropValueStr(CBlockReference::GetPropID("acs.axis_y.z"),sText)>0)
		pPropList->SetItemPropValue(CBlockReference::GetPropID("acs.axis_y.z"),sText);
	//
	if(pBlockRef->GetPropValueStr(CBlockReference::GetPropID("acs.axis_z.x"),sText)>0)
		pPropList->SetItemPropValue(CBlockReference::GetPropID("acs.axis_z.x"),sText);
	if(pBlockRef->GetPropValueStr(CBlockReference::GetPropID("acs.axis_z.y"),sText)>0)
		pPropList->SetItemPropValue(CBlockReference::GetPropID("acs.axis_z.y"),sText);
	if(pBlockRef->GetPropValueStr(CBlockReference::GetPropID("acs.axis_z.z"),sText)>0)
		pPropList->SetItemPropValue(CBlockReference::GetPropID("acs.axis_z.z"),sText);
}
static void RefreshBlockUcsAxisItemValue(CPropertyList *pPropList,CBlockModel *pBlock)	
{
	char sText[MAX_TEMP_CHAR_100+1]="";
	CPropTreeItem* pFindItem;
	//部件
	CParaCS::PARAMETRIC_INFO param=pBlock->des_lcs.GetParamInfo();
	if(pBlock->GetPropValueStr(CBlockModel::GetPropID("lcs.origin.x"),sText)>0)
	{
		pPropList->SetItemPropValue(CBlockModel::GetPropID("lcs.origin.x"),sText);
		pFindItem = pPropList->FindItemByPropId(CBlockModel::GetPropID("lcs.origin.x"),NULL);
		if(pFindItem)
			pFindItem->SetReadOnly(param.desOrg.datum_pos_style!=0);
	}
	if(pBlock->GetPropValueStr(CBlockModel::GetPropID("lcs.origin.y"),sText)>0)
	{
		pPropList->SetItemPropValue(CBlockModel::GetPropID("lcs.origin.y"),sText);
		pFindItem = pPropList->FindItemByPropId(CBlockModel::GetPropID("lcs.origin.y"),NULL);
		if(pFindItem)
			pFindItem->SetReadOnly(param.desOrg.datum_pos_style!=0);
	}
	if(pBlock->GetPropValueStr(CBlockModel::GetPropID("lcs.origin.z"),sText)>0)
	{
		pPropList->SetItemPropValue(CBlockModel::GetPropID("lcs.origin.z"),sText);
		pFindItem = pPropList->FindItemByPropId(CBlockModel::GetPropID("lcs.origin.z"),NULL);
		if(pFindItem)
			pFindItem->SetReadOnly(param.desOrg.datum_pos_style!=0);
	}
	//
	if(pBlock->GetPropValueStr(CBlockModel::GetPropID("lcs.axis_x.x"),sText)>0)
		pPropList->SetItemPropValue(CBlockModel::GetPropID("lcs.axis_x.x"),sText);
	if(pBlock->GetPropValueStr(CBlockModel::GetPropID("lcs.axis_x.y"),sText)>0)
		pPropList->SetItemPropValue(CBlockModel::GetPropID("lcs.axis_x.y"),sText);
	if(pBlock->GetPropValueStr(CBlockModel::GetPropID("lcs.axis_x.z"),sText)>0)
		pPropList->SetItemPropValue(CBlockModel::GetPropID("lcs.axis_x.z"),sText);
	//
	if(pBlock->GetPropValueStr(CBlockModel::GetPropID("lcs.axis_y.x"),sText)>0)
		pPropList->SetItemPropValue(CBlockModel::GetPropID("lcs.axis_y.x"),sText);
	if(pBlock->GetPropValueStr(CBlockModel::GetPropID("lcs.axis_y.y"),sText)>0)
		pPropList->SetItemPropValue(CBlockModel::GetPropID("lcs.axis_y.y"),sText);
	if(pBlock->GetPropValueStr(CBlockModel::GetPropID("lcs.axis_y.z"),sText)>0)
		pPropList->SetItemPropValue(CBlockModel::GetPropID("lcs.axis_y.z"),sText);
	//
	if(pBlock->GetPropValueStr(CBlockModel::GetPropID("lcs.axis_z.x"),sText)>0)
		pPropList->SetItemPropValue(CBlockModel::GetPropID("lcs.axis_z.x"),sText);
	if(pBlock->GetPropValueStr(CBlockModel::GetPropID("lcs.axis_z.y"),sText)>0)
		pPropList->SetItemPropValue(CBlockModel::GetPropID("lcs.axis_z.y"),sText);
	if(pBlock->GetPropValueStr(CBlockModel::GetPropID("lcs.axis_z.z"),sText)>0)
		pPropList->SetItemPropValue(CBlockModel::GetPropID("lcs.axis_z.z"),sText);
}
BOOL BlockReferenceButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem=NULL,*pParentItem=NULL,*pPropItem=NULL,*pLeafItem=NULL;
	CBlockReference *pFirstBlockRef=NULL,*pBlockRef=NULL;
	CLDSPart *pPart = NULL;
	CReportVectorDlg vecDlg;
	char sText[100]="";
	CString cmdStr,valueStr = _T("");
	if(pView->selectObjs.GetNodeNum()>0)
		pFirstBlockRef = (CBlockReference*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	try
	{
		BOOL bRedrawSolid=FALSE;
		CParaCS::PARAMETRIC_INFO param=pFirstBlockRef->des_acs.GetParamInfo();
		if(pItem->m_idProp==CBlockReference::GetPropID("cfgword"))
		{	//更新其他部件引用的配材号
			CCfgPartNoDlg cfgdlg;
			if(pFirstBlockRef->layer(0)!='L')
				cfgdlg.cfg_style=CFG_BODY_NO;
			else
				cfgdlg.cfg_style=CFG_LEG_NO;
			if(cfgdlg.DoModal()==IDOK)
			{
				for(CBlockReference *pBlockRef=(CBlockReference*)pView->selectObjs.GetFirst();pBlockRef;pBlockRef=(CBlockReference*)pView->selectObjs.GetNext())
					pBlockRef->cfgword = cfgdlg.cfgword;		//在此处更新第一个部件引用的配材料号
				char sText[MAX_TEMP_CHAR_100+1]="";
				if(pFirstBlockRef->GetPropValueStr(pItem->m_idProp,sText))	//更新配材号
					pItem->m_lpNodeInfo->m_strPropValue=sText;
			}
		}
		else if(pItem->m_idProp==CBlockReference::GetPropID("acs"))
		{
			CPartUcsDlg part_ucs_dlg;
			part_ucs_dlg.ucs=pFirstBlockRef->GetACS();
			part_ucs_dlg.DoModal();
		}
		else if(pItem->m_idProp==CBlockReference::GetPropID("acs.origin"))
		{
			CDatumPointPropertyDlg desPointDlg;
			desPointDlg.m_xPoint=param.desOrg;;
			if(desPointDlg.DoModal()==IDOK)
			{
				param.desOrg=desPointDlg.m_xPoint;
				pFirstBlockRef->des_acs.SetParamInfo(param);
				pFirstBlockRef->UpdateAcs();
				RefreshUcsAxisItemValue(pPropList,pFirstBlockRef);
				bRedrawSolid=TRUE;
			}
		}
		else if(pItem->m_idProp==CBlockReference::GetPropID("acs.axis_x"))
		{	
			if(pFirstBlockRef->des_acs.IsDesAxisX())
			{
				CDesignVectorPropDlg desVecDlg;
					desVecDlg.m_xVector = param.desAxisX;
				if(desVecDlg.DoModal()==IDOK)
				{
					param.desAxisX=desVecDlg.m_xVector;
					pFirstBlockRef->des_acs.SetParamInfo(param);
					pFirstBlockRef->UpdateAcs();
					RefreshUcsAxisItemValue(pPropList,pFirstBlockRef);
					bRedrawSolid=TRUE;
				}
			}
			else
			{
				CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
				vecDlg.m_sCaption = "X axis direction";
#else
				vecDlg.m_sCaption = "X轴方向";
#endif
				vecDlg.m_fVectorX = pFirstBlockRef->GetACS().axis_x.x;
				vecDlg.m_fVectorY = pFirstBlockRef->GetACS().axis_x.y;
				vecDlg.m_fVectorZ = pFirstBlockRef->GetACS().axis_x.z;
				vecDlg.m_bReadOnlyX=TRUE;
				vecDlg.m_bReadOnlyY=TRUE;
				vecDlg.m_bReadOnlyZ=TRUE;
				vecDlg.DoModal();
			}
		}
		else if(pItem->m_idProp==CBlockReference::GetPropID("acs.axis_y"))
		{
			if(pFirstBlockRef->des_acs.IsDesAxisY())
			{
				CDesignVectorPropDlg desVecDlg;
				desVecDlg.m_xVector = param.desAxisY;
				if(desVecDlg.DoModal()==IDOK)
				{
					param.desAxisY = desVecDlg.m_xVector;
					pFirstBlockRef->des_acs.SetParamInfo(param);
					pFirstBlockRef->UpdateAcs();
					RefreshUcsAxisItemValue(pPropList,pFirstBlockRef);
					bRedrawSolid=TRUE;
				}
			}
			else
			{
				CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
				vecDlg.m_sCaption = "Y axis direction";
#else
				vecDlg.m_sCaption = "Y轴方向";
#endif
				vecDlg.m_fVectorX = pFirstBlockRef->GetACS().axis_y.x;
				vecDlg.m_fVectorY = pFirstBlockRef->GetACS().axis_y.y;
				vecDlg.m_fVectorZ = pFirstBlockRef->GetACS().axis_y.z;
				vecDlg.m_bReadOnlyX=TRUE;
				vecDlg.m_bReadOnlyY=TRUE;
				vecDlg.m_bReadOnlyZ=TRUE;
				vecDlg.DoModal();
			}	
		}
		else if(pItem->m_idProp==CBlockReference::GetPropID("acs.axis_z"))
		{
			if(pFirstBlockRef->des_acs.IsDesAxisZ())
			{
				CDesignVectorPropDlg desVecDlg;
				desVecDlg.m_xVector = param.desAxisZ;
				if(desVecDlg.DoModal()==IDOK)
				{
					param.desAxisZ=desVecDlg.m_xVector;
					pFirstBlockRef->des_acs.SetParamInfo(param);
					pFirstBlockRef->UpdateAcs();
					RefreshUcsAxisItemValue(pPropList,pFirstBlockRef);
					bRedrawSolid=TRUE;
				}
			}
			else
			{
				CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
				vecDlg.m_sCaption = "Z axis direction";
#else
				vecDlg.m_sCaption = "Z轴方向";
#endif
				vecDlg.m_fVectorX = pFirstBlockRef->GetACS().axis_z.x;
				vecDlg.m_fVectorY = pFirstBlockRef->GetACS().axis_z.y;
				vecDlg.m_fVectorZ = pFirstBlockRef->GetACS().axis_z.z;
				vecDlg.m_bReadOnlyX=TRUE;
				vecDlg.m_bReadOnlyY=TRUE;
				vecDlg.m_bReadOnlyZ=TRUE;
				vecDlg.DoModal();
			}	
		}
		if(bRedrawSolid)
		{
			for(CBlockReference *pBlockRef=(CBlockReference*)pView->selectObjs.GetFirst();pBlockRef;pBlockRef=(CBlockReference*)pView->selectObjs.GetNext())
			{
				pBlockRef->SetModified();
				pBlockRef->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBlockRef->GetSolidPartObject());
				if(pBlockRef->IsExploded())
				{	//更新打碎构件的位置
					for(CLDSPart *pPart=pBlockRef->BelongModel()->EnumPartFirst();pPart;pPart=pBlockRef->BelongModel()->EnumPartNext())
					{
						if(!pPart->IsMemberOfBlockRef(pBlockRef))
							continue;
						if(pPart->IsLinePart())
							((CLDSLinePart*)pPart)->ClearFlag();
						pPart->ModifyPartNoSegI(pBlockRef->iSeg);
						pPart->iSeg=pBlockRef->iSeg;
						pPart->cfgword=pBlockRef->cfgword;
						pPart->ShadowDesign();
						pPart->SetModified();
						pPart->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
					}
					for(CLDSNode *pNode=pBlockRef->BelongModel()->EnumNodeFirst();pNode;pNode=pBlockRef->BelongModel()->EnumNodeNext())
					{
						if(!pNode->IsMemberOfBlockRef(pBlockRef))
							continue;
						pNode->Set();
						pNode->iSeg=pBlockRef->iSeg;
						pNode->cfgword=pBlockRef->cfgword;
						f3dPoint point = pNode->ActivePosition();
						point.ID = pNode->handle;
						g_pSolidDraw->NewPoint(point);
					}
				}
			}
			g_pSolidDraw->Draw();
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	return TRUE;
}

BOOL ModifyBlockReferenceItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CBlockReference::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
//
BOOL ModifyBlockModelProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem=NULL, *pParentItem=NULL, *pPropItem=NULL;
	CBlockModel* pFirstBlock = (CBlockModel*)pView->m_pSelectBlock;
	CBlockModel *pBlockRef = NULL;
	CItemInfo *lpInfo=NULL;
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	int iTemp=0;
	try
	{
		CParaCS::PARAMETRIC_INFO param=pFirstBlock->des_lcs.GetParamInfo();
		if(pItem->m_idProp==CBlockModel::GetPropID("name"))
		{
			strcpy(pFirstBlock->name,valueStr);
			CTowerTreeDlg *pTreeView = (CTowerTreeDlg*)((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
			char sText[100]="";
			sprintf(sText,"%s(0X%X)",valueStr,pFirstBlock->handle);
			pTreeView->RenameSelectedItem(sText);
		}
		else if(pItem->m_idProp==CBlockModel::GetPropID("m_bEmbeded"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("是")==0)
#endif
				pFirstBlock->SetEmbeded(TRUE);
			else
				pFirstBlock->SetEmbeded(FALSE);
		}
		else if(pItem->m_idProp==CBlockModel::GetPropID("des_lcs.m_eDatumPlane"))
		{
			param.m_eDatumPlane=valueStr[0]-'0';
			pFirstBlock->des_lcs.SetParamInfo(param);
			char sText[100]="";
			long idProp=CBlockModel::GetPropID("lcs.axis_x");
			if(pFirstBlock->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CBlockModel::GetPropID("lcs.axis_y");
			if(pFirstBlock->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CBlockModel::GetPropID("lcs.axis_z");
			if(pFirstBlock->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
		}
		else if(pItem->m_idProp==CBlockModel::GetPropID("lcs.origin.x"))
		{
			param.desOrg.datum_pos.x=atof(valueStr);
			pFirstBlock->des_lcs.SetParamInfo(param);
			pFirstBlock->UpdatePos();
		}
		else if(pItem->m_idProp==CBlockModel::GetPropID("lcs.origin.y"))
		{
			param.desOrg.datum_pos.y=atof(valueStr);
			pFirstBlock->des_lcs.SetParamInfo(param);
			pFirstBlock->UpdatePos();
		}
		else if(pItem->m_idProp==CBlockModel::GetPropID("lcs.origin.z"))
		{
			param.desOrg.datum_pos.z=atof(valueStr);
			pFirstBlock->des_lcs.SetParamInfo(param);
			pFirstBlock->UpdatePos();
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL BlockModelButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem=NULL,*pParentItem=NULL,*pPropItem=NULL,*pLeafItem=NULL;
	CBlockModel *pFirstBlock=NULL,*pBlock=NULL;
	CLDSPart *pPart = NULL;
	CReportVectorDlg vecDlg;
	char sText[100]="";
	CString cmdStr,valueStr = _T("");
	pFirstBlock = (CBlockModel*)pView->m_pSelectBlock;
	BOOL bRetCode=TRUE;
	try
	{
		CParaCS::PARAMETRIC_INFO param=pFirstBlock->des_lcs.GetParamInfo();
		if(pItem->m_idProp==CBlockModel::GetPropID("position"))
		{
			CPartUcsDlg part_ucs_dlg;
			part_ucs_dlg.ucs=pFirstBlock->GetLCS();
			part_ucs_dlg.DoModal();
		}
		else if(pItem->m_idProp==CBlockModel::GetPropID("lcs.origin"))
		{
			CDatumPointPropertyDlg desPointDlg;
			desPointDlg.m_pBlock=pFirstBlock;
			desPointDlg.m_xPoint=param.desOrg;;
			if(desPointDlg.DoModal()==IDOK)
			{
				param.desOrg=desPointDlg.m_xPoint;
				pFirstBlock->des_lcs.SetParamInfo(param);
				pFirstBlock->UpdateLCS();
				RefreshBlockUcsAxisItemValue(pPropList,pFirstBlock);
				if(g_sysPara.m_bDispBlockLCS)
					g_pSolidDraw->AddCS(ISolidDraw::BLK_LOCATE_CS,pFirstBlock->GetLCS(),"Xa","Ya","Za");
			}
		}
		else if(pItem->m_idProp==CBlockModel::GetPropID("lcs.axis_x"))
		{	
			if(pFirstBlock->des_lcs.IsDesAxisX())
			{
				CDesignVectorPropDlg desVecDlg;
				desVecDlg.m_pBlock=pFirstBlock;
				desVecDlg.m_xVector = param.desAxisX;
				if(desVecDlg.DoModal()==IDOK)
				{
					param.desAxisX=desVecDlg.m_xVector;
					pFirstBlock->des_lcs.SetParamInfo(param);
					pFirstBlock->UpdateLCS();
					RefreshBlockUcsAxisItemValue(pPropList,pFirstBlock);
				}
				if(g_sysPara.m_bDispBlockLCS)
					g_pSolidDraw->AddCS(ISolidDraw::BLK_LOCATE_CS,pFirstBlock->GetLCS(),"Xa","Ya","Za");
			}
			else
			{
				CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
				vecDlg.m_sCaption = "X axis direction";
#else
				vecDlg.m_sCaption = "X轴方向";
#endif
				vecDlg.m_fVectorX = pFirstBlock->GetLCS().axis_x.x;
				vecDlg.m_fVectorY = pFirstBlock->GetLCS().axis_x.y;
				vecDlg.m_fVectorZ = pFirstBlock->GetLCS().axis_x.z;
				vecDlg.m_bReadOnlyX=TRUE;
				vecDlg.m_bReadOnlyY=TRUE;
				vecDlg.m_bReadOnlyZ=TRUE;
				vecDlg.DoModal();
			}
		}
		else if(pItem->m_idProp==CBlockModel::GetPropID("lcs.axis_y"))
		{
			if(pFirstBlock->des_lcs.IsDesAxisY())
			{
				CDesignVectorPropDlg desVecDlg;
				desVecDlg.m_pBlock=pFirstBlock;
				desVecDlg.m_xVector = param.desAxisY;
				if(desVecDlg.DoModal()==IDOK)
				{
					param.desAxisY = desVecDlg.m_xVector;
					pFirstBlock->des_lcs.SetParamInfo(param);
					pFirstBlock->UpdateLCS();
					RefreshBlockUcsAxisItemValue(pPropList,pFirstBlock);
				}
				if(g_sysPara.m_bDispBlockLCS)
					g_pSolidDraw->AddCS(ISolidDraw::BLK_LOCATE_CS,pFirstBlock->GetLCS(),"Xa","Ya","Za");
			}
			else
			{
				CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
				vecDlg.m_sCaption = "Y axis direction";
#else
				vecDlg.m_sCaption = "Y轴方向";
#endif
				vecDlg.m_fVectorX = pFirstBlock->GetLCS().axis_y.x;
				vecDlg.m_fVectorY = pFirstBlock->GetLCS().axis_y.y;
				vecDlg.m_fVectorZ = pFirstBlock->GetLCS().axis_y.z;
				vecDlg.m_bReadOnlyX=TRUE;
				vecDlg.m_bReadOnlyY=TRUE;
				vecDlg.m_bReadOnlyZ=TRUE;
				vecDlg.DoModal();
			}	
		}
		else if(pItem->m_idProp==CBlockModel::GetPropID("lcs.axis_z"))
		{
			if(pFirstBlock->des_lcs.IsDesAxisZ())
			{
				CDesignVectorPropDlg desVecDlg;
				desVecDlg.m_pBlock=pFirstBlock;
				desVecDlg.m_xVector = param.desAxisZ;
				if(desVecDlg.DoModal()==IDOK)
				{
					param.desAxisZ=desVecDlg.m_xVector;
					pFirstBlock->des_lcs.SetParamInfo(param);
					pFirstBlock->UpdateLCS();
					RefreshBlockUcsAxisItemValue(pPropList,pFirstBlock);
				}
				if(g_sysPara.m_bDispBlockLCS)
					g_pSolidDraw->AddCS(ISolidDraw::BLK_LOCATE_CS,pFirstBlock->GetLCS(),"Xa","Ya","Za");
			}
			else
			{
				CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
				vecDlg.m_sCaption = "Z axis direction";
#else
				vecDlg.m_sCaption = "Z轴方向";
#endif
				vecDlg.m_fVectorX = pFirstBlock->GetLCS().axis_z.x;
				vecDlg.m_fVectorY = pFirstBlock->GetLCS().axis_z.y;
				vecDlg.m_fVectorZ = pFirstBlock->GetLCS().axis_z.z;
				vecDlg.m_bReadOnlyX=TRUE;
				vecDlg.m_bReadOnlyY=TRUE;
				vecDlg.m_bReadOnlyZ=TRUE;
				vecDlg.DoModal();
			}	
		}
		else if(pItem->m_idProp==CBlockModel::GetPropID("partList"))
		{
			ARRAY_LIST<long> arrBlkDbObjs(0,100);
			for(CLDSNode* pNode=console.DispNodeSet.GetFirst();pNode;pNode=console.DispNodeSet.GetNext())
			{
				if(pNode->m_hBlock==pFirstBlock->handle)
					arrBlkDbObjs.append(pNode->handle);
			}
			for(CLDSPart* pPart=console.DispPartSet.GetFirst();pPart;pPart=console.DispPartSet.GetNext())
			{
				if(pPart->m_hBlock==pFirstBlock->handle)
					arrBlkDbObjs.append(pPart->handle);
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(arrBlkDbObjs.m_pData,arrBlkDbObjs.Size());
			/*CBlockPartListDlg  blockPartDlg;
			blockPartDlg.m_pBlock=pFirstBlock;
			pPropList->Lock();
			blockPartDlg.DoModal();
			pPropList->Unlock();*/
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	return bRetCode;
}

BOOL ModifyBlockModelItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CBlockModel::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
//更新部件装配坐标系
static void UpdateBlockAssemblePosPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CItemInfo *lpInfo=NULL;
	CBlockModel* pFirstBlock = pView->m_pSelectBlock;
	char sText[MAX_TEMP_CHAR_100+1]="";
	CPropTreeItem *pPropItem=NULL,*pLeafItem=NULL;
	//装配平面
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbItems=CParaCS::GetDatumPlaneCmbItems();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "datum plane";
	lpInfo->m_strPropHelp = "Specify two axes direction, automatically calculate the direction of the perpendicular with current plane, modify the second axis's direction with the first axis direction unchanged, for example: x-y plane, calculate Z axis direction with X and Y axes, then modify Y axis direction with X and Z axis.";
#else
	lpInfo->m_strPropName = "基准面";
	lpInfo->m_strPropHelp = "指定两个轴方向，自动计算出垂直与当前面的方向，第一个轴方向不变修正第二个轴方向。如:X-Y面,先以X、Y轴计算Z轴方向，以X、Z轴修正Y轴方向。";
#endif
	pLeafItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("des_lcs.m_eDatumPlane");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//装配位置
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "location";
	lpInfo->m_strPropHelp = "location for assembling datum point.";
#else
	lpInfo->m_strPropName = "位置";
	lpInfo->m_strPropHelp = "装配基点的位置。";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
	pPropItem->m_idProp = CBlockModel::GetPropID("lcs.origin");
	pPropItem->m_bHideChildren = FALSE;
	if(CBlockModel::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CBlockModel::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

	CParaCS::PARAMETRIC_INFO param=pFirstBlock->des_lcs.GetParamInfo();
	//装配坐标系原点位置X坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "Assemble coordinate system origin position X coordinate component.";
#else
	lpInfo->m_strPropName = "X坐标";
	lpInfo->m_strPropHelp = "装配坐标系原点位置X坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("lcs.origin.x");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly(param.desOrg.datum_pos_style!=0);
	//装配坐标系原点位置Y坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "Assemble coordinate system origin position Y coordinate component.";
#else
	lpInfo->m_strPropName = "Y坐标";
	lpInfo->m_strPropHelp = "装配坐标系原点位置Y坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("lcs.origin.y");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly(param.desOrg.datum_pos_style!=0);
	//装配坐标系原点位置Z坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z ";
	lpInfo->m_strPropHelp = "Assemble coordinate system origin position Z coordinate components.";
#else
	lpInfo->m_strPropName = "Z坐标";
	lpInfo->m_strPropHelp = "装配坐标系原点位置Z坐标分量。";
#endif
	pLeafItem= pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("lcs.origin.z");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly(param.desOrg.datum_pos_style!=0);
	//装配时X轴方向
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Axis";
	lpInfo->m_strPropHelp = "X axis direction when assembling";
#else
	lpInfo->m_strPropName = "X轴方向";
	lpInfo->m_strPropHelp = "装配时X轴方向";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
	pPropItem->m_idProp = CBlockModel::GetPropID("lcs.axis_x");
	pPropItem->m_bHideChildren = FALSE;
	if(CBlockModel::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CBlockModel::GetPropStatus(pPropItem->m_idProp);
	if(pFirstBlock->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

	//装配坐标系X轴X坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "assemble coordinate system X coordinate component.";
#else
	lpInfo->m_strPropName = "X坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系X轴X坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("lcs.axis_x.x");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配坐标系X轴Y坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "assemble coordinate system Y coordinate component.";
#else
	lpInfo->m_strPropName = "Y坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系X轴Y坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("lcs.axis_x.y");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配坐标系X轴Z坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "assemble coordinate system X axis Z coordinate component.";
#else
	lpInfo->m_strPropName = "Z坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系X轴Z坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("lcs.axis_x.z");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配时Y轴方向
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Axis";
	lpInfo->m_strPropHelp = "Y axis direction when assembling";
#else
	lpInfo->m_strPropName = "Y轴方向";
	lpInfo->m_strPropHelp = "装配时Y轴方向";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
	pPropItem->m_idProp = CBlockModel::GetPropID("lcs.axis_y");
	pPropItem->m_bHideChildren = FALSE;
	if(CBlockModel::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CBlockModel::GetPropStatus(pPropItem->m_idProp);
	if(pFirstBlock->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//装配坐标系Y轴X坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "assemble coordinate system Y axis X coordinate component.";
#else
	lpInfo->m_strPropName = "X坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系Y轴X坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("lcs.axis_y.x");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配坐标系Y轴Y坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "assemble coordinate system Y axis Y coordinate component.";
#else
	lpInfo->m_strPropName = "Y坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系Y轴Y坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("lcs.axis_y.y");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配坐标系Y轴Z坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "assemble coordinate system Y axis Z coordinate component.";
#else
	lpInfo->m_strPropName = "Z坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系Y轴Z坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("lcs.axis_y.z");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配时Z轴方向
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Axis";
	lpInfo->m_strPropHelp = "Z axis direction when assembling";
#else
	lpInfo->m_strPropName = "Z轴方向";
	lpInfo->m_strPropHelp = "装配时Z轴方向";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
	pPropItem->m_idProp = CBlockModel::GetPropID("lcs.axis_z");
	pPropItem->m_bHideChildren = FALSE;
	if(CBlockModel::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CBlockModel::GetPropStatus(pPropItem->m_idProp);
	if(pFirstBlock->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

	//装配坐标系Z轴X坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "assemble coordinate system Z axis X coordinate component.";
#else
	lpInfo->m_strPropName = "X坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系Z轴X坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("lcs.axis_z.x");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配坐标系Z轴Y坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "assemble coordinate system Z axis Y coordinate component.";
#else
	lpInfo->m_strPropName = "Y坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系Z轴Y坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("lcs.axis_z.y");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配坐标系Z轴Z坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "assemble coordinate system Z axis Z coordinate component.";
#else
	lpInfo->m_strPropName = "Z坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系Z轴Z坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockModel::GetPropID("lcs.axis_z.z");
	if(pFirstBlock->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
}
//更新装配坐标系参数
static void UpdateAssemblePosPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CBlockReference *pFirstBlockRef = (CBlockReference*)pView->selectObjs.GetFirst();
	CItemInfo *lpInfo=NULL;
	char sText[MAX_TEMP_CHAR_100+1]="";
	CPropTreeItem *pPropItem=NULL,*pLeafItem=NULL;
	//装配平面
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbItems=CParaCS::GetDatumPlaneCmbItems();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "datum plane";
	lpInfo->m_strPropHelp = "Specify two axes direction, automatically calculate the direction of the perpendicular with current plane, modify the second axis's direction with the first axis direction unchanged, for example: x-y plane, calculate Z axis direction with X and Y axes, then modify Y axis direction with X and Z axes.";
#else
	lpInfo->m_strPropName = "基准面";
	lpInfo->m_strPropHelp = "指定两个轴方向，自动计算出垂直与当前面的方向，第一个轴方向不变修正第二个轴方向。如:X-Y面,先以X、Y轴计算Z轴方向，以X、Z轴修正Y轴方向。";
#endif
	pLeafItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("des_acs.m_eDatumPlane");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//装配位置
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "location";
	lpInfo->m_strPropHelp = "location of the assembling datum point.";
#else
	lpInfo->m_strPropName = "位置";
	lpInfo->m_strPropHelp = "装配基点的位置。";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
	pPropItem->m_idProp = CBlockReference::GetPropID("acs.origin");
	pPropItem->m_bHideChildren = FALSE;
	if(CBlockReference::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CBlockReference::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	CParaCS::PARAMETRIC_INFO param=pFirstBlockRef->des_acs.GetParamInfo();

	//装配坐标系原点位置X坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "Assemble coordinate system origin position X coordinate components.";
#else
	lpInfo->m_strPropName = "X坐标";
	lpInfo->m_strPropHelp = "装配坐标系原点位置X坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("acs.origin.x");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly(param.desOrg.datum_pos_style!=0);
	//装配坐标系原点位置Y坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "Assemble coordinate system origin position Y coordinate components.";
#else
	lpInfo->m_strPropName = "Y坐标";
	lpInfo->m_strPropHelp = "装配坐标系原点位置Y坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("acs.origin.y");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly(param.desOrg.datum_pos_style!=0);
	//装配坐标系原点位置Z坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
   	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "Assemble coordinate system origin position Z coordinate components.";
#else
	lpInfo->m_strPropName = "Z坐标";
	lpInfo->m_strPropHelp = "装配坐标系原点位置Z坐标分量。";
#endif
	pLeafItem= pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("acs.origin.z");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly(param.desOrg.datum_pos_style!=0);
	//装配时X轴方向
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Axis";
	lpInfo->m_strPropHelp = "X axis direction when assembling";
#else
	lpInfo->m_strPropName = "X轴方向";
	lpInfo->m_strPropHelp = "装配时X轴方向";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
	pPropItem->m_idProp = CBlockReference::GetPropID("acs.axis_x");
	pPropItem->m_bHideChildren = FALSE;
	if(CBlockReference::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CBlockReference::GetPropStatus(pPropItem->m_idProp);
	if(pFirstBlockRef->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

	//装配坐标系X轴X坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "assemble coordinate system  X axis  X coordinate component.";
#else
	lpInfo->m_strPropName = "X坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系X轴X坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("acs.axis_x.x");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配坐标系X轴Y坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "assemble coordinate system  X axis  Y coordinate component.";
#else
	lpInfo->m_strPropName = "Y坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系X轴Y坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("acs.axis_x.y");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配坐标系X轴Z坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "assemble coordinate system  X axis  Z coordinate component.";
#else
	lpInfo->m_strPropName = "Z坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系X轴Z坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("acs.axis_x.z");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配时Y轴方向
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Axis";
	lpInfo->m_strPropHelp = "Y axis direction when assembling";
#else
	lpInfo->m_strPropName = "Y轴方向";
	lpInfo->m_strPropHelp = "装配时Y轴方向";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
	pPropItem->m_idProp = CBlockReference::GetPropID("acs.axis_y");
	pPropItem->m_bHideChildren = FALSE;
	if(CBlockReference::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CBlockReference::GetPropStatus(pPropItem->m_idProp);
	if(pFirstBlockRef->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	//装配坐标系Y轴X坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "assemble coordinate system  Y axis  X coordinate component.";
#else
	lpInfo->m_strPropName = "X坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系Y轴X坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("acs.axis_y.x");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配坐标系Y轴Y坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "assemble coordinate system  Y axis  Y coordinate component.";
#else
	lpInfo->m_strPropName = "Y坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系Y轴Y坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("acs.axis_y.y");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配坐标系Y轴Z坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "assemble coordinate system  Y axis  Z coordinate component.";
#else
	lpInfo->m_strPropName = "Z坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系Y轴Z坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("acs.axis_y.z");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配时Z轴方向
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Axis";
	lpInfo->m_strPropHelp = "Z axis direction When assembling";
#else
	lpInfo->m_strPropName = "Z轴方向";
	lpInfo->m_strPropHelp = "装配时Z轴方向";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
	pPropItem->m_idProp = CBlockReference::GetPropID("acs.axis_z");
	pPropItem->m_bHideChildren = FALSE;
	if(CBlockReference::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CBlockReference::GetPropStatus(pPropItem->m_idProp);
	if(pFirstBlockRef->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

	//装配坐标系Z轴X坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "assembling coordinate system  Z axis  X coordinate component.";
#else
	lpInfo->m_strPropName = "X坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系Z轴X坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("acs.axis_z.x");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配坐标系Z轴Y坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "assembling coordinate system  Z axis  Y coordinate component.";
#else
	lpInfo->m_strPropName = "Y坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系Z轴Y坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("acs.axis_z.y");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
	//装配坐标系Z轴Z坐标分量
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "assembling coordinate system  Z axis  Z coordinate component.";
#else
	lpInfo->m_strPropName = "Z坐标分量";
	lpInfo->m_strPropHelp = "装配坐标系Z轴Z坐标分量。";
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CBlockReference::GetPropID("acs.axis_z.z");
	if(pFirstBlockRef->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	pLeafItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	pLeafItem->SetReadOnly();
}

void CLDSView::DisplayBlockReferenceProperty(BOOL bRebuild)
{
	CBlockReference *pFirstBlockRef = (CBlockReference*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_POS=2;
	if(bRebuild)
	{	//需要重建对象属性页
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(2);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"Location");
#else
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"常规");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"位置");
#endif
		pPropDlg->RefreshTabCtrl(CBlockReference::m_iCurDisplayPropGroup);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_BLOCKREF;
		pPropList->SetModifyValueFunc(ModifyBlockReferenceProperty);
		pPropList->SetButtonClickFunc(BlockReferenceButtonClick);
		pPropList->SetModifyStatusFunc(ModifyBlockReferenceItemStatus);
		//部件引用基本信息
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "block assembly basic info";
		lpInfo->m_strPropHelp = "block assembly basic information ";
#else
		lpInfo->m_strPropName = "部件引用基本信息";
		lpInfo->m_strPropHelp = "部件引用基本信息";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CBlockReference::GetPropID("basicInfo");
		if(CBlockReference::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CBlockReference::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
		//句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "block assembly handle";
#else
		lpInfo->m_strPropName = "句柄";
		lpInfo->m_strPropHelp = "部件引用句柄";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CBlockReference::GetPropID("handle");
		if(pFirstBlockRef->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
		//句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Belong block";
		lpInfo->m_strPropHelp = "block assembly subordinate parts";
#else
		lpInfo->m_strPropName = "从属部件";
		lpInfo->m_strPropHelp = "部件引用从属的部件";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CBlockReference::GetPropID("BelongBlock");
		if(pFirstBlockRef->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
		//名称
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Name";
		lpInfo->m_strPropHelp = "block assembly name";
#else
		lpInfo->m_strPropName = "名称";
		lpInfo->m_strPropHelp = "部件引用名称";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CBlockReference::GetPropID("descript");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstBlockRef->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//是否炸开
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName = "explode";
		lpInfo->m_strPropHelp = ", is the block assembly exploded? after explosion, inner parts can be selected separately; if not exploded, parts displayed as a whole unit.";
#else
		lpInfo->m_cmbItems="是|否";
		lpInfo->m_strPropName = "炸开";
		lpInfo->m_strPropHelp = "部件引用是否被炸开，炸开后部件内构件可以单独选中，未炸开的构件按整体显示。";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CBlockReference::GetPropID("m_bExploded");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstBlockRef->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//分段号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Segement No.";
		lpInfo->m_strPropHelp = "Segement No.";
#else
		lpInfo->m_strPropName = "分段号";
		lpInfo->m_strPropHelp = "分段号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CBlockReference::GetPropID("iSeg");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstBlockRef->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//图层
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "layer of block assembly"; 
#else
		lpInfo->m_strPropName = "图层";
		lpInfo->m_strPropHelp = "部件引用所在图层"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CBlockReference::GetPropID("layer");
		if(pFirstBlockRef->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//配材号
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Cfg word";
		lpInfo->m_strPropHelp = "model flag";
#else
		lpInfo->m_strPropName = "配材号";
		lpInfo->m_strPropHelp = "配材号";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CBlockReference::GetPropID("cfgword");
		if(pFirstBlockRef->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);

		//装配方位
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Assembling location";
		lpInfo->m_strPropHelp = "parts'assembling location and assembling direction";
#else
		lpInfo->m_strPropName = "装配方位";
		lpInfo->m_strPropHelp = "构件的装配位置及装配时的装配方向。";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CBlockReference::GetPropID("acs");
		if(CBlockReference::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CBlockReference::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//
		UpdateAssemblePosPropItem(pPropList,pGroupItem);
		
	}
	else
	{	//不需要重建对象属性页
		SetPartMutiObjsProp(pFirstBlockRef,"handle");
		SetPartMutiObjsProp(pFirstBlockRef,"BelongBlock");
		SetPartMutiObjsProp(pFirstBlockRef,"descript");
		SetPartMutiObjsProp(pFirstBlockRef,"m_bExploded");
		SetPartMutiObjsProp(pFirstBlockRef,"iSeg");
		SetPartMutiObjsProp(pFirstBlockRef,"cfgword");
		SetPartMutiObjsProp(pFirstBlockRef,"layer");
		SetPartMutiObjsProp(pFirstBlockRef,"acs");
		SetPartMutiObjsProp(pFirstBlockRef,"des_acs.m_eDatumPlane");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.origin");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.origin.x");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.origin.y");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.origin.z");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.axis_x");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.axis_x.x");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.axis_x.y");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.axis_x.z");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.axis_y");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.axis_y.x");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.axis_y.y");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.axis_y.z");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.axis_z");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.axis_z.x");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.axis_z.y");
		SetPartMutiObjsProp(pFirstBlockRef,"acs.axis_z.z");
	}
	pPropList->Redraw();
}

void CLDSView::DisplayBlockModelProperty(BOOL bRebuild)
{
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pRoot=pPropList->GetRootItem();
	CBlockModel *pFirstBlock = (CBlockModel*)m_pSelectBlock;
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_POS=2;
	
	//需要重建对象属性页
	pPropDlg->m_arrPropGroupLabel.RemoveAll();
	pPropDlg->m_arrPropGroupLabel.SetSize(2);
#ifdef AFX_TARG_ENU_ENGLISH
	pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
	pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"Location");
#else
	pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"常规");
	pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"位置");
#endif
	pPropDlg->RefreshTabCtrl(CBlockModel::m_iCurDisplayPropGroup);

	pPropList->CleanTree();
	pPropList->m_nObjClassTypeId = CLS_BLOCK;
	pPropList->SetModifyValueFunc(ModifyBlockModelProperty);
	pPropList->SetButtonClickFunc(BlockModelButtonClick);
	pPropList->SetModifyStatusFunc(ModifyBlockModelItemStatus);
	//部件基本信息
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "block's basic info";
	lpInfo->m_strPropHelp = "block's basic information";
#else
	lpInfo->m_strPropName = "部件基本信息";
	lpInfo->m_strPropHelp = "部件基本信息";
#endif
	CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp = CBlockModel::GetPropID("basicInfo");
	if(CBlockModel::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CBlockModel::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
	//句柄
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Handle";
	lpInfo->m_strPropHelp = "block's handle";
#else
	lpInfo->m_strPropName = "句柄";
	lpInfo->m_strPropHelp = "部件句柄";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CBlockModel::GetPropID("handle");
	if(pFirstBlock->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->SetReadOnly();
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
	//名称
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Name";
	lpInfo->m_strPropHelp = "block's name ";
#else
	lpInfo->m_strPropName = "名称";
	lpInfo->m_strPropHelp = "部件名称";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CBlockModel::GetPropID("name");
	if(pFirstBlock->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	//是否内嵌式部件块
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="Yes|No";
	lpInfo->m_strPropName = "embedded";
	lpInfo->m_strPropHelp = "is embedded block unit?";
#else
	lpInfo->m_cmbItems="是|否";
	lpInfo->m_strPropName = "内嵌";
	lpInfo->m_strPropHelp = "是否是内嵌式部件块";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CBlockModel::GetPropID("m_bEmbeded");
	if(pFirstBlock->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	pPropItem->SetReadOnly();
	//构件信息
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_COMMON;
	lpInfo->m_sButtonName="选中显示";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "part's info";
	lpInfo->m_strPropHelp = "block's information contained in block";
#else
	lpInfo->m_strPropName = "数据对象";
	lpInfo->m_strPropHelp = "部件所包含的构件信息";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CBlockModel::GetPropID("partList");
	if(pFirstBlock->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	//装配方位
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Positioning direction";
	lpInfo->m_strPropHelp = "positioning location of parts and coordinate direction.";
#else
	lpInfo->m_strPropName = "定位方位";
	lpInfo->m_strPropHelp = "构件的定位位置及定位时的坐标轴方向。";
#endif
	pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp = CBlockModel::GetPropID("position");
	if(CBlockModel::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CBlockModel::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
	UpdateBlockAssemblePosPropItem(pPropList,pGroupItem);
	
	pPropList->Redraw();
}
#elif !defined(__TSA_)
void CLDSView::DisplayBlockReferenceProperty(BOOL bRebuild){;}
void CLDSView::DisplayBlockModelProperty(BOOL bRebuild){;}	
#endif
