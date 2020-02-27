//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "TowerPropertyDlg.h"
#include "DesignTubeEndLjDlg.h"
#include "CfgPartNoDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "LmaDlg.h"
#include "Query.h"
#include "OddmentDesignDlg.h"
#include "RelativeObjectDlg.h"
#include "dlg.h"
#include "ReportPointDlg.h"
#include "PartUcsDlg.h"
#include "ModifyRelativeObjPartNoDlg.h"
#include "XhCharString.h"
#include "DesignShadowDlg.h"
#include "DatumPointPropertyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//from DisplayPropertyPage.cpp
BOOL SetPartMutiObjsProp(CLDSObject *pObj, char *propName,long idClassType=0);
void SetPropItemReadOnly(CLDSDbObject *pObj,char *propName,BOOL bReadOnly);

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL ModifyLineSlotItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSLineSlot::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL ModifyLineFlatItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSLineFlat::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
void UpdateLineSlot_DesPos_EndPosType_TreeItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,BOOL bStart);
BOOL LineSlotButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem, *pParentItem;
	CLDSLineSlot *pLineSlot=NULL, *pFirstLineSlot=NULL;
	CReportVectorDlg vecDlg;
	CReportPointDlg pointDlg;
	CCfgPartNoDlg cfgdlg;
	COddmentDesignDlg odddlg;
	char sText[MAX_TEMP_CHAR_100+1];
	CString valueStr = _T("");
	if(pView->selectObjs.GetNodeNum()>0)
		pFirstLineSlot = (CLDSLineSlot*)pView->selectObjs.GetFirst();
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
	if(pItem->m_idProp==CLDSLineSlot::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstLineSlot->cfgword; 
		if(pFirstLineSlot->layer(0)!='L')
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
		{
			for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			{
				pLineSlot->cfgword = cfgdlg.cfgword;			//�ڴ˴����µ�һ���ڵ������Ϻ�
				pLineSlot->SetModified(TRUE,FALSE);
				pLineSlot->SyncMirProp("cfgword");
			}
		}
		if(pFirstLineSlot->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("sPartNo"))
	{
		CModifyRelativeObjPartNoDlg modify_partno_dlg;
		modify_partno_dlg.m_pWorkPart=pFirstLineSlot;
		if(modify_partno_dlg.DoModal()==IDOK)
		{
			pFirstLineSlot->SetPartNo(modify_partno_dlg.m_sCurObjPartNo.GetBuffer(0));
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
			SEGI iSeg;
			if(CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(modify_partno_dlg.m_sCurObjPartNo,&iSeg,NULL))
			{
				//pView->selectObjs������۸ֱ�ŵĶκ�ͬ����������CModifyRelativeObjPartNoDlg����� wjh-2016.6.14
				pFirstLineSlot->iSeg=iSeg;
				pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("iSeg"),iSeg.ToString());
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("nearWorkPlaneNorm"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Near Work Plane Normal";
#else 
		vecDlg.m_sCaption = "�۸ֻ�׼����Ʒ�������";
#endif
		vecDlg.m_fVectorX = pFirstLineSlot->nearWorkPlaneNorm.x;
		vecDlg.m_fVectorY = pFirstLineSlot->nearWorkPlaneNorm.y;
		vecDlg.m_fVectorZ = pFirstLineSlot->nearWorkPlaneNorm.z;
		pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("nearWorkPlaneNorm.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("nearWorkPlaneNorm.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("nearWorkPlaneNorm.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("nearWorkPlaneNorm.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("nearWorkPlaneNorm.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("nearWorkPlaneNorm.z"),CString(sText));
			for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			{	//���¼���۸ַ��߷���
				pLineSlot->nearWorkPlaneNorm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				pLineSlot->CalWorkPlaneNorm();
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("workPlaneNorm")
		||pItem->m_idProp==CLDSLineSlot::GetPropID("workPlaneNorm.x")
		||pItem->m_idProp==CLDSLineSlot::GetPropID("workPlaneNorm.y")
		||pItem->m_idProp==CLDSLineSlot::GetPropID("workPlaneNorm.z"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "U-Steel Datum Plane Normal";
#else 
		vecDlg.m_sCaption = "�۸ֻ�׼�淨������";
#endif
		vecDlg.m_fVectorX = pFirstLineSlot->WorkPlaneNorm().x;
		vecDlg.m_fVectorY = pFirstLineSlot->WorkPlaneNorm().y;
		vecDlg.m_fVectorZ = pFirstLineSlot->WorkPlaneNorm().z;
		pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("workPlaneNorm.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("workPlaneNorm.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("workPlaneNorm.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("workPlaneNorm.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("workPlaneNorm.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("workPlaneNorm.z"),CString(sText));
			for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
				pLineSlot->SetWorkPlaneNorm(f3dPoint(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ));
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("hStart"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Rod Start Node";
		modifyhdlg.m_sTitle="Start Node:";
#else 
		modifyhdlg.m_sCaption="�޸ĸ˼�ʼ�˽ڵ�";
		modifyhdlg.m_sTitle="ʼ�˽ڵ�:";
#endif
		if(pFirstLineSlot->pStart)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineSlot->pStart->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the datum node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�ڵ�:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind nodes��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
			else
			{
				CUndoOperObject undo(&Ta);
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("hStart"),valueStr);
				for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
				{
					CLDSNode* pOldNode=pLineSlot->pStart;
					pLineSlot->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),pNode,NULL);
					pLineSlot->pStart=pNode;
					pLineSlot->SetModified();
					pLineSlot->ClearFlag();
					pLineSlot->CalPosition();
					//���¸˼��ϵĶ˽ڵ������Ϣ
					pLineSlot->UpdateRodEndNodeRelation(pOldNode,pNode);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("hEnd"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Rod End Node";
		modifyhdlg.m_sTitle="End Node:";
#else 
		modifyhdlg.m_sCaption="�޸ĸ˼��ն˽ڵ�";
		modifyhdlg.m_sTitle="�ն˽ڵ�:";
#endif
		if(pFirstLineSlot->pEnd)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineSlot->pEnd->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the datum node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�ڵ�:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind nodes��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
			else
			{
				CUndoOperObject undo(&Ta);
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("hEnd"),valueStr);
				for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
				{
					CLDSNode* pOldNode=pLineSlot->pEnd;
					pLineSlot->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),NULL,pNode);
					pLineSlot->pEnd=pNode;
					pLineSlot->SetModified();
					pLineSlot->ClearFlag();
					pLineSlot->CalPosition();
					//���¸˼��ϵĶ˽ڵ������Ϣ
					pLineSlot->UpdateRodEndNodeRelation(pOldNode,pNode);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("oddStart"))
	{
		odddlg.m_hRefPart1.Format("0X%X",pFirstLineSlot->desStartOdd.m_hRefPart1);
		odddlg.m_hRefPart2.Format("0X%X",pFirstLineSlot->desStartOdd.m_hRefPart2);
		odddlg.m_fCollideDist = pFirstLineSlot->desStartOdd.m_fCollideDist;
		odddlg.m_iOddCalStyle = pFirstLineSlot->desStartOdd.m_iOddCalStyle;
		if(odddlg.DoModal()==IDOK)
		{
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart1);
			sscanf(sText,"%X",&pFirstLineSlot->desStartOdd.m_hRefPart1);
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart2);
			sscanf(sText,"%X",&pFirstLineSlot->desStartOdd.m_hRefPart2);
			pFirstLineSlot->desStartOdd.m_fCollideDist = odddlg.m_fCollideDist;
			pFirstLineSlot->desStartOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
			pFirstLineSlot->CalStartOddment();
			char valueStr[MAX_TEMP_CHAR_50+1]="";
			if(pFirstLineSlot->GetPropValueStr(pItem->m_idProp,valueStr)>0)
				pItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("startPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "Start Design Coordinate";
#else 
		pointDlg.m_sCaption = "�۸�ʼ���������";
#endif
		pointDlg.m_fPointX = pFirstLineSlot->Start().x;
		pointDlg.m_fPointY = pFirstLineSlot->Start().y;
		pointDlg.m_fPointZ = pFirstLineSlot->Start().z;
		if(pFirstLineSlot->desStartPos.endPosType!=4)	//4.ָ������
			pointDlg.m_bReadOnly=TRUE;
		pointDlg.m_bLockPoint=pFirstLineSlot->IsStartPosLocked();
		if(pointDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("startPos.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("startPos.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("startPos.z"),CString(sText));
			for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			{
				pLineSlot->SetStart(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
				if(pointDlg.m_bLockPoint)
					pLineSlot->LockStartPos();
				else
					pLineSlot->UnlockStartPos();
			}
		}
		//ʼ������
		if(pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart)
		{	//����������Ϊֻ������
			SetPropItemReadOnly(pFirstLineSlot,"startPos.x",TRUE);
			SetPropItemReadOnly(pFirstLineSlot,"startPos.y",TRUE);
			SetPropItemReadOnly(pFirstLineSlot,"startPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ����ֻ������
			SetPropItemReadOnly(pFirstLineSlot,"startPos.x",pFirstLineSlot->desStartPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineSlot,"startPos.y",pFirstLineSlot->desStartPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineSlot,"startPos.z",pFirstLineSlot->desStartPos.endPosType!=4);	//4.ָ������
		}
		SetPropItemReadOnly(pFirstLineSlot,"hStart",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineSlot,"oddStart",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineSlot,"desStartPos",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineSlot,"desStartPos.endPosType",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineSlot,"desStartPos.datum_pos_style",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineSlot,"desStartPos.hDatumPart",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineSlot,"desStartPos.fNormOffset",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineSlot,"desStartPos.fEccentricDist",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineSlot,"desStartPos.face_offset_norm",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineSlot,"desStartPos.hDatumStartPart",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineSlot,"desStartPos.bDatumStartStartEnd",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineSlot,"desStartPos.hDatumEndPart",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineSlot,"desStartPos.bDatumEndStartEnd",pFirstLineSlot->IsStartPosLocked()||pFirstLineSlot->bHuoQuStart);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("oddEnd"))
	{
		odddlg.m_hRefPart1.Format("0X%X",pFirstLineSlot->desEndOdd.m_hRefPart1);
		odddlg.m_hRefPart2.Format("0X%X",pFirstLineSlot->desEndOdd.m_hRefPart2);
		odddlg.m_fCollideDist = pFirstLineSlot->desEndOdd.m_fCollideDist;
		odddlg.m_iOddCalStyle = pFirstLineSlot->desEndOdd.m_iOddCalStyle;
		if(odddlg.DoModal()==IDOK)
		{
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart1);
			sscanf(sText,"%X",&pFirstLineSlot->desEndOdd.m_hRefPart1);
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart2);
			sscanf(sText,"%X",&pFirstLineSlot->desEndOdd.m_hRefPart2);
			pFirstLineSlot->desEndOdd.m_fCollideDist = odddlg.m_fCollideDist;
			pFirstLineSlot->desEndOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
			pFirstLineSlot->CalEndOddment();
			char valueStr[MAX_TEMP_CHAR_50+1]="";
			if(pFirstLineSlot->GetPropValueStr(pItem->m_idProp,valueStr)>0)
				pItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("actualStartPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "U-Steel Start Coordinate";
#else 
		pointDlg.m_sCaption = "�۸�ʼ��ʵ������";
#endif
		pointDlg.m_bEnableLock=FALSE;
		pointDlg.m_bReadOnly=TRUE;
		f3dPoint actualStart,vec=pFirstLineSlot->Start()-pFirstLineSlot->End();
		normalize(vec);
		actualStart = pFirstLineSlot->Start()+vec*pFirstLineSlot->startOdd();
		pointDlg.m_fPointX = actualStart.x;
		pointDlg.m_fPointY = actualStart.y;
		pointDlg.m_fPointZ = actualStart.z;
		pointDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("actualEndPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "U-Steel End Coordinate";
#else 
		pointDlg.m_sCaption = "�۸��ն�ʵ������";
#endif
		pointDlg.m_bEnableLock=FALSE;
		pointDlg.m_bReadOnly=TRUE;
		f3dPoint actualStart,vec=pFirstLineSlot->Start()-pFirstLineSlot->End();
		normalize(vec);
		actualStart = pFirstLineSlot->Start()+vec*pFirstLineSlot->startOdd();
		pointDlg.m_fPointX = actualStart.x;
		pointDlg.m_fPointY = actualStart.y;
		pointDlg.m_fPointZ = actualStart.z;
		pointDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("endPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "U-Steel End Design Coordinate";
#else 
		pointDlg.m_sCaption = "�۸��ն��������";
#endif
		pointDlg.m_fPointX = pFirstLineSlot->End().x;
		pointDlg.m_fPointY = pFirstLineSlot->End().y;
		pointDlg.m_fPointZ = pFirstLineSlot->End().z;
		if(pFirstLineSlot->desEndPos.endPosType!=4)	//4.ָ������
			pointDlg.m_bReadOnly=TRUE;
		pointDlg.m_bLockPoint=pFirstLineSlot->IsEndPosLocked();
		if(pointDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("endPos.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("endPos.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("endPos.z"),CString(sText));
			for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			{
				pLineSlot->SetEnd(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
				if(pointDlg.m_bLockPoint)
					pLineSlot->LockEndPos();
				else
					pLineSlot->UnlockEndPos();
			}
		}
		//�ն�����
		if(pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd)
		{	//����������Ϊֻ������
			SetPropItemReadOnly(pFirstLineSlot,"endPos.x",TRUE);
			SetPropItemReadOnly(pFirstLineSlot,"endPos.y",TRUE);
			SetPropItemReadOnly(pFirstLineSlot,"endPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ����ֻ������
			SetPropItemReadOnly(pFirstLineSlot,"endPos.x",pFirstLineSlot->desEndPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineSlot,"endPos.y",pFirstLineSlot->desEndPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineSlot,"endPos.z",pFirstLineSlot->desEndPos.endPosType!=4);	//4.ָ������
		}
		SetPropItemReadOnly(pFirstLineSlot,"hEnd",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineSlot,"oddEnd",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineSlot,"desEndPos",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineSlot,"desEndPos.endPosType",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineSlot,"desEndPos.datum_pos_style",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineSlot,"desEndPos.hDatumPart",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineSlot,"desEndPos.fNormOffset",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineSlot,"desEndPos.fEccentricDist",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineSlot,"desEndPos.face_offset_norm",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineSlot,"desEndPos.hDatumStartPart",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineSlot,"desEndPos.bDatumStartStartEnd",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineSlot,"desEndPos.hDatumEndPart",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineSlot,"desEndPos.bDatumEndStartEnd",pFirstLineSlot->IsEndPosLocked()||pFirstLineSlot->bHuoQuEnd);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("relativeObjs"))
	{
		CRelativeObjectDlg relativeDlg;
		relativeDlg.m_pWorkObj = pFirstLineSlot;
		relativeDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("shadowInfo"))
	{
		CDesignShadowDlg shadowdlg;
		shadowdlg.SetShadowObj((CLDSObject*)pFirstLineSlot);
		shadowdlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumPart")||pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.hDatumPart"))
	{
		//ͨ��ѡ�񹹼��޸Ĵ��λ�û�׼�˼�
		CModifyPartHandleDlg modifyhdlg;
		CTubeEndPosPara *pSlotEndPosPara=NULL;
		if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumPart"))
		{
			pSlotEndPosPara=&(pFirstLineSlot->desStartPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineSlot->desStartPos.hDatumPart);
		}
		else
		{
			pSlotEndPosPara=&(pFirstLineSlot->desEndPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineSlot->desEndPos.hDatumPart);
		}
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		if(pSlotEndPosPara&&pSlotEndPosPara->endPosType==3)
		{	//���߽��㶨λ
			modifyhdlg.m_sCaption="Modify Cross Datum Tube";
			modifyhdlg.m_sTitle="Datum Tube:";
			modifyhdlg.SetCmdPickPromptStr("Please select the cross datum tube:");
		}
		else
		{
			modifyhdlg.m_sCaption="Modify Lap Joint Datum Rod";
			modifyhdlg.m_sTitle="Datum Rod:";
			modifyhdlg.SetCmdPickPromptStr("Please select the lap-joint datum rod:");
		}
#else 
		if(pSlotEndPosPara&&pSlotEndPosPara->endPosType==3)
		{	//���߽��㶨λ
			modifyhdlg.m_sCaption="�޸����߽����׼�ֹ�";
			modifyhdlg.m_sTitle="��׼�ֹ�:";
			modifyhdlg.SetCmdPickPromptStr("��ѡ�����߽����׼�ֹ�:");
		}
		else
		{
			modifyhdlg.m_sCaption="�޸Ĵ��λ�û�׼�˼�";
			modifyhdlg.m_sTitle="��׼�˼�:";
			modifyhdlg.SetCmdPickPromptStr("��ѡ���ӻ�׼�˼�:");
		}
#endif
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind parts��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumPart"))
					pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("desStartPos.hDatumPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("desEndPos.hDatumPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineSlot* pMirLineSlot=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineSlot=(CLDSLineSlot*)pLineSlot->GetMirPart(msg);
						if(pMirLineSlot)
						{
							if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumPart"))
								pMirLineSlot->desStartPos.hDatumPart=pPart->handle;
							else
								pMirLineSlot->desEndPos.hDatumPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumPart"))
						pLineSlot->desStartPos.hDatumPart=pPart->handle;
					else
						pLineSlot->desEndPos.hDatumPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineSlot->handle);
				}
				if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumPart"))
				{
					pParentItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desStartPos.endPosType"),NULL);
					if(pParentItem)
						UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
				}
				else
				{
					pParentItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desEndPos.endPosType"),NULL);
					if(pParentItem)
						UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumStartPart")
		||pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.hDatumStartPart"))
	{	//�޸�ͶӰ��ʼ�����ڻ�׼�˼�
		CModifyPartHandleDlg modifyhdlg;
		CTubeEndPosPara *pSlotEndPosPara=NULL;
		if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumStartPart"))
		{
			pSlotEndPosPara=&(pFirstLineSlot->desStartPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineSlot->desStartPos.hDatumStartPart);
		}
		else
		{
			pSlotEndPosPara=&(pFirstLineSlot->desEndPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineSlot->desEndPos.hDatumStartPart);
		}
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		if(pSlotEndPosPara&&pSlotEndPosPara->endPosType==3)
		{	//���߽��㶨λ
			modifyhdlg.m_sCaption="Modify Cross Tube";
			modifyhdlg.m_sTitle="Cross Tube:";
			modifyhdlg.SetCmdPickPromptStr("Please select the cross tube:");
		}
		else
		{
			modifyhdlg.m_sCaption="Modify Start Datum Rod ";
			modifyhdlg.m_sTitle="Datum Rod:";
			modifyhdlg.SetCmdPickPromptStr("Please select the projective datum rod:");
		}
#else 
		if(pSlotEndPosPara&&pSlotEndPosPara->endPosType==3)
		{	//���߽��㶨λ
			modifyhdlg.m_sCaption="�޸Ľ���ֹ�";
			modifyhdlg.m_sTitle="����ֹ�:";
			modifyhdlg.SetCmdPickPromptStr("��ѡ�����߽��㽻��ֹ�:");
		}
		else
		{
			modifyhdlg.m_sCaption="�޸�ͶӰʼ�����ڻ�׼�˼�";
			modifyhdlg.m_sTitle="��׼�˼�:";
			modifyhdlg.SetCmdPickPromptStr("��ѡ��ͶӰ��׼�˼�:");
		}
#endif
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind parts��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				//������ʾ
				if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumStartPart"))
					pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("desStartPos.hDatumStartPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("desEndPos.hDatumStartPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineSlot* pMirLineSlotPart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineSlotPart=(CLDSLineSlot*)pLineSlot->GetMirPart(msg);
						if(pMirLineSlotPart)
						{
							if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumStartPart"))
								pMirLineSlotPart->desStartPos.hDatumStartPart=pPart->handle;
							else
								pMirLineSlotPart->desEndPos.hDatumStartPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumStartPart"))
						pFirstLineSlot->desStartPos.hDatumStartPart=pPart->handle;
					else
						pFirstLineSlot->desEndPos.hDatumStartPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineSlot->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumEndPart")
		||pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.hDatumEndPart"))
	{	//�޸�ͶӰ���ն����ڻ�׼�˼�
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify End Datum Rod";
		modifyhdlg.m_sTitle="Datum Rod:";
#else 
		modifyhdlg.m_sCaption="�޸�ͶӰ�ն����ڻ�׼�˼�";
		modifyhdlg.m_sTitle="��׼�˼�:";
#endif
		if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumEndPart"))
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineSlot->desStartPos.hDatumEndPart);
		else
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineSlot->desEndPos.hDatumEndPart);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the projective datum tube:");
#else 
		modifyhdlg.SetCmdPickPromptStr("��ѡ��ͶӰ��׼�ֹ�:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind parts��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				//������ʾ
				if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumEndPart"))
					pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("desStartPos.hDatumEndPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("desEndPos.hDatumEndPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineSlot* pMirLineSlotPart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineSlotPart=(CLDSLineSlot*)pLineSlot->GetMirPart(msg);
						if(pMirLineSlotPart)
						{
							if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumEndPart"))
								pMirLineSlotPart->desStartPos.hDatumEndPart=pPart->handle;
							else
								pMirLineSlotPart->desEndPos.hDatumEndPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumEndPart"))
						pFirstLineSlot->desStartPos.hDatumEndPart=pPart->handle;
					else
						pFirstLineSlot->desEndPos.hDatumEndPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineSlot->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("hWorkNormRefPart"))
	{
		//ͨ��ѡ�񹹼��޸Ļ�׼�淨�߲��ո˼�
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Normal Reference Rod";
		modifyhdlg.m_sTitle="Reference Rod:";
#else 
		modifyhdlg.m_sCaption="�޸ķ��߲��ո˼�";
		modifyhdlg.m_sTitle="���ո˼�:";
#endif
		modifyhdlg.m_sHandle.Format("0X%X",pFirstLineSlot->hWorkNormRefPart);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the reference rod:");
#else 
		modifyhdlg.SetCmdPickPromptStr("��ѡ����չ���:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind parts��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("hWorkNormRefPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineSlot* pMirLineSlot=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineSlot=(CLDSLineSlot*)pLineSlot->GetMirPart(msg);
						if(pMirLineSlot)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//������ѡ���߲��ո˼��ĶԳƹ��� wht 11-05-07
								pMirLineSlot->hWorkNormRefPart=pMirPart->handle;
						}
					}
					pLineSlot->hWorkNormRefPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineSlot->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("m_hPartWeldParent"))
	{
		//ͨ��ѡ�񹹼��޸ĺ��Ӹ����������ڽ������޸ĵ��������ĺ��Ӹ�����
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Weld Parent Part";
		modifyhdlg.m_sTitle="Weld Parent Part:";
		modifyhdlg.SetCmdPickPromptStr("Please select weld parent part:");
#else 
		modifyhdlg.m_sCaption="�޸Ĳ۸ֺ��Ӹ�����";
		modifyhdlg.m_sTitle="���Ӹ�����:";
		modifyhdlg.SetCmdPickPromptStr("��ѡ�񺸽Ӹ�����:");
#endif
		modifyhdlg.m_sHandle.Format("0X%X",pFirstLineSlot->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind parts��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("m_hPartWeldParent"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLinePart* pMirLinePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLinePart=(CLDSLinePart*)pLineSlot->GetMirPart(msg);
						if(pMirLinePart)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//������ѡ�������ĶԳƹ��� wht 11-05-07
								pMirLinePart->m_hPartWeldParent=pMirPart->handle;
						}
					}
					pLineSlot->m_hPartWeldParent=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineSlot->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.face_offset_norm"))
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Offset Normal";
#else 
		vecDlg.m_sCaption = "ƽ���淨��";
#endif
		vecDlg.m_fVectorX = pFirstLineSlot->desStartPos.face_offset_norm.x;
		vecDlg.m_fVectorY = pFirstLineSlot->desStartPos.face_offset_norm.y;
		vecDlg.m_fVectorZ = pFirstLineSlot->desStartPos.face_offset_norm.z;
		if(vecDlg.DoModal()==IDOK)
		{
			for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
				pLineSlot->desStartPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.face_offset_norm"))
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Offset Normal";
#else 
		vecDlg.m_sCaption = "ƽ���淨��";
#endif
		vecDlg.m_fVectorX = pFirstLineSlot->desEndPos.face_offset_norm.x;
		vecDlg.m_fVectorY = pFirstLineSlot->desEndPos.face_offset_norm.y;
		vecDlg.m_fVectorZ = pFirstLineSlot->desEndPos.face_offset_norm.z;
		if(vecDlg.DoModal()==IDOK)
		{
			for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
				pLineSlot->desEndPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("hAttachTube"))
	{
		//ͨ��ѡ�񹹼��޸ĺ��Ӹ����������ڽ������޸ĵ��������ĺ��Ӹ�����
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Attached Tube";
		modifyhdlg.m_sTitle="Attached Tube:";
		modifyhdlg.SetCmdPickPromptStr("Please select the attached tube:");
#else 
		modifyhdlg.m_sCaption="�޸Ĳ۸������ĸֹ�";
		modifyhdlg.m_sTitle="�����ֹ�:";
		modifyhdlg.SetCmdPickPromptStr("��ѡ�������ֹ�:");
#endif
		modifyhdlg.m_sHandle.Format("0X%X",pFirstLineSlot->hAttachTube);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(modifyhdlg.m_nHandle,CLS_LINETUBE);
			if(pLineTube==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind parts��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("hAttachTube"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineSlot* pMirLineSlot=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineSlot=(CLDSLineSlot*)pLineSlot->GetMirPart(msg);
						if(pMirLineSlot)
						{
							CLDSPart *pMirPart=pLineTube->GetMirPart(msg);
							if(pMirPart)	//������ѡ�������ĶԳƹ��� wht 11-05-07
								pMirLineSlot->hAttachTube=pMirPart->handle;
						}
					}
					pLineSlot->hAttachTube=pLineTube->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineSlot->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desDatumPoint"))
	{
		CDatumPointPropertyDlg datumPointDlg;
		datumPointDlg.m_pCurEditObj=pFirstLineSlot;
		datumPointDlg.m_xPoint=pFirstLineSlot->desDatumPoint;
		datumPointDlg.m_propList.m_bDisBaseInfo=FALSE;
		if(datumPointDlg.DoModal()==IDOK)
		{
			pFirstLineSlot->desDatumPoint=datumPointDlg.m_xPoint;
			pFirstLineSlot->desDatumPoint.UpdatePos(pFirstLineSlot->BelongModel());
			pFirstLineSlot->ClearFlag();
			pFirstLineSlot->CalPosition();
			CXhChar100 sText;
			if(pFirstLineSlot->GetPropValueStr(pItem->m_idProp,sText)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,sText);
			long idProp=CLDSLineSlot::GetPropID("desDatumPoint.x");
			if(pFirstLineSlot->GetPropValueStr(idProp,sText)>0)
			{
				pPropList->SetItemPropValue(idProp,sText);
				pPropList->SetItemReadOnly(idProp,pFirstLineSlot->desDatumPoint.datum_pos_style!=0);
			}
			idProp=CLDSLineSlot::GetPropID("desDatumPoint.y");
			if(pFirstLineSlot->GetPropValueStr(idProp,sText)>0)
			{
				pPropList->SetItemPropValue(idProp,sText);
				pPropList->SetItemReadOnly(idProp,pFirstLineSlot->desDatumPoint.datum_pos_style!=0);
			}
			idProp=CLDSLineSlot::GetPropID("desDatumPoint.z");
			if(pFirstLineSlot->GetPropValueStr(idProp,sText)>0)
			{
				pPropList->SetItemPropValue(idProp,sText);
				pPropList->SetItemReadOnly(idProp,pFirstLineSlot->desDatumPoint.datum_pos_style!=0);
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desDatumPoint.param.hOffsetDatumNode"))
	{
		int update_type=0;
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Update Dependent Node";
		modifyhdlg.m_sTitle="Dependent Node:";
#else 
		modifyhdlg.m_sCaption="�޸�ƫ����ʼ�ڵ�";
		modifyhdlg.m_sTitle="��ʼ�ڵ�:";
#endif
		g_pSolidDraw->SetEntSnapStatus(pFirstLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode);
		modifyhdlg.m_sHandle.Format("0X%X",abs(pFirstLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode));
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select dependent node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("��ѡ��ƫ����ʼ�ڵ�:");
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
				for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
				{
					if(pLineSlot->desDatumPoint.datum_pos_style==11)
					{
						if(pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode>=0)
							pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=modifyhdlg.m_nHandle;
						else
							pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=-modifyhdlg.m_nHandle;
						//�¸��������ڵ�ͬʱһ�㻹�����ƫ������ƫ�����ͣ���ʱ���¼���۸�λ��û������ wjh-2015.10.12
						//pLineSlot->CalPosition();
						pLineSlot->SetModified();
					}
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			return TRUE;
		}
		else
		{
			g_pSolidDraw->ReleaseSnapStatus();
			return FALSE;
		}
	}
	else
		return FALSE;
	for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
	{
		pLineSlot->SetModified();
		pLineSlot->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pLineSlot->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL UpdateLineSlotColumnRayVecParamsProperty(CPropertyList* pPropList,CPropTreeItem* pPropItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineSlot *pFirstSlot = (CLDSLineSlot*)pView->selectObjs.GetFirst();
	if(pFirstSlot==NULL)
		return FALSE;
	CXhChar100 sText;
	CItemInfo* lpInfo;
	CPropTreeItem* pFindItem=NULL;
	if( pFirstSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style==0||
		pFirstSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style==1)
	{	
		//if(m_bDisplayPointSonItem)
		pFindItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecStdStyle"),NULL);
		if(pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Specify";
			lpInfo->m_strPropHelp = "Specify vector";
#else 
			lpInfo->m_strPropName = "ָ������";
			lpInfo->m_strPropHelp = "ָ������";
#endif
			pFindItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
			pFindItem->m_idProp =CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecStdStyle");
		}
		if(pFirstSlot->GetPropValueStr(pFindItem->m_idProp,sText)>0)
			pFindItem->m_lpNodeInfo->m_strPropValue = sText;
		pFindItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.x"),NULL);
		if(pFindItem==NULL)
		{	//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="X";
			if(pFirstSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="X coordinate of ray vector";
			else 
				lpInfo->m_strPropHelp="X coordinate of datum edge vector";
#else 
				lpInfo->m_strPropHelp="���߷���X����";
			else 
				lpInfo->m_strPropHelp="��׼�߷���X����";
#endif
			pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pFindItem->m_idProp = CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.x");
		}
		if(pFirstSlot->GetPropValueStr(pFindItem->m_idProp,sText)>0)
			pFindItem->m_lpNodeInfo->m_strPropValue=sText;
		pFindItem->SetReadOnly(pFirstSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style!=0);

		pFindItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.y"),NULL);
		if(pFindItem==NULL)
		{	//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Y";
			if(pFirstSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Y coordinate of ray vector";
			else 
				lpInfo->m_strPropHelp="Y coordinate of datum edge vector";
#else 
				lpInfo->m_strPropHelp="���߷���Y����";
			else 
				lpInfo->m_strPropHelp="��׼�߷���Y����";
#endif
			pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pFindItem->m_idProp = CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.y");
		}
		if(pFirstSlot->GetPropValueStr(pFindItem->m_idProp,sText))
			pFindItem->m_lpNodeInfo->m_strPropValue=sText;
		pFindItem->SetReadOnly(pFirstSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style!=0);
		
		pFindItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.z"),NULL);
		if(pFindItem==NULL)
		{	//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Z";
			if(pFirstSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Z coordinate of ray vector";
			else 
				lpInfo->m_strPropHelp="Z coordinate of datum edge vector";
#else 
				lpInfo->m_strPropHelp="���߷���Z����";
			else 
				lpInfo->m_strPropHelp="��׼�߷���Z����";
#endif
			pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pFindItem->m_idProp = CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.z");
		}
		if(pFirstSlot->GetPropValueStr(pFindItem->m_idProp,sText))
			pFindItem->m_lpNodeInfo->m_strPropValue=sText;
		pFindItem->SetReadOnly(pFirstSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style!=0);
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecStdStyle"));
		pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.x"));
		pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.y"));
		pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.z"));
	}
	if(pFirstSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style!=0
		&&pFirstSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style!=4)
	{	//��ת�Ƕ�
		pFindItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecRotateAngle"),NULL);
		if(pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Rotate Angle";
			lpInfo->m_strPropHelp="Rotate angle relates to specify datum edge,unit is ��";
#else 
			lpInfo->m_strPropName="��ת�Ƕ�";
			lpInfo->m_strPropHelp="�����ָ����ʼ�ߵ���ת�Ƕ�,��λΪ��";
#endif
			pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pFindItem->m_idProp = CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecRotateAngle");
		}
		if(pFirstSlot->GetPropValueStr(pFindItem->m_idProp,sText))
			pFindItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else
		pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecRotateAngle"));
	return TRUE;
}
BOOL ModifyLineSlotProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineSlot *pCurSlot = (CLDSLineSlot*)pView->selectObjs.GetFirst();;
	CLDSLineSlot *pLineSlot = NULL;
	CItemInfo *lpInfo;
	CPropTreeItem *pFindItem,*pSonPropItem,*pParentItem,*pLeafPropItem;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	const int GROUP_GENERAL=1,GROUP_START=2,GROUP_END=3;
	if(pItem->m_idProp==CLDSLineSlot::GetPropID("layer"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			pLineSlot->SetLayer(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//��ת�Գ�
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
					CalMirAtomLayer(pLineSlot->layer(),pMirLineSlot->layer(),pMirObj->mirInfo);
			}
		}	
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("cMaterial"))
	{	//�۸ֲ���
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			pLineSlot->cMaterial = QuerySteelBriefMatMark(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
					pMirLineSlot->cMaterial=pLineSlot->cMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("m_cQualityLevel"))
	{	//�۸������ȼ�
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Default")==0)
				pLineSlot->m_cQualityLevel=0;
			else
#else
			if(valueStr.CompareNoCase("Ĭ��")==0)
				pLineSlot->m_cQualityLevel=0;
			else
#endif
				pLineSlot->m_cQualityLevel=valueStr[0];
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
					pMirLineSlot->m_cQualityLevel=pLineSlot->m_cQualityLevel;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("specification"))
	{	//�۸ֹ��
		SLOT_PARA_TYPE *pSlotPara=FindSlotType(tem_str);
		if(pSlotPara==NULL)
			return FALSE;
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			_snprintf(pLineSlot->spec,MAX_CHAR_GUIGE_16,"%s",tem_str);
			pLineSlot->size_wide=pSlotPara->b;
			pLineSlot->size_thick=pSlotPara->d;
			pLineSlot->size_height=pSlotPara->h;
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
				{
					strcpy(pMirLineSlot->spec,pLineSlot->spec);
					pMirLineSlot->size_wide=pLineSlot->GetWidth();
					pMirLineSlot->size_thick=pLineSlot->GetThick();
					pMirLineSlot->size_height=pLineSlot->GetHeight();
				}
			}
		}
		if(pCurSlot->GetPropValueStr(CLDSLineSlot::GetPropID("width"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("width"),CString(tem_str));
		if(pCurSlot->GetPropValueStr(CLDSLineSlot::GetPropID("thick"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("thick"),CString(tem_str));
		if(pCurSlot->GetPropValueStr(CLDSLineSlot::GetPropID("height"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("height"),CString(tem_str));
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("iSeg"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			pLineSlot->iSeg = SEGI(valueStr.GetBuffer());
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
					pMirLineSlot->iSeg=pLineSlot->iSeg;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("sPartNo"))
	{
		bool parsed=false;
		SEGI iSeg;
		if(CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(tem_str,&iSeg,NULL))
		{
			parsed=true;
			pPropList->SetItemPropValue(CLDSLineSlot::GetPropID("iSeg"),iSeg.ToString());
		}
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{	
			if(parsed)
				pLineSlot->iSeg=iSeg;
			pLineSlot->SetPartNo(tem_str);
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("cfgword"))
	{
		//���������˼�����ĺ�
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			pLineSlot->cfgword = pCurSlot->cfgword;
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
					pMirLineSlot->cfgword=pLineSlot->cfgword;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("m_uStatMatCoef"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->m_uStatMatCoef = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("m_hPartWeldParent"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->m_hPartWeldParent=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("m_bDisplayHole"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pLineSlot->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pLineSlot->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pLineSlot->m_bDisplayHole = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
					pMirLineSlot->m_bDisplayHole=pLineSlot->m_bDisplayHole;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("m_bVirtualPart"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pLineSlot->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pLineSlot->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pLineSlot->m_bVirtualPart = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
					pMirLineSlot->m_bVirtualPart=pLineSlot->m_bVirtualPart;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("width"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			pLineSlot->size_wide=atof(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
					pMirLineSlot->size_wide=pLineSlot->GetWidth();
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("thick"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{	
			pLineSlot->size_thick=atof(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
					pMirLineSlot->size_thick=pLineSlot->GetThick();
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("height"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			pLineSlot->size_height=atof(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
					pMirLineSlot->size_height=pLineSlot->GetHeight();
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("m_bUserSpecColor"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pLineSlot->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pLineSlot->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pLineSlot->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
					pMirLineSlot->m_bUserSpecColor=pLineSlot->m_bUserSpecColor;
			}
		}
		if(pCurSlot->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("crMaterial"),NULL);
			if(pFindItem==NULL)
			{
				char sText[MAX_TEMP_CHAR_100+1]="";
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Part Color";
				lpInfo->m_strPropHelp = "Part Color";
#else 
				lpInfo->m_strPropName = "������ɫ";
				lpInfo->m_strPropHelp = "������ɫ";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = CLDSLineSlot::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if(pCurSlot->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("crMaterial"));
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			pLineSlot->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
				CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
					pMirLineSlot->crMaterial=pLineSlot->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("sNotes"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			_snprintf(pLineSlot->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
			//��ע���ù����޸� wht 10-10-22
			/*for(RELATIVE_OBJECT *pMirObj=pLineSlot->relativeObjs.GetFirst();pMirObj;pMirObj=pLineSlot->relativeObjs.GetNext())
			{
			CLDSLineSlot *pMirLineSlot=(CLDSLineSlot*)pLineSlot->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
			if(pMirLineSlot)
			strcpy(pMirLineSlot->sNotes,pLineSlot->sNotes);
			}*/
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("hAttachTube"))
	{
		long hLineTube=0;
		sscanf(tem_str,"%X",&hLineTube);
		pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();
		CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(hLineTube,CLS_LINETUBE);
		if(pLineTube==NULL)
			hLineTube=0;
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->hAttachTube=hLineTube;
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("length"))
	{
		double fLen=atof(valueStr);
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			if(pLineSlot->hAttachTube==0)
			{
				AfxMessageBox("�����������ֹ�!");
				return FALSE;
			}
			pLineSlot->fLen=fLen;
		}
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("m_iNormCalStyle"))
	{
		char sText[MAX_TEMP_CHAR_100+1]="";
		pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("0.Specify Directly")==0)
			pLineSlot->m_iNormCalStyle = 0;
		else if(valueStr.CompareNoCase("1.Reference Rod")==0) 
			pLineSlot->m_iNormCalStyle = 1;
		else //if(valueStr.CompareNoCase("2.Horizontal Pedestal")==0)
			pLineSlot->m_iNormCalStyle = 2;
#else 
		if(valueStr.CompareNoCase("0.ֱ��ָ��")==0)
			pLineSlot->m_iNormCalStyle = 0;
		else if(valueStr.CompareNoCase("1.���ո˼�")==0) 
			pLineSlot->m_iNormCalStyle = 1;
		else //if (valueStr.CompareNoCase("2.ˮƽ����")==0)
			pLineSlot->m_iNormCalStyle = 2;
#endif
		pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("nearWorkPlaneNorm.x"),&pParentItem);
		pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("m_iNormCalStyle"),&pParentItem);//���»�ø��ڵ�
		if(pFindItem==NULL)
		{
			if(pLineSlot->m_iNormCalStyle == 1)
			{
				//��׼����Ʒ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Datum Plane Near Normal";
				lpInfo->m_strPropHelp = "Its generally specified by the user, the system can calculate it according to the near normals and channel steel axis��";
#else 
				lpInfo->m_strPropName = "��׼����Ʒ���";
				lpInfo->m_strPropHelp = "�۸ֻ�׼����Ʒ���һ�����û�ָ����ϵͳ�ɸ��ݽ��Ʒ��߼��۸����߾�ȷ������۸ֵĻ�׼�淨�ߡ�";
#endif
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 1,TRUE);
				pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("nearWorkPlaneNorm");
				pSonPropItem->m_bHideChildren = TRUE;
				if(CLDSLineSlot::GetPropStatus(pSonPropItem->m_idProp)!=-1)
					pSonPropItem->m_bHideChildren = CLDSLineSlot::GetPropStatus(pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "X Coordinate";
				lpInfo->m_strPropHelp = "The x coordinate of datum plane's normal";
#else 
				lpInfo->m_strPropName = "X�������";
				lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��X�᷽�������";
#endif
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, 1,TRUE);
				pLeafPropItem->m_idProp = CLDSLineSlot::GetPropID("nearWorkPlaneNorm.x");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pLineSlot->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Y Coordinate";
				lpInfo->m_strPropHelp = "The y coordinate of datum plane's normal";
#else 
				lpInfo->m_strPropName = "Y�������";
				lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��Y�᷽�������";
#endif
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,2,TRUE);
				pLeafPropItem->m_idProp = CLDSLineSlot::GetPropID("nearWorkPlaneNorm.y");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pLineSlot->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Z Coordinate";
				lpInfo->m_strPropHelp = "The z coordinate of datum plane's normal";
#else 
				lpInfo->m_strPropName = "Z�������";
				lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��Z�᷽�������";
#endif
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, 3,TRUE);
				pLeafPropItem->m_idProp = CLDSLineSlot::GetPropID("nearWorkPlaneNorm.z");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pLineSlot->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//hWorkNormRefPart
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Normal Reference Rod";
				lpInfo->m_strPropHelp = "It is work reference rod when calculating datum plane's normal��It is start node or end node of parent rod when it is 0X0(It can't be rod itself)";
#else 
				lpInfo->m_strPropName = "���߲��ո˼�";
				lpInfo->m_strPropHelp = "����۸ֻ�׼�淨��ʱ�Ĺ�������ո˼���0X0ʱĬ��Ϊʼ�˻��ն˽ڵ㸸�˼�(����Ϊ�˼�����)";
#endif
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 2,TRUE);
				pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("hWorkNormRefPart");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pLineSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
			pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("workPlaneNorm.x"),&pParentItem);
			pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("workPlaneNorm.x"),&pParentItem);//���»�ȡһ�����ĸ��ڵ�
			if(pFindItem == NULL)
			{
				//���ߵ�X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "X Coordinate";
				lpInfo->m_strPropHelp = "The x coordinate of datum plane's normal";
#else 
				lpInfo->m_strPropName = "X�������";
				lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��X�᷽�������";
#endif
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 3,TRUE);
				pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("workPlaneNorm.x");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pLineSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Y Coordinate";
				lpInfo->m_strPropHelp = "The y coordinate of datum plane's normal";
#else 
				lpInfo->m_strPropName = "Y�������";
				lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��Y�᷽�������";
#endif
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 4,TRUE);
				pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("workPlaneNorm.y");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pLineSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Z Coordinate";
				lpInfo->m_strPropHelp = "The z coordinate of datum plane's normal";
#else 
				lpInfo->m_strPropName = "Z�������";
				lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��Z�᷽�������";
#endif
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 5,TRUE);
				pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("workPlaneNorm.z");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pLineSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else
		{
			if(pLineSlot->m_iNormCalStyle == 0||pLineSlot->m_iNormCalStyle == 2)
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("nearWorkPlaneNorm"),&pParentItem);
				if(pFindItem!=NULL)
				{
					pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("nearWorkPlaneNorm"));
					pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("nearWorkPlaneNorm.x"));
					pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("nearWorkPlaneNorm.y"));
					pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("nearWorkPlaneNorm.z"));
					pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("hWorkNormRefPart"));
				}
			}
		}
		pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("workPlaneNorm.x"),&pParentItem);
		pFindItem->SetReadOnly(pLineSlot->m_iNormCalStyle==1||pLineSlot->m_iNormCalStyle==2);
		pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("workPlaneNorm.y"),&pParentItem);
		pFindItem->SetReadOnly(pLineSlot->m_iNormCalStyle==1||pLineSlot->m_iNormCalStyle==2);
		pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("workPlaneNorm.z"),&pParentItem);
		pFindItem->SetReadOnly(pLineSlot->m_iNormCalStyle==1||pLineSlot->m_iNormCalStyle==2);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("nearWorkPlaneNorm.x"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{	//���¼��㷨�߷���
			pLineSlot->nearWorkPlaneNorm.x = atof(valueStr);
			pLineSlot->CalWorkPlaneNorm();
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("nearWorkPlaneNorm.y"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{	//���¼��㷨�߷���
			pLineSlot->nearWorkPlaneNorm.y = atof(valueStr);
			pLineSlot->CalWorkPlaneNorm();
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("nearWorkPlaneNorm.z"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{	//���¼��㷨�߷���
			pLineSlot->nearWorkPlaneNorm.z = atof(valueStr);
			pLineSlot->CalWorkPlaneNorm();
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("workPlaneNorm.x"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{	//���¼��㷨�߷���
			GEPOINT workPlaneNorm=pLineSlot->WorkPlaneNorm();
			workPlaneNorm.x=atof(valueStr);
			pLineSlot->SetWorkPlaneNorm(workPlaneNorm);
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("workPlaneNorm.y"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{	//���¼��㷨�߷���
			GEPOINT workPlaneNorm=pLineSlot->WorkPlaneNorm();
			workPlaneNorm.y=atof(valueStr);
			pLineSlot->SetWorkPlaneNorm(workPlaneNorm);
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("workPlaneNorm.z"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{	//���¼��㷨�߷���
			GEPOINT workPlaneNorm=pLineSlot->WorkPlaneNorm();
			workPlaneNorm.z=atof(valueStr);
			pLineSlot->SetWorkPlaneNorm(workPlaneNorm);
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("hWorkNormRefPart"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineSlot->hWorkNormRefPart);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("oddStart"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{	//����ֶ������˸˼�����ͷӦ���ȣ�������ͷ�ļ��㷽ʽ�޸�Ϊ������
			//�����������ø˼���صĸְ�ʱ�������¼�������ͷ��ֵ	wht 09-09-02
			pLineSlot->desStartOdd.m_iOddCalStyle=2;
			pLineSlot->SetStartOdd(atof(valueStr));//,TRUE,TRUE);���͸ִ��벻���ƣ��ݲ�������˨�������塡wjh-2018.1.8
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("startPos.x"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->pStart->SetPositionX(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("startPos.y"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->pStart->SetPositionY(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("startPos.z"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->pStart->SetPositionZ(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.endPosType"))
	{
		int end_pos_type=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Node Is Datum Node")==0)
			end_pos_type = 0;
		else if(valueStr.CompareNoCase("Lap Joint Connect")==0)
			end_pos_type = 1;
		else if(valueStr.CompareNoCase("Point Line Project")==0)
			end_pos_type = 2;
		else if(valueStr.CompareNoCase("Center Line Cross Point")==0)
			end_pos_type = 3;
		else if(valueStr.CompareNoCase("Specified Coordinate")==0)
#else 
		if(valueStr.CompareNoCase("�ڵ㼴����")==0)
			end_pos_type = 0;
		else if(valueStr.CompareNoCase("�������")==0)
			end_pos_type = 1;
		else if(valueStr.CompareNoCase("����ͶӰ")==0)
			end_pos_type = 2;
		else if(valueStr.CompareNoCase("���߽���")==0)
			end_pos_type = 3;
		else if(valueStr.CompareNoCase("ָ������")==0)
#endif
			end_pos_type = 4;
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->desStartPos.endPosType = end_pos_type;
		UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.datum_pos_style"))
	{
		int datum_pos_style=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Ridge Plane Project")==0)
			datum_pos_style = 0;
		else if(valueStr.CompareNoCase("X Leg Project")==0)
			datum_pos_style = 1;
		else if(valueStr.CompareNoCase("Y Leg Project")==0)
			datum_pos_style = 2;
		else if(valueStr.CompareNoCase("Ridge Plane Offset")==0)
			datum_pos_style = 3;
		else if(valueStr.CompareNoCase("X Leg Plane Offset")==0)
			datum_pos_style = 4;
		else if(valueStr.CompareNoCase("Y Leg Plane Offset")==0)
#else 
		if(valueStr.CompareNoCase("������ͶӰ")==0)
			datum_pos_style = 0;
		else if(valueStr.CompareNoCase("X֫����ͶӰ")==0)
			datum_pos_style = 1;
		else if(valueStr.CompareNoCase("Y֫����ͶӰ")==0)
			datum_pos_style = 2;
		else if(valueStr.CompareNoCase("������ƽ��")==0)
			datum_pos_style = 3;
		else if(valueStr.CompareNoCase("X֫����ƽ��")==0)
			datum_pos_style = 4;
		else if(valueStr.CompareNoCase("Y֫����ƽ��")==0)
#endif
			datum_pos_style = 5;
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->desStartPos.datum_pos_style = datum_pos_style;
		pParentItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desStartPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
		if(datum_pos_style>2)
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Offset Normal";
#else 
			vecDlg.m_sCaption = "ƽ���淨��";
#endif
			vecDlg.m_fVectorX = pCurSlot->desStartPos.face_offset_norm.x;
			vecDlg.m_fVectorY = pCurSlot->desStartPos.face_offset_norm.y;
			vecDlg.m_fVectorZ = pCurSlot->desStartPos.face_offset_norm.z;
			if(vecDlg.DoModal()==IDOK)
			{
				for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
					pLineSlot->desStartPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumPart"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineSlot->desStartPos.hDatumPart);
		pParentItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desStartPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.fNormOffset"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->desStartPos.fNormOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.fEccentricDist"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->desStartPos.fEccentricDist = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.fTransverseOffset"))
	{	//����ƫ����
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->desStartPos.fTransverseOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumStartPart"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineSlot->desStartPos.hDatumStartPart);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.bDatumStartStartEnd"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start")==0)
				pLineSlot->desStartPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineSlot->desStartPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
#endif
				pLineSlot->desStartPos.bDatumStartStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.hDatumEndPart"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineSlot->desStartPos.hDatumEndPart);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desStartPos.bDatumEndStartEnd"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start")==0)
				pLineSlot->desStartPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineSlot->desStartPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
#endif
				pLineSlot->desStartPos.bDatumEndStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("oddEnd"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{	//����ֶ������˸˼�����ͷӦ���ȣ�������ͷ�ļ��㷽ʽ�޸�Ϊ������
			//�����������ø˼���صĸְ�ʱ�������¼�������ͷ��ֵ	wht 09-09-02
			pLineSlot->desEndOdd.m_iOddCalStyle=2;
			pLineSlot->SetEndOdd(atof(valueStr));//,TRUE,TRUE);���͸ִ��벻���ƣ��ݲ�������˨�������塡wjh-2018.1.8
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("endPos.x"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->pEnd->SetPositionX(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("endPos.y"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->pEnd->SetPositionY(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("endPos.z"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->pEnd->SetPositionZ(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.endPosType"))
	{
		int end_pos_type=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Node Is Datum Node")==0)
			end_pos_type = 0;
		else if(valueStr.CompareNoCase("Lap Joint Connect")==0)
			end_pos_type = 1;
		else if(valueStr.CompareNoCase("Point Line Project")==0)
			end_pos_type = 2;
		else if(valueStr.CompareNoCase("Center Line Cross Point")==0)
			end_pos_type = 3;
		else if(valueStr.CompareNoCase("Specified Coordinate")==0)
#else 
		if(valueStr.CompareNoCase("�ڵ㼴����")==0)
			end_pos_type = 0;
		else if(valueStr.CompareNoCase("�������")==0)
			end_pos_type = 1;
		else if(valueStr.CompareNoCase("����ͶӰ")==0)
			end_pos_type = 2;
		else if(valueStr.CompareNoCase("���߽���")==0)
			end_pos_type = 3;
		else if(valueStr.CompareNoCase("ָ������")==0)
#endif
			end_pos_type = 4;
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->desEndPos.endPosType = end_pos_type;
		UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.datum_pos_style"))
	{
		int datum_pos_style=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Ridge Plane Project")==0)
			datum_pos_style = 0;
		else if(valueStr.CompareNoCase("X Leg Project")==0)
			datum_pos_style = 1;
		else if(valueStr.CompareNoCase("Y Leg Project")==0)
			datum_pos_style = 2;
		else if(valueStr.CompareNoCase("Ridge Plane Offset")==0)
			datum_pos_style = 3;
		else if(valueStr.CompareNoCase("X Leg Plane Offset")==0)
			datum_pos_style = 4;
		else if(valueStr.CompareNoCase("Y Leg Plane Offset")==0)
#else 
		if(valueStr.CompareNoCase("������ͶӰ")==0)
			datum_pos_style = 0;
		else if(valueStr.CompareNoCase("X֫����ͶӰ")==0)
			datum_pos_style = 1;
		else if(valueStr.CompareNoCase("Y֫����ͶӰ")==0)
			datum_pos_style = 2;
		else if(valueStr.CompareNoCase("������ƽ��")==0)
			datum_pos_style = 3;
		else if(valueStr.CompareNoCase("X֫����ƽ��")==0)
			datum_pos_style = 4;
		else if(valueStr.CompareNoCase("Y֫����ƽ��")==0)
#endif
			datum_pos_style = 5;
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->desEndPos.datum_pos_style = datum_pos_style;
		pParentItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desEndPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
		if(datum_pos_style>2)
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Offset Normal";
#else 
			vecDlg.m_sCaption = "ƽ���淨��";
#endif
			vecDlg.m_fVectorX = pCurSlot->desEndPos.face_offset_norm.x;
			vecDlg.m_fVectorY = pCurSlot->desEndPos.face_offset_norm.y;
			vecDlg.m_fVectorZ = pCurSlot->desEndPos.face_offset_norm.z;
			if(vecDlg.DoModal()==IDOK)
			{
				for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
					pLineSlot->desEndPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.hDatumPart"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineSlot->desEndPos.hDatumPart);
		pParentItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desEndPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.fNormOffset"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->desEndPos.fNormOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.fEccentricDist"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->desEndPos.fEccentricDist = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.fTransverseOffset"))
	{	//����ƫ����
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			pLineSlot->desEndPos.fTransverseOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.hDatumStartPart"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineSlot->desEndPos.hDatumStartPart);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.bDatumStartStartEnd"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start")==0)
				pLineSlot->desEndPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineSlot->desEndPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
#endif
				pLineSlot->desEndPos.bDatumStartStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.hDatumEndPart"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineSlot->desEndPos.hDatumEndPart);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desEndPos.bDatumEndStartEnd"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start")==0)
				pLineSlot->desEndPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineSlot->desEndPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
#endif
				pLineSlot->desEndPos.bDatumEndStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desDatumPoint.x"))
	{
		GEPOINT pos;
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			pos=pLineSlot->desDatumPoint.Position();
			pLineSlot->desDatumPoint.SetPosition(atof(valueStr),pos.y,pos.z);
		}
		CXhChar100 sText;
		long idProp=CLDSLineSlot::GetPropID("desDatumPoint");
		if(pCurSlot->GetPropValueStr(idProp,sText)>0)
			pPropList->SetItemPropValue(idProp,sText);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desDatumPoint.y"))
	{
		GEPOINT pos;
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			pos=pLineSlot->desDatumPoint.Position();
			pLineSlot->desDatumPoint.SetPosition(pos.x,atof(valueStr),pos.z);
		}
		CXhChar100 sText;
		long idProp=CLDSLineSlot::GetPropID("desDatumPoint");
		if(pCurSlot->GetPropValueStr(idProp,sText)>0)
			pPropList->SetItemPropValue(idProp,sText);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desDatumPoint.z"))
	{
		GEPOINT pos;
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			pos=pLineSlot->desDatumPoint.Position();
			pLineSlot->desDatumPoint.SetPosition(pos.x,pos.y,atof(valueStr));
		}
		CXhChar100 sText;
		long idProp=CLDSLineSlot::GetPropID("desDatumPoint");
		if(pCurSlot->GetPropValueStr(idProp,sText)>0)
			pPropList->SetItemPropValue(idProp,sText);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desDatumPoint.param.hOffsetDatumNode"))
	{
		long hNode=HexStrToLong(valueStr);
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			if(pLineSlot->desDatumPoint.datum_pos_style==11)
			{
				if(pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode>=0)
					pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=hNode;
				else
					pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=-hNode;
				pLineSlot->SetModified();
				//�¸��������ڵ�ͬʱһ�㻹�����ƫ������ƫ�����ͣ���ʱ���¼���۸�λ��û������ wjh-2015.10.12
				//pLineSlot->CalPosition(true);
			}
		}
		return TRUE;
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desDatumPoint.param.lenOffsetType"))
	{
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			if(pLineSlot->desDatumPoint.datum_pos_style==11)
			{
				if(valueStr[0]=='0')//0��ʼ->��;
					pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode= abs(pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode);
				else
					pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=-abs(pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode);
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desDatumPoint.param.lenOffset"))
	{
		double lenOffset=atof(valueStr);
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			if(pLineSlot->desDatumPoint.datum_pos_style==11)
				pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.len_offset= lenOffset;
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecStyle"))
	{
		char style=valueStr[0]-'0';
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			if(pLineSlot->desDatumPoint.datum_pos_style==11)
			{
				pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style= style;
				if(style==4)	//ˮƽ���ò۸ֵ���
					pLineSlot->m_iNormCalStyle=2;
				else if(style!=4&&pLineSlot->m_iNormCalStyle==2)
					pLineSlot->m_iNormCalStyle=0;
			}
		}
		UpdateLineSlotColumnRayVecParamsProperty(pPropList,pItem);
		CXhChar100 sText;
		pCurSlot->CalWorkPlaneNorm();
		long propId=CLDSLineSlot::GetPropID("m_iNormCalStyle");
		if(pCurSlot->GetPropValueStr(propId,sText)>0)
			pPropList->SetItemPropValue(propId,sText);
		propId=CLDSLineSlot::GetPropID("workPlaneNorm.x");
		if(pCurSlot->GetPropValueStr(propId,sText)>0)
			pPropList->SetItemPropValue(propId,sText);
		propId=CLDSLineSlot::GetPropID("workPlaneNorm.y");
		if(pCurSlot->GetPropValueStr(propId,sText)>0)
			pPropList->SetItemPropValue(propId,sText);
		propId=CLDSLineSlot::GetPropID("workPlaneNorm.z");
		if(pCurSlot->GetPropValueStr(propId,sText)>0)
			pPropList->SetItemPropValue(propId,sText);
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecStdStyle"))
	{
		long idPropX = CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.x");
		long idPropY = CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.y");
		long idPropZ = CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.z");
		if(valueStr[0]=='0')	//�Զ��巽��
		{
			pPropList->SetItemReadOnly(idPropX,pCurSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style!=0);
			pPropList->SetItemReadOnly(idPropY,pCurSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style!=0);
			pPropList->SetItemReadOnly(idPropZ,pCurSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style!=0);
		}
		else
		{
			pPropList->SetItemReadOnly(idPropX,TRUE);
			pPropList->SetItemReadOnly(idPropY,TRUE);
			pPropList->SetItemReadOnly(idPropZ,TRUE);
			f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
			pCurSlot->desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec=vector;
			CXhChar100 sText;
			if(pCurSlot->GetPropValueStr(idPropX,sText))
				pPropList->SetItemPropValue(idPropX,sText);
			if(pCurSlot->GetPropValueStr(idPropY,sText))
				pPropList->SetItemPropValue(idPropY,sText);
			if(pCurSlot->GetPropValueStr(idPropZ,sText))
				pPropList->SetItemPropValue(idPropZ,sText);
			for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
			{
				if(pLineSlot->desDatumPoint.datum_pos_style==11)
					pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec=vector;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecRotateAngle"))
	{
		double rotangle=atof(valueStr);
		for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
		{
			if(pLineSlot->desDatumPoint.datum_pos_style==11)
				pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_angle= rotangle;
		}
	}
	else
		return FALSE;
	for(pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetFirst();pLineSlot;pLineSlot=(CLDSLineSlot*)pView->selectObjs.GetNext())
	{
		pLineSlot->SetModified();
		pLineSlot->CalPosition(true);	
		pLineSlot->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pLineSlot->GetSolidPartObject());
		if(pLineSlot->desDatumPoint.datum_pos_style==11)
		{
			for(CLsRef* pLsRef=pLineSlot->GetFirstLsRef();pLsRef;pLsRef=pLineSlot->GetNextLsRef())
			{
				(*pLsRef)->correct_worknorm();
				(*pLsRef)->correct_pos();
				(*pLsRef)->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
			}
		}
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

void UpdateLineSlot_DesPos_EndPosType_TreeItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineSlot *pFirstSlot = (CLDSLineSlot*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem;
	int idProp;
	CItemInfo *lpInfo=NULL;
	if(pFirstSlot==NULL||pParentItem==NULL)
		return;
	CTubeEndPosPara *pDesPos;
	char valueStr[MAX_TEMP_CHAR_50+1]="";
	const int GROUP_GENERAL=1,GROUP_START=2,GROUP_END=3;
	DWORD dwPropGroup=0;
	if(bStart)
	{
		dwPropGroup=GetSingleWord(GROUP_START);
		pDesPos=&pFirstSlot->desStartPos;
	}
	else
	{
		dwPropGroup=GetSingleWord(GROUP_END);
		pDesPos=&pFirstSlot->desEndPos;
	}
	char sText[MAX_TEMP_CHAR_100+1]="";
	//0:�˵㼴�ڵ�;1:����ڽڵ㸸�˼���;2:�ڵ���ָ����׼��ͶӰ;3:�˼����߽���;4:ָ������; wht 10-10-20
	if(pDesPos->endPosType==0)	
	{
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.hDatumPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.fTransverseOffset"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.hDatumStartPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.datum_pos_style"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.hDatumPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.fTransverseOffset"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.hDatumStartPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.datum_pos_style"));
		}
		//��׼�淨��ƫ����
		if(bStart)
			idProp=CLDSLineSlot::GetPropID("desStartPos.fNormOffset");
		else
			idProp=CLDSLineSlot::GetPropID("desEndPos.fNormOffset");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Normal Offset";
			lpInfo->m_strPropHelp = "The datum plane's normal offset";
#else 
			lpInfo->m_strPropName = "��׼�淨��ƫ����";
			lpInfo->m_strPropHelp = "��׼�淨��ƫ����";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		else
			pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
	}
	else if(pDesPos->endPosType==1)
	{
		CLDSLinePart *pDatumLinePart=NULL;
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.hDatumStartPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.face_offset_norm"));
			//
			pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstSlot->desStartPos.hDatumPart,CLS_LINEPART);
			if(pDatumLinePart==NULL&&pFirstSlot->pStart)
				pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstSlot->pStart->hFatherPart,CLS_LINEPART);
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.hDatumStartPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.face_offset_norm"));
			//
			pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstSlot->desEndPos.hDatumPart,CLS_LINEPART);
			if(pDatumLinePart==NULL&&pFirstSlot->pEnd)
				pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstSlot->pEnd->hFatherPart,CLS_LINEPART);
		}
		BOOL bIsDatumJg=FALSE;
		if(pDatumLinePart&&pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE)
			bIsDatumJg=TRUE;
		//���λ�û�׼�˼�
		if(bStart)
			idProp=CLDSLineSlot::GetPropID("desStartPos.hDatumPart");
		else
			idProp=CLDSLineSlot::GetPropID("desEndPos.hDatumPart");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			if(bIsDatumJg)
			{
				lpInfo->m_strPropName = "Lap Joint Datum Angle";
				lpInfo->m_strPropHelp = "The handle of lap joint datum angle";
			}
			else
			{
				lpInfo->m_strPropName = "Lap Joint Datum Tube";
				lpInfo->m_strPropHelp = "The handle of lap joint datum tube";
			}
#else 
			if(bIsDatumJg)
			{
				lpInfo->m_strPropName = "���λ�û�׼�Ǹ�";
				lpInfo->m_strPropHelp = "�۸ִ��λ�û�׼�Ǹ־��";
			}
			else
			{
				lpInfo->m_strPropName = "���λ�û�׼�ֹ�";
				lpInfo->m_strPropHelp = "�۸ִ��λ�û�׼�ֹܾ��";
			}
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(bIsDatumJg)
			{
				pPropItem->m_lpNodeInfo->m_strPropName = "Lap Joint Datum Angle";
				pPropItem->m_lpNodeInfo->m_strPropHelp = "The handle of lap joint datum angle";
			}
			else
			{
				pPropItem->m_lpNodeInfo->m_strPropName = "Lap Joint Datum Tube";
				pPropItem->m_lpNodeInfo->m_strPropHelp = "The handle of lap joint datum tube";
			}
#else 
			if(bIsDatumJg)
			{
				pPropItem->m_lpNodeInfo->m_strPropName = "���λ�û�׼�Ǹ�";
				pPropItem->m_lpNodeInfo->m_strPropHelp = "�۸ִ��λ�û�׼�Ǹ־��";
			}
			else
			{
				pPropItem->m_lpNodeInfo->m_strPropName = "���λ�û�׼�ֹ�";
				pPropItem->m_lpNodeInfo->m_strPropHelp = "�۸ִ��λ�û�׼�ֹܾ��";
			}
#endif
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		else
			pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		//
		if(bStart)
			idProp=CLDSLineSlot::GetPropID("desStartPos.datum_pos_style");
		else
			idProp=CLDSLineSlot::GetPropID("desEndPos.datum_pos_style");
		if(bIsDatumJg)
		{	//���㶨λ��ʽ
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType=CDT_LIST;	
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Datum Point Locate Mode";
				if(bStart)
					lpInfo->m_strPropHelp = "The start locate mode of u-steel";
				else 
					lpInfo->m_strPropHelp = "The end locate mode of u-steel";
				lpInfo->m_cmbItems = "Ridge Plane Project|X Leg Project|Y Leg Project|Ridge Plane Offset|X Leg Plane Offset|Y Leg Plane Offset|";
#else 
				lpInfo->m_strPropName = "���㶨λ��ʽ";
				if(bStart)
					lpInfo->m_strPropHelp = "�۸�ʼ�˻��㶨λ��ʽ";
				else 
					lpInfo->m_strPropHelp = "�۸��ն˻��㶨λ��ʽ";
				lpInfo->m_cmbItems = "����ͶӰ|X֫����ͶӰ|Y֫����ͶӰ|������ƽ��|X֫����ƽ��|Y֫����ƽ��|";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,1,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
				if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,valueStr))
					pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			else
			{
				if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropList->SetItemPropValue(idProp,CString(valueStr));
			}
			if(bStart)
				pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
			else
				pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		}
		else
			pPropList->DeleteItemByPropId(idProp);
		//��׼�淨��ƫ����
		if(bStart)
			idProp=CLDSLineSlot::GetPropID("desStartPos.fNormOffset");
		else
			idProp=CLDSLineSlot::GetPropID("desEndPos.fNormOffset");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Normal Offset";
			lpInfo->m_strPropHelp = "The datum plane's normal offset";
#else 
			lpInfo->m_strPropName = "��׼�淨��ƫ����";
			lpInfo->m_strPropHelp = "��׼�淨��ƫ����";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,2,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		else
			pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		//����ƫ����
		if(bStart)
			idProp=CLDSLineSlot::GetPropID("desStartPos.fTransverseOffset");
		else
			idProp=CLDSLineSlot::GetPropID("desEndPos.fTransverseOffset");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Lateral Offset";
			lpInfo->m_strPropHelp = "The offset of x negative direction";
#else 
			lpInfo->m_strPropName = "����ƫ����";
			lpInfo->m_strPropHelp = "X�Ḻ����ƫ����";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,3,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		else
			pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		if(!bIsDatumJg)
		{	//ƫ�ľ�
			if(bStart)
				idProp=CLDSLineSlot::GetPropID("desStartPos.fEccentricDist");
			else
				idProp=CLDSLineSlot::GetPropID("desEndPos.fEccentricDist");
			pPropItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Offset Distance";
				lpInfo->m_strPropHelp = "Datum offset distance";
#else 
				lpInfo->m_strPropName = "ƫ�ľ�";
				lpInfo->m_strPropHelp = "��׼ƫ�ľ�";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,4,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
				if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			if(bStart)
				pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
			else
				pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		}
		if((bIsDatumJg&&pDesPos->datum_pos_style>2)||!bIsDatumJg)
		{	//ƽ���淨��
			if(bStart)
				idProp=CLDSLineSlot::GetPropID("desStartPos.face_offset_norm");
			else
				idProp=CLDSLineSlot::GetPropID("desEndPos.face_offset_norm");
			pPropItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Offset Normal";
				lpInfo->m_strPropHelp = "Offset Normal";
#else 
				lpInfo->m_strPropName = "ƽ���淨��";
				lpInfo->m_strPropHelp = "ƽ���淨��";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,5,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
				if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			if(bStart)
				pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
			else
				pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		}
	}
	else if(pDesPos->endPosType==2)
	{
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.hDatumPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.fTransverseOffset"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.fNormOffset"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.hDatumPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.fTransverseOffset"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.fNormOffset"));
		}
		//ͶӰ��ʼ�����ڻ�׼�˼�
		if(bStart)
			idProp=CLDSLineSlot::GetPropID("desStartPos.hDatumStartPart");
		else
			idProp=CLDSLineSlot::GetPropID("desEndPos.hDatumStartPart");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Start Datum Rod";
			lpInfo->m_strPropHelp = "The datum rod  is at project line's start ";
#else 
			lpInfo->m_strPropName = "ͶӰ��ʼ�����ڻ�׼�˼�";
			lpInfo->m_strPropHelp = "ͶӰ��ʼ�����ڻ�׼�˼�";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		else
			pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		//��׼�˼��Ļ�׼�˵�
		if(bStart)
			idProp=CLDSLineSlot::GetPropID("desStartPos.bDatumStartStartEnd");
		else
			idProp=CLDSLineSlot::GetPropID("desEndPos.bDatumStartStartEnd");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Endpoint";
			lpInfo->m_strPropHelp = "The datum flat steel's datum endpoint";
			lpInfo->m_cmbItems = "Start|End|";
#else 
			lpInfo->m_strPropName = "��׼��ֵĻ�׼�˵�";
			lpInfo->m_strPropHelp = "��׼��ֵĻ�׼�˵�";
			lpInfo->m_cmbItems = "ʼ��|�ն�|";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,1,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		else
			pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		//ͶӰ���ն����ڻ�׼�˼�
		if(bStart)
			idProp=CLDSLineSlot::GetPropID("desStartPos.hDatumEndPart");
		else
			idProp=CLDSLineSlot::GetPropID("desEndPos.hDatumEndPart");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "End Datum Rod";
			lpInfo->m_strPropHelp = "The datum rod at project line's end";
#else 
			lpInfo->m_strPropName = "ͶӰ���ն����ڻ�׼�˼�";
			lpInfo->m_strPropHelp = "ͶӰ���ն����ڻ�׼�˼�";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,2,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else 
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropName = "End Datum Rod";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "The datum rod at project line's end";
#else 
			pPropItem->m_lpNodeInfo->m_strPropName = "ͶӰ���ն����ڻ�׼�˼�";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "ͶӰ���ն����ڻ�׼�˼�";
#endif
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		else
			pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		//��׼�˼��Ļ�׼�˵�
		if(bStart)
			idProp=CLDSLineSlot::GetPropID("desStartPos.bDatumEndStartEnd");
		else
			idProp=CLDSLineSlot::GetPropID("desEndPos.bDatumEndStartEnd");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Endpoint";
			lpInfo->m_strPropHelp = "The datum rod's datum endpoint";
			lpInfo->m_cmbItems = "Start|End|";
#else 
			lpInfo->m_strPropName = "��׼�˼��Ļ�׼�˵�";
			lpInfo->m_strPropHelp = "��׼�˼��Ļ�׼�˵�";
			lpInfo->m_cmbItems = "ʼ��|�ն�|";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,3,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		else
			pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
	}
	else if(pDesPos->endPosType==3)
	{	//�˼����߽���
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.datum_pos_style"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.fTransverseOffset"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.datum_pos_style"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.fTransverseOffset"));
		}
		//��׼�˼�
		if(bStart)
			idProp=CLDSLineSlot::GetPropID("desStartPos.hDatumPart");
		else
			idProp=CLDSLineSlot::GetPropID("desEndPos.hDatumPart");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Rod";
			lpInfo->m_strPropHelp = "The Datum Rod of the crossing center line";
#else 
			lpInfo->m_strPropName = "��׼�˼�";
			lpInfo->m_strPropHelp = "���߽����׼�˼�";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropName = "Datum Rod";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "The Datum Rod of the crossing center line";
#else 
			pPropItem->m_lpNodeInfo->m_strPropName = "��׼�˼�";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "���߽����׼�˼�";
#endif
		}
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		//����˼�
		if(bStart)
			idProp=CLDSLineSlot::GetPropID("desStartPos.hDatumStartPart");
		else
			idProp=CLDSLineSlot::GetPropID("desEndPos.hDatumStartPart");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Cross Rod";
			lpInfo->m_strPropHelp = "The crossing rod at cross point of center line ";
#else 
			lpInfo->m_strPropName = "����˼�";
			lpInfo->m_strPropHelp = "���߽��㽻��˼�";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,1,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropName = "Cross Rod";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "The crossing rod at cross point of center line ";
#else 
			pPropItem->m_lpNodeInfo->m_strPropName = "����˼�";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "���߽��㽻��˼�";
#endif
		}
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		//��׼�淨��ƫ����
		if(bStart)
			idProp=CLDSLineSlot::GetPropID("desStartPos.fNormOffset");
		else
			idProp=CLDSLineSlot::GetPropID("desEndPos.fNormOffset");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Normal Offset";
			lpInfo->m_strPropHelp = "The datum plane's normal offset";
#else 
			lpInfo->m_strPropName = "��׼�淨��ƫ����";
			lpInfo->m_strPropHelp = "��׼�淨��ƫ����";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,2,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
	}
	else //if(pDesPos->endPosType==4)
	{	//ֱ��ָ������
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.hDatumPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.hDatumStartPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.datum_pos_style"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.fNormOffset"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desStartPos.fTransverseOffset"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.hDatumPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.hDatumStartPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.datum_pos_style"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.fNormOffset"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desEndPos.fTransverseOffset"));
		}
	}
	if(bStart)
	{	
		if(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart)
		{	//�������Ϊֻ������
			SetPropItemReadOnly(pFirstSlot,"startPos.x",TRUE);
			SetPropItemReadOnly(pFirstSlot,"startPos.y",TRUE);
			SetPropItemReadOnly(pFirstSlot,"startPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ���ж�
			SetPropItemReadOnly(pFirstSlot,"startPos.x",pFirstSlot->desStartPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstSlot,"startPos.y",pFirstSlot->desStartPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstSlot,"startPos.z",pFirstSlot->desStartPos.endPosType!=4);	//4.ָ������
		}
	}
	else
	{
		if(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd)
		{	//�������Ϊֻ������
			SetPropItemReadOnly(pFirstSlot,"endPos.x",TRUE);
			SetPropItemReadOnly(pFirstSlot,"endPos.y",TRUE);
			SetPropItemReadOnly(pFirstSlot,"endPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ���ж�
			SetPropItemReadOnly(pFirstSlot,"endPos.x",pFirstSlot->desEndPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstSlot,"endPos.y",pFirstSlot->desEndPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstSlot,"endPos.z",pFirstSlot->desEndPos.endPosType!=4);	//4.ָ������
		}
	}
}

void UpdateLineFlat_DesPos_EndPosType_TreeItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,BOOL bStart);
BOOL LineFlatButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem,*pParentItem;
	CLDSLineFlat *pLineFlat=NULL, *pFirstLineFlat=NULL;
	CReportVectorDlg vecDlg;
	CReportPointDlg pointDlg;
	CCfgPartNoDlg cfgdlg;
	COddmentDesignDlg odddlg;
	char sText[MAX_TEMP_CHAR_100+1];
	CString valueStr = _T("");

	if(pView->selectObjs.GetNodeNum()>0)
		pFirstLineFlat = (CLDSLineFlat*)pView->selectObjs.GetFirst();
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
	if(pItem->m_idProp==CLDSLineFlat::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstLineFlat->cfgword; 
		if(pFirstLineFlat->layer(0)!='L')
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
		{
			for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			{
				pLineFlat->cfgword = cfgdlg.cfgword;		//�ڴ˴����µ�һ���ڵ������Ϻ�
				pLineFlat->SetModified(TRUE,FALSE);
				pLineFlat->SyncMirProp("cfgword");
			}
		}
		if(pFirstLineFlat->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("sPartNo"))
	{
		CModifyRelativeObjPartNoDlg modify_partno_dlg;
		modify_partno_dlg.m_pWorkPart=pFirstLineFlat;
		if(modify_partno_dlg.DoModal()==IDOK)
		{
			pFirstLineFlat->SetPartNo(modify_partno_dlg.m_sCurObjPartNo.GetBuffer(0));
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
			SEGI iSeg;
			if(CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(modify_partno_dlg.m_sCurObjPartNo,&iSeg,NULL))
			{
				//pView->selectObjs�����๹����ŵĶκ�ͬ����������CModifyRelativeObjPartNoDlg����� wjh-2016.6.14
				pFirstLineFlat->iSeg=iSeg;
				pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("iSeg"),iSeg.ToString());
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("nearWorkPlaneNorm"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Near Normal Coordinate";
#else 
		vecDlg.m_sCaption = "��ֻ�׼����Ʒ�������";
#endif
		vecDlg.m_fVectorX = pFirstLineFlat->nearWorkPlaneNorm.x;
		vecDlg.m_fVectorY = pFirstLineFlat->nearWorkPlaneNorm.y;
		vecDlg.m_fVectorZ = pFirstLineFlat->nearWorkPlaneNorm.z;
		pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("nearWorkPlaneNorm.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("nearWorkPlaneNorm.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("nearWorkPlaneNorm.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("nearWorkPlaneNorm.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("nearWorkPlaneNorm.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("nearWorkPlaneNorm.z"),CString(sText));
			for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
				pLineFlat->nearWorkPlaneNorm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("workPlaneNorm")
		||pItem->m_idProp==CLDSLineFlat::GetPropID("workPlaneNorm.x")
		||pItem->m_idProp==CLDSLineFlat::GetPropID("workPlaneNorm.y")
		||pItem->m_idProp==CLDSLineFlat::GetPropID("workPlaneNorm.z"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Normal Coordinate";
#else 
		vecDlg.m_sCaption = "��ֻ�׼�淨������";
#endif
		vecDlg.m_fVectorX = pFirstLineFlat->WorkPlaneNorm().x;
		vecDlg.m_fVectorY = pFirstLineFlat->WorkPlaneNorm().y;
		vecDlg.m_fVectorZ = pFirstLineFlat->WorkPlaneNorm().z;
		pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("workPlaneNorm.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("workPlaneNorm.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("workPlaneNorm.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("workPlaneNorm.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("workPlaneNorm.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("workPlaneNorm.z"),CString(sText));
			for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
				pLineFlat->SetWorkPlaneNorm(f3dPoint(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ));
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("hStart"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Rod Start Node";
		modifyhdlg.m_sTitle="Start Node:";
#else 
		modifyhdlg.m_sCaption="�޸ĸ˼�ʼ�˽ڵ�";
		modifyhdlg.m_sTitle="ʼ�˽ڵ�:";
#endif
		if(pFirstLineFlat->pStart)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineFlat->pStart->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the datum node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�ڵ�:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind nodes��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
			else
			{
				CUndoOperObject undo(&Ta);
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("hStart"),valueStr);
				for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
				{
					CLDSNode* pOldNode=pLineFlat->pStart;
					pLineFlat->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),pNode,NULL);
					pLineFlat->pStart=pNode;
					pLineFlat->SetModified();
					pLineFlat->ClearFlag();
					pLineFlat->CalPosition();
					//���¸˼��ϵĶ˽ڵ������Ϣ
					pLineFlat->UpdateRodEndNodeRelation(pOldNode,pNode);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("hEnd"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Rod End Node";
		modifyhdlg.m_sTitle="End Node:";
#else 
		modifyhdlg.m_sCaption="�޸ĸ˼��ն˽ڵ�";
		modifyhdlg.m_sTitle="�ն˽ڵ�:";
#endif
		if(pFirstLineFlat->pEnd)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineFlat->pEnd->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the datum node:");
#else 
		modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�ڵ�:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind nodes��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
			else
			{
				CUndoOperObject undo(&Ta);
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("hEnd"),valueStr);
				for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
				{
					CLDSNode* pOldNode=pLineFlat->pEnd;
					pLineFlat->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),NULL,pNode);
					pLineFlat->pEnd=pNode;
					pLineFlat->SetModified();
					pLineFlat->ClearFlag();
					pLineFlat->CalPosition();
					//���¸˼��ϵĶ˽ڵ������Ϣ
					pLineFlat->UpdateRodEndNodeRelation(pOldNode,pNode);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("oddStart"))
	{
		odddlg.m_hRefPart1.Format("0X%X",pFirstLineFlat->desStartOdd.m_hRefPart1);
		odddlg.m_hRefPart2.Format("0X%X",pFirstLineFlat->desStartOdd.m_hRefPart2);
		odddlg.m_fCollideDist = pFirstLineFlat->desStartOdd.m_fCollideDist;
		odddlg.m_iOddCalStyle = pFirstLineFlat->desStartOdd.m_iOddCalStyle;
		if(odddlg.DoModal()==IDOK)
		{
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart1);
			sscanf(sText,"%X",&pFirstLineFlat->desStartOdd.m_hRefPart1);
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart2);
			sscanf(sText,"%X",&pFirstLineFlat->desStartOdd.m_hRefPart2);
			pFirstLineFlat->desStartOdd.m_fCollideDist = odddlg.m_fCollideDist;
			pFirstLineFlat->desStartOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
			pFirstLineFlat->CalStartOddment();
			char valueStr[MAX_TEMP_CHAR_50+1]="";
			if(pFirstLineFlat->GetPropValueStr(pItem->m_idProp,valueStr)>0)
				pItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("startPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "Start Design Coordinate";
#else 
		pointDlg.m_sCaption = "���ʼ���������";
#endif
		pointDlg.m_fPointX = pFirstLineFlat->Start().x;
		pointDlg.m_fPointY = pFirstLineFlat->Start().y;
		pointDlg.m_fPointZ = pFirstLineFlat->Start().z;
		if(pFirstLineFlat->desStartPos.endPosType!=4)	//4.ָ������
			pointDlg.m_bReadOnly=TRUE;
		pointDlg.m_bLockPoint=pFirstLineFlat->IsStartPosLocked();
		if(pointDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("startPos.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("startPos.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("startPos.z"),CString(sText));
			for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			{
				pLineFlat->SetStart(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
				if(pointDlg.m_bLockPoint)
					pLineFlat->LockStartPos();
				else 
					pLineFlat->UnlockStartPos();
			}
		}
		//ʼ������
		if(pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart)
		{	//����������Ϊֻ������
			SetPropItemReadOnly(pFirstLineFlat,"startPos.x",TRUE);
			SetPropItemReadOnly(pFirstLineFlat,"startPos.y",TRUE);
			SetPropItemReadOnly(pFirstLineFlat,"startPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ����ֻ������
			SetPropItemReadOnly(pFirstLineFlat,"startPos.x",pFirstLineFlat->desStartPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineFlat,"startPos.y",pFirstLineFlat->desStartPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineFlat,"startPos.z",pFirstLineFlat->desStartPos.endPosType!=4);	//4.ָ������
		}
		SetPropItemReadOnly(pFirstLineFlat,"hStart",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineFlat,"oddStart",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineFlat,"desStartPos",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineFlat,"desStartPos.endPosType",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineFlat,"desStartPos.datum_pos_style",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineFlat,"desStartPos.hDatumPart",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineFlat,"desStartPos.fNormOffset",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineFlat,"desStartPos.fEccentricDist",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineFlat,"desStartPos.face_offset_norm",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineFlat,"desStartPos.hDatumStartPart",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineFlat,"desStartPos.bDatumStartStartEnd",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineFlat,"desStartPos.hDatumEndPart",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineFlat,"desStartPos.bDatumEndStartEnd",pFirstLineFlat->IsStartPosLocked()||pFirstLineFlat->bHuoQuStart);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("actualStartPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "Flat steel's start actual coordinate";
#else 
		pointDlg.m_sCaption = "���ʼ��ʵ������";
#endif
		pointDlg.m_bEnableLock=FALSE;
		pointDlg.m_bReadOnly=TRUE;
		f3dPoint actualStart,vec=pFirstLineFlat->Start()-pFirstLineFlat->End();
		normalize(vec);
		actualStart = pFirstLineFlat->Start()+vec*pFirstLineFlat->startOdd();
		pointDlg.m_fPointX = actualStart.x;
		pointDlg.m_fPointY = actualStart.y;
		pointDlg.m_fPointZ = actualStart.z;
		pointDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("actualEndPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "Flat steel's end actual coordinate";
#else 
		pointDlg.m_sCaption = "����ն�ʵ������";
#endif
		pointDlg.m_bEnableLock=FALSE;
		pointDlg.m_bReadOnly=TRUE;
		f3dPoint actualStart,vec=pFirstLineFlat->Start()-pFirstLineFlat->End();
		normalize(vec);
		actualStart = pFirstLineFlat->Start()+vec*pFirstLineFlat->startOdd();
		pointDlg.m_fPointX = actualStart.x;
		pointDlg.m_fPointY = actualStart.y;
		pointDlg.m_fPointZ = actualStart.z;
		pointDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("oddEnd"))
	{
		odddlg.m_hRefPart1.Format("0X%X",pFirstLineFlat->desEndOdd.m_hRefPart1);
		odddlg.m_hRefPart2.Format("0X%X",pFirstLineFlat->desEndOdd.m_hRefPart2);
		odddlg.m_fCollideDist = pFirstLineFlat->desEndOdd.m_fCollideDist;
		odddlg.m_iOddCalStyle = pFirstLineFlat->desEndOdd.m_iOddCalStyle;
		if(odddlg.DoModal()==IDOK)
		{
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart1);
			sscanf(sText,"%X",&pFirstLineFlat->desEndOdd.m_hRefPart1);
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart2);
			sscanf(sText,"%X",&pFirstLineFlat->desEndOdd.m_hRefPart2);
			pFirstLineFlat->desEndOdd.m_fCollideDist = odddlg.m_fCollideDist;
			pFirstLineFlat->desEndOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
			pFirstLineFlat->CalEndOddment();
			char valueStr[MAX_TEMP_CHAR_50+1]="";
			if(pFirstLineFlat->GetPropValueStr(pItem->m_idProp,valueStr)>0)
				pItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("endPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "Plate Steel End Design Coordinate";
#else 
		pointDlg.m_sCaption = "����ն��������";
#endif
		pointDlg.m_fPointX = pFirstLineFlat->End().x;
		pointDlg.m_fPointY = pFirstLineFlat->End().y;
		pointDlg.m_fPointZ = pFirstLineFlat->End().z;
		if(pFirstLineFlat->desEndPos.endPosType!=4)	//4.ָ������
			pointDlg.m_bReadOnly=TRUE;
		pointDlg.m_bLockPoint=pFirstLineFlat->IsEndPosLocked();
		if(pointDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("endPos.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("endPos.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("endPos.z"),CString(sText));
			for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			{
				pLineFlat->SetEnd(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
				if(pointDlg.m_bLockPoint)
					pLineFlat->LockEndPos();
				else 
					pLineFlat->UnlockEndPos();
			}
		}
		//�ն�����
		if(pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd)
		{	//����������Ϊֻ������
			SetPropItemReadOnly(pFirstLineFlat,"endPos.x",TRUE);
			SetPropItemReadOnly(pFirstLineFlat,"endPos.y",TRUE);
			SetPropItemReadOnly(pFirstLineFlat,"endPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ����ֻ������
			SetPropItemReadOnly(pFirstLineFlat,"endPos.x",pFirstLineFlat->desEndPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineFlat,"endPos.y",pFirstLineFlat->desEndPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineFlat,"endPos.z",pFirstLineFlat->desEndPos.endPosType!=4);	//4.ָ������
		}
		SetPropItemReadOnly(pFirstLineFlat,"hEnd",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineFlat,"oddEnd",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineFlat,"desEndPos",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineFlat,"desEndPos.endPosType",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineFlat,"desEndPos.datum_pos_style",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineFlat,"desEndPos.hDatumPart",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineFlat,"desEndPos.fNormOffset",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineFlat,"desEndPos.fEccentricDist",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineFlat,"desEndPos.face_offset_norm",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineFlat,"desEndPos.hDatumStartPart",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineFlat,"desEndPos.bDatumStartStartEnd",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineFlat,"desEndPos.hDatumEndPart",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineFlat,"desEndPos.bDatumEndStartEnd",pFirstLineFlat->IsEndPosLocked()||pFirstLineFlat->bHuoQuEnd);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("relativeObjs"))
	{
		CRelativeObjectDlg relativeDlg;
		relativeDlg.m_pWorkObj = pFirstLineFlat;
		relativeDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("shadowInfo"))
	{
		CDesignShadowDlg shadowdlg;
		shadowdlg.SetShadowObj((CLDSObject*)pFirstLineFlat);
		shadowdlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumPart")
		||pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.hDatumPart"))
	{
		//ͨ��ѡ�񹹼��޸Ĵ��λ�û�׼�˼�
		CModifyPartHandleDlg modifyhdlg;
		CTubeEndPosPara *pFlatEndPosPara=NULL;
		if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumPart"))
		{
			pFlatEndPosPara=&(pFirstLineFlat->desStartPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineFlat->desStartPos.hDatumPart);
		}
		else
		{
			pFlatEndPosPara=&(pFirstLineFlat->desEndPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineFlat->desEndPos.hDatumPart);
		}
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		if(pFlatEndPosPara&&pFlatEndPosPara->endPosType==3)
		{	//���߽��㶨λ
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Cross Datum Tube";
			modifyhdlg.m_sTitle="Datum Tube:";
			modifyhdlg.SetCmdPickPromptStr("Please select the cross datum tube:");
#else 
			modifyhdlg.m_sCaption="�޸����߽����׼�ֹ�";
			modifyhdlg.m_sTitle="��׼�ֹ�:";
			modifyhdlg.SetCmdPickPromptStr("��ѡ�����߽����׼�ֹ�:");
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Lap Joint Datum Rod";
			modifyhdlg.m_sTitle="Datum Rod:";
			modifyhdlg.SetCmdPickPromptStr("Please select the lap-joint datum rod:");
#else 
			modifyhdlg.m_sCaption="�޸Ĵ��λ�û�׼�˼�";
			modifyhdlg.m_sTitle="��׼�˼�:";
			modifyhdlg.SetCmdPickPromptStr("��ѡ���ӻ�׼�˼�:");
#endif
		}
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind parts��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumPart"))
					pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("desStartPos.hDatumPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("desEndPos.hDatumPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineFlat* pMirLineFlat=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineFlat=(CLDSLineFlat*)pLineFlat->GetMirPart(msg);
						if(pMirLineFlat)
						{
							if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumPart"))
								pMirLineFlat->desStartPos.hDatumPart=pPart->handle;
							else
								pMirLineFlat->desEndPos.hDatumPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumPart"))
						pLineFlat->desStartPos.hDatumPart=pPart->handle;
					else
						pLineFlat->desEndPos.hDatumPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineFlat->handle);
				}
				if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumPart"))
				{
					pParentItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("desStartPos.endPosType"),NULL);
					if(pParentItem)
						UpdateLineFlat_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
				}
				else
				{
					pParentItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("desEndPos.endPosType"),NULL);
					if(pParentItem)
						UpdateLineFlat_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumStartPart")
		||pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.hDatumStartPart"))
	{	//�޸�ͶӰ��ʼ�����ڻ�׼�˼�
		CModifyPartHandleDlg modifyhdlg;
		CTubeEndPosPara *pFlatEndPosPara=NULL;
		if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumStartPart"))
		{
			pFlatEndPosPara=&(pFirstLineFlat->desStartPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineFlat->desStartPos.hDatumStartPart);
		}
		else
		{
			pFlatEndPosPara=&(pFirstLineFlat->desEndPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineFlat->desEndPos.hDatumStartPart);
		}
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		if(pFlatEndPosPara&&pFlatEndPosPara->endPosType==3)
		{	//���߽��㶨λ
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Cross Tube";
			modifyhdlg.m_sTitle="Cross Tube:";
			modifyhdlg.SetCmdPickPromptStr("Please select the cross tube:");
#else 
			modifyhdlg.m_sCaption="�޸Ľ���ֹ�";
			modifyhdlg.m_sTitle="����ֹ�:";
			modifyhdlg.SetCmdPickPromptStr("��ѡ�����߽��㽻��ֹ�:");
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Start Datum Rod ";
			modifyhdlg.m_sTitle="Datum Rod:";
			modifyhdlg.SetCmdPickPromptStr("Please select the projective datum rod:");
#else 
			modifyhdlg.m_sCaption="�޸�ͶӰʼ�����ڻ�׼�˼�";
			modifyhdlg.m_sTitle="��׼�˼�:";
			modifyhdlg.SetCmdPickPromptStr("��ѡ��ͶӰ��׼�˼�:");
#endif
		}
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind parts��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				//������ʾ
				if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumStartPart"))
					pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("desStartPos.hDatumStartPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("desEndPos.hDatumStartPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineFlat* pMirLineFlatPart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineFlatPart=(CLDSLineFlat*)pLineFlat->GetMirPart(msg);
						if(pMirLineFlatPart)
						{
							if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumStartPart"))
								pMirLineFlatPart->desStartPos.hDatumStartPart=pPart->handle;
							else
								pMirLineFlatPart->desEndPos.hDatumStartPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumStartPart"))
						pFirstLineFlat->desStartPos.hDatumStartPart=pPart->handle;
					else
						pFirstLineFlat->desEndPos.hDatumStartPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineFlat->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumEndPart")
		||pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.hDatumEndPart"))
	{	//�޸�ͶӰ���ն����ڻ�׼�˼�
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify End Datum Rod";
		modifyhdlg.m_sTitle="Datum Rod:";
#else 
		modifyhdlg.m_sCaption="�޸�ͶӰ�ն����ڻ�׼�˼�";
		modifyhdlg.m_sTitle="��׼�˼�:";
#endif
		if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumEndPart"))
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineFlat->desStartPos.hDatumEndPart);
		else
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineFlat->desEndPos.hDatumEndPart);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.SetCmdPickPromptStr("Please select the projective datum tube:");
#else 
		modifyhdlg.SetCmdPickPromptStr("��ѡ��ͶӰ��׼�ֹ�:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind parts��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				//������ʾ
				if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumEndPart"))
					pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("desStartPos.hDatumEndPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("desEndPos.hDatumEndPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineFlat* pMirLineFlatPart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineFlatPart=(CLDSLineFlat*)pLineFlat->GetMirPart(msg);
						if(pMirLineFlatPart)
						{
							if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumEndPart"))
								pMirLineFlatPart->desStartPos.hDatumEndPart=pPart->handle;
							else
								pMirLineFlatPart->desEndPos.hDatumEndPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumEndPart"))
						pFirstLineFlat->desStartPos.hDatumEndPart=pPart->handle;
					else
						pFirstLineFlat->desEndPos.hDatumEndPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineFlat->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("hWorkNormRefPart"))
	{
		//ͨ��ѡ�񹹼��޸ĺ��Ӹ����������ڽ������޸ĵ��������ĺ��Ӹ�����
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Normal Reference Rod";
		modifyhdlg.m_sTitle="Reference Rod:";
		modifyhdlg.SetCmdPickPromptStr("Please select reference rod:");
#else 
		modifyhdlg.m_sCaption="�޸ı�ַ��߲��ո˼�";
		modifyhdlg.m_sTitle="���չ���:";
		modifyhdlg.SetCmdPickPromptStr("��ѡ�����:");
#endif
		modifyhdlg.m_sHandle.Format("0X%X",pFirstLineFlat->hWorkNormRefPart);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind parts��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("hWorkNormRefPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineFlat* pMirLineFlat=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineFlat=(CLDSLineFlat*)pLineFlat->GetMirPart(msg);
						if(pMirLineFlat)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//������ѡ���߲��ո˼��ĶԳƹ��� wht 11-05-07
								pMirLineFlat->hWorkNormRefPart=pMirPart->handle;
						}
					}
					pLineFlat->hWorkNormRefPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineFlat->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("m_hPartWeldParent"))
	{
		//ͨ��ѡ�񹹼��޸ĺ��Ӹ����������ڽ������޸ĵ��������ĺ��Ӹ�����
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Weld Parent Part";
		modifyhdlg.m_sTitle="Weld Parent Part:";
		modifyhdlg.SetCmdPickPromptStr("Please select weld parent part:");
#else 
		modifyhdlg.m_sCaption="�޸ı�ֺ��Ӹ�����";
		modifyhdlg.m_sTitle="���Ӹ�����:";
		modifyhdlg.SetCmdPickPromptStr("��ѡ�񺸽Ӹ�����:");
#endif
		modifyhdlg.m_sHandle.Format("0X%X",pFirstLineFlat->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong so can't find correspondind parts��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("m_hPartWeldParent"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLinePart* pMirLinePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLinePart=(CLDSLinePart*)pLineFlat->GetMirPart(msg);
						if(pMirLinePart)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//������ѡ�������ĶԳƹ��� wht 11-05-07
								pMirLinePart->m_hPartWeldParent=pMirPart->handle;
						}
					}
					pLineFlat->m_hPartWeldParent=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineFlat->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.face_offset_norm"))
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Offset Normal";
#else 
		vecDlg.m_sCaption = "ƽ���淨��";
#endif
		vecDlg.m_fVectorX = pFirstLineFlat->desStartPos.face_offset_norm.x;
		vecDlg.m_fVectorY = pFirstLineFlat->desStartPos.face_offset_norm.y;
		vecDlg.m_fVectorZ = pFirstLineFlat->desStartPos.face_offset_norm.z;
		if(vecDlg.DoModal()==IDOK)
		{
			for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
				pLineFlat->desStartPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.face_offset_norm"))
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Offset Normal";
#else 
		vecDlg.m_sCaption = "ƽ���淨��";
#endif
		vecDlg.m_fVectorX = pFirstLineFlat->desEndPos.face_offset_norm.x;
		vecDlg.m_fVectorY = pFirstLineFlat->desEndPos.face_offset_norm.y;
		vecDlg.m_fVectorZ = pFirstLineFlat->desEndPos.face_offset_norm.z;
		if(vecDlg.DoModal()==IDOK)
		{
			for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
				pLineFlat->desEndPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else
		return FALSE;
	for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
	{
		pLineFlat->SetModified();
		pLineFlat->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pLineFlat->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL ModifyLineFlatProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	pPropList = pPropDlg->GetPropertyList();
	CLDSLineFlat *pCurFlat = (CLDSLineFlat*)pView->selectObjs.GetFirst();
	CLDSLineFlat *pLineFlat = NULL;
	CItemInfo *lpInfo;
	CPropTreeItem *pFindItem,*pSonPropItem,*pParentItem,*pLeafPropItem;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;

	const int GROUP_GENERAL=1,GROUP_START=2,GROUP_END=3;
	if(pItem->m_idProp==CLDSLineFlat::GetPropID("layer"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
			pLineFlat->SetLayer(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pLineFlat->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//��ת�Գ�
				CLDSLineFlat *pMirLineFlat=(CLDSLineFlat*)pLineFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEFLAT);
				if(pMirLineFlat)
					CalMirAtomLayer(pLineFlat->layer(),pMirLineFlat->layer(),pMirObj->mirInfo);
			}
		}	
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("cMaterial"))
	{	//��ֲ���
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
			pLineFlat->cMaterial = QuerySteelBriefMatMark(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pLineFlat->relativeObjs.GetNext())
			{
				CLDSLineFlat *pMirLineFlat=(CLDSLineFlat*)pLineFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEFLAT);
				if(pMirLineFlat)
					pMirLineFlat->cMaterial=pLineFlat->cMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("m_cQualityLevel"))
	{	//��������ȼ�
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Default")==0)
				pLineFlat->m_cQualityLevel=0;
			else
#else
			if(valueStr.CompareNoCase("Ĭ��")==0)
				pLineFlat->m_cQualityLevel=0;
			else
#endif
				pLineFlat->m_cQualityLevel = valueStr[0];
			for(RELATIVE_OBJECT *pMirObj=pLineFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pLineFlat->relativeObjs.GetNext())
			{
				CLDSLineFlat *pMirLineFlat=(CLDSLineFlat*)pLineFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEFLAT);
				if(pMirLineFlat)
					pMirLineFlat->m_cQualityLevel=pLineFlat->m_cQualityLevel;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("specification"))
	{	//��ֹ��
		double wide,thick;
		restore_JG_guige(tem_str,wide,thick);
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
			pLineFlat->size_wide = wide;
			pLineFlat->size_thick = thick;
			for(RELATIVE_OBJECT *pMirObj=pLineFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pLineFlat->relativeObjs.GetNext())
			{
				CLDSLineFlat *pMirLineFlat=(CLDSLineFlat*)pLineFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEFLAT);
				if(pMirLineFlat)
				{
					pMirLineFlat->size_wide=wide;
					pMirLineFlat->size_thick=thick;
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("iSeg"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
			pLineFlat->iSeg = SEGI(valueStr.GetBuffer());
			for(RELATIVE_OBJECT *pMirObj=pLineFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pLineFlat->relativeObjs.GetNext())
			{
				CLDSLineFlat *pMirLineFlat=(CLDSLineFlat*)pLineFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEFLAT);
				if(pMirLineFlat)
					pMirLineFlat->iSeg=pLineFlat->iSeg;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("sPartNo"))
	{
		bool parsed=false;
		SEGI iSeg;
		if(CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(tem_str,&iSeg,NULL))
		{
			parsed=true;
			pPropList->SetItemPropValue(CLDSLineFlat::GetPropID("iSeg"),iSeg.ToString());
		}
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{	
			if(parsed)
				pLineFlat->iSeg=iSeg;
			pLineFlat->SetPartNo(tem_str);
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("cfgword"))
	{
		//���������˼�����ĺ�
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
			pLineFlat->cfgword = pCurFlat->cfgword;
			for(RELATIVE_OBJECT *pMirObj=pLineFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pLineFlat->relativeObjs.GetNext())
			{
				CLDSLineFlat *pMirLineFlat=(CLDSLineFlat*)pLineFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEFLAT);
				if(pMirLineFlat)
					pMirLineFlat->cfgword=pLineFlat->cfgword;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("m_uStatMatCoef"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->m_uStatMatCoef = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("m_hPartWeldParent"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->m_hPartWeldParent=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("m_bDisplayHole"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pLineFlat->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pLineFlat->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pLineFlat->m_bDisplayHole = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pLineFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pLineFlat->relativeObjs.GetNext())
			{
				CLDSLineFlat *pMirLineFlat=(CLDSLineFlat*)pLineFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEFLAT);
				if(pMirLineFlat)
					pMirLineFlat->m_bDisplayHole=pLineFlat->m_bDisplayHole;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("m_bVirtualPart"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pLineFlat->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pLineFlat->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pLineFlat->m_bVirtualPart = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pLineFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pLineFlat->relativeObjs.GetNext())
			{
				CLDSLineFlat *pMirLineFlat=(CLDSLineFlat*)pLineFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEFLAT);
				if(pMirLineFlat)
					pMirLineFlat->m_bVirtualPart=pLineFlat->m_bVirtualPart;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("m_bUserSpecColor"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pLineFlat->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pLineFlat->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pLineFlat->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pLineFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pLineFlat->relativeObjs.GetNext())
			{
				CLDSLineFlat *pMirLineFlat=(CLDSLineFlat*)pLineFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEFLAT);
				if(pMirLineFlat)
					pMirLineFlat->m_bUserSpecColor=pLineFlat->m_bUserSpecColor;
			}
		}
		if(pCurFlat->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("crMaterial"),NULL);
			if(pFindItem==NULL)
			{
				char sText[MAX_TEMP_CHAR_100+1]="";
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Part Color";
				lpInfo->m_strPropHelp = "Part Color";
#else 
				lpInfo->m_strPropName = "������ɫ";
				lpInfo->m_strPropHelp = "������ɫ";
#endif
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = CLDSLineFlat::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if(pCurFlat->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("crMaterial"));
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
			pLineFlat->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pLineFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pLineFlat->relativeObjs.GetNext())
			{
				CLDSLineFlat *pMirLineFlat=(CLDSLineFlat*)pLineFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEFLAT);
				if(pMirLineFlat)
					pMirLineFlat->crMaterial=pLineFlat->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("sNotes"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
			_snprintf(pLineFlat->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
			//��ע���ù����޸� wht 10-10-22
			/*for(RELATIVE_OBJECT *pMirObj=pLineFlat->relativeObjs.GetFirst();pMirObj;pMirObj=pLineFlat->relativeObjs.GetNext())
			{
			CLDSLineFlat *pMirLineFlat=(CLDSLineFlat*)pLineFlat->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEFLAT);
			if(pMirLineFlat)
			strcpy(pMirLineFlat->sNotes,pLineFlat->sNotes);
			}*/
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("m_iNormCalStyle"))
	{
		char sText[MAX_TEMP_CHAR_100+1]="";
		pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("0.Specify Directly")==0)
#else 
		if(valueStr.CompareNoCase("0.ֱ��ָ��")==0)
#endif
			pLineFlat->m_iNormCalStyle = 0;
		else
			pLineFlat->m_iNormCalStyle = 1;
		pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("nearWorkPlaneNorm.x"),&pParentItem);
		pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("m_iNormCalStyle"),&pParentItem);//���»�ø��ڵ�
		if(pFindItem==NULL)
		{
			if(pLineFlat->m_iNormCalStyle == 1)
			{
				//��׼����Ʒ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Datum Plane Near Normal";
				lpInfo->m_strPropHelp = "Its generally specified by the user, the system can calculate it according to the near normals and channel steel axis��";
#else 
				lpInfo->m_strPropName = "��׼����Ʒ���";
				lpInfo->m_strPropHelp = "��ֻ�׼����Ʒ���һ�����û�ָ����ϵͳ�ɸ��ݽ��Ʒ��߼�������߾�ȷ�������ֵĻ�׼�淨�ߡ�";
#endif
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 1,TRUE);
				pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("nearWorkPlaneNorm");
				pSonPropItem->m_bHideChildren = TRUE;
				if(CLDSLineFlat::GetPropStatus(pSonPropItem->m_idProp)!=-1)
					pSonPropItem->m_bHideChildren = CLDSLineFlat::GetPropStatus(pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "X Coordinate";
				lpInfo->m_strPropHelp = "The x coodinate of datum plane's normal";
#else 
				lpInfo->m_strPropName = "X�������";
				lpInfo->m_strPropHelp = "��ֻ�׼�淨��X�᷽�������";
#endif
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, 1,TRUE);
				pLeafPropItem->m_idProp = CLDSLineFlat::GetPropID("nearWorkPlaneNorm.x");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pLineFlat->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Y Coordinate";
				lpInfo->m_strPropHelp = "The y coodinate of datum plane's normal";
#else 
				lpInfo->m_strPropName = "Y�������";
				lpInfo->m_strPropHelp = "��ֻ�׼�淨��Y�᷽�������";
#endif
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,2,TRUE);
				pLeafPropItem->m_idProp = CLDSLineFlat::GetPropID("nearWorkPlaneNorm.y");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pLineFlat->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Z Coordinate";
				lpInfo->m_strPropHelp = "The z coodinate of datum plane's normal";
#else 
				lpInfo->m_strPropName = "Z�������";
				lpInfo->m_strPropHelp = "��ֻ�׼�淨��Z�᷽�������";
#endif
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, 3,TRUE);
				pLeafPropItem->m_idProp = CLDSLineFlat::GetPropID("nearWorkPlaneNorm.z");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pLineFlat->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//hWorkNormRefPart
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Normal Reference Rod";
				lpInfo->m_strPropHelp = "It is work reference rod when calculating datum plane's normal��It is start node or end node of parent rod when it is 0X0(It can't be rod itself)";
#else 
				lpInfo->m_strPropName = "���߲��ո˼�";
				lpInfo->m_strPropHelp = "�����ֻ�׼�淨��ʱ�Ĺ�������ո˼���0X0ʱĬ��Ϊʼ�˻��ն˽ڵ㸸�˼�(����Ϊ�˼�����)";
#endif
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 2,TRUE);
				pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("hWorkNormRefPart");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pLineFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
			pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("workPlaneNorm.x"),&pParentItem);
			pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("workPlaneNorm.x"),&pParentItem);//���»�ȡһ�����ĸ��ڵ�
			if(pFindItem == NULL)
			{
				//���ߵ�X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "X Coordinate";
				lpInfo->m_strPropHelp = "The x coodinate of datum plane's normal";
#else 
				lpInfo->m_strPropName = "X�������";
				lpInfo->m_strPropHelp = "��ֻ�׼�淨��X�᷽�������";
#endif
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 3,TRUE);
				pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("workPlaneNorm.x");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pLineFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Y Coordinate";
				lpInfo->m_strPropHelp = "The y coodinate of datum plane's normal";
#else 
				lpInfo->m_strPropName = "Y�������";
				lpInfo->m_strPropHelp = "��ֻ�׼�淨��Y�᷽�������";
#endif
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 4,TRUE);
				pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("workPlaneNorm.y");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pLineFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Z Coordinate";
				lpInfo->m_strPropHelp = "The z coodinate of datum plane's normal";
#else 
				lpInfo->m_strPropName = "Z�������";
				lpInfo->m_strPropHelp = "��ֻ�׼�淨��Z�᷽�������";
#endif
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 5,TRUE);
				pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("workPlaneNorm.z");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pLineFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else
		{
			if(pLineFlat->m_iNormCalStyle == 0)
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("nearWorkPlaneNorm"),&pParentItem);
				if(pFindItem!=NULL)
				{
					pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("nearWorkPlaneNorm"));
					pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("nearWorkPlaneNorm.x"));
					pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("nearWorkPlaneNorm.y"));
					pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("nearWorkPlaneNorm.z"));
					pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("hWorkNormRefPart"));
				}
			}
		}
		pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("workPlaneNorm.x"),&pParentItem);
		pFindItem->SetReadOnly(pLineFlat->m_iNormCalStyle==1||pLineFlat->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("workPlaneNorm.y"),&pParentItem);
		pFindItem->SetReadOnly(pLineFlat->m_iNormCalStyle==1||pLineFlat->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("workPlaneNorm.z"),&pParentItem);
		pFindItem->SetReadOnly(pLineFlat->m_iNormCalStyle==1||pLineFlat->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));

	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("nearWorkPlaneNorm.x"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->nearWorkPlaneNorm.x = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("nearWorkPlaneNorm.y"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->nearWorkPlaneNorm.y = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("nearWorkPlaneNorm.z"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->nearWorkPlaneNorm.z = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("hWorkNormRefPart"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineFlat->hWorkNormRefPart);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("oddStart"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{	//����ֶ������˸˼�����ͷӦ���ȣ�������ͷ�ļ��㷽ʽ�޸�Ϊ������
			//�����������ø˼���صĸְ�ʱ�������¼�������ͷ��ֵ	wht 09-09-02
			pLineFlat->desStartOdd.m_iOddCalStyle=2;
			pLineFlat->SetStartOdd(atof(valueStr));//,TRUE,TRUE);�������벻���ƣ��ݲ�������˨�������塡wjh-2018.1.8
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("startPos.x"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->pStart->SetPositionX(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("startPos.y"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->pStart->SetPositionY(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("startPos.z"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->pStart->SetPositionZ(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.endPosType"))
	{
		int end_pos_type=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Node Is Datum Node")==0)
			end_pos_type = 0;
		else if(valueStr.CompareNoCase("Lap Joint Connect")==0)
			end_pos_type = 1;
		else if(valueStr.CompareNoCase("Point Line Project")==0)
			end_pos_type = 2;
		else if(valueStr.CompareNoCase("Center Line Cross Point")==0)
			end_pos_type = 3;
		else if(valueStr.CompareNoCase("Specified Coordinate")==0)
#else 
		if(valueStr.CompareNoCase("�ڵ㼴����")==0)
			end_pos_type = 0;
		else if(valueStr.CompareNoCase("�������")==0)
			end_pos_type = 1;
		else if(valueStr.CompareNoCase("����ͶӰ")==0)
			end_pos_type = 2;
		else if(valueStr.CompareNoCase("���߽���")==0)
			end_pos_type = 3;
		else if(valueStr.CompareNoCase("ָ������")==0)
#endif
			end_pos_type = 4;
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->desStartPos.endPosType = end_pos_type;
		UpdateLineFlat_DesPos_EndPosType_TreeItem(pPropList,pItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.datum_pos_style"))
	{
		int datum_pos_style=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Ridge Plane Project")==0)
			datum_pos_style = 0;
		else if(valueStr.CompareNoCase("X Leg Project")==0)
			datum_pos_style = 1;
		else if(valueStr.CompareNoCase("Y Leg Project")==0)
			datum_pos_style = 2;
		else if(valueStr.CompareNoCase("Ridge Plane Offset")==0)
			datum_pos_style = 3;
		else if(valueStr.CompareNoCase("X Leg Plane Offset")==0)
			datum_pos_style = 4;
		else if(valueStr.CompareNoCase("Y Leg Plane Offset")==0)
#else 
		if(valueStr.CompareNoCase("������ͶӰ")==0)
			datum_pos_style = 0;
		else if(valueStr.CompareNoCase("X֫����ͶӰ")==0)
			datum_pos_style = 1;
		else if(valueStr.CompareNoCase("Y֫����ͶӰ")==0)
			datum_pos_style = 2;
		else if(valueStr.CompareNoCase("������ƽ��")==0)
			datum_pos_style = 3;
		else if(valueStr.CompareNoCase("X֫����ƽ��")==0)
			datum_pos_style = 4;
		else if(valueStr.CompareNoCase("Y֫����ƽ��")==0)
#endif
			datum_pos_style = 5;
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->desStartPos.datum_pos_style = datum_pos_style;
		pParentItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("desStartPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineFlat_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
		if(datum_pos_style>2)
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Offset Normal";
#else 
			vecDlg.m_sCaption = "ƽ���淨��";
#endif
			vecDlg.m_fVectorX = pCurFlat->desStartPos.face_offset_norm.x;
			vecDlg.m_fVectorY = pCurFlat->desStartPos.face_offset_norm.y;
			vecDlg.m_fVectorZ = pCurFlat->desStartPos.face_offset_norm.z;
			if(vecDlg.DoModal()==IDOK)
			{
				for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
					pLineFlat->desStartPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumPart"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineFlat->desStartPos.hDatumPart);
		pParentItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("desStartPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineFlat_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.fNormOffset"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->desStartPos.fNormOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.fEccentricDist"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->desStartPos.fEccentricDist = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.fTransverseOffset"))
	{	//����ƫ����
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->desStartPos.fTransverseOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumStartPart"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineFlat->desStartPos.hDatumStartPart);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.bDatumStartStartEnd"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start")==0)
				pLineFlat->desStartPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineFlat->desStartPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
#endif
				pLineFlat->desStartPos.bDatumStartStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.hDatumEndPart"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineFlat->desStartPos.hDatumEndPart);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desStartPos.bDatumEndStartEnd"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start")==0)
				pLineFlat->desStartPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineFlat->desStartPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
#endif
				pLineFlat->desStartPos.bDatumEndStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("oddEnd"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{	//����ֶ������˸˼�����ͷӦ���ȣ�������ͷ�ļ��㷽ʽ�޸�Ϊ������
			//�����������ø˼���صĸְ�ʱ�������¼�������ͷ��ֵ	wht 09-09-02
			pLineFlat->desEndOdd.m_iOddCalStyle=2;
			pLineFlat->SetEndOdd(atof(valueStr));//,TRUE,TRUE);�������벻���ƣ��ݲ�������˨�������塡wjh-2018.1.8
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("endPos.x"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->pEnd->SetPositionX(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("endPos.y"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->pEnd->SetPositionY(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("endPos.z"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->pEnd->SetPositionZ(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.endPosType"))
	{
		int end_pos_type=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Node Is Datum Node")==0)
			end_pos_type = 0;
		else if(valueStr.CompareNoCase("Lap Joint Connect")==0)
			end_pos_type = 1;
		else if(valueStr.CompareNoCase("Point Line Project")==0)
			end_pos_type = 2;
		else if(valueStr.CompareNoCase("Center Line Cross Point")==0)
			end_pos_type = 3;
		else if(valueStr.CompareNoCase("Specified Coordinate")==0)
#else 
		if(valueStr.CompareNoCase("�ڵ㼴����")==0)
			end_pos_type = 0;
		else if(valueStr.CompareNoCase("�������")==0)
			end_pos_type = 1;
		else if(valueStr.CompareNoCase("����ͶӰ")==0)
			end_pos_type = 2;
		else if(valueStr.CompareNoCase("���߽���")==0)
			end_pos_type = 3;
		else if(valueStr.CompareNoCase("ָ������")==0)
#endif
			end_pos_type = 4;
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->desEndPos.endPosType = end_pos_type;
		UpdateLineFlat_DesPos_EndPosType_TreeItem(pPropList,pItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.datum_pos_style"))
	{
		int datum_pos_style=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Ridge Plane Project")==0)
			datum_pos_style = 0;
		else if(valueStr.CompareNoCase("X Leg Project")==0)
			datum_pos_style = 1;
		else if(valueStr.CompareNoCase("Y Leg Project")==0)
			datum_pos_style = 2;
		else if(valueStr.CompareNoCase("Ridge Plane Offset")==0)
			datum_pos_style = 3;
		else if(valueStr.CompareNoCase("X Leg Plane Offset")==0)
			datum_pos_style = 4;
		else if(valueStr.CompareNoCase("Y Leg Plane Offset")==0)
#else 
		if(valueStr.CompareNoCase("������ͶӰ")==0)
			datum_pos_style = 0;
		else if(valueStr.CompareNoCase("X֫����ͶӰ")==0)
			datum_pos_style = 1;
		else if(valueStr.CompareNoCase("Y֫����ͶӰ")==0)
			datum_pos_style = 2;
		else if(valueStr.CompareNoCase("������ƽ��")==0)
			datum_pos_style = 3;
		else if(valueStr.CompareNoCase("X֫����ƽ��")==0)
			datum_pos_style = 4;
		else if(valueStr.CompareNoCase("Y֫����ƽ��")==0)
#endif
			datum_pos_style = 5;
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->desEndPos.datum_pos_style = datum_pos_style;
		pParentItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("desEndPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineFlat_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
		if(datum_pos_style>2)
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Offset Normal";
#else 
			vecDlg.m_sCaption = "ƽ���淨��";
#endif
			vecDlg.m_fVectorX = pCurFlat->desEndPos.face_offset_norm.x;
			vecDlg.m_fVectorY = pCurFlat->desEndPos.face_offset_norm.y;
			vecDlg.m_fVectorZ = pCurFlat->desEndPos.face_offset_norm.z;
			if(vecDlg.DoModal()==IDOK)
			{
				for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
					pLineFlat->desEndPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.hDatumPart"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineFlat->desEndPos.hDatumPart);
		pParentItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("desEndPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineFlat_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.fNormOffset"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->desEndPos.fNormOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.fEccentricDist"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->desEndPos.fEccentricDist = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.fTransverseOffset"))
	{	//����ƫ����
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			pLineFlat->desEndPos.fTransverseOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.hDatumStartPart"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineFlat->desEndPos.hDatumStartPart);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.bDatumStartStartEnd"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start")==0)
				pLineFlat->desEndPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineFlat->desEndPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
#endif
				pLineFlat->desEndPos.bDatumStartStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.hDatumEndPart"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineFlat->desEndPos.hDatumEndPart);
	}
	else if(pItem->m_idProp==CLDSLineFlat::GetPropID("desEndPos.bDatumEndStartEnd"))
	{
		for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start")==0)
				pLineFlat->desEndPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineFlat->desEndPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
#endif
				pLineFlat->desEndPos.bDatumEndStartEnd = FALSE;
		}
	}
	else
		return FALSE;
	for(pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetFirst();pLineFlat;pLineFlat=(CLDSLineFlat*)pView->selectObjs.GetNext())
	{
		pLineFlat->ClearFlag();
		pLineFlat->CalPosition();
		pLineFlat->SetModified();
		pLineFlat->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pLineFlat->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

void UpdateLineFlat_DesPos_EndPosType_TreeItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineFlat *pFirstFlat = (CLDSLineFlat*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem;
	int idProp;
	CItemInfo *lpInfo;
	if(pFirstFlat==NULL||pParentItem==NULL)
		return;
	CTubeEndPosPara *pDesPos;
	char valueStr[MAX_TEMP_CHAR_50+1]="";
	const int GROUP_GENERAL=1,GROUP_START=2,GROUP_END=3;
	DWORD dwPropGroup=0;
	if(bStart)
	{
		dwPropGroup=GetSingleWord(GROUP_START);
		pDesPos=&pFirstFlat->desStartPos;
	}
	else
	{
		dwPropGroup=GetSingleWord(GROUP_END);
		pDesPos=&pFirstFlat->desEndPos;
	}
	char sText[MAX_TEMP_CHAR_100+1]="";
	//0:�˵㼴�ڵ�;1:����ڽڵ㸸�˼���;2:�ڵ���ָ����׼��ͶӰ;3:�˼����߽���;4:ָ������; wht 10-10-20
	if(pDesPos->endPosType==0)	
	{
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.hDatumPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.fTransverseOffset"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.hDatumStartPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.datum_pos_style"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.hDatumPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.fTransverseOffset"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.hDatumStartPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.datum_pos_style"));
		}
		//��׼�淨��ƫ����
		if(bStart)
			idProp=CLDSLineFlat::GetPropID("desStartPos.fNormOffset");
		else
			idProp=CLDSLineFlat::GetPropID("desEndPos.fNormOffset");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Normal Offset";
			lpInfo->m_strPropHelp = "The datum plane's normal offset";
#else 
			lpInfo->m_strPropName = "��׼�淨��ƫ����";
			lpInfo->m_strPropHelp = "��׼�淨��ƫ����";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	}
	else if(pDesPos->endPosType==1)
	{
		CLDSLinePart *pDatumLinePart=NULL;
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.hDatumStartPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.face_offset_norm"));
			//
			pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstFlat->desStartPos.hDatumPart,CLS_LINEPART);
			if(pDatumLinePart==NULL&&pFirstFlat->pStart)
				pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstFlat->pStart->hFatherPart,CLS_LINEPART);
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.hDatumStartPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.face_offset_norm"));
			//
			pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstFlat->desEndPos.hDatumPart,CLS_LINEPART);
			if(pDatumLinePart==NULL&&pFirstFlat->pEnd)
				pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstFlat->pEnd->hFatherPart,CLS_LINEPART);
		}
		BOOL bIsDatumJg=FALSE;
		if(pDatumLinePart&&pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE)
			bIsDatumJg=TRUE;
		//���λ�û�׼�˼�
		if(bStart)
			idProp=CLDSLineFlat::GetPropID("desStartPos.hDatumPart");
		else
			idProp=CLDSLineFlat::GetPropID("desEndPos.hDatumPart");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			if(bIsDatumJg)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Lap Joint Datum Angle";
				lpInfo->m_strPropHelp = "The flate angle's lap joint position of datum angle handle";
#else 
				lpInfo->m_strPropName = "���λ�û�׼�Ǹ�";
				lpInfo->m_strPropHelp = "��ִ��λ�û�׼�Ǹ־��";
#endif
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Lap Joint Datum Tube";
				lpInfo->m_strPropHelp = "The flate angle's lap joint position of datum tube handle";
#else 
				lpInfo->m_strPropName = "���λ�û�׼�ֹ�";
				lpInfo->m_strPropHelp = "��ִ��λ�û�׼�ֹܾ��";
#endif
			}
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
			if(bIsDatumJg)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pPropItem->m_lpNodeInfo->m_strPropName = "Lap Joint Datum Angle";
				pPropItem->m_lpNodeInfo->m_strPropHelp = "The flate angle's lap joint position of datum angle handle";
#else 
				pPropItem->m_lpNodeInfo->m_strPropName = "���λ�û�׼�Ǹ�";
				pPropItem->m_lpNodeInfo->m_strPropHelp = "��ִ��λ�û�׼�Ǹ־��";
#endif
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pPropItem->m_lpNodeInfo->m_strPropName = "Lap Joint Datum Tube";
				pPropItem->m_lpNodeInfo->m_strPropHelp = "The flate angle's lap joint position of datum tube handle";
#else 
				pPropItem->m_lpNodeInfo->m_strPropName = "���λ�û�׼�ֹ�";
				pPropItem->m_lpNodeInfo->m_strPropHelp = "��ִ��λ�û�׼�ֹܾ��";
#endif
			}
		}
		//
		if(bStart)
			idProp=CLDSLineFlat::GetPropID("desStartPos.datum_pos_style");
		else
			idProp=CLDSLineFlat::GetPropID("desEndPos.datum_pos_style");
		if(bIsDatumJg)
		{	//���㶨λ��ʽ
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType=CDT_LIST;	
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Datum Point Locate Mode";
				if(bStart)
					lpInfo->m_strPropHelp = "The locate mode of flat angle's start";
				else 
					lpInfo->m_strPropHelp = "The locate mode of flat angle's end";
				lpInfo->m_cmbItems = "Ridge Plane Project|X Leg Project|Y Leg Project|Ridge Plane Offset|X Leg Plane Offset|Y Leg Plane Offset|";
#else 
				lpInfo->m_strPropName = "���㶨λ��ʽ";
				if(bStart)
					lpInfo->m_strPropHelp = "���ʼ�˻��㶨λ��ʽ";
				else 
					lpInfo->m_strPropHelp = "����ն˻��㶨λ��ʽ";
				lpInfo->m_cmbItems = "����ͶӰ|X֫����ͶӰ|Y֫����ͶӰ|������ƽ��|X֫����ƽ��|Y֫����ƽ��|";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,1,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
				if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,valueStr))
					pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			else
			{
				if(pFirstFlat->GetPropValueStr(idProp,valueStr)>0)
					pPropList->SetItemPropValue(idProp,CString(valueStr));
			}
			if(bStart)
				pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
											pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			else
				pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
											pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
		else
			pPropList->DeleteItemByPropId(idProp);
		//
		if(bStart)
			pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//��׼�淨��ƫ����
		if(bStart)
			idProp=CLDSLineFlat::GetPropID("desStartPos.fNormOffset");
		else
			idProp=CLDSLineFlat::GetPropID("desEndPos.fNormOffset");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Normal Offset";
			lpInfo->m_strPropHelp = "The datum plane's normal offset";
#else 
			lpInfo->m_strPropName = "��׼�淨��ƫ����";
			lpInfo->m_strPropHelp = "��׼�淨��ƫ����";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,2,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//����ƫ����
		if(bStart)
			idProp=CLDSLineFlat::GetPropID("desStartPos.fTransverseOffset");
		else
			idProp=CLDSLineFlat::GetPropID("desEndPos.fTransverseOffset");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Lateral Offset";
			lpInfo->m_strPropHelp = "The offset of x negative direction";
#else 
			lpInfo->m_strPropName = "����ƫ����";
			lpInfo->m_strPropHelp = "X�Ḻ����ƫ����";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,3,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		if(!bIsDatumJg)
		{	//ƫ�ľ�
			if(bStart)
				idProp=CLDSLineFlat::GetPropID("desStartPos.fEccentricDist");
			else
				idProp=CLDSLineFlat::GetPropID("desEndPos.fEccentricDist");
			pPropItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Offset Distance";
				lpInfo->m_strPropHelp = "Datum offset distance";
#else 
				lpInfo->m_strPropName = "ƫ�ľ�";
				lpInfo->m_strPropHelp = "��׼ƫ�ľ�";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,4,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
				if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			if(bStart)
				pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
										pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			else
				pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
										pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
		if((bIsDatumJg&&pDesPos->datum_pos_style>2)||!bIsDatumJg)
		{	//ƽ���淨��
			if(bStart)
				idProp=CLDSLineFlat::GetPropID("desStartPos.face_offset_norm");
			else
				idProp=CLDSLineFlat::GetPropID("desEndPos.face_offset_norm");
			pPropItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Offset Normal";
				lpInfo->m_strPropHelp = "Offset Normal";
#else 
				lpInfo->m_strPropName = "ƽ���淨��";
				lpInfo->m_strPropHelp = "ƽ���淨��";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,5,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
				if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			if(bStart)
				pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
										pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			else
				pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
										pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
	}
	else if(pDesPos->endPosType==2)
	{
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.hDatumPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.fTransverseOffset"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.fNormOffset"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.datum_pos_style"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.hDatumPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.fTransverseOffset"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.fNormOffset"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.datum_pos_style"));
		}
		//ͶӰ��ʼ�����ڻ�׼���
		if(bStart)
			idProp=CLDSLineFlat::GetPropID("desStartPos.hDatumStartPart");
		else
			idProp=CLDSLineFlat::GetPropID("desEndPos.hDatumStartPart");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Start Datum Rod";
			lpInfo->m_strPropHelp = "The datum rod  is at project line's start ";
#else 
			lpInfo->m_strPropName = "ͶӰ��ʼ�����ڻ�׼�˼�";
			lpInfo->m_strPropHelp = "ͶӰ��ʼ�����ڻ�׼�˼�";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropName = "Start Datum Rod";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "The datum rod  is at project line's start ";
#else 
			pPropItem->m_lpNodeInfo->m_strPropName = "ͶӰ��ʼ�����ڻ�׼�˼�";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "ͶӰ��ʼ�����ڻ�׼�˼�";
#endif
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//��׼�˼��Ļ�׼�˵�
		if(bStart)
			idProp=CLDSLineFlat::GetPropID("desStartPos.bDatumStartStartEnd");
		else
			idProp=CLDSLineFlat::GetPropID("desEndPos.bDatumStartStartEnd");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Endpoint";
			lpInfo->m_strPropHelp = "The datum rod's datum endpoint";
			lpInfo->m_cmbItems = "Start|End|";
#else 
			lpInfo->m_strPropName = "��׼�˼��Ļ�׼�˵�";
			lpInfo->m_strPropHelp = "��׼�˼��Ļ�׼�˵�";
			lpInfo->m_cmbItems = "ʼ��|�ն�|";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,1,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//ͶӰ���ն����ڻ�׼�˼�
		if(bStart)
			idProp=CLDSLineFlat::GetPropID("desStartPos.hDatumEndPart");
		else
			idProp=CLDSLineFlat::GetPropID("desEndPos.hDatumEndPart");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "End Datum Rod";
			lpInfo->m_strPropHelp = "The datum rod at project line's end";
#else 
			lpInfo->m_strPropName = "ͶӰ���ն����ڻ�׼�˼�";
			lpInfo->m_strPropHelp = "ͶӰ���ն����ڻ�׼�˼�";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,2,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//��׼�˼��Ļ�׼�˵�
		if(bStart)
			idProp=CLDSLineFlat::GetPropID("desStartPos.bDatumEndStartEnd");
		else
			idProp=CLDSLineFlat::GetPropID("desEndPos.bDatumEndStartEnd");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Endpoint";
			lpInfo->m_strPropHelp = "The datum rod's datum endpoint";
			lpInfo->m_cmbItems = "Start|End|";
#else 
			lpInfo->m_strPropName = "��׼�˼��Ļ�׼�˵�";
			lpInfo->m_strPropHelp = "��׼�˼��Ļ�׼�˵�";
			lpInfo->m_cmbItems = "ʼ��|�ն�|";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,3,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
									pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	}
	else if(pDesPos->endPosType==3)
	{	//�˼����߽���
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.datum_pos_style"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.fTransverseOffset"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.datum_pos_style"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.fTransverseOffset"));
		}
		//��׼�˼�
		if(bStart)
			idProp=CLDSLineFlat::GetPropID("desStartPos.hDatumPart");
		else
			idProp=CLDSLineFlat::GetPropID("desEndPos.hDatumPart");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Rod";
			lpInfo->m_strPropHelp = "The Datum Rod of the crossing center line";
#else 
			lpInfo->m_strPropName = "��׼�˼�";
			lpInfo->m_strPropHelp = "���߽����׼�˼�";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropName = "Datum Rod";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "The Datum Rod of the crossing center line";
#else 
			pPropItem->m_lpNodeInfo->m_strPropName = "��׼�˼�";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "���߽����׼�˼�";
#endif
		}
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
										pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
										pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//����˼�
		if(bStart)
			idProp=CLDSLineFlat::GetPropID("desStartPos.hDatumStartPart");
		else
			idProp=CLDSLineFlat::GetPropID("desEndPos.hDatumStartPart");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Cross Rod";
			lpInfo->m_strPropHelp = "The crossing rod at cross point of center line ";
#else 
			lpInfo->m_strPropName = "����˼�";
			lpInfo->m_strPropHelp = "���߽��㽻��˼�";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,1,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropName = "Cross Rod";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "The crossing rod at cross point of center line ";
#else 
			pPropItem->m_lpNodeInfo->m_strPropName = "����˼�";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "���߽��㽻��˼�";
#endif
		}
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
										pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
										pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//��׼�淨��ƫ����
		if(bStart)
			idProp=CLDSLineFlat::GetPropID("desStartPos.fNormOffset");
		else
			idProp=CLDSLineFlat::GetPropID("desEndPos.fNormOffset");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Normal Offset";
			lpInfo->m_strPropHelp = "The datum plane's normal offset";
#else 
			lpInfo->m_strPropName = "��׼�淨��ƫ����";
			lpInfo->m_strPropHelp = "��׼�淨��ƫ����";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,2,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
										pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
										pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	}
	else //if(pDesPos->endPosType==4)
	{	//ֱ��ָ������
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.hDatumPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.hDatumStartPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.datum_pos_style"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.fNormOffset"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desStartPos.fTransverseOffset"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.hDatumPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.hDatumStartPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.bDatumStartStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.hDatumEndPart"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.bDatumEndStartEnd"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.datum_pos_style"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.fNormOffset"));
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("desEndPos.fTransverseOffset"));
		}
	}
	if(bStart)
	{	
		if(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart)
		{	//�������Ϊֻ������
			SetPropItemReadOnly(pFirstFlat,"startPos.x",TRUE);
			SetPropItemReadOnly(pFirstFlat,"startPos.y",TRUE);
			SetPropItemReadOnly(pFirstFlat,"startPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ���ж�
			SetPropItemReadOnly(pFirstFlat,"startPos.x",pFirstFlat->desStartPos.endPosType!=4||
								pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("startPos.x")));	//4.ָ������
			SetPropItemReadOnly(pFirstFlat,"startPos.y",pFirstFlat->desStartPos.endPosType!=4||
								pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("startPos.y")));	//4.ָ������
			SetPropItemReadOnly(pFirstFlat,"startPos.z",pFirstFlat->desStartPos.endPosType!=4||
								pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("startPos.z")));	//4.ָ������
		}
	}
	else
	{
		if(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd)
		{	//�������Ϊֻ������
			SetPropItemReadOnly(pFirstFlat,"endPos.x",TRUE);
			SetPropItemReadOnly(pFirstFlat,"endPos.y",TRUE);
			SetPropItemReadOnly(pFirstFlat,"endPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ���ж�
			SetPropItemReadOnly(pFirstFlat,"endPos.x",pFirstFlat->desEndPos.endPosType!=4||
								pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("endPos.x")));	//4.ָ������
			SetPropItemReadOnly(pFirstFlat,"endPos.y",pFirstFlat->desEndPos.endPosType!=4||
								pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("endPos.y")));	//4.ָ������
			SetPropItemReadOnly(pFirstFlat,"endPos.z",pFirstFlat->desEndPos.endPosType!=4||
								pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("endPos.z")));	//4.ָ������
		}
	}
}

void CLDSView::DisplayLineSlotProperty(BOOL bRebuild)
{
	CLDSLineSlot *pFirstSlot = (CLDSLineSlot*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pSonPropItem,*pFindItem,*pParentItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_START=2,GROUP_END=3;
	if(bRebuild)
	{	//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		if (pFirstSlot->pStart!=NULL&&pFirstSlot->pEnd!=NULL)
		{
			pPropDlg->m_arrPropGroupLabel.SetSize(3);
#ifdef AFX_TARG_ENU_ENGLISH
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"Start");
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"End");
#else 
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"ʼ��");
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"�ն�");
#endif
			pPropDlg->RefreshTabCtrl(CLDSLineSlot::m_iCurDisplayPropGroup);
		}
		else 
		{
			pPropDlg->m_arrPropGroupLabel.SetSize(0);
			pPropDlg->RefreshTabCtrl(0);
		}
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_LINESLOT;
		pPropList->SetModifyValueFunc(ModifyLineSlotProperty);
		pPropList->SetButtonClickFunc(LineSlotButtonClick);
		pPropList->SetModifyStatusFunc(ModifyLineSlotItemStatus);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstSlot->BelongModel()->IsTowerModel()||pFirstSlot->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//�۸ֻ�����Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "U-steel Basic Information";
		lpInfo->m_strPropHelp = "U-steel's basic information";
#else 
		lpInfo->m_strPropName = "�۸ֻ�����Ϣ";
		lpInfo->m_strPropHelp = "�۸ֻ�����Ϣ";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineSlot::GetPropID("basicInfo");
		if(CLDSLineSlot::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineSlot::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "U-steel handle";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�۸־��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("handle");
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
		//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "The layer of U-steel"; 
#else 
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "�۸�����ͼ��"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("layer");
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ĺ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Configure Word";
		lpInfo->m_strPropHelp = "Configure Word";
#else 
		lpInfo->m_strPropName = "��ĺ�";
		lpInfo->m_strPropHelp = "��ĺ�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("cfgword");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�ֶκ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Segment Number";
		lpInfo->m_strPropHelp = "Segment Number";
#else 
		lpInfo->m_strPropName = "�ֶκ�";
		lpInfo->m_strPropHelp = "�ֶκ�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("iSeg");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part Number";
		lpInfo->m_strPropHelp = "Part Number"; 
#else 
		lpInfo->m_strPropName = "�������";
		lpInfo->m_strPropHelp = "�������"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("sPartNo");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//����	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Material";
		lpInfo->m_strPropHelp = "U-steel material";
#else 
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "�۸ֲ���";
#endif
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390|Q420|Q460";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("cMaterial");
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�����ȼ�	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Quality Grade";
		lpInfo->m_strPropHelp = "U-steel material";
		lpInfo->m_cmbItems = "Default|A|B|C|D|";
#else 
		lpInfo->m_strPropName = "�����ȼ�";
		lpInfo->m_strPropHelp = "�۸������ȼ�";
		lpInfo->m_cmbItems = "Ĭ��|A|B|C|D|";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("m_cQualityLevel");
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specification";
		lpInfo->m_strPropHelp = "U-steel's specification";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�۸ֵĹ��";
#endif
		lpInfo->m_cmbItems = MakeSlotSetString();
		lpInfo->m_cmbType = CDT_LIST;	//CDT_EDIT;
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("specification");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Width";
		lpInfo->m_strPropHelp = "U-steel's width";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�۸ֵĿ��";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("width");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pSonPropItem->SetReadOnly();
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Thick";
		lpInfo->m_strPropHelp = "U-steel's thick";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�۸ֵĺ��";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("thick");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pSonPropItem->SetReadOnly();
		//�߶�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Height";
		lpInfo->m_strPropHelp = "U-steel's height";
#else 
		lpInfo->m_strPropName = "�߶�";
		lpInfo->m_strPropHelp = "�۸ֵĲ۸�";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("height");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pSonPropItem->SetReadOnly();
		//ͳ��ϵ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "BOM Number";
		lpInfo->m_strPropHelp = "BOM Number";
#else 
		lpInfo->m_strPropName = "ͳ��ϵ��";
		lpInfo->m_strPropHelp = "ͳ��ϵ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("m_uStatMatCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���Ӹ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType =	PIT_BUTTON;	//PIT_EDIT
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weld Parent Part";
		lpInfo->m_strPropHelp = " if the part needs to weld to other parts, then this attribute is the handle of attached part; If the part does not need to be welded, the value is 0x0��";
#else 
		lpInfo->m_strPropName = "���Ӹ�����";
		lpInfo->m_strPropHelp = "����˹�����Ҫ���ӵ�����������,��ô�����Ծ��Ǳ������������ĺ��ӹ����ľ��;����˹�������Ҫ����,��ֵΪ0X0��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("m_hPartWeldParent");
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ʾ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Hole Visible";
		lpInfo->m_strPropHelp = "Whethe to show the hole";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "��ʾ��";
		lpInfo->m_strPropHelp = "�Ƿ���ʾ��";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("m_bDisplayHole");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���⸨������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual Auxiliary Part";
		lpInfo->m_strPropHelp = "Virtual auxiliary parts are only used to assist other part's position, is not the real statistic material of parts,so not contributing to the material statistics��";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "���⸨������";
		lpInfo->m_strPropHelp = "���⸨������ֻ���ڸ�������������λ�������ǽṹ����ʵ���е�ͳ�Ĺ��������Բ���Ԥ����ͳ�ơ�";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("m_bVirtualPart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�û�ָ��������ɫ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specify Part Color";
		lpInfo->m_strPropHelp = "Whether the part's color is specified by the user";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "ָ��������ɫ";
		lpInfo->m_strPropHelp = "������ɫ�Ƿ�Ϊ�û�ָ��";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstSlot->m_bUserSpecColor)
		{	//������ɫ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Part Color";
			lpInfo->m_strPropHelp = "Part Color";
#else 
			lpInfo->m_strPropName = "������ɫ";
			lpInfo->m_strPropHelp = "������ɫ";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//ʵ�ʳ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Length";
		lpInfo->m_strPropHelp = "U-steel's physical length";
#else 
		lpInfo->m_strPropName = "ʵ�ʳ���";
		lpInfo->m_strPropHelp = "�۸ֵ�ʵ�ʳ���";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("length");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstSlot!=NULL&&pFirstSlot->pStart!=NULL&&pFirstSlot->pEnd!=NULL)
			pPropItem->SetReadOnly();
		//
		if(pFirstSlot->IsShadowObject())
		{
			//Ӱ����Ϣ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Information";
			lpInfo->m_strPropHelp = "Project Information";
#else 
			lpInfo->m_strPropName = "Ӱ����Ϣ";
			lpInfo->m_strPropHelp = "Ӱ����Ϣ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineSlot::GetPropID("shadowInfo");
			pPropItem->m_bHideChildren = FALSE;
			if(CLDSLineSlot::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineSlot::GetPropStatus(pPropItem->m_idProp);
			pPropItem->SetReadOnly(FALSE);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//Ӱ��ĸ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Shadow Mother Object";
			lpInfo->m_strPropHelp = "The shadow mother object's handle of currently part,when it's 0x0 means that the currently object is original object��";
#else 
			lpInfo->m_strPropName = "Ӱ��ĸ����";
			lpInfo->m_strPropHelp = "��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("m_hMotherObj");
			if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstSlot->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstSlot->IsBlockRefShadowObject())
			{
				//������������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block Assembly";
				lpInfo->m_strPropHelp = "The handle of vest in block assembly ��";
#else 
				lpInfo->m_strPropName = "������������";
				lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("m_hBlockRef");
				if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstSlot->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//��������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block";
				lpInfo->m_strPropHelp = "The handle of vest in block��";
#else 
				lpInfo->m_strPropName = "��������";
				lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("m_hBlock");
				if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstSlot->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else 
		{
			//��������
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
			pPropItem->m_idProp = CLDSLineSlot::GetPropID("relativeObjs");
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
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
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("sNotes");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��һ������
		CPropTreeItem *pLeafPropItem;
		//��׼�淨��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Work Plane Normal";
		lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��";
#else 
		lpInfo->m_strPropName = "��׼�淨��";
		lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("workPlaneNorm");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSLineSlot::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineSlot::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//����ָ����ʽ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Calculate Mode";
		lpInfo->m_strPropHelp = "ֱ�Ӹ�������ͨ�����˼�����";
		lpInfo->m_cmbItems = "0.Specify Directly|1.Reference Rod|2.Horizontal Pedestal";
#else 
		lpInfo->m_strPropName = "���㷽ʽ";
		lpInfo->m_strPropHelp = "ֱ�Ӹ�������ͨ�����˼�����";
		lpInfo->m_cmbItems = "0.ֱ��ָ��|1.���ո˼�|2.ˮƽ����";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("m_iNormCalStyle");
		pSonPropItem->m_bHideChildren = FALSE;
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstSlot->m_iNormCalStyle==1)
		{
			//��׼����Ʒ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Plane Near Normal";
			lpInfo->m_strPropHelp = "Its generally specified by the user, the system can calculate it according to the near normals and channel steel axis��";
#else 
			lpInfo->m_strPropName = "��׼����Ʒ���";
			lpInfo->m_strPropHelp = "�۸ֻ�׼����Ʒ���һ�����û�ָ����ϵͳ�ɸ��ݽ��Ʒ��߼��۸����߾�ȷ������۸ֵĻ�׼�淨�ߡ�";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("nearWorkPlaneNorm");
			pSonPropItem->m_bHideChildren = TRUE;
			if(CLDSLineSlot::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren = CLDSLineSlot::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Coordinate";
			lpInfo->m_strPropHelp = "The x coordinate of datum plane's normal";
#else 
			lpInfo->m_strPropName = "X�������";
			lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��X�᷽�������";
#endif
			pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
			pLeafPropItem->m_idProp = CLDSLineSlot::GetPropID("nearWorkPlaneNorm.x");
			pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
				pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Coordinate";
			lpInfo->m_strPropHelp = "The y coordinate of datum plane's normal";
#else 
			lpInfo->m_strPropName = "Y�������";
			lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��Y�᷽�������";
#endif
			pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
			pLeafPropItem->m_idProp = CLDSLineSlot::GetPropID("nearWorkPlaneNorm.y");
			pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
				pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Coordinate";
			lpInfo->m_strPropHelp = "The z coordinate of datum plane's normal";
#else 
			lpInfo->m_strPropName = "Z�������";
			lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��Z�᷽�������";
#endif
			pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
			pLeafPropItem->m_idProp = CLDSLineSlot::GetPropID("nearWorkPlaneNorm.z");
			pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
				pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

			//hWorkNormRefPart
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Normal Reference Rod";
			lpInfo->m_strPropHelp = "It is work reference rod when calculating datum plane's normal��It is start node or end node of parent rod when it is 0X0(It can't be rod itself)";
#else 
			lpInfo->m_strPropName = "���߲��ո˼�";
			lpInfo->m_strPropHelp = "����۸ֻ�׼�淨��ʱ�Ĺ�������ո˼���0X0ʱĬ��Ϊʼ�˻��ն˽ڵ㸸�˼�(����Ϊ�˼�����)";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("hWorkNormRefPart");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Coordinate";
		lpInfo->m_strPropHelp = "The x coordinate of datum plane's normal";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��X�᷽�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("workPlaneNorm.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(pFirstSlot->m_iNormCalStyle==1);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Coordinate";
		lpInfo->m_strPropHelp = "The y coordinate of datum plane's normal";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��Y�᷽�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("workPlaneNorm.y");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(pFirstSlot->m_iNormCalStyle==1);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Coordinate";
		lpInfo->m_strPropHelp = "The z coordinate of datum plane's normal";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "�۸ֻ�׼�淨��Z�᷽�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("workPlaneNorm.z");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(pFirstSlot->m_iNormCalStyle==1);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//ʼ����Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Information";
		lpInfo->m_strPropHelp = "U-steel's start information";
#else 
		lpInfo->m_strPropName = "ʼ����Ϣ";
		lpInfo->m_strPropHelp = "�۸�ʼ����Ϣ";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineSlot::GetPropID("startInfo");
		if(CLDSLineSlot::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineSlot::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Handle";
		lpInfo->m_strPropHelp = "U-steel's start handle"; 
#else 
		lpInfo->m_strPropName = "�����";
		lpInfo->m_strPropHelp = "�۸������"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("hStart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//����ͷ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Rod Oddment";
		lpInfo->m_strPropHelp = "U-steel's start oddment";
#else 
		lpInfo->m_strPropName = "����ͷ";
		lpInfo->m_strPropHelp = "�۸�ʼ������ͷֵ";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("oddStart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//ʼ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Design Coordinate";
		lpInfo->m_strPropHelp = "U-steel's start design coordinate";
#else 
		lpInfo->m_strPropName = "ʼ���������";
		lpInfo->m_strPropHelp = "�۸�ʼ���������";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = TRUE;
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("startPos");
		if(CLDSLineSlot::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineSlot::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Coordinate";
		lpInfo->m_strPropHelp = "The U-steel's start x coordinate";
#else 
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "�۸�ʼ��X�᷽�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("startPos.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstSlot->desStartPos.endPosType!=4);	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Coordinate";
		lpInfo->m_strPropHelp = "The U-steel's start y coordinate";
#else 
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "�۸�ʼ��Y�᷽�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("startPos.y");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstSlot->desStartPos.endPosType!=4);	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Coordinate";
		lpInfo->m_strPropHelp = "The U-steel's start z coordinate";
#else 
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "�۸�ʼ��Z�᷽�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("startPos.z");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstSlot->desStartPos.endPosType!=4);	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//�ն�ʵ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Actual Coordinate";
		lpInfo->m_strPropHelp = "The start coordinate of  u-steel after considering oddment";
#else 
		lpInfo->m_strPropName = "ʼ��ʵ������";
		lpInfo->m_strPropHelp = "�۸�ʼ�˿�������ͷ�������λ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("actualStartPos");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//�˵�λ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Locate Mode";
		lpInfo->m_strPropHelp = "The datum positon of U-steel angle's start ";
		lpInfo->m_cmbItems = "Node Is Datum Node|Lap Joint Connect|Point Line Project|Center Line Cross Point|Specified Coordinate|";
#else 
		lpInfo->m_strPropName = "��λ��ʽ";
		lpInfo->m_strPropHelp = "�۸�ʼ�˻���λ��";
		lpInfo->m_cmbItems = "�ڵ㼴����|�������|����ͶӰ|���߽���|ָ������|";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("desStartPos.endPosType");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(CLDSLineSlot::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineSlot::GetPropStatus(pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstSlot->IsStartPosLocked()||pFirstSlot->bHuoQuStart);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pPropItem,TRUE);
		//�ն���Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Information";
		lpInfo->m_strPropHelp = "U-steel's end information";
#else 
		lpInfo->m_strPropName = "�ն���Ϣ";
		lpInfo->m_strPropHelp = "�۸��ն���Ϣ";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineSlot::GetPropID("endInfo");
		if(CLDSLineSlot::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineSlot::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//�յ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Handle";
		lpInfo->m_strPropHelp = "U-steel's end handle";
#else 
		lpInfo->m_strPropName = "�յ���";
		lpInfo->m_strPropHelp = "�۸��յ���";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("hEnd");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//����ͷ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Rod Oddment";
		lpInfo->m_strPropHelp = "U-steel's end oddment";
#else 
		lpInfo->m_strPropName = "����ͷ";
		lpInfo->m_strPropHelp = "�۸��ն�����ͷֵ";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("oddEnd");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//�ն�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "U-steel End Design Coordinate";
		lpInfo->m_strPropHelp = "The design coodinate of U-steel's end";
#else 
		lpInfo->m_strPropName = "�ն��������";
		lpInfo->m_strPropHelp = "�۸��ն��������";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = TRUE;
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("endPos");
		if(CLDSLineSlot::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineSlot::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Coordinate";
		lpInfo->m_strPropHelp = "The x coordinate of U-steel angle's end";
#else 
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "�۸��ն�����X�᷽�������ֵ";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("endPos.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstSlot->desEndPos.endPosType!=4);	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Coordinate";
		lpInfo->m_strPropHelp = "The y coordinate of U-steel angle's end"; 
#else 
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "�۸��ն�����Y�᷽�������ֵ"; 
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("endPos.y");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstSlot->desEndPos.endPosType!=4);	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Coordinate";
		lpInfo->m_strPropHelp = "The z coordinate of U-steel angle's end";
#else 
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "�۸��ն�����Z�᷽�������ֵ";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("endPos.z");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstSlot->desEndPos.endPosType!=4);	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//�ն�ʵ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Actual Coordinate";
		lpInfo->m_strPropHelp = "The end coordinate of  u-steel after considering oddment";
#else 
		lpInfo->m_strPropName = "�ն�ʵ������";
		lpInfo->m_strPropHelp = "�۸��ն˿�������ͷ�������λ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("actualEndPos");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//�˵�λ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Locate Mode";
		lpInfo->m_strPropHelp = "The datum positon of U-steel angle's end";
		lpInfo->m_cmbItems = "Node Is Datum Node|Lap Joint Connect|Point Line Project|Center Line Cross Point|Specified Coordinate|";
#else 
		lpInfo->m_strPropName = "��λ��ʽ";
		lpInfo->m_strPropHelp = "�۸��ն˻���λ��";
		lpInfo->m_cmbItems = "�ڵ㼴����|�������|����ͶӰ|���߽���|ָ������|";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("desEndPos.endPosType");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(CLDSLineSlot::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineSlot::GetPropStatus(pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstSlot->IsEndPosLocked()||pFirstSlot->bHuoQuEnd);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pPropItem,FALSE);

		//�ֹܲ۸ֶ�λ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Special Position Parameter";
		lpInfo->m_strPropHelp = "When u-steels are accessory parts, according to the attached tube handle, locating datum point, and length to locate��";
#else 
		lpInfo->m_strPropName = "����۸ֶ�λ����";
		lpInfo->m_strPropHelp = "�۸�Ϊ�ֹܸ�����ʱ,���������ֹܾ������λ��׼�㼰�۸ֳ��Ƚ��ж�λ��";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineSlot::GetPropID("specialSlot");
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//������ֹ��ϵĲ۸ֶ�λ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Attached Tube";
		lpInfo->m_strPropHelp = "The tube's handle of U-steel attachment. 0x0 expressed as normal U-steel,U-steel is accessory part after specifying tube's handle,U-steel's extension direction and tube's in the same direction��";
#else 
		lpInfo->m_strPropName = "�����ֹ�";
		lpInfo->m_strPropHelp = "�۸��������ĸֹܾ����0X0��ʾΪ��ͨ�۸֣�ָ���ֹܾ����۸�Ϊ�ֹܸ��������۸����췽����ֹ����췽��һ�¡�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineSlot::GetPropID("hAttachTube");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Location Postion";
		lpInfo->m_strPropHelp = "The u-steel natual position,the u-steel's start and end position are defined by its datum position��";
#else 
		lpInfo->m_strPropName = "��λ��׼��";
		lpInfo->m_strPropHelp = "�۸��е�λ�ã����ݲ۸ֳ��ȴӻ�׼��λ�����������춨��۸�ʼ�ն�λ�á�";
#endif
		CPropTreeItem * pDesDatumPointGroup = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pDesDatumPointGroup->m_idProp = CLDSLineSlot::GetPropID("desDatumPoint");
		pDesDatumPointGroup->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr( pDesDatumPointGroup->m_idProp,sText)>0)
			 pDesDatumPointGroup->m_lpNodeInfo->m_strPropValue = sText;
		 pDesDatumPointGroup->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		 pDesDatumPointGroup->m_bHideChildren=TRUE;
		//desDatumPoint.x
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X";
		lpInfo->m_strPropHelp = "datum point's X coordinate component.";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "��׼���X�������";
#endif
		pSonPropItem= pPropList->InsertItem(pDesDatumPointGroup,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("desDatumPoint.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pSonPropItem->SetReadOnly(pFirstSlot->desDatumPoint.datum_pos_style!=0);
		//desDatumPoint.y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y";
		lpInfo->m_strPropHelp = "datum point's Y coordinate component.";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "��׼���Y�������";
#endif
		pSonPropItem = pPropList->InsertItem(pDesDatumPointGroup,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("desDatumPoint.y");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pSonPropItem->SetReadOnly(pFirstSlot->desDatumPoint.datum_pos_style!=0);
		//desDatumPoint.z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z";
		lpInfo->m_strPropHelp = "datum point's Z coordinate component.";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "��׼���Z�������";
#endif
		pSonPropItem = pPropList->InsertItem(pDesDatumPointGroup,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("desDatumPoint.z");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pSonPropItem->SetReadOnly(pFirstSlot->desDatumPoint.datum_pos_style!=0);
		//ֱ���޸ĽŶ��۸ֵ����Ķ�λ��desDatumPoint������Ϣ
		CLDSLineTube* pLineTube=(CLDSLineTube*)Ta.Parts.FromHandle(pFirstSlot->hAttachTube,CLS_LINETUBE);
		if(pLineTube)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Offset from node";
			lpInfo->m_strPropHelp = "Section center's location offset datum node";
#else 
			lpInfo->m_strPropName = "ƫ����ʼ�ڵ�";
			lpInfo->m_strPropHelp = "����Բ�ĵ��ƫ�ƶ�λ��ʼ�ڵ㡣";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineSlot::GetPropID("desDatumPoint.param.hOffsetDatumNode");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr( pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Offset Direction";
			lpInfo->m_strPropHelp = "Offset Direction from datum node along rod";
			lpInfo->m_cmbItems="0.Start->End|1.End->Start";
#else 
			lpInfo->m_strPropName = "����ƫ�Ʒ���";
			lpInfo->m_strPropHelp = "��ƫ����ʼ�ڵ��ػ�׼�ֹܵ�ƫ�Ʒ���";
			lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineSlot::GetPropID("desDatumPoint.param.lenOffsetType");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr( pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Offset Distance";
			lpInfo->m_strPropHelp = "Offset distance from datum node";
#else 
			lpInfo->m_strPropName = "����ƫ����";
			lpInfo->m_strPropHelp = "����Բ�ĵ���ƫ����ʼ�ڵ��������ֹ����߷����ƫ����";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineSlot::GetPropID("desDatumPoint.param.lenOffset");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
			if(pFirstSlot->GetPropValueStr( pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//�����巽ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Specify Directly|1.Specify Datum Edge|";
			if(pLineTube)
				lpInfo->m_cmbItems+="2.Tube's Datum Edge As Datum|3.Tube's Weld Line As Datum|4.Horizontal ray vector";
			lpInfo->m_strPropName="Ray Vector";
			lpInfo->m_strPropHelp="Ray vector define type:0.Specify directly,1 to 3 input rotate angle (1.Specify Datum Edge,2.Tube's Datum Edge As Datum,3.Tube's Weld Line As Datum)";
#else 
			lpInfo->m_cmbItems="0.ֱ��ָ��|1.ָ����׼��|";
			if(pLineTube)
				lpInfo->m_cmbItems+="2.�Ի�׼����ת|3.�Ժ�������ת|4.ˮƽ����";
			lpInfo->m_strPropName="���߷���";
			lpInfo->m_strPropHelp="���߷���������:0.ֱ��ָ��,1~3������ת�Ƕ�(1.��ָ������ת,2.�Ի�׼��Ϊ��׼��ת,3.�Ժ�����Ϊ��׼��ת,4.�����ϵ�ˮƽ���߷���)";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecStyle");
			if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=sText;
			UpdateLineSlotColumnRayVecParamsProperty(pPropList,pPropItem);
		}
	}
	else
	{	//����Ҫ�ؽ���������ҳ
		if(pFirstSlot->pEnd==NULL&&pFirstSlot->pStart==NULL)
		{
			if(pPropDlg->m_arrPropGroupLabel.GetCount()==3)
			{
				pPropDlg->m_arrPropGroupLabel.RemoveAll();
				pPropDlg->m_arrPropGroupLabel.SetSize(0);
				pPropDlg->RefreshTabCtrl(0);
			}
		}
		else
		{
			if(pPropDlg->m_arrPropGroupLabel.GetCount()==0)
			{
				pPropDlg->m_arrPropGroupLabel.SetSize(3);
#ifdef AFX_TARG_ENU_ENGLISH
				pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
				pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"Start");
				pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"End");
#else 
				pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
				pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"ʼ��");
				pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"�ն�");
#endif
				pPropDlg->RefreshTabCtrl(CLDSLineSlot::m_iCurDisplayPropGroup);
			}
		}
		SetPartMutiObjsProp(pFirstSlot,"handle");
		SetPartMutiObjsProp(pFirstSlot,"layer");
		SetPartMutiObjsProp(pFirstSlot,"cMaterial");
		SetPartMutiObjsProp(pFirstSlot,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstSlot,"specification");
		SetPartMutiObjsProp(pFirstSlot,"iSeg");
		SetPartMutiObjsProp(pFirstSlot,"sPartNo");
		SetPartMutiObjsProp(pFirstSlot,"cfgword");
		pFindItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("relativeObjs"),&pParentItem);
		if (pFindItem)
		{
			if (pFirstSlot->IsShadowObject())
			{	//����ͨ�����л���Ӱ�乹��
				pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("relativeObjs"));
				//Ӱ����Ϣ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Project Information";
				lpInfo->m_strPropHelp = "Project Information";
#else 
				lpInfo->m_strPropName = "Ӱ����Ϣ";
				lpInfo->m_strPropHelp = "Ӱ����Ϣ";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,17,true);
				pPropItem->m_idProp = CLDSLineSlot::GetPropID("shadowInfo");
				if(CLDSLineSlot::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSLineSlot::GetPropStatus(pPropItem->m_idProp);
				pPropItem->SetReadOnly(FALSE);
				//Ӱ��ĸ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Shadow Mother Object";
				lpInfo->m_strPropHelp = "The shadow mother object's handle of currently part,when it's 0x0 means that the currently object is original object��";
#else 
				lpInfo->m_strPropName = "Ӱ��ĸ����";
				lpInfo->m_strPropHelp = "��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("m_hMotherObj");
				if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstSlot->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				if (pFirstSlot->IsBlockRefShadowObject())
				{
					//������������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Assembly";
					lpInfo->m_strPropHelp = "The handle of vest in block assembly ��";
#else 
					lpInfo->m_strPropName = "������������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("m_hBlockRef");
					if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstSlot->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
					//��������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block";
					lpInfo->m_strPropHelp = "The handle of vest in block��";
#else 
					lpInfo->m_strPropName = "��������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("m_hBlock");
					if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstSlot->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
			else
				SetPartMutiObjsProp(pFirstSlot,"relativeObjs");
		}
		pFindItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("shadowInfo"),&pParentItem);
		if (pFindItem)
		{
			if (!pFirstSlot->IsShadowObject())
			{	//��Ӱ�乹���л�����ͨ����
				pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("shadowInfo"));
				pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("m_hMotherObj"));
				pFindItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("m_hBlockRef"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("m_hBlockRef"));
				pFindItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("m_hBlock"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("m_hBlock"));
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Relative Object";
				lpInfo->m_strPropHelp = "The object relate to currently object and mirror style";
#else 
				lpInfo->m_strPropName = "��������";
				lpInfo->m_strPropHelp = "�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,17,true);
				pPropItem->m_idProp = CLDSLineSlot::GetPropID("relativeObjs");
				if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			else
			{
				SetPartMutiObjsProp(pFirstSlot,"m_hMotherObj");
				pFindItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("m_hBlockRef"),&pParentItem);
				if (pFindItem&&pFirstSlot->IsBlockRefShadowObject())
				{
					SetPartMutiObjsProp(pFirstSlot,"m_hBlockRef");
					SetPartMutiObjsProp(pFirstSlot,"m_hBlock");
				}
				else if (pFindItem&&!pFirstSlot->IsBlockRefShadowObject())
				{
					pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("m_hBlockRef"));
					pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("m_hBlock"));
				}
				else if(pFindItem==NULL&&pFirstSlot->IsBlockRefShadowObject())
				{
					pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("m_hMotherObj"),&pParentItem);
					//������������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Assembly";
					lpInfo->m_strPropHelp = "The handle of vest in block assembly ��";
#else 
					lpInfo->m_strPropName = "������������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
					pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("m_hBlockRef");
					if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstSlot->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
					//��������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block";
					lpInfo->m_strPropHelp = "The handle of vest in block��";
#else 
					lpInfo->m_strPropName = "��������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
					pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSLineSlot::GetPropID("m_hBlock");
					if(pFirstSlot->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstSlot->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
		}
		SetPartMutiObjsProp(pFirstSlot,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstSlot,"m_hPartWeldParent");
		SetPartMutiObjsProp(pFirstSlot,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstSlot,"m_bVirtualPart");
		SetPartMutiObjsProp(pFirstSlot,"width");
		SetPartMutiObjsProp(pFirstSlot,"thick");
		SetPartMutiObjsProp(pFirstSlot,"height");
		SetPartMutiObjsProp(pFirstSlot,"m_bUserSpecColor");
		if(pFirstSlot->m_bUserSpecColor)
		{	
			if(!SetPartMutiObjsProp(pFirstSlot,"crMaterial"))
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("m_bUserSpecColor"),NULL);
				if(pFindItem)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
					lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Part Color";
					lpInfo->m_strPropHelp = "Part Color";
#else 
					lpInfo->m_strPropName = "������ɫ";
					lpInfo->m_strPropHelp = "������ɫ";
#endif
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSLineSlot::GetPropID("crMaterial");
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(selectObjs,pPropItem->m_idProp);
					if(pFirstSlot->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				}
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstSlot,"length");
		SetPartMutiObjsProp(pFirstSlot,"sNotes");
		SetPartMutiObjsProp(pFirstSlot,"m_iNormCalStyle");
		SetPartMutiObjsProp(pFirstSlot,"nearWorkPlaneNorm.x");
		SetPartMutiObjsProp(pFirstSlot,"nearWorkPlaneNorm.y");
		SetPartMutiObjsProp(pFirstSlot,"nearWorkPlaneNorm.z");
		SetPartMutiObjsProp(pFirstSlot,"hWorkNormRefPart");
		SetPartMutiObjsProp(pFirstSlot,"workPlaneNorm.x");
		SetPartMutiObjsProp(pFirstSlot,"workPlaneNorm.y");
		SetPartMutiObjsProp(pFirstSlot,"workPlaneNorm.z");
		SetPartMutiObjsProp(pFirstSlot,"hStart");
		SetPartMutiObjsProp(pFirstSlot,"oddStart");
		SetPartMutiObjsProp(pFirstSlot,"startPos.x");
		SetPartMutiObjsProp(pFirstSlot,"startPos.y");
		SetPartMutiObjsProp(pFirstSlot,"startPos.z");
		SetPartMutiObjsProp(pFirstSlot,"desStartPos.endPosType");
		SetPartMutiObjsProp(pFirstSlot,"desStartPos.datum_pos_style");
		pFindItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desStartPos.endPosType"),&pParentItem);
		if(pFindItem)
			UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pFindItem,TRUE);
		SetPartMutiObjsProp(pFirstSlot,"desStartPos.hDatumPart");
		SetPartMutiObjsProp(pFirstSlot,"desStartPos.fNormOffset");
		SetPartMutiObjsProp(pFirstSlot,"desStartPos.fEccentricDist");
		SetPartMutiObjsProp(pFirstSlot,"desStartPos.face_offset_norm");
		SetPartMutiObjsProp(pFirstSlot,"desStartPos.fTransverseOffset");
		SetPartMutiObjsProp(pFirstSlot,"desStartPos.bDatumEndStartEnd");
		SetPartMutiObjsProp(pFirstSlot,"desStartPos.bDatumStartStartEnd");
		SetPartMutiObjsProp(pFirstSlot,"desStartPos.hDatumStartPart");
		SetPartMutiObjsProp(pFirstSlot,"desStartPos.hDatumEndPart");
		//�ն���Ϣ
		SetPartMutiObjsProp(pFirstSlot,"hEnd");
		SetPartMutiObjsProp(pFirstSlot,"oddEnd");	//����ͷ
		SetPartMutiObjsProp(pFirstSlot,"endPos.x");	//�ն�����--X
		SetPartMutiObjsProp(pFirstSlot,"endPos.y");	//�ն�����--Y
		SetPartMutiObjsProp(pFirstSlot,"endPos.z");	//�ն�����--Z
		SetPartMutiObjsProp(pFirstSlot,"desEndPos.endPosType");	//�˻���λ��
		SetPartMutiObjsProp(pFirstSlot,"desEndPos.datum_pos_style");
		pFindItem=pPropList->FindItemByPropId(CLDSLineSlot::GetPropID("desEndPos.endPosType"),&pParentItem);
		if(pFindItem)
			UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pFindItem,FALSE);
		SetPartMutiObjsProp(pFirstSlot,"desEndPos.hDatumPart");
		SetPartMutiObjsProp(pFirstSlot,"desEndPos.fNormOffset");
		SetPartMutiObjsProp(pFirstSlot,"desEndPos.fEccentricDist");
		SetPartMutiObjsProp(pFirstSlot,"desEndPos.face_offset_norm");
		SetPartMutiObjsProp(pFirstSlot,"desEndPos.fTransverseOffset");
		SetPartMutiObjsProp(pFirstSlot,"desEndPos.bDatumEndStartEnd");
		SetPartMutiObjsProp(pFirstSlot,"desEndPos.bDatumStartStartEnd");
		SetPartMutiObjsProp(pFirstSlot,"desEndPos.hDatumStartPart");
		SetPartMutiObjsProp(pFirstSlot,"desEndPos.hDatumEndPart");
		
		//������ֹ��ϵĲ۸ֶ�λ����
		if(pFirstSlot->hAttachTube<=0x20)
		{
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("hAttachTube"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.x"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.y"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.z"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.hOffsetDatumNode"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.lenOffset"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.lenOffsetType"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecStyle"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecStdStyle"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.x"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.y"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVec.z"));
			pPropList->DeleteItemByPropId(CLDSLineSlot::GetPropID("desDatumPoint.param.RayVecRotateAngle"));
		}
		else
		{
			SetPartMutiObjsProp(pFirstSlot,"hAttachTube");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint.x");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint.y");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint.z");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint.param.hOffsetDatumNode");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint.param.lenOffset");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint.param.lenOffsetType");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint.param.RayVecStyle");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint.param.RayVecStdStyle");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint.param.RayVec.x");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint.param.RayVec.y");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint.param.RayVec.z");
			SetPartMutiObjsProp(pFirstSlot,"desDatumPoint.param.RayVecRotateAngle");
		}
	}
	pPropList->Redraw();
}

void CLDSView::DisplayLineFlatProperty(BOOL bRebuild)
{
	CLDSLineFlat *pFirstFlat = (CLDSLineFlat*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pGroupItem, *pPropItem, *pSonPropItem, *pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo=NULL;
	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_START=2,GROUP_END=3;
	if(bRebuild)
	{	//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(3);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"Start");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"End");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"ʼ��");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"�ն�");
#endif
		pPropDlg->RefreshTabCtrl(CLDSLineFlat::m_iCurDisplayPropGroup);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_LINEFLAT;
		pPropList->SetModifyValueFunc(ModifyLineFlatProperty);
		pPropList->SetButtonClickFunc(LineFlatButtonClick);
		pPropList->SetModifyStatusFunc(ModifyLineFlatItemStatus);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstFlat->BelongModel()->IsTowerModel()||pFirstFlat->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//��ֻ�����Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Flat Angle Basic Information";
		lpInfo->m_strPropHelp = "The flat angle's basic information";
#else 
		lpInfo->m_strPropName = "��ֻ�����Ϣ";
		lpInfo->m_strPropHelp = "��ֻ�����Ϣ";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineFlat::GetPropID("basicInfo");
		if(CLDSLineFlat::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineFlat::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "Flat angle's handle";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "��־��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("handle");
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
		//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "The layer of flat angle"; 
#else 
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "�������ͼ��"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("layer");
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//����	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Material";
		lpInfo->m_strPropHelp = "Flat angle's material";
#else 
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "��ֲ���";
#endif
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390|Q420|Q460";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("cMaterial");
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�����ȼ�	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Quality Grade";
		lpInfo->m_strPropHelp = "Flat angle's quality grade";
		lpInfo->m_cmbItems = "Default|A|B|C|D|";
#else 
		lpInfo->m_strPropName = "�����ȼ�";
		lpInfo->m_strPropHelp = "��������ȼ�";
		lpInfo->m_cmbItems = "Ĭ��|A|B|C|D|";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("m_cQualityLevel");
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specification";
		lpInfo->m_strPropHelp = "Flat angle's specification( Width X Thick)";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "��ֵĹ��(���X���)";
#endif
		lpInfo->m_cmbItems = MakeFlatSetString();
		lpInfo->m_cmbType = CDT_EDIT;
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("specification");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�ֶκ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Segment Number";
		lpInfo->m_strPropHelp = "Segment Number";
#else 
		lpInfo->m_strPropName = "�ֶκ�";
		lpInfo->m_strPropHelp = "�ֶκ�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("iSeg");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		//�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part Number";
		lpInfo->m_strPropHelp = "Part Number"; 
#else 
		lpInfo->m_strPropName = "�������";
		lpInfo->m_strPropHelp = "�������"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("sPartNo");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ĺ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Configure Word";
		lpInfo->m_strPropHelp = "Configure Word";
#else 
		lpInfo->m_strPropName = "��ĺ�";
		lpInfo->m_strPropHelp = "��ĺ�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("cfgword");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//ͳ��ϵ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "BOM Number";
		lpInfo->m_strPropHelp = "BOM Number";
#else 
		lpInfo->m_strPropName = "ͳ��ϵ��";
		lpInfo->m_strPropHelp = "ͳ��ϵ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("m_uStatMatCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���Ӹ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType =	PIT_BUTTON;	//PIT_EDIT
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weld Parent Part";
		lpInfo->m_strPropHelp = " if the part needs to weld to other parts, then this attribute is the handle of attached part; If the part does not need to be welded, the value is 0x0��";
#else 
		lpInfo->m_strPropName = "���Ӹ�����";
		lpInfo->m_strPropHelp = "����˹�����Ҫ���ӵ�����������,��ô�����Ծ��Ǳ������������ĺ��ӹ����ľ��;����˹�������Ҫ����,��ֵΪ0X0��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("m_hPartWeldParent");
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ʾ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Hole Visible";
		lpInfo->m_strPropHelp = "Whethe to show the hole";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "��ʾ��";
		lpInfo->m_strPropHelp = "�Ƿ���ʾ��";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("m_bDisplayHole");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���⸨������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual Auxiliary Part";
		lpInfo->m_strPropHelp = "Virtual auxiliary parts are only used to assist other part's position, is not the real statistic material of parts,so not contributing to the material statistics��";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "���⸨������";
		lpInfo->m_strPropHelp = "���⸨������ֻ���ڸ�������������λ�������ǽṹ����ʵ���е�ͳ�Ĺ��������Բ���Ԥ����ͳ�ơ�";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("m_bVirtualPart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�û�ָ��������ɫ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specify Part Color";
		lpInfo->m_strPropHelp = "Whether the part's color is specified by the user";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "ָ��������ɫ";
		lpInfo->m_strPropHelp = "������ɫ�Ƿ�Ϊ�û�ָ��";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstFlat->m_bUserSpecColor)
		{	//������ɫ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Part Color";
			lpInfo->m_strPropHelp = "Part Color";
#else 
			lpInfo->m_strPropName = "������ɫ";
			lpInfo->m_strPropHelp = "������ɫ";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//ʵ�ʳ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Length";
		lpInfo->m_strPropHelp = "Flat anlge's physical length";
#else 
		lpInfo->m_strPropName = "ʵ�ʳ���";
		lpInfo->m_strPropHelp = "��ֵ�ʵ�ʳ���";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("length");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
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
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("sNotes");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//
		if(pFirstFlat->IsShadowObject())
		{
			//Ӱ����Ϣ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Information";
			lpInfo->m_strPropHelp = "Project Information";
#else 
			lpInfo->m_strPropName = "Ӱ����Ϣ";
			lpInfo->m_strPropHelp = "Ӱ����Ϣ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineFlat::GetPropID("shadowInfo");
			pPropItem->m_bHideChildren = FALSE;
			if(CLDSLineFlat::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineFlat::GetPropStatus(pPropItem->m_idProp);
			pPropItem->SetReadOnly(FALSE);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//Ӱ��ĸ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Shadow Mother Object";
			lpInfo->m_strPropHelp = "The shadow mother object's handle of currently part,when it's 0x0 means that the currently object is original object��";
#else 
			lpInfo->m_strPropName = "Ӱ��ĸ����";
			lpInfo->m_strPropHelp = "��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("m_hMotherObj");
			if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstFlat->IsBlockRefShadowObject())
			{
				//������������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block Assembly";
				lpInfo->m_strPropHelp = "The handle of vest in block assembly ��";
#else 
				lpInfo->m_strPropName = "������������";
				lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("m_hBlockRef");
				if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//��������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block";
				lpInfo->m_strPropHelp = "The handle of vest in block��";
#else 
				lpInfo->m_strPropName = "��������";
				lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("m_hBlock");
				if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else 
		{
			//��������
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
			pPropItem->m_idProp = CLDSLineFlat::GetPropID("relativeObjs");
			if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//��һ������
		CPropTreeItem *pLeafPropItem;
		//��׼�淨��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Plane Normal";
		lpInfo->m_strPropHelp = "The normal of flat angle's datum plane";
#else 
		lpInfo->m_strPropName = "��׼�淨��";
		lpInfo->m_strPropHelp = "��ֻ�׼�淨��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("workPlaneNorm");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSLineFlat::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineFlat::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//����ָ����ʽ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Calculate Mode";
		lpInfo->m_strPropHelp = "Specify directly or specify by parent rod";
		lpInfo->m_cmbItems = "0.Specify Directly|1.Reference Rod";
#else 
		lpInfo->m_strPropName = "���㷽ʽ";
		lpInfo->m_strPropHelp = "ֱ�Ӹ�������ͨ�����˼�����";
		lpInfo->m_cmbItems = "0.ֱ��ָ��|1.���ո˼�";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("m_iNormCalStyle");
		pSonPropItem->m_bHideChildren = FALSE;
		if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstFlat->m_iNormCalStyle==1)
		{
			//��׼����Ʒ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Plane Near Normal";
			lpInfo->m_strPropHelp = "Its generally specified by the user, the system can calculate it according to the near normals and channel steel axis��";
#else 
			lpInfo->m_strPropName = "��׼����Ʒ���";
			lpInfo->m_strPropHelp = "��ֻ�׼����Ʒ���һ�����û�ָ����ϵͳ�ɸ��ݽ��Ʒ��߼�������߾�ȷ�������ֵĻ�׼�淨�ߡ�";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("nearWorkPlaneNorm");
			pSonPropItem->m_bHideChildren = TRUE;
			if(CLDSLineFlat::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren = CLDSLineFlat::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Coordinate";
			lpInfo->m_strPropHelp = "The x coodinate of datum plane's normal";
#else 
			lpInfo->m_strPropName = "X�������";
			lpInfo->m_strPropHelp = "��ֻ�׼�淨��X�᷽�������";
#endif
			pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
			pLeafPropItem->m_idProp = CLDSLineFlat::GetPropID("nearWorkPlaneNorm.x");
			pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
				pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Coordinate";
			lpInfo->m_strPropHelp = "The y coodinate of datum plane's normal";
#else 
			lpInfo->m_strPropName = "Y�������";
			lpInfo->m_strPropHelp = "��ֻ�׼�淨��Y�᷽�������";
#endif
			pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
			pLeafPropItem->m_idProp = CLDSLineFlat::GetPropID("nearWorkPlaneNorm.y");
			pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
				pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Coordinate";
			lpInfo->m_strPropHelp = "The z coodinate of datum plane's normal";
#else 
			lpInfo->m_strPropName = "Z�������";
			lpInfo->m_strPropHelp = "��ֻ�׼�淨��Z�᷽�������";
#endif
			pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
			pLeafPropItem->m_idProp = CLDSLineFlat::GetPropID("nearWorkPlaneNorm.z");
			pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
				pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

			//hWorkNormRefPart
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Normal Reference Rod";
			lpInfo->m_strPropHelp = "It is work reference rod when calculating datum plane's normal��It is start node or end node of parent rod when it is 0X0(It can't be rod itself)";
#else 
			lpInfo->m_strPropName = "���߲��ո˼�";
			lpInfo->m_strPropHelp = "�����ֻ�׼�淨��ʱ�Ĺ�������ո˼���0X0ʱĬ��Ϊʼ�˻��ն˽ڵ㸸�˼�(����Ϊ�˼�����)";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("hWorkNormRefPart");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Coordinate";
		lpInfo->m_strPropHelp = "The x coodinate of datum plane's normal";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "��ֻ�׼�淨��X�᷽�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("workPlaneNorm.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(pFirstFlat->m_iNormCalStyle==1||pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Coordinate";
		lpInfo->m_strPropHelp = "The y coodinate of datum plane's normal";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "��ֻ�׼�淨��Y�᷽�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("workPlaneNorm.y");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(pFirstFlat->m_iNormCalStyle==1||pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Coordinate";
		lpInfo->m_strPropHelp = "The z coodinate of datum plane's normal";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "��ֻ�׼�淨��Z�᷽�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("workPlaneNorm.z");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(pFirstFlat->m_iNormCalStyle==1||pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//ʼ����Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Information";
		lpInfo->m_strPropHelp = "Flat angle's start information";
#else 
		lpInfo->m_strPropName = "ʼ����Ϣ";
		lpInfo->m_strPropHelp = "���ʼ����Ϣ";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineFlat::GetPropID("startInfo");
		if(CLDSLineFlat::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineFlat::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Handle";
		lpInfo->m_strPropHelp = "Flat angle's start handle"; 
#else 
		lpInfo->m_strPropName = "�����";
		lpInfo->m_strPropHelp = "��������"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("hStart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//����ͷ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Rod Oddment";
		lpInfo->m_strPropHelp = "Flat angle's start oddment";
#else 
		lpInfo->m_strPropName = "����ͷ";
		lpInfo->m_strPropHelp = "���ʼ������ͷֵ";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("oddStart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//ʼ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Design Coordinate";
		lpInfo->m_strPropHelp = "Flat steel's start design coordinate";
#else 
		lpInfo->m_strPropName = "ʼ���������";
		lpInfo->m_strPropHelp = "���ʼ���������";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = TRUE;
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("startPos");
		if(CLDSLineFlat::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineFlat::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Coordinate";
		lpInfo->m_strPropHelp = "The x coordinate of flat angle's start";
#else 
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "���ʼ��X�᷽�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("startPos.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstFlat->desStartPos.endPosType!=4||pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Coordinate";
		lpInfo->m_strPropHelp = "The y coordinate of flat angle's start";
#else 
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "���ʼ��Y�᷽�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("startPos.y");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstFlat->desStartPos.endPosType!=4||pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Coordinate";
		lpInfo->m_strPropHelp = "The z coordinate of flat angle's start";
#else 
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "���ʼ��Z�᷽�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("startPos.z");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstFlat->desStartPos.endPosType!=4||pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//ʼ��ʵ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Actual Coordinate";
		lpInfo->m_strPropHelp = "The start coordinate of  flat angle after considering oddment";
#else 
		lpInfo->m_strPropName = "ʼ��ʵ������";
		lpInfo->m_strPropHelp = "���ʼ�˿�������ͷ�������λ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("actualStartPos");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//�˵�λ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Locate Mode";
		lpInfo->m_strPropHelp = "The datum position of flat angle's start";
		lpInfo->m_cmbItems = "Node Is Datum Node|Lap Joint Connect|Point Line Project|Center Line Cross Point|Specified Coordinate|";
#else 
		lpInfo->m_strPropName = "��λ��ʽ";
		lpInfo->m_strPropHelp = "���ʼ�˻���λ��";
		lpInfo->m_cmbItems = "�ڵ㼴����|�������|����ͶӰ|���߽���|ָ������|";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("desStartPos.endPosType");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(CLDSLineFlat::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineFlat::GetPropStatus(pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		UpdateLineFlat_DesPos_EndPosType_TreeItem(pPropList,pPropItem,TRUE);

		//�ն���Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Information";
		lpInfo->m_strPropHelp = "Flat anlge's end information";
#else 
		lpInfo->m_strPropName = "�ն���Ϣ";
		lpInfo->m_strPropHelp = "����ն���Ϣ";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineFlat::GetPropID("endInfo");
		if(CLDSLineFlat::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineFlat::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//�յ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Handle";
		lpInfo->m_strPropHelp = "Flat angle's end handle";
#else 
		lpInfo->m_strPropName = "�յ���";
		lpInfo->m_strPropHelp = "����յ���";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("hEnd");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//����ͷ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Rod Oddment";
		lpInfo->m_strPropHelp = "The oddment of flat angle's end";
#else 
		lpInfo->m_strPropName = "����ͷ";
		lpInfo->m_strPropHelp = "����ն�����ͷֵ";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("oddEnd");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//�ն�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Design Coordinate";
		lpInfo->m_strPropHelp = "Flat steel's end design coordinate";
#else 
		lpInfo->m_strPropName = "�ն��������";
		lpInfo->m_strPropHelp = "����ն��������";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = TRUE;
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("endPos");
		if(CLDSLineFlat::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineFlat::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Coordinate";
		lpInfo->m_strPropHelp = "Flat steel's end x coordinate";
#else 
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "����ն�����X�᷽�������ֵ";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("endPos.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstFlat->desEndPos.endPosType!=4||pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Coordinate";
		lpInfo->m_strPropHelp = "Flat steel's end y coordinate"; 
#else 
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "����ն�����Y�᷽�������ֵ"; 
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("endPos.y");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstFlat->desEndPos.endPosType!=4||pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Coordinate";
		lpInfo->m_strPropHelp = "Flat steel's end z coordinate";
#else 
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "����ն�����Z�᷽�������ֵ";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("endPos.z");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstFlat->desEndPos.endPosType!=4||pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//�ն�ʵ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Actual Coordinate";
		lpInfo->m_strPropHelp = "The end coordinate of  flat angle after considering oddment";
#else 
		lpInfo->m_strPropName = "�ն�ʵ������";
		lpInfo->m_strPropHelp = "����ն˿�������ͷ�������λ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("actualEndPos");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//�˵�λ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Locate Mode";
		lpInfo->m_strPropHelp = "The datum position of flat angle's end";
		lpInfo->m_cmbItems = "Node Is Datum Node|Lap Joint Connect|Point Line Project|Center Line Cross Point|Specified Coordinate|";
#else 
		lpInfo->m_strPropName = "��λ��ʽ";
		lpInfo->m_strPropHelp = "����ն˻���λ��";
		lpInfo->m_cmbItems = "�ڵ㼴����|�������|����ͶӰ|���߽���|ָ������|";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineFlat::GetPropID("desEndPos.endPosType");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(CLDSLineFlat::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineFlat::GetPropStatus(pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||pFirstFlat->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		UpdateLineFlat_DesPos_EndPosType_TreeItem(pPropList,pPropItem,FALSE);
	}
	else
	{//����Ҫ�ؽ���������ҳ
		CPropTreeItem *pFindItem, *pParentItem = NULL;
		SetPartMutiObjsProp(pFirstFlat,"handle");
		SetPartMutiObjsProp(pFirstFlat,"layer");
		SetPartMutiObjsProp(pFirstFlat,"cMaterial");
		SetPartMutiObjsProp(pFirstFlat,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstFlat,"specification");
		SetPartMutiObjsProp(pFirstFlat,"iSeg");
		SetPartMutiObjsProp(pFirstFlat,"sPartNo");
		SetPartMutiObjsProp(pFirstFlat,"cfgword");
		pFindItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("relativeObjs"),&pParentItem);
		if (pFindItem)
		{
			if (pFirstFlat->IsShadowObject())
			{	//����ͨ�����л���Ӱ�乹��
				pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("relativeObjs"));
				//Ӱ����Ϣ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Project Information";
				lpInfo->m_strPropHelp = "Project Information";
#else 
				lpInfo->m_strPropName = "Ӱ����Ϣ";
				lpInfo->m_strPropHelp = "Ӱ����Ϣ";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,14,true);
				pPropItem->m_idProp = CLDSLineFlat::GetPropID("shadowInfo");
				if(CLDSLineFlat::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSLineFlat::GetPropStatus(pPropItem->m_idProp);
				pPropItem->SetReadOnly(FALSE);
				//Ӱ��ĸ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Shadow Mother Object";
				lpInfo->m_strPropHelp = "The shadow mother object's handle of currently part,when it's 0x0 means that the currently object is original object��";
#else 
				lpInfo->m_strPropName = "Ӱ��ĸ����";
				lpInfo->m_strPropHelp = "��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("m_hMotherObj");
				if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				if (pFirstFlat->IsBlockRefShadowObject())
				{
					//������������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Assembly";
					lpInfo->m_strPropHelp = "The handle of vest in block assembly ��";
#else 
					lpInfo->m_strPropName = "������������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("m_hBlockRef");
					if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
					//��������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block";
					lpInfo->m_strPropHelp = "The handle of vest in block��";
#else 
					lpInfo->m_strPropName = "��������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("m_hBlock");
					if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
			else
				SetPartMutiObjsProp(pFirstFlat,"relativeObjs");
		}
		pFindItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("shadowInfo"),&pParentItem);
		if (pFindItem)
		{
			if (!pFirstFlat->IsShadowObject())
			{	//��Ӱ�乹���л�����ͨ����
				pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("shadowInfo"));
				pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("m_hMotherObj"));
				pFindItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("m_hBlockRef"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("m_hBlockRef"));
				pFindItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("m_hBlock"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("m_hBlock"));
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Relative Object";
				lpInfo->m_strPropHelp = "The object relate to currently object and mirror style";
#else 
				lpInfo->m_strPropName = "��������";
				lpInfo->m_strPropHelp = "�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,14,true);
				pPropItem->m_idProp = CLDSLineFlat::GetPropID("relativeObjs");
				if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			else
			{
				SetPartMutiObjsProp(pFirstFlat,"m_hMotherObj");
				pFindItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("m_hBlockRef"),&pParentItem);
				if (pFindItem&&pFirstFlat->IsBlockRefShadowObject())
				{
					SetPartMutiObjsProp(pFirstFlat,"m_hBlockRef");
					SetPartMutiObjsProp(pFirstFlat,"m_hBlock");
				}
				else if (pFindItem&&!pFirstFlat->IsBlockRefShadowObject())
				{
					pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("m_hBlockRef"));
					pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("m_hBlock"));
				}
				else if(pFindItem==NULL&&pFirstFlat->IsBlockRefShadowObject())
				{
					pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("m_hMotherObj"),&pParentItem);
					//������������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Assembly";
					lpInfo->m_strPropHelp = "The handle of vest in block assembly ��";
#else 
					lpInfo->m_strPropName = "������������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
					pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("m_hBlockRef");
					if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
					//��������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block";
					lpInfo->m_strPropHelp = "The handle of vest in block��";
#else 
					lpInfo->m_strPropName = "��������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
					pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSLineFlat::GetPropID("m_hBlock");
					if(pFirstFlat->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstFlat->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
		}
		SetPartMutiObjsProp(pFirstFlat,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstFlat,"m_hPartWeldParent");
		SetPartMutiObjsProp(pFirstFlat,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstFlat,"m_bVirtualPart");
		SetPartMutiObjsProp(pFirstFlat,"m_bUserSpecColor");
		if(pFirstFlat->m_bUserSpecColor)
		{	
			if(!SetPartMutiObjsProp(pFirstFlat,"crMaterial"))
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("m_bUserSpecColor"),NULL);
				if(pFindItem)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
					lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Part Color";
					lpInfo->m_strPropHelp = "Part Color";
#else 
					lpInfo->m_strPropName = "������ɫ";
					lpInfo->m_strPropHelp = "������ɫ";
#endif
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSLineFlat::GetPropID("crMaterial");
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(selectObjs,pPropItem->m_idProp);
					if(pFirstFlat->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				}
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSLineFlat::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstFlat,"length");
		SetPartMutiObjsProp(pFirstFlat,"sNotes");
		SetPartMutiObjsProp(pFirstFlat,"nearWorkPlaneNorm.x");
		SetPartMutiObjsProp(pFirstFlat,"nearWorkPlaneNorm.y");
		SetPartMutiObjsProp(pFirstFlat,"nearWorkPlaneNorm.z");
		SetPartMutiObjsProp(pFirstFlat,"hWorkNormRefPart");
		SetPartMutiObjsProp(pFirstFlat,"workPlaneNorm.x");
		SetPartMutiObjsProp(pFirstFlat,"workPlaneNorm.y");
		SetPartMutiObjsProp(pFirstFlat,"workPlaneNorm.z");
		SetPropItemReadOnly(pFirstFlat,"workPlaneNorm.x",pFirstFlat->m_iNormCalStyle==1||
							pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("workPlaneNorm.x")));
		SetPropItemReadOnly(pFirstFlat,"workPlaneNorm.y",pFirstFlat->m_iNormCalStyle==1||
							pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("workPlaneNorm.y")));
		SetPropItemReadOnly(pFirstFlat,"workPlaneNorm.z",pFirstFlat->m_iNormCalStyle==1||
							pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("workPlaneNorm.z")));
		SetPartMutiObjsProp(pFirstFlat,"hStart");
		SetPartMutiObjsProp(pFirstFlat,"oddStart");
		SetPartMutiObjsProp(pFirstFlat,"startPos.x");
		SetPartMutiObjsProp(pFirstFlat,"startPos.y");
		SetPartMutiObjsProp(pFirstFlat,"startPos.z");
		SetPartMutiObjsProp(pFirstFlat,"desStartPos.endPosType");
		SetPropItemReadOnly(pFirstFlat,"hStart",pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
							pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("hStart")));
		SetPropItemReadOnly(pFirstFlat,"oddStart",pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
							pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("oddStart")));
		SetPropItemReadOnly(pFirstFlat,"desStartPos.endPosType",pFirstFlat->IsStartPosLocked()||pFirstFlat->bHuoQuStart||
							pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("desStartPos.endPosType")));
		SetPartMutiObjsProp(pFirstFlat,"desStartPos.datum_pos_style");
		pFindItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("desStartPos.endPosType"),&pParentItem);
		if(pFindItem)
			UpdateLineFlat_DesPos_EndPosType_TreeItem(pPropList,pFindItem,TRUE);
		SetPartMutiObjsProp(pFirstFlat,"desStartPos.hDatumPart");
		SetPartMutiObjsProp(pFirstFlat,"desStartPos.fNormOffset");
		SetPartMutiObjsProp(pFirstFlat,"desStartPos.fEccentricDist");
		SetPartMutiObjsProp(pFirstFlat,"desStartPos.face_offset_norm");
		SetPartMutiObjsProp(pFirstFlat,"desStartPos.bDatumEndStartEnd");
		SetPartMutiObjsProp(pFirstFlat,"desStartPos.bDatumStartStartEnd");
		SetPartMutiObjsProp(pFirstFlat,"desStartPos.hDatumStartPart");
		SetPartMutiObjsProp(pFirstFlat,"desStartPos.hDatumEndPart");
		//�ն���Ϣ
		SetPartMutiObjsProp(pFirstFlat,"hEnd");
		SetPartMutiObjsProp(pFirstFlat,"oddEnd");	//����ͷ
		SetPartMutiObjsProp(pFirstFlat,"endPos.x");	//�ն�����--X
		SetPartMutiObjsProp(pFirstFlat,"endPos.y");	//�ն�����--Y
		SetPartMutiObjsProp(pFirstFlat,"endPos.z");	//�ն�����--Z
		SetPartMutiObjsProp(pFirstFlat,"desEndPos.endPosType");	//�˻���λ��
		SetPropItemReadOnly(pFirstFlat,"hEnd",pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
							pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("hEnd")));
		SetPropItemReadOnly(pFirstFlat,"oddEnd",pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
							pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("oddEnd")));
		SetPropItemReadOnly(pFirstFlat,"desEndPos.endPosType",pFirstFlat->IsEndPosLocked()||pFirstFlat->bHuoQuEnd||
							pFirstFlat->GetObjPropReadOnlyDefaultState(CLDSLineFlat::GetPropID("desEndPos.endPosType")));
		SetPartMutiObjsProp(pFirstFlat,"desEndPos.datum_pos_style");
		pFindItem=pPropList->FindItemByPropId(CLDSLineFlat::GetPropID("desEndPos.endPosType"),&pParentItem);
		if(pFindItem)
			UpdateLineFlat_DesPos_EndPosType_TreeItem(pPropList,pFindItem,FALSE);
		SetPartMutiObjsProp(pFirstFlat,"desEndPos.hDatumPart");
		SetPartMutiObjsProp(pFirstFlat,"desEndPos.fNormOffset");
		SetPartMutiObjsProp(pFirstFlat,"desEndPos.fEccentricDist");
		SetPartMutiObjsProp(pFirstFlat,"desEndPos.face_offset_norm");
		SetPartMutiObjsProp(pFirstFlat,"desEndPos.bDatumEndStartEnd");
		SetPartMutiObjsProp(pFirstFlat,"desEndPos.bDatumStartStartEnd");
		SetPartMutiObjsProp(pFirstFlat,"desEndPos.hDatumStartPart");
		SetPartMutiObjsProp(pFirstFlat,"desEndPos.hDatumEndPart");
	}
	pPropList->Redraw();
}

#endif