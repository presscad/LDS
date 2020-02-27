//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "TowerPropertyDlg.h"
#include "CfgPartNoDlg.h"
#include "NodeDesignPosDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "LmaDlg.h"
#include "InputJgGDlg.h"
#include "CutWingAngleDlg.h"
#include "DesignJgWingNorm.h"
#include "Query.h"
#include "OddmentDesignDlg.h"
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
#include "TubeEndLjGuiGeTblDlg.h"
#include "DesignVectorDlg.h"
#include "ReportPointDlg.h"
#include "PartUcsDlg.h"
#include "ModifyRelativeObjPartNoDlg.h"
#include "FaceOffsetDlg.h"
#include "ParamPlateParamListDlg.h"
#include "GroupJgStyleDlg.h"
#include "DefPlaneDlg.h"
#include "DesignArcPartBaseLineDlg.h"
#include "ConfirmArcPartStyleDlg.h"
#include "BaseLineStartEndPosDlg.h"
#include "IntelligentConnectParaDlg.h"
#include "DefHuoquFaceDlg.h"
#include "EditPlateDatumFaceDlg.h"
#include "XhCharString.h"
#include "RevisionOperDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//from DisplayPropertyPage.cpp
BOOL SetPartMutiObjsProp(CLDSObject *pObj, char *propName,long idClassType=0);
BOOL LinePartButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLinePart *pFirstLinePart=NULL, *pLinePart=NULL;
	CCfgPartNoDlg cfgdlg;
	COddmentDesignDlg odddlg;
	char sText[MAX_TEMP_CHAR_100+1]="";
	if(pView->selectObjs.GetNodeNum()>0)
		pFirstLinePart = (CLDSLinePart*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	CXhChar16 sSoftName("LMA");
#if defined(__LDS_FILE_)||defined(__LDS_)
	sSoftName.Copy("LDS");
#elif defined(__LMA_FILE_)||defined(__LMA_)
	sSoftName.Copy("LMA");
#elif defined(__TMA_FILE_)||defined(__TMA_)
	sSoftName.Copy("TMA");
#elif defined(__TSA_FILE_)||defined(__TSA_)
	sSoftName.Copy("TSA");
#endif
	CString valueStr = _T("");
	if(pItem->m_idProp==CLDSLinePart::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstLinePart->cfgword; 
		if(pFirstLinePart->IsBodyObj())
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
			pFirstLinePart->cfgword = cfgdlg.cfgword;		//�ڴ˴����µ�һ���ڵ������Ϻ�
		if(pFirstLinePart->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("hStart"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Start Node";
		modifyhdlg.m_sTitle="Start Node:";
#else 
		modifyhdlg.m_sCaption="�޸ĸ˼�ʼ�˽ڵ�";
		modifyhdlg.m_sTitle="ʼ�˽ڵ�:";
#endif
		if(pFirstLinePart->pStart)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLinePart->pStart->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select datum node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�ڵ�:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong,so can't find the corresponding node��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
			else
			{
				CUndoOperObject undo(&Ta);
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLinePart::GetPropID("hStart"),valueStr);
				for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
				{
					CLDSNode* pOldNode=pLinePart->pStart;
					pLinePart->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),pNode,NULL);
					pLinePart->pStart=pNode;
					pLinePart->SetModified();
					pLinePart->ClearFlag();
					pLinePart->CalPosition();
					//���¸˼��ϵĶ˽ڵ������Ϣ
					pLinePart->UpdateRodEndNodeRelation(pOldNode,pNode);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("hEnd"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify End Node";
		modifyhdlg.m_sTitle="End Node:";
#else 
		modifyhdlg.m_sCaption="�޸ĸ˼��ն˽ڵ�";
		modifyhdlg.m_sTitle="�ն˽ڵ�:";
#endif
		if(pFirstLinePart->pEnd)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLinePart->pEnd->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select datum node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�ڵ�:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong,so can't find the corresponding node��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
			else
			{
				CUndoOperObject undo(&Ta);
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLinePart::GetPropID("hEnd"),valueStr);
				for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
				{
					CLDSNode* pOldNode=pLinePart->pEnd;
					pLinePart->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),NULL,pNode);
					pLinePart->pEnd=pNode;
					pLinePart->SetModified();
					pLinePart->ClearFlag();
					pLinePart->CalPosition();
					//���¸˼��ϵĶ˽ڵ������Ϣ
					pLinePart->UpdateRodEndNodeRelation(pOldNode,pNode);
				}
			}
		}
	}
#if defined(__TSA_)||defined(__LDS_)
		else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_uStatMatNo"))
		{
			UINT iNo=Ta.GetNewStatNo();
			valueStr.Format("%d",iNo);
			for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
				pLinePart->m_uStatMatNo = iNo;
			pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
		}
		else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.pRefPole"))
		{
			CModifyPartHandleDlg modifyhdlg;
			int update_type=0;	//0.�ڵ㸸�˼� 1.�ڵ���������1 2.�ڵ���������2
			modifyhdlg.m_sCaption="�޸Ĳ��ո˼�";
			modifyhdlg.m_sTitle="���ո˼�:";
			if(pFirstLinePart->CalLenCoef.hRefPole>0x20)
			{
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLinePart->CalLenCoef.hRefPole);
				g_pSolidDraw->SetEntSnapStatus(pFirstLinePart->CalLenCoef.hRefPole,TRUE);
			}
			else
				modifyhdlg.m_sHandle="0X0";
			modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
			modifyhdlg.m_bDispSelectBtn=TRUE;	//��ʾѡ��ڵ�/�˼��İ�ť
			modifyhdlg.m_bEnableMir=FALSE;		//������Գ�
			BOOL bRetOk=FALSE;
			if(modifyhdlg.DoModal()==IDOK)
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if(pPart==NULL||(pPart&&!pPart->IsLinePart()&&!pPart->IsArcPart()))
					MessageBox(NULL,"�����������Ҳ�����Ӧ�˼���","LDS",MB_OK);
				else
				{
					g_pSolidDraw->SetEntSnapStatus(pPart->handle,FALSE);
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLinePart::GetPropID("CalLenCoef.pRefPole"),valueStr);
					for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
						pLinePart->CalLenCoef.hRefPole=modifyhdlg.m_nHandle;
				}
			}
		}
		else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.pHorizPole"))
		{
			CModifyPartHandleDlg modifyhdlg;
			int update_type=0;	//0.�ڵ㸸�˼� 1.�ڵ���������1 2.�ڵ���������2
			modifyhdlg.m_sCaption="�޸ĺ����˼�";
			modifyhdlg.m_sTitle="�����˼�:";
			if(pFirstLinePart->CalLenCoef.hHorizPole>0x20)
			{
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLinePart->CalLenCoef.hHorizPole);
				g_pSolidDraw->SetEntSnapStatus(pFirstLinePart->CalLenCoef.hHorizPole,TRUE);
			}
			else
				modifyhdlg.m_sHandle="0X0";
			modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�/�˼��İ�ť
			if(modifyhdlg.DoModal()==IDOK)
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if(pPart==NULL||(pPart&&!pPart->IsLinePart()&&!pPart->IsArcPart()))
					MessageBox(NULL,"�����������Ҳ�����Ӧ�˼���","LDS",MB_OK);
				else
				{
					g_pSolidDraw->SetEntSnapStatus(pPart->handle,FALSE);
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLinePart::GetPropID("CalLenCoef.pHorizPole"),valueStr);
					for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
						pLinePart->CalLenCoef.hHorizPole=modifyhdlg.m_nHandle;
				}
			}
		}
		else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.minR.hStartNode"))
		{
			CModifyPartHandleDlg modifyhdlg;
			modifyhdlg.m_sCaption="�޸���С����㳤����ʼ�ڵ�";
			modifyhdlg.m_sTitle="��ʼ�ڵ�:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLinePart->CalLenCoef.minR.hStartNode);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
			modifyhdlg.SetCmdPickPromptStr("��ѡ����С����㳤����ʼ�ڵ�:");
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pNode==NULL)
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLinePart::GetPropID("CalLenCoef.minR.hStartNode"),valueStr);
					for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
						pLinePart->CalLenCoef.minR.hStartNode=pNode->handle;
				}
			}
		}
		else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.minR.hEndNode"))
		{
			CModifyPartHandleDlg modifyhdlg;
			modifyhdlg.m_sCaption="�޸���С����㳤����ֹ�ڵ�";
			modifyhdlg.m_sTitle="��ֹ�ڵ�:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLinePart->CalLenCoef.minR.hEndNode);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
			modifyhdlg.SetCmdPickPromptStr("��ѡ����С����㳤����ֹ�ڵ�:");
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pNode==NULL)
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLinePart::GetPropID("CalLenCoef.minR.hEndNode"),valueStr);
					for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
						pLinePart->CalLenCoef.minR.hEndNode=pNode->handle;
				}
			}
		}
		else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.paraR.hStartNode"))
		{
			CModifyPartHandleDlg modifyhdlg;
			modifyhdlg.m_sCaption="�޸�ƽ������㳤����ʼ�ڵ�";
			modifyhdlg.m_sTitle="��ʼ�ڵ�:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLinePart->CalLenCoef.paraR.hStartNode);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
			modifyhdlg.SetCmdPickPromptStr("��ѡ��ƽ������㳤����ʼ�ڵ�:");
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pNode==NULL)
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLinePart::GetPropID("CalLenCoef.paraR.hStartNode"),valueStr);
					for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
						pLinePart->CalLenCoef.paraR.hStartNode=pNode->handle;
				}
			}
		}
		else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.paraR.hEndNode"))
		{
			CModifyPartHandleDlg modifyhdlg;
			modifyhdlg.m_sCaption="�޸�ƽ������㳤����ֹ�ڵ�";
			modifyhdlg.m_sTitle="��ֹ�ڵ�:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLinePart->CalLenCoef.paraR.hEndNode);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
			modifyhdlg.SetCmdPickPromptStr("��ѡ��ƽ������㳤����ֹ�ڵ�:");
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pNode==NULL)
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLinePart::GetPropID("CalLenCoef.paraR.hEndNode"),valueStr);
					for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
						pLinePart->CalLenCoef.paraR.hEndNode=pNode->handle;
				}
			}
		}
#endif
		else if(pItem->m_idProp==CLDSLinePart::GetPropID("relativeObjs"))
		{	
			CRelativeObjectDlg relativeDlg;
			relativeDlg.m_pWorkObj = pFirstLinePart;
			relativeDlg.DoModal();
		}	
#ifndef __TSA_
		else if(pFirstLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle *pGroupLineAngle=(CLDSGroupLineAngle*)pFirstLinePart;
			if(pItem->m_idProp==CLDSGroupLineAngle::GetPropID("group_style")==0)
			{
				CGroupJgStyleDlg group_jg_style_dlg;
				//group_jg_style_dlg.m_iJgGroupStyle=pGroupLineAngle->group_style;
				group_jg_style_dlg.m_fFillThick=pGroupLineAngle->jg_space;
				/*
				if(group_jg_style_dlg.DoModal()==IDOK)
				{
					pGroupLineAngle->group_style=group_jg_style_dlg.m_iJgGroupStyle;
					pGroupLineAngle->jg_space=group_jg_style_dlg.m_fFillThick;
				}
				if(pGroupLineAngle->GetPropValueStr(CLDSGroupLineAngle::GetPropID("jg_space"),sText)>0)
					pPropList->SetItemPropValue(CLDSGroupLineAngle::GetPropID("jg_space"),CString(sText));
				*/
			}
			else
				return FALSE;
		}
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("oddStart"))
	{
		odddlg.m_hRefPart1.Format("0X%X",pFirstLinePart->desStartOdd.m_hRefPart1);
		odddlg.m_hRefPart2.Format("0X%X",pFirstLinePart->desStartOdd.m_hRefPart2);
		odddlg.m_fCollideDist = pFirstLinePart->desStartOdd.m_fCollideDist;
		odddlg.m_iOddCalStyle = pFirstLinePart->desStartOdd.m_iOddCalStyle;
		if(odddlg.DoModal()==IDOK)
		{
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart1);
			sscanf(sText,"%X",&pFirstLinePart->desStartOdd.m_hRefPart1);
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart2);
			sscanf(sText,"%X",&pFirstLinePart->desStartOdd.m_hRefPart2);
			pFirstLinePart->desStartOdd.m_fCollideDist = odddlg.m_fCollideDist;
			pFirstLinePart->desStartOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
			pFirstLinePart->CalStartOddment();
			if(pFirstLinePart->GetPropValueStr(pItem->m_idProp,sText)>0)
				pItem->m_lpNodeInfo->m_strPropValue=sText;
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("oddEnd"))
	{
		odddlg.m_hRefPart1.Format("0X%X",pFirstLinePart->desEndOdd.m_hRefPart1);
		odddlg.m_hRefPart2.Format("0X%X",pFirstLinePart->desEndOdd.m_hRefPart2);
		odddlg.m_fCollideDist = pFirstLinePart->desEndOdd.m_fCollideDist;
		odddlg.m_iOddCalStyle = pFirstLinePart->desEndOdd.m_iOddCalStyle;
		if(odddlg.DoModal()==IDOK)
		{
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart1);
			sscanf(sText,"%X",&pFirstLinePart->desEndOdd.m_hRefPart1);
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart2);
			sscanf(sText,"%X",&pFirstLinePart->desEndOdd.m_hRefPart2);
			pFirstLinePart->desEndOdd.m_fCollideDist = odddlg.m_fCollideDist;
			pFirstLinePart->desEndOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
			pFirstLinePart->CalEndOddment();
			if(pFirstLinePart->GetPropValueStr(pItem->m_idProp,sText)>0)
				pItem->m_lpNodeInfo->m_strPropValue=sText;
		}
	}
#endif
	else
		return FALSE;
	return TRUE;
}

BOOL ModifyRodPropItemHelpLiveString(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSObject *pObj=pView->selectObjs.GetFirst();
	if( pObj==NULL||!pObj->IsDbObject()||
		!((CLDSDbObject*)pObj)->IsPart()||!((CLDSPart*)pObj)->IsLinePart())
		return FALSE;	//���Ǹ˼�
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	g_pSolidDraw->DestroyAllMark();
	CLDSLinePart *pRod = (CLDSLinePart*)pObj;
	if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.minR.hStartNode"))
	{
		CLDSNode* pAttachNode=console.FromNodeHandle(pRod->CalLenCoef.minR.hStartNode);
		if(pAttachNode)
			g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
		pItem->m_lpNodeInfo->m_strPropHelp="The start node of minimal gyration length";
#else 
		pItem->m_lpNodeInfo->m_strPropHelp="��С����㳤����ʼ�ڵ�";
#endif
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.minR.hEndNode"))
	{
		CLDSNode* pAttachNode=console.FromNodeHandle(pRod->CalLenCoef.minR.hEndNode);
		if(pAttachNode)
			g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
		pItem->m_lpNodeInfo->m_strPropHelp="The end node of minimal gyration length";
#else 
		pItem->m_lpNodeInfo->m_strPropHelp="��С����㳤����ֹ�ڵ�";
#endif
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.paraR.hStartNode"))
	{
		CLDSNode* pAttachNode=console.FromNodeHandle(pRod->CalLenCoef.paraR.hStartNode);
		if(pAttachNode)
			g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
		pItem->m_lpNodeInfo->m_strPropHelp="The start node of parallel gyration length";
#else 
		pItem->m_lpNodeInfo->m_strPropHelp="ƽ������㳤����ʼ�ڵ�";
#endif
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.paraR.hEndNode"))
	{
		CLDSNode* pAttachNode=console.FromNodeHandle(pRod->CalLenCoef.paraR.hEndNode);
		if(pAttachNode)
			g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
		pItem->m_lpNodeInfo->m_strPropHelp="The end node of parallel gyration length";
#else 
		pItem->m_lpNodeInfo->m_strPropHelp="ƽ������㳤����ֹ�ڵ�";
#endif
	}
	else
#endif
		return FALSE;
	return TRUE;
}
BOOL ModifyLinePartItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSLinePart::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
BOOL ModifyLinePartProperty(CPropertyList *pPropList, CPropTreeItem* pItem, CString &valueStr);
#if defined(__TSA_)||defined(__LDS_)
void UpdateAuxLinePartPropItem(CLDSLinePart *pFirstLinePart,CPropertyList *pPropList,CLDSView *pView)
{
	CItemInfo *lpInfo;
	CPropTreeItem *pGroupItem,*pPropItem,*pParentItem;
	char sText[100]="";
	const int GROUP_GENERAL=1,GROUP_DESIGNINFO=2;
	if(pFirstLinePart->IsAuxPole())//||pFirstLinePart->IsPrimaryRod(true))
	{
		pGroupItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("designInfo"),&pParentItem);
		int idProp=CLDSLinePart::GetPropID("sel_mat_method");
		pPropItem = pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_strPropName = "����֧��������";
			lpInfo->m_strPropHelp = "���ĶԶ�ͷ�˼�֧�����ļ�������";
			if(pFirstLinePart->IsAuxPole())
				lpInfo->m_cmbItems ="�Զ����|����ʼ��|�����ն�|����б��";
			else
				lpInfo->m_cmbItems ="�Զ����|����ʼ��|�����ն�|������";
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,2,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("sel_mat_method");
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);
		
		/*idProp=CLDSLinePart::GetPropID("bear_load_coef");
		pPropItem = pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "������֧����ϵ��";
			lpInfo->m_strPropHelp = "������֧����ϵ��";
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,3,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("bear_load_coef");
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);*/
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("sel_mat_method"));
		//pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("bear_load_coef"));
	}
}
void UpdateLinePartCalLenCoefPropItem(CLDSLinePart *pFirstLinePart,CPropertyList *pPropList,CLDSView *pView)
{
	int idProp;
	CItemInfo *lpInfo;
	char sText[100]="";
	CPropTreeItem *pPropItem,*pSonPropItem,*pParentItem;
	const int GROUP_GENERAL=1,GROUP_DESIGNINFO=2;
	pPropItem = pPropList->FindItemByPropId(CLDSLinePart::GetPropID("CalLenCoef.iTypeNo"),&pParentItem);
	if(pFirstLinePart->CalLenCoef.iTypeNo==0||pFirstLinePart->CalLenCoef.iTypeNo==6)
	{
		idProp=CLDSLinePart::GetPropID("CalLenCoef.pRefPole");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "ͬʱ��ѹ���ո˼�";
			lpInfo->m_strPropHelp = "ͬʱ��ѹ���ո˼�";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);
	}
	else
		pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("CalLenCoef.pRefPole"));
	if(pFirstLinePart->CalLenCoef.iTypeNo==3||pFirstLinePart->CalLenCoef.iTypeNo==4)
	{
		idProp=CLDSLinePart::GetPropID("CalLenCoef.pHorizPole");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "���պ�����";
			lpInfo->m_strPropHelp = "���㳤�Ȳ��պ�����";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);
	}
	else
		pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("CalLenCoef.pHorizPole"));
	if(pFirstLinePart->CalLenCoef.iTypeNo==5||pFirstLinePart->CalLenCoef.iTypeNo==6)
	{
		idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenMinR");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "��С�᳤��";
			lpInfo->m_strPropHelp = "��С�᳤��,��λ(mm)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_bHideChildren=FALSE;
			if(CLDSLinePart::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonPropItem->SetReadOnly();
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);

		idProp=CLDSLinePart::GetPropID("CalLenCoef.minR.hStartNode");
		CPropTreeItem* pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "��ʼ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ʼ�ڵ���";
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.minR.hStartNode");
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);

		idProp=CLDSLinePart::GetPropID("CalLenCoef.minR.hEndNode");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "��ֹ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ֹ�ڵ���";
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.minR.hEndNode");
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);

		idProp=CLDSLinePart::GetPropID("CalLenCoef.minR.coef");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ϵ��";
			lpInfo->m_strPropHelp = "���㳤�ȵ�����ϵ��";
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.minR.coef");
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		
		idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenParaR");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "ƽ���᳤��";
			lpInfo->m_strPropHelp = "ƽ���᳤��,��λ(mm)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_bHideChildren=FALSE;
			if(CLDSLinePart::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonPropItem->SetReadOnly();
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);

		idProp=CLDSLinePart::GetPropID("CalLenCoef.paraR.hStartNode");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "��ʼ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ʼ�ڵ���";
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.paraR.hStartNode");
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);

		idProp=CLDSLinePart::GetPropID("CalLenCoef.paraR.hEndNode");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "��ֹ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ֹ�ڵ���";
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.paraR.hEndNode");
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);

		idProp=CLDSLinePart::GetPropID("CalLenCoef.paraR.coef");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ϵ��";
			lpInfo->m_strPropHelp = "���㳤�ȵ�����ϵ��";
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.paraR.coef");
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("CalLenCoef.spec_callenMinR"));
		pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("CalLenCoef.minR.hStartNode"));
		pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("CalLenCoef.minR.hEndNode"));
		pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("CalLenCoef.minR.coef"));
		pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("CalLenCoef.spec_callenParaR"));
		pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("CalLenCoef.paraR.hStartNode"));
		pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("CalLenCoef.paraR.hEndNode"));
		pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("CalLenCoef.paraR.coef"));
	}
}
#endif
CString GetBlockListStr();	//defined in DisplayNodeProperty.cpp
long RetrieveBlockHandle(const char* blockIdName);
BOOL ModifyLinePartProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLinePart *pFirstLinePart = (CLDSLinePart*)pView->selectObjs.GetFirst();
	CLDSLinePart *pLinePart = NULL;
#if defined(__LDS_)||defined(__TSA_)
	int idProp;
	CItemInfo *lpInfo;
	CPropTreeItem *pFindItem,*pPropItem,*pParentItem;//,*pNewItem;
#endif
	int m_cPosCalType = 0;
	const int GROUP_GENERAL=1,GROUP_DESIGNINFO=2;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	if(pItem->m_idProp==CLDSLinePart::GetPropID("layer"))
	{
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->SetLayer(tem_str);	
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//ͼ�㲻֧����ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart)
					CalMirAtomLayer(pLinePart->layer(),pMirPart->layer(),pMirObj->mirInfo);
			}
		}
#if defined(__TSA_)||defined(__LDS_)
		//���¸���������ʾ��Ŀ��Ϣ
		//UpdateAuxLinePartPropItem(pFirstLinePart,pPropList,pView);
		//���³�ϸ�����������ʾ��Ŀ��Ϣ(ֻ��ͼ����������������ļ�ת��ʱ�Ÿ�����Ч)
		UpdateLinePartCalLenCoefPropItem(pFirstLinePart,pPropList,pView);
#endif
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("cMaterial"))
	{	//����
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->cMaterial = QuerySteelBriefMatMark(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				CLDSLinePart *pMirLinePart=(CLDSLinePart*)pLinePart->BelongModel()->FromPartHandle(pMirObj->hObj);
				if(pMirLinePart)
					pMirLinePart->cMaterial=pLinePart->cMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_cQualityLevel"))
	{	//�����ȼ�
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Default")==0)
				pLinePart->m_cQualityLevel=0;
			else
#else
			if(valueStr.CompareNoCase("Ĭ��")==0)
				pLinePart->m_cQualityLevel=0;
			else
#endif
				pLinePart->m_cQualityLevel = valueStr[0];
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				CLDSLinePart *pMirLinePart=(CLDSLinePart*)pLinePart->BelongModel()->FromPartHandle(pMirObj->hObj);
				if(pMirLinePart)
					pMirLinePart->m_cQualityLevel=pLinePart->m_cQualityLevel;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("iSeg"))
	{
		SEGI iSeg = SEGI(valueStr.GetBuffer());
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->iSeg = iSeg;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				CLDSLinePart *pMirLinePart=(CLDSLinePart*)pLinePart->BelongModel()->FromPartHandle(pMirObj->hObj);
				if(pMirLinePart)
					pMirLinePart->iSeg=pLinePart->iSeg;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_hBlock"))
	{
		long hBlock=RetrieveBlockHandle(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->m_hBlock=hBlock;
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("sPartNo"))
	{
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->SetPartNo(tem_str);
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("cfgword"))
	{
		//���������˼�����ĺ�
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->cfgword = pFirstLinePart->cfgword;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				CLDSLinePart *pMirLinePart=(CLDSLinePart*)pLinePart->BelongModel()->FromPartHandle(pMirObj->hObj);
				if(pMirLinePart)
					pMirLinePart->cfgword=pLinePart->cfgword;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_uStatMatCoef"))
	{
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->m_uStatMatCoef = atoi(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				CLDSLinePart *pMirLinePart=(CLDSLinePart*)pLinePart->BelongModel()->FromPartHandle(pMirObj->hObj);
				if(pMirLinePart)
					pMirLinePart->m_uStatMatCoef=pLinePart->m_uStatMatCoef;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_hPartWeldParent"))
	{
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->m_hPartWeldParent=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_bDisplayHole"))
	{
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pLinePart->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pLinePart->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pLinePart->m_bDisplayHole = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_bVirtualPart"))
	{
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pLinePart->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pLinePart->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pLinePart->m_bVirtualPart = FALSE;
		}
	}
	
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_bUserSpecColor"))
	{
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pLinePart->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pLinePart->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pLinePart->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				CLDSLinePart *pMirPart=(CLDSLinePart*)pLinePart->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEPART);
				if(pMirPart)
					pMirPart->m_bUserSpecColor=pLinePart->m_bUserSpecColor;
			}
		}
		if(pFirstLinePart->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSLinePart::GetPropID("crMaterial"),NULL);
			if(pFindItem==NULL)
			{
				char sText[MAX_TEMP_CHAR_100+1]="";
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Part Color";
				lpInfo->m_strPropHelp = "Part's color";
#else 
				lpInfo->m_strPropName = "������ɫ";
				lpInfo->m_strPropHelp = "������ɫ";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = CLDSLinePart::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if(pFirstLinePart->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("crMaterial"));
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				CLDSLinePart *pMirLinePart=(CLDSLinePart*)pLinePart->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEPART);
				if(pMirLinePart)
					pMirLinePart->crMaterial=pLinePart->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("sNotes"))
	{
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			_snprintf(pLinePart->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("oddStart"))
	{	//����ͷ	//�����Ƿ�Ϊ������ �ж��Ƿ������������ͷ�� δ���...  
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->SetStartOdd(atof(valueStr));
			pLinePart->desStartOdd.m_iOddCalStyle=2;	//�ֶ��Ķ�����ͷ������ͷ���㷽ʽ����Ϊ����������ͷ
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("oddEnd"))
	{
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{	
			pLinePart->SetEndOdd(atof(valueStr));
			pLinePart->desEndOdd.m_iOddCalStyle=2;	//�ֶ��Ķ�����ͷ������ͷ���㷽ʽ����Ϊ����������ͷ
		}
	}
#endif
//
#if defined(__LDS_)||defined(__TSA_)
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("size.idClassType"))
	{
		if(valueStr.Compare("��֫�Ǹ�")==0)
		{
			pFirstLinePart->size_idClassType=CLS_LINEANGLE;
			pFirstLinePart->size_cSubClassType='L';
		}
		else if(valueStr.Compare("T�����")==0)
		{
			pFirstLinePart->size_idClassType=CLS_GROUPLINEANGLE;
			pFirstLinePart->size_cSubClassType='T';
		}
		else if(valueStr.Compare("�Խ����")==0)
		{
			pFirstLinePart->size_idClassType=CLS_GROUPLINEANGLE;
			pFirstLinePart->size_cSubClassType='D';
		}
		else if(valueStr.Compare("ʮ�����")==0)
		{
			pFirstLinePart->size_idClassType=CLS_GROUPLINEANGLE;
			pFirstLinePart->size_cSubClassType='X';
		}
		else if(valueStr.Compare("�ֹ�")==0)
			pFirstLinePart->size_idClassType=CLS_LINETUBE;
		else if(valueStr.Compare("���")==0)
			pFirstLinePart->size_idClassType=CLS_LINEFLAT;
		else if(valueStr.Compare("�۸�")==0)
			pFirstLinePart->size_idClassType=CLS_LINESLOT;
		else if(valueStr.Compare("����")==0)
			pFirstLinePart->size_idClassType=CLS_CABLE;
		else if(valueStr.Compare("�û�����")==0)
			pFirstLinePart->size_idClassType=0;
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->size_idClassType = pFirstLinePart->size_idClassType;
			((CLDSLinePart*)pLinePart)->size_cSubClassType=pFirstLinePart->size_cSubClassType;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
				{
					((CLDSLinePart*)pMirPart)->size_idClassType=pLinePart->size_idClassType;
					((CLDSLinePart*)pMirPart)->size_cSubClassType=pLinePart->size_cSubClassType;
				}
			}
		}

		if(pFirstLinePart->size_idClassType==CLS_CABLE)	//�û��������
		{
			pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("specification"));
			pFindItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("m_sWireType"),&pParentItem);
			if(pFindItem==NULL)
			{
				pParentItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("basicInfo"),NULL);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_strPropName = "��������";
				lpInfo->m_strPropHelp = "��������";
				lpInfo->m_cmbItems=MakeWireTypeSetString();
				pFindItem = pPropList->InsertItem(pParentItem,lpInfo,pItem->m_iIndex,TRUE);
				pFindItem->m_idProp = CLDSLinePart::GetPropID("m_sWireType");
				if(CLDSLinePart::GetPropStatus(pFindItem->m_idProp)!=-1)
					pFindItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pFindItem->m_idProp);
				else
					pFindItem->m_bHideChildren=FALSE;
			}
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_sWireType"));
			pFindItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("specification"),&pParentItem);
			if(pFindItem==NULL)
			{
				pParentItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("basicInfo"),NULL);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_strPropName = "�˼����";
				lpInfo->m_strPropHelp = "�˼����";
				pFindItem = pPropList->InsertItem(pParentItem,lpInfo,pItem->m_iIndex,TRUE);
				pFindItem->m_idProp = CLDSLinePart::GetPropID("specification");
			}
		}
		if(pFirstLinePart->GetPropValueStr(pFindItem->m_idProp,tem_str)>0)
			pFindItem->m_lpNodeInfo->m_strPropValue=tem_str;
		if(CLDSLinePart::GetPropStatus(pFindItem->m_idProp)!=-1)
			pFindItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pFindItem->m_idProp);
		if(pFirstLinePart->size_idClassType==CLS_CABLE)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_strPropName = "���¹��";
			lpInfo->m_strPropHelp = "���¾������ͺ�";
			lpInfo->m_cmbItems=MakeWireSetString(tem_str);
			pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("m_sWireSpec");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
				lpInfo->m_strPropValue = tem_str;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�����";
			lpInfo->m_strPropHelp = "���½����,��λ(mm2)";
			pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fWireArea");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
				lpInfo->m_strPropValue = tem_str;
			pPropItem->SetReadOnly();
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ϵ��(MPa)";
			lpInfo->m_strPropHelp = "���µ���ϵ��(ģ��),��λ(MPa)";
			pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fElasticCoef");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
				lpInfo->m_strPropValue = tem_str;
			pPropItem->SetReadOnly();
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�¶�������ϵ��";
			lpInfo->m_strPropHelp = "�¶�������ϵ��,��λ�����������¶�����1��Cʱ�ĳ���������";
			pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fExpansionCoef");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
				lpInfo->m_strPropValue = tem_str;
			pPropItem->SetReadOnly();
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "��λ����(kg/km)";
			lpInfo->m_strPropHelp = "��λ�������µ�����,��λ(kg/km)";
			pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fUnitWeight");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
				lpInfo->m_strPropValue = tem_str;
			pPropItem->SetReadOnly();
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�����ǿ��";
			lpInfo->m_strPropHelp = "�����ǿ��,kN";
			pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fForcePermit");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
				lpInfo->m_strPropValue = tem_str;
			pPropItem->SetReadOnly();
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Ԥ�Ž���";
			lpInfo->m_strPropHelp = "��װʱԤ�Ž���,kN";
			pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fPreTension");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
				lpInfo->m_strPropValue = tem_str;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		}
		else if(pFirstLinePart->size_idClassType==0)	//�û��������
		{
			pFindItem->m_lpNodeInfo->m_controlType=PIT_EDIT;
			
			idProp=CLDSLinePart::GetPropID("userdef.size");
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "�Զ�����";
				lpInfo->m_strPropHelp = "�Զ���˼�������";
				pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.size");
			}
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);
			if(pFirstLinePart->GetPropValueStr(idProp,tem_str)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = tem_str;
			
			idProp=CLDSLinePart::GetPropID("userdef.area");
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "�����";
				lpInfo->m_strPropHelp = "�˼������,��λ(mm2)";
				pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.area");
			}
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);
			if(pFirstLinePart->GetPropValueStr(idProp,tem_str)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = tem_str;

			idProp=CLDSLinePart::GetPropID("userdef.E");
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "����ģ��";
				lpInfo->m_strPropHelp = "�˼�����������ģ��,��λ(MPa)";
				pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = idProp;
			}
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);
			if(pFirstLinePart->GetPropValueStr(idProp,tem_str)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = tem_str;

			idProp=CLDSLinePart::GetPropID("userdef.fc");
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��ѹǿ�����ֵ";
				lpInfo->m_strPropHelp = "��ѹǿ�����ֵ,��λ(MPa)";
				pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = idProp;
			}
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);
			if(pFirstLinePart->GetPropValueStr(idProp,tem_str)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = tem_str;

			idProp=CLDSLinePart::GetPropID("userdef.ft");
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "����ǿ�����ֵ";
				lpInfo->m_strPropHelp = "����ǿ�����ֵ,��λ(MPa)";
				pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = idProp;
			}
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);
			if(pFirstLinePart->GetPropValueStr(idProp,tem_str)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = tem_str;

			idProp=CLDSLinePart::GetPropID("userdef.pre_stress");
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "ԤӦ��";
				lpInfo->m_strPropHelp = "��װʱԤ��Ӧ��ֵ,��λ(Pa)";
				pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = idProp;
			}
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);
			if(pFirstLinePart->GetPropValueStr(idProp,tem_str)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = tem_str;

			idProp=CLDSLinePart::GetPropID("userdef.unit_weight");
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��λ����";
				lpInfo->m_strPropHelp = "��λ���ȸ˼�����,��λ(kg/m)";
				pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = idProp;
			}
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);
			if(pFirstLinePart->GetPropValueStr(idProp,tem_str)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = tem_str;
		}
		else
		{
			pFindItem->m_lpNodeInfo->m_controlType=PIT_COMBO;
			pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.size"));	//���û�ָ��ʱɾ���û������� wht 10-11-22
			pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.area"));
			pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.E"));
			pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.fc"));
			pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.ft"));
			pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.pre_stress"));
			pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.unit_weight"));

			if(pFirstLinePart->size_idClassType==CLS_LINEANGLE)
				pFindItem->m_lpNodeInfo->m_cmbItems=MakeAngleSetString();
			else if(pFirstLinePart->size_idClassType==CLS_GROUPLINEANGLE)
				pFindItem->m_lpNodeInfo->m_cmbItems=MakeAngleSetString(pFirstLinePart->size_cSubClassType);
			else if(pFirstLinePart->size_idClassType==CLS_LINETUBE)
				pFindItem->m_lpNodeInfo->m_cmbItems=MakeTubeSetString();
			else if(pFirstLinePart->size_idClassType==CLS_LINESLOT)
				pFindItem->m_lpNodeInfo->m_cmbItems=MakeSlotSetString();
			else if(pFirstLinePart->size_idClassType==CLS_LINEFLAT)
				pFindItem->m_lpNodeInfo->m_cmbItems=MakeFlatSetString();
			else if(pFirstLinePart->size_idClassType==CLS_CABLE)
				pFindItem->m_lpNodeInfo->m_cmbItems=MakeFlatSetString();
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_sWireType"))
	{
		CXhChar200 wireType;
		strcpy(wireType,valueStr);
		pFirstLinePart->SetWireType(wireType);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->SetWireType(pFirstLinePart->GetWireType());
		pFindItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("m_sWireSpec"),&pParentItem);
		if(pFindItem)
			pFindItem->m_lpNodeInfo->m_cmbItems=::MakeWireSetString(pFirstLinePart->GetWireType());
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_sWireSpec"))
	{
		CXhChar200 size_spec;
		size_spec.Copy(valueStr);
		pFirstLinePart->SetSpec(size_spec);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->SetSpec(pFirstLinePart->GetSpec());
		idProp=CLDSLinePart::GetPropID("m_fWireArea");
		if(pFirstLinePart->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
		idProp=CLDSLinePart::GetPropID("m_fElasticCoef");
		if(pFirstLinePart->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
		idProp=CLDSLinePart::GetPropID("m_fForcePermit");
		if(pFirstLinePart->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
		idProp=CLDSLinePart::GetPropID("m_fExpansionCoef");
		if(pFirstLinePart->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
		idProp=CLDSLinePart::GetPropID("m_fPreTension");
		if(pFirstLinePart->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
		idProp=CLDSLinePart::GetPropID("m_fUnitWeight");
		if(pFirstLinePart->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_fPreTension"))
	{
		pFirstLinePart->m_fPreTension=1000*atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->m_fPreTension=pFirstLinePart->m_fPreTension;
	}
#endif
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("specification")||pItem->m_idProp==CLDSLinePart::GetPropID("userdef.size"))
	{
		char sWide[20]={0},sThick[20]={0};
		int i=0,j=0;
		BOOL bWideFinished=FALSE;
		for(i=0;i<valueStr.GetLength();i++)
		{
			if((valueStr[i]>='0'&&valueStr[i]<='9')||valueStr[i]=='.')
			{
				if(!bWideFinished)
					sWide[j]=valueStr[i];
				else
					sThick[j]=valueStr[i];
				j++;
			}
			else
			{
				if(strlen(sWide)>0)
					bWideFinished=TRUE;
				else if(strlen(sThick)>0)
					break;
				j=0;
			}
		}
		pFirstLinePart->size_wide=atof(sWide);
		pFirstLinePart->size_thick=atof(sThick);
#ifndef __TSA_
		if(pFirstLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			((CLDSGroupLineAngle*)pFirstLinePart)->UpdateSonJgBasicInfo();
#endif
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->size_wide = pFirstLinePart->size_wide;
			pLinePart->size_thick = pFirstLinePart->size_thick;
			if(pLinePart->size_idClassType==CLS_LINEANGLE||pLinePart->size_idClassType==CLS_GROUPLINEANGLE)
			{
				pLinePart->connectStart.d=SelBoltDByWingWide(pLinePart->size_wide,pLinePart->connectStart.grade);
				pLinePart->connectEnd.d=SelBoltDByWingWide(pLinePart->size_wide,pLinePart->connectEnd.grade);
				if(pLinePart==pFirstLinePart)
				{
					SetPartMutiObjsProp(pFirstLinePart,"connectStart.d");
					SetPartMutiObjsProp(pFirstLinePart,"connectStart.grade");
					SetPartMutiObjsProp(pFirstLinePart,"connectEnd.d");
					SetPartMutiObjsProp(pFirstLinePart,"connectEnd.grade");
				}
			}
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�	
				//�˼�����֧����ת�Գ� wht 10-11-20
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
				{
					((CLDSLinePart*)pMirPart)->size_wide=pLinePart->size_wide;
					((CLDSLinePart*)pMirPart)->size_thick=pLinePart->size_thick;
					if(((CLDSLinePart*)pMirPart)->size_idClassType==CLS_LINEANGLE||((CLDSLinePart*)pMirPart)->size_idClassType==CLS_GROUPLINEANGLE)
					{
						((CLDSLinePart*)pMirPart)->connectStart.d=SelBoltDByWingWide(pLinePart->size_wide,((CLDSLinePart*)pMirPart)->connectStart.grade);
						((CLDSLinePart*)pMirPart)->connectEnd.d=SelBoltDByWingWide(pLinePart->size_wide,((CLDSLinePart*)pMirPart)->connectEnd.grade);
					}
#ifndef __TSA_
					if(((CLDSLinePart*)pMirPart)->GetClassTypeId()==CLS_GROUPLINEANGLE)
						((CLDSGroupLineAngle*)pMirPart)->UpdateSonJgBasicInfo();
#endif
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("userdef.area"))
	{
		pFirstLinePart->userdef.area=atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->userdef.area = pFirstLinePart->userdef.area;
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("userdef.E"))
	{
		pFirstLinePart->userdef.E=atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->userdef.E = pFirstLinePart->userdef.E;
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("userdef.fc"))
	{
		pFirstLinePart->userdef.fc=atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->userdef.fc = pFirstLinePart->userdef.fc;
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("userdef.ft"))
	{
		pFirstLinePart->userdef.ft=atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->userdef.ft = pFirstLinePart->userdef.ft;
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("userdef.pre_stress"))
	{
		pFirstLinePart->userdef.pre_stress=atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->userdef.pre_stress = pFirstLinePart->userdef.pre_stress;
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("userdef.unit_weight"))
	{
		pFirstLinePart->userdef.unit_weight=atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->userdef.unit_weight = pFirstLinePart->userdef.unit_weight;
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("hStart"))
	{
		char *endptr;
		long h=strtol(tem_str,&endptr,16);
		pFirstLinePart->pStart=console.FromNodeHandle(h);
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("hEnd"))
	{
		char *endptr;
		long h=strtol(tem_str,&endptr,16);
		pFirstLinePart->pEnd=console.FromNodeHandle(h);
	}
#if defined(__TSA_)||defined(__LDS_)
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_uStatMatNo"))
	{
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->m_uStatMatNo = (WORD)atoi(valueStr);
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
				{
					((CLDSLinePart*)pMirPart)->m_uStatMatNo=pLinePart->m_uStatMatNo;
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_fWeightRaiseCoef"))
	{
		pFirstLinePart->m_fWeightRaiseCoef=atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->m_fWeightRaiseCoef = pFirstLinePart->m_fWeightRaiseCoef;
#ifdef __LDS_
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
				((CLDSGroupLineAngle*)pLinePart)->UpdateSonJgBasicInfo();
#endif
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
				{
					((CLDSLinePart*)pMirPart)->m_fWeightRaiseCoef=pLinePart->m_fWeightRaiseCoef;
#ifdef __LDS_
					if(pMirPart->GetClassTypeId()==CLS_LINEANGLE)
						((CLDSGroupLineAngle*)pMirPart)->UpdateSonJgBasicInfo();
#endif
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_bNeedSelSize"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else 
		if(valueStr.Compare("��")==0)
#endif
			pFirstLinePart->m_bNeedSelSize=TRUE;
		else
			pFirstLinePart->m_bNeedSelSize=FALSE;
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->m_bNeedSelSize = pFirstLinePart->m_bNeedSelSize;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
				{
					((CLDSLinePart*)pMirPart)->m_bNeedSelSize=pLinePart->m_bNeedSelSize;
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_iElemType"))
	{
		pFirstLinePart->m_iElemType=atoi(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->m_iElemType = pFirstLinePart->m_iElemType;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
				{
					((CLDSLinePart*)pMirPart)->m_iElemType=pLinePart->m_iElemType;
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("sel_mat_method"))
	{
		if(valueStr.Compare("�Զ����")==0)
			pFirstLinePart->sel_mat_method=AUX_AUTODESIGN;
		else if(valueStr.Compare("����ʼ��")==0)
			pFirstLinePart->sel_mat_method=AUX_BY_START;
		else if(valueStr.Compare("�����ն�")==0)
			pFirstLinePart->sel_mat_method=AUX_BY_END;
		else if(valueStr.Compare("����б��")==0||valueStr.Compare("������")==0)
			pFirstLinePart->sel_mat_method=AUX_ONLY_PRIMARY;
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->sel_mat_method = pFirstLinePart->sel_mat_method;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				CLDSLinePart *pMirRod=console.FromRodHandle(pMirObj->hObj);
				if(pMirRod)
					pMirRod->sel_mat_method=pLinePart->sel_mat_method;
			}
		}
	}
	//else if(pItem->m_idProp==CLDSLinePart::GetPropID("bear_load_coef"))
	//{
	//	pFirstLinePart->bear_load_coef=atof(valueStr);
	//	for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
	//		pLinePart->bear_load_coef = pFirstLinePart->bear_load_coef;
	//}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("hole_out"))
	{
		pFirstLinePart->hole_out=atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->hole_out = pFirstLinePart->hole_out;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
				{
					((CLDSLinePart*)pMirPart)->hole_out=pLinePart->hole_out;
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("m_bAutoCalHoleOutCoef"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else 
		if(valueStr.Compare("��")==0)
#endif
			pFirstLinePart->m_bAutoCalHoleOutCoef=TRUE;
		else
			pFirstLinePart->m_bAutoCalHoleOutCoef=FALSE;
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->m_bAutoCalHoleOutCoef = pFirstLinePart->m_bAutoCalHoleOutCoef;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->m_bAutoCalHoleOutCoef=pLinePart->m_bAutoCalHoleOutCoef;
			}
		}
		pFindItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("hole_out"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(pFirstLinePart->m_bAutoCalHoleOutCoef);
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.iTypeNo"))
	{
		if(valueStr.CompareNoCase("0.�����ж�")==0)
			pFirstLinePart->CalLenCoef.iTypeNo=0;
		else if(valueStr.CompareNoCase("1.Vб��(��Ų�����)")==0)
			pFirstLinePart->CalLenCoef.iTypeNo=1;
		else if(valueStr.CompareNoCase("2.Vб��(V������)")==0)
			pFirstLinePart->CalLenCoef.iTypeNo=2;
		else if(valueStr.CompareNoCase("3.Vб��3�ڼ��������")==0)
			pFirstLinePart->CalLenCoef.iTypeNo=3;
		else if(valueStr.CompareNoCase("4.Vб��4�ڼ��������")==0)
			pFirstLinePart->CalLenCoef.iTypeNo=4;
		else if(valueStr.CompareNoCase("6.ָ������ļ��㳤��")==0)
			pFirstLinePart->CalLenCoef.iTypeNo=6;
		else if(valueStr.CompareNoCase("5.ָ���˼����㳤��")==0)
			pFirstLinePart->CalLenCoef.iTypeNo=5;
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->CalLenCoef.iTypeNo = pFirstLinePart->CalLenCoef.iTypeNo;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->CalLenCoef.iTypeNo=pLinePart->CalLenCoef.iTypeNo;
			}
		}
		//���³�ϸ�����������ʾ��Ŀ��Ϣ
		UpdateLinePartCalLenCoefPropItem(pFirstLinePart,pPropList,pView);
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.pRefPole"))
	{	//ͬʱ��ѹ���ո˼�
		char *endptr;
		long h=strtol(tem_str,&endptr,16);
		CLDSPart *pPart=console.FromPartHandle(h);
		if(pPart&&pPart->IsLinePart())
			pFirstLinePart->CalLenCoef.hRefPole=pPart->handle;
		else
			pFirstLinePart->CalLenCoef.hRefPole=0;
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->CalLenCoef.hRefPole = pFirstLinePart->CalLenCoef.hRefPole;
			pLinePart->SyncMirProp("DesignInfo");
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.pHorizPole"))
	{	//���պ�����
		char *endptr;
		long h=strtol(tem_str,&endptr,16);
		CLDSPart *pPart=console.FromPartHandle(h);
		if(pPart&&pPart->IsLinePart())
			pFirstLinePart->CalLenCoef.hHorizPole=pPart->handle;
		else
			pFirstLinePart->CalLenCoef.hRefPole=0;
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->CalLenCoef.hHorizPole = pFirstLinePart->CalLenCoef.hHorizPole;
			pLinePart->SyncMirProp("DesignInfo");
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.minR.hStartNode"))
	{
		char *endptr;
		pFirstLinePart->CalLenCoef.minR.hStartNode=strtol(tem_str,&endptr,16);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->CalLenCoef.minR.hStartNode = pFirstLinePart->CalLenCoef.minR.hStartNode;
			pLinePart->SyncMirProp("DesignInfo");
		}
		long idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenMinR");
		if(pFirstLinePart->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.minR.hEndNode"))
	{
		char *endptr;
		pFirstLinePart->CalLenCoef.minR.hEndNode=strtol(tem_str,&endptr,16);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->CalLenCoef.minR.hEndNode = pFirstLinePart->CalLenCoef.minR.hEndNode;
			pLinePart->SyncMirProp("DesignInfo");
		}
		long idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenMinR");
		if(pFirstLinePart->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.minR.coef"))
	{
		pFirstLinePart->CalLenCoef.minR.coef=atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->CalLenCoef.minR.coef = pFirstLinePart->CalLenCoef.minR.coef;
			pLinePart->SyncMirProp("DesignInfo");
		}
		long idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenMinR");
		if(pFirstLinePart->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.paraR.hStartNode"))
	{
		char *endptr;
		pFirstLinePart->CalLenCoef.paraR.hStartNode=strtol(tem_str,&endptr,16);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->CalLenCoef.paraR.hStartNode = pFirstLinePart->CalLenCoef.paraR.hStartNode;
			pLinePart->SyncMirProp("DesignInfo");
		}
		long idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenParaR");
		if(pFirstLinePart->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.paraR.hEndNode"))
	{
		char *endptr;
		pFirstLinePart->CalLenCoef.paraR.hEndNode=strtol(tem_str,&endptr,16);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->CalLenCoef.paraR.hEndNode = pFirstLinePart->CalLenCoef.paraR.hEndNode;
			pLinePart->SyncMirProp("DesignInfo");
		}
		long idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenParaR");
		if(pFirstLinePart->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.paraR.coef"))
	{
		pFirstLinePart->CalLenCoef.paraR.coef=atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->CalLenCoef.paraR.coef = pFirstLinePart->CalLenCoef.paraR.coef;
			pLinePart->SyncMirProp("DesignInfo");
		}
		long idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenParaR");
		if(pFirstLinePart->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	/*else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.spec_callenMinR"))
	{
		pFirstLinePart->CalLenCoef.spec_callenMinR=atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->CalLenCoef.spec_callenMinR = pFirstLinePart->CalLenCoef.spec_callenMinR;
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("CalLenCoef.spec_callenParaR"))
	{
		pFirstLinePart->CalLenCoef.spec_callenParaR=atof(valueStr);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
			pLinePart->CalLenCoef.spec_callenParaR = pFirstLinePart->CalLenCoef.spec_callenParaR;
	}*/
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("start_joint_type"))
	{
		if(valueStr.Compare("�½�")==0)
			pFirstLinePart->start_joint_type=JOINT_HINGE;
		else if(valueStr.Compare("�ս�")==0)
			pFirstLinePart->start_joint_type=JOINT_RIGID;
		else
			pFirstLinePart->start_joint_type=JOINT_AUTOJUSTIFY;
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->start_joint_type = pFirstLinePart->start_joint_type;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->start_joint_type=pLinePart->start_joint_type;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("start_force_type"))
	{
		if(valueStr.Compare("��������")==0)
			pFirstLinePart->start_force_type=CENTRIC_FORCE;
		else if(valueStr.Compare("ƫ������")==0)
			pFirstLinePart->start_force_type=ECCENTRIC_FORCE;
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->start_force_type = pFirstLinePart->start_force_type;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->start_force_type=pLinePart->start_force_type;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("connectStart.grade"))
	{
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			_snprintf(pLinePart->connectStart.grade,4,"%s",valueStr);
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					strcpy(((CLDSLinePart*)pMirPart)->connectStart.grade,pLinePart->connectStart.grade);
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("connectStart.d"))
	{
		tem_str[0]=' '; //������˨ֱ��M20�ĵ�һ����ĸ M 
		pFirstLinePart->connectStart.d=atoi(tem_str);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->connectStart.d=pFirstLinePart->connectStart.d;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->connectStart.d=pLinePart->connectStart.d;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("connectStart.N"))
	{
		pFirstLinePart->connectStart.wnConnBoltN=atoi(tem_str);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->connectStart.wnConnBoltN=pFirstLinePart->connectStart.wnConnBoltN;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->connectStart.wnConnBoltN=pLinePart->connectStart.wnConnBoltN;
			}
		}
	}
	/* ��Щ���������Ǽ������������û��ӽ����޸� wjh-2017.2.20
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("connectStart.maxTension"))
	{
		pFirstLinePart->connectStart.maxTension=atoi(tem_str);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->connectStart.maxTension=pFirstLinePart->connectStart.maxTension;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->connectStart.maxTension=pLinePart->connectStart.maxTension;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("connectStart.maxCompression"))
	{
		pFirstLinePart->connectStart.maxCompression=atoi(tem_str);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->connectStart.maxCompression=pFirstLinePart->connectStart.maxCompression;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->connectStart.maxCompression=pLinePart->connectStart.maxCompression;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("connectEnd.N"))
	{
		pFirstLinePart->connectEnd.N=atoi(tem_str);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->connectEnd.N=pFirstLinePart->connectEnd.N;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->connectEnd.N=pLinePart->connectEnd.N;
			}
		}
	}
	*/
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("end_joint_type"))
	{
		if(valueStr.Compare("�½�")==0)
			pFirstLinePart->end_joint_type=JOINT_HINGE;
		else if(valueStr.Compare("�ս�")==0)
			pFirstLinePart->end_joint_type=JOINT_RIGID;
		else
			pFirstLinePart->end_joint_type=JOINT_AUTOJUSTIFY;
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->end_joint_type = pFirstLinePart->end_joint_type;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->end_joint_type=pLinePart->end_joint_type;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("end_force_type"))
	{
		if(valueStr.Compare("��������")==0)
			pFirstLinePart->end_force_type=CENTRIC_FORCE;
		else if(valueStr.Compare("ƫ������")==0)
			pFirstLinePart->end_force_type=ECCENTRIC_FORCE;
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->end_force_type = pFirstLinePart->end_force_type;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->end_force_type=pLinePart->end_force_type;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("connectEnd.grade"))
	{
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			_snprintf(pLinePart->connectEnd.grade,4,"%s",valueStr);
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					strcpy(((CLDSLinePart*)pMirPart)->connectEnd.grade,pLinePart->connectEnd.grade);
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("connectEnd.d"))
	{
		tem_str[0]=' ';	//������˨ֱ��M20�ĵ�һ����ĸ M 
		pFirstLinePart->connectEnd.d=atoi(tem_str);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->connectEnd.d=pFirstLinePart->connectEnd.d;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->connectEnd.d=pLinePart->connectEnd.d;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("connectEndt.maxTension"))
	{
		pFirstLinePart->connectEnd.maxTension=atoi(tem_str);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->connectEnd.maxTension=pFirstLinePart->connectEnd.maxTension;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->connectEnd.maxTension=pLinePart->connectEnd.maxTension;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLinePart::GetPropID("connectEnd.maxCompression"))
	{
		pFirstLinePart->connectEnd.maxCompression=atoi(tem_str);
		for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
		{
			pLinePart->connectEnd.maxCompression=pFirstLinePart->connectEnd.maxCompression;
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
					((CLDSLinePart*)pMirPart)->connectEnd.maxCompression=pLinePart->connectEnd.maxCompression;
			}
		}
	}
#endif
//
	else
		return FALSE;
	for(pLinePart=(CLDSLinePart*)pView->selectObjs.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)pView->selectObjs.GetNext())
	{
		pLinePart->SetModified();
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
		if(g_sysPara.iColorSchema==1)		//TSA��ɫ����
		{
			PEN_STRU pen;
			pen.style=PS_SOLID;
			pen.width=0;
			if(pLinePart->IsPostRod())
				pen.crColor = g_sysPara.crTsaMode.crPostLine;
			else if(!pLinePart->IsAuxPole())
				pen.crColor = g_sysPara.crTsaMode.crPrimaryLine;
			else
				pen.crColor = g_sysPara.crTsaMode.crAuxLine;
			g_pSolidSet->SetPen(pen);
		}
#endif
		pLinePart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pLinePart->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

void CLDSView::DisplayLinePartProperty(BOOL bRebuild)
{
	CLDSLinePart *pFirstLinePart = (CLDSLinePart*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pSonPropItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_DESIGNINFO=2;
	if(bRebuild)
	{//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
#if defined(__TSA_)||defined(__LDS_)
		pPropDlg->m_arrPropGroupLabel.SetSize(2);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DESIGNINFO-1,"Design");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DESIGNINFO-1,"���");
#endif
		pPropDlg->RefreshTabCtrl(CLDSLinePart::m_iCurDisplayPropGroup);
#else
		pPropDlg->m_arrPropGroupLabel.SetSize(0);
		pPropDlg->RefreshTabCtrl(0);
#endif

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_LINEPART;
		pPropList->SetModifyValueFunc(ModifyLinePartProperty);
		pPropList->SetButtonClickFunc(LinePartButtonClick);
		pPropList->SetModifyStatusFunc(ModifyLinePartItemStatus);
		pPropList->SetPropHelpPromptFunc(ModifyRodPropItemHelpLiveString);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstLinePart->BelongModel()->IsTowerModel()||pFirstLinePart->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//������Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Rod Basic Information";
		lpInfo->m_strPropHelp = "The basic information of rod.";
#else 
		lpInfo->m_strPropName = "�˼�������Ϣ";
		lpInfo->m_strPropHelp = "ֱ���͸˼�������Ϣ";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLinePart::GetPropID("basicInfo");
		if(CLDSLinePart::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLinePart::GetPropStatus(pGroupItem->m_idProp);
#if defined(__TSA_)||defined(__LDS_)
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
#endif
			//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "The handle is typical unique identifier string in the current system.";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "����ǵ�ǰϵͳ�жԹ�����ȫ��Ψһ��ʶ";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("handle");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
#if defined(__TSA_)||defined(__LDS_)
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
#endif
			//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "The part in the layer";
#else 
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "��������ͼ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("layer");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
#if defined(__TSA_)||defined(__LDS_)
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
#endif
		//��ĺ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Configure Word";
		lpInfo->m_strPropHelp = "Configure word";
#else 
		lpInfo->m_strPropName = "��ĺ�";
		lpInfo->m_strPropHelp = "��ĺ�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("cfgword");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
#if defined(__TSA_)||defined(__LDS_)
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
#endif
#ifndef __TSA_
		//�ֶκ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Segment Number";
		lpInfo->m_strPropHelp = "Segment number";
#else 
		lpInfo->m_strPropName = "�ֶκ�";
		lpInfo->m_strPropHelp = "�ֶκ�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("iSeg");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
#endif
#if (defined(__TSA_)||defined(__LDS_)) && !defined(__LDS_TDA_)
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_FILEPATH;
		lpInfo->m_strPropName = "ͳ�ı��";
		lpInfo->m_strPropHelp = "ͳ�ı�ţ������Ҳఴť�ɻ�ȡһ��δʹ�õ��ºš�";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("m_uStatMatNo");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
#endif
			//����	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Material";
		lpInfo->m_strPropHelp = "The material of part.";
#else 
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "����";
#endif
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390|Q420|Q460";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("cMaterial");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
#if defined(__TSA_)||defined(__LDS_)
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
#endif
		//����	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Quality Grade";
		lpInfo->m_strPropHelp = "The quality grade of part.";
		lpInfo->m_cmbItems = "Default|A|B|C|D|";
#else 
		lpInfo->m_strPropName = "�����ȼ�";
		lpInfo->m_strPropHelp = "�����ȼ�";
		lpInfo->m_cmbItems = "Ĭ��|A|B|C|D|";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("m_cQualityLevel");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
#if defined(__TSA_)||defined(__LDS_)
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
#endif
#if defined(__TSA_)||defined(__LDS_)
		CPropTreeItem *pSonPropItem;
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�˼�����";
		lpInfo->m_strPropHelp = "�˼�����";
		lpInfo->m_cmbItems = "��֫�Ǹ�|T�����|�Խ����|ʮ�����|�ֹ�|�۸�|���|����|�û�����";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("size.idClassType");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		if(pFirstLinePart->size_idClassType==CLS_CABLE)
		{
			lpInfo->m_strPropName = "��������";
			lpInfo->m_strPropHelp = "��������";
			lpInfo->m_cmbItems=MakeWireTypeSetString();
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("m_sWireType");
		}
		else
		{
			lpInfo->m_strPropName = "�˼����";
			lpInfo->m_strPropHelp = "�˼����";
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("specification");
		}
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSLinePart::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pPropItem->m_idProp);
		else
			pPropItem->m_bHideChildren=FALSE;
		if(pFirstLinePart->size_idClassType==CLS_CABLE)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_strPropName = "���¹��";
			lpInfo->m_strPropHelp = "���¾������ͺ�";
			lpInfo->m_cmbItems=MakeWireSetString(sText);
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("m_sWireSpec");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�����";
			lpInfo->m_strPropHelp = "���½����,��λ(mm2)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("m_fWireArea");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ϵ��(MPa)";
			lpInfo->m_strPropHelp = "���µ���ϵ��(ģ��),��λ(MPa)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("m_fElasticCoef");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�¶�������ϵ��";
			lpInfo->m_strPropHelp = "�¶�������ϵ��,��λ�����������¶�����1��Cʱ�ĳ���������";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("m_fExpansionCoef");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "��λ����(kg/km)";
			lpInfo->m_strPropHelp = "��λ�������µ�����,��λ(kg/km)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("m_fUnitWeight");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�����ǿ��";
			lpInfo->m_strPropHelp = "�����ǿ��,kN";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("m_fForcePermit");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Ԥ�Ž���";
			lpInfo->m_strPropHelp = "��װʱԤ�Ž���,kN";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("m_fPreTension");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
		}
		else if(pFirstLinePart->size_idClassType==0)//TRUSS_POLE_USERDEF)
		{
			pPropItem->m_lpNodeInfo->m_controlType = PIT_EDIT;
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�Զ�����";
			lpInfo->m_strPropHelp = "�Զ���˼�������";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.size");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�����";
			lpInfo->m_strPropHelp = "�˼������,��λ(mm2)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.area");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ģ��";
			lpInfo->m_strPropHelp = "�˼�����������ģ��,��λ(MPa)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.E");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "��ѹǿ�����ֵ";
			lpInfo->m_strPropHelp = "��ѹǿ�����ֵ,��λ(MPa)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.fc");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ǿ�����ֵ";
			lpInfo->m_strPropHelp = "����ǿ�����ֵ,��λ(MPa)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.ft");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "��λ����";
			lpInfo->m_strPropHelp = "��λ���ȸ˼�����,��λ(kg/m)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.unit_weight");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "ԤӦ��";
			lpInfo->m_strPropHelp = "��װʱԤ��Ӧ��ֵ,��λ(Pa)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.pre_stress");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGNINFO);
		}
		else
		{
			pPropItem->m_lpNodeInfo->m_controlType = PIT_COMBO;
			if(pFirstLinePart->size_idClassType==CLS_LINEANGLE)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeAngleSetString();
			else if(pFirstLinePart->size_idClassType==CLS_GROUPLINEANGLE)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeAngleSetString(pFirstLinePart->size_cSubClassType);
			else if(pFirstLinePart->size_idClassType==CLS_LINETUBE)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeTubeSetString();
			else if(pFirstLinePart->size_idClassType==CLS_LINESLOT)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeSlotSetString();
			else if(pFirstLinePart->size_idClassType==CLS_LINEFLAT)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeFlatSetString();
		}
#endif
		if(!pFirstLinePart->IsShadowObject())
		{	//��������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Relative Object";
			lpInfo->m_strPropHelp = "The object relate to currently object and mirror style";
#else
			lpInfo->m_strPropName = "��������";
			lpInfo->m_strPropHelp = "�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("relativeObjs");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		}
		//�˼�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Single Line Length";
		lpInfo->m_strPropHelp = "The length of single line.";
#else 
		lpInfo->m_strPropName = "���߳���";
		lpInfo->m_strPropHelp = "���߳���";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("lengthSingleLine");
		pPropItem->SetReadOnly();
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(!theApp.m_bAnalysisWorkEnvOnly)
		{	//ͳ��ϵ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "BOM Number";
			lpInfo->m_strPropHelp = "BOM number";
#else 
			lpInfo->m_strPropName = "ͳ��ϵ��";
			lpInfo->m_strPropHelp = "ͳ��ϵ��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("m_uStatMatCoef");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		//
		if(pFirstLinePart->IsShadowObject())
		{
			//Ӱ����Ϣ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Shadow Info";
			lpInfo->m_strPropHelp = "Shadow information";
#else
			lpInfo->m_strPropName = "Ӱ����Ϣ";
			lpInfo->m_strPropHelp = "Ӱ����Ϣ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("shadowInfo");
			pPropItem->m_bHideChildren = FALSE;
			if(CLDSLinePart::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLinePart::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			pPropItem->SetReadOnly(FALSE);
			//Ӱ��ĸ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Shadow Mother Object";
			lpInfo->m_strPropHelp = "The shadow mother object's handle of currently part,when it's 0x0 means that the currently object is original object";
#else
			lpInfo->m_strPropName = "Ӱ��ĸ����";
			lpInfo->m_strPropHelp = "��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("m_hMotherObj");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstLinePart->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstLinePart->IsBlockRefShadowObject())
			{
				//������������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Belong Block Reference";
				lpInfo->m_strPropHelp = "The block reference's handle currently part belong to.";
#else
				lpInfo->m_strPropName = "������������";
				lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSLinePart::GetPropID("m_hBlockRef");
				if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstLinePart->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else 
		{
			//��������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Belong Block";
			lpInfo->m_strPropHelp = "The block's handle currently part belong to.";
#else
			lpInfo->m_strPropName = "��������";
			lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
			lpInfo->m_cmbItems=GetBlockListStr();
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("m_hBlock");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstLinePart->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
#ifndef __TSA_
		if(!theApp.m_bAnalysisWorkEnv)
		{
			//���Ӹ�����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Weld Parent Part";
			lpInfo->m_strPropHelp = "If the part need to weld to other part, then the property is the handle of the part's dependent welded part;If the part doesn't need to be welded,the value is 0X0.";
#else 
			lpInfo->m_strPropName = "���Ӹ�����";
			lpInfo->m_strPropHelp = "����˹�����Ҫ���ӵ�����������,��ô�����Ծ��Ǳ������������ĺ��ӹ����ľ��;����˹�������Ҫ����,��ֵΪ0X0��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("m_hPartWeldParent");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//��ʾ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Hole Visible";
			lpInfo->m_strPropHelp = "Whether show the hole";
			lpInfo->m_cmbItems = "Yes|No";
#else 
			lpInfo->m_strPropName = "��ʾ��";
			lpInfo->m_strPropHelp = "�Ƿ���ʾ��";
			lpInfo->m_cmbItems = "��|��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("m_bDisplayHole");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
#endif
#ifndef __TSA_
			//���⸨������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual Auxiliary Part";
		lpInfo->m_strPropHelp = "Virtual auxiliary parts are only used to assist other part's position, is not the real statistic material of parts,so not contributing to the material statistics.";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "���⸨������";
		lpInfo->m_strPropHelp = "���⸨������ֻ���ڸ�������������λ�������ǽṹ����ʵ���е�ͳ�Ĺ��������Բ���Ԥ����ͳ�ơ�";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("m_bVirtualPart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//�û�ָ��������ɫ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specified Part Color";
		lpInfo->m_strPropHelp = "Whether the part's color is specified by the user.";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "ָ��������ɫ";
		lpInfo->m_strPropHelp = "������ɫ�Ƿ�Ϊ�û�ָ��";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstLinePart->m_bUserSpecColor)
		{	//������ɫ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Part Color";
			lpInfo->m_strPropHelp = "Part's color";
#else 
			lpInfo->m_strPropName = "������ɫ";
			lpInfo->m_strPropHelp = "������ɫ";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
			//��ע
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Notes";
		lpInfo->m_strPropHelp = "Notes"; 
#else 
		lpInfo->m_strPropName = "��ע";
		lpInfo->m_strPropHelp = "��ע"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("sNotes");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
#endif
		//
		//������Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Rod Tip Node Information";
		lpInfo->m_strPropHelp = "The tip node information of rod.";
#else 
		lpInfo->m_strPropName = "��ͷ��Ϣ";
		lpInfo->m_strPropHelp = "ֱ���͸˼����Ӷ�ͷ��Ϣ";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLinePart::GetPropID("nodeInfo");
		if(CLDSLinePart::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLinePart::GetPropStatus(pGroupItem->m_idProp);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Node";
		lpInfo->m_strPropHelp = "The handle of start node";
#else 
		lpInfo->m_strPropName = "ʼ�˽ڵ�";
		lpInfo->m_strPropHelp = "ʼ�˽ڵ���";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("hStart");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Node";
		lpInfo->m_strPropHelp = "The handle of end node";
#else 
		lpInfo->m_strPropName = "�ն˽ڵ�";
		lpInfo->m_strPropHelp = "�ն˽ڵ���";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("hEnd");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
#ifndef __TSA_
		if(!theApp.m_bAnalysisWorkEnv)
		{
			//ʼ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Start Odd Length";
			lpInfo->m_strPropHelp = "The start odd length of rod.";
#else 
			lpInfo->m_strPropName = "ʼ��";
			lpInfo->m_strPropHelp = "�˼�ʼ������ͷ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("oddStart");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//�ն�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "End Odd Length";
			lpInfo->m_strPropHelp = "The end odd length of rod.";
#else 
			lpInfo->m_strPropName = "�ն�";
			lpInfo->m_strPropHelp = "�˼��ն�����ͷ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("oddEnd");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		}
#endif
#if defined(__TSA_)||defined(__LDS_)
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ʼ��λ��";
		lpInfo->m_strPropHelp = "ʼ��λ��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("startInfo");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_bHideChildren=FALSE;
		if(CLDSLinePart::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pPropItem->m_idProp);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "���X����";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("startPos.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "���Y����";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("startPos.y");
		if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "���Z����";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("startPos.z");
		if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�ն�λ��";
		lpInfo->m_strPropHelp = "�ն�λ��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("endInfo");
		pPropItem->m_bHideChildren=FALSE;
		if(CLDSLinePart::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pPropItem->m_idProp);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "�յ�X����";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("endPos.x");
		if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "�յ�Y����";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("endPos.y");
		if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "�յ�Z����";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("endPos.z");
		if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->SetReadOnly();
		//�����Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_STATIC;
		lpInfo->m_strPropName = "�����Ϣ";
		lpInfo->m_strPropHelp = "�����Ϣ";
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1,TRUE);
		pGroupItem->m_idProp = CLDSLinePart::GetPropID("designInfo");
		if(CLDSLinePart::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�����Ŵ�ϵ��";
		lpInfo->m_strPropHelp = "�����Ŵ�ϵ��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fWeightRaiseCoef");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		/*lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�Ƿ���в�����ѡ";
		lpInfo->m_strPropHelp = "�Ƿ���в�����ѡ";
		lpInfo->m_cmbItems = "��|��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("m_bNeedSelMat");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);*/
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�Ƿ���й����ѡ";
		lpInfo->m_strPropHelp = "�Ƿ���й����ѡ";
		lpInfo->m_cmbItems = "��|��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("m_bNeedSelSize");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��Ԫ����";
		lpInfo->m_strPropHelp = "��Ԫ����";
		lpInfo->m_cmbItems = "0.�Զ��ж�|1.������|2.������|3.������Ԫ";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("m_iElemType");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstLinePart->size_idClassType==CLS_CABLE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		if(pFirstLinePart->IsAuxPole())//||pFirstLinePart->IsPrimaryRod(true))
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_strPropName = "����֧��������";
			lpInfo->m_strPropHelp = "���ĶԶ�ͷ�˼�֧�����ļ�������";
			if(pFirstLinePart->IsAuxPole())
				lpInfo->m_cmbItems ="�Զ����|����ʼ��|�����ն�|����б��";
			else
				lpInfo->m_cmbItems ="�Զ����|����ʼ��|�����ն�|������";
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("sel_mat_method");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
			//
			/*if(pFirstLineAngle->sel_mat_method!=AUX_AUTODESIGN)
			{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "������֧����ϵ��";
			lpInfo->m_strPropHelp = "������֧����ϵ��";
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CLDSLinePart::GetPropID("bear_load_coef");
			if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
			}*/
		}
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�Զ�������˨������";
		lpInfo->m_strPropHelp = "�Զ�������˨������";
		lpInfo->m_cmbItems = "��|��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("m_bAutoCalHoleOutCoef");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_bHideChildren=TRUE;	//Ĭ�ϲ���ʾ�������ʼǱ���һ����ʾ���� wjh-2015.3.12
		if(CLDSLinePart::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��˨������";
		lpInfo->m_strPropHelp = "��˨������";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("hole_out");
		if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pSonPropItem->SetReadOnly(pFirstLinePart->m_bAutoCalHoleOutCoef);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//���㳤����Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "���㳤������";
		lpInfo->m_strPropHelp = "���㳤������";
		lpInfo->m_cmbItems = "0.�����ж�|1.Vб��(��Ų�����)|2.Vб��(V������)|3.Vб��3�ڼ��������|4.Vб��4�ڼ��������|5.ָ���˼����㳤��|6.ָ������ļ��㳤��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.iTypeNo");
		if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_bHideChildren=TRUE;	//Ĭ�ϲ���ʾ�������ʼǱ���һ����ʾ���� wjh-2015.3.12
		if(CLDSLinePart::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		if(pFirstLinePart->CalLenCoef.iTypeNo==0||pFirstLinePart->CalLenCoef.iTypeNo==6)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "ͬʱ��ѹ���ո˼�";
			lpInfo->m_strPropHelp = "ͬʱ��ѹ���ո˼�";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.pRefPole");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		//
		if(pFirstLinePart->CalLenCoef.iTypeNo==3||pFirstLinePart->CalLenCoef.iTypeNo==3)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "���պ�����";
			lpInfo->m_strPropHelp = "���㳤�Ȳ��պ�����";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.pHorizPole");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		//
		if(pFirstLinePart->CalLenCoef.iTypeNo==5||pFirstLinePart->CalLenCoef.iTypeNo==6)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "��С�᳤��";
			lpInfo->m_strPropHelp = "��С�᳤��,��λ(mm)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.spec_callenMinR");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_bHideChildren=TRUE;
			if(CLDSLinePart::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "��ʼ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ʼ�ڵ���";
			CPropTreeItem* pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.minR.hStartNode");
			if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "��ֹ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ֹ�ڵ���";
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.minR.hEndNode");
			if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ϵ��";
			lpInfo->m_strPropHelp = "���㳤�ȵ�����ϵ��";
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.minR.coef");
			if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "ƽ���᳤��";
			lpInfo->m_strPropHelp = "ƽ���᳤��,��λ(mm)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.spec_callenParaR");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_bHideChildren=TRUE;
			if(CLDSLinePart::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "��ʼ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ʼ�ڵ���";
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.paraR.hStartNode");
			if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "��ֹ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ֹ�ڵ���";
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.paraR.hEndNode");
			if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ϵ��";
			lpInfo->m_strPropHelp = "���㳤�ȵ�����ϵ��";
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLinePart::GetPropID("CalLenCoef.paraR.coef");
			if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		}
		//ʼ����Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ʼ����Ϣ";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("designStartInfo");
		if(CLDSLinePart::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "ʼ��Լ������";
		lpInfo->m_strPropHelp = "ʼ��Լ������";
		lpInfo->m_cmbItems = "�½�|�ս�|�Զ��ж�";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("start_joint_type");
		if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "ʼ����������";
		lpInfo->m_strPropHelp = "ʼ����������";
		lpInfo->m_cmbItems = "��������|ƫ������";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("start_force_type");
		if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ʼ������";
		lpInfo->m_strPropHelp = "ʼ��������Ϣ";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("connectStart");
		if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_bHideChildren=FALSE;
		if(CLDSLinePart::GetPropStatus(pSonPropItem->m_idProp)!=-1)
			pSonPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��˨����";
		lpInfo->m_strPropHelp = "��˨����";
		lpInfo->m_cmbItems = "4.8|5.8|6.8|8.8";
		CPropTreeItem *pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
		pLeafItem->m_idProp = CLDSLinePart::GetPropID("connectStart.grade");
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��˨ֱ��";
		lpInfo->m_strPropHelp = "��˨ֱ��";
		lpInfo->m_cmbItems = "M12|M16|M20|M24";
		pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
		pLeafItem->m_idProp = CLDSLinePart::GetPropID("connectStart.d");
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��˨����";
		lpInfo->m_strPropHelp = "��˨����";
		pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
		pLeafItem->m_idProp = CLDSLinePart::GetPropID("connectStart.N");
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//ʼ�����������������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�������(kN)";
		lpInfo->m_strPropHelp = "�����������,kN";
		pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
		pLeafItem->m_idProp = CLDSLinePart::GetPropID("connectStart.maxTension");
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//ʼ�������������ѹ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "���ѹ��(kN)";
		lpInfo->m_strPropHelp = "�������ѹ��,kN";
		pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
		pLeafItem->m_idProp = CLDSLinePart::GetPropID("connectStart.maxCompression");
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//�ն���Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�ն���Ϣ";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLinePart::GetPropID("designEndInfo");
		if(CLDSLinePart::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�ն�Լ������";
		lpInfo->m_strPropHelp = "�ն�Լ������";
		lpInfo->m_cmbItems = "�½�|�ս�|�Զ��ж�";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("end_joint_type");
		if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�ն���������";
		lpInfo->m_strPropHelp = "�ն���������";
		lpInfo->m_cmbItems = "��������|ƫ������";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("end_force_type");
		if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�ն�����";
		lpInfo->m_strPropHelp = "�ն�������Ϣ";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CLDSLinePart::GetPropID("connectEnd");
		pSonPropItem->m_bHideChildren=FALSE;
		if(CLDSLinePart::GetPropStatus(pSonPropItem->m_idProp)!=-1)
			pSonPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��˨����";
		lpInfo->m_strPropHelp = "��˨����";
		lpInfo->m_cmbItems = "4.8|5.8|6.8|8.8";
		pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
		pLeafItem->m_idProp = CLDSLinePart::GetPropID("connectEnd.grade");
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��˨ֱ��";
		lpInfo->m_strPropHelp = "��˨ֱ��";
		lpInfo->m_cmbItems = "M12|M16|M20|M24";
		pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
		pLeafItem->m_idProp = CLDSLinePart::GetPropID("connectEnd.d");
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��˨����";
		lpInfo->m_strPropHelp = "��˨����";
		pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
		pLeafItem->m_idProp = CLDSLinePart::GetPropID("connectEnd.N");
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//�ն����������������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�������(kN)";
		lpInfo->m_strPropHelp = "�����������,kN";
		pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
		pLeafItem->m_idProp = CLDSLinePart::GetPropID("connectEnd.maxTension");
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
		//�ն������������ѹ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "���ѹ��(kN)";
		lpInfo->m_strPropHelp = "�������ѹ��,kN";
		pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
		pLeafItem->m_idProp = CLDSLinePart::GetPropID("connectEnd.maxCompression");
		if(pFirstLinePart->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_DESIGNINFO);
#endif
	}
	else
	{
		SetPartMutiObjsProp(pFirstLinePart,"handle");
		SetPartMutiObjsProp(pFirstLinePart,"layer");
		SetPartMutiObjsProp(pFirstLinePart,"cMaterial");
		SetPartMutiObjsProp(pFirstLinePart,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstLinePart,"m_uStatMatNo");
		SetPartMutiObjsProp(pFirstLinePart,"iSeg");
		SetPartMutiObjsProp(pFirstLinePart,"lengthSingleLine");
		SetPartMutiObjsProp(pFirstLinePart,"cfgword");
		CPropTreeItem *pParentItem;
#if defined(__LDS_)||defined(__TSA_)
		if(pFirstLinePart->size_idClassType==CLS_CABLE)	//�û��������
		{
			pParentItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("specification"),NULL);
			if(pParentItem==NULL)
			{	//ԭ��ʾ�˼������������ͣ�����ͨ�����Ŀ
				SetPartMutiObjsProp(pFirstLinePart,"m_sWireType");
				SetPartMutiObjsProp(pFirstLinePart,"m_sWireSpec");
				SetPartMutiObjsProp(pFirstLinePart,"m_fWireArea");
				SetPartMutiObjsProp(pFirstLinePart,"m_fElasticCoef");
				SetPartMutiObjsProp(pFirstLinePart,"m_fForcePermit");
				SetPartMutiObjsProp(pFirstLinePart,"m_fExpansionCoef");
				SetPartMutiObjsProp(pFirstLinePart,"m_fPreTension");
				SetPartMutiObjsProp(pFirstLinePart,"m_fUnitWeight");
			}
			else
			{	//ԭ��ʾ�˼�Ϊ��ͨ�˼�����Ҫ�任��������ʾģʽ
				//����Զ��������͸˼���������ʾ��Ŀ
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.size"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.area"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.E"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.fc"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.ft"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.pre_stress"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.unit_weight"));
				//�ı�˼����������ĿΪ��������������Ŀ
				pParentItem->m_lpNodeInfo->m_controlType = PIT_COMBO;
				pParentItem->m_lpNodeInfo->m_strPropName = "��������";
				pParentItem->m_lpNodeInfo->m_strPropHelp = "��������";
				pParentItem->m_lpNodeInfo->m_cmbItems=MakeWireTypeSetString();
				pParentItem->m_lpNodeInfo->m_strPropValue=pFirstLinePart->GetWireType();
				pParentItem->m_idProp = CLDSLinePart::GetPropID("m_sWireType");
				if(CLDSLinePart::GetPropStatus(pParentItem->m_idProp)!=-1)
					pParentItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pParentItem->m_idProp);
				else
					pParentItem->m_bHideChildren=FALSE;

				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_strPropName = "���¹��";
				lpInfo->m_strPropHelp = "���¾������ͺ�";
				lpInfo->m_cmbItems=MakeWireSetString(pFirstLinePart->GetWireType());
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_sWireSpec");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "�����";
				lpInfo->m_strPropHelp = "���½����,��λ(mm2)";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fWireArea");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->SetReadOnly();
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "����ϵ��(MPa)";
				lpInfo->m_strPropHelp = "���µ���ϵ��(ģ��),��λ(MPa)";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fElasticCoef");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->SetReadOnly();
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "�¶�������ϵ��";
				lpInfo->m_strPropHelp = "�¶�������ϵ��,��λ�����������¶�����1��Cʱ�ĳ���������";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fExpansionCoef");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->SetReadOnly();
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��λ����(kg/km)";
				lpInfo->m_strPropHelp = "��λ�������µ�����,��λ(kg/km)";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fUnitWeight");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->SetReadOnly();
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "�����ǿ��";
				lpInfo->m_strPropHelp = "�����ǿ��,kN";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fForcePermit");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->SetReadOnly();
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Ԥ�Ž���";
				lpInfo->m_strPropHelp = "��װʱԤ�Ž���,kN";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fPreTension");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			}
		}
		else
		{
			pParentItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("m_sWireType"),NULL);
			if(pParentItem)
			{	//ԭ��ʾ�˼����������߸˼���Ҫת��Ϊ��ͨ�˼������ʾģʽ
				pParentItem->m_lpNodeInfo->m_strPropName = "�˼����";
				pParentItem->m_lpNodeInfo->m_strPropHelp = "�˼����";
				pParentItem->m_idProp = CLDSLinePart::GetPropID("specification");
				//���ԭ���߸˼����������ʾ��Ŀ
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_sWireSpec"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_fWireArea"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_fElasticCoef"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_fForcePermit"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_fExpansionCoef"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_fPreTension"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_fUnitWeight"));
			}
			else//ԭ��ʾ�˼�����������ͨ�˼�(�����߸˼�)
				pParentItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("specification"),NULL);
			SetPartMutiObjsProp(pFirstLinePart,"specification");
			if(pFirstLinePart->size_idClassType==0)	//�û��������
			{
				pParentItem->m_lpNodeInfo->m_controlType=PIT_EDIT;
				
				long idProp=CLDSLinePart::GetPropID("userdef.size");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "�Զ�����";
					lpInfo->m_strPropHelp = "�Զ���˼�������";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.size");
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				
				idProp=CLDSLinePart::GetPropID("userdef.area");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "�����";
					lpInfo->m_strPropHelp = "�˼������,��λ(mm2)";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.area");
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;

				idProp=CLDSLinePart::GetPropID("userdef.E");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "����ģ��";
					lpInfo->m_strPropHelp = "�˼�����������ģ��,��λ(MPa)";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = idProp;
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;

				idProp=CLDSLinePart::GetPropID("userdef.fc");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "��ѹǿ�����ֵ";
					lpInfo->m_strPropHelp = "��ѹǿ�����ֵ,��λ(MPa)";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = idProp;
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;

				idProp=CLDSLinePart::GetPropID("userdef.ft");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "����ǿ�����ֵ";
					lpInfo->m_strPropHelp = "����ǿ�����ֵ,��λ(MPa)";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = idProp;
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;

				idProp=CLDSLinePart::GetPropID("userdef.pre_stress");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "ԤӦ��";
					lpInfo->m_strPropHelp = "��װʱԤ��Ӧ��ֵ,��λ(Pa)";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = idProp;
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;

				idProp=CLDSLinePart::GetPropID("userdef.unit_weight");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "��λ����";
					lpInfo->m_strPropHelp = "��λ���ȸ˼�����,��λ(kg/m)";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = idProp;
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			else
			{
				pParentItem->m_lpNodeInfo->m_controlType=PIT_COMBO;
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.size"));	//���û�ָ��ʱɾ���û������� wht 10-11-22
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.area"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.E"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.fc"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.ft"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.pre_stress"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("userdef.unit_weight"));

				if(pFirstLinePart->size_idClassType==CLS_LINEANGLE)
					pParentItem->m_lpNodeInfo->m_cmbItems=MakeAngleSetString();
				else if(pFirstLinePart->size_idClassType==CLS_GROUPLINEANGLE)
					pParentItem->m_lpNodeInfo->m_cmbItems=MakeAngleSetString(pFirstLinePart->size_cSubClassType);
				else if(pFirstLinePart->size_idClassType==CLS_LINETUBE)
					pParentItem->m_lpNodeInfo->m_cmbItems=MakeTubeSetString();
				else if(pFirstLinePart->size_idClassType==CLS_LINESLOT)
					pParentItem->m_lpNodeInfo->m_cmbItems=MakeSlotSetString();
				else if(pFirstLinePart->size_idClassType==CLS_LINEFLAT)
					pParentItem->m_lpNodeInfo->m_cmbItems=MakeFlatSetString();
				else if(pFirstLinePart->size_idClassType==CLS_CABLE)
					pParentItem->m_lpNodeInfo->m_cmbItems=MakeFlatSetString();
			}
		}

		SetPartMutiObjsProp(pFirstLinePart,"size.idClassType");
		//�����Ϣ
		int idProp=0;
		SetPartMutiObjsProp(pFirstLinePart,"m_fWeightRaiseCoef");
		SetPartMutiObjsProp(pFirstLinePart,"m_bNeedSelSize");
		//SetPartMutiObjsProp(pFirstLineAngle,"m_bNeedSelMat");
		SetPartMutiObjsProp(pFirstLinePart,"m_iElemType");
		pPropItem = pPropList->FindItemByPropId(CLDSLinePart::GetPropID("m_iElemType"),&pParentItem);
		if(pPropItem)
			pPropItem->SetReadOnly(pFirstLinePart->size_idClassType==CLS_CABLE);
		SetPartMutiObjsProp(pFirstLinePart,"m_bAutoCalHoleOutCoef");
		pPropItem = pPropList->FindItemByPropId(CLDSLinePart::GetPropID("hole_out"),&pParentItem);
		if(pPropItem)
			pPropItem->SetReadOnly(pFirstLinePart->m_bAutoCalHoleOutCoef);
		SetPartMutiObjsProp(pFirstLinePart,"hole_out");
		SetPartMutiObjsProp(pFirstLinePart,"CalLenCoef.iTypeNo");
		UpdateLinePartCalLenCoefPropItem(pFirstLinePart,pPropList,this);
		//���¸���������ʾ��Ŀ��Ϣ
		UpdateAuxLinePartPropItem(pFirstLinePart,pPropList,this);
		//���³�ϸ�����������ʾ��Ŀ��Ϣ
		UpdateLinePartCalLenCoefPropItem(pFirstLinePart,pPropList,this);
		SetPartMutiObjsProp(pFirstLinePart,"start_joint_type");
		SetPartMutiObjsProp(pFirstLinePart,"start_force_type");
		SetPartMutiObjsProp(pFirstLinePart,"connectStart.grade");
		SetPartMutiObjsProp(pFirstLinePart,"connectStart.d");
		SetPartMutiObjsProp(pFirstLinePart,"connectStart.N");
		SetPartMutiObjsProp(pFirstLinePart,"connectStart.maxTension");
		SetPartMutiObjsProp(pFirstLinePart,"connectStart.maxCompression");
		SetPartMutiObjsProp(pFirstLinePart,"end_joint_type");
		SetPartMutiObjsProp(pFirstLinePart,"end_force_type");
		SetPartMutiObjsProp(pFirstLinePart,"connectEnd.grade");
		SetPartMutiObjsProp(pFirstLinePart,"connectEnd.d");
		SetPartMutiObjsProp(pFirstLinePart,"connectEnd.N");
		SetPartMutiObjsProp(pFirstLinePart,"connectEnd.maxTension");
		SetPartMutiObjsProp(pFirstLinePart,"connectEnd.maxCompression");
#endif
		CPropTreeItem *pFindItem;
		pFindItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("m_hBlock"),&pParentItem);
		if (pFindItem)
		{
			if (pFirstLinePart->IsShadowObject())
			{	//����ͨ�����л���Ӱ�乹��
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_hBlock"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("relativeObjs"));
				//Ӱ����Ϣ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Shadow Info";
				lpInfo->m_strPropHelp = "Shadow information";
#else
				lpInfo->m_strPropName = "Ӱ����Ϣ";
				lpInfo->m_strPropHelp = "Ӱ����Ϣ";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,10,true);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("shadowInfo");
				if(CLDSLinePart::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSLinePart::GetPropStatus(pPropItem->m_idProp);
				pPropItem->SetReadOnly(FALSE);
				//Ӱ��ĸ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Shadow Mother Object";
				lpInfo->m_strPropHelp = "The shadow mother object's handle of currently part,when it's 0x0 means that the currently object is original object";
#else
				lpInfo->m_strPropName = "Ӱ��ĸ����";
				lpInfo->m_strPropHelp = "��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSLinePart::GetPropID("m_hMotherObj");
				if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstLinePart->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				if (pFirstLinePart->IsBlockRefShadowObject())
				{
					//������������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Belong Block Reference";
					lpInfo->m_strPropHelp = "The block reference's handle currently part belong to.";
#else
					lpInfo->m_strPropName = "������������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSLinePart::GetPropID("m_hBlockRef");
					if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstLinePart->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstLinePart,"m_hBlock");
				SetPartMutiObjsProp(pFirstLinePart,"relativeObjs");
			}
		}
		pFindItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("shadowInfo"),&pParentItem);
		if (pFindItem)
		{
			if (!pFirstLinePart->IsShadowObject())
			{	//��Ӱ�乹���л�����ͨ����
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("shadowInfo"));
				pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_hMotherObj"));
				pFindItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("m_hBlockRef"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_hBlockRef"));
				pFindItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("m_hBlock"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_hBlock"));
				if(pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstLinePart,"m_hBlock");
				else
				{	//��������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Handle";
					lpInfo->m_strPropHelp = "The handle of the vest in block��";
#else 
					lpInfo->m_strPropName = "��������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSLinePart::GetPropID("m_hBlock");
					if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
					pPropItem->SetReadOnly(pFirstLinePart->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
				pFindItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("relativeObjs"),NULL);
				if(pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstLinePart,"relativeObjs");
				else
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Relative Object";
					lpInfo->m_strPropHelp = "The object relate to currently object and mirror style";
#else
					lpInfo->m_strPropName = "��������";
					lpInfo->m_strPropHelp = "�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,10,true);
					pPropItem->m_idProp = CLDSLinePart::GetPropID("relativeObjs");
					if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstLinePart,"m_hMotherObj");
				pFindItem=pPropList->FindItemByPropId(CLDSLinePart::GetPropID("m_hBlockRef"),&pParentItem);
				if (pFindItem&&pFirstLinePart->IsBlockRefShadowObject())
				{
					SetPartMutiObjsProp(pFirstLinePart,"m_hBlockRef");
					SetPartMutiObjsProp(pFirstLinePart,"m_hBlock");
				}
				else if (pFindItem&&!pFirstLinePart->IsBlockRefShadowObject())
				{
					pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_hBlockRef"));
					pPropList->DeleteItemByPropId(CLDSLinePart::GetPropID("m_hBlock"));
				}
				else if(pFindItem==NULL&&pFirstLinePart->IsBlockRefShadowObject())
				{
					pPropList->FindItemByPropId(CLDSLinePart::GetPropID("m_hMotherObj"),&pParentItem);
					//������������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Belong Block Reference";
					lpInfo->m_strPropHelp = "The block reference's handle currently part belong to.";
#else
					lpInfo->m_strPropName = "������������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
					pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSLinePart::GetPropID("m_hBlockRef");
					if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstLinePart->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
		}
		SetPartMutiObjsProp(pFirstLinePart,"hStart");
		SetPartMutiObjsProp(pFirstLinePart,"startPos.x");
		SetPartMutiObjsProp(pFirstLinePart,"startPos.y");
		SetPartMutiObjsProp(pFirstLinePart,"startPos.z");
		SetPartMutiObjsProp(pFirstLinePart,"hEnd");
		SetPartMutiObjsProp(pFirstLinePart,"endPos.x");
		SetPartMutiObjsProp(pFirstLinePart,"endPos.y");
		SetPartMutiObjsProp(pFirstLinePart,"endPos.z");
	}
	
	pPropList->Redraw();
}
#ifdef __PART_DESIGN_INC_
void UpdateSpecLoSection(CPropertyList *pPropList, CPropTreeItem* pItem,bool bSpecSection,
						 long start_node_prop_id,long end_node_prop_id)
{
	const int GROUP_GENERAL=1,GROUP_LAMDAINFO=2;
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	CPropTreeItem *pLeafItem,*pParentItem;
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CElemInfo *pFirstElemInfo = (CElemInfo*)pView->selectObjs.GetFirst();
	if(bSpecSection)
	{
		pLeafItem =pPropList->FindItemByPropId(start_node_prop_id,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "��ʼ�ڵ��";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ʼ�ڵ���";
			pLeafItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = start_node_prop_id;
		}
		if(pFirstElemInfo->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		pLeafItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);

		pLeafItem =pPropList->FindItemByPropId(end_node_prop_id,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "��ֹ�ڵ��";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ֹ�ڵ���";
			pLeafItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = end_node_prop_id;
		}
		if(pFirstElemInfo->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		pLeafItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
	}
	else
	{
		pPropList->DeleteItemByPropId(start_node_prop_id);
		pPropList->DeleteItemByPropId(end_node_prop_id);
	}
}

void UpdateLoRyoInfo(CPropertyList *pPropList, CPropTreeItem* pItem,int para)
{
	long idProp;
	const int GROUP_GENERAL=1,GROUP_LAMDAINFO=2;
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	CPropTreeItem *pPropItem,*pParentItem;
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CElemInfo *pFirstElemInfo = (CElemInfo*)pView->selectObjs.GetFirst();
	if(para==0)
	{
		pPropList->DeleteItemByPropId(CElemInfo::GetPropID("LoRyo.CalType"));
		pPropList->DeleteItemByPropId(CElemInfo::GetPropID("LoRyo.hStartNode"));
		pPropList->DeleteItemByPropId(CElemInfo::GetPropID("LoRyo.hEndNode"));
		pPropList->DeleteItemByPropId(CElemInfo::GetPropID("LoRyo.coef"));
		pPropList->DeleteItemByPropId(CElemInfo::GetPropID("LoRyo"));
	}
	else
	{
		idProp=CElemInfo::GetPropID("LoRyo.CalType");
		pPropItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("LoRyo.CalType"),&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_strPropName = "��׼��������";
			lpInfo->m_strPropHelp = "���㳤��=��׼����*����ϵ��";
			lpInfo->m_cmbItems = "0.��Ԫ����|1.ָ���ڼ�";
			pPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = idProp;
		}
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());
		pPropItem->m_bHideChildren=FALSE;
		if(CElemInfo::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CElemInfo::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		if(pFirstElemInfo->LoRyoCalType()==1)
			UpdateSpecLoSection(pPropList,pPropItem,true,CElemInfo::GetPropID("LoRyo.hStartNode"),CElemInfo::GetPropID("LoRyo.hEndNode"));
		else
			UpdateSpecLoSection(pPropList,pPropItem,false,CElemInfo::GetPropID("LoRyo.hStartNode"),CElemInfo::GetPropID("LoRyo.hEndNode"));

		idProp=CElemInfo::GetPropID("LoRyo.coef");
		pPropItem =pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem ==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ϵ��";
			lpInfo->m_strPropHelp = "���㳤�ȵ�����ϵ��";
			pPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = idProp;
		}
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);

		idProp=CElemInfo::GetPropID("LoRyo");
		pPropItem =pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem ==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "���㳤��";
			lpInfo->m_strPropHelp = "��С����㳤��,��λ(mm)";
			pPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = idProp;
		}
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		pPropItem->SetReadOnly();
	}
}
void UpdateLoRxInfo(CPropertyList *pPropList, CPropTreeItem* pItem,int para)
{
	long idProp;
	const int GROUP_GENERAL=1,GROUP_LAMDAINFO=2;
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	CPropTreeItem *pPropItem,*pParentItem;
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CElemInfo *pFirstElemInfo = (CElemInfo*)pView->selectObjs.GetFirst();
	if(para==0)
	{
		pPropList->DeleteItemByPropId(CElemInfo::GetPropID("LoRx.CalType"));
		pPropList->DeleteItemByPropId(CElemInfo::GetPropID("LoRx.hStartNode"));
		pPropList->DeleteItemByPropId(CElemInfo::GetPropID("LoRx.hEndNode"));
		pPropList->DeleteItemByPropId(CElemInfo::GetPropID("LoRx.coef"));
		pPropList->DeleteItemByPropId(CElemInfo::GetPropID("LoRx"));
	}
	else
	{
		idProp=CElemInfo::GetPropID("LoRx.CalType");
		pPropItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("LoRx.CalType"),&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_strPropName = "��׼��������";
			lpInfo->m_strPropHelp = "���㳤��=��׼����*����ϵ��";
			lpInfo->m_cmbItems = "0.��Ԫ����|1.ָ���ڼ�";
			pPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = idProp;
		}
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());
		pPropItem->m_bHideChildren=FALSE;
		if(CElemInfo::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CElemInfo::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		if(pFirstElemInfo->LoRxCalType()==1)
			UpdateSpecLoSection(pPropList,pPropItem,true,CElemInfo::GetPropID("LoRx.hStartNode"),CElemInfo::GetPropID("LoRx.hEndNode"));
		else
			UpdateSpecLoSection(pPropList,pPropItem,false,CElemInfo::GetPropID("LoRx.hStartNode"),CElemInfo::GetPropID("LoRx.hEndNode"));

		idProp=CElemInfo::GetPropID("LoRx.coef");
		pPropItem =pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem ==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ϵ��";
			lpInfo->m_strPropHelp = "���㳤�ȵ�����ϵ��";
			pPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = idProp;
		}
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);

		idProp=CElemInfo::GetPropID("LoRx");
		pPropItem =pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem ==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "���㳤��";
			lpInfo->m_strPropHelp = "ƽ������㳤��,��λ(mm)";
			pPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = idProp;
		}
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		pPropItem->SetReadOnly();
	}
}
void UpdateLoRxcInfo(CPropertyList *pPropList, CPropTreeItem* pItem)
{
	const int GROUP_GENERAL=1,GROUP_LAMDAINFO=2;
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	CPropTreeItem *pPropItem=NULL,*pGroupItem=pItem;
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CElemInfo *pFirstElemInfo = (CElemInfo*)pView->selectObjs.GetFirst();
	pPropList->DeleteAllSonItems(pItem);
	int scmItemsNum,smcItemsTag;
	scmItemsNum=pFirstElemInfo->GetSmcItemsNum(&smcItemsTag);
	if(scmItemsNum==0)
		return;
	int i=0;
	char tagStr[50]="",propStr[50]="",startNodeStr[50]="",endNodeStr[50]="";
	for(i=0;i<scmItemsNum;i++)
	{
		if(scmItemsNum==1)	//����
			sprintf(tagStr,"smcItem%d",smcItemsTag);
		else if(scmItemsNum==2)	//˫��
		{
			if(smcItemsTag==3)	//��һ��͵ڶ���
				strcpy(tagStr,"smcItem1,smcItem2");
			else if(smcItemsTag==4)	//��һ��͵�����
				strcpy(tagStr,"smcItem1,smcItem3");
			else if(smcItemsTag==5)	//�ڶ���͵�����
				strcpy(tagStr,"smcItem2,smcItem3");
		}
		else //����
			strcpy(tagStr,"smcItem1,smcItem2,smcItem3");
	}
	i=0;
	for(char* key=strtok(tagStr,", ");key;key=strtok(NULL,", "))
	{
		if(scmItemsNum>1)
		{
			if(i==0)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��һ����ѹ��Ϣ";
				pGroupItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pGroupItem->m_idProp = CElemInfo::GetPropID(key);
				pGroupItem->SetReadOnly();
				pGroupItem->m_bHideChildren=FALSE;
				if(CElemInfo::GetPropStatus(pGroupItem->m_idProp)!=-1)
					pGroupItem->m_bHideChildren=CElemInfo::GetPropStatus(pPropItem->m_idProp);
				pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
			}
			if(i==1)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "�ڶ�����ѹ��Ϣ";
				pGroupItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pGroupItem->m_idProp = CElemInfo::GetPropID(key);
				pGroupItem->SetReadOnly();
				pGroupItem->m_bHideChildren=FALSE;
				if(CElemInfo::GetPropStatus(pGroupItem->m_idProp)!=-1)
					pGroupItem->m_bHideChildren=CElemInfo::GetPropStatus(pPropItem->m_idProp);
				pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
			}
			if(i==2)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��������ѹ��Ϣ";
				pGroupItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pGroupItem->m_idProp = CElemInfo::GetPropID(key);
				pGroupItem->SetReadOnly();
				pGroupItem->m_bHideChildren=FALSE;
				if(CElemInfo::GetPropStatus(pGroupItem->m_idProp)!=-1)
					pGroupItem->m_bHideChildren=CElemInfo::GetPropStatus(pPropItem->m_idProp);
				pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
			}
		}
		//��ѹ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��ѹ����";
		lpInfo->m_strPropHelp = "ͬʱ��ѹ����";
		lpInfo->m_cmbItems = "0.X��Գ�|1.Y��Գ�|2.ʼ��ͬ�˵�|3.�ն�ͬ�˵�|4.����ͬ�˵�|5.ָ��ͬʱ��ѹ��Ԫ";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		sprintf(propStr,"%s.cSMCType",key);	
		pPropItem->m_idProp = CElemInfo::GetPropID(propStr);
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		//��ת�뾶
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��ת�뾶";
		lpInfo->m_strPropHelp = "ͬʱ��ѹ��ת�뾶";
		lpInfo->m_cmbItems = "0.ƽ����|1.��С��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		sprintf(propStr,"%s.gyreRadius",key);
		pPropItem->m_idProp = CElemInfo::GetPropID(propStr);
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		//��׼��������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��׼��������";
		lpInfo->m_strPropHelp = "���㳤��=��׼����*����ϵ��";
		lpInfo->m_cmbItems = "0.��Ԫ����|1.ָ���ڼ�";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		sprintf(propStr,"%s.LoRxc.CalType",key);
		pPropItem->m_idProp = CElemInfo::GetPropID(propStr);
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());
		pPropItem->m_bHideChildren=FALSE;
		if(CElemInfo::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CElemInfo::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		//
		sprintf(startNodeStr,"%s.LoRxc.hStartNode",key);
		sprintf(endNodeStr,"%s.LoRxc.hEndNode",key);
		if(pFirstElemInfo->LoCalType(pFirstElemInfo->smcItems[0].LoRc)==1)
			UpdateSpecLoSection(pPropList,pPropItem,true,CElemInfo::GetPropID(startNodeStr),CElemInfo::GetPropID(endNodeStr));
		else
			UpdateSpecLoSection(pPropList,pPropItem,false,CElemInfo::GetPropID(startNodeStr),CElemInfo::GetPropID(endNodeStr));
		//����ϵ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "����ϵ��";
		lpInfo->m_strPropHelp = "���㳤�ȵ�����ϵ��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		sprintf(propStr,"%s.LoRxc.coef",key);
		pPropItem->m_idProp = CElemInfo::GetPropID(propStr);
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		//���㳤��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "���㳤��";
		lpInfo->m_strPropHelp = "ƽ������㳤��,��λ(mm)";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		sprintf(propStr,"%s.LoRxc",key);
		pPropItem->m_idProp = CElemInfo::GetPropID(propStr);
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		pPropItem->SetReadOnly();
		//���յ�Ԫ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
		lpInfo->m_strPropName = "���յ�Ԫ";
		lpInfo->m_strPropHelp = "ͬʱ��ѹ���ո˼�";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		sprintf(propStr,"%s.relaElem",key);
		pPropItem->m_idProp = CElemInfo::GetPropID(propStr);
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		i++;
	}
}
struct ELEM_LABEL{
	CElemInfo* pRelaElemInfo;
	long startPointI,endPointI;
	ELEM_LABEL(){pRelaElemInfo=NULL;startPointI=endPointI=0;}
	ELEM_LABEL(CElemInfo *pElemInfo,long starti,long endi){pRelaElemInfo=pElemInfo;startPointI=starti;endPointI=endi;}
};
BOOL ModifyElementProperty(CPropertyList *pPropList, CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
	CRevisionOperDlg *pRevisionDlg=((CMainFrame*)theApp.m_pMainWnd)->GetRevisionOperPage();
	CElemInfo *pFirstElemInfo = (CElemInfo*)pView->selectObjs.GetFirst();
	CElemInfo *pElemInfo = NULL;
	//int idProp;
	//CItemInfo *lpInfo;
	CPropTreeItem *pFindItem,*pParentItem;
	char sText[100]="";

	const int GROUP_GENERAL=1,GROUP_LAMDAINFO=2;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	if(pItem->m_idProp==CElemInfo::GetPropID("layer"))
	{
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
		{
			CLDSLinePart *pRod=(CLDSLinePart*)console.FromPartHandle(pElemInfo->m_hParentRod,CLS_LINEPART);
			if(pRod==NULL)
				continue;
			pRod->SetLayer(tem_str);	
			for(RELATIVE_OBJECT *pMirObj=pRod->relativeObjs.GetFirst();pMirObj;pMirObj=pRod->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//ͼ�㲻֧����ת�Գ�
				CLDSLinePart *pMirRod=(CLDSLinePart*)console.FromPartHandle(pMirObj->hObj,CLS_LINEPART);
				if(pMirRod)
					CalMirAtomLayer(pRod->layer(),pMirRod->layer(),pMirObj->mirInfo);
			}
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("cMaterial"))
	{
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
		{
			CLDSLinePart *pRod=(CLDSLinePart*)console.FromPartHandle(pElemInfo->m_hParentRod,CLS_LINEPART);
			if(pRod==NULL)
				continue;
			pRod->cMaterial = QuerySteelBriefMatMark(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pRod->relativeObjs.GetFirst();pMirObj;pMirObj=pRod->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//ͼ�㲻֧����ת�Գ�
				CLDSLinePart *pMirRod=(CLDSLinePart*)console.FromPartHandle(pMirObj->hObj,CLS_LINEPART);
				if(pMirRod)
					pMirRod->cMaterial = pRod->cMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("m_uStatMatNo"))
	{
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
		{
			CLDSLinePart *pRod=(CLDSLinePart*)console.FromPartHandle(pElemInfo->m_hParentRod,CLS_LINEPART);
			if(pRod==NULL)
				continue;
			pRod->m_uStatMatNo = atoi(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pRod->relativeObjs.GetFirst();pMirObj;pMirObj=pRod->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//ͼ�㲻֧����ת�Գ�
				CLDSLinePart *pMirRod=(CLDSLinePart*)console.FromPartHandle(pMirObj->hObj,CLS_LINEPART);
				if(pMirRod)
					pMirRod->m_uStatMatNo = pRod->m_uStatMatNo;
			}
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("specification"))
	{
		char sWide[20]={0},sThick[20]={0};
		int i=0,j=0;
		BOOL bWideFinished=FALSE;
		for(i=0;i<valueStr.GetLength();i++)
		{
			if((valueStr[i]>='0'&&valueStr[i]<='9')||valueStr[i]=='.')
			{
				if(!bWideFinished)
					sWide[j]=valueStr[i];
				else
					sThick[j]=valueStr[i];
				j++;
			}
			else
			{
				if(strlen(sWide)>0)
					bWideFinished=TRUE;
				else if(strlen(sThick)>0)
					break;
				j=0;
			}
		}
		BOOL bFirstRod=TRUE;
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
		{
			CLDSLinePart *pRod=(CLDSLinePart*)console.FromPartHandle(pElemInfo->m_hParentRod,CLS_LINEPART);
			if(pRod==NULL)
				continue;
			pRod->size_wide=atof(sWide);
			pRod->size_thick=atof(sThick);
#ifdef __ANGLE_PART_INC_
			if(pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
				((CLDSGroupLineAngle*)pRod)->UpdateSonJgBasicInfo();
#endif
			if(pRod->size_idClassType==CLS_LINEANGLE||pRod->size_idClassType==CLS_GROUPLINEANGLE)
			{
				pRod->connectStart.d=SelBoltDByWingWide(pRod->size_wide,pRod->connectStart.grade);
				pRod->connectEnd.d=SelBoltDByWingWide(pRod->size_wide,pRod->connectEnd.grade);
				if(bFirstRod)
				{
					SetPartMutiObjsProp(pElemInfo,"bolt_type");
					bFirstRod=FALSE;
				}
			}
			for(RELATIVE_OBJECT *pMirObj=pRod->relativeObjs.GetFirst();pMirObj;pMirObj=pRod->relativeObjs.GetNext())
			{
				//if(pMirObj->mirInfo.axis_flag&8)
				//	continue;	//��ת�Գ�	
				//�˼�����֧����ת�Գ� wht 10-11-20
				CLDSPart *pMirPart=console.FromPartHandle(pMirObj->hObj);
				if(pMirPart&&pMirPart->IsLinePart())
				{
					((CLDSLinePart*)pMirPart)->size_wide=pRod->size_wide;
					((CLDSLinePart*)pMirPart)->size_thick=pRod->size_thick;
					if(((CLDSLinePart*)pMirPart)->size_idClassType==CLS_LINEANGLE||((CLDSLinePart*)pMirPart)->size_idClassType==CLS_GROUPLINEANGLE)
					{
						((CLDSLinePart*)pMirPart)->connectStart.d=SelBoltDByWingWide(pRod->size_wide,((CLDSLinePart*)pMirPart)->connectStart.grade);
						((CLDSLinePart*)pMirPart)->connectEnd.d=SelBoltDByWingWide(pRod->size_wide,((CLDSLinePart*)pMirPart)->connectEnd.grade);
					}
#ifdef __ANGLE_PART_INC_
					if(((CLDSLinePart*)pMirPart)->GetClassTypeId()==CLS_GROUPLINEANGLE)
						((CLDSGroupLineAngle*)pMirPart)->UpdateSonJgBasicInfo();
#endif
				}
			}
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("m_bAutoCalHoleOutCoef"))
	{
		BOOL bAutoCal;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else 
		if(valueStr.Compare("��")==0)
#endif
			bAutoCal=TRUE;
		else
			bAutoCal=FALSE;
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
		{
			CLDSLinePart *pRod=(CLDSLinePart*)console.FromPartHandle(pElemInfo->m_hParentRod,CLS_LINEPART);
			if(pRod==NULL)
				continue;
			pRod->m_bAutoCalHoleOutCoef=bAutoCal;
			for(RELATIVE_OBJECT *pMirObj=pRod->relativeObjs.GetFirst();pMirObj;pMirObj=pRod->relativeObjs.GetNext())
			{
				CLDSLinePart *pMirRod=(CLDSLinePart*)console.FromPartHandle(pMirObj->hObj,CLS_LINEPART);
				if(pMirRod)
					pMirRod->m_bAutoCalHoleOutCoef=bAutoCal;
			}
		}
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("hole_out"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(bAutoCal);
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("hole_out"))
	{
		double hole_out=atof(valueStr);
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
		{
			CLDSLinePart *pRod=(CLDSLinePart*)console.FromPartHandle(pElemInfo->m_hParentRod,CLS_LINEPART);
			if(pRod==NULL)
				continue;
			pRod->hole_out=hole_out;
			for(RELATIVE_OBJECT *pMirObj=pRod->relativeObjs.GetFirst();pMirObj;pMirObj=pRod->relativeObjs.GetNext())
			{
				CLDSLinePart *pMirRod=(CLDSLinePart*)console.FromPartHandle(pMirObj->hObj,CLS_LINEPART);
				if(pMirRod)
					pMirRod->hole_out=hole_out;
			}
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("bolt_N"))
	{
		int bolt_n=atoi(valueStr);
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
		{
			CLDSLinePart *pRod=(CLDSLinePart*)console.FromPartHandle(pElemInfo->m_hParentRod,CLS_LINEPART);
			if(pRod==NULL)
				continue;
			if(pElemInfo->m_hStartNode!=pRod->pStart->handle&&pElemInfo->m_hEndNode!=pRod->pEnd->handle)
				continue;
			if(pElemInfo->m_hStartNode==pRod->pStart->handle)
				pRod->connectStart.wnSummBoltN=bolt_n;
			if(pElemInfo->m_hEndNode==pRod->pEnd->handle)
				pRod->connectEnd.wnSummBoltN=bolt_n;
			for(RELATIVE_OBJECT *pMirObj=pRod->relativeObjs.GetFirst();pMirObj;pMirObj=pRod->relativeObjs.GetNext())
			{
				CLDSLinePart *pMirRod=(CLDSLinePart*)console.FromPartHandle(pMirObj->hObj,CLS_LINEPART);
				if(pMirRod)
				{
					if(pElemInfo->m_hStartNode==pRod->pStart->handle)
						pMirRod->connectStart.wnSummBoltN=bolt_n;
					if(pElemInfo->m_hEndNode==pRod->pEnd->handle)
						pMirRod->connectEnd.wnSummBoltN=bolt_n;
				}
			}
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("cCalLenType"))
	{
		bool bSpecLoInfo;
		if(valueStr[0]=='0')	//���ܼ���
			bSpecLoInfo=false;
		else
			bSpecLoInfo=true;
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			pElemInfo->cCalLenType=valueStr[0]-'0';
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("lamdaRyoInfo"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("lamdaRxInfo"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("lamdaRxcInfo"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("LoRyo.CalType"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("LoRx.CalType"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem1.LoRxc.CalType"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem2.LoRxc.CalType"),&pParentItem);
			if(pFindItem)
				pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem3.LoRxc.CalType"),&pParentItem);
			if(pFindItem)
				pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("LoRyo.coef"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("LoRx.coef"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem1.LoRxc.coef"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem2.LoRxc.coef"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem3.LoRxc.coef"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem1.relaElem"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem2.relaElem"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem3.relaElem"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("LoRyo.hStartNode"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo||pFirstElemInfo->LoRyoCalType()!=1);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("LoRx.hStartNode"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo||pFirstElemInfo->LoRxCalType()!=1);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem1.LoRxc.hStartNode"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo||pFirstElemInfo->LoCalType(pFirstElemInfo->smcItems[0].LoRc)!=1);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem2.LoRxc.hStartNode"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo||pFirstElemInfo->LoCalType(pFirstElemInfo->smcItems[1].LoRc)!=1);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem3.LoRxc.hStartNode"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo||pFirstElemInfo->LoCalType(pFirstElemInfo->smcItems[2].LoRc)!=1);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("LoRyo.hEndNode"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo||pFirstElemInfo->LoRyoCalType()!=1);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("LoRx.hEndNode"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo||pFirstElemInfo->LoRxCalType()!=1);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem1.LoRxc.hEndNode"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo||pFirstElemInfo->LoCalType(pFirstElemInfo->smcItems[0].LoRc)!=1);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem2.LoRxc.hEndNode"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo||pFirstElemInfo->LoCalType(pFirstElemInfo->smcItems[1].LoRc)!=1);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem3.LoRxc.hEndNode"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo||pFirstElemInfo->LoCalType(pFirstElemInfo->smcItems[2].LoRc)!=1);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem1.cSMCType"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem1.LoRxc"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem1.gyreRadius"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem2.cSMCType"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem2.LoRxc"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem2.gyreRadius"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem3.cSMCType"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem3.LoRxc"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pFindItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("smcItem3.gyreRadius"),&pParentItem);
		if(pFindItem)
			pFindItem->SetReadOnly(!bSpecLoInfo);
		pPropList->RedrawWindow();
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("lamdaRyoInfo"))
	{
		if(valueStr[0]=='0')//������
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			{
				pElemInfo->LoRyo.hStartNode=-pElemInfo->LoRyo.hStartNode;
				pElemInfo->LoRyo.hEndNode=-pElemInfo->LoRyo.hEndNode;
			}
			UpdateLoRyoInfo(pPropList,pItem,0);
		}
		else
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			{
				if(pElemInfo->LoRyo.hStartNode<0)
					pElemInfo->LoRyo.hStartNode=-pElemInfo->LoRyo.hStartNode;
				if(pElemInfo->LoRyo.hEndNode<0)
					pElemInfo->LoRyo.hEndNode=-pElemInfo->LoRyo.hEndNode;
				CLDSNode* pStart=console.FromNodeHandle(pElemInfo->LoRyo.hStartNode);
				CLDSNode* pEnd=console.FromNodeHandle(pElemInfo->LoRyo.hEndNode);
				if(pStart==NULL||pEnd==NULL)
					pElemInfo->LoRyo.bCalByElemLen=TRUE;
			}
			UpdateLoRyoInfo(pPropList,pItem,1);
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("lamdaRxInfo"))
	{
		if(valueStr[0]=='0')//������
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			{
				pElemInfo->LoRx.hStartNode=-pElemInfo->LoRx.hStartNode;
				pElemInfo->LoRx.hEndNode=-pElemInfo->LoRx.hEndNode;
			}
			UpdateLoRxInfo(pPropList,pItem,0);
		}
		else
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			{
				if(pElemInfo->LoRx.hStartNode<0)
					pElemInfo->LoRx.hStartNode=-pElemInfo->LoRx.hStartNode;
				if(pElemInfo->LoRx.hEndNode<0)
					pElemInfo->LoRx.hEndNode=-pElemInfo->LoRx.hEndNode;
				CLDSNode* pStart=console.FromNodeHandle(pElemInfo->LoRx.hStartNode);
				CLDSNode* pEnd=console.FromNodeHandle(pElemInfo->LoRx.hEndNode);
				if(pStart==NULL||pEnd==NULL)
					pElemInfo->LoRx.bCalByElemLen=TRUE;
			}
			UpdateLoRxInfo(pPropList,pItem,1);
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("lamdaRxcInfo"))
	{
		if(valueStr[0]=='0')//������
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			{
				pElemInfo->smcItems[0].LoRc.hStartNode=-pElemInfo->smcItems[0].LoRc.hStartNode;
				pElemInfo->smcItems[0].LoRc.hEndNode=-pElemInfo->smcItems[0].LoRc.hEndNode;
			}
		}
		else
		{
			CLDSNode *pStart=NULL,*pEnd=NULL;
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			{
				if(pElemInfo->smcItems[0].cSMCType!=SMC_ITEM::SMC_NONE)
				{
					if(pElemInfo->smcItems[0].LoRc.hStartNode<0)
						pElemInfo->smcItems[0].LoRc.hStartNode=-pElemInfo->smcItems[0].LoRc.hStartNode;
					if(pElemInfo->smcItems[0].LoRc.hEndNode<0)
						pElemInfo->smcItems[0].LoRc.hEndNode=-pElemInfo->smcItems[0].LoRc.hEndNode;
					pStart=console.FromNodeHandle(pElemInfo->smcItems[0].LoRc.hStartNode);
					pEnd=console.FromNodeHandle(pElemInfo->smcItems[0].LoRc.hEndNode);
					if(pStart==NULL||pEnd==NULL)
						pElemInfo->smcItems[0].LoRc.bCalByElemLen=TRUE;
				}
				if(pElemInfo->smcItems[1].cSMCType!=SMC_ITEM::SMC_NONE)
				{
					if(pElemInfo->smcItems[1].LoRc.hStartNode<0)
						pElemInfo->smcItems[1].LoRc.hStartNode=-pElemInfo->smcItems[1].LoRc.hStartNode;
					if(pElemInfo->smcItems[1].LoRc.hEndNode<0)
						pElemInfo->smcItems[1].LoRc.hEndNode=-pElemInfo->smcItems[1].LoRc.hEndNode;
					pStart=console.FromNodeHandle(pElemInfo->smcItems[1].LoRc.hStartNode);
					pEnd=console.FromNodeHandle(pElemInfo->smcItems[1].LoRc.hEndNode);
					if(pStart==NULL||pEnd==NULL)
						pElemInfo->smcItems[1].LoRc.bCalByElemLen=TRUE;
				}
				if(pElemInfo->smcItems[2].cSMCType!=SMC_ITEM::SMC_NONE)
				{
					if(pElemInfo->smcItems[2].LoRc.hStartNode<0)
						pElemInfo->smcItems[2].LoRc.hStartNode=-pElemInfo->smcItems[2].LoRc.hStartNode;
					if(pElemInfo->smcItems[2].LoRc.hEndNode<0)
						pElemInfo->smcItems[2].LoRc.hEndNode=-pElemInfo->smcItems[2].LoRc.hEndNode;
					pStart=console.FromNodeHandle(pElemInfo->smcItems[2].LoRc.hStartNode);
					pEnd=console.FromNodeHandle(pElemInfo->smcItems[2].LoRc.hEndNode);
					if(pStart==NULL||pEnd==NULL)
						pElemInfo->smcItems[2].LoRc.bCalByElemLen=TRUE;
				}
			}
		}
		UpdateLoRxcInfo(pPropList,pItem);
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("LoRyo.CalType"))
	{
		if(valueStr[0]=='1')
			UpdateSpecLoSection(pPropList,pItem,true,CElemInfo::GetPropID("LoRyo.hStartNode"),CElemInfo::GetPropID("LoRyo.hEndNode"));
		else
		{	//��׼����Ϊ��Ԫ����
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->LoRyo.bCalByElemLen=TRUE;
			UpdateSpecLoSection(pPropList,pItem,false,CElemInfo::GetPropID("LoRyo.hStartNode"),CElemInfo::GetPropID("LoRyo.hEndNode"));
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("LoRx.CalType"))
	{
		if(valueStr[0]=='1')
			UpdateSpecLoSection(pPropList,pItem,true,CElemInfo::GetPropID("LoRx.hStartNode"),CElemInfo::GetPropID("LoRx.hEndNode"));
		else
		{	//��׼����Ϊ��Ԫ����
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->LoRx.bCalByElemLen=TRUE;
			UpdateSpecLoSection(pPropList,pItem,false,CElemInfo::GetPropID("LoRx.hStartNode"),CElemInfo::GetPropID("LoRx.hEndNode"));
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem1.LoRxc.CalType"))
	{
		if(valueStr[0]=='1')
			UpdateSpecLoSection(pPropList,pItem,true,CElemInfo::GetPropID("smcItem1.LoRxc.hStartNode"),CElemInfo::GetPropID("smcItem1.LoRxc.hEndNode"));
		else
		{	//��׼����Ϊ��Ԫ����
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->smcItems[0].LoRc.bCalByElemLen=TRUE;
			UpdateSpecLoSection(pPropList,pItem,false,CElemInfo::GetPropID("smcItem1.LoRxc.hStartNode"),CElemInfo::GetPropID("smcItem1.LoRxc.hEndNode"));
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem2.LoRxc.CalType"))
	{
		if(valueStr[0]=='1')
			UpdateSpecLoSection(pPropList,pItem,true,CElemInfo::GetPropID("smcItem2.LoRxc.hStartNode"),CElemInfo::GetPropID("smcItem2.LoRxc.hEndNode"));
		else
		{	//��׼����Ϊ��Ԫ����
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->smcItems[1].LoRc.bCalByElemLen=TRUE;
			UpdateSpecLoSection(pPropList,pItem,false,CElemInfo::GetPropID("smcItem2.LoRxc.hStartNode"),CElemInfo::GetPropID("smcItem2.LoRxc.hEndNode"));
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem3.LoRxc.CalType"))
	{
		if(valueStr[0]=='1')
			UpdateSpecLoSection(pPropList,pItem,true,CElemInfo::GetPropID("smcItem3.LoRxc.hStartNode"),CElemInfo::GetPropID("smcItem3.LoRxc.hEndNode"));
		else
		{	//��׼����Ϊ��Ԫ����
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->smcItems[2].LoRc.bCalByElemLen=TRUE;
			UpdateSpecLoSection(pPropList,pItem,false,CElemInfo::GetPropID("smcItem3.LoRxc.hStartNode"),CElemInfo::GetPropID("smcItem3.LoRxc.hEndNode"));
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("LoRyo.coef"))
	{
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			pElemInfo->LoRyo.coef=atof(valueStr);
		SetPartMutiObjsProp(pFirstElemInfo,"LoRyo");
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("LoRx.coef"))
	{
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			pElemInfo->LoRx.coef=atof(valueStr);
		SetPartMutiObjsProp(pFirstElemInfo,"LoRx");
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem1.LoRxc.coef"))
	{
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			pElemInfo->smcItems[0].LoRc.coef=atof(valueStr);
		SetPartMutiObjsProp(pFirstElemInfo,"smcItem1.LoRxc");
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem2.LoRxc.coef"))
	{
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			pElemInfo->smcItems[1].LoRc.coef=atof(valueStr);
		SetPartMutiObjsProp(pFirstElemInfo,"smcItem2.LoRxc");
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem3.LoRxc.coef"))
	{
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			pElemInfo->smcItems[2].LoRc.coef=atof(valueStr);
		SetPartMutiObjsProp(pFirstElemInfo,"smcItem3.LoRxc");
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem1.cSMCType")||
			pItem->m_idProp==CElemInfo::GetPropID("smcItem2.cSMCType")||
			pItem->m_idProp==CElemInfo::GetPropID("smcItem3.cSMCType"))
	{
		int iPara=valueStr[0]-'0';
		BYTE cSMCType;
		if(iPara==0)
			cSMCType=SMC_ITEM::SMC_REFER_MIRX;
		else if(iPara==1)
			cSMCType=SMC_ITEM::SMC_REFER_MIRY;
		else if(iPara==2)
			cSMCType=SMC_ITEM::SMC_REFER_START;
		else if(iPara==3)
			cSMCType=SMC_ITEM::SMC_REFER_END;
		else if(iPara==4)
			cSMCType=SMC_ITEM::SMC_REFER_2ENDS;
		else if(iPara==5)
			cSMCType=SMC_ITEM::SMC_REFER_UDF;
		if(pItem->m_idProp==CElemInfo::GetPropID("smcItem1.cSMCType"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->smcItems[0].cSMCType=cSMCType;
		}
		
		if(pItem->m_idProp==CElemInfo::GetPropID("smcItem2.cSMCType"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->smcItems[1].cSMCType=cSMCType;
		}
		if(pItem->m_idProp==CElemInfo::GetPropID("smcItem3.cSMCType"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->smcItems[2].cSMCType=cSMCType;
		}
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem1.gyreRadius"))
	{
		pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();
		BYTE cSMCType=pElemInfo->smcItems[0].cSMCType;
		if(valueStr[0]=='0')	//ƽ����
			cSMCType&=0X7F;
		else					//��С��
			cSMCType|=0X80;
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			pElemInfo->smcItems[0].cSMCType=cSMCType;
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem2.gyreRadius"))
	{
		pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();
		BYTE cSMCType=pElemInfo->smcItems[1].cSMCType;
		if(valueStr[0]=='0')	//ƽ����
			cSMCType&=0X7F;
		else					//��С��
			cSMCType|=0X80;
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			pElemInfo->smcItems[1].cSMCType=cSMCType;
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem3.gyreRadius"))
	{
		pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();
		BYTE cSMCType=pElemInfo->smcItems[2].cSMCType;
		if(valueStr[0]=='0')	//ƽ����
			cSMCType&=0X7F;
		else					//��С��
			cSMCType|=0X80;
		for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			pElemInfo->smcItems[2].cSMCType=cSMCType;
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("LoRyo.hStartNode")||pItem->m_idProp==CElemInfo::GetPropID("LoRyo.hEndNode")||
		pItem->m_idProp==CElemInfo::GetPropID("LoRx.hStartNode")||pItem->m_idProp==CElemInfo::GetPropID("LoRx.hEndNode")||
		pItem->m_idProp==CElemInfo::GetPropID("smcItem1.LoRxc.hStartNode")||pItem->m_idProp==CElemInfo::GetPropID("smcItem1.LoRxc.hEndNode")||
		pItem->m_idProp==CElemInfo::GetPropID("smcItem2.LoRxc.hStartNode")||pItem->m_idProp==CElemInfo::GetPropID("smcItem2.LoRxc.hEndNode")||
		pItem->m_idProp==CElemInfo::GetPropID("smcItem3.LoRxc.hStartNode")||pItem->m_idProp==CElemInfo::GetPropID("smcItem3.LoRxc.hEndNode"))
	{
		long point_i=atoi(valueStr);
		long hNode=0;
		for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(pNode->point_i==point_i)
			{
				hNode=pNode->handle;
				break;
			}
		}
		if(pItem->m_idProp==CElemInfo::GetPropID("LoRyo.hStartNode"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->LoRyo.hStartNode=hNode;
		}
		else if(pItem->m_idProp==CElemInfo::GetPropID("LoRyo.hEndNode"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->LoRyo.hEndNode=hNode;
		}
		else if(pItem->m_idProp==CElemInfo::GetPropID("LoRx.hStartNode"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->LoRx.hStartNode=hNode;
		}
		else if(pItem->m_idProp==CElemInfo::GetPropID("LoRx.hEndNode"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->LoRx.hEndNode=hNode;
		}
		else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem1.LoRxc.hStartNode"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->smcItems[0].LoRc.hStartNode=hNode;
		}
		else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem1.LoRxc.hEndNode"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->smcItems[0].LoRc.hEndNode=hNode;
		}
		else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem2.LoRxc.hStartNode"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->smcItems[1].LoRc.hStartNode=hNode;
		}
		else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem2.LoRxc.hEndNode"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->smcItems[1].LoRc.hEndNode=hNode;
		}
		else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem3.LoRxc.hStartNode"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->smcItems[2].LoRc.hStartNode=hNode;
		}
		else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem3.LoRxc.hEndNode"))
		{
			for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
				pElemInfo->smcItems[2].LoRc.hEndNode=hNode;
		}
	}
	CHashList<ELEM_LABEL> hashKeyElems;
	for(pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
	{
		long hStart=pElemInfo->m_hStartNode;
		long hEnd=pElemInfo->m_hEndNode;
		CElemInfo* pFatherElemInfo=pElemInfo;
		if(!pElemInfo->relaElem.bFatherElem&&pElemInfo->relaElem.xFatherElem.pElemInfo)
			pFatherElemInfo=pElemInfo->relaElem.xFatherElem.pElemInfo;
		CLDSNode* pStart=Ta.Node.FromHandle(pFatherElemInfo->m_hStartNode);
		CLDSNode* pEnd=Ta.Node.FromHandle(pFatherElemInfo->m_hEndNode);
		hashKeyElems.SetValue(pFatherElemInfo->Id,ELEM_LABEL(pFatherElemInfo,pStart->point_i,pEnd->point_i));
	}
	if(pRevisionDlg)
	{
		for(ELEM_LABEL* pLabel=hashKeyElems.GetFirst();pLabel;pLabel=hashKeyElems.GetNext())
			pRevisionDlg->UpdateElem(pLabel->startPointI,pLabel->endPointI);
	}
	return TRUE;
}
BOOL ElementButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView *pView = ((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSNode *pStartNode=NULL,*pEndNode=NULL;
	CString valueStr;
	if(pItem->m_idProp==CElemInfo::GetPropID("m_uStatMatNo"))
	{
		UINT iNo=Ta.GetNewStatNo();
		valueStr.Format("%d",iNo);
		for(CElemInfo* pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
		{
			CLDSLinePart* pRod=(CLDSLinePart*)Ta.Parts.FromHandle(pElemInfo->m_hParentRod);
			if(pRod)
				pRod->m_uStatMatNo = iNo;
		}
		pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
	}
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem1.relaElem")||
			pItem->m_idProp==CElemInfo::GetPropID("smcItem2.relaElem")||
			pItem->m_idProp==CElemInfo::GetPropID("smcItem3.relaElem"))
	{
		CElemInfo* pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();
		f3dPoint *pPoint;
		CLockPropertyList lockPropertyList(pPropList);
		pCmdLine->FillCmdLine("��ѡ����յ�Ԫʼ�˽ڵ�:","");
		g_pSolidSnap->SetSnapType(SNAP_POINT);
		int iRet=g_pSolidSnap->SnapPoint(pPoint);
		if(iRet<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		g_pSolidDraw->SetEntSnapStatus(pPoint->ID,TRUE);
		pStartNode=console.FromNodeHandle(pPoint->ID);
		pCmdLine->FillCmdLine("��ѡ����յ�Ԫ�ն˽ڵ�:","");
		g_pSolidSnap->SetSnapType(SNAP_POINT);
		iRet=g_pSolidSnap->SnapPoint(pPoint);
		if(iRet<0)
		{
			pCmdLine->CancelCmdLine();	
			return FALSE;
		}
		g_pSolidDraw->SetEntSnapStatus(pPoint->ID,TRUE);
		pEndNode=console.FromNodeHandle(pPoint->ID);
		g_pSolidDraw->ReleaseSnapStatus();
		pCmdLine->FinishCmdLine();
		if(pItem->m_idProp==CElemInfo::GetPropID("smcItem1.relaElem"))
		{
			for(CElemInfo* pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			{
				pElemInfo->smcItems[0].relaElem.hStart=pStartNode->handle;
				pElemInfo->smcItems[0].relaElem.hEnd=pEndNode->handle;
			}
		}
		else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem2.relaElem"))
		{
			for(CElemInfo* pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			{
				pElemInfo->smcItems[1].relaElem.hStart=pStartNode->handle;
				pElemInfo->smcItems[1].relaElem.hEnd=pEndNode->handle;
			}
		}
		else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem3.relaElem"))
		{
			for(CElemInfo* pElemInfo=(CElemInfo*)pView->selectObjs.GetFirst();pElemInfo;pElemInfo=(CElemInfo*)pView->selectObjs.GetNext())
			{
				pElemInfo->smcItems[2].relaElem.hStart=pStartNode->handle;
				pElemInfo->smcItems[2].relaElem.hEnd=pEndNode->handle;
			}
		}
		valueStr.Format("%d-%d",pStartNode->point_i,pEndNode->point_i);
		pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
	}
	return TRUE;
}
BOOL ElementItemPromptHelp(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView *pView = ((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
	CLDSObject *pObj=pView->selectObjs.GetFirst();
	if(pObj==NULL||pObj->GetClassTypeId()!=CLS_FEMELEMENT)
		return FALSE;
	CElemInfo *pFirstElemInfo = (CElemInfo*)pObj;
	g_pSolidDraw->DestroyAllMark();
	CLDSNode *pNode=NULL;
	if(pItem->m_idProp==CElemInfo::GetPropID("LoRyo.hStartNode"))
		pNode=console.FromNodeHandle(pFirstElemInfo->LoRyo.hStartNode);
	else if(pItem->m_idProp==CElemInfo::GetPropID("LoRyo.hEndNode"))
		pNode=console.FromNodeHandle(pFirstElemInfo->LoRyo.hEndNode);
	else if(pItem->m_idProp==CElemInfo::GetPropID("LoRx.hStartNode"))
		pNode=console.FromNodeHandle(pFirstElemInfo->LoRx.hStartNode);
	else if(pItem->m_idProp==CElemInfo::GetPropID("LoRx.hEndNode"))
		pNode=console.FromNodeHandle(pFirstElemInfo->LoRx.hEndNode);
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem1.LoRxc.hStartNode"))
		pNode=console.FromNodeHandle(pFirstElemInfo->smcItems[0].LoRc.hStartNode);
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem1.LoRxc.hEndNode"))
		pNode=console.FromNodeHandle(pFirstElemInfo->smcItems[0].LoRc.hEndNode);
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem2.LoRxc.hStartNode"))
		pNode=console.FromNodeHandle(pFirstElemInfo->smcItems[1].LoRc.hStartNode);
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem2.LoRxc.hEndNode"))
		pNode=console.FromNodeHandle(pFirstElemInfo->smcItems[1].LoRc.hEndNode);
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem3.LoRxc.hStartNode"))
		pNode=console.FromNodeHandle(pFirstElemInfo->smcItems[1].LoRc.hStartNode);
	else if(pItem->m_idProp==CElemInfo::GetPropID("smcItem3.LoRxc.hEndNode"))
		pNode=console.FromNodeHandle(pFirstElemInfo->smcItems[2].LoRc.hEndNode);
	if(pNode)
		g_pSolidDraw->NewMark(pNode->Position(false),5,AtomType::mkRectX);
	return TRUE;
}
BOOL ModifyElementItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CElemInfo::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
#endif
void CLDSView::DisplayFemElementProperty(BOOL bRebuild)
{
#ifdef __PART_DESIGN_INC_
	CRevisionOperDlg *pRevision=((CMainFrame*)theApp.m_pMainWnd)->GetRevisionOperPage();
	for(CLDSObject* pObj=selectObjs.GetFirst();pObj;pObj=selectObjs.GetNext())
	{
		if(pObj->GetClassTypeId()!=CLS_FEMELEMENT)
			continue;
		CElemInfo* pSelElemInfo=(CElemInfo*)pObj;
		long hStart=pSelElemInfo->m_hStartNode;
		long hEnd=pSelElemInfo->m_hEndNode;
		if(!pSelElemInfo->relaElem.bFatherElem&&pSelElemInfo->relaElem.xFatherElem.pElemInfo)
		{
			hStart=pSelElemInfo->relaElem.xFatherElem.pElemInfo->m_hStartNode;
			hEnd=pSelElemInfo->relaElem.xFatherElem.pElemInfo->m_hEndNode;
		}
		CLDSNode* pStart=Ta.Node.FromHandle(hStart);
		CLDSNode* pEnd=Ta.Node.FromHandle(hEnd);
		pRevision->LocateElem(pStart->point_i,pEnd->point_i);
	}
	CElemInfo *pFirstElemInfo = (CElemInfo*)selectObjs.GetFirst();
	CLDSLinePart *pParentRod=(CLDSLinePart*)console.FromPartHandle(pFirstElemInfo->m_hParentRod);
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pSonPropItem,*pLeafItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_LAMDAINFO=2;
	if(bRebuild)
	{//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(2);
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_LAMDAINFO-1,"��ϸ��");
		pPropDlg->RefreshTabCtrl(CElemInfo::m_iCurDisplayPropGroup);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_FEMELEMENT;
		pPropList->SetModifyValueFunc(ModifyElementProperty);
		pPropList->SetButtonClickFunc(ElementButtonClick);
		pPropList->SetModifyStatusFunc(ModifyElementItemStatus);
		pPropList->SetPropHelpPromptFunc(ElementItemPromptHelp);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstElemInfo->BelongModel()->IsTowerModel()||pFirstElemInfo->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//������Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_strPropName = "��Ԫ������Ϣ";
		lpInfo->m_strPropHelp = "��Ԫ������Ϣ";
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CElemInfo::GetPropID("basicInfo");
		if(CElemInfo::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CElemInfo::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_LAMDAINFO);
			//��Ԫ��ʶ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��ʶ";
		lpInfo->m_strPropHelp = "��ʶ�ɵ�Ԫ���˽ڵ����ɣ�С����ǰ������ں�";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CElemInfo::GetPropID("Id");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly();
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_LAMDAINFO);
			//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "��������ͼ��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CElemInfo::GetPropID("layer");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_LAMDAINFO);
			//����	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "����";
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390|Q420|Q460";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CElemInfo::GetPropID("cMaterial");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_FILEPATH;
		lpInfo->m_strPropName = "ͳ�ı��";
		lpInfo->m_strPropHelp = "ͳ�ı�ţ������Ҳఴť�ɻ�ȡһ��δʹ�õ��ºš�";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CElemInfo::GetPropID("m_uStatMatNo");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ĺ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "��ĺ�";
		lpInfo->m_strPropHelp = "��ĺ�";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CElemInfo::GetPropID("cfgword");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly();
		//�˼�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��Ԫ����";
		lpInfo->m_strPropHelp = "��Ԫ����";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CElemInfo::GetPropID("length");
		pPropItem->SetReadOnly();
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_LAMDAINFO);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�˼�����";
		lpInfo->m_strPropHelp = "�˼�����";
		lpInfo->m_cmbItems = "��֫�Ǹ�|T�����|�Խ����|ʮ�����|�ֹ�|�۸�|���|����|�û�����";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CElemInfo::GetPropID("size.idClassType");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		if(pParentRod&&pParentRod->size_idClassType==CLS_CABLE)
		{
			lpInfo->m_strPropName = "��������";
			lpInfo->m_strPropHelp = "��������";
			lpInfo->m_cmbItems=MakeWireTypeSetString();
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CElemInfo::GetPropID("m_sWireType");

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_strPropName = "���¹��";
			lpInfo->m_strPropHelp = "���¾������ͺ�";
			lpInfo->m_cmbItems=MakeWireSetString(sText);
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CElemInfo::GetPropID("m_sWireSpec");
			if(pFirstElemInfo->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�����";
			lpInfo->m_strPropHelp = "���½����,��λ(mm2)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CElemInfo::GetPropID("m_fWireArea");
			if(pFirstElemInfo->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ϵ��(MPa)";
			lpInfo->m_strPropHelp = "���µ���ϵ��(ģ��),��λ(MPa)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CElemInfo::GetPropID("m_fElasticCoef");
			if(pFirstElemInfo->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�¶�������ϵ��";
			lpInfo->m_strPropHelp = "�¶�������ϵ��,��λ�����������¶�����1��Cʱ�ĳ���������";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CElemInfo::GetPropID("m_fExpansionCoef");
			if(pFirstElemInfo->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "��λ����(kg/km)";
			lpInfo->m_strPropHelp = "��λ�������µ�����,��λ(kg/km)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CElemInfo::GetPropID("m_fUnitWeight");
			if(pFirstElemInfo->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�����ǿ��";
			lpInfo->m_strPropHelp = "�����ǿ��,kN";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CElemInfo::GetPropID("m_fForcePermit");
			if(pFirstElemInfo->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Ԥ�Ž���";
			lpInfo->m_strPropHelp = "��װʱԤ�Ž���,kN";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CElemInfo::GetPropID("m_fPreTension");
			if(pFirstElemInfo->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		}
		else
		{
			lpInfo->m_strPropName = "�˼����";
			lpInfo->m_strPropHelp = "�˼����";
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CElemInfo::GetPropID("specification");
			pPropItem->m_lpNodeInfo->m_controlType = PIT_COMBO;
			if(pParentRod->size_idClassType==CLS_LINEANGLE)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeAngleSetString();
			else if(pParentRod->size_idClassType==CLS_GROUPLINEANGLE)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeAngleSetString(pParentRod->size_cSubClassType);
			else if(pParentRod->size_idClassType==CLS_LINETUBE)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeTubeSetString();
			else if(pParentRod->size_idClassType==CLS_LINESLOT)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeSlotSetString();
			else if(pParentRod->size_idClassType==CLS_LINEFLAT)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeFlatSetString();
		}
#ifdef lksdf
		if(pFirstLinePart->size_idClassType==CLS_CABLE)
		{
		}
		else if(pFirstLinePart->size_idClassType==0)//TRUSS_POLE_USERDEF)
		{
			pPropItem->m_lpNodeInfo->m_controlType = PIT_EDIT;
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�Զ�����";
			lpInfo->m_strPropHelp = "�Զ���˼�������";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.size");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pPropItem->m_lpNodeInfo->m_controlType = PIT_EDIT;
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�����";
			lpInfo->m_strPropHelp = "�˼������,��λ(mm2)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.area");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ģ��";
			lpInfo->m_strPropHelp = "�˼�����������ģ��,��λ(MPa)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.E");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "��ѹǿ�����ֵ";
			lpInfo->m_strPropHelp = "��ѹǿ�����ֵ,��λ(MPa)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.fc");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "����ǿ�����ֵ";
			lpInfo->m_strPropHelp = "����ǿ�����ֵ,��λ(MPa)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.ft");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "��λ����";
			lpInfo->m_strPropHelp = "��λ���ȸ˼�����,��λ(kg/m)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.unit_weight");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "ԤӦ��";
			lpInfo->m_strPropHelp = "��װʱԤ��Ӧ��ֵ,��λ(Pa)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.pre_stress");
			if(pFirstLinePart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		}
		else
		{
			pPropItem->m_lpNodeInfo->m_controlType = PIT_COMBO;
			if(pFirstLinePart->size.idClassType==CLS_LINEANGLE)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeAngleSetString();
			else if(pFirstLinePart->size.idClassType==CLS_GROUPLINEANGLE)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeAngleSetString(pFirstLinePart->size.cSubClassType);
			else if(pFirstLinePart->size.idClassType==CLS_LINETUBE)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeTubeSetString();
			else if(pFirstLinePart->size.idClassType==CLS_LINESLOT)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeSlotSetString();
			else if(pFirstLinePart->size.idClassType==CLS_LINEFLAT)
				pPropItem->m_lpNodeInfo->m_cmbItems=MakeFlatSetString();
		}
		//
#endif
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_bHideChildren=TRUE;
		if(CElemInfo::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pPropItem->m_idProp);
		else
			pPropItem->m_bHideChildren=FALSE;
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�Ƿ���й����ѡ";
		lpInfo->m_strPropHelp = "�Ƿ���й����ѡ";
		lpInfo->m_cmbItems = "��|��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CElemInfo::GetPropID("m_bNeedSelSize");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��Ԫ����";
		lpInfo->m_strPropHelp = "��Ԫ����";
		lpInfo->m_cmbItems = "0.�Զ��ж�|1.������|2.������|3.������Ԫ";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CElemInfo::GetPropID("m_iElemType");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pParentRod->size_idClassType==CLS_CABLE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "ʼ��Լ������";
		lpInfo->m_strPropHelp = "ʼ��Լ������";
		lpInfo->m_cmbItems = "�½�|�ս�|�Զ��ж�";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CElemInfo::GetPropID("start_joint_type");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�ն�Լ������";
		lpInfo->m_strPropHelp = "�ն�Լ������";
		lpInfo->m_cmbItems = "�½�|�ս�|�Զ��ж�";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CElemInfo::GetPropID("end_joint_type");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "ʼ����������";
		lpInfo->m_strPropHelp = "ʼ����������";
		lpInfo->m_cmbItems = "��������|ƫ������";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CElemInfo::GetPropID("start_force_type");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�ն���������";
		lpInfo->m_strPropHelp = "�ն���������";
		lpInfo->m_cmbItems = "��������|ƫ������";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CElemInfo::GetPropID("end_force_type");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��˨�����Ϣ";
		lpInfo->m_strPropHelp = "��˨�����Ϣ";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CElemInfo::GetPropID("boltRelaInfo");
		pPropItem->m_bHideChildren=FALSE;
		if(CElemInfo::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CElemInfo::GetPropStatus(pPropItem->m_idProp);
		pPropItem->SetReadOnly();
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�Զ�������˨������";
		lpInfo->m_strPropHelp = "�Զ�������˨������";
		lpInfo->m_cmbItems = "��|��";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CElemInfo::GetPropID("m_bAutoCalHoleOutCoef");
		if(pFirstElemInfo->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��˨������";
		lpInfo->m_strPropHelp = "��˨������";
		pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
		pLeafItem->m_idProp = CElemInfo::GetPropID("hole_out");
		if(pFirstElemInfo->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->SetReadOnly(pParentRod->m_bAutoCalHoleOutCoef);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��˨���";
		lpInfo->m_strPropHelp = "��˨���";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CElemInfo::GetPropID("bolt_type");
		if(pFirstElemInfo->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pSonPropItem->SetReadOnly();
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��˨����";
		lpInfo->m_strPropHelp = "��˨����";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CElemInfo::GetPropID("bolt_N");
		if(pFirstElemInfo->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���㳤����Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_strPropName = "���㳤����Ϣ";
		lpInfo->m_strPropHelp = "���㳤����Ϣ";
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CElemInfo::GetPropID("lamdaInfo");
		if(CElemInfo::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CElemInfo::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��ϸ�ȼ�������";
		lpInfo->m_strPropHelp = "��ϸ�ȼ�������";
		lpInfo->m_cmbItems = "0.���ܼ���|1.Vб��(��Ų�����)|2.Vб��(V������)|3.Vб��3�ڼ��������|4.Vб��4�ڼ��������|5.ָ����Ԫ���㳤��|6.ָ������ļ��㳤��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CElemInfo::GetPropID("cCalLenType");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(CElemInfo::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=CElemInfo::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		//���㳤����С�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��С��";
		lpInfo->m_strPropHelp = "��С��ʧ��ʱ���㳤����Ϣ";
		lpInfo->m_cmbItems = "0.������|1.����";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CElemInfo::GetPropID("lamdaRyoInfo");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		if(CElemInfo::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CElemInfo::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		pPropItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());

		if(pFirstElemInfo->LoRyoCalType()<2)
			UpdateLoRyoInfo(pPropList,pPropItem,1);	//����
		else
			UpdateLoRyoInfo(pPropList,pPropItem,0);	//������
		
		//���㳤��ƽ�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "ƽ����";
		lpInfo->m_strPropHelp = "�Ǹ�ƽ����ʧ��ʱ���㳤����Ϣ";
		lpInfo->m_cmbItems = "0.������|1.����";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CElemInfo::GetPropID("lamdaRxInfo");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		if(CElemInfo::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CElemInfo::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		pPropItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());

		if(pFirstElemInfo->LoRxCalType()<2)
			UpdateLoRxInfo(pPropList,pPropItem,1);	//����
		else
			UpdateLoRxInfo(pPropList,pPropItem,0);	//������
		//���㳤��ͬʱ��ѹ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "ͬʱ��ѹ";
		lpInfo->m_strPropHelp = "ͬʱ��ѹʧ��ʱ���㳤����Ϣ";
		lpInfo->m_cmbItems = "0.������|1.����|2.˫��|3.����";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CElemInfo::GetPropID("lamdaRxcInfo");
		if(pFirstElemInfo->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		if(CElemInfo::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CElemInfo::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_LAMDAINFO);
		pPropItem->SetReadOnly(!pFirstElemInfo->IsSpecLoInfo());
		UpdateLoRxcInfo(pPropList,pPropItem);
	}
	else
	{
		SetPartMutiObjsProp(pFirstElemInfo,"Id");
		SetPartMutiObjsProp(pFirstElemInfo,"layer");
		SetPartMutiObjsProp(pFirstElemInfo,"cMaterial");
		SetPartMutiObjsProp(pFirstElemInfo,"m_uStatMatNo");
		SetPartMutiObjsProp(pFirstElemInfo,"cfgword");
		SetPartMutiObjsProp(pFirstElemInfo,"length");
		CPropTreeItem *pParentItem;
		if(pParentRod->size_idClassType==CLS_CABLE)	//�û��������
		{
			pParentItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("specification"),NULL);
			/*if(pParentItem==NULL)
			{	//ԭ��ʾ�˼������������ͣ�����ͨ�����Ŀ
				SetPartMutiObjsProp(pFirstElemInfo,"m_sWireType");
				SetPartMutiObjsProp(pFirstElemInfo,"m_sWireSpec");
				SetPartMutiObjsProp(pFirstElemInfo,"m_fWireArea");
				SetPartMutiObjsProp(pFirstElemInfo,"m_fElasticCoef");
				SetPartMutiObjsProp(pFirstElemInfo,"m_fForcePermit");
				SetPartMutiObjsProp(pFirstElemInfo,"m_fExpansionCoef");
				SetPartMutiObjsProp(pFirstElemInfo,"m_fPreTension");
				SetPartMutiObjsProp(pFirstElemInfo,"m_fUnitWeight");
			}
			else
			{	//ԭ��ʾ�˼�Ϊ��ͨ�˼�����Ҫ�任��������ʾģʽ
				//����Զ��������͸˼���������ʾ��Ŀ
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.size"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.area"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.E"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.fc"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.ft"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.pre_stress"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.unit_weight"));
				//�ı�˼����������ĿΪ��������������Ŀ
				pParentItem->m_lpNodeInfo->m_controlType = PIT_COMBO;
				pParentItem->m_lpNodeInfo->m_strPropName = "��������";
				pParentItem->m_lpNodeInfo->m_strPropHelp = "��������";
				pParentItem->m_lpNodeInfo->m_cmbItems=MakeWireTypeSetString();
				pParentItem->m_lpNodeInfo->m_strPropValue=pFirstLinePart->GetWireType();
				pParentItem->m_idProp = CLDSLinePart::GetPropID("m_sWireType");
				if(CLDSLinePart::GetPropStatus(pParentItem->m_idProp)!=-1)
					pParentItem->m_bHideChildren=CLDSLinePart::GetPropStatus(pParentItem->m_idProp);
				else
					pParentItem->m_bHideChildren=FALSE;

				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_strPropName = "���¹��";
				lpInfo->m_strPropHelp = "���¾������ͺ�";
				lpInfo->m_cmbItems=MakeWireSetString(pFirstLinePart->GetWireType());
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_sWireSpec");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "�����";
				lpInfo->m_strPropHelp = "���½����,��λ(mm2)";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fWireArea");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->SetReadOnly();
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "����ϵ��(MPa)";
				lpInfo->m_strPropHelp = "���µ���ϵ��(ģ��),��λ(MPa)";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fElasticCoef");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->SetReadOnly();
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "�¶�������ϵ��";
				lpInfo->m_strPropHelp = "�¶�������ϵ��,��λ�����������¶�����1��Cʱ�ĳ���������";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fExpansionCoef");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->SetReadOnly();
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��λ����(kg/km)";
				lpInfo->m_strPropHelp = "��λ�������µ�����,��λ(kg/km)";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fUnitWeight");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->SetReadOnly();
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "�����ǿ��";
				lpInfo->m_strPropHelp = "�����ǿ��,kN";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fForcePermit");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->SetReadOnly();
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Ԥ�Ž���";
				lpInfo->m_strPropHelp = "��װʱԤ�Ž���,kN";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CLDSLinePart::GetPropID("m_fPreTension");
				if(pFirstLinePart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			}*/
		}
		else
		{
			pParentItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("m_sWireType"),NULL);
			if(pParentItem)
			{	//ԭ��ʾ�˼����������߸˼���Ҫת��Ϊ��ͨ�˼������ʾģʽ
				pParentItem->m_lpNodeInfo->m_strPropName = "�˼����";
				pParentItem->m_lpNodeInfo->m_strPropHelp = "�˼����";
				pParentItem->m_idProp = CElemInfo::GetPropID("specification");
				//���ԭ���߸˼����������ʾ��Ŀ
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("m_sWireSpec"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("m_fWireArea"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("m_fElasticCoef"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("m_fForcePermit"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("m_fExpansionCoef"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("m_fPreTension"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("m_fUnitWeight"));
			}
			else//ԭ��ʾ�˼�����������ͨ�˼�(�����߸˼�)
				pParentItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("specification"),NULL);
			SetPartMutiObjsProp(pFirstElemInfo,"specification");
			if(pParentRod->size_idClassType==0)	//�û��������
			{
				pParentItem->m_lpNodeInfo->m_controlType=PIT_EDIT;
				
				long idProp=CLDSLinePart::GetPropID("userdef.size");
				/*pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "�Զ�����";
					lpInfo->m_strPropHelp = "�Զ���˼�������";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = CElemInfo::GetPropID("userdef.size");
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstElemInfo->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				
				idProp=CLDSLinePart::GetPropID("userdef.area");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "�����";
					lpInfo->m_strPropHelp = "�˼������,��λ(mm2)";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = CLDSLinePart::GetPropID("userdef.area");
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;

				idProp=CLDSLinePart::GetPropID("userdef.E");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "����ģ��";
					lpInfo->m_strPropHelp = "�˼�����������ģ��,��λ(MPa)";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = idProp;
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;

				idProp=CLDSLinePart::GetPropID("userdef.fc");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "��ѹǿ�����ֵ";
					lpInfo->m_strPropHelp = "��ѹǿ�����ֵ,��λ(MPa)";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = idProp;
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;

				idProp=CLDSLinePart::GetPropID("userdef.ft");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "����ǿ�����ֵ";
					lpInfo->m_strPropHelp = "����ǿ�����ֵ,��λ(MPa)";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = idProp;
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;

				idProp=CLDSLinePart::GetPropID("userdef.pre_stress");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "ԤӦ��";
					lpInfo->m_strPropHelp = "��װʱԤ��Ӧ��ֵ,��λ(Pa)";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = idProp;
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;

				idProp=CLDSLinePart::GetPropID("userdef.unit_weight");
				pPropItem=pPropList->FindItemByPropId(idProp,NULL);
				if(pPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "��λ����";
					lpInfo->m_strPropHelp = "��λ���ȸ˼�����,��λ(kg/m)";
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = idProp;
				}
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(selectObjs,idProp);
				if(pFirstLinePart->GetPropValueStr(idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;*/
			}
			else
			{
				pParentItem->m_lpNodeInfo->m_controlType=PIT_COMBO;
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.size"));	//���û�ָ��ʱɾ���û������� wht 10-11-22
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.area"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.E"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.fc"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.ft"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.pre_stress"));
				pPropList->DeleteItemByPropId(CElemInfo::GetPropID("userdef.unit_weight"));

				if(pParentRod->size_idClassType==CLS_LINEANGLE)
					pParentItem->m_lpNodeInfo->m_cmbItems=MakeAngleSetString();
				else if(pParentRod->size_idClassType==CLS_GROUPLINEANGLE)
					pParentItem->m_lpNodeInfo->m_cmbItems=MakeAngleSetString(pParentRod->size_cSubClassType);
				else if(pParentRod->size_idClassType==CLS_LINETUBE)
					pParentItem->m_lpNodeInfo->m_cmbItems=MakeTubeSetString();
				else if(pParentRod->size_idClassType==CLS_LINESLOT)
					pParentItem->m_lpNodeInfo->m_cmbItems=MakeSlotSetString();
				else if(pParentRod->size_idClassType==CLS_LINEFLAT)
					pParentItem->m_lpNodeInfo->m_cmbItems=MakeFlatSetString();
				else if(pParentRod->size_idClassType==CLS_CABLE)
					pParentItem->m_lpNodeInfo->m_cmbItems=MakeFlatSetString();
			}
		}

		SetPartMutiObjsProp(pFirstElemInfo,"size.idClassType");
		//�����Ϣ
		int idProp=0;
		SetPartMutiObjsProp(pFirstElemInfo,"m_bNeedSelSize");
		SetPartMutiObjsProp(pFirstElemInfo,"m_iElemType");
		/*pPropItem = pPropList->FindItemByPropId(CElemInfo::GetPropID("m_iElemType"),&pParentItem);
		if(pPropItem)
			pPropItem->SetReadOnly(pParentRod->size_idClassType==CLS_CABLE);*/
		SetPartMutiObjsProp(pFirstElemInfo,"lamdaRyoInfo");
		pPropItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("lamdaRyoInfo"),&pParentItem);
		if(pFirstElemInfo->LoRyoCalType()<2)
			UpdateLoRyoInfo(pPropList,pPropItem,1);	//����
		else
			UpdateLoRyoInfo(pPropList,pPropItem,0);	//������
		SetPartMutiObjsProp(pFirstElemInfo,"lamdaRxInfo");
		pPropItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("lamdaRxInfo"),&pParentItem);
		if(pFirstElemInfo->LoRxCalType()<2)
			UpdateLoRxInfo(pPropList,pPropItem,1);	//����
		else
			UpdateLoRxInfo(pPropList,pPropItem,0);	//������
		SetPartMutiObjsProp(pFirstElemInfo,"lamdaRxcInfo");
		pPropItem=pPropList->FindItemByPropId(CElemInfo::GetPropID("lamdaRxcInfo"),&pParentItem);
		UpdateLoRxcInfo(pPropList,pPropItem);
		SetPartMutiObjsProp(pFirstElemInfo,"cCalLenType");
		SetPartMutiObjsProp(pFirstElemInfo,"start_joint_type");
		SetPartMutiObjsProp(pFirstElemInfo,"start_force_type");
		SetPartMutiObjsProp(pFirstElemInfo,"end_joint_type");
		SetPartMutiObjsProp(pFirstElemInfo,"end_force_type");
		SetPartMutiObjsProp(pFirstElemInfo,"bolt_type");
		SetPartMutiObjsProp(pFirstElemInfo,"bolt_N");
		SetPartMutiObjsProp(pFirstElemInfo,"m_bAutoCalHoleOutCoef");
		pPropItem = pPropList->FindItemByPropId(CElemInfo::GetPropID("hole_out"),&pParentItem);
		if(pPropItem)
			pPropItem->SetReadOnly(pParentRod->m_bAutoCalHoleOutCoef);
		SetPartMutiObjsProp(pFirstElemInfo,"hole_out");
	}
	pPropList->Redraw();
#endif
}