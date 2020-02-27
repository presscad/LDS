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
#include "PartUcsDlg.h"
#include "ModifyRelativeObjPartNoDlg.h"
#include "FaceOffsetDlg.h"
#include "ParamPlateParamListDlg.h"
#include "DefHuoquFaceDlg.h"
#include "EditPlateDatumFaceDlg.h"
#include "XhCharString.h"
#include "CutAngleDlg.h"
#include "BoltPropDlg.h"
#include "PartWorkPlaneDlg.h"
#include "DatumPointPropertyDlg.h"
#include "DesignVectorPropDlg.h"
#include "DesignRibbedPlateDlg.h"
#include "DesignCircularPlateDlg.h"
#include "DesignShadowDlg.h"
#include "LogFile.h"
#include "InputHuoQuLineDlg.h"
#include "DrawingToolKits.h"
#include "btc.h"
#include "DesLineAngleRibPlateDlg.h"
#include "DbObjPropId.h"
#include "PropertyListOper.h"
#include "SnapTypeVerify.h"
#include "DesignJdb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//from DisplayPropertyPage.cpp
BOOL SetPartMutiObjsProp(CLDSObject *pObj,char *propName,long idClassType=0);
void SetPropItemReadOnly(CLDSDbObject *pObj,char *propName,BOOL bReadOnly);

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL ModifyPlateProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr);
void Update_Plate_Norm_CalStyle_TreeItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,BOOL bFirst)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPlate *pFirstPlate = (CLDSPlate*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem,*pFindItem,*pItem;
	CItemInfo *lpInfo;
	long idProp=0;
	if (pFirstPlate==NULL||pParentItem==NULL)
		return;
	CXhChar100 sText;
	CLDSVector facenorm;
	const int GROUP_GENERAL=1,GROUP_POS=2,GROUP_PROFILE=3,GROUP_OTHER=4;
	DWORD dwPropGroup=GetSingleWord(GROUP_PROFILE);
	if (bFirst)
		facenorm = pFirstPlate->designInfo.huoqufacenorm[0];
	else
		facenorm = pFirstPlate->designInfo.huoqufacenorm[1];
	if (facenorm.norm_style!=0)
	{
		if (bFirst)
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart");
		else
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart");
		pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		if (pFindItem==NULL)
		{	//��׼���߹���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Normal Part Handle";
			lpInfo->m_strPropHelp = "The handle of datum normal part.";
#else 
			lpInfo->m_strPropName = "��׼���߹���";
			lpInfo->m_strPropHelp = "��׼���߹���";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if (bFirst)
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.x"),&pItem);
		else
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"),&pItem);
		if (pFindItem)
			pFindItem->SetReadOnly();
		if (bFirst)
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.y"),&pItem);
		else
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"),&pItem);
		if (pFindItem)
			pFindItem->SetReadOnly();
		if (bFirst)
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.z"),&pItem);
		else
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"),&pItem);
		if (pFindItem)
			pFindItem->SetReadOnly();
	}
	else
	{
		if (bFirst)
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].norm_wing"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].direction"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].norm_wing"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].direction"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"));
		}
		if (bFirst)
		{
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].vector.x");
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		}
		else
		{
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].vector.x");
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		}
		if (pFindItem)
			pFindItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(idProp));
		if (bFirst)
		{
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].vector.y");
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		}
		else
		{
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].vector.y");
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		}
		if (pFindItem)
			pFindItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(idProp));
		if (bFirst)
		{
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].vector.z");
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		}
		else
		{
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].vector.z");
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		}
		if (pFindItem)
			pFindItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(idProp));
	}
	if (facenorm.norm_style==0)
	{
		if (bFirst)
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].angle");
		else
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].angle");
		pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		if (pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "huo qu angle";
			lpInfo->m_strPropHelp = "huo qu angle";
#else 
			lpInfo->m_strPropName = "�����ǣ���)";
			lpInfo->m_strPropHelp = "�����ǣ���)";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
			if (bFirst)
				SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].angle");
			else
				SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].angle");
		}
	}
	if (facenorm.norm_style==2)
	{

		if (bFirst)
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].angle"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].norm_wing"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].direction"));
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart");
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].angle"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].norm_wing"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].direction"));
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart");
		}
		pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		if (pFindItem==NULL)
		{	//����ο��˼�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Cross Rod Handle";
			lpInfo->m_strPropHelp = "The handle of cross reference rod.";
#else 
			lpInfo->m_strPropName = "����ο��˼�";
			lpInfo->m_strPropHelp = "����ο��˼�";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
			if (bFirst)
				SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].hCrossPart");
			else
				SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].hCrossPart");
		}
	}
	else if (facenorm.norm_style!=0)
	{
		if (bFirst)
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].angle"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"));
			idProp = CLDSPlate::GetPropID("huoqufacenorm[0].norm_wing");
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].angle"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"));
			idProp = CLDSPlate::GetPropID("huoqufacenorm[1].norm_wing");
		}
		if (facenorm.norm_style==1)
		{
			pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
			if (pFindItem==NULL)
			{	//��׼֫
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Datum Angle Leg";
				lpInfo->m_strPropHelp = "Datum angle leg";
				lpInfo->m_cmbItems = "X leg normal|Y leg normal";
#else 
				lpInfo->m_strPropName = "��׼֫";
				lpInfo->m_strPropHelp = "��׼֫";
				lpInfo->m_cmbItems = "X֫����|Y֫����";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
				pPropItem->m_idProp = idProp;
				if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			else
			{
				if (bFirst)
					SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].norm_wing");
				else
					SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].norm_wing");
			}
		}
		if (bFirst)
			idProp=CLDSPlate::GetPropID("huoqufacenorm[0].direction");
		else
			idProp=CLDSPlate::GetPropID("huoqufacenorm[1].direction");
		pFindItem = pPropList->FindItemByPropId(idProp,&pItem);
		if (pFindItem==NULL)
		{	//����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Normal";
#else 
			lpInfo->m_strPropName = "���߳���";
#endif
			if (bFirst)
			{
				lpInfo->m_strPropHelp = pFirstPlate->designInfo.huoqufacenorm[0].GetDirectionHelpStr();
				lpInfo->m_cmbItems = pFirstPlate->designInfo.huoqufacenorm[0].GetDirectionCmbItems();
			}
			else
			{
				lpInfo->m_strPropHelp = pFirstPlate->designInfo.huoqufacenorm[1].GetDirectionHelpStr();
				lpInfo->m_cmbItems = pFirstPlate->designInfo.huoqufacenorm[1].GetDirectionCmbItems();
			}
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		else
		{
			if (bFirst)
				SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].direction");
			else
				SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].direction");
		}
	}
}

void Update_Plate_Face_N_TreeItem(CPropertyList *pPropList,CPropTreeItem *pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPlate *pFirstPlate = (CLDSPlate*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem,*pSonPropItem,*pNextLeafItem;
	CItemInfo *lpInfo;
	if (pFirstPlate==NULL||pParentItem==NULL)
		return;
	CXhChar100 sText;
	const int GROUP_GENERAL=1,GROUP_POS=2,GROUP_PROFILE=3,GROUP_OTHER=4;
	DWORD dwPropGroup=GetSingleWord(GROUP_PROFILE);
	//�ְ��׼�� ��ʱ����ʾ�ְ��׼�淨�߶��� wht 11-01-24
	/*pPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("basicFace"),&pParentItem);
	if(pPropItem==NULL)
	{
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_strPropName = "��׼�涨��";
	lpInfo->m_strPropHelp = "��׼�涨��";
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
	pPropItem->m_idProp = CLDSPlate::GetPropID("basicFace");
	pPropItem->m_bHideChildren = TRUE;
	if(CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
	pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup=dwPropGroup;
	}*/
	if (pFirstPlate->face_N==1)
	{
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_bSelfCorrectBendline"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquFace1"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_start[0]"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_end[0]"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.x"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.y"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.z"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0]"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].norm_wing"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].direction"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquFace2"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_start[1]"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_end[1]"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1]"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].norm_wing"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].direction"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"));
	}
	if (pFirstPlate->face_N>1)
	{
		if (pFirstPlate->face_N==2)
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquFace2"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_start[1]"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_end[1]"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1]"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].norm_wing"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].direction"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("peakVertexOffsetY"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("extendVertexOffsetLen"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("peakPointMargin"));
		}
		//�ְ�����߼�������
		pPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_bSelfCorrectBendline"),&pParentItem);
		if (pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Bending line defination";
			lpInfo->m_strPropHelp = "Bending line defination";
			lpInfo->m_cmbItems = "0.Line specified|1.Intersection";
#else 
			lpInfo->m_strPropName = "�����߼��㷽ʽ";
			lpInfo->m_strPropHelp = "�����߼��㷽ʽ";
			lpInfo->m_cmbItems = "0.ָ��������|1.��������";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSPlate::GetPropID("m_bSelfCorrectBendline");
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		//�ְ��һ�����涨��
		pPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquFace1"),&pParentItem);
		if (pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "1st Bending Plane Defination";
			lpInfo->m_strPropHelp = "The defination of first bending plane";
#else 
			lpInfo->m_strPropName = "��һ�����涨��";
			lpInfo->m_strPropHelp = "��һ�����涨��";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSPlate::GetPropID("huoquFace1");
			pPropItem->m_bHideChildren = TRUE;
			if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		//��ʼ��
		pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquline_start[0]"),&pPropItem);
		if (pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,true);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoquline_start[0]");
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		if (pFirstPlate->designInfo.m_bSelfCorrectBendline)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pSonPropItem->m_lpNodeInfo->m_strPropName = "Bending Plane Pick Point";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "The pick point of bending plane ";
#else 
			pSonPropItem->m_lpNodeInfo->m_strPropName = "������ʰȡ��";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "������ʰȡ��";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pSonPropItem->m_lpNodeInfo->m_strPropName = "Bending Line Start Point";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "The start point of bending line ";
#else 
			pSonPropItem->m_lpNodeInfo->m_strPropName = "��������ʼ��";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "��������ʼ��";
#endif
		}
		//��ֹ��
		if (!pFirstPlate->designInfo.m_bSelfCorrectBendline)
		{
			pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquline_end[0]"),&pPropItem);
			if (pSonPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Bending Line End Point";
				lpInfo->m_strPropHelp = "The end point of bending line";
#else 
				lpInfo->m_strPropName = "��������ֹ��";
				lpInfo->m_strPropHelp = "��������ֹ��";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,true);
				pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoquline_end[0]");
				pSonPropItem->m_dwPropGroup=dwPropGroup;
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_end[0]"));
		pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector"),&pPropItem);
		if (pSonPropItem==NULL)
		{	//��������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Plane Normal";
#else 
			lpInfo->m_strPropName = "��������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,true);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[0].vector");
			pSonPropItem->m_bHideChildren = TRUE;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.x"),&pSonPropItem);
		if (pSonPropItem==NULL)
			return;
		if (pNextLeafItem==NULL)
		{	//��������X�᷽������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Coordinate";
			lpInfo->m_strPropHelp = "The plane normal coordinates of X axis direction.";
#else 
			lpInfo->m_strPropName = "��������X�᷽������";
			lpInfo->m_strPropHelp = "��������X�᷽������";
#endif
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,0,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[0].vector.x");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			if (pFirstPlate->designInfo.huoqufacenorm[0].norm_style!=0)
				pNextLeafItem->SetReadOnly();
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.y"),&pSonPropItem);
		if (pNextLeafItem==NULL)
		{	//��������Y�᷽������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Coordinate";
			lpInfo->m_strPropHelp = "The plane normal coordinates of Y axis direction.";
#else 
			lpInfo->m_strPropName = "��������Y�᷽������";
			lpInfo->m_strPropHelp = "��������Y�᷽������";
#endif
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,1,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[0].vector.y");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			if (pFirstPlate->designInfo.huoqufacenorm[0].norm_style!=0)
				pNextLeafItem->SetReadOnly();
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.z"),&pSonPropItem);
		if (pNextLeafItem==NULL)
		{	//��������Z�᷽������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Coordinate";
			lpInfo->m_strPropHelp = "The plane normal coordinates of Z axis direction.";
#else 
			lpInfo->m_strPropName = "��������Z�᷽������";
			lpInfo->m_strPropHelp = "��������Z�᷽������";
#endif
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,2,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[0].vector.z");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			if (pFirstPlate->designInfo.huoqufacenorm[0].norm_style!=0)
				pNextLeafItem->SetReadOnly();
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		//�ְ��һ�����淨�߶���
		pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0]"),&pPropItem);
		if (pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Defination";
			lpInfo->m_strPropHelp = "The normal defination of bending plane.";
#else 
			lpInfo->m_strPropName = "�����淨�߶���";
			lpInfo->m_strPropHelp = "�����淨�߶���";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,3,true);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[0]");
			pSonPropItem->m_bHideChildren = TRUE;
			if (CLDSPlate::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].norm_style"),&pSonPropItem);
		if (pSonPropItem==NULL)
			return;
		if (pNextLeafItem==NULL)
		{
			//֫���߼��㷽ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Calculative Type";
			lpInfo->m_strPropHelp = "The desgin parameter of work normal-- leg's normal calculative type";
#else 
			lpInfo->m_strPropName = "֫���߼��㷽ʽ";
			lpInfo->m_strPropHelp = "����������Ʋ���--֫���߼��㷽ʽ";
#endif
			lpInfo->m_cmbItems = CLDSVector::GetNormStyleCmbItems();
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,0,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[0].norm_style");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		Update_Plate_Norm_CalStyle_TreeItem(pPropList,pSonPropItem,TRUE);
	}
	if (pFirstPlate->face_N>2)
	{	//˫��嵽3����л��������˫��������ĿΪչ��״̬�����
		//pPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquFace1"),&pParentItem);
		//if(pPropItem)
		//	pPropItem->m_bHideChildren=TRUE;
		//�ְ�ڶ������涨��
		pPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquFace2"),&pParentItem);
		if (pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "2nd Bending Plane Defination";
			lpInfo->m_strPropHelp = "The defination of second bending plane";
#else 
			lpInfo->m_strPropName = "�ڶ������涨��";
			lpInfo->m_strPropHelp = "�ڶ������涨��";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSPlate::GetPropID("huoquFace2");
			pPropItem->m_bHideChildren = TRUE;
			if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		//��ʼ��
		pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquline_start[1]"),&pPropItem);
		if (pPropItem==NULL)
			return;
		if (pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,true);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoquline_start[1]");
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		if (pFirstPlate->designInfo.m_bSelfCorrectBendline)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pSonPropItem->m_lpNodeInfo->m_strPropName = "Bending Plane Pick Point";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "The pick point of bending plane ";
#else 
			pSonPropItem->m_lpNodeInfo->m_strPropName = "������ʰȡ��";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "������ʰȡ��";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pSonPropItem->m_lpNodeInfo->m_strPropName = "Bending Line Start Point";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "The start point of bending line ";
#else 
			pSonPropItem->m_lpNodeInfo->m_strPropName = "��������ʼ��";
			pSonPropItem->m_lpNodeInfo->m_strPropHelp = "��������ʼ��";
#endif
		}
		//��ֹ��
		if (!pFirstPlate->designInfo.m_bSelfCorrectBendline)
		{
			pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoquline_end[1]"),&pPropItem);
			if (pSonPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Bending Line End Point";
				lpInfo->m_strPropHelp = "The end point of bending line";
#else 
				lpInfo->m_strPropName = "��������ֹ��";
				lpInfo->m_strPropHelp = "��������ֹ��";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,true);
				pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoquline_end[1]");
				pSonPropItem->m_dwPropGroup=dwPropGroup;
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("huoquline_end[1]"));
		pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector"),&pPropItem);
		if (pSonPropItem==NULL)
		{	//��������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Plane Normal";
#else 
			lpInfo->m_strPropName = "��������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,true);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[1].vector");
			pSonPropItem->m_bHideChildren = TRUE;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"),&pSonPropItem);
		if (pSonPropItem==NULL)
			return;
		if (pNextLeafItem==NULL)
		{	//��������X�᷽������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Coordinate";
			lpInfo->m_strPropHelp = "The bending normal coordinates of X axis direction.";
#else 
			lpInfo->m_strPropName = "��������X�᷽������";
			lpInfo->m_strPropHelp = "��������X�᷽������";
#endif
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,0,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[1].vector.x");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			if (pFirstPlate->designInfo.huoqufacenorm[1].norm_style!=0)
				pNextLeafItem->SetReadOnly();
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"),&pSonPropItem);
		if (pNextLeafItem==NULL)
		{	//��������Y�᷽������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Coordinate";
			lpInfo->m_strPropHelp = "The bending normal coordinates of Y axis direction.";
#else 
			lpInfo->m_strPropName = "��������Y�᷽������";
			lpInfo->m_strPropHelp = "��������Y�᷽������";
#endif
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,1,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[1].vector.y");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			if (pFirstPlate->designInfo.huoqufacenorm[1].norm_style!=0)
				pNextLeafItem->SetReadOnly();
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"),&pSonPropItem);
		if (pNextLeafItem==NULL)
		{	//��������Z�᷽������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Coordinate";
			lpInfo->m_strPropHelp = "The bending normal coordinates of z axis direction.";
#else 
			lpInfo->m_strPropName = "��������Z�᷽������";
			lpInfo->m_strPropHelp = "��������Z�᷽������";
#endif
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,2,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[1].vector.z");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			if (pFirstPlate->designInfo.huoqufacenorm[1].norm_style!=0)
				pNextLeafItem->SetReadOnly();
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		//�ְ�ڶ������淨�߶���
		pSonPropItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1]"),&pPropItem);
		if (pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Plane Normal Defination";
			lpInfo->m_strPropHelp = "The normal defination of bending plane.";
#else 
			lpInfo->m_strPropName = "�����淨�߶���";
			lpInfo->m_strPropHelp = "�����淨�߶���";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,3,true);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[1]");
			pSonPropItem->m_bHideChildren = TRUE;
			if (CLDSPlate::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pNextLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].norm_style"),&pSonPropItem);
		if (pSonPropItem==NULL)
			return;
		if (pNextLeafItem==NULL)
		{
			//֫���߼��㷽ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Calculative Type";
			lpInfo->m_strPropHelp = "The desgin parameter of work normal-- leg's normal calculative type";
#else 
			lpInfo->m_strPropName = "֫���߼��㷽ʽ";
			lpInfo->m_strPropHelp = "����������Ʋ���--֫���߼��㷽ʽ";
#endif
			lpInfo->m_cmbItems = CLDSVector::GetNormStyleCmbItems();
			pNextLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,true);
			pNextLeafItem->m_idProp = CLDSPlate::GetPropID("huoqufacenorm[1].norm_style");
			if (pFirstPlate->GetPropValueStr(pNextLeafItem->m_idProp,sText)>0)
				pNextLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pNextLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pNextLeafItem->m_idProp);
			pNextLeafItem->m_dwPropGroup=dwPropGroup;
		}
		Update_Plate_Norm_CalStyle_TreeItem(pPropList,pSonPropItem,FALSE);
		//
		if (pFirstPlate->IsHuoQuLineCollinear3FacePlate())
		{
			CPropListOper<CLDSPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&pView->selectObjs);
			oper.InsertEditPropItem(pParentItem,"peakVertexOffsetY");
			oper.InsertEditPropItem(pParentItem,"extendVertexOffsetLen");
			oper.InsertEditPropItem(pParentItem,"peakPointMargin");
		}
	}
}

BOOL ModifyPlateItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSPlate::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL ModifyParamPlateItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSParamPlate::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL FirePlatePopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSPlate *pFirstPlate=(CLDSPlate*)pView->selectObjs.GetFirst();
	if (pFirstPlate==NULL)
		return FALSE;
	if (pItem->m_idProp==CLDSPlate::GetPropID("plateLinkPoles"))
	{
		if (0==iMenu)		//�鿴���Ӹ˼���
		{	//��ʾ����Ĺ���
			CLinkLinePartDlg linkdlg;
			linkdlg.m_pLjPara = &pFirstPlate->designInfo;
			linkdlg.DoModal();
		}
		else if (1==iMenu)	//�������Ӹ˼���
			pView->ImportLinePartToPlate(pFirstPlate);
	}
	else if (CLDSPlate::GetPropID("plateProfile")==pItem->m_idProp)
	{
		//�������ͣ�1-3����ʾ��Ӧ����������ͨ�壬4��ʾ�۵��壬5��ʾ��˨���
		//"���ܶ������ְ�|���������۵���|��˨���|��ͷ���ְ�|��ǿ�߰�|��ͨ���Ӱ�";
		int iFaceType=pFirstPlate->designInfo.iFaceType;
		if (iMenu==0)
			iFaceType=7;
		else if (iMenu==1)
			iFaceType=6;
		else if (iMenu==2)
			iFaceType=5;
		/*else if(iMenu==3)
			;
		else if(iMenu==4)
			;*/
		else if (iMenu==5)
		{
			if (pFirstPlate->designInfo.iFaceType==7||pFirstPlate->designInfo.iFaceType==6)
				iFaceType=1;
			else
				iFaceType=pFirstPlate->face_N;
		}
		pPropList->SetItemPropValue(pItem->m_idProp,sMenuName);
		if (pFirstPlate->designInfo.iFaceType!=iFaceType)
		{
			CUndoOperObject undo(&Ta);
			pFirstPlate->designInfo.iFaceType=iFaceType;
			pFirstPlate->SetModified();
			pFirstPlate->DesignPlate();
			pFirstPlate->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pFirstPlate->GetSolidPartObject());
			Update_Plate_Face_N_TreeItem(pPropList,pItem);
			CXhChar100 sText;
			int idProp=CLDSPlate::GetPropID("face_N");
			if (pFirstPlate->GetPropValueStr(idProp,sText))	//������ĺ�
				pPropList->SetItemPropValue(idProp,sText);

		}
	}
	return TRUE;
}
static UCS_STRU TransWCSFromLocal(CLDSGeneralPlate* pPlate,UCS_STRU& csLocal)
{
	UCS_STRU ucs=csLocal;
	coord_trans(ucs.origin,pPlate->ucs,TRUE,TRUE);
	vector_trans(ucs.axis_x,pPlate->ucs,TRUE,TRUE);
	vector_trans(ucs.axis_y,pPlate->ucs,TRUE,TRUE);
	ucs.axis_z=ucs.axis_x^ucs.axis_y;
	//vector_trans(ucs.axis_z,pPlate->ucs,TRUE,TRUE);
	return ucs;
}
BOOL PlateButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem=NULL,*pParentItem=NULL,*pPropItem=NULL,*pLeafItem=NULL;
	CLDSPlate *pFirstPlate=NULL,*pPlate=NULL;
	CLDSPart *pPart = NULL;
	CReportVectorDlg vecDlg;
	CCfgPartNoDlg cfgdlg;
	char sText[100]="";
	CString cmdStr,valueStr = _T("");

	if (pView->selectObjs.GetNodeNum()>0)
		pFirstPlate = (CLDSPlate*)pView->selectObjs.GetFirst();
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
	BOOL bRetCode=TRUE;
	try
	{
		CPropListOper<CLDSPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&pView->selectObjs);
		if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.hNode"))
		{
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Dependant node";
			modifyhdlg.m_sTitle="Dependant node:";
#else
			modifyhdlg.m_sCaption="�޸������ڵ�";
			modifyhdlg.m_sTitle="�����ڵ�:";
#endif
			if (pFirstPlate==NULL)
				return FALSE;
			modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->designInfo.m_hBaseNode);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
			modifyhdlg.m_iMirMsg=0;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select Dependant node:");
#else
			modifyhdlg.SetCmdPickPromptStr("��ѡ�������ڵ�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if (IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if (pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related node��",sSoftName,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
				else
				{
					CUndoOperObject undo(&Ta);
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pFirstPlate->designInfo.m_hBaseNode=modifyhdlg.m_nHandle;
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("designInfo.hNode"),valueStr);
					g_pSolidDraw->ReleaseSnapStatus();
					for (CLDSObject* pObj=pView->selectObjs.GetFirst();pObj;pObj=pView->selectObjs.GetNext())
					{
						if (pObj->GetClassTypeId()==CLS_PLATE)
						{
							CLDSPlate* pPate=(CLDSPlate*)pObj;
							pPate->designInfo.m_hBaseNode = modifyhdlg.m_nHandle;
							g_pSolidDraw->SetEntSnapStatus(pObj->handle);
						}
					}
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.hRod"))
		{
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Rod Start Node";
			modifyhdlg.m_sTitle="Start Node:";
#else
			modifyhdlg.m_sCaption="�޸������˼�";
			modifyhdlg.m_sTitle="�����˼�:";
#endif
			if (pFirstPlate==NULL)
				return FALSE;
			modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->designInfo.m_hBasePart);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
			modifyhdlg.m_iMirMsg=0;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select part:");
#else
			modifyhdlg.SetCmdPickPromptStr("��ѡ�������˼�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			if (IDOK==modifyhdlg.DoModal())
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if (pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��",sSoftName,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ�˼���",sSoftName,MB_OK);
#endif
				else
				{
					CUndoOperObject undo(&Ta);
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("designInfo.hRod"),valueStr);
					g_pSolidDraw->ReleaseSnapStatus();
					for (CLDSObject* pObj=pView->selectObjs.GetFirst();pObj;pObj=pView->selectObjs.GetNext())
					{
						if (pObj->GetClassTypeId()==CLS_PLATE)
						{
							CLDSPlate* pPate=(CLDSPlate*)pObj;
							pPate->designInfo.m_hBasePart= modifyhdlg.m_nHandle;
							g_pSolidDraw->SetEntSnapStatus(pObj->handle);
						}
					}
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("cfgword"))
		{
			cfgdlg.cfgword = pFirstPlate->cfgword;
			if (pFirstPlate->IsBodyObj())
				cfgdlg.cfg_style=CFG_BODY_NO;
			else
				cfgdlg.cfg_style=CFG_LEG_NO;
			if (cfgdlg.DoModal()==IDOK)
			{
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					pPlate->cfgword = cfgdlg.cfgword;		//�ڴ˴����µ�һ���ڵ������Ϻ�
			}
			CXhChar100 sText;
			if (pFirstPlate->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
				pItem->m_lpNodeInfo->m_strPropValue=sText;
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.origin")||
			pItem->m_idProp==CLDSPlate::GetPropID("wcs.axis_x")||pItem->m_idProp==CLDSPlate::GetPropID("wcs.axis_y"))
		{
			f3dPoint spec_point;
			CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
			int iMenu=0;	//pItem->m_idProp==CLDSPlate::GetPropID("wcs.origin")
			CString cmdStr="��ѡ��ǰ��������ϵԭ�����ڵĶ������˨:";
			if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.axis_x"))
			{
				iMenu=1;
				cmdStr="��ѡ��ǰ��������ϵX������궨�����ڵĶ������˨:";
			}
			else if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.axis_y"))
			{
				iMenu=2;
				cmdStr="��ѡ��ǰ��������ϵY������궨�����ڵĶ������˨:";
			}
			pCmdLine->FillCmdLine(cmdStr,"");
			CSnapTypeVerify verifier;
			DWORD dwhObj=0,dwExportFlag=0;
			verifier.SetVerifyFlag(OBJPROVIDER::DRAWINGSPACE,GetSingleWord(IDbEntity::DbPoint));
			verifier.AddVerifyType(OBJPROVIDER::SOLIDSPACE,CLS_BOLT);
			CLDSBolt* pPickBolt=NULL;
			PROFILE_VERTEX* pPickVertex=NULL;
			while (true)
			{
				if ((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				{
					pCmdLine->CancelCmdLine();
					return FALSE;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				if (obj.provider==OBJPROVIDER::SOLIDSPACE)
					pPickBolt=(CLDSBolt*)Ta.FromPartHandle(obj.hObj,CLS_BOLT);
				else if (obj.provider==OBJPROVIDER::DRAWINGSPACE)
				{
					IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
					IDbEntity* pEnt=pBuddyDrawing->GetDbEntity(obj.idEnt);
					if (pEnt==NULL || pEnt->GetDbEntType()!=IDbEntity::DbPoint)
						continue;	//��������
					if (pEnt->GetHiberId().masterId!=pFirstPlate->handle)
						continue;	//�ǵ�ǰ�ְ��ϵ�������
					DWORD dwObjectId=pEnt->GetHiberId().HiberDownId(1);
					for (PROFILE_VERTEX* pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
					{
						if (pVertex->vertex.ID==dwObjectId)
						{
							pPickVertex=pVertex;
							break;
						}
					}
				}
				else
					continue;
				if (pPickBolt!=NULL||pPickVertex!=NULL)
					break;
			}
			pCmdLine->FinishCmdLine();
			PLATEEDIT_STATE* pState=CLDSPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
			//���㹤������ϵ
			f3dPoint xLocalPickPoint;
			if (pPickBolt)
			{
				pFirstPlate->GetBoltIntersPos(pPickBolt,xLocalPickPoint);
				coord_trans(xLocalPickPoint,pFirstPlate->ucs,FALSE,TRUE);
			}
			else
				xLocalPickPoint=pPickVertex->vertex;
			if (iMenu==0)		//��׽ԭ��
			{
				console.m_curWCS.origin = xLocalPickPoint;

				pState->m_fLocalWcsOrgX=xLocalPickPoint.x;
				pState->m_fLocalWcsOrgY=xLocalPickPoint.y;
			}
			else if (iMenu==1)	//��׽X����궨��
			{	//��׽������
				console.m_curWCS.axis_x = xLocalPickPoint-console.m_curWCS.origin;
				normalize(console.m_curWCS.axis_x);
				console.m_curWCS.axis_z.Set(0,0,1);
				console.m_curWCS.axis_y = console.m_curWCS.axis_z^console.m_curWCS.axis_x;

				pState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
				pState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
				pState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
				pState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
			}
			else if (iMenu==2)	//��׽������˨
			{
				console.m_curWCS.axis_y = xLocalPickPoint-console.m_curWCS.origin;
				normalize(console.m_curWCS.axis_y);
				console.m_curWCS.axis_z.Set(0,0,1);
				console.m_curWCS.axis_x = console.m_curWCS.axis_y^console.m_curWCS.axis_z;

				pState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
				pState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
				pState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
				pState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
			}
			//�ڸְ�ʵ������ʾ��������ϵ
			UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
			g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
			g_pSolidDraw->Draw();
			//������������ʾ
			if (iMenu==0)
			{
				oper.UpdatePropItemValue("wcs.origin");
				oper.UpdatePropItemValue("wcs.origin.x");
				oper.UpdatePropItemValue("wcs.origin.y");
			}
			else
			{
				oper.UpdatePropItemValue("wcs.axis_x");
				oper.UpdatePropItemValue("wcs.axis_x.a");
				oper.UpdatePropItemValue("wcs.axis_y");
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("InsertProfileVertex"))
		{		//�����������
			return pView->InsertPlankVertex();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("AddNewHole"))
		{	//�����˨
			return pView->InsertPlankBolt();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("FilletVertex"))
		{	//�н��л�
			return pView->FilletPlateCorner();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("DeleteFeature"))
		{	//ɾ���ְ���������
			return pView->DeletePlankFeature();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("sPartNo"))
		{
			CModifyRelativeObjPartNoDlg modify_partno_dlg;
			modify_partno_dlg.m_pWorkPart=pFirstPlate;
			if (modify_partno_dlg.DoModal()==IDOK)
			{
				pFirstPlate->SetPartNo(modify_partno_dlg.m_sCurObjPartNo.GetBuffer(0));
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
				SEGI iSeg;
				if (CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(modify_partno_dlg.m_sCurObjPartNo,&iSeg,NULL))
				{
					pFirstPlate->iSeg=iSeg;
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("iSeg"),iSeg.ToString());
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs"))
		{
			CPartUcsDlg part_ucs_dlg;
			part_ucs_dlg.ucs=pFirstPlate->ucs;
			if (part_ucs_dlg.DoModal()==IDOK)
			{
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					pPlate->ucs=part_ucs_dlg.ucs;
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.origin"))
		{	//�ڴ˴���ʾ�ְ嶨λ��׼�� wht 11-01-24
			if (pFirstPlate->designInfo.origin.datum_pos_style==10)
			{
				/*CDatumPointPropertyDlg desPointDlg;
				desPointDlg.m_pPoint=&pFirstPlate->designInfo.origin;
				desPointDlg.DoModal();
				pFirstPlate->ucs.origin=pFirstPlate->designInfo.origin.UpdatePos(&Ta);
				pFirstPlate->DesignSetupUcs();
				CLsRef *pLsRef=NULL;
				for(pLsRef=pFirstPlate->GetFirstLsRef();pLsRef;pLsRef=pFirstPlate->GetNextLsRef())
				{	//���¼�����˨λ��
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}*/
#ifdef AFX_TARG_ENU_ENGLISH
				vecDlg.m_sCaption = "Assembly CS origin";
#else 
				vecDlg.m_sCaption = "װ������ϵԭ��λ��";
#endif
				vecDlg.m_fVectorX = pFirstPlate->ucs.origin.x;
				vecDlg.m_fVectorY = pFirstPlate->ucs.origin.y;
				vecDlg.m_fVectorZ = pFirstPlate->ucs.origin.z;
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.origin.x"),&pParentItem);
				vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.origin.y"),&pParentItem);
				vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.origin.z"),&pParentItem);
				vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
				if (vecDlg.DoModal()==IDOK)
				{
					CXhChar100 sText;
					sText.Printf("%f",vecDlg.m_fVectorX);
					SimplifiedNumString(sText);
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.origin.x"),CString(sText));
					sText.Printf("%f",vecDlg.m_fVectorY);
					SimplifiedNumString(sText);
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.origin.y"),CString(sText));
					sText.Printf("%f",vecDlg.m_fVectorZ);
					SimplifiedNumString(sText);
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.origin.z"),CString(sText));
					for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
						pPlate->ucs.origin.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				}
			}
			else
			{
				/*CDatumPointDlg des_point_dlg;
				des_point_dlg.m_pPoint=&pFirstPlate->designInfo.origin;
				des_point_dlg.m_shDatumPoint.Format("���:��");
				des_point_dlg.m_sPointDescription = "���Ӱ嶨λ����";
				if(des_point_dlg.DoModal()==IDOK)
				pFirstPlate->designInfo.origin=*des_point_dlg.m_pPoint;*/
				//
				CDatumPointPropertyDlg desPointDlg;
				desPointDlg.m_pCurEditObj=pFirstPlate;
				desPointDlg.m_xPoint=pFirstPlate->designInfo.origin;
				if (desPointDlg.m_xPoint.datum_pos_style==0)
					desPointDlg.m_xPoint.SetPosition(pFirstPlate->ucs.origin);
				if (desPointDlg.DoModal()==IDOK)
				{
					pFirstPlate->designInfo.origin=desPointDlg.m_xPoint;
					pFirstPlate->designInfo.origin.UpdatePos(&Ta);
					pFirstPlate->ucs.origin=pFirstPlate->designInfo.origin.Position();
					CUndoOperObject undo(&Ta);
					pFirstPlate->DesignPlate();
					pFirstPlate->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pFirstPlate->GetSolidPartObject());
				}
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_x"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "X Axis Direction";
#else 
			vecDlg.m_sCaption = "װ��ʱX�᷽��";
#endif
			vecDlg.m_fVectorX = pFirstPlate->ucs.axis_x.x;
			vecDlg.m_fVectorY = pFirstPlate->ucs.axis_x.y;
			vecDlg.m_fVectorZ = pFirstPlate->ucs.axis_x.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.axis_x.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.axis_x.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.axis_x.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.axis_x.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.axis_x.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.axis_x.z"),CString(sText));
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->ucs.axis_x.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_x*1,0);
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_y"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Y Axis Direction";
#else 
			vecDlg.m_sCaption = "װ��ʱY�᷽��";
#endif
			vecDlg.m_fVectorX = pFirstPlate->ucs.axis_y.x;
			vecDlg.m_fVectorY = pFirstPlate->ucs.axis_y.y;
			vecDlg.m_fVectorZ = pFirstPlate->ucs.axis_y.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.axis_y.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.axis_y.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.axis_y.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.axis_y.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.axis_y.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.axis_y.z"),CString(sText));
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->ucs.axis_y.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_y*1,1);
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_z"))
		{	//�ְ巨�߶�λ��ʽ wht 11-01-24
			CDesignVectorPropDlg vector_dlg;
			vector_dlg.m_hCurEditPart=pFirstPlate->handle;
			if (vector_dlg.m_xVector.norm_style==0)
				pFirstPlate->designInfo.norm.vector=pFirstPlate->ucs.axis_z;
			vector_dlg.m_xVector = pFirstPlate->designInfo.norm;
			//Ϊ����������巨������������һ�²�����ʮ�ֲ�帽�Ӱ��ʼ��ʱ��Ҫ���⴦�� wht 11-05-25
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pFirstPlate->m_hPartWeldParent,CLS_PARAMPLATE);
			if (pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
			{	//�ְ常����Ϊʮ�ֲ��
				vector_dlg.m_hCurEditPart=pFirstPlate->handle;
				//1.��ǰ��λ��ʽδʹ�ý��Ʒ��߱�����ʹ�øְ巽���ʼ�����Ʒ���
				if (vector_dlg.m_xVector.norm_style!=2&&vector_dlg.m_xVector.norm_style!=4)
					vector_dlg.m_xVector.nearVector=pFirstPlate->ucs.axis_z;
				//2.��ǰ��λ��ʽδʹ�ý���˼�������ʹ�ò�����ڸ˼���ʼ������˼����
				if (vector_dlg.m_xVector.norm_style!=2)
					vector_dlg.m_xVector.hCrossPart=pParamPlate->m_hPartWeldParent;
			}
			//3.��ʾ����ϵ
			UCS_STRU object_ucs,ucs = pFirstPlate->ucs;
			g_pSolidSet->GetObjectUcs(object_ucs);
			coord_trans(ucs.origin,object_ucs,TRUE);
			vector_trans(ucs.axis_x,object_ucs,TRUE);
			vector_trans(ucs.axis_y,object_ucs,TRUE);
			vector_trans(ucs.axis_z,object_ucs,TRUE);
			g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
			//g_pSolidSet->SetDispWorkUcs();
			g_pSolidDraw->Draw();
			if (vector_dlg.DoModal()==IDOK)
			{
				CUndoOperObject undo(&Ta);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.norm=vector_dlg.m_xVector;
					pPlate->designInfo.norm.UpdateVector(&Ta);
					//����������Ʋ�����������Ƶ�ǰ�ְ�
					pPlate->DesignSetupUcs();
					pPlate->DesignPlate();
					pPlate->SetModified();
					pPlate->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
					//����Z�᷽�����ʾ���� wht 11-05-31
					long idProp=CLDSPlate::GetPropID("ucs.axis_z.x");
					if (pFirstPlate->GetPropValueStr(idProp,sText)>0)
						pPropList->SetItemPropValue(idProp,sText);
					idProp=CLDSPlate::GetPropID("ucs.axis_z.y");
					if (pFirstPlate->GetPropValueStr(idProp,sText)>0)
						pPropList->SetItemPropValue(idProp,sText);
					idProp=CLDSPlate::GetPropID("ucs.axis_z.z");
					if (pFirstPlate->GetPropValueStr(idProp,sText)>0)
						pPropList->SetItemPropValue(idProp,sText);
					if (pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
					{	//������Ʋ�帽�Ӱ����ѵ�ǰ�ְ�Ϊ��λ�����İ���
						ATTACHPART_ITEM *pItem=NULL;
						for (pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
						{
							CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pItem->h,CLS_PLATE);
							if (pPlate==NULL)
								continue;
							if (pPlate->designInfo.norm.hVicePart==pFirstPlate->handle
								&&pPlate->designInfo.norm.norm_style>0)
							{
								pPlate->DesignSetupUcs();
								pPlate->DesignPlate();
								pPlate->SetModified();
								pPlate->Create3dSolidModel();
								g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
							}
						}
						//������Ʋ�����ʱ���丽������˨�������
						for (CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
						{
							CLDSBolt *pBolt=pLsRef->GetLsPtr();
							pBolt->correct_worknorm();
							pBolt->correct_pos();
							pBolt->CalGuigeAuto();
							pBolt->SetModified();
							pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
							g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
						}
					}
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
			//
			//g_pSolidSet->SetDispWorkUcs(FALSE);
			g_pSolidDraw->BatchClearCS(2);
			g_pSolidDraw->Draw();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("plate_work_plane"))
		{
			CPartWorkPlaneDlg planeDlg;
			planeDlg.m_pDatumPart=pFirstPlate;
			planeDlg.DoModal();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerColumnNorm"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Plate Inner Circle Normal";
#else 
			vecDlg.m_sCaption = "�ְ��ڻ����߷���";
#endif
			vecDlg.m_fVectorX = pFirstPlate->m_tInnerColumnNorm.x;
			vecDlg.m_fVectorY = pFirstPlate->m_tInnerColumnNorm.y;
			vecDlg.m_fVectorZ = pFirstPlate->m_tInnerColumnNorm.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_tInnerColumnNorm.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_tInnerColumnNorm.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_tInnerColumnNorm.z"),CString(sText));
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					pPlate->m_tInnerColumnNorm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerOrigin"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Inner Circle Position";
#else 
			vecDlg.m_sCaption = "�ְ��ڻ�λ��";
#endif
			vecDlg.m_fVectorX = pFirstPlate->m_tInnerOrigin.x;
			vecDlg.m_fVectorY = pFirstPlate->m_tInnerOrigin.y;
			vecDlg.m_fVectorZ = pFirstPlate->m_tInnerOrigin.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_tInnerOrigin.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_tInnerOrigin.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_tInnerOrigin.z"),CString(sText));
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					pPlate->m_tInnerOrigin.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_fInnerRadius"))
		{
			valueStr.Format("%lf",pFirstPlate->m_fInnerRadius);
			SimplifiedNumString(valueStr);
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the inner circle's radius",&valueStr,'s');
#else 
			Input("�����ڻ��뾶",&valueStr,'s');
#endif
			CString sOffset;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pFirstPlate->m_fInnerRadius=atof(valueStr);
			pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_fInnerRadius"),valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoquline_start[0]"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstPlate;
			GEPOINT pos=pFirstPlate->HuoQuLine[0].startPt;
			coord_trans(pos,pFirstPlate->ucs,TRUE,TRUE);
			pFirstPlate->designInfo.huoquline_start[0].SetPosition(pos);
			datumPointDlg.m_xPoint = pFirstPlate->designInfo.huoquline_start[0];
			if (IDOK==datumPointDlg.DoModal())
			{
				CXhChar100 sText;
				CLDSPoint datumPoint=datumPointDlg.m_xPoint;
				sText.Printf("%f",datumPoint.Position().x);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().y);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().z);
				SimplifiedNumString(sText);
				datumPoint.UpdatePos(&Ta);
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.huoquline_start[0]=datumPoint;
					pPlate->HuoQuLine[0].startPt=datumPoint.Position();
					coord_trans(pPlate->HuoQuLine[0].startPt,pPlate->ucs,FALSE,TRUE);
					pPlate->HuoQuLine[0].startPt.z=0;
					pPlate->DesignPlate();
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoquline_end[0]"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstPlate;
			GEPOINT pos=pFirstPlate->HuoQuLine[0].endPt;
			coord_trans(pos,pFirstPlate->ucs,TRUE,TRUE);
			pFirstPlate->designInfo.huoquline_end[0].SetPosition(pos);
			datumPointDlg.m_xPoint = pFirstPlate->designInfo.huoquline_end[0];
			if (IDOK==datumPointDlg.DoModal())
			{
				CXhChar100 sText;
				CLDSPoint datumPoint=datumPointDlg.m_xPoint;
				sText.Printf("%f",datumPoint.Position().x);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().y);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().z);
				SimplifiedNumString(sText);
				datumPoint.UpdatePos(&Ta);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.huoquline_end[0]=datumPoint;
					pPlate->HuoQuLine[0].endPt=datumPoint.Position();
					coord_trans(pPlate->HuoQuLine[0].endPt,pPlate->ucs,FALSE,TRUE);
					pPlate->HuoQuLine[0].endPt.z=0;
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].vector"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Plane Normal";
#else 
			vecDlg.m_sCaption = "��������";
#endif
			vecDlg.m_fVectorX = pFirstPlate->huoqufacenorm[0].x;	//designInfo.huoqufacenorm[0].vector.x;
			vecDlg.m_fVectorY = pFirstPlate->huoqufacenorm[0].y;	//designInfo.huoqufacenorm[0].vector.y;
			vecDlg.m_fVectorZ = pFirstPlate->huoqufacenorm[0].z;	//designInfo.huoqufacenorm[0].vector.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0].vector.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("huoqufacenorm[0].vector.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("huoqufacenorm[0].vector.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("huoqufacenorm[0].vector.z"),CString(sText));
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{	//���������޸Ļ����淨�ߴ��� wjh/2009-4-16
					pPlate->designInfo.huoqufacenorm[0].vector.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					pPlate->huoqufacenorm[0].Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					pPlate->DesignPlate();
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoquline_start[1]"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstPlate;
			GEPOINT pos=pFirstPlate->HuoQuLine[1].startPt;
			coord_trans(pos,pFirstPlate->ucs,TRUE,TRUE);
			pFirstPlate->designInfo.huoquline_start[1].SetPosition(pos);
			datumPointDlg.m_xPoint = pFirstPlate->designInfo.huoquline_start[1];
			if (IDOK==datumPointDlg.DoModal())
			{
				CXhChar100 sText;
				CLDSPoint datumPoint=datumPointDlg.m_xPoint;
				sText.Printf("%f",datumPoint.Position().x);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().y);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().z);
				SimplifiedNumString(sText);
				datumPoint.UpdatePos(&Ta);
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.huoquline_start[1]=datumPoint;
					pPlate->HuoQuLine[1].startPt=datumPoint.Position();
					coord_trans(pPlate->HuoQuLine[1].startPt,pPlate->ucs,FALSE,TRUE);
					pPlate->HuoQuLine[1].startPt.z=0;
					pPlate->DesignPlate();
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoquline_end[1]"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstPlate;
			GEPOINT pos=pFirstPlate->HuoQuLine[1].endPt;
			coord_trans(pos,pFirstPlate->ucs,TRUE,TRUE);
			pFirstPlate->designInfo.huoquline_end[1].SetPosition(pos);
			datumPointDlg.m_xPoint = pFirstPlate->designInfo.huoquline_end[1];
			if (IDOK==datumPointDlg.DoModal())
			{
				CXhChar100 sText;
				CLDSPoint datumPoint=datumPointDlg.m_xPoint;
				sText.Printf("%f",datumPoint.Position().x);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().y);
				SimplifiedNumString(sText);
				sText.Printf("%f",datumPoint.Position().z);
				SimplifiedNumString(sText);
				datumPoint.UpdatePos(&Ta);
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.huoquline_end[1]=datumPoint;
					pPlate->HuoQuLine[1].endPt=datumPoint.Position();
					coord_trans(pPlate->HuoQuLine[1].endPt,pPlate->ucs,FALSE,TRUE);
					pPlate->HuoQuLine[1].endPt.z=0;
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].vector"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Plane Normal";
#else 
			vecDlg.m_sCaption = "��������";
#endif
			vecDlg.m_fVectorX = pFirstPlate->huoqufacenorm[1].x;	//designInfo.huoqufacenorm[1].vector.x;
			vecDlg.m_fVectorY = pFirstPlate->huoqufacenorm[1].y;	//designInfo.huoqufacenorm[1].vector.y;
			vecDlg.m_fVectorZ = pFirstPlate->huoqufacenorm[1].z;	//designInfo.huoqufacenorm[1].vector.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"),CString(sText));
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{	//���������޸Ļ����淨�ߴ��� wjh/2009-4-16
					pPlate->designInfo.huoqufacenorm[1].vector.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					pPlate->huoqufacenorm[1].Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					pPlate->SetModified();
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0]"))
		{	//�����淨�����
			CDesignVectorPropDlg vector_dlg;
			vector_dlg.m_hCurEditPart=pFirstPlate->handle;
			if (pFirstPlate->designInfo.huoqufacenorm[0].norm_style==0
				&&pFirstPlate->designInfo.huoqufacenorm[0].vector.IsZero())
				pFirstPlate->designInfo.huoqufacenorm[0].vector=pFirstPlate->huoqufacenorm[0];
			vector_dlg.m_xVector=pFirstPlate->designInfo.huoqufacenorm[0];
			if (vector_dlg.DoModal()==IDOK)
			{
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.huoqufacenorm[0]=vector_dlg.m_xVector;
					pPlate->designInfo.huoqufacenorm[0].UpdateVector(&Ta);
					//���¸ְ�����淨�� wht 15-09-08
					pPlate->huoqufacenorm[0]=pPlate->designInfo.huoqufacenorm[0].vector;
					pPlate->SetModified();
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1]"))
		{	//�����淨�����
			CDesignVectorPropDlg vector_dlg;
			vector_dlg.m_hCurEditPart=pFirstPlate->handle;
			if (pFirstPlate->designInfo.huoqufacenorm[1].norm_style==0
				&&pFirstPlate->designInfo.huoqufacenorm[1].vector.IsZero())
				pFirstPlate->designInfo.huoqufacenorm[1].vector=pFirstPlate->huoqufacenorm[1];
			vector_dlg.m_xVector=pFirstPlate->designInfo.huoqufacenorm[1];
			if (vector_dlg.DoModal()==IDOK)
			{
				CUndoOperObject undo(&Ta,true);
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pPlate->designInfo.huoqufacenorm[1]=vector_dlg.m_xVector;
					pPlate->designInfo.huoqufacenorm[1].UpdateVector(&Ta);
					//���¸ְ�����淨�� wht 15-09-08
					pPlate->huoqufacenorm[1]=pPlate->designInfo.huoqufacenorm[1].vector;
					pPlate->SetModified();
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("basicFace"))
		{	//��׼�涨��
			CEditPlateDatumFaceDlg datum_face_dlg;
			if (pFirstPlate->designInfo.origin.datum_pos_style==0
				&&pFirstPlate->designInfo.origin.Position().IsZero())
				pFirstPlate->designInfo.origin.SetPosition(pFirstPlate->ucs.origin);
			else
				pFirstPlate->designInfo.origin.UpdatePos(&Ta);
			if (pFirstPlate->designInfo.norm.norm_style==0
				&&pFirstPlate->designInfo.norm.vector.IsZero())
				pFirstPlate->designInfo.norm.vector=pFirstPlate->ucs.axis_z;
			else
				pFirstPlate->designInfo.norm.UpdateVector(&Ta);
			datum_face_dlg.m_pLjPara=&pFirstPlate->designInfo;
			datum_face_dlg.DoModal();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoquFace1"))
		{	//��һ�����涨��	
			CDefHuoquFaceDlg huoqu_face_dlg;
			pFirstPlate->designInfo.origin.UpdatePos(&Ta);
			//���»����߷�����Ʋ���
			if (pFirstPlate->designInfo.huoqufacenorm[0].norm_style==0
				&&pFirstPlate->designInfo.huoqufacenorm[0].vector.IsZero())
				pFirstPlate->designInfo.huoqufacenorm[0].vector=pFirstPlate->huoqufacenorm[0];
			//���»������յ���Ʋ���
			if (pFirstPlate->designInfo.huoquline_start[0].datum_pos_style==0
				&&pFirstPlate->designInfo.huoquline_start[0].Position().IsZero())
				pFirstPlate->designInfo.huoquline_start[0].SetPosition(pFirstPlate->HuoQuLine[0].startPt);
			else
				pFirstPlate->designInfo.huoquline_start[0].UpdatePos(&Ta);
			//���»������յ���Ʋ���
			if (pFirstPlate->designInfo.huoquline_end[0].datum_pos_style==0
				&&pFirstPlate->designInfo.huoquline_end[0].Position().IsZero())
				pFirstPlate->designInfo.huoquline_end[0].SetPosition(pFirstPlate->HuoQuLine[0].endPt);
			else
				pFirstPlate->designInfo.huoquline_end[0].UpdatePos(&Ta);
			huoqu_face_dlg.m_pLjPara=&pFirstPlate->designInfo;
			huoqu_face_dlg.m_iHuoquFaceType=0;
			if (huoqu_face_dlg.DoModal()==IDOK)	//���¸ְ�����淨�� wjh 15-09-09
				pFirstPlate->huoqufacenorm[0].Set(huoqu_face_dlg.m_fHuoquNormX,huoqu_face_dlg.m_fHuoquNormY,huoqu_face_dlg.m_fHuoquNormZ);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoquFace2"))
		{	//�ڶ������涨��	
			CDefHuoquFaceDlg huoqu_face_dlg;
			pFirstPlate->designInfo.origin.UpdatePos(&Ta);
			//���»����߷�����Ʋ���
			if (pFirstPlate->designInfo.huoqufacenorm[1].norm_style==0
				&&pFirstPlate->designInfo.huoqufacenorm[1].vector.IsZero())
				pFirstPlate->designInfo.huoqufacenorm[1].vector=pFirstPlate->huoqufacenorm[1];
			//���»������յ���Ʋ���
			if (pFirstPlate->designInfo.huoquline_start[1].datum_pos_style==0
				&&pFirstPlate->designInfo.huoquline_start[1].Position().IsZero())
				pFirstPlate->designInfo.huoquline_start[1].SetPosition(pFirstPlate->HuoQuLine[1].startPt);
			else
				pFirstPlate->designInfo.huoquline_start[1].UpdatePos(&Ta);
			//���»������յ���Ʋ���
			if (pFirstPlate->designInfo.huoquline_end[1].datum_pos_style==0
				&&pFirstPlate->designInfo.huoquline_end[1].Position().IsZero())
				pFirstPlate->designInfo.huoquline_end[1].SetPosition(pFirstPlate->HuoQuLine[1].endPt);
			else
				pFirstPlate->designInfo.huoquline_end[1].UpdatePos(&Ta);
			huoqu_face_dlg.m_pLjPara=&pFirstPlate->designInfo;
			huoqu_face_dlg.m_iHuoquFaceType=1;
			if (huoqu_face_dlg.DoModal()==IDOK)	//���¸ְ�����淨�� wjh 15-09-09
				pFirstPlate->huoqufacenorm[1].Set(huoqu_face_dlg.m_fHuoquNormX,huoqu_face_dlg.m_fHuoquNormY,huoqu_face_dlg.m_fHuoquNormZ);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("plateParam"))
		{	//�ְ����
			CDesignLjParaDlg ljDlg;
			if (pFirstPlate->jdb_style==4)	//��˨���
				pFirstPlate->designInfo.iFaceType=5;
			pFirstPlate->designInfo.origin.SetPosition(pFirstPlate->ucs.origin);
			pFirstPlate->designInfo.norm.vector=pFirstPlate->ucs.axis_z;
			ljDlg.m_pPlate = pFirstPlate;
			ljDlg.DoModal();
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("plateLinkPoles"))
		{	//�鿴���Ӹ˼���
			CLinkLinePartDlg linkdlg;
			linkdlg.m_hPlate=pFirstPlate->handle;
			linkdlg.m_pLjPara = &pFirstPlate->designInfo;
			linkdlg.DoModal();
			CUndoOperObject undo(&Ta,true);
			pFirstPlate->DesignPlate();
			g_pSolidDraw->NewSolidPart(pFirstPlate->GetSolidPartObject());
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("relativeObjs"))
		{
			CRelativeObjectDlg relativeDlg;
			relativeDlg.m_pWorkObj = pFirstPlate;
			if (relativeDlg.DoModal()==IDOK)
			{
				CXhChar200 sText;
				pFirstPlate->GetPropValueStr(pItem->m_idProp,sText);
				pPropList->SetItemPropValue(pItem->m_idProp,sText);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("shadowInfo"))
		{
			CDesignShadowDlg shadowdlg;
			shadowdlg.SetShadowObj((CLDSObject*)pFirstPlate);
			shadowdlg.DoModal();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_hPartWeldParent"))
		{
			//ͨ��ѡ�񹹼��޸ĺ��Ӹ����������ڽ������޸ĵ��������ĺ��Ӹ�����
			CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Weld Parent Part";
			modifyhdlg.m_sTitle="Weld Parent Part:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->m_hPartWeldParent);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
			modifyhdlg.SetCmdPickPromptStr("Please select weld parent part:");
#else 
			modifyhdlg.m_sCaption="�޸ĸְ庸�Ӹ�����";
			modifyhdlg.m_sTitle="���Ӹ�����:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->m_hPartWeldParent);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
			modifyhdlg.SetCmdPickPromptStr("��ѡ�񺸽Ӹ�����:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			if (IDOK==modifyhdlg.DoModal())
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if (pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"The input handle is wrong,so can't find the corresponding part��",sSoftName,MB_OK);
#else 
					MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSPlate::GetPropID("m_hPartWeldParent"),valueStr);
					g_pSolidDraw->ReleaseSnapStatus();
					MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
					for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					{
						int flagArr[3]={ 1,2,4 };
						CLDSPlate* pMirPlate=NULL;
						for (int i=0;i<3;i++)
						{
							if ((mirmsg.axis_flag&flagArr[i])==0)
								continue;
							MIRMSG msg=mirmsg.GetSubMirItem(i+1);
							//���Ҹְ�ĶԳƹ���
							pMirPlate=(CLDSPlate*)pPlate->GetMirPart(msg);
							if (pMirPlate)
							{
								CLDSPart *pMirPart=pPart->GetMirPart(msg);
								if (pMirPart)	//������ѡ�������ĶԳƹ��� wht 11-05-07
									pMirPlate->m_hPartWeldParent=pMirPart->handle;
							}
						}
						pPlate->m_hPartWeldParent=pPart->handle;
						g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
					}
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart")||
			pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart"))
		{
			//ͨ��ѡ�񹹼��޸Ķ�λ��׼����,һ�㲻��Ҫ�Գƹ��ܹ�֧��ֱ��ѡ�񹹼� wht 11-04-27
			//�ظ�ִ��GetPartsFromScr���ᵼ�¶���߳�ͬʱִ�е�����ͬʱ��ֹ������һ��selectObjs�ҵ����� wjh-2011.7.19
			CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify normal of the bending plane";
			modifyhdlg.m_sTitle=" Normal of the bending plane:";
#else
			modifyhdlg.m_sCaption="�޸Ļ����淨�߻�׼����";
			modifyhdlg.m_sTitle="�����淨�߻�׼����:";
#endif
			if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"))
				modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->designInfo.huoqufacenorm[0].hVicePart);
			else
				modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->designInfo.huoqufacenorm[1].hVicePart);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Select part to determine normal��");
#else
			modifyhdlg.SetCmdPickPromptStr("ѡ���߻�׼����:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			modifyhdlg.m_iMirMsg=0;
			if (IDOK==modifyhdlg.DoModal())
			{
				CLDSDbObject *pObj=console.FromPartHandle(modifyhdlg.m_nHandle);
				if (pObj&&pObj->IsPart())
				{
					pPropList->SetItemPropValue(pItem->m_idProp,"0X%X",pObj->handle);
					g_pSolidDraw->ReleaseSnapStatus();
					for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					{
						if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"))
							pPlate->designInfo.huoqufacenorm[0].hVicePart=pObj->handle;
						else
							pPlate->designInfo.huoqufacenorm[1].hVicePart=pObj->handle;
						g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
					}
					//ѡ���׼�����󣬸��ݹ������͸��·������������ݡ��������ݼ�������ʾ���� wht 12-09-26
					long idProp=0;
					CLDSVector *pVector=NULL;
					if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"))
					{
						idProp=CLDSPlate::GetPropID("huoqufacenorm[0].direction");
						pVector=&(pFirstPlate->designInfo.huoqufacenorm[0]);
					}
					else
					{
						idProp=CLDSPlate::GetPropID("huoqufacenorm[1].direction");
						pVector=&(pFirstPlate->designInfo.huoqufacenorm[1]);
					}
					pFindItem=pPropList->FindItemByPropId(idProp,NULL);
					if (pFindItem)
					{
						pFindItem->m_lpNodeInfo->m_cmbItems=pVector->GetDirectionCmbItems();
						pFindItem->m_lpNodeInfo->m_strPropHelp=pVector->GetDirectionHelpStr();
						CXhChar100 sDirection;
						if (pFirstPlate->GetPropValueStr(idProp,sDirection)>0)
							pFindItem->m_lpNodeInfo->m_strPropValue=sDirection;
					}
				}
				pPropList->SetCurSel(pItem->m_iIndex);
				pPropList->SetFocus();
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart")||
			pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"))
		{
			//ͨ��ѡ�񹹼��޸Ķ�λ��׼����,һ�㲻��Ҫ�Գƹ��ܹ�֧��ֱ��ѡ�񹹼� wht 11-04-27
			//�ظ�ִ��GetPartsFromScr���ᵼ�¶���߳�ͬʱִ�е�����ͬʱ��ֹ������һ��selectObjs�ҵ����� wjh-2011.7.19
			CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify normal of the bending plane";
			modifyhdlg.m_sTitle=" Normal of the bending plane:";
#else
			modifyhdlg.m_sCaption="�޸Ļ����淨�߻�׼����";
			modifyhdlg.m_sTitle="�����淨�߻�׼����:";
#endif
			if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"))
				modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->designInfo.huoqufacenorm[0].hCrossPart);
			else
				modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->designInfo.huoqufacenorm[1].hCrossPart);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Select part to determine normal��");
#else
			modifyhdlg.SetCmdPickPromptStr("ѡ���߻�׼����:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			modifyhdlg.m_iMirMsg=0;
			if (IDOK==modifyhdlg.DoModal())
			{
				CLDSDbObject *pObj=console.FromPartHandle(modifyhdlg.m_nHandle);
				if (pObj&&pObj->IsPart())
				{
					pPropList->SetItemPropValue(pItem->m_idProp,"0X%X",pObj->handle);
					g_pSolidDraw->ReleaseSnapStatus();
					for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
					{
						if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"))
							pPlate->designInfo.huoqufacenorm[0].hCrossPart=pObj->handle;
						else
							pPlate->designInfo.huoqufacenorm[1].hCrossPart=pObj->handle;
						g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
					}
					//ѡ���׼�����󣬸��ݹ������͸��·������������ݡ��������ݼ�������ʾ���� wht 12-09-26
					long idProp=0;
					CLDSVector *pVector=NULL;
					if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"))
					{
						idProp=CLDSPlate::GetPropID("huoqufacenorm[0].direction");
						pVector=&(pFirstPlate->designInfo.huoqufacenorm[0]);
					}
					else
					{
						idProp=CLDSPlate::GetPropID("huoqufacenorm[1].direction");
						pVector=&(pFirstPlate->designInfo.huoqufacenorm[1]);
					}
					pFindItem=pPropList->FindItemByPropId(idProp,NULL);
					if (pFindItem)
					{
						pFindItem->m_lpNodeInfo->m_cmbItems=pVector->GetDirectionCmbItems();
						pFindItem->m_lpNodeInfo->m_strPropHelp=pVector->GetDirectionHelpStr();
						CXhChar100 sDirection;
						if (pFirstPlate->GetPropValueStr(idProp,sDirection)>0)
							pFindItem->m_lpNodeInfo->m_strPropValue=sDirection;
					}
				}
				pPropList->SetCurSel(pItem->m_iIndex);
				pPropList->SetFocus();
			}
		}

		for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
		{
			pPlate->SetModified();
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		}
		g_pSolidDraw->Draw();
	}
	catch (char *sError)
	{
		AfxMessageBox(sError);
	}
	return bRetCode;
}
BOOL RedrawPlateParamDrawing(CLDSGeneralPlate* pPlate,const char *sPropKey,UCS_STRU *pObjectCS=NULL,bool removeOldEntities=true);
BOOL FirePlateHelpStr(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPlate *pCurPlate = (CLDSPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL||pItem==NULL)
		return FALSE;
	long *id_arr=NULL;
	long n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if (pItem->m_idProp==CLDSPlate::GetPropID("m_hPartWeldParent"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurPlate->m_hPartWeldParent);
	}
	else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurPlate->designInfo.huoqufacenorm[0].hVicePart);
	}
	else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurPlate->designInfo.huoqufacenorm[0].hCrossPart);
	}
	else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurPlate->designInfo.huoqufacenorm[1].hVicePart);
	}
	else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurPlate->designInfo.huoqufacenorm[1].hCrossPart);
	}
	else if (n==1&&id_arr[0]!=pCurPlate->handle)
	{	//��һ��ѡ�е��Ǻ��Ӹ�����
		g_pSolidDraw->ReleaseSnapStatus();
		for (CLDSObject* pObj=pView->selectObjs.GetFirst();pObj;pObj=pView->selectObjs.GetNext())
			g_pSolidDraw->SetEntSnapStatus(pObj->handle);
	}
	else if (pItem->m_idProp==CLDSPlate::GetPropID("peakVertexOffsetY"))
		RedrawPlateParamDrawing(pCurPlate,"peakVertexOffsetY");
	else if (pItem->m_idProp==CLDSPlate::GetPropID("extendVertexOffsetLen"))
		RedrawPlateParamDrawing(pCurPlate,"extendVertexOffsetLen");
	else
		return FALSE;

	return TRUE;
}
long RetrieveBlockHandle(const char* blockIdName);
BOOL ModifyPlateProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPlate *pFirstPlate = (CLDSPlate*)pView->selectObjs.GetFirst();
	CPropTreeItem *pFindItem,*pParentItem,*pPropItem;
	CLDSPlate *pPlate = NULL;
	CItemInfo *lpInfo;
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	int iTemp;
	if (pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	try {
		BOOL bUpdateWcs=FALSE;
		CPropListOper<CLDSPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&pView->selectObjs);
		_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
		if (pItem->m_idProp==CLDSPlate::GetPropID("layer"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->SetLayer(tem_str);
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					if (pMirObj->mirInfo.axis_flag&8)
						continue;	//��ת�Գ�
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						CalMirAtomLayer(pPlate->layer(),pMirPlate->layer(),pMirObj->mirInfo);
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("cMaterial"))
		{	//�ְ����
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->cMaterial = QuerySteelBriefMatMark(tem_str);
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->cMaterial=pPlate->cMaterial;
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_cQualityLevel"))
		{	//�ְ������ȼ�
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Default")==0)
					pPlate->m_cQualityLevel=0;
				else
#else
				if (valueStr.CompareNoCase("Ĭ��")==0)
					pPlate->m_cQualityLevel=0;
				else
#endif
					pPlate->m_cQualityLevel = valueStr[0];
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->m_cQualityLevel=pPlate->m_cQualityLevel;
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.hNode"))
		{
			pFirstPlate->designInfo.m_hBaseNode=HexStrToLong(valueStr);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.m_hBaseNode=pFirstPlate->designInfo.m_hBaseNode;
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.hRod"))
		{
			pFirstPlate->designInfo.m_hBasePart=HexStrToLong(valueStr);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.m_hBasePart=pFirstPlate->designInfo.m_hBasePart;
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_hBlock"))
		{
			long hBlock=RetrieveBlockHandle(valueStr);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_hBlock=hBlock;
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("thick"))
		{	//�ְ���
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->Thick=atoi(tem_str);
				pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->Thick=ftoi(pPlate->GetThick());
				}
			}
			g_pSolidDraw->Draw();
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("iSeg"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->iSeg = SEGI(valueStr);
				pPlate->SyncMirProp("iSeg");
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("sPartNo"))
		{	//���ݼ�����ȡ�κţ������¸ְ�κ�
			bool parsed=false;
			SEGI iSeg;
			if (CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(tem_str,&iSeg,NULL))
			{
				parsed=true;
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("iSeg"),iSeg.ToString());
			}
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				if (parsed)
					pPlate->iSeg=iSeg;
				pPlate->SetPartNo(tem_str);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("cfgword"))
		{	//�ø��¹��ĵ�һ�������е���ĺŸ�����������������Ϻ�
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->cfgword = pFirstPlate->cfgword;
				pPlate->SetModified(TRUE,FALSE);
				pPlate->SyncMirProp("cfgword");
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_uStatMatCoef"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_uStatMatCoef = atoi(tem_str);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_hPartWeldParent"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_hPartWeldParent=HexStrToLong(tem_str);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bDisplayHole"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
					pPlate->m_bDisplayHole = TRUE;
				else if (valueStr.CompareNoCase("No")==0)
#else 
				if (valueStr.CompareNoCase("��")==0)
					pPlate->m_bDisplayHole = TRUE;
				else if (valueStr.CompareNoCase("��")==0)
#endif
					pPlate->m_bDisplayHole = FALSE;
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->m_bDisplayHole=pPlate->m_bDisplayHole;
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_cPickedDisplayMode"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("None")==0)
					pPlate->m_cPickedDisplayMode = 0;
				else if (valueStr.CompareNoCase("Actual Profile")==0)
					pPlate->m_cPickedDisplayMode = 1;
				else if (valueStr.CompareNoCase("Spread Profile")==0)
					pPlate->m_cPickedDisplayMode = 2;
#else 
				pPlate->m_cPickedDisplayMode=valueStr[0]-'0';
#endif
				IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
				if (pPlate->m_cPickedDisplayMode)
				{
					CLDSView::RedrawPlateProfileDrawing(pPlate);
					g_pSolidDraw->Draw();
					return TRUE;
				}
				else if (pBuddyDrawing)
					pBuddyDrawing->DeleteDbEntityByHiberMasterId(pPlate->handle);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bVirtualPart"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
					pPlate->m_bVirtualPart = TRUE;
				else if (valueStr.CompareNoCase("No")==0)
#else 
				if (valueStr.CompareNoCase("��")==0)
					pPlate->m_bVirtualPart = TRUE;
				else if (valueStr.CompareNoCase("��")==0)
#endif
					pPlate->m_bVirtualPart = FALSE;
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bAttachPartByWeldRoad"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
					pPlate->m_bAttachPartByWeldRoad = TRUE;
				else if (valueStr.CompareNoCase("No")==0)
#else 
				if (valueStr.CompareNoCase("��")==0)
					pPlate->m_bAttachPartByWeldRoad = TRUE;
				else if (valueStr.CompareNoCase("��")==0)
#endif
					pPlate->m_bAttachPartByWeldRoad = FALSE;
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.m_bEnableFlexibleDesign"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
					pPlate->designInfo.m_bEnableFlexibleDesign = TRUE;
				else if (valueStr.CompareNoCase("No")==0)
#else 
				if (valueStr.CompareNoCase("��")==0)
					pPlate->designInfo.m_bEnableFlexibleDesign = TRUE;
				else if (valueStr.CompareNoCase("��")==0)
#endif
					pPlate->designInfo.m_bEnableFlexibleDesign = FALSE;
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->designInfo.m_bEnableFlexibleDesign=pPlate->designInfo.m_bEnableFlexibleDesign;
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bUserSpecColor"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
					pPlate->m_bUserSpecColor = TRUE;
				else if (valueStr.CompareNoCase("No")==0)
#else 
				if (valueStr.CompareNoCase("��")==0)
					pPlate->m_bUserSpecColor = TRUE;
				else if (valueStr.CompareNoCase("��")==0)
#endif
					pPlate->m_bUserSpecColor = FALSE;
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->m_bUserSpecColor=pPlate->m_bUserSpecColor;
				}
			}
			if (pFirstPlate->m_bUserSpecColor)
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("crMaterial"),NULL);
				if (pFindItem==NULL)
				{
					CXhChar100 sText;
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
					pFindItem->m_idProp = CLDSPlate::GetPropID("crMaterial");
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
				}
			}
			else
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("crMaterial"));
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("crMaterial"))
		{
			COLORREF crCurSel=0;
			memmove(tem_str,tem_str+3,97);	//����RGB
			sscanf(tem_str,"%X",&crCurSel);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->crMaterial = crCurSel;
				for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
					CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
					if (pMirPlate)
						pMirPlate->crMaterial=pPlate->crMaterial;
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("sNotes"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				_snprintf(pPlate->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
				//��ע���ù����޸� wht 10-10-22
				/*for(RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
				{
				CLDSPlate *pMirPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PLATE);
				if(pMirPlate)
				strcpy(pMirPlate->sNotes,pPlate->sNotes);
				}*/
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_fNormOffset"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->m_fNormOffset = atof(valueStr);
				pPlate->SetModified();
				pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
			g_pSolidDraw->Draw();
		}
		else if ((CLDSPlate::GetPropID("des_para.SCALE_NODE.hLinePart")==pItem->m_idProp)		//��׼�˼����
			||(CLDSPlate::GetPropID("des_para.SCALE_NODE.hStartNode")==pItem->m_idProp)	//ʼ�˽ڵ���
			||(CLDSPlate::GetPropID("des_para.SCALE_NODE.hEndNode")==pItem->m_idProp)		//�ն˽ڵ���
			||(CLDSPlate::GetPropID("des_para.SCALE_NODE.start_offset_dist")==pItem->m_idProp) //ʼ��ƫ����
			||(CLDSPlate::GetPropID("des_para.SCALE_NODE.end_offset_dist")==pItem->m_idProp)	//�ն�ƫ����
			||(CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_scale")==pItem->m_idProp)		//ƫ�Ʊ���
			||(CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_dist")==pItem->m_idProp))		//�ڵȷֻ����ϵ�ƫ��ֵ
		{
			if (CLDSPlate::GetPropID("des_para.SCALE_NODE.hLinePart")==pItem->m_idProp)			//��׼�˼����
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.hLinePart=HexStrToLong(tem_str);
			else if (CLDSPlate::GetPropID("des_para.SCALE_NODE.hStartNode")==pItem->m_idProp)	//ʼ�˽ڵ���
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.hStartNode=HexStrToLong(tem_str);
			else if (CLDSPlate::GetPropID("des_para.SCALE_NODE.hEndNode")==pItem->m_idProp)		//�ն˽ڵ���
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.hEndNode=HexStrToLong(tem_str);
			else if (CLDSPlate::GetPropID("des_para.SCALE_NODE.start_offset_dist")==pItem->m_idProp)//ʼ��ƫ����
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.start_offset_dist=atof(valueStr);
			else if (CLDSPlate::GetPropID("des_para.SCALE_NODE.end_offset_dist")==pItem->m_idProp)	//�ն�ƫ����
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.end_offset_dist=atof(valueStr);
			else if (CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_scale")==pItem->m_idProp)		//ƫ�Ʊ���
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.offset_scale=atof(valueStr);
			else //if(CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_dist")==pItem->m_idProp)	//�ڵȷֻ����ϵ�ƫ��ֵ
				pFirstPlate->designInfo.origin.des_para.SCALE_NODE.offset_dist=atof(valueStr);
			pFirstPlate->ucs.origin=pFirstPlate->designInfo.origin.UpdatePos(&Ta);
			pFirstPlate->DesignSetupUcs();
			CLsRef *pLsRef=NULL;
			for (pLsRef=pFirstPlate->GetFirstLsRef();pLsRef;pLsRef=pFirstPlate->GetNextLsRef())
			{	//���¼�����˨λ��
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.origin.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->ucs.origin.x = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.origin.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->ucs.origin.y = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.origin.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->ucs.origin.z = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_x.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_x(atof(valueStr),pPlate->ucs.axis_x.y,pPlate->ucs.axis_x.z);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_x,0);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_x.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_x(pPlate->ucs.axis_x.x,atof(valueStr),pPlate->ucs.axis_x.z);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_x,0);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_x.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_x(pPlate->ucs.axis_x.x,pPlate->ucs.axis_x.y,atof(valueStr));
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_x,0);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_y.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_y(atof(valueStr),pPlate->ucs.axis_y.y,pPlate->ucs.axis_y.z);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_y,1);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_y.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_y(pPlate->ucs.axis_y.x,atof(valueStr),pPlate->ucs.axis_y.z);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_y,1);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_y.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_y(pPlate->ucs.axis_y.x,pPlate->ucs.axis_y.y,atof(valueStr));
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_y,1);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_z.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_z(atof(valueStr),pPlate->ucs.axis_z.y,pPlate->ucs.axis_z.z);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_z,2);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_z.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_z(pPlate->ucs.axis_z.x,atof(valueStr),pPlate->ucs.axis_z.z);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_z,2);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("ucs.axis_z.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dPoint axis_z(pPlate->ucs.axis_z.x,pPlate->ucs.axis_z.y,atof(valueStr));
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+axis_z,2);
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_fInnerRadius"))
		{
			CXhChar100 sText;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_fInnerRadius = atof(valueStr);
			if (pFirstPlate->m_fInnerRadius>0)
			{
				pParentItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_fInnerRadius"),&pFindItem);
				if (pParentItem)
				{
					pParentItem->m_bHideChildren=TRUE;
					pPropItem=NULL;
					long idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm");
					pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
					if (pFindItem==NULL)
					{
						lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Inner Circle Normal";
						lpInfo->m_strPropHelp = "The normal of inner circle.";
#else 
						lpInfo->m_strPropName = "�ڻ����߷���";
						lpInfo->m_strPropHelp = "�ڻ����߷���";
#endif
						pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
						pPropItem->m_idProp = idProp;
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
						pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
					}
					if (pPropItem)
					{
						idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm.x");
						pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "X ";
							lpInfo->m_strPropHelp = "The X coordinate component of inner circle's normal.";
#else 
							lpInfo->m_strPropName = "�ڻ�����X����";
							lpInfo->m_strPropHelp = "�ڻ����߷����X����ֵ��";
#endif
							pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
							pFindItem->m_idProp = idProp;
							if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
								pFindItem->m_lpNodeInfo->m_strPropValue = sText;
							pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
							pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
						}
						idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm.y");
						pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Y ";
							lpInfo->m_strPropHelp = "The Y coordinate component of inner circle's normal.";
#else 
							lpInfo->m_strPropName = "�ڻ�����Y����";
							lpInfo->m_strPropHelp = "�ڻ����߷����Y����ֵ��";
#endif
							pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
							pFindItem->m_idProp = idProp;
							if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
								pFindItem->m_lpNodeInfo->m_strPropValue = sText;
							pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
							pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
						}
						idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm.z");
						pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Z ";
							lpInfo->m_strPropHelp = "The Z coordinate component of inner circle's normal.";
#else 
							lpInfo->m_strPropName = "�ڻ�����Z����";
							lpInfo->m_strPropHelp = "�ڻ����߷����Z����ֵ��";
#endif
							pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
							pFindItem->m_idProp = idProp;
							if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
								pFindItem->m_lpNodeInfo->m_strPropValue = sText;
							pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
							pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
						}
					}
					//�ڻ�λ��
					pPropItem=NULL;
					idProp=CLDSPlate::GetPropID("m_tInnerOrigin");
					pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
					if (pFindItem==NULL)
					{
						lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Inner Circle Center Position";
						lpInfo->m_strPropHelp = "The inner circle position is the relative coordinates of plate coordinates.";
#else 
						lpInfo->m_strPropName = "�ڻ�λ��";
						lpInfo->m_strPropHelp = "�ڻ�λ��,������Ϊ�ְ�����ϵ�µ�������ꡣ";
#endif
						pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
						pPropItem->m_idProp = idProp;
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
						pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
					}
					if (pPropItem)
					{
						idProp=CLDSPlate::GetPropID("m_tInnerOrigin.x");
						pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Inner Center X Coordinate";
							lpInfo->m_strPropHelp = "The X coordinate component of inner circle's center.";
#else 
							lpInfo->m_strPropName = "�ڻ�λ��X����";
							lpInfo->m_strPropHelp = "�ڻ�λ�õ�X����ֵ��";
#endif
							pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
							pFindItem->m_idProp = idProp;
							if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
								pFindItem->m_lpNodeInfo->m_strPropValue = sText;
							pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
							pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
						}
						idProp=CLDSPlate::GetPropID("m_tInnerOrigin.y");
						pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Inner Center Y Coordinate";
							lpInfo->m_strPropHelp = "The Y coordinate component of inner circle's center.";
#else 
							lpInfo->m_strPropName = "�ڻ�λ��Y����";
							lpInfo->m_strPropHelp = "�ڻ�λ�õ�Y����ֵ��";
#endif
							pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
							pFindItem->m_idProp = idProp;
							if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
								pFindItem->m_lpNodeInfo->m_strPropValue = sText;
							pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
							pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
						}
						idProp=CLDSPlate::GetPropID("m_tInnerOrigin.z");
						pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Inner Center Z Coordinate";
							lpInfo->m_strPropHelp = "The Z coordinate component of inner circle's center.";
#else 
							lpInfo->m_strPropName = "�ڻ�λ��Z����";
							lpInfo->m_strPropHelp = "�ڻ�λ�õ�Z����ֵ��";
#endif
							pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
							pFindItem->m_idProp = idProp;
							if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
								pFindItem->m_lpNodeInfo->m_strPropValue = sText;
							pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
							pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
						}
					}
				}
			}
			else
			{
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.x"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.y"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.z"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.x"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.y"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.z"));
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerColumnNorm.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_tInnerColumnNorm.x = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerColumnNorm.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_tInnerColumnNorm.y = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerColumnNorm.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_tInnerColumnNorm.z = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerOrigin.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_tInnerOrigin.x = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerOrigin.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_tInnerOrigin.y = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_tInnerOrigin.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_tInnerOrigin.z = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("iProfileStyle0123"))
		{
			sscanf(valueStr,"%d",&iTemp);
			CUndoOperObject undo(&Ta);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->designInfo.iProfileStyle0123 = iTemp;
				pPlate->DesignPlate();	//�޸����μ��㷽ʽ��������Ƹְ�
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_cRollProcessType"))
		{
			BYTE cProcessType= valueStr[0]-'0';
			if (cProcessType==2)
				cProcessType=0xFF;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_cRollProcessType=cProcessType;
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_cRollOffsetType"))
		{
			BYTE cProcessType= valueStr[0]-'0';
			if (cProcessType==2)
				cProcessType=0xFF;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_ciRollOffsetType=cProcessType;
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_cDeformType"))
		{
			BYTE cProcessType= valueStr[0]-'0';
			if (cProcessType==4)
				cProcessType=0xFF;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_ciDeformType=cProcessType;
			//
			pPropList->DeleteAllSonItems(pItem);
			if (cProcessType==1&&pFirstPlate->GetFaceN()>1)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Deform Coef";
#else 
				lpInfo->m_strPropName = "���Բ�ϵ��";
#endif
				pPropItem = pPropList->InsertItem(pItem,lpInfo,0,true);
				pPropItem->m_idProp = CLDSPlate::GetPropID("m_fDeformCoef");
				if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = tem_str;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
				if (pFirstPlate->GetFaceN()==3)	//�����
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Deform Coef2";
#else
					lpInfo->m_strPropName = "���Բ�ϵ��2";
#endif
					pPropItem = pPropList->InsertItem(pItem,lpInfo,-1);
					pPropItem->m_idProp = CLDSPlate::GetPropID("m_fDeformCoef2");
					if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = tem_str;
					pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_fDeformCoef"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_fDeformCoef=atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_fDeformCoef2"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->m_fDeformCoef2=atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("face_N"))
		{
			iTemp=valueStr[0]-'0';//sscanf(valueStr,"%d",&iTemp);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				long nOldFaceN=pPlate->face_N;
				if (iTemp==4)
				{
					pPlate->face_N = 1;
					pPlate->jdb_style=4;	//��˨���
				}
				else if (iTemp==3)
				{
					pPlate->face_N = 3;
					pPlate->jdb_style=1;	//�۵���
				}
				else
				{
					pPlate->face_N=iTemp+1;
					if (pPlate->designInfo.iFaceType!=6&&pPlate->designInfo.iFaceType!=7)
						pPlate->designInfo.iFaceType=iTemp+1;
					pPlate->jdb_style=0;	//��ͨ�ְ�
				}
				//��ű�С֮��,�޸Ĺ����˼���ţ�������Ƹְ�����
				//��繹���������ְ�֮�����Ϊ������˫���,��ͨ�����������޸�Ϊ�����	wht 16-10-20
				if (pPlate->face_N<nOldFaceN)
				{
					for (CDesignLjPartPara *pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
					{
						if (pLjPara->iFaceNo>pPlate->face_N)
							pLjPara->iFaceNo=1;
					}
				}
				pPlate->DesignPlate();
				pPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("plateProfile"),&pParentItem);
			if (pFindItem)
				Update_Plate_Face_N_TreeItem(pPropList,pFindItem);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bSelfCorrectBendline"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->designInfo.m_bSelfCorrectBendline=(valueStr[0]!='0');
				if (pPlate->face_N>1)
				{
					pPlate->SetModified();
					pPlate->DesignPlate();
					pPlate->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].norm_style"))
		{	//��һ�����߷��߶���
			sscanf(valueStr,"%d",&iTemp);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].norm_style = iTemp;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0]"),&pParentItem);
			if (pFindItem)
				Update_Plate_Norm_CalStyle_TreeItem(pPropList,pFindItem,TRUE);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].vector.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].vector.x = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].vector.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].vector.y = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].vector.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].vector.z = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hVicePart"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].hVicePart=HexStrToLong(tem_str);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].norm_wing"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("X leg normal")==0)
					pPlate->designInfo.huoqufacenorm[0].norm_wing = 0;
				else if (valueStr.CompareNoCase("Y leg normal")==0)
#else 
				if (valueStr.CompareNoCase("X֫����")==0)
					pPlate->designInfo.huoqufacenorm[0].norm_wing = 0;
				else if (valueStr.CompareNoCase("Y֫����")==0)
#endif
					pPlate->designInfo.huoqufacenorm[0].norm_wing = 1;
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].angle"))
		{
			double angle =atof(valueStr)*RADTODEG_COEF;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dLine huoquline;
				pPlate->GetHuoQuAngle(1,&huoquline);
				GEPOINT rotaxis=huoquline.endPt-huoquline.startPt;
				rotaxis.z=0;
				vector_trans(rotaxis,pPlate->ucs,TRUE,TRUE);
				pPlate->huoqufacenorm[0]=pPlate->ucs.axis_z;
				RotateVectorAroundVector(pPlate->huoqufacenorm[0],angle,rotaxis);
				pPlate->designInfo.huoqufacenorm[0].vector=pPlate->huoqufacenorm[0];
				pPlate->SetModified();
				//���¹��߰��ϵ��׹ܼ����߿ױڵ�λ�ü�ʵ����ʾ wjh-2016.11.04
				for (CLsRef* pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					CLDSBolt* pBolt=pLsRef->GetLsPtr();
					if (pBolt->IsLocalCoordLocation()&&pBolt->des_base_pos.reside_plane==1)
					{
						pBolt->correct_worknorm();
						pBolt->correct_pos();
						if (!pBolt->m_bVirtualPart)
						{
							pBolt->Create3dSolidModel();
							g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
						}
						else
						{
							CLdsPartListStack stack(&Ta.Parts);
							HOLE_WALL* pHoleWall = console.GetHoleWall(pBolt,pPlate->handle);
							if (pHoleWall)
							{
								pHoleWall->UpdateSolid();
								g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
							}
							if (pBolt->get_d()+pBolt->hole_d_increment>26)	//�п������׹ܿ�
							{
								for (CLDSLineTube* pBushTube=(CLDSLineTube*)Ta.Parts.GetFirst(CLS_LINETUBE);pBushTube;pBushTube=(CLDSLineTube*)Ta.Parts.GetNext(CLS_LINETUBE))
								{
									if (pBushTube->FuncType()==CLDSLineTube::FUNC_WIREBUSH&&pBushTube->m_hPartWeldParent==pPlate->handle)
									{
										pBushTube->CalPosition();
										pBushTube->Create3dSolidModel();
										g_pSolidDraw->NewSolidPart(pBushTube->GetSolidPartObject());
										break;
									}
								}
							}
						}
					}
				}
			}
		}
		else  if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].angle"))
		{
			double angle =atof(valueStr)*RADTODEG_COEF;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				f3dLine huoquline;
				pPlate->GetHuoQuAngle(2,&huoquline);
				GEPOINT rotaxis=huoquline.endPt-huoquline.startPt;
				vector_trans(rotaxis,pPlate->ucs,TRUE,TRUE);
				pPlate->huoqufacenorm[1]=pPlate->ucs.axis_z;
				RotateVectorAroundVector(pPlate->huoqufacenorm[1],angle,rotaxis);
				pPlate->designInfo.huoqufacenorm[1].vector=pPlate->huoqufacenorm[1];
				pPlate->SetModified();
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].direction"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].direction = valueStr[0]-'0';
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[0].hCrossPart"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[0].hCrossPart=HexStrToLong(tem_str);
		}
		//�ڶ������߷��߶���
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].norm_style"))
		{
			sscanf(valueStr,"%d",&iTemp);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].norm_style = iTemp;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1]"),&pParentItem);
			if (pFindItem)
				Update_Plate_Norm_CalStyle_TreeItem(pPropList,pFindItem,FALSE);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].vector.x"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].vector.x = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].vector.y"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].vector.y = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].vector.z"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].vector.z = atof(valueStr);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].hVicePart"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].hVicePart=HexStrToLong(tem_str);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].norm_wing"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("X leg normal")==0)
					pPlate->designInfo.huoqufacenorm[1].norm_wing = 0;
				else if (valueStr.CompareNoCase("Y leg normal")==0)
#else 
				if (valueStr.CompareNoCase("X֫����")==0)
					pPlate->designInfo.huoqufacenorm[1].norm_wing = 0;
				else if (valueStr.CompareNoCase("Y֫����")==0)
#endif
					pPlate->designInfo.huoqufacenorm[1].norm_wing = 1;
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].direction"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].direction = valueStr[0]-'0';
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("huoqufacenorm[1].hCrossPart"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				pPlate->designInfo.huoqufacenorm[1].hCrossPart=HexStrToLong(tem_str);
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bWeldPart"))
		{
			CXhChar100 sText;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
#else 
				if (valueStr.CompareNoCase("��")==0)
#endif
					pPlate->m_bWeldPart=TRUE;
				else //if(valueStr.CompareNoCase("��")==0)
					pPlate->m_bWeldPart=FALSE;
			}
			if (pFirstPlate->m_bWeldPart)
			{
				//ָ��������
				for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
				{
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("iWeldLineStyle"),&pParentItem);
					if (pFindItem==NULL)
					{
						lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Weld Line Type";
						lpInfo->m_strPropHelp = "Weld line size calculation type";
						lpInfo->m_cmbItems = "0.Calculate freely|1.Specified Weld Line|2.Extention Aligned";
#else 
						lpInfo->m_strPropName = "����������";
						lpInfo->m_strPropHelp = "�����߳ߴ��������";
						lpInfo->m_cmbItems = "0.���ɼ���|1.ָ������|2.�������";
#endif
						pPropItem = pPropList->InsertItem(pItem,lpInfo,0,true);
						pPropItem->m_idProp = CLDSPlate::GetPropID("iWeldLineStyle");
						if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
							pPropItem->m_lpNodeInfo->m_strPropValue = sText;
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
						pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
					}
					if (pPlate->designInfo.iWeldLineStyle==1)
					{
						pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"),&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Weld Line Start Height";
							lpInfo->m_strPropHelp = "The start height of weld line.";
#else 
							lpInfo->m_strPropName = "��������ʼ��";
							lpInfo->m_strPropHelp = "��������ʼ��";
#endif
							pPropItem = pPropList->InsertItem(pItem,lpInfo,1,true);
							pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_start_high");
							if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
								pPropItem->m_lpNodeInfo->m_strPropValue = sText;
							pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
						}
						pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"),&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Weld Line End Height";
							lpInfo->m_strPropHelp = "The end height of weld line.";
#else 
							lpInfo->m_strPropName = "��������ֹ��";
							lpInfo->m_strPropHelp = "��������ֹ��";
#endif
							pPropItem = pPropList->InsertItem(pItem,lpInfo,2,true);
							pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_end_high");
							if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
								pPropItem->m_lpNodeInfo->m_strPropValue = sText;
							pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
						}
						pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"),&pParentItem);
						if (pFindItem==NULL)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "Weld Width";
							lpInfo->m_strPropHelp = "Weld width";
#else 
							lpInfo->m_strPropName = "�������";
							lpInfo->m_strPropHelp = "�������";
#endif
							pPropItem = pPropList->InsertItem(pItem,lpInfo,3,true);
							pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_width");
							if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
								pPropItem->m_lpNodeInfo->m_strPropValue = sText;
							pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
						}
					}
				}
			}
			else
			{
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("iWeldLineStyle"),&pParentItem);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("iWeldLineStyle"));
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"),&pParentItem);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"));
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"),&pParentItem);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"));
				pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"),&pParentItem);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"));
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("iWeldLineStyle"))
		{
			CXhChar100 sText;
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->designInfo.iWeldLineStyle=valueStr[0]-'0';
				if (pPlate->designInfo.iWeldLineStyle==1)
				{
					CPropTreeItem *pFatherItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_bWeldPart"),&pParentItem);
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"),&pParentItem);
					if (pFindItem==NULL)
					{
						lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Weld Line Start Height";
						lpInfo->m_strPropHelp = "The start height of weld line.";
#else 
						lpInfo->m_strPropName = "��������ʼ��";
						lpInfo->m_strPropHelp = "��������ʼ��";
#endif
						pPropItem = pPropList->InsertItem(pFatherItem,lpInfo,1,true);
						pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_start_high");
						if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
							pPropItem->m_lpNodeInfo->m_strPropValue = sText;
						pPropItem->m_dwPropGroup=pFatherItem->m_dwPropGroup;
					}
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"),&pParentItem);
					if (pFindItem==NULL)
					{
						lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Weld Line End Height";
						lpInfo->m_strPropHelp = "The end height of weld line.";
#else 
						lpInfo->m_strPropName = "��������ֹ��";
						lpInfo->m_strPropHelp = "��������ֹ��";
#endif
						pPropItem = pPropList->InsertItem(pFatherItem,lpInfo,2,true);
						pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_end_high");
						if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
							pPropItem->m_lpNodeInfo->m_strPropValue = sText;
						pPropItem->m_dwPropGroup=pFatherItem->m_dwPropGroup;
					}
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"),&pParentItem);
					if (pFindItem==NULL)
					{
						lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Weld Width";
						lpInfo->m_strPropHelp = "Weld width";
#else 
						lpInfo->m_strPropName = "�������";
						lpInfo->m_strPropHelp = "�������";
#endif
						pPropItem = pPropList->InsertItem(pFatherItem,lpInfo,3,true);
						pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_width");
						if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
							pPropItem->m_lpNodeInfo->m_strPropValue = sText;
						pPropItem->m_dwPropGroup=pFatherItem->m_dwPropGroup;
					}
				}
				else
				{
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"),&pParentItem);
					if (pFindItem)
						pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"));
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"),&pParentItem);
					if (pFindItem)
						pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"));
					pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"),&pParentItem);
					if (pFindItem)
						pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"));
				}
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("m_bNeedFillet"))
		{
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (valueStr.CompareNoCase("Yes")==0)
#else 
				if (valueStr.CompareNoCase("��")==0)
#endif
					pPlate->m_bNeedFillet=TRUE;
				else //if(valueStr.CompareNoCase("��")==0)
					pPlate->m_bNeedFillet=FALSE;
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.weld_start_high"))
		{
			CUndoOperObject undo(&Ta);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->designInfo.weld_start_high = atof(valueStr);
				pPlate->DesignPlate();
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.weld_end_high"))
		{
			CUndoOperObject undo(&Ta);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->designInfo.weld_end_high = atof(valueStr);
				pPlate->DesignPlate();
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("designInfo.weld_width"))
		{
			CUndoOperObject undo(&Ta);
			for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
			{
				pPlate->designInfo.weld_width = atof(valueStr);
				pPlate->DesignPlate();
			}
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.origin.x"))
		{
			bUpdateWcs=TRUE;
			PLATEEDIT_STATE* pEditState=CLDSPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
			pEditState->m_fLocalWcsOrgX=console.m_curWCS.origin.x=atof(valueStr);
			//
			oper.UpdatePropItemValue("wcs.origin");
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.origin.y"))
		{
			bUpdateWcs=TRUE;
			PLATEEDIT_STATE* pEditState=CLDSPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
			pEditState->m_fLocalWcsOrgY=console.m_curWCS.origin.y=atof(valueStr);
			//
			oper.UpdatePropItemValue("wcs.origin");
		}
		else if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.axis_x.a"))
		{
			bUpdateWcs=TRUE;
			double alfa=atof(valueStr)*RADTODEG_COEF;	//�ɽǶ���תΪ������
			PLATEEDIT_STATE* pEditState=CLDSPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
			console.m_curWCS.axis_x=GEPOINT(cos(alfa),sin(alfa));
			console.m_curWCS.axis_y.Set(-console.m_curWCS.axis_x.y,console.m_curWCS.axis_x.x);
			pEditState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
			pEditState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
			pEditState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
			pEditState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
			//
			oper.UpdatePropItemValue("wcs.axis_x");
			oper.UpdatePropItemValue("wcs.axis_y");
		}
		if (pItem->m_idProp==CLDSPlate::GetPropID("peakVertexOffsetY")||
			pItem->m_idProp==CLDSPlate::GetPropID("extendVertexOffsetLen")||
			pItem->m_idProp==CLDSPlate::GetPropID("peakPointMargin"))
		{
			if (pItem->m_idProp==CLDSPlate::GetPropID("peakVertexOffsetY"))
				pFirstPlate->peakVertexOffsetY=atof(valueStr);
			else if (pItem->m_idProp==CLDSPlate::GetPropID("extendVertexOffsetLen"))
				pFirstPlate->extendVertexOffsetLen=atof(valueStr);
			else if (pItem->m_idProp==CLDSPlate::GetPropID("peakPointMargin"))
				pFirstPlate->peakPointMargin=atof(valueStr);
			pFirstPlate->DesignPlate();
			CLDSView::RedrawPlateProfileDrawing(pFirstPlate);
		}
		if (bUpdateWcs==TRUE)
		{
			UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
			g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
			g_pSolidDraw->Draw();
		}
		for (pPlate=(CLDSPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSPlate*)pView->selectObjs.GetNext())
		{
			pPlate->SetModified();
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		}
	}
	catch (char *sError)
	{
		AfxMessageBox(sError);
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

CPropTreeItem* InsertOrUpdateItem(CPropertyList* pPropList,PROPLIST_ITEM* pPropItem,CPropTreeItem *pParent,int controlType/*=0*/,
	const char* valueText/*=NULL*/,int iInsertIndex/*=-1*/,BOOL bUpdate/*=0*/)
{
	if (pPropItem==NULL)
		return NULL;
	CPropTreeItem* pItem=pPropList->FindItemByPropId(pPropItem->id,NULL);
	if (pItem==NULL)
	{
		CItemInfo* lpInfo = new CItemInfo();
		lpInfo->m_strPropName=pPropItem->name;
		lpInfo->m_strPropHelp=pPropItem->propHelpStr;
		lpInfo->m_controlType=controlType;
		if (lpInfo->m_controlType==PIT_COMBO)
			lpInfo->m_cmbItems=pPropItem->propCmbItemsStr;
		if (valueText)
			lpInfo->m_strPropValue = valueText;
		pItem=pPropList->InsertItem(pParent,lpInfo,iInsertIndex,bUpdate);
		pItem->m_idProp=pPropItem->id;
		pItem->m_dwPropGroup=pParent->m_dwPropGroup;
	}
	else if (valueText!=NULL)
	{
		pItem->m_lpNodeInfo->m_strPropName = pPropItem->name;
		pItem->m_lpNodeInfo->m_strPropHelp = pPropItem->propHelpStr;
		pItem->m_lpNodeInfo->m_strPropValue = valueText;
		if (bUpdate&&!pItem->IsHideSelf())
		{
			RECT rc;
			pPropList->GetItemRect(pItem->m_iIndex,&rc);
			pPropList->InvalidateRect(&rc);
		}
	}
	return pItem;
}
CPropTreeItem* InsertOrUpdatePlateParamItem(CPropertyList *pPropList,CPropTreeItem *pParent,const char* propKey,CLDSParamPlate *pPlate,
	int controlType=0,BOOL bUpdateValueText=TRUE,int iInsertIndex=-1,BOOL bUpdate=0)
{
	PROPLIST_ITEM* pPropItem=CLDSParamPlate::propHashtable.GetValue(propKey);
	if (pPropItem==NULL)
	{
		DYNA_PROPITEM_GROUP *pGroup=CLDSParamPlate::hashParamItemPropGroup.GetValue(pPlate->m_iParamType);
		if (pGroup)
		{
			long id=pGroup->GetPropID(propKey);
			pPropItem=pGroup->GetPropItem(id);
		}
	}
	if (pPropItem==NULL)
		return NULL;
	CXhChar50 sText;
	CPropTreeItem* pItem;
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if (bUpdateValueText&&pPlate->GetPropValueStr(pPropItem->id,sText)>0)
		pItem=::InsertOrUpdateItem(pPropList,pPropItem,pParent,controlType,sText,iInsertIndex,bUpdate);
	else
		pItem=::InsertOrUpdateItem(pPropList,pPropItem,pParent,controlType,NULL,iInsertIndex,bUpdate);
	pItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(pView->selectObjs,pPropItem->id);
	return pItem;
}
//����(��ţ���)�װ�
void RebuildOrUpdateBasePlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem *pSonPropItem=NULL,*pPropItem=NULL;
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"S",pCurPlate,PIT_COMBO);
	long shapeType=0;
	pCurPlate->GetDesignItemValue(KEY2C("S"),&shapeType);
	if (0==shapeType)
	{
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("R",pCurPlate->m_iParamType));
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"XA",pCurPlate,PIT_EDIT,TRUE,0,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"XB",pCurPlate,PIT_EDIT,TRUE,1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"YA",pCurPlate,PIT_EDIT,TRUE,2,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"YB",pCurPlate,PIT_EDIT,TRUE,3,TRUE);
		pSonPropItem=InsertOrUpdatePlateParamItem(pPropList,pPropItem,"X2",pCurPlate,PIT_EDIT,TRUE,4,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pSonPropItem,"Y2",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pSonPropItem=InsertOrUpdatePlateParamItem(pPropList,pPropItem,"X3",pCurPlate,PIT_EDIT,TRUE,5,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pSonPropItem,"Y3",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pSonPropItem=InsertOrUpdatePlateParamItem(pPropList,pPropItem,"X1",pCurPlate,PIT_EDIT,TRUE,6,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pSonPropItem,"Y1",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pSonPropItem=InsertOrUpdatePlateParamItem(pPropList,pPropItem,"X0",pCurPlate,PIT_EDIT,TRUE,7,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pSonPropItem,"Y0",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("XA",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("XB",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("YA",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("YB",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("X0",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("X1",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("X2",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("X3",pCurPlate->m_iParamType));
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"R",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
}
BOOL ModifyBasePlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	CXhChar100 tem_str("%s",valueStr);
	KEY2C paramKey=CLDSParamPlate::GetPropKey(pItem->m_idProp,pPlate->m_iParamType);
	if (KEY2C("S")==paramKey)
	{	//�װ���������
		long iType=valueStr[0]-'0';
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey,iType);
		//����������
		RebuildOrUpdateBasePlateParamsPropertyList(pPropList,pItem->m_pParent);
	}
	else
		return FALSE;
	return TRUE;
}
//����(��ţ���)ѥ��
void RebuildOrUpdateShoePlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	long iConnStyle=0,shapeType=0,hLinkXieAngle=0;
	pCurPlate->GetDesignItemValue(KEY2C("LX"),&hLinkXieAngle);
	pCurPlate->GetDesignItemValue(KEY2C("F"),&shapeType);	//������� 0.�ᴩ�װ�|1.֫����|2.���ߵ���
	pCurPlate->GetDesignItemValue(KEY2C("CS"),&iConnStyle);	//0.��˨����|1.�Ժ�����
	//
	CPropTreeItem* pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"F",pCurPlate,PIT_COMBO);
	pItem->SetReadOnly();
	if (shapeType!=1)
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
	}
	if (shapeType!=2)
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"IA",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"IB",pCurPlate,PIT_EDIT);
	}
	if (hLinkXieAngle>0X20)
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"IC",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"ID",pCurPlate,PIT_EDIT);
	}
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"M",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"AL",pCurPlate,PIT_EDIT);
	if (0==shapeType)
	{
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("CM",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("L",pCurPlate->m_iParamType));
	}
	else
	{
		long CW=0;
		pCurPlate->GetDesignItemValue(KEY2C("L"),&CW);
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"L",pCurPlate,PIT_EDIT);
		if (CW>0)
			InsertOrUpdatePlateParamItem(pPropList,pItem,"CM",pCurPlate,PIT_COMBO);
	}
	//�Ժ�ģʽ�µ����߲�ѥ��
	if (iConnStyle==1 && shapeType==2)
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"NL",pCurPlate,PIT_EDIT);
	else
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("NL",pCurPlate->m_iParamType));
}
BOOL ModifyShoePlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	KEY2C paramKey=CLDSParamPlate::GetPropKey(pItem->m_idProp,pPlate->m_iParamType);
	if (KEY2C("F")==paramKey||KEY2C("W")==paramKey||KEY2C("CS")==paramKey||KEY2C("CM")==paramKey)
	{	//�װ���������
		long iType=valueStr[0]-'0';
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey,iType);
		//����������
		if (KEY2C("F")==paramKey)
			RebuildOrUpdateShoePlateParamsPropertyList(pPropList,pItem->m_pParent);
	}
	else if (KEY2C("L")==paramKey)
	{
		long CW=atoi(valueStr);
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey,CW);
		//����������
		CLDSParamPlate* pFirstParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();
		if (CW==0)
			pPropList->DeleteAllSonItems(pItem);
		else
			InsertOrUpdatePlateParamItem(pPropList,pItem,"e",pFirstParamPlate,PIT_COMBO,TRUE,-1,TRUE);
	}
	else
		return FALSE;
	return TRUE;
}
//�Ǹ��߰�
void RebuildOrUpdateAngleRibPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	long plateType=0,edgeType=0;
	pCurPlate->GetDesignItemValue(KEY2C("C"),&edgeType);
	pCurPlate->GetDesignItemValue(KEY2C("I"),&plateType);	//������� 0.�ڲ��߰�|1.����߰�
	//InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_COMBO);
	//InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
	//InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
	//InsertOrUpdatePlateParamItem(pPropList,pParentItem,"S",pCurPlate,PIT_EDIT);
	//InsertOrUpdatePlateParamItem(pPropList,pParentItem,"L",pCurPlate,PIT_EDIT);
	if (plateType==0)
	{	//�ڲ�Ǹ��߰�
		double d=0;
		pCurPlate->GetDesignItemValue(KEY2C("d"),&d);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"L",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Ly",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Wy",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList, pParentItem,"E",pCurPlate,PIT_COMBO, TRUE, -1, TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"S",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"C",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		CPropTreeItem* pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"d",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		if (d>0)
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"P",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			if (edgeType>0)
				InsertOrUpdatePlateParamItem(pPropList,pItem,"R",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
	}
	else if (plateType==1)
	{	//���Ǹ��߰�
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Hy",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Wy",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		CPropTreeItem* pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"S",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pItem->m_lpNodeInfo->m_strPropName="����Բ��R";
	}
	else if (plateType==2)
	{	//����Ǹ��߰�
		CPropTreeItem* pItem=NULL;
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pItem->m_lpNodeInfo->m_strPropName="����ƫ����";
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"L",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pItem->m_lpNodeInfo->m_strPropName="�߰峤��L";
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pItem->m_lpNodeInfo->m_strPropName="�߰峤��W";
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pItem->m_lpNodeInfo->m_strPropName="����Բ��R";
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"d",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
}
BOOL ModifyAngleRibPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CXhChar100 sText;
	CLDSParamPlate *pPlate=NULL,*pFirstPlate=(CLDSParamPlate*)selectObjs.GetFirst();
	KEY2C paramKey=CLDSParamPlate::GetPropKey(pItem->m_idProp,pFirstPlate->m_iParamType);
	if (KEY2C("Lr")==paramKey)
	{
		long hAngle=0;
		double fLr=atof(valueStr);
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			pPlate->GetDesignItemValue(KEY2C("A"),&hAngle);
			CLDSLineAngle *pAngle=(CLDSLineAngle*)console.FromPartHandle(hAngle,CLS_LINEANGLE);
			if (pAngle)
			{
				pPlate->SetDesignItemValue(KEY2C("L"),pAngle->GetWidth()-fLr);
				long idProp=CLDSParamPlate::GetPropID("L",pPlate->m_iParamType);
				if (pPlate->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,CString(sText));
			}
		}
	}
	else if (KEY2C("L")==paramKey)
	{
		double fValue=atof(valueStr);
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			//{
			pPlate->SetDesignItemValue(paramKey,fValue);
		//	long idProp=CLDSParamPlate::GetPropID("Lr",pPlate->m_iParamType);
		//	if(pPlate->GetPropValueStr(idProp,sText)>0)
		//		pPropList->SetItemPropValue(idProp,CString(sText));
		//}
	}
	/*else if(KEY2C("I")==paramKey)
	{	//�װ���������
		long iOldType=0,iType=valueStr[0]-'0';
		pFirstPlate->GetDesignItemValue(KEY2C("I"),&iOldType);
		for(pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			pPlate->SetDesignItemValue(paramKey,iType);
			if(iOldType!=iType)
			{
				if(iType==0)
				{
					if(pPlate->GetDesignItem(KEY2C("C"))==NULL)
						pPlate->SetDesignItemValue(KEY2C("C"),0);
					if(pPlate->GetDesignItem(KEY2C("P"))==NULL)
						pPlate->SetDesignItemValue(KEY2C("P"),0);
					if(pPlate->GetDesignItem(KEY2C("d"))==NULL)
						pPlate->SetDesignItemValue(KEY2C("d"),0);
					if(pPlate->GetDesignItem(KEY2C("R"))==NULL)
						pPlate->SetDesignItemValue(KEY2C("R"),0);
				}
				else
				{
					if(pPlate->GetDesignItem(KEY2C("H"))==NULL)
						pPlate->SetDesignItemValue(KEY2C("H"),0);
				}
			}
		}
		//����������
		if(iOldType!=iType)
		{
			CPropTreeItem *pParentItem=pItem->m_pParent;
			if(iType==0)
			{
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("H",pFirstPlate->m_iParamType));
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Lr",pFirstPlate,PIT_EDIT,TRUE,5,TRUE);
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"C",pFirstPlate,PIT_COMBO,TRUE,6,TRUE);
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"P",pFirstPlate,PIT_EDIT,TRUE,7,TRUE);
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"d",pFirstPlate,PIT_EDIT,TRUE,8,TRUE);
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pFirstPlate,PIT_EDIT,TRUE,9,TRUE);
			}
			else
			{
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("Lr",pFirstPlate->m_iParamType));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("C",pFirstPlate->m_iParamType));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("P",pFirstPlate->m_iParamType));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("d",pFirstPlate->m_iParamType));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("R",pFirstPlate->m_iParamType));
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pFirstPlate,PIT_EDIT,TRUE,5,TRUE);
			}
		}
	}*/
	else if (KEY2C("C")==paramKey)
	{
		long type=valueStr[0]-'0';
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey,type,NULL,PARAM_SHAPE,1);
		//
		CPropTreeItem* pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("d",pFirstPlate->m_iParamType),NULL);
		if (pFindItem==NULL)
			return FALSE;
		pPropList->DeleteAllSonItems(pFindItem);
		double d=0;
		pFirstPlate->GetDesignItemValue(KEY2C("d"),&d);
		if (d>0)
		{
			InsertOrUpdatePlateParamItem(pPropList,pFindItem,"P",pFirstPlate,PIT_EDIT,TRUE,-1,TRUE);
			if (type>0)
				InsertOrUpdatePlateParamItem(pPropList,pFindItem,"R",pFirstPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
	}
	else if (KEY2C("E") == paramKey)
	{
		long type = valueStr[0] - '0';
		for (pPlate = (CLDSParamPlate*)selectObjs.GetFirst(); pPlate; pPlate = (CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey, type, NULL, PARAM_SHAPE, 1);
	}
	else if (KEY2C("d")==paramKey)
	{
		double d=atof(valueStr);
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey,d);
		//
		long edgeType=0,I=0;
		pFirstPlate->GetDesignItemValue(KEY2C("C"),&edgeType);
		pFirstPlate->GetDesignItemValue(KEY2C("I"),&I);
		pPropList->DeleteAllSonItems(pItem);
		if (d>0 && I==0)
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"P",pFirstPlate,PIT_EDIT,TRUE,-1,TRUE);
			if (edgeType>0)
				InsertOrUpdatePlateParamItem(pPropList,pItem,"R",pFirstPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
	}
	else
		return FALSE;
	return TRUE;
}
//�ֹܹ��߰�
void RebuildOrUpdateStayWirePlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"L",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"N",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"d",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"P",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
}
struct RIB_PLATE
{	//�ְ��������
	static const BYTE TYPE_IN_CORNER = 0;
	static const BYTE TYPE_IN_SLOT  = 1;
	static const BYTE TYPE_ON_ANGLE = 2;
	static const BYTE TYPE_ON_PLATE = 3;
	static const BYTE TYPE_ON_TUBE  = 4;
	BYTE m_cPlateType;	//�߰����� 0.���߰� 1.���߰� 2.�Ǹ��߰� 3.�ְ��߰� 4.�ֹ��߰� 5.Բ���ŶԽ��߰� 6.Բ������໷���õ��߰�
	CLDSNode *m_pDatumNode;
	CLDSBolt *m_pDatumBolt;
	CSuperSmartPtr<CLDSPart> m_pDatumPart,m_pRelaPart1,m_pRelaPart2,m_pVecPart;
	struct tagTUBE_PARA
	{	//��׼����Ϊ�ֹ�ʱʹ��
		int iDatumEdge;				//0.��׼�� 1.������ 2.�����(�ɵ�����������)
		int iWorkNormStyle;			//0.��ֱ�ֹ����췽�� 1.�ֹ����� 2.�Զ���
		BYTE cLeftTopCornerType;	//Y+�����߽����� 0.���� 1.Բ�� 2.ֱ��
		double fLeftTopCornerWidth;	//����ʱ��ʾ�нǿ�ȣ�Բ��ʱ��ʾ�л��뾶
		double fLeftTopCornerHeight;//����ʱ��ʾ�нǸ߶ȣ�Բ��ʱ��ʾ�л��뾶
		BYTE cRightTopCornerType;	//Y-�����߽����� 0.���� 1.Բ�� 2.ֱ��
		double fRightTopCornerWidth;//����ʱ��ʾ�нǿ�ȣ�Բ��ʱ��ʾ�л��뾶
		double fRightTopCornerHeight;//����ʱ��ʾ�нǸ߶ȣ�Բ��ʱ��ʾ�л��뾶
		tagTUBE_PARA() { memset(this,0,sizeof(tagTUBE_PARA)); }
	}tube;
	struct tagPLATE_PARA
	{	//��׼����Ϊ�ְ�ʱʹ��
		BOOL bSpecOrg;				//ָ������ԭ����ָ����λ��׼������ѡһ
		BOOL bSpecAxisX;			//�ְ��߰�ָ��X�᷽����ָ����������˼���ѡһ��
		BOOL bSpecAxisY;			//��װ���б�Ƕȣ���Y����װ����X�����ת���뷽���׼�˼���ѡһ��
		BOOL bOnDatumPlateTopFace;	//��׼����Ϊ�ְ�ʱʹ��
		BOOL bAlongVecPart;			//�ְ����췽���뷽��˼�ͬ��
		BOOL bHashBendPlane;		//�Ƿ���������
		tagPLATE_PARA() { memset(this,0,sizeof(tagPLATE_PARA)); }
	}plate;
	BOOL bOnRelaPlateTopFace;		//һ�鸨���ְ��ʾ�߰�λ�ڸְ嶥��һ�࣬�޸���������ʾ��ֹ����췽����ͬ
	RIB_PLATE() { memset(this,0,sizeof(RIB_PLATE)); }
};
static BOOL InitRibPlatePara(RIB_PLATE &xRibPlate,CLDSParamPlate *pRibPlate)
{
	if (pRibPlate==NULL||pRibPlate->m_iParamType!=TYPE_RIBPLATE)
		return FALSE;
	//��ʼ����λ����
	long hDatumPart=0,hNode=0,hDatumBolt=0,hRelaPart1=0,hRelaPart2=0,hVecPart=0;
	pRibPlate->GetDesignItemValue('A',&hDatumPart);
	pRibPlate->GetDesignItemValue('B',&hNode);
	pRibPlate->GetDesignItemValue('C',&hRelaPart1);
	pRibPlate->GetDesignItemValue('D',&hRelaPart2);
	pRibPlate->GetDesignItemValue('O',&hDatumBolt);
	pRibPlate->GetDesignItemValue('V',&hVecPart);
	xRibPlate.m_pDatumPart=console.FromPartHandle(abs(hDatumPart));
	xRibPlate.m_pDatumNode=console.FromNodeHandle(hNode);
	xRibPlate.m_pDatumBolt=(CLDSBolt*)console.FromPartHandle(hDatumBolt,CLS_BOLT);
	xRibPlate.m_pRelaPart1=console.FromPartHandle(abs(hRelaPart1));
	xRibPlate.m_pRelaPart2=console.FromPartHandle(abs(hRelaPart2));
	xRibPlate.m_pVecPart=console.FromPartHandle(abs(hVecPart));
	if (xRibPlate.m_pRelaPart1.IsNULL()&&xRibPlate.m_pRelaPart1.IsNULL())
	{
		if (xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			xRibPlate.bOnRelaPlateTopFace=(hDatumPart>0);
		else
			xRibPlate.plate.bOnDatumPlateTopFace=(hDatumPart>0);
		//
		if (xRibPlate.m_pVecPart.IsHasPtr()&&xRibPlate.m_pVecPart->IsLinePart())
			xRibPlate.plate.bAlongVecPart=(hVecPart>0);
	}
	else if (xRibPlate.m_pRelaPart1.IsHasPtr())
	{
		xRibPlate.bOnRelaPlateTopFace=(hRelaPart1>0);
		xRibPlate.plate.bOnDatumPlateTopFace=(hDatumPart>0);
	}
	//��ʼ�����β���
	if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
	{	//��׼����Ϊ�ֹ�
		long h=0;
		pRibPlate->GetDesignItemValue('J',&h);
		xRibPlate.tube.iDatumEdge=(int)h;
		pRibPlate->GetDesignItemValue(KEY2C("Nt"),&h);
		xRibPlate.tube.iWorkNormStyle=(int)h;
		pRibPlate->GetDesignItemValue(KEY2C("Lt"),(long*)&xRibPlate.tube.cLeftTopCornerType);
		pRibPlate->GetDesignItemValue(KEY2C("Lw"),&xRibPlate.tube.fLeftTopCornerWidth);
		pRibPlate->GetDesignItemValue(KEY2C("Lh"),&xRibPlate.tube.fLeftTopCornerHeight);
		pRibPlate->GetDesignItemValue(KEY2C("Rt"),(long*)&xRibPlate.tube.cRightTopCornerType);
		pRibPlate->GetDesignItemValue(KEY2C("Rw"),&xRibPlate.tube.fRightTopCornerWidth);
		pRibPlate->GetDesignItemValue(KEY2C("Rh"),&xRibPlate.tube.fRightTopCornerHeight);
	}
	else if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->IsPlate())
	{
		double fValue=0;
		BOOL bSpecOrg=TRUE,bSpecAxisX=TRUE,bSpecAxisY=TRUE;
		bSpecOrg=bSpecOrg&&pRibPlate->GetDesignItemValue(KEY2C("Ox"),&fValue);
		bSpecOrg=bSpecOrg&&pRibPlate->GetDesignItemValue(KEY2C("Oy"),&fValue);
		bSpecAxisX=bSpecAxisX&&pRibPlate->GetDesignItemValue(KEY2C("Xx"),&fValue);
		bSpecAxisX=bSpecAxisX&&pRibPlate->GetDesignItemValue(KEY2C("Xy"),&fValue);
		bSpecAxisY=bSpecAxisY&&pRibPlate->GetDesignItemValue(KEY2C("Ga"),&fValue);
		xRibPlate.plate.bSpecOrg=bSpecOrg;
		xRibPlate.plate.bSpecAxisX=bSpecAxisX;
		xRibPlate.plate.bSpecAxisY=bSpecAxisY;
		//----------VVVVVVVVVVVVVVVVVVVVV-----------��չ���ܺ����������
		CLDSParamPlate::DESIGN_PARAM Ex,Ey,Bx,By,hTipBolt,Tx,Ty,hBendVecPart;
		pRibPlate->GetDesignItemValue(KEY2C("Ex"),&Ex);		//ZO->EΪ�߰��׼�������ʼĩ����
		pRibPlate->GetDesignItemValue(KEY2C("Ey"),&Ey);		//ZO->EΪ�߰��׼�������ʼĩ����
		pRibPlate->GetDesignItemValue(KEY2C("TB"),&hTipBolt); //������ĩ��T��Ļ�׼��˨
		pRibPlate->GetDesignItemValue(KEY2C("Tx"),&Tx);		//������ĩ��T����û����붨��ֵ
		pRibPlate->GetDesignItemValue(KEY2C("Ty"),&Ty);		//������ĩ��T����û����붨��ֵ
		pRibPlate->GetDesignItemValue(KEY2C("BV"),&hBendVecPart);//'E'->TΪ�������߰�����ն˻�����E'->������ĩ��T��
		pRibPlate->GetDesignItemValue(KEY2C("Bx"),&Bx);
		pRibPlate->GetDesignItemValue(KEY2C("By"),&By);
		if ((Tx.bExist&&Ty.bExist)||hTipBolt.bExist)
			xRibPlate.plate.bHashBendPlane=TRUE;
		else
			xRibPlate.plate.bHashBendPlane=FALSE;
	}
	if (xRibPlate.m_pDatumPart.IsNULL())
		return FALSE;
	if (xRibPlate.m_pDatumPart->IsLinePart()&&xRibPlate.m_pRelaPart1.IsNULL()
		&&xRibPlate.m_pRelaPart2.IsNULL()&&xRibPlate.m_pDatumNode==NULL)
	{
		//AfxMessageBox("ȱ���߰嶨λ��׼�ڵ㣡");
		return FALSE;
	}
	if (xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE
		&&(xRibPlate.m_pRelaPart1.IsNULL()||xRibPlate.m_pRelaPart1->GetClassTypeId()==CLS_LINETUBE)
		&&xRibPlate.m_pRelaPart2.IsNULL()&&xRibPlate.m_pDatumNode==NULL)
	{
		//AfxMessageBox("�ְ�а�ȱ�ٶ�λ��׼�ڵ㣡");
		return FALSE;
	}
	//��ʼ���ְ�����
	if (xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsNULL())
		xRibPlate.m_cPlateType=0;			//0.���߰�
	else if (xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsHasPtr())
	{
		xRibPlate.m_cPlateType=1;			//1.���߰�
		BOOL bSpecRayAngle=(pRibPlate->GetDesignItem(KEY2C("Ra"))!=NULL);
		BOOL bSpecRayDist=(pRibPlate->GetDesignItem(KEY2C("Rd"))!=NULL);
		if (bSpecRayAngle)
		{
			if (bSpecRayDist)
				xRibPlate.m_cPlateType=6;
			else
				xRibPlate.m_cPlateType=5;
		}
	}
	else if (xRibPlate.m_pRelaPart1.IsNULL()&&xRibPlate.m_pRelaPart2.IsNULL())
	{
		if (xRibPlate.m_pDatumPart->IsAngle())
			xRibPlate.m_cPlateType=2;		//2.�Ǹ��߰�
		else if (xRibPlate.m_pDatumPart->IsPlate())
			xRibPlate.m_cPlateType=3;		//3.�ְ��߰�
		else if (xRibPlate.m_pDatumPart->IsTube())
			xRibPlate.m_cPlateType=4;		//4.�ֹ��߰�
	}
	return TRUE;
}
//

bool CorrectRibPlatePropItemName(RIB_PLATE &xRibPlate,const char* propKey,CItemInfo* lpInfo)
{
	if (stricmp(propKey,"RibPlate.plate.bSpecAxisX")==0)
	{
		if (xRibPlate.m_cPlateType==3)
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Along Rod|1.Direct to Bolt|2.Direct to Point|3.Input Directly";
		else
			lpInfo->m_cmbItems="0.Calculate Automatically|1.Input Directly";
#else
			lpInfo->m_cmbItems="0.�ظ˼�����|1.ָ����˨|2.�����|3.ֱ������";
		else
			lpInfo->m_cmbItems="0.���򹹼�|1.ֱ������";
#endif
	}
	else if (stricmp(propKey,"RibPlate.plate.bSpecAxisY")==0)
	{
		if (xRibPlate.m_cPlateType!=3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y axis";
			lpInfo->m_cmbItems="0.Direction Rod|1.Inclination Angle";
			lpInfo->m_strPropHelp = "determine axis Y direction, through axis Y direction rod or the inclination angle��degree measure)input.";
#else
			lpInfo->m_strPropName = "Y�᷽��";
			lpInfo->m_cmbItems="0.����˼�|1.��б�Ƕ�";
			lpInfo->m_strPropHelp = "ͨ��Y�᷽��˼����������б�Ƕ�(�Ƕ���)��ȷ��Y�᷽��";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y axis";
			lpInfo->m_cmbItems="0.Vertical Basic Plane|1.Inclination Angle";
			lpInfo->m_strPropHelp = "determine axis Y direction, through inputting inclination angle��degree measure).";
#else
			lpInfo->m_strPropName = "Y�᷽��";
			lpInfo->m_cmbItems="0.��ֱ����|1.��б�Ƕ�";
			lpInfo->m_strPropHelp = "ͨ���������б�Ƕ�(�Ƕ���)��ȷ��Y�᷽��";
#endif
		}
	}
	else if (stricmp(propKey,"RibPlate.plate.bOnRelaPlateTopFace")==0)
	{
		if (xRibPlate.m_pRelaPart1.IsNULL()&&xRibPlate.m_pRelaPart2.IsNULL())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName = "same direction with tube";
			lpInfo->m_strPropHelp = "rib plate's axis X has the same direction with tube extending direction";
#else
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName = "��ֹ�ͬ��";
			lpInfo->m_strPropHelp = "�߰�X�᷽����ֹ����췽��ͬ��";
#endif
		}
		else if (xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsNULL()&&xRibPlate.m_pRelaPart1->IsPlate())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName = "At plate front side";
			lpInfo->m_strPropHelp = "rib plate is on the top side of the supplementary plate";
#else
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName = "λ�ڸ����ְ�����";
			lpInfo->m_strPropHelp = "�߰��ڸ����ְ嶥��һ�ࡣ";
#endif
		}
	}
	else if (stricmp(propKey,"RibPlate.fHeight")==0)
	{
		if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Parameter C";
			lpInfo->m_strPropHelp = "Heigth of step";//top width of rib plate, that is, the length of edge connected with supplementary plate 2, initial value automatically extends to the outer edge of supplementary plate 2.";
#else
			lpInfo->m_strPropName = "̨�׸߶�C";
			lpInfo->m_strPropHelp = "̨�׸߶�";
#endif

		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "top width D";
			lpInfo->m_strPropHelp = "Width of rib plate";
#else
			lpInfo->m_strPropName = "�������D";
			lpInfo->m_strPropHelp = "�߰嶥�����";
#endif
		}
	}
	else if (stricmp(propKey,"RibPlate.fTopWidth")==0)
	{
		if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "top width D";
			lpInfo->m_strPropHelp = "Width of rib plate, that is, the length of edge connected with supplementary plate 2, initial value automatically extends to the outer edge of supplementary plate 2.";
#else
			lpInfo->m_strPropName = "�������D";
			lpInfo->m_strPropHelp = "�߰嶥�����,���븨���ְ�2�����ߵĵı߳�,��ʼֵ�Զ������������ְ�2������Ե��";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Parameter C";
			lpInfo->m_strPropHelp = "Heigth of step";
#else
			lpInfo->m_strPropName = "̨�׸߶�C";
			lpInfo->m_strPropHelp = "̨�׸߶�";
#endif
		}
	}
	else if (stricmp(propKey,"E")==0)
	{
		if (xRibPlate.m_pRelaPart1.IsNULL()&&xRibPlate.m_pRelaPart2.IsNULL())
		{
			if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "rib plate width";
				lpInfo->m_strPropHelp = "Rib plate width, that is, the length of edge which is vertical to datum part.";
#else
				lpInfo->m_strPropName = "�߰���";
				lpInfo->m_strPropHelp = "�߰���,����ֱ�ڻ�׼�����ıߵĳ��ȡ�";
#endif
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "rib plate's height H";
				lpInfo->m_strPropHelp = "height of rib plate, that is, the length of edge which is vertical to datum part.";
#else
				lpInfo->m_strPropName = "�߰�߶�H";
				lpInfo->m_strPropHelp = "�߰�߶�,����ֱ�ڻ�׼�����ıߵĳ��ȡ�";
#endif
			}
		}
		else
		{
			if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "bottom width B";
				lpInfo->m_strPropHelp = "bottom width,that is, length of edge connected with supplementary plate, initial value automatically extends to the outer edge of supplementary plate.";
#else
				lpInfo->m_strPropName = "�ײ����B";
				lpInfo->m_strPropHelp = "���븨���ְ������ߵı߳�,��ʼֵ�Զ����쵽�����ְ�����Ե��";
#endif
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Parameter A";
				lpInfo->m_strPropHelp = "height of rib plate";
#else
				lpInfo->m_strPropName = "�߰峤��A";
				lpInfo->m_strPropHelp = "�븨�����������ߵı߳�,��ʼֵ�Զ������������ְ������Ե";
#endif
			}
		}
	}
	else if (stricmp(propKey,"G")==0)
	{
		if (xRibPlate.m_pRelaPart1.IsNULL()&&xRibPlate.m_pRelaPart2.IsNULL())
		{
			if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "rib plate height";
				lpInfo->m_strPropHelp = "minimum vertical distance in axis Z direction between datum plate's outer surface and rib plate's out edge.";
#else
				lpInfo->m_strPropName = "�߰�߶�";
				lpInfo->m_strPropHelp = "��׼�ֹ���ھ��߰����Ե��СZ����ֱ���롣";
#endif
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "rib plate width W";
				lpInfo->m_strPropHelp = "Parameter W, length of the edge connected with datum part.";
#else
				lpInfo->m_strPropName = "�߰���W";
				lpInfo->m_strPropHelp = "����W, ���׼���������ߵı߳���";
#endif
			}
		}
		else
		{
			if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "rib plate length A";
				lpInfo->m_strPropHelp = "the length of edge connected with datum part, initial value automatically extends to the outer edge of datum plate.";
#else
				lpInfo->m_strPropName = "�߰峤��A";
				lpInfo->m_strPropHelp = "���׼���������ߵı߳�,��ʼֵ�Զ���������׼�ְ������Ե��";
#endif
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Parameter B";
				lpInfo->m_strPropHelp = "Width of bottom";//the length of edge connected with datum part, initial value automatically extends to the outer edge of datum plate.";
#else
				lpInfo->m_strPropName = "�ײ����B";
				lpInfo->m_strPropHelp = "���׼���������ߵı߳�,��ʼֵ�Զ���������׼�ְ������Ե��";
#endif
			}
		}
	}
	else
		return false;
	return true;
}
//�߰�
void RebuildOrUpdateRibPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	RIB_PLATE xRibPlate;
	if (!InitRibPlatePara(xRibPlate,pCurPlate))
		return;
	if (xRibPlate.m_pDatumPart.IsHasPtr()&&xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsNULL()&&
		((xRibPlate.m_pDatumPart->IsTube()&&xRibPlate.m_pRelaPart1->IsPlate())||
		(xRibPlate.m_pDatumPart->IsPlate()&&xRibPlate.m_pRelaPart1->IsTube())))
	{	//�ֹ����ʽ���߰�
		CPropTreeItem* pPropItem=NULL;
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"J",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="��׼��";
		pPropItem->m_lpNodeInfo->m_cmbItems="0.��׼��|1.������";
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"K",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"G",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="�߰�߶�A";
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="�ײ����B";
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="̨�׸߶�C";
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"F",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="�������D";
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="���ǿ��E";
		long CW=0,MH=0;
		pCurPlate->GetDesignItemValue(KEY2C("I"),&CW);
		if (CW>0)
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"e",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"MH",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pCurPlate->GetDesignItemValue(KEY2C("MH"),&MH);
		if (MH>0)
		{
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"MF",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"MG",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		return;
	}
	else if (xRibPlate.m_pDatumPart->IsTube()&&xRibPlate.m_pDatumNode!=NULL&&xRibPlate.m_pRelaPart1.IsNULL())
	{
		CPropTreeItem* pPropItem=NULL;
		//��λ����
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"J",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		if (xRibPlate.tube.iDatumEdge==2)	//ָ��չ������
		{
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"AxisY",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Wx",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Wy",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Wz",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		else
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"K",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		if (xRibPlate.tube.iWorkNormStyle!=0)
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"DX",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"DM",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="����ƫ����M";
		//���β���	
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"G",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="�߰�߶�H";
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="�ײ����W";
		//X+�򶥲��߽�����
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Rt",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		if (xRibPlate.tube.cLeftTopCornerType!=0)	//�нǿ��/�л��뾶
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Rw",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		if (xRibPlate.tube.cLeftTopCornerType==1)	//�нǿ��/�л��뾶
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Rh",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		//X-�򶥲��߽�����
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Lt",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		if (xRibPlate.tube.cRightTopCornerType!=0)	//�нǿ��/�л��뾶
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Lw",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		if (xRibPlate.tube.cRightTopCornerType==1)
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Lh",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		return;
	}
	else if (xRibPlate.m_pDatumPart->IsTube()&&xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsHasPtr())
	{
		CPropTreeItem* pPropItem=NULL;
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"J",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="��׼��";
		pPropItem->m_lpNodeInfo->m_cmbItems="0.��׼��|1.������";
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"K",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		double R=0,r=0;
		pCurPlate->GetDesignItemValue('R',&R);
		pCurPlate->GetDesignItemValue('r',&r);
		if (R>0&&r>0)
		{
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"r",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		else
		{
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"F",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"G",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->SetReadOnly();
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"e",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="���ǿ��1";
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I2",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName="���ǿ��2";
		return;
	}
	else if (xRibPlate.m_pDatumPart->IsTube())
		return;//���������ݲ�֧����ʾ��׼����Ϊ�ֹܵ��߰����
	//�߰����� 0.���߰� 1.���߰� 2.�Ǹ��߰� 3.�ְ��߰� 4.�ֹ��߰�  5.Բ���ŶԽ��߰� 6.Բ������໷���õ��߰�
	BYTE cPlateType=xRibPlate.m_cPlateType;
	CPropTreeItem* pTempItem=NULL;
	if (cPlateType==5||cPlateType==6)
	{
		if (cPlateType==5)
		{
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"F",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		else
		{
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Rd",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		return;
	}
	pTempItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_BUTTON,TRUE,-1,TRUE);		//��λ��׼����
	if (pTempItem!=NULL)	//TODO: ����pTempItem==NULL����� wjh-2016.6.18
		pTempItem->SetReadOnly(TRUE);
	if (0==cPlateType||1==cPlateType)	//0.���߰� //1.���߰�
	{
		pTempItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"C",pCurPlate,PIT_BUTTON,TRUE,-1,TRUE);	//��λ��������1
		if (pTempItem!=NULL)	//TODO: ����pTempItem==NULL����� wjh-2016.6.18
			pTempItem->m_lpNodeInfo->m_buttonType=BDT_COMMONEDIT;
		if (1==cPlateType)				//1.���߰�
		{
			pTempItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_BUTTON,TRUE,-1,TRUE);//��λ��������2
			if (pTempItem!=NULL)	//TODO: ����pTempItem==NULL����� wjh-2016.6.18
				pTempItem->m_lpNodeInfo->m_buttonType=BDT_COMMONEDIT;
		}
	}
	//2.�Ǹ��߰� //4.�ֹ��߰�
	if (2==cPlateType||4==cPlateType)
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_BUTTON,TRUE,-1,TRUE);	//��λ��׼�ڵ�
	if (xRibPlate.m_pDatumPart->IsPlate())
	{	//
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"RibPlate.plate.bOnDatumPlateTopFace",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		//if(!xRibPlate.plate.bSpecAxisX&&xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsNULL()&&xRibPlate.m_pRelaPart1->IsPlate())
		if (xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart2.IsNULL()&&xRibPlate.m_pRelaPart1->IsPlate())
		{
			pTempItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"RibPlate.plate.bOnRelaPlateTopFace",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
			CorrectRibPlatePropItemName(xRibPlate,"RibPlate.plate.bOnRelaPlateTopFace",pTempItem->m_lpNodeInfo);
		}
		CPropTreeItem *pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"RibPlate.plate.bSpecOrg",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
		if (xRibPlate.plate.bSpecOrg&&pPropItem)
		{
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Ox",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Oy",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		else
			InsertOrUpdatePlateParamItem(pPropList,pPropItem,"O",pCurPlate,PIT_BUTTON,TRUE,-1,TRUE);	//��λ��˨
		if (0==cPlateType||1==cPlateType||3==cPlateType)
		{
			pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"RibPlate.plate.bSpecAxisY",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
			if (pPropItem!=NULL)
			{
				CorrectRibPlatePropItemName(xRibPlate,"RibPlate.plate.bSpecAxisY",pPropItem->m_lpNodeInfo);
				if (xRibPlate.plate.bSpecAxisY)
					InsertOrUpdatePlateParamItem(pPropList,pPropItem,"Ga",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
				else if (0==cPlateType||1==cPlateType)
					InsertOrUpdatePlateParamItem(pPropList,pPropItem,"V",pCurPlate,PIT_BUTTON,TRUE,-1,TRUE);
			}
		}
		//
		if (xRibPlate.m_pRelaPart1.IsNULL()&&xRibPlate.m_pRelaPart2.IsNULL())
		{	//�ְ��߰�
			pTempItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			CorrectRibPlatePropItemName(xRibPlate,"E",pTempItem->m_lpNodeInfo);
			pTempItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"G",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			CorrectRibPlatePropItemName(xRibPlate,"G",pTempItem->m_lpNodeInfo);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"X",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);	//X����ƫ�ƾ���
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Z",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);	//Z����ƫ�ƾ���
			if (xRibPlate.plate.bHashBendPlane)
			{
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"dx",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);//X����ƫ�ƾ���
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"dz",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);//Z����ƫ�ƾ���
			}
			else
			{
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("dx",pCurPlate->m_iParamType));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("dz",pCurPlate->m_iParamType));
			}
		}
		//���߰����߰�
		else if (xRibPlate.m_pRelaPart1.IsHasPtr()&&xRibPlate.m_pRelaPart1->IsPlate())
		{
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"El",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
			pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			long liSideEdgeLength=0;
			if (pCurPlate->GetDesignItemValue(KEY2C("El"),&liSideEdgeLength)&&liSideEdgeLength!=0)
				pPropItem->m_lpNodeInfo->m_strPropName="��߸߶�A";
			else
				pPropItem->m_lpNodeInfo->m_strPropName="��߳���A";
			pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"G",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			pPropItem->SetReadOnly(1==xRibPlate.m_cPlateType);	//����(Ϊ���ְ�а�ʱ�ó����Զ�����)
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"F",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);	//��λ��������1
			if (1==cPlateType)
			{
				InsertOrUpdatePlateParamItem(pPropList,pParentItem,"e",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
				pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
				pPropItem->m_lpNodeInfo->m_strPropName="���ǿ��1";
				pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I2",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
				pPropItem->m_lpNodeInfo->m_strPropName="���ǿ��2";
			}
			else
			{
				long CW=0;
				pCurPlate->GetDesignItemValue(KEY2C("I"),&CW);
				pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);		//�нǿ��
				if (CW>0)	//���ǿ��E>0
					InsertOrUpdatePlateParamItem(pPropList,pPropItem,"e",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
			}
			long MH=0;
			pCurPlate->GetDesignItemValue(KEY2C("MH"),&MH);
			CPropTreeItem *pMHItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"MH",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			if (MH>0)
			{
				InsertOrUpdatePlateParamItem(pPropList,pMHItem,"MF",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
				InsertOrUpdatePlateParamItem(pPropList,pMHItem,"MG",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
			}
			else
			{
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("MF",pCurPlate->m_iParamType));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("MG",pCurPlate->m_iParamType));
			}
		}
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"Z",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);		//Z����ƫ�ƾ���
	}
}
BOOL ModifyRibPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	long h=0;
	CLDSParamPlate* pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("RibPlate.plate.bOnDatumPlateTopFace",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			pParamPlate->GetDesignItemValue('A',&h);
			if (valueStr.CompareNoCase("��")==0)
				pParamPlate->SetDesignItemValue('A',abs(h),NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
			else
				pParamPlate->SetDesignItemValue('A',abs(h)*-1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("RibPlate.plate.bSpecOrg",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		BOOL bSpecOrg=valueStr[0]-'0';
		pPropList->DeleteAllSonItems(pItem);
		if (bSpecOrg)
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Ox",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Oy",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		else
			InsertOrUpdatePlateParamItem(pPropList,pItem,"O",pParamPlate,PIT_BUTTON,TRUE,-1,TRUE);
		//
		double fValue=0;
		long hBolt=0;
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			BOOL bSpecOrgX=pParamPlate->GetDesignItemValue(KEY2C("Ox"),&fValue);
			BOOL bSpecOrgY=pParamPlate->GetDesignItemValue(KEY2C("Oy"),&fValue);
			pParamPlate->GetDesignItemValue(KEY2C("O"),&hBolt);
			if (bSpecOrg)
			{
				if (!bSpecOrgX)
					pParamPlate->SetDesignItemValue(KEY2C("Ox"),fValue);
				if (!bSpecOrgY)
					pParamPlate->SetDesignItemValue(KEY2C("Oy"),fValue);
				pParamPlate->SetDesignItemValue(KEY2C("O"),0,NULL,0,1);
			}
			else
			{
				if (bSpecOrgX)
					pParamPlate->DeleteDesignItem(KEY2C("Ox"));
				if (bSpecOrgY)
					pParamPlate->DeleteDesignItem(KEY2C("Oy"));
				pParamPlate->SetDesignItemValue(KEY2C("O"),hBolt,NULL,0,1);
			}
		}
	}
	else if (CLDSParamPlate::GetPropID("Ox",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double fValue=atof(valueStr);
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pParamPlate->SetDesignItemValue(KEY2C("Ox"),fValue);
	}
	else if (CLDSParamPlate::GetPropID("Oy",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double fValue=atof(valueStr);
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pParamPlate->SetDesignItemValue(KEY2C("Oy"),fValue);
	}
	else if (CLDSParamPlate::GetPropID("Ga",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double fValue=atof(valueStr);
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pParamPlate->SetDesignItemValue(KEY2C("Ga"),fValue);
	}
	else if (CLDSParamPlate::GetPropID("RibPlate.plate.bSpecAxisX",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double fValue=0;
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			BOOL bSpecX=pParamPlate->GetDesignItemValue(KEY2C("Xx"),&fValue);
			BOOL bSpecY=pParamPlate->GetDesignItemValue(KEY2C("Xy"),&fValue);
			if (valueStr[0]-'0'==1)
			{
				if (!bSpecX)
					pParamPlate->SetDesignItemValue(KEY2C("Xx"),fValue);
				if (!bSpecY)
					pParamPlate->SetDesignItemValue(KEY2C("Xy"),fValue);
			}
			else
			{
				if (bSpecX)
					pParamPlate->DeleteDesignItem(KEY2C("Xx"));
				if (bSpecY)
					pParamPlate->DeleteDesignItem(KEY2C("Xy"));
			}
		}
	}
	else if (CLDSParamPlate::GetPropID("RibPlate.plate.bSpecAxisY",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		BOOL bSpecAxisY=valueStr[0]-'0';
		pPropList->DeleteAllSonItems(pItem);
		if (bSpecAxisY)
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Ga",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
		else
			InsertOrUpdatePlateParamItem(pPropList,pItem,"V",pParamPlate,PIT_BUTTON,TRUE,-1,TRUE);
		double fValue=0;
		long hVicePart=0;
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			BOOL bSpecY=pParamPlate->GetDesignItemValue(KEY2C("Ga"),&fValue);
			if (!bSpecY)	//���޳�ʼֵ����ȡĬ��ֵ90�� wjh-2019.7.30
				fValue=90;
			pParamPlate->GetDesignItemValue(KEY2C("V"),&hVicePart);
			if (bSpecAxisY)
			{
				if (!bSpecY)
					pParamPlate->SetDesignItemValue(KEY2C("Ga"),fValue);
				pParamPlate->SetDesignItemValue(KEY2C("V"),0,NULL,0,1);
			}
			else if (bSpecY)
			{
				pParamPlate->DeleteDesignItem(KEY2C("Ga"));
				pParamPlate->SetDesignItemValue(KEY2C("V"),hVicePart,NULL,0,1);
			}
		}
	}
	else if (CLDSParamPlate::GetPropID("Ga",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double fValue=atof(valueStr);
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pParamPlate->GetDesignItemValue(KEY2C("Ga"),&fValue);
	}
	else if (CLDSParamPlate::GetPropID("RibPlate.plate.bAlongVecPart",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			pParamPlate->GetDesignItemValue('V',&h);
			if (valueStr.CompareNoCase("��")==0)
				pParamPlate->SetDesignItemValue('V',abs(h),NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
			else
				pParamPlate->SetDesignItemValue('V',abs(h)*-1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("RibPlate.plate.bOnRelaPlateTopFace",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		long hDatumPart=0,hRelaPart1=0,hRelaPart2=0;
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			pParamPlate->GetDesignItemValue('A',&hDatumPart);
			pParamPlate->GetDesignItemValue('C',&hRelaPart1);
			pParamPlate->GetDesignItemValue('D',&hRelaPart2);
			CLDSPart *pDatumPart=pParamPlate->BelongModel()->FromPartHandle(abs(hDatumPart));
			CLDSPart *pRelaPart1=pParamPlate->BelongModel()->FromPartHandle(abs(hRelaPart1));
			CLDSPart *pRelaPart2=pParamPlate->BelongModel()->FromPartHandle(abs(hRelaPart2));
			if (pRelaPart1==NULL&&pRelaPart2==NULL)
			{
				if (pDatumPart&&pDatumPart->GetClassTypeId()==CLS_LINETUBE)
				{
					if (valueStr.CompareNoCase("��")==0)
						pParamPlate->SetDesignItemValue('A',abs(hDatumPart),NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
					else
						pParamPlate->SetDesignItemValue('A',abs(hDatumPart)*-1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
				}
			}
			else if (pRelaPart1)
			{
				if (valueStr.CompareNoCase("��")==0)
					pParamPlate->SetDesignItemValue('C',abs(hRelaPart1),NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
				else
					pParamPlate->SetDesignItemValue('C',abs(hRelaPart1)*-1,NULL,PARAM_POS,DESIGN_PARAM_ITEM::TYPE_HANDLE,TRUE);
			}
		}
	}
	else if (CLDSParamPlate::GetPropID("MH",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		long MH=atoi(valueStr);
		pParamPlate->SetDesignItemValue(KEY2C("MH"),MH);
		if (MH==0)
			pPropList->DeleteAllSonItems(pItem);
		else
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"MF",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"MG",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("Lt",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		long Lt=valueStr[0]-'0';
		pParamPlate->SetDesignItemValue(KEY2C("Lt"),Lt,NULL,PARAM_SHAPE,1,TRUE);
		pPropList->DeleteAllSonItems(pItem);
		if (Lt!=0)
		{
			CPropTreeItem* pSonItem=InsertOrUpdatePlateParamItem(pPropList,pItem,"Lw",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			if (Lt==2)
				pSonItem->m_lpNodeInfo->m_strPropName="�л��뾶";
		}
		if (Lt==1)
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Lh",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
	else if (CLDSParamPlate::GetPropID("Rt",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		long Rt=valueStr[0]-'0';
		pParamPlate->SetDesignItemValue(KEY2C("Rt"),Rt,NULL,PARAM_SHAPE,1,TRUE);
		pPropList->DeleteAllSonItems(pItem);
		if (Rt!=0)
		{
			CPropTreeItem* pSonItem=InsertOrUpdatePlateParamItem(pPropList,pItem,"Rw",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			if (Rt==2)
				pSonItem->m_lpNodeInfo->m_strPropName="�л��뾶";
		}
		if (Rt==1)
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Rh",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
	else if (CLDSParamPlate::GetPropID("I",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//���ǿ��E
		int CW=atoi(valueStr);
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType==CLDSParamPlate::TYPE_RIB_ELBOW_PLATE||iSubType==CLDSParamPlate::TYPE_RIB_CORNER_PLATE||iSubType==CLDSParamPlate::TYPE_RIB_SLOT_PLATE)
				pParamPlate->SetDesignItemValue(KEY2C("I"),CW,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER,TRUE);
		}
		CLDSParamPlate* pFirstParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();
		if (pFirstParamPlate->GetRibPlateSubType()!=CLDSParamPlate::TYPE_RIB_SLOT_PLATE)
		{
			if (CW==0)
				pPropList->DeleteAllSonItems(pItem);
			else
				InsertOrUpdatePlateParamItem(pPropList,pItem,"e",pFirstParamPlate,PIT_COMBO,TRUE,-1,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("I2",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//���ǿ��2
		int CW=atoi(valueStr);
		for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType==CLDSParamPlate::TYPE_RIB_SLOT_PLATE)
				pParamPlate->SetDesignItemValue(KEY2C("I2"),CW,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("e",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//����ģʽ
		int iType=valueStr[0]-'0';
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType==CLDSParamPlate::TYPE_RIB_ELBOW_PLATE||iSubType==CLDSParamPlate::TYPE_RIB_CORNER_PLATE||iSubType==CLDSParamPlate::TYPE_RIB_SLOT_PLATE)
				pParamPlate->SetDesignItemValue(KEY2C("e"),iType,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("J",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//�߰�չ������������
		int iType=valueStr[0]-'0';
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType==CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE||iSubType==CLDSParamPlate::TYPE_RIB_SLOT_PLATE||
				iSubType==CLDSParamPlate::TYPE_RIB_ELBOW_PLATE)
				pParamPlate->SetDesignItemValue(KEY2C("J"),iType,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER,TRUE);
		}
		//
		CLDSParamPlate* pFirstParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();
		pPropList->DeleteAllSonItems(pItem);
		if (iType==2)
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"AxisY",pFirstParamPlate,PIT_COMBO,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Wx",pFirstParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Wy",pFirstParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"Wz",pFirstParamPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
		else
			InsertOrUpdatePlateParamItem(pPropList,pItem,"K",pFirstParamPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
	else if (CLDSParamPlate::GetPropID("AxisY",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//�Զ����߰�չ������
		f3dPoint spread_vec=CStdVector::GetVector(valueStr[0]-'0');
		if (!spread_vec.IsZero())
		{
			for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			{
				int iSubType=pParamPlate->GetRibPlateSubType();
				if (iSubType!=CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE)	//�ֹ��߰�
					continue;
				pParamPlate->SetDesignItemValue(KEY2C("Wx"),spread_vec.x,NULL,PARAM_POS);
				pParamPlate->SetDesignItemValue(KEY2C("Wy"),spread_vec.y,NULL,PARAM_POS);
				pParamPlate->SetDesignItemValue(KEY2C("Wz"),spread_vec.z,NULL,PARAM_POS);
			}
			//
			CXhChar16 sText;
			CLDSParamPlate* pFirstParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();
			sprintf(sText,"%f",spread_vec.x);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("Wx",pFirstParamPlate->m_iParamType),sText);
			sprintf(sText,"%f",spread_vec.y);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("Wy",pFirstParamPlate->m_iParamType),sText);
			sprintf(sText,"%f",spread_vec.z);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("Wz",pFirstParamPlate->m_iParamType),sText);
		}
	}
	else if (CLDSParamPlate::GetPropID("Wx",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double Wx=atof(valueStr);
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType!=CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE)	//�ֹ��߰�
				pParamPlate->SetDesignItemValue(KEY2C("Wx"),Wx,NULL,PARAM_POS);
		}
	}
	else if (CLDSParamPlate::GetPropID("Wy",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double Wy=atof(valueStr);
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType!=CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE)	//�ֹ��߰�
				pParamPlate->SetDesignItemValue(KEY2C("Wy"),Wy,NULL,PARAM_POS);
		}
	}
	else if (CLDSParamPlate::GetPropID("Wz",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double Wz=atof(valueStr);
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType!=CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE)	//�ֹ��߰�
				pParamPlate->SetDesignItemValue(KEY2C("Wz"),Wz,NULL,PARAM_POS);
		}
	}
	else if (CLDSParamPlate::GetPropID("El",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		long liSideEdgeLengthByHeight=valueStr[0]-'0';
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			int iSubType=pParamPlate->GetRibPlateSubType();
			if (iSubType==CLDSParamPlate::TYPE_RIB_CORNER_PLATE)	//���߰�
				pParamPlate->SetDesignItemValue(KEY2C("El"),liSideEdgeLengthByHeight,NULL,PARAM_SHAPE,2);
		}
		CLDSParamPlate* pFirstParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
		CPropTreeItem* pFindItem=NULL;
		long idRelaItem=CLDSParamPlate::GetPropID("E",pFirstParamPlate->m_iParamType);
		if ((pFindItem=pPropList->FindItemByPropId(idRelaItem,NULL))!=NULL)
		{
			CString szPropName=(liSideEdgeLengthByHeight!=0)?"��߸߶�A":"��߳���A";
			pPropList->SetItemPropName(idRelaItem,szPropName);
		}
	}
	else
		return FALSE;
	return TRUE;
}
//�˰�
void RebuildOrUpdateStationConnPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem* pItem=NULL;
	pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"LP",pCurPlate,PIT_COMBO);
	pItem->SetReadOnly();
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_COMBO);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"C",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"r",pCurPlate,PIT_EDIT);
}
BOOL ModifyStationConnPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	CXhChar100 tem_str("%s",valueStr);
	KEY2C paramKey=CLDSParamPlate::GetPropKey(pItem->m_idProp,pPlate->m_iParamType);
	if (KEY2C("I")==paramKey)
	{	//�װ���������
		long iType=valueStr[0]-'0';
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(paramKey,iType);
	}
	else
		return FALSE;
	return TRUE;
}
//֧���а�
void RebuildOrUpdateStationShoePlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem *pPropItem=NULL;
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"LB",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly();
	long hAssistPlate=0;
	pCurPlate->GetDesignItemValue(KEY2C("LE"),&hAssistPlate);
	CLDSParamPlate* pEndPlate=(CLDSParamPlate*)console.FromPartHandle(hAssistPlate,CLS_PARAMPLATE);
	if (pEndPlate && pEndPlate->m_iParamType==TYPE_STATIONCONNPLATE)
	{
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("H",pCurPlate->m_iParamType));
		pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"LE",pCurPlate,PIT_EDIT);
		pPropItem->SetReadOnly();
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT);
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("LE",pCurPlate->m_iParamType));
		pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("R",pCurPlate->m_iParamType));
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
	}
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"CR",pCurPlate,PIT_EDIT);
}
BOOL ModifyStationShoePlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	return FALSE;
}
//�˳�����
void RebuildOrUpdateStationLinkPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem *pPropItem=NULL;
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly();
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly();
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"C",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly();
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly();
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"OW",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"OB",pCurPlate,PIT_EDIT);
}
BOOL ModifyStationLinkPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	return FALSE;
}
//�ֹ����
void RebuildOrUpdateElbowPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem *pPropItem=NULL;
	long MH=0,CW=0;
	pCurPlate->GetDesignItemValue(KEY2C("H"),&MH);
	pCurPlate->GetDesignItemValue(KEY2C("E"),&CW);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"C",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	if (CW>0)
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"e",pCurPlate,PIT_COMBO,TRUE,-1,TRUE);
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	if (MH>0)
	{
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"F",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"G",pCurPlate,PIT_EDIT,TRUE,-1,TRUE);
	}
}
BOOL ModifyElbowPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("H",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		long MH=atoi(valueStr);
		pParamPlate->SetDesignItemValue(KEY2C("H"),MH);
		if (MH==0)
			pPropList->DeleteAllSonItems(pItem);
		else
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"F",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"G",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
	}
	else if (CLDSParamPlate::GetPropID("I",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//���ǿ��E
		int CW=atoi(valueStr);
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pParamPlate->SetDesignItemValue(KEY2C("I"),CW,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER,TRUE);
		//����������
		CLDSParamPlate* pFirstParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();
		if (CW==0)
			pPropList->DeleteAllSonItems(pItem);
		else
			InsertOrUpdatePlateParamItem(pPropList,pItem,"e",pFirstParamPlate,PIT_COMBO,TRUE,-1,TRUE);
	}
	else if (CLDSParamPlate::GetPropID("e",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//����ģʽ
		int iType=valueStr[0]-'0';
		for (pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pParamPlate->SetDesignItemValue(KEY2C("e"),iType,NULL,PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_INTEGER,TRUE);
	}
	else
		return FALSE;
	return TRUE;
}
//�˳Ų��
void RebuildOrUpdateStationCrossPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	long I=0;
	pCurPlate->GetDesignItemValue(KEY2C("I"),&I);
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"LP",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly();
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"I",pCurPlate,PIT_COMBO);
	if (I==0||I==3)
		pPropItem->m_lpNodeInfo->m_cmbItems="X��ͨ��|Y��ͨ��";
	else
	{
		pPropItem->m_lpNodeInfo->m_cmbItems="X+���|X-���|Y+���|Y-���";
		pSonPropItem=InsertOrUpdatePlateParamItem(pPropList,pPropItem,"LA",pCurPlate,PIT_EDIT);
		pSonPropItem->SetReadOnly();
	}
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
}
BOOL ModifyStationCrossPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("I",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		int iVecType=0;
		if (valueStr.CompareNoCase("X��ͨ��")==0)
			iVecType=0;
		else if (valueStr.CompareNoCase("Y��ͨ��")==0)
			iVecType=3;
		else if (valueStr.CompareNoCase("X+���")==0)
			iVecType=1;
		else if (valueStr.CompareNoCase("X-���")==0)
			iVecType=2;
		else if (valueStr.CompareNoCase("Y+���")==0)
			iVecType=4;
		else if (valueStr.CompareNoCase("Y-���")==0)
			iVecType=5;
		pParamPlate->SetDesignItemValue(KEY2C("I"),iVecType);
	}
	else
		return FALSE;
	return TRUE;
}
//��ˮ��
void RebuildOrUpdateWaterShieldedPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	double angle=0;
	pCurPlate->GetDesignItemValue(KEY2C("A"),&angle);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"E",pCurPlate,PIT_COMBO);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"S",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
	CPropTreeItem *pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
	if (angle>0)
	{
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"O",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"L",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"K",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pPropItem,"H",pCurPlate,PIT_EDIT);
	}
}
BOOL ModifyWaterShieldedPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("E",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		int iType=valueStr[0]-'0';
		for (CLDSParamPlate* pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(KEY2C("E"),iType);
	}
	else if (CLDSParamPlate::GetPropID("A",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		double angle=atof(valueStr);
		for (CLDSParamPlate* pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->SetDesignItemValue(KEY2C("A"),angle);
		//
		pPropList->DeleteAllSonItems(pItem);
		if (angle>0)
		{
			InsertOrUpdatePlateParamItem(pPropList,pItem,"O",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"L",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"K",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
			InsertOrUpdatePlateParamItem(pPropList,pItem,"H",pParamPlate,PIT_EDIT,TRUE,-1,TRUE);
		}
	}
	else
		return FALSE;
	return TRUE;
}
//������
void RebuildOrUpdateCircularCoverPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	long iCoverPlateType=0;
	pCurPlate->GetDesignItemValue('Y',&iCoverPlateType);
	if (iCoverPlateType==3)
	{
		CPropTreeItem* pItem=NULL;
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		pItem->m_lpNodeInfo->m_strPropName="�����W";
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"N",pCurPlate,PIT_EDIT);
		pItem->m_lpNodeInfo->m_strPropName="�����H";
	}
	else
	{
		if (iCoverPlateType==0)
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
		if (iCoverPlateType!=0)
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"N",pCurPlate,PIT_EDIT);
	}
}
BOOL ModifyCircularCoverPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	return FALSE;
}
//�����߰�
void RebuildOrUpdateCircularRibPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem* pItem=NULL;
	int iSubType=pCurPlate->GetCircularPlateSubType();
	if (pCurPlate->m_iParamType==TYPE_TUBERIBPLATE)
	{	//�ֹ��߰壨��Խ���壩
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"N",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"h",pCurPlate,PIT_EDIT);
	}
	else if (iSubType==CLDSParamPlate::TYPE_SPECIAL_CIR_PLATE)			//���ͷ���
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"r",pCurPlate,PIT_EDIT);
	}
	else if (iSubType==CLDSParamPlate::TYPE_COMMON_CIR_PLATE)	//��ͨ�����߰�
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT);
	}
	else if (iSubType==CLDSParamPlate::TYPE_3WL_CIR_PLATE)
	{
		double D=0;
		pCurPlate->GetDesignItemValue('D',&D);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"N",pCurPlate,PIT_EDIT);
		if (D>0)
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
		else
		{
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"h",pCurPlate,PIT_EDIT);
		}
	}
	else if (iSubType==CLDSParamPlate::TYPE_3WA_CIR_PLATE)
	{
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
		pItem->m_lpNodeInfo->m_strPropName="���δ���W";
	}
	else if (iSubType==CLDSParamPlate::TYPE_2WL_CIR_PLATE)
	{
		double angle=0;
		pCurPlate->GetDesignItemValue('B',&angle);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
		pItem->m_lpNodeInfo->m_strPropName="̨�ڸ߶�H";
		if (angle>0)
		{
			pItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"h",pCurPlate,PIT_EDIT);
			pItem->m_lpNodeInfo->m_strPropName="̨�ڸ߶�h";
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
		}
		else
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"D",pCurPlate,PIT_EDIT);
	}
	else if (iSubType==CLDSParamPlate::TYPE_2WA_CIR_PLATE)
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"y",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
	}
	else if (iSubType==CLDSParamPlate::TYPE_CIR_AISLE_PLATE)
	{
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"B",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
		InsertOrUpdatePlateParamItem(pPropList,pParentItem,"ES",pCurPlate,PIT_EDIT);
	}
	if (iSubType!=CLDSParamPlate::TYPE_CIR_AISLE_PLATE)
	{
		double offsetDist=0;
		pCurPlate->GetDesignItemValue('T',&offsetDist);
		if (offsetDist>0)
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"T",pCurPlate,PIT_EDIT);
		else
			InsertOrUpdatePlateParamItem(pPropList,pParentItem,"S",pCurPlate,PIT_EDIT);
	}
}
BOOL ModifyCircularRibPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	bool processed=false;
	CLDSLinePart* pRod=NULL;
	for (CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();pParamPlate!=NULL;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
	{
		KEY2C key=CLDSParamPlate::GetPropKey(pItem->m_idProp,pParamPlate->m_iParamType);
		if (key=='S'&&(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)&&
			(pRod=pParamPlate->BelongModel()->FromRodHandle(pParamPlate->m_hBaseRod))!=NULL)
		{
			GEPOINT vDatumAxis=pRod->End()-pRod->Start();
			double S=atof(valueStr);
			if (vDatumAxis*pParamPlate->ucs.axis_z<0)
				S*=-1;
			pParamPlate->SetDesignItemValue(key,S,"����ƫ����",PARAM_POS,0,TRUE);
			processed=true;
		}
		else if (key=='d'&& pParamPlate->GetCircularPlateSubType()==CLDSParamPlate::TYPE_SPECIAL_CIR_PLATE)
		{
			long hTube=0;
			pParamPlate->GetDesignItemValue('C',&hTube);
			CLDSLineTube* pTube=(CLDSLineTube*)console.FromPartHandle(hTube,CLS_LINETUBE);
			if (pTube)
			{
				double d=atof(valueStr);
				double r=pTube->GetDiameter()*0.5-pTube->GetThick()-d;
				pParamPlate->SetDesignItemValue('r',r,"",PARAM_POS,0,TRUE);
				processed=true;
			}
		}
	}
	return processed;
}
void RebuildOrUpdateFLPlateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	CPropTreeItem* pPropItem=NULL;
	pPropItem=InsertOrUpdatePlateParamItem(pPropList,pParentItem,"A",pCurPlate,PIT_EDIT);
	pPropItem->SetReadOnly(TRUE);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"H",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"W",pCurPlate,PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList,pParentItem,"R",pCurPlate,PIT_EDIT);
}
BOOL ModifyFLPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("H",pParamPlate->m_iParamType)==pItem->m_idProp)
	{
		for (CLDSParamPlate* pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			if (pPlate->m_iParamType!=TYPE_FL)
				continue;
			pPlate->SetDesignItemValue('H',atof(valueStr));
			//�޸�H����������ݸֹܶ�ͷ������¼��㷨����λ��
			CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
			if (pLineTube)
			{
				if (pLineTube->m_tJointEnd.hLinkObj==pPlate->handle)
					pLineTube->DesignEndJointUCS(pPlate);
				else if (pLineTube->m_tJointStart.hLinkObj==pPlate->handle)
					pLineTube->DesignStartJointUCS(pPlate);
			}
		}
		return TRUE;
	}
	return FALSE;
}
void RebuildOrUpdatePedalPlateParamsPropertyList(CPropertyList* pPropList, CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate == NULL)
		return;
	CPropTreeItem* pPropItem = NULL;
	pPropItem = InsertOrUpdatePlateParamItem(pPropList, pParentItem, "A", pCurPlate, PIT_EDIT);
	pPropItem->SetReadOnly(TRUE);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "V", pCurPlate, PIT_COMBO);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "C", pCurPlate, PIT_COMBO);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "S", pCurPlate, PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "L", pCurPlate, PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "W", pCurPlate, PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "H", pCurPlate, PIT_EDIT);
	InsertOrUpdatePlateParamItem(pPropList, pParentItem, "D", pCurPlate, PIT_EDIT);
}
BOOL ModifyPedalPlateParamProperty(CPropertyList *pPropList, CPropTreeItem* pItem, CString &valueStr, HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum() < 0)
		return FALSE;
	long type = valueStr[0] - '0';
	CLDSParamPlate *pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("V", pParamPlate->m_iParamType) == pItem->m_idProp)
	{
		for (CLDSParamPlate* pPlate = (CLDSParamPlate*)selectObjs.GetFirst(); pPlate; pPlate = (CLDSParamPlate*)selectObjs.GetNext())
		{
			if (pPlate->m_iParamType != TYPE_PEDALPLATE)
				continue;
			pPlate->SetDesignItemValue('V', type);
		}
		return TRUE;
	}
	else if (CLDSParamPlate::GetPropID("C", pParamPlate->m_iParamType) == pItem->m_idProp)
	{
		for (CLDSParamPlate* pPlate = (CLDSParamPlate*)selectObjs.GetFirst(); pPlate; pPlate = (CLDSParamPlate*)selectObjs.GetNext())
		{
			if (pPlate->m_iParamType != TYPE_PEDALPLATE)
				continue;
			pPlate->SetDesignItemValue('C', type);
		}
		//
		CLDSView::PROMPT_LIFE lifeprompt(true);
		if (type == 0)
			lifeprompt.SetPromptPicture(IDB_BMP_PEDAL_PLATE1);
		else
			lifeprompt.SetPromptPicture(IDB_BMP_PEDAL_PLATE2);
		return TRUE;
	}
	return FALSE;
}
//
void RebuildOrUpdateParamsPropertyList(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pCurPlate==NULL)
		return;
	pPropList->DeleteAllSonItems(pParentItem);
	if (pCurPlate->m_iParamType==TYPE_BASEPLATE)
		RebuildOrUpdateBasePlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_SHOEPLATE)
		RebuildOrUpdateShoePlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
		RebuildOrUpdateAngleRibPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_TUBESTAYWIREPLATE)
		RebuildOrUpdateStayWirePlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_RIBPLATE)
		RebuildOrUpdateRibPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_STATIONCONNPLATE)
		RebuildOrUpdateStationConnPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_STATIONSHOEPLATE)
		RebuildOrUpdateStationShoePlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_STATIONLINKPLATE)
		RebuildOrUpdateStationLinkPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_ELBOWPLATE)	//���
		RebuildOrUpdateElbowPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_STATIONCROSSPLATE)
		RebuildOrUpdateStationCrossPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
		RebuildOrUpdateWaterShieldedPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		RebuildOrUpdateCircularCoverPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_CIRRIBPLATE||pCurPlate->m_iParamType==TYPE_TUBERIBPLATE)
		RebuildOrUpdateCircularRibPlateParamsPropertyList(pPropList,pParentItem);
	else if (pCurPlate->m_iParamType==TYPE_FL)
		RebuildOrUpdateFLPlateParamsPropertyList(pPropList,pParentItem);
	else if(pCurPlate->m_iParamType==TYPE_PEDALPLATE)
		RebuildOrUpdatePedalPlateParamsPropertyList(pPropList, pParentItem);
}
BOOL ModifyParamPlateParamProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr,HASHOBJSET &selectObjs)
{
	CLDSParamPlate *pFirstPlate=(CLDSParamPlate*)selectObjs.GetFirst();
	CLDSParamPlate *pPlate=pFirstPlate;
	if (pPlate==NULL)
		return FALSE;
	BOOL bRetCode=FALSE;
	if (pPlate->m_iParamType==TYPE_BASEPLATE)
		bRetCode=ModifyBasePlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_SHOEPLATE)
		bRetCode=ModifyShoePlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
		bRetCode=ModifyAngleRibPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_RIBPLATE)
		bRetCode=ModifyRibPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_STATIONCONNPLATE)
		bRetCode=ModifyStationConnPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_STATIONSHOEPLATE)
		bRetCode=ModifyStationShoePlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_STATIONLINKPLATE)
		bRetCode=ModifyStationLinkPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_ELBOWPLATE)
		bRetCode=ModifyElbowPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_STATIONCROSSPLATE)
		bRetCode=ModifyStationCrossPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
		bRetCode=ModifyWaterShieldedPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		bRetCode=ModifyCircularCoverPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_CIRRIBPLATE||pPlate->m_iParamType==TYPE_TUBERIBPLATE)
		bRetCode=ModifyCircularRibPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if (pPlate->m_iParamType==TYPE_FL)
		bRetCode=ModifyFLPlateParamProperty(pPropList,pItem,valueStr,selectObjs);
	else if(pPlate->m_iParamType==TYPE_PEDALPLATE)
		bRetCode = ModifyPedalPlateParamProperty(pPropList, pItem, valueStr, selectObjs);
	if (!bRetCode)
	{	//�����������޸Ĳ���ֵ
		KEY2C paramKey=CLDSParamPlate::GetPropKey(pItem->m_idProp,pPlate->m_iParamType);
		DESIGN_PLATE_ITEM *pDesignItem=pPlate->GetDesignItem(paramKey);
		if (pDesignItem)
		{
			bRetCode=TRUE;
			if (0==pDesignItem->dataType)
			{	//1.������
				pDesignItem->value.fValue=atof(valueStr);
				for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
					pPlate->SetDesignItemValue(paramKey,pDesignItem->value.fValue,pDesignItem->sNotes,pDesignItem->paramType,pDesignItem->dataType,TRUE);
			}
			else if (1==pDesignItem->dataType)
			{	//2.���
				pDesignItem->value.hPart=HexStrToLong(valueStr);
				for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
					pPlate->SetDesignItemValue(paramKey,pDesignItem->value.hPart,pDesignItem->sNotes,pDesignItem->paramType,pDesignItem->dataType,TRUE);
			}
			else if (2==pDesignItem->dataType)
			{	//3.����
				pDesignItem->value.iType=atoi(valueStr);
				for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
					pPlate->SetDesignItemValue(paramKey,pDesignItem->value.iType,pDesignItem->sNotes,pDesignItem->paramType,pDesignItem->dataType,TRUE);
			}
			else
				bRetCode=FALSE;
		}
		else
		{	//����PROPLIST_ITEM��Ӹְ���δ������������� wht 15-07-06
			PROPLIST_ITEM *pPropListItem=NULL;
			DYNA_PROPITEM_GROUP *pGroup=CLDSParamPlate::hashParamItemPropGroup.GetValue(pPlate->m_iParamType);
			if (pGroup)
				pPropListItem=pGroup->GetPropItem(pItem->m_idProp);
			bRetCode=TRUE;
			if (pPropListItem)
				pPlate->SetDesignItemValue(paramKey,atoi(valueStr));
			else
				bRetCode=FALSE;
		}
	}
	if (bRetCode)
	{	//�޸Ĳ���֮���������ò�������
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
			pPlate->DesignPlate();
		KEY2C key=CLDSParamPlate::GetPropKey(pItem->m_idProp,pFirstPlate->m_iParamType);
		RedrawPlateParamDrawing(pFirstPlate,key.Label());//��������DesignPlate֮�󣬷������ɳ�ʼ����ʾ��ͼλ���д�wjh-2016.11.1
		CLDSView::RedrawPlateProfileDrawing(pFirstPlate);
		//������ص�Լ�����ƶ���
		long idDbProp=CLDSParamPlate::GetDbPropId(key,pFirstPlate->m_iParamType);
		CHashSet<CLDSDbObject*> hashUpdateObjs;
		for (pPlate=(CLDSParamPlate*)selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
		{
			Ta.ClearConstraintSyncStates();	//���ڹ����빹�����Լ����������״���������ȫ������ͬ��״̬
			pPlate->ReDesignConstraintSonObjs((WORD)idDbProp,&hashUpdateObjs);
			if (pPlate->IsModified())
			{
				pPlate->DesignPlate();
				pPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
		}
		for (CLDSDbObject* pDbObj=hashUpdateObjs.GetFirst();pDbObj;pDbObj=hashUpdateObjs.GetNext())
		{
			if (!pDbObj->IsPart()||!((CLDSPart*)pDbObj)->IsPlate())
				continue;
			CSuperSmartPtr<CLDSGeneralPlate>pPlate=(CLDSGeneralPlate*)pDbObj;
			if (pPlate->wiType>0)
			{
				pPlate.pParamPlate->DesignPlate();
				pPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
		}
	}
	return bRetCode;
}

void DrawParamArrowLine(IDrawing *pDrawing,GEPOINT base_pos,GEPOINT line_vec,
	GEPOINT work_norm,const char* param_name=NULL,double offset_dist=0,
	double line_len=250,long hPart=0,UCS_STRU *pObjectCS=NULL)
{
	UCS_STRU ocs;
	if (pObjectCS!=NULL)
		ocs=*pObjectCS;
	else
	{
		CDisplayView *pDispView=console.GetActiveView();
		if (pDispView==NULL)
			return;
		LoadDefaultUCS(&ocs);
		ocs.origin.Set();
		vector_trans(ocs.axis_x,pDispView->ucs,FALSE);
		vector_trans(ocs.axis_y,pDispView->ucs,FALSE);
		vector_trans(ocs.axis_z,pDispView->ucs,FALSE);
		coord_trans(ocs.origin,pDispView->ucs,FALSE);
	}
	HIBERID hiberId(hPart,HIBERARCHY(0,0,0,0));
	//1.���ƻ�׼��λ��
	GEPOINT real_base_pos=base_pos;
	coord_trans(real_base_pos,ocs,TRUE,TRUE);
	hiberId.SetHiberarchy(5,0,1,0);
	AppendDbPoint(pDrawing,real_base_pos,hiberId,PS_SOLID,RGB(0,0,255),100);
	//2.���Ƽ�ͷ��(���Ǽ�ͷƫ����)
	GEPOINT real_line_vec=line_vec,real_offset_vec=line_vec,real_work_norm=work_norm;
	RotateVectorAroundVector(real_offset_vec,0.5*Pi,work_norm);
	vector_trans(real_line_vec,ocs,TRUE,TRUE);
	vector_trans(real_offset_vec,ocs,TRUE,TRUE);
	vector_trans(real_work_norm,ocs,TRUE,TRUE);
	GEPOINT line_start=real_base_pos;
	if (offset_dist>0&&!real_offset_vec.IsZero())
		line_start+=real_offset_vec*offset_dist;
	GEPOINT line_end=line_start+real_line_vec*line_len;
	//2.1 ����ֱ��
	hiberId.SetHiberarchy(5,0,2,0);
	BYTE line_width=3;
	COLORREF clrLine=RGB(255,0,0);
	AppendDbLine(pDrawing,line_start,line_end,hiberId,PS_SOLID,clrLine,line_width);
	//2.2 ���Ƽ�ͷ
	double fAngle=15*RADTODEG_COEF;
	double fEdgeLen=25/cos(fAngle);	//��ͷ��������Ϊ10
	GEPOINT arrowEndPtArr[2];
	for (int i=0;i<2;i++)
	{
		f3dPoint arrowVec=real_line_vec*-1;
		RotateVectorAroundVector(arrowVec,i==0?fAngle:-1*fAngle,real_work_norm);
		arrowEndPtArr[i]=line_end+arrowVec*fEdgeLen;
		AppendDbLine(pDrawing,line_end,arrowEndPtArr[i],hiberId,PS_SOLID,clrLine,line_width);
	}
	AppendDbLine(pDrawing,arrowEndPtArr[0],arrowEndPtArr[1],hiberId,PS_SOLID,clrLine,line_width);
	//3.����+\-�ż���������
	GEPOINT plus_sign_pos=real_base_pos+real_line_vec*line_len*0.5;
	GEPOINT minus_sign_pos=real_base_pos-real_line_vec*line_len*0.5;
	IDbText* pText=AppendDbText(pDrawing,plus_sign_pos,"+",0,80,IDbText::AlignMiddleCenter,hiberId,PS_SOLID,RGB(255,0,255),5);
	pText->SetNormal(real_work_norm);
	pText=AppendDbText(pDrawing,minus_sign_pos,"-",0,80,IDbText::AlignMiddleCenter,hiberId,PS_SOLID,RGB(255,0,255),5);
	pText->SetNormal(real_work_norm);
}

//���Ʋ����������ʾ��ͼ
//TODO:��֧����ͨ���߰�,�������߰�,�������߰��Ĭ�Ϸ�λҲ����,
bool CreateSketchPlaneByPlate(CLDSGeneralPlate* pPlate,btc::SKETCH_PLANE* pSketchPlane,
	int iFaceNo=0,const double* distFromSketch=NULL);
BOOL RedrawPlateParamDrawing(CLDSGeneralPlate* pGeneralPlate,const char *sPropKey,UCS_STRU *pObjectCS/*=NULL*/,bool removeOldEntities/*=true*/)
{
	if (pGeneralPlate==NULL)
		return FALSE;
	/*IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	if(pBuddyDrawing==NULL)
		return FALSE;
	DimStyleTable::InitDimStyle2(pBuddyDrawing->Database());
	long idDimStyle=DimStyleTable::dimStyle.m_idDimStyle;
	if(removeOldEntities)
		pBuddyDrawing->DeleteDbEntityByHiberMasterId(pPlate->handle);*/
	if (removeOldEntities)
	{
		g_pSolidDraw->DelWorkPlaneSketch(1);
		g_pSolidDraw->Draw();
	}
	HIBERID hiberId(pGeneralPlate->handle,HIBERARCHY(0,0,0,0));
	CLDSParamPlate *pParamPlate=NULL;
	CLDSPlate *pPlate=NULL;
	if (pGeneralPlate->GetClassTypeId()==CLS_PARAMPLATE)
		pParamPlate=(CLDSParamPlate*)pGeneralPlate;
	else if (pGeneralPlate->GetClassTypeId()==CLS_PLATE)
		pPlate=(CLDSPlate*)pGeneralPlate;
	else
		return FALSE;
	btc::SKETCH_PLANE sketch;
	if (pParamPlate)
	{
		if (pParamPlate->m_iParamType==TYPE_RIBPLATE)
		{
			long hAssPart1=0,hAssPart2=0,hDatumPart=0;
			pParamPlate->GetDesignItemValue('A',&hDatumPart);//��׼����
			pParamPlate->GetDesignItemValue('C',&hAssPart1);	//��������1
			pParamPlate->GetDesignItemValue('D',&hAssPart2);	//��������2
			CLDSPart* pDatumPart=console.FromPartHandle(abs(hDatumPart));
			if (pDatumPart==NULL)
				return FALSE;
			if (pDatumPart->IsPlate())
			{
				f3dPoint dimStart,dimEnd,dimPos;
				if (stricmp("X",sPropKey)==0||stricmp("Z",sPropKey)==0)
				{
					double fOffsetX=0,fOffsetZ=0;
					pParamPlate->GetDesignItemValue(KEY2C("X"),&fOffsetX);
					pParamPlate->GetDesignItemValue(KEY2C("Z"),&fOffsetZ);
					f3dPoint basePos=pParamPlate->ucs.origin-pParamPlate->ucs.axis_x*fOffsetX-pParamPlate->ucs.axis_z*fOffsetZ;
					basePos+=pParamPlate->ucs.axis_y*10;
					if (stricmp("X",sPropKey)==0)
					{
						double offset_dist=fOffsetZ>pParamPlate->Thick?0:pParamPlate->Thick*2;
						sketch.CreateStdPlane(basePos,pParamPlate->ucs.axis_x,pParamPlate->ucs.axis_z);
						g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),sketch.pVertexArr,sketch.VertexCount,pParamPlate->ucs.axis_x,"��X");
						//DrawParamArrowLine(pBuddyDrawing,basePos,pPlate->ucs.axis_x,pPlate->ucs.axis_y,"��X",offset_dist);
					}
					else
					{
						CreateSketchPlaneByPlate(pParamPlate,&sketch,1);
						g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),sketch.pVertexArr,sketch.VertexCount,pParamPlate->ucs.axis_z,"��Z",150,true,sketch.m_fUnderLength);
						//DrawParamArrowLine(pBuddyDrawing,basePos,pPlate->ucs.axis_z,pPlate->ucs.axis_y,"��Z");
					}
				}
				else if (stricmp("dx",sPropKey)==0||stricmp("dz",sPropKey)==0)
				{
					CLDSParamPlate::DESIGN_PARAM hDatumPlate,hTipBolt,Tx,Ty,TipOffsetX,TipOffsetZ;
					pParamPlate->GetDesignItemValue(KEY2C("TB"),&hTipBolt);	//������ĩ��T��Ļ�׼��˨
					pParamPlate->GetDesignItemValue(KEY2C("Tx"),&Tx);		//������ĩ��T����û����붨��ֵ
					pParamPlate->GetDesignItemValue(KEY2C("Ty"),&Ty);		//������ĩ��T����û����붨��ֵ
					pParamPlate->GetDesignItemValue(KEY2C("A"),&hDatumPlate);//����
					pParamPlate->GetDesignItemValue(KEY2C("dx"),&TipOffsetX);
					pParamPlate->GetDesignItemValue(KEY2C("dz"),&TipOffsetZ);
					CLDSGeneralPlate *pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(hDatumPlate.value.hValue,CLS_PARAMPLATE,CLS_PLATE);
					if (pDatumPlate==NULL)
						return FALSE;
					GEPOINT direction,vpTipPoint;
					if (hTipBolt.bExist)
					{
						CLDSBolt* pBendTipBolt=(CLDSBolt*)Ta.Parts.FromHandle(hTipBolt.value.hValue,CLS_BOLT);
						if (pBendTipBolt==NULL)
							return FALSE;
						if (!pDatumPlate->CalBoltIntersPos(pBendTipBolt,vpTipPoint,CLDSPlate::TOP_PLANE,NULL))
							return FALSE;
					}
					else if (Tx.bExist&&Ty.bExist)
					{	//�Դ��п�ʼbendplane.vpTipPoint��Ϊ��׼�ְ�pDatumPlateװ������ϵ�¾ֲ�չ������ wjh-2016.5.7
						PROFILE_VERTEX* pBendlineStart[2]={ NULL },*pBendlineEnd[2]={ NULL };
						int iResidePlane=-1;
						if (pDatumPlate->GetFaceN()>=2&&pBendlineStart[0]==NULL&&iResidePlane==-1)
							pDatumPlate->GetBendlineVertexies(&pBendlineStart[0],&pBendlineEnd[0],&pBendlineStart[1],&pBendlineEnd[1]);
						bool middlePick=true;
						GEPOINT vpLocalTipPoint(Tx.value.fValue,Ty.value.fValue,pDatumPlate->GetNormOffset());
						if (iResidePlane==-1&&pBendlineStart[0]&&pBendlineEnd[0])
						{
							double left_or_right=DistOf2dPtLine(vpLocalTipPoint,pBendlineStart[0]->vertex,pBendlineEnd[0]->vertex);
							//middlePick=(vpTipPoint-pBendlineStart[0]->vertex)*(vpTipPoint-pBendlineEnd[0]->vertex)<0;
							if (left_or_right<0&&middlePick)
								iResidePlane=1;
						}
						if (iResidePlane==-1&&iResidePlane==0&&pBendlineStart[1]!=NULL&&pBendlineEnd[1]!=NULL)
						{
							double left_or_right=DistOf2dPtLine(vpLocalTipPoint,pBendlineStart[1]->vertex,pBendlineEnd[1]->vertex)<0;
							//middlePick=(vpTipPoint-pBendlineStart[1]->vertex)*(vpTipPoint-pBendlineEnd[1]->vertex)<0;
							if (left_or_right<0&&middlePick)
								iResidePlane=2;
						}
						iResidePlane=max(0,iResidePlane);
						vpTipPoint=pDatumPlate->TransToActual(vpLocalTipPoint,iResidePlane+1,CLDSGeneralPlate::FROM_ACS|CLDSGeneralPlate::TO_MCS|CLDSGeneralPlate::IS_VERTEX);
						GEPOINT bendplanenorm=iResidePlane==0?pDatumPlate->ucs.axis_z:pDatumPlate->GetHuoquFaceNorm(iResidePlane-1);
						if (hDatumPlate.value.hValue>0)
							vpTipPoint+=pDatumPlate->GetThick()*bendplanenorm;
					}
					else
						return FALSE;
					direction=pParamPlate->HuoquAxisVec();
					RotateVectorAroundVector(direction,-0.5*Pi,pParamPlate->GetHuoquFaceNorm(0));
					double fOffsetX=0,fOffsetZ=0;
					pParamPlate->GetDesignItemValue(KEY2C("X"),&fOffsetX);
					pParamPlate->GetDesignItemValue(KEY2C("Z"),&fOffsetZ);
					GEPOINT basePos=vpTipPoint+pParamPlate->ucs.axis_y*10;
					if (stricmp("dx",sPropKey)==0)
					{
						double offset_dist=TipOffsetZ.value.fValue>pParamPlate->Thick?0:pParamPlate->Thick*2;
						//DrawParamArrowLine(pBuddyDrawing,basePos,direction,pPlate->ucs.axis_y,"��X",offset_dist);
						sketch.CreateStdPlane(basePos,direction,pParamPlate->ucs.axis_y);
						g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),sketch.pVertexArr,sketch.VertexCount,direction,"��X");
					}
					else
					{
						//DrawParamArrowLine(pBuddyDrawing,basePos,pPlate->GetHuoquFaceNorm(0),pPlate->ucs.axis_y,"��Z");
						CreateSketchPlaneByPlate(pParamPlate,&sketch,2);
						g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),sketch.pVertexArr,sketch.VertexCount,pParamPlate->GetHuoquFaceNorm(0),"��Z");
					}
				}
			}
			else if (pDatumPart->GetClassTypeId()==CLS_LINETUBE&&pParamPlate->GetRibPlateSubType()==CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE)
			{
				CLDSLineTube* pDatumTube=(CLDSLineTube*)pDatumPart;
				CLDSParamPlate::DESIGN_PARAM W,H;
				pParamPlate->GetDesignItemValue(KEY2C("E"),&W);
				pParamPlate->GetDesignItemValue(KEY2C("G"),&H);
				GEPOINT base_pos,work_norm,direction;
				base_pos=pParamPlate->ucs.origin;
				base_pos+=pParamPlate->ucs.axis_y*H.ToDouble()*0.5;
				if (stricmp("DX",sPropKey)==0)
				{
					direction=pParamPlate->ucs.axis_z;
					btc::SKETCH_PLANE sketch(base_pos,pParamPlate->ucs.axis_x,direction);
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),sketch.pVertexArr,sketch.VertexCount,sketch.normal,"��X",200);
				}
				else if (stricmp("DM",sPropKey)==0)
				{
					direction=pDatumTube->ucs.axis_y;
					btc::SKETCH_PLANE sketch(base_pos,pDatumTube->ucs.axis_z,direction);
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),sketch.pVertexArr,sketch.VertexCount,sketch.normal,"�ֹ�����",200);
				}
				else if (stricmp("Rt",sPropKey)==0||stricmp("Rw",sPropKey)==0)
				{
					base_pos+=pParamPlate->ucs.axis_x*W.ToDouble()*(-0.5);
					direction=pParamPlate->ucs.axis_x;
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),&base_pos,1,direction,"X+");
				}
				else if (stricmp("Lt",sPropKey)==0||stricmp("Lw",sPropKey)==0)
				{
					base_pos+=pParamPlate->ucs.axis_x*W.ToDouble()*(-0.5);
					direction=-1*pParamPlate->ucs.axis_x;
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),&base_pos,1,direction,"X-");
				}
				else if (stricmp("Rh",sPropKey)==0||stricmp("Lh",sPropKey)==0)
				{
					base_pos+=pParamPlate->ucs.axis_x*W.ToDouble()*(-0.5);
					direction=pParamPlate->ucs.axis_y;
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),&base_pos,1,direction,"Y");
				}
			}
			else
				return FALSE;
		}
		else if (pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
		{
			long hDatumTube=0;
			pParamPlate->GetDesignItemValue('C',&hDatumTube);
			CLDSLineTube *pDatumTube=(CLDSLineTube*)console.FromPartHandle(hDatumTube,CLS_LINETUBE);
			if (pDatumTube==NULL)
				return FALSE;
			double fTubeR=pDatumTube->GetDiameter()*0.5;
			if (stricmp("T",sPropKey)==0||stricmp("S",sPropKey)==0)
			{
				GEPOINT basePos=pParamPlate->ucs.origin;
				if (pParamPlate->GetCircularPlateSubType()==1)
				{	//��ͨ���ΰ�
					double angle=0,R=0;
					pParamPlate->GetDesignItemValue('R',&R);
					pParamPlate->GetDesignItemValue('B',&angle);
					angle*=0.5*RADTODEG_COEF;
					basePos.Set(R,0,0);
					rotate_point_around_axis(basePos,angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
					coord_trans(basePos,pParamPlate->ucs,TRUE);
				}
				else
				{
					double W=0,R=0;
					pParamPlate->GetDesignItemValue('W',&W);
					pParamPlate->GetDesignItemValue('R',&R);
					if (W>0)
						basePos+=pParamPlate->ucs.axis_x*(fTubeR+W);
					else if (R>0)
						basePos+=pParamPlate->ucs.axis_x*R;
				}
				if (stricmp("T",sPropKey)==0)	//�ط��߷���
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),&basePos,1,pParamPlate->ucs.axis_z,"��Z",200);
				else		//�ظֹ�����
				{	//���ݻ���ķ��߷�������ֹ�������ʾ
					f3dPoint norm_vec=pDatumTube->ucs.axis_z;
					//ʵ�ʲ����з������д���ᵼ�»����߰����ʾ������ֹ�������
					//�Ʋ�Ӧ����֮ǰ���������ƫ�����԰巨��Ϊ��������ʱ�ѷ������������ʵƫ�Ʒ���һ�£�
					//��������GetPropValueStr��ƫ������ʾ����ֵ�н�����Ӧ������wjh-2018.2.1
					//if(pDatumTube->ucs.axis_z*pPlate->ucs.axis_z<0)
					//	norm_vec*=-1;
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(158,158,158),&basePos,1,norm_vec,"�ֹ�����",200);
				}
			}
		}
		else
			return FALSE;
	}
	else if (pPlate)
	{
		if (pPlate->IsHuoQuLineCollinear3FacePlate())
		{
			if (stricmp("peakVertexOffsetY",sPropKey)==0||stricmp("extendVertexOffsetLen",sPropKey)==0)
			{
				f3dPoint basePos=pPlate->ucs.origin;
				if (stricmp("peakVertexOffsetY",sPropKey)==0)
				{
					sketch.CreateStdPlane(basePos,pPlate->ucs.axis_y);
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(0,255,0),sketch.pVertexArr,sketch.VertexCount,pPlate->ucs.axis_y,"");
				}
				else
				{
					sketch.CreateStdPlane(basePos,-1.0*pPlate->ucs.axis_x);
					g_pSolidDraw->NewWorkPlaneSketch(1,RGB(0,255,0),sketch.pVertexArr,sketch.VertexCount,-1.0*pPlate->ucs.axis_x,"");
				}
			}
		}
	}
	else
		return FALSE;
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL FireParamPlateHelpStr(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	CLDSView::PROMPT_LIFE lifeprompt(true);
	if (pCurPlate==NULL||pItem==NULL)
		return FALSE;
	long id=CLDSParamPlate::GetPropID("m_iParamType");
	CPropTreeItem *pParentItem=pItem->m_pParent;
	while (pParentItem)
	{
		if (id==pParentItem->m_idProp)
			break;
		pParentItem=pParentItem->m_pParent;
	}
	CTowerPropertyDlg *pPropDlg=(CTowerPropertyDlg*)pPropList->GetParent();
	if (pPropDlg==NULL)
		return FALSE;
	if (pParentItem&&pCurPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
	{
		long iType=0,iCirEdge=0;
		pCurPlate->GetDesignItemValue(KEY2C("I"),&iType);
		pCurPlate->GetDesignItemValue(KEY2C("C"),&iCirEdge);
		if (iType==1)
			lifeprompt.SetPromptPicture(IDB_BMP_LINEANGLE_RIB_PLATE2);
		else if (iType==2)
			lifeprompt.SetPromptPicture(IDB_BMP_LINEANGLE_RIB_PLATE3);
		else
		{
			if (iCirEdge==1)
				lifeprompt.SetPromptPicture(IDB_BMP_LINEANGLE_RIB_PLATE1);
			else
				lifeprompt.SetPromptPicture(IDB_BMP_LINEANGLE_RIB_PLATE);
		}
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_RIBPLATE)
	{
		long hDatumPart=0,hRelaPart1=0,hRelaPart2=0;
		pCurPlate->GetDesignItemValue('A',&hDatumPart);
		pCurPlate->GetDesignItemValue('C',&hRelaPart1);
		pCurPlate->GetDesignItemValue('D',&hRelaPart2);
		CLDSPart *pDatumPart=console.FromPartHandle(abs(hDatumPart));
		CLDSPart *pRelaPart1=console.FromPartHandle(abs(hRelaPart1));
		CLDSPart *pRelaPart2=console.FromPartHandle(abs(hRelaPart2));
		if (pDatumPart==NULL)
			return FALSE;
		if (pRelaPart1&&pRelaPart2==NULL)	//���߰�
		{
			if (pDatumPart->IsTube())
			{
				if (pRelaPart1->IsTube())
					lifeprompt.SetPromptPicture(IDB_BMP_TUBE_BETWEEN_PLATE);	//�ֹܼа�
				else
					lifeprompt.SetPromptPicture(IDB_BMP_CORNER_RIB_PLATE);	//�ֹ����
			}
			else //if(pDatumPart->IsPlate())
			{
				CXhChar200 imagefile("%simages\\prompt_images\\bmp_ribcorner.bmp",theApp.execute_path);
				lifeprompt.SetPromptPicture(imagefile);
			}
		}
		else if (pRelaPart1&&pRelaPart2)	//���߰�
		{
			if (pDatumPart->IsTube())
				lifeprompt.SetPromptPicture(IDB_BMP_SLOT_RIB_PLATE);
			else
			{
				CXhChar200 imagefile("%simages\\prompt_images\\bmp_ribslot.bmp",theApp.execute_path);
				lifeprompt.SetPromptPicture(imagefile);
				//lifeprompt.SetPromptPicture(IDB_BMP_SLOT_RIB_PLATE2);
			}
		}
		else //if(pRelaPart1==NULL&&pRelaPart2==NULL)
		{
			if (pDatumPart->IsPlate())		//�ְ��߰�
				lifeprompt.SetPromptPicture(IDB_BMP_PLATE_RIB_PLATE);
			else if (pDatumPart->IsTube())	//�ֹ��߰�
			{
				long iWorkNormStyle=0;	//�߰嶨λ��׼��:0.��׼�� 1.������ 2.ָ������
				pCurPlate->GetDesignItemValue(KEY2C("Nt"),&iWorkNormStyle);
				if (iWorkNormStyle!=0)
					lifeprompt.SetPromptPicture(IDB_BMP_TUBE_RIB_PLATE2);
				else
					lifeprompt.SetPromptPicture(IDB_BMP_TUBE_RIB_PLATE1);
			}
		}
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_SHOEPLATE)
	{
		long iConnStyle=0;
		pCurPlate->GetDesignItemValue(KEY2C("CS"),&iConnStyle);
		CXhChar200 imagefile;
		if (iConnStyle==0)	//��˨����
			imagefile.Printf("%simages\\prompt_images\\shoe_plate.bmp",theApp.execute_path);
		else				//�Ժ�����
			imagefile.Printf("%simages\\prompt_images\\shoe_plate2.bmp",theApp.execute_path);
		lifeprompt.SetPromptPicture(imagefile);
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_BASEPLATE)
	{
		long shapeType=0;
		pCurPlate->GetDesignItemValue('S',&shapeType);
		CXhChar200 imagefile("%simages\\prompt_images\\base_plate1.bmp",theApp.execute_path);
		if (shapeType==1)
			imagefile.Printf("%simages\\prompt_images\\base_plate2.bmp",theApp.execute_path);
		lifeprompt.SetPromptPicture(imagefile);
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_STATIONCONNPLATE)
	{	//�˳Ŷ˰�ʾ��ͼ
		CXhChar200 imagefile("%simages\\prompt_images\\station_conn_plate.bmp",theApp.execute_path);
		lifeprompt.SetPromptPicture(imagefile);
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_STATIONSHOEPLATE)
	{	//�����а�
		long hEndPlate=0;
		pCurPlate->GetDesignItemValue(KEY2C("LE"),&hEndPlate);
		CLDSParamPlate* pEndPlate=(CLDSParamPlate*)console.FromPartHandle(hEndPlate,CLS_PARAMPLATE);
		CXhChar200 imagefile("%simages\\prompt_images\\station_shoe_plate2.bmp",theApp.execute_path);
		if (pEndPlate&&pEndPlate->m_iParamType==TYPE_STATIONCONNPLATE)
			imagefile.Printf("%simages\\prompt_images\\station_shoe_plate1.bmp",theApp.execute_path);
		lifeprompt.SetPromptPicture(imagefile);
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
		lifeprompt.SetPromptPicture(IDB_BMP_SHIELDEDWATER_PLATE);
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{
		long Y=0;
		pCurPlate->GetDesignItemValue('Y',&Y);
		if (Y==0)
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_COVERPLATE2);
		else if (Y==3)
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_COVERPLATE4);
		else
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_COVERPLATE3);
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_CIRRIBPLATE)
	{
		int iSubType=pCurPlate->GetCircularPlateSubType();
		if (iSubType==CLDSParamPlate::TYPE_COMMON_CIR_PLATE)		//��ͨ�����߰�
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_PLATE);
		else if (iSubType==CLDSParamPlate::TYPE_SPECIAL_CIR_PLATE)//���η���
			lifeprompt.SetPromptPicture(IDB_BMP_SPECIAL_PROFILE_FL);
		else if (iSubType==CLDSParamPlate::TYPE_3WL_CIR_PLATE)	//3WL�߰�
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE1);
		else if (iSubType==CLDSParamPlate::TYPE_3WA_CIR_PLATE)	//3WA�߰�
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE2);
		else if (iSubType==CLDSParamPlate::TYPE_2WL_CIR_PLATE)	//2WL�߰�
		{
			double angle=0;
			pCurPlate->GetDesignItemValue('B',&angle);
			lifeprompt.SetPromptPicture((angle<=0)?IDB_BMP_CIRCULAR_RIBPLATE3:IDB_BMP_CIRCULAR_RIBPLATE7);
		}
		else if (iSubType==CLDSParamPlate::TYPE_2WA_CIR_PLATE)	//2WA�߰�
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE4);
		else if (iSubType==CLDSParamPlate::TYPE_CIR_AISLE_PLATE)	//�ߵ���
			lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE8);
	}
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_TUBERIBPLATE)
		lifeprompt.SetPromptPicture(IDB_BMP_CIRCULAR_RIBPLATE5);
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_FL)
		lifeprompt.SetPromptPicture(IDB_BMP_PLATE_FL2);
	else if (pParentItem&&pCurPlate->m_iParamType==TYPE_TUBESTAYWIREPLATE)
		lifeprompt.SetPromptPicture(IDB_BMP_STAYWIRE_RIBPLATE);
	else if (pParentItem&&pCurPlate->m_iParamType == TYPE_PEDALPLATE)
	{
		long C = 0;
		pCurPlate->GetDesignItemValue('C', &C);
		if (C == 0)
			lifeprompt.SetPromptPicture(IDB_BMP_PEDAL_PLATE1);
		else
			lifeprompt.SetPromptPicture(IDB_BMP_PEDAL_PLATE2);
	}
	else
	{
		pPropDlg->SetPropHelpBmpID(NULL);
		::SetWindowText(pPropList->m_hPromptWnd,pItem->m_lpNodeInfo->m_strPropHelp);
		::UpdateWindow(pPropList->m_hPromptWnd);
	}
	KEY2C key=CLDSParamPlate::GetPropKey(pItem->m_idProp,pCurPlate->m_iParamType);
	RedrawPlateParamDrawing(pCurPlate,key.Label());
	return TRUE;
}
BOOL ParamRibPlateButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem,HASHOBJSET &selectObjs)
{
	if (selectObjs.GetNodeNum()<0)
		return FALSE;
	long h=0;
	CString valueStr;
	CModifyPartHandleDlg modifyhdlg;
	modifyhdlg.m_iMirMsg=0;
	CLDSParamPlate* pParamPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	if (CLDSParamPlate::GetPropID("A",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//��λ��׼����
		pParamPlate->GetDesignItemValue(KEY2C("A"),&h);
		modifyhdlg.m_sCaption="�߰��׼����";
		modifyhdlg.m_sTitle="�߰��׼����";
		modifyhdlg.m_sHandle.Format("0X%X",h);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ�߰��׼����:");
		modifyhdlg.SetPickObjTypeId(CLS_PLATE);
		if (modifyhdlg.DoModal()!=IDOK)
			return FALSE;
		CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
		if (pPart==NULL || !pPart->IsPart())
		{
			AfxMessageBox("�����������Ҳ�����Ӧ������");
			return FALSE;
		}
		valueStr.Format("0X%X",modifyhdlg.m_nHandle);
		pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("A",pParamPlate->m_iParamType),valueStr);
		g_pSolidDraw->ReleaseSnapStatus();
	}
	else if (CLDSParamPlate::GetPropID("C",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//��������1
		pParamPlate->GetDesignItemValue(KEY2C("C"),&h);
		modifyhdlg.m_sCaption="�߰帨������1";
		modifyhdlg.m_sTitle="�߰帨������1";
		modifyhdlg.m_sHandle.Format("0X%X",abs(h));
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť_
		modifyhdlg.SetCmdPickPromptStr("��ѡ��������:");
		modifyhdlg.SetPickObjTypeId(CLS_PLATE);
		if (modifyhdlg.DoModal()!=IDOK)
			return FALSE;
		CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
		if (pPart==NULL || !pPart->IsPlate())
		{
			AfxMessageBox("�����������Ҳ�����Ӧ������");
			return FALSE;
		}
		valueStr.Format("0X%X",modifyhdlg.m_nHandle);
		pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("C",pParamPlate->m_iParamType),valueStr);
		g_pSolidDraw->ReleaseSnapStatus();
	}
	else if (CLDSParamPlate::GetPropID("O",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//��λ��˨
		pParamPlate->GetDesignItemValue(KEY2C("O"),&h);
		modifyhdlg.m_sCaption="�߰嶨λ��˨";
		modifyhdlg.m_sTitle="�߰嶨λ��˨";
		modifyhdlg.m_sHandle.Format("0X%X",h);
		modifyhdlg.m_bDispSelectBtn=TRUE;
		modifyhdlg.SetCmdPickPromptStr("��ѡ��λ��˨:");
		modifyhdlg.SetPickObjTypeId(CLS_BOLT);
		if (modifyhdlg.DoModal()!=IDOK)
			return FALSE;
		CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
		if (pPart==NULL)
		{
			AfxMessageBox("�����������Ҳ�����Ӧ������");
			return FALSE;
		}
		valueStr.Format("0X%X",modifyhdlg.m_nHandle);
		pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("O",pParamPlate->m_iParamType),valueStr);
		g_pSolidDraw->ReleaseSnapStatus();
	}
	else if (CLDSParamPlate::GetPropID("V",pParamPlate->m_iParamType)==pItem->m_idProp)
	{	//����˼�
		pParamPlate->GetDesignItemValue(KEY2C("V"),&h);
		modifyhdlg.m_sCaption="�߰巽��˼�";
		modifyhdlg.m_sTitle="�߰巽��˼�";
		modifyhdlg.m_sHandle.Format("0X%X",h);
		modifyhdlg.m_bDispSelectBtn=TRUE;
		modifyhdlg.SetCmdPickPromptStr("��ѡ����˼�:");
		modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
		if (modifyhdlg.DoModal()!=IDOK)
			return FALSE;
		CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
		if (pPart==NULL || !pPart->IsLinePart())
		{
			AfxMessageBox("�����������Ҳ�����Ӧ������");
			return FALSE;
		}
		valueStr.Format("0X%X",modifyhdlg.m_nHandle);
		pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("V",pParamPlate->m_iParamType),valueStr);
		g_pSolidDraw->ReleaseSnapStatus();
	}
	else
		return FALSE;
	for (pParamPlate=(CLDSParamPlate*)selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)selectObjs.GetNext())
	{
		g_pSolidDraw->SetEntSnapStatus(pParamPlate->handle);
		if (CLDSParamPlate::GetPropID("A",pParamPlate->m_iParamType)==pItem->m_idProp)
			pParamPlate->SetDesignItemValue(KEY2C("A"),modifyhdlg.m_nHandle,NULL,0,1);
		else if (CLDSParamPlate::GetPropID("C",pParamPlate->m_iParamType)==pItem->m_idProp)
			pParamPlate->SetDesignItemValue(KEY2C("C"),modifyhdlg.m_nHandle,NULL,0,1);
		else if (CLDSParamPlate::GetPropID("O",pParamPlate->m_iParamType)==pItem->m_idProp)
			pParamPlate->SetDesignItemValue(KEY2C("O"),modifyhdlg.m_nHandle,NULL,0,1);
		else if (CLDSParamPlate::GetPropID("V",pParamPlate->m_iParamType)==pItem->m_idProp)
			pParamPlate->SetDesignItemValue(KEY2C("V"),modifyhdlg.m_nHandle,NULL,0,1);
	}
	return TRUE;
}
BOOL ParamPlateButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem,*pParentItem;
	CLDSParamPlate *pFirstPlate=NULL,*pPlate=NULL;
	CReportVectorDlg vecDlg;
	CCfgPartNoDlg cfgdlg;
	CString valueStr = _T("");

	if (pView->selectObjs.GetNodeNum()>0)
		pFirstPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	BOOL bRetCode=FALSE;
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
	CPropListOper<CLDSParamPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&pView->selectObjs);
	if (pItem->m_idProp==CLDSParamPlate::GetPropID("cfgword"))
	{
		cfgdlg.cfgword = pFirstPlate->cfgword;
		if (pFirstPlate->IsBodyObj())
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		if (cfgdlg.DoModal()==IDOK)
			pFirstPlate->cfgword = cfgdlg.cfgword;		//�ڴ˴����µ�һ���ڵ������Ϻ�
		CXhChar100 sText;
		if (pFirstPlate->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
			pItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("sPartNo"))
	{
		if (pFirstPlate->m_bStdPart)
		{	//�ڱ�׼�������в��ҿ�ʹ�õı�׼�������޸ı�׼�������
			//�������ɱ�׼����(ʹ�þɾ��),���º�ȡ����ʡ���ظְ�����
			CLDSStdPartModel stdPartModel;
			if (pFirstPlate->m_iParamType==TYPE_FLD)
			{
				CXhChar16 sPartNo(pFirstPlate->GetPartNo());
				int len=sPartNo.GetLength();
				char cMark=sPartNo.At(len-1);
				if (cMark<'0'||cMark>'9')	//���ڱں�����ʶ�ַ�
				{
					sPartNo.At(len-1)='?';
					pFirstPlate->SetPartNo(sPartNo);
				}
				pFirstPlate->GetStdPartModel(&stdPartModel);
				pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("sPartNo"),pFirstPlate->GetPartNo());
			}
		}
		else
		{
			CModifyRelativeObjPartNoDlg modify_partno_dlg;
			modify_partno_dlg.m_pWorkPart=pFirstPlate;
			if (modify_partno_dlg.DoModal()==IDOK)
			{
				pFirstPlate->SetPartNo(modify_partno_dlg.m_sCurObjPartNo.GetBuffer(0));
				pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
				SEGI iSeg;
				if (CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(modify_partno_dlg.m_sCurObjPartNo,&iSeg,NULL))
				{
					pFirstPlate->iSeg=iSeg;
					pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("iSeg"),iSeg.ToString());
				}
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs"))
	{
		CPartUcsDlg part_dlg_ucs;
		part_dlg_ucs.ucs=pFirstPlate->ucs;
		if (part_dlg_ucs.DoModal()==IDOK)
		{
			for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
				pPlate->ucs=part_dlg_ucs.ucs;
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.origin"))
	{
		if (pFirstPlate->design.origin.datum_pos_style==10)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Assembly CS origin";
#else 
			vecDlg.m_sCaption = "װ������ϵԭ��λ��";
#endif
			vecDlg.m_fVectorX = pFirstPlate->ucs.origin.x;
			vecDlg.m_fVectorY = pFirstPlate->ucs.origin.y;
			vecDlg.m_fVectorZ = pFirstPlate->ucs.origin.z;
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.origin.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.origin.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("ucs.origin.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.origin.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.origin.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSPlate::GetPropID("ucs.origin.z"),CString(sText));
				for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
					pPlate->ucs.origin.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
		else
		{
			CDatumPointPropertyDlg desPointDlg;
			desPointDlg.m_pCurEditObj=pFirstPlate;
			desPointDlg.m_xPoint=pFirstPlate->design.origin;
			if (desPointDlg.m_xPoint.datum_pos_style==0)
				desPointDlg.m_xPoint.SetPosition(pFirstPlate->ucs.origin);
			if (desPointDlg.DoModal()==IDOK)
			{
				pFirstPlate->design.origin=desPointDlg.m_xPoint;
				pFirstPlate->design.origin.UpdatePos(&Ta);
				pFirstPlate->ucs.origin=pFirstPlate->design.origin.Position();
				pFirstPlate->SetModified();
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_x"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "X Axis Direction";
#else 
		vecDlg.m_sCaption = "װ��ʱX�᷽��";
#endif
		vecDlg.m_fVectorX = pFirstPlate->ucs.axis_x.x;
		vecDlg.m_fVectorY = pFirstPlate->ucs.axis_x.y;
		vecDlg.m_fVectorZ = pFirstPlate->ucs.axis_x.z;
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_x.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_x.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_x.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if (vecDlg.DoModal()==IDOK)
		{
			CXhChar100 sText;
			sText.Printf("%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_x.x"),CString(sText));
			sText.Printf("%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_x.y"),CString(sText));
			sText.Printf("%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_x.z"),CString(sText));
			for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			{
				pPlate->ucs.axis_x.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_x*1,0);
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_y"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Y Axis Direction";
#else 
		vecDlg.m_sCaption = "װ��ʱY�᷽��";
#endif
		vecDlg.m_fVectorX = pFirstPlate->ucs.axis_y.x;
		vecDlg.m_fVectorY = pFirstPlate->ucs.axis_y.y;
		vecDlg.m_fVectorZ = pFirstPlate->ucs.axis_y.z;
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_y.x"),&pParentItem);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_y.y"),&pParentItem);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_y.z"),&pParentItem);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if (vecDlg.DoModal()==IDOK)
		{
			CXhChar100 sText;
			sText.Printf("%f",vecDlg.m_fVectorX);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_y.x"),CString(sText));
			sText.Printf("%f",vecDlg.m_fVectorY);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_y.y"),CString(sText));
			sText.Printf("%f",vecDlg.m_fVectorZ);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_y.z"),CString(sText));
			for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			{
				pPlate->ucs.axis_y.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_y*1,1);
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_z"))
	{
		if (pFirstPlate->m_iParamType!=TYPE_XEND)
		{	//��ʮ�ֲ����ʱ��֧�ֶ��Ʋ�幤������
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Z Axis Direction";
#else 
			vecDlg.m_sCaption = "װ��ʱZ�᷽��";
#endif
			vecDlg.m_fVectorX = pFirstPlate->ucs.axis_z.x;
			vecDlg.m_fVectorY = pFirstPlate->ucs.axis_z.y;
			vecDlg.m_fVectorZ = pFirstPlate->ucs.axis_z.z;
			pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_z.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_z.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("ucs.axis_z.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if (vecDlg.DoModal()==IDOK)
			{
				CXhChar100 sText;
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_z.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_z.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("ucs.axis_z.z"),CString(sText));
				for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
				{
					pPlate->ucs.axis_z.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_z*1,2);
				}
			}
		}
		else
		{	//ʮ�ֲ��֧�ֶ��Ʋ�幤������
			CDesignVectorPropDlg vector_dlg;
			vector_dlg.m_hCurEditPart=pFirstPlate->handle;
			vector_dlg.m_xVector = pFirstPlate->workNorm;
			//��ʾ����ϵ
			UCS_STRU object_ucs,ucs = pFirstPlate->ucs;
			g_pSolidSet->GetObjectUcs(object_ucs);
			coord_trans(ucs.origin,object_ucs,TRUE);
			vector_trans(ucs.axis_x,object_ucs,TRUE);
			vector_trans(ucs.axis_y,object_ucs,TRUE);
			vector_trans(ucs.axis_z,object_ucs,TRUE);
			g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
			//g_pSolidSet->SetDispWorkUcs();
			g_pSolidDraw->Draw();
			if (vector_dlg.DoModal()==IDOK)
			{
				pFirstPlate->workNorm=vector_dlg.m_xVector;
				pFirstPlate->workNorm.UpdateVector(&Ta);
				//����������Ʋ�����������Ƶ�ǰ�ְ�
				//pFirstPlate->DesignSetupUcs();
				pFirstPlate->DesignPlate();
				pFirstPlate->SetModified();
				pFirstPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pFirstPlate->GetSolidPartObject());
				//����Z�᷽�����ʾ����
				CXhChar100 sText;
				long idProp=CLDSParamPlate::GetPropID("ucs.axis_z.x");
				if (pFirstPlate->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,sText);
				idProp=CLDSParamPlate::GetPropID("ucs.axis_z.y");
				if (pFirstPlate->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,sText);
				idProp=CLDSParamPlate::GetPropID("ucs.axis_z.z");
				if (pFirstPlate->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,sText);
				if (pFirstPlate->m_iParamType==TYPE_XEND)
				{	//������Ʋ�帽�Ӱ����ѵ�ǰ�ְ�Ϊ��λ�����İ���
					ATTACHPART_ITEM *pItem=NULL;
					for (pItem=pFirstPlate->GetFirstAttachPart();pItem;pItem=pFirstPlate->GetNextAttachPart())
					{
						CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pItem->h,CLS_PLATE);
						if (pPlate==NULL)
							continue;
						if (pPlate->designInfo.norm.hVicePart==pFirstPlate->handle
							&&pPlate->designInfo.norm.norm_style>0)
						{
							pPlate->DesignSetupUcs();
							pPlate->DesignPlate();
							pPlate->SetModified();
							pPlate->Create3dSolidModel();
							g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
						}
					}
					//������Ʋ�����ʱ���丽������˨�������
					for (CLsRef *pLsRef=pFirstPlate->GetFirstLsRef();pLsRef;pLsRef=pFirstPlate->GetNextLsRef())
					{
						CLDSBolt *pBolt=pLsRef->GetLsPtr();
						pBolt->correct_worknorm();
						pBolt->correct_pos();
						pBolt->CalGuigeAuto();
						pBolt->SetModified();
						pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
						g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
					}
				}
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstPlate->handle);
			//
			//g_pSolidSet->SetDispWorkUcs(FALSE);
			g_pSolidDraw->BatchClearCS(2);
			g_pSolidDraw->Draw();
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("plate_work_plane"))
	{
		CPartWorkPlaneDlg planeDlg;
		planeDlg.m_pDatumPart=pFirstPlate;
		planeDlg.DoModal();
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_iParamType"))
	{	//�ְ���Ʋ����б�
		CUndoOperObject undo(&Ta,true);
		if (pFirstPlate->m_iParamType==TYPE_RIBPLATE)
		{
			CDesignRibbedPlateDlg ribPlateDlg;
			ribPlateDlg.SetRibPlate(pFirstPlate);
			ribPlateDlg.DoModal();
			RebuildOrUpdateParamsPropertyList(pPropList,pItem);
		}
		else if (pFirstPlate->m_iParamType==TYPE_ELBOWPLATE)
		{	//��尴���߰�Ի��������ʾ
			CDesignRibbedPlateDlg ribPlateDlg;
			pFirstPlate->ShiftElbowAndRibPlateSchema(TYPE_RIBPLATE);
			ribPlateDlg.SetRibPlate(pFirstPlate);
			ribPlateDlg.DoModal();
			pFirstPlate->ShiftElbowAndRibPlateSchema(TYPE_ELBOWPLATE);
			RebuildOrUpdateParamsPropertyList(pPropList,pItem);
		}
		else if (pFirstPlate->m_iParamType==TYPE_LINEANGLERIBPLATE||
			pFirstPlate->m_iParamType==TYPE_TUBESTAYWIREPLATE)
		{
			CDesLineAngleRibPlateDlg lineAngleRibPlateDlg;
			lineAngleRibPlateDlg.m_bNewRibPlate=FALSE;
			lineAngleRibPlateDlg.m_pRibPlate=pFirstPlate;
			lineAngleRibPlateDlg.DoModal();
			RebuildOrUpdateParamsPropertyList(pPropList,pItem);
		}
#ifdef __COMPLEX_PART_INC_
		else if (pFirstPlate->m_iParamType==TYPE_CIRCOVERPLATE
			||pFirstPlate->m_iParamType==TYPE_TUBERIBPLATE
			||pFirstPlate->m_iParamType==TYPE_CIRRIBPLATE
			||pFirstPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
		{
			CDesignCircularPlateDlg cirPlateDlg;
			cirPlateDlg.m_bNewCirPlate=FALSE;
			cirPlateDlg.m_pCirPlate=pFirstPlate;
			cirPlateDlg.DoModal();
		}
#endif
		else
		{
			CParamPlateParamListDlg paramDlg;
			paramDlg.m_pParamPlate=pFirstPlate;
			paramDlg.DoModal();
		}
		/*
		if(pItem->m_lpNodeInfo->m_strPropValue[0]=='2')
		dlgDesginParam.m_bAddItem = TRUE;
		else
		dlgDesginParam.m_bAddItem = FALSE;
		dlgDesginParam.m_pWorkPart = pFirstPlate;
		if(IDOK==dlgDesginParam.DoModal())
		pFirstPlate = ((CLDSParamPlate*)dlgDesginParam.m_pWorkPart);
		CString sPartNo=pFirstPlate->sPartNo;
		for(CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pParamPlate;pParamPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{	//������ͬ�ұ����ͬ�Ĳ�����Ĳ�������һ���޸�
		if(pParamPlate==pFirstPlate)
		continue;
		if(pParamPlate->m_iParamType!=pFirstPlate->m_iParamType)
		continue;
		if(sPartNo.CompareNoCase(pParamPlate->sPartNo)!=0)
		continue;
		for(DESIGN_PLATE_ITEM *pItem=pFirstPlate->designItemList.GetFirst();pItem;pItem=pFirstPlate->designItemList.GetNext())
		{
		//if(pItem->dataType==1)	//������͵Ĳ�������
		//	continue;
		if(!pItem->bModify)
		continue;
		if(pItem->dataType==1)
		pParamPlate->SetDesignItemValue(pItem->cItem,pItem->value.fValue,pItem->sNotes,1);
		else
		pParamPlate->SetDesignItemValue(pItem->cItem,pItem->value.fValue,pItem->sNotes);
		}
		}*/
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("relativeObjs"))
	{
		CRelativeObjectDlg relativeDlg;
		relativeDlg.m_pWorkObj = pFirstPlate;
		if (relativeDlg.DoModal()==IDOK)
		{
			CXhChar200 sText;
			pFirstPlate->GetPropValueStr(pItem->m_idProp,sText);
			pPropList->SetItemPropValue(pItem->m_idProp,sText);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("shadowInfo"))
	{
		CDesignShadowDlg shadowdlg;
		shadowdlg.SetShadowObj((CLDSObject*)pFirstPlate);
		shadowdlg.DoModal();
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_hPartWeldParent"))
	{
		//ͨ��ѡ�񹹼��޸ĺ��Ӹ����������ڽ������޸ĵ��������ĺ��Ӹ�����
		CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
		modifyhdlg.m_sCaption="Modify Parametric Weld Parent Part";
		modifyhdlg.m_sTitle="Weld Parent Part:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("Please select weld parent part:");
#else 
		modifyhdlg.m_sCaption="�޸Ĳ������ְ庸�Ӹ�����";
		modifyhdlg.m_sTitle="���Ӹ�����:";
		modifyhdlg.m_sHandle.Format("0X%X",pFirstPlate->m_hPartWeldParent);
		modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
		modifyhdlg.SetCmdPickPromptStr("��ѡ�񺸽Ӹ�����:");
#endif
		modifyhdlg.SetPickObjTypeId(CLS_PART);
		if (IDOK==modifyhdlg.DoModal())
		{
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if (pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The input handle is wrong,so can't find the corresponding part��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
			else
			{
				valueStr.Format("0X%X",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("m_hPartWeldParent"),valueStr);
				g_pSolidDraw->ReleaseSnapStatus();
				MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
				for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
				{
					int flagArr[3]={ 1,2,4 };
					CLDSPlate* pMirPlate=NULL;
					for (int i=0;i<3;i++)
					{
						if ((mirmsg.axis_flag&flagArr[i])==0)
							continue;
						MIRMSG msg=mirmsg.GetSubMirItem(i+1);
						pMirPlate=(CLDSPlate*)pPlate->GetMirPart(msg);
						if (pMirPlate)
						{
							CLDSPart *pMirPart=pPart->GetMirPart(msg);
							if (pMirPart)	//������ѡ�������ĶԳƹ��� wht 11-05-07
								pMirPlate->m_hPartWeldParent=pMirPart->handle;
						}
					}
					pPlate->m_hPartWeldParent=pPart->handle;
					g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
				}
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.origin")||
		pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.axis_x")||pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.axis_y"))
	{
		f3dPoint spec_point;
		CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
		CString cmdStr="��ѡ��ǰ��������ϵԭ�����ڵĶ������˨:";
		int iMenu=0;
		if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.axis_x"))
		{
			iMenu=1;
			cmdStr="��ѡ��ǰ��������ϵX������궨�����ڵĶ������˨:";
		}
		else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.axis_y"))
		{
			iMenu=2;
			cmdStr="��ѡ��ǰ��������ϵY������궨�����ڵĶ������˨:";
		}
		pCmdLine->FillCmdLine(cmdStr,"");
		CSnapTypeVerify verifier;
		DWORD dwhObj=0,dwExportFlag=0;
		verifier.SetVerifyFlag(OBJPROVIDER::DRAWINGSPACE,GetSingleWord(IDbEntity::DbPoint));
		verifier.AddVerifyType(OBJPROVIDER::SOLIDSPACE,CLS_BOLT);
		CLDSBolt* pPickBolt=NULL;
		PROFILE_VERTEX* pPickVertex=NULL;
		while (true)
		{
			if ((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			if (obj.provider==OBJPROVIDER::SOLIDSPACE)
				pPickBolt=(CLDSBolt*)Ta.FromPartHandle(obj.hObj,CLS_BOLT);
			else if (obj.provider==OBJPROVIDER::DRAWINGSPACE)
			{
				IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
				IDbEntity* pEnt=pBuddyDrawing->GetDbEntity(obj.idEnt);
				if (pEnt==NULL || pEnt->GetDbEntType()!=IDbEntity::DbPoint)
					continue;	//��������
				if (pEnt->GetHiberId().masterId!=pFirstPlate->handle)
					continue;	//�ǵ�ǰ�ְ��ϵ�������
				DWORD dwObjectId=pEnt->GetHiberId().HiberDownId(1);
				for (PROFILE_VERTEX* pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
				{
					if (pVertex->vertex.ID==dwObjectId)
					{
						pPickVertex=pVertex;
						break;
					}
				}
			}
			else
				continue;
			if (pPickBolt!=NULL||pPickVertex!=NULL)
				break;
		}
		pCmdLine->FinishCmdLine();
		PLATEEDIT_STATE* pState=CLDSParamPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		//���㹤������ϵ
		f3dPoint xLocalPickPoint;
		if (pPickBolt)
		{
			pFirstPlate->GetBoltIntersPos(pPickBolt,xLocalPickPoint);
			coord_trans(xLocalPickPoint,pFirstPlate->ucs,FALSE,TRUE);
		}
		else
			xLocalPickPoint=pPickVertex->vertex;
		if (iMenu==0)		//��׽ԭ��
		{
			console.m_curWCS.origin = xLocalPickPoint;
			//
			pState->m_fLocalWcsOrgX=xLocalPickPoint.x;
			pState->m_fLocalWcsOrgY=xLocalPickPoint.y;
		}
		else if (iMenu==1)	//��׽X����궨��
		{	//��׽������
			console.m_curWCS.axis_x = xLocalPickPoint-console.m_curWCS.origin;
			normalize(console.m_curWCS.axis_x);
			console.m_curWCS.axis_z.Set(0,0,1);
			console.m_curWCS.axis_y = console.m_curWCS.axis_z^console.m_curWCS.axis_x;
			//
			pState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
			pState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
			pState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
			pState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
		}
		else if (iMenu==2)	//��׽������˨
		{
			console.m_curWCS.axis_y = xLocalPickPoint-console.m_curWCS.origin;
			normalize(console.m_curWCS.axis_y);
			console.m_curWCS.axis_z.Set(0,0,1);
			console.m_curWCS.axis_x = console.m_curWCS.axis_y^console.m_curWCS.axis_z;
			//
			pState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
			pState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
			pState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
			pState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
		}
		//�ڸְ�ʵ������ʾ��������ϵ
		UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
		g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
		g_pSolidDraw->Draw();
		//������������ʾ
		if (iMenu==0)
		{
			oper.UpdatePropItemValue("wcs.origin");
			oper.UpdatePropItemValue("wcs.origin.x");
			oper.UpdatePropItemValue("wcs.origin.y");
		}
		else
		{
			oper.UpdatePropItemValue("wcs.axis_x");
			oper.UpdatePropItemValue("wcs.axis_x.a");
			oper.UpdatePropItemValue("wcs.axis_y");
		}
	}
	else if (pItem->m_idProp==CLDSPlate::GetPropID("wcs.origin.x"))
	{
		PLATEEDIT_STATE* pEditState=CLDSPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		pEditState->m_fLocalWcsOrgX=console.m_curWCS.origin.x=atof(valueStr);
		UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
		g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
		g_pSolidDraw->Draw();
		return TRUE;
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.origin.y"))
	{
		PLATEEDIT_STATE* pEditState=CLDSParamPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		pEditState->m_fLocalWcsOrgY=console.m_curWCS.origin.y=atof(valueStr);
		UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
		g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
		g_pSolidDraw->Draw();
		return TRUE;
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.axis_x.a"))
	{
		double alfa=atof(valueStr)*RADTODEG_COEF;	//�ɽǶ���תΪ������
		PLATEEDIT_STATE* pEditState=CLDSParamPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		console.m_curWCS.axis_x=GEPOINT(cos(alfa),sin(alfa));
		console.m_curWCS.axis_y.Set(-console.m_curWCS.axis_x.y,console.m_curWCS.axis_x.x);
		pEditState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
		pEditState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
		pEditState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
		pEditState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
		oper.UpdatePropItemValue("wcs.axis_x");
		oper.UpdatePropItemValue("wcs.axis_y");
		UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
		g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
		g_pSolidDraw->Draw();
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("InsertProfileVertex"))	//�����������
		return pView->InsertPlankVertex();
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("AddNewHole"))	//�����˨
		return pView->InsertPlankBolt();
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("DeleteFeature"))	//ɾ���ְ���������
		return pView->DeletePlankFeature();
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("FilletVertex"))	//
		return pView->FilletPlateCorner();
	else if (pFirstPlate->m_iParamType==TYPE_RIBPLATE)
		bRetCode=ParamRibPlateButtonClick(pPropList,pItem,pView->selectObjs);
	else
		return FALSE;
	try {
		if (bRetCode)
		{	//�����޸ĺ��߰��������
			for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
				pPlate->DesignPlate();
		}
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->SetModified();
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		}
	}
	catch (char* error)
	{
		AfxMessageBox(error);
	}
	catch (...)
	{
		AfxMessageBox("ParamPlateButtonClick encountered an unexpected error");
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL FireParamPlatePopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pFirstPlate=NULL,*pPlate=NULL;
	if (pView->selectObjs.GetNodeNum()>0)
		pFirstPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	if (pItem->m_idProp==CLDSParamPlate::GetPropID("plateAttachElbowPlate"))
	{
		CXhChar100 sText;
		if (0==iMenu)		//�鿴�����������
		{
			g_pLsRefDlg->InitLsRefDlg(pFirstPlate,0,TRUE);
			/*
			g_pLsRefDlg->m_pWorkPart = pFirstPlate;
			g_pLsRefDlg->m_bPlateAttachPart = TRUE;
			if(g_pLsRefDlg->DoModal()==IDOK)
				pFirstPlate = (CLDSParamPlate*)(dlgLsRef.m_pWorkPart);
			*/
		}
		else if (1==iMenu)	//�������
		{
			CLDSPart *pPart = NULL;
			CString cmdStr = _T("");
			//����HashTable
			CHashTable<CLDSPart*>partsetTable;
			partsetTable.Empty();
			partsetTable.CreateHashTable(100);
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Select the importing elbow plate,or directly input handle(Seperate: ','), press enter key to confirm:","");
#else 
			pCmdLine->FillCmdLine("ѡ�����������,��ֱ��������(����','�ָ�),���س���ȷ��:","");
#endif
			if (pCmdLine->GetStrFromCmdLine(cmdStr))
			{
				//���ݾ���ַ������빹��
				long *id_arr1=NULL,arr1_len=0;;
				if (cmdStr.GetLength()>0)
				{
					id_arr1=new long[cmdStr.GetLength()];
					sText.Printf("%s",cmdStr);
					for (char* key=strtok(sText,",");key;key=strtok(NULL,","))
					{
						long h=HexStrToLong(key);
						pPart=console.FromPartHandle(h);
						id_arr1[arr1_len]=h;
						arr1_len++;
					}
				}
				long *id_arr;
				int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
				for (int i=0;i<arr1_len+n;i++)
				{
					if (i<arr1_len)
						pPart=console.FromPartHandle(id_arr1[i]);
					else
						pPart=console.FromPartHandle(id_arr[i-arr1_len]);
					if (pPart&&pPart->GetClassTypeId()==CLS_PARAMPLATE)
					{
						if (((CLDSParamPlate*)pPart)->m_iParamType==TYPE_ELBOWPLATE)
						{
							if (!partsetTable.GetValueAt(pPart->handle,pPart))
							{
								partsetTable.SetValueAt(pPart->handle,pPart);
								pFirstPlate->AppendAttachPart(pPart);
							}
						}
					}
				}
				if (id_arr1)
					delete[]id_arr1;
				g_pSolidDraw->ReleaseSnapStatus();
			}
			else
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Command:","");
#else 
			pCmdLine->FillCmdLine("����:","");
#endif
		}
		else if (2==iMenu)	//�������
		{
			static CAddNewElbowPlateDlg dlgElbowPlate;
			dlgElbowPlate.m_pDatumTube=(CLDSLineTube*)console.FromPartHandle(pFirstPlate->m_hPartWeldParent);
			dlgElbowPlate.m_pDatumPlate = pFirstPlate;
			dlgElbowPlate.DoModal();
		}
	}
	return TRUE;
}

long RetrieveBlockHandle(const char* blockIdName);
BOOL ModifyParamPlateProperty(CPropertyList *pPropList,CPropTreeItem* pItem,CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSParamPlate *pCurPlate = (CLDSParamPlate*)pView->selectObjs.GetFirst();
	CLDSParamPlate *pPlate = NULL;
	char tem_str[MAX_TEMP_CHAR_100+1]="";

	if (pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	CLDSParamPlate* pFirstPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();
	BOOL bUpdateWcs=FALSE;
	CPropListOper<CLDSParamPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&pView->selectObjs);
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if (pItem->m_idProp==CLDSParamPlate::GetPropID("layer"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->SetLayer(tem_str);
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				if (pMirObj->mirInfo.axis_flag&8)
					continue;	//��ת�Գ�
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					CalMirAtomLayer(pPlate->layer(),pMirPlate->layer(),pMirObj->mirInfo);
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("cMaterial"))
	{	//�ְ����
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->cMaterial = QuerySteelBriefMatMark(tem_str);
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					pMirPlate->cMaterial=pPlate->cMaterial;
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_cQualityLevel"))
	{	//�ְ������ȼ�
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if (valueStr.CompareNoCase("Default")==0)
				pPlate->m_cQualityLevel=0;
			else
#else
			if (valueStr.CompareNoCase("Ĭ��")==0)
				pPlate->m_cQualityLevel=0;
			else
#endif
				pPlate->m_cQualityLevel = valueStr[0];
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					pMirPlate->m_cQualityLevel=pPlate->m_cQualityLevel;
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_hBlock"))
	{
		long hBlock=RetrieveBlockHandle(valueStr);
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			pPlate->m_hBlock=hBlock;
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("thick"))
	{	//�ְ���
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->thick=atoi(tem_str);
			if (pPlate->m_iParamType==TYPE_BASEPLATE)
			{
				pPlate->Create3dSolidModel();
				if (pPlate->IsSolidModified())
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
			else if (pPlate->m_iParamType==TYPE_FL)
			{	//�޸ķ����������ݸֹܶ�ͷ������¼��㷨����λ��
				CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
				if (pLineTube)
				{
					if (pLineTube->m_tJointEnd.hLinkObj==pPlate->handle)
						pLineTube->DesignEndJointUCS(pPlate);
					else if (pLineTube->m_tJointStart.hLinkObj==pPlate->handle)
						pLineTube->DesignStartJointUCS(pPlate);
				}
			}
			//pPlate->DesignPlate();		//���ڻ���(������)���ĺ��ʱ�Ѽ�����DesignPlate����,����������Ƹְ� wjh-2016.8.31
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
				{
					pMirPlate->thick=pPlate->thick;
					if (pPlate->m_iParamType==TYPE_BASEPLATE)
						pPlate->Create3dSolidModel();
				}
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("iSeg"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->iSeg = SEGI(valueStr.GetBuffer());
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					pMirPlate->iSeg=pPlate->iSeg;
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("sPartNo"))
	{	//���ݼ�����ȡ�κţ������¸ְ�κ�
		bool parsed=false;
		SEGI iSeg;
		if (CLDSPart::UPDATE_PARTNO_AND_SEG&&!pCurPlate->m_bStdPart&&ParsePartNo(tem_str,&iSeg,NULL))
		{
			parsed=true;
			pPropList->SetItemPropValue(CLDSParamPlate::GetPropID("iSeg"),iSeg.ToString());
		}
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			if (parsed)
				pPlate->iSeg=iSeg;
			pPlate->SetPartNo(tem_str);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("cfgword"))
	{	//ʹ�õ�һ��ְ����ĺŸ��������ְ����ĺ�
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->cfgword = pCurPlate->cfgword;
			pPlate->SetModified(TRUE,FALSE);
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
				{
					pPlate->SetModified(TRUE,FALSE);
					pMirPlate->cfgword=pPlate->cfgword;
				}
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_uStatMatCoef"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->m_uStatMatCoef = atoi(tem_str);
			pPlate->SetModified(TRUE,FALSE);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_hPartWeldParent"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->SetModified(TRUE,FALSE);
			pPlate->m_hPartWeldParent=HexStrToLong(tem_str);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_bDisplayHole"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if (valueStr.CompareNoCase("Yes")==0)
				pPlate->m_bDisplayHole = TRUE;
			else if (valueStr.CompareNoCase("No")==0)
#else 
			if (valueStr.CompareNoCase("��")==0)
				pPlate->m_bDisplayHole = TRUE;
			else if (valueStr.CompareNoCase("��")==0)
#endif
				pPlate->m_bDisplayHole = FALSE;
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					pMirPlate->m_bDisplayHole=pPlate->m_bDisplayHole;
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_bVirtualPart"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if (valueStr.CompareNoCase("Yes")==0)
				pPlate->m_bVirtualPart = TRUE;
			else if (valueStr.CompareNoCase("No")==0)
#else 
			if (valueStr.CompareNoCase("��")==0)
				pPlate->m_bVirtualPart = TRUE;
			else if (valueStr.CompareNoCase("��")==0)
#endif
				pPlate->m_bVirtualPart = FALSE;
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_bUserSpecColor"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if (valueStr.CompareNoCase("Yes")==0)
				pPlate->m_bUserSpecColor = TRUE;
			else if (valueStr.CompareNoCase("No")==0)
#else 
			if (valueStr.CompareNoCase("��")==0)
				pPlate->m_bUserSpecColor = TRUE;
			else if (valueStr.CompareNoCase("��")==0)
#endif
				pPlate->m_bUserSpecColor = FALSE;
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					pMirPlate->m_bUserSpecColor=pPlate->m_bUserSpecColor;
			}
		}
		if (pCurPlate->m_bUserSpecColor)
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("crMaterial"),NULL);
			if (pFindItem==NULL)
			{
				CXhChar100 sText;
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
				pFindItem->m_idProp = CLDSParamPlate::GetPropID("crMaterial");
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				if (pCurPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("crMaterial"));
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str,tem_str+3,97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->crMaterial = crCurSel;
			for (RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
				CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
				if (pMirPlate)
					pMirPlate->crMaterial=pPlate->crMaterial;
			}
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("sNotes"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			_snprintf(pPlate->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
			//��ע���ù����޸� wht 10-10-22
			/*for(RELATIVE_OBJECT *pMirObj=pPlate->relativeObjs.GetFirst();pMirObj;pMirObj=pPlate->relativeObjs.GetNext())
			{
			CLDSParamPlate *pMirPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_PARAMPLATE);
			if(pMirPlate)
			strcpy(pMirPlate->sNotes,pPlate->sNotes);
			}*/
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_cRollProcessType"))
	{
		BYTE cProcessType = valueStr[0]-'0';
		if (cProcessType==3)
			cProcessType=0xFF;
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			pPlate->m_cRollProcessType=cProcessType;
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_cRollOffsetType"))
	{
		BYTE cProcessType = valueStr[0]-'0';
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			pPlate->m_ciRollOffsetType=cProcessType;
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_cDeformType"))
	{
		BYTE cProcessType = valueStr[0]-'0';
		if (cProcessType==4)
			cProcessType=0xFF;
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			pPlate->m_ciDeformType=cProcessType;
		//
		pPropList->DeleteAllSonItems(pItem);
		if (cProcessType==1)
		{
			CItemInfo* lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Deform Coef";
#else 
			lpInfo->m_strPropName = "���Բ�ϵ��";
#endif
			CXhChar100 sText;
			CPropTreeItem *pPropItem = pPropList->InsertItem(pItem,lpInfo,0,true);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_fDeformCoef");
			if (pCurPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_fDeformCoef"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			pPlate->m_fDeformCoef=atof(valueStr);
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("m_fNormOffset"))
	{
		double norm_offset=atof(valueStr);
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
			pPlate->SetNormOffset(norm_offset);
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.origin.x"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.origin.x = atof(valueStr);
			if (pPlate->design.origin.datum_pos_style==0)
				pPlate->design.origin.datum_pos.x=pPlate->ucs.origin.x;
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.origin.y"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.origin.y = atof(valueStr);
			if (pPlate->design.origin.datum_pos_style==0)
				pPlate->design.origin.datum_pos.y=pPlate->ucs.origin.y;
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.origin.z"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.origin.z = atof(valueStr);
			if (pPlate->design.origin.datum_pos_style==0)
				pPlate->design.origin.datum_pos.z=pPlate->ucs.origin.z;
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_x.x"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_x.x = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_x*1,0);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_x.y"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_x.y = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_x*1,0);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_x.z"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_x.z = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_x*1,0);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_y.x"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_y.x = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_y*1,1);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_y.y"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_y.y = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_y*1,1);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_y.z"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_y.z = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_y*1,1);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_z.x"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_z.x = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_z*1,2);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_z.y"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_z.y = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_z*1,2);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("ucs.axis_z.z"))
	{
		for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
		{
			pPlate->ucs.axis_z.z = atof(valueStr);
			rotate_ucs_axis(pPlate->ucs,pPlate->ucs.origin+pPlate->ucs.axis_z*1,2);
		}
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.origin.x"))
	{
		bUpdateWcs=TRUE;
		PLATEEDIT_STATE* pEditState=CLDSParamPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		pEditState->m_fLocalWcsOrgX=console.m_curWCS.origin.x=atof(valueStr);
		//
		oper.UpdatePropItemValue("wcs.origin");
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.origin.y"))
	{
		bUpdateWcs=TRUE;
		PLATEEDIT_STATE* pEditState=CLDSParamPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		pEditState->m_fLocalWcsOrgY=console.m_curWCS.origin.y=atof(valueStr);
		//
		oper.UpdatePropItemValue("wcs.origin");
	}
	else if (pItem->m_idProp==CLDSParamPlate::GetPropID("wcs.axis_x.a"))
	{
		bUpdateWcs=TRUE;
		double alfa=atof(valueStr)*RADTODEG_COEF;	//�ɽǶ���תΪ������
		PLATEEDIT_STATE* pEditState=CLDSParamPlate::m_hashDisplayPlateStates.Add(pFirstPlate->handle);
		console.m_curWCS.axis_x=GEPOINT(cos(alfa),sin(alfa));
		console.m_curWCS.axis_y.Set(-console.m_curWCS.axis_x.y,console.m_curWCS.axis_x.x);
		pEditState->m_fLocalWcsAxisX_x=console.m_curWCS.axis_x.x;
		pEditState->m_fLocalWcsAxisX_y=console.m_curWCS.axis_x.y;
		pEditState->m_fLocalWcsAxisY_x=console.m_curWCS.axis_y.x;
		pEditState->m_fLocalWcsAxisY_y=console.m_curWCS.axis_y.y;
		//
		oper.UpdatePropItemValue("wcs.axis_x");
		oper.UpdatePropItemValue("wcs.axis_y");
	}
	else if (!ModifyParamPlateParamProperty(pPropList,pItem,valueStr,pView->selectObjs))
		return FALSE;
	if (bUpdateWcs==TRUE)
	{
		UCS_STRU csModelWcs=TransWCSFromLocal(pFirstPlate,console.m_curWCS);
		g_pSolidDraw->AddCS(pFirstPlate->handle,csModelWcs);
	}
	for (pPlate=(CLDSParamPlate*)pView->selectObjs.GetFirst();pPlate;pPlate=(CLDSParamPlate*)pView->selectObjs.GetNext())
	{
		pPlate->SetModified();
		pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return TRUE;
}
CString GetBlockListStr();
void CLDSView::DisplayPlateProperty(BOOL bRebuild)
{
	CLDSPlate *pFirstPlate = (CLDSPlate*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pLeafItem,*pSonLeafItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;
	CXhChar100 sText;

	CPropListOper<CLDSPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&selectObjs);
	const int GROUP_GENERAL=1,GROUP_POS=2,GROUP_PROFILE=3,GROUP_MODIFY=4,GROUP_OTHER=5;
	if (bRebuild)
	{	//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(5);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"Position");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"Profile");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_MODIFY-1,"Modify");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_OTHER-1,"Others");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"λ��");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_MODIFY-1,"�޸�");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_OTHER-1,"����");
#endif
		pPropDlg->RefreshTabCtrl(CLDSPlate::m_iCurDisplayPropGroup);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_PLATE;
		pPropList->SetModifyValueFunc(ModifyPlateProperty);
		pPropList->SetButtonClickFunc(PlateButtonClick);
		pPropList->SetModifyStatusFunc(ModifyPlateItemStatus);
		pPropList->SetPropHelpPromptFunc(FirePlateHelpStr);
		pPropList->SetPopMenuClickFunc(FirePlatePopMenuClick);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if (!pFirstPlate->BelongModel()->IsTowerModel()||pFirstPlate->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//�ְ������Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Plate Basic Information";
		lpInfo->m_strPropHelp = "The basic information of plate";
#else 
		lpInfo->m_strPropName = "�ְ������Ϣ";
		lpInfo->m_strPropHelp = "�ְ屾��Ϣ";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pGroupItem->m_idProp = CLDSPlate::GetPropID("basicInfo");
		if (CLDSPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|
			GetSingleWord(GROUP_PROFILE)|GetSingleWord(GROUP_OTHER);
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "Plate's handle is typical unique identifier string in the  LDS system.";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�ְ�����LDSϵͳ�жԸְ��ȫ��Ψһ��ʶ";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("handle");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|
			GetSingleWord(GROUP_PROFILE)|GetSingleWord(GROUP_OTHER);
		//�����ڵ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Dependant node";
		lpInfo->m_strPropHelp = "Dependant node";
#else 
		lpInfo->m_strPropName = "�����ڵ���";
		lpInfo->m_strPropHelp = "��Ƹְ��������ѡ������ӽڵ�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.hNode");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//�����˼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Dependant rod";
		lpInfo->m_strPropHelp = "Dependant rod";
#else 
		lpInfo->m_strPropName = "�����˼����";
		lpInfo->m_strPropHelp = "��Ƹְ��������ѡ������Ӷ�λ�˼�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.hRod");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "Plate in the layer";
#else 
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "�ְ�����ͼ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("layer");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
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
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("cfgword");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Material";
		lpInfo->m_strPropHelp = "The material of plate.";
#else 
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "�ְ����";
#endif
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("cMaterial");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|
			GetSingleWord(GROUP_PROFILE)|GetSingleWord(GROUP_OTHER);
		//�����ȼ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Quality Grade";
		lpInfo->m_strPropHelp = "The quality grade of plate.";
		lpInfo->m_cmbItems = "Default|A|B|C|D|";
#else 
		lpInfo->m_strPropName = "�����ȼ�";
		lpInfo->m_strPropHelp = "�ְ������ȼ�";
		lpInfo->m_cmbItems = "Ĭ��|A|B|C|D|";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_cQualityLevel");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|
			GetSingleWord(GROUP_PROFILE)|GetSingleWord(GROUP_OTHER);
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Thickness";
		lpInfo->m_strPropHelp = "The thickness of plate(mm)��";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�ְ�ĺ��(mm)��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("thick");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|
			GetSingleWord(GROUP_PROFILE)|GetSingleWord(GROUP_OTHER);
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
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("iSeg");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
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
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("sPartNo");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|
			GetSingleWord(GROUP_PROFILE)|GetSingleWord(GROUP_OTHER);
		//
		if (pFirstPlate->IsShadowObject())
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
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("shadowInfo");
			pPropItem->m_bHideChildren = FALSE;
			if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			pPropItem->SetReadOnly(FALSE);
			//Ӱ��ĸ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Mother Part";
			lpInfo->m_strPropHelp = "The current part is project mother part.The current part is original part when the handle is Ox0.";
#else 
			lpInfo->m_strPropName = "Ӱ��ĸ����";
			lpInfo->m_strPropHelp = "��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("m_hMotherObj");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//�������±��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Synchornize part label";
			lpInfo->m_strPropHelp = "Synchornize part label and segment number with mother part.";
			lpInfo->m_cmbItems = "Yes|No";
#else
			lpInfo->m_strPropName = "�������±��";
			lpInfo->m_strPropHelp = "��ǰӰ�乹������Ӱ��ĸ����ͬ�����¹�����š��κ���Ϣ��";
			lpInfo->m_cmbItems = "��|��";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("m_bSyncShadowPartNo");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstPlate->IsBlockRefShadowObject())
			{
				//������������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block Assembly";
				lpInfo->m_strPropHelp = "The reference handle of the vest in block.";
#else 
				lpInfo->m_strPropName = "������������";
				lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("m_hBlockRef");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
				pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else
		{
			//��������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Relative Part";
			lpInfo->m_strPropHelp = "The part associated with the current part and symmetrical type";
#else 
			lpInfo->m_strPropName = "��������";
			lpInfo->m_strPropHelp = "�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("relativeObjs");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//��������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Dependent Block Handle";
			lpInfo->m_strPropHelp = "The handle of the dependent block.";
#else 
			lpInfo->m_strPropName = "��������";
			lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
			lpInfo->m_cmbItems=GetBlockListStr();
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("m_hBlock");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//���Ӹ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weld Parent Part";
		lpInfo->m_strPropHelp = "If the part need to weld to other part, then the property is the handle of the part's attached welded part;If the part doesn't need to be welded,the value is 0X0.";
#else 
		lpInfo->m_strPropName = "���Ӹ�����";
		lpInfo->m_strPropHelp = "����˹�����Ҫ���ӵ�����������,��ô�����Ծ��Ǳ������������ĺ��ӹ����ľ��;����˹�������Ҫ����,��ֵΪ0X0��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_hPartWeldParent");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
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
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_uStatMatCoef");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��ʾ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Hole Visible";
		lpInfo->m_strPropHelp = "Hole visible";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "��ʾ��";
		lpInfo->m_strPropHelp = "�Ƿ���ʾ��";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_bDisplayHole");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		/*��ʾ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Profile Visible";
		lpInfo->m_strPropHelp = "Profile visible";
		lpInfo->m_cmbItems = "None|Actual Profile|Spread Profile";
#else
		lpInfo->m_strPropName = "��ʾ������";
		lpInfo->m_strPropHelp = "ʵ��ģʽ�¸ְ����Ƿ���������";
		if(pFirstPlate->face_N==1)
			lpInfo->m_cmbItems = "0.����ʾ|1.������";
		else
			lpInfo->m_cmbItems = "0.����ʾ|1.������|2.������(չ��)";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_cPickedDisplayMode");
		if(pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);	//GetSingleWord(GROUP_POS)| Ϊ���������ͳһ����ʾ��<λ��>ҳ�� wjh-2017.11.20
		*/
		//��ʾ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual Part";
		lpInfo->m_strPropHelp = "Virtual Part";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "����ְ�";
		lpInfo->m_strPropHelp = "";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_bVirtualPart");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
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
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if (pFirstPlate->m_bUserSpecColor)
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
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSPlate::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if (pFirstPlate->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
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
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("sNotes");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
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
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		//pGroupItem->m_bHideChildren = TRUE;
		pGroupItem->m_idProp = CLDSPlate::GetPropID("assemblePos");
		if (CLDSPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//װ����㷨��λ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Normal Offset";
		lpInfo->m_strPropHelp = "The offset distance of plane to the datum node in the normal .";
#else 
		lpInfo->m_strPropName = "���㷨��λ��";
		lpInfo->m_strPropHelp = "װ����㷨��λ�á�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_fNormOffset");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//�ְ嶨λ����
		if (pFirstPlate->designInfo.origin.datum_pos_style==10)
		{	//���ڵ��ı����ȷֵ�
			//װ����㷨��λ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Plate Locating Parameter";
			lpInfo->m_strPropHelp = "Plate locating parameter��";
#else 
			lpInfo->m_strPropName = "�ְ嶨λ����";
			lpInfo->m_strPropHelp = "�ְ嶨λ������";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("designinfo.des_para");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//��ϽǸ־��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Rod Handle";
			lpInfo->m_strPropHelp="The handle of datum rod;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
			lpInfo->m_strPropName="��׼�˼����";
			lpInfo->m_strPropHelp="��׼�˼��������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.hLinePart");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//ʼ�˻�׼�ڵ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Start Node Handle";
			lpInfo->m_strPropHelp="The handle of start node;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
			lpInfo->m_strPropName="ʼ�˻�׼�ڵ���";
			lpInfo->m_strPropHelp="ʼ�˻�׼�ڵ�������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.hStartNode");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//ʼ��ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Start Offset";
			lpInfo->m_strPropHelp="The offset of start node along the direction of start node to end node.";
#else 
			lpInfo->m_strPropName="ʼ��ƫ����";
			lpInfo->m_strPropHelp="ʼ�˽ڵ�����ʼ�˽ڵ�->�ն˽ڵ㷽���ƫ������";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.start_offset_dist");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//�ն˻�׼�ڵ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="End Node Handle";
			lpInfo->m_strPropHelp="The handle of end node;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
			lpInfo->m_strPropName="�ն˻�׼�ڵ���";
			lpInfo->m_strPropHelp="�ն˻�׼�ڵ�������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.hEndNode");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//�ն�ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="End Offset";
			lpInfo->m_strPropHelp="The offset of head node along the direction of end node to start node.";
#else 
			lpInfo->m_strPropName="�ն�ƫ����";
			lpInfo->m_strPropHelp="�ն˽ڵ������ն˽ڵ�->ʼ�˽ڵ㷽���ƫ������";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.end_offset_dist");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//ƫ�Ʊ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Scale";
			lpInfo->m_strPropHelp="Calculate data from start node's offset position.";
#else 
			lpInfo->m_strPropName="ƫ�Ʊ���";
			lpInfo->m_strPropHelp="��ʼ�˽ڵ㿼��ʼ��ƫ�����Ժ��λ�ÿ�ʼ���㡣";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_scale");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Distance";
			lpInfo->m_strPropHelp="Calculated on offset ratio first and then get distance after offset.";
#else 
			lpInfo->m_strPropName="ƫ�ƾ���";
			lpInfo->m_strPropHelp="��ƫ�Ʊ��������Ժ��ٽ���ƫ�Ƶľ��롣";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_dist");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		}
		//װ������ϵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Assemble CS";
		lpInfo->m_strPropHelp = "Assemble CS";
#else 
		lpInfo->m_strPropName = "װ������ϵ";
		lpInfo->m_strPropHelp = "װ������ϵ��";
#endif
		CPropTreeItem *pAcsItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pAcsItem->m_idProp = CLDSPlate::GetPropID("ucs");
		if (CLDSPlate::GetPropStatus(pAcsItem->m_idProp)!=-1)
			pAcsItem->m_bHideChildren = CLDSPlate::GetPropStatus(pAcsItem->m_idProp);
		pAcsItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
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
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("ucs.origin");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		/*
		//װ����㷨��λ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "���㷨��λ��";
		lpInfo->m_strPropHelp = "װ����㷨��λ�á�";
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("m_fNormOffset");
		if(pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		*/
		//װ������ϵԭ��λ��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS.";
#else 
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "װ������ϵԭ��λ��X���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.origin.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//װ������ϵԭ��λ��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS.";
#else 
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Y���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.origin.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//װ������ϵԭ��λ��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS";
#else 
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Z���������";
#endif
		pLeafItem= pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.origin.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
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
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_x");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵX��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X ";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS's X axis.";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "װ������ϵX��X���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_x.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵX��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y ";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS's X axis.";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "װ������ϵX��Y���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_x.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵX��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z ";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS's X axis.";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "װ������ϵX��Z���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_x.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
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
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_y");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵY��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X ";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS's Y axis.";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "װ������ϵY��X���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_y.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵY��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y ";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS's Y axis.";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "װ������ϵY��Y���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_y.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵY��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z ";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS's Y axis.";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "װ������ϵY��Z���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_y.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
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
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_z");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//װ������ϵZ��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X ";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS's Z axis.";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "װ������ϵZ��X���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_z.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//װ������ϵZ��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y ";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS's Z axis.";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "װ������ϵZ��Y���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_z.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//װ������ϵZ��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z ";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS's Z axis.";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "װ������ϵZ��Z���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSPlate::GetPropID("ucs.axis_z.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//�ְ幤����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Work Plane";
		lpInfo->m_strPropHelp = "The datum point of plate's work-plane  is  origin point of assembly CS,the Z axis diretion of plate is the direction of work normal.";
#else 
		lpInfo->m_strPropName = "�ְ幤����";
		lpInfo->m_strPropHelp = "�ְ幤�����׼��Ϊװ������ϵԭ�㣬�����淨��Ϊ�ְ�Z�᷽��";
#endif
		pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("plate_work_plane");
		pPropItem->m_bHideChildren = TRUE;
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_POS);
		//�ְ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
		lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Plate Profile";
		lpInfo->m_strPropHelp = "The plate profile information";
		lpInfo->m_cmbItems="Splay Fold Plate|Curved Beam Fold Plate|Bolt Pad|Joint Plate|Rib Plate|Connect Plate";
#else 
		lpInfo->m_strPropName = "�ְ�����";
		lpInfo->m_strPropHelp = "�ְ�������Ϣ";
#ifdef __STATION_
		lpInfo->m_cmbItems="���ܶ������ְ�|���������۵���|��˨���|��ͷ���ְ�|��ǿ�߰�|��ͨ���Ӱ�";
#else
		lpInfo->m_cmbItems="��˨���|��ͷ���ְ�|��ǿ�߰�|��ͨ���Ӱ�";
#endif
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pGroupItem->m_idProp = CLDSPlate::GetPropID("plateProfile");
		if (pFirstPlate->GetPropValueStr(pGroupItem->m_idProp,sText)>0)
			pGroupItem->m_lpNodeInfo->m_strPropValue = sText;
		if (CLDSPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		//pGroupItem->SetReadOnly();
		//�����ڻ��뾶
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Inner Circle Radius";
		lpInfo->m_strPropHelp = "If the plate exists inner circle in basis points,the property will be the radius of inner circle,otherwise should be set to 0.";
#else 
		lpInfo->m_strPropName = "�����ڻ��뾶";
		lpInfo->m_strPropHelp = "������ڻ��㴦�����ڻ�,��Ϊ�ڻ��İ뾶������Ӧ��Ϊ0��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("m_fInnerRadius");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_bHideChildren=TRUE;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		if (pFirstPlate->m_fInnerRadius>0)
		{
			//�ڻ����߷���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Inner Circle Normal";
			lpInfo->m_strPropHelp = "The normal of inner circle.";
#else 
			lpInfo->m_strPropName = "�ڻ����߷���";
			lpInfo->m_strPropHelp = "�ڻ����߷���";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerColumnNorm");
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//�ڻ�����X����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X ";
			lpInfo->m_strPropHelp = "The X coordinate component of inner circle's normal.";
#else 
			lpInfo->m_strPropName = "�ڻ�����X����";
			lpInfo->m_strPropHelp = "�ڻ����߷����X����ֵ��";
#endif
			pSonLeafItem = pPropList->InsertItem(pLeafItem,lpInfo,-1);
			pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerColumnNorm.x");
			if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
				pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonLeafItem->m_idProp);
			pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//�ڻ�����Y����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y ";
			lpInfo->m_strPropHelp = "The Y coordinate component of inner circle's normal.";
#else 
			lpInfo->m_strPropName = "�ڻ�����Y����";
			lpInfo->m_strPropHelp = "�ڻ����߷����Y����ֵ��";
#endif
			pSonLeafItem = pPropList->InsertItem(pLeafItem,lpInfo,-1);
			pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerColumnNorm.y");
			if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
				pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonLeafItem->m_idProp);
			pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//�ڻ�����Z����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z ";
			lpInfo->m_strPropHelp = "The Z coordinate component of inner circle's normal.";
#else 
			lpInfo->m_strPropName = "�ڻ�����Z����";
			lpInfo->m_strPropHelp = "�ڻ����߷����Z����ֵ��";
#endif
			pSonLeafItem = pPropList->InsertItem(pLeafItem,lpInfo,-1);
			pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerColumnNorm.z");
			if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
				pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonLeafItem->m_idProp);
			pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//�ڻ�λ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Inner Circle Center Position";
			lpInfo->m_strPropHelp = "The inner circle position is the relative coordinates of plate coordinates.";
#else 
			lpInfo->m_strPropName = "�ڻ�λ��";
			lpInfo->m_strPropHelp = "�ڻ�λ��,������Ϊ�ְ�����ϵ�µ�������ꡣ";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerOrigin");
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//�ڻ�λ��X����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Inner Circle X Coordinate";
			lpInfo->m_strPropHelp = "The X coordinate component of inner circle's center.";
#else 
			lpInfo->m_strPropName = "�ڻ�λ��X����";
			lpInfo->m_strPropHelp = "�ڻ�λ�õ�X����ֵ��";
#endif
			pSonLeafItem = pPropList->InsertItem(pLeafItem,lpInfo,-1);
			pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerOrigin.x");
			if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
				pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonLeafItem->m_idProp);
			pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//�ڻ�λ��Y����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Inner Circle Y Coordinate";
			lpInfo->m_strPropHelp = "The Y coordinate component of inner circle's center.";
#else 
			lpInfo->m_strPropName = "�ڻ�λ��Y����";
			lpInfo->m_strPropHelp = "�ڻ�λ�õ�Y����ֵ��";
#endif
			pSonLeafItem = pPropList->InsertItem(pLeafItem,lpInfo,-1);
			pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerOrigin.y");
			if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
				pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonLeafItem->m_idProp);
			pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//�ڻ�λ��Z����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Inner Circle Z Coordinate";
			lpInfo->m_strPropHelp = "The Z coordinate component of inner circle's center.";
#else 
			lpInfo->m_strPropName = "�ڻ�λ��Z����";
			lpInfo->m_strPropHelp = "�ڻ�λ�õ�Z����ֵ��";
#endif
			pSonLeafItem = pPropList->InsertItem(pLeafItem,lpInfo,-1);
			pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_tInnerOrigin.z");
			if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
				pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pSonLeafItem->m_idProp);
			pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		}
		//�Ƿ�������Բ��������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "IF Flexible Design";
		lpInfo->m_strPropHelp = "Whethe to enable the flexible parametric design";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "��Ԥ�������";
		lpInfo->m_strPropHelp = "�Ƿ�������Բ��������";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("designInfo.m_bEnableFlexibleDesign");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		//�ְ��������ɷ�ʽ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems = "0.Standard Profile|1.Customized Profile|2.Envelope Profile|3.Just Update Coordinate";
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "Create Profile Style";
		lpInfo->m_strPropHelp = "The creating style of profile";
#else 
		lpInfo->m_cmbItems = "0.��׼����|1.��������|2.��������|3.����������";
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�������ɷ�ʽ";
		lpInfo->m_strPropHelp = "�������ɷ�ʽ";
#endif
		lpInfo->m_controlType = PIT_COMBO;
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("iProfileStyle0123");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		//�ְ��������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Plate Type";
		lpInfo->m_strPropHelp = "plate type";
		lpInfo->m_cmbItems = "0. Planar Plate|1. 2 Planar Plate|2. 3 Planar Plate|3. Folded Plate|4. Bolt Gasket";
#else 
		lpInfo->m_strPropName = "�ְ�����";
		lpInfo->m_strPropHelp = "�ְ�����";
		lpInfo->m_cmbItems = "0.�����|1.˫���|2.�����|3.�۵���|4.��˨���";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("face_N");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		Update_Plate_Face_N_TreeItem(pPropList,pGroupItem);
		//��������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Profile vertex";
		lpInfo->m_strPropName = "Profile vertex";
		lpInfo->m_strPropHelp = "Profile vertex";
#else 
		lpInfo->m_cmbItems="��������";
		lpInfo->m_strPropName = "��������";
		lpInfo->m_strPropHelp = "��������";
#endif
		pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("plateParam");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		//�ְ����Ӹ˼�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMON;//BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="View|Import";
		lpInfo->m_strPropName = "Link Rod";
		lpInfo->m_strPropHelp = "Link rod";
#else 
		lpInfo->m_cmbItems="�鿴|����";
		lpInfo->m_strPropName = "���Ӹ˼�";
		lpInfo->m_strPropHelp = "���Ӹ˼�";
#endif
		pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pPropItem->m_idProp = CLDSPlate::GetPropID("plateLinkPoles");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		//�ְ��ߴ���ʽ
		PROFILE_VERTEX* pVertex=NULL;
		for (pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
		{
			if (pVertex->m_bRollEdge)
				break;
		}
		if (pVertex)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rolling Style of Plate Edge";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Mechanical Priority|1.Processing Priority|2.Default ";
#else
			lpInfo->m_strPropName = "�ְ��ߴ���ʽ";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.�������|1.���ٵ���|2.Ĭ��ֵ";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("m_cRollProcessType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			//�ְ����߼��㷽ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "RollLine Cal Type";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Outside|1.Inside";
#else
			lpInfo->m_strPropName = "���ƫ�Ʒ�ʽ";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.���|1.�ڲ�|2.Ĭ��ֵ";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("m_cRollOffsetType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		}
		if (pFirstPlate->GetFaceN()>1)
		{	//�ְ�������δ���ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Deform Type";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Assembly Plane|1.Middle Plane|2.Raised Plane|3.Sunk Plane";
#else
			lpInfo->m_strPropName = "�������δ���ʽ";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.װ����|1.���Բ�|2.͹��|3.����|4.Ĭ��ֵ";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("m_cDeformType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			if (sText[0]-'0'==1)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Deform Coef";
				lpInfo->m_strPropHelp = "";
#else
				lpInfo->m_strPropName = "���Բ�ϵ��";
				lpInfo->m_strPropHelp = "���л������ε����Բ�ϵ��";
#endif
				pSonLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_fDeformCoef");
				if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
					pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
				if (pFirstPlate->GetFaceN()==3)	//�����
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Deform Coef2";
					lpInfo->m_strPropHelp = "";
#else
					lpInfo->m_strPropName = "���Բ�ϵ��2";
					lpInfo->m_strPropHelp = "���л������εĵڶ��������ߵ����Բ�ϵ��";
#endif
					pSonLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pSonLeafItem->m_idProp = CLDSPlate::GetPropID("m_fDeformCoef2");
					if (pFirstPlate->GetPropValueStr(pSonLeafItem->m_idProp,sText)>0)
						pSonLeafItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
				}
			}
		}
		//�ӹ�����
		pGroupItem=oper.InsertPropItem(pRoot,"manuProcess");
		if (CLDSPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_OTHER);
		//���ӹ���
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bWeldPart");
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_OTHER);
		if (pFirstPlate->m_bWeldPart)
		{	//�û�ָ��������
			oper.InsertCmbListPropItem(pPropItem,"iWeldLineStyle");
			if (pFirstPlate->designInfo.iWeldLineStyle==1)
			{
				//������ʼ��
				oper.InsertEditPropItem(pPropItem,"designInfo.weld_start_high");
				//������ֹ��
				oper.InsertEditPropItem(pPropItem,"designInfo.weld_end_high");
				//�������
				oper.InsertEditPropItem(pPropItem,"designInfo.weld_width");
			}
		}
		//�¿ڹ���
		oper.InsertCmbListPropItem(pGroupItem,"m_bNeedFillet");
		if (selectObjs.GetNodeNum()==1&&pFirstPlate->face_N==1&&g_sysPara.display.bPartSketch)
		{	//��ʱ��֧����ʾ������������б� wht 10-09-02
			pGroupItem=oper.InsertPropItem(pRoot,"VertexList");
			if (CLDSPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
				pGroupItem->m_bHideChildren = CLDSPlate::GetPropStatus(pGroupItem->m_idProp);
			pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_OTHER);
			int i=0;
			for (PROFILE_VERTEX *pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
			{
				CXhChar16 sPropId("%c",'A'+i);
				CXhChar16 sPosX("%s.x",(char*)sPropId);
				CXhChar16 sPosY("%s.y",(char*)sPropId);
				//����
				pPropItem=oper.InsertButtonPropItem(pGroupItem,sPropId);
				pPropItem->m_bHideChildren = FALSE;
				if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=GetSingleWord(GROUP_OTHER);
				//װ������ϵY��X�������
				oper.InsertEditPropItem(pPropItem,sPosX);
				//װ������ϵY��Y�������
				oper.InsertEditPropItem(pPropItem,sPosY);
				i++;
			}
		}
		//�ְ�༭����
		//oper.SetPreferPropGroup(GetSingleWord(GROUP_MODIFY));
		pGroupItem=oper.InsertPropItem(pRoot,"PlateEdit");
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_MODIFY);
		//��������ϵ
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"WorkUcs");
		pPropItem->m_bHideChildren=FALSE;
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		//����ϵԭ��
		pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"wcs.origin");
		pLeafItem->SetReadOnly();
		oper.InsertEditPropItem(pLeafItem,"wcs.origin.x");
		oper.InsertEditPropItem(pLeafItem,"wcs.origin.y");
		//����ϵX��
		pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"wcs.axis_x");
		pLeafItem->SetReadOnly();
		oper.InsertEditPropItem(pLeafItem,"wcs.axis_x.a");
		//����ϵY��
		pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"wcs.axis_y");
		pLeafItem->SetReadOnly();
		//�༭��(���붥�㡢������˨������)
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"InsertProfileVertex");		//
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"AddNewHole");	//
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"DeleteFeature");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"FilletVertex");
		pPropItem->SetReadOnly();
		//oper.SetPreferPropGroup(0);
	}
	else
		DisplayPlateNeedlessRebuildProperty();
	pPropList->Redraw();
}

void CLDSView::DisplayPlateNeedlessRebuildProperty()
{
	CLDSPlate *pFirstPlate;
	pFirstPlate = (CLDSPlate*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pLeafItem,*pRoot=pPropList->GetRootItem();
	const int GROUP_GENERAL=1,GROUP_POS=2,GROUP_PROFILE=3,GROUP_OTHER=4;
	CItemInfo* lpInfo;
	CXhChar100 sText;
	CPropTreeItem *pFindItem,*pParentItem;
	SetPartMutiObjsProp(pFirstPlate,"handle");
	SetPartMutiObjsProp(pFirstPlate,"layer");
	SetPartMutiObjsProp(pFirstPlate,"cMaterial");
	SetPartMutiObjsProp(pFirstPlate,"m_cQualityLevel");
	SetPartMutiObjsProp(pFirstPlate,"thick");
	SetPartMutiObjsProp(pFirstPlate,"iSeg");
	SetPartMutiObjsProp(pFirstPlate,"sPartNo");
	SetPartMutiObjsProp(pFirstPlate,"cfgword");
	SetPartMutiObjsProp(pFirstPlate,"m_bDisplayHole");
	SetPartMutiObjsProp(pFirstPlate,"m_cPickedDisplayMode");
	SetPartMutiObjsProp(pFirstPlate,"m_bVirtualPart");
	SetPartMutiObjsProp(pFirstPlate,"m_fInnerRadius");

	SetPartMutiObjsProp(pFirstPlate,"wcs.origin");
	SetPartMutiObjsProp(pFirstPlate,"wcs.origin.x");
	SetPartMutiObjsProp(pFirstPlate,"wcs.origin.y");
	SetPartMutiObjsProp(pFirstPlate,"wcs.axis_x");
	SetPartMutiObjsProp(pFirstPlate,"wcs.axis_x.a");
	SetPartMutiObjsProp(pFirstPlate,"wcs.axis_y");

	pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_hBlock"),&pParentItem);
	if (pFindItem)
	{
		if (pFirstPlate->IsShadowObject())
		{	//����ͨ�����л���Ӱ�乹��
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_hBlock"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("relativeObjs"));
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
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,12,true);
			pPropItem->m_idProp = CLDSPlate::GetPropID("shadowInfo");
			if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
			pPropItem->SetReadOnly(FALSE);
			//Ӱ��ĸ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Mother Part";
			lpInfo->m_strPropHelp = "The current part is project mother part.The current part is original part when the handle is Ox0.";
#else 
			lpInfo->m_strPropName = "Ӱ��ĸ����";
			lpInfo->m_strPropHelp = "��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("m_hMotherObj");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			//�������±��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Synchornize part label";
			lpInfo->m_strPropHelp = "Synchornize part label and segment number with mother part.";
			lpInfo->m_cmbItems = "Yes|No";
#else
			lpInfo->m_strPropName = "�������±��";
			lpInfo->m_strPropHelp = "��ǰӰ�乹������Ӱ��ĸ����ͬ�����¹�����š��κ���Ϣ��";
			lpInfo->m_cmbItems = "��|��";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("m_bSyncShadowPartNo");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstPlate->IsBlockRefShadowObject())
			{
				//������������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block Assembly";
				lpInfo->m_strPropHelp = "The reference handle of the vest in block.";
#else 
				lpInfo->m_strPropName = "������������";
				lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("m_hBlockRef");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			}
		}
		else
		{
			SetPartMutiObjsProp(pFirstPlate,"m_hBlock");
			SetPartMutiObjsProp(pFirstPlate,"relativeObjs");
		}
	}
	pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("shadowInfo"),&pParentItem);
	if (pFindItem)
	{
		if (!pFirstPlate->IsShadowObject())
		{	//��Ӱ�乹���л�����ͨ����
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("shadowInfo"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_hMotherObj"));
			pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_hBlockRef"),NULL);
			if (pFindItem)
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_hBlockRef"));
			pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_hBlock"),NULL);
			if (pFindItem!=NULL)
				SetPartMutiObjsProp(pFirstPlate,"m_hBlock");
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
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
				pPropItem->m_idProp = CLDSPlate::GetPropID("m_hBlock");
				if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
				pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			}
			pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("relativeObjs"),NULL);
			if (pFindItem!=NULL)
				SetPartMutiObjsProp(pFirstPlate,"relativeObjs");
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
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,12,true);
				pPropItem->m_idProp = CLDSPlate::GetPropID("relativeObjs");
				if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
		{
			SetPartMutiObjsProp(pFirstPlate,"m_hMotherObj");
			pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_hBlockRef"),&pParentItem);
			if (pFindItem&&pFirstPlate->IsBlockRefShadowObject())
			{
				SetPartMutiObjsProp(pFirstPlate,"m_hBlockRef");
				SetPartMutiObjsProp(pFirstPlate,"m_hBlock");
			}
			else if (pFindItem&&!pFirstPlate->IsBlockRefShadowObject())
			{
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_hBlockRef"));
				pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_hBlock"));
			}
			else if (pFindItem==NULL&&pFirstPlate->IsBlockRefShadowObject())
			{
				pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_hMotherObj"),&pParentItem);
				//������������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block Assembly";
				lpInfo->m_strPropHelp = "The reference handle of the vest in block.";
#else 
				lpInfo->m_strPropName = "������������";
				lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
				pLeafItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("m_hBlockRef");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			}
		}
	}
	SetPartMutiObjsProp(pFirstPlate,"designInfo.hNode");
	SetPartMutiObjsProp(pFirstPlate,"designInfo.hRod");
	SetPartMutiObjsProp(pFirstPlate,"plateProfile");
	if (pFirstPlate->m_fInnerRadius>0)
	{
		pParentItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_fInnerRadius"),&pFindItem);
		if (pParentItem)
		{
			pPropItem=NULL;
			long idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm");
			pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if (pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Inner Circle Normal";
				lpInfo->m_strPropHelp = "The normal of inner circle.";
#else 
				lpInfo->m_strPropName = "�ڻ����߷���";
				lpInfo->m_strPropHelp = "�ڻ����߷���";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
				pPropItem->m_idProp = idProp;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
			else
				SetPartMutiObjsProp(pFirstPlate,"m_tInnerColumnNorm");
			if (pPropItem)
			{
				idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm.x");
				pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "X ";
					lpInfo->m_strPropHelp = "The X coordinate component of inner circle's normal.";
#else 
					lpInfo->m_strPropName = "�ڻ�����X����";
					lpInfo->m_strPropHelp = "�ڻ����߷����X����ֵ��";
#endif
					pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pFindItem->m_idProp = idProp;
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				}
				else
					SetPartMutiObjsProp(pFirstPlate,"m_tInnerColumnNorm.x");
				idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm.y");
				pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Y ";
					lpInfo->m_strPropHelp = "The Y coordinate component of inner circle's normal.";
#else 
					lpInfo->m_strPropName = "�ڻ�����Y����";
					lpInfo->m_strPropHelp = "�ڻ����߷����Y����ֵ��";
#endif
					pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pFindItem->m_idProp = idProp;
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				}
				else
					SetPartMutiObjsProp(pFirstPlate,"m_tInnerColumnNorm.y");
				idProp=CLDSPlate::GetPropID("m_tInnerColumnNorm.z");
				pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Z ";
					lpInfo->m_strPropHelp = "The Z coordinate component of inner circle's normal.";
#else 
					lpInfo->m_strPropName = "�ڻ�����Z����";
					lpInfo->m_strPropHelp = "�ڻ����߷����Z����ֵ��";
#endif
					pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pFindItem->m_idProp = idProp;
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				}
				else
					SetPartMutiObjsProp(pFirstPlate,"m_tInnerColumnNorm.z");
			}
			//�ڻ�λ��
			pPropItem=NULL;
			idProp=CLDSPlate::GetPropID("m_tInnerOrigin");
			pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if (pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Inner Circle Position";
				lpInfo->m_strPropHelp = "The inner circle position is the relative coordinates of plate CS.";
#else 
				lpInfo->m_strPropName = "�ڻ�λ��";
				lpInfo->m_strPropHelp = "�ڻ�λ��,������Ϊ�ְ�����ϵ�µ�������ꡣ";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
				pPropItem->m_idProp = idProp;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
			else
				SetPartMutiObjsProp(pFirstPlate,"m_tInnerOrigin");
			if (pPropItem)
			{
				idProp=CLDSPlate::GetPropID("m_tInnerOrigin.x");
				pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Inner Center X Coordinate";
					lpInfo->m_strPropHelp = "The X coordinate component of inner circle's center.";
#else 
					lpInfo->m_strPropName = "�ڻ�λ��X����";
					lpInfo->m_strPropHelp = "�ڻ�λ�õ�X����ֵ��";
#endif
					pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pFindItem->m_idProp = idProp;
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				}
				else
					SetPartMutiObjsProp(pFirstPlate,"m_tInnerOrigin.x");
				idProp=CLDSPlate::GetPropID("m_tInnerOrigin.y");
				pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Inner Center Y Coordinate";
					lpInfo->m_strPropHelp = "The Y coordinate component of inner circle's center.";
#else 
					lpInfo->m_strPropName = "�ڻ�λ��Y����";
					lpInfo->m_strPropHelp = "�ڻ�λ�õ�Y����ֵ��";
#endif
					pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pFindItem->m_idProp = idProp;
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				}
				else
					SetPartMutiObjsProp(pFirstPlate,"m_tInnerOrigin.y");
				idProp=CLDSPlate::GetPropID("m_tInnerOrigin.z");
				pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Inner Center Z Coordinate";
					lpInfo->m_strPropHelp = "The Z coordinate component of inner circle's center.";
#else 
					lpInfo->m_strPropName = "�ڻ�λ��Z����";
					lpInfo->m_strPropHelp = "�ڻ�λ�õ�Z����ֵ��";
#endif
					pFindItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pFindItem->m_idProp = idProp;
					if (pFirstPlate->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pFindItem->m_idProp);
					pFindItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				}
				else
					SetPartMutiObjsProp(pFirstPlate,"m_tInnerOrigin.z");
			}
		}
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.x"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.y"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerColumnNorm.z"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.x"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.y"));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_tInnerOrigin.z"));
	}
	SetPartMutiObjsProp(pFirstPlate,"m_uStatMatCoef");
	SetPartMutiObjsProp(pFirstPlate,"m_hPartWeldParent");
	SetPartMutiObjsProp(pFirstPlate,"m_bUserSpecColor");
	if (pFirstPlate->m_bUserSpecColor)
	{
		if (!SetPartMutiObjsProp(pFirstPlate,"crMaterial"))
		{
			CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_bUserSpecColor"),NULL);
			if (pFindItem)
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
				pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1);
				pPropItem->m_idProp = CLDSPlate::GetPropID("crMaterial");
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
				if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			}
		}
	}
	else
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("crMaterial"));
	SetPartMutiObjsProp(pFirstPlate,"sNotes");
	SetPartMutiObjsProp(pFirstPlate,"m_fNormOffset");
	pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designinfo.des_para"));
	if (pFirstPlate->designInfo.origin.datum_pos_style==10)
	{	//���ڵ��ı����ȷֵ�
		CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("assemblePos"),NULL);
		if (pFindItem)
		{
			//װ����㷨��λ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Plate Locating Parameter";
			lpInfo->m_strPropHelp = "Plate locating parameter��";
#else 
			lpInfo->m_strPropName = "�ְ嶨λ����";
			lpInfo->m_strPropHelp = "�ְ嶨λ������";
#endif
			pPropItem = pPropList->InsertItem(pFindItem,lpInfo,1);
			pPropItem->m_idProp = CLDSPlate::GetPropID("designinfo.des_para");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//��ϽǸ־��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Datum Rod Handle";
			lpInfo->m_strPropHelp="The handle of datum rod;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
			lpInfo->m_strPropName="��׼�˼����";
			lpInfo->m_strPropHelp="��׼�˼��������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.hLinePart");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//ʼ�˻�׼�ڵ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Start Node Handle";
			lpInfo->m_strPropHelp="The handle of start node;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
			lpInfo->m_strPropName="ʼ�˻�׼�ڵ���";
			lpInfo->m_strPropHelp="ʼ�˻�׼�ڵ�������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.hStartNode");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//ʼ��ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Start Offset";
			lpInfo->m_strPropHelp="The offset of head node along the direction of start node to end node.";
#else 
			lpInfo->m_strPropName="ʼ��ƫ����";
			lpInfo->m_strPropHelp="ʼ�˽ڵ�����ʼ�˽ڵ�->�ն˽ڵ㷽���ƫ������";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.start_offset_dist");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//�ն˻�׼�ڵ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="End Node Handle";
			lpInfo->m_strPropHelp="The handle of end node;consider the rod's odd length information when the datum node as datum part's endpoint.";
#else 
			lpInfo->m_strPropName="�ն˻�׼�ڵ���";
			lpInfo->m_strPropHelp="�ն˻�׼�ڵ�������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.hEndNode");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//�ն�ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="End Offset";
			lpInfo->m_strPropHelp="The offset of head node along the direction of end node to start node.";
#else 
			lpInfo->m_strPropName="�ն�ƫ����";
			lpInfo->m_strPropHelp="�ն˽ڵ������ն˽ڵ�->ʼ�˽ڵ㷽���ƫ������";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.end_offset_dist");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//ƫ�Ʊ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Scale";
			lpInfo->m_strPropHelp="Calculate data from start node's offset position.";
#else 
			lpInfo->m_strPropName="ƫ�Ʊ���";
			lpInfo->m_strPropHelp="��ʼ�˽ڵ㿼��ʼ��ƫ�����Ժ��λ�ÿ�ʼ���㡣";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_scale");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Distance";
			lpInfo->m_strPropHelp="Calculated on offset ratio first and then get distance after offset.";
#else 
			lpInfo->m_strPropName="ƫ�ƾ���";
			lpInfo->m_strPropHelp="��ƫ�Ʊ��������Ժ��ٽ���ƫ�Ƶľ��롣";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID("des_para.SCALE_NODE.offset_dist");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue=sText;
			pLeafItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
		}
	}
	SetPartMutiObjsProp(pFirstPlate,"ucs.origin.x");
	SetPartMutiObjsProp(pFirstPlate,"ucs.origin.y");
	SetPartMutiObjsProp(pFirstPlate,"ucs.origin.z");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_x.x");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_x.y");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_x.z");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_y.x");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_y.y");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_y.z");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_z.x");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_z.y");
	SetPartMutiObjsProp(pFirstPlate,"ucs.axis_z.z");
	SetPartMutiObjsProp(pFirstPlate,"iProfileStyle0123");
	SetPartMutiObjsProp(pFirstPlate,"face_N");
	pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("plateProfile"),&pParentItem);
	if (pFindItem)
		Update_Plate_Face_N_TreeItem(pPropList,pFindItem);
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].norm_style");
	pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[0]"),&pParentItem);
	if (pFindItem)
		Update_Plate_Norm_CalStyle_TreeItem(pPropList,pFindItem,TRUE);
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].vector.x");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].vector.y");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].vector.z");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].hVicePart");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].norm_wing");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].direction");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[0].hCrossPart");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].norm_style");
	pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("huoqufacenorm[1]"),&pParentItem);
	if (pFindItem)
		Update_Plate_Norm_CalStyle_TreeItem(pPropList,pFindItem,FALSE);
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].vector.x");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].vector.y");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].vector.z");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].hVicePart");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].norm_wing");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].direction");
	SetPartMutiObjsProp(pFirstPlate,"huoqufacenorm[1].hCrossPart");
	SetPartMutiObjsProp(pFirstPlate,"iWeldLineStyle");
	SetPartMutiObjsProp(pFirstPlate,"m_bNeedFillet");
	SetPartMutiObjsProp(pFirstPlate,"designInfo.m_bEnableFlexibleDesign");
	SetPartMutiObjsProp(pFirstPlate,"m_bWeldPart");
	SetPartMutiObjsProp(pFirstPlate,"designInfo.weld_start_high");
	SetPartMutiObjsProp(pFirstPlate,"designInfo.weld_end_high");
	SetPartMutiObjsProp(pFirstPlate,"designInfo.weld_width");
	//��ͬ�๹�����л�ʱ��ĳЩ���Դ���->�� ��->�� ���ܶ�̬���ɾ��������
	pPropItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_bWeldPart"),&pParentItem);
	if (pPropItem)
	{
		if (pFirstPlate->m_bWeldPart)
		{
			pLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("iWeldLineStyle"),&pParentItem);
			if (pLeafItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Weld Line Type";
				lpInfo->m_strPropHelp = "Weld line size calculation type";
				lpInfo->m_cmbItems = "0.Calculate freely|1.Specified Weld Line|2.Extention Aligned";
#else 
				lpInfo->m_strPropName = "����������";
				lpInfo->m_strPropHelp = "�����߳ߴ��������";
				lpInfo->m_cmbItems = "0.���ɼ���|1.ָ������|2.�������";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("iWeldLineStyle");
				pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_OTHER);
			}
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		else
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("iWeldLineStyle"));
		if (!pFirstPlate->m_bWeldPart||pFirstPlate->designInfo.iWeldLineStyle!=1)
		{
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"));
			pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"));
		}
		else
		{	//ָ��������λ�ú�����ʼ��
			pLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"),&pParentItem);
			if (pLeafItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Weld Line Start Height";
				lpInfo->m_strPropHelp = "The start height of weld line.";
#else 
				lpInfo->m_strPropName = "������ʼ��";
				lpInfo->m_strPropHelp = "������ʼ��";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_start_high");
				pLeafItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
			}
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			//������ʼ��
			pLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_start_high"),&pParentItem);
			if (pLeafItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Weld Line Start Height";
				lpInfo->m_strPropHelp = "The start height of weld line.";
#else 
				lpInfo->m_strPropName = "������ʼ��";
				lpInfo->m_strPropHelp = "������ʼ��";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_start_high");
				pLeafItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
			}
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			//������ֹ��
			pLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_end_high"),&pParentItem);
			if (pLeafItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Weld Line End Height";
				lpInfo->m_strPropHelp = "The end height of weld line.";
#else 
				lpInfo->m_strPropName = "������ֹ��";
				lpInfo->m_strPropHelp = "������ֹ��";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_end_high");
				pLeafItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
			}
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			//�������
			pLeafItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("designInfo.weld_width"),&pParentItem);
			if (pLeafItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Weld Width";
				lpInfo->m_strPropHelp = "Weld width";
#else 
				lpInfo->m_strPropName = "�������";
				lpInfo->m_strPropHelp = "�������";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSPlate::GetPropID("designInfo.weld_width");
				pLeafItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
			}
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
	//
	//ɾ�������б�
	pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("VertexList"));
	int i=0;
	for (i=0;i<26;i++)
	{
		CXhChar16 sPropId("%c",'A'+i);
		CXhChar16 sPosX("%s.x",(char*)sPropId);
		CXhChar16 sPosY("%s.y",(char*)sPropId);
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID(sPropId));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID(sPosX));
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID(sPosY));
	}
	pFindItem = pPropList->FindItemByPropId(CLDSPlate::GetPropID("manuProcess"),&pParentItem); //�����������
	if (selectObjs.GetNodeNum()==1&&pFirstPlate->face_N==1&&g_sysPara.display.bPartSketch&&pFindItem)
	{	//��ʱ��֧����ʾ������������б� wht 10-09-02
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Vertex List";
		lpInfo->m_strPropHelp = "The vertex list of plate";
#else 
		lpInfo->m_strPropName = "������";
		lpInfo->m_strPropHelp = "�ְ��������б�";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pGroupItem->m_idProp = CLDSPlate::GetPropID("VertexList");
		if (CLDSPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=pFindItem->m_dwPropGroup;
		PROFILE_VERTEX *pVertex=NULL;
		int i=0;
		for (pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
		{
			CXhChar16 sPropId("%c",'A'+i);
			CXhChar16 sPosX("%s.x",(char*)sPropId);
			CXhChar16 sPosY("%s.y",(char*)sPropId);
			//����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("Vertex %s",(char*)sPropId);
			lpInfo->m_strPropHelp.Format("Vertex %s",(char*)sPropId);
#else 
			lpInfo->m_strPropName.Format("����%s",(char*)sPropId);
			lpInfo->m_strPropHelp.Format("����%s",(char*)sPropId);
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSPlate::GetPropID(sPropId);
			pPropItem->m_bHideChildren = FALSE;
			if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
			//װ������ϵY��X�������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("X");
			lpInfo->m_strPropHelp.Format("The X coordinate of vertex %s",(char*)sPropId);
#else 
			lpInfo->m_strPropName.Format("X����");
			lpInfo->m_strPropHelp.Format("����%s��X����ֵ",(char*)sPropId);
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID(sPosX);
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
			//װ������ϵY��Y�������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("Y");
			lpInfo->m_strPropHelp.Format("The Y coordinate of vertex %s",(char*)sPropId);
#else 
			lpInfo->m_strPropName.Format("Y����");
			lpInfo->m_strPropHelp.Format("����%s��Y����ֵ",(char*)sPropId);
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSPlate::GetPropID(sPosY);
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
			i++;
		}
	}
	PROFILE_VERTEX* pVertex=NULL;
	for (pVertex=pFirstPlate->vertex_list.GetFirst();pVertex;pVertex=pFirstPlate->vertex_list.GetNext())
	{
		if (pVertex->m_bRollEdge)
			break;
	}
	if (pVertex)
	{
		pFindItem=pPropList->FindItemByPropId(CLDSPlate::GetPropID("m_cRollProcessType"),&pParentItem);
		if (pFindItem)
			SetPartMutiObjsProp(pFirstPlate,"m_cRollProcessType");
		else
		{	//���ߴ���ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rolling Style of Plate Edge";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Mechanical Priority|1.Processing Priority|2.Default ";
#else
			lpInfo->m_strPropName = "�ְ��ߴ���ʽ";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.�������|1.���ٵ���|2.Ĭ��ֵ";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = 	CLDSPlate::GetPropID("m_cRollProcessType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
	else
		pPropList->DeleteItemByPropId(CLDSPlate::GetPropID("m_cRollProcessType"));
}

void CLDSView::DisplayParamPlateProperty(BOOL bRebuild)
{
	CLDSParamPlate *pFirstPlate = (CLDSParamPlate*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem,*pLeafItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo=NULL;
	CXhChar100 sText;
	CPropListOper<CLDSParamPlate,HASHOBJSET> oper(pPropList,pFirstPlate,&selectObjs);
	const int GROUP_GENERAL=1,GROUP_POS=2,GROUP_PROFILE=3,GROUP_MODIFY=4;
	if (bRebuild)
	{//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(4);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"Position");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"Profile");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_MODIFY-1,"Modify");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"λ��");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_MODIFY-1,"�޸�");
#endif
		//pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"����");
		pPropDlg->RefreshTabCtrl(CLDSParamPlate::m_iCurDisplayPropGroup);
		//pPropDlg->RefreshTabCtrl(0);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_PARAMPLATE;
		pPropList->SetModifyValueFunc(ModifyParamPlateProperty);
		pPropList->SetButtonClickFunc(ParamPlateButtonClick);
		pPropList->SetModifyStatusFunc(ModifyParamPlateItemStatus);
		pPropList->SetPropHelpPromptFunc(FireParamPlateHelpStr);
		pPropList->SetPopMenuClickFunc(FireParamPlatePopMenuClick);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if (!pFirstPlate->BelongModel()->IsTowerModel()||pFirstPlate->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//�ְ������Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Parametric Plate Basic Information";
		lpInfo->m_strPropHelp = "The basic information of parametric plate";
#else 
		lpInfo->m_strPropName = "�������ְ������Ϣ";
		lpInfo->m_strPropHelp = "�������ְ������Ϣ";
#endif
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pGroupItem->m_idProp = CLDSParamPlate::GetPropID("basicInfo");
		if (CLDSParamPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "Parametric plate's handle is typical unique identifier string in the  LDS system.";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�������ְ�����LDSϵͳ�жԸְ��ȫ��Ψһ��ʶ";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("handle");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//�����ڵ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Dependant node";
		lpInfo->m_strPropHelp = "Dependant node";
#else 
		lpInfo->m_strPropName = "�����ڵ���";
		lpInfo->m_strPropHelp = "��Ƹְ��������ѡ������ӽڵ�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("designInfo.hNode");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		pPropItem->SetReadOnly();
		//�����˼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Dependant rod";
		lpInfo->m_strPropHelp = "Dependant rod";
#else 
		lpInfo->m_strPropName = "�����˼����";
		lpInfo->m_strPropHelp = "��Ƹְ��������ѡ������Ӷ�λ�˼�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("designInfo.hRod");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		pPropItem->SetReadOnly();
		//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "Parametric plate in the layer";
#else 
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "�ְ�����ͼ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("layer");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
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
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("cfgword");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		//����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Material";
		lpInfo->m_strPropHelp = "The material of plate.";
#else 
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "�ְ����";
#endif
		lpInfo->m_cmbItems=MakeMaterialMarkSetString();
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("cMaterial");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		if (pFirstPlate->m_bStdPart)	//��׼����
			pPropItem->SetReadOnly();
		//����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Quality Grade";
		lpInfo->m_strPropHelp = "The quality grade of plate.";
		lpInfo->m_cmbItems="Default|A|B|C|D|";
#else 
		lpInfo->m_strPropName = "�����ȼ�";
		lpInfo->m_strPropHelp = "�ְ������ȼ�";
		lpInfo->m_cmbItems="Ĭ��|A|B|C|D|";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_cQualityLevel");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		if (pFirstPlate->m_bStdPart)	//��׼����
			pPropItem->SetReadOnly();
		//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Thickness";
		lpInfo->m_strPropHelp = "The thickness of plate(mm)��";
#else 
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�ְ�ĺ��(mm)��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("thick");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		if (pFirstPlate->m_bStdPart)	//��׼����
			pPropItem->SetReadOnly();
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
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("iSeg");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);//|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE); ����ͨ�ְ�ͳһ����<����>ҳ����ʾ wjh-2017.11.20
		//�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		if (pFirstPlate->m_bStdPart)	//��׼����
			lpInfo->m_buttonType = BDT_COMMON;
		else
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part Number";
		lpInfo->m_strPropHelp = "Part number";
#else 
		lpInfo->m_strPropName = "�������";
		lpInfo->m_strPropHelp = "�������";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("sPartNo");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(GROUP_PROFILE);
		//
		if (pFirstPlate->IsShadowObject())
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
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("shadowInfo");
			pPropItem->m_bHideChildren = FALSE;
			if (CLDSParamPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pPropItem->m_idProp);
			pPropItem->SetReadOnly(FALSE);
			//Ӱ��ĸ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Mother Part";
			lpInfo->m_strPropHelp = "The current part is project mother part.The current part is original part when the handle is Ox0.";
#else 
			lpInfo->m_strPropName = "Ӱ��ĸ����";
			lpInfo->m_strPropHelp = "��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_hMotherObj");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			//�������±��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Synchornize part label";
			lpInfo->m_strPropHelp = "Synchornize part label and segment number with mother part.";
			lpInfo->m_cmbItems = "Yes|No";
#else
			lpInfo->m_strPropName = "�������±��";
			lpInfo->m_strPropHelp = "��ǰӰ�乹������Ӱ��ĸ����ͬ�����¹�����š��κ���Ϣ��";
			lpInfo->m_cmbItems = "��|��";
#endif
			pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_bSyncShadowPartNo");
			if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstPlate->IsBlockRefShadowObject())
			{
				//������������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Vest In Block Assembly";
				lpInfo->m_strPropHelp = "The reference handle of the vest in block.";
#else 
				lpInfo->m_strPropName = "������������";
				lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_hBlockRef");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			}
		}
		else
		{
			//��������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Relative Part";
			lpInfo->m_strPropHelp = "The part associated with the current part and symmetrical type";
#else 
			lpInfo->m_strPropName = "��������";
			lpInfo->m_strPropHelp = "�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("relativeObjs");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			//��������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Dependent Block Handle";
			lpInfo->m_strPropHelp = "The handle of the dependent block.";
#else 
			lpInfo->m_strPropName = "��������";
			lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
			lpInfo->m_cmbItems=GetBlockListStr();
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_hBlock");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
		//���Ӹ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weld Parent Part";
		lpInfo->m_strPropHelp = "If the part need to weld to other part, then the property is the handle of the part's dependent welded part;If the part doesn't need to be welded,the value is 0X0.";
#else 
		lpInfo->m_strPropName = "���Ӹ�����";
		lpInfo->m_strPropHelp = "����˹�����Ҫ���ӵ�����������,��ô�����Ծ��Ǳ������������ĺ��ӹ����ľ��;����˹�������Ҫ����,��ֵΪ0X0��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_hPartWeldParent");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
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
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_uStatMatCoef");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		//��ʾ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Hole Visible";
		lpInfo->m_strPropHelp = "Hole visible";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "��ʾ��";
		lpInfo->m_strPropHelp = "�Ƿ���ʾ��";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_bDisplayHole");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		/*��ʾ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Profile Visible";
		lpInfo->m_strPropHelp = "Profile visible";
		lpInfo->m_cmbItems = "None|Actual Profile|Spread Profile";
#else
		lpInfo->m_strPropName = "��ʾ������";
		lpInfo->m_strPropHelp = "ʵ��ģʽ�¸ְ����Ƿ���������";
		//if(pFirstPlate->face_N==1)
			lpInfo->m_cmbItems = "0.����ʾ|1.������";
		//else
		//	lpInfo->m_cmbItems = "0.����ʾ|1.������|2.������(չ��)";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_cPickedDisplayMode");
		if(pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		pPropItem->SetReadOnly();
		*/
		//��ʾ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual Part";
		lpInfo->m_strPropHelp = "Virtual Part";
		lpInfo->m_cmbItems = "Yes|No";
#else 
		lpInfo->m_strPropName = "����ְ�";
		lpInfo->m_strPropHelp = "";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_bVirtualPart");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
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
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if (pFirstPlate->m_bUserSpecColor)
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
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSParamPlate::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if (pFirstPlate->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
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
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("sNotes");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		//װ�䷽λ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Assemble Position";
		lpInfo->m_strPropHelp = "The assembly location of part and assembly direction.";
#else 
		lpInfo->m_strPropName = "װ�䷽λ";
		lpInfo->m_strPropHelp = "������װ��λ�ü�װ��ʱ��װ�䷽��";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		//pGroupItem->m_bHideChildren = TRUE;
		pGroupItem->m_idProp = CLDSParamPlate::GetPropID("assemblePos");
		if (CLDSParamPlate::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//���㷨��λ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Normal Offset";
		lpInfo->m_strPropHelp = "In the normal the offset distance of plane to the datum node.";
#else 
		lpInfo->m_strPropName = "���㷨��λ��";
		lpInfo->m_strPropHelp = "װ����㷨��λ�á�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_fNormOffset");
		if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Assemble CS";
		lpInfo->m_strPropHelp = "Assemble CS.";
#else 
		lpInfo->m_strPropName = "װ������ϵ";
		lpInfo->m_strPropHelp = "װ������ϵ��";
#endif
		CPropTreeItem *pAcsItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pAcsItem->m_bHideChildren = FALSE;
		pAcsItem->m_idProp = CLDSParamPlate::GetPropID("ucs");
		if (CLDSParamPlate::GetPropStatus(pAcsItem->m_idProp)!=-1)
			pAcsItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pAcsItem->m_idProp);
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
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("ucs.origin");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSParamPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		/*		//���㷨��λ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "���㷨��λ��";
		lpInfo->m_strPropHelp = "װ����㷨��λ�á�";
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_fNormOffset");
		if(pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		*/
		//װ������ϵԭ��λ��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS.";
#else 
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "װ������ϵԭ��λ��X���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.origin.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵԭ��λ��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS.";
#else 
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Y���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.origin.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵԭ��λ��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS.";
#else 
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Z���������";
#endif
		pLeafItem= pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.origin.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
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
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_x");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSParamPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵX��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X ";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS's X axis.";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "װ������ϵX��X���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_x.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵX��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y ";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS's X axis.";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "װ������ϵX��Y���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_x.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵX��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z ";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS's X axis.";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "װ������ϵX��Z���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_x.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
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
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_y");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSParamPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵY��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X ";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS's Y axis.";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "װ������ϵY��X���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_y.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵY��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y ";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS's Y axis.";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "װ������ϵY��Y���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_y.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵY��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z ";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS's Y axis.";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "װ������ϵY��Z���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_y.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
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
		pPropItem = pPropList->InsertItem(pAcsItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_z");
		pPropItem->m_bHideChildren = TRUE;
		if (CLDSParamPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵZ��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X ";
		lpInfo->m_strPropHelp = "The x coordinate component of assembling CS's Z axis.";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "װ������ϵZ��X���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_z.x");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵZ��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y ";
		lpInfo->m_strPropHelp = "The y coordinate component of assembling CS's Z axis.";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "װ������ϵZ��Y���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_z.y");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵZ��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z ";
		lpInfo->m_strPropHelp = "The z coordinate component of assembling CS's Z axis.";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "װ������ϵZ��Z���������";
#endif
		pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pLeafItem->m_idProp = CLDSParamPlate::GetPropID("ucs.axis_z.z");
		if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//�ְ幤����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Plate Work Plane";
		lpInfo->m_strPropHelp = "The datum point of plate's work-plane  is  origin point of assembly CS,the Z axis diretion of plane is the direction of work normal.";
#else 
		lpInfo->m_strPropName = "�ְ幤����";
		lpInfo->m_strPropHelp = "�ְ幤�����׼��Ϊװ������ϵԭ�㣬�����淨��Ϊ�ְ�Z�᷽��";
#endif
		pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pPropItem->m_idProp = CLDSParamPlate::GetPropID("plate_work_plane");
		pPropItem->m_bHideChildren = TRUE;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//�ְ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Plate Type";
		lpInfo->m_strPropHelp = "Plate type";
		lpInfo->m_cmbItems = "0.Flange Plate|1.U Plugboard|2.Trough Board|3.Elbow Plate|4.Vertical Stiffener Plate|5.Head Plate|6.Circular Elbow Plate|7.Circular Sealing Plate|8.Tube Elbow Plate|9.Angle Elbow Plate|10.Cross Board|11.Flat Weld Flange|12.Weld Neck Flange|13.Water Plate";
#else 
		lpInfo->m_strPropName = "�ְ�����";
		lpInfo->m_strPropHelp = "�ְ�����";
		lpInfo->m_cmbItems = "0.������|1.U�Ͳ��|2.���Ͳ��|3.���|4.����Ӿ���|5.��ͷ��|6.�����߰�|7.������|8.�ֹ��߰�|9.�Ǹ��߰�|10.ʮ�ֲ��|11.ƽ������|12.�Ժ�����|13.��ˮ��|14.�߰�";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
		pGroupItem->m_idProp = CLDSParamPlate::GetPropID("m_iParamType");
		if (pFirstPlate->GetPropValueStr(pGroupItem->m_idProp,sText)>0)
			pGroupItem->m_lpNodeInfo->m_strPropValue = sText;
		pGroupItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pGroupItem->m_idProp);
		pGroupItem->SetReadOnly(pGroupItem->m_lpNodeInfo->m_bMutiObjsProp);	//��ͬ���Ͳ������岻����һ���޸Ĳ���
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		//
		RebuildOrUpdateParamsPropertyList(pPropList,pGroupItem);
		if (pFirstPlate->m_iParamType==TYPE_ROLLEND)
		{  //���Ͳ���ߴ���ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rolling Style of Plate Edge";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Mechanical Priority|1.Processing Priority 1|2.Processing Priority 2|3.Default ";
#else
			lpInfo->m_strPropName = "���Ͳ���ߴ���ʽ";
			lpInfo->m_strPropHelp = "�������:չ���󵹽Ǵ�Ϊ����ֱ��, ���ٵ���1:��֤������߳���ΪL-N,���ٵ���2:��֤����ڲ�߳���ΪL-N��";
			lpInfo->m_cmbItems = "0.�������|1.���ٵ���1|2.���ٵ���2|3.Ĭ��ֵ";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = 	CLDSParamPlate::GetPropID("m_cRollProcessType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//�ְ����߼��㷽ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "RollLine Cal Type";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Outside|1.Inside";
#else
			lpInfo->m_strPropName = "���ƫ�Ʒ�ʽ";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.���|1.�ڲ�|3.Ĭ��ֵ";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_cRollOffsetType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		}
		if (pFirstPlate->GetFaceN()>1)
		{
			//�ְ�������δ���ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Deform Type";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.Assembly Plane|1.Middle Plane|2.Raised Plane|3.Sunk Plane";
#else
			lpInfo->m_strPropName = "�������δ���ʽ";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems = "0.װ����|1.���Բ�|2.͹��|3.����|4.Ĭ��ֵ";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_cDeformType");
			if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			if (sText[0]-'0'==1)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Deform Coef";
				lpInfo->m_strPropHelp = "Deform Coef";
#else
				lpInfo->m_strPropName = "���Բ�ϵ��";
				lpInfo->m_strPropHelp = "���л������ε����Բ�ϵ��";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_fDeformCoef");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
			}
		}
		//�ְ���Ʋ����б�
		if (pFirstPlate->m_iParamType==TYPE_FL||pFirstPlate->m_iParamType==TYPE_FLG||
			pFirstPlate->m_iParamType==TYPE_FLR)
		{
			//�ְ�������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
			if (pFirstPlate->m_bStdPart)	//��׼����
				lpInfo->m_cmbItems="View";
			else
				lpInfo->m_cmbItems="View|Import|Create";
			lpInfo->m_strPropName = "Relative Elbow Plate ";
			lpInfo->m_strPropHelp = "Relative elbow plate";
#else 
			if (pFirstPlate->m_bStdPart)	//��׼����
				lpInfo->m_cmbItems="�鿴";
			else
				lpInfo->m_cmbItems="�鿴|����|�������";
			lpInfo->m_strPropName = "�������";
			lpInfo->m_strPropHelp = "�������";
#endif
			pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
			pPropItem->m_idProp = CLDSParamPlate::GetPropID("plateAttachElbowPlate");
		}
		//�ְ�༭����
		pGroupItem=oper.InsertPropItem(pRoot,"PlateEdit");
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_MODIFY);
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"WorkUcs");
		pPropItem->m_bHideChildren=FALSE;
		if (CLDSPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSPlate::GetPropStatus(pPropItem->m_idProp);
		//��������ϵԭ��
		pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"wcs.origin");
		pLeafItem->SetReadOnly();
		oper.InsertEditPropItem(pLeafItem,"wcs.origin.x");
		oper.InsertEditPropItem(pLeafItem,"wcs.origin.y");
		//��������ϵX��
		pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"wcs.axis_x");
		pLeafItem->SetReadOnly();
		oper.InsertEditPropItem(pLeafItem,"wcs.axis_x.a");
		//��������ϵX��
		pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"wcs.axis_y");
		pLeafItem->SetReadOnly();
		//�༭��(���붥�㡢������˨)
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"InsertProfileVertex");	//
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"AddNewHole");	//
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"DeleteFeature");
		pPropItem->SetReadOnly();
		pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"FilletVertex");
		pPropItem->SetReadOnly();
	}
	else
	{
		SetPartMutiObjsProp(pFirstPlate,"handle");
		SetPartMutiObjsProp(pFirstPlate,"designInfo.hNode");
		SetPartMutiObjsProp(pFirstPlate,"designInfo.hRod");
		SetPartMutiObjsProp(pFirstPlate,"layer");
		SetPartMutiObjsProp(pFirstPlate,"cMaterial");
		SetPartMutiObjsProp(pFirstPlate,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstPlate,"iSeg");
		SetPartMutiObjsProp(pFirstPlate,"thick");
		SetPartMutiObjsProp(pFirstPlate,"sPartNo");
		SetPartMutiObjsProp(pFirstPlate,"cfgword");
		SetPartMutiObjsProp(pFirstPlate,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstPlate,"m_hPartWeldParent");
		SetPartMutiObjsProp(pFirstPlate,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstPlate,"m_cPickedDisplayMode");
		SetPartMutiObjsProp(pFirstPlate,"m_bVirtualPart");
		SetPartMutiObjsProp(pFirstPlate,"m_bUserSpecColor");

		SetPartMutiObjsProp(pFirstPlate,"wcs.origin");
		SetPartMutiObjsProp(pFirstPlate,"wcs.origin.x");
		SetPartMutiObjsProp(pFirstPlate,"wcs.origin.y");
		SetPartMutiObjsProp(pFirstPlate,"wcs.axis_x");
		SetPartMutiObjsProp(pFirstPlate,"wcs.axis_x.a");
		SetPartMutiObjsProp(pFirstPlate,"wcs.axis_y");

		CPropTreeItem *pFindItem,*pParentItem;
		pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_hBlock"),&pParentItem);
		if (pFindItem)
		{
			if (pFirstPlate->IsShadowObject())
			{	//����ͨ�����л���Ӱ�乹��
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_hBlock"));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("relativeObjs"));
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
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,12,true);
				pPropItem->m_idProp = CLDSParamPlate::GetPropID("shadowInfo");
				if (CLDSParamPlate::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSParamPlate::GetPropStatus(pPropItem->m_idProp);
				pPropItem->SetReadOnly(FALSE);
				//Ӱ��ĸ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Project Mother Part";
				lpInfo->m_strPropHelp = "The current part is project mother part.The current part is original part when the handle is Ox0.";
#else 
				lpInfo->m_strPropName = "Ӱ��ĸ����";
				lpInfo->m_strPropHelp = "��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_hMotherObj");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
				//�������±��
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Synchornize part label";
				lpInfo->m_strPropHelp = "Synchornize part label and segment number with mother part.";
				lpInfo->m_cmbItems = "Yes|No";
#else
				lpInfo->m_strPropName = "�������±��";
				lpInfo->m_strPropHelp = "��ǰӰ�乹������Ӱ��ĸ����ͬ�����¹�����š��κ���Ϣ��";
				lpInfo->m_cmbItems = "��|��";
#endif
				pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_bSyncShadowPartNo");
				if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
					pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				if (pFirstPlate->IsBlockRefShadowObject())
				{
					//������������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Assembly";
					lpInfo->m_strPropHelp = "The reference handle of the vest in block.";
#else 
					lpInfo->m_strPropName = "������������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
					pLeafItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
					pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_hBlockRef");
					if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
					pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
					pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstPlate,"m_hBlock");
				SetPartMutiObjsProp(pFirstPlate,"relativeObjs");
			}
		}
		pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("shadowInfo"),&pParentItem);
		if (pFindItem)
		{
			if (!pFirstPlate->IsShadowObject())
			{	//��Ӱ�乹���л�����ͨ����
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("shadowInfo"));
				pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_hMotherObj"));
				pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_hBlockRef"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_hBlockRef"));
				pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_hBlock"),NULL);
				if (pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstPlate,"m_hBlock");
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
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
					pPropItem->m_idProp = CLDSParamPlate::GetPropID("m_hBlock");
					if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
					pPropItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
				pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("relativeObjs"),NULL);
				if (pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstPlate,"relativeObjs");
				else
				{
					if (pFindItem)
						pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_hBlock"));
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Relative Part";
					lpInfo->m_strPropHelp = "The part associated with the current part and symmetrical type";
#else 
					lpInfo->m_strPropName = "��������";
					lpInfo->m_strPropHelp = "�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,12,true);
					pPropItem->m_idProp = CLDSParamPlate::GetPropID("relativeObjs");
					if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstPlate,"m_hMotherObj");
				pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_hBlockRef"),&pParentItem);
				if (pFindItem&&pFirstPlate->IsBlockRefShadowObject())
				{
					SetPartMutiObjsProp(pFirstPlate,"m_hBlockRef");
					SetPartMutiObjsProp(pFirstPlate,"m_hBlock");
				}
				else if (pFindItem&&!pFirstPlate->IsBlockRefShadowObject())
				{
					pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_hBlockRef"));
					pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_hBlock"));
				}
				else if (pFindItem==NULL&&pFirstPlate->IsBlockRefShadowObject())
				{
					pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_hMotherObj"),&pParentItem);
					//������������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Assembly";
					lpInfo->m_strPropHelp = "The reference handle of the vest in block.";
#else 
					lpInfo->m_strPropName = "������������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
					pLeafItem = pPropList->InsertItem(pParentItem,lpInfo,-1);
					pLeafItem->m_idProp = CLDSParamPlate::GetPropID("m_hBlockRef");
					if (pFirstPlate->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
					pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pLeafItem->m_idProp);
					pLeafItem->SetReadOnly(pFirstPlate->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
				}
			}
		}
		if (pFirstPlate->m_bUserSpecColor)
		{
			if (!SetPartMutiObjsProp(pFirstPlate,"crMaterial"))
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_bUserSpecColor"),NULL);
				if (pFindItem)
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
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo,-1);
					pPropItem->m_idProp = CLDSParamPlate::GetPropID("crMaterial");
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSParamPlate::IsPropEqual(selectObjs,pPropItem->m_idProp);
					if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				}
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstPlate,"sNotes");
		SetPartMutiObjsProp(pFirstPlate,"m_fNormOffset");
		SetPartMutiObjsProp(pFirstPlate,"ucs.origin.x");
		SetPartMutiObjsProp(pFirstPlate,"ucs.origin.y");
		SetPartMutiObjsProp(pFirstPlate,"ucs.origin.z");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_x.x");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_x.y");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_x.z");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_y.x");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_y.y");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_y.z");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_z.x");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_z.y");
		SetPartMutiObjsProp(pFirstPlate,"ucs.axis_z.z");
		SetPartMutiObjsProp(pFirstPlate,"m_iParamType");
		pFindItem = pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_iParamType"),NULL);
		if (pFindItem)
		{
			pFindItem->SetReadOnly(pFindItem->m_lpNodeInfo->m_bMutiObjsProp);
			RebuildOrUpdateParamsPropertyList(pPropList,pFindItem);
			pFindItem=pPropList->FindItemByPropId(CLDSParamPlate::GetPropID("m_cRollProcessType"),&pParentItem);
			if (pFindItem)
			{
				if (pFirstPlate->m_iParamType!=TYPE_ROLLEND)
					pPropList->DeleteItemByPropId(CLDSParamPlate::GetPropID("m_cRollProcessType"));
				else
					SetPartMutiObjsProp(pFirstPlate,"m_cRollProcessType");
			}
			else if (pFindItem==NULL&&pFirstPlate->m_iParamType==TYPE_ROLLEND)
			{   //���Ͳ���ߴ���ʽ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_strPropName = "���Ͳ���ߴ���ʽ";
				lpInfo->m_strPropHelp = "�������:չ���󵹽Ǵ�Ϊ����ֱ��, ���ٵ���1:��֤������߳���ΪL-N,���ٵ���2:��֤����ڲ�߳���ΪL-N��";
				lpInfo->m_cmbItems = "0.�������|1.���ٵ���1|2.���ٵ���2|3.Ĭ��ֵ";
				pPropItem = pPropList->InsertItem(pRoot,lpInfo,-1);
				pPropItem->m_idProp = 	CLDSParamPlate::GetPropID("m_cRollProcessType");
				if (pFirstPlate->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
	}
	pPropList->Redraw();
}
#endif
//
void CLDSView::OnCreateBoltInPart()
{
	/*
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(console.m_pWorkPart&&m_curAppType==TA_ASSEMBLE_ENV
		&&g_sysPara.display.bPartSketch	//ѡ����ʾ������ͼ wht 11-05-16
		&&(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE
		||console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE
		||console.m_pWorkPart->IsLinePart()||console.m_pWorkPart->IsArcPart()))
	{
		Ta.BeginUndoListen();
		static CBoltPropDlg new_lsdlg;
		UCS_STRU object_ucs,wcs;
		wcs=console.m_curWCS;
		object_ucs.origin.Set();
		object_ucs.axis_x.Set(1,0,0);
		object_ucs.axis_y.Set(0,0,1);
		object_ucs.axis_z.Set(0,-1,0);
		coord_trans(wcs.origin,object_ucs,FALSE);
		vector_trans(wcs.axis_x,object_ucs,FALSE);
		vector_trans(wcs.axis_y,object_ucs,FALSE);
		vector_trans(wcs.axis_z,object_ucs,FALSE);
		new_lsdlg.m_pWorkPart=console.m_pWorkPart;
		new_lsdlg.work_ucs=wcs;
		CLDSBolt ls;
		if(new_lsdlg.DoModal()==IDOK)
		{
			if(theApp.m_bCooperativeWork)
			{	//Эͬ����ʱ�ж��Ƿ�����ӦȨ��
				BOOL bPermit=TRUE;
				if(console.m_pWorkPart->IsLinePart())
				{
					CLDSLinePart *pLinePart=(CLDSLinePart*)console.m_pWorkPart;
					bPermit=((theApp.GetPermDword()&(pLinePart->dwPermission|pLinePart->dwStartPermission|pLinePart->dwEndPermission))!=0);
				}
				else
					bPermit=((theApp.GetPermDword()&console.m_pWorkPart->dwPermission)!=0);
				if(!bPermit)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("No corresponding permissions��");
#else
					AfxMessageBox("û����Ӧ��Ȩ�ޣ�");
#endif
					return;
				}
			}
			Ta.BeginUndoListen();
			CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pLs->iSeg=console.m_pWorkPart->iSeg;
			pLs->cfgword=console.m_pWorkPart->cfgword;	//������˨��ĺ����׼������ĺ�һ��
			pLs->m_cFuncType=new_lsdlg.m_iHoleFuncType;
			pLs->m_bVirtualPart=(pLs->m_cFuncType>=2);
			if(!restore_Ls_guige(new_lsdlg.m_sLsGuiGe,ls))
			{
				console.DeletePart(pLs->handle);
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Input specification is illegal,Creat bolt failure!");
#else
				AfxMessageBox("������˨���Ƿ�,��˨����ʧ��!");
#endif
				return ;
			}
			pLs->set_d(ls.get_d());
			pLs->set_L(ls.get_L());
			pLs->hole_d_increment = new_lsdlg.m_fHoleD-ls.get_d();
			pLs->SetGrade(ls.Grade());
			//
			if(new_lsdlg.m_nWaistLen>0)
			{	//������Բ�׷���
				vector_trans(new_lsdlg.waist_vec,wcs,TRUE);
				vector_trans(new_lsdlg.waist_vec,console.m_pWorkPart->ucs,TRUE);
				pLs->ucs.axis_x=new_lsdlg.waist_vec;
			}
			//��˨λ��
			f3dPoint ls_pos;
			ls_pos.Set(new_lsdlg.m_fPosX,new_lsdlg.m_fPosY,new_lsdlg.m_fPosZ);
			coord_trans(ls_pos,wcs,TRUE);
			pLs->ucs.origin=ls_pos;
			pLs->m_cFuncType=new_lsdlg.m_iHoleFuncType;
			//�����˨����
			CLsRef LsRef=pLs->GetLsRef();
			LsRef.waistLen=new_lsdlg.m_nWaistLen;
			if(console.m_pWorkPart->IsLinePart())
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)console.m_pWorkPart;
				if(theApp.m_bCooperativeWork)
				{
					if(pLinePart->dwPermission&theApp.GetPermDword())
						pLinePart->AppendMidLsRef(LsRef);
					else if(pLinePart->dwStartPermission&theApp.GetPermDword())
						pLinePart->AppendStartLsRef(LsRef);
					else if(pLinePart->dwEndPermission&theApp.GetPermDword())
						pLinePart->AppendEndLsRef(LsRef);
					else
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("No corresponding permissions��");
#else
						AfxMessageBox("û����Ӧ��Ȩ�ޣ�");
#endif
				}
				else
					pLinePart->AppendMidLsRef(LsRef);
			}
			else
			{
				if(console.m_pWorkPart->IsArcPart())
				{
					CLDSArcPart *pArcPart=(CLDSArcPart*)console.m_pWorkPart;
					new_lsdlg.bak_des_ls_pos.hPart=pArcPart->handle;
					new_lsdlg.bak_des_ls_pos.R=pArcPart->baseline.Radius();
					new_lsdlg.bak_des_ls_pos.offset_angle=new_lsdlg.m_fPosX/pArcPart->baseline.Radius();
					new_lsdlg.bak_des_ls_pos.wing_offset_dist=new_lsdlg.m_fPosY;
					new_lsdlg.bak_des_ls_pos.direction=0;
					pArcPart->AppendLsRef(LsRef);
				}
				else if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
				{
					CLDSPlate *pPlate=(CLDSPlate*)console.m_pWorkPart;
					//�۵���������˨ʱ������������˨,FindLsByPos()���ܵ���ĳЩ��˨�޷����� wht 11-07-13
					if(pPlate->IsFoldPlate())
						pPlate->AppendLsRef(LsRef,FALSE);
					else
						pPlate->AppendLsRef(LsRef);
				}
				else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
					((CLDSParamPlate*)console.m_pWorkPart)->AppendLsRef(LsRef);
			}
			pLs->AddL0Thick(console.m_pWorkPart->handle,TRUE,TRUE);
			//��˨����
			pLs->ucs.axis_x.Set();
			pLs->ucs.axis_y.Set();
			pLs->des_work_norm=new_lsdlg.bak_des_ls_norm;
			f3dPoint ls_norm(new_lsdlg.m_fNormX,new_lsdlg.m_fNormY,new_lsdlg.m_fNormZ);
			normalize(ls_norm);
			vector_trans(ls_norm,wcs,TRUE);
			pLs->set_norm(ls_norm);
			pLs->des_work_norm.vector=pLs->ucs.axis_z;
			if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
			{	//���ýǸ���˨���߲���
				CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.m_pWorkPart;
				if(fabs(pLineAngle->get_norm_x_wing()*ls_norm)>fabs(pLineAngle->get_norm_y_wing()*ls_norm))
					pLs->des_work_norm.norm_wing=0;
				else
					pLs->des_work_norm.norm_wing=1;
			}
			pLs->des_base_pos=new_lsdlg.bak_des_ls_pos;
			pLs->des_base_pos.datumPoint.datum_pos_style=0;
			pLs->des_base_pos.datumPoint.SetPosition(pLs->ucs.origin);
			pLs->des_base_pos.hPart=console.m_pWorkPart->handle;
			pLs->correct_worknorm();
			pLs->correct_pos();
			pLs->CalGuigeAuto();
			g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
			Ta.EndUndoListen();
		}
	}
	else
	{
		long *id_arr=NULL;
		if(g_pSolidSnap->GetLastSelectEnts(id_arr)!=1)
			return;
		CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
		if(!pPart)
			return;
		if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			Ta.BeginUndoListen();
			static CAddNewBoltDlg boltdlg;
			boltdlg.m_pWorkPart = pPart;
			boltdlg.DoModal();
			m_pDoc->SetModifiedFlag();
			Ta.EndUndoListen();
		}
	}
#endif
	*/
}

void CLDSView::OnImportBoltRefToPart()
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	long *id_arr=NULL;
	if (g_pSolidSnap->GetLastSelectEnts(id_arr)!=1)
		return;
	CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
	if (pPart)
		ImportBoltRefToPart(pPart);
#endif
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
//�ж���˨�Ƿ�Ϊʮ�ֲ��������˨ ���򷵻���˨��λ���� �񷵻�NULL
static CLDSPlate* IsVerticalPlateBolt(CLDSBolt *pBolt)
{
	if (pBolt)
	{
		//1.��˨��λ����Ϊ�ְ�
		CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pBolt->des_base_pos.hPart,CLS_PLATE);
		if (pPlate==NULL)
			return NULL;
		//2.��˨��λ�ְ�ĺ��Ӹ�����Ϊʮ�ֲ��
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pPlate->m_hPartWeldParent,CLS_PARAMPLATE);
		if (pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
			return pPlate;
	}
	return NULL;
}
//<DEVELOP_PROCESS_MARK nameId="CLDSView.ImportBoltRefToPart">
bool CLDSView::AppendBoltRefToPart(CLDSPart *pBasePart,CLDSBolt* pBolt,int style/* =0 */)
{
	if (pBasePart==NULL || pBolt==NULL)
		return false;
	CSuperSmartPtr<CLDSPart> pDatumPart = pBasePart;
	if (pDatumPart.IsNULL())
		return false;
	CLsRef *pLsRef=NULL;
	if (pDatumPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate=pDatumPart.PlatePointer();
		//�۵���������˨ʱ������������˨,FindLsByPos()���ܵ���ĳЩ��˨�޷����� wht 11-07-13
		if (pPlate->IsFoldPlate())
			pLsRef=pDatumPart.PlatePointer()->AppendLsRef(pBolt->GetLsRef(),FALSE);
		else
			pLsRef=pDatumPart.PlatePointer()->AppendLsRef(pBolt->GetLsRef());
		pDatumPart.PlatePointer()->GetLsRefList()->ResetBoltRefRayNo(pDatumPart);
	}
	else if (pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		pLsRef=pDatumPart.ParamPlatePointer()->AppendLsRef(pBolt->GetLsRef());
		pDatumPart.ParamPlatePointer()->GetLsRefList()->ResetBoltRefRayNo(pDatumPart);
	}
	else if (pDatumPart->IsLinePart())
	{	//����˨���뵽ָ���˼��� wht 12-03-19
		if (style==1)
			pLsRef=pDatumPart.LinePartPointer()->AppendStartLsRef(pBolt->GetLsRef());
		else if (style==2)
			pLsRef=pDatumPart.LinePartPointer()->AppendEndLsRef(pBolt->GetLsRef());
		else //if(style==0)
			pLsRef=pDatumPart.LinePartPointer()->AppendMidLsRef(pBolt->GetLsRef());
	}
	else if (pDatumPart->IsArcPart())
		pDatumPart.ArcPartPointer()->AppendLsRef(pBolt->GetLsRef());
	else
	{
		char class_name[51];
		pDatumPart->GetClassType(class_name);
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The part type 0X%X can't support {%s},perforate failed!",pDatumPart->handle,class_name);
#else 
		logerr.Log("0X%X��������{%s}��֧��,��˨���׹���ʧ��!",pDatumPart->handle,class_name);
#endif
		return false;
	}
	pDatumPart->SetModified();
	pBolt->AddL0Thick(pDatumPart->handle,TRUE,TRUE);
	pBolt->CalGuigeAuto();
	pBolt->SetModified();
	if (!pBolt->m_bVirtualPart)
	{
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	else
	{	//��ӹ��߿ף�����ʾ��˨ʵ�壬��ʾ�ױ�  wxc-2019.7.24
		HOLE_WALL* pHoleWall = console.MakeHoleWall(pBolt,pDatumPart);
		if (pHoleWall==NULL)
		{
			pHoleWall = console.hashHoles.Add(DUALKEY(pBolt->handle,pDatumPart->handle));
			pHoleWall->pBolt = pBolt;
			for (int j = 0; j < 4; j++)
			{
				if (pHoleWall->items[j].hRelaPart == pDatumPart->handle)
					break;
				else if (pHoleWall->items[j].hRelaPart == 0)
				{
					pHoleWall->items[j].hRelaPart = pDatumPart->handle;
					break;
				}
			}
			pHoleWall->is_visible = TRUE;
			pHoleWall->Create3dSolidModel(g_pSolidOper->GetScaleUserToScreen(),g_pSolidOper->GetScaleUserToScreen(),g_sysPara.display.nSmoothness);
		}
		pBolt->is_visible = FALSE;
		g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
		pDatumPart->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pDatumPart->GetSolidPartObject());
	}
	return true;
	//����ʮ�ֲ�����巨�߷��� 
	/*if(bFirstCalNorm&&(pDatumPart->GetClassTypeId()==CLS_PLATE
	||pDatumPart->GetClassTypeId()==CLS_PARAMPLATE))
	{
	CLDSPlate *pPlate=IsVerticalPlateBolt(pBolt);
	if(pPlate)
	bFirstCalNorm=!CalVerticalPlateNorm(pPlate,pDatumPart);
	}*/
}
void CLDSView::ImportBoltRefToPart(CLDSPart *pBasePart,int style/*=0*/)
{
	if (pBasePart==NULL)
		return;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	long *id_arr=NULL;
	CString cmdStr;
	CLDSPart *pPart=NULL;
	if (pCmdLine->m_bGettingStrFromCmdLine)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The command running, please follow new command after the termination of the current command!");
#else 
		AfxMessageBox("������������,����ֹ��ǰ�������ִ��������!");
#endif
		return;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Select the importing bolts,or directly input handle(Seperate: ','), press enter key to confirm:","");
#else 
	pCmdLine->FillCmdLine("ѡ��Ҫ�������˨,��ֱ��������(����','�ָ�),���س���ȷ��:","");
#endif
	g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
	OpenWndSel();	//����ѡ����˨
	//����HashTable
	CHashTable<CLDSPart*>partsetTable;
	partsetTable.Empty();
	partsetTable.CreateHashTable(100);
	if (pCmdLine->GetStrFromCmdLine(cmdStr))
	{
		Ta.BeginUndoListen();
		try {
			//���ݾ���ַ������빹��
			CArray<long,long> id_arr1;
			CXhChar100 sText;
			if (cmdStr.GetLength()>0)
			{
				sText.Printf("%s",cmdStr);
				for (char* key=strtok(sText,",");key;key=strtok(NULL,","))
				{
					long h=HexStrToLong(key);
					pPart=console.FromPartHandle(h);
					id_arr1.Add(h);
				}
			}
			//BOOL bFirstCalNorm=TRUE;	//��֤������һ��ʮ�ֲ�����巨��
			int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
			for (int i=0;i<id_arr1.GetSize()+n;i++)
			{
				if (i<id_arr1.GetSize())
					pPart=console.FromPartHandle(id_arr1[i]);
				else
					pPart=console.FromPartHandle(id_arr[i-id_arr1.GetSize()]);
				if (!pPart || pPart->GetClassTypeId()!=CLS_BOLT)
					continue;
				if (partsetTable.GetValueAt(pPart->handle,pPart))
					continue;
				partsetTable.SetValueAt(pPart->handle,pPart);
				CLDSBolt* pBolt=(CLDSBolt*)pPart;
				AppendBoltRefToPart(pBasePart,pBolt,style);
			}
		}
		catch (char* sError)
		{
			AfxMessageBox(sError);
		}
		g_pSolidDraw->ReleaseSnapStatus();
		m_pDoc->SetModifiedFlag();
		Ta.EndUndoListen();
	}
	else
	{
		pCmdLine->CancelCmdLine();
		return;
	}
	g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	OnOperOther();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("����:","");
#endif
}
#endif
//</DEVELOP_PROCESS_MARK>
/*
void CLDSView::OnLookBoltInPart()
{
#ifndef __TSA_
	long *id_arr=NULL;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)!=1)
		return;
	CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
	if(!pPart)
		return;
	if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE||
		pPart->IsLinePart()||pPart->IsArcPart())
	{
		int start1_end2_mid0=0;
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			start1_end2_mid0=3;//�Ǹ��ϵ�������˨
		g_pLsRefDlg->InitLsRefDlg(pPart,start1_end2_mid0);
	}
#endif
}
*/
void CLDSView::OnLookLinePartInPlate()
{
	/*
#ifndef __TSA_
	long *id_arr=NULL;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)!=1)
		return;
	CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
	if(pPart==NULL || pPart->GetClassTypeId()!=CLS_PLATE)
		return;
	CLDSPlate* pPlate=(CLDSPlate*)pPart;
	CLinkLinePartDlg linkdlg;
	linkdlg.m_pLjPara = &pPlate->designInfo;
	linkdlg.DoModal();
#endif
	*/
}
void CLDSView::OnLookDesignParam()
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	long *id_arr=NULL,nCount=0;
	nCount=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if (nCount<1)
		return;
	CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.FromPartHandle(id_arr[0],CLS_PARAMPLATE);
	if (pParamPlate==NULL)
		return;
	for (int i=1;i<nCount;i++)
	{
		CLDSParamPlate *pPart = (CLDSParamPlate*)console.FromPartHandle(id_arr[i],CLS_PARAMPLATE);
		if (pPart==NULL)
			return;
		if (pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			return;
		if (pPart->m_iParamType!=pParamPlate->m_iParamType)
			return;
	}
	//�鿴�����������Ʋ��� wht 11-05-18
	if (pParamPlate->m_iParamType==TYPE_RIBPLATE)
	{
		CDesignRibbedPlateDlg ribPlateDlg;
		ribPlateDlg.SetRibPlate(pParamPlate);
		ribPlateDlg.DoModal();
	}
#ifdef __COMPLEX_PART_INC_
	else if (pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE
		||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE
		||pParamPlate->m_iParamType==TYPE_CIRRIBPLATE
		||pParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
	{
		CDesignCircularPlateDlg cirPlateDlg;
		cirPlateDlg.m_bNewCirPlate=FALSE;
		cirPlateDlg.m_pCirPlate=pParamPlate;
		cirPlateDlg.DoModal();
	}
#endif
	else
	{
		CParamPlateParamListDlg paramDlg;
		paramDlg.m_pParamPlate=pParamPlate;
		paramDlg.DoModal();
	}
#endif
}

void CLDSView::OnImportLinePartToPlate()
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	long *id_arr=NULL;
	if (g_pSolidSnap->GetLastSelectEnts(id_arr)!=1)
		return;
	CLDSPart *pPart = console.FromPartHandle(id_arr[0]);
	if (pPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate* pPlate=(CLDSPlate*)pPart;
		ImportLinePartToPlate(pPlate);
	}
#endif
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
void CLDSView::ImportLinePartToPlate(CLDSPlate *pPlate)
{
	if (pPlate==NULL)
		return;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr;
	CLDSPart *pPart=NULL;
	CXhChar100 sText;
	if (pCmdLine->m_bGettingStrFromCmdLine)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The command running, please follow new command after the termination of the current command!");
#else 
		AfxMessageBox("������������,����ֹ��ǰ�������ִ��������!");
#endif
		return;
	}
	g_pSolidSnap->SetSelectPartsType(SELECT_LINEPART);
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Select the importing rod,or directly input handle(Seperate: ','), press enter key to confirm:","");
#else 
	pCmdLine->FillCmdLine("ѡ��Ҫ��������Ӹ˼�,��ֱ��������(����','�ָ�),���س���ȷ��:","");
#endif
	CUndoOperObject undo(&Ta,true);
	if (!pCmdLine->GetStrFromCmdLine(cmdStr))
	{
		pCmdLine->CancelCmdLine();
		return;
	}
	//���ݾ���ַ������빹��
	CArray<long,long>id_arr1;
	if (cmdStr.GetLength()>0)
	{
		sText.Printf("%s",cmdStr);
		for (char* key=strtok(sText,",");key;key=strtok(NULL,","))
		{
			long h=HexStrToLong(key);
			pPart=console.FromPartHandle(h);
			id_arr1.Add(h);
		}
	}
	long *id_arr=NULL;
	int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	DYN_ARRAY<long> idarr(n);	//Ϊ��ֹ����ʹ��id_arr֮ǰ�û������Ļ����ѡ��״̬�������ڴ���ǰ�ͷţ���������ʱ���� wjh-2015.5.17
	memcpy(idarr,id_arr,sizeof(long)*n);
	char cDefaultPlane='1';
	if (pPlate->face_N > 1)
	{	//�����ʱ��ѡ�ڸ˼����ڹ�����
		if (pPlate->face_N == 3)
			cmdStr = "��ǰ�������Ӹ˼�λ�ڻ�׼������(1)/��һ������(2)/�ڶ�������(3)<1>:";
		else if (pPlate->face_N == 2)
			cmdStr = "��ǰ�������Ӹ˼�λ�ڻ�׼������(1)/��һ������(2)<1>:";
		while (1)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if (pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"1|2|3"))
			{
				char cPlane = '1';
				if (cmdStr.GetLength() > 0)
					cPlane = cmdStr[0];
				if (cPlane<'1' || cPlane>pPlate->face_N + '1' || cPlane > '3')
				{
					pCmdLine->FillCmdLine("����������Ŵ���","");
					pCmdLine->FinishCmdLine();
					continue;
				}
				cDefaultPlane = cPlane;
			}
			break;
		}
	}
	LINEPARTSET newrods;
	for (int i=0;i<id_arr1.GetSize()+n;i++)
	{
		if (i<id_arr1.GetSize())
			pPart=console.FromPartHandle(id_arr1[i]);
		else
			pPart=console.FromPartHandle(idarr[i-id_arr1.GetSize()]);
		if (pPart==NULL||!pPart->IsLinePart())
			continue;
		if (pPlate->designInfo.partList.GetValue(pPart->handle) != NULL)
			continue;	//�ְ����Ӹ˼����Ѵ��ڴ˸˼�
		CLDSLinePart* pRod=(CLDSLinePart*)pPart;
		//if (pRod->pStart!=NULL&&pRod->pEnd!=NULL)
		{//���Ӹ˼���֧�ּ���̽Ǹ� wxc-2019.11.5
			CLDSNode*  pBaseNode = console.FromNodeHandle(pPlate->designInfo.m_hBaseNode);
			CLDSLinePart* pBaseRod = (CLDSLinePart*)console.FromPartHandle(pPlate->designInfo.m_hBasePart);
			CDesignLjPartPara desLjPartPara;
			desLjPartPara.hPart = pPart->handle;
			desLjPartPara.iFaceNo = cDefaultPlane - '0';
			desLjPartPara.m_nClassTypeId = pPart->GetClassTypeId();
			desLjPartPara.start0_end1 = CDesignJdb::LjPosInPart(pBaseRod, pBaseNode, pRod);
			if (pPart->GetClassTypeId() == CLS_LINEANGLE || pPart->GetClassTypeId() == CLS_GROUPLINEANGLE)
			{
				CLDSLineAngle* pLineAngle = (CLDSLineAngle*)pPart;
				double justify_x = pLineAngle->get_norm_x_wing()*pPlate->ucs.axis_z;
				double justify_y = pLineAngle->get_norm_y_wing()*pPlate->ucs.axis_z;
				if (fabs(justify_x) > fabs(justify_y))//&&justify_x>EPS_COS2)
					desLjPartPara.angle.cur_wing_x0_y1 = 0;//����Ӧ��������˼��������������wjh-2015.1.05
				else //if(fabs(justify_x)<fabs(justify_y))//&&justify_y>EPS_COS2)
					desLjPartPara.angle.cur_wing_x0_y1 = 1;
				double max_d = 0;
				for (CLsRef* pLsRef = pPlate->GetFirstLsRef(); pLsRef; pLsRef = pPlate->GetNextLsRef())
				{
					CLDSBolt* pBolt = pLsRef->GetLsPtr();
					if (pLineAngle->FindLsByHandle(pBolt->handle) && pBolt->get_d() > max_d)
						max_d = pBolt->get_d();
				}
				LSSPACE_STRU LsSpace;
				if (!GetLsSpace(LsSpace, ftoi(max_d)))
				{	//��ֹδ������˨�ȼ������߸˼������
					if (pLineAngle->GetWidth() < 63)
						LsSpace.EndSpace = 25;
					else if (pLineAngle->GetWidth() <= 110)
						LsSpace.EndSpace = 30;
					else
						LsSpace.EndSpace = 40;
				}
				desLjPartPara.ber_space = desLjPartPara.wing_space = desLjPartPara.end_space = LsSpace.EndSpace;
				desLjPartPara.angle.cbSpaceFlag.SetEndSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
			}
			pPlate->designInfo.partList.SetValue(desLjPartPara.hPart, desLjPartPara);
			newrods.append(pRod);
		}
	}
	if (newrods.GetNodeNum()>0)
	{
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Select connection bolts in new imported rods, press enter key to confirm:","");
#else 
		pCmdLine->FillCmdLine("ѡ�����������Ӹ˼��ϵ�������˨,���س���ȷ��:","");
#endif
		if (!pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			pCmdLine->CancelCmdLine();
			return;
		}
		n=g_pSolidSnap->GetLastSelectEnts(id_arr);
		for (i=0;i<n;i++)
		{
			CLDSBolt* pBolt=(CLDSBolt*)Ta.Parts.FromHandle(id_arr[i],CLS_BOLT);
			if (pBolt==NULL)
				continue;
			for (CLDSLinePart* pRod =newrods.GetFirst(); pRod; pRod =newrods.GetNext())
			{
				if (pRod->FindLsByHandle(pBolt->handle)==NULL)
					continue;
				pPlate->AppendLsRef(pBolt->GetLsRef());
			}
		}
	}
	if (cDefaultPlane-'0'>pPlate->face_N)
	{
		pPlate->designInfo.m_bEnableFlexibleDesign=TRUE;//��ֹ֮ǰδ�����������ֱ����Ϊ�����嵼���������ʧ�ܡ�wjh-2015.9.9
		pPlate->face_N++;
		long hDatumPart=pPlate->designInfo.m_hBasePart;
		CInputHuoQuLineDlg dlg;
		dlg.m_pCrossRod=newrods.GetFirst(); //������ĸ�ֵ
		dlg.m_sJgHandle.Format("0X%X",hDatumPart);
		dlg.m_iOffsetWing=0;
		dlg.m_iWingOffsetType=4;			//�Զ���
		pPart=console.FromPartHandle(pPlate->designInfo.m_hBasePart);
		if (pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pPart;
			double justify_x=pLineAngle->get_norm_x_wing()*pPlate->ucs.axis_z;
			double justify_y=pLineAngle->get_norm_y_wing()*pPlate->ucs.axis_z;
			if (fabs(justify_x)>fabs(justify_y))
				dlg.m_iOffsetWing=0;
			else
				dlg.m_iOffsetWing=1;
		}
		if (dlg.DoModal()!=IDOK)
		{
			cmdStr.Format("%d���Ļ����߳�ʼ��ʧ��",pPlate->face_N);
			AfxMessageBox(cmdStr);
			pPlate->face_N--;
			return;
		}
		pPlate->designInfo.iFaceType=(int)pPlate->face_N;
		if (pPlate->face_N==3)
		{	//��ʱ��֧���۵���ʽ������壬iFaceType���Խ���Ӧ��wiLegacyStyle����������ͳһȡ�� wjh-2017.8.30
			pPlate->wiLegacyStyle=1;
			pPlate->designInfo.iFaceType=4;
		}
		pPlate->designInfo.iProfileStyle0123=1;
		pPlate->designInfo.huoquline_start[pPlate->face_N-2]=dlg.m_xStartPt;
		pPlate->designInfo.huoquline_end[pPlate->face_N-2]=dlg.m_xEndPt;
		pPlate->designInfo.huoqufacenorm[pPlate->face_N-2]=dlg.m_xVector;
		dlg.m_xVector.UpdateVector(console.GetActiveModel());
		pPlate->huoqufacenorm[pPlate->face_N-2]=dlg.m_xVector.vector;
	}
	pPlate->DesignPlate();
	pPlate->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	OnOperOther();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("����:","");
#endif
}
#endif