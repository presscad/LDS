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
#include "PropertyListOper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __H_SHAPE_STEEL_
BOOL SetPartMutiObjsProp(CLDSObject *pObj, char *propName,long idClassType=0);
void SetPropItemReadOnly(CLDSDbObject *pObj,char *propName,BOOL bReadOnly);

CXhChar16 GetHShapeSpec(BYTE bHShapeType)
{
	CXhChar16 text;
	if(bHShapeType==0)
		_snprintf(text,MAX_TEMP_CHAR_100,"%gX%g",hw_guige_table[0].h,hw_guige_table[0].b);
	else if(bHShapeType==1)
		_snprintf(text,MAX_TEMP_CHAR_100,"%gX%g",hm_guige_table[0].h,hm_guige_table[0].b);
	else if(bHShapeType==2)
		_snprintf(text,MAX_TEMP_CHAR_100,"%gX%g",hn_guige_table[0].h,hn_guige_table[0].b);
	else if(bHShapeType==3)
		_snprintf(text,MAX_TEMP_CHAR_100,"%gX%g",hp_guige_table[0].h,hp_guige_table[0].b);
	return text;
}
BOOL ModifyLineHShapeItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSLineHShape::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
BOOL LineHShapeButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem, *pParentItem;
	CLDSLineHShape *pLineHShape=NULL, *pFirstLineHShape=NULL;
	CReportVectorDlg vecDlg;
	CReportPointDlg pointDlg;
	CCfgPartNoDlg cfgdlg;
	COddmentDesignDlg odddlg;
	char sText[MAX_TEMP_CHAR_100+1];
	CString valueStr = _T("");
	if(pView->selectObjs.GetNodeNum()>0)
		pFirstLineHShape = (CLDSLineHShape*)pView->selectObjs.GetFirst();
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
	if(pItem->m_idProp==CLDSLineHShape::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstLineHShape->cfgword; 
		if(pFirstLineHShape->layer(0)!='L')
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
		{
			for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			{
				pLineHShape->cfgword = cfgdlg.cfgword;			//�ڴ˴����µ�һ���ڵ������Ϻ�
				pLineHShape->SetModified(TRUE,FALSE);
				pLineHShape->SyncMirProp("cfgword");
			}
		}
		if(pFirstLineHShape->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("sPartNo"))
	{
		CModifyRelativeObjPartNoDlg modify_partno_dlg;
		modify_partno_dlg.m_pWorkPart=pFirstLineHShape;
		if(modify_partno_dlg.DoModal()==IDOK)
		{
			pFirstLineHShape->SetPartNo(modify_partno_dlg.m_sCurObjPartNo.GetBuffer(0));
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
			SEGI iSeg;
			if(CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(modify_partno_dlg.m_sCurObjPartNo,&iSeg,NULL))
			{
				pFirstLineHShape->iSeg=iSeg;
				pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("iSeg"),iSeg.ToString());
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("nearWorkPlaneNorm"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Near Work Plane Normal";
#else 
		vecDlg.m_sCaption = "�۸ֻ�׼����Ʒ�������";
#endif
		vecDlg.m_fVectorX = pFirstLineHShape->nearWorkPlaneNorm.x;
		vecDlg.m_fVectorY = pFirstLineHShape->nearWorkPlaneNorm.y;
		vecDlg.m_fVectorZ = pFirstLineHShape->nearWorkPlaneNorm.z;
		pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("nearWorkPlaneNorm.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("nearWorkPlaneNorm.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("nearWorkPlaneNorm.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("nearWorkPlaneNorm.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("nearWorkPlaneNorm.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("nearWorkPlaneNorm.z"),CString(sText));
			for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			{	//���¼���۸ַ��߷���
				pLineHShape->nearWorkPlaneNorm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				pLineHShape->CalWorkPlaneNorm();
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("workPlaneNorm")
		||pItem->m_idProp==CLDSLineHShape::GetPropID("workPlaneNorm.x")
		||pItem->m_idProp==CLDSLineHShape::GetPropID("workPlaneNorm.y")
		||pItem->m_idProp==CLDSLineHShape::GetPropID("workPlaneNorm.z"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "U-Steel Datum Plane Normal";
#else 
		vecDlg.m_sCaption = "�۸ֻ�׼�淨������";
#endif
		vecDlg.m_fVectorX = pFirstLineHShape->WorkPlaneNorm().x;
		vecDlg.m_fVectorY = pFirstLineHShape->WorkPlaneNorm().y;
		vecDlg.m_fVectorZ = pFirstLineHShape->WorkPlaneNorm().z;
		pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("workPlaneNorm.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("workPlaneNorm.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("workPlaneNorm.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("workPlaneNorm.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("workPlaneNorm.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("workPlaneNorm.z"),CString(sText));
			for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
				pLineHShape->SetWorkPlaneNorm(f3dPoint(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ));
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("hStart"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Rod Start Node";
		modifyhdlg.m_sTitle="Start Node:";
#else 
		modifyhdlg.m_sCaption="�޸ĸ˼�ʼ�˽ڵ�";
		modifyhdlg.m_sTitle="ʼ�˽ڵ�:";
#endif
		if(pFirstLineHShape->pStart)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineHShape->pStart->handle);
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
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("hStart"),valueStr);
				for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
				{
					pLineHShape->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),pNode,NULL);
					pLineHShape->pStart=pNode;
					pLineHShape->SetModified();
					pLineHShape->ClearFlag();
					pLineHShape->CalPosition();
					pLineHShape->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					g_pSolidDraw->NewSolidPart(pLineHShape->GetSolidPartObject());
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("hEnd"))
	{
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Rod End Node";
		modifyhdlg.m_sTitle="End Node:";
#else 
		modifyhdlg.m_sCaption="�޸ĸ˼��ն˽ڵ�";
		modifyhdlg.m_sTitle="�ն˽ڵ�:";
#endif
		if(pFirstLineHShape->pEnd)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineHShape->pEnd->handle);
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
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("hEnd"),valueStr);
				for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
				{
					pLineHShape->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),NULL,pNode);
					pLineHShape->pEnd=pNode;
					pLineHShape->SetModified();
					pLineHShape->ClearFlag();
					pLineHShape->CalPosition();
					pLineHShape->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					g_pSolidDraw->NewSolidPart(pLineHShape->GetSolidPartObject());
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("oddStart"))
	{
		odddlg.m_hRefPart1.Format("0X%X",pFirstLineHShape->desStartOdd.m_hRefPart1);
		odddlg.m_hRefPart2.Format("0X%X",pFirstLineHShape->desStartOdd.m_hRefPart2);
		odddlg.m_fCollideDist = pFirstLineHShape->desStartOdd.m_fCollideDist;
		odddlg.m_iOddCalStyle = pFirstLineHShape->desStartOdd.m_iOddCalStyle;
		if(odddlg.DoModal()==IDOK)
		{
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart1);
			sscanf(sText,"%X",&pFirstLineHShape->desStartOdd.m_hRefPart1);
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart2);
			sscanf(sText,"%X",&pFirstLineHShape->desStartOdd.m_hRefPart2);
			pFirstLineHShape->desStartOdd.m_fCollideDist = odddlg.m_fCollideDist;
			pFirstLineHShape->desStartOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
			pFirstLineHShape->CalStartOddment();
			char valueStr[MAX_TEMP_CHAR_50+1]="";
			if(pFirstLineHShape->GetPropValueStr(pItem->m_idProp,valueStr)>0)
				pItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("startPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "Start Design Coordinate";
#else 
		pointDlg.m_sCaption = "�۸�ʼ���������";
#endif
		pointDlg.m_fPointX = pFirstLineHShape->Start().x;
		pointDlg.m_fPointY = pFirstLineHShape->Start().y;
		pointDlg.m_fPointZ = pFirstLineHShape->Start().z;
		if(pFirstLineHShape->desStartPos.endPosType!=4)	//4.ָ������
			pointDlg.m_bReadOnly=TRUE;
		pointDlg.m_bLockPoint=pFirstLineHShape->IsStartPosLocked();
		if(pointDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("startPos.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("startPos.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("startPos.z"),CString(sText));
			for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			{
				pLineHShape->SetStart(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
				if(pointDlg.m_bLockPoint)
					pLineHShape->LockStartPos();
				else
					pLineHShape->UnlockStartPos();
			}
		}
		//ʼ������
		if(pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart)
		{	//����������Ϊֻ������
			SetPropItemReadOnly(pFirstLineHShape,"startPos.x",TRUE);
			SetPropItemReadOnly(pFirstLineHShape,"startPos.y",TRUE);
			SetPropItemReadOnly(pFirstLineHShape,"startPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ����ֻ������
			SetPropItemReadOnly(pFirstLineHShape,"startPos.x",pFirstLineHShape->desStartPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineHShape,"startPos.y",pFirstLineHShape->desStartPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineHShape,"startPos.z",pFirstLineHShape->desStartPos.endPosType!=4);	//4.ָ������
		}
		SetPropItemReadOnly(pFirstLineHShape,"hStart",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineHShape,"oddStart",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineHShape,"desStartPos",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineHShape,"desStartPos.endPosType",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineHShape,"desStartPos.datum_pos_style",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineHShape,"desStartPos.hDatumPart",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineHShape,"desStartPos.fNormOffset",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineHShape,"desStartPos.fEccentricDist",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineHShape,"desStartPos.face_offset_norm",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineHShape,"desStartPos.hDatumStartPart",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineHShape,"desStartPos.bDatumStartStartEnd",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineHShape,"desStartPos.hDatumEndPart",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineHShape,"desStartPos.bDatumEndStartEnd",pFirstLineHShape->IsStartPosLocked()||pFirstLineHShape->bHuoQuStart);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("oddEnd"))
	{
		odddlg.m_hRefPart1.Format("0X%X",pFirstLineHShape->desEndOdd.m_hRefPart1);
		odddlg.m_hRefPart2.Format("0X%X",pFirstLineHShape->desEndOdd.m_hRefPart2);
		odddlg.m_fCollideDist = pFirstLineHShape->desEndOdd.m_fCollideDist;
		odddlg.m_iOddCalStyle = pFirstLineHShape->desEndOdd.m_iOddCalStyle;
		if(odddlg.DoModal()==IDOK)
		{
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart1);
			sscanf(sText,"%X",&pFirstLineHShape->desEndOdd.m_hRefPart1);
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart2);
			sscanf(sText,"%X",&pFirstLineHShape->desEndOdd.m_hRefPart2);
			pFirstLineHShape->desEndOdd.m_fCollideDist = odddlg.m_fCollideDist;
			pFirstLineHShape->desEndOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
			pFirstLineHShape->CalEndOddment();
			char valueStr[MAX_TEMP_CHAR_50+1]="";
			if(pFirstLineHShape->GetPropValueStr(pItem->m_idProp,valueStr)>0)
				pItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("actualStartPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "U-Steel Start Coordinate";
#else 
		pointDlg.m_sCaption = "�۸�ʼ��ʵ������";
#endif
		pointDlg.m_bEnableLock=FALSE;
		pointDlg.m_bReadOnly=TRUE;
		f3dPoint actualStart,vec=pFirstLineHShape->Start()-pFirstLineHShape->End();
		normalize(vec);
		actualStart = pFirstLineHShape->Start()+vec*pFirstLineHShape->startOdd();
		pointDlg.m_fPointX = actualStart.x;
		pointDlg.m_fPointY = actualStart.y;
		pointDlg.m_fPointZ = actualStart.z;
		pointDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("actualEndPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "U-Steel End Coordinate";
#else 
		pointDlg.m_sCaption = "�۸��ն�ʵ������";
#endif
		pointDlg.m_bEnableLock=FALSE;
		pointDlg.m_bReadOnly=TRUE;
		f3dPoint actualStart,vec=pFirstLineHShape->Start()-pFirstLineHShape->End();
		normalize(vec);
		actualStart = pFirstLineHShape->Start()+vec*pFirstLineHShape->startOdd();
		pointDlg.m_fPointX = actualStart.x;
		pointDlg.m_fPointY = actualStart.y;
		pointDlg.m_fPointZ = actualStart.z;
		pointDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("endPos"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pointDlg.m_sCaption = "U-Steel End Design Coordinate";
#else 
		pointDlg.m_sCaption = "�۸��ն��������";
#endif
		pointDlg.m_fPointX = pFirstLineHShape->End().x;
		pointDlg.m_fPointY = pFirstLineHShape->End().y;
		pointDlg.m_fPointZ = pFirstLineHShape->End().z;
		if(pFirstLineHShape->desEndPos.endPosType!=4)	//4.ָ������
			pointDlg.m_bReadOnly=TRUE;
		pointDlg.m_bLockPoint=pFirstLineHShape->IsEndPosLocked();
		if(pointDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("endPos.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("endPos.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("endPos.z"),CString(sText));
			for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			{
				pLineHShape->SetEnd(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
				if(pointDlg.m_bLockPoint)
					pLineHShape->LockEndPos();
				else
					pLineHShape->UnlockEndPos();
			}
		}
		//�ն�����
		if(pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd)
		{	//����������Ϊֻ������
			SetPropItemReadOnly(pFirstLineHShape,"endPos.x",TRUE);
			SetPropItemReadOnly(pFirstLineHShape,"endPos.y",TRUE);
			SetPropItemReadOnly(pFirstLineHShape,"endPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ����ֻ������
			SetPropItemReadOnly(pFirstLineHShape,"endPos.x",pFirstLineHShape->desEndPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineHShape,"endPos.y",pFirstLineHShape->desEndPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineHShape,"endPos.z",pFirstLineHShape->desEndPos.endPosType!=4);	//4.ָ������
		}
		SetPropItemReadOnly(pFirstLineHShape,"hEnd",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineHShape,"oddEnd",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineHShape,"desEndPos",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineHShape,"desEndPos.endPosType",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineHShape,"desEndPos.datum_pos_style",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineHShape,"desEndPos.hDatumPart",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineHShape,"desEndPos.fNormOffset",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineHShape,"desEndPos.fEccentricDist",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineHShape,"desEndPos.face_offset_norm",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineHShape,"desEndPos.hDatumStartPart",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineHShape,"desEndPos.bDatumStartStartEnd",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineHShape,"desEndPos.hDatumEndPart",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineHShape,"desEndPos.bDatumEndStartEnd",pFirstLineHShape->IsEndPosLocked()||pFirstLineHShape->bHuoQuEnd);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("relativeObjs"))
	{
		CRelativeObjectDlg relativeDlg;
		relativeDlg.m_pWorkObj = pFirstLineHShape;
		relativeDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("shadowInfo"))
	{
		CDesignShadowDlg shadowdlg;
		shadowdlg.SetShadowObj((CLDSObject*)pFirstLineHShape);
		shadowdlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumPart")||pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.hDatumPart"))
	{
		//ͨ��ѡ�񹹼��޸Ĵ��λ�û�׼�˼�
		CModifyPartHandleDlg modifyhdlg;
		CTubeEndPosPara *pHShapeEndPosPara=NULL;
		if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumPart"))
		{
			pHShapeEndPosPara=&(pFirstLineHShape->desStartPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineHShape->desStartPos.hDatumPart);
		}
		else
		{
			pHShapeEndPosPara=&(pFirstLineHShape->desEndPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineHShape->desEndPos.hDatumPart);
		}
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		if(pHShapeEndPosPara&&pHShapeEndPosPara->endPosType==3)
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
		if(pHShapeEndPosPara&&pHShapeEndPosPara->endPosType==3)
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
				if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumPart"))
					pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("desStartPos.hDatumPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("desEndPos.hDatumPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineHShape* pMirLineHShape=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineHShape=(CLDSLineHShape*)pLineHShape->GetMirPart(msg);
						if(pMirLineHShape)
						{
							if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumPart"))
								pMirLineHShape->desStartPos.hDatumPart=pPart->handle;
							else
								pMirLineHShape->desEndPos.hDatumPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumPart"))
						pLineHShape->desStartPos.hDatumPart=pPart->handle;
					else
						pLineHShape->desEndPos.hDatumPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineHShape->handle);
				}
				if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumPart"))
				{
					pParentItem=pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("desStartPos.endPosType"),NULL);
					//if(pParentItem)
						//UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
				}
				else
				{
					pParentItem=pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("desEndPos.endPosType"),NULL);
					//if(pParentItem)
						//UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumStartPart")
		||pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.hDatumStartPart"))
	{	//�޸�ͶӰ��ʼ�����ڻ�׼�˼�
		CModifyPartHandleDlg modifyhdlg;
		CTubeEndPosPara *pEndPosPara=NULL;
		if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumStartPart"))
		{
			pEndPosPara=&(pFirstLineHShape->desStartPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineHShape->desStartPos.hDatumStartPart);
		}
		else
		{
			pEndPosPara=&(pFirstLineHShape->desEndPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineHShape->desEndPos.hDatumStartPart);
		}
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
		if(pEndPosPara&&pEndPosPara->endPosType==3)
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
		if(pEndPosPara&&pEndPosPara->endPosType==3)
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
				if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumStartPart"))
					pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("desStartPos.hDatumStartPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("desEndPos.hDatumStartPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineHShape* pMirLineHShape=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineHShape=(CLDSLineHShape*)pLineHShape->GetMirPart(msg);
						if(pMirLineHShape)
						{
							if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumStartPart"))
								pMirLineHShape->desStartPos.hDatumStartPart=pPart->handle;
							else
								pMirLineHShape->desEndPos.hDatumStartPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumStartPart"))
						pFirstLineHShape->desStartPos.hDatumStartPart=pPart->handle;
					else
						pFirstLineHShape->desEndPos.hDatumStartPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineHShape->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumEndPart")
		||pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.hDatumEndPart"))
	{	//�޸�ͶӰ���ն����ڻ�׼�˼�
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify End Datum Rod";
		modifyhdlg.m_sTitle="Datum Rod:";
#else 
		modifyhdlg.m_sCaption="�޸�ͶӰ�ն����ڻ�׼�˼�";
		modifyhdlg.m_sTitle="��׼�˼�:";
#endif
		if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumEndPart"))
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineHShape->desStartPos.hDatumEndPart);
		else
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineHShape->desEndPos.hDatumEndPart);
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
				if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumEndPart"))
					pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("desStartPos.hDatumEndPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("desEndPos.hDatumEndPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineHShape* pMirLineHShape=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineHShape=(CLDSLineHShape*)pLineHShape->GetMirPart(msg);
						if(pMirLineHShape)
						{
							if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumEndPart"))
								pMirLineHShape->desStartPos.hDatumEndPart=pPart->handle;
							else
								pMirLineHShape->desEndPos.hDatumEndPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumEndPart"))
						pFirstLineHShape->desStartPos.hDatumEndPart=pPart->handle;
					else
						pFirstLineHShape->desEndPos.hDatumEndPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineHShape->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("hWorkNormRefPart"))
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
		modifyhdlg.m_sHandle.Format("0X%X",pFirstLineHShape->hWorkNormRefPart);
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
				pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("hWorkNormRefPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineHShape* pMirLineHShape=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineHShape=(CLDSLineHShape*)pLineHShape->GetMirPart(msg);
						if(pMirLineHShape)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//������ѡ���߲��ո˼��ĶԳƹ��� wht 11-05-07
								pMirLineHShape->hWorkNormRefPart=pMirPart->handle;
						}
					}
					pLineHShape->hWorkNormRefPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineHShape->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("m_hPartWeldParent"))
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
		modifyhdlg.m_sHandle.Format("0X%X",pFirstLineHShape->m_hPartWeldParent);
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
				pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("m_hPartWeldParent"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLinePart* pMirLinePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLinePart=(CLDSLinePart*)pLineHShape->GetMirPart(msg);
						if(pMirLinePart)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//������ѡ�������ĶԳƹ��� wht 11-05-07
								pMirLinePart->m_hPartWeldParent=pMirPart->handle;
						}
					}
					pLineHShape->m_hPartWeldParent=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineHShape->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.face_offset_norm"))
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Offset Normal";
#else 
		vecDlg.m_sCaption = "ƽ���淨��";
#endif
		vecDlg.m_fVectorX = pFirstLineHShape->desStartPos.face_offset_norm.x;
		vecDlg.m_fVectorY = pFirstLineHShape->desStartPos.face_offset_norm.y;
		vecDlg.m_fVectorZ = pFirstLineHShape->desStartPos.face_offset_norm.z;
		if(vecDlg.DoModal()==IDOK)
		{
			for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
				pLineHShape->desStartPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.face_offset_norm"))
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Offset Normal";
#else 
		vecDlg.m_sCaption = "ƽ���淨��";
#endif
		vecDlg.m_fVectorX = pFirstLineHShape->desEndPos.face_offset_norm.x;
		vecDlg.m_fVectorY = pFirstLineHShape->desEndPos.face_offset_norm.y;
		vecDlg.m_fVectorZ = pFirstLineHShape->desEndPos.face_offset_norm.z;
		if(vecDlg.DoModal()==IDOK)
		{
			for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
				pLineHShape->desEndPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else
		return FALSE;
	for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
	{
		pLineHShape->SetModified();
		pLineHShape->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pLineHShape->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}
BOOL ModifyLineHShapeProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineHShape *pCurHShape = (CLDSLineHShape*)pView->selectObjs.GetFirst();
	CLDSLineHShape *pLineHShape = NULL;
	CPropertyListOper<CLDSLineHShape> oper(pPropList,pCurHShape);
	CPropTreeItem *pFindItem,*pSonPropItem,*pParentItem;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	const int GROUP_GENERAL=1,GROUP_START=2,GROUP_END=3;
	if(pItem->m_idProp==CLDSLineHShape::GetPropID("layer"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			pLineHShape->SetLayer(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//��ת�Գ�
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					CalMirAtomLayer(pLineHShape->layer(),pMirLineHShape->layer(),pMirObj->mirInfo);
			}
		}	
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("cMaterial"))
	{	//H�͸ֲ���
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			pLineHShape->cMaterial = QuerySteelBriefMatMark(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					pMirLineHShape->cMaterial=pLineHShape->cMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("m_cQualityLevel"))
	{	//�����ȼ�
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Default")==0)
				pLineHShape->m_cQualityLevel=0;
			else
#else
			if(valueStr.CompareNoCase("Ĭ��")==0)
				pLineHShape->m_cQualityLevel=0;
			else
#endif
				pLineHShape->m_cQualityLevel = valueStr[0];
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					pMirLineHShape->m_cQualityLevel=pLineHShape->m_cQualityLevel;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("ibHShapeType"))
	{	// H�͸����
		int iShapeType=tem_str[0]-'0';
		if(iShapeType==pCurHShape->m_ibHShapeType)
			return TRUE;
		pCurHShape->m_ibHShapeType=iShapeType;
		SIZESPEC size_spec;
		CXhChar16 tem_spec=GetHShapeSpec(iShapeType);
		ParseHShapeSpec(tem_spec,iShapeType,size_spec);
		pCurHShape->size_height=size_spec.height;
		pCurHShape->size_wide=size_spec.wide;
		pCurHShape->size_thick=size_spec.thick;
		pCurHShape->size_thick2=size_spec.thick2;
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			pLineHShape->m_ibHShapeType=pCurHShape->m_ibHShapeType;
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineHShape)
				{
					pMirLineHShape->m_ibHShapeType=pLineHShape->m_ibHShapeType;
					if(iShapeType==pLineHShape->m_ibHShapeType)
						return TRUE;
					tem_spec=GetHShapeSpec(iShapeType);
					ParseHShapeSpec(tem_spec,iShapeType,size_spec);
					pMirLineHShape->size_height=pLineHShape->size_height;
					pMirLineHShape->size_wide=pLineHShape->size_wide;
					pMirLineHShape->size_thick=pLineHShape->size_thick;
					pMirLineHShape->size_thick2=pLineHShape->size_thick2;
				}
			}
		}
		//���½���
		CPropTreeItem* pFindItem=pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("specification"),NULL);
		if(pFindItem)
		{
			pFindItem->m_lpNodeInfo->m_cmbItems=MakeHShapeSetString(iShapeType);
			if(pCurHShape->GetPropValueStr(CLDSLineHShape::GetPropID("specification"),tem_str)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=tem_str;
		}
		if(pCurHShape->GetPropValueStr(CLDSLineHShape::GetPropID("thick1"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("thick1"),CString(tem_str));
		if(pCurHShape->GetPropValueStr(CLDSLineHShape::GetPropID("thick2"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("thick2"),CString(tem_str));
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("specification"))
	{	//H�͸ֹ��
		//SLOT_PARA_TYPE *pSlotPara=FindSlotType(tem_str);
		//if(pSlotPara==NULL)
			//return FALSE;
		SIZESPEC size_spec;
		ParseHShapeSpec(tem_str,pCurHShape->m_ibHShapeType,size_spec);
		pCurHShape->size_height=size_spec.height;
		pCurHShape->size_wide=size_spec.wide;
		pCurHShape->size_thick=size_spec.thick;
		pCurHShape->size_thick2=size_spec.thick2;
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			_snprintf(pLineHShape->spec,MAX_CHAR_GUIGE_16,"%s",tem_str);
			/*pLineHShape->size_wide=pSlotPara->b;
			pLineHShape->size_thick=pSlotPara->d;
			pLineHShape->size_height=pSlotPara->h;
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineSlot=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINESLOT);
				if(pMirLineSlot)
				{
					strcpy(pMirLineSlot->spec,pLineHShape->spec);
					pMirLineSlot->size_wide=pLineHShape->GetWidth();
					pMirLineSlot->size_thick=pLineHShape->GetThick();
					pMirLineSlot->size_height=pLineHShape->GetHeight();
				}
			}*/
		}
		/*if(pCurHShape->GetPropValueStr(CLDSLineHShape::GetPropID("width"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("width"),CString(tem_str));
		if(pCurHShape->GetPropValueStr(CLDSLineHShape::GetPropID("thick"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("thick"),CString(tem_str));
		if(pCurHShape->GetPropValueStr(CLDSLineHShape::GetPropID("height"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("height"),CString(tem_str));*/
		if(pCurHShape->GetPropValueStr(CLDSLineHShape::GetPropID("thick1"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("thick1"),CString(tem_str));
		if(pCurHShape->GetPropValueStr(CLDSLineHShape::GetPropID("thick2"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("thick2"),CString(tem_str));
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("iSeg"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			pLineHShape->iSeg = SEGI(valueStr.GetBuffer());
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					pMirLineHShape->iSeg=pLineHShape->iSeg;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("sPartNo"))
	{
		bool parsed=false;
		SEGI iSeg;
		if(CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(tem_str,&iSeg,NULL))
		{
			parsed=true;
			pPropList->SetItemPropValue(CLDSLineHShape::GetPropID("iSeg"),iSeg.ToString());
		}
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{	
			if(parsed)
				pLineHShape->iSeg=iSeg;
			pLineHShape->SetPartNo(tem_str);
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("cfgword"))
	{
		//���������˼�����ĺ�
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			pLineHShape->cfgword = pCurHShape->cfgword;
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					pMirLineHShape->cfgword=pLineHShape->cfgword;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("m_uStatMatCoef"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->m_uStatMatCoef = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("m_hPartWeldParent"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineHShape->m_hPartWeldParent);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("m_bDisplayHole"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pLineHShape->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pLineHShape->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pLineHShape->m_bDisplayHole = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					pMirLineHShape->m_bDisplayHole=pLineHShape->m_bDisplayHole;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("m_bVirtualPart"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pLineHShape->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pLineHShape->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pLineHShape->m_bVirtualPart = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					pMirLineHShape->m_bVirtualPart=pLineHShape->m_bVirtualPart;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("width"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			pLineHShape->size_wide=atof(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					pMirLineHShape->size_wide=pLineHShape->GetWidth();
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("thick1"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{	
			pLineHShape->size_thick=atof(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					pMirLineHShape->size_thick=pLineHShape->GetThick();
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("thick2"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{	
			pLineHShape->size_thick2=atof(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					pMirLineHShape->size_thick2=pLineHShape->GetThick2();
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("height"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			pLineHShape->size_height=atof(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					pMirLineHShape->size_height=pLineHShape->GetHeight();
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("m_bUserSpecColor"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pLineHShape->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pLineHShape->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pLineHShape->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					pMirLineHShape->m_bUserSpecColor=pLineHShape->m_bUserSpecColor;
			}
		}
		if(pCurHShape->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("crMaterial"),NULL);
			if(pFindItem==NULL)
			{
				oper.InsertCmbColorPropItem(pItem,"crMaterial");
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSLineHShape::GetPropID("crMaterial"));
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			pLineHShape->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pLineHShape->relativeObjs.GetFirst();pMirObj;pMirObj=pLineHShape->relativeObjs.GetNext())
			{
				CLDSLineHShape *pMirLineHShape=(CLDSLineHShape*)pLineHShape->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEHSHAPE);
				if(pMirLineHShape)
					pMirLineHShape->crMaterial=pLineHShape->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("sNotes"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			_snprintf(pLineHShape->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
		}
	}
	/*else if(pItem->m_idProp==CLDSLineHShape::GetPropID("hAttachTube"))
	{
		long hLineTube=0;
		sscanf(tem_str,"%X",&hLineTube);
		pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();
		CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(hLineTube,CLS_LINETUBE);
		if(pLineTube==NULL)
			hLineTube=0;
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->hAttachTube=hLineTube;
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("length"))
	{
		double fLen=atof(valueStr);
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			if(pLineHShape->hAttachTube==0)
			{
				AfxMessageBox("�����������ֹ�!");
				return FALSE;
			}
			pLineHShape->fLen=fLen;
		}
		g_pSolidDraw->Draw();
	}*/
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("m_iNormCalStyle"))
	{
		char sText[MAX_TEMP_CHAR_100+1]="";
		pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("0.Specify Directly")==0)
			pLineHShape->m_iNormCalStyle = 0;
		else if(valueStr.CompareNoCase("1.Reference Rod")==0) 
			pLineHShape->m_iNormCalStyle = 1;
		else //if(valueStr.CompareNoCase("2.Horizontal Pedestal")==0)
			pLineHShape->m_iNormCalStyle = 2;
#else 
		if(valueStr.CompareNoCase("0.ֱ��ָ��")==0)
			pLineHShape->m_iNormCalStyle = 0;
		else if(valueStr.CompareNoCase("1.���ո˼�")==0) 
			pLineHShape->m_iNormCalStyle = 1;
		else //if (valueStr.CompareNoCase("2.ˮƽ����")==0)
			pLineHShape->m_iNormCalStyle = 2;
#endif
		pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("nearWorkPlaneNorm.x"),&pParentItem);
		pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("m_iNormCalStyle"),&pParentItem);//���»�ø��ڵ�
		if(pFindItem==NULL)
		{
			if(pLineHShape->m_iNormCalStyle == 1)
			{
				//��׼����Ʒ���
				pSonPropItem=oper.InsertButtonPropItem(pParentItem,"nearworkPlaneNorm");
				//X
				oper.InsertEditPropItem(pSonPropItem,"nearworkPlaneNorm.x");
				//Y
				oper.InsertEditPropItem(pSonPropItem,"nearworkPlaneNorm.y");
 				//Z
				oper.InsertEditPropItem(pSonPropItem,"nearworkPlaneNorm.z");
				//
				oper.InsertEditPropItem(pParentItem,"hWorkNormRefPart");
			}
			pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("workPlaneNorm.x"),&pParentItem);
			pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("workPlaneNorm.x"),&pParentItem);//���»�ȡһ�����ĸ��ڵ�
			if(pFindItem == NULL)
			{
				//���ߵ�X
				oper.InsertEditPropItem(pParentItem,"workPlaneNorm.x");
				//Y
				oper.InsertEditPropItem(pParentItem,"workPlaneNorm.y");
				//Z
				oper.InsertEditPropItem(pParentItem,"workPlaneNorm.z");
			}
		}
		else
		{
			if(pLineHShape->m_iNormCalStyle == 0||pLineHShape->m_iNormCalStyle == 2)
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("nearWorkPlaneNorm"),&pParentItem);
				if(pFindItem!=NULL)
				{
					pPropList->DeleteItemByPropId(CLDSLineHShape::GetPropID("nearWorkPlaneNorm"));
					pPropList->DeleteItemByPropId(CLDSLineHShape::GetPropID("nearWorkPlaneNorm.x"));
					pPropList->DeleteItemByPropId(CLDSLineHShape::GetPropID("nearWorkPlaneNorm.y"));
					pPropList->DeleteItemByPropId(CLDSLineHShape::GetPropID("nearWorkPlaneNorm.z"));
					pPropList->DeleteItemByPropId(CLDSLineHShape::GetPropID("hWorkNormRefPart"));
				}
			}
		}
		pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("workPlaneNorm.x"),&pParentItem);
		pFindItem->SetReadOnly(pLineHShape->m_iNormCalStyle==1||pLineHShape->m_iNormCalStyle==2);
		pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("workPlaneNorm.y"),&pParentItem);
		pFindItem->SetReadOnly(pLineHShape->m_iNormCalStyle==1||pLineHShape->m_iNormCalStyle==2);
		pFindItem = pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("workPlaneNorm.z"),&pParentItem);
		pFindItem->SetReadOnly(pLineHShape->m_iNormCalStyle==1||pLineHShape->m_iNormCalStyle==2);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("nearWorkPlaneNorm.x"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{	//���¼��㷨�߷���
			pLineHShape->nearWorkPlaneNorm.x = atof(valueStr);
			pLineHShape->CalWorkPlaneNorm();
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("nearWorkPlaneNorm.y"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{	//���¼��㷨�߷���
			pLineHShape->nearWorkPlaneNorm.y = atof(valueStr);
			pLineHShape->CalWorkPlaneNorm();
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("nearWorkPlaneNorm.z"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{	//���¼��㷨�߷���
			pLineHShape->nearWorkPlaneNorm.z = atof(valueStr);
			pLineHShape->CalWorkPlaneNorm();
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("hWorkNormRefPart"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineHShape->hWorkNormRefPart);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("oddStart"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{	//����ֶ������˸˼�����ͷӦ���ȣ�������ͷ�ļ��㷽ʽ�޸�Ϊ������
			//�����������ø˼���صĸְ�ʱ�������¼�������ͷ��ֵ	wht 09-09-02
			pLineHShape->desStartOdd.m_iOddCalStyle=2;
			pLineHShape->SetStartOdd(atof(valueStr));//,TRUE,TRUE);H�͸ִ��벻���ƣ��ݲ�������˨�������塡wjh-2018.1.8
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("startPos.x"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->pStart->SetPositionX(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("startPos.y"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->pStart->SetPositionY(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("startPos.z"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->pStart->SetPositionZ(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.endPosType"))
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
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->desStartPos.endPosType = end_pos_type;
		//UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.datum_pos_style"))
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
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->desStartPos.datum_pos_style = datum_pos_style;
		pParentItem=pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("desStartPos.endPosType"),NULL);
		//if(pParentItem)
			//UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
		if(datum_pos_style>2)
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Offset Normal";
#else 
			vecDlg.m_sCaption = "ƽ���淨��";
#endif
			vecDlg.m_fVectorX = pCurHShape->desStartPos.face_offset_norm.x;
			vecDlg.m_fVectorY = pCurHShape->desStartPos.face_offset_norm.y;
			vecDlg.m_fVectorZ = pCurHShape->desStartPos.face_offset_norm.z;
			if(vecDlg.DoModal()==IDOK)
			{
				for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
					pLineHShape->desStartPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumPart"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineHShape->desStartPos.hDatumPart);
		pParentItem=pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("desStartPos.endPosType"),NULL);
		//if(pParentItem)
			//UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.fNormOffset"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->desStartPos.fNormOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.fEccentricDist"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->desStartPos.fEccentricDist = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.fTransverseOffset"))
	{	//����ƫ����
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->desStartPos.fTransverseOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumStartPart"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineHShape->desStartPos.hDatumStartPart);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.bDatumStartStartEnd"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start")==0)
				pLineHShape->desStartPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineHShape->desStartPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
#endif
				pLineHShape->desStartPos.bDatumStartStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.hDatumEndPart"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineHShape->desStartPos.hDatumEndPart);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desStartPos.bDatumEndStartEnd"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start")==0)
				pLineHShape->desStartPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineHShape->desStartPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
#endif
				pLineHShape->desStartPos.bDatumEndStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("oddEnd"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{	//����ֶ������˸˼�����ͷӦ���ȣ�������ͷ�ļ��㷽ʽ�޸�Ϊ������
			//�����������ø˼���صĸְ�ʱ�������¼�������ͷ��ֵ	wht 09-09-02
			pLineHShape->desEndOdd.m_iOddCalStyle=2;
			pLineHShape->SetEndOdd(atof(valueStr));//,TRUE,TRUE);H�͸ִ��벻���ƣ��ݲ�������˨�������塡wjh-2018.1.8,TRUE,TRUE);
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("endPos.x"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->pEnd->SetPositionX(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("endPos.y"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->pEnd->SetPositionY(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("endPos.z"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->pEnd->SetPositionZ(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.endPosType"))
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
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->desEndPos.endPosType = end_pos_type;
		//UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.datum_pos_style"))
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
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->desEndPos.datum_pos_style = datum_pos_style;
		pParentItem=pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("desEndPos.endPosType"),NULL);
		//if(pParentItem)
			//UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
		if(datum_pos_style>2)
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Offset Normal";
#else 
			vecDlg.m_sCaption = "ƽ���淨��";
#endif
			vecDlg.m_fVectorX = pCurHShape->desEndPos.face_offset_norm.x;
			vecDlg.m_fVectorY = pCurHShape->desEndPos.face_offset_norm.y;
			vecDlg.m_fVectorZ = pCurHShape->desEndPos.face_offset_norm.z;
			if(vecDlg.DoModal()==IDOK)
			{
				for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
					pLineHShape->desEndPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.hDatumPart"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineHShape->desEndPos.hDatumPart);
		pParentItem=pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("desEndPos.endPosType"),NULL);
		//if(pParentItem)
			//UpdateLineSlot_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.fNormOffset"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->desEndPos.fNormOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.fEccentricDist"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->desEndPos.fEccentricDist = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.fTransverseOffset"))
	{	//����ƫ����
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			pLineHShape->desEndPos.fTransverseOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.hDatumStartPart"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineHShape->desEndPos.hDatumStartPart);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.bDatumStartStartEnd"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start")==0)
				pLineHShape->desEndPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineHShape->desEndPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
#endif
				pLineHShape->desEndPos.bDatumStartStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.hDatumEndPart"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineHShape->desEndPos.hDatumEndPart);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desEndPos.bDatumEndStartEnd"))
	{
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start")==0)
				pLineHShape->desEndPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineHShape->desEndPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
#endif
				pLineHShape->desEndPos.bDatumEndStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desDatumPoint.x"))
	{
		GEPOINT pos;
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			//pos=pLineHShape->desDatumPoint.Position();
			//pLineHShape->desDatumPoint.SetPosition(atof(valueStr),pos.y,pos.z);
		}
		CXhChar100 sText;
		long idProp=CLDSLineHShape::GetPropID("desDatumPoint");
		if(pCurHShape->GetPropValueStr(idProp,sText)>0)
			pPropList->SetItemPropValue(idProp,sText);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desDatumPoint.y"))
	{
		GEPOINT pos;
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			//pos=pLineHShape->desDatumPoint.Position();
			//pLineHShape->desDatumPoint.SetPosition(pos.x,atof(valueStr),pos.z);
		}
		CXhChar100 sText;
		long idProp=CLDSLineHShape::GetPropID("desDatumPoint");
		if(pCurHShape->GetPropValueStr(idProp,sText)>0)
			pPropList->SetItemPropValue(idProp,sText);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desDatumPoint.z"))
	{
		GEPOINT pos;
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			//pos=pLineHShape->desDatumPoint.Position();
			//pLineHShape->desDatumPoint.SetPosition(pos.x,pos.y,atof(valueStr));
		}
		CXhChar100 sText;
		long idProp=CLDSLineHShape::GetPropID("desDatumPoint");
		if(pCurHShape->GetPropValueStr(idProp,sText)>0)
			pPropList->SetItemPropValue(idProp,sText);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desDatumPoint.param.hOffsetDatumNode"))
	{
		long hNode=HexStrToLong(valueStr);
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			//if(pLineHShape->desDatumPoint.datum_pos_style==11)
			//{
			//	if(pLineHShape->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode>=0)
			//		pLineHShape->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=hNode;
			//	else
			//		pLineHShape->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=-hNode;
			//	pLineHShape->SetModified();
			//	//�¸��������ڵ�ͬʱһ�㻹�����ƫ������ƫ�����ͣ���ʱ���¼���۸�λ��û������ wjh-2015.10.12
			//	pLineHShape->CalPosition(true);
			//}
		}
		return TRUE;
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desDatumPoint.param.lenOffsetType"))
	{
		//for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		//{
		//	if(pLineHShape->desDatumPoint.datum_pos_style==11)
		//	{
		//		if(valueStr[0]=='0')//0��ʼ->��;
		//			pLineHShape->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode= abs(pLineHShape->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode);
		//		else
		//			pLineHShape->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=-abs(pLineHShape->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode);
		//	}
		//}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desDatumPoint.param.lenOffset"))
	{
		double lenOffset=atof(valueStr);
		/*for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			if(pLineHShape->desDatumPoint.datum_pos_style==11)
				pLineHShape->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.len_offset= lenOffset;
		}*/
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desDatumPoint.param.RayVecStyle"))
	{
		//char style=valueStr[0]-'0';
		//for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		//{
		//	if(pLineHShape->desDatumPoint.datum_pos_style==11)
		//	{
		//		pLineHShape->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style= style;
		//		if(style==4)	//ˮƽ���ò۸ֵ���
		//			pLineHShape->m_iNormCalStyle=2;
		//		else if(style!=4&&pLineHShape->m_iNormCalStyle==2)
		//			pLineHShape->m_iNormCalStyle=0;
		//	}
		//}
		//UpdateLineSlotColumnRayVecParamsProperty(pPropList,pItem);
		CXhChar100 sText;
		pCurHShape->CalWorkPlaneNorm();
		long propId=CLDSLineHShape::GetPropID("m_iNormCalStyle");
		if(pCurHShape->GetPropValueStr(propId,sText)>0)
			pPropList->SetItemPropValue(propId,sText);
		propId=CLDSLineHShape::GetPropID("workPlaneNorm.x");
		if(pCurHShape->GetPropValueStr(propId,sText)>0)
			pPropList->SetItemPropValue(propId,sText);
		propId=CLDSLineHShape::GetPropID("workPlaneNorm.y");
		if(pCurHShape->GetPropValueStr(propId,sText)>0)
			pPropList->SetItemPropValue(propId,sText);
		propId=CLDSLineHShape::GetPropID("workPlaneNorm.z");
		if(pCurHShape->GetPropValueStr(propId,sText)>0)
			pPropList->SetItemPropValue(propId,sText);
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desDatumPoint.param.RayVecStdStyle"))
	{
		long idPropX = CLDSLineHShape::GetPropID("desDatumPoint.param.RayVec.x");
		long idPropY = CLDSLineHShape::GetPropID("desDatumPoint.param.RayVec.y");
		long idPropZ = CLDSLineHShape::GetPropID("desDatumPoint.param.RayVec.z");
		//if(valueStr[0]=='0')	//�Զ��巽��
		//{
		//	pPropList->SetItemReadOnly(idPropX,pCurHShape->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style!=0);
		//	pPropList->SetItemReadOnly(idPropY,pCurHShape->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style!=0);
		//	pPropList->SetItemReadOnly(idPropZ,pCurHShape->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style!=0);
		//}
		//else
		//{
		//	pPropList->SetItemReadOnly(idPropX,TRUE);
		//	pPropList->SetItemReadOnly(idPropY,TRUE);
		//	pPropList->SetItemReadOnly(idPropZ,TRUE);
		//	f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
		//	pCurHShape->desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec=vector;
		//	CXhChar100 sText;
		//	if(pCurHShape->GetPropValueStr(idPropX,sText))
		//		pPropList->SetItemPropValue(idPropX,sText);
		//	if(pCurHShape->GetPropValueStr(idPropY,sText))
		//		pPropList->SetItemPropValue(idPropY,sText);
		//	if(pCurHShape->GetPropValueStr(idPropZ,sText))
		//		pPropList->SetItemPropValue(idPropZ,sText);
		//	for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		//	{
		//		if(pLineHShape->desDatumPoint.datum_pos_style==11)
		//			pLineHShape->desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec=vector;
		//	}
		//}
	}
	else if(pItem->m_idProp==CLDSLineHShape::GetPropID("desDatumPoint.param.RayVecRotateAngle"))
	{
		/*double rotangle=atof(valueStr);
		for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
		{
			if(pLineHShape->desDatumPoint.datum_pos_style==11)
				pLineHShape->desDatumPoint.des_para.COLUMN_INTERS.ray_angle= rotangle;
		}*/
	}
	else
		return FALSE;
	for(pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetFirst();pLineHShape;pLineHShape=(CLDSLineHShape*)pView->selectObjs.GetNext())
	{
		pLineHShape->SetModified();
		pLineHShape->CalPosition(true);	
		pLineHShape->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pLineHShape->GetSolidPartObject());
		/*if(pLineHShape->desDatumPoint.datum_pos_style==11)
		{
			for(CLsRef* pLsRef=pLineHShape->GetFirstLsRef();pLsRef;pLsRef=pLineHShape->GetNextLsRef())
			{
				(*pLsRef)->correct_worknorm();
				(*pLsRef)->correct_pos();
				(*pLsRef)->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
			}
		}*/
	}
	g_pSolidDraw->Draw();
	return TRUE;
}
void UpdateLineHShapeStart_EndDesPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineHShape *pFirstHShape = (CLDSLineHShape *)pView->selectObjs.GetFirst();
	if(pFirstHShape==NULL||pParentItem==NULL)
		return;
	CPropertyListOper<CLDSLineHShape> oper(pPropList,pFirstHShape);
	CTubeEndPosPara *pDesPos;
	if(bStart)
		pDesPos=&pFirstHShape->desStartPos;
	else
		pDesPos=&pFirstHShape->desEndPos;
	pPropList->DeleteAllSonItems(pParentItem);
	CPropTreeItem* pPropItem=NULL;
	if(pDesPos->endPosType==0)	//0:�˵㼴�ڵ�
	{
		if(bStart)
		{
			pPropItem=oper.InsertEditPropItem(pParentItem,"desStartPos.fNormOffset");
			pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		}
		else
		{	
			pPropItem=oper.InsertEditPropItem(pParentItem,"desEndPos.fNormOffset");
			pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		}
	}
	else if(pDesPos->endPosType==1)	//1:����ڽڵ㸸�˼���
	{
		CLDSLinePart *pDatumLinePart=NULL;
		if(bStart)
		{
			pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstHShape->desStartPos.hDatumPart,CLS_LINEPART);
			if(pDatumLinePart==NULL&&pFirstHShape->pStart)
				pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstHShape->pStart->hFatherPart,CLS_LINEPART);
		}
		else
		{
			pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstHShape->desEndPos.hDatumPart,CLS_LINEPART);
			if(pDatumLinePart==NULL&&pFirstHShape->pEnd)
				pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstHShape->pEnd->hFatherPart,CLS_LINEPART);
		}
		BOOL bIsDatumJg=FALSE;
		if(pDatumLinePart&&pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE)
			bIsDatumJg=TRUE;
		//���λ�û�׼�˼�
		if(bStart)
		{
			pPropItem=oper.InsertBtnEditPropItem(pParentItem,"desStartPos.hDatumPart");
			pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		}
		else
		{	
			pPropItem=oper.InsertBtnEditPropItem(pPropItem,"desEndPos.hDatumPart");
			pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		}
		if(bIsDatumJg)
		{	//���㶨λ��ʽ
			if(bStart)
			{
				pPropItem=oper.InsertCmbListPropItem(pParentItem,"desStartPos.datum_pos_style");
				pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
			}
			else
			{
				pParentItem=oper.InsertCmbListPropItem(pParentItem,"desEndPos.datum_pos_style");
				pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
			}	
		}
		//��׼�淨��ƫ����
		if(bStart)
		{
			pPropItem=oper.InsertEditPropItem(pParentItem,"desStartPos.fNormOffset");
			pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		}
		else
		{	
			pPropItem=oper.InsertEditPropItem(pParentItem,"desEndPos.fNormOffset");
			pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		}
		//����ƫ����
		if(bStart)
		{
			pPropItem=oper.InsertEditPropItem(pParentItem,"desStartPos.fTransverseOffset");
			pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		}
		else
		{	
			pPropItem=oper.InsertEditPropItem(pParentItem,"desEndPos.fTransverseOffset");
			pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		}
		if(!bIsDatumJg)
		{	//ƫ�ľ�
			if(bStart)
			{
				pPropItem=oper.InsertEditPropItem(pParentItem,"desStartPos.fEccentricDist");
				pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
			}
			else
			{	
				pPropItem=oper.InsertEditPropItem(pParentItem,"desEndPos.fEccentricDist");
				pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
			}
		}
		if((bIsDatumJg&&pDesPos->datum_pos_style>2)||!bIsDatumJg)
		{	//ƽ���淨��
			if(bStart)
			{
				pPropItem=oper.InsertButtonPropItem(pParentItem,"desStartPos.face_offset_norm");
				pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
			}
			else
			{
				pPropItem=oper.InsertButtonPropItem(pParentItem,"desEndPos.face_offset_norm");
				pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
			}
		}
	}
	else if(pDesPos->endPosType==2)	//2:�ڵ���ָ����׼��ͶӰ
	{
		//ͶӰ��ʼ�����ڻ�׼�˼�
		if(bStart)
		{
			pPropItem=oper.InsertBtnEditPropItem(pParentItem,"desStartPos.hDatumStartPart");
			pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		}
		else
		{	
			pPropItem=oper.InsertBtnEditPropItem(pParentItem,"desEndPos.hDatumStartPart");
			pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		}
		//��׼�˼��Ļ�׼�˵�
		if(bStart)
		{
			pPropItem=oper.InsertCmbListPropItem(pParentItem,"desStartPos.bDatumStartStartEnd");
			pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		}
		else
		{	
			pPropItem=oper.InsertCmbListPropItem(pParentItem,"desEndPos.bDatumStartStartEnd");
			pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		}
		//ͶӰ���ն����ڻ�׼�˼�
		if(bStart)
		{
			pPropItem=oper.InsertCmbEditPropItem(pParentItem,"desStartPos.hDatumEndPart");
			pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		}
		else
		{	
			pPropItem=oper.InsertCmbEditPropItem(pParentItem,"desEndPos.hDatumEndPart");
			pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		}
		//��׼�˼��Ļ�׼�˵�
		if(bStart)
		{
			pPropItem=oper.InsertCmbListPropItem(pParentItem,"desStartPos.bDatumEndStartEnd");
			pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		}
		else
		{	
			pPropItem=oper.InsertCmbListPropItem(pParentItem,"desEndPos.bDatumEndStartEnd");
			pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		}
	}
	else if(pDesPos->endPosType==3)	//3:�˼����߽���
	{
		//��׼�˼�
		if(bStart)
		{
			pPropItem=oper.InsertBtnEditPropItem(pParentItem,"desStartPos.hDatumPart");
			pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		}
		else
		{	
			pPropItem=oper.InsertBtnEditPropItem(pParentItem,"desEndPos.hDatumPart");
			pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		}
		//����˼�
		if(bStart)
		{
			pPropItem=oper.InsertBtnEditPropItem(pParentItem,"desStartPos.hDatumStartPart");
			pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		}
		else
		{	
			pPropItem=oper.InsertBtnEditPropItem(pParentItem,"desEndPos.hDatumStartPart");
			pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		}	
		//��׼�淨��ƫ����
		if(bStart)
		{
			pPropItem=oper.InsertEditPropItem(pParentItem,"desStartPos.fNormOffset");
			pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		}
		else
		{
			pPropItem=oper.InsertEditPropItem(pParentItem,"desEndPos.fNormOffset");
			pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		}	
	}
	else if(pDesPos->endPosType==4)	//ֱ��ָ������
	{	
		
	}
}
void CLDSView::DisplayLineHShapeProperty(BOOL bRebuild)
{
	CLDSLineHShape *pFirstHShape = (CLDSLineHShape*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropertyListOper<CLDSLineHShape> oper(pPropList,pFirstHShape);
	//
	const int GROUP_GENERAL=1,GROUP_START=2,GROUP_END=3;
	pPropDlg->m_arrPropGroupLabel.RemoveAll();
	if(pFirstHShape->pStart!=NULL&&pFirstHShape->pEnd!=NULL)
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
		pPropDlg->RefreshTabCtrl(CLDSLineHShape::m_iCurDisplayPropGroup);
	}
	else 
	{
		pPropDlg->m_arrPropGroupLabel.SetSize(0);
		pPropDlg->RefreshTabCtrl(0);
	}
	if(bRebuild)
	{	//��Ҫ�ؽ���������ҳ
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_LINEHSHAPE;
		pPropList->SetModifyValueFunc(ModifyLineHShapeProperty);
		pPropList->SetButtonClickFunc(LineHShapeButtonClick);
		pPropList->SetModifyStatusFunc(ModifyLineHShapeItemStatus);
		if(!pFirstHShape->BelongModel()->IsTowerModel()||pFirstHShape->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//H�͸ֻ�����Ϣ
		CPropTreeItem* pRoot=pPropList->GetRootItem();
		CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pSonPropItem=NULL;
		pGroupItem=oper.InsertPropItem(pRoot,"basicInfo");
		if(CLDSLineHShape::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineHShape::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
		pPropItem=oper.InsertEditPropItem(pGroupItem,"handle");	//���
		pPropItem->SetReadOnly();
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
		oper.InsertEditPropItem(pGroupItem,"layer");			//ͼ��
		oper.InsertButtonPropItem(pGroupItem,"cfgword");			//��ĺ�
		oper.InsertEditPropItem(pGroupItem,"iSeg");				//�ֶκ�
		oper.InsertBtnEditPropItem(pGroupItem,"sPartNo");			//�������
		oper.InsertCmbListPropItem(pGroupItem,"cMaterial",MakeMaterialMarkSetString());		//����
		oper.InsertCmbListPropItem(pGroupItem,"m_cQualityLevel");	//���������ȼ�
		oper.InsertCmbListPropItem(pGroupItem,"ibHShapeType");	//H�͸����
		oper.InsertCmbEditPropItem(pGroupItem,"specification",MakeHShapeSetString(pFirstHShape->m_ibHShapeType));//���
		pPropItem=oper.InsertEditPropItem(pGroupItem,"thick1");		//������
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertEditPropItem(pGroupItem,"thick2");		//��Ե���
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem,"m_uStatMatCoef");		//ͳ��ϵ��
		oper.InsertBtnEditPropItem(pGroupItem,"m_hPartWeldParent");		//���Ӹ�����
		oper.InsertCmbListPropItem(pGroupItem,"m_bDisplayHole");	//��ʾ��	
		oper.InsertCmbListPropItem(pGroupItem,"m_bVirtualPart");	//���⸨������
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bUserSpecColor");	//�û�ָ��������ɫ
		if(pFirstHShape->m_bUserSpecColor)
			oper.InsertCmbColorPropItem(pPropItem,"crMaterial");		//������ɫ
		pPropItem=oper.InsertEditPropItem(pGroupItem,"length");		//ʵ�ʳ���
		pPropItem->SetReadOnly();
		//
		if(pFirstHShape->IsShadowObject())
		{	//Ӱ����Ϣ
			pPropItem=oper.InsertEditPropItem(pGroupItem,"shadowInfo");
			pSonPropItem=oper.InsertEditPropItem(pPropItem,"m_hMotherObj");	//Ӱ��ĸ����
			pSonPropItem->SetReadOnly(pFirstHShape->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			if(pFirstHShape->IsBlockRefShadowObject())
			{
				pSonPropItem=oper.InsertEditPropItem(pPropItem,"m_BlockRef");	//������������
				pSonPropItem->SetReadOnly(pFirstHShape->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				pSonPropItem=oper.InsertEditPropItem(pPropItem,"m_hBlock");	//��������
				pSonPropItem->SetReadOnly(pFirstHShape->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			}
		}
		else 
			oper.InsertButtonPropItem(pGroupItem,"relativeObjs");	//��������
		oper.InsertEditPropItem(pGroupItem,"sNotes");			//��ע
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"workPlaneNorm");	//��׼�淨��	
		oper.InsertCmbListPropItem(pPropItem,"m_iNormCalStyle");//����ָ����ʽ
		if(pFirstHShape->m_iNormCalStyle==1)
		{
			pSonPropItem=oper.InsertButtonPropItem(pPropItem,"nearWorkPlaneNorm");	//��׼����Ʒ���
			oper.InsertEditPropItem(pSonPropItem,"nearWorkPlaneNorm.x");	//X
			oper.InsertEditPropItem(pSonPropItem,"nearWorkPlaneNorm.y");	//Y
			oper.InsertEditPropItem(pSonPropItem,"nearWorkPlaneNorm.z");	//Z
			oper.InsertEditPropItem(pPropItem,"hWorkNormRefPart");	//���߲��ո˼�
		}
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"workPlaneNorm.x");	//X
		pSonPropItem->SetReadOnly(pFirstHShape->m_iNormCalStyle==1);
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"workPlaneNorm.y");	//Y
		pSonPropItem->SetReadOnly(pFirstHShape->m_iNormCalStyle==1);
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"workPlaneNorm.z");	//Z
		pSonPropItem->SetReadOnly(pFirstHShape->m_iNormCalStyle==1);
		//ʼ����Ϣ
		pGroupItem=oper.InsertEditPropItem(pRoot,"startInfo");
		if(CLDSLineHShape::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineHShape::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		pPropItem=oper.InsertEditPropItem(pGroupItem,"hStart");		//�����
		pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		pPropItem=oper.InsertEditPropItem(pGroupItem,"oddStart");	//����ͷ
		pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		pPropItem=oper.InsertEditPropItem(pGroupItem,"startPos");	//ʼ������
		if(pPropItem)
		{
			pSonPropItem=oper.InsertEditPropItem(pPropItem,"startPos.x");	//X
			if(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart)
				pSonPropItem->SetReadOnly(TRUE);
			else 
				pSonPropItem->SetReadOnly(pFirstHShape->desStartPos.endPosType!=4);
			pSonPropItem=oper.InsertEditPropItem(pPropItem,"startPos.y");	//Y
			if(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart)
				pSonPropItem->SetReadOnly(TRUE);
			else 
				pSonPropItem->SetReadOnly(pFirstHShape->desStartPos.endPosType!=4);
			pSonPropItem=oper.InsertEditPropItem(pPropItem,"startPos.z");	//Z
			if(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart)
				pSonPropItem->SetReadOnly(TRUE);
			else 
				pSonPropItem->SetReadOnly(pFirstHShape->desStartPos.endPosType!=4);
		}
		oper.InsertEditPropItem(pGroupItem,"actualStartPos");//ʼ��ʵ������
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"desStartPos.endPosType");	//�˵�λ��
		pPropItem->SetReadOnly(pFirstHShape->IsStartPosLocked()||pFirstHShape->bHuoQuStart);
		UpdateLineHShapeStart_EndDesPropItem(pPropList,pPropItem,TRUE);
		//�ն���Ϣ
		pGroupItem=oper.InsertEditPropItem(pRoot,"endInfo");
		if(CLDSLineHShape::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineHShape::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		pPropItem=oper.InsertEditPropItem(pGroupItem,"hEnd");	//�յ���
		pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		pPropItem=oper.InsertEditPropItem(pGroupItem,"oddEnd");	//����ͷ
		pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		pPropItem=oper.InsertEditPropItem(pGroupItem,"endPos");	//�ն�����
		if(pPropItem)
		{
			pSonPropItem=oper.InsertEditPropItem(pPropItem,"endPos.x");	//X
			if(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd)
				pSonPropItem->SetReadOnly(TRUE);
			else 
				pSonPropItem->SetReadOnly(pFirstHShape->desEndPos.endPosType!=4);

			pSonPropItem=oper.InsertEditPropItem(pPropItem,"endPos.y");	//Y
			if(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd)
				pSonPropItem->SetReadOnly(TRUE);
			else 
				pSonPropItem->SetReadOnly(pFirstHShape->desEndPos.endPosType!=4);

			pSonPropItem=oper.InsertEditPropItem(pPropItem,"endPos.z");	//Z
			if(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd)
				pSonPropItem->SetReadOnly(TRUE);
			else 
				pSonPropItem->SetReadOnly(pFirstHShape->desEndPos.endPosType!=4);
		}
		oper.InsertEditPropItem(pGroupItem,"actualEndPos");	//�ն�ʵ������
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"desEndPos.endPosType");	//�˵�λ��
		pPropItem->SetReadOnly(pFirstHShape->IsEndPosLocked()||pFirstHShape->bHuoQuEnd);
		UpdateLineHShapeStart_EndDesPropItem(pPropList,pPropItem,FALSE);	
	}
	else
	{	//����Ҫ�ؽ���������ҳ
		CPropTreeItem *pFindItem=NULL,*pParentItem=NULL,*pPropItem=NULL,*pSonPropItem=NULL;
		SetPartMutiObjsProp(pFirstHShape,"handle");
		SetPartMutiObjsProp(pFirstHShape,"layer");
		SetPartMutiObjsProp(pFirstHShape,"cfgword");
		SetPartMutiObjsProp(pFirstHShape,"iSeg");
		SetPartMutiObjsProp(pFirstHShape,"sPartNo");
		SetPartMutiObjsProp(pFirstHShape,"cMaterial");
		SetPartMutiObjsProp(pFirstHShape,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstHShape,"ibHShapeType");
		SetPartMutiObjsProp(pFirstHShape,"specification");
		SetPartMutiObjsProp(pFirstHShape,"thick1");
		SetPartMutiObjsProp(pFirstHShape,"thick2");
		SetPartMutiObjsProp(pFirstHShape,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstHShape,"m_hPatWeldParent");
		SetPartMutiObjsProp(pFirstHShape,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstHShape,"m_bVirtualPart");
		SetPartMutiObjsProp(pFirstHShape,"m_bUserSpecColor");
		pFindItem=pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("m_bUserSpecColor"),&pParentItem);
		if(pFirstHShape->m_bUserSpecColor)
			oper.InsertEditPropItem(pFindItem,"crMaterial");
		else
			pPropList->DeleteItemByPropId(CLDSLineHShape::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstHShape,"length");
		pFindItem=pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("relativeObjs"),&pParentItem);
		pFindItem=pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("shadowInfo"),&pParentItem);
		if(pFirstHShape->IsShadowObject())
		{	//����ͨ�����л���Ӱ�乹��
			pPropList->DeleteItemByPropId(CLDSLineHShape::GetPropID("relativeObjs"));
			pPropItem=oper.InsertEditPropItem(pParentItem,"shadowInfo");	//Ӱ����Ϣ
			pSonPropItem=oper.InsertEditPropItem(pPropItem,"m_hMotherObj");	//Ӱ��ĸ����
			pSonPropItem->SetReadOnly(pFirstHShape->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			if(pFirstHShape->IsBlockRefShadowObject())
			{
				pSonPropItem=oper.InsertEditPropItem(pPropItem,"m_hBlockRef");	//������������
				pSonPropItem->SetReadOnly(pFirstHShape->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				pSonPropItem=oper.InsertEditPropItem(pPropItem,"m_hBlock");		//��������
				pSonPropItem->SetReadOnly(pFirstHShape->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			}
		}
		else
		{
			if(pFindItem)
			{
				pPropList->DeleteAllSonItems(pFindItem);
				pPropList->DeleteItemByPropId(pFindItem->m_idProp);
			}
			oper.InsertEditPropItem(pParentItem,"relativeObjs");	//��������
		}
		SetPartMutiObjsProp(pFirstHShape,"sNotes");
		//��׼��
		SetPartMutiObjsProp(pFirstHShape,"m_iNormCalStyle");
		if(pFirstHShape->m_iNormCalStyle==1)
		{
			SetPartMutiObjsProp(pFirstHShape,"nearWorkPlaneNorm");
			SetPartMutiObjsProp(pFirstHShape,"nearWorkPlaneNorm.x");
			SetPartMutiObjsProp(pFirstHShape,"nearWorkPlaneNorm.y");
			SetPartMutiObjsProp(pFirstHShape,"nearWorkPlaneNorm.z");
			SetPartMutiObjsProp(pFirstHShape,"hWorkNormRefPart");
		}
		SetPartMutiObjsProp(pFirstHShape,"workPlaneNorm.x");
		SetPartMutiObjsProp(pFirstHShape,"workPlaneNorm.y");
		SetPartMutiObjsProp(pFirstHShape,"workPlaneNorm.z");
		//ʼ����Ϣ
		SetPartMutiObjsProp(pFirstHShape,"hStart");
		SetPartMutiObjsProp(pFirstHShape,"oddStart");
		SetPartMutiObjsProp(pFirstHShape,"startPos.x");
		SetPartMutiObjsProp(pFirstHShape,"startPos.y");
		SetPartMutiObjsProp(pFirstHShape,"startPos.z");
		SetPartMutiObjsProp(pFirstHShape,"desStartPos.endPosType");
		pFindItem=pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("desStartPos.endPosType"),&pParentItem);
		if(pFindItem)
			UpdateLineHShapeStart_EndDesPropItem(pPropList,pFindItem,TRUE);
		//�ն���Ϣ
		SetPartMutiObjsProp(pFirstHShape,"hEnd");
		SetPartMutiObjsProp(pFirstHShape,"oddEnd");
		SetPartMutiObjsProp(pFirstHShape,"endPos.x");
		SetPartMutiObjsProp(pFirstHShape,"endPos.y");
		SetPartMutiObjsProp(pFirstHShape,"endPos.z");
		SetPartMutiObjsProp(pFirstHShape,"desEndPos.endPosType");
		pFindItem=pPropList->FindItemByPropId(CLDSLineHShape::GetPropID("desEndPos.endPosType"),&pParentItem);
		if(pFindItem)
			UpdateLineHShapeStart_EndDesPropItem(pPropList,pFindItem,FALSE);
	}
	pPropList->Redraw();
}
#endif