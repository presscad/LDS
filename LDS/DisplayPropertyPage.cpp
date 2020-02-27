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
#include "dlg.h"
#include "PartUcsDlg.h"
#include "XhCharString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL UpdatePropItemValue(CPropertyList* pPropList,CLDSObject *pObj, long prop_id,int clstype_id)
{
	CXhChar100 valueText;
	if(clstype_id==CLS_NODE&&((CLDSNode*)pObj)->GetPropValueStr(prop_id,valueText)>0)
		return pPropList->SetItemPropValue(prop_id,valueText);
	else if(clstype_id==CLS_LINEANGLE&&((CLDSLineAngle*)pObj)->GetPropValueStr(prop_id,valueText)>0)
		return pPropList->SetItemPropValue(prop_id,valueText);
	else if(clstype_id==CLS_PLATE&&((CLDSPlate*)pObj)->GetPropValueStr(prop_id,valueText)>0)
		return pPropList->SetItemPropValue(prop_id,valueText);
	return false;
}
BOOL SetPartMutiObjsProp(CLDSObject *pObj, char *propName,long idClassType=0)
{
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pFindItem, *pParentItem;
//#ifdef __LDS_
	//ѡ�ж���˼�ʱ��Ҫ�ж�ѡ�е��ǲ���ͬһ��˼� wht 10-11-22
	//�ڴ˴��ж���Ҫ���ѭ��selectObjs,�����Ա�֤�޸ĵ�λ������ ��ʱ�ڴ˴��ж�
	BOOL bSamePoleType=TRUE;
	if(idClassType==0)
		idClassType=pObj->GetClassTypeId();
	BOOL pushed=pView->selectObjs.push_stack();
	for(CLDSObject *pTempObj=pView->selectObjs.GetFirst();pTempObj;pTempObj=pView->selectObjs.GetNext())
	{
		if(idClassType!=pTempObj->GetClassTypeId())
		{
			bSamePoleType=FALSE;
			break;
		}
	}
	pView->selectObjs.pop_stack(pushed);
//#endif
	long idProp = 0;
	if(idClassType==CLS_NODE)
		idProp = CLDSNode::GetPropID(propName);
	else if(idClassType==CLS_DBOBJECT)
		idProp = CLDSDbObject::GetPropID(propName);
	else if(idClassType==CLS_PART)
		idProp = CLDSPart::GetPropID(propName);
//#ifdef __LDS_
	//LDS��ѡ�и˼������Ͳ�һ��ʱ(����ѧ����ģʽ��)������CLDSLinePart������ wht 10-11-20
	else if(idClassType==CLS_LINEPART||	
		(pObj->IsDbObject()&&((CLDSDbObject*)pObj)->IsPart()&&((CLDSPart*)pObj)->IsLinePart()&&
		(!bSamePoleType||theApp.m_bAnalysisWorkEnv)))
		idProp = CLDSLinePart::GetPropID(propName);	
//#endif
#ifdef __PART_DESIGN_INC_//__LDS_
	else if(idClassType==CLS_FEMELEMENT)
		idProp = CElemInfo::GetPropID(propName);
#endif
#ifndef __TSA_
	else if(idClassType==CLS_LINETUBE)
		idProp = CLDSLineTube::GetPropID(propName);
#ifdef __GGG_
	else if(idClassType==CLS_CONETUBE)
		idProp = CLDSConeTube::GetPropID(propName);
#endif // __GGG_
	else if(idClassType==CLS_LINEANGLE)
		idProp = CLDSLineAngle::GetPropID(propName);
	else if(idClassType==CLS_GROUPLINEANGLE)
		idProp = CLDSGroupLineAngle::GetPropID(propName);
	else if(idClassType==CLS_ARCANGLE)
		idProp = CLDSArcAngle::GetPropID(propName);
	else if(idClassType==CLS_ARCFLAT)
		idProp = CLDSArcFlat::GetPropID(propName);
	else if(idClassType==CLS_ARCSLOT)
		idProp = CLDSArcSlot::GetPropID(propName);
	else if(idClassType==CLS_LINESLOT)
		idProp = CLDSLineSlot::GetPropID(propName);
	else if(idClassType==CLS_BOLT)
		idProp = CLDSBolt::GetPropID(propName);
	else if(idClassType==CLS_PLATE)
		idProp = CLDSPlate::GetPropID(propName);
	else if(idClassType==CLS_PARAMPLATE)
		idProp = CLDSParamPlate::GetPropID(propName);
	else if(idClassType==CLS_SPHERE)
		idProp = CLDSSphere::GetPropID(propName);
	else if(idClassType==CLS_LINEFLAT)
		idProp = CLDSLineFlat::GetPropID(propName);
	else if(idClassType==CLS_BLOCKREF)
		idProp = CBlockReference::GetPropID(propName);
#ifdef __H_SHAPE_STEEL_
	else if(idClassType==CLS_LINEHSHAPE)
		idProp = CLDSLineHShape::GetPropID(propName);
#endif
#endif
	char sText[MAX_TEMP_CHAR_100+1]="";
	int ret=-1; 
	pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
	if(pFindItem)
	{
		if(idClassType==CLS_DBOBJECT)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSDbObject*)pObj)->GetPropValueStr(idProp,sText);
			pFindItem->SetReadOnly(pObj->GetObjPropReadOnlyDefaultState(idProp));
		}
		else if(idClassType==CLS_NODE)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSNode*)pObj)->GetPropValueStr(idProp,sText);
			pFindItem->SetReadOnly(pObj->GetObjPropReadOnlyDefaultState(idProp));
		}
		else if(idClassType==CLS_PART)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSPart*)pObj)->GetPropValueStr(idProp,sText);
			pFindItem->SetReadOnly(pObj->GetObjPropReadOnlyDefaultState(idProp));
		}
//ifdef __LDS_
		//LDS��ѡ�и˼������Ͳ�һ��ʱ(����ѧ����ģʽ��)������CLDSLinePart������ wht 10-11-20
		else if(idClassType==CLS_LINEPART||
		(pObj->IsDbObject()&&((CLDSDbObject*)pObj)->IsPart()&&((CLDSPart*)pObj)->IsLinePart()&&
		(!bSamePoleType||theApp.m_bAnalysisWorkEnv)))
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLinePart::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSLinePart*)pObj)->GetPropValueStr(idProp,sText);
		}
#ifdef __PART_DESIGN_INC_ //__LDS_
		else if(idClassType==CLS_FEMELEMENT)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CElemInfo::IsPropEqual(pView->selectObjs,idProp);
			ret=((CElemInfo*)pObj)->GetPropValueStr(idProp,sText);
		}
#endif
#ifndef __TSA_
		else if(idClassType==CLS_LINETUBE)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSLineTube*)pObj)->GetPropValueStr(idProp,sText);
			pFindItem->SetReadOnly(pObj->GetObjPropReadOnlyDefaultState(idProp));
		}
#ifdef __GGG_
		else if (idClassType==CLS_CONETUBE)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSConeTube*)pObj)->GetPropValueStr(idProp,sText);
		}
#endif
		else if(idClassType==CLS_LINEANGLE)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSLineAngle*)pObj)->GetPropValueStr(idProp,sText);
			pFindItem->SetReadOnly(pObj->GetObjPropReadOnlyDefaultState(idProp));
		}
		else if(idClassType==CLS_GROUPLINEANGLE)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSGroupLineAngle::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSGroupLineAngle*)pObj)->GetPropValueStr(idProp,sText);
			pFindItem->SetReadOnly(pObj->GetObjPropReadOnlyDefaultState(idProp));
		}
		else if(idClassType==CLS_ARCANGLE)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcAngle::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSArcAngle*)pObj)->GetPropValueStr(idProp,sText);
		}
		else if(idClassType==CLS_ARCSLOT)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcSlot::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSArcSlot*)pObj)->GetPropValueStr(idProp,sText);
		}
		else if(idClassType==CLS_ARCFLAT)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSArcFlat::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSArcFlat*)pObj)->GetPropValueStr(idProp,sText);
		}
		else if(idClassType==CLS_LINESLOT)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineSlot::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSLineSlot*)pObj)->GetPropValueStr(idProp,sText);
		}
		else if(idClassType==CLS_BOLT)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSBolt*)pObj)->GetPropValueStr(idProp,sText);
			pFindItem->SetReadOnly(pObj->GetObjPropReadOnlyDefaultState(idProp));
		}
		else if(idClassType==CLS_PLATE)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSPlate*)pObj)->GetPropValueStr(idProp,sText);
			pFindItem->SetReadOnly(pObj->GetObjPropReadOnlyDefaultState(idProp));
		}
		else if(idClassType==CLS_PARAMPLATE)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSParamPlate*)pObj)->GetPropValueStr(idProp,sText);
			pFindItem->SetReadOnly(pObj->GetObjPropReadOnlyDefaultState(idProp));
		}
		else if(idClassType==CLS_SPHERE)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSSphere*)pObj)->GetPropValueStr(idProp,sText);
		}
		else if(idClassType==CLS_LINEFLAT)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineFlat::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSLineFlat*)pObj)->GetPropValueStr(idProp,sText);
			pFindItem->SetReadOnly(pObj->GetObjPropReadOnlyDefaultState(idProp));
		}
		else if(idClassType==CLS_BLOCKREF)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CBlockReference::IsPropEqual(pView->selectObjs,idProp);
			ret=((CBlockReference*)pObj)->GetPropValueStr(idProp,sText);
		}
#ifdef __H_SHAPE_STEEL_
		else if(idClassType==CLS_LINEHSHAPE)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineHShape::IsPropEqual(pView->selectObjs,idProp);
			ret=((CLDSLineHShape*)pObj)->GetPropValueStr(idProp,sText);
			pFindItem->SetReadOnly(pObj->GetObjPropReadOnlyDefaultState(idProp));
		}
#endif
#endif
		//ret==0 1.δ�ҵ���Ӧ���� 2.�ҵ���Ӧ���Ե�������ֵΪ�ַ�����Ϊ��
		if(ret>0)
			pFindItem->m_lpNodeInfo->m_strPropValue=sText;
		else if(ret==0&&(strcmp(propName,"sNotes")==0||strcmp(propName,"sPartNo")==0))
			pFindItem->m_lpNodeInfo->m_strPropValue="";
		pPropList->SetItemPropValue(idProp,CString(sText));
		return TRUE;
	}
	else 
		return FALSE;
}

void SetPropItemReadOnly(CLDSDbObject *pObj,char *propName,BOOL bReadOnly)
{
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pFindItem, *pParentItem;
	long idProp = 0;
	if(pObj->GetClassTypeId()==CLS_NODE)
		idProp = CLDSNode::GetPropID(propName);
	else if(pObj->GetClassTypeId()==CLS_LINEPART)
		idProp = CLDSLinePart::GetPropID(propName);
#ifndef __TSA_
	else if(pObj->GetClassTypeId()==CLS_LINETUBE)
		idProp = CLDSLineTube::GetPropID(propName);
#ifdef __GGG_
	else if(pObj->GetClassTypeId()==CLS_CONETUBE)
		idProp = CLDSConeTube::GetPropID(propName);
#endif
	else if(pObj->GetClassTypeId()==CLS_LINEANGLE)
		idProp = CLDSLineAngle::GetPropID(propName);
	else if(pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
		idProp = CLDSGroupLineAngle::GetPropID(propName);
	else if(pObj->GetClassTypeId()==CLS_LINESLOT)
		idProp = CLDSLineSlot::GetPropID(propName);
	else if(pObj->GetClassTypeId()==CLS_BOLT)
		idProp = CLDSBolt::GetPropID(propName);
	else if(pObj->GetClassTypeId()==CLS_PLATE)
		idProp = CLDSPlate::GetPropID(propName);
	else if(pObj->GetClassTypeId()==CLS_PARAMPLATE)
		idProp = CLDSParamPlate::GetPropID(propName);
	else if(pObj->GetClassTypeId()==CLS_SPHERE)
		idProp = CLDSSphere::GetPropID(propName);
	else if(pObj->GetClassTypeId()==CLS_LINEFLAT)
		idProp = CLDSLineFlat::GetPropID(propName);
	else if(pObj->GetClassTypeId()==CLS_ARCANGLE)
		idProp = CLDSArcAngle::GetPropID(propName);
	else if(pObj->GetClassTypeId()==CLS_ARCFLAT)
		idProp = CLDSArcFlat::GetPropID(propName);
	else if(pObj->GetClassTypeId()==CLS_ARCSLOT)
		idProp = CLDSArcSlot::GetPropID(propName);
#endif
	pFindItem = pPropList->FindItemByPropId(idProp,&pParentItem);
	if(pFindItem)
		pPropList->SetItemReadOnly(pFindItem->m_idProp,bReadOnly|pObj->GetObjPropReadOnlyDefaultState(idProp));
}

BOOL ModifyDbObjectItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSDbObject::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL ModifyPartItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSPart::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL ModifyPolyJointItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSPolyJoint::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL ModifySphereItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSSphere::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
#endif

BOOL DbObjectButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSDbObject *pFirstDbObject=NULL, *pDbObject=NULL;
	CCfgPartNoDlg cfgdlg;

	if(pView->selectObjs.GetNodeNum()>0)
		pFirstDbObject = (CLDSDbObject*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	if(pItem->m_idProp==CLDSDbObject::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstDbObject->cfgword; 
		if(pFirstDbObject->IsBodyObj())
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
			pFirstDbObject->cfgword = cfgdlg.cfgword;	//�ڴ˴����µ�һ���ڵ������Ϻ�
		char sText[MAX_TEMP_CHAR_100+1]="";
		if(pFirstDbObject->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else
		return FALSE;
	return TRUE;
}
#ifndef __TSA_
BOOL PartButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPart *pFirstPart=NULL, *pPart=NULL;
	CCfgPartNoDlg cfgdlg;

	if(pView->selectObjs.GetNodeNum()>0)
		pFirstPart = (CLDSPart*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	if(pItem->m_idProp==CLDSPart::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstPart->cfgword; 
		if(pFirstPart->IsBodyObj())
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
			pFirstPart->cfgword = cfgdlg.cfgword;		//�ڴ˴����µ�һ���ڵ������Ϻ�
		char sText[MAX_TEMP_CHAR_100+1]="";
		if(pFirstPart->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else
		return FALSE;
	return TRUE;
}
#endif

#ifndef __TSA_
BOOL SphereButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem, *pParentItem;
	CLDSSphere *pFirstSphere=NULL, *pSphere=NULL;
	CLDSPart *pPart = NULL;
	CReportVectorDlg vecDlg;
	CCfgPartNoDlg cfgdlg;
	CString cmdStr,valueStr = _T("");

	if(pView->selectObjs.GetNodeNum()>0)
		pFirstSphere = (CLDSSphere*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	if(pItem->m_idProp==CLDSSphere::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstSphere->cfgword; 
		if(pFirstSphere->IsBodyObj())
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
			pFirstSphere->cfgword = cfgdlg.cfgword;		//�ڴ˴����µ�һ���ڵ������Ϻ�
		char sText[MAX_TEMP_CHAR_100+1]="";
		if(pFirstSphere->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs"))
	{
		CPartUcsDlg part_ucs_dlg;
		part_ucs_dlg.ucs=pFirstSphere->ucs;
		if(part_ucs_dlg.DoModal()==IDOK)
		{
			for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
				pSphere->ucs=part_ucs_dlg.ucs;
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.origin"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Origin Posion";
#else 
		vecDlg.m_sCaption = "װ������ϵԭ��λ��";
#endif
		vecDlg.m_fVectorX = pFirstSphere->ucs.origin.x;
		vecDlg.m_fVectorY = pFirstSphere->ucs.origin.y;
		vecDlg.m_fVectorZ = pFirstSphere->ucs.origin.z;
		pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("ucs.origin.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("ucs.origin.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("ucs.origin.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.origin.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.origin.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.origin.z"),CString(sText));
			for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
				pSphere->ucs.origin.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.axis_x"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "X Axis Direction";
#else 
		vecDlg.m_sCaption = "װ��ʱX�᷽��";
#endif
		vecDlg.m_fVectorX = pFirstSphere->ucs.axis_x.x;
		vecDlg.m_fVectorY = pFirstSphere->ucs.axis_x.y;
		vecDlg.m_fVectorZ = pFirstSphere->ucs.axis_x.z;
		pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("ucs.axis_x.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("ucs.axis_x.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("ucs.axis_x.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.axis_x.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.axis_x.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.axis_x.z"),CString(sText));
			for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
			{
				pSphere->ucs.axis_x.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				rotate_ucs_axis(pSphere->ucs,pSphere->ucs.origin+pSphere->ucs.axis_x*1,0);
			}
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.axis_y"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Y Axis Direction";
#else 
		vecDlg.m_sCaption = "װ��ʱY�᷽��";
#endif
		vecDlg.m_fVectorX = pFirstSphere->ucs.axis_y.x;
		vecDlg.m_fVectorY = pFirstSphere->ucs.axis_y.y;
		vecDlg.m_fVectorZ = pFirstSphere->ucs.axis_y.z;
		pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("ucs.axis_y.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("ucs.axis_y.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("ucs.axis_y.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.axis_y.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.axis_y.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.axis_y.z"),CString(sText));
			for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
			{
				pSphere->ucs.axis_y.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				rotate_ucs_axis(pSphere->ucs,pSphere->ucs.origin+pSphere->ucs.axis_y*1,1);
			}
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.axis_z"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Z Axis Direction";
#else 
		vecDlg.m_sCaption = "װ��ʱZ�᷽��";
#endif
		vecDlg.m_fVectorX = pFirstSphere->ucs.axis_z.x;
		vecDlg.m_fVectorY = pFirstSphere->ucs.axis_z.y;
		vecDlg.m_fVectorZ = pFirstSphere->ucs.axis_z.z;
		pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("ucs.axis_z.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("ucs.axis_z.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("ucs.axis_z.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.axis_z.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.axis_z.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.axis_z.z"),CString(sText));
			for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
			{
				pSphere->ucs.axis_z.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				rotate_ucs_axis(pSphere->ucs,pSphere->ucs.origin+pSphere->ucs.axis_z*1,2);
			}
		}
	}
	else 
		return FALSE;
	for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
	{
		pSphere->SetModified();
		pSphere->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pSphere->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}
BOOL FireSpherePopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	if(pItem->m_idProp==CLDSSphere::GetPropID("sphereLinkPoles"))
	{
		if(0==iMenu)		//�鿴���Ӹ˼���
		{	//��ʾ����Ĺ���
			;
		}
		else if(1==iMenu)	//�������Ӹ˼���
		{
			;
		}
	}
	return TRUE;
}
#endif
CString GetBlockListStr();	//defined in DisplayNodeProperty.cpp
long RetrieveBlockHandle(const char* blockIdName);
BOOL ModifyDbObjectProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSDbObject *pFirstDbObject = (CLDSDbObject*)pView->selectObjs.GetFirst();
	CLDSDbObject *pDbObject = NULL;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	if(pItem->m_idProp==CLDSDbObject::GetPropID("layer"))
	{
		for(pDbObject=(CLDSDbObject*)pView->selectObjs.GetFirst();pDbObject;pDbObject=(CLDSDbObject*)pView->selectObjs.GetNext())
			pDbObject->SetLayer(tem_str);
	}
	else if(pItem->m_idProp==CLDSDbObject::GetPropID("layer1"))
	{
		for(pDbObject=(CLDSDbObject*)pView->selectObjs.GetFirst();pDbObject;pDbObject=(CLDSDbObject*)pView->selectObjs.GetNext())
			pDbObject->layer(0)=tem_str[0];
	}
	else if(pItem->m_idProp==CLDSDbObject::GetPropID("layer2"))
	{
		for(pDbObject=(CLDSDbObject*)pView->selectObjs.GetFirst();pDbObject;pDbObject=(CLDSDbObject*)pView->selectObjs.GetNext())
			pDbObject->layer(1)=tem_str[0];
	}
	else if(pItem->m_idProp==CLDSDbObject::GetPropID("layer3"))
	{
		for(pDbObject=(CLDSDbObject*)pView->selectObjs.GetFirst();pDbObject;pDbObject=(CLDSDbObject*)pView->selectObjs.GetNext())
			pDbObject->layer(2)=tem_str[0];
	}
	else if(pItem->m_idProp==CLDSDbObject::GetPropID("iSeg"))
	{
		SEGI iSeg = SEGI(valueStr.GetBuffer());
		for(pDbObject=(CLDSDbObject*)pView->selectObjs.GetFirst();pDbObject;pDbObject=(CLDSDbObject*)pView->selectObjs.GetNext())
			pDbObject->iSeg = iSeg;
	}
	else if(pItem->m_idProp==CLDSDbObject::GetPropID("cfgword"))
	{
		//���������˼�����ĺ�
		for(pDbObject=(CLDSDbObject*)pView->selectObjs.GetFirst();pDbObject;pDbObject=(CLDSDbObject*)pView->selectObjs.GetNext())
			pDbObject->cfgword = pFirstDbObject->cfgword;
	}
	else if(pItem->m_idProp==CLDSDbObject::GetPropID("m_hBlock"))
	{
		long hBlock=RetrieveBlockHandle(valueStr);
		for(pDbObject=(CLDSDbObject*)pView->selectObjs.GetFirst();pDbObject;pDbObject=(CLDSDbObject*)pView->selectObjs.GetNext())
			pDbObject->m_hBlock=hBlock;
	}
	else if(pItem->m_idProp==CLDSDbObject::GetPropID("m_hArcLift"))
	{
		CLDSArcLift *pLift=NULL;
		CLDSDbObject *pDbObject = (CLDSDbObject*)pView->selectObjs.GetFirst();
		CXhChar500 sText;
		for(pLift=Ta.ArcLift.GetFirst();pLift;pLift=Ta.ArcLift.GetNext())
		{
			CString sLiftName;
			sLiftName.Format("%s(%d)",pLift->name,pLift->iNo);
			if(sLiftName.CompareNoCase(valueStr)==0)
				break;
		}
		for(pDbObject=(CLDSDbObject*)pView->selectObjs.GetFirst();pDbObject;pDbObject=(CLDSDbObject*)pView->selectObjs.GetNext())
		{
			if(pLift)
				pDbObject->SetArcLift(pLift->handle);
			else 
				pDbObject->SetArcLift(0);
			pDbObject->CalPosition(true);
			pDbObject->UpdateRelaObjArcLiftProp();
		}
	}
	else if(pItem->m_idProp==CLDSDbObject::GetPropID("sNotes"))
	{
		for(pDbObject=(CLDSDbObject*)pView->selectObjs.GetFirst();pDbObject;pDbObject=(CLDSDbObject*)pView->selectObjs.GetNext())
			_snprintf(pDbObject->sNotes,MAX_TEMP_CHAR_50,"%s", valueStr);
	}
	else
		return FALSE;
	return TRUE;
}
#ifndef __TSA_
BOOL ModifyPartProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPart *pFirstPart = (CLDSPart*)pView->selectObjs.GetFirst();
	CLDSPart *pPart = NULL;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	if(pItem->m_idProp==CLDSPart::GetPropID("layer"))
	{
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
			pPart->SetLayer(tem_str);	
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("cMaterial"))
	{	//����
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
			pPart->cMaterial = QuerySteelBriefMatMark(tem_str);
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("m_cQualityLevel"))
	{	//�����ȼ�
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
			pPart->m_cQualityLevel = valueStr[0];
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("iSeg"))
	{
		SEGI iSeg = SEGI(valueStr.GetBuffer());
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
			pPart->iSeg = iSeg;
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("m_hBlock"))
	{
		long hBlock=RetrieveBlockHandle(valueStr);
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
			pPart->m_hBlock=hBlock;
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("sPartNo"))
	{
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
			pPart->SetPartNo(tem_str);
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("cfgword"))
	{	//���������˼�����ĺ�
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
			pPart->cfgword = pFirstPart->cfgword;
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("m_uStatMatCoef"))
	{
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
			pPart->m_uStatMatCoef = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("m_hPartWeldParent"))
	{
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
			pPart->m_hPartWeldParent=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("m_bDisplayHole"))
	{
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
#endif
			{
				if(!pPart->m_bDisplayHole)
					pPart->SetModified();
				pPart->m_bDisplayHole = TRUE;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(valueStr.CompareNoCase("No")==0)
#else 
			else if(valueStr.CompareNoCase("��")==0)
#endif
			{
				if(pPart->m_bDisplayHole)
					pPart->SetModified();
				pPart->m_bDisplayHole = FALSE;
			}
			pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
		}
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("m_bVirtualPart"))
	{
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pPart->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pPart->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pPart->m_bVirtualPart = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("m_bUserSpecColor"))
	{
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pPart->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pPart->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pPart->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pPart->relativeObjs.GetFirst();pMirObj;pMirObj=pPart->relativeObjs.GetNext())
			{
				CLDSPart *pMirPart=pPart->BelongModel()->FromPartHandle(pMirObj->hObj);
				if(pMirPart)
					pMirPart->m_bUserSpecColor=pPart->m_bUserSpecColor;
			}
		}
		if(pFirstPart->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPart::GetPropID("crMaterial"),NULL);
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
				pFindItem->m_idProp = CLDSPart::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if(pFirstPart->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSPart::GetPropID("crMaterial"));
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
		{
			pPart->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pPart->relativeObjs.GetFirst();pMirObj;pMirObj=pPart->relativeObjs.GetNext())
			{
				CLDSPart *pMirPart=pPart->BelongModel()->FromPartHandle(pMirObj->hObj);
				if(pMirPart)
					pMirPart->crMaterial=pPart->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSPart::GetPropID("sNotes"))
	{
		for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
			_snprintf(pPart->sNotes,MAX_TEMP_CHAR_50,"%s", valueStr);
	}
	else
		return FALSE;
	for(pPart=(CLDSPart*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPart*)pView->selectObjs.GetNext())
	{
		pPart->SetModified();
		pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL PolyJointButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPolyJoint *pFirstPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst(),*pPart=NULL;
	CCfgPartNoDlg cfgdlg;
	CString valueStr = _T("");
	if(pFirstPart!=NULL&&pItem->m_idProp==CLDSPolyJoint::GetPropID("cfgword"))
	{
		if(pFirstPart->IsBodyObj())
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		cfgdlg.cfgword=pFirstPart->cfgword;
		if(cfgdlg.DoModal()==IDOK)
		{
			for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
				pPart->cfgword = cfgdlg.cfgword;		//�ڴ˴����µ�һ���ڵ������Ϻ�
			char sText[MAX_TEMP_CHAR_100+1]="";
			if(pFirstPart->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
				pItem->m_lpNodeInfo->m_strPropValue=sText;
		}
		else
			return FALSE;
	}
	return TRUE; 
}
BOOL ModifyPolyJointProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPolyJoint *pFirstPart = (CLDSPolyJoint*)pView->selectObjs.GetFirst();
	CLDSPolyJoint *pPart = NULL;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	if(pItem->m_idProp==CLDSPolyJoint::GetPropID("layer"))
	{
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
			pPart->SetLayer(tem_str);	
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("cMaterial"))
	{	//����
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
			pPart->cMaterial = QuerySteelBriefMatMark(tem_str);
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("iSeg"))
	{
		SEGI iSeg = SEGI(valueStr.GetBuffer());
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
			pPart->iSeg = iSeg;
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("sPartNo"))
	{
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
			pPart->SetPartNo(tem_str);
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("cfgword"))
	{	//���������˼�����ĺ�
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
			pPart->cfgword = pFirstPart->cfgword;
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("m_uStatMatCoef"))
	{
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
			pPart->m_uStatMatCoef = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("m_hPartWeldParent"))
	{
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
			pPart->m_hPartWeldParent=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("m_bDisplayHole"))
	{
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
#endif
			{
				if(!pPart->m_bDisplayHole)
					pPart->SetModified();
				pPart->m_bDisplayHole = TRUE;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(valueStr.CompareNoCase("No")==0)
#else 
			else if(valueStr.CompareNoCase("��")==0)
#endif
			{
				if(pPart->m_bDisplayHole)
					pPart->SetModified();
				pPart->m_bDisplayHole = FALSE;
			}
			pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
		}
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("m_bVirtualPart"))
	{
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pPart->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pPart->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pPart->m_bVirtualPart = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("pPrevLinePart"))
	{
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
		{
			long hPrevLinePart=0;
			sscanf(tem_str,"%X",&hPrevLinePart);
			CLDSLinePart *pLinePart=NULL;
			pLinePart=(CLDSLinePart*)console.FromPartHandle(hPrevLinePart,CLS_LINEPART);
			if(pLinePart&&pLinePart->IsLinePart())
				pPart->pPrevLinePart=pLinePart;
		}
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("pNextLinePart"))
	{
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
		{
			long hNextLinePart=0;
			sscanf(tem_str,"%X",&hNextLinePart);
			CLDSLinePart *pLinePart=NULL;
			pLinePart=(CLDSLinePart*)console.FromPartHandle(hNextLinePart,CLS_LINEPART);
			if(pLinePart&&pLinePart->IsLinePart())
				pPart->pNextLinePart=pLinePart;
		}
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("rBending"))
	{
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
		{
			pPart->rBending=atof(tem_str);
			if(pPart->pPrevLinePart&&pPart->pPrevLinePart->GetClassTypeId()==CLS_LINEANGLE)
				((CLDSLineAngle*)pPart->pPrevLinePart)->huoqu_r=pPart->rBending;
		}
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("huoqu_base_wing_x0_y1_both2"))
	{
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
		{
			int huoqu_base_wing_x0_y1_both2=tem_str[0]-'0';
			if(pPart->pPrevLinePart&&pPart->pPrevLinePart->GetClassTypeId()==CLS_LINEANGLE)
				((CLDSLineAngle*)pPart->pPrevLinePart)->huoqu_base_wing_x0_y1_both2=huoqu_base_wing_x0_y1_both2;
		}
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("m_bUserSpecColor"))
	{
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pPart->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pPart->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pPart->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pPart->relativeObjs.GetFirst();pMirObj;pMirObj=pPart->relativeObjs.GetNext())
			{
				CLDSPolyJoint *pMirPart=(CLDSPolyJoint*)pPart->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_POLYJOINT);
				if(pMirPart)
					pMirPart->m_bUserSpecColor=pPart->m_bUserSpecColor;
			}
		}
		if(pFirstPart->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPolyJoint::GetPropID("crMaterial"),NULL);
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
				pFindItem->m_idProp = CLDSPolyJoint::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if(pFirstPart->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSPolyJoint::GetPropID("crMaterial"));
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
		{
			pPart->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pPart->relativeObjs.GetFirst();pMirObj;pMirObj=pPart->relativeObjs.GetNext())
			{
				CLDSPolyJoint *pMirPart=(CLDSPolyJoint*)pPart->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_POLYJOINT);
				if(pMirPart)
					pMirPart->crMaterial=pPart->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSPolyJoint::GetPropID("sNotes"))
	{
		for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
			_snprintf(pPart->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
	}
	else
		return FALSE;
	for(pPart=(CLDSPolyJoint*)pView->selectObjs.GetFirst();pPart;pPart=(CLDSPolyJoint*)pView->selectObjs.GetNext())
	{
		pPart->SetModified();
		pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}
#endif

#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
void UpdateWireLoadInfoItem(CPropertyList *pPropList,CPropTreeItem* pItem,int update0_insert1)
{
	/*CItemInfo *lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	CPropTreeItem *pPropItem,*pSonPropItem,*pParentItem;
	const int GROUP_GENERAL=1,GROUP_DESIGN=2,GROUP_WIRELOAD=3;
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSNode *pFirstNode = (CLDSNode*)pView->selectObjs.GetFirst();
	pPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("wireInfo"),&pParentItem);
	if(pPropItem==NULL)
	{
		//������Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "������Ϣ";
		lpInfo->m_strPropHelp = "������Ϣ"; 
		lpInfo->m_cmbItems=MakeWireTypeSetString();
		pPropItem = pPropList->InsertItem(pItem,lpInfo, -1,update0_insert1==0);
	}
	pPropItem->m_idProp = CLDSNode::GetPropID("wireInfo");
	if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_bHideChildren=TRUE;
	if(CLDSNode::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSNode::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	
	//��������
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("wireType"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��������";
		lpInfo->m_strPropHelp = "��������"; 
		lpInfo->m_cmbItems=MakeWireTypeSetString();
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("wireType");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//����ͺ�����
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("wireSpec"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "����ͺ�";
		lpInfo->m_strPropHelp = "����ͺ�"; 
		lpInfo->m_cmbItems=MakeWireSetString(pFirstNode->wireInfo.wire.specWireType);
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("wireSpec");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//�����⾶
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("wireDiameter"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�����⾶(mm)";
		lpInfo->m_strPropHelp = "�����⾶(mm)��Ӱ����������ؼ��������ؼ���"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("wireDiameter");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//���߽����
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("wireArea"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�����(mm2)";
		lpInfo->m_strPropHelp = "���߽����(mm2)"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("wireArea");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//���ߵ�λ������,kg/km
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("unitWeight"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��λ����(kg/km)";
		lpInfo->m_strPropHelp = "���ߵ�λ������,kg/km"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("unitWeight");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//���߶������,N
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("forcePermit"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�������(N)";
		lpInfo->m_strPropHelp = "�������(N)���Ѽ�������ϵ��"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("forcePermit");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//��ȫϵ��
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("saftyCoef"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��ȫϵ��";
		lpInfo->m_strPropHelp = "��ȫϵ��"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("saftyCoef");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//�������ʹ������,N
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("maxTension"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "���ʹ������(N)";
		lpInfo->m_strPropHelp = "���ʹ������(N)=�������ֵ/��ȫϵ��"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("maxTension");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//���ߵ���ϵ��,MPa
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("elasticityCoef"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "����ϵ��(MPa)";
		lpInfo->m_strPropHelp = "���ߵ���ϵ��,MPa"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("elasticityCoef");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//�����¶�������ϵ��
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("expansionCoef"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�¶�������ϵ��";
		lpInfo->m_strPropHelp = "�����¶�������ϵ��,�¶�����1��Cʱ����λ��(1m)���ߵ��쳤��(m)"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("expansionCoef");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//�ҽӵ���ʱ��һ�ർ�ߵķ�����
	pPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("splitNumber"),&pParentItem);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "������";
		lpInfo->m_strPropHelp = "�ҽӵ���ʱ��һ�ർ�ߵķ�����"; 
		pPropItem = pPropList->InsertItem(pItem,lpInfo, -1,update0_insert1==0);
	}
	pPropItem->m_idProp = CLDSNode::GetPropID("splitNumber");
	if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//��ѹ�ȼ�
	pPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("voltage"),&pParentItem);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��ѹ�ȼ�(kv)";
		lpInfo->m_strPropHelp = "�����ҽӵ��ߵĵ�ѹ�ȼ�(kv)"; 
		pPropItem = pPropList->InsertItem(pItem,lpInfo, -1,update0_insert1==0);
	}
	pPropItem->m_idProp = CLDSNode::GetPropID("voltage");
	if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//���ߵ���ظ�(m)
	pPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("landHigh"),&pParentItem);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "���ߵ���ظ�(m)";
		lpInfo->m_strPropHelp = "���ߵ���ظ�(m)"; 
		pPropItem = pPropList->InsertItem(pItem,lpInfo, -1,update0_insert1==0);
	}
	pPropItem->m_idProp = CLDSNode::GetPropID("landHigh");
	if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	
	//�ҽӷ�ʽ
	pPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("hangingStyle"),&pParentItem);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�ҽӷ�ʽ";
		lpInfo->m_strPropHelp = "�����ߵĹҽӷ�ʽ"; 
		lpInfo->m_cmbItems = "���Ź���|��������";
		pPropItem = pPropList->InsertItem(pItem,lpInfo, -1,update0_insert1==0);
	}
	pPropItem->m_idProp = CLDSNode::GetPropID("hangingStyle");
	if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_bHideChildren=FALSE;
	if(CLDSNode::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSNode::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//ˮƽ����
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("horiSpace"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ˮƽ����";
		lpInfo->m_strPropHelp = "ˮƽ����(m)"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("horiSpace");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//��ֱ����
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("vertSpace"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��ֱ����";
		lpInfo->m_strPropHelp = "��ֱ����(m)"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("vertSpace");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//��·ת��
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("xianluAngle"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��·ת��";
		lpInfo->m_strPropHelp = "��·ת��(��)"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("xianluAngle");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	if(pFirstNode->wireInfo.m_bNaiZhang)
	{
		//������
		pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("representSpace"),&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "������";
			lpInfo->m_strPropHelp = "������(m)"; 
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
		}
		pSonPropItem->m_idProp = CLDSNode::GetPropID("representSpace");
		if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
		//�߲�ϵ��
		pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("HtoL"),&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "�߲�ϵ��";
			lpInfo->m_strPropHelp = "�߲�ϵ��"; 
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
		}
		pSonPropItem->m_idProp = CLDSNode::GetPropID("HtoL");
		if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
		//ˮƽ�������ϵ��
		pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("allocHoriCoef"),&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "ˮƽ�������ϵ��";
			lpInfo->m_strPropHelp = "ˮƽ�������ϵ��"; 
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
		}
		pSonPropItem->m_idProp = CLDSNode::GetPropID("allocHoriCoef");
		if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
		//��ֱ�������ϵ��
		pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("allocVertCoef"),&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "��ֱ�������ϵ��";
			lpInfo->m_strPropHelp = "��ֱ�������ϵ��"; 
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
		}
		pSonPropItem->m_idProp = CLDSNode::GetPropID("allocVertCoef");
		if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	}
	//��������ϸ
	pPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("attachmentSpec"),&pParentItem);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��������ϸ";
		lpInfo->m_strPropHelp = "��߼���Ե�ӵȸ�������ϸ�������������� ',' ����"; 
		pPropItem = pPropList->InsertItem(pItem,lpInfo, -1,update0_insert1==0);
	}
	pPropItem->m_idProp = CLDSNode::GetPropID("attachmentSpec");
	pPropItem->m_bHideChildren=TRUE;
	if(CLDSNode::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSNode::GetPropStatus(pPropItem->m_idProp);
	if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//����������
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("attachmentWeight"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "����������(kg)";
		lpInfo->m_strPropHelp = "��߼���Ե�ӵȸ���������(kg)"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("attachmentWeight");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//�������������
	pSonPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("attachmentWindShieldArea"),&pParentItem);
	if(pSonPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�������������(m2)";
		lpInfo->m_strPropHelp = "��߼���Ե�ӵȸ������������(m2)"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,update0_insert1==0);
	}
	pSonPropItem->m_idProp = CLDSNode::GetPropID("attachmentWindShieldArea");
	if(pFirstNode->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//���߰�װ��Ϣ
	CPropTreeItem* pWireSetupGroupItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("setupWireInfo"),&pParentItem);
	if(pWireSetupGroupItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "���߰�װ��Ϣ";
		lpInfo->m_strPropHelp = "���߰�װ��Ϣ";
		pWireSetupGroupItem = pPropList->InsertItem(pItem,lpInfo, -1,update0_insert1==0);
	}
	pWireSetupGroupItem->m_idProp = CLDSNode::GetPropID("setupWireInfo");
	if(CLDSNode::GetPropStatus(pWireSetupGroupItem->m_idProp)!=-1)
		pWireSetupGroupItem->m_bHideChildren = CLDSNode::GetPropStatus(pWireSetupGroupItem->m_idProp);
	pWireSetupGroupItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//ê�߶Եؼн�
	pPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("anchorLineAngle"),&pParentItem);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ê�߶Եؼн�";
		lpInfo->m_strPropHelp = "ê�߶Եؼн�(��)"; 
		pPropItem = pPropList->InsertItem(pWireSetupGroupItem,lpInfo, -1,update0_insert1==0);
	}
	pPropItem->m_idProp = CLDSNode::GetPropID("anchorLineAngle");
	if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	//ǣ���߶Եؼн�
	pPropItem=pPropList->FindItemByPropId(CLDSNode::GetPropID("tractionLineAngle"),&pParentItem);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ǣ���߶Եؼн�";
		lpInfo->m_strPropHelp = "ǣ���߶Եؼн�(��)"; 
		pPropItem = pPropList->InsertItem(pWireSetupGroupItem,lpInfo, -1,update0_insert1==0);
	}
	pPropItem->m_idProp = CLDSNode::GetPropID("tractionLineAngle");
	if(pFirstNode->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSNode::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_WIRELOAD);
	*/
}
#endif

#ifndef __TSA_
BOOL ModifySphereProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSSphere *pFirstSphere = (CLDSSphere*)pView->selectObjs.GetFirst();
	CLDSSphere *pSphere = NULL;
	char tem_str[MAX_TEMP_CHAR_100+1];
	if(pView->selectObjs.GetNodeNum()<0)
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
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	const int GROUP_GENERAL=1,GROUP_POS=2;
	if(pItem->m_idProp==CLDSSphere::GetPropID("layer"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->SetLayer(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pSphere->relativeObjs.GetFirst();pMirObj;pMirObj=pSphere->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//��ת�Գ�
				CLDSSphere *pMirSphere=(CLDSSphere*)pSphere->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SPHERE);
				if(pMirSphere)
					CalMirAtomLayer(pSphere->layer(),pMirSphere->layer(),pMirObj->mirInfo);
			}
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("cMaterial"))
	{	//�������
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->cMaterial = QuerySteelBriefMatMark(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pSphere->relativeObjs.GetFirst();pMirObj;pMirObj=pSphere->relativeObjs.GetNext())
			{
				CLDSSphere *pMirSphere=(CLDSSphere*)pSphere->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SPHERE);
				if(pMirSphere)
					pMirSphere->cMaterial=pSphere->cMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("m_cQualityLevel"))
	{	//���������ȼ�
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->m_cQualityLevel = valueStr[0];
			for(RELATIVE_OBJECT *pMirObj=pSphere->relativeObjs.GetFirst();pMirObj;pMirObj=pSphere->relativeObjs.GetNext())
			{
				CLDSSphere *pMirSphere=(CLDSSphere*)pSphere->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SPHERE);
				if(pMirSphere)
					pMirSphere->m_cQualityLevel=pSphere->m_cQualityLevel;
			}
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("specification"))
	{	//������
		CString ss;
		double D,t;
		restore_JG_guige(tem_str,D,t);
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->D = D;
			pSphere->d = D-2*t;
			pSphere->thick=t;
			for(RELATIVE_OBJECT *pMirObj=pSphere->relativeObjs.GetFirst();pMirObj;pMirObj=pSphere->relativeObjs.GetNext())
			{
				CLDSSphere *pMirSphere=(CLDSSphere*)pSphere->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SPHERE);
				if(pMirSphere)
				{
					pMirSphere->D=D;
					pMirSphere->d=D-2*t;
					pMirSphere->thick=t;
				}
			}
		}
		ss.Format("%.f",pFirstSphere->D);
		pPropList->SetItemPropValue(CLDSSphere::GetPropID("D"),ss);
		ss.Format("%.f",pFirstSphere->d);
		pPropList->SetItemPropValue(CLDSSphere::GetPropID("d"),ss);
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("D"))
	{	//�����⾶
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->D=(atof(tem_str));
			pSphere->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pSphere->GetSolidPartObject());
			for(RELATIVE_OBJECT *pMirObj=pSphere->relativeObjs.GetFirst();pMirObj;pMirObj=pSphere->relativeObjs.GetNext())
			{
				CLDSSphere *pMirSphere=(CLDSSphere*)pSphere->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SPHERE);
				if(pMirSphere)
					pMirSphere->D=pSphere->D;
			}
		}
		CString ss;
		ss.Format("%.f",(pFirstSphere->D-2*pFirstSphere->thick));
		pPropList->SetItemPropValue(CLDSSphere::GetPropID("d"),ss);
		ss.Format("%.fX%.f",pFirstSphere->D,pFirstSphere->thick);
		pPropList->SetItemPropValue(CLDSSphere::GetPropID("specification"),ss);
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("d"))
	{	//�����ھ�
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->d=(atof(tem_str));
			pSphere->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pSphere->GetSolidPartObject());
			for(RELATIVE_OBJECT *pMirObj=pSphere->relativeObjs.GetFirst();pMirObj;pMirObj=pSphere->relativeObjs.GetNext())
			{
				CLDSSphere *pMirSphere=(CLDSSphere*)pSphere->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SPHERE);
				if(pMirSphere)
					pMirSphere->d=pSphere->d;
			}
		}
		pFirstSphere->thick=0.5*(pFirstSphere->D-pFirstSphere->d);
		CString ss;
		ss.Format("%.fX%.f",pFirstSphere->D,pFirstSphere->thick);
		pPropList->SetItemPropValue(CLDSSphere::GetPropID("specification"),ss);
		g_pSolidDraw->Draw();
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("iSeg"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->iSeg = SEGI(valueStr.GetBuffer());
			for(RELATIVE_OBJECT *pMirObj=pSphere->relativeObjs.GetFirst();pMirObj;pMirObj=pSphere->relativeObjs.GetNext())
			{
				CLDSSphere *pMirSphere=(CLDSSphere*)pSphere->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SPHERE);
				if(pMirSphere)
					pMirSphere->iSeg=pSphere->iSeg;
			}
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("sPartNo"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
			pSphere->SetPartNo(tem_str);
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("cfgword"))
	{	//�ø��¹��ĵ�һ�������е���ĺŸ�����������������Ϻ�
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->cfgword = pFirstSphere->cfgword;
			for(RELATIVE_OBJECT *pMirObj=pSphere->relativeObjs.GetFirst();pMirObj;pMirObj=pSphere->relativeObjs.GetNext())
			{
				CLDSSphere *pMirSphere=(CLDSSphere*)pSphere->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SPHERE);
				if(pMirSphere)
					pMirSphere->cfgword=pSphere->cfgword;
			}
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("m_uStatMatCoef"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
			pSphere->m_uStatMatCoef = atoi(tem_str);
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("m_hPartWeldParent"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
			pSphere->m_hPartWeldParent=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("m_bDisplayHole"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pSphere->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pSphere->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pSphere->m_bDisplayHole = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pSphere->relativeObjs.GetFirst();pMirObj;pMirObj=pSphere->relativeObjs.GetNext())
			{
				CLDSSphere *pMirSphere=(CLDSSphere*)pSphere->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SPHERE);
				if(pMirSphere)
					pMirSphere->m_bDisplayHole=pSphere->m_bDisplayHole;
			}
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("m_bUserSpecColor"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pSphere->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pSphere->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pSphere->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pSphere->relativeObjs.GetFirst();pMirObj;pMirObj=pSphere->relativeObjs.GetNext())
			{
				CLDSSphere *pMirSphere=(CLDSSphere*)pSphere->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SPHERE);
				if(pMirSphere)
					pMirSphere->m_bUserSpecColor=pSphere->m_bUserSpecColor;
			}
		}
		if(pFirstSphere->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("crMaterial"),NULL);
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
				pFindItem->m_idProp = CLDSSphere::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if(pFirstSphere->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSSphere::GetPropID("crMaterial"));
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pSphere->relativeObjs.GetFirst();pMirObj;pMirObj=pSphere->relativeObjs.GetNext())
			{
				CLDSSphere *pMirSphere=(CLDSSphere*)pSphere->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SPHERE);
				if(pMirSphere)
					pMirSphere->crMaterial=pSphere->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("sNotes"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			_snprintf(pSphere->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
			//��ע���ù����޸� wht 10-10-22
			/*for(RELATIVE_OBJECT *pMirObj=pSphere->relativeObjs.GetFirst();pMirObj;pMirObj=pSphere->relativeObjs.GetNext())
			{
				CLDSSphere *pMirSphere=(CLDSSphere*)pSphere->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SPHERE);
				if(pMirSphere)
					strcpy(pMirSphere->sNotes,pSphere->sNotes);
			}*/
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("hDatumNode"))
	{
		long h=0;
		sscanf(tem_str,"%X",&h);
		CLDSNode *pDatumNode=console.FromNodeHandle(h);
		if(pDatumNode)
		{
			for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
			{
				pSphere->hDatumNode = h;
				pSphere->ucs.origin=pDatumNode->Position(true);
			}
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pDatumNode->Position(true).x);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.origin.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pDatumNode->Position(true).y);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.origin.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pDatumNode->Position(true).z);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSSphere::GetPropID("ucs.origin.z"),CString(sText));
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox(NULL,"The input handle is error��can't find the corresponding node��",sSoftName,MB_OK);
#else 
			MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
			return FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.origin.x"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
			pSphere->ucs.origin.x = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.origin.y"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
			pSphere->ucs.origin.y = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.origin.z"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
			pSphere->ucs.origin.z = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.axis_x.x"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->ucs.axis_x.x = atof(valueStr);
			rotate_ucs_axis(pSphere->ucs,pSphere->ucs.origin+pSphere->ucs.axis_x*1,0);
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.axis_x.y"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->ucs.axis_x.y = atof(valueStr);
			rotate_ucs_axis(pSphere->ucs,pSphere->ucs.origin+pSphere->ucs.axis_x*1,0);
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.axis_x.z"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->ucs.axis_x.z = atof(valueStr);
			rotate_ucs_axis(pSphere->ucs,pSphere->ucs.origin+pSphere->ucs.axis_x*1,0);
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.axis_y.x"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->ucs.axis_y.x = atof(valueStr);
			rotate_ucs_axis(pSphere->ucs,pSphere->ucs.origin+pSphere->ucs.axis_y*1,1);
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.axis_y.y"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->ucs.axis_y.y = atof(valueStr);
			rotate_ucs_axis(pSphere->ucs,pSphere->ucs.origin+pSphere->ucs.axis_y*1,1);
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.axis_y.z"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->ucs.axis_y.z = atof(valueStr);
			rotate_ucs_axis(pSphere->ucs,pSphere->ucs.origin+pSphere->ucs.axis_y*1,1);
		}	
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.axis_z.x"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->ucs.axis_z.x = atof(valueStr);
			rotate_ucs_axis(pSphere->ucs,pSphere->ucs.origin+pSphere->ucs.axis_z*1,2);
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.axis_z.y"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->ucs.axis_z.y = atof(valueStr);
			rotate_ucs_axis(pSphere->ucs,pSphere->ucs.origin+pSphere->ucs.axis_z*1,2);
		}
	}
	else if(pItem->m_idProp==CLDSSphere::GetPropID("ucs.axis_z.z"))
	{
		for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
		{
			pSphere->ucs.axis_z.z = atof(valueStr);
			rotate_ucs_axis(pSphere->ucs,pSphere->ucs.origin+pSphere->ucs.axis_z*1,2);
		}
	}
	else 
		return FALSE;
	for(pSphere=(CLDSSphere*)pView->selectObjs.GetFirst();pSphere;pSphere=(CLDSSphere*)pView->selectObjs.GetNext())
	{
		pSphere->SetModified();
		pSphere->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pSphere->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}
#endif
BOOL ModifyWorkViewProperty(CPropertyList *pPropList,CPropTreeItem *pItem,CString &valueStr)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CString sNewValue;
	if(pItem->m_idProp==CLDSView::GetPropID("wcs.origin.x"))
		console.m_curWCS.origin.x = atof(valueStr);
	else if(pItem->m_idProp==CLDSView::GetPropID("wcs.origin.y"))
		console.m_curWCS.origin.y = atof(valueStr);
	else if(pItem->m_idProp==CLDSView::GetPropID("wcs.origin.z"))
		console.m_curWCS.origin.z = atof(valueStr);
	else if(pItem->m_idProp==CLDSView::GetPropID("wcs.axis_x.a"))
	{
		double alfa=atof(valueStr);
		alfa*=RADTODEG_COEF;
		console.m_curWCS.axis_x.x = cos(alfa);
		console.m_curWCS.axis_x.y = sin(alfa);
		console.m_curWCS.axis_y.x =-console.m_curWCS.axis_x.y;
		console.m_curWCS.axis_y.y = console.m_curWCS.axis_x.x;
		sNewValue.Format("%f",console.m_curWCS.axis_x.x);
		SimplifiedNumString(sNewValue);
		pPropList->SetItemPropValue(CLDSView::GetPropID("wcs.axis_x.x"),sNewValue);
		sNewValue.Format("%f",console.m_curWCS.axis_x.y);
		SimplifiedNumString(sNewValue);
		pPropList->SetItemPropValue(CLDSView::GetPropID("wcs.axis_x.y"),sNewValue);
		sNewValue.Format("%f",console.m_curWCS.axis_y.x);
		SimplifiedNumString(sNewValue);
		pPropList->SetItemPropValue(CLDSView::GetPropID("wcs.axis_y.x"),sNewValue);
		sNewValue.Format("%f",console.m_curWCS.axis_y.y);
		SimplifiedNumString(sNewValue);
		pPropList->SetItemPropValue(CLDSView::GetPropID("wcs.axis_y.y"),sNewValue);
	}
	/*else if(pItem->m_idProp==CLDSView::GetPropID("vcs.origin.x"))
		Ta.m_curACS.origin.x = atof(valueStr);
	else if(pItem->m_idProp==CLDSView::GetPropID("vcs.origin.y"))
		Ta.m_curACS.origin.y = atof(valueStr);
	else if(pItem->m_idProp==CLDSView::GetPropID("vcs.origin.z"))
		Ta.m_curACS.origin.z = atof(valueStr);
	else if(pItem->m_idProp==CLDSView::GetPropID("vcs.axis_x.x"))
		Ta.m_curACS.origin.x = atof(valueStr);
	else if(pItem->m_idProp==CLDSView::GetPropID("vcs.axis_x.y"))
		Ta.m_curACS.origin.y = atof(valueStr);
	else if(pItem->m_idProp==CLDSView::GetPropID("vcs.axis_x.z"))
		Ta.m_curACS.origin.z = atof(valueStr);*/
	else
		return FALSE;
	g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,console.m_curWCS);
	pView->Invalidate(FALSE);
	return TRUE;
}
BOOL WorkViewClick(CPropertyList *pPropList,CPropTreeItem *pItem)
{
#ifndef __TSA_
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(pItem->m_idProp==CLDSView::GetPropID("wcs.origin"))
		pView->SpecWcsOrgin();
	else if(pItem->m_idProp==CLDSView::GetPropID("wcs.axis_x"))
		pView->SpecAxisXVertex();
	else if(pItem->m_idProp==CLDSView::GetPropID("wcs.axis_y"))
		pView->SpecAxisYVertex();
	else
		return FALSE;
#endif
	return TRUE;
}
BOOL ModifyWorkViewStatus(CPropertyList *pPropList,CPropTreeItem *pItem)
{
	CLDSView::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
void CLDSView::DisplayWorkViewProperty()
{
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem* pRootItem=pPropList->GetRootItem();
	CPropTreeItem* pGroupItem, *pPropItem,*pSonPropItem;//,*pParentItem;
	const int GROUP_GENERAL=1;
	pPropDlg->m_arrPropGroupLabel.RemoveAll();
	pPropDlg->m_arrPropGroupLabel.SetSize(1);
#ifdef AFX_TARG_ENU_ENGLISH
	pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
#else 
	pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
#endif
	pPropDlg->RefreshTabCtrl(CLDSView::m_iCurDisplayPropGroup);

	pPropList->CleanTree();
	pPropList->m_nObjClassTypeId = -1;	//��0Ҫ����0��ʾϵͳ����������
	pPropList->SetModifyValueFunc(ModifyWorkViewProperty);
	pPropList->SetButtonClickFunc(WorkViewClick);
	pPropList->SetModifyStatusFunc(ModifyWorkViewStatus);
	//
	CItemInfo* lpInfo = new CItemInfo();
	//�����ְ���Ϣ		
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "reside plate information";
	lpInfo->m_strPropHelp = "";
#else 
	lpInfo->m_strPropName = "�����ְ���Ϣ";
	lpInfo->m_strPropHelp = "";
#endif
	pGroupItem = pPropList->InsertItem(pRootItem,lpInfo, -1);
	pGroupItem->m_idProp = CLDSView::GetPropID("residePlateInfo");
	//pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSView::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CLDSView::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	//�����ְ���
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Datum Position";
	lpInfo->m_strPropHelp = "The datum position of view plane.";
#else 
	lpInfo->m_strPropName = "���";
	lpInfo->m_strPropHelp = "�����ְ���";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CLDSView::GetPropID("m_hSubjectPlate");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->SetReadOnly();
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	//�����ְ����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Datum Position";
	lpInfo->m_strPropHelp = "The datum position of view plane.";
#else 
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "�����ְ���";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CLDSView::GetPropID("m_sSubjectPlatePartNo");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->SetReadOnly();
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "View Coordinate System";
#else 
	lpInfo->m_strPropName = "��ͼ��ʾ����ϵ";
#endif
	pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CLDSView::GetPropID("VCS");
	pGroupItem->m_bHideChildren=TRUE;
	if(CLDSView::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CLDSView::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL); 
		//��ͼ��ʾƽ�����λ��		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Datum Position";
	lpInfo->m_strPropHelp = "The datum position of view plane.";
#else 
	lpInfo->m_strPropName = "λ��";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ�����λ��";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CLDSView::GetPropID("vcs.origin");
	//pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=TRUE;
	if(CLDSView::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSView::GetPropStatus(pPropItem->m_idProp);
	pPropItem->SetReadOnly();
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ�����X����		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
	lpInfo->m_strPropHelp = "The X component of view plane.";
#else 
	lpInfo->m_strPropName = "X����";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ�����X�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("vcs.origin.x");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ�����Y����		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
	lpInfo->m_strPropHelp = "The Y component of view plane.";
#else 
	lpInfo->m_strPropName = "Y����";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ�����Y�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("vcs.origin.y");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ�����Z����		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
	lpInfo->m_strPropHelp = "The Z component of view plane.";
#else 
	lpInfo->m_strPropName = "Z����";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ�����Z�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("vcs.origin.z");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ������ϵX�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Axis Direction";
	lpInfo->m_strPropHelp = "The X axis direction of view plane.";
#else 
	lpInfo->m_strPropName = "X�᷽��";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ������ϵX�᷽��";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CLDSView::GetPropID("vcs.axis_x");
	//pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=TRUE;
	if(CLDSView::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSView::GetPropStatus(pPropItem->m_idProp);
	pPropItem->SetReadOnly();
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ������ϵX�᷽��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Component";
	lpInfo->m_strPropHelp = "The X component of view coordinate system's x axis.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ������ϵX�᷽��X�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("vcs.axis_x.x");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ������ϵX�᷽��Y�������		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Component";
	lpInfo->m_strPropHelp = "The Y component of view coordinate system's x axis.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ������ϵX�᷽��Y�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("vcs.axis_x.y");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ������ϵX�᷽��Z�������		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Component";
	lpInfo->m_strPropHelp = "The Z component of view coordinate system's x axis.";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ������ϵX�᷽��Z�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("vcs.axis_x.z");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ������ϵY�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Axis Direction";
	lpInfo->m_strPropHelp = "The Y axis direction of view plane.";
#else 
	lpInfo->m_strPropName = "Y�᷽��";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ������ϵY�᷽��";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CLDSView::GetPropID("vcs.axis_y");
	//pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=TRUE;
	if(CLDSView::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSView::GetPropStatus(pPropItem->m_idProp);
	pPropItem->SetReadOnly();
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ������ϵY�᷽��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Component";
	lpInfo->m_strPropHelp = "The X component of view coordinate system's y axis.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ������ϵY�᷽��X�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("vcs.axis_y.x");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ������ϵY�᷽��Y�������		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Component";
	lpInfo->m_strPropHelp = "The Y component of view coordinate system's y axis.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ������ϵY�᷽��Y�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("vcs.axis_y.y");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ������ϵY�᷽��Z�������		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Component";
	lpInfo->m_strPropHelp = "The Z component of view coordinate system's y axis.";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ������ϵY�᷽��Z�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("vcs.axis_y.z");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ������ϵZ�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Axis Direction";
	lpInfo->m_strPropHelp = "The Z axis direction of view plane.";
#else 
	lpInfo->m_strPropName = "Z�᷽��";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ������ϵZ�᷽��";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CLDSView::GetPropID("vcs.axis_z");
	//pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=TRUE;
	if(CLDSView::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSView::GetPropStatus(pPropItem->m_idProp);
	pPropItem->SetReadOnly();
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ������ϵZ�᷽��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Component";
	lpInfo->m_strPropHelp = "The X component of view coordinate system's z axis.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ������ϵZ�᷽��X�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("vcs.axis_z.x");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ������ϵZ�᷽��Y�������		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Component";
	lpInfo->m_strPropHelp = "The Y component of view coordinate system's z axis.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ������ϵZ�᷽��Y�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("vcs.axis_z.y");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ͼ��ʾƽ������ϵZ�᷽��Z�������		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Component";
	lpInfo->m_strPropHelp = "The Z component of view coordinate system's z axis.";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "��ͼ��ʾƽ������ϵZ�᷽��Z�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("vcs.axis_z.z");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	//��������ϵ
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Work Coordinate System";
#else 
	lpInfo->m_strPropName = "��������ϵ";
#endif
	pGroupItem = pPropList->InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CLDSView::GetPropID("WCS");
	pGroupItem->m_bHideChildren=FALSE;
	if(CLDSView::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CLDSView::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL); 
		//��������ϵ����λ��		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Datum Position";
	lpInfo->m_strPropHelp = "The datum position of work coordinate system.";
#else 
	lpInfo->m_strPropName = "λ��";
	lpInfo->m_strPropHelp = "��������ϵ����λ��";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CLDSView::GetPropID("wcs.origin");
	//pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=FALSE;
	if(CLDSView::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSView::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��������ϵ����X����		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
	lpInfo->m_strPropHelp = "The X component of work coordinate system's datum point.";
#else 
	lpInfo->m_strPropName = "X����";
	lpInfo->m_strPropHelp = "��������ϵ����X�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("wcs.origin.x");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��������ϵ����Y����		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
	lpInfo->m_strPropHelp = "The Y component of work coordinate system's datum point.";
#else 
	lpInfo->m_strPropName = "Y����";
	lpInfo->m_strPropHelp = "��������ϵ����Y�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("wcs.origin.y");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��������ϵX�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Axis Direction";
	lpInfo->m_strPropHelp = "The X axis direction of work coordinate.";
#else 
	lpInfo->m_strPropName = "X�᷽��";
	lpInfo->m_strPropHelp = "��������ϵX�᷽��";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CLDSView::GetPropID("wcs.axis_x");
	//pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=FALSE;
	if(CLDSView::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSView::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��������ϵX�᷽������н�
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "������нǦ�";
	lpInfo->m_strPropHelp = "��������ϵX������ͼ��ʾ����ϵX�����ʱ��нǡ�";
#else 
	lpInfo->m_strPropName = "������нǦ�";
	lpInfo->m_strPropHelp = "��������ϵX������ͼ��ʾ����ϵX�����ʱ��нǡ�";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("wcs.axis_x.a");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��������ϵX�᷽��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Component";
	lpInfo->m_strPropHelp = "The x component of work coordinate system's X axis.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "��������ϵX�᷽��X�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("wcs.axis_x.x");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��������ϵX�᷽��Y�������		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Component";
	lpInfo->m_strPropHelp = "The y component of work coordinate system's X axis.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "��������ϵX�᷽��Y�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("wcs.axis_x.y");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��������ϵY�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Axis Direction";
	lpInfo->m_strPropHelp = "The Y axis direction of work coordinate.";
#else 
	lpInfo->m_strPropName = "Y�᷽��";
	lpInfo->m_strPropHelp = "��������ϵY�᷽��";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CLDSView::GetPropID("wcs.axis_y");
	//pPropItem->m_lpNodeInfo->m_strPropValue = theApp.GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_bHideChildren=FALSE;
	if(CLDSView::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CLDSView::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��������ϵY�᷽��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Component";
	lpInfo->m_strPropHelp = "The x component of work coordinate system's Y axis.";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "��������ϵY�᷽��X�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("wcs.axis_y.x");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��������ϵY�᷽��Y�������		
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Component";
	lpInfo->m_strPropHelp = "The y component of work coordinate system's Y axis.";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "��������ϵY�᷽��Y�������";
#endif
	pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pSonPropItem->m_idProp = CLDSView::GetPropID("wcs.axis_y.y");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->SetReadOnly();
	pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	pPropList->Redraw();
}

#ifndef __TSA_
void CLDSView::DisplaySphereProperty(BOOL bRebuild)
{
	CLDSSphere *pFirstSphere = (CLDSSphere*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pLeafItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo=NULL;

	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_POS=2;
	if(bRebuild)
	{	//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(2);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"Position");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"λ��");
#endif
		pPropDlg->RefreshTabCtrl(CLDSSphere::m_iCurDisplayPropGroup);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_SPHERE;
		pPropList->SetModifyValueFunc(ModifySphereProperty);
		pPropList->SetButtonClickFunc(SphereButtonClick);
		pPropList->SetModifyStatusFunc(ModifySphereItemStatus);
		pPropList->SetPopMenuClickFunc(FireSpherePopMenuClick);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstSphere->BelongModel()->IsTowerModel()||pFirstSphere->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//�ְ������Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Basic Information";
		lpInfo->m_strPropHelp = "The sphere's basic information.";
#else 
		lpInfo->m_strPropName = "���������Ϣ";
		lpInfo->m_strPropHelp = "���������Ϣ";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSSphere::GetPropID("basicInfo");
		if(CLDSSphere::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSSphere::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
			//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "Sphere's handle is typical unique identifier string in the  LDS system.";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "��������LDSϵͳ�ж������ȫ��Ψһ��ʶ";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("handle");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
			//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "Sphere in the layer";
#else 
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "��������ͼ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("layer");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Material";
		lpInfo->m_strPropHelp = "The material of sphere.";
#else 
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "�������";
#endif
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("cMaterial");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//�����ȼ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "m_cQualityLevel";
		lpInfo->m_strPropHelp = "The quality grade of sphere.";
#else 
		lpInfo->m_strPropName = "�����ȼ�";
		lpInfo->m_strPropHelp = "���������ȼ�";
#endif
		lpInfo->m_cmbItems = "A|B|C|D|";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("m_cQualityLevel");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specification";
		lpInfo->m_strPropHelp = "The specification of sphere(external diameter X thickness)";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "����Ĺ��(��ֱ��X���)";
#endif
		lpInfo->m_cmbItems = MakeTubeSetString();
		lpInfo->m_cmbType = CDT_EDIT;
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("specification");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//��ֱ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "External Diameter";
		lpInfo->m_strPropHelp = "The external diameter of sphere(mm)��";
#else 
		lpInfo->m_strPropName = "��ֱ��";
		lpInfo->m_strPropHelp = "�������ֱ��(mm)��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("D");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//��ֱ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Internal diameter";
		lpInfo->m_strPropHelp = "The internal diameter of sphere(mm)��";
#else 
		lpInfo->m_strPropName = "��ֱ��";
		lpInfo->m_strPropHelp = "�������ֱ��(mm)��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("d");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
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
		pPropItem->m_idProp = CLDSSphere::GetPropID("iSeg");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part Number";
		lpInfo->m_strPropHelp = "Part number";
#else 
		lpInfo->m_strPropName = "�������";
		lpInfo->m_strPropHelp = "�������";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("sPartNo");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
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
		pPropItem->m_idProp = CLDSSphere::GetPropID("cfgword");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//ͳ��ϵ��
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
		pPropItem->m_idProp = CLDSSphere::GetPropID("m_uStatMatCoef");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		  //���Ӹ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weld Parent Part";
		lpInfo->m_strPropHelp = "If the part need to weld to other part, then the property is the handle of the part's attached welded part;If the part doesn't need to be welded,the value is 0X0.";
#else 
		lpInfo->m_strPropName = "���Ӹ�����";
		lpInfo->m_strPropHelp = "����˹�����Ҫ���ӵ�����������,��ô�����Ծ��Ǳ������������ĺ��ӹ����ľ��;����˹�������Ҫ����,��ֵΪ0X0��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("m_hPartWeldParent");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
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
		pPropItem->m_idProp = CLDSSphere::GetPropID("m_bDisplayHole");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//�û�ָ��������ɫ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specify Part Color";
		lpInfo->m_strPropHelp = "Whether the part's color is specified by the user.";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "ָ��������ɫ";
		lpInfo->m_strPropHelp = "������ɫ�Ƿ�Ϊ�û�ָ��";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstSphere->m_bUserSpecColor)
		{	//������ɫ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Part Color";
			lpInfo->m_strPropHelp = "Part's color";
#else 
			lpInfo->m_strPropName = "������ɫ";
			lpInfo->m_strPropHelp = "������ɫ";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSSphere::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstSphere->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
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
		pPropItem->m_idProp = CLDSSphere::GetPropID("sNotes");
		if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//װ�䷽λ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Assemble Position";
		lpInfo->m_strPropHelp = "The assembly location of part and assembly direction.";
#else 
		lpInfo->m_strPropName = "װ�䷽λ";
		lpInfo->m_strPropHelp = "������װ��λ�ü�װ��ʱ��װ�䷽��";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_bHideChildren = FALSE;
		pGroupItem->m_idProp = CLDSSphere::GetPropID("assemblePos");
		if(CLDSSphere::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSSphere::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Assemble Coordinate System";
		lpInfo->m_strPropHelp = "Assemble coordinate system";
#else 
		lpInfo->m_strPropName = "װ������ϵ";
		lpInfo->m_strPropHelp = "װ������ϵ��";
#endif
		CPropTreeItem *pAcsItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pAcsItem->m_idProp = CLDSSphere::GetPropID("ucs");
		if(CLDSSphere::GetPropStatus(pAcsItem->m_idProp)!=-1)
			pAcsItem->m_bHideChildren = CLDSSphere::GetPropStatus(pAcsItem->m_idProp);
		pAcsItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//װ��λ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Position";
		lpInfo->m_strPropHelp = "The position of assembling datum point.";
#else 
		lpInfo->m_strPropName = "λ��";
		lpInfo->m_strPropHelp = "װ������λ�á�";
#endif
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("ucs.origin");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSSphere::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSSphere::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//��׼�ڵ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Node";
		lpInfo->m_strPropHelp = "Datum node";
#else 
		lpInfo->m_strPropName = "��׼�ڵ�";
		lpInfo->m_strPropHelp = "��׼�ڵ�";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("hDatumNode");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//װ������ϵԭ��λ��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Coordinate";
		lpInfo->m_strPropHelp = "The x component of assembling coordinate system.";
#else 
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "װ������ϵԭ��λ��X���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("ucs.origin.x");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//װ������ϵԭ��λ��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Coordinate";
		lpInfo->m_strPropHelp = "The y component of assembling coordinate system.";
#else 
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Y���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("ucs.origin.y");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//װ������ϵԭ��λ��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Coordinate";
		lpInfo->m_strPropHelp = "The z component of assembling coordinate system.";
#else 
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Z���������";
#endif
		pLeafItem= pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("ucs.origin.z");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//װ��ʱX�᷽��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Axis Direction";
		lpInfo->m_strPropHelp = "The assembling X axis direction";
#else 
		lpInfo->m_strPropName = "X�᷽��";
		lpInfo->m_strPropHelp = "װ��ʱX�᷽��";
#endif
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("ucs.axis_x");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSSphere::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSSphere::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//װ������ϵX��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Component";
		lpInfo->m_strPropHelp = "The x component of assembling coordinate system's X axis.";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "װ������ϵX��X���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("ucs.axis_x.x");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//װ������ϵX��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Component";
		lpInfo->m_strPropHelp = "The y component of assembling coordinate system's X axis.";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "װ������ϵX��Y���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("ucs.axis_x.y");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//װ������ϵX��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Component";
		lpInfo->m_strPropHelp = "The z component of assembling coordinate system's X axis.";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "װ������ϵX��Z���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("ucs.axis_x.z");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//װ��ʱY�᷽��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Axis Direction";
		lpInfo->m_strPropHelp = "The assembling Y axis direction";
#else 
		lpInfo->m_strPropName = "Y�᷽��";
		lpInfo->m_strPropHelp = "װ��ʱY�᷽��";
#endif
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("ucs.axis_y");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSSphere::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSSphere::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//װ������ϵY��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Component";
		lpInfo->m_strPropHelp = "The x component of assembling coordinate system's Y axis.";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "װ������ϵY��X���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("ucs.axis_y.x");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//װ������ϵY��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Component";
		lpInfo->m_strPropHelp = "The y component of assembling coordinate system's Y axis.";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "װ������ϵY��Y���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("ucs.axis_y.y");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//װ������ϵY��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Component";
		lpInfo->m_strPropHelp = "The z component of assembling coordinate system's Y axis.";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "װ������ϵY��Z���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("ucs.axis_y.z");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//װ��ʱZ�᷽��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Axis Direction";
		lpInfo->m_strPropHelp = "The assembling Z axis direction";
#else 
		lpInfo->m_strPropName = "Z�᷽��";
		lpInfo->m_strPropHelp = "װ��ʱZ�᷽��";
#endif
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("ucs.axis_z");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSSphere::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSSphere::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//װ������ϵZ��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Component";
		lpInfo->m_strPropHelp = "The x component of assembling coordinate system's Z axis.";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "װ������ϵZ��X���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("ucs.axis_z.x");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//װ������ϵZ��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Component";
		lpInfo->m_strPropHelp = "The y component of assembling coordinate system's Z axis.";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "װ������ϵZ��Y���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("ucs.axis_z.y");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
				//װ������ϵZ��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z Component";
		lpInfo->m_strPropHelp = "The z component of assembling coordinate system's Z axis.";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "װ������ϵZ��Z���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSSphere::GetPropID("ucs.axis_z.z");
		if(pFirstSphere->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//�������Ӹ˼�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="View|Import";
		lpInfo->m_strPropName = "Link Rod";
		lpInfo->m_strPropHelp = "Link rod";
#else 
		lpInfo->m_cmbItems="�鿴|����";
		lpInfo->m_strPropName = "���Ӹ˼�";
		lpInfo->m_strPropHelp = "���Ӹ˼�";
#endif
		pPropItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp = CLDSSphere::GetPropID("sphereLinkPoles");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
	}
	else
	{
		SetPartMutiObjsProp(pFirstSphere,"handle");
		SetPartMutiObjsProp(pFirstSphere,"layer");
		SetPartMutiObjsProp(pFirstSphere,"cMaterial");
		SetPartMutiObjsProp(pFirstSphere,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstSphere,"specification");
		SetPartMutiObjsProp(pFirstSphere,"iSeg");
		SetPartMutiObjsProp(pFirstSphere,"sPartNo");
		SetPartMutiObjsProp(pFirstSphere,"cfgword");
		SetPartMutiObjsProp(pFirstSphere,"D");
		SetPartMutiObjsProp(pFirstSphere,"d");
		SetPartMutiObjsProp(pFirstSphere,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstSphere,"m_hPartWeldParent");
		SetPartMutiObjsProp(pFirstSphere,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstSphere,"m_bUserSpecColor");
		if(pFirstSphere->m_bUserSpecColor)
		{	
			if(!SetPartMutiObjsProp(pFirstSphere,"crMaterial"))
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSSphere::GetPropID("m_bUserSpecColor"),NULL);
				if(pFindItem)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
					lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Part Color";
					lpInfo->m_strPropHelp = "Part's color";
#else 
					lpInfo->m_strPropName = "������ɫ";
					lpInfo->m_strPropHelp = "������ɫ";
#endif
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSSphere::GetPropID("crMaterial");
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSSphere::IsPropEqual(selectObjs,pPropItem->m_idProp);
					if(pFirstSphere->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				}
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSSphere::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstSphere,"sNotes");
		SetPartMutiObjsProp(pFirstSphere,"ucs.origin.x");
		SetPartMutiObjsProp(pFirstSphere,"ucs.origin.y");
		SetPartMutiObjsProp(pFirstSphere,"ucs.origin.z");
		SetPartMutiObjsProp(pFirstSphere,"ucs.axis_x.x");
		SetPartMutiObjsProp(pFirstSphere,"ucs.axis_x.y");
		SetPartMutiObjsProp(pFirstSphere,"ucs.axis_x.z");
		SetPartMutiObjsProp(pFirstSphere,"ucs.axis_y.x");
		SetPartMutiObjsProp(pFirstSphere,"ucs.axis_y.y");
		SetPartMutiObjsProp(pFirstSphere,"ucs.axis_y.z");
		SetPartMutiObjsProp(pFirstSphere,"ucs.axis_z.x");
		SetPartMutiObjsProp(pFirstSphere,"ucs.axis_z.y");
		SetPartMutiObjsProp(pFirstSphere,"ucs.axis_z.z");
	}
	pPropList->Redraw();
}
#endif

void CLDSView::DisplayDbObjectProperty(BOOL bRebuild)
{
	CLDSDbObject *pFirstDbObject = (CLDSDbObject*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pSonPropItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	const int GROUP_GENERAL=1;
	char sText[MAX_TEMP_CHAR_100+1]="";
	if(bRebuild)
	{//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(0);
		pPropDlg->RefreshTabCtrl(0);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_DBOBJECT;
		pPropList->SetModifyValueFunc(ModifyDbObjectProperty);
		pPropList->SetButtonClickFunc(DbObjectButtonClick);
		pPropList->SetModifyStatusFunc(ModifyDbObjectItemStatus);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstDbObject->BelongModel()->IsTowerModel()||pFirstDbObject->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//������Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Data Object Basic information";
		lpInfo->m_strPropHelp = "The basic information of data object.";
#else 
		lpInfo->m_strPropName = "���ݶ��������Ϣ";
		lpInfo->m_strPropHelp = "���ݶ��������Ϣ";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSDbObject::GetPropID("basicInfo");
		if(CLDSDbObject::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSDbObject::GetPropStatus(pGroupItem->m_idProp);
			//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
#ifdef __TMA_
		lpInfo->m_strPropHelp = "Part's handle is typical unique identifier string in the  TMA system.";
#elif defined(__LMA_)
		lpInfo->m_strPropHelp = "Part's handle is typical unique identifier string in the  LMA system.";
#elif defined(__TSA_)
		lpInfo->m_strPropHelp = "Part's handle is typical unique identifier string in the  TSA system.";
#else
		lpInfo->m_strPropHelp = "Part's handle is typical unique identifier string in the  LDS system.";
#endif
#else 
		lpInfo->m_strPropName = "���";
#ifdef __TMA_
		lpInfo->m_strPropHelp = "�����TMAϵͳ�жԹ�����ȫ��Ψһ��ʶ";
#elif defined(__LMA_)
		lpInfo->m_strPropHelp = "�����LMAϵͳ�жԹ�����ȫ��Ψһ��ʶ";
#elif defined(__TSA_)
		lpInfo->m_strPropHelp = "�����TSAϵͳ�жԹ�����ȫ��Ψһ��ʶ";
#else
		lpInfo->m_strPropHelp = "�����LDSϵͳ�жԹ�����ȫ��Ψһ��ʶ";
#endif
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSDbObject::GetPropID("handle");
		if(pFirstDbObject->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
			//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "Part in the layer";
#else 
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "��������ͼ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSDbObject::GetPropID("layer");
		if(pFirstDbObject->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_bHideChildren=FALSE;
		//ͼ���һ��ĸ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems="T|S|L";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "First Char";
#else 
		lpInfo->m_strPropName = "��һ�ַ�";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSDbObject::GetPropID("layer1");
		if(pFirstDbObject->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		//ͼ��ڶ���ĸ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems="Z|X|H|F";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Second Char";
#else 
		lpInfo->m_strPropName = "�ڶ��ַ�";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSDbObject::GetPropID("layer2");
		if(pFirstDbObject->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		//ͼ�������ĸ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems="Q|H|Z|Y|1|2|3|4|G";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Third Char";
#else 
		lpInfo->m_strPropName = "�����ַ�";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSDbObject::GetPropID("layer3");
		if(pFirstDbObject->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
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
		pPropItem->m_idProp = CLDSDbObject::GetPropID("iSeg");
		if(pFirstDbObject->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pPropItem->m_idProp);
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
		pPropItem->m_idProp = CLDSDbObject::GetPropID("cfgword");
		if(pFirstDbObject->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pPropItem->m_idProp);
#ifdef __COMMON_PART_INC_	//Ԥ�������ͼʱȫ��ѡ�нڵ�͸˼��趨Ԥ����ʱ��
		if(!theApp.m_bAnalysisWorkEnvOnly)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Lift arc Handle";
			lpInfo->m_strPropHelp = "The lift arc handle of part acting.";
#else 
			lpInfo->m_strPropName = "Ԥ����";
			lpInfo->m_strPropHelp = "����ִ�е�Ԥ����";
#endif
			lpInfo->m_cmbType=CDT_LIST;
			lpInfo->m_cmbItems=MakeArcLiftSetString();
			CPropTreeItem *pArcLiftItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pArcLiftItem->m_bHideChildren = FALSE;
			pArcLiftItem->m_idProp = CLDSDbObject::GetPropID("m_hArcLift");
			if(pFirstDbObject->GetPropValueStr(pArcLiftItem->m_idProp,sText)>0)
				pArcLiftItem->m_lpNodeInfo->m_strPropValue = sText;
			pArcLiftItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pArcLiftItem->m_idProp);
			//if(CLDSDbObject::GetPropStatus(pPropItem->m_idProp)!=-1)
			//	pArcLiftItem->m_bHideChildren = CLDSDbObject::GetPropStatus(pArcLiftItem->m_idProp);
			//pArcLiftItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		}
#endif
		//
		if(pFirstDbObject->IsShadowObject())
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
			pPropItem->m_idProp = CLDSDbObject::GetPropID("shadowInfo");
			pPropItem->m_bHideChildren = FALSE;
			if(CLDSDbObject::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSDbObject::GetPropStatus(pPropItem->m_idProp);
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
			pSonPropItem->m_idProp = CLDSDbObject::GetPropID("m_hMotherObj");
			if(pFirstDbObject->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstDbObject->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstDbObject->IsBlockRefShadowObject())
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
				pSonPropItem->m_idProp = CLDSDbObject::GetPropID("m_hBlockRef");
				if(pFirstDbObject->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstDbObject->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
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
			pPropItem->m_idProp = CLDSDbObject::GetPropID("m_hBlock");
			if(pFirstDbObject->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstDbObject->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
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
		pPropItem->m_idProp = CLDSDbObject::GetPropID("sNotes");
		if(pFirstDbObject->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pPropItem->m_idProp);
	}
	else
	{
		SetPartMutiObjsProp(pFirstDbObject,"handle",CLS_DBOBJECT);
		SetPartMutiObjsProp(pFirstDbObject,"layer",CLS_DBOBJECT);
		SetPartMutiObjsProp(pFirstDbObject,"iSeg",CLS_DBOBJECT);
		SetPartMutiObjsProp(pFirstDbObject,"cfgword",CLS_DBOBJECT);
		SetPartMutiObjsProp(pFirstDbObject,"m_hArcLift",CLS_DBOBJECT);
		CPropTreeItem *pFindItem,*pParentItem;
		pFindItem=pPropList->FindItemByPropId(CLDSDbObject::GetPropID("m_hBlock"),&pParentItem);
		if (pFindItem)
		{
			if (pFirstDbObject->IsShadowObject())
			{	//����ͨ�����л���Ӱ�乹��
				pPropList->DeleteItemByPropId(CLDSDbObject::GetPropID("m_hBlock"));
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
				pPropItem->m_idProp = CLDSDbObject::GetPropID("shadowInfo");
				if(CLDSDbObject::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSDbObject::GetPropStatus(pPropItem->m_idProp);
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
				pSonPropItem->m_idProp = CLDSDbObject::GetPropID("m_hMotherObj");
				if(pFirstDbObject->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstDbObject->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				if (pFirstDbObject->IsBlockRefShadowObject())
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
					pSonPropItem->m_idProp = CLDSDbObject::GetPropID("m_hBlockRef");
					if(pFirstDbObject->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstDbObject->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstDbObject,"m_hBlock",CLS_DBOBJECT);
			}
		}
		pFindItem=pPropList->FindItemByPropId(CLDSDbObject::GetPropID("shadowInfo"),&pParentItem);
		if (pFindItem)
		{
			if (!pFirstDbObject->IsShadowObject())
			{	//��Ӱ�乹���л�����ͨ����
				pPropList->DeleteItemByPropId(CLDSDbObject::GetPropID("shadowInfo"));
				pPropList->DeleteItemByPropId(CLDSDbObject::GetPropID("m_hMotherObj"));
				pFindItem=pPropList->FindItemByPropId(CLDSDbObject::GetPropID("m_hBlockRef"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSDbObject::GetPropID("m_hBlockRef"));
				pFindItem=pPropList->FindItemByPropId(CLDSDbObject::GetPropID("m_hBlock"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSDbObject::GetPropID("m_hBlock"));
				if(pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstDbObject,"m_hBlock",CLS_DBOBJECT);
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
					pPropItem->m_idProp = CLDSDbObject::GetPropID("m_hBlock");
					if(pFirstDbObject->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pPropItem->m_idProp);
					pPropItem->SetReadOnly(pFirstDbObject->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstDbObject,"m_hMotherObj",CLS_DBOBJECT);
				pFindItem=pPropList->FindItemByPropId(CLDSDbObject::GetPropID("m_hBlockRef"),&pParentItem);
				if (pFindItem&&pFirstDbObject->IsBlockRefShadowObject())
				{
					SetPartMutiObjsProp(pFirstDbObject,"m_hBlockRef",CLS_DBOBJECT);
					SetPartMutiObjsProp(pFirstDbObject,"m_hBlock",CLS_DBOBJECT);
				}
				else if (pFindItem&&!pFirstDbObject->IsBlockRefShadowObject())
				{
					pPropList->DeleteItemByPropId(CLDSDbObject::GetPropID("m_hBlockRef"));
					pPropList->DeleteItemByPropId(CLDSDbObject::GetPropID("m_hBlock"));
				}
				else if(pFindItem==NULL&&pFirstDbObject->IsBlockRefShadowObject())
				{
					pPropList->FindItemByPropId(CLDSDbObject::GetPropID("m_hMotherObj"),&pParentItem);
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
					pSonPropItem->m_idProp = CLDSDbObject::GetPropID("m_hBlockRef");
					if(pFirstDbObject->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSDbObject::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstDbObject->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
		}
		SetPartMutiObjsProp(pFirstDbObject,"sNotes",CLS_DBOBJECT);
	}
	pPropList->Redraw();
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
void CLDSView::DisplayPartProperty(BOOL bRebuild)
{
	CLDSPart *pFirstPart = (CLDSPart*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pSonPropItem,*pRoot=pPropList->GetRootItem();
	const int GROUP_GENERAL=1;
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	if(bRebuild)
	{//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(0);
		pPropDlg->RefreshTabCtrl(0);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_PART;
		pPropList->SetModifyValueFunc(ModifyPartProperty);
		pPropList->SetButtonClickFunc(PartButtonClick);
		pPropList->SetModifyStatusFunc(ModifyPartItemStatus);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstPart->BelongModel()->IsTowerModel()||pFirstPart->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//������Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Basic Information";
		lpInfo->m_strPropHelp = "Basic information";
#else 
		lpInfo->m_strPropName = "������Ϣ";
		lpInfo->m_strPropHelp = "������Ϣ";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSPart::GetPropID("basicInfo");
		if(CLDSPart::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSPart::GetPropStatus(pGroupItem->m_idProp);
			//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
#ifdef __TMA_
		lpInfo->m_strPropHelp = "Part's handle is typical unique identifier string in the  TMA system.";
#elif defined(__LMA_)
		lpInfo->m_strPropHelp = "Part's handle is typical unique identifier string in the  LMA system.";
#else
		lpInfo->m_strPropHelp = "Part's handle is typical unique identifier string in the  LDS system.";
#endif
#else 
		lpInfo->m_strPropName = "���";
#ifdef __TMA_
		lpInfo->m_strPropHelp = "�����TMAϵͳ�жԹ�����ȫ��Ψһ��ʶ";
#elif defined(__LMA_)
		lpInfo->m_strPropHelp = "�����LMAϵͳ�жԹ�����ȫ��Ψһ��ʶ";
#else
		lpInfo->m_strPropHelp = "�����LDSϵͳ�жԹ�����ȫ��Ψһ��ʶ";
#endif
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPart::GetPropID("handle");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
			//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "Part in the layer";
#else 
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "��������ͼ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPart::GetPropID("layer");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
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
		pPropItem->m_idProp = CLDSPart::GetPropID("cMaterial");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//�����ȼ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "m_cQualityLevel";
		lpInfo->m_strPropHelp = "The quality grade of part.";
#else 
		lpInfo->m_strPropName = "�����ȼ�";
		lpInfo->m_strPropHelp = "�����ȼ�";
#endif
		lpInfo->m_cmbItems = "A|B|C|D|";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPart::GetPropID("m_cQualityLevel");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
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
		pPropItem->m_idProp = CLDSPart::GetPropID("iSeg");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part Number";
		lpInfo->m_strPropHelp = "Part number"; 
#else 
		lpInfo->m_strPropName = "�������";
		lpInfo->m_strPropHelp = "�������"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPart::GetPropID("sPartNo");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
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
		pPropItem->m_idProp = CLDSPart::GetPropID("cfgword");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//ͳ��ϵ��
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
		pPropItem->m_idProp = CLDSPart::GetPropID("m_uStatMatCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		  //���Ӹ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weld Parent Part";
		lpInfo->m_strPropHelp = "If the part need to weld to other part, then the property is the handle of the part's attached welded part;If the part doesn't need to be welded,the value is 0X0.";
#else 
		lpInfo->m_strPropName = "���Ӹ�����";
		lpInfo->m_strPropHelp = "����˹�����Ҫ���ӵ�����������,��ô�����Ծ��Ǳ������������ĺ��ӹ����ľ��;����˹�������Ҫ����,��ֵΪ0X0��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPart::GetPropID("m_hPartWeldParent");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
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
		pPropItem->m_idProp = CLDSPart::GetPropID("m_bDisplayHole");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//���⸨������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual Part";
		lpInfo->m_strPropHelp = "Virtual auxiliary parts are only used to assist other part's positioning, is not the real statistic material of parts,so not contributing to the material statistics.";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "���⸨������";
		lpInfo->m_strPropHelp = "���⸨������ֻ���ڸ�������������λ�������ǽṹ����ʵ���е�ͳ�Ĺ��������Բ���Ԥ����ͳ�ơ�";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPart::GetPropID("m_bVirtualPart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
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
		pPropItem->m_idProp = CLDSPart::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstPart->m_bUserSpecColor)
		{	//������ɫ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Part Color";
			lpInfo->m_strPropHelp = "Part's color";
#else 
			lpInfo->m_strPropName = "������ɫ";
			lpInfo->m_strPropHelp = "������ɫ";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSPart::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstPart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		if(pFirstPart->IsShadowObject())
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
			pPropItem->m_idProp = CLDSPart::GetPropID("shadowInfo");
			pPropItem->m_bHideChildren = FALSE;
			if(CLDSPart::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSPart::GetPropStatus(pPropItem->m_idProp);
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
			pSonPropItem->m_idProp = CLDSPart::GetPropID("m_hMotherObj");
			if(pFirstPart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstPart->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstPart->IsBlockRefShadowObject())
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
				pSonPropItem->m_idProp = CLDSPart::GetPropID("m_hBlockRef");
				if(pFirstPart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstPart->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
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
			pPropItem->m_idProp = CLDSPart::GetPropID("m_hBlock");
			if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstPart->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
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
		pPropItem->m_idProp = CLDSPart::GetPropID("sNotes");
		if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
	}
	else
	{
		CPropTreeItem *pFindItem,*pParentItem;
		SetPartMutiObjsProp(pFirstPart,"handle",CLS_PART);
		SetPartMutiObjsProp(pFirstPart,"layer",CLS_PART);
		SetPartMutiObjsProp(pFirstPart,"cMaterial",CLS_PART);
		SetPartMutiObjsProp(pFirstPart,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstPart,"iSeg",CLS_PART);
		SetPartMutiObjsProp(pFirstPart,"sPartNo",CLS_PART);
		SetPartMutiObjsProp(pFirstPart,"cfgword",CLS_PART);
		SetPartMutiObjsProp(pFirstPart,"m_uStatMatCoef",CLS_PART);
		SetPartMutiObjsProp(pFirstPart,"m_hPartWeldParent",CLS_PART);
		SetPartMutiObjsProp(pFirstPart,"m_bDisplayHole",CLS_PART);
		SetPartMutiObjsProp(pFirstPart,"m_bVirtualPart",CLS_PART);
		SetPartMutiObjsProp(pFirstPart,"m_bUserSpecColor",CLS_PART);
		SetPartMutiObjsProp(pFirstPart,"crMaterial",CLS_PART);
		pFindItem=pPropList->FindItemByPropId(CLDSPart::GetPropID("m_hBlock"),&pParentItem);
		if (pFindItem)
		{
			if (pFirstPart->IsShadowObject())
			{	//����ͨ�����л���Ӱ�乹��
				pPropList->DeleteItemByPropId(CLDSPart::GetPropID("m_hBlock"));
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
				pPropItem->m_idProp = CLDSPart::GetPropID("shadowInfo");
				if(CLDSPart::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSPart::GetPropStatus(pPropItem->m_idProp);
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
				pSonPropItem->m_idProp = CLDSPart::GetPropID("m_hMotherObj");
				if(pFirstPart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstPart->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				if (pFirstPart->IsBlockRefShadowObject())
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
					pSonPropItem->m_idProp = CLDSPart::GetPropID("m_hBlockRef");
					if(pFirstPart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstPart->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstPart,"m_hBlock",CLS_PART);
			}
		}
		pFindItem=pPropList->FindItemByPropId(CLDSPart::GetPropID("shadowInfo"),&pParentItem);
		if (pFindItem)
		{
			if (!pFirstPart->IsShadowObject())
			{	//��Ӱ�乹���л�����ͨ����
				pPropList->DeleteItemByPropId(CLDSPart::GetPropID("shadowInfo"));
				pPropList->DeleteItemByPropId(CLDSPart::GetPropID("m_hMotherObj"));
				pFindItem=pPropList->FindItemByPropId(CLDSPart::GetPropID("m_hBlockRef"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSPart::GetPropID("m_hBlockRef"));
				pFindItem=pPropList->FindItemByPropId(CLDSPart::GetPropID("m_hBlock"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSPart::GetPropID("m_hBlock"));
				if(pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstPart,"m_hBlock",CLS_PART);
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
					pPropItem->m_idProp = CLDSPart::GetPropID("m_hBlock");
					if(pFirstPart->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pPropItem->m_idProp);
					pPropItem->SetReadOnly(pFirstPart->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstPart,"m_hMotherObj",CLS_PART);
				pFindItem=pPropList->FindItemByPropId(CLDSPart::GetPropID("m_hBlockRef"),&pParentItem);
				if (pFindItem&&pFirstPart->IsBlockRefShadowObject())
				{
					SetPartMutiObjsProp(pFirstPart,"m_hBlockRef",CLS_PART);
					SetPartMutiObjsProp(pFirstPart,"m_hBlock",CLS_PART);
				}
				else if (pFindItem&&!pFirstPart->IsBlockRefShadowObject())
				{
					pPropList->DeleteItemByPropId(CLDSPart::GetPropID("m_hBlockRef"));
					pPropList->DeleteItemByPropId(CLDSPart::GetPropID("m_hBlock"));
				}
				else if(pFindItem==NULL&&pFirstPart->IsBlockRefShadowObject())
				{
					pPropList->FindItemByPropId(CLDSPart::GetPropID("m_hMotherObj"),&pParentItem);
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
					pSonPropItem->m_idProp = CLDSPart::GetPropID("m_hBlockRef");
					if(pFirstPart->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPart::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstPart->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
		}
		SetPartMutiObjsProp(pFirstPart,"sNotes",CLS_PART);
	}
	pPropList->Redraw();
}
void CLDSView::DisplayPolyJointProperty(BOOL bRebuild)
{
	CLDSPolyJoint *pFirstPolyJoint = (CLDSPolyJoint*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	char sText[MAX_TEMP_CHAR_100+1]="";
	if(bRebuild)
	{//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(0);
		pPropDlg->RefreshTabCtrl(0);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_PART;
		pPropList->SetModifyValueFunc(ModifyPolyJointProperty);
		pPropList->SetButtonClickFunc(PolyJointButtonClick);
		pPropList->SetModifyStatusFunc(ModifyPolyJointItemStatus);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstPolyJoint->BelongModel()->IsTowerModel()||pFirstPolyJoint->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//������Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Bend Point Basic Information";
		lpInfo->m_strPropHelp = "The basic information of bending point.";
#else 
		lpInfo->m_strPropName = "�������ε������Ϣ";
		lpInfo->m_strPropHelp = "�������ε������Ϣ";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSPolyJoint::GetPropID("basicInfo");
		if(CLDSPolyJoint::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSPolyJoint::GetPropStatus(pGroupItem->m_idProp);
			//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
#ifdef __TMA_
		lpInfo->m_strPropHelp = "Part's handle is typical unique identifier string in the  TMA system.";
#elif defined(__LMA_)
		lpInfo->m_strPropHelp = "Part's handle is typical unique identifier string in the  LMA system.";
#elif defined(__TSA_)
		lpInfo->m_strPropHelp = "Part's handle is typical unique identifier string in the  TSA system.";
#else
		lpInfo->m_strPropHelp = "Part's handle is typical unique identifier string in the  LDS system.";
#endif
#else 
		lpInfo->m_strPropName = "���";
#ifdef __TMA_
		lpInfo->m_strPropHelp = "�����TMAϵͳ�жԹ�����ȫ��Ψһ��ʶ";
#elif defined(__LMA_)
		lpInfo->m_strPropHelp = "�����LMAϵͳ�жԹ�����ȫ��Ψһ��ʶ";
#elif defined(__TSA_)
		lpInfo->m_strPropHelp = "�����TSAϵͳ�жԹ�����ȫ��Ψһ��ʶ";
#else
		lpInfo->m_strPropHelp = "�����LDSϵͳ�жԹ�����ȫ��Ψһ��ʶ";
#endif
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("handle");
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
			//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "Part in the layer";
#else 
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "��������ͼ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("layer");
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
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
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("iSeg");
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
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
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("cfgword");
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
		/*	//����	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "����";
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390|Q420|Q460";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("cMaterial");
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�������";
		lpInfo->m_strPropHelp = "�������"; 
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("sPartNo");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//ͳ��ϵ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ͳ��ϵ��";
		lpInfo->m_strPropHelp = "ͳ��ϵ��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("m_uStatMatCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		  //���Ӹ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "���Ӹ�����";
		lpInfo->m_strPropHelp = "����˹�����Ҫ���ӵ�����������,��ô�����Ծ��Ǳ������������ĺ��ӹ����ľ��;����˹�������Ҫ����,��ֵΪ0X0��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("m_hPartWeldParent");
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
			//��ʾ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��ʾ��";
		lpInfo->m_strPropHelp = "�Ƿ���ʾ��";
		lpInfo->m_cmbItems = "��|��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("m_bDisplayHole");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//���⸨������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "���⸨������";
		lpInfo->m_strPropHelp = "���⸨������ֻ���ڸ�������������λ�������ǽṹ����ʵ���е�ͳ�Ĺ��������Բ���Ԥ����ͳ�ơ�";
		lpInfo->m_cmbItems = "��|��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("m_bVirtualPart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;*/
		//�����뾶
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Bending Radius";
		lpInfo->m_strPropHelp = "The inner arc radius of bending node(mm)"; 
#else 
		lpInfo->m_strPropName = "�����뾶";
		lpInfo->m_strPropHelp = "����ʱ���������Բ���뾶(mm)"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("rBending");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		//������׼֫
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Bending Datum Leg";
		lpInfo->m_strPropHelp = "Bending datum leg";
		lpInfo->m_cmbItems = "0.X Leg|1.Y Leg|2.Double Leg";
#else 
		lpInfo->m_strPropName = "������׼֫";
		lpInfo->m_strPropHelp = "������׼֫";
		lpInfo->m_cmbItems = "0.X֫|1.Y֫|2.˫֫";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("huoqu_base_wing_x0_y1_both2");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		//ǰһ�������ӹ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Previous Bending Child Part";
		lpInfo->m_strPropHelp = "The previous bending child part"; 
#else 
		lpInfo->m_strPropName = "ǰһ�������ӹ���";
		lpInfo->m_strPropHelp = "ǰһ�������ӹ���"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("pPrevLinePart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		//��һ�������ӹ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Next Bending Child Part";
		lpInfo->m_strPropHelp = "The next bending child part."; 
#else 
		lpInfo->m_strPropName = "��һ�������ӹ���";
		lpInfo->m_strPropHelp = "��һ�������ӹ���"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("pNextLinePart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
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
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstPolyJoint->m_bUserSpecColor)
		{	//������ɫ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Part Color";
			lpInfo->m_strPropHelp = "Part's color";
#else 
			lpInfo->m_strPropName = "������ɫ";
			lpInfo->m_strPropHelp = "������ɫ";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSPolyJoint::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstPolyJoint->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
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
		pPropItem->m_idProp = CLDSPolyJoint::GetPropID("sNotes");
		if(pFirstPolyJoint->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPolyJoint::IsPropEqual(selectObjs,pPropItem->m_idProp);
	}
	else
	{
		SetPartMutiObjsProp(pFirstPolyJoint,"handle");
		SetPartMutiObjsProp(pFirstPolyJoint,"layer");
		SetPartMutiObjsProp(pFirstPolyJoint,"cMaterial");
		SetPartMutiObjsProp(pFirstPolyJoint,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstPolyJoint,"specification");
		SetPartMutiObjsProp(pFirstPolyJoint,"iSeg");
		SetPartMutiObjsProp(pFirstPolyJoint,"sPartNo");
		SetPartMutiObjsProp(pFirstPolyJoint,"cfgword");
		SetPartMutiObjsProp(pFirstPolyJoint,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstPolyJoint,"m_hPartWeldParent");
		SetPartMutiObjsProp(pFirstPolyJoint,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstPolyJoint,"m_bVirtualPart");
		SetPartMutiObjsProp(pFirstPolyJoint,"m_bUserSpecColor");
	}
	pPropList->Redraw();
}
#endif
