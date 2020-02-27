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
#include "Query.h"
#include "RelativeObjectDlg.h"
#include "dlg.h"
#include "XhCharString.h"
#include "DesignShadowDlg.h"
#include "DatumPointPropertyDlg.h"
#include "ParaDatumPointDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//from DisplayPropertyPage.cpp
BOOL SetPartMutiObjsProp(CLDSObject *pObj, char *propName,long idClassType=0);
BOOL UpdatePropItemValue(CPropertyList* pPropList,CLDSObject *pObj, long prop_id,int clstype_id);
void SetPropItemReadOnly(CLDSDbObject *pObj,char *propName,BOOL bReadOnly);

CPropTreeItem *InsertPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,
	CLDSNode* pObj,int idProp,CString sPropName,CString sPropHelp="",int ctrlType=PIT_STATIC,
	int subType=0,CString sItemAttachValue="")
{
	CXhChar500 sText;
	CItemInfo* lpInfo = new CItemInfo();
	lpInfo->m_controlType=ctrlType;
	if(lpInfo->m_controlType==PIT_BUTTON)
	{
		lpInfo->m_buttonType=subType;
		if(sItemAttachValue.GetLength()>0)
			lpInfo->m_sButtonName=sItemAttachValue;
	}
	else if(lpInfo->m_controlType==PIT_COMBO)
	{
		lpInfo->m_cmbType=subType;
		lpInfo->m_cmbItems = sItemAttachValue;
	}
	lpInfo->m_strPropName = sPropName;
	if(sPropHelp.GetLength()>0)
		lpInfo->m_strPropHelp = sPropHelp;
	else 
		lpInfo->m_strPropName = sPropName;
	CPropTreeItem *pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
	pPropItem->m_idProp = idProp;
	if(pObj->GetPropValueStr(pPropItem->m_idProp,sText)>0 && ctrlType!=PIT_STATIC)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	return pPropItem;
}
BOOL ModifyNodeItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSNode::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL NodeButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSNode *pFirstNode=NULL, *pNode=NULL;
	CPropTreeItem *pFindItem, *pParentItem = NULL;
	CReportVectorDlg vecDlg;
	CNodeDesignPosDlg designPosDlg;
	CCfgPartNoDlg cfgdlg;

	CString valueStr=_T("");
	if(pView->selectObjs.GetNodeNum()>0)
		pFirstNode = (CLDSNode*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	CXhChar50 sSoftName("LMA");
	CXhChar100 sPropKey; 
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
	try{
		if(pItem->m_idProp==CLDSNode::GetPropID("pos"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Node Coordinate";
#else 
			vecDlg.m_sCaption = "�ڵ�����";
#endif
			vecDlg.m_fVectorX = pFirstNode->Position(false).x;
			vecDlg.m_fVectorY = pFirstNode->Position(false).y;
			vecDlg.m_fVectorZ = pFirstNode->Position(false).z;
			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("pos.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("pos.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("pos.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSNode::GetPropID("pos.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSNode::GetPropID("pos.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSNode::GetPropID("pos.z"),CString(sText));
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetPosition(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					pNode->ResetRelativeObjs();
				}
				g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("m_xArcLiftPos")||
			pItem->m_idProp==CLDSNode::GetPropID("m_xArcLiftPos.x")||
			pItem->m_idProp==CLDSNode::GetPropID("m_xArcLiftPos.y")||
			pItem->m_idProp==CLDSNode::GetPropID("m_xArcLiftPos.z"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Prelift Coordinate";
#else 
			vecDlg.m_sCaption = "Ԥ����ڵ�����";
#endif
			vecDlg.m_fVectorX = pFirstNode->ArcLiftPos().x;
			vecDlg.m_fVectorY = pFirstNode->ArcLiftPos().y;
			vecDlg.m_fVectorZ = pFirstNode->ArcLiftPos().z;
			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("m_xArcLiftPos.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("m_xArcLiftPos.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("m_xArcLiftPos.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			vecDlg.DoModal();
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("cfgword"))
		{
			cfgdlg.cfgword = pFirstNode->cfgword; 
			if(pFirstNode->IsBodyObj())
				cfgdlg.cfg_style=CFG_BODY_NO;
			else
				cfgdlg.cfg_style=CFG_LEG_NO;
			if(cfgdlg.DoModal()==IDOK)
				pFirstNode->cfgword = cfgdlg.cfgword;		//�ڴ˴����µ�һ���ڵ������Ϻ�
			char sText[MAX_TEMP_CHAR_100+1]="";
			if(pFirstNode->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
				pItem->m_lpNodeInfo->m_strPropValue=sText;
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("relativeObjs"))
		{
			CRelativeObjectDlg relativeDlg;
			relativeDlg.m_pWorkObj = pFirstNode;
			if(relativeDlg.DoModal()==IDOK)
			{
				CXhChar200 sText;
				pFirstNode->GetPropValueStr(pItem->m_idProp,sText);
				pPropList->SetItemPropValue(pItem->m_idProp,sText);
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("shadowInfo"))
		{
			CDesignShadowDlg shadowdlg;
			shadowdlg.SetShadowObj((CLDSObject*)pFirstNode);
			shadowdlg.DoModal();
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode"))
		{	//Ŀǰ�����ڲ�汾���Ŵ˹���
			CLogErrorLife life(&logerr);
			CUndoOperObject undo(&Ta);
			CLDSNode* pFirstNode= (CLDSNode*)pView->selectObjs.GetFirst();
			bool search_modified=false;
			CHashList<CLDSNodePtr>hashSelNodes;
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				hashSelNodes.SetValue(pNode->handle,pNode);
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				if(pNode->IsIntegrityParams())
					continue;
				bool searched=pNode->SearchRelaDependNodeRods();
				if(!searched)
#ifdef __PART_DESIGN_INC_
					logerr.Log("0x%X�ڵ�{%d��}�����ڵ��Զ�����ʧ��",pNode->handle,pNode->pointI);
#else
					logerr.Log("0x%X�ڵ������ڵ��Զ�����ʧ��",pNode->handle);
#endif
				if(pFirstNode==pNode)
					search_modified=searched;
				for(RELATIVE_OBJECT* pRelaObj=pNode->relativeObjs.GetFirst();pRelaObj&&searched;pRelaObj=pNode->relativeObjs.GetNext())
				{
					if(hashSelNodes.GetValue(pRelaObj->hObj))
						continue;
					CLDSNode* pRelaMirNode=Ta.Node.FromHandle(pRelaObj->hObj);
					if(pRelaMirNode)
						pNode->SyncMirPropTo(pRelaMirNode,pRelaObj->mirInfo,"pos");
				}
			}
			CXhChar50 sText;
			if(search_modified&&(pFirstNode->m_cPosCalType==CLDSNode::COORD_SCALE||(
				pFirstNode->m_cPosCalType>=CLDSNode::COORD_X_SPECIFIED&&pFirstNode->m_cPosCalType<=CLDSNode::COORD_Z_SPECIFIED)))
			{
				UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("hFatherPart"),CLS_NODE);
				UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attachNode1"),CLS_NODE);
				UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attachNode2"),CLS_NODE);
				if(pFirstNode->m_cPosCalType==CLDSNode::COORD_SCALE)
					UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attach_scale"),CLS_NODE);
			}
			else if(search_modified&&pFirstNode->m_cPosCalType==CLDSNode::COORD_INTERSECTION)
			{
				UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("hFatherPart"),CLS_NODE);
				UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attachNode1"),CLS_NODE);
				UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attachNode2"),CLS_NODE);
				UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attachNode3"),CLS_NODE);
				UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attachNode4"),CLS_NODE);
				UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attachPole1"),CLS_NODE);
				UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attachPole2"),CLS_NODE);
			}
		}
#ifndef __TSA_
		else if(pItem->m_idProp==CLDSNode::GetPropID("xFatherAngleZhunJu"))
		{
			CNodeDesignPosDlg nodeposdlg;
			nodeposdlg.m_iWingXOffsetStyle=pFirstNode->xFatherAngleZhunJu.offset_X_dist_style;
			nodeposdlg.m_iWingYOffsetStyle=pFirstNode->xFatherAngleZhunJu.offset_Y_dist_style;
			nodeposdlg.m_fOffsetWingX=pFirstNode->xFatherAngleZhunJu.offset_X_dist;
			nodeposdlg.m_fOffsetWingY=pFirstNode->xFatherAngleZhunJu.offset_Y_dist;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pFirstNode->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pLineAngle)
			{
				if(pLineAngle->m_bEnableTeG)
				{
					nodeposdlg.wing_x_jgzj=pLineAngle->xWingXZhunJu;
					nodeposdlg.wing_y_jgzj=pLineAngle->xWingYZhunJu;
				}
				else
				{
					getjgzj(nodeposdlg.wing_x_jgzj,pLineAngle->GetWidth());
					nodeposdlg.wing_y_jgzj=nodeposdlg.wing_x_jgzj;
				}
			}
			if(nodeposdlg.DoModal()==IDOK)
			{
				pFirstNode->xFatherAngleZhunJu.offset_X_dist_style=nodeposdlg.m_iWingXOffsetStyle;
				pFirstNode->xFatherAngleZhunJu.offset_Y_dist_style=nodeposdlg.m_iWingYOffsetStyle;
				pFirstNode->xFatherAngleZhunJu.offset_X_dist=nodeposdlg.m_fOffsetWingX;
				pFirstNode->xFatherAngleZhunJu.offset_Y_dist=nodeposdlg.m_fOffsetWingY;
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					if(pNode!=pFirstNode)
						pNode->xFatherAngleZhunJu=pFirstNode->xFatherAngleZhunJu;
				}
			}
		}
#endif
		else if(pItem->m_idProp==CLDSNode::GetPropID("hFatherPart")||pItem->m_idProp==CLDSNode::GetPropID("attachPole1")||
			pItem->m_idProp==CLDSNode::GetPropID("attachPole2"))
		{
			static CModifyPartHandleDlg modifyhdlg;
			int update_type=0;	//0.�ڵ㸸�˼� 1.�ڵ���������1 2.�ڵ���������2
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Update Dependent Rod";
			modifyhdlg.m_sTitle="Dependent Rod:";
#else 
			modifyhdlg.m_sCaption="�޸Ľڵ������˼�";
			modifyhdlg.m_sTitle="�����˼�:";
#endif
			if(CLDSNode::GetPropID("hFatherPart")==pItem->m_idProp)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				modifyhdlg.m_sCaption="Update Node Parent Rod";
				modifyhdlg.m_sTitle="Parent Rod:";
#else 
				modifyhdlg.m_sCaption="�޸Ľڵ㸸�˼�";
				modifyhdlg.m_sTitle="���˼�:";
#endif
				update_type=0;	//�ڵ㸸�˼�
				modifyhdlg.m_sHandle.Format("0X%X",pFirstNode->hFatherPart);
				g_pSolidDraw->SetEntSnapStatus(pFirstNode->hFatherPart,TRUE);
			}
			else if(CLDSNode::GetPropID("attachPole1")==pItem->m_idProp)
			{
				update_type=1;	//�ڵ������˼�1
				modifyhdlg.m_sHandle.Format("0X%X",pFirstNode->arrRelationPole[0]);
				g_pSolidDraw->SetEntSnapStatus(pFirstNode->arrRelationPole[0],TRUE);
			}
			else if(CLDSNode::GetPropID("attachPole2")==pItem->m_idProp)
			{
				update_type=2;	//�ڵ������˼�2
				modifyhdlg.m_sHandle.Format("0X%X",pFirstNode->arrRelationPole[1]);
				g_pSolidDraw->SetEntSnapStatus(pFirstNode->arrRelationPole[1],TRUE);
			}
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�/�˼��İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select node's dependent rod:");
			if(CLDSNode::GetPropID("hFatherPart")==pItem->m_idProp)
				modifyhdlg.SetCmdPickPromptStr("Please select node's parent rod:");
#else 
			modifyhdlg.SetCmdPickPromptStr("��ѡ��ڵ������˼�:");
			if(CLDSNode::GetPropID("hFatherPart")==pItem->m_idProp)
				modifyhdlg.SetCmdPickPromptStr("��ѡ��ڵ㸸�˼�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if(pPart==NULL||(pPart&&!pPart->IsLinePart()&&!pPart->IsArcPart()))
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"The input handle is error,can't find the corresponding rod��",sSoftName,MB_OK);
#else 
					MessageBox(NULL,"�����������Ҳ�����Ӧ�˼���",sSoftName,MB_OK);
#endif
				else
				{
					g_pSolidDraw->SetEntSnapStatus(pPart->handle,FALSE);
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					if(CLDSNode::GetPropID("hFatherPart")==pItem->m_idProp)
					{
						pFirstNode->hFatherPart=modifyhdlg.m_nHandle;
						pPropList->SetItemPropValue(CLDSNode::GetPropID("hFatherPart"),valueStr);
						for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
						{
							pNode->hFatherPart=modifyhdlg.m_nHandle;
							pNode->UpdateMirNodeFatherPart(MIRMSG(modifyhdlg.m_iMirMsg),pPart,update_type);
						}
					}
					else if(CLDSNode::GetPropID("attachPole1")==pItem->m_idProp)
					{
						pFirstNode->arrRelationPole[0]=modifyhdlg.m_nHandle;
						pPropList->SetItemPropValue(CLDSNode::GetPropID("attachPole1"),valueStr);
						for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
							pNode->arrRelationPole[0]=modifyhdlg.m_nHandle;
					}
					else if(CLDSNode::GetPropID("attachPole2")==pItem->m_idProp)
					{
						pFirstNode->arrRelationPole[1]=modifyhdlg.m_nHandle;
						pPropList->SetItemPropValue(CLDSNode::GetPropID("attachPole2"),valueStr);
						for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
							pNode->arrRelationPole[1]=modifyhdlg.m_nHandle;
					}
					g_pSolidDraw->ReleaseSnapStatus();
					for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
						g_pSolidDraw->SetEntSnapStatus(pNode->handle); //ѡ�й���
				}
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode1")||pItem->m_idProp==CLDSNode::GetPropID("attachNode2")||
			pItem->m_idProp==CLDSNode::GetPropID("attachNode3")||pItem->m_idProp==CLDSNode::GetPropID("attachNode4"))
		{
			int update_type=0;
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Update Dependent Node";
			modifyhdlg.m_sTitle="Dependent Node:";
#else 
			modifyhdlg.m_sCaption="�޸������ڵ�";
			modifyhdlg.m_sTitle="�����ڵ�:";
#endif
			if(pItem->m_idProp==CLDSNode::GetPropID("attachNode1"))
			{
				update_type=0;	//���������ڵ�1
				g_pSolidDraw->SetEntSnapStatus(pFirstNode->arrRelationNode[0]);
				modifyhdlg.m_sHandle.Format("0X%X",pFirstNode->arrRelationNode[0]);
			}
			else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode2"))
			{
				update_type=1;	//���������ڵ�2
				g_pSolidDraw->SetEntSnapStatus(pFirstNode->arrRelationNode[1]);
				modifyhdlg.m_sHandle.Format("0X%X",pFirstNode->arrRelationNode[1]);
			}
			else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode3"))
			{
				update_type=2;	//���������ڵ�3
				g_pSolidDraw->SetEntSnapStatus(pFirstNode->arrRelationNode[2]);
				modifyhdlg.m_sHandle.Format("0X%X",pFirstNode->arrRelationNode[2]);
			}
			else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode4"))
			{
				update_type=3;	//���������ڵ�4
				g_pSolidDraw->SetEntSnapStatus(pFirstNode->arrRelationNode[3]);
				modifyhdlg.m_sHandle.Format("0X%X",pFirstNode->arrRelationNode[3]);
			}
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select dependent node:");
#else 
			modifyhdlg.SetCmdPickPromptStr("��ѡ�������ڵ�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if(modifyhdlg.DoModal()==IDOK)
			{
				CLDSNode *pAttachNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pAttachNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"The input handle is error��can't find the corresponding node��",sSoftName,MB_OK);
#else 
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
					bool updatedFirstNodeParam=false;
					CLDSNode* pFirstNode= (CLDSNode*)pView->selectObjs.GetFirst();
					for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
					{
						pNode->UpdateMirNodeAttachNode(MIRMSG(modifyhdlg.m_iMirMsg),pAttachNode,update_type);
						bool updated=false;
						bool integrity=pNode->IsIntegrityParams();
						pNode->arrRelationNode[update_type]=modifyhdlg.m_nHandle;
						bool integrity_now=pNode->IsIntegrityParams();
						pNode->Unlock();
						if(!integrity&&integrity_now)
							updated=pNode->ReverseUpdateParams();
						else if(integrity_now)
							pNode->Set();
						if(pNode==pFirstNode)
							updatedFirstNodeParam=updated;
					}
					CXhChar50 sText;
					if(updatedFirstNodeParam&&pFirstNode->m_cPosCalType==CLDSNode::COORD_SCALE)
						UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attach_scale"),CLS_NODE);
					else if(updatedFirstNodeParam&&pFirstNode->m_cPosCalType==CLDSNode::COORD_OFFSET)
						UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attach_offset"),CLS_NODE);
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
		}
		else if(CLDSNode::GetPropID("hDatumPoint")==pItem->m_idProp)
		{
			if(pFirstNode->m_cPosCalType==12)
				pFirstNode->arrRelationNode[0]=HexStrToLong(valueStr);
			else
			{
			CLDSPoint *pDatumPoint=console.FromPointHandle(pFirstNode->hDatumPoint);
			if(pDatumPoint)
			{
				CDatumPointPropertyDlg datumPointDlg;
				datumPointDlg.m_pCurEditObj=pFirstNode;
				datumPointDlg.m_xPoint=*pDatumPoint;
				if(datumPointDlg.DoModal()==IDOK)
				{
					long handle=pDatumPoint->handle;
					*pDatumPoint=datumPointDlg.m_xPoint;
					pDatumPoint->handle=handle;
				}
			}
			else
			{
				CParaDatumPointDlg datumPointDlg;
				if(datumPointDlg.DoModal()==IDOK)
				{
					pFirstNode->hDatumPoint=datumPointDlg.m_xPoint.handle;
					valueStr.Format("0X%X",pFirstNode->hDatumPoint);
					pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
				}
			}
			}
		}
		/*
#ifdef __PART_DESIGN_INC_
		else if(CLDSNode::GetPropID("m_idNodePlace")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(AfxMessageBox("Sure to add��(Y/N)",MB_YESNO)==IDNO)
#else 
			if(AfxMessageBox("ȷ��Ҫ���һ����(Y/N)",MB_YESNO)==IDNO)
#endif
				return FALSE;
			WIREPLACE* pWirePlace=pFirstNode->wires.Add(0);
			int key=pFirstNode->wires.GetKey(pWirePlace);
			CXhChar50 sPropId("accessaryNodePlace_%d",key);
			long idProp=CLDSNode::RegisterUdfPropId(sPropId);
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar50 sItemName("Wire Place%d",pFirstNode->wires.GetNodeNum());
			InsertPropItem(pPropList,pItem,pFirstNode,idProp,CString(sItemName),
				"Attached Wire Place",PIT_BUTTON,BDT_COMMONEDIT," Delete ");
#else 
			CXhChar50 sItemName("����λ%d",pFirstNode->wires.GetNodeNum());
			InsertPropItem(pPropList,pItem,pFirstNode,idProp,CString(sItemName),
				"���ӹ���λ",PIT_BUTTON,BDT_COMMONEDIT," ɾ�� ");
#endif
		}
		else if(pFirstNode->FindUdfPropKeyStrByPropId(pItem->m_idProp,sPropKey,100))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(AfxMessageBox("Sure to delete��(Y/N)",MB_YESNO)==IDNO)
#else 
			if(AfxMessageBox("ȷ��Ҫɾ����ǰ����(Y/N)",MB_YESNO)==IDNO)
#endif
				return FALSE;
			//1���ӹ�ϣ����ɾ����ǰѡ�����Ӧ������
			int key=pFirstNode->RetrieveWirePlaceKey(sPropKey);
			if(key>0)
				pFirstNode->wires.DeleteNode(key);
			//2����������������,ɾ����ǰ��
			pPropList->DeleteItemByPropId(pItem->m_idProp);
			pPropList->HideInputCtrl();
			return FALSE;	//ɾ��Ӧ��ֱ�ӷ���FALSE
			//��ǰ��ؼ�������BUTTON,ɾ�������pItem=NULL�������������TURE�����ж��ؼ����Ͳ��������ݵ��޸ģ����Ի�����쳣
		}
#endif
		*/
		else 
			return FALSE;
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	return TRUE;
}

BOOL ModifyNodePropItemHelpLiveString(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSObject *pObj=pView->selectObjs.GetFirst();
	if(pObj==NULL||pObj->GetClassTypeId()!=CLS_NODE)
		return FALSE;
	g_pSolidDraw->DestroyAllMark();
	CLDSNode *pCurNode = (CLDSNode*)pObj;
	if(pItem->m_idProp==CLDSNode::GetPropID("hFatherPart"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurNode->hFatherPart);
		return TRUE;
	}
	else
	{
		g_pSolidDraw->ReleaseSnapStatus();
		ARRAY_LIST<long> harr(0,pView->selectObjs.GetNodeNum());
		for(pObj=pView->selectObjs.GetFirst();pObj;pObj=pView->selectObjs.GetNext())
			harr.append(pObj->handle);
		g_pSolidDraw->SetEntSnapStatus(harr.m_pData,harr.GetSize());
	}
	if(pCurNode->m_cPosCalType==1||pCurNode->m_cPosCalType==2||pCurNode->m_cPosCalType==3)
	{	//�˼���ָ��ĳһ��������Ľڵ�
		if(pItem->m_idProp==CLDSNode::GetPropID("attachNode1"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[0]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The datum rod's start node handle";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ����ڻ�׼�˼���ʼ�˽ڵ���";
#endif
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode2"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[1]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The datum rod's end node handle";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ����ڻ�׼�˼����ն˽ڵ���";
#endif
		}
	}
	else if(pCurNode->m_cPosCalType==4)
	{	//���˼�����ڵ�
		if(pItem->m_idProp==CLDSNode::GetPropID("attachNode1"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[0]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The first cross datum rod's start node handle";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ�����ĵ�һ�������׼�˼���ʼ�˽ڵ���";
#endif
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode2"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[1]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The first cross datum rod's end node handle";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ�����ĵ�һ�������׼�˼����ն˽ڵ���";
#endif
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode3"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[2]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The second cross datum rod's start node handle";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ�����ĵڶ��������׼�˼���ʼ�˽ڵ���";
#endif
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode4"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[3]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The second cross datum rod's end node handle";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ�����ĵڶ��������׼�˼����ն˽ڵ���";
#endif
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(pItem->m_idProp==CLDSNode::GetPropID("m_cPosCalViceType"))
		{
			if(pCurNode->m_cPosCalViceType==1)
				pItem->m_lpNodeInfo->m_strPropHelp="Two cross line are different plane,��������X-Zƽ����ͶӰ��,������X-Zƽ����ͶӰ�߽����غ�!";
			else if(pCurNode->m_cPosCalViceType==2)
				pItem->m_lpNodeInfo->m_strPropHelp="Two cross line are different plane,��������Y-Zƽ����ͶӰ��,������Y-Zƽ����ͶӰ�߽����غ�!";
			else if(pCurNode->m_cPosCalViceType==3)												 
				pItem->m_lpNodeInfo->m_strPropHelp="Two cross line are different plane,��������X-Yƽ����ͶӰ��,������X-Yƽ����ͶӰ�߽����غ�!";
			else
				pItem->m_lpNodeInfo->m_strPropHelp="Two cross line are in the same cross plane";
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachPole1"))
			pItem->m_lpNodeInfo->m_strPropHelp="The first cross datum rod's handle��is mainly used to define angle's intersection bolt";
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachPole2"))
			pItem->m_lpNodeInfo->m_strPropHelp="The second cross datum rod's handle is mainly used to define angle's intersection bolt";
#else 
		else if(pItem->m_idProp==CLDSNode::GetPropID("m_cPosCalViceType"))
		{
			if(pCurNode->m_cPosCalViceType==1)
				pItem->m_lpNodeInfo->m_strPropHelp="��������������,��������X-Zƽ����ͶӰ��,������X-Zƽ����ͶӰ�߽����غ�!";
			else if(pCurNode->m_cPosCalViceType==2)
				pItem->m_lpNodeInfo->m_strPropHelp="��������������,��������Y-Zƽ����ͶӰ��,������Y-Zƽ����ͶӰ�߽����غ�!";
			else if(pCurNode->m_cPosCalViceType==3)												 
				pItem->m_lpNodeInfo->m_strPropHelp="��������������,��������X-Yƽ����ͶӰ��,������X-Yƽ����ͶӰ�߽����غ�!";
			else
				pItem->m_lpNodeInfo->m_strPropHelp="����������λ��ͬһ����ƽ����";
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachPole1"))
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ�����ĵ�һ�������׼�˼��������Ҫ���ڶ���Ǹֽ������˨";
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachPole2"))
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ�����ĵڶ��������׼�˼��������Ҫ���ڶ���Ǹֽ������˨";
#endif
	}
	else if(pCurNode->m_cPosCalType==5)
	{	//���˼�����ڵ�
		if(pItem->m_idProp==CLDSNode::GetPropID("attachNode1"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[0]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The start defined handle of section";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ�ȷֽڼ����ʼ�˶���ڵ���";
#endif
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode2"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[1]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The end defined handle of section";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ�ȷֽڼ����ֹ�˶���ڵ���";
#endif
		}
	}
	else if(pCurNode->m_cPosCalType==6)
	{	//�ȸ߽ڵ�
		if(pItem->m_idProp==CLDSNode::GetPropID("attachNode1"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[0]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The handle of node with the same height as the current node";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="�뵱ǰ�ڵ�ȸߵĽڵ���";
#endif
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode2"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[1]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The datum rod's start node handle";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ����ڻ�׼�˼���ʼ�˽ڵ���";
#endif
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode3"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[2]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The datum rod's end node handle";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ����ڻ�׼�˼����ն˽ڵ���";
#endif
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachPole1"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[3]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The handle of the current node's datum rod";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ����ڻ�׼�˼����";
#endif
		}
	}
	else if(pCurNode->m_cPosCalType==7)
	{	//�ظ˼���ĳ�ڵ��ƫ�Ƶ�
		if(pItem->m_idProp==CLDSNode::GetPropID("attachNode1"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[0]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The handle of the current node's offset start datum node ";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ㶨λƫ�Ƶ���ʼ��׼�ڵ���";
#endif
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode2"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[1]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The handle of the current node's offset start datum line'node ";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ㶨λƫ�ƻ�׼�ߵ���ʼ�˶���ڵ���";
#endif
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode3"))
		{
			CLDSNode* pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[2]);
			if(pAttachNode)
				g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
#ifdef AFX_TARG_ENU_ENGLISH
			pItem->m_lpNodeInfo->m_strPropHelp="The handle of the current node's offset end datum line's node";
#else 
			pItem->m_lpNodeInfo->m_strPropHelp="��ǰ�ڵ㶨λƫ�ƻ�׼�ߵ���ֹ�˶���ڵ���";
#endif
		}
	}
	else if(pCurNode->m_cPosCalType>=8||pCurNode->m_cPosCalType<=10)
	{	//�����󽻻��׼����ָ�����������
		CLDSNode* pAttachNode=NULL;
		CLDSPlane* pPlane=Ta.Plane.FromHandle(pCurNode->hRelationPlane);
		if(pItem->m_idProp==CLDSNode::GetPropID("attachNode1"))
			pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[0]);
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode2"))
			pAttachNode=console.FromNodeHandle(pCurNode->arrRelationNode[1]);
		else if(pItem->m_idProp==CLDSNode::GetPropID("hPlaneRefNode#0")&&pPlane)
			pAttachNode=console.FromNodeHandle(pPlane->hPointArr[0].handle);//hNodePoint);
		else if(pItem->m_idProp==CLDSNode::GetPropID("hPlaneRefNode#1")&&pPlane)
			pAttachNode=console.FromNodeHandle(pPlane->hPointArr[1].handle);//hNodePoint);
		else if(pItem->m_idProp==CLDSNode::GetPropID("hPlaneRefNode#2")&&pPlane)
			pAttachNode=console.FromNodeHandle(pPlane->hPointArr[2].handle);//hNodePoint);
		if(pAttachNode)
			g_pSolidDraw->NewMark(pAttachNode->ActivePosition(),5,AtomType::mkRectX);
	}
	return TRUE;
}

#include "DisplayPointProperty.h"
void UpdateNode_AttachType_TreeItem(CPropertyList *pPropList,CPropTreeItem *pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSNode *pCurNode = (CLDSNode*)pView->selectObjs.GetFirst();
	CPropTreeItem *pNewItem=NULL,*pFindItem=NULL;
	CItemInfo *lpInfo=NULL;
	long idProp=0;
	if(pCurNode==NULL||pParentItem==NULL)
		return;
	const int GROUP_GENERAL=1,GROUP_DESIGN=2;
	DWORD dwPropGroup=GetSingleWord(GROUP_GENERAL);
	char sText[MAX_TEMP_CHAR_100+1]="";
	if(pCurNode->m_cPosCalType==0)
	{	//�����������ڵ�
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_cPosCalViceType"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachNode"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachPole"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_scale"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_offset"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hRelationPlane"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hDatumPoint"));
		SetPropItemReadOnly(pCurNode,"pos.x",FALSE);
		SetPropItemReadOnly(pCurNode,"pos.y",FALSE);
		SetPropItemReadOnly(pCurNode,"pos.z",FALSE);
	}
	else if(pCurNode->m_cPosCalType==1||pCurNode->m_cPosCalType==2||pCurNode->m_cPosCalType==3)
	{	//�˼��ϵ��ƶ����������
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_cPosCalViceType"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachNode3"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachNode4"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachPole"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_scale"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_offset"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hRelationPlane"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hDatumPoint"));
		//�˼���ָ��X|Y|Z����ֵ�ڵ�
		if(pCurNode->m_cPosCalType==1)
		{
			SetPropItemReadOnly(pCurNode,"pos.x",FALSE);
			SetPropItemReadOnly(pCurNode,"pos.y",TRUE);
			SetPropItemReadOnly(pCurNode,"pos.z",TRUE);
		}
		else if(pCurNode->m_cPosCalType==2)
		{
			SetPropItemReadOnly(pCurNode,"pos.x",TRUE);
			SetPropItemReadOnly(pCurNode,"pos.y",FALSE);
			SetPropItemReadOnly(pCurNode,"pos.z",TRUE);
		}
		else if(pCurNode->m_cPosCalType==3)
		{
			SetPropItemReadOnly(pCurNode,"pos.x",TRUE);
			SetPropItemReadOnly(pCurNode,"pos.y",TRUE);
			SetPropItemReadOnly(pCurNode,"pos.z",FALSE);
		}
		pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("attachNode"),NULL);	//�����ڵ㼯��
		if(pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Dependent Node Handle";
			lpInfo->m_sButtonName="Auto Search";
#else 
			lpInfo->m_strPropName="�����ڵ���";
			lpInfo->m_sButtonName="�Զ�����";
#endif
			lpInfo->m_cmbItems="Project|X,Z��Y,Z|X,Y|X|Y|Z";
			pFindItem=pPropList->InsertItem(pParentItem,lpInfo,0,TRUE);
			pFindItem->m_idProp=CLDSNode::GetPropID("attachNode");
			pFindItem->m_dwPropGroup=dwPropGroup;
			if(CLDSNode::GetPropStatus(pFindItem->m_idProp)!=-1)
				pFindItem->m_bHideChildren = CLDSNode::GetPropStatus(pFindItem->m_idProp);
			//pFindItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Dependent Node 1";
			lpInfo->m_strPropHelp="The node which coordinate component is invariable should be located in a straight line, and the datum line should be determined by two nodes, property of dependent node 1 is one of the nodes' handle��";
#else 
			lpInfo->m_strPropName="�����ڵ�1";
			lpInfo->m_strPropHelp="ĳһ�������ֵ�̶��Ľڵ㣬Ӧλ��ĳһ��׼ֱ���ϣ����˻�׼ֱ����Ҫ�������ڵ���ȷ���������ڵ�1���Ծ���ָ���е�һ���ڵ�����";
#endif
			pNewItem=pPropList->InsertItem(pFindItem,lpInfo,0,TRUE);
			pNewItem->m_idProp=CLDSNode::GetPropID("attachNode1");
			if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
				pNewItem->m_lpNodeInfo->m_strPropValue = sText;
			pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
			pNewItem->m_dwPropGroup=dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Dependent Node 2";
			lpInfo->m_strPropHelp="The node which coordinate component is invariable should be located in a straight line, and the datum line should be determined by two nodes, property of dependent node 2 is another one of the nodes' handle��";
#else 
			lpInfo->m_strPropName="�����ڵ�2";
			lpInfo->m_strPropHelp="ĳһ�������ֵ�̶��Ľڵ㣬Ӧλ��ĳһ��׼ֱ���ϣ����˻�׼ֱ����Ҫ�������ڵ���ȷ���������ڵ�2���Ծ���ָ���е���һ���ڵ�����";
#endif
			pNewItem=pPropList->InsertItem(pFindItem,lpInfo,1,TRUE);
			pNewItem->m_idProp=CLDSNode::GetPropID("attachNode2");
			if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
				pNewItem->m_lpNodeInfo->m_strPropValue = sText;
			pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
			pNewItem->m_dwPropGroup=dwPropGroup;
		}
	}
	else if(pCurNode->m_cPosCalType==4)
	{	//�˼�����ڵ�
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_cPosCalViceType"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachNode"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_scale"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_offset"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hRelationPlane"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hDatumPoint"));
		SetPropItemReadOnly(pCurNode,"pos.x",TRUE);
		SetPropItemReadOnly(pCurNode,"pos.y",TRUE);
		SetPropItemReadOnly(pCurNode,"pos.z",TRUE);

		//�����������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Cross Same Plane|1.X-Z Plane Cross|2.Y-Z Plane Cross|3.X-Y Plane Cross";
		lpInfo->m_strPropName="Cross Calculate Type";
#else 
		lpInfo->m_cmbItems="0.���湲��|1.������X-Zƽ��|2.������Y-Zƽ��|3.������X-Yƽ��";
		lpInfo->m_strPropName="�����������";
#endif
		pNewItem=pPropList->InsertItem(pParentItem,lpInfo,0,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("m_cPosCalViceType");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		//�����ڵ㼯��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node Handle";
		lpInfo->m_sButtonName="Auto Search";
#else 
		lpInfo->m_strPropName="�����ڵ���";
		lpInfo->m_sButtonName="�Զ�����";
#endif
		pFindItem=pPropList->InsertItem(pParentItem,lpInfo,1,TRUE);
		pFindItem->m_idProp=CLDSNode::GetPropID("attachNode");
		pFindItem->m_dwPropGroup=dwPropGroup;
		if(CLDSNode::GetPropStatus(pFindItem->m_idProp)!=-1)
			pFindItem->m_bHideChildren = CLDSNode::GetPropStatus(pFindItem->m_idProp);
		//pFindItem->SetReadOnly();

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node 1";
		lpInfo->m_strPropHelp="The node which coordinate component is invariable should be located in a straight line, and the datum line should be determined by two nodes, property of dependent node 1 is one of the nodes' handle.";
#else 
		lpInfo->m_strPropName="�����ڵ�1";
		lpInfo->m_strPropHelp="ĳһ�������ֵ�̶��Ľڵ㣬Ӧλ��ĳһ��׼ֱ���ϣ����˻�׼ֱ����Ҫ�������ڵ���ȷ���������ڵ�1���Ծ���ָ���е�һ���ڵ�����";
#endif
		pNewItem=pPropList->InsertItem(pFindItem,lpInfo,0,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("attachNode1");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node 2";
		lpInfo->m_strPropHelp="The node which coordinate component is invariable should be located in a straight line, and the datum line should be determined by two nodes, property of dependent node 2 is another one of the nodes' handle.";
#else 
		lpInfo->m_strPropName="�����ڵ�2";
		lpInfo->m_strPropHelp="ĳһ�������ֵ�̶��Ľڵ㣬Ӧλ��ĳһ��׼ֱ���ϣ����˻�׼ֱ����Ҫ�������ڵ���ȷ���������ڵ�2���Ծ���ָ���е���һ���ڵ�����";
#endif
		pNewItem=pPropList->InsertItem(pFindItem,lpInfo,1,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("attachNode2");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node 3";
		lpInfo->m_strPropHelp="The node which coordinate component is invariable should be located in a straight line, and the datum line should be determined by two nodes, property of dependent node 1 isone of the nodes' handle��";
#else 
		lpInfo->m_strPropName="�����ڵ�3";
		lpInfo->m_strPropHelp="ĳһ�������ֵ�̶��Ľڵ㣬Ӧλ��ĳһ��׼ֱ���ϣ����˻�׼ֱ����Ҫ�������ڵ���ȷ���������ڵ�1���Ծ���ָ���е�һ���ڵ�����";
#endif
		pNewItem=pPropList->InsertItem(pFindItem,lpInfo,2,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("attachNode3");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node 4";
		lpInfo->m_strPropHelp="The node which coordinate component is invariable should be located in a straight line, and the datum line should be determined by two nodes, property of dependent node 2 is another one of the nodes' handle��";
#else 
		lpInfo->m_strPropName="�����ڵ�4";
		lpInfo->m_strPropHelp="ĳһ�������ֵ�̶��Ľڵ㣬Ӧλ��ĳһ��׼ֱ���ϣ����˻�׼ֱ����Ҫ�������ڵ���ȷ���������ڵ�2���Ծ���ָ���е���һ���ڵ�����";
#endif
		pNewItem=pPropList->InsertItem(pFindItem,lpInfo,3,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("attachNode4");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("attachPole"),NULL);	//�����˼�����
		if(pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Dependent Rod Handle";
#else 
			lpInfo->m_strPropName="�����˼����";
#endif
			pFindItem=pPropList->InsertItem(pParentItem,lpInfo,2,TRUE);
			pFindItem->m_idProp=CLDSNode::GetPropID("attachPole");
			pFindItem->SetReadOnly();

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Dependent Rod 1";
			lpInfo->m_strPropHelp="Node's dependent rod 1";
#else 
			lpInfo->m_strPropName="�����˼�1";
			lpInfo->m_strPropHelp="�ڵ������˼�1";
#endif
			pNewItem=pPropList->InsertItem(pFindItem,lpInfo,0,TRUE);
			pNewItem->m_idProp=CLDSNode::GetPropID("attachPole1");
			if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
				pNewItem->m_lpNodeInfo->m_strPropValue = sText;
			pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
			pNewItem->m_dwPropGroup=dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Dependent Rod 2";
			lpInfo->m_strPropHelp="Node's dependent rod 2";
#else 
			lpInfo->m_strPropName="�����˼�2";
			lpInfo->m_strPropHelp="�ڵ������˼�2";
#endif
			pNewItem=pPropList->InsertItem(pFindItem,lpInfo,1,TRUE);
			pNewItem->m_idProp=CLDSNode::GetPropID("attachPole2");
			if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
				pNewItem->m_lpNodeInfo->m_strPropValue = sText;
			pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
			pNewItem->m_dwPropGroup=dwPropGroup;
		}
	}
	else if(pCurNode->m_cPosCalType==5)
	{	//�˼��ϵı����ȷֵ�
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_cPosCalViceType"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachNode"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachPole"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_offset"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hRelationPlane"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hDatumPoint"));
		SetPropItemReadOnly(pCurNode,"pos.x",TRUE);
		SetPropItemReadOnly(pCurNode,"pos.y",TRUE);
		SetPropItemReadOnly(pCurNode,"pos.z",TRUE);

		//�����ڵ㼯��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node Handle";
		lpInfo->m_sButtonName="Auto Search";
#else 
		lpInfo->m_strPropName="�����ڵ���";
		lpInfo->m_sButtonName="�Զ�����";
#endif
		pFindItem=pPropList->InsertItem(pParentItem,lpInfo,0,TRUE);
		pFindItem->m_idProp=CLDSNode::GetPropID("attachNode");
		pFindItem->m_dwPropGroup=dwPropGroup;
		if(CLDSNode::GetPropStatus(pFindItem->m_idProp)!=-1)
			pFindItem->m_bHideChildren = CLDSNode::GetPropStatus(pFindItem->m_idProp);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node 1";
		lpInfo->m_strPropHelp="The node which coordinate component is invariable should be located in a straight line, and the datum line should be determined by two nodes, property of dependent node 1 is one of the nodes' handle��";
#else 
		lpInfo->m_strPropName="�����ڵ�1";
		lpInfo->m_strPropHelp="ĳһ�������ֵ�̶��Ľڵ㣬Ӧλ��ĳһ��׼ֱ���ϣ����˻�׼ֱ����Ҫ�������ڵ���ȷ���������ڵ�1���Ծ���ָ���е�һ���ڵ�����";
#endif
		pNewItem=pPropList->InsertItem(pFindItem,lpInfo,0,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("attachNode1");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node 2";
		lpInfo->m_strPropHelp="The node which coordinate component is invariable should be located in a straight line, and the datum line should be determined by two nodes, property of dependent node 2 is another one of the nodes' handle��";
#else 
		lpInfo->m_strPropName="�����ڵ�2";
		lpInfo->m_strPropHelp="ĳһ�������ֵ�̶��Ľڵ㣬Ӧλ��ĳһ��׼ֱ���ϣ����˻�׼ֱ����Ҫ�������ڵ���ȷ���������ڵ�2���Ծ���ָ���е���һ���ڵ�����";
#endif
		pNewItem=pPropList->InsertItem(pFindItem,lpInfo,1,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("attachNode2");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("attach_scale"),NULL);
		if(pFindItem==NULL)
		{	//��������ռ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Relative Whole Angle Scale";
			lpInfo->m_strPropHelp = "From the angle of the start point relative to the whole angle.";
#else 
			lpInfo->m_strPropName = "��������ռ����";
			lpInfo->m_strPropHelp = "��������ռ����";
#endif
			pNewItem = pPropList->InsertItem(pParentItem,lpInfo,1,TRUE);
			pNewItem->m_idProp = CLDSNode::GetPropID("attach_scale");
			if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
				pNewItem->m_lpNodeInfo->m_strPropValue = sText;
			pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
			pNewItem->m_dwPropGroup=dwPropGroup;	
		}
	}
	else if(pCurNode->m_cPosCalType==6)
	{	//�ظ˼���ĳ�ڵ�ȸߵĽڵ�
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_cPosCalViceType"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachNode"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachPole"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_scale"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_offset"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hRelationPlane"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hDatumPoint"));
		SetPropItemReadOnly(pCurNode,"pos.x",TRUE);
		SetPropItemReadOnly(pCurNode,"pos.y",TRUE);
		SetPropItemReadOnly(pCurNode,"pos.z",TRUE);

		//�����ڵ㼯��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node Handle";
#else 
		lpInfo->m_strPropName="�����ڵ���";
#endif
		pFindItem=pPropList->InsertItem(pParentItem,lpInfo,0,TRUE);
		pFindItem->m_idProp=CLDSNode::GetPropID("attachNode");
		pFindItem->m_dwPropGroup=dwPropGroup;
		if(CLDSNode::GetPropStatus(pFindItem->m_idProp)!=-1)
			pFindItem->m_bHideChildren = CLDSNode::GetPropStatus(pFindItem->m_idProp);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node 1";
		lpInfo->m_strPropHelp="The node with the same height as the current node";
#else 
		lpInfo->m_strPropName="�����ڵ�1";
		lpInfo->m_strPropHelp="�뵱ǰ�ڵ�ȸߵĽڵ㡣";
#endif
		pNewItem=pPropList->InsertItem(pFindItem,lpInfo,0,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("attachNode1");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node 2";
		lpInfo->m_strPropHelp="The handle of datum rod's start node";
#else 
		lpInfo->m_strPropName="�����ڵ�2";
		lpInfo->m_strPropHelp="��ǰ�ڵ����ڻ�׼�˼���ʼ�˽ڵ�����";
#endif
		pNewItem=pPropList->InsertItem(pFindItem,lpInfo,1,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("attachNode2");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node 3";
		lpInfo->m_strPropHelp="The handle of datum rod's end node.";
#else 
		lpInfo->m_strPropName="�����ڵ�3";
		lpInfo->m_strPropHelp="��ǰ�ڵ����ڻ�׼�˼����ն˽ڵ�����";
#endif
		pNewItem=pPropList->InsertItem(pFindItem,lpInfo,2,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("attachNode3");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("attachPole"),NULL);	//�����˼�����
		if(pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Dependent Rod Handle";
#else 
			lpInfo->m_strPropName="�����˼����";
#endif
			pFindItem=pPropList->InsertItem(pParentItem,lpInfo,1,TRUE);
			pFindItem->m_idProp=CLDSNode::GetPropID("attachPole");
			pFindItem->m_dwPropGroup=dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Dependent Rod 1";
			lpInfo->m_strPropHelp="Node's dependent rod 1";
#else 
			lpInfo->m_strPropName="�����˼�1";
			lpInfo->m_strPropHelp="�ڵ������˼�1";
#endif
			pNewItem=pPropList->InsertItem(pFindItem,lpInfo,0,TRUE);
			pNewItem->m_idProp=CLDSNode::GetPropID("attachPole1");
			if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
				pNewItem->m_lpNodeInfo->m_strPropValue = sText;
			pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
			pNewItem->m_dwPropGroup=dwPropGroup;
		}
	}
	else if(pCurNode->m_cPosCalType==7)
	{	//�ظ˼���ĳ�ڵ��ƫ�Ƶ�/�ظ˼���ĳ�ڵ�Ĵ�ֱƫ�Ƶ�
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_cPosCalViceType"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachNode"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachPole"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_scale"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hRelationPlane"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hDatumPoint"));
		SetPropItemReadOnly(pCurNode,"pos.x",TRUE);
		SetPropItemReadOnly(pCurNode,"pos.y",TRUE);
		SetPropItemReadOnly(pCurNode,"pos.z",TRUE);

		//ƫ�Ʒ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Lengh|X Axis|Y Axis|Z Axis";
		lpInfo->m_strPropName="Offset Direction";
#else 
		lpInfo->m_cmbItems="����|X��|Y��|Z��";
		lpInfo->m_strPropName="ƫ�Ʒ���";
#endif
		pNewItem=pPropList->InsertItem(pParentItem,lpInfo,0,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("m_cPosCalViceType");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		//�����ڵ㼯��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node Handle";
#else 
		lpInfo->m_strPropName="�����ڵ���";
#endif
		pFindItem=pPropList->InsertItem(pParentItem,lpInfo,1,TRUE);
		pFindItem->m_idProp=CLDSNode::GetPropID("attachNode");
		if(CLDSNode::GetPropStatus(pFindItem->m_idProp)!=-1)
			pFindItem->m_bHideChildren = CLDSNode::GetPropStatus(pFindItem->m_idProp);
		pFindItem->m_dwPropGroup=dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node 1";
		lpInfo->m_strPropHelp="The node which coordinate component is invariable should be located in a straight line, and the datum line should be determined by two nodes, property of dependent node 1 is one of the nodes' handle��";
#else 
		lpInfo->m_strPropName="�����ڵ�1";
		lpInfo->m_strPropHelp="ĳһ�������ֵ�̶��Ľڵ㣬Ӧλ��ĳһ��׼ֱ���ϣ����˻�׼ֱ����Ҫ�������ڵ���ȷ���������ڵ�1���Ծ���ָ���е�һ���ڵ�����";
#endif
		pNewItem=pPropList->InsertItem(pFindItem,lpInfo,0,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("attachNode1");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node 2";
		lpInfo->m_strPropHelp="The node which coordinate component is invariable should be located in a straight line, and the datum line should be determined by two nodes, property of dependent node 2 is another one of the nodes' handle��";
#else 
		lpInfo->m_strPropName="�����ڵ�2";
		lpInfo->m_strPropHelp="ĳһ�������ֵ�̶��Ľڵ㣬Ӧλ��ĳһ��׼ֱ���ϣ����˻�׼ֱ����Ҫ�������ڵ���ȷ���������ڵ�2���Ծ���ָ���е���һ���ڵ�����";
#endif
		pNewItem=pPropList->InsertItem(pFindItem,lpInfo,1,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("attachNode2");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Dependent Node 3";
		lpInfo->m_strPropHelp="The node which coordinate component is invariable should be located in a straight line, and the datum line should be determined by two nodes, property of dependent node 1 is one of the nodes' handle��";
#else 
		lpInfo->m_strPropName="�����ڵ�3";
		lpInfo->m_strPropHelp="ĳһ�������ֵ�̶��Ľڵ㣬Ӧλ��ĳһ��׼ֱ���ϣ����˻�׼ֱ����Ҫ�������ڵ���ȷ���������ڵ�1���Ծ���ָ���е�һ���ڵ�����";
#endif
		pNewItem=pPropList->InsertItem(pFindItem,lpInfo,2,TRUE);
		pNewItem->m_idProp=CLDSNode::GetPropID("attachNode3");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;
		pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("attach_offset"),NULL);
		if(pFindItem==NULL)
		{	//ƫ�Ƶ�ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Offset Node Offset";
			lpInfo->m_strPropHelp = "Offset node's offset";
#else 
			lpInfo->m_strPropName = "ƫ�Ƶ�ƫ�ƾ���";
			lpInfo->m_strPropHelp = "ƫ�Ƶ�ƫ�ƾ���";
#endif
			pNewItem = pPropList->InsertItem(pParentItem,lpInfo,2,TRUE);
			pNewItem->m_idProp = CLDSNode::GetPropID("attach_offset");
			if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
				pNewItem->m_lpNodeInfo->m_strPropValue = sText;
			pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
			pNewItem->m_dwPropGroup=dwPropGroup;
		}
	}
	else if(pCurNode->m_cPosCalType==8||pCurNode->m_cPosCalType==9||pCurNode->m_cPosCalType==10)
	{	//��׼����ָ��XY/YZ/XZ�����
		if(pCurNode->m_cPosCalType==8)
		{	//��׼����ָ��XY�����
			SetPropItemReadOnly(pCurNode,"pos.x",FALSE);
			SetPropItemReadOnly(pCurNode,"pos.y",FALSE);
			SetPropItemReadOnly(pCurNode,"pos.z",TRUE);
		}
		else if(pCurNode->m_cPosCalType==9)
		{	//��׼����ָ��YZ�����
			SetPropItemReadOnly(pCurNode,"pos.x",TRUE);
			SetPropItemReadOnly(pCurNode,"pos.y",FALSE);
			SetPropItemReadOnly(pCurNode,"pos.z",FALSE);
		}
		else
		{	//��׼����ָ��XZ�����
			SetPropItemReadOnly(pCurNode,"pos.x",FALSE);
			SetPropItemReadOnly(pCurNode,"pos.y",TRUE);
			SetPropItemReadOnly(pCurNode,"pos.z",FALSE);
		}
		//pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_cPosCalViceType"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_scale"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_offset"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachPole"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hDatumPoint"));
		if(pCurNode->m_cPosCalType==8&&pCurNode->m_cPosCalViceType=='I')
		{
			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("attachNode"),NULL);
			if(pFindItem==NULL)	//��׼��
			{
				lpInfo = new CItemInfo();
				pFindItem = pPropList->InsertItem(pParentItem,lpInfo,1,TRUE);
				pFindItem->m_idProp = CLDSNode::GetPropID("attachNode");
			}
			else
				lpInfo=pFindItem->m_lpNodeInfo;
			CPropTreeItem* pAttachNodesItem=pFindItem;
			//�����ڵ㼯��
			lpInfo->m_controlType=PIT_EDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Dependent Node Handle";
	#else 
			lpInfo->m_strPropName="��׼��";
	#endif
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
			pFindItem->m_dwPropGroup=dwPropGroup;
			if(CLDSNode::GetPropStatus(pFindItem->m_idProp)!=-1)
				pFindItem->m_bHideChildren = CLDSNode::GetPropStatus(pFindItem->m_idProp);

			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("attachNode1"),NULL);
			if(pFindItem==NULL)	//��׼��
			{
				lpInfo = new CItemInfo();
				pFindItem = pPropList->InsertItem(pAttachNodesItem,lpInfo,0,TRUE);
				pFindItem->m_idProp = CLDSNode::GetPropID("attachNode1");
			}
			else
				lpInfo=pFindItem->m_lpNodeInfo;
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="start tip node";
			lpInfo->m_strPropHelp="";
	#else 
			lpInfo->m_strPropName="��ʼ�ڵ�";
			lpInfo->m_strPropHelp="��׼����ʼ�˽ڵ�����";
	#endif
			if(pCurNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
			pFindItem->m_dwPropGroup=dwPropGroup;

			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("attachNode2"),NULL);
			if(pFindItem==NULL)	//��ֹ��
			{
				lpInfo = new CItemInfo();
				pFindItem = pPropList->InsertItem(pAttachNodesItem,lpInfo,1,TRUE);
				pFindItem->m_idProp = CLDSNode::GetPropID("attachNode2");
			}
			else
				lpInfo=pFindItem->m_lpNodeInfo;
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="end tip node";
			lpInfo->m_strPropHelp="";
	#else 
			lpInfo->m_strPropName="��ֹ�ڵ�";
			lpInfo->m_strPropHelp="��׼����ֹ�˽ڵ�����";
	#endif
			if(pCurNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
			pFindItem->m_dwPropGroup=dwPropGroup;
		}
		else
			pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachNode"));
		pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("hRelationPlane"),NULL);
		if(pFindItem==NULL)	//��׼��
		{
			lpInfo = new CItemInfo();
			pFindItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
			pFindItem->m_idProp = CLDSNode::GetPropID("hRelationPlane");
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
			pFindItem->m_dwPropGroup=dwPropGroup;
		}
		else
			lpInfo=pFindItem->m_lpNodeInfo;
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Plane Handle";
		lpInfo->m_strPropHelp = "Datum plane handle";
#else 
		lpInfo->m_strPropName = "��׼��";
		lpInfo->m_strPropHelp = "��׼��";
#endif
		lpInfo->m_cmbItems=CDisplayPointProperty::GetPlaneHandleStr();
		if(pCurNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		CPropTreeItem* pPlaneItem=pFindItem;
		CLDSPlane* pPlane=Ta.Plane.FromHandle(pCurNode->hRelationPlane);
		if(pPlane!=NULL&&pPlane->hPointArr[0].bSimpleNodePoint&&pPlane->hPointArr[1].bSimpleNodePoint&&pPlane->hPointArr[2].bSimpleNodePoint)
		{
			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("hPlaneRefNode#0"),NULL);
			if(pFindItem==NULL)	//��ֹ��
			{
				lpInfo = new CItemInfo();
				pFindItem = pPropList->InsertItem(pPlaneItem,lpInfo,0,TRUE);
				pFindItem->m_idProp = CLDSNode::GetPropID("hPlaneRefNode#0");
			}
			else
				lpInfo=pFindItem->m_lpNodeInfo;
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="plane's reference node 1";
			lpInfo->m_strPropHelp="";
	#else 
			lpInfo->m_strPropName="���սڵ�1";
			lpInfo->m_strPropHelp="��׼��Ķ�λ���սڵ�1��";
	#endif
			if(pCurNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
			pFindItem->SetReadOnly();
			pFindItem->m_dwPropGroup=dwPropGroup;

			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("hPlaneRefNode#1"),NULL);
			if(pFindItem==NULL)	//��ֹ��
			{
				lpInfo = new CItemInfo();
				pFindItem = pPropList->InsertItem(pPlaneItem,lpInfo,1,TRUE);
				pFindItem->m_idProp = CLDSNode::GetPropID("hPlaneRefNode#1");
			}
			else
				lpInfo=pFindItem->m_lpNodeInfo;
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="plane's reference node 2";
			lpInfo->m_strPropHelp="";
	#else 
			lpInfo->m_strPropName="���սڵ�2";
			lpInfo->m_strPropHelp="��׼��Ķ�λ���սڵ�2��";
	#endif
			if(pCurNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
			pFindItem->SetReadOnly();
			pFindItem->m_dwPropGroup=dwPropGroup;

			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("hPlaneRefNode#2"),NULL);
			if(pFindItem==NULL)	//��ֹ��
			{
				lpInfo = new CItemInfo();
				pFindItem = pPropList->InsertItem(pPlaneItem,lpInfo,2,TRUE);
				pFindItem->m_idProp = CLDSNode::GetPropID("hPlaneRefNode#2");
			}
			else
				lpInfo=pFindItem->m_lpNodeInfo;
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="plane's reference node 3";
			lpInfo->m_strPropHelp="";
	#else 
			lpInfo->m_strPropName="���սڵ�3";
			lpInfo->m_strPropHelp="��׼��Ķ�λ���սڵ�3��";
	#endif
			if(pCurNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
			pFindItem->SetReadOnly();
			pFindItem->m_dwPropGroup=dwPropGroup;
		}

		pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("m_cPosCalViceType"),NULL);
		if(pFindItem==NULL)
		{	//���յ�ԳƷ�ʽ
			lpInfo = new CItemInfo();
			pFindItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
			lpInfo->m_controlType = PIT_COMBO;
		}
		else
			lpInfo=pFindItem->m_lpNodeInfo;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Reference Point Symmetric Mode";
		lpInfo->m_cmbItems="No Symmetric Reference|X Axis Symmetry|Y Axis Symmetry|Z Axis Symmetry";
		lpInfo->m_strPropHelp = "Symmetrical reference point was generated according to the current point first, then calculate with the datum plane,then symmetrical to the original position.";
#else 
		lpInfo->m_strPropName = "���յ�ԳƷ�ʽ";
		lpInfo->m_cmbItems="�޶ԳƲ���|X��Գ�|Y��Գ�|Z��Գ�";
		lpInfo->m_strPropHelp = "�ȸ��ݵ�ǰ�����ɶԳƵĲ��յ㣬�����׼�����������Ȼ���ٶԳƻ�ԭλ�á�";
#endif
		pFindItem->m_idProp = CLDSNode::GetPropID("m_cPosCalViceType");
		if(pCurNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
			pFindItem->m_lpNodeInfo->m_strPropValue = sText;
		pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
		pFindItem->m_dwPropGroup=dwPropGroup;
	}
	else if(pCurNode->m_cPosCalType==11)
	{
		pPropList->DeleteAllSonItems(pParentItem);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Point";
		lpInfo->m_strPropHelp = "Datum point";
#else 
		lpInfo->m_strPropName = "��׼��";
		lpInfo->m_strPropHelp = "��׼��";
#endif
		pNewItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
		pNewItem->m_idProp = CLDSNode::GetPropID("hDatumPoint");
		if(pCurNode->GetPropValueStr(pNewItem->m_idProp,sText)>0)
			pNewItem->m_lpNodeInfo->m_strPropValue = sText;
		pNewItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pNewItem->m_idProp);
		pNewItem->m_dwPropGroup=dwPropGroup;
	}
	else if(pCurNode->m_cPosCalType==12)
	{
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_scale"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attach_offset"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachNode"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("attachPole"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("hRelationPlane"));
		pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("hDatumPoint"),NULL);
		if(pFindItem==NULL)	//ԭʼ�ڵ�
		{
			lpInfo = new CItemInfo();
			pFindItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
			pFindItem->m_idProp = CLDSNode::GetPropID("hDatumPoint");
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
			pFindItem->m_dwPropGroup=dwPropGroup;
		}
		else
			lpInfo=pFindItem->m_lpNodeInfo;
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Original node";
		lpInfo->m_strPropHelp = "Original node handle";
#else 
		lpInfo->m_strPropName = "ԭʼ��";
		lpInfo->m_strPropHelp = "ԭʼ��";
#endif
		if(pCurNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;

		pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("m_cPosCalViceType"),NULL);
		if(pFindItem==NULL)
		{	//���յ�ԳƷ�ʽ
			lpInfo = new CItemInfo();
			pFindItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
			lpInfo->m_controlType = PIT_COMBO;
		}
		else
			lpInfo=pFindItem->m_lpNodeInfo;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Reference Point Symmetric Mode";
		lpInfo->m_cmbItems="No Symmetric Reference|X Axis Symmetry|Y Axis Symmetry|Z Axis Symmetry";
		lpInfo->m_strPropHelp = "Symmetric axis to original node point.";
#else 
		lpInfo->m_strPropName = "�ԳƷ�ʽ";
		lpInfo->m_cmbItems="�޶Գ�|X��Գ�|Y��Գ�|Z��Գ�";
		lpInfo->m_strPropHelp = "�����ԭʼ�ڵ�ĶԳ��ᡣ";
#endif
		pFindItem->m_idProp = CLDSNode::GetPropID("m_cPosCalViceType");
		if(pCurNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
			pFindItem->m_lpNodeInfo->m_strPropValue = sText;
		pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
		pFindItem->m_dwPropGroup=dwPropGroup;
	}
	else
	{
		SetPropItemReadOnly(pCurNode,"pos.x",FALSE);
		SetPropItemReadOnly(pCurNode,"pos.y",FALSE);
		SetPropItemReadOnly(pCurNode,"pos.z",FALSE);
	}	
}
void UpdateNode_ArcLift_TreeItem(CPropertyList *pPropList,CPropTreeItem *pArcLiftItem,bool bOnlyUpdatePosition=false)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSNode *pFirstNode = (CLDSNode*)pView->selectObjs.GetFirst();
	CPropTreeItem *pFindItem=NULL;
	CItemInfo *lpInfo=NULL;
	long idProp=0;
	if(pFirstNode==NULL||pArcLiftItem==NULL)
		return;
	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_DESIGN=2;
	DWORD dwPropGroup=GetSingleWord(GROUP_GENERAL);
	if(Ta.ArcLift.FromHandle(abs(pFirstNode->ArcLiftHandle()))!=NULL)
	{	//Ԥ�������
		if(!bOnlyUpdatePosition)
		{	//����Ԥ������Ԥ��
			idProp=CLDSNode::GetPropID("ArcLiftDirection");
			pFindItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=CItemInfo::TYPE_COMBO;
				lpInfo->m_cmbType=CDT_LIST;
				lpInfo->m_strPropName = "Ԥ������";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "Prelift direction";
				lpInfo->m_cmbItems="Lift direction|Reverse direction";
#else 
				lpInfo->m_strPropHelp = "Ԥ������";
				lpInfo->m_cmbItems="����|����";
#endif
				pFindItem = pPropList->InsertItem(pArcLiftItem,lpInfo, 0,TRUE);
				pFindItem->m_idProp = idProp;
			}
			if(pFirstNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
			pFindItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		}
		//X
		idProp=CLDSNode::GetPropID("m_xArcLiftPos.x");
		pFindItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMON;
			lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "The x direction coordinates component after the node's lift arc";
#else 
			lpInfo->m_strPropHelp = "�ڵ�Ԥ���X�����������";
#endif
			pFindItem = pPropList->InsertItem(pArcLiftItem,lpInfo, -1,TRUE);
			pFindItem->m_idProp = idProp;
		}
		if(pFirstNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
			pFindItem->m_lpNodeInfo->m_strPropValue = sText;
		pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
		//pFindItem->SetReadOnly();
		pFindItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//Y
		idProp=CLDSNode::GetPropID("m_xArcLiftPos.y");
		pFindItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMON;
			lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "The y direction coordinates component after the node's lift arc";
#else 
			lpInfo->m_strPropHelp = "�ڵ�Ԥ���Y�����������";
#endif
			pFindItem = pPropList->InsertItem(pArcLiftItem,lpInfo, -1,TRUE);
			pFindItem->m_idProp = idProp;
		}
		if(pFirstNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
			pFindItem->m_lpNodeInfo->m_strPropValue = sText;
		pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
		//pFindItem->SetReadOnly();
		pFindItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//Z
		idProp=CLDSNode::GetPropID("m_xArcLiftPos.z");
		pFindItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMON;
			lpInfo->m_strPropName = "Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "The y direction coordinates component after the node's lift arc";
#else 
			lpInfo->m_strPropHelp = "�ڵ�Ԥ���Z�����������";
#endif
			pFindItem = pPropList->InsertItem(pArcLiftItem,lpInfo, -1,TRUE);
			pFindItem->m_idProp = idProp;
		}
		if(pFirstNode->GetPropValueStr(pFindItem->m_idProp,sText)>0)
			pFindItem->m_lpNodeInfo->m_strPropValue = sText;
		pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
		//pFindItem->SetReadOnly();
		pFindItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("ArcLiftDirection"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_xArcLiftPos.x"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_xArcLiftPos.y"));
		pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_xArcLiftPos.z"));
	}	
}

long RetrieveBlockHandle(const char* blockIdName)
{
	CXhChar50 keyText(blockIdName);
	long hBlock=0;
	if(!keyText.Equal("��"))
	{
		char* key=strchr(keyText,'#');
		if(key)
			key++;
		else
			key=keyText;
		if(key)
			hBlock=HexStrToLong(key);
	}
	return hBlock;
}
BOOL ModifyNodeProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSNode *pCurNode = (CLDSNode*)pView->selectObjs.GetFirst();
	CLDSNode *pNode = NULL;
	int cPosCalType = 0;
	char tem_str[MAX_TEMP_CHAR_100+1],sPropKey[101]="";
	if(!pCurNode)
		return FALSE;
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	CLogErrorLife loglife;
	const int GROUP_GENERAL=1,GROUP_DESIGN=2,GROUP_WIRELOAD=3;
	try{
		CNodeMoveOperObject movenodes;
		CStackVariant<bool> stackbool(&CNodeMoveDetector::m_bGlobalEnabled, true);
		if(pItem->m_idProp==CLDSNode::GetPropID("layer"))
		{
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				pNode->SetLayer(tem_str);
				for(RELATIVE_OBJECT *pMirObj=pNode->relativeObjs.GetFirst();pMirObj;pMirObj=pNode->relativeObjs.GetNext())
				{
					if(pMirObj->mirInfo.axis_flag&8)
						continue;	//��ת�Գ�
					CLDSNode *pMirNode=pNode->BelongModel()->FromNodeHandle(pMirObj->hObj);
					if(pMirNode)
						CalMirAtomLayer(pNode->layer(),pMirNode->layer(),pMirObj->mirInfo);
				}
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("m_bVirtualObj"))
		{
			bool virtualNode=false;
			if(valueStr.CompareNoCase("��")==0)
				virtualNode=true;
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				pNode->m_bVirtualObj=virtualNode;
				pNode->SetModified();
				for(RELATIVE_OBJECT *pRelaObj=pNode->relativeObjs.GetFirst();pRelaObj;pRelaObj=pNode->relativeObjs.GetNext())
				{
					CLDSNode* pMirNode=Ta.FromNodeHandle(pRelaObj->hObj);
					if(pMirNode)
					{
						pMirNode->m_bVirtualObj=virtualNode;
						pMirNode->SetModified();
					}
				}
			}
			return TRUE;
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("hFatherPart"))
		{
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				pNode->hFatherPart=HexStrToLong(tem_str);
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("pos.x"))
		{
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				CNodeMoveDetector detector(pNode);
				pNode->SetPositionX(atof(valueStr));
				pNode->SyncMirProp("pos");
				//pNode->ResetRelativeObjs();
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("pos.y"))
		{
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				CNodeMoveDetector detector(pNode);
				pNode->SetPositionY(atof(valueStr));
				pNode->SyncMirProp("pos");
				//pNode->ResetRelativeObjs();
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("pos.z"))
		{
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				CNodeMoveDetector detector(pNode);
				pNode->SetPositionZ(atof(valueStr));
				pNode->SyncMirProp("pos");
				//pNode->ResetRelativeObjs();
			}
		}
		//Ԥ����
		else if(pItem->m_idProp==CLDSNode::GetPropID("m_hArcLift"))
		{
			CLDSArcLift *pLift=NULL;
			CLDSNode *pFirstNode = (CLDSNode*)pView->selectObjs.GetFirst();
			CXhChar500 sText;
			for(pLift=Ta.ArcLift.GetFirst();pLift;pLift=Ta.ArcLift.GetNext())
			{
				CString sLiftName;
				sLiftName.Format("%s(%d)",pLift->name,pLift->iNo);
				if(sLiftName.CompareNoCase(valueStr)==0)
					break;
			}
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				if(pLift)
					pNode->SetArcLift(pLift->handle);
				else 
					pNode->SetArcLift(0);
				pNode->CalPosition(true);
				pNode->UpdateRelaObjArcLiftProp();
			}
			UpdateNode_ArcLift_TreeItem(pPropList,pItem);
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("iSeg"))
		{
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				pNode->iSeg = SEGI(valueStr.GetBuffer());
				pNode->SyncMirProp("iSeg");
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("ArcLiftDirection"))
		{
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				long hArcLift=abs(pNode->ArcLiftHandle());
#ifdef AFX_TARG_ENU_ENGLISH
				bool reverse=valueStr.CompareNoCase("Reverse direction")==0;
#else
				bool reverse=valueStr.CompareNoCase("����")==0;
#endif
				if(pNode->ArcLiftHandle()!=0)
					pNode->SetArcLift(reverse?-hArcLift:hArcLift);
				else 
					pNode->SetArcLift(0);
				pNode->CalPosition(true);
				pNode->UpdateRelaObjArcLiftProp();
			}
			CPropTreeItem* pArcLiftItem=NULL;
			pPropList->FindItemByPropId(pItem->m_idProp,&pArcLiftItem);
			if(pArcLiftItem)
				UpdateNode_ArcLift_TreeItem(pPropList,pArcLiftItem,true);
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("cfgword"))
		{	//���µ���ĺ��Ѿ����浽��һ���ڵ���,ʹ�������������ڵ����ĺ�
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				pNode->cfgword = pCurNode->cfgword;
				pNode->SyncMirProp("cfgword");
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("m_hBlock"))
		{
			long hBlock=RetrieveBlockHandle(valueStr);
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				pNode->m_hBlock=hBlock;
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("m_cPosCalType"))
		{
			sscanf(valueStr,"%d",&cPosCalType);
			//������������
			bool isCurrIntersLineViceType=(valueStr[1]=='I');
			if(pCurNode->m_cPosCalType==cPosCalType&&cPosCalType!=8)
				return TRUE;
			else if(pCurNode->m_cPosCalType==cPosCalType&&cPosCalType==8)
			{
				bool isIntersLineViceType=pCurNode->m_cPosCalViceType=='I';
				if(isIntersLineViceType!=isCurrIntersLineViceType)
				{
					if(pCurNode->m_cPosCalViceType==0&&isCurrIntersLineViceType)
						pCurNode->m_cPosCalViceType='I';
					else if(pCurNode->m_cPosCalViceType=='I'&&!isCurrIntersLineViceType)
						pCurNode->m_cPosCalViceType=0;
				}
			}
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				if(cPosCalType==CLDSNode::COORD_OFFSET&&(pNode->m_cPosCalType==CLDSNode::COORD_SCALE||
					(pNode->m_cPosCalType>=CLDSNode::COORD_X_SPECIFIED&&pNode->m_cPosCalType<=CLDSNode::COORD_Z_SPECIFIED)))
				{
					pNode->arrRelationNode[2]=pNode->arrRelationNode[1];
					pNode->arrRelationNode[1]=pNode->arrRelationNode[0];
					CLDSNode* pDatumNode=pNode->BelongModel()->FromNodeHandle(pNode->arrRelationNode[0]);
					if(pDatumNode)
						pNode->attach_offset=DISTANCE(pDatumNode->Position(false),pNode->Position(false));
					pNode->m_cPosCalViceType=0;
				}
				else if(cPosCalType==CLDSNode::COORD_SCALE||(
					cPosCalType>=CLDSNode::COORD_X_SPECIFIED&&cPosCalType<=CLDSNode::COORD_Z_SPECIFIED))
				{
					if(pNode->m_cPosCalType==CLDSNode::COORD_OFFSET)
					{
						pNode->arrRelationNode[0]=pNode->arrRelationNode[1];
						pNode->arrRelationNode[1]=pNode->arrRelationNode[2];
					}
					if(cPosCalType==CLDSNode::COORD_SCALE)
					{
						CLDSNode* pStartNode=pNode->BelongModel()->FromNodeHandle(pNode->arrRelationNode[0]);
						CLDSNode* pEndNode=pNode->BelongModel()->FromNodeHandle(pNode->arrRelationNode[1]);
						if(pStartNode&&pEndNode)
						{
							GEPOINT scalevec=pNode->Position(false)-pStartNode->Position(false);
							GEPOINT lenvec=pEndNode->Position(false)-pStartNode->Position(false);
							double length=lenvec.mod();
							pNode->attach_scale=(scalevec*lenvec)/(length*length);
						}
					}
				}
				pNode->m_cPosCalType = cPosCalType;
				pNode->ResetRelativeObjs();
				pNode->Unlock();
				BYTE warninglevel=logerr.WarningLevel();
				logerr.SetWarningLevel(CLogFile::WARNING_LEVEL0_NONE);	//ֻ���˲��ֲ����п��ܼ����쳣���������쳣��������wjh-2014.9.2
				pNode->Set();
				logerr.SetWarningLevel(warninglevel);
			}
			UpdateNode_AttachType_TreeItem(pPropList,pItem);
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("m_cPosCalViceType"))
		{
			if(pCurNode->m_cPosCalType==4)
				cPosCalType=valueStr[0]-'0';
			else
			{
				if(valueStr[0]>='X'&&valueStr[0]<='Z')
					cPosCalType=valueStr[0];
				else
					cPosCalType=0;
				//������������
				if(pCurNode->m_cPosCalViceType==cPosCalType)
					return TRUE;
			}
			CNodeMoveOperObject movenodes;
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				pNode->m_cPosCalViceType = cPosCalType;
				pNode->ResetRelativeObjs();
				pNode->Unlock();
				pNode->Set();
				g_pSolidDraw->ModifyDbPoint(pNode->handle,pNode->ActivePosition());
				//g_pSolidDraw->DelEnt(pNode->handle);
				//GEPOINT pos=pNode->ActivePosition();
				//g_pSolidDraw->NewPoint(pos.x,pos.y,pos.z,pNode->handle);
			}
			g_pSolidDraw->Draw();
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode1")||pItem->m_idProp==CLDSNode::GetPropID("attachNode2")||
			pItem->m_idProp==CLDSNode::GetPropID("attachNode3")||pItem->m_idProp==CLDSNode::GetPropID("attachNode4"))
		{
			int update_type=0;
			if(pItem->m_idProp==CLDSNode::GetPropID("attachNode1"))
				update_type=0;	//���������ڵ�1
			else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode2"))
				update_type=1;	//���������ڵ�2
			else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode3"))
				update_type=2;	//���������ڵ�3
			else if(pItem->m_idProp==CLDSNode::GetPropID("attachNode4"))
				update_type=3;	//���������ڵ�4
			//�����ڵ���1
			bool updatedFirstNodeParam=false;
			CLDSNode* pFirstNode= (CLDSNode*)pView->selectObjs.GetFirst();
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				//CLDSNode *pAttachNode=console.FromNodeHandle(pNode->arrRelationNode[update_type]);
				//pNode->UpdateMirNodeAttachNode(mirmsg,pAttachNode,update_type);
				bool integrity=pNode->IsIntegrityParams();
				pNode->arrRelationNode[update_type]=HexStrToLong(tem_str);
				pNode->Unlock();
				bool integrity_now=pNode->IsIntegrityParams();
				bool updated=false;
				if(!integrity&&integrity_now)
					updated=pNode->ReverseUpdateParams();
				else if(integrity_now)
					pNode->Set();
				if(pNode==pFirstNode)
					updatedFirstNodeParam=updated;
				pNode->ResetRelativeObjs();
			}
			CXhChar50 sText;
			if(updatedFirstNodeParam&&pFirstNode->m_cPosCalType==CLDSNode::COORD_SCALE)
				UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attach_scale"),CLS_NODE);
			else if(updatedFirstNodeParam&&pFirstNode->m_cPosCalType==CLDSNode::COORD_OFFSET)
				UpdatePropItemValue(pPropList,pFirstNode,CLDSNode::GetPropID("attach_offset"),CLS_NODE);
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachPole1"))
		{
			//�����ڵ�˼�1
			int i=0;
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				long hNewPole=0;
				sscanf(tem_str,"%X",&hNewPole);
				if(pNode->m_cPosCalType==4)
				{	//���½���ڵ������˼�ʱͬʱ���������ڵ�
					CLDSLinePart *pNewPole=(CLDSLinePart*)console.FromPartHandle(hNewPole,CLS_LINEPART);
					if(pNewPole&&pNode->arrRelationNode[0]==0&&pNode->arrRelationNode[1]==0)
					{	//�˼��˵�Ϊ�����ʱ����ֹ��ԭ����ȷ����Ϣ�Ĵ�
						if(pNewPole->pStart)
							pNode->arrRelationNode[0]=pNewPole->pStart->handle;
						else if(pNewPole->pEnd)
							pNode->arrRelationNode[1]=pNewPole->pEnd->handle;
						if(pNode->GetPropValueStr(CLDSNode::GetPropID("attachNode1"),tem_str)>0)
							pPropList->SetItemPropValue(CLDSNode::GetPropID("attachNode1"),CString(tem_str));
						if(pNode->GetPropValueStr(CLDSNode::GetPropID("attachNode2"),tem_str)>0)
							pPropList->SetItemPropValue(CLDSNode::GetPropID("attachNode2"),CString(tem_str));
					}
				}	
				//�޸������˼����
				pNode->arrRelationPole[0]=hNewPole;
				pNode->Unlock();
				pNode->Set();
				pNode->ResetRelativeObjs();
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachPole2"))
		{
			//�����ڵ�˼�2
			int i=0;
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				long hNewPole=0;
				sscanf(tem_str,"%X",&hNewPole);
				if(pNode->m_cPosCalType==4)
				{	//���½���ڵ������˼�ʱͬʱ���������ڵ�
					CLDSLinePart *pNewPole=(CLDSLinePart*)console.FromPartHandle(hNewPole,CLS_LINEPART);
					if(pNewPole&&pNode->arrRelationNode[2]==0&&pNode->arrRelationNode[3]==0)
					{	//�˼��˵�Ϊ�����ʱ����ֹ��ԭ����ȷ����Ϣ�Ĵ�
						if(pNewPole->pStart)
							pNode->arrRelationNode[2]=pNewPole->pStart->handle;
						else if(pNewPole->pEnd)
							pNode->arrRelationNode[3]=pNewPole->pEnd->handle;
						if(pNode->GetPropValueStr(CLDSNode::GetPropID("attachNode2"),tem_str)>0)
							pPropList->SetItemPropValue(CLDSNode::GetPropID("attachNode2"),CString(tem_str));
						if(pNode->GetPropValueStr(CLDSNode::GetPropID("attachNode3"),tem_str)>0)
							pPropList->SetItemPropValue(CLDSNode::GetPropID("attachNode3"),CString(tem_str));
					}
				}	
				//�޸������˼����
				pNode->arrRelationPole[1]=hNewPole;
				pNode->Unlock();
				pNode->Set();
				pNode->ResetRelativeObjs();
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attach_scale"))
		{
			CUndoOperObject undo(&Ta);
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				pNode->attach_scale = atof(valueStr);
				pNode->Unlock();
				pNode->Set();
				pNode->ResetRelativeObjs();
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attach_offset"))
		{
			CUndoOperObject undo(&Ta);
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				pNode->attach_offset = atof(valueStr);
				pNode->Unlock();
				pNode->Set();
				pNode->ResetRelativeObjs();
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("hRelationPlane"))
		{
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())	
				sscanf(tem_str,"%X",&pNode->hRelationPlane);
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("hDatumPoint"))
		{
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())	
				sscanf(tem_str,"%X",&pNode->hDatumPoint);
		}
#if defined(__TSA_)||defined(__LDS_)
		//���
		else if(pItem->m_idProp==CLDSNode::GetPropID("point_i"))
		{
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())	
				sscanf(tem_str,"%ld",&pNode->point_i);
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("hangingInfo"))
		{
			char cHangWireType=0;
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.Compare("Earth wire")==0)
				cHangWireType='E';
			else if(valueStr.Compare("Conduct wire")==0)
				cHangWireType='C';
			else if(valueStr.Compare("Jump wire")==0)
				cHangWireType='J';
			else
				cHangWireType=0;
#else
			if(valueStr.Compare("����")==0)
				cHangWireType='E';
			else if(valueStr.Compare("����")==0)
				cHangWireType='C';
			else if(valueStr.Compare("����")==0)
				cHangWireType='J';
			else
				cHangWireType=0;
#endif
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())	
			{
				pNode->m_cHangWireType=cHangWireType;
				pNode->SyncMirProp("hangingInfo");
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("hangingDsec"))
			pCurNode->m_sHangWireDesc.Printf("%s",valueStr);
		/*
		else if(pItem->m_idProp==CLDSNode::GetPropID("m_idNodePlace"))
		{
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
			{
				WIREPLACE* pWirePlace=pNode->wires.GetFirst();
				if(pWirePlace==NULL)
					pWirePlace=pNode->wires.Add(0);
				pWirePlace->idNodePlace.FromString(valueStr.GetString());
			}
			if(pCurNode->wires.GetNodeNum()>0)
			{
				pItem->m_lpNodeInfo->m_controlType = PIT_BUTTON;
				pItem->m_lpNodeInfo->m_buttonType  = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				pItem->m_lpNodeInfo->m_sButtonName = " Add ";
#else 
				pItem->m_lpNodeInfo->m_sButtonName = " ��� ";
#endif
			}
		}
		else if(pCurNode->FindUdfPropKeyStrByPropId(pItem->m_idProp,sPropKey,100))
		{
			WIREPLACE* pWirePlace=pCurNode->wires.GetValue(pCurNode->RetrieveWirePlaceKey(sPropKey));
			if(pWirePlace)
				pWirePlace->idNodePlace.FromString(valueStr.GetString());
		}
		*/
		else if(pItem->m_idProp==CLDSNode::GetPropID("force_type"))
		{
			NODE_FORCE_TYPE force_type=AUTO_NODE; 
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("1.Force Node")==0)
				force_type=FORCE_NODE;
			else if(valueStr.CompareNoCase("2.Swing Node")==0)
#else 
			if(valueStr.CompareNoCase("1.�����ڵ�")==0)
				force_type=FORCE_NODE;
			else if(valueStr.CompareNoCase("2.�ڶ��ڵ�")==0)
#endif
				force_type=SWING_NODE;
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())	
				pNode->force_type=force_type;
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachForeverLoad_X"))
		{
			double fx=atof(valueStr);
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())	
				pNode->PermanentLoad.x=fx*1000;
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachForeverLoad_Y"))
		{
			double fy=atof(valueStr);
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())	
				pNode->PermanentLoad.y=fy*1000;
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachForeverLoad_Z"))
		{
			double fz=atof(valueStr);
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())	
				pNode->PermanentLoad.z=fz*1000;
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("permanentFz"))
		{
			double permanentFz=atof(valueStr);
			CWorkStatus *pStatus=Ta.GetActiveWorkStatus();
			if(pStatus)
			{
				CExternalNodeLoad nodeLoad;
				nodeLoad.permanentFz=permanentFz*1000;
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())	
				{
					CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
					if(pNodeLoad)
						pNodeLoad->permanentFz=nodeLoad.permanentFz;
					else
						pStatus->hashNodeLoad.SetValue(pNode->handle,nodeLoad);
				}
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachVolLoad_X"))
		{
			double fx=atof(valueStr);
			CWorkStatus *pStatus=Ta.GetActiveWorkStatus();
			if(pStatus)
			{
				CExternalNodeLoad nodeLoad;
				if(pStatus->m_bLoadIncFactor)
					nodeLoad.designLoad.x=fx*1000;
				else
					nodeLoad.Fxyz.x=fx*1000;
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())	
				{
					CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
					if(pNodeLoad)
					{
						if(pStatus->m_bLoadIncFactor)
							pNodeLoad->designLoad.x=fx*1000;
						else
							pNodeLoad->Fxyz.x=nodeLoad.Fxyz.x;
					}
					else
						pStatus->hashNodeLoad.SetValue(pNode->handle,nodeLoad);
				}
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachVolLoad_Y"))
		{
			double fy=atof(valueStr);
			CWorkStatus *pStatus=Ta.GetActiveWorkStatus();
			if(pStatus)
			{
				CExternalNodeLoad nodeLoad;
				if(pStatus->m_bLoadIncFactor)
					nodeLoad.designLoad.y=fy*1000;
				else
					nodeLoad.Fxyz.y=fy*1000;
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())	
				{
					CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
					if(pNodeLoad)
					{
						if(pStatus->m_bLoadIncFactor)
							pNodeLoad->designLoad.y=fy*1000;
						else
							pNodeLoad->Fxyz.y=nodeLoad.Fxyz.y;
					}
					else
						pStatus->hashNodeLoad.SetValue(pNode->handle,nodeLoad);
				}
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("attachVolLoad_Z"))
		{
			double fz=atof(valueStr);
			CWorkStatus *pStatus=Ta.GetActiveWorkStatus();
			if(pStatus)
			{
				CExternalNodeLoad nodeLoad;
				if(pStatus->m_bLoadIncFactor)
					nodeLoad.designLoad.z=fz*1000;
				else
					nodeLoad.Fxyz.z=fz*1000;
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())	
				{
					CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
					if(pNodeLoad)
					{
						if(pStatus->m_bLoadIncFactor)
							pNodeLoad->designLoad.z=fz*1000;
						else
							pNodeLoad->Fxyz.z=nodeLoad.Fxyz.z;
					}
					else
						pStatus->hashNodeLoad.SetValue(pNode->handle,nodeLoad);
				}
			}
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("restrict_all"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.Compare("Yes")==0)
#else 
			if(valueStr.Compare("��")==0)
#endif
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(ALL_RESTRICT);
					pNode->SyncMirProp("restrict");
				}
			}
			else
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(ALL_RESTRICT,FALSE);
					pNode->SyncMirProp("restrict");
				}
			}
			CLDSNode* pFirstNode=(CLDSNode*)pView->selectObjs.GetFirst();
			SetPartMutiObjsProp(pFirstNode,"restrict_x");
			SetPartMutiObjsProp(pFirstNode,"restrict_y");
			SetPartMutiObjsProp(pFirstNode,"restrict_z");
			SetPartMutiObjsProp(pFirstNode,"restrict_rot_x");
			SetPartMutiObjsProp(pFirstNode,"restrict_rot_y");
			SetPartMutiObjsProp(pFirstNode,"restrict_rot_z");
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("restrict_x"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.Compare("Yes")==0)
#else 
			if(valueStr.Compare("��")==0)
#endif
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(X_RESTRICT);
					pNode->SyncMirProp("restrict");
				}
			}
			else
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(X_RESTRICT,FALSE);
					pNode->SyncMirProp("restrict");
				}
			}
			CLDSNode* pFirstNode=(CLDSNode*)pView->selectObjs.GetFirst();
			SetPartMutiObjsProp(pFirstNode,"restrict_all");
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("restrict_y"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.Compare("Yes")==0)
#else 
			if(valueStr.Compare("��")==0)
#endif
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(Y_RESTRICT);
					pNode->SyncMirProp("restrict");
				}
			}
			else
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(Y_RESTRICT,FALSE);
					pNode->SyncMirProp("restrict");
				}
			}
			CLDSNode* pFirstNode=(CLDSNode*)pView->selectObjs.GetFirst();
			SetPartMutiObjsProp(pFirstNode,"restrict_all");
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("restrict_z"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.Compare("Yes")==0)
#else 
			if(valueStr.Compare("��")==0)
#endif
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(Z_RESTRICT);
					pNode->SyncMirProp("restrict");
				}
			}
			else
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(Z_RESTRICT,FALSE);
					pNode->SyncMirProp("restrict");
				}
			}
			CLDSNode* pFirstNode=(CLDSNode*)pView->selectObjs.GetFirst();
			SetPartMutiObjsProp(pFirstNode,"restrict_all");
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("restrict_rot_x"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.Compare("Yes")==0)
#else 
			if(valueStr.Compare("��")==0)
#endif
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(X_ROT_RESTRICT);
					pNode->SyncMirProp("restrict");
				}
			}
			else
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(X_ROT_RESTRICT,FALSE);
					pNode->SyncMirProp("restrict");
				}
			}
			CLDSNode* pFirstNode=(CLDSNode*)pView->selectObjs.GetFirst();
			SetPartMutiObjsProp(pFirstNode,"restrict_all");
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("restrict_rot_y"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.Compare("Yes")==0)
#else 
			if(valueStr.Compare("��")==0)
#endif
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(Y_ROT_RESTRICT);
					pNode->SyncMirProp("restrict");
				}
			}
			else
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(Y_ROT_RESTRICT,FALSE);
					pNode->SyncMirProp("restrict");
				}
			}
			CLDSNode* pFirstNode=(CLDSNode*)pView->selectObjs.GetFirst();
			SetPartMutiObjsProp(pFirstNode,"restrict_all");
		}
		else if(pItem->m_idProp==CLDSNode::GetPropID("restrict_rot_z"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.Compare("Yes")==0)
#else 
			if(valueStr.Compare("��")==0)
#endif
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(Z_ROT_RESTRICT);
					pNode->SyncMirProp("restrict");
				}
			}
			else
			{
				for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				{
					pNode->SetRestrict(Z_ROT_RESTRICT,FALSE);
					pNode->SyncMirProp("restrict");
				}
			}
			CLDSNode* pFirstNode=(CLDSNode*)pView->selectObjs.GetFirst();
			SetPartMutiObjsProp(pFirstNode,"restrict_all");
		}
#endif
		else 
			return FALSE;
		if( pItem->m_idProp==CLDSNode::GetPropID("pos.x")||
			pItem->m_idProp==CLDSNode::GetPropID("pos.y")||
			pItem->m_idProp==CLDSNode::GetPropID("pos.z")||
			pItem->m_idProp==CLDSNode::GetPropID("attach_scale")||
			pItem->m_idProp==CLDSNode::GetPropID("attach_offset"))
		{
			g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
			for(pNode=(CLDSNode*)pView->selectObjs.GetFirst();pNode;pNode=(CLDSNode*)pView->selectObjs.GetNext())
				g_pSolidDraw->SetEntSnapStatus(pNode->handle);
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	return TRUE;
}
CString GetBlockListStr()
{
	CString listStr;
#ifdef AFX_TARG_ENU_ENGLISH
	listStr.Append("None");
#else
	listStr.Append("��");
#endif
	for(CBlockModel* pBlock=Ta.Block.GetFirst();pBlock;pBlock=Ta.Block.GetNext())
	{
		if(pBlock->IsEmbeded())
		{
			if(listStr.GetLength()>0)
				listStr.AppendChar('|');
			listStr.Append(CXhChar50("%s#0X%X",pBlock->name,pBlock->handle));
		}
	}
	return listStr;
}
void CLDSView::DisplayNodeProperty(BOOL bRebuild)
{
	CItemInfo *lpInfo;
	CLDSNode *pFirstNode = (CLDSNode*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pSonPropItem,*pRoot=pPropList->GetRootItem();
	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_DESIGN=2,GROUP_WIRELOAD=3;
	if(bRebuild)
	{	//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
#if defined(__TSA_)||defined(__LDS_)
		pPropDlg->m_arrPropGroupLabel.SetSize(2);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DESIGN-1,"Design");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DESIGN-1,"���");
#endif
		//pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_WIRELOAD-1,"��������");
		pPropDlg->RefreshTabCtrl(CLDSNode::m_iCurDisplayPropGroup);
#else
		pPropDlg->m_arrPropGroupLabel.SetSize(0);
		pPropDlg->RefreshTabCtrl(0);
#endif
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_NODE;
		pPropList->SetModifyValueFunc(ModifyNodeProperty);
		pPropList->SetButtonClickFunc(NodeButtonClick);
		pPropList->SetModifyStatusFunc(ModifyNodeItemStatus);
		pPropList->SetPropHelpPromptFunc(ModifyNodePropItemHelpLiveString);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstNode->BelongModel()->IsTowerModel()||pFirstNode->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;

		//�ڵ������Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Node Basic Information";
		lpInfo->m_strPropHelp = "The basic information of node";
#else 
		lpInfo->m_strPropName = "�ڵ������Ϣ";
		lpInfo->m_strPropHelp = "�ڵ������Ϣ";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSNode::GetPropID("basicInfo");
		if(CLDSNode::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSNode::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGN)|GetSingleWord(GROUP_WIRELOAD);
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "The handle of the node";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�ڵ���";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSNode::GetPropID("handle");
		if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGN)|GetSingleWord(GROUP_WIRELOAD);
#if defined(__TSA_)||defined(__LDS_)
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Label Code";
		lpInfo->m_strPropHelp = "Node label code"; 
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�ڵ���"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSNode::GetPropID("point_i");
		if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGN)|GetSingleWord(GROUP_WIRELOAD);
		//������Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_strPropName = "���߽ڵ�";
		lpInfo->m_strPropHelp = "�Ƿ�ҽ��е����߻����ߣ���Ҫ���ں��ؼ���ʱʶ��ҵ�";
		lpInfo->m_controlType=CItemInfo::TYPE_COMBO;
		lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="None|Earth wire|Conduct wire|Jump wire";
#else
		lpInfo->m_cmbItems="��|����|����|����";
#endif
		CPropTreeItem *pWireLoadGroupItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pWireLoadGroupItem->m_idProp = CLDSNode::GetPropID("hangingInfo");
		if(pFirstNode->GetPropValueStr(pWireLoadGroupItem->m_idProp,sText)>0)
			pWireLoadGroupItem->m_lpNodeInfo->m_strPropValue = sText;
		if(CLDSNode::GetPropStatus(pWireLoadGroupItem->m_idProp)!=-1)
			pWireLoadGroupItem->m_bHideChildren = CLDSNode::GetPropStatus(pWireLoadGroupItem->m_idProp);
		pWireLoadGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
		//������λ
		/*lpInfo = new CItemInfo();
		if(pFirstNode->wires.GetNodeNum()<=0)
			lpInfo->m_controlType = PIT_EDIT;
		else 
		{
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType  = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_sButtonName = " Add ";
#else 
			lpInfo->m_sButtonName = " ��� ";
#endif
		}
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Hang Wire Place Serial Number";
		lpInfo->m_strPropHelp = "Basic hang wire serial number,format is \"stringing scheme -hang wire place serial number\""; 
#else 
		lpInfo->m_strPropName = "��λ���";
		lpInfo->m_strPropHelp = "������λ���,��ʽΪ\"���߷�����-��λ���\""; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSNode::GetPropID("m_idNodePlace");
		if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGN)|GetSingleWord(GROUP_WIRELOAD);
		//���ӹ�λ
		int index=2;
		WIREPLACE* pWirePlace=pFirstNode->wires.GetFirst();
		for(pWirePlace=pFirstNode->wires.GetNext();pWirePlace;pWirePlace=pFirstNode->wires.GetNext(),index++)
		{
			int key=pFirstNode->wires.GetCursorKey();
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar50 sPropId("accessaryNodePlace_%d",key),sItemName("Hang Wire Place%d",index);
#else 
			CXhChar50 sPropId("accessaryNodePlace_%d",key),sItemName("����λ%d",index);
#endif
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_sButtonName=" Delete ";
			lpInfo->m_strPropName = sItemName;
			lpInfo->m_strPropHelp = "Attached hang wire place"; 
#else 
			lpInfo->m_sButtonName=" ɾ�� ";
			lpInfo->m_strPropName = sItemName;
			lpInfo->m_strPropHelp = "���ӹ���λ"; 
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSNode::RegisterUdfPropId(sPropId);
			if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGN)|GetSingleWord(GROUP_WIRELOAD);
		}*/
		//�ҵ�����
		lpInfo = new CItemInfo();
		lpInfo->m_strPropName = "�ҵ�����";
		lpInfo->m_strPropHelp = "�ҵ㴦�Ĺ�������(ǰ������)";
		lpInfo->m_controlType = PIT_EDIT;
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSNode::GetPropID("hangingDsec");
		if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
		//�ҽ�����
		/*lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�ҽ�����";
		lpInfo->m_strPropHelp = "�ҽӹ�������"; 
		lpInfo->m_cmbItems = "����|����|����|δ����";
		CPropTreeItem *pHanginStatePropItem = pPropList->InsertItem(pWireLoadGroupItem,lpInfo, -1);
		pHanginStatePropItem->m_idProp = CLDSNode::GetPropID("hangingWireType");
		if(pFirstNode->GetPropValueStr(pHanginStatePropItem->m_idProp,sText)>0)
		pHanginStatePropItem->m_lpNodeInfo->m_strPropValue = sText;
		pHanginStatePropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pHanginStatePropItem->m_idProp);
		pHanginStatePropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
		//if(pFirstNode->wireInfo.m_cXianType!=0)
		UpdateWireLoadInfoItem(pPropList,pHanginStatePropItem,1);*/
#endif
		//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "The node on the layer"; 
#else 
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "�ڵ�����ͼ��"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSNode::GetPropID("layer");
		if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
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
		pPropItem->m_idProp = CLDSNode::GetPropID("cfgword");
		if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
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
		pPropItem->m_idProp = CLDSNode::GetPropID("iSeg");
		if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//���˼�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Father Rod";
		lpInfo->m_strPropHelp = "The handle of the node's father rod";
#else 
		lpInfo->m_strPropName = "���˼�";
		lpInfo->m_strPropHelp = "�ڵ�ĸ��˼��ľ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSNode::GetPropID("hFatherPart");
		if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//����ڵ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual Node";
		lpInfo->m_strPropHelp = "Virtual Node";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "����ڵ�";
		lpInfo->m_strPropHelp = "����ڵ�";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSNode::GetPropID("m_bVirtualObj");
		if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Original Position";
		lpInfo->m_strPropHelp = "Original Position";
#else 
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "�ڵ�����";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_idProp = CLDSNode::GetPropID("pos");
		if(CLDSNode::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSNode::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		pPropItem->SetReadOnly(FALSE);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp = "The coordinate component of node's X direction";
#else 
		lpInfo->m_strPropHelp = "�ڵ�X�����������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSNode::GetPropID("pos.x");
		if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->SetReadOnly();
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp = "The coordinate component of node's Y direction";
#else 
		lpInfo->m_strPropHelp = "�ڵ�Y�����������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSNode::GetPropID("pos.y");
		if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->SetReadOnly();
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp = "The coordinate component of node's Z direction";
#else 
		lpInfo->m_strPropHelp = "�ڵ�Z�����������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSNode::GetPropID("pos.z");
		if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->SetReadOnly();
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
#ifdef __COMMON_PART_INC_
		if(!theApp.m_bAnalysisWorkEnvOnly)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Lift Arc Coordinate";
			lpInfo->m_strPropHelp = "Prelift coordinate";
#else 
			lpInfo->m_strPropName = "Ԥ������";
			lpInfo->m_strPropHelp = "�ڵ�Ԥ��������";
#endif
			lpInfo->m_cmbType=CDT_LIST;
			lpInfo->m_cmbItems=MakeArcLiftSetString();
			CPropTreeItem *pArcLiftItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pArcLiftItem->m_bHideChildren = FALSE;
			pArcLiftItem->m_idProp = CLDSNode::GetPropID("m_hArcLift");
			if(pFirstNode->GetPropValueStr(pArcLiftItem->m_idProp,sText)>0)
				pArcLiftItem->m_lpNodeInfo->m_strPropValue = sText;
			if(CLDSNode::GetPropStatus(pPropItem->m_idProp)!=-1)
				pArcLiftItem->m_bHideChildren = CLDSNode::GetPropStatus(pArcLiftItem->m_idProp);
			pArcLiftItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
			UpdateNode_ArcLift_TreeItem(pPropList,pArcLiftItem);
		}
#endif
#if defined(__TSA_)||defined(__LDS_)
		if(!theApp.m_bDrawingWorkEnvOnly)
		{	//�ڵ㴫������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Node Force Judgement";
			lpInfo->m_strPropHelp = "Normal select default��Individual circumstances can choose force nodes contributing to calculate node to force form��"; 
			lpInfo->m_cmbItems = "0.Default|1.Force Node|2.Swing Node";
#else 
			lpInfo->m_strPropName = "�ڵ㴫���ж�";
			lpInfo->m_strPropHelp = "һ���ѡ'�Զ��ж�'�����������ѡ��'�����ڵ�'ǿ�ƽڵ��Դ����ڵ���ʽ��Ԥ���㡣"; 
			lpInfo->m_cmbItems = "0.�Զ��ж�|1.�����ڵ�|2.�ڶ��ڵ�";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("force_type");
			if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
		}
#endif
		if(!pFirstNode->IsShadowObject())
		{	//�����ڵ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Relative Node";
			lpInfo->m_strPropHelp = "The node associated with the current node and symmetrical mode";
#else 
			lpInfo->m_strPropName = "�����ڵ�";
			lpInfo->m_strPropHelp = "�뵱ǰ�ڵ�������Ľڵ��Լ��ԳƷ�ʽ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("relativeObjs");
			if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		}
		//
		if(pFirstNode->IsShadowObject())
		{
			//Ӱ����Ϣ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Information";
			lpInfo->m_strPropHelp = "Project information";
#else 
			lpInfo->m_strPropName = "Ӱ����Ϣ";
			lpInfo->m_strPropHelp = "Ӱ����Ϣ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("shadowInfo");
			pPropItem->m_bHideChildren = FALSE;
			if(CLDSNode::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSNode::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			pPropItem->SetReadOnly(FALSE);
			//Ӱ��ĸ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Mother Node";
			lpInfo->m_strPropHelp = "The current node is project mother node .The current node is original node when the handle is Ox0.";
#else 
			lpInfo->m_strPropName = "Ӱ��ĸ�ڵ�";
			lpInfo->m_strPropHelp = "��ǰ�ڵ��Ӧ��Ӱ��ĸ�ڵ������þ��Ϊ0x0ʱ��ʾ��ǰ�ڵ�Ϊԭʼ�ڵ㡣";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSNode::GetPropID("m_hMotherObj");
			if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstNode->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstNode->IsBlockRefShadowObject())
			{
				//������������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block Assembly";
				lpInfo->m_strPropHelp = "The assemble vest in block's handle.";
#else 
				lpInfo->m_strPropName = "������������";
				lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSNode::GetPropID("m_hBlockRef");
				if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstNode->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else 
		{
			//��������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			//lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Vest In Block Handle";
			lpInfo->m_strPropHelp = "The vest in block's handle.";
#else 
			lpInfo->m_strPropName = "��������";
			lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
			lpInfo->m_cmbItems=GetBlockListStr();
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("m_hBlock");
			if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstNode->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
#ifndef __TSA_
		if(!theApp.m_bAnalysisWorkEnvOnly)
		{	//λ�����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Father Angle Stadia Setting";
			lpInfo->m_strPropHelp = "The stadia setting of node's parent angle";
#else 
			lpInfo->m_strPropName = "׼���趨";
			lpInfo->m_strPropHelp = "�ڵ����ڸ��Ǹ�׼���趨";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("xFatherAngleZhunJu");
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		}
#endif
		//������ϵ	
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Dependent Relation";
		lpInfo->m_strPropHelp = "Node Dependent Relation";
#else 
		lpInfo->m_strPropName = "������ϵ";
		lpInfo->m_strPropHelp = "�ڵ�������ϵ";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSNode::GetPropID("attachRelation");
		if(CLDSNode::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSNode::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//������������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Dependent Type";
		lpInfo->m_strPropHelp = "The dependent type of node coordinate";
		lpInfo->m_cmbItems = 
			"0-No Dependent|1-X Coordinate Invariant|2-Y Coordinate Invariant|3-Z Coordinate Invariant|"
			"4-Two Rod Intersection|5-Proportion|6-With One Node Same Height|7-With One Node Offset Component|"
			"8I-Intersection of line and plane|"
			"8-X&Y Coordinate Invariant|9-Z&Y Coordinate Invariant|10-X&Z Coordinate Invariant|"\
			"11-Parametric Position|12-Symmetric Creation|13-Same coord to";
#else 
		lpInfo->m_strPropName = "������������";
		lpInfo->m_strPropHelp = "�ڵ�������������";
		lpInfo->m_cmbItems = 
			"0-�������κ������Ľڵ�|1-�˼���ָ��X�����|2-�˼���ָ��Y�����|3-�˼���ָ��Z�����|"
			"4-���˼����潻��|5-�˼��ϵı����ȷֵ�|6-�ظ˼���ĳ�ڵ�ȸߵĽڵ�|7-�ظ˼���ĳ�ڵ��ƫ�Ƶ�|"
			"8I-ָ�����潻��|"
			"8 -��׼����ָ��XY�����|9 -��׼����ָ��YZ�����|10-��׼����ָ��XZ�����|11-ָ����׼��|"\
			"12-�Գ�����|13-�������";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSNode::GetPropID("m_cPosCalType");
		if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		UpdateNode_AttachType_TreeItem(pPropList,pPropItem);
#if defined(__TSA_)||defined(__LDS_)
		if(!theApp.m_bDrawingWorkEnvOnly)
		{
			//���Ӻ���
			lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Attach Load";
			lpInfo->m_strPropHelp = "Attached Load";
#else 
			lpInfo->m_strPropName = "���Ӻ���";
			lpInfo->m_strPropHelp = "���Ӻ���";
#endif
			pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
			pGroupItem->m_idProp = CLDSNode::GetPropID("attachLoad");
			if(CLDSNode::GetPropStatus(pGroupItem->m_idProp)!=-1)
				pGroupItem->m_bHideChildren = CLDSNode::GetPropStatus(pGroupItem->m_idProp);
			pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//���������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Permanent External Load(kN)";
			lpInfo->m_strPropHelp = "Permanent external load";
#else 
			lpInfo->m_strPropName = "���������(kN)";
			lpInfo->m_strPropHelp = "���������";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("attachForeverLoad");
			if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->SetReadOnly();
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//X����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Component";
			lpInfo->m_strPropHelp = "X Component";
#else 
			lpInfo->m_strPropName = "X����";
			lpInfo->m_strPropHelp = "X����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSNode::GetPropID("attachForeverLoad_X");
			if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//Y����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Component";
			lpInfo->m_strPropHelp = "Y Component";
#else 
			lpInfo->m_strPropName = "Y����";
			lpInfo->m_strPropHelp = "Y����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSNode::GetPropID("attachForeverLoad_Y");
			if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//Z����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Component";
			lpInfo->m_strPropHelp = "Z Component";
#else 
			lpInfo->m_strPropName = "Z����";
			lpInfo->m_strPropHelp = "Z����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSNode::GetPropID("attachForeverLoad_Z");
			if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//�ɱ������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Variable External Load(kN)";
			lpInfo->m_strPropHelp = "Variable External Load";
#else 
			lpInfo->m_strPropName = "�ɱ������(kN)";
			lpInfo->m_strPropHelp = "�ɱ������";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("attachVolLoad");
			if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//X����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Component";
			lpInfo->m_strPropHelp = "X Component";
#else 
			lpInfo->m_strPropName = "X����";
			lpInfo->m_strPropHelp = "X����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSNode::GetPropID("attachVolLoad_X");
			if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//Y����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Component";
			lpInfo->m_strPropHelp = "Y Component";
#else 
			lpInfo->m_strPropName = "Y����";
			lpInfo->m_strPropHelp = "Y����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSNode::GetPropID("attachVolLoad_Y");
			if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//Z����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Component";
			lpInfo->m_strPropHelp = "Z Component";
#else 
			lpInfo->m_strPropName = "Z����";
			lpInfo->m_strPropHelp = "Z����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSNode::GetPropID("attachVolLoad_Z");
			if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//Z2����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z_2 Component";
			lpInfo->m_strPropHelp = "The variable load in permanent direction of gravity load";
#else 
			lpInfo->m_strPropName = "Z_2����";
			lpInfo->m_strPropHelp = "�ɱ������׼���������������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSNode::GetPropID("permanentFz");
			if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);

			//λ��Լ��
			lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Node Restrict";
			lpInfo->m_strPropHelp = "Node restrict";
#else 
			lpInfo->m_strPropName = "Լ��";
			lpInfo->m_strPropHelp = "Լ��";
#endif
			pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
			pGroupItem->m_idProp = CLDSNode::GetPropID("restrict");
			if(CLDSNode::GetPropStatus(pGroupItem->m_idProp)!=-1)
				pGroupItem->m_bHideChildren = CLDSNode::GetPropStatus(pGroupItem->m_idProp);
			pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//ȫ��Լ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "All";
			lpInfo->m_strPropHelp = "X/Y/Z axis direction displacement restrict";
			lpInfo->m_cmbItems = "Yes|No";
#else 
			lpInfo->m_strPropName = "ȫ��";
			lpInfo->m_strPropHelp = "X/Y/Z����λ��Լ��";
			lpInfo->m_cmbItems = "��|��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("restrict_all");
			if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//X����λ��Լ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Axis Direction Displacement Restrict";
			lpInfo->m_strPropHelp = "X Axis Direction Displacement Restrict";
			lpInfo->m_cmbItems = "Yes|No";
#else 
			lpInfo->m_strPropName = "X����λ��Լ��";
			lpInfo->m_strPropHelp = "X����λ��Լ��";
			lpInfo->m_cmbItems = "��|��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("restrict_x");
			if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//Y����λ��Լ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Axis Direction Displacement Restrict";
			lpInfo->m_strPropHelp = "Y Axis Direction Displacement Restrict";
			lpInfo->m_cmbItems = "Yes|No";
#else 
			lpInfo->m_strPropName = "Y����λ��Լ��";
			lpInfo->m_strPropHelp = "Y����λ��Լ��";
			lpInfo->m_cmbItems = "��|��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("restrict_y");
			if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//Z����λ��Լ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Axis Direction Displacement Restrict";
			lpInfo->m_strPropHelp = "Z Axis Direction Displacement Restrict";
			lpInfo->m_cmbItems = "Yes|No";
#else 
			lpInfo->m_strPropName = "Z����λ��Լ��";
			lpInfo->m_strPropHelp = "Z����λ��Լ��";
			lpInfo->m_cmbItems = "��|��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("restrict_z");
			if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//X������תԼ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Axis Direction Rotation Restrict";
			lpInfo->m_strPropHelp = "X Axis Direction Rotation Restrict";
			lpInfo->m_cmbItems = "Yes|No";
#else 
			lpInfo->m_strPropName = "X������תԼ��";
			lpInfo->m_strPropHelp = "X������תԼ��";
			lpInfo->m_cmbItems = "��|��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("restrict_rot_x");
			if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//Y����λ��Լ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Axis Direction Rotation Restrict";
			lpInfo->m_strPropHelp = "Y Axis Direction Rotation Restrict";
			lpInfo->m_cmbItems = "Yes|No";
#else 
			lpInfo->m_strPropName = "Y������תԼ��";
			lpInfo->m_strPropHelp = "Y������תԼ��";
			lpInfo->m_cmbItems = "��|��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("restrict_rot_y");
			if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			//Z����λ��Լ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Axis Direction Rotation Restrict";
			lpInfo->m_strPropHelp = "Z Axis Direction Rotation Restrict";
			lpInfo->m_cmbItems = "Yes|No";
#else 
			lpInfo->m_strPropName = "Z������תԼ��";
			lpInfo->m_strPropHelp = "Z������תԼ��";
			lpInfo->m_cmbItems = "��|��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSNode::GetPropID("restrict_rot_z");
			if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
		}
#endif
	}
	else
	{	//����Ҫ�ؽ���������ҳ
		CPropTreeItem *pFindItem,*pParentItem;
		SetPartMutiObjsProp(pFirstNode,"handle");
		SetPartMutiObjsProp(pFirstNode,"layer");
		SetPartMutiObjsProp(pFirstNode,"hFatherPart");
		SetPartMutiObjsProp(pFirstNode,"hangingInfo");
		SetPartMutiObjsProp(pFirstNode,"m_bVirtualObj");
		SetPartMutiObjsProp(pFirstNode,"pos.x");
		SetPartMutiObjsProp(pFirstNode,"pos.y");
		SetPartMutiObjsProp(pFirstNode,"pos.z");
		if(!theApp.m_bAnalysisWorkEnvOnly)
		{
			SetPartMutiObjsProp(pFirstNode,"m_hArcLift");
			SetPartMutiObjsProp(pFirstNode,"m_xArcLiftPos.x");
			SetPartMutiObjsProp(pFirstNode,"m_xArcLiftPos.y");
			SetPartMutiObjsProp(pFirstNode,"m_xArcLiftPos.z");
		}
		SetPartMutiObjsProp(pFirstNode,"iSeg");
		SetPartMutiObjsProp(pFirstNode,"point_i");
		SetPartMutiObjsProp(pFirstNode,"hangingInfo");
		SetPartMutiObjsProp(pFirstNode,"hangingDsec");
		/*
		SetPartMutiObjsProp(pFirstNode,"m_idNodePlace");
#ifdef __PART_DESIGN_INC_
		pFindItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("m_idNodePlace"),&pParentItem);
		if(pFindItem!=NULL&&pFirstNode->wires.GetNodeNum()<=0)
			pFindItem->m_lpNodeInfo->m_controlType = PIT_EDIT;
		else if(pFindItem!=NULL)
		{
			pFindItem->m_lpNodeInfo->m_controlType = PIT_BUTTON;
			pFindItem->m_lpNodeInfo->m_buttonType  = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			pFindItem->m_lpNodeInfo->m_sButtonName = " Add ";
#else 
			pFindItem->m_lpNodeInfo->m_sButtonName = " ��� ";
#endif
		}
		if(pFindItem!=NULL)
			pPropList->DeleteAllSonItems(pFindItem);
		if(pFindItem!=NULL && pFirstNode->wires.GetNodeNum()>1)
		{	//���ӹ�λ
			int index=2;
			pFirstNode->wires.GetFirst();
			for(WIREPLACE* pWirePlace=pFirstNode->wires.GetNext();pWirePlace;pWirePlace=pFirstNode->wires.GetNext(),index++)
			{
				int key=pFirstNode->wires.GetCursorKey();
#ifdef AFX_TARG_ENU_ENGLISH
				CXhChar50 sPropId("accessaryNodePlace_%d",key),sItemName("Hang Wire Place %d",index);
#else 
				CXhChar50 sPropId("accessaryNodePlace_%d",key),sItemName("����λ%d",index);
#endif
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMONEDIT;
				lpInfo->m_strPropName = sItemName;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_sButtonName=" Delete ";
				lpInfo->m_strPropName = sItemName;
				lpInfo->m_strPropHelp = "Attached hang wire place"; 
#else 
				lpInfo->m_sButtonName=" ɾ�� ";
				lpInfo->m_strPropName = sItemName;
				lpInfo->m_strPropHelp = "���ӹ���λ"; 
#endif
				pSonPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSNode::RegisterUdfPropId(sPropId);
				if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_DESIGN)|GetSingleWord(GROUP_WIRELOAD);
			}
		}
#endif
		*/
		SetPartMutiObjsProp(pFirstNode,"cfgword");
		pFindItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("m_hBlock"),&pParentItem);
		if (pFindItem)
		{
			if (pFirstNode->IsShadowObject())
			{	//����ͨ�����л���Ӱ�乹��
				pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_hBlock"));
				pPropList->DeleteItemByPropId(CLDSNode::GetPropID("relativeObjs"));
				//Ӱ����Ϣ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Project Informaiton";
				lpInfo->m_strPropHelp = "Project informaiton";
#else 
				lpInfo->m_strPropName = "Ӱ����Ϣ";
				lpInfo->m_strPropHelp = "Ӱ����Ϣ";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,8,true);
				pPropItem->m_idProp = CLDSNode::GetPropID("shadowInfo");
				if(CLDSNode::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSNode::GetPropStatus(pPropItem->m_idProp);
				pPropItem->SetReadOnly(FALSE);
				//Ӱ��ĸ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Project Mother Node";
				lpInfo->m_strPropHelp = "The current node is project mother node .The current node is original node when the handle is Ox0.";
#else 
				lpInfo->m_strPropName = "Ӱ��ĸ�ڵ�";
				lpInfo->m_strPropHelp = "��ǰ�ڵ��Ӧ��Ӱ��ĸ�ڵ������þ��Ϊ0x0ʱ��ʾ��ǰ�ڵ�Ϊԭʼ�ڵ㡣";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSNode::GetPropID("m_hMotherObj");
				if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstNode->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				if (pFirstNode->IsBlockRefShadowObject())
				{
					//������������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Assembly";
					lpInfo->m_strPropHelp = "The assemble vest in block's handle.";
#else 
					lpInfo->m_strPropName = "������������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSNode::GetPropID("m_hBlockRef");
					if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstNode->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstNode,"m_hBlock");
				SetPartMutiObjsProp(pFirstNode,"relativeObjs");
			}
		}
		pFindItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("shadowInfo"),&pParentItem);
		if (pFindItem)
		{
			if (!pFirstNode->IsShadowObject())
			{	//��Ӱ�乹���л�����ͨ����
				pPropList->DeleteItemByPropId(CLDSNode::GetPropID("shadowInfo"));
				pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_hMotherObj"));
				pFindItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("m_hBlockRef"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_hBlockRef"));
				pFindItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("m_hBlock"),NULL);
				if(pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstNode,"m_hBlock");
				else
				{	//��������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Handle";
					lpInfo->m_strPropHelp = "The vest in block's handle.";
#else
					lpInfo->m_strPropName = "��������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSNode::GetPropID("m_hBlock");
					if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pPropItem->m_idProp);
					pPropItem->SetReadOnly(pFirstNode->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
				pFindItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("relativeObjs"),NULL);
				if(pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstNode,"relativeObjs");
				else
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Relative Part";
					lpInfo->m_strPropHelp = "The part associated with the current part and symmetrical type";
#else
					lpInfo->m_strPropName = "��������";
					lpInfo->m_strPropHelp = "�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,8,true);
					pPropItem->m_idProp = CLDSNode::GetPropID("relativeObjs");
					if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstNode,"m_hMotherObj");
				pFindItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("m_hBlockRef"),&pParentItem);
				if (pFindItem&&pFirstNode->IsBlockRefShadowObject())
					SetPartMutiObjsProp(pFirstNode,"m_hBlockRef");
				else if (pFindItem&&!pFirstNode->IsBlockRefShadowObject())
					pPropList->DeleteItemByPropId(CLDSNode::GetPropID("m_hBlockRef"));
				else if(pFindItem==NULL&&pFirstNode->IsBlockRefShadowObject())
				{
					pPropList->FindItemByPropId(CLDSNode::GetPropID("m_hMotherObj"),&pParentItem);
					//������������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Assembly";
					lpInfo->m_strPropHelp = "The assemble vest in block's handle.";
#else 
					lpInfo->m_strPropName = "������������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
					pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSNode::GetPropID("m_hBlockRef");
					if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstNode->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
		}
		SetPartMutiObjsProp(pFirstNode,"m_cPosCalType");
		pFindItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("m_cPosCalType"),&pParentItem);
		if(pFindItem)
			UpdateNode_AttachType_TreeItem(pPropList,pFindItem);
		SetPartMutiObjsProp(pFirstNode,"m_cPosCalViceType");
		SetPartMutiObjsProp(pFirstNode,"attachNode1");
		SetPartMutiObjsProp(pFirstNode,"attachNode2");
		SetPartMutiObjsProp(pFirstNode,"attachNode3");
		SetPartMutiObjsProp(pFirstNode,"attachNode4");
		SetPartMutiObjsProp(pFirstNode,"attachPole1");
		SetPartMutiObjsProp(pFirstNode,"attachPole2");
		SetPartMutiObjsProp(pFirstNode,"attach_scale");
		SetPartMutiObjsProp(pFirstNode,"attach_offset");
		SetPartMutiObjsProp(pFirstNode,"hRelationPlane");
		SetPartMutiObjsProp(pFirstNode,"hDatumPoint");
#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__LDS_)
		if(!theApp.m_bDrawingWorkEnvOnly)
		{
			SetPartMutiObjsProp(pFirstNode,"force_type");
			SetPartMutiObjsProp(pFirstNode,"attachForeverLoad_X");
			SetPartMutiObjsProp(pFirstNode,"attachForeverLoad_Y");
			SetPartMutiObjsProp(pFirstNode,"attachForeverLoad_Z");
			SetPartMutiObjsProp(pFirstNode,"permanentFz");
			SetPartMutiObjsProp(pFirstNode,"attachVolLoad_X");
			SetPartMutiObjsProp(pFirstNode,"attachVolLoad_Y");
			SetPartMutiObjsProp(pFirstNode,"attachVolLoad_Z");
			SetPartMutiObjsProp(pFirstNode,"restrict_all");
			SetPartMutiObjsProp(pFirstNode,"restrict_x");
			SetPartMutiObjsProp(pFirstNode,"restrict_y");
			SetPartMutiObjsProp(pFirstNode,"restrict_z");
			SetPartMutiObjsProp(pFirstNode,"restrict_rot_x");
			SetPartMutiObjsProp(pFirstNode,"restrict_rot_y");
			SetPartMutiObjsProp(pFirstNode,"restrict_rot_z");
		}
#endif
#ifdef __COMMON_PART_INC_//defined(__TSA_)||defined(__LDS_)
		if(!theApp.m_bAnalysisWorkEnvOnly)
		{	//Ԥ����
			SetPartMutiObjsProp(pFirstNode,"m_hArcLift");
			pFindItem = pPropList->FindItemByPropId(CLDSNode::GetPropID("m_hArcLift"),&pParentItem);
			if(pFindItem)
				UpdateNode_ArcLift_TreeItem(pPropList,pFindItem);
		}
#endif
	}
	pPropList->Redraw();
}