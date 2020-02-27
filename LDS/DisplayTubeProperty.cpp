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
#include "PartWorkPlaneDlg.h"
#include "ConeTubeLjParaDlg.h"
#include "SlotArm.h"
#include "DesignShadowDlg.h"
#include "PropertyListOper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//from DisplayPropertyPage.cpp
BOOL SetPartMutiObjsProp(CLDSObject *pObj, char *propName,long idClassType=0);
void SetPropItemReadOnly(CLDSDbObject *pObj,char *propName,BOOL bReadOnly);

//#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
BOOL ModifyTubeItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSLineTube::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
CDesignTubeEndLjDlg endLjDlg;
BOOL FireLineTubePopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineTube *pFirstLineTube = (CLDSLineTube*)pView->selectObjs.GetFirst();
	if(pFirstLineTube==NULL)
		return FALSE;
	char valueStr[MAX_TEMP_CHAR_50+1]="";
	if(pItem->m_idProp==CLDSLineTube::GetPropID("jointStart"))
	{
		endLjDlg.m_pBaseNode=pFirstLineTube->pStart;
		endLjDlg.m_bStartEnd=TRUE;
		endLjDlg.m_pCurLineTube=pFirstLineTube;
		endLjDlg.m_iTubeEndLjStyle=iMenu;	
		CUndoOperObject undo(&Ta,true);
		if(endLjDlg.DoModal()==IDOK)
		{
			if(pFirstLineTube->GetPropValueStr(pItem->m_idProp,valueStr))
				pPropList->SetItemPropValue(pItem->m_idProp,CString(valueStr));
			long idProp = CLDSLineTube::GetPropID("oddStart");
			if(pFirstLineTube->GetPropValueStr(idProp,valueStr)>0)
				pPropList->SetItemPropValue(idProp,CString(valueStr));
			pItem->m_lpNodeInfo->SetCheck(iMenu+1);
		}
		return TRUE;
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("jointEnd"))
	{
		endLjDlg.m_pBaseNode=pFirstLineTube->pEnd;
		endLjDlg.m_bStartEnd=FALSE;
		endLjDlg.m_pCurLineTube=pFirstLineTube;
		endLjDlg.m_iTubeEndLjStyle=iMenu;
		CUndoOperObject undo(&Ta,true);
		if(endLjDlg.DoModal()==IDOK)
		{
			if(pFirstLineTube->GetPropValueStr(pItem->m_idProp,valueStr))
				pPropList->SetItemPropValue(pItem->m_idProp,CString(valueStr));
			long idProp = CLDSLineTube::GetPropID("oddEnd");
			if(pFirstLineTube->GetPropValueStr(idProp,valueStr)>0)
				pPropList->SetItemPropValue(idProp,CString(valueStr));
			pItem->m_lpNodeInfo->SetCheck(iMenu+1);
		}
		return TRUE;
	}
	return FALSE;
}
BOOL ModifyLineTubeProperty(CPropertyList* pPropList,CPropTreeItem* pItem, CString &valueStr);
void UpdateLineTube_DesPos_EndPosType_TreeItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineTube *pFirstTube = (CLDSLineTube*)pView->selectObjs.GetFirst();
	if(pFirstTube==NULL||pParentItem==NULL)
		return;
	char sPropKeyName[100] = "";
	CPropTreeItem* pPropItem = NULL;
	CPropertyListOper<CLDSLineTube> oper(pPropList, pFirstTube);
	pPropList->DeleteAllSonItems(pParentItem);
	CTubeEndPosPara *pDesPos = (bStart) ? &pFirstTube->desStartPos : &pFirstTube->desEndPos;
	//0:�˵㼴�ڵ�;1:����ڽڵ㸸�˼���;2:�ڵ���ָ����׼��ͶӰ;3:�˼����߽���;4:ָ������; wht 10-10-18
	if(pDesPos->endPosType==0)	
	{	//�ڵ㼴����
		//��׼�淨��ƫ����
		if (bStart)
			strcpy(sPropKeyName, "desStartPos.fNormOffset");
		else
			strcpy(sPropKeyName, "desEndPos.fNormOffset");
		pPropItem = oper.InsertEditPropItem(pParentItem, sPropKeyName, "��׼�淨��ƫ����", "", -1, TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	}
	else if(pDesPos->endPosType==1)
	{	//�������	
		CLDSLinePart *pDatumLinePart=NULL;
		if(bStart)
		{	//���λ�û�׼�ֹ�
			pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstTube->desStartPos.hDatumPart,CLS_LINEPART);
			if(pDatumLinePart==NULL&&pFirstTube->pStart)
				pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstTube->pStart->hFatherPart,CLS_LINEPART);
		}
		else
		{	//���λ�û�׼�ֹ�
			pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstTube->desEndPos.hDatumPart,CLS_LINEPART);
			if(pDatumLinePart==NULL&&pFirstTube->pEnd)
				pDatumLinePart=(CLDSLinePart*)console.FromPartHandle(pFirstTube->pEnd->hFatherPart,CLS_LINEPART);
		}
		BOOL bIsDatumJg=FALSE;
		if(pDatumLinePart&&pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE)
			bIsDatumJg=TRUE;
		//��׼�Ǹ�
		if (bStart)
			strcpy(sPropKeyName, "desStartPos.hDatumPart");
		else
			strcpy(sPropKeyName, "desEndPos.hDatumPart");
		pPropItem = oper.InsertBtnEditPropItem(pParentItem, sPropKeyName, "", "", -1, TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs, pPropItem->m_idProp);
		if(bIsDatumJg)
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "���λ�û�׼�Ǹ�";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "�ֹܴ��λ�û�׼�Ǹ־��";
		}
		else
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "���λ�û�׼�ֹ�";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "�ֹܴ��λ�û�׼�ֹܾ��";
		}
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		if(bIsDatumJg)
		{	//���㶨λ��ʽ
			if (bStart)
				strcpy(sPropKeyName, "desStartPos.datum_pos_style");
			else
				strcpy(sPropKeyName, "desEndPos.datum_pos_style");
			pPropItem = oper.InsertCmbListPropItem(pParentItem, sPropKeyName, "", "", "", -1, TRUE);
			pPropItem->m_lpNodeInfo->m_cmbItems = "����ͶӰ|X֫����ͶӰ|Y֫����ͶӰ|������ƽ��|X֫����ƽ��|Y֫����ƽ��|";
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);	
			if(bStart)
				pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
											pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			else
				pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
											pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
		//�������췽��ƫ����
		if (bStart)
			strcpy(sPropKeyName, "desStartPos.fLenVecOffsetDist");
		else
			strcpy(sPropKeyName, "desEndPos.fLenVecOffsetDist");
		pPropItem = oper.InsertEditPropItem(pParentItem, sPropKeyName, "", "", -1, TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//��׼�淨��ƫ����
		if (bStart)
			strcpy(sPropKeyName, "desStartPos.fNormOffset");
		else
			strcpy(sPropKeyName, "desEndPos.fNormOffset");
		pPropItem = oper.InsertEditPropItem(pParentItem, sPropKeyName, "", "", -1, TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//��׼�˼�Ϊ�Ǹ�ʱҲ��Ҫ����ƫ�ľ࣬����ʵ�����߸˼�����Ԥ��׼��(��ͶӰX��Y֫�����߶�λ��ʽ) wht 10-07-08
		if (bStart)
			strcpy(sPropKeyName, "desStartPos.fEccentricDist");
		else
			strcpy(sPropKeyName, "desEndPos.fEccentricDist");
		pPropItem = oper.InsertEditPropItem(pParentItem, sPropKeyName, "", "", -1, TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
											pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		if((bIsDatumJg&&pDesPos->datum_pos_style>2)||!bIsDatumJg)
		{	//ƽ���淨��
			if (bStart)
				strcpy(sPropKeyName, "desStartPos.face_offset_norm");
			else
				strcpy(sPropKeyName, "desEndPos.face_offset_norm");
			pPropItem = oper.InsertButtonPropItem(pParentItem, sPropKeyName, "", "", -1, TRUE);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(bStart)
				pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
											pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			else
				pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
											pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
	}
	else if(pDesPos->endPosType==2)
	{	//����ͶӰ
		//ͶӰ��ʼ�����ڻ�׼�ֹ�
		if (bStart)
			strcpy(sPropKeyName, "desStartPos.hDatumStartPart");
		else
			strcpy(sPropKeyName, "desEndPos.hDatumStartPart");
		pPropItem = oper.InsertBtnEditPropItem(pParentItem, sPropKeyName, "ͶӰ��ʼ�����ڻ�׼�ֹ�", "", -1, TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs, pPropItem->m_idProp);
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//��׼�ֹܵĻ�׼�˵�
		if(bStart)
			strcpy(sPropKeyName, "desStartPos.bDatumStartStartEnd");
		else
			strcpy(sPropKeyName, "desEndPos.bDatumStartStartEnd");
		pPropItem = oper.InsertCmbListPropItem(pParentItem, sPropKeyName, "", "", "", -1, TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//ͶӰ���ն����ڻ�׼�ֹ�
		if(bStart)
			strcpy(sPropKeyName, "desStartPos.hDatumEndPart");
		else
			strcpy(sPropKeyName, "desEndPos.hDatumEndPart");
		pPropItem = oper.InsertBtnEditPropItem(pParentItem, sPropKeyName, "", "", -1, TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//��׼�ֹܵĻ�׼�˵�
		if(bStart)
			strcpy(sPropKeyName, "desStartPos.bDatumEndStartEnd");
		else
			strcpy(sPropKeyName, "desEndPos.bDatumEndStartEnd");
		pPropItem = oper.InsertCmbListPropItem(pParentItem, sPropKeyName, "", "", "", -1, TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	}
	else if(pDesPos->endPosType==3)
	{	//�˼����߽���
		//��׼�˼�
		if (bStart)
			strcpy(sPropKeyName, "desStartPos.hDatumPart");
		else
			strcpy(sPropKeyName, "desEndPos.hDatumPart");
		pPropItem = oper.InsertBtnEditPropItem(pParentItem, sPropKeyName, "��׼�˼�", "���߽����׼�˼�", -1, TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//����˼�
		if(bStart)
			strcpy(sPropKeyName, "desStartPos.hDatumStartPart");
		else
			strcpy(sPropKeyName, "desEndPos.hDatumStartPart");
		pPropItem = oper.InsertBtnEditPropItem(pParentItem, sPropKeyName, "����˼�", "���߽��㽻��˼�", -1, TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//��׼�淨��ƫ����
		if(bStart)
			strcpy(sPropKeyName, "desStartPos.fNormOffset");
		else
			strcpy(sPropKeyName, "desEndPos.fNormOffset");
		pPropItem = oper.InsertEditPropItem(pParentItem, sPropKeyName, "��׼�淨��ƫ����", "", -1, TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(bStart)
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropList->SetItemReadOnly(pPropItem->m_idProp,pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
										pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	}
	if(bStart)
	{	
		if(pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart)
		{	//�������Ϊֻ������
			SetPropItemReadOnly(pFirstTube,"startPos.x",TRUE);
			SetPropItemReadOnly(pFirstTube,"startPos.y",TRUE);
			SetPropItemReadOnly(pFirstTube,"startPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ���ж�
			SetPropItemReadOnly(pFirstTube,"startPos.x",pFirstTube->desStartPos.endPosType!=4||
								pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("startPos.x")));
			SetPropItemReadOnly(pFirstTube,"startPos.y",pFirstTube->desStartPos.endPosType!=4||
								pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("startPos.y")));
			SetPropItemReadOnly(pFirstTube,"startPos.z",pFirstTube->desStartPos.endPosType!=4||
								pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("startPos.z")));
		}
	}
	else
	{
		if(pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd)
		{	//�������Ϊֻ������
			SetPropItemReadOnly(pFirstTube,"endPos.x",TRUE);
			SetPropItemReadOnly(pFirstTube,"endPos.y",TRUE);
			SetPropItemReadOnly(pFirstTube,"endPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ���ж�
			SetPropItemReadOnly(pFirstTube,"endPos.x",pFirstTube->desEndPos.endPosType!=4||
								pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("endPos.x")));
			SetPropItemReadOnly(pFirstTube,"endPos.y",pFirstTube->desEndPos.endPosType!=4||
								pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("endPos.y")));
			SetPropItemReadOnly(pFirstTube,"endPos.z",pFirstTube->desEndPos.endPosType!=4||
								pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("endPos.z")));
		}
	}
}

CString GetBlockListStr();	//defined in DisplayNodeProperty.cpp
long RetrieveBlockHandle(const char* blockIdName);
static void UpdatePickWorkPlaneItemsPropStr(CPropertyList *pPropList,short iNormCalStyle)
{
	if(iNormCalStyle == 1)
	{
		pPropList->SetItemPropName(CLDSLineTube::GetPropID("pickWorkPlane"),CString("��׼����Ʒ���"));
		pPropList->SetItemPropHelpStr(CLDSLineTube::GetPropID("pickWorkPlane"),"�ֹܻ�׼����Ʒ���һ�����û�ָ����ϵͳ�ɸ��ݽ��Ʒ��߼��ֹ����߾�ȷ������ֹܵĻ�׼�淨�ߡ�");
		pPropList->SetItemPropHelpStr(CLDSLineTube::GetPropID("pickWorkPlane.x"),"�ֹܻ�׼�淨��X�᷽�������");
		pPropList->SetItemPropHelpStr(CLDSLineTube::GetPropID("pickWorkPlane.y"),"�ֹܻ�׼�淨��Y�᷽�������");
		pPropList->SetItemPropHelpStr(CLDSLineTube::GetPropID("pickWorkPlane.z"),"�ֹܻ�׼�淨��Z�᷽�������");
	}
	else if(iNormCalStyle == 2)
	{
		pPropList->SetItemPropName(CLDSLineTube::GetPropID("pickWorkPlane"),CString("������յ�"));
		pPropList->SetItemPropHelpStr(CLDSLineTube::GetPropID("pickWorkPlane"),"������յ���Ʒ���һ�����û�ָ����");
		pPropList->SetItemPropHelpStr(CLDSLineTube::GetPropID("pickWorkPlane.x"),"������յ��X�������");
		pPropList->SetItemPropHelpStr(CLDSLineTube::GetPropID("pickWorkPlane.y"),"������յ��Y�������");
		pPropList->SetItemPropHelpStr(CLDSLineTube::GetPropID("pickWorkPlane.z"),"������յ��Z�������");
	}
}
BOOL ModifyLineTubeProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineTube *pCurTube = (CLDSLineTube*)pView->selectObjs.GetFirst();
	CPropTreeItem *pFindItem,*pSonPropItem,*pParentItem,*pLeafPropItem;
	CLDSLineTube *pLineTube = NULL;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	CPropertyListOper<CLDSLineTube> oper(pPropList, pCurTube);
	if(pItem->m_idProp==CLDSLineTube::GetPropID("layer"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->SetLayer(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineTube->relativeObjs.GetFirst();pMirObj;pMirObj=pLineTube->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//��ת�Գ�
				CLDSLineTube *pMirLineTube=(CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINETUBE);
				if(pMirLineTube)
					CalMirAtomLayer(pLineTube->layer(),pMirLineTube->layer(),pMirObj->mirInfo);
			}
		}	
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("cMaterial"))
	{	//�ֹܲ���
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->cMaterial = QuerySteelBriefMatMark(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pLineTube->relativeObjs.GetFirst();pMirObj;pMirObj=pLineTube->relativeObjs.GetNext())
			{
				CLDSLineTube *pMirLineTube=(CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINETUBE);
				if(pMirLineTube)
					pMirLineTube->cMaterial=pLineTube->cMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("m_cQualityLevel"))
	{	//�ְ������ȼ�
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Default")==0)
				pLineTube->m_cQualityLevel=0;
			else
#else
			if(valueStr.CompareNoCase("Ĭ��")==0)
				pLineTube->m_cQualityLevel=0;
			else
#endif
				pLineTube->m_cQualityLevel = valueStr[0];
			for(RELATIVE_OBJECT *pMirObj=pLineTube->relativeObjs.GetFirst();pMirObj;pMirObj=pLineTube->relativeObjs.GetNext())
			{
				CLDSLineTube *pMirLineTube=(CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINETUBE);
				if(pMirLineTube)
					pMirLineTube->m_cQualityLevel=pLineTube->m_cQualityLevel;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("specification"))
	{	//�ֹܹ��
		CString ss;
		double wide=0,thick=0;
		restore_JG_guige(tem_str,wide,thick);
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->size_wide = wide;
			pLineTube->size_thick = thick;
			if(pLineTube->GetDiameter()>g_sysPara.fTubeWeldRoadMinD)
			{
				pLineTube->m_bHasWeldRoad=TRUE;
				ss.Format("��");
			}
			else
			{
				pLineTube->m_bHasWeldRoad=FALSE;
				ss.Format("��");
			}
			for(RELATIVE_OBJECT *pMirObj=pLineTube->relativeObjs.GetFirst();pMirObj;pMirObj=pLineTube->relativeObjs.GetNext())
			{
				CLDSLineTube *pMirLineTube=(CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINETUBE);
				if(pMirLineTube)
				{
					pMirLineTube->size_wide=wide;
					pMirLineTube->size_thick=thick;
					pMirLineTube->m_bHasWeldRoad=pLineTube->m_bHasWeldRoad;
				}
			}
		}
		pPropList->SetItemPropValue(CLDSLineTube::GetPropID("m_bHasWeldRoad"),ss);
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("m_bHasWeldRoad"),&pParentItem);
		if(pFindItem)
			ModifyLineTubeProperty(pPropList,pFindItem, ss);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("m_hBlock"))
	{
		long hBlock=RetrieveBlockHandle(valueStr);
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->m_hBlock=hBlock;
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("iSeg"))
	{
		SEGI iSeg = SEGI(valueStr.GetBuffer());
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->iSeg = iSeg;
			for(RELATIVE_OBJECT *pMirObj=pLineTube->relativeObjs.GetFirst();pMirObj;pMirObj=pLineTube->relativeObjs.GetNext())
			{
				CLDSLineTube *pMirLineTube=(CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINETUBE);
				if(pMirLineTube)
					pMirLineTube->iSeg=pLineTube->iSeg;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("sPartNo"))
	{
		bool parsed=false;
		SEGI iSeg;
		if(CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(tem_str,&iSeg,NULL))
		{
			parsed=true;
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("iSeg"),iSeg.ToString());
		}
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{	
			if(parsed)
				pLineTube->iSeg=iSeg;
			pLineTube->SetPartNo(tem_str);
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("cfgword"))
	{
		//���������˼�����ĺ�
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->cfgword = pCurTube->cfgword;
			for(RELATIVE_OBJECT *pMirObj=pLineTube->relativeObjs.GetFirst();pMirObj;pMirObj=pLineTube->relativeObjs.GetNext())
			{
				CLDSLineTube *pMirLineTube=(CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINETUBE);
				if(pMirLineTube)
					pMirLineTube->cfgword=pLineTube->cfgword;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("m_uStatMatCoef"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->m_uStatMatCoef = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("m_hPartWeldParent"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->m_hPartWeldParent=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("m_bDisplayHole"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("��")==0)
				pLineTube->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
				pLineTube->m_bDisplayHole = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pLineTube->relativeObjs.GetFirst();pMirObj;pMirObj=pLineTube->relativeObjs.GetNext())
			{
				CLDSLineTube *pMirLineTube=(CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINETUBE);
				if(pMirLineTube)
					pMirLineTube->m_bDisplayHole=pLineTube->m_bDisplayHole;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("m_bVirtualPart"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("��")==0)
				pLineTube->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
				pLineTube->m_bVirtualPart = FALSE;
			//���⹹�����Բ��ܶԳ�,�����ڻ�������ʱ�ǳ�������(ֻ��1����,����3����Ҳ��Ҫ��ģ) wjh-2015.4.30
			//for(RELATIVE_OBJECT *pMirObj=pLineTube->relativeObjs.GetFirst();pMirObj;pMirObj=pLineTube->relativeObjs.GetNext())
			//{
			//	CLDSLineTube *pMirLineTube=(CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINETUBE);
			//	if(pMirLineTube)
			//		pMirLineTube->m_bVirtualPart=pLineTube->m_bVirtualPart;
			//}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("m_bUserSpecColor"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("��")==0)
				pLineTube->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
				pLineTube->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pLineTube->relativeObjs.GetFirst();pMirObj;pMirObj=pLineTube->relativeObjs.GetNext())
			{
				CLDSLineTube *pMirLineTube=(CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINETUBE);
				if(pMirLineTube)
					pMirLineTube->m_bUserSpecColor=pLineTube->m_bUserSpecColor;
			}
		}
		pPropList->DeleteAllSonItems(pItem);
		if(pCurTube->m_bUserSpecColor)
			oper.InsertCmbColorPropItem(pItem, "crMaterial", "", "", "", -1, TRUE);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pLineTube->relativeObjs.GetFirst();pMirObj;pMirObj=pLineTube->relativeObjs.GetNext())
			{
				CLDSLineTube *pMirLineTube=(CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINETUBE);
				if(pMirLineTube)
					pMirLineTube->crMaterial=pLineTube->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("m_hArcLift"))
	{
		CLDSArcLift *pLift=NULL;
		for(pLift=Ta.ArcLift.GetFirst();pLift;pLift=Ta.ArcLift.GetNext())
		{
			CString sLiftName;
			sLiftName.Format("%s(%d)",pLift->name,pLift->iNo);
			if(sLiftName.CompareNoCase(valueStr)==0)
				break;
		}
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			if(pLift)
				pLineTube->SetArcLift(pLift->handle);
			else 
				pLineTube->SetArcLift(0);
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("sNotes"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			_snprintf(pLineTube->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
			//��ע���ù����޸� wht 10-10-22
			/*for(RELATIVE_OBJECT *pMirObj=pLineTube->relativeObjs.GetFirst();pMirObj;pMirObj=pLineTube->relativeObjs.GetNext())
			{
			CLDSLineTube *pMirLineTube=(CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINETUBE);
			if(pMirLineTube)
			strcpy(pMirLineTube->sNotes,pLineTube->sNotes);
			}*/
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("m_bHasWeldRoad"))
	{
		char sText[MAX_TEMP_CHAR_100+1]="";
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("��")==0)
				pLineTube->m_bHasWeldRoad = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
				pLineTube->m_bHasWeldRoad = FALSE;
		}
		//
		pPropList->DeleteAllSonItems(pItem);
		if(pCurTube->m_bHasWeldRoad)
		{
			//������ָ����ʽ
			pSonPropItem = oper.InsertCmbListPropItem(pItem, "m_iWeldRoadStyle", "", "", "", -1, TRUE);
			//���׼�߽Ƕ�
			if (pCurTube->m_iWeldRoadStyle == 1)
				oper.InsertEditPropItem(pSonPropItem, "weldRoadAngle", "", "", -1, TRUE);
			//��������
			pSonPropItem = oper.InsertButtonPropItem(pItem, "nearWeldRoadVec", "", "", -1, TRUE);
			pSonPropItem->m_bHideChildren = FALSE;
			if(CLDSLineTube::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pSonPropItem->m_idProp);
			pPropList->SetItemReadOnly(pSonPropItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1||
										pCurTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			//�����߷���X����
			pLeafPropItem = oper.InsertEditPropItem(pSonPropItem, "nearWeldRoadVec.x", "", "", -1, TRUE);
			pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
			pPropList->SetItemReadOnly(pLeafPropItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1||
										pCurTube->GetObjPropReadOnlyDefaultState(pLeafPropItem->m_idProp));
			//�����߷���Y����
			pLeafPropItem = oper.InsertEditPropItem(pSonPropItem, "nearWeldRoadVec.y", "", "", -1, TRUE);
			pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs, pLeafPropItem->m_idProp);
			pPropList->SetItemReadOnly(pLeafPropItem->m_idProp, pCurTube->m_iWeldRoadStyle == 1 ||
				pCurTube->GetObjPropReadOnlyDefaultState(pLeafPropItem->m_idProp));
			//�����߷���Z����
			pLeafPropItem = oper.InsertEditPropItem(pSonPropItem, "nearWeldRoadVec.z", "", "", -1, TRUE);
			pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs, pLeafPropItem->m_idProp);
			pPropList->SetItemReadOnly(pLeafPropItem->m_idProp, pCurTube->m_iWeldRoadStyle == 1 ||
				pCurTube->GetObjPropReadOnlyDefaultState(pLeafPropItem->m_idProp));
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("m_iWeldRoadStyle"))
	{
		if(valueStr.CompareNoCase("0.ֱ��ָ��")==0)
			pCurTube->m_iWeldRoadStyle = 0;
		else
			pCurTube->m_iWeldRoadStyle = 1;
		//
		pPropList->DeleteAllSonItems(pItem);
		if (pCurTube->m_iWeldRoadStyle == 1)
			oper.InsertEditPropItem(pItem, "weldRoadAngle", "", "", -1, TRUE);
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec"), &pParentItem);
		if(pFindItem)
			pPropList->SetItemReadOnly(pFindItem->m_idProp, pCurTube->m_iWeldRoadStyle == 1 || pCurTube->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec.x"), &pParentItem);
		if (pFindItem)
			pPropList->SetItemReadOnly(pFindItem->m_idProp, pCurTube->m_iWeldRoadStyle == 1 || pCurTube->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec.y"), &pParentItem);
		if (pFindItem)
			pPropList->SetItemReadOnly(pFindItem->m_idProp, pCurTube->m_iWeldRoadStyle == 1 || pCurTube->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec.z"), &pParentItem);
		if (pFindItem)
			pPropList->SetItemReadOnly(pFindItem->m_idProp, pCurTube->m_iWeldRoadStyle == 1 || pCurTube->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("weldRoadAngle"))
	{
		char sText[MAX_TEMP_CHAR_100+1]="";
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			double fWeldRoadAngle=atof(valueStr);
			pLineTube->CalWeldRoadVec(fWeldRoadAngle);//���¼��㹤���淨��
		}
		oper.UpdatePropItemValue("nearWeldRoadVec.x");
		oper.UpdatePropItemValue("nearWeldRoadVec.y");
		oper.UpdatePropItemValue("nearWeldRoadVec.z");
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("nearWeldRoadVec.x"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->nearWeldRoadVec.x = atof(valueStr);
			pLineTube->CalWorkPlaneNorm();	//���¼��㹤���淨��
		}
		char sText[MAX_TEMP_CHAR_100+1]="";
		pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();
		//�ѻ���ת��Ϊ�ǶȲ���֤��0-360֮��
		double fAngle = pLineTube->CalWeldRoadAngle()*DEGTORAD_COEF;
		int iTmp=0;
		if(fAngle>0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360;
		}
		else if(fAngle<0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360+360;
		}
		_snprintf(sText,MAX_TEMP_CHAR_100,"%f",fAngle);
		pPropList->SetItemPropValue(CLDSLineTube::GetPropID("weldRoadAngle"),CString(sText));
		SimplifiedNumString(sText);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("nearWeldRoadVec.y"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->nearWeldRoadVec.y = atof(valueStr);
			pLineTube->CalWorkPlaneNorm();	//���¼��㹤���淨��
		}
		char sText[MAX_TEMP_CHAR_100+1]="";
		pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();
		//�ѻ���ת��Ϊ�ǶȲ���֤��0-360֮��
		double fAngle = pLineTube->CalWeldRoadAngle()*DEGTORAD_COEF;
		int iTmp=0;
		if(fAngle>0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360;
		}
		else if(fAngle<0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360+360;
		}
		_snprintf(sText,MAX_TEMP_CHAR_100,"%f",fAngle);
		pPropList->SetItemPropValue(CLDSLineTube::GetPropID("weldRoadAngle"),CString(sText));
		SimplifiedNumString(sText);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("nearWeldRoadVec.z"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->nearWeldRoadVec.z = atof(valueStr);
			pLineTube->CalWorkPlaneNorm();	//���¼��㹤���淨��
		}
		char sText[MAX_TEMP_CHAR_100+1]="";
		pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();
		//�ѻ���ת��Ϊ�ǶȲ���֤��0-360֮��
		double fAngle = pLineTube->CalWeldRoadAngle()*DEGTORAD_COEF;
		int iTmp=0;
		if(fAngle>0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360;
		}
		else if(fAngle<0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360+360;
		}
		_snprintf(sText,MAX_TEMP_CHAR_100,"%f",fAngle);
		SimplifiedNumString(sText);
		pPropList->SetItemPropValue(CLDSLineTube::GetPropID("weldRoadAngle"),CString(sText));
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("m_iNormCalStyle"))
	{
		pCurTube->m_iNormCalStyle = valueStr[0]-'0';
		//
		pPropList->DeleteAllSonItems(pItem);
		if(pCurTube->m_iNormCalStyle == 1||pCurTube->m_iNormCalStyle == 2)
		{
			if (pCurTube->m_iNormCalStyle == 1)
				oper.InsertBtnEditPropItem(pItem, "hWorkNormRefPart", "", "", -1, TRUE);
			CPropTreeItem *pCurItem = oper.InsertButtonPropItem(pItem, "pickWorkPlane", "", "", -1, TRUE);
			oper.InsertEditPropItem(pCurItem, "pickWorkPlane.x", "", "", -1, TRUE);
			oper.InsertEditPropItem(pCurItem, "pickWorkPlane.y", "", "", -1, TRUE);
			oper.InsertEditPropItem(pCurItem, "pickWorkPlane.z", "", "", -1, TRUE);
			UpdatePickWorkPlaneItemsPropStr(pPropList,pCurTube->m_iNormCalStyle);
		}
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("workPlaneNorm.x"),&pParentItem);
		pPropList->SetItemReadOnly(pFindItem->m_idProp,pCurTube->m_iNormCalStyle==2||
									pCurTube->m_iNormCalStyle==1||pCurTube->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("workPlaneNorm.y"),&pParentItem);
		pPropList->SetItemReadOnly(pFindItem->m_idProp,pCurTube->m_iNormCalStyle==2||
									pCurTube->m_iNormCalStyle==1||pCurTube->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("workPlaneNorm.z"),&pParentItem);
		pPropList->SetItemReadOnly(pFindItem->m_idProp,pCurTube->m_iNormCalStyle==2||
									pCurTube->m_iNormCalStyle==1||pCurTube->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		for(CLDSLineTube* pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->m_iNormCalStyle=pCurTube->m_iNormCalStyle;
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("pickWorkPlane.x"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->pickWorkPlane[0] = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("pickWorkPlane.y"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->pickWorkPlane[1] = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("pickWorkPlane.z"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->pickWorkPlane[2] = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("hWorkNormRefPart"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineTube->hWorkNormRefPart);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("cutPlaneOrigin.x"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->cutPlaneOrigin.x = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("cutPlaneOrigin.y"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->cutPlaneOrigin.y = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("cutPlaneOrigin.z"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->cutPlaneOrigin.z = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("cutPlaneNorm.x"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->cutPlaneNorm.x = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("cutPlaneNorm.y"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->cutPlaneNorm.y = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("cutPlaneNorm.z"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->cutPlaneNorm.z = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("oddStart"))
	{
		CAngleMoveOperObject oddmodified;
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{	//����ֶ������˸˼�����ͷӦ���ȣ�������ͷ�ļ��㷽ʽ�޸�Ϊ������
			//�����������ø˼���صĸְ�ʱ�������¼�������ͷ��ֵ	wht 09-09-02
			pLineTube->desStartOdd.m_iOddCalStyle=2;
			pLineTube->SetStartOdd(atof(valueStr),CLDSLineTube::ciFlTubeOddment==1);
			//���ĶԽӸֹ����Ӷ˵�����ͷ wjh-2015.3.20
			if(g_sysPara.m_bModifyCoupledFLTubeOddment)
			{
				CLDSLineTube* pCoupleTube=FindTubeInLine(pLineTube,TRUE);
				if(pCoupleTube&&pCoupleTube->pStart==pLineTube->pStart&&(
					pCoupleTube->m_tJointStart.type==2||pCoupleTube->m_tJointStart.type==6))
					pCoupleTube->SetStartOdd(-pLineTube->startOdd(true),true);
				else if(pCoupleTube&&pCoupleTube->pEnd==pLineTube->pStart&&(
					pCoupleTube->m_tJointEnd.type==2||pCoupleTube->m_tJointEnd.type==6))
					pCoupleTube->SetEndOdd(-pLineTube->startOdd(true),true);
			}
		}
		//���¸ֹܳ���
		oper.UpdatePropItemValue("oddStart.m_iOddCalStyle");
		oper.UpdatePropItemValue("length");
	}
	else if (pItem->m_idProp == CLDSLineTube::GetPropID("oddStart.m_iOddCalStyle"))
	{
		CAngleMoveOperObject oddmodified;
		for (pLineTube = (CLDSLineTube*)pView->selectObjs.GetFirst(); pLineTube; pLineTube = (CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->desStartOdd.m_iOddCalStyle = valueStr[0] - '0';
			pLineTube->CalStartOddment();
		}
		//���¸ֹܳ���
		oper.UpdatePropItemValue("oddStart");
		oper.UpdatePropItemValue("length");
	}
	else if (pItem->m_idProp == CLDSLineTube::GetPropID("oddStart.m_fCollideDist"))
	{
		CUndoOperObject undo(&Ta);
		CAngleMoveOperObject oddmodified;
		for (pLineTube = (CLDSLineTube*)pView->selectObjs.GetFirst(); pLineTube; pLineTube = (CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->desStartOdd.m_fCollideDist = atof(valueStr);
			pLineTube->CalStartOddment();
		}
		//���¸ֹܳ���
		oper.UpdatePropItemValue("oddStart");
		oper.UpdatePropItemValue("length");
	}
	else if (pItem->m_idProp == CLDSLineTube::GetPropID("oddStart.m_hRefPart1"))
	{
		CAngleMoveOperObject oddmodified;
		for (pLineTube = (CLDSLineTube*)pView->selectObjs.GetFirst(); pLineTube; pLineTube = (CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->desStartOdd.m_hRefPart1 = HexStrToLong(valueStr);
			pLineTube->CalStartOddment();
		}
		//���¸ֹܳ���
		oper.UpdatePropItemValue("oddStart");
		oper.UpdatePropItemValue("length");
	}
	else if (pItem->m_idProp == CLDSLineTube::GetPropID("oddStart.m_hRefPart2"))
	{
		CAngleMoveOperObject oddmodified;
		for (pLineTube = (CLDSLineTube*)pView->selectObjs.GetFirst(); pLineTube; pLineTube = (CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->desStartOdd.m_hRefPart2 = HexStrToLong(valueStr);
			pLineTube->CalStartOddment();
		}
		//���¸ֹܳ���
		oper.UpdatePropItemValue("oddStart");
		oper.UpdatePropItemValue("length");
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("startPos.x"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->pStart->SetPositionX(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("startPos.y"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->pStart->SetPositionY(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("startPos.z"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->pStart->SetPositionZ(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.endPosType"))
	{
		int end_pos_type=0;
		if(valueStr.CompareNoCase("�ڵ㼴����")==0)
			end_pos_type = 0;
		else if(valueStr.CompareNoCase("�������")==0)
			end_pos_type = 1;
		else if(valueStr.CompareNoCase("����ͶӰ")==0)
			end_pos_type = 2;
		else if(valueStr.CompareNoCase("���߽���")==0)
			end_pos_type = 3;
		else if(valueStr.CompareNoCase("ָ������")==0)
			end_pos_type = 4;
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->desStartPos.endPosType = end_pos_type;
		UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.datum_pos_style"))
	{
		int datum_pos_style=0;
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
			datum_pos_style = 5;
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->desStartPos.datum_pos_style = datum_pos_style;
		pParentItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("desStartPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
		if(datum_pos_style>2)
		{
			CReportVectorDlg vecDlg;
			vecDlg.m_sCaption = "ƽ���淨��";
			vecDlg.m_fVectorX = pCurTube->desStartPos.face_offset_norm.x;
			vecDlg.m_fVectorY = pCurTube->desStartPos.face_offset_norm.y;
			vecDlg.m_fVectorZ = pCurTube->desStartPos.face_offset_norm.z;
			if(vecDlg.DoModal()==IDOK)
			{
				for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
					pLineTube->desStartPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumPart"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineTube->desStartPos.hDatumPart);
		pParentItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("desStartPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.fNormOffset"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->desStartPos.fNormOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.fEccentricDist"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->desStartPos.fEccentricDist = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.fLenVecOffsetDist"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->desStartPos.fLenVecOffsetDist = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumStartPart"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineTube->desStartPos.hDatumStartPart);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.bDatumStartStartEnd"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineTube->desStartPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
				pLineTube->desStartPos.bDatumStartStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumEndPart"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineTube->desStartPos.hDatumEndPart);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.bDatumEndStartEnd"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineTube->desStartPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
				pLineTube->desStartPos.bDatumEndStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("oddEnd"))
	{
		CAngleMoveOperObject oddmodified;
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{	//����ֶ������˸˼�����ͷӦ���ȣ�������ͷ�ļ��㷽ʽ�޸�Ϊ������
			//�����������ø˼���صĸְ�ʱ�������¼�������ͷ��ֵ	wht 09-09-02
			pLineTube->desEndOdd.m_iOddCalStyle=2;
			pLineTube->SetEndOdd(atof(valueStr),CLDSLineTube::ciFlTubeOddment==1);
			//���ĶԽӸֹ����Ӷ˵�����ͷ wjh-2015.3.20
			if(g_sysPara.m_bModifyCoupledFLTubeOddment)
			{
				CLDSLineTube* pCoupleTube=FindTubeInLine(pLineTube,FALSE);
				if(pCoupleTube&&pCoupleTube->pStart==pLineTube->pEnd&&(
					pCoupleTube->m_tJointStart.type==2||pCoupleTube->m_tJointStart.type==6))
					pCoupleTube->SetStartOdd(-pLineTube->endOdd(true),true);
				else if(pCoupleTube&&pCoupleTube->pEnd==pLineTube->pEnd&&(
					pCoupleTube->m_tJointEnd.type==2||pCoupleTube->m_tJointEnd.type==6))
					pCoupleTube->SetEndOdd(-pLineTube->endOdd(true),true);
			}
		}
		//���¸ֹܳ���
		oper.UpdatePropItemValue("oddEnd.m_iOddCalStyle");
		oper.UpdatePropItemValue("length");
	}
	else if (pItem->m_idProp == CLDSLineTube::GetPropID("oddEnd.m_iOddCalStyle"))
	{
		CAngleMoveOperObject oddmodified;
		for (pLineTube = (CLDSLineTube*)pView->selectObjs.GetFirst(); pLineTube; pLineTube = (CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->desEndOdd.m_iOddCalStyle = valueStr[0] - '0';
			pLineTube->CalEndOddment();
		}
		//���¸ֹܳ���
		oper.UpdatePropItemValue("oddEnd");
		oper.UpdatePropItemValue("length");
	}
	else if (pItem->m_idProp == CLDSLineTube::GetPropID("oddEnd.m_fCollideDist"))
	{
		CAngleMoveOperObject oddmodified;
		for (pLineTube = (CLDSLineTube*)pView->selectObjs.GetFirst(); pLineTube; pLineTube = (CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->desEndOdd.m_fCollideDist = atof(valueStr);
			pLineTube->CalEndOddment();
		}
		//���¸ֹܳ���
		oper.UpdatePropItemValue("oddEnd");
		oper.UpdatePropItemValue("length");
	}
	else if (pItem->m_idProp == CLDSLineTube::GetPropID("oddEnd.m_hRefPart1"))
	{
		CAngleMoveOperObject oddmodified;
		for (pLineTube = (CLDSLineTube*)pView->selectObjs.GetFirst(); pLineTube; pLineTube = (CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->desEndOdd.m_hRefPart1 = HexStrToLong(valueStr);
			pLineTube->CalEndOddment();
		}
		//���¸ֹܳ���
		oper.UpdatePropItemValue("oddEnd");
		oper.UpdatePropItemValue("length");
	}
	else if (pItem->m_idProp == CLDSLineTube::GetPropID("oddEnd.m_hRefPart2"))
	{
		CAngleMoveOperObject oddmodified;
		for (pLineTube = (CLDSLineTube*)pView->selectObjs.GetFirst(); pLineTube; pLineTube = (CLDSLineTube*)pView->selectObjs.GetNext())
		{
			pLineTube->desEndOdd.m_hRefPart2 = HexStrToLong(valueStr);
			pLineTube->CalEndOddment();
		}
		//���¸ֹܳ���
		oper.UpdatePropItemValue("oddEnd");
		oper.UpdatePropItemValue("length");
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("endPos.x"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->pEnd->SetPositionX(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("endPos.y"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->pEnd->SetPositionY(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("endPos.z"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->pEnd->SetPositionZ(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.endPosType"))
	{
		int end_pos_type=0;
		if(valueStr.CompareNoCase("�ڵ㼴����")==0)
			end_pos_type = 0;
		else if(valueStr.CompareNoCase("�������")==0)
			end_pos_type = 1;
		else if(valueStr.CompareNoCase("����ͶӰ")==0)
			end_pos_type = 2;
		else if(valueStr.CompareNoCase("���߽���")==0)
			end_pos_type = 3;
		else if(valueStr.CompareNoCase("ָ������")==0)
			end_pos_type = 4;
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->desEndPos.endPosType = end_pos_type;
		UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.datum_pos_style"))
	{
		int datum_pos_style=0;
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
			datum_pos_style = 5;
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->desEndPos.datum_pos_style = datum_pos_style;
		pParentItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("desEndPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
		if(datum_pos_style>2)
		{
			CReportVectorDlg vecDlg;
			vecDlg.m_sCaption = "ƽ���淨��";
			vecDlg.m_fVectorX = pCurTube->desEndPos.face_offset_norm.x;
			vecDlg.m_fVectorY = pCurTube->desEndPos.face_offset_norm.y;
			vecDlg.m_fVectorZ = pCurTube->desEndPos.face_offset_norm.z;
			if(vecDlg.DoModal()==IDOK)
			{
				for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
					pLineTube->desEndPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.hDatumPart"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineTube->desEndPos.hDatumPart);
		pParentItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("desEndPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.fNormOffset"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->desEndPos.fNormOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.fEccentricDist"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->desEndPos.fEccentricDist = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.fLenVecOffsetDist"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->desEndPos.fLenVecOffsetDist = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.hDatumStartPart"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineTube->desEndPos.hDatumStartPart);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.bDatumStartStartEnd"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineTube->desEndPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
				pLineTube->desEndPos.bDatumStartStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.hDatumEndPart"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pLineTube->desEndPos.hDatumEndPart);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.bDatumEndStartEnd"))
	{
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("ʼ��")==0)
				pLineTube->desEndPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
				pLineTube->desEndPos.bDatumEndStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("bush.height"))
	{
		pPropList->SetItemPropValue(CLDSLineTube::GetPropID("length"),valueStr);
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			pLineTube->bush.height=atoi(valueStr);
	}
	else
		return FALSE;
	for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
	{
		pLineTube->SetModified();
		pLineTube->CalPosition(true);
		pLineTube->CalWorkPlaneNorm();
		pLineTube->CalWeldRoadAngle();
		pLineTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pLineTube->GetSolidPartObject());
		//�޸�����ͷ���������������˨���Լ���ذ�
		if(pItem->m_idProp==CLDSLineTube::GetPropID("oddStart")||pItem->m_idProp==CLDSLineTube::GetPropID("oddEnd"))
		{	
			CLsRef *pLsRef=NULL;
			CLDSBolt *pFirstBolt=NULL;
			CLDSParamPlate *pParamPlate=NULL;
			if((pItem->m_idProp==CLDSLineTube::GetPropID("oddStart")&&pLineTube->m_tJointStart.type>=2&&pLineTube->m_tJointStart.type<=10))
				pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
			else if((pItem->m_idProp==CLDSLineTube::GetPropID("oddEnd")&&pLineTube->m_tJointEnd.type>=2&&pLineTube->m_tJointEnd.type<=10))
				pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
			if(pParamPlate)
			{
				pParamPlate->SetModified();
				pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
				for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{
					if(pFirstBolt==NULL)
						pFirstBolt=pLsRef->GetLsPtr();
					(*pLsRef)->SetModified();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
				for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
				{	//�������ɸ��ӹ����б��еĹ���
					CLDSPart *pPlate=console.FromPartHandle(pItem->h,CLS_PLATE,CLS_PARAMPLATE);
					if(!pPlate)
						continue;
					pPlate->SetModified();
					pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				}
			}
			if(pFirstBolt)
			{
				CLDSPlate *pPlate=NULL;
				for(pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
				{
					if(pPlate->FindLsByHandle(pFirstBolt->handle))
					{
						pPlate->SetModified();
						pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
						g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
						break;
					}
				}
			}
		}
	}
	//���¼�������һ�µ�ǰѡ�и˼��Ļ�׼�淨��
	pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();
	if(pLineTube!=NULL)
	{
		if(pLineTube->GetPropValueStr(CLDSLineTube::GetPropID("workPlaneNorm.x"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("workPlaneNorm.x"),CString(tem_str));
		if(pLineTube->GetPropValueStr(CLDSLineTube::GetPropID("workPlaneNorm.y"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("workPlaneNorm.y"),CString(tem_str));
		if(pLineTube->GetPropValueStr(CLDSLineTube::GetPropID("workPlaneNorm.z"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("workPlaneNorm.z"),CString(tem_str));
	}
	g_pSolidDraw->Draw();
	//theApp.GetLDSDoc()->SetModifiedFlag();
	//Ta.EndUndoListen();
	return TRUE;
}

BOOL LineTubeButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem, *pParentItem;
	CLDSLineTube *pLineTube=NULL, *pFirstLineTube=NULL;
	CReportVectorDlg vecDlg;
	CReportPointDlg pointDlg;
	CCfgPartNoDlg cfgdlg;
	COddmentDesignDlg odddlg;
	char sText[MAX_TEMP_CHAR_100+1];
	CString valueStr = _T("");

	if(pView->selectObjs.GetNodeNum()>0)
		pFirstLineTube = (CLDSLineTube*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	CPropertyListOper<CLDSLineTube> oper(pPropList, pFirstLineTube);
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
	if(pItem->m_idProp==CLDSLineTube::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstLineTube->cfgword; 
		if(!pFirstLineTube->IsLegObj())
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
		{
			for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			{
				pLineTube->cfgword = cfgdlg.cfgword;	//�ڴ˴����µ�һ���ڵ������Ϻ�
				pLineTube->SetModified(TRUE,FALSE);
				pLineTube->SyncMirProp("cfgword");
			}
		}
		if(pFirstLineTube->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("sPartNo"))
	{
		CModifyRelativeObjPartNoDlg modify_partno_dlg;
		modify_partno_dlg.m_pWorkPart=pFirstLineTube;
		if(modify_partno_dlg.DoModal()==IDOK)
		{
			pFirstLineTube->SetPartNo(modify_partno_dlg.m_sCurObjPartNo);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
			SEGI iSeg;
			if(CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(modify_partno_dlg.m_sCurObjPartNo.GetString(),&iSeg,NULL))
			{
				//pView->selectObjs�����๹����ŵĶκ�ͬ����������CModifyRelativeObjPartNoDlg����� wjh-2016.6.14
				pFirstLineTube->iSeg=iSeg;
				pPropList->SetItemPropValue(CLDSLineTube::GetPropID("iSeg"),iSeg.ToString());
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("nearWeldRoadVec"))
	{
		vecDlg.m_sCaption = "�ֹܺ��������ڷ���";
		vecDlg.m_fVectorX = pFirstLineTube->nearWeldRoadVec.x;
		vecDlg.m_fVectorY = pFirstLineTube->nearWeldRoadVec.y;
		vecDlg.m_fVectorZ = pFirstLineTube->nearWeldRoadVec.z;
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1]="";
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("nearWeldRoadVec.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("nearWeldRoadVec.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("nearWeldRoadVec.z"),CString(sText));
			for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				pLineTube->nearWeldRoadVec.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("m_hPartWeldParent"))
	{	//ͨ��ѡ�񹹼��޸ĺ��Ӹ����������ڽ������޸ĵ��������ĺ��Ӹ�����
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸ĸֹܺ��Ӹ�����";
		modifyhdlg.m_sTitle="���Ӹ�����:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstLineTube->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ�񺸽Ӹ�����:");
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineTube::GetPropID("m_hPartWeldParent"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLinePart* pMirLinePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLinePart=(CLDSLinePart*)pLineTube->GetMirPart(msg);
						if(pMirLinePart)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//������ѡ�������ĶԳƹ��� wht 11-05-07
								pMirLinePart->m_hPartWeldParent=pMirPart->handle;
						}
					}
					pLineTube->m_hPartWeldParent=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineTube->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("hWorkNormRefPart"))
	{
		//ͨ��ѡ�񹹼��޸ķ��߲��ո˼�
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸ĸֹܷ��߲��չ���";
		modifyhdlg.m_sTitle="���߲��ո˼�:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstLineTube->hWorkNormRefPart);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ���߲��չ���:");
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineTube::GetPropID("hWorkNormRefPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineTube* pMirLineTubePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineTubePart=(CLDSLineTube*)pLineTube->GetMirPart(msg);
						if(pMirLineTubePart)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//������ѡ���߲��ո˼��ĶԳƹ��� wht 11-05-07
								pMirLineTubePart->hWorkNormRefPart=pMirPart->handle;
						}
					}
					pLineTube->hWorkNormRefPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineTube->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumPart")||pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.hDatumPart"))
	{
		//�޸Ĵ��λ�û�׼�ֹ�
		CModifyPartHandleDlg modifyhdlg;
		CTubeEndPosPara *pTubeEndPosPara=NULL;
		if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumPart"))
		{
			pTubeEndPosPara=&(pFirstLineTube->desStartPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineTube->desStartPos.hDatumPart);
		}
		else
		{
			pTubeEndPosPara=&(pFirstLineTube->desEndPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineTube->desEndPos.hDatumPart);
		}
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		if(pTubeEndPosPara&&pTubeEndPosPara->endPosType==3)
		{	//���߽��㶨λ
			modifyhdlg.m_sCaption="�޸����߽����׼�ֹ�";
			modifyhdlg.m_sTitle="��׼�ֹ�:";
			modifyhdlg.SetCmdPickPromptStr("��ѡ�����߽����׼�ֹ�:");
		}
		else
		{
			modifyhdlg.m_sCaption="�޸Ĵ��λ�û�׼�ֹ�";
			modifyhdlg.m_sTitle="��׼�ֹ�:";
			modifyhdlg.SetCmdPickPromptStr("��ѡ���ӻ�׼�ֹ�:");
		}
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				//������ʾ
				if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumPart"))
					pPropList->SetItemPropValue(CLDSLineTube::GetPropID("desStartPos.hDatumPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSLineTube::GetPropID("desEndPos.hDatumPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineTube* pMirLineTubePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineTubePart=(CLDSLineTube*)pLineTube->GetMirPart(msg);
						CLDSPart* pMirDatumPart=pPart->GetMirPart(msg);
						if(pMirLineTubePart&&pMirDatumPart)
						{
							if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumPart"))
								pMirLineTubePart->desStartPos.hDatumPart=pMirDatumPart->handle;
							else
								pMirLineTubePart->desEndPos.hDatumPart=pMirDatumPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumPart"))
						pLineTube->desStartPos.hDatumPart=pPart->handle;
					else
						pLineTube->desEndPos.hDatumPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineTube->handle);
				}
				if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumPart"))
				{
					pParentItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("desStartPos.endPosType"),NULL);
					if(pParentItem)
						UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
				}
				else
				{
					pParentItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("desEndPos.endPosType"),NULL);
					if(pParentItem)
						UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumStartPart")||pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.hDatumStartPart"))
	{
		//�޸�ͶӰ��ʼ�����ڻ�׼�ֹ�
		CModifyPartHandleDlg modifyhdlg;
		CTubeEndPosPara *pTubeEndPosPara=NULL;
		if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumStartPart"))
		{
			pTubeEndPosPara=&(pFirstLineTube->desStartPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineTube->desStartPos.hDatumStartPart);
		}
		else
		{
			pTubeEndPosPara=&(pFirstLineTube->desEndPos);
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineTube->desEndPos.hDatumStartPart);
		}
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		if(pTubeEndPosPara&&pTubeEndPosPara->endPosType==3)
		{	//���߽��㶨λ
			modifyhdlg.m_sCaption="�޸Ľ���ֹ�";
			modifyhdlg.m_sTitle="����ֹ�:";
			modifyhdlg.SetCmdPickPromptStr("��ѡ�����߽��㽻��ֹ�:");
		}
		else
		{
			modifyhdlg.m_sCaption="�޸�ͶӰʼ�����ڻ�׼�ֹ�";
			modifyhdlg.m_sTitle="��׼�ֹ�:";
			modifyhdlg.SetCmdPickPromptStr("��ѡ��ͶӰ��׼�ֹ�:");
		}
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				//������ʾ
				if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumStartPart"))
					pPropList->SetItemPropValue(CLDSLineTube::GetPropID("desStartPos.hDatumStartPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSLineTube::GetPropID("desEndPos.hDatumStartPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineTube* pMirLineTubePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineTubePart=(CLDSLineTube*)pLineTube->GetMirPart(msg);
						if(pMirLineTubePart)
						{
							if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumStartPart"))
								pMirLineTubePart->desStartPos.hDatumStartPart=pPart->handle;
							else
								pMirLineTubePart->desEndPos.hDatumStartPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumStartPart"))
						pFirstLineTube->desStartPos.hDatumStartPart=pPart->handle;
					else
						pFirstLineTube->desEndPos.hDatumStartPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineTube->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumEndPart")||pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.hDatumEndPart"))
	{
		//�޸�ͶӰ���ն����ڻ�׼�ֹ�
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸�ͶӰ�ն����ڻ�׼�ֹ�";
		modifyhdlg.m_sTitle="��׼�ֹ�:";
		if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumEndPart"))
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineTube->desStartPos.hDatumEndPart);
		else
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineTube->desEndPos.hDatumEndPart);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ��ͶӰ��׼�ֹ�:");
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				//������ʾ
				if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumEndPart"))
					pPropList->SetItemPropValue(CLDSLineTube::GetPropID("desStartPos.hDatumEndPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSLineTube::GetPropID("desEndPos.hDatumEndPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLineTube* pMirLineTubePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineTubePart=(CLDSLineTube*)pLineTube->GetMirPart(msg);
						if(pMirLineTubePart)
						{
							if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumEndPart"))
								pMirLineTubePart->desStartPos.hDatumEndPart=pPart->handle;
							else
								pMirLineTubePart->desEndPos.hDatumEndPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.hDatumEndPart"))
						pFirstLineTube->desStartPos.hDatumEndPart=pPart->handle;
					else
						pFirstLineTube->desEndPos.hDatumEndPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pLineTube->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("pickWorkPlane"))
	{
		vecDlg.m_sCaption = "�ֹܻ�׼����Ʒ�������";
		vecDlg.m_fVectorX = pFirstLineTube->pickWorkPlane[0];
		vecDlg.m_fVectorY = pFirstLineTube->pickWorkPlane[1];
		vecDlg.m_fVectorZ = pFirstLineTube->pickWorkPlane[2];
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("pickWorkPlane.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("pickWorkPlane.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("pickWorkPlane.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("pickWorkPlane.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("pickWorkPlane.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("pickWorkPlane.z"),CString(sText));
			for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			{
				pLineTube->pickWorkPlane[0]=vecDlg.m_fVectorX;
				pLineTube->pickWorkPlane[1]=vecDlg.m_fVectorY;
				pLineTube->pickWorkPlane[2]=vecDlg.m_fVectorZ;
				pLineTube->CalWorkPlaneNorm();	//���¼���ֹܷ��߷���
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("workPlaneNorm")
		||pItem->m_idProp==CLDSLineTube::GetPropID("workPlaneNorm.x")
		||pItem->m_idProp==CLDSLineTube::GetPropID("workPlaneNorm.y")
		||pItem->m_idProp==CLDSLineTube::GetPropID("workPlaneNorm.z"))
	{
		vecDlg.m_sCaption = "�ֹܻ�׼�淨������";
		vecDlg.m_fVectorX = pFirstLineTube->WorkPlaneNorm().x;
		vecDlg.m_fVectorY = pFirstLineTube->WorkPlaneNorm().y;
		vecDlg.m_fVectorZ = pFirstLineTube->WorkPlaneNorm().z;
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("workPlaneNorm.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("workPlaneNorm.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("workPlaneNorm.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("workPlaneNorm.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("workPlaneNorm.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("workPlaneNorm.z"),CString(sText));
			for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				pLineTube->SetWorkPlaneNorm(f3dPoint(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ));
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("cutPlaneOrigin"))
	{
		pointDlg.m_sCaption = "�ֹ��и����������";
		pointDlg.m_fPointX = pFirstLineTube->cutPlaneOrigin.x;
		pointDlg.m_fPointY = pFirstLineTube->cutPlaneOrigin.y;
		pointDlg.m_fPointZ = pFirstLineTube->cutPlaneOrigin.z;
		if(pointDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("cutPlaneOrigin.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("cutPlaneOrigin.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("cutPlaneOrigin.z"),CString(sText));
			for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				pLineTube->cutPlaneOrigin.Set(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ);
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("cutPlaneNorm"))
	{
		vecDlg.m_sCaption = "�ֹ��и��淨������";
		vecDlg.m_fVectorX = pFirstLineTube->cutPlaneNorm.x;
		vecDlg.m_fVectorY = pFirstLineTube->cutPlaneNorm.y;
		vecDlg.m_fVectorZ = pFirstLineTube->cutPlaneNorm.z;
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("cutPlaneNorm.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("cutPlaneNorm.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("cutPlaneNorm.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("cutPlaneNorm.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("cutPlaneNorm.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("cutPlaneNorm.z"),CString(sText));
			for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				pLineTube->cutPlaneNorm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("hStart"))
	{
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸ĸ˼�ʼ�˽ڵ�";
		modifyhdlg.m_sTitle="ʼ�˽ڵ�:";
		if(pFirstLineTube->pStart)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineTube->pStart->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�ڵ�:");
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(modifyhdlg.DoModal()==IDOK)
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
			else
			{
				CUndoOperObject undo(&Ta);
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineTube::GetPropID("hStart"),valueStr);
				for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				{
					CLDSNode* pOldNode=pLineTube->pStart;
					pLineTube->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),pNode,NULL);
					pLineTube->pStart=pNode;
					pLineTube->SetModified();
					pLineTube->ClearFlag();
					pLineTube->CalPosition();
					//���¸˼��ϵĶ˽ڵ������Ϣ
					pLineTube->UpdateRodEndNodeRelation(pOldNode,pNode);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("hEnd"))
	{
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸ĸ˼��ն˽ڵ�";
		modifyhdlg.m_sTitle="�ն˽ڵ�:";
		if(pFirstLineTube->pEnd)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineTube->pEnd->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�ڵ�:");
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
			else
			{
				CUndoOperObject undo(&Ta);
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineTube::GetPropID("hEnd"),valueStr);
				for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				{
					CLDSNode* pOldNode=pLineTube->pEnd;
					pLineTube->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),NULL,pNode);
					pLineTube->pEnd=pNode;
					pLineTube->SetModified();
					pLineTube->ClearFlag();
					pLineTube->CalPosition();
					//���¸˼��ϵĶ˽ڵ������Ϣ
					pLineTube->UpdateRodEndNodeRelation(pOldNode,pNode);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("oddStart"))
	{
		odddlg.m_hRefPart1.Format("0X%X",pFirstLineTube->desStartOdd.m_hRefPart1);
		odddlg.m_hRefPart2.Format("0X%X",pFirstLineTube->desStartOdd.m_hRefPart2);
		odddlg.m_fCollideDist = pFirstLineTube->desStartOdd.m_fCollideDist;
		odddlg.m_iOddCalStyle = pFirstLineTube->desStartOdd.m_iOddCalStyle;
		if(odddlg.DoModal()==IDOK)
		{
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart1);
			sscanf(sText,"%X",&pFirstLineTube->desStartOdd.m_hRefPart1);
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart2);
			sscanf(sText,"%X",&pFirstLineTube->desStartOdd.m_hRefPart2);
			for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			{
				pLineTube->desStartOdd.m_hRefPart1=pFirstLineTube->desStartOdd.m_hRefPart1;
				pLineTube->desStartOdd.m_hRefPart2=pFirstLineTube->desStartOdd.m_hRefPart2;
				pLineTube->desStartOdd.m_fCollideDist = odddlg.m_fCollideDist;
				pLineTube->desStartOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
				pLineTube->CalStartOddment();
				char valueStr[MAX_TEMP_CHAR_50+1]="";
				if(pLineTube->GetPropValueStr(pItem->m_idProp,valueStr)>0)
					pItem->m_lpNodeInfo->m_strPropValue=valueStr;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("jointStart"))
	{
		endLjDlg.m_pBaseNode=pFirstLineTube->pStart;
		endLjDlg.m_bStartEnd=TRUE;
		endLjDlg.m_pCurLineTube=pFirstLineTube;
		//�����Ӽ���δָ���и��淨��ʱ,Ԥ�иֹܶ˲�����������
		if( pFirstLineTube->m_tJointStart.type==0&&!pFirstLineTube->m_tJointStart.cutPlaneNorm.IsZero())
			endLjDlg.m_iTubeEndLjStyle=0;
		else if(pFirstLineTube->m_tJointStart.type==1&&pFirstLineTube->m_tJointStart.hLinkObj>0x20)
			endLjDlg.m_iTubeEndLjStyle=1;
		else if(pFirstLineTube->m_tJointStart.type==2&&pFirstLineTube->m_tJointStart.hLinkObj>0x20)
			endLjDlg.m_iTubeEndLjStyle=2;
		else if(pFirstLineTube->m_tJointStart.hLinkObj>0x20)
			endLjDlg.m_iTubeEndLjStyle=pFirstLineTube->m_tJointStart.type;
		else if( pFirstLineTube->m_tJointStart.hLinkObj==NULL&&pFirstLineTube->cutPlaneNorm.IsZero()&&
			endLjDlg.m_pBaseNode->m_cPosCalType!=4&&FindTubeInLine(pFirstLineTube,endLjDlg.m_bStartEnd))
			endLjDlg.m_iTubeEndLjStyle=2;
		CUndoOperObject undo(&Ta,true);
		if(endLjDlg.DoModal()==IDOK)
		{
			if(pFirstLineTube->m_tJointStart.hLinkObj>0x20)
			{
				if(pFirstLineTube->m_tJointStart.type==0)
					pItem->m_lpNodeInfo->m_strPropValue.Format("0X%X /=",pFirstLineTube->m_tJointStart.hLinkObj);
				else
					pItem->m_lpNodeInfo->m_strPropValue.Format("0X%X",pFirstLineTube->m_tJointStart.hLinkObj);
			}
			else if(!pFirstLineTube->m_tJointStart.cutPlaneNorm.IsZero())
				pItem->m_lpNodeInfo->m_strPropValue = "ƽ���и�";
			else
				pItem->m_lpNodeInfo->m_strPropValue = "������";
			long idProp = CLDSLineTube::GetPropID("oddStart");
			char valueStr[MAX_TEMP_CHAR_50+1]="";
			if(pFirstLineTube->GetPropValueStr(idProp,valueStr)>0)
				pPropList->SetItemPropValue(idProp,CString(valueStr));
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("startPos"))
	{
		pointDlg.m_sCaption = "�ֹ�ʼ���������";
		pointDlg.m_fPointX = pFirstLineTube->Start().x;
		pointDlg.m_fPointY = pFirstLineTube->Start().y;
		pointDlg.m_fPointZ = pFirstLineTube->Start().z;
		if(pFirstLineTube->desStartPos.endPosType!=4)	//4.ָ������
			pointDlg.m_bReadOnly=TRUE;
		pointDlg.m_bLockPoint=pFirstLineTube->IsStartPosLocked();
		if(pointDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("startPos.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("startPos.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("startPos.z"),CString(sText));
			for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			{
				pLineTube->SetStart(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
				if(pointDlg.m_bLockPoint)
					pLineTube->LockStartPos();
				else 
					pLineTube->UnlockStartPos();
			}
		}
		//ʼ������
		if(pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart)
		{	//����������Ϊֻ������
			SetPropItemReadOnly(pFirstLineTube,"startPos.x",TRUE);
			SetPropItemReadOnly(pFirstLineTube,"startPos.y",TRUE);
			SetPropItemReadOnly(pFirstLineTube,"startPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ����ֻ������
			SetPropItemReadOnly(pFirstLineTube,"startPos.x",pFirstLineTube->desStartPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineTube,"startPos.y",pFirstLineTube->desStartPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineTube,"startPos.z",pFirstLineTube->desStartPos.endPosType!=4);	//4.ָ������
		}
		SetPropItemReadOnly(pFirstLineTube,"hStart",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"oddStart",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"desStartPos",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"desStartPos.endPosType",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"desStartPos.datum_pos_style",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"desStartPos.hDatumPart",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"desStartPos.fNormOffset",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"desStartPos.fEccentricDist",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"desStartPos.fLenVecOffsetDist",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"desStartPos.face_offset_norm",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"desStartPos.hDatumStartPart",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"desStartPos.bDatumStartStartEnd",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"desStartPos.hDatumEndPart",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstLineTube,"desStartPos.bDatumEndStartEnd",pFirstLineTube->IsStartPosLocked()||pFirstLineTube->bHuoQuStart);
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("actualStartPos"))
	{
		pointDlg.m_sCaption = "�ֹ�ʼ��ʵ������";
		pointDlg.m_bEnableLock=FALSE;
		pointDlg.m_bReadOnly=TRUE;
		f3dPoint actualStart,vec=pFirstLineTube->Start()-pFirstLineTube->End();
		normalize(vec);
		actualStart = pFirstLineTube->Start()+vec*pFirstLineTube->startOdd();
		pointDlg.m_fPointX = actualStart.x;
		pointDlg.m_fPointY = actualStart.y;
		pointDlg.m_fPointZ = actualStart.z;
		pointDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("oddEnd"))
	{
		odddlg.m_hRefPart1.Format("0X%X",pFirstLineTube->desEndOdd.m_hRefPart1);
		odddlg.m_hRefPart2.Format("0X%X",pFirstLineTube->desEndOdd.m_hRefPart2);
		odddlg.m_fCollideDist = pFirstLineTube->desEndOdd.m_fCollideDist;
		odddlg.m_iOddCalStyle = pFirstLineTube->desEndOdd.m_iOddCalStyle;
		if(odddlg.DoModal()==IDOK)
		{
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart1);
			sscanf(sText,"%X",&pFirstLineTube->desEndOdd.m_hRefPart1);
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart2);
			sscanf(sText,"%X",&pFirstLineTube->desEndOdd.m_hRefPart2);
			for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			{
				pLineTube->desEndOdd.m_hRefPart1=pFirstLineTube->desEndOdd.m_hRefPart1;
				pLineTube->desEndOdd.m_hRefPart2=pFirstLineTube->desEndOdd.m_hRefPart2;
				pLineTube->desEndOdd.m_fCollideDist = odddlg.m_fCollideDist;
				pLineTube->desEndOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
				pLineTube->CalEndOddment();
				char valueStr[MAX_TEMP_CHAR_50+1]="";
				if(pLineTube->GetPropValueStr(pItem->m_idProp,valueStr)>0)
					pItem->m_lpNodeInfo->m_strPropValue=valueStr;
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("jointEnd"))
	{
		endLjDlg.m_pBaseNode=pFirstLineTube->pEnd;
		endLjDlg.m_bStartEnd=FALSE;
		endLjDlg.m_pCurLineTube=pFirstLineTube;
		//�����Ӽ���δָ���и��淨��ʱ,Ԥ�иֹܶ˷�����������
		if( pFirstLineTube->m_tJointEnd.type==0&&!pFirstLineTube->m_tJointEnd.cutPlaneNorm.IsZero())
			endLjDlg.m_iTubeEndLjStyle=0;
		else if(pFirstLineTube->m_tJointEnd.type==1&&pFirstLineTube->m_tJointEnd.hLinkObj>0x20)
			endLjDlg.m_iTubeEndLjStyle=1;
		else if(pFirstLineTube->m_tJointEnd.type==2&&pFirstLineTube->m_tJointEnd.hLinkObj>0x20)
			endLjDlg.m_iTubeEndLjStyle=2;
		else if(pFirstLineTube->m_tJointEnd.hLinkObj>0x20)
			endLjDlg.m_iTubeEndLjStyle=pFirstLineTube->m_tJointEnd.type;
		else if( pFirstLineTube->m_tJointEnd.hLinkObj==NULL&&pFirstLineTube->cutPlaneNorm.IsZero()&&
			endLjDlg.m_pBaseNode->m_cPosCalType!=4&&FindTubeInLine(pFirstLineTube,endLjDlg.m_bStartEnd))
			endLjDlg.m_iTubeEndLjStyle=2;
		CUndoOperObject undo(&Ta,true);
		if(endLjDlg.DoModal()==IDOK)
		{
			if(pFirstLineTube->m_tJointEnd.hLinkObj)
			{
				if(pFirstLineTube->m_tJointEnd.type==0)
					pItem->m_lpNodeInfo->m_strPropValue.Format("0X%X /=",pFirstLineTube->m_tJointEnd.hLinkObj);
				else
					pItem->m_lpNodeInfo->m_strPropValue.Format("0X%X",pFirstLineTube->m_tJointEnd.hLinkObj);
			}
			else if(!pFirstLineTube->m_tJointEnd.cutPlaneNorm.IsZero())
				pItem->m_lpNodeInfo->m_strPropValue = "ƽ���и�";
			else
				pItem->m_lpNodeInfo->m_strPropValue = "������";
			long idProp = CLDSLineTube::GetPropID("oddEnd");
			char valueStr[MAX_TEMP_CHAR_50+1]="";
			if(pFirstLineTube->GetPropValueStr(idProp,valueStr)>0)
				pPropList->SetItemPropValue(idProp,CString(valueStr));
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("endPos"))
	{
		pointDlg.m_sCaption = "�ֹ��ն��������";
		pointDlg.m_fPointX = pFirstLineTube->End().x;
		pointDlg.m_fPointY = pFirstLineTube->End().y;
		pointDlg.m_fPointZ = pFirstLineTube->End().z;
		if(pFirstLineTube->desEndPos.endPosType!=4)	//4.ָ������
			pointDlg.m_bReadOnly=TRUE;
		pointDlg.m_bLockPoint=pFirstLineTube->IsEndPosLocked();
		if(pointDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("endPos.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("endPos.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSLineTube::GetPropID("endPos.z"),CString(sText));
			for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
			{
				pLineTube->SetEnd(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
				if(pointDlg.m_bLockPoint)
					pLineTube->LockEndPos();
				else 
					pLineTube->UnlockEndPos();
			}
		}
		//�ն�����
		if(pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd)
		{	//����������Ϊֻ������
			SetPropItemReadOnly(pFirstLineTube,"endPos.x",TRUE);
			SetPropItemReadOnly(pFirstLineTube,"endPos.y",TRUE);
			SetPropItemReadOnly(pFirstLineTube,"endPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ����ֻ������
			SetPropItemReadOnly(pFirstLineTube,"endPos.x",pFirstLineTube->desEndPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstLineTube,"endPos.y",pFirstLineTube->desEndPos.endPosType!=4);
			SetPropItemReadOnly(pFirstLineTube,"endPos.z",pFirstLineTube->desEndPos.endPosType!=4);
		}
		SetPropItemReadOnly(pFirstLineTube,"hEnd",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"oddEnd",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"desEndPos",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"desEndPos.endPosType",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"desEndPos.datum_pos_style",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"desEndPos.hDatumPart",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"desEndPos.fNormOffset",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"desEndPos.fEccentricDist",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"desEndPos.fLenVecOffsetDist",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"desEndPos.face_offset_norm",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"desEndPos.hDatumStartPart",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"desEndPos.bDatumStartStartEnd",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"desEndPos.hDatumEndPart",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstLineTube,"desEndPos.bDatumEndStartEnd",pFirstLineTube->IsEndPosLocked()||pFirstLineTube->bHuoQuEnd);
	} 
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("actualEndPos"))
	{
		pointDlg.m_sCaption = "�ֹ��ն�ʵ������";
		pointDlg.m_bEnableLock=FALSE;
		pointDlg.m_bReadOnly=TRUE;
		f3dPoint actualEnd,vec=pFirstLineTube->End()-pFirstLineTube->Start();
		normalize(vec);
		actualEnd = pFirstLineTube->End()+vec*pFirstLineTube->endOdd();
		pointDlg.m_fPointX = actualEnd.x;
		pointDlg.m_fPointY = actualEnd.y;
		pointDlg.m_fPointZ = actualEnd.z;
		pointDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desStartPos.face_offset_norm"))
	{	
		vecDlg.m_sCaption = "ƽ���淨��";
		vecDlg.m_fVectorX = pFirstLineTube->desStartPos.face_offset_norm.x;
		vecDlg.m_fVectorY = pFirstLineTube->desStartPos.face_offset_norm.y;
		vecDlg.m_fVectorZ = pFirstLineTube->desStartPos.face_offset_norm.z;
		if(vecDlg.DoModal()==IDOK)
		{
			for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				pLineTube->desStartPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("desEndPos.face_offset_norm"))
	{	
		vecDlg.m_sCaption = "ƽ���淨��";
		vecDlg.m_fVectorX = pFirstLineTube->desEndPos.face_offset_norm.x;
		vecDlg.m_fVectorY = pFirstLineTube->desEndPos.face_offset_norm.y;
		vecDlg.m_fVectorZ = pFirstLineTube->desEndPos.face_offset_norm.z;
		if(vecDlg.DoModal()==IDOK)
		{
			for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
				pLineTube->desEndPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("startWorkPlane")
		||pItem->m_idProp==CLDSLineTube::GetPropID("endWorkPlane"))
	{
		CPartWorkPlaneDlg planeDlg;
		if(pItem->m_idProp==CLDSLineTube::GetPropID("startWorkPlane"))
			planeDlg.m_bStart=TRUE;
		else
			planeDlg.m_bStart=FALSE;
		planeDlg.m_pDatumPart=pFirstLineTube;
		planeDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("relativeObjs"))
	{
		CRelativeObjectDlg relativeDlg;
		relativeDlg.m_pWorkObj = pFirstLineTube;
		if(relativeDlg.DoModal()==IDOK)
		{
			CXhChar200 sText;
			pFirstLineTube->GetPropValueStr(pItem->m_idProp,sText);
			pPropList->SetItemPropValue(pItem->m_idProp,sText);
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("shadowInfo"))
	{
		CDesignShadowDlg shadowdlg;
		shadowdlg.SetShadowObj((CLDSObject*)pFirstLineTube);
		shadowdlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("bush.hBushResidePlate"))
	{
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸��׹������ְ�";
		modifyhdlg.m_sTitle="�����ְ�:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstLineTube->bush.hBushResidePlate);
		modifyhdlg.m_iMirMsg=0;
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ���׹������ְ�:");
		modifyhdlg.SetPickObjTypeId(CLS_PLATE);
		if(IDOK!=modifyhdlg.DoModal())
			return FALSE;
		CLDSPart* pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
		if(pPart==NULL || pPart->GetClassTypeId()!=CLS_PLATE)
		{
			AfxMessageBox("�����������Ҳ�����Ӧ������");
			return FALSE;
		}
		valueStr.Format("0X%X",modifyhdlg.m_nHandle);
		pPropList->SetItemPropValue(CLDSLineTube::GetPropID("m_hPartWeldParent"),valueStr);
		pPropList->SetItemPropValue(CLDSLineTube::GetPropID("bush.hBushResidePlate"),valueStr);
		g_pSolidDraw->ReleaseSnapStatus();
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{			
			pLineTube->m_hPartWeldParent=pPart->handle;
			pLineTube->bush.hBushResidePlate=pPart->handle;
			pLineTube->CalPosition();
		}
	}
	else if(pItem->m_idProp==CLDSLineTube::GetPropID("bush.hBushRelyBolt"))
	{
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸��׹ܶ�λ��˨";
		modifyhdlg.m_sTitle="��λ��˨:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstLineTube->bush.hBushRelyBolt);
		modifyhdlg.m_iMirMsg=0;
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ���׹ܶ�λ��˨:");
		modifyhdlg.SetPickObjTypeId(CLS_BOLT);
		if(IDOK!=modifyhdlg.DoModal())
			return FALSE;
		CLDSPart* pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
		if(pPart==NULL || pPart->GetClassTypeId()!=CLS_BOLT)
		{
			AfxMessageBox("�����������Ҳ�����Ӧ��˨��");
			return FALSE;
		}
		valueStr.Format("0X%X",modifyhdlg.m_nHandle);
		pPropList->SetItemPropValue(CLDSLineTube::GetPropID("bush.hBushRelyBolt"),valueStr);
		g_pSolidDraw->ReleaseSnapStatus();
		for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
		{			
			pLineTube->bush.hBushRelyBolt=pPart->handle;
			pLineTube->CalPosition();
		}
	}
	else if (pItem->m_idProp == CLDSLineTube::GetPropID("oddStart.m_hRefPart1") ||
			pItem->m_idProp == CLDSLineTube::GetPropID("oddStart.m_hRefPart2"))
	{
		int iRefPart1Or2 = 1;
		if (pItem->m_idProp == CLDSLineTube::GetPropID("oddStart.m_hRefPart1"))
			iRefPart1Or2 = 1;
		else
			iRefPart1Or2 = 2;
		static CModifyPartHandleDlg modifyhdlg;
		if (iRefPart1Or2 == 1)
			modifyhdlg.m_sHandle.Format("0X%X", pFirstLineTube->desStartOdd.m_hRefPart1);
		else
			modifyhdlg.m_sHandle.Format("0X%X", pFirstLineTube->desStartOdd.m_hRefPart2);

		modifyhdlg.m_sCaption = "�޸Ĳ��չ���";
		modifyhdlg.m_sTitle.Format("���չ���%d", iRefPart1Or2);
		modifyhdlg.m_bDispSelectBtn = TRUE;//��ʾѡ��ڵ�/�˼��İ�ť
		CString cmdStr;
		cmdStr.Format("��ѡ����չ���%d:", iRefPart1Or2);
		modifyhdlg.SetCmdPickPromptStr(cmdStr);
		modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
		if (modifyhdlg.DoModal() != IDOK)
			return FALSE;
		CLDSPart *pPart = console.FromPartHandle(modifyhdlg.m_nHandle);
		if (pPart == NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox(NULL, "The handle is error,so can't find the rod��", sSoftName, MB_OK);
#else 
			MessageBox(NULL, "�����������Ҳ�����Ӧ�˼���", sSoftName, MB_OK);
#endif
			return FALSE;
		}
		CUndoOperObject undo(&Ta);
		CAngleMoveOperObject oddmodified;
		for (pLineTube = (CLDSLineTube*)pView->selectObjs.GetFirst(); pLineTube; pLineTube = (CLDSLineTube*)pView->selectObjs.GetNext())
		{
			if (iRefPart1Or2 == 1)
				pLineTube->desStartOdd.m_hRefPart1 = modifyhdlg.m_nHandle;
			else
				pLineTube->desStartOdd.m_hRefPart2 = modifyhdlg.m_nHandle;
			pLineTube->CalStartOddment();
			pLineTube->SyncMirProp("oddStart");
		}
		//��������ֵ	
		oper.UpdatePropItemValue("oddStart");
	}
	else if (pItem->m_idProp == CLDSLineTube::GetPropID("oddEnd.m_hRefPart1") ||
		pItem->m_idProp == CLDSLineTube::GetPropID("oddEnd.m_hRefPart2"))
	{
		int iRefPart1Or2 = 1;
		if (pItem->m_idProp == CLDSLineTube::GetPropID("oddEnd.m_hRefPart1"))
			iRefPart1Or2 = 1;
		else
			iRefPart1Or2 = 2;
		static CModifyPartHandleDlg modifyhdlg;
		if (iRefPart1Or2 == 1)
			modifyhdlg.m_sHandle.Format("0X%X", pFirstLineTube->desEndOdd.m_hRefPart1);
		else
			modifyhdlg.m_sHandle.Format("0X%X", pFirstLineTube->desEndOdd.m_hRefPart2);
		modifyhdlg.m_sCaption = "�޸Ĳ��չ���";
		modifyhdlg.m_sTitle.Format("���չ���%d:", iRefPart1Or2);
		modifyhdlg.m_bDispSelectBtn = TRUE;//��ʾѡ��ڵ�/�˼��İ�ť
		CString cmdStr;
		cmdStr.Format("��ѡ����չ���%d:", iRefPart1Or2);
		modifyhdlg.SetCmdPickPromptStr(cmdStr);
		modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
		if (modifyhdlg.DoModal() != IDOK)
			return FALSE;
		CLDSPart *pPart = console.FromPartHandle(modifyhdlg.m_nHandle);
		if (pPart == NULL)
		{
			MessageBox(NULL, "�����������Ҳ�����Ӧ������", sSoftName, MB_OK);
			return FALSE;
		}
		CUndoOperObject undo(&Ta);
		CAngleMoveOperObject oddmodified;
		for (pLineTube = (CLDSLineTube*)pView->selectObjs.GetFirst(); pLineTube; pLineTube = (CLDSLineTube*)pView->selectObjs.GetNext())
		{
			if (iRefPart1Or2 == 1)
				pLineTube->desEndOdd.m_hRefPart1 = modifyhdlg.m_nHandle;
			else
				pLineTube->desEndOdd.m_hRefPart2 = modifyhdlg.m_nHandle;
			pLineTube->CalEndOddment();
			pLineTube->SyncMirProp("oddEnd");
		}
		//��������ֵ
		oper.UpdatePropItemValue("oddEnd");
	}
	else
		return FALSE;
	for(pLineTube=(CLDSLineTube*)pView->selectObjs.GetFirst();pLineTube;pLineTube=(CLDSLineTube*)pView->selectObjs.GetNext())
	{
		pLineTube->SetModified();
		pLineTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pLineTube->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

//defined in DesignTubeEndLjDlg.cpp
static void UpdateBushLocationProperty(CPropertyList *pPropList)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineTube *pFirstTube = (CLDSLineTube*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem=NULL, *pGroupItem=NULL, *pRoot=pPropList->GetRootItem();
	CPropertyListOper<CLDSLineTube> oper(pPropList, pFirstTube);
	const int GROUP_GENERAL=1;
	//
	pGroupItem = oper.InsertPropItem(pRoot, "bushTube");
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	//�׹ܸ߶�
	pPropItem = oper.InsertEditPropItem(pGroupItem, "bush.height");
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	//�����ְ���
	pPropItem = oper.InsertButtonPropItem(pGroupItem, "bush.hBushResidePlate");
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	//��λ��˨���
	pPropItem = oper.InsertButtonPropItem(pGroupItem, "bush.hBushRelyBolt");
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
}
void CLDSView::DisplayTubeProperty(BOOL bRebuild)
{
	CLDSLineTube *pFirstTube = (CLDSLineTube*)selectObjs.GetFirst();
	if (pFirstTube == NULL)
		return;
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL, *pRoot=pPropList->GetRootItem();
	CPropTreeItem *pSonPropItem = NULL, *pLeafPropItem = NULL;
	CPropertyListOper<CLDSLineTube> oper(pPropList, pFirstTube);
	const int GROUP_GENERAL=1,GROUP_START=2,GROUP_END=3;
	if(bRebuild)
	{	//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		if (pFirstTube->pStart!=NULL&&pFirstTube->pEnd!=NULL)
		{
			pPropDlg->m_arrPropGroupLabel.SetSize(3);
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"ʼ��");
			pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"�ն�");
			pPropDlg->RefreshTabCtrl(CLDSLineTube::m_iCurDisplayPropGroup);
		}
		else
		{
			pPropDlg->m_arrPropGroupLabel.SetSize(0);
			pPropDlg->RefreshTabCtrl(0);
		}
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_LINETUBE;
		pPropList->SetModifyValueFunc(ModifyLineTubeProperty);
		pPropList->SetButtonClickFunc(LineTubeButtonClick);
		pPropList->SetModifyStatusFunc(ModifyTubeItemStatus);
		pPropList->SetPopMenuClickFunc(FireLineTubePopMenuClick);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstTube->BelongModel()->IsTowerModel()||pFirstTube->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//�ֹܻ�����Ϣ
		pGroupItem = oper.InsertPropItem(pRoot,"basicInfo");
		if(CLDSLineTube::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
		//���
		pPropItem = oper.InsertEditPropItem(pGroupItem, "handle");
		pPropItem->SetReadOnly(pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL) | GetSingleWord(GROUP_START) | GetSingleWord(GROUP_END);
		//ͼ��
		pPropItem = oper.InsertEditPropItem(pGroupItem, "layer");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//����	
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "cMaterial");
		pPropItem->m_lpNodeInfo->m_cmbItems = MakeMaterialMarkSetString();
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�����ȼ�
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "m_cQualityLevel");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���
		pPropItem = oper.InsertCmbEditPropItem(pGroupItem, "specification");
		pPropItem->m_lpNodeInfo->m_cmbItems = MakeTubeSetString();
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�������
		pPropItem = oper.InsertBtnEditPropItem(pGroupItem, "sPartNo");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�ֶκ�
		pPropItem = oper.InsertEditPropItem(pGroupItem, "iSeg");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ĺ�
		pPropItem = oper.InsertButtonPropItem(pGroupItem, "cfgword");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//ͳ��ϵ��
		pPropItem = oper.InsertEditPropItem(pGroupItem, "m_uStatMatCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���Ӹ�����
		pPropItem = oper.InsertBtnEditPropItem(pGroupItem, "m_hPartWeldParent");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(!pFirstTube->IsShadowObject())
		{	//��������
			pPropItem = oper.InsertCmbListPropItem(pGroupItem, "m_hBlock");
			pPropItem->m_lpNodeInfo->m_cmbItems = GetBlockListStr();
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//��ʾ��
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "m_bDisplayHole");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���⸨������
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "m_bVirtualPart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�û�ָ��������ɫ
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstTube->m_bUserSpecColor)
		{	//������ɫ
			pSonPropItem = oper.InsertCmbColorPropItem(pPropItem, "crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		}
        //Ԥ����
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "m_hArcLift");
		pPropItem->m_lpNodeInfo->m_cmbItems= MakeArcLiftSetString();
		if(CLDSLineTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//ʵ�ʳ���
		pPropItem = oper.InsertEditPropItem(pGroupItem, "length");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//
		if(pFirstTube->IsShadowObject())
		{
			//Ӱ����Ϣ
			pPropItem = oper.InsertButtonPropItem(pGroupItem, "shadowInfo");
			if(CLDSLineTube::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//Ӱ��ĸ����
			pSonPropItem = oper.InsertBtnEditPropItem(pPropItem, "m_hMotherObj");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			if (pFirstTube->IsBlockRefShadowObject())
			{
				//������������
				pSonPropItem = oper.InsertBtnEditPropItem(pPropItem, "m_hBlockRef");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			}
		}
		else 
		{
			//��������
			pPropItem = oper.InsertButtonPropItem(pGroupItem, "relativeObjs");
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//������
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "m_bHasWeldRoad");
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(CLDSLineTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstTube->m_bHasWeldRoad)
		{
			//������ָ����ʽ
			oper.InsertCmbListPropItem(pPropItem, "m_iWeldRoadStyle");
			//���׼�߽Ƕ�
			if (pFirstTube->m_iWeldRoadStyle == 1)
				oper.InsertEditPropItem(pPropItem, "weldRoadAngle");
			//��������
			pSonPropItem = oper.InsertButtonPropItem(pPropItem, "nearWeldRoadVec");
			pSonPropItem->m_bHideChildren = FALSE;
			if(CLDSLineTube::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstTube->m_iWeldRoadStyle == 1||pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			//X
			pLeafPropItem = oper.InsertEditPropItem(pSonPropItem, "nearWeldRoadVec.x");
			pLeafPropItem->SetReadOnly(pFirstTube->m_iWeldRoadStyle == 1||pFirstTube->GetObjPropReadOnlyDefaultState(pLeafPropItem->m_idProp));
			//Y
			pLeafPropItem = oper.InsertEditPropItem(pSonPropItem, "nearWeldRoadVec.y");
			pLeafPropItem->SetReadOnly(pFirstTube->m_iWeldRoadStyle == 1||pFirstTube->GetObjPropReadOnlyDefaultState(pLeafPropItem->m_idProp));
			//Z
			pLeafPropItem = oper.InsertEditPropItem(pSonPropItem, "nearWeldRoadVec.z");
			pLeafPropItem->SetReadOnly(pFirstTube->m_iWeldRoadStyle == 1||pFirstTube->GetObjPropReadOnlyDefaultState(pLeafPropItem->m_idProp));
		}
		//��׼�淨��
		pPropItem = oper.InsertButtonPropItem(pGroupItem, "workPlaneNorm");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSLineTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//����ָ����ʽ
		pSonPropItem = oper.InsertCmbListPropItem(pPropItem, "m_iNormCalStyle");
		pSonPropItem->m_bHideChildren = FALSE;
		if(pFirstTube->m_iNormCalStyle==1||pFirstTube->m_iNormCalStyle==2)
		{
			pLeafPropItem = oper.InsertButtonPropItem(pSonPropItem,"pickWorkPlane");
			oper.InsertEditPropItem(pLeafPropItem, "pickWorkPlane.x");
			oper.InsertEditPropItem(pLeafPropItem, "pickWorkPlane.y");
			oper.InsertEditPropItem(pLeafPropItem, "pickWorkPlane.z");
			if (pFirstTube->m_iNormCalStyle == 1)
				oper.InsertBtnEditPropItem(pSonPropItem, "hWorkNormRefPart");
			else if(pFirstTube->m_iNormCalStyle == 2)
				UpdatePickWorkPlaneItemsPropStr(pPropList,2);
		}
		//X
		pSonPropItem = oper.InsertButtonPropItem(pPropItem, "workPlaneNorm.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->SetReadOnly(pFirstTube->m_iNormCalStyle==1||pFirstTube->m_iNormCalStyle==2
			                     ||pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//Y
		pSonPropItem = oper.InsertButtonPropItem(pPropItem, "workPlaneNorm.y");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->SetReadOnly(pFirstTube->m_iNormCalStyle==1||pFirstTube->m_iNormCalStyle==2
			                    ||pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//Z
		pSonPropItem = oper.InsertButtonPropItem(pPropItem, "workPlaneNorm.z");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->SetReadOnly(pFirstTube->m_iNormCalStyle==1||pFirstTube->m_iNormCalStyle==2
			                  ||pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//�ֹ��и���
		pPropItem = oper.InsertEditPropItem(pGroupItem, "cutPlane");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSLineTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��������
		pSonPropItem = oper.InsertButtonPropItem(pPropItem, "cutPlaneOrigin");
		pSonPropItem->m_bHideChildren = TRUE;
		if(CLDSLineTube::GetPropStatus(pSonPropItem->m_idProp)!=-1)
			pSonPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pSonPropItem->m_idProp);
		//X
		oper.InsertEditPropItem(pSonPropItem, "cutPlaneOrigin.x");
		//Y
		oper.InsertEditPropItem(pSonPropItem, "cutPlaneOrigin.y");
		//Z
		oper.InsertEditPropItem(pSonPropItem, "cutPlaneOrigin.z");
		//���߷���
		pSonPropItem = oper.InsertButtonPropItem(pPropItem, "cutPlaneNorm");
		pSonPropItem->m_bHideChildren = TRUE;
		if(CLDSLineTube::GetPropStatus(pSonPropItem->m_idProp)!=-1)
			pSonPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pSonPropItem->m_idProp);
		//X
		oper.InsertEditPropItem(pSonPropItem, "cutPlaneNorm.x");
		//Y
		oper.InsertEditPropItem(pSonPropItem, "cutPlaneNorm.y");
		//Z
		oper.InsertEditPropItem(pSonPropItem, "cutPlaneNorm.z");
		//��ע
		pPropItem = oper.InsertEditPropItem(pGroupItem, "sNotes");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//ʼ����Ϣ
		pGroupItem = oper.InsertPropItem(pRoot, "startInfo");
		if(CLDSLineTube::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//ʼ������
		pPropItem = oper.InsertButtonPropItem(pGroupItem, "startPos");
		if(CLDSLineTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pPropItem->m_idProp);
		//X
		pSonPropItem = oper.InsertEditPropItem(pPropItem, "startPos.x");
		if(pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart)
			pSonPropItem->SetReadOnly(TRUE);
		else //endPosType==4 ָ������
			pSonPropItem->SetReadOnly(pFirstTube->desStartPos.endPosType!=4||pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//Y
		pSonPropItem = oper.InsertEditPropItem(pPropItem, "startPos.y");
		if (pFirstTube->IsStartPosLocked() || pFirstTube->bHuoQuStart)
			pSonPropItem->SetReadOnly(TRUE);
		else //endPosType==4 ָ������
			pSonPropItem->SetReadOnly(pFirstTube->desStartPos.endPosType != 4 || pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_START);
		//Z
		pSonPropItem = oper.InsertEditPropItem(pPropItem, "startPos.z");
		if (pFirstTube->IsStartPosLocked() || pFirstTube->bHuoQuStart)
			pSonPropItem->SetReadOnly(TRUE);
		else //endPosType==4 ָ������
			pSonPropItem->SetReadOnly(pFirstTube->desStartPos.endPosType != 4 || pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_START);
		//ʼ��ʵ������
		oper.InsertButtonPropItem(pGroupItem, "actualStartPos");
		//�����
		pPropItem = oper.InsertButtonPropItem(pGroupItem, "hStart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//����ͷ
		pPropItem = oper.InsertBtnEditPropItem(pGroupItem, "oddStart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//ʼ������ͷ�ļ��㷽ʽ
		pSonPropItem = oper.InsertCmbListPropItem(pPropItem, "oddStart.m_iOddCalStyle");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs, pSonPropItem->m_idProp);
		if (CLDSLineTube::GetPropStatus(pSonPropItem->m_idProp) != -1)
			pSonPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pSonPropItem->m_idProp);
		//��ײ��϶
		oper.InsertEditPropItem(pSonPropItem, "oddStart.m_fCollideDist");
		//���ո˼�1
		oper.InsertBtnEditPropItem(pSonPropItem, "oddStart.m_hRefPart1");
		//���ո˼�2
		oper.InsertBtnEditPropItem(pSonPropItem, "oddStart.m_hRefPart2");
		//ʼ������
		pPropItem = oper.InsertButtonPropItem(pGroupItem, "jointStart");
		if(pFirstTube->m_tJointStart.hLinkObj<0X20&&!pFirstTube->m_tJointStart.IsValidPlaneCut())
		{	//���ǺϷ�������
			pPropItem->m_lpNodeInfo->m_buttonType = BDT_POPMENUEDIT;
			pPropItem->m_lpNodeInfo->m_cmbItems="ƽ���и�|�ֹ����|����|U�Ͳ��|���Ͳ��|ʮ�ֲ��|";
		}	
		//�˵�λ��
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "desStartPos.endPosType");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(CLDSLineTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pPropItem,TRUE);	
		//�ն���Ϣ
		pGroupItem = oper.InsertPropItem(pRoot, "endInfo");
		if(CLDSLineTube::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//�ն�����
		pPropItem = oper.InsertButtonPropItem(pGroupItem, "endPos");
		if(CLDSLineTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pPropItem->m_idProp);
		//X
		pSonPropItem = oper.InsertEditPropItem(pPropItem, "endPos.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd)
			pSonPropItem->SetReadOnly(TRUE);
		else //endPosType==4 ָ������
			pSonPropItem->SetReadOnly(pFirstTube->desEndPos.endPosType!=4||pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//Y
		pSonPropItem = oper.InsertEditPropItem(pPropItem, "endPos.y");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs, pSonPropItem->m_idProp);
		if (pFirstTube->IsEndPosLocked() || pFirstTube->bHuoQuEnd)
			pSonPropItem->SetReadOnly(TRUE);
		else //endPosType==4 ָ������
			pSonPropItem->SetReadOnly(pFirstTube->desEndPos.endPosType != 4 || pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//Z
		pSonPropItem = oper.InsertEditPropItem(pPropItem, "endPos.z");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs, pSonPropItem->m_idProp);
		if (pFirstTube->IsEndPosLocked() || pFirstTube->bHuoQuEnd)
			pSonPropItem->SetReadOnly(TRUE);
		else //endPosType==4 ָ������
			pSonPropItem->SetReadOnly(pFirstTube->desEndPos.endPosType != 4 || pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//�ն�ʵ������
		oper.InsertButtonPropItem(pGroupItem, "actualEndPos");
		//�յ���
		pPropItem = oper.InsertButtonPropItem(pGroupItem, "hEnd");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//����ͷ
		pPropItem = oper.InsertBtnEditPropItem(pGroupItem, "oddEnd");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//ʼ������ͷ�ļ��㷽ʽ
		pSonPropItem = oper.InsertCmbListPropItem(pPropItem, "oddEnd.m_iOddCalStyle");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs, pSonPropItem->m_idProp);
		if (CLDSLineTube::GetPropStatus(pSonPropItem->m_idProp) != -1)
			pSonPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pSonPropItem->m_idProp);
		//��ײ��϶
		oper.InsertEditPropItem(pSonPropItem, "oddEnd.m_fCollideDist");
		//���ո˼�1
		oper.InsertBtnEditPropItem(pSonPropItem, "oddEnd.m_hRefPart1");
		//���ո˼�2
		oper.InsertBtnEditPropItem(pSonPropItem, "oddEnd.m_hRefPart2");
		//�ն�����
		pPropItem = oper.InsertButtonPropItem(pGroupItem, "jointEnd");
		if(pFirstTube->m_tJointEnd.hLinkObj<0X20&&pFirstTube->m_tJointEnd.IsValidPlaneCut())
		{
			pPropItem->m_lpNodeInfo->m_buttonType = BDT_POPMENUEDIT;
			pPropItem->m_lpNodeInfo->m_cmbItems="ƽ���и�|�ֹ����|����|U�Ͳ��|���Ͳ��|ʮ�ֲ��|";
		}
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		//�˵�λ��
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "desEndPos.endPosType");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(CLDSLineTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pPropItem,FALSE);
		//////////////////////////////////////////////////////////////////////////
		//�׹ܵĶ�λ����----CLDSLineTube::m_cFuncTypeδ���д洢
		if(pFirstTube->pStart==NULL||pFirstTube->pEnd==NULL)
			UpdateBushLocationProperty(pPropList);
	}
	else
	{//����Ҫ�ؽ���������ҳ
		CPropTreeItem *pFindItem, *pParentItem = NULL;
		SetPartMutiObjsProp(pFirstTube,"handle");
		SetPartMutiObjsProp(pFirstTube,"layer");
		SetPartMutiObjsProp(pFirstTube,"cMaterial");
		SetPartMutiObjsProp(pFirstTube,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstTube,"specification");
		SetPartMutiObjsProp(pFirstTube,"iSeg");
		SetPartMutiObjsProp(pFirstTube,"sPartNo");
		SetPartMutiObjsProp(pFirstTube,"cfgword");
		SetPartMutiObjsProp(pFirstTube,"m_hBlock");
		SetPartMutiObjsProp(pFirstTube,"m_hBlockRef");
		pFindItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("relativeObjs"),&pParentItem);
		if (pFindItem)
		{
			if (pFirstTube->IsShadowObject())
			{	//����ͨ�����л���Ӱ�乹��
				pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("m_hBlock"));
				pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("relativeObjs"));
				//Ӱ����Ϣ
				pPropItem = oper.InsertButtonPropItem(pParentItem, "shadowInfo", "", "", 14, TRUE);
				if(CLDSLineTube::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pPropItem->m_idProp);
				//Ӱ��ĸ����
				pSonPropItem = oper.InsertBtnEditPropItem(pPropItem, "m_hMotherObj", "", "", -1, TRUE);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				if (pFirstTube->IsBlockRefShadowObject())
				{
					//������������
					pSonPropItem = oper.InsertBtnEditPropItem(pPropItem, "m_hBlockRef", "", "", -1, TRUE);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
			else
				SetPartMutiObjsProp(pFirstTube,"relativeObjs");
		}
		pFindItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("shadowInfo"),&pParentItem);
		if (pFindItem)
		{
			if (!pFirstTube->IsShadowObject())
			{	//��Ӱ�乹���л�����ͨ����
				pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("shadowInfo"));
				pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("m_hMotherObj"));
				pFindItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("m_hBlockRef"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("m_hBlockRef"));
				oper.InsertButtonPropItem(pParentItem, "relativeObjs", "", "", 14, TRUE);
			}
			else
			{
				SetPartMutiObjsProp(pFirstTube,"m_hMotherObj");
				pFindItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("m_hBlockRef"),&pParentItem);
				if (pFindItem&&pFirstTube->IsBlockRefShadowObject())
				{
					SetPartMutiObjsProp(pFirstTube,"m_hBlockRef");
					SetPartMutiObjsProp(pFirstTube,"m_hBlock");
				}
				else if (pFindItem&&!pFirstTube->IsBlockRefShadowObject())
				{
					pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("m_hBlockRef"));
					pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("m_hBlock"));
				}
				else if(pFindItem==NULL&&pFirstTube->IsBlockRefShadowObject())
				{
					pPropList->FindItemByPropId(CLDSLineTube::GetPropID("m_hMotherObj"),&pParentItem);
					//������������
					pSonPropItem = oper.InsertBtnEditPropItem(pParentItem, "m_hBlockRef", "", "", -1, TRUE);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstTube->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
		}
		SetPartMutiObjsProp(pFirstTube,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstTube,"m_hPartWeldParent");
		SetPartMutiObjsProp(pFirstTube,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstTube,"m_bVirtualPart");
		SetPartMutiObjsProp(pFirstTube,"m_bUserSpecColor");
		if(pFirstTube->m_bUserSpecColor)
		{	
			if(!SetPartMutiObjsProp(pFirstTube,"crMaterial"))
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("m_bUserSpecColor"),NULL);
				if(pFindItem)
					oper.InsertCmbColorPropItem(pFindItem, "crMaterial", "", "", "", -1, TRUE);
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstTube,"m_hArcLift");
		SetPartMutiObjsProp(pFirstTube,"length");
		SetPartMutiObjsProp(pFirstTube,"sNotes");
		SetPartMutiObjsProp(pFirstTube,"m_bHasWeldRoad");
		SetPartMutiObjsProp(pFirstTube,"m_hPartWeldParent");
		long idProp;
		if(pFirstTube->m_bHasWeldRoad)
		{
			//�����߶�λ��ʽ
			idProp = CLDSLineTube::GetPropID("m_iWeldRoadStyle");
			if(!SetPartMutiObjsProp(pFirstTube,"m_iWeldRoadStyle"))
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("m_iWeldRoadStyle"),&pParentItem);
				if(pFindItem)
					oper.InsertCmbListPropItem(pFindItem, "m_iWeldRoadStyle","","","",-1,TRUE);
			}
			//�����Ƕ�
			idProp = CLDSLineTube::GetPropID("weldRoadAngle");
			if(!SetPartMutiObjsProp(pFirstTube,"weldRoadAngle"))
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("m_bHasWeldRoad"),&pParentItem);
				if (pFindItem)
					oper.InsertEditPropItem(pFindItem, "weldRoadAngle", "", "", -1, TRUE);
			}
			//��������
			idProp = CLDSLineTube::GetPropID("nearWeldRoadVec");
			if(!SetPartMutiObjsProp(pFirstTube,"nearWeldRoadVec"))
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("m_bHasWeldRoad"),&pParentItem);
				if(pFindItem)
				{
					pPropItem = oper.InsertButtonPropItem(pFindItem, "nearWeldRoadVec", "", "", -1, TRUE);
					pPropItem->m_bHideChildren = TRUE;
					if(CLDSLineTube::GetPropStatus(pPropItem->m_idProp)!=-1)
						pPropItem->m_bHideChildren = CLDSLineTube::GetPropStatus(pPropItem->m_idProp);
					pPropItem->SetReadOnly(pFirstTube->m_iWeldRoadStyle == 1||pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
			}
			//��������X����
			idProp = CLDSLineTube::GetPropID("nearWeldRoadVec.x");
			if(!SetPartMutiObjsProp(pFirstTube,"nearWeldRoadVec.x"))
			{
				//��סSHIFT��ѡ��һ������ʱ��ˢ����������������һ��Ϊ����ˢ�£��ڶ���Ϊ����ˢ��
				//�ڽ��еڶ���ˢ��ʱ������Ϊ������ʾ�����Ҳ���ĳЩ����
				pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec"), &pParentItem);
				if(pFindItem)
				{
					pPropItem = oper.InsertEditPropItem(pFindItem, "nearWeldRoadVec.x", "", "", -1, TRUE);
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,idProp);
					pPropItem->SetReadOnly(pFirstTube->m_iWeldRoadStyle == 1||pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
			}
			//��������Y����
			idProp = CLDSLineTube::GetPropID("nearWeldRoadVec.y");
			if(!SetPartMutiObjsProp(pFirstTube,"nearWeldRoadVec.y"))
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec"),&pParentItem);
				if(pFindItem)
				{
					pPropItem = oper.InsertEditPropItem(pFindItem, "nearWeldRoadVec.y", "", "", -1, TRUE);
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,idProp);
					pPropItem->SetReadOnly(pFirstTube->m_iWeldRoadStyle == 1||pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
			}
			//��������Z����
			idProp = CLDSLineTube::GetPropID("nearWeldRoadVec.z");
			if(!SetPartMutiObjsProp(pFirstTube,"nearWeldRoadVec.z"))
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec"),&pParentItem);
				if(pFindItem)
				{
					pPropItem = oper.InsertEditPropItem(pFindItem, "nearWeldRoadVec.z");
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,idProp);
					pPropItem->SetReadOnly(pFirstTube->m_iWeldRoadStyle == 1||pFirstTube->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
			}
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("weldRoadAngle"));
			pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("m_iWeldRoadStyle"));
			pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec"));
			pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec.x"));
			pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec.y"));
			pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("nearWeldRoadVec.z"));
		}
		//SetPartMutiObjsProp(pFirstTube,"nearWeldRoadVec.x");
		//SetPartMutiObjsProp(pFirstTube,"nearWeldRoadVec.y");
		//SetPartMutiObjsProp(pFirstTube,"nearWeldRoadVec.z");
		SetPartMutiObjsProp(pFirstTube,"m_iNormCalStyle");
		pFindItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("m_iNormCalStyle"),NULL);
		if(pFirstTube->m_iNormCalStyle==1||pFirstTube->m_iNormCalStyle==2)
		{
			pPropItem = oper.InsertButtonPropItem(pFindItem, "pickWorkPlane", "", "", 2, TRUE);
			oper.InsertEditPropItem(pPropItem, "pickWorkPlane.x", "", "", -1, TRUE);
			oper.InsertEditPropItem(pPropItem, "pickWorkPlane.y", "", "", -1, TRUE);
			oper.InsertEditPropItem(pPropItem, "pickWorkPlane.z", "", "", -1, TRUE);
			SetPartMutiObjsProp(pFirstTube,"pickWorkPlane.x");
			SetPartMutiObjsProp(pFirstTube,"pickWorkPlane.y");
			SetPartMutiObjsProp(pFirstTube,"pickWorkPlane.z");
			if(pFirstTube->m_iNormCalStyle==1)
			{
				oper.InsertBtnEditPropItem(pFindItem, "hWorkNormRefPart", "", "", 1, TRUE);
				SetPartMutiObjsProp(pFirstTube,"hWorkNormRefPart");
			}
			else
				pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("hWorkNormRefPart"));
			UpdatePickWorkPlaneItemsPropStr(pPropList,pFirstTube->m_iNormCalStyle);
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("pickWorkPlane"));
			pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("pickWorkPlane.x"));
			pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("pickWorkPlane.y"));
			pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("pickWorkPlane.z"));
			pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("hWorkNormRefPart"));
		}
		SetPartMutiObjsProp(pFirstTube,"workPlaneNorm.x");
		SetPartMutiObjsProp(pFirstTube,"workPlaneNorm.y");
		SetPartMutiObjsProp(pFirstTube,"workPlaneNorm.z");
		SetPropItemReadOnly(pFirstTube,"workPlaneNorm.x",pFirstTube->m_iNormCalStyle==1||pFirstTube->m_iNormCalStyle==2||
							pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("workPlaneNorm.x")));
		SetPropItemReadOnly(pFirstTube,"workPlaneNorm.y",pFirstTube->m_iNormCalStyle==1||pFirstTube->m_iNormCalStyle==2||
							pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("workPlaneNorm.y")));
		SetPropItemReadOnly(pFirstTube,"workPlaneNorm.z",pFirstTube->m_iNormCalStyle==1||pFirstTube->m_iNormCalStyle==2||
							pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("workPlaneNorm.z")));
		SetPartMutiObjsProp(pFirstTube,"cutPlaneOrigin.x");
		SetPartMutiObjsProp(pFirstTube,"cutPlaneOrigin.y");
		SetPartMutiObjsProp(pFirstTube,"cutPlaneOrigin.z");
		SetPartMutiObjsProp(pFirstTube,"cutPlaneNorm.x");
		SetPartMutiObjsProp(pFirstTube,"cutPlaneNorm.y");
		SetPartMutiObjsProp(pFirstTube,"cutPlaneNorm.z");
		SetPartMutiObjsProp(pFirstTube,"hStart");
		SetPartMutiObjsProp(pFirstTube,"oddStart");
		SetPartMutiObjsProp(pFirstTube,"jointStart");
		if((pFindItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("jointStart"),&pParentItem))!=NULL)
		{
			pFindItem->m_lpNodeInfo->m_controlType = PIT_BUTTON;
			if(pFirstTube->m_tJointStart.hLinkObj<0X20&&!pFirstTube->m_tJointStart.IsValidPlaneCut())
			{
				pFindItem->m_lpNodeInfo->m_buttonType = BDT_POPMENUEDIT;
				pFindItem->m_lpNodeInfo->m_cmbItems="ƽ���и�|�ֹ����|����|U�Ͳ��|���Ͳ��|ʮ�ֲ��|";
			}
			else
				pFindItem->m_lpNodeInfo->m_buttonType = BDT_COMMON;
		}
		SetPartMutiObjsProp(pFirstTube,"startPos.x");
		SetPartMutiObjsProp(pFirstTube,"startPos.y");
		SetPartMutiObjsProp(pFirstTube,"startPos.z");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.endPosType");
		SetPropItemReadOnly(pFirstTube,"hStart",pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
							pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("hStart")));
		SetPropItemReadOnly(pFirstTube,"oddStart",pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
							pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("oddStart")));
		SetPropItemReadOnly(pFirstTube,"desStartPos.endPosType",pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart||
							pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("desStartPos.endPosType")));
		SetPartMutiObjsProp(pFirstTube,"desStartPos.datum_pos_style");
		pFindItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("desStartPos.endPosType"),&pParentItem);
		if(pFindItem)
			UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pFindItem,TRUE);
		SetPartMutiObjsProp(pFirstTube,"desStartPos.hDatumPart");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.fNormOffset");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.fEccentricDist");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.fLenVecOffsetDist");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.face_offset_norm");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.bDatumEndStartEnd");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.bDatumStartStartEnd");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.hDatumStartPart");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.hDatumEndPart");
		//�ն���Ϣ
		SetPartMutiObjsProp(pFirstTube,"hEnd");
		SetPartMutiObjsProp(pFirstTube,"oddEnd");	//����ͷ
		SetPartMutiObjsProp(pFirstTube,"jointEnd");	//�ն�����
		if((pFindItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("jointEnd"),&pParentItem))!=NULL)
		{
			pFindItem->m_lpNodeInfo->m_controlType = PIT_BUTTON;
			if(pFirstTube->m_tJointEnd.hLinkObj<0X20&&!pFirstTube->m_tJointEnd.IsValidPlaneCut())
			{
				pFindItem->m_lpNodeInfo->m_buttonType = BDT_POPMENUEDIT;
				pFindItem->m_lpNodeInfo->m_cmbItems="ƽ���и�|�ֹ����|����|U�Ͳ��|���Ͳ��|ʮ�ֲ��|";
			}
			else
				pFindItem->m_lpNodeInfo->m_buttonType = BDT_COMMON;
		}
		SetPartMutiObjsProp(pFirstTube,"endPos.x");	//�ն�����--X
		SetPartMutiObjsProp(pFirstTube,"endPos.y");	//�ն�����--Y
		SetPartMutiObjsProp(pFirstTube,"endPos.z");	//�ն�����--Z
		SetPartMutiObjsProp(pFirstTube,"desEndPos.endPosType");	//�˻���λ��
		SetPropItemReadOnly(pFirstTube,"hEnd",pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
							pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("hEnd")));
		SetPropItemReadOnly(pFirstTube,"oddEnd",pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
							pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("oddEnd")));
		SetPropItemReadOnly(pFirstTube,"desEndPos.endPosType",pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd||
							pFirstTube->GetObjPropReadOnlyDefaultState(CLDSLineTube::GetPropID("desEndPos.endPosType")));
		SetPartMutiObjsProp(pFirstTube,"desEndPos.datum_pos_style");
		pFindItem=pPropList->FindItemByPropId(CLDSLineTube::GetPropID("desEndPos.endPosType"),&pParentItem);
		if(pFindItem)
			UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pFindItem,FALSE);
		SetPartMutiObjsProp(pFirstTube,"desEndPos.hDatumPart");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.fNormOffset");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.fEccentricDist");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.fLenVecOffsetDist");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.face_offset_norm");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.bDatumEndStartEnd");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.bDatumStartStartEnd");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.hDatumStartPart");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.hDatumEndPart");
		//
		pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("bushTube"));
		pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("bush.hBushResidePlate"));
		pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("bush.height"));
		pPropList->DeleteItemByPropId(CLDSLineTube::GetPropID("bush.hBushRelyBolt"));
		if(pFirstTube->pStart==NULL||pFirstTube->pEnd==NULL)
			UpdateBushLocationProperty(pPropList);
	}
	pPropList->Redraw();
}

#ifdef __GGG_
BOOL ModifyConeTubeItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSConeTube::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
BOOL ModifySlotArmItemStatus(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSSlotArm::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
BOOL ConeTubeButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CXhChar16 sSoftName("GGG");
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem, *pParentItem;
	CLDSConeTube *pConeTube=NULL, *pFirstConeTube=NULL;
	CReportVectorDlg vecDlg;
	CReportPointDlg pointDlg;
	CCfgPartNoDlg cfgdlg;
	COddmentDesignDlg odddlg;
	char sText[MAX_TEMP_CHAR_100+1];
	CString valueStr = _T("");

	if(pView->selectObjs.GetNodeNum()>0)
		pFirstConeTube = (CLDSConeTube*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	//	dlg.m_pLineTube = pFirstConeTube;
	if(pItem->m_idProp==CLDSConeTube::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstConeTube->cfgword; 
		if(!pFirstConeTube->IsLegObj())
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if(cfgdlg.DoModal()==IDOK)
		{
			for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			{
				pConeTube->cfgword = cfgdlg.cfgword;		//�ڴ˴����µ�һ���ڵ������Ϻ�
				pConeTube->SetModified(TRUE,FALSE);
			}
		}
		if(pFirstConeTube->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("sPartNo"))
	{
		CModifyRelativeObjPartNoDlg modify_partno_dlg;
		modify_partno_dlg.m_pWorkPart=pFirstConeTube;
		if(modify_partno_dlg.DoModal()==IDOK)
		{
			pFirstConeTube->SetPartNo(modify_partno_dlg.m_sCurObjPartNo.GetBuffer(1));
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
			SEGI iSeg;
			if(CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(modify_partno_dlg.m_sCurObjPartNo.GetString(),&iSeg,NULL))
			{
				//pView->selectObjs�����๹����ŵĶκ�ͬ����������CModifyRelativeObjPartNoDlg����� wjh-2016.6.14
				pFirstConeTube->iSeg=iSeg;
				pPropList->SetItemPropValue(CLDSConeTube::GetPropID("iSeg"),iSeg.ToString());
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("nearWeldRoadVec"))
	{
		vecDlg.m_sCaption = "�ֹܺ��������ڷ���";
		vecDlg.m_fVectorX = pFirstConeTube->nearWeldRoadVec.x;
		vecDlg.m_fVectorY = pFirstConeTube->nearWeldRoadVec.y;
		vecDlg.m_fVectorZ = pFirstConeTube->nearWeldRoadVec.z;
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("nearWeldRoadVec.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("nearWeldRoadVec.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("nearWeldRoadVec.z"),CString(sText));
			for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				pConeTube->nearWeldRoadVec.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("m_hPartWeldParent"))
	{	//ͨ��ѡ�񹹼��޸ĺ��Ӹ����������ڽ������޸ĵ��������ĺ��Ӹ�����
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸ĸֹܺ��Ӹ�����";
		modifyhdlg.m_sTitle="���Ӹ�����:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstConeTube->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ�񺸽Ӹ�����:");
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSConeTube::GetPropID("m_hPartWeldParent"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSLinePart* pMirLinePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLinePart=(CLDSLinePart*)pConeTube->GetMirPart(msg);
						if(pMirLinePart)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//������ѡ�������ĶԳƹ��� wht 11-05-07
								pMirLinePart->m_hPartWeldParent=pMirPart->handle;
						}
					}
					pConeTube->m_hPartWeldParent=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pConeTube->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("hWorkNormRefPart"))
	{
		//ͨ��ѡ�񹹼��޸ķ��߲��ո˼�
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸ĸֹܷ��߲��չ���";
		modifyhdlg.m_sTitle="���߲��ո˼�:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstConeTube->hWorkNormRefPart);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ���߲��չ���:");
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSConeTube::GetPropID("hWorkNormRefPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSConeTube* pMirLineTubePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineTubePart=(CLDSConeTube*)pConeTube->GetMirPart(msg);
						if(pMirLineTubePart)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if(pMirPart)	//������ѡ���߲��ո˼��ĶԳƹ��� wht 11-05-07
								pMirLineTubePart->hWorkNormRefPart=pMirPart->handle;
						}
					}
					pConeTube->hWorkNormRefPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pConeTube->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumPart")||pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.hDatumPart"))
	{
		//�޸Ĵ��λ�û�׼�ֹ�
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸Ĵ��λ�û�׼�ֹ�";
		modifyhdlg.m_sTitle="��׼�ֹ�:";
		if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumPart"))
			modifyhdlg.m_sHandle.Format("0X%X",pFirstConeTube->desStartPos.hDatumPart);
		else
			modifyhdlg.m_sHandle.Format("0X%X",pFirstConeTube->desEndPos.hDatumPart);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ���ӻ�׼�ֹ�:");
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				//������ʾ
				if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumPart"))
					pPropList->SetItemPropValue(CLDSConeTube::GetPropID("desStartPos.hDatumPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSConeTube::GetPropID("desEndPos.hDatumPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSConeTube* pMirLineTubePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineTubePart=(CLDSConeTube*)pConeTube->GetMirPart(msg);
						if(pMirLineTubePart)
						{
							if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumPart"))
								pMirLineTubePart->desStartPos.hDatumPart=pPart->handle;
							else
								pMirLineTubePart->desEndPos.hDatumPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumPart"))
						pFirstConeTube->desStartPos.hDatumPart=pPart->handle;
					else
						pFirstConeTube->desEndPos.hDatumPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pConeTube->handle);
				}
				if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumPart"))
				{
					pParentItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("desStartPos.endPosType"),NULL);
					if(pParentItem)
						UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
				}
				else
				{
					pParentItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("desEndPos.endPosType"),NULL);
					if(pParentItem)
						UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumStartPart")||pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.hDatumStartPart"))
	{
		//�޸�ͶӰ��ʼ�����ڻ�׼�ֹ�
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸�ͶӰʼ�����ڻ�׼�ֹ�";
		modifyhdlg.m_sTitle="��׼�ֹ�:";
		if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumStartPart"))
			modifyhdlg.m_sHandle.Format("0X%X",pFirstConeTube->desStartPos.hDatumStartPart);
		else
			modifyhdlg.m_sHandle.Format("0X%X",pFirstConeTube->desEndPos.hDatumStartPart);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ��ͶӰ��׼�ֹ�:");
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				//������ʾ
				if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumStartPart"))
					pPropList->SetItemPropValue(CLDSConeTube::GetPropID("desStartPos.hDatumStartPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSConeTube::GetPropID("desEndPos.hDatumStartPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSConeTube* pMirLineTubePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineTubePart=(CLDSConeTube*)pConeTube->GetMirPart(msg);
						if(pMirLineTubePart)
						{
							if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumStartPart"))
								pMirLineTubePart->desStartPos.hDatumStartPart=pPart->handle;
							else
								pMirLineTubePart->desEndPos.hDatumStartPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumStartPart"))
						pFirstConeTube->desStartPos.hDatumStartPart=pPart->handle;
					else
						pFirstConeTube->desEndPos.hDatumStartPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pConeTube->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumEndPart")||pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.hDatumEndPart"))
	{
		//�޸�ͶӰ���ն����ڻ�׼�ֹ�
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸�ͶӰ�ն����ڻ�׼�ֹ�";
		modifyhdlg.m_sTitle="��׼�ֹ�:";
		if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumEndPart"))
			modifyhdlg.m_sHandle.Format("0X%X",pFirstConeTube->desStartPos.hDatumEndPart);
		else
			modifyhdlg.m_sHandle.Format("0X%X",pFirstConeTube->desEndPos.hDatumEndPart);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ��ͶӰ��׼�ֹ�:");
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				//������ʾ
				if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumEndPart"))
					pPropList->SetItemPropValue(CLDSConeTube::GetPropID("desStartPos.hDatumEndPart"),valueStr);
				else
					pPropList->SetItemPropValue(CLDSConeTube::GetPropID("desEndPos.hDatumEndPart"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={1,2,4};
					CLDSConeTube* pMirLineTubePart=NULL;
					for(int i=0;i<3;i++)
					{
						if((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirLineTubePart=(CLDSConeTube*)pConeTube->GetMirPart(msg);
						if(pMirLineTubePart)
						{
							if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumEndPart"))
								pMirLineTubePart->desStartPos.hDatumEndPart=pPart->handle;
							else
								pMirLineTubePart->desEndPos.hDatumEndPart=pPart->handle;
						}
					}
					if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumEndPart"))
						pFirstConeTube->desStartPos.hDatumEndPart=pPart->handle;
					else
						pFirstConeTube->desEndPos.hDatumEndPart=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pConeTube->handle);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("nearWorkPlaneNorm"))
	{
		vecDlg.m_sCaption = "�ֹܻ�׼����Ʒ�������";
		vecDlg.m_fVectorX = pFirstConeTube->nearWorkPlaneNorm.x;
		vecDlg.m_fVectorY = pFirstConeTube->nearWorkPlaneNorm.y;
		vecDlg.m_fVectorZ = pFirstConeTube->nearWorkPlaneNorm.z;
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWorkPlaneNorm.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWorkPlaneNorm.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWorkPlaneNorm.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("nearWorkPlaneNorm.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("nearWorkPlaneNorm.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("nearWorkPlaneNorm.z"),CString(sText));
			for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			{
				pConeTube->nearWorkPlaneNorm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				pConeTube->CalWorkPlaneNorm();	//���¼���ֹܷ��߷���
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("workPlaneNorm")
		||pItem->m_idProp==CLDSConeTube::GetPropID("workPlaneNorm.x")
		||pItem->m_idProp==CLDSConeTube::GetPropID("workPlaneNorm.y")
		||pItem->m_idProp==CLDSConeTube::GetPropID("workPlaneNorm.z"))
	{
		vecDlg.m_sCaption = "�ֹܻ�׼�淨������";
		vecDlg.m_fVectorX = pFirstConeTube->WorkPlaneNorm().x;
		vecDlg.m_fVectorY = pFirstConeTube->WorkPlaneNorm().y;
		vecDlg.m_fVectorZ = pFirstConeTube->WorkPlaneNorm().z;
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("workPlaneNorm.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("workPlaneNorm.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("workPlaneNorm.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("workPlaneNorm.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("workPlaneNorm.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("workPlaneNorm.z"),CString(sText));
			for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				pConeTube->SetWorkPlaneNorm(f3dPoint(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ));
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("cutPlaneOrigin"))
	{
		pointDlg.m_sCaption = "�ֹ��и����������";
		pointDlg.m_fPointX = pFirstConeTube->cutPlaneOrigin.x;
		pointDlg.m_fPointY = pFirstConeTube->cutPlaneOrigin.y;
		pointDlg.m_fPointZ = pFirstConeTube->cutPlaneOrigin.z;
		if(pointDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("cutPlaneOrigin.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("cutPlaneOrigin.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("cutPlaneOrigin.z"),CString(sText));
			for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				pConeTube->cutPlaneOrigin.Set(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ);
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("cutPlaneNorm"))
	{
		vecDlg.m_sCaption = "�ֹ��и��淨������";
		vecDlg.m_fVectorX = pFirstConeTube->cutPlaneNorm.x;
		vecDlg.m_fVectorY = pFirstConeTube->cutPlaneNorm.y;
		vecDlg.m_fVectorZ = pFirstConeTube->cutPlaneNorm.z;
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("cutPlaneNorm.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("cutPlaneNorm.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("cutPlaneNorm.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if(vecDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("cutPlaneNorm.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("cutPlaneNorm.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("cutPlaneNorm.z"),CString(sText));
			for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				pConeTube->cutPlaneNorm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("hStart"))
	{
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸ĸ˼�ʼ�˽ڵ�";
		modifyhdlg.m_sTitle="ʼ�˽ڵ�:";
		if(pFirstConeTube->pStart)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstConeTube->pStart->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�ڵ�:");
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(modifyhdlg.DoModal()==IDOK)
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
			else
			{
				CUndoOperObject undo(&Ta);
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSConeTube::GetPropID("hStart"),valueStr);
				for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				{
					CLDSNode* pOldNode=pConeTube->pStart;
					pConeTube->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),pNode,NULL);
					pConeTube->pStart=pNode;
					pConeTube->SetModified();
					pConeTube->ClearFlag();
					pConeTube->CalPosition();
					//���¸˼��ϵĶ˽ڵ������Ϣ
					pConeTube->UpdateRodEndNodeRelation(pOldNode,pNode);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("hEnd"))
	{
		CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸ĸ˼��ն˽ڵ�";
		modifyhdlg.m_sTitle="�ն˽ڵ�:";
		if(pFirstConeTube->pEnd)
			modifyhdlg.m_sHandle.Format("0X%X",pFirstConeTube->pEnd->handle);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�ڵ�:");
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(IDOK==modifyhdlg.DoModal())
		{
			CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
			if(pNode==NULL)
				MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
			else
			{
				CUndoOperObject undo(&Ta);
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSConeTube::GetPropID("hEnd"),valueStr);
				for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				{
					CLDSNode* pOldNode=pConeTube->pEnd;
					pConeTube->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),NULL,pNode);
					pConeTube->pEnd=pNode;
					pConeTube->SetModified();
					pConeTube->ClearFlag();
					pConeTube->CalPosition();
					//���¸˼��ϵĶ˽ڵ������Ϣ
					pConeTube->UpdateRodEndNodeRelation(pOldNode,pNode);
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("oddStart"))
	{
		odddlg.m_hRefPart1.Format("0X%X",pFirstConeTube->desStartOdd.m_hRefPart1);
		odddlg.m_hRefPart2.Format("0X%X",pFirstConeTube->desStartOdd.m_hRefPart2);
		odddlg.m_fCollideDist = pFirstConeTube->desStartOdd.m_fCollideDist;
		odddlg.m_iOddCalStyle = pFirstConeTube->desStartOdd.m_iOddCalStyle;
		if(odddlg.DoModal()==IDOK)
		{
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart1);
			sscanf(sText,"%X",&pFirstConeTube->desStartOdd.m_hRefPart1);
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart2);
			sscanf(sText,"%X",&pFirstConeTube->desStartOdd.m_hRefPart2);
			pFirstConeTube->desStartOdd.m_fCollideDist = odddlg.m_fCollideDist;
			pFirstConeTube->desStartOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
			pFirstConeTube->CalStartOddment();
			char valueStr[MAX_TEMP_CHAR_50+1]="";
			if(pFirstConeTube->GetPropValueStr(pItem->m_idProp,valueStr)>0)
				pItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("jointStart"))
	{
		CConeTubeLjParaDlg dlg;
		dlg.m_handle = pFirstConeTube->handle;
		dlg.m_bStartLj = TRUE;
		if(IDOK == dlg.DoModal())
		{
			pItem->m_lpNodeInfo->m_strPropValue = dlg.m_sType;
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("startPos"))
	{
		pointDlg.m_sCaption = "�ֹ�ʼ���������";
		pointDlg.m_fPointX = pFirstConeTube->Start().x;
		pointDlg.m_fPointY = pFirstConeTube->Start().y;
		pointDlg.m_fPointZ = pFirstConeTube->Start().z;
		if(pFirstConeTube->desStartPos.endPosType!=4)	//4.ָ������
			pointDlg.m_bReadOnly=TRUE;
		pointDlg.m_bLockPoint=pFirstConeTube->IsStartPosLocked();
		if(pointDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("startPos.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("startPos.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("startPos.z"),CString(sText));
			for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			{
				pConeTube->SetStart(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
				if(pointDlg.m_bLockPoint)
					pConeTube->LockStartPos();
				else 
					pConeTube->UnlockStartPos();
			}
		}
		//ʼ������
		if(pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart)
		{	//����������Ϊֻ������
			SetPropItemReadOnly(pFirstConeTube,"startPos.x",TRUE);
			SetPropItemReadOnly(pFirstConeTube,"startPos.y",TRUE);
			SetPropItemReadOnly(pFirstConeTube,"startPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ����ֻ������
			SetPropItemReadOnly(pFirstConeTube,"startPos.x",pFirstConeTube->desStartPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstConeTube,"startPos.y",pFirstConeTube->desStartPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstConeTube,"startPos.z",pFirstConeTube->desStartPos.endPosType!=4);	//4.ָ������
		}
		SetPropItemReadOnly(pFirstConeTube,"hStart",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstConeTube,"oddStart",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstConeTube,"desStartPos",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstConeTube,"desStartPos.endPosType",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstConeTube,"desStartPos.datum_pos_style",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstConeTube,"desStartPos.hDatumPart",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstConeTube,"desStartPos.fNormOffset",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstConeTube,"desStartPos.fEccentricDist",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstConeTube,"desStartPos.face_offset_norm",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstConeTube,"desStartPos.hDatumStartPart",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstConeTube,"desStartPos.bDatumStartStartEnd",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstConeTube,"desStartPos.hDatumEndPart",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
		SetPropItemReadOnly(pFirstConeTube,"desStartPos.bDatumEndStartEnd",pFirstConeTube->IsStartPosLocked()||pFirstConeTube->bHuoQuStart);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("actualStartPos"))
	{
		pointDlg.m_sCaption = "�ֹ�ʼ��ʵ������";
		pointDlg.m_bEnableLock=FALSE;
		pointDlg.m_bReadOnly=TRUE;
		f3dPoint actualStart,vec=pFirstConeTube->Start()-pFirstConeTube->End();
		normalize(vec);
		actualStart = pFirstConeTube->Start()+vec*pFirstConeTube->startOdd();
		pointDlg.m_fPointX = actualStart.x;
		pointDlg.m_fPointY = actualStart.y;
		pointDlg.m_fPointZ = actualStart.z;
		pointDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("oddEnd"))
	{
		odddlg.m_hRefPart1.Format("0X%X",pFirstConeTube->desEndOdd.m_hRefPart1);
		odddlg.m_hRefPart2.Format("0X%X",pFirstConeTube->desEndOdd.m_hRefPart2);
		odddlg.m_fCollideDist = pFirstConeTube->desEndOdd.m_fCollideDist;
		odddlg.m_iOddCalStyle = pFirstConeTube->desEndOdd.m_iOddCalStyle;
		if(odddlg.DoModal()==IDOK)
		{
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart1);
			sscanf(sText,"%X",&pFirstConeTube->desEndOdd.m_hRefPart1);
			_snprintf(sText,MAX_TEMP_CHAR_100,odddlg.m_hRefPart2);
			sscanf(sText,"%X",&pFirstConeTube->desEndOdd.m_hRefPart2);
			pFirstConeTube->desEndOdd.m_fCollideDist = odddlg.m_fCollideDist;
			pFirstConeTube->desEndOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
			pFirstConeTube->CalEndOddment();
			char valueStr[MAX_TEMP_CHAR_50+1]="";
			if(pFirstConeTube->GetPropValueStr(pItem->m_idProp,valueStr)>0)
				pItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("jointEnd"))
	{
		CConeTubeLjParaDlg dlg;
		dlg.m_handle = pFirstConeTube->handle;
		dlg.m_bStartLj = FALSE;
		if(IDOK == dlg.DoModal())
		{
			pItem->m_lpNodeInfo->m_strPropValue = dlg.m_sType;
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("endPos"))
	{
		pointDlg.m_sCaption = "�ֹ��ն��������";
		pointDlg.m_fPointX = pFirstConeTube->End().x;
		pointDlg.m_fPointY = pFirstConeTube->End().y;
		pointDlg.m_fPointZ = pFirstConeTube->End().z;
		if(pFirstConeTube->desEndPos.endPosType!=4)	//4.ָ������
			pointDlg.m_bReadOnly=TRUE;
		pointDlg.m_bLockPoint=pFirstConeTube->IsEndPosLocked();
		if(pointDlg.DoModal()==IDOK)
		{
			char sText[MAX_TEMP_CHAR_100+1];
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("endPos.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("endPos.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("endPos.z"),CString(sText));
			for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			{
				pConeTube->SetEnd(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
				if(pointDlg.m_bLockPoint)
					pConeTube->LockEndPos();
				else 
					pConeTube->UnlockEndPos();
			}
		}
		//�ն�����
		if(pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd)
		{	//����������Ϊֻ������
			SetPropItemReadOnly(pFirstConeTube,"endPos.x",TRUE);
			SetPropItemReadOnly(pFirstConeTube,"endPos.y",TRUE);
			SetPropItemReadOnly(pFirstConeTube,"endPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ����ֻ������
			SetPropItemReadOnly(pFirstConeTube,"endPos.x",pFirstConeTube->desEndPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstConeTube,"endPos.y",pFirstConeTube->desEndPos.endPosType!=4);	//4.ָ������
			SetPropItemReadOnly(pFirstConeTube,"endPos.z",pFirstConeTube->desEndPos.endPosType!=4);	//4.ָ������
		}
		SetPropItemReadOnly(pFirstConeTube,"hEnd",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstConeTube,"oddEnd",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstConeTube,"desEndPos",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstConeTube,"desEndPos.endPosType",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstConeTube,"desEndPos.datum_pos_style",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstConeTube,"desEndPos.hDatumPart",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstConeTube,"desEndPos.fNormOffset",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstConeTube,"desEndPos.fEccentricDist",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstConeTube,"desEndPos.face_offset_norm",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstConeTube,"desEndPos.hDatumStartPart",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstConeTube,"desEndPos.bDatumStartStartEnd",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstConeTube,"desEndPos.hDatumEndPart",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
		SetPropItemReadOnly(pFirstConeTube,"desEndPos.bDatumEndStartEnd",pFirstConeTube->IsEndPosLocked()||pFirstConeTube->bHuoQuEnd);
	} 
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("actualEndPos"))
	{
		pointDlg.m_sCaption = "�ֹ��ն�ʵ������";
		pointDlg.m_bEnableLock=FALSE;
		pointDlg.m_bReadOnly=TRUE;
		f3dPoint actualEnd,vec=pFirstConeTube->End()-pFirstConeTube->Start();
		normalize(vec);
		actualEnd = pFirstConeTube->End()+vec*pFirstConeTube->endOdd();
		pointDlg.m_fPointX = actualEnd.x;
		pointDlg.m_fPointY = actualEnd.y;
		pointDlg.m_fPointZ = actualEnd.z;
		pointDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.face_offset_norm"))
	{	
		vecDlg.m_sCaption = "ƽ���淨��";
		vecDlg.m_fVectorX = pFirstConeTube->desStartPos.face_offset_norm.x;
		vecDlg.m_fVectorY = pFirstConeTube->desStartPos.face_offset_norm.y;
		vecDlg.m_fVectorZ = pFirstConeTube->desStartPos.face_offset_norm.z;
		if(vecDlg.DoModal()==IDOK)
		{
			for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				pConeTube->desStartPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.face_offset_norm"))
	{	
		vecDlg.m_sCaption = "ƽ���淨��";
		vecDlg.m_fVectorX = pFirstConeTube->desEndPos.face_offset_norm.x;
		vecDlg.m_fVectorY = pFirstConeTube->desEndPos.face_offset_norm.y;
		vecDlg.m_fVectorZ = pFirstConeTube->desEndPos.face_offset_norm.z;
		if(vecDlg.DoModal()==IDOK)
		{
			for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
				pConeTube->desEndPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("startWorkPlane")
		||pItem->m_idProp==CLDSConeTube::GetPropID("endWorkPlane"))
	{
		CPartWorkPlaneDlg planeDlg;
		if(pItem->m_idProp==CLDSConeTube::GetPropID("startWorkPlane"))
			planeDlg.m_bStart=TRUE;
		else
			planeDlg.m_bStart=FALSE;
		planeDlg.m_pDatumPart=pFirstConeTube;
		planeDlg.DoModal();
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("relativeObjs"))
	{
		CRelativeObjectDlg relativeDlg;
		relativeDlg.m_pWorkObj = pFirstConeTube;
		relativeDlg.DoModal();
	}
	else
		return FALSE;
	for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
	{
		pConeTube->SetModified();
		pConeTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),0);
		g_pSolidDraw->NewSolidPart(pConeTube->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL ModifyConeTubeProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSConeTube *pCurTube = (CLDSConeTube*)pView->selectObjs.GetFirst();
	CPropTreeItem *pFindItem,*pSonPropItem,*pParentItem,*pLeafPropItem;
	CLDSConeTube *pConeTube = NULL;
	CItemInfo *lpInfo;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	const int GROUP_GENERAL=1,GROUP_START=2,GROUP_END=3;
	//Ta.BeginUndoListen();
	if(pItem->m_idProp==CLDSConeTube::GetPropID("layer"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			pConeTube->SetLayer(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pConeTube->relativeObjs.GetFirst();pMirObj;pMirObj=pConeTube->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//��ת�Գ�
				CLDSConeTube *pMirLineTube=(CLDSConeTube*)pConeTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_CONETUBE);
				if(pMirLineTube)
					CalMirAtomLayer(pConeTube->layer(),pMirLineTube->layer(),pMirObj->mirInfo);
			}
		}	
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("cMaterial"))
	{	//�ֹܲ���
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			pConeTube->cMaterial = QuerySteelBriefMatMark(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pConeTube->relativeObjs.GetFirst();pMirObj;pMirObj=pConeTube->relativeObjs.GetNext())
			{
				CLDSConeTube *pMirLineTube=(CLDSConeTube*)pConeTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_CONETUBE);
				if(pMirLineTube)
					pMirLineTube->cMaterial=pConeTube->cMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("specification"))
	{	//�ֹܸ˹��
		CString ss;
		double wide,thick;
		restore_JG_guige(tem_str,wide,thick);
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			pConeTube->size_wide = wide;
			pConeTube->size_wideUpSide = thick;
			if(pConeTube->GetRadius()>g_sysPara.fTubeWeldRoadMinD)
			{
				pConeTube->m_bHasWeldRoad=TRUE;
				ss.Format("��");
			}
			else
			{
				pConeTube->m_bHasWeldRoad=FALSE;
				ss.Format("��");
			}
			for(RELATIVE_OBJECT *pMirObj=pConeTube->relativeObjs.GetFirst();pMirObj;pMirObj=pConeTube->relativeObjs.GetNext())
			{
				CLDSConeTube *pMirLineTube=(CLDSConeTube*)pConeTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_CONETUBE);
				if(pMirLineTube)
				{
					pMirLineTube->size_wide=wide;
					pMirLineTube->size_wideUpSide=thick;
					pMirLineTube->m_bHasWeldRoad=pConeTube->m_bHasWeldRoad;
				}
			}
		}
		pPropList->SetItemPropValue(CLDSConeTube::GetPropID("m_bHasWeldRoad"),ss);
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("m_bHasWeldRoad"),&pParentItem);
		if(pFindItem)
			ModifyLineTubeProperty(pPropList,pFindItem, ss);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("iSeg"))
	{
		SEGI iSeg = SEGI(valueStr.GetBuffer());
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			pConeTube->iSeg = iSeg;
			for(RELATIVE_OBJECT *pMirObj=pConeTube->relativeObjs.GetFirst();pMirObj;pMirObj=pConeTube->relativeObjs.GetNext())
			{
				CLDSConeTube *pMirLineTube=(CLDSConeTube*)pConeTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_CONETUBE);
				if(pMirLineTube)
					pMirLineTube->iSeg=pConeTube->iSeg;
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("sPartNo"))
	{
		
		bool parsed=false;
		SEGI iSeg;
		if(CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(tem_str,&iSeg,NULL))
		{
			parsed=true;
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("iSeg"),iSeg.ToString());
		}
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			if(parsed)
				pConeTube->iSeg=iSeg;
			pConeTube->SetPartNo(tem_str);
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("cfgword"))
	{
		//���������˼�����ĺ�
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			pConeTube->cfgword = pCurTube->cfgword;
			for(RELATIVE_OBJECT *pMirObj=pConeTube->relativeObjs.GetFirst();pMirObj;pMirObj=pConeTube->relativeObjs.GetNext())
			{
				CLDSConeTube *pMirLineTube=(CLDSConeTube*)pConeTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_CONETUBE);
				if(pMirLineTube)
					pMirLineTube->cfgword=pConeTube->cfgword;
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("m_uStatMatCoef"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->m_uStatMatCoef = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("m_hPartWeldParent"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->m_hPartWeldParent=HexStrToLong(tem_str);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("m_bDisplayHole"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("��")==0)
				pConeTube->m_bDisplayHole = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
				pConeTube->m_bDisplayHole = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pConeTube->relativeObjs.GetFirst();pMirObj;pMirObj=pConeTube->relativeObjs.GetNext())
			{
				CLDSConeTube *pMirLineTube=(CLDSConeTube*)pConeTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_CONETUBE);
				if(pMirLineTube)
					pMirLineTube->m_bDisplayHole=pConeTube->m_bDisplayHole;
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("m_bVirtualPart"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("��")==0)
				pConeTube->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
				pConeTube->m_bVirtualPart = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pConeTube->relativeObjs.GetFirst();pMirObj;pMirObj=pConeTube->relativeObjs.GetNext())
			{
				CLDSConeTube *pMirLineTube=(CLDSConeTube*)pConeTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_CONETUBE);
				if(pMirLineTube)
					pMirLineTube->m_bVirtualPart=pConeTube->m_bVirtualPart;
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("m_bUserSpecColor"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("��")==0)
				pConeTube->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
				pConeTube->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pConeTube->relativeObjs.GetFirst();pMirObj;pMirObj=pConeTube->relativeObjs.GetNext())
			{
				CLDSConeTube *pMirLineTube=(CLDSConeTube*)pConeTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_CONETUBE);
				if(pMirLineTube)
					pMirLineTube->m_bUserSpecColor=pConeTube->m_bUserSpecColor;
			}
		}
		if(pCurTube->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("crMaterial"),NULL);
			if(pFindItem==NULL)
			{
				char sText[MAX_TEMP_CHAR_100+1]="";
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType = CDT_COLOR;
				lpInfo->m_strPropName = "������ɫ";
				lpInfo->m_strPropHelp = "������ɫ";
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
				pFindItem->m_idProp = CLDSConeTube::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if(pCurTube->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
				pFindItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("crMaterial"));
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			pConeTube->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pConeTube->relativeObjs.GetFirst();pMirObj;pMirObj=pConeTube->relativeObjs.GetNext())
			{
				CLDSConeTube *pMirConeTube=(CLDSConeTube*)pConeTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_CONETUBE);
				if(pMirConeTube)
					pMirConeTube->crMaterial=pConeTube->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("sNotes"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			_snprintf(pConeTube->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
			//��ע���ù����޸� wht 10-10-22
			/*for(RELATIVE_OBJECT *pMirObj=pConeTube->relativeObjs.GetFirst();pMirObj;pMirObj=pConeTube->relativeObjs.GetNext())
			{
			CLDSConeTube *pMirLineTube=(CLDSConeTube*)pConeTube->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_CONETUBE);
			if(pMirLineTube)
			strcpy(pMirLineTube->sNotes,pConeTube->sNotes);
			}*/
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("m_bHasWeldRoad"))
	{
		char sText[MAX_TEMP_CHAR_100+1]="";
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("��")==0)
				pConeTube->m_bHasWeldRoad = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
				pConeTube->m_bHasWeldRoad = FALSE;
		}
		if(pCurTube->m_bHasWeldRoad)
		{
			pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.x"),&pParentItem);
			if(pFindItem==NULL)
			{
				//������ָ����ʽ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_strPropName = "������ָ����ʽ";
				lpInfo->m_strPropHelp = "ֱ�Ӹ�������ͨ�����׼�߼нǶ�λ";
				lpInfo->m_cmbItems = "0.ֱ��ָ��|1.�Ƕȶ�λ";
				pSonPropItem = pPropList->InsertItem(pItem,lpInfo, 1,true);
				pSonPropItem->m_idProp = CLDSConeTube::GetPropID("m_iWeldRoadStyle");
				pSonPropItem->m_bHideChildren = FALSE;
				if(pCurTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//���׼�߽Ƕ�
				if (pCurTube->m_iWeldRoadStyle == 1)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "�����н�";
					lpInfo->m_strPropHelp = "���׼��֮��ļн�(ȡֵ��Χ��0-360֮��)";
					pSonPropItem = pPropList->InsertItem(pItem,lpInfo,2,true);
					pSonPropItem->m_idProp = CLDSConeTube::GetPropID("weldRoadAngle");
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
					if(pCurTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				}
				//��������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_strPropName = "��������";
				lpInfo->m_strPropHelp = "��������";
				pSonPropItem = pPropList->InsertItem(pItem,lpInfo, 3,true);
				pSonPropItem->m_idProp = CLDSConeTube::GetPropID("nearWeldRoadVec");
				pSonPropItem->m_bHideChildren = FALSE;
				if(CLDSConeTube::GetPropStatus(pSonPropItem->m_idProp)!=-1)
					pSonPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pSonPropItem->m_idProp);
				pPropList->SetItemReadOnly(pSonPropItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1);
				//�����߷���X����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��������X����";
				lpInfo->m_strPropHelp = "�����߷���X����";
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,4,true);
				pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("nearWeldRoadVec.x");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropList->SetItemReadOnly(pLeafPropItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1);
				//�����߷���Y����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��������Y����";
				lpInfo->m_strPropHelp = "�����߷���Y����";
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, 5,true);
				pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("nearWeldRoadVec.y");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropList->SetItemReadOnly(pLeafPropItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1);
				//�����߷���Z����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��������Z����";
				lpInfo->m_strPropHelp = "�����߷���Z����";
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,6,true);
				pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("nearWeldRoadVec.z");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropList->SetItemReadOnly(pLeafPropItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1);
			}
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("weldRoadAngle"));
			pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("m_iWeldRoadStyle"));
			pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec"));
			pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.x"));
			pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.y"));
			pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.z"));
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("m_iWeldRoadStyle"))
	{
		char sText[MAX_TEMP_CHAR_100+1]="";
		if(valueStr.CompareNoCase("0.ֱ��ָ��")==0)
			pCurTube->m_iWeldRoadStyle = 0;
		else
			pCurTube->m_iWeldRoadStyle = 1;
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("weldRoadAngle"),&pParentItem);
		if(pFindItem==NULL)
		{
			//���׼�߽Ƕ�
			if (pCurTube->m_iWeldRoadStyle == 1)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "�����н�";
				lpInfo->m_strPropHelp = "���׼��֮��ļн�(ȡֵ��Χ��0-360֮��)";
				pSonPropItem = pPropList->InsertItem(pItem,lpInfo,1,true);
				pSonPropItem->m_idProp = CLDSConeTube::GetPropID("weldRoadAngle");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
			pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec"),&pParentItem);
			if(pFindItem==NULL)
			{
				//��������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_strPropName = "��������";
				lpInfo->m_strPropHelp = "��������";
				pSonPropItem = pPropList->InsertItem(pItem,lpInfo, 2,true);
				pSonPropItem->m_idProp = CLDSConeTube::GetPropID("nearWeldRoadVec");
				pSonPropItem->m_bHideChildren = FALSE;
				if(CLDSConeTube::GetPropStatus(pSonPropItem->m_idProp)!=-1)
					pSonPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pSonPropItem->m_idProp);
				pPropList->SetItemReadOnly(pSonPropItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1);
				//�����߷���X����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��������X����";
				lpInfo->m_strPropHelp = "�����߷���X����";
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,3,true);
				pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("nearWeldRoadVec.x");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropList->SetItemReadOnly(pLeafPropItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1);
				//�����߷���Y����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��������Y����";
				lpInfo->m_strPropHelp = "�����߷���Y����";
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, 4,true);
				pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("nearWeldRoadVec.y");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropList->SetItemReadOnly(pLeafPropItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1);
				//�����߷���Z����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "��������Z����";
				lpInfo->m_strPropHelp = "�����߷���Z����";
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,5,true);
				pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("nearWeldRoadVec.z");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropList->SetItemReadOnly(pLeafPropItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1);
			}
		}
		else
		{
			pFindItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec"),&pParentItem);
			pFindItem->SetReadOnly(pCurTube->m_iWeldRoadStyle == 1);
			pFindItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.x"),&pParentItem);
			pFindItem->SetReadOnly(pCurTube->m_iWeldRoadStyle == 1);
			pFindItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.y"),&pParentItem);
			pFindItem->SetReadOnly(pCurTube->m_iWeldRoadStyle == 1);
			pFindItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.z"),&pParentItem);
			pFindItem->SetReadOnly(pCurTube->m_iWeldRoadStyle == 1);
		}
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("weldRoadAngle"),&pParentItem);
		if(pCurTube->m_iWeldRoadStyle==0)
		{
			if(pFindItem!=NULL)
				pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("weldRoadAngle"));
		}
		else if(pCurTube->m_iWeldRoadStyle==1)
		{
			if(pFindItem==NULL)
			{
				//���׼�߽Ƕ�
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "�����н�";
				lpInfo->m_strPropHelp = "���׼��֮��ļн�(ȡֵ��Χ��0-360֮��)";
				pSonPropItem = pPropList->InsertItem(pItem,lpInfo,1,true);
				pSonPropItem->m_idProp = CLDSConeTube::GetPropID("weldRoadAngle");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("weldRoadAngle"),&pParentItem);
				pFindItem->m_bHideChildren = FALSE;
			}
			pFindItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec"),&pParentItem);
			pPropList->SetItemReadOnly(pFindItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1);
			pFindItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.x"),&pParentItem);
			pPropList->SetItemReadOnly(pFindItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1);
			pFindItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.y"),&pParentItem);
			pPropList->SetItemReadOnly(pFindItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1);
			pFindItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.z"),&pParentItem);
			pPropList->SetItemReadOnly(pFindItem->m_idProp,pCurTube->m_iWeldRoadStyle == 1);
		}
		//�ѻ���ת��Ϊ�ǶȲ���֤��0-360֮��
		double fAngle = pCurTube->CalWeldRoadAngle()*DEGTORAD_COEF;
		int iTmp=0;
		if(fAngle>0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360;
		}
		else if(fAngle<0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360+360;
		}
		_snprintf(sText,MAX_TEMP_CHAR_100,"%f",fAngle);
		pPropList->SetItemPropValue(CLDSConeTube::GetPropID("weldRoadAngle"),CString(sText));
		SimplifiedNumString(sText);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("weldRoadAngle"))
	{
		char sText[MAX_TEMP_CHAR_100+1]="";
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			double fWeldRoadAngle=atof(valueStr);
			pConeTube->CalWeldRoadVec(fWeldRoadAngle);//���¼��㹤���淨��
		}
		pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();
		_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pConeTube->nearWeldRoadVec.x);
		pPropList->SetItemPropValue(CLDSConeTube::GetPropID("nearWeldRoadVec.x"),CString(sText));
		SimplifiedNumString(sText);
		_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pConeTube->nearWeldRoadVec.y);
		pPropList->SetItemPropValue(CLDSConeTube::GetPropID("nearWeldRoadVec.y"),CString(sText));
		SimplifiedNumString(sText);
		_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pConeTube->nearWeldRoadVec.z);
		pPropList->SetItemPropValue(CLDSConeTube::GetPropID("nearWeldRoadVec.z"),CString(sText));
		SimplifiedNumString(sText);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("nearWeldRoadVec.x"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			pConeTube->nearWeldRoadVec.x = atof(valueStr);
			pConeTube->CalWorkPlaneNorm();	//���¼��㹤���淨��
		}
		char sText[MAX_TEMP_CHAR_100+1]="";
		pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();
		//�ѻ���ת��Ϊ�ǶȲ���֤��0-360֮��
		double fAngle = pConeTube->CalWeldRoadAngle()*DEGTORAD_COEF;
		int iTmp=0;
		if(fAngle>0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360;
		}
		else if(fAngle<0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360+360;
		}
		_snprintf(sText,MAX_TEMP_CHAR_100,"%f",fAngle);
		pPropList->SetItemPropValue(CLDSConeTube::GetPropID("weldRoadAngle"),CString(sText));
		SimplifiedNumString(sText);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("nearWeldRoadVec.y"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			pConeTube->nearWeldRoadVec.y = atof(valueStr);
			pConeTube->CalWorkPlaneNorm();	//���¼��㹤���淨��
		}
		char sText[MAX_TEMP_CHAR_100+1]="";
		pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();
		//�ѻ���ת��Ϊ�ǶȲ���֤��0-360֮��
		double fAngle = pConeTube->CalWeldRoadAngle()*DEGTORAD_COEF;
		int iTmp=0;
		if(fAngle>0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360;
		}
		else if(fAngle<0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360+360;
		}
		_snprintf(sText,MAX_TEMP_CHAR_100,"%f",fAngle);
		pPropList->SetItemPropValue(CLDSConeTube::GetPropID("weldRoadAngle"),CString(sText));
		SimplifiedNumString(sText);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("nearWeldRoadVec.z"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			pConeTube->nearWeldRoadVec.z = atof(valueStr);
			pConeTube->CalWorkPlaneNorm();	//���¼��㹤���淨��
		}
		char sText[MAX_TEMP_CHAR_100+1]="";
		pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();
		//�ѻ���ת��Ϊ�ǶȲ���֤��0-360֮��
		double fAngle = pConeTube->CalWeldRoadAngle()*DEGTORAD_COEF;
		int iTmp=0;
		if(fAngle>0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360;
		}
		else if(fAngle<0)
		{
			iTmp=(int)fAngle/360;
			fAngle = fAngle-iTmp*360+360;
		}
		_snprintf(sText,MAX_TEMP_CHAR_100,"%f",fAngle);
		pPropList->SetItemPropValue(CLDSConeTube::GetPropID("weldRoadAngle"),CString(sText));
		SimplifiedNumString(sText);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("m_iNormCalStyle"))
	{
		char sText[MAX_TEMP_CHAR_100+1]="";
		if(valueStr.CompareNoCase("0.ֱ��ָ��")==0)
			pCurTube->m_iNormCalStyle = 0;
		else
			pCurTube->m_iNormCalStyle = 1;
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWorkPlaneNorm.x"),&pParentItem);
		pPropList->FindItemByPropId(CLDSConeTube::GetPropID("m_iNormCalStyle"),&pParentItem);//���»�ø��ڵ�
		if(pFindItem==NULL)
		{
			if(pCurTube->m_iNormCalStyle == 1)
			{
				//��׼����Ʒ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_strPropName = "��׼����Ʒ���";
				lpInfo->m_strPropHelp = "�ֹܻ�׼����Ʒ���һ�����û�ָ����ϵͳ�ɸ��ݽ��Ʒ��߼��ֹ����߾�ȷ������ֹܵĻ�׼�淨�ߡ�";
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 1,TRUE);
				pSonPropItem->m_idProp = CLDSConeTube::GetPropID("nearWorkPlaneNorm");
				pSonPropItem->m_bHideChildren = TRUE;
				if(CLDSConeTube::GetPropStatus(pSonPropItem->m_idProp)!=-1)
					pSonPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "X�������";
				lpInfo->m_strPropHelp = "�ֹܻ�׼�淨��X�᷽�������";
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, 1,TRUE);
				pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("nearWorkPlaneNorm.x");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Y�������";
				lpInfo->m_strPropHelp = "�ֹܻ�׼�淨��Y�᷽�������";
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,2,TRUE);
				pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("nearWorkPlaneNorm.y");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Z�������";
				lpInfo->m_strPropHelp = "�ֹܻ�׼�淨��Z�᷽�������";
				pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, 3,TRUE);
				pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("nearWorkPlaneNorm.z");
				pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pLeafPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
					pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//hWorkNormRefPart
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "���߲��ո˼�";
				lpInfo->m_strPropHelp = "����ֹܻ�׼�淨��ʱ�Ĺ�������ո˼���0X0ʱĬ��Ϊʼ�˻��ն˽ڵ㸸�˼�(����Ϊ�˼�����)";
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 2,TRUE);
				pSonPropItem->m_idProp = CLDSConeTube::GetPropID("hWorkNormRefPart");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
			pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("workPlaneNorm.x"),&pParentItem);
			pPropList->FindItemByPropId(CLDSConeTube::GetPropID("workPlaneNorm.x"),&pParentItem);//���»�ȡһ�����ĸ��ڵ�
			if(pFindItem == NULL)
			{
				//���ߵ�X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_strPropName = "X�������";
				lpInfo->m_strPropHelp = "�ֹܻ�׼�淨��X�᷽�������";
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 3,TRUE);
				pSonPropItem->m_idProp = CLDSConeTube::GetPropID("workPlaneNorm.x");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_strPropName = "Y�������";
				lpInfo->m_strPropHelp = "�ֹܻ�׼�淨��Y�᷽�������";
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 4,TRUE);
				pSonPropItem->m_idProp = CLDSConeTube::GetPropID("workPlaneNorm.y");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_strPropName = "Z�������";
				lpInfo->m_strPropHelp = "�ֹܻ�׼�淨��Z�᷽�������";
				pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, 5,TRUE);
				pSonPropItem->m_idProp = CLDSConeTube::GetPropID("workPlaneNorm.z");
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pCurTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else
		{
			if(pCurTube->m_iNormCalStyle == 0)
			{
				pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("nearWorkPlaneNorm"),&pParentItem);
				if(pFindItem!=NULL)
				{
					pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("nearWorkPlaneNorm"));
					pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("nearWorkPlaneNorm.x"));
					pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("nearWorkPlaneNorm.y"));
					pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("nearWorkPlaneNorm.z"));
					pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("hWorkNormRefPart"));
				}
			}
		}
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("workPlaneNorm.x"),&pParentItem);
		pPropList->SetItemReadOnly(pFindItem->m_idProp,pCurTube->m_iNormCalStyle==1);
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("workPlaneNorm.y"),&pParentItem);
		pPropList->SetItemReadOnly(pFindItem->m_idProp,pCurTube->m_iNormCalStyle==1);
		pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("workPlaneNorm.z"),&pParentItem);
		pPropList->SetItemReadOnly(pFindItem->m_idProp,pCurTube->m_iNormCalStyle==1);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("nearWorkPlaneNorm.x"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->nearWorkPlaneNorm.x = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("nearWorkPlaneNorm.y"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->nearWorkPlaneNorm.y = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("nearWorkPlaneNorm.z"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->nearWorkPlaneNorm.z = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("hWorkNormRefPart"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pConeTube->hWorkNormRefPart);
	}
	/*else if(pItem->m_idProp==CLDSConeTube::GetPropID("workPlaneNorm.x"))
	{
	for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
	{
	f3dPoint workPlaneNorm=pConeTube->WorkPlaneNorm();
	workPlaneNorm.x=atof(valueStr);
	pConeTube->SetWorkPlaneNorm(workPlaneNorm);
	}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("workPlaneNorm.y"))
	{
	for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
	{
	f3dPoint workPlaneNorm=pConeTube->WorkPlaneNorm();
	workPlaneNorm.y=atof(valueStr);
	pConeTube->SetWorkPlaneNorm(workPlaneNorm);
	}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("workPlaneNorm.z"))
	{
	for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
	{
	f3dPoint workPlaneNorm=pConeTube->WorkPlaneNorm();
	workPlaneNorm.z=atof(valueStr);
	pConeTube->SetWorkPlaneNorm(workPlaneNorm);
	}
	}*/
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("cutPlaneOrigin.x"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->cutPlaneOrigin.x = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("cutPlaneOrigin.y"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->cutPlaneOrigin.y = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("cutPlaneOrigin.z"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->cutPlaneOrigin.z = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("cutPlaneNorm.x"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->cutPlaneNorm.x = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("cutPlaneNorm.y"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->cutPlaneNorm.y = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("cutPlaneNorm.z"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->cutPlaneNorm.z = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("oddStart"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{	//����ֶ������˸˼�����ͷӦ���ȣ�������ͷ�ļ��㷽ʽ�޸�Ϊ������
			//�����������ø˼���صĸְ�ʱ�������¼�������ͷ��ֵ	wht 09-09-02
			pConeTube->desStartOdd.m_iOddCalStyle=2;
			pConeTube->SetStartOdd(atof(valueStr));//,TRUE,TRUE);
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("startPos.x"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->pStart->SetPositionX(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("startPos.y"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->pStart->SetPositionY(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("startPos.z"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->pStart->SetPositionZ(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.endPosType"))
	{
		int end_pos_type=0;
		if(valueStr.CompareNoCase("�ڵ㼴����")==0)
			end_pos_type = 0;
		else if(valueStr.CompareNoCase("�������")==0)
			end_pos_type = 1;
		else if(valueStr.CompareNoCase("����ͶӰ")==0)
			end_pos_type = 2;
		else if(valueStr.CompareNoCase("���߽���")==0)
			end_pos_type = 3;
		else if(valueStr.CompareNoCase("ָ������")==0)
			end_pos_type = 4;
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->desStartPos.endPosType = end_pos_type;
		//UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.datum_pos_style"))
	{
		int datum_pos_style=0;
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
			datum_pos_style = 5;
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->desStartPos.datum_pos_style = datum_pos_style;
		pParentItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("desStartPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
		if(datum_pos_style>2)
		{
			CReportVectorDlg vecDlg;
			vecDlg.m_sCaption = "ƽ���淨��";
			vecDlg.m_fVectorX = pCurTube->desStartPos.face_offset_norm.x;
			vecDlg.m_fVectorY = pCurTube->desStartPos.face_offset_norm.y;
			vecDlg.m_fVectorZ = pCurTube->desStartPos.face_offset_norm.z;
			if(vecDlg.DoModal()==IDOK)
			{
				for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
					pConeTube->desStartPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumPart"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pConeTube->desStartPos.hDatumPart);
		pParentItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("desStartPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pParentItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.fNormOffset"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->desStartPos.fNormOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.fEccentricDist"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->desStartPos.fEccentricDist = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumStartPart"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pConeTube->desStartPos.hDatumStartPart);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.bDatumStartStartEnd"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("ʼ��")==0)
				pConeTube->desStartPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
				pConeTube->desStartPos.bDatumStartStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.hDatumEndPart"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pConeTube->desStartPos.hDatumEndPart);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desStartPos.bDatumEndStartEnd"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("ʼ��")==0)
				pConeTube->desStartPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
				pConeTube->desStartPos.bDatumEndStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("oddEnd"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{	//����ֶ������˸˼�����ͷӦ���ȣ�������ͷ�ļ��㷽ʽ�޸�Ϊ������
			//�����������ø˼���صĸְ�ʱ�������¼�������ͷ��ֵ	wht 09-09-02
			pConeTube->desEndOdd.m_iOddCalStyle=2;
			pConeTube->SetEndOdd(atof(valueStr));//,TRUE,TRUE);
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("endPos.x"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->pEnd->SetPositionX(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("endPos.y"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->pEnd->SetPositionY(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("endPos.z"))
	{
		for(pConeTube=(CLDSConeTube*)Ta.Parts.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)Ta.Parts.GetNext())
			pConeTube->pEnd->SetPositionZ(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.endPosType"))
	{
		int end_pos_type=0;
		if(valueStr.CompareNoCase("�ڵ㼴����")==0)
			end_pos_type = 0;
		else if(valueStr.CompareNoCase("�������")==0)
			end_pos_type = 1;
		else if(valueStr.CompareNoCase("����ͶӰ")==0)
			end_pos_type = 2;
		else if(valueStr.CompareNoCase("���߽���")==0)
			end_pos_type = 3;
		else if(valueStr.CompareNoCase("ָ������")==0)
			end_pos_type = 4;
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->desEndPos.endPosType = end_pos_type;
		//UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.datum_pos_style"))
	{
		int datum_pos_style=0;
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
			datum_pos_style = 5;
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->desEndPos.datum_pos_style = datum_pos_style;
		pParentItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("desEndPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
		if(datum_pos_style>2)
		{
			CReportVectorDlg vecDlg;
			vecDlg.m_sCaption = "ƽ���淨��";
			vecDlg.m_fVectorX = pCurTube->desEndPos.face_offset_norm.x;
			vecDlg.m_fVectorY = pCurTube->desEndPos.face_offset_norm.y;
			vecDlg.m_fVectorZ = pCurTube->desEndPos.face_offset_norm.z;
			if(vecDlg.DoModal()==IDOK)
			{
				for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
					pConeTube->desEndPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.hDatumPart"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pConeTube->desEndPos.hDatumPart);
		pParentItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("desEndPos.endPosType"),NULL);
		if(pParentItem)
			UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pParentItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.fNormOffset"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->desEndPos.fNormOffset = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.fEccentricDist"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			pConeTube->desEndPos.fEccentricDist = atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.hDatumStartPart"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pConeTube->desEndPos.hDatumStartPart);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.bDatumStartStartEnd"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("ʼ��")==0)
				pConeTube->desEndPos.bDatumStartStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
				pConeTube->desEndPos.bDatumStartStartEnd = FALSE;
		}
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.hDatumEndPart"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pConeTube->desEndPos.hDatumEndPart);
	}
	else if(pItem->m_idProp==CLDSConeTube::GetPropID("desEndPos.bDatumEndStartEnd"))
	{
		for(pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("ʼ��")==0)
				pConeTube->desEndPos.bDatumEndStartEnd = TRUE;
			else if(valueStr.CompareNoCase("�ն�")==0)
				pConeTube->desEndPos.bDatumEndStartEnd = FALSE;
		}
	}
	else
		return FALSE;
	for(pConeTube=(CLDSConeTube*)Ta.Parts.GetFirst();pConeTube;pConeTube=(CLDSConeTube*)Ta.Parts.GetNext())
	{
		pConeTube->SetModified();
		pConeTube->ClearFlag();
		pConeTube->CalPosition();
		pConeTube->CalWorkPlaneNorm();
		pConeTube->CalWeldRoadAngle();
		pConeTube->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pConeTube->GetSolidPartObject());
		//�޸�����ͷ���������������˨���Լ���ذ�
		if(pItem->m_idProp==CLDSConeTube::GetPropID("oddStart")||pItem->m_idProp==CLDSConeTube::GetPropID("oddEnd"))
		{	
			CLsRef *pLsRef=NULL;
			CLDSBolt *pFirstBolt=NULL;
			CLDSParamPlate *pParamPlate=NULL;
			if((pItem->m_idProp==CLDSConeTube::GetPropID("oddStart")&&pConeTube->m_tJointStart.type>=2&&pConeTube->m_tJointStart.type<=10))
				pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pConeTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
			else if((pItem->m_idProp==CLDSConeTube::GetPropID("oddEnd")&&pConeTube->m_tJointEnd.type>=2&&pConeTube->m_tJointEnd.type<=10))
				pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pConeTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
			if(pParamPlate)
			{
				pParamPlate->SetModified();
				pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
				for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{
					if(pFirstBolt==NULL)
						pFirstBolt=pLsRef->GetLsPtr();
					(*pLsRef)->SetModified();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
				for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
				{	//�������ɸ��ӹ����б��еĹ���
					CLDSPart *pPlate=console.FromPartHandle(pItem->h,CLS_PLATE,CLS_PARAMPLATE);
					if(!pPlate)
						continue;
					pPlate->SetModified();
					pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				}
			}
			if(pFirstBolt)
			{
				CLDSPlate *pPlate=NULL;
				for(pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
				{
					if(pPlate->FindLsByHandle(pFirstBolt->handle))
					{
						pPlate->SetModified();
						pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
						g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
						break;
					}
				}
			}
		}
	}
	//���¼�������һ�µ�ǰѡ�и˼��Ļ�׼�淨��
	pConeTube=(CLDSConeTube*)pView->selectObjs.GetFirst();
	if(pConeTube!=NULL)
	{
		if(pConeTube->GetPropValueStr(CLDSConeTube::GetPropID("workPlaneNorm.x"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("workPlaneNorm.x"),CString(tem_str));
		if(pConeTube->GetPropValueStr(CLDSConeTube::GetPropID("workPlaneNorm.y"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("workPlaneNorm.y"),CString(tem_str));
		if(pConeTube->GetPropValueStr(CLDSConeTube::GetPropID("workPlaneNorm.z"),tem_str)>0)
			pPropList->SetItemPropValue(CLDSConeTube::GetPropID("workPlaneNorm.z"),CString(tem_str));
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL ModifySlotArmProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr)
{
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	pPropList = pPropDlg->GetPropertyList();
	CLDSSlotArm *pCurSlotArm = (CLDSSlotArm*)pView->selectObjs.GetFirst();
	CLDSSlotArm *pSlotArm = NULL;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;

	const int GROUP_GENERAL=1;
	if(pItem->m_idProp==CLDSSlotArm::GetPropID("layer"))
	{
		for(pSlotArm=(CLDSSlotArm*)pView->selectObjs.GetFirst();pSlotArm;pSlotArm=(CLDSSlotArm*)pView->selectObjs.GetNext())
		{
			pSlotArm->SetLayer(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pSlotArm->relativeObjs.GetFirst();pMirObj;pMirObj=pSlotArm->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//��ת�Գ�
				CLDSSlotArm *pMirSlotArm=(CLDSSlotArm*)pSlotArm->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_SLOTARM);
				if(pMirSlotArm)
					CalMirAtomLayer(pSlotArm->layer(),pMirSlotArm->layer(),pMirObj->mirInfo);
			}
		}	
	}
	else if(pItem->m_idProp==CLDSSlotArm::GetPropID("cMaterial"))
	{
		for(pSlotArm=(CLDSSlotArm*)pView->selectObjs.GetFirst();pSlotArm;pSlotArm=(CLDSSlotArm*)pView->selectObjs.GetNext())
		{
			pSlotArm->cMaterial = QuerySteelBriefMatMark(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pSlotArm->relativeObjs.GetFirst();pMirObj;pMirObj=pSlotArm->relativeObjs.GetNext())
			{
				CLDSSlotArm *pMirSlotArm=(CLDSSlotArm*)pSlotArm->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEFLAT);
				if(pMirSlotArm)
					pMirSlotArm->cMaterial=pSlotArm->cMaterial;
			}
		}
	}
	else
		return FALSE;
	g_pSolidDraw->Draw();
	return TRUE;
}

void CLDSView::DisplayConeTubeProperty(BOOL bRebuild)
{
	CLDSConeTube *pFirstTube = (CLDSConeTube*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem, *pSonPropItem, *pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo=NULL;
	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_START=2,GROUP_END=3;
	if(bRebuild)
	{	//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(3);
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"ʼ��");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"�ն�");
		pPropDlg->RefreshTabCtrl(CLDSConeTube::m_iCurDisplayPropGroup);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_CONETUBE;
		pPropList->SetModifyValueFunc(ModifyConeTubeProperty);
		pPropList->SetButtonClickFunc(ConeTubeButtonClick);
		pPropList->SetModifyStatusFunc(ModifyConeTubeItemStatus);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstTube->BelongModel()->IsTowerModel()||pFirstTube->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//�ֹܻ�����Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_strPropName = "׶�ܸ˻�����Ϣ";
		lpInfo->m_strPropHelp = "׶�ܸ˻�����Ϣ";
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSConeTube::GetPropID("basicInfo");
		if(CLDSConeTube::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "׶�ܾ��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("handle");
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
		//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "׶������ͼ��"; 
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("layer");
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//����	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "׶�ܲ���";
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390|Q420|Q460";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("cMaterial");
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "׶�ܸ˵Ĺ��(����뾶X����뾶)";
		lpInfo->m_cmbItems = "200X400|200X600|400X800";
		lpInfo->m_cmbType = CDT_EDIT;
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("specification");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
		lpInfo->m_strPropName = "�������";
		lpInfo->m_strPropHelp = "�������"; 
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("sPartNo");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�ֶκ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�ֶκ�";
		lpInfo->m_strPropHelp = "�ֶκ�";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("iSeg");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ĺ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "��ĺ�";
		lpInfo->m_strPropHelp = "��ĺ�";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("cfgword");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//ͳ��ϵ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ͳ��ϵ��";
		lpInfo->m_strPropHelp = "ͳ��ϵ��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("m_uStatMatCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���Ӹ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
		lpInfo->m_strPropName = "���Ӹ�����";
		lpInfo->m_strPropHelp = "����˹�����Ҫ���ӵ�����������,��ô�����Ծ��Ǳ������������ĺ��ӹ����ľ��;����˹�������Ҫ����,��ֵΪ0X0��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("m_hPartWeldParent");
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ʾ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��ʾ��";
		lpInfo->m_strPropHelp = "�Ƿ���ʾ��";
		lpInfo->m_cmbItems = "��|��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("m_bDisplayHole");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���⸨������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "���⸨������";
		lpInfo->m_strPropHelp = "���⸨������ֻ���ڸ�������������λ�������ǽṹ����ʵ���е�ͳ�Ĺ��������Բ���Ԥ����ͳ�ơ�";
		lpInfo->m_cmbItems = "��|��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("m_bVirtualPart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�û�ָ��������ɫ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "ָ��������ɫ";
		lpInfo->m_strPropHelp = "������ɫ�Ƿ�Ϊ�û�ָ��";
		lpInfo->m_cmbItems = "��|��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineTube::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstTube->m_bUserSpecColor)
		{	//������ɫ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
			lpInfo->m_cmbType = CDT_COLOR;
			lpInfo->m_strPropName = "������ɫ";
			lpInfo->m_strPropHelp = "������ɫ";
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSConeTube::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//ʵ�ʳ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ʵ�ʳ���";
		lpInfo->m_strPropHelp = "׶�ܵ�ʵ�ʳ���";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("length");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		pPropItem->SetReadOnly();
		//��ע
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��ע";
		lpInfo->m_strPropHelp = "��ע";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("sNotes");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "��������";
		lpInfo->m_strPropHelp = "�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("relativeObjs");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
		CPropTreeItem *pLeafPropItem;
		//������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�к�����";
		lpInfo->m_strPropHelp = "�Ƿ���ں�����,�����޷�׶����˵�����ں�����";
		lpInfo->m_cmbItems = "��|��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("m_bHasWeldRoad");
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(CLDSConeTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��׼�淨��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "��׼�淨��";
		lpInfo->m_strPropHelp = "׶�ܸ˻�׼�淨��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("workPlaneNorm");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSConeTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//����ָ����ʽ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "���㷽ʽ";
		lpInfo->m_strPropHelp = "ֱ�Ӹ�������ͨ�����˼�����";
		lpInfo->m_cmbItems = "0.ֱ��ָ��|1.���ո˼�";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSConeTube::GetPropID("m_iNormCalStyle");
		pSonPropItem->m_bHideChildren = FALSE;
		if(pFirstTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstTube->m_iNormCalStyle==1)
		{
			//��׼����Ʒ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "��׼����Ʒ���";
			lpInfo->m_strPropHelp = "׶�ܻ�׼����Ʒ���һ�����û�ָ����ϵͳ�ɸ��ݽ��Ʒ��߼�׶�����߾�ȷ�����׶�ܵĻ�׼�淨�ߡ�";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSConeTube::GetPropID("nearWorkPlaneNorm");
			pSonPropItem->m_bHideChildren = TRUE;
			if(CLDSConeTube::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "X�������";
			lpInfo->m_strPropHelp = "׶�ܻ�׼�淨��X�᷽�������";
			pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
			pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("nearWorkPlaneNorm.x");
			pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
			if(pFirstTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
				pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Y�������";
			lpInfo->m_strPropHelp = "׶�ܻ�׼�淨��Y�᷽�������";
			pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
			pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("nearWorkPlaneNorm.y");
			pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
			if(pFirstTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
				pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Z�������";
			lpInfo->m_strPropHelp = "׶�ܻ�׼�淨��Z�᷽�������";
			pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
			pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("nearWorkPlaneNorm.z");
			pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
			if(pFirstTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
				pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);

			//hWorkNormRefPart
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
			lpInfo->m_strPropName = "���߲��ո˼�";
			lpInfo->m_strPropHelp = "����׶�ܸ˻�׼�淨��ʱ�Ĺ�������ո˼���0X0ʱĬ��Ϊʼ�˻��ն˽ڵ㸸�˼�(����Ϊ�˼�����)";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSConeTube::GetPropID("hWorkNormRefPart");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "׶�ܻ�׼�淨��X�᷽�������";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSConeTube::GetPropID("workPlaneNorm.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(pFirstTube->m_iNormCalStyle==1);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "׶�ܸ˻�׼�淨��Y�᷽�������";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSConeTube::GetPropID("workPlaneNorm.y");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(pFirstTube->m_iNormCalStyle==1);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "׶�ܸ˻�׼�淨��Z�᷽�������";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSConeTube::GetPropID("workPlaneNorm.z");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(pFirstTube->m_iNormCalStyle==1);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//׶���и���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "�и���";
		lpInfo->m_strPropHelp = "׶���и���";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("cutPlane");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSConeTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "׶�ܸ��и����������";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSConeTube::GetPropID("cutPlaneOrigin");
		pSonPropItem->m_bHideChildren = TRUE;
		if(CLDSConeTube::GetPropStatus(pSonPropItem->m_idProp)!=-1)
			pSonPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "׶�ܸ��и������X�᷽������";
		pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
		pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("cutPlaneOrigin.x");
		pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
			pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "׶�ܸ��и������Y�᷽������";
		pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
		pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("cutPlaneOrigin.y");
		pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
			pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "׶�ܸ��и������Z�᷽������";
		pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
		pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("cutPlaneOrigin.z");
		pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
			pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���߷���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "׶�ܸ��и��淨�߷���";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSConeTube::GetPropID("cutPlaneNorm");
		pSonPropItem->m_bHideChildren = TRUE;
		if(CLDSConeTube::GetPropStatus(pSonPropItem->m_idProp)!=-1)
			pSonPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "׶���и��淨�߷���X�᷽������";
		pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
		pLeafPropItem->m_idProp = CLDSLineTube::GetPropID("cutPlaneNorm.x");
		pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineTube::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
			pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "׶�ܸ��и��淨�߷���Y�᷽������";
		pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
		pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("cutPlaneNorm.y");
		pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
			pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "׶���и��淨�߷���Z�᷽������";
		pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo, -1);
		pLeafPropItem->m_idProp = CLDSConeTube::GetPropID("cutPlaneNorm.z");
		pLeafPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pLeafPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
			pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
#endif
		//ʼ����Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_strPropName = "ʼ����Ϣ";
		lpInfo->m_strPropHelp = "׶�ܸ�ʼ����Ϣ";
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSConeTube::GetPropID("startInfo");
		if(CLDSConeTube::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//ʼ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "ʼ���������";
		lpInfo->m_strPropHelp = "׶�ܸ�ʼ���������";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = FALSE;
		pPropItem->m_idProp = CLDSConeTube::GetPropID("startPos");
		if(CLDSConeTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "׶�ܸ�ʼ��X�᷽�������";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSConeTube::GetPropID("startPos.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstTube->desStartPos.endPosType!=4);	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "׶�ܸ�ʼ��Y�᷽�������";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSConeTube::GetPropID("startPos.y");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstTube->desStartPos.endPosType!=4);	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "׶�ܸ�ʼ��Z�᷽�������";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSConeTube::GetPropID("startPos.z");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstTube->desStartPos.endPosType!=4);	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//ʼ��ʵ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "ʼ��ʵ������";
		lpInfo->m_strPropHelp = "׶�ܸ�ʼ�˿�������ͷ���ʵ������";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("actualStartPos");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		//�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "�����";
		lpInfo->m_strPropHelp = "׶�ܸ������"; 
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("hStart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);

		//����ͷ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
		lpInfo->m_strPropName = "����ͷ";
		lpInfo->m_strPropHelp = "׶�ܸ�ʼ������ͷֵ";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("oddStart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
		//ʼ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "ʼ������";
		lpInfo->m_strPropHelp = "׶��ʼ������";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("jointStart");
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
#endif
		//�˵�λ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��λ��ʽ";
		lpInfo->m_strPropHelp = "׶��ʼ�˻���λ��";
		lpInfo->m_cmbItems = "�ڵ㼴����|�������|����ͶӰ|";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("desStartPos.endPosType");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(CLDSConeTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_START);
		pPropItem->SetReadOnly(pFirstTube->IsStartPosLocked()||pFirstTube->bHuoQuStart);
		UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pPropItem,TRUE);
		//׶�ܹ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
		lpInfo->m_strPropName = "׶�ܹ�����";
		lpInfo->m_strPropHelp = "׶�ܹ������׼��Ϊʼ��λ�ã�����ȡ�����淨�߻��Զ��塣";
		pPropItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("startWorkPlane");
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_START);		
		//�ն���Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_strPropName = "�ն���Ϣ";
		lpInfo->m_strPropHelp = "׶���ն���Ϣ";
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSConeTube::GetPropID("endInfo");
		if(CLDSConeTube::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//�ն�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "�ն��������";
		lpInfo->m_strPropHelp = "׶���ն��������";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_bHideChildren = TRUE;
		pPropItem->m_idProp = CLDSConeTube::GetPropID("endPos");
		if(CLDSConeTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "׶���ն�����X�᷽�������ֵ";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSConeTube::GetPropID("endPos.x");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstTube->desEndPos.endPosType!=4);	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "׶���ն�����Y�᷽�������ֵ"; 
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSConeTube::GetPropID("endPos.y");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstTube->desEndPos.endPosType!=4);	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "׶���ն�����Z�᷽�������ֵ";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSConeTube::GetPropID("endPos.z");
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd)
			pSonPropItem->SetReadOnly(TRUE);
		else 
			pSonPropItem->SetReadOnly(pFirstTube->desEndPos.endPosType!=4);	//4.ָ������
		pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//�ն�ʵ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "�ն�ʵ������";
		lpInfo->m_strPropHelp = "׶���ն˿�������ͷ�������λ��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("actualEndPos");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//�յ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "�յ���";
		lpInfo->m_strPropHelp = "׶���յ���";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("hEnd");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		//����ͷ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
		lpInfo->m_strPropName = "����ͷ";
		lpInfo->m_strPropHelp = "׶���ն�����ͷֵ";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("oddEnd");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
		//�ն�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_strPropName = "�ն�����";
		lpInfo->m_strPropHelp = "׶�ܵ��ն�����";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("jointEnd");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
#endif
		//�˵�λ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��λ��ʽ";
		lpInfo->m_strPropHelp = "׶���ն˻���λ��";
		lpInfo->m_cmbItems = "�ڵ㼴����|�������|����ͶӰ|";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("desEndPos.endPosType");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(CLDSConeTube::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSConeTube::GetPropStatus(pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstTube->IsEndPosLocked()||pFirstTube->bHuoQuEnd);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_END);
		UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pPropItem,FALSE);
		//�ֹܹ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
		lpInfo->m_strPropName = "׶�ܹ�����";
		lpInfo->m_strPropHelp = "׶�ܹ������׼��Ϊ�ն�λ�ã�����ȡ�����淨�߻��Զ��塣";
		pPropItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp = CLDSConeTube::GetPropID("endWorkPlane");
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_END);		
	}
	else
	{//����Ҫ�ؽ���������ҳ
		CPropTreeItem *pFindItem, *pParentItem = NULL;
		SetPartMutiObjsProp(pFirstTube,"handle");
		SetPartMutiObjsProp(pFirstTube,"layer");
		SetPartMutiObjsProp(pFirstTube,"cMaterial");
		SetPartMutiObjsProp(pFirstTube,"specification");
		SetPartMutiObjsProp(pFirstTube,"iSeg");
		SetPartMutiObjsProp(pFirstTube,"sPartNo");
		SetPartMutiObjsProp(pFirstTube,"cfgword");
		SetPartMutiObjsProp(pFirstTube,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstTube,"m_hPartWeldParent");
		SetPartMutiObjsProp(pFirstTube,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstTube,"m_bVirtualPart");
		SetPartMutiObjsProp(pFirstTube,"m_bUserSpecColor");
		if(pFirstTube->m_bUserSpecColor)
		{	
			if(!SetPartMutiObjsProp(pFirstTube,"crMaterial"))
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSConeTube::GetPropID("m_bUserSpecColor"),NULL);
				if(pFindItem)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
					lpInfo->m_cmbType = CDT_COLOR;
					lpInfo->m_strPropName = "������ɫ";
					lpInfo->m_strPropHelp = "������ɫ";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSConeTube::GetPropID("crMaterial");
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSConeTube::IsPropEqual(selectObjs,pPropItem->m_idProp);
					if(pFirstTube->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				}
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstTube,"length");
		SetPartMutiObjsProp(pFirstTube,"sNotes");
		SetPartMutiObjsProp(pFirstTube,"m_bHasWeldRoad");
#ifdef __COMPLEX_PART_INC_//defined(__LMA_)||defined(__LDS_)
		long idProp = 0;	
		pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("weldRoadAngle"));
		pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("m_iWeldRoadStyle"));
		pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec"));
		pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.x"));
		pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.y"));
		pPropList->DeleteItemByPropId(CLDSConeTube::GetPropID("nearWeldRoadVec.z"));
#endif
		//SetPartMutiObjsProp(pFirstTube,"nearWeldRoadVec.x");
		//SetPartMutiObjsProp(pFirstTube,"nearWeldRoadVec.y");
		//SetPartMutiObjsProp(pFirstTube,"nearWeldRoadVec.z");
		SetPartMutiObjsProp(pFirstTube,"nearWorkPlaneNorm.x");
		SetPartMutiObjsProp(pFirstTube,"nearWorkPlaneNorm.y");
		SetPartMutiObjsProp(pFirstTube,"nearWorkPlaneNorm.z");
		SetPartMutiObjsProp(pFirstTube,"hWorkNormRefPart");
		SetPartMutiObjsProp(pFirstTube,"workPlaneNorm.x");
		SetPartMutiObjsProp(pFirstTube,"workPlaneNorm.y");
		SetPartMutiObjsProp(pFirstTube,"workPlaneNorm.z");
		SetPartMutiObjsProp(pFirstTube,"cutPlaneOrigin.x");
		SetPartMutiObjsProp(pFirstTube,"cutPlaneOrigin.y");
		SetPartMutiObjsProp(pFirstTube,"cutPlaneOrigin.z");
		SetPartMutiObjsProp(pFirstTube,"cutPlaneNorm.x");
		SetPartMutiObjsProp(pFirstTube,"cutPlaneNorm.y");
		SetPartMutiObjsProp(pFirstTube,"cutPlaneNorm.z");
		SetPartMutiObjsProp(pFirstTube,"hStart");
		SetPartMutiObjsProp(pFirstTube,"oddStart");
		SetPartMutiObjsProp(pFirstTube,"jointStart");
		SetPartMutiObjsProp(pFirstTube,"startPos.x");
		SetPartMutiObjsProp(pFirstTube,"startPos.y");
		SetPartMutiObjsProp(pFirstTube,"startPos.z");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.endPosType");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.datum_pos_style");
		pFindItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("desStartPos.endPosType"),&pParentItem);
		if(pFindItem)
			UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pFindItem,TRUE);
		SetPartMutiObjsProp(pFirstTube,"desStartPos.hDatumPart");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.fNormOffset");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.fEccentricDist");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.face_offset_norm");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.bDatumEndStartEnd");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.bDatumStartStartEnd");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.hDatumStartPart");
		SetPartMutiObjsProp(pFirstTube,"desStartPos.hDatumEndPart");
		//�ն���Ϣ
		SetPartMutiObjsProp(pFirstTube,"hEnd");
		SetPartMutiObjsProp(pFirstTube,"oddEnd");	//����ͷ
		SetPartMutiObjsProp(pFirstTube,"jointEnd");	//�ն�����
		SetPartMutiObjsProp(pFirstTube,"endPos.x");	//�ն�����--X
		SetPartMutiObjsProp(pFirstTube,"endPos.y");	//�ն�����--Y
		SetPartMutiObjsProp(pFirstTube,"endPos.z");	//�ն�����--Z
		SetPartMutiObjsProp(pFirstTube,"desEndPos.endPosType");	//�˻���λ��
		SetPartMutiObjsProp(pFirstTube,"desEndPos.datum_pos_style");
		pFindItem=pPropList->FindItemByPropId(CLDSConeTube::GetPropID("desEndPos.endPosType"),&pParentItem);
		if(pFindItem)
			UpdateLineTube_DesPos_EndPosType_TreeItem(pPropList,pFindItem,FALSE);
		SetPartMutiObjsProp(pFirstTube,"desEndPos.hDatumPart");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.fNormOffset");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.fEccentricDist");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.face_offset_norm");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.bDatumEndStartEnd");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.bDatumStartStartEnd");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.hDatumStartPart");
		SetPartMutiObjsProp(pFirstTube,"desEndPos.hDatumEndPart");
	}
	pPropList->Redraw();
}
void CLDSView::DisplaySlotArmProperty(BOOL bRebuild)
{
	CLDSSlotArm *pFirstSlotArm = (CLDSSlotArm*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	CXhChar500 sText;

	const int GROUP_GENERAL=1;
	if(bRebuild)
	{
		//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(1);
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_SLOTARM;
		pPropList->SetModifyValueFunc(ModifySlotArmProperty);
		//pPropList->SetButtonClickFunc(SlotArmButtonClick);
		pPropList->SetModifyStatusFunc(ModifySlotArmItemStatus);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstSlotArm->BelongModel()->IsTowerModel()||pFirstSlotArm->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//���ͺᵣ������Ϣ
		lpInfo = new CItemInfo();
		lpInfo->m_strPropName = "���ͺᵣ������Ϣ";
		lpInfo->m_strPropHelp = "���ͺᵣ������Ϣ";
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSSlotArm::GetPropID("basicInfo");
		if(CLDSSlotArm::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSSlotArm::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "���ͺᵣ�����LDSϵͳ�жԸְ��ȫ��Ψһ��ʶ";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSlotArm::GetPropID("handle");
		if(pFirstSlotArm->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText; 
		pPropItem->SetReadOnly();
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "���ͺᵣ����ͼ��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSSlotArm::GetPropID("layer");
		if(pFirstSlotArm->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "�ְ����";
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("cMaterial");
		if(pFirstSlotArm->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	}
	else
	{//����Ҫ�ؽ���������ҳ
		SetPartMutiObjsProp(pFirstSlotArm,"handle");
		SetPartMutiObjsProp(pFirstSlotArm,"layer");
		SetPartMutiObjsProp(pFirstSlotArm,"cMaterial");
	}
}
#endif
//#endif