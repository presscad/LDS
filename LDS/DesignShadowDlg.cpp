//<LOCALE_TRANSLATE Confirm by hxr/>
// DesignShadowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "DesignShadowDlg.h"
#include "PartUcsDlg.h"
#include "DatumPointPropertyDlg.h"
#include "DesignVectorPropDlg.h"
#include "LmaDlg.h"
#include ".\designshadowdlg.h"
#include "ReportPointDlg.h"
#include "ScrTools.h"
#include "DisplayPointProperty.h"
#include "DisplayVectorProperty.h"


// CDesignShadowDlg �Ի���

static void CalAndRefreshItemValue(CPropertyList *pPropList)
{
	CDesignShadowDlg *pDesignShadowDlg=(CDesignShadowDlg*)pPropList->GetParent();
	CParaCS::PARAMETRIC_INFO paramcs=pDesignShadowDlg->des_cs.GetParamInfo();
	if (pDesignShadowDlg->m_iShadowCreateStyle==2)
	{
		paramcs.desOrg = pDesignShadowDlg->datumPoint;
		if (paramcs.m_eDatumPlane==CParaCS::DP_XY)
		{
			paramcs.desAxisX=pDesignShadowDlg->datumVector1;
			paramcs.desAxisY=pDesignShadowDlg->datumVector2;
		}
		else if (paramcs.m_eDatumPlane==CParaCS::DP_YX)
		{
			paramcs.desAxisY=pDesignShadowDlg->datumVector1;
			paramcs.desAxisX=pDesignShadowDlg->datumVector2;
		}
		else if (paramcs.m_eDatumPlane==CParaCS::DP_YZ)
		{
			paramcs.desAxisY=pDesignShadowDlg->datumVector1;
			paramcs.desAxisZ=pDesignShadowDlg->datumVector2;
		}
		else if (paramcs.m_eDatumPlane==CParaCS::DP_ZY)
		{
			paramcs.desAxisZ=pDesignShadowDlg->datumVector1;
			paramcs.desAxisY=pDesignShadowDlg->datumVector2;
		}
		else if (paramcs.m_eDatumPlane==CParaCS::DP_ZX)
		{
			paramcs.desAxisZ=pDesignShadowDlg->datumVector1;
			paramcs.desAxisX=pDesignShadowDlg->datumVector2;
		}
		else if (paramcs.m_eDatumPlane==CParaCS::DP_XZ)
		{
			paramcs.desAxisX=pDesignShadowDlg->datumVector1;
			paramcs.desAxisZ=pDesignShadowDlg->datumVector2;
		}
	}
	pDesignShadowDlg->des_cs.SetParamInfo(paramcs);
	char sText[MAX_TEMP_CHAR_100+1]="";
	//
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.x"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.x"),sText);
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.y"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.y"),sText);
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.z"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.z"),sText);
	//
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("_direction.x"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("_direction.x"),sText);
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("_direction.y"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("_direction.y"),sText);
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("_direction.z"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("_direction.z"),sText);
	//
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.desOrg.x"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.desOrg.x"),sText);
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.desOrg.y"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.desOrg.y"),sText);
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.desOrg.z"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.desOrg.z"),sText);
	//
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.desAxisX.x"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.desAxisX.x"),sText);
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.desAxisX.y"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.desAxisX.y"),sText);
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.desAxisX.z"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.desAxisX.z"),sText);
	//
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.desAxisY.x"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.desAxisY.x"),sText);
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.desAxisY.y"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.desAxisY.y"),sText);
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.desAxisY.z"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.desAxisY.z"),sText);
	//
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.desAxisZ.x"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.desAxisZ.x"),sText);
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.desAxisZ.y"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.desAxisZ.y"),sText);
	if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("paramcs.desAxisZ.z"),sText)>0)
		pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("paramcs.desAxisZ.z"),sText);
}

BOOL DatumPointButtonClick(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CDesignShadowDlg *pDesignShadowDlg=(CDesignShadowDlg*)pPropList->GetParent();
	if(pDesignShadowDlg==NULL)
		return FALSE;
	CPropTreeItem *pParentItem = pDesignShadowDlg->m_xDesignInfoPropList.GetSelectedItem();
	if(pParentItem!=NULL)
		pDesignShadowDlg->m_idParentEventProp=pParentItem->m_idProp;
	return CDisplayPointProperty::DatumPointButtonClick(pPropList,pItem);
}

BOOL DatumVectorButtonClick(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CDesignShadowDlg *pDesignShadowDlg=(CDesignShadowDlg*)pPropList->GetParent();
	if(pDesignShadowDlg==NULL)
		return FALSE;
	CPropTreeItem *pParentItem = pDesignShadowDlg->m_xDesignInfoPropList.GetSelectedItem();
	if(pParentItem!=NULL)
		pDesignShadowDlg->m_idParentEventProp=pParentItem->m_idProp;
	return CDisplayVectorProperty::VectorButtonClick(pPropList,pItem);
}

static BOOL FireValueModify(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CDesignShadowDlg *pDesignShadowDlg=(CDesignShadowDlg*)pPropList->GetParent();
	int idProp;
	CPropTreeItem *pParentItem,*pPropItem,*pLeafItem;
	char sText[MAX_TEMP_CHAR_100+1]="";
	CItemInfo* lpInfo = NULL;
	try
	{
		if(pItem->m_idProp==CDesignShadowDlg::GetPropID("m_bMirAxisX"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("��")==0)
#endif
			{
				pDesignShadowDlg->m_bMirAxisX=TRUE;
				if (pDesignShadowDlg->GetShadowObj())
				{	//�޸�Ӱ�乹�������Ϣʱֻ��ѡ��������е�һ����Գ�
					pDesignShadowDlg->m_bMirAxisY=FALSE;
					pDesignShadowDlg->m_bMirAxisZ=FALSE;
					if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("m_bMirAxisX"),sText)>0)
						pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("m_bMirAxisX"),sText);
					if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("m_bMirAxisY"),sText)>0)
						pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("m_bMirAxisY"),sText);
				}
			}
			else
				pDesignShadowDlg->m_bMirAxisX=FALSE;
			if (pDesignShadowDlg->m_bMirAxisX)
			{
				idProp=CDesignShadowDlg::GetPropID("m_bAdvancedMir");
				pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pPropItem==NULL)
				{
					//�߼��Գ�
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
					lpInfo->m_cmbType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_cmbItems="Yes|No";
					lpInfo->m_strPropName = "Senior mirror";
					lpInfo->m_strPropHelp = "is Senior mirror or not";
#else
					lpInfo->m_cmbItems="��|��";
					lpInfo->m_strPropName = "�߼��Գ�";
					lpInfo->m_strPropHelp = "�Ƿ�߼��Գơ�";
#endif
					pPropItem = pPropList->InsertItem(pItem->m_pParent,lpInfo, -1,TRUE);
					pPropItem->m_idProp = CDesignShadowDlg::GetPropID("m_bAdvancedMir");
					if(pDesignShadowDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				}
			}
			if (pDesignShadowDlg->m_bMirAxisX==FALSE&&pDesignShadowDlg->m_bMirAxisY==FALSE&&pDesignShadowDlg->m_bMirAxisZ==FALSE)
			{
				pPropList->DeleteItemByPropId(CDesignShadowDlg::GetPropID("m_bAdvancedMir"));
				pDesignShadowDlg->m_bAdvancedMir=FALSE;
			}
		}
		else if(pItem->m_idProp==CDesignShadowDlg::GetPropID("m_bMirAxisY"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("��")==0)
#endif
			{
				pDesignShadowDlg->m_bMirAxisY=TRUE;
				if (pDesignShadowDlg->GetShadowObj())
				{	//�޸�Ӱ�乹�������Ϣʱֻ��ѡ��������е�һ����Գ�
					pDesignShadowDlg->m_bMirAxisX=FALSE;
					pDesignShadowDlg->m_bMirAxisZ=FALSE;
					if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("m_bMirAxisX"),sText)>0)
						pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("m_bMirAxisX"),sText);
					if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("m_bMirAxisZ"),sText)>0)
						pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("m_bMirAxisZ"),sText);
				}
			}
			else
				pDesignShadowDlg->m_bMirAxisY=FALSE;
			if (pDesignShadowDlg->m_bMirAxisY)
			{
				idProp=CDesignShadowDlg::GetPropID("m_bAdvancedMir");
				pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pPropItem==NULL)
				{
					//�߼��Գ�
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
					lpInfo->m_cmbType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_cmbItems="Yes|No";
					lpInfo->m_strPropName = "Senior mirror";
					lpInfo->m_strPropHelp = "is Senior mirror or not.";
#else
					lpInfo->m_cmbItems="��|��";
					lpInfo->m_strPropName = "�߼��Գ�";
					lpInfo->m_strPropHelp = "�Ƿ�߼��Գơ�";
#endif
					pPropItem = pPropList->InsertItem(pItem->m_pParent,lpInfo, -1,TRUE);
					pPropItem->m_idProp = CDesignShadowDlg::GetPropID("m_bAdvancedMir");
					if(pDesignShadowDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				}
			}
			if (pDesignShadowDlg->m_bMirAxisX==FALSE&&pDesignShadowDlg->m_bMirAxisY==FALSE&&pDesignShadowDlg->m_bMirAxisZ==FALSE)
			{
				pPropList->DeleteItemByPropId(CDesignShadowDlg::GetPropID("m_bAdvancedMir"));
				pDesignShadowDlg->m_bAdvancedMir=FALSE;
			}
		}
		else if(pItem->m_idProp==CDesignShadowDlg::GetPropID("m_bMirAxisZ"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("��")==0)
#endif
			{
				pDesignShadowDlg->m_bMirAxisZ=TRUE;
				if (pDesignShadowDlg->GetShadowObj())
				{	//�޸�Ӱ�乹�������Ϣʱֻ��ѡ��������е�һ����Գ�
					pDesignShadowDlg->m_bMirAxisX=FALSE;
					pDesignShadowDlg->m_bMirAxisY=FALSE;
					if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("m_bMirAxisY"),sText)>0)
						pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("m_bMirAxisY"),sText);
					if(pDesignShadowDlg->GetPropValueStr(CDesignShadowDlg::GetPropID("m_bMirAxisZ"),sText)>0)
						pPropList->SetItemPropValue(CDesignShadowDlg::GetPropID("m_bMirAxisZ"),sText);
				}
			}
			else
				pDesignShadowDlg->m_bMirAxisZ=FALSE;
			if (pDesignShadowDlg->m_bMirAxisZ)
			{
				idProp=CDesignShadowDlg::GetPropID("m_bAdvancedMir");
				pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pPropItem==NULL)
				{
					//�߼��Գ�
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
					lpInfo->m_cmbType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_cmbItems="Yes|No";
					lpInfo->m_strPropName = "Senior mirror";
					lpInfo->m_strPropHelp = "is Senior mirror or not.";
#else
					lpInfo->m_cmbItems="��|��";
					lpInfo->m_strPropName = "�߼��Գ�";
					lpInfo->m_strPropHelp = "�Ƿ�߼��Գơ�";
#endif
					pPropItem = pPropList->InsertItem(pItem->m_pParent,lpInfo, -1,TRUE);
					pPropItem->m_idProp = CDesignShadowDlg::GetPropID("m_bAdvancedMir");
					if(pDesignShadowDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				}
			}
			if (pDesignShadowDlg->m_bMirAxisX==FALSE&&pDesignShadowDlg->m_bMirAxisY==FALSE&&pDesignShadowDlg->m_bMirAxisZ==FALSE)
			{
				pPropList->DeleteItemByPropId(CDesignShadowDlg::GetPropID("m_bAdvancedMir"));
				pDesignShadowDlg->m_bAdvancedMir=FALSE;
			}
		}
		else if(pItem->m_idProp==CDesignShadowDlg::GetPropID("m_bMirRotation"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("��")==0)
#endif
				pDesignShadowDlg->m_bMirRotation=TRUE;
			else
				pDesignShadowDlg->m_bMirRotation=FALSE;
			if (pDesignShadowDlg->m_bMirRotation)
			{
				idProp=CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.rotate_angle");
				pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pLeafItem==NULL)
				{
					//��ת�Ƕ�
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Rotation Angle��";
					lpInfo->m_strPropHelp = "Rotation Angle of mapping parts relative to source parts.";
#else
					lpInfo->m_strPropName = "��ת�Ƕȡ�";
					lpInfo->m_strPropHelp = "Ӱ�乹�����Դ��������ת�Ƕȡ�";
#endif
					pLeafItem = pPropList->InsertItem(pItem,lpInfo, 4,TRUE);
					pLeafItem->m_idProp = CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.rotate_angle");
					if(pDesignShadowDlg->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				}

				idProp=CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.array_num");
				pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pLeafItem==NULL)
				{
					//�Գ�����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "mirror quantity";
					lpInfo->m_strPropHelp = "mirror quantity when rotating symmetrically.";
#else
					lpInfo->m_strPropName = "�Գ�����";
					lpInfo->m_strPropHelp = "��ת�Գ�ʱ�ԳƵ�������";
#endif
					pLeafItem = pPropList->InsertItem(pItem,lpInfo, 5,TRUE);
					pLeafItem->m_idProp = CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.array_num");
					if(pDesignShadowDlg->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
				}
			}
			else
			{
				pPropList->DeleteItemByPropId(CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.rotate_angle"));
				pPropList->DeleteItemByPropId(CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.array_num"));
			}
		}
		else if (pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.rotate_angle"))
		{
			CParaCS::PARAMETRIC_INFO paramcs=pDesignShadowDlg->des_cs.GetParamInfo();
			paramcs.m_xMirInfo.rotate_angle=atoi(valueStr);
			pDesignShadowDlg->des_cs.SetParamInfo(paramcs);
		}
		else if (pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.array_num"))
		{
			CParaCS::PARAMETRIC_INFO paramcs=pDesignShadowDlg->des_cs.GetParamInfo();
			paramcs.m_xMirInfo.array_num=atoi(valueStr);
			pDesignShadowDlg->des_cs.SetParamInfo(paramcs);
		}
		else if(pItem->m_idProp==CDesignShadowDlg::GetPropID("m_bAdvancedMir"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else
			if(valueStr.CompareNoCase("��")==0)
#endif
				pDesignShadowDlg->m_bAdvancedMir=TRUE;
			else
				pDesignShadowDlg->m_bAdvancedMir=FALSE;
			if ((pDesignShadowDlg->m_bMirAxisX||pDesignShadowDlg->m_bMirAxisY||pDesignShadowDlg->m_bMirAxisZ)
				&&pDesignShadowDlg->m_bAdvancedMir)
			{
				idProp=CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin");
				pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pPropItem==NULL)
				{
					//��׼��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "mirror datum points";
					lpInfo->m_strPropHelp = "mirror datum points in Senior mirror and specified plane mirror mirror  ";
#else
					lpInfo->m_strPropName = "�Գƻ���";
					lpInfo->m_strPropHelp = "�߼��ԳƼ�ָ���澵��Գ�ʱ�ĶԳƻ��㡣";
#endif
					pPropItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
					pPropItem->m_idProp = CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin");
					pPropItem->SetReadOnly();
				}

				idProp=CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.x");
				pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pLeafItem==NULL)
				{
					//��׼��X�������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "X";
					lpInfo->m_strPropHelp = "datum point's X coordinate component";
#else
					lpInfo->m_strPropName = "X����";
					lpInfo->m_strPropHelp = "��׼��X���������";
#endif
					pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
					pLeafItem->m_idProp = CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.x");
					if(pDesignShadowDlg->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
					pLeafItem->SetReadOnly();
				}
				idProp=CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.y");
				pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pLeafItem==NULL)
				{
					//��׼��Y�������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Y";
					lpInfo->m_strPropHelp = "datum point's Y coordinate component.";
#else
					lpInfo->m_strPropName = "Y����";
					lpInfo->m_strPropHelp = "��׼��Y���������";
#endif
					pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
					pLeafItem->m_idProp = CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.y");
					if(pDesignShadowDlg->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
					pLeafItem->SetReadOnly();
				}
				idProp=CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.z");
				pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if (pLeafItem==NULL)
				{
					//��׼��Z�������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Z";
					lpInfo->m_strPropHelp = "datum point's Z coordinate component.";
#else
					lpInfo->m_strPropName = "Z����";
					lpInfo->m_strPropHelp = "��׼��Z���������";
#endif
					pLeafItem= pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
					pLeafItem->m_idProp = CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.z");
					if(pDesignShadowDlg->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
					pLeafItem->SetReadOnly();
				}
			}
			else
			{
				pPropList->DeleteItemByPropId(CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin"));
				pPropList->DeleteItemByPropId(CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.x"));
				pPropList->DeleteItemByPropId(CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.y"));
				pPropList->DeleteItemByPropId(CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin.z"));
			}
		}
		else if (pItem->m_idProp==CDesignShadowDlg::GetPropID("m_fOffsetDist"))
			pDesignShadowDlg->m_fOffsetDist=atof(valueStr);
		else if (pItem->m_idProp==CDesignShadowDlg::GetPropID("m_nOffsetNum"))
			pDesignShadowDlg->m_nOffsetNum=atoi(valueStr);
		else if(pItem->m_idProp==CDesignShadowDlg::GetPropID("des_cs.m_eDatumPlane"))
		{
			CParaCS::PARAMETRIC_INFO paramcs=pDesignShadowDlg->des_cs.GetParamInfo();
			paramcs.m_eDatumPlane=valueStr[0]-'0';
			pDesignShadowDlg->des_cs.SetParamInfo(paramcs);
			char sText[100]="";
			long idProp=CDesignShadowDlg::GetPropID("paramcs.desAxisX");
			if(pDesignShadowDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CDesignShadowDlg::GetPropID("paramcs.desAxisY");
			if(pDesignShadowDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CDesignShadowDlg::GetPropID("paramcs.desAxisZ");
			if(pDesignShadowDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
		}
		else if (pDesignShadowDlg->m_iCurDesParaType==0)
			CDisplayPointProperty::ModifyDatumPointPropValue(pPropList,pItem,valueStr);
		else if (pDesignShadowDlg->m_iCurDesParaType==1||pDesignShadowDlg->m_iCurDesParaType==2)
			CDisplayVectorProperty::ModifyDatumVectorPropValue(pPropList,pItem,valueStr);
		else
			return FALSE;
		CalAndRefreshItemValue(&(pDesignShadowDlg->m_xDesignInfoPropList));
		if (pDesignShadowDlg->GetShadowObj())
			pDesignShadowDlg->RefreshShadowDesignInfo();
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	return TRUE;
}

static BOOL FireButtonClick(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CDesignShadowDlg *pDesignShadowDlg=(CDesignShadowDlg*)pPropList->GetParent();
	try
	{
		if (pDesignShadowDlg->m_iCurDesParaType==0)
			DatumPointButtonClick(pPropList,pItem);
		else if (pDesignShadowDlg->m_iCurDesParaType==1||pDesignShadowDlg->m_iCurDesParaType==2)
			DatumVectorButtonClick(pPropList,pItem);
		else
			return FALSE;
		if (pDesignShadowDlg->GetShadowObj())
			pDesignShadowDlg->RefreshShadowDesignInfo();
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	return TRUE;
}

static BOOL FirePropHelpPrompt(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CDesignShadowDlg *pDesignShadowDlg=(CDesignShadowDlg*)pPropList->GetParent();
	pDesignShadowDlg->m_idParentEventProp = pItem->m_idProp;
	if (pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.m_xMirInfo.origin")||pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.desOrg"))
		pDesignShadowDlg->m_iCurDesParaType=0;
	else if (pItem->m_idProp==CDesignShadowDlg::GetPropID("_direction"))
		pDesignShadowDlg->m_iCurDesParaType=1;
	else if ((pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.desAxisX")||pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.desAxisY")
#ifdef AFX_TARG_ENU_ENGLISH
		||pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.desAxisZ"))&&pItem->m_lpNodeInfo->m_strPropValue.CompareNoCase("Datum Axis")==0)
		pDesignShadowDlg->m_iCurDesParaType=1;
	else if ((pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.desAxisX")||pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.desAxisY")
		||pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.desAxisZ"))&&pItem->m_lpNodeInfo->m_strPropValue.CompareNoCase("Auxiliary Axis")==0)
#else
		||pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.desAxisZ"))&&pItem->m_lpNodeInfo->m_strPropValue.CompareNoCase("��׼��")==0)
		pDesignShadowDlg->m_iCurDesParaType=1;
	else if ((pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.desAxisX")||pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.desAxisY")
		||pItem->m_idProp==CDesignShadowDlg::GetPropID("paramcs.desAxisZ"))&&pItem->m_lpNodeInfo->m_strPropValue.CompareNoCase("������")==0)
#endif
		pDesignShadowDlg->m_iCurDesParaType=2;
	else
		pDesignShadowDlg->m_iCurDesParaType=-1;
	pDesignShadowDlg->DisplaySubPropList(pItem);
	return TRUE;
}

IMPLEMENT_DYNAMIC(CDesignShadowDlg, CDialog)
CDesignShadowDlg::CDesignShadowDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignShadowDlg::IDD, pParent)
{
	CParaCS::PARAMETRIC_INFO paramcs=des_cs.GetParamInfo();
	m_bMirAxisX=FALSE;
	m_bMirAxisY=FALSE;
	m_bMirAxisZ=FALSE;
	m_bMirRotation=FALSE;
	paramcs.m_xMirInfo.rotate_angle=90;
	paramcs.m_xMirInfo.array_num=1;
	m_bAdvancedMir=FALSE;
	datumVector1.vector.Set(1,0,0);
	datumVector2.vector.Set(0,1,0);
	m_fOffsetDist=0;
	m_nOffsetNum=1;
	m_iCurDesParaType=-1;
	paramcs.m_xMirInfo.origin.Set(0,0,0);
	paramcs.desOrg.SetPosition(0,0,0);
	paramcs.desAxisX.vector.Set(1,0,0);
	paramcs.desAxisY.vector.Set(0,1,0);
	paramcs.desAxisZ.vector.Set(0,0,1);
	m_iShadowCreateStyle=1;	//��CParaCS::CS_TRANSLATE��Ӧ
	paramcs.m_cDefStyle=CParaCS::CS_TRANSLATE;
	//paramcs.m_cDefStyle=CParaCS::CS_MIRTRANS;
	des_cs.SetParamInfo(paramcs);
	m_pShadowObj=NULL;
	m_idParentEventProp=0;
}

CDesignShadowDlg::~CDesignShadowDlg()
{
}

void CDesignShadowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_xDesignInfoPropList);
	DDX_Control(pDX, IDC_LIST_BOX_ASSEMBLE_POS, m_xSubPropList);
	DDX_CBIndex(pDX, IDC_CMB_SHADOW_CREATE_STYLE, m_iShadowCreateStyle);
}


BEGIN_MESSAGE_MAP(CDesignShadowDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_CMB_SHADOW_CREATE_STYLE, OnCbnSelchangeCmbShadowCreateStyle)
END_MESSAGE_MAP()

// CDesignShadowDlg ��Ϣ�������

BOOL CDesignShadowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitPropHashtable();
	m_xDesignInfoPropList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xDesignInfoPropList.SetDividerScale(0.6);
	m_xDesignInfoPropList.SetModifyValueFunc(FireValueModify);
	m_xDesignInfoPropList.SetButtonClickFunc(FireButtonClick);
	m_xDesignInfoPropList.SetPropHelpPromptFunc(FirePropHelpPrompt);
	//
	m_xSubPropList.SetDividerScale(0.5);
	m_xSubPropList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xSubPropList.SetModifyValueFunc(FireValueModify);
	m_xSubPropList.SetButtonClickFunc(FireButtonClick);
	if (!m_bInernalStart)
	{
		if (m_pShadowObj)
		{	//�鿴���޸�Ӱ�乹�������Ϣ
			if (m_pShadowObj->GetParaCS())
			{	//������λ��˨���ܵ���������Ӱ����Ϣ��ͨ������Ӱ�乹�����ġ�
				CParaCS::PARAMETRIC_INFO paramcs=m_pShadowObj->GetParaCS()->GetParamInfo();
				des_cs.SetParamInfo(paramcs);
				if (paramcs.m_cDefStyle==CParaCS::CS_PARAM)
					m_iShadowCreateStyle=2;
				else if (paramcs.m_cDefStyle==CParaCS::CS_TRANSLATE)
				{
					m_iShadowCreateStyle=1;
					datumVector1=paramcs.desAxisX;
					m_fOffsetDist=paramcs.m_xMirInfo.origin.x;				
				}
				else if (paramcs.m_cDefStyle==CParaCS::CS_MIRTRANS)
				{
					m_iShadowCreateStyle=0;
					if(paramcs.m_xMirInfo.axis_flag&0x01)
						m_bMirAxisX=TRUE;
					if(paramcs.m_xMirInfo.axis_flag&0x02)
						m_bMirAxisY=TRUE;
					if(paramcs.m_xMirInfo.axis_flag&0x04)
						m_bMirAxisZ=TRUE;
					if(paramcs.m_xMirInfo.axis_flag&0x08)
						m_bMirRotation=TRUE;
					if(paramcs.m_xMirInfo.axis_flag&0x10)
						m_iShadowCreateStyle=3;
					if (m_iShadowCreateStyle==0&&!paramcs.m_xMirInfo.origin.IsZero())
						m_bAdvancedMir=TRUE;
					datumPoint.SetPosition(paramcs.m_xMirInfo.origin);
					datumVector1.vector=paramcs.m_xMirInfo.mir_norm;
				}
				UpdateData(FALSE);
			}
		}
		DisplayShadowDesignPropList();
		DisplaySubPropList(SetDesignInfoPropListCurSel());
	}
	else
	{
		FinishSelectObjOper();	//���ѡ�����ĺ�������
		DisplayShadowDesignPropList();
		DisplaySubPropList(NULL);
		//�ָ�ѡ����
		CPropTreeItem *pItem=m_xDesignInfoPropList.FindItemByPropId(m_idParentEventProp,NULL);
		m_xDesignInfoPropList.SetFocus();	//��������ý���
		if(pItem)
			m_xDesignInfoPropList.SetCurSel(pItem->m_iIndex);	//����ѡ����
		DisplaySubPropList(pItem);	//��ʾ������
		CPropTreeItem *pSubItem=m_xSubPropList.FindItemByPropId(m_idEventProp,NULL);
		m_xSubPropList.SetFocus();
		if(pSubItem)	//ѡ��������
			m_xSubPropList.SetCurSel(pSubItem->m_iIndex);
	}
	CalAndRefreshItemValue(&m_xDesignInfoPropList);
	//�����Ի���λ��
	CRect rect,rc;
	GetWindowRect(rect);
	rc.top=rc.left=0;
	rc.right=rect.Width();
	rc.bottom=rect.Height();
	MoveWindow(rc,TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

const DWORD HASHTABLESIZE = 500;
CStringKeyHashTable<long> CDesignShadowDlg::propHashtable;
void CDesignShadowDlg::InitPropHashtable()
{
	int id = 501;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	propHashtable.SetValueAt("designInfo",id++);
	propHashtable.SetValueAt("m_bMirAxisX",id++);
	propHashtable.SetValueAt("m_bMirAxisY",id++);
	propHashtable.SetValueAt("m_bMirAxisZ",id++);
	propHashtable.SetValueAt("m_bMirRotation",id++);
	propHashtable.SetValueAt("paramcs.m_xMirInfo.rotate_angle",id++);
	propHashtable.SetValueAt("paramcs.m_xMirInfo.array_num",id++);
	propHashtable.SetValueAt("m_bAdvancedMir",id++);
	propHashtable.SetValueAt("paramcs.m_xMirInfo.origin",id++);
	propHashtable.SetValueAt("paramcs.m_xMirInfo.origin.x",id++);
	propHashtable.SetValueAt("paramcs.m_xMirInfo.origin.y",id++);
	propHashtable.SetValueAt("paramcs.m_xMirInfo.origin.z",id++);
	propHashtable.SetValueAt("_direction",id++);		//ƫ�Ʒ���/�����淨�߷���
	propHashtable.SetValueAt("_direction.x",id++);
	propHashtable.SetValueAt("_direction.y",id++);
	propHashtable.SetValueAt("_direction.z",id++);
	propHashtable.SetValueAt("m_fOffsetDist",id++);
	propHashtable.SetValueAt("m_nOffsetNum",id++);
	//
	propHashtable.SetValueAt("des_cs.m_eDatumPlane",id++);
	propHashtable.SetValueAt("paramcs.desOrg",id++);
	propHashtable.SetValueAt("paramcs.desOrg.x",id++);
	propHashtable.SetValueAt("paramcs.desOrg.y",id++);
	propHashtable.SetValueAt("paramcs.desOrg.z",id++);
	propHashtable.SetValueAt("paramcs.desAxisX",id++);
	propHashtable.SetValueAt("paramcs.desAxisX.x",id++);
	propHashtable.SetValueAt("paramcs.desAxisX.y",id++);
	propHashtable.SetValueAt("paramcs.desAxisX.z",id++);
	propHashtable.SetValueAt("paramcs.desAxisY",id++);
	propHashtable.SetValueAt("paramcs.desAxisY.x",id++);
	propHashtable.SetValueAt("paramcs.desAxisY.y",id++);
	propHashtable.SetValueAt("paramcs.desAxisY.z",id++);
	propHashtable.SetValueAt("paramcs.desAxisZ",id++);
	propHashtable.SetValueAt("paramcs.desAxisZ.x",id++);
	propHashtable.SetValueAt("paramcs.desAxisZ.y",id++);
	propHashtable.SetValueAt("paramcs.desAxisZ.z",id++);
}

long CDesignShadowDlg::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
	{
		/*char prop_str[500];
		strcpy(prop_str,"û�ҵ���Ϊ\"");
		strcat(prop_str,propStr);
		strcat(prop_str,"\"������ID��";
		AfxMessageBox(prop_str);*/
		return 0;
	}
}

//�������Ӱ�乹��
void CDesignShadowDlg::ReDesignShadowObj()
{
	if(m_pShadowObj&&m_pShadowObj->GetClassTypeId()!=CLS_NODE)
	{
		CLDSPart *pShadowPart=(CLDSPart*)m_pShadowObj;
		pShadowPart->ShadowDesign();
		pShadowPart->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pShadowPart->GetSolidPartObject());
		g_pSolidDraw->AddCS(pShadowPart->handle,pShadowPart->ucs);
		g_pSolidDraw->Draw();
		//������ƹ����ϵĹ�����˨
		CLsRefList *pLsRefList=pShadowPart->GetLsRefList();
		if (pLsRefList!=NULL)
		{
			for (CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			{
				CLDSBolt *pShadowBolt=pLsRef->GetLsPtr();
				if (pShadowBolt->GetShadowObjType()!=CLDSObject::SHADOW_ISOLATED_RELA_LOCATION)
					continue;
				pShadowBolt->ShadowDesign();
				pShadowBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pShadowBolt->GetSolidPartObject());
				g_pSolidDraw->Draw();
			}
		}
	}
}

int CDesignShadowDlg::GetPropValueStr(long id, char *valueStr)
{	//��������ID�õ�����ֵ
	CParaCS::PARAMETRIC_INFO paramcs=des_cs.GetParamInfo();
	CXhChar500 sText;
	if (id==GetPropID("designInfo"))
		strcpy(sText,"");
	else if (id==GetPropID("m_bMirAxisX"))
	{
		if (m_bMirAxisX)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if (id==GetPropID("m_bMirAxisY"))
	{
		if (m_bMirAxisY)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if (id==GetPropID("m_bMirAxisZ"))
	{
		if (m_bMirAxisZ)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if (id==GetPropID("m_bMirRotation"))
	{
		if (m_bMirRotation)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if (id==GetPropID("paramcs.m_xMirInfo.rotate_angle"))
	{
		sprintf(sText,"%d",paramcs.m_xMirInfo.rotate_angle);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.m_xMirInfo.array_num"))
	{
		sprintf(sText,"%d",paramcs.m_xMirInfo.array_num);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("m_bAdvancedMir"))
	{
		if (m_bAdvancedMir)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if (id==GetPropID("paramcs.m_xMirInfo.origin.x"))
	{
		sprintf(sText,"%f",datumPoint.Position().x);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.m_xMirInfo.origin.y"))
	{
		sprintf(sText,"%f",datumPoint.Position().y);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.m_xMirInfo.origin.z"))
	{
		sprintf(sText,"%f",datumPoint.Position().z);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("_direction.x"))
	{
		sprintf(sText,"%f",datumVector1.vector.x);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("_direction.y"))
	{
		sprintf(sText,"%f",datumVector1.vector.y);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("_direction.z"))
	{
		sprintf(sText,"%f",datumVector1.vector.z);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("m_fOffsetDist"))
	{
		sprintf(sText,"%f",m_fOffsetDist);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("m_nOffsetNum"))
	{
		sprintf(sText,"%d",m_nOffsetNum);
		SimplifiedNumString(sText);
	}
	//
	else if (id==GetPropID("des_cs.m_eDatumPlane"))
	{
		if (m_iShadowCreateStyle==2)
		{
			if(paramcs.m_eDatumPlane==CParaCS::DP_XY)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"0.X-Y Plane");
			else if(paramcs.m_eDatumPlane==CParaCS::DP_YX)
				strcpy(sText,"1.Y-X Plane");
			else if(paramcs.m_eDatumPlane==CParaCS::DP_YZ)
				strcpy(sText,"2.Y-Z Plane");
			else if(paramcs.m_eDatumPlane==CParaCS::DP_ZY)
				strcpy(sText,"3.Z-Y Plane");
			else if(paramcs.m_eDatumPlane==CParaCS::DP_ZX)
				strcpy(sText,"4.Z-X Plane");
			else if(paramcs.m_eDatumPlane==CParaCS::DP_XZ)
				strcpy(sText,"5.X-Z Plane");
#else
				strcpy(sText,"0.X-Y��");
			else if(paramcs.m_eDatumPlane==CParaCS::DP_YX)
				strcpy(sText,"1.Y-X��");
			else if(paramcs.m_eDatumPlane==CParaCS::DP_YZ)
				strcpy(sText,"2.Y-Z��");
			else if(paramcs.m_eDatumPlane==CParaCS::DP_ZY)
				strcpy(sText,"3.Z-Y��");
			else if(paramcs.m_eDatumPlane==CParaCS::DP_ZX)
				strcpy(sText,"4.Z-X��");
			else if(paramcs.m_eDatumPlane==CParaCS::DP_XZ)
				strcpy(sText,"5.X-Z��");
#endif
		}
		else
			strcpy(sText,"");
	}
	else if (id==GetPropID("paramcs.desOrg.x"))
	{
		sprintf(sText,"%f",paramcs.desOrg.Position().x);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.desOrg.y"))
	{
		sprintf(sText,"%f",paramcs.desOrg.Position().y);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.desOrg.z"))
	{
		sprintf(sText,"%f",paramcs.desOrg.Position().z);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.desAxisX"))
	{
		if (m_iShadowCreateStyle==2)
			strcpy(sText,des_cs.GetAxisDescription('X'));
		else
			strcpy(sText," ");
	}
	else if (id==GetPropID("paramcs.desAxisX.x"))
	{
		sprintf(sText,"%f",paramcs.desAxisX.vector.x);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.desAxisX.y"))
	{
		sprintf(sText,"%f",paramcs.desAxisX.vector.y);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.desAxisX.z"))
	{
		sprintf(sText,"%f",paramcs.desAxisX.vector.z);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.desAxisY"))
	{
		if (m_iShadowCreateStyle==2)
			strcpy(sText,des_cs.GetAxisDescription('Y'));
		else
			strcpy(sText," ");
	}
	else if (id==GetPropID("paramcs.desAxisY.x"))
	{
		sprintf(sText,"%f",paramcs.desAxisY.vector.x);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.desAxisY.y"))
	{
		sprintf(sText,"%f",paramcs.desAxisY.vector.y);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.desAxisY.z"))
	{
		sprintf(sText,"%f",paramcs.desAxisY.vector.z);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.desAxisZ"))
	{
		if (m_iShadowCreateStyle==2)
			strcpy(sText,des_cs.GetAxisDescription('Z'));
		else
			strcpy(sText," ");
	}
	else if (id==GetPropID("paramcs.desAxisZ.x"))
	{
		sprintf(sText,"%f",paramcs.desAxisZ.vector.x);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.desAxisZ.y"))
	{
		sprintf(sText,"%f",paramcs.desAxisZ.vector.y);
		SimplifiedNumString(sText);
	}
	else if (id==GetPropID("paramcs.desAxisZ.z"))
	{
		sprintf(sText,"%f",paramcs.desAxisZ.vector.z);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}

void CDesignShadowDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	//g_pSolidDraw->ReleaseSnapStatus();
	//g_pSolidDraw->SetEntSnapStatus(m_pAngle->handle);
	CParaCS::PARAMETRIC_INFO paramcs=des_cs.GetParamInfo();
	CLDSPoint *pPoint = NULL;
	CLDSVector *pVector = NULL;
	if (m_iCurDesParaType==0)
		pPoint=&datumPoint;
	else if (m_iCurDesParaType==1)
		pVector=&datumVector1;
	else if (m_iCurDesParaType==2)
		pVector=&datumVector2;
	if (m_iCurDesParaType==0)
		CDisplayPointProperty::FinishSelectObjOper(pPoint,m_hPickObj,GetEventPropId());
	else if (m_iCurDesParaType==1||m_iCurDesParaType==2)
		CDisplayVectorProperty::FinishSelectObjOper(pVector,m_hPickObj,GetEventPropId());
	if (GetEventPropId()==GetPropID("paramcs.desOrg"))
		paramcs.desOrg=*pPoint;
	else if (GetEventPropId()==GetPropID("_direction"))
		datumVector1=*pVector;
	else if (GetEventPropId()==GetPropID("paramcs.desAxisX"))
		paramcs.desAxisX=*pVector;
	else if (GetEventPropId()==GetPropID("paramcs.desAxisY"))
		paramcs.desAxisY=*pVector;
	else if (GetEventPropId()==GetPropID("paramcs.desAxisZ"))
		paramcs.desAxisZ=*pVector;
	des_cs.SetParamInfo(paramcs);
}

void CDesignShadowDlg::DisplayShadowDesignPropList()
{
	m_xDesignInfoPropList.CleanTree();
	m_xDesignInfoPropList.m_nObjClassTypeId = 0;
	//
	CPropTreeItem *pGroupItem,*pPropItem,*pLeafItem,*pRoot=m_xDesignInfoPropList.GetRootItem();
	char sText[MAX_TEMP_CHAR_100+1]="";
	CItemInfo* lpInfo = NULL;
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "design info";
	lpInfo->m_strPropHelp = "design information";
#else
	lpInfo->m_strPropName = "�����Ϣ";
	lpInfo->m_strPropHelp = "�����Ϣ";
#endif
	pGroupItem = m_xDesignInfoPropList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp =GetPropID("designInfo");
	//
	if (m_iShadowCreateStyle==0)
	{
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName = "X axis mirror";
		lpInfo->m_strPropHelp = "is X axis mirror";
#else
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName = "X��Գ�";
		lpInfo->m_strPropHelp = "�Ƿ����X��Գ�";
#endif
		pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("m_bMirAxisX");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName = "Y axis mirror";
		lpInfo->m_strPropHelp = "is Y axis mirror";
#else
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName = "Y��Գ�";
		lpInfo->m_strPropHelp = "�Ƿ����Y��Գ�";
#endif
		pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("m_bMirAxisY");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName = "Y axis mirror";
		lpInfo->m_strPropHelp = "is Y axis mirror";
#else
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName = "Z��Գ�";
		lpInfo->m_strPropHelp = "�Ƿ����Z��Գ�";
#endif
		pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("m_bMirAxisZ");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName = "rotation mirror";
		lpInfo->m_strPropHelp = "is rotation mirror";
#else
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName = "��ת�Գ�";
		lpInfo->m_strPropHelp = "�Ƿ���ת�Գ�";
#endif
		pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("m_bMirRotation");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if (m_bMirRotation)
		{
			//��ת�Ƕ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "rotating angle��";
			lpInfo->m_strPropHelp = "rotating angle of mapping parts relative to source parts.";
#else
			lpInfo->m_strPropName = "��ת�Ƕȡ�";
			lpInfo->m_strPropHelp = "Ӱ�乹�����Դ��������ת�Ƕȡ�";
#endif
			pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
			pLeafItem->m_idProp = GetPropID("paramcs.m_xMirInfo.rotate_angle");
			if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;

			//�Գ�����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "mirror quantity";
			lpInfo->m_strPropHelp = "mirror quantity when rotating symmetrically.";
#else
			lpInfo->m_strPropName = "�Գ�����";
			lpInfo->m_strPropHelp = "��ת�Գ�ʱ�ԳƵ�������";
#endif
			pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
			pLeafItem->m_idProp = GetPropID("paramcs.m_xMirInfo.array_num");
			if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		//
		if (m_bMirAxisX||m_bMirAxisY||m_bMirAxisZ)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName = "Senior mirror";
			lpInfo->m_strPropHelp = "is Senior mirror";
#else
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName = "�߼��Գ�";
			lpInfo->m_strPropHelp = "�Ƿ�߼��Գ�";
#endif
			pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = GetPropID("m_bAdvancedMir");
			if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
	if ((m_iShadowCreateStyle==0&&(m_bMirAxisX||m_bMirAxisY||m_bMirAxisZ)&&m_bAdvancedMir)
		||m_iShadowCreateStyle==3)
	{
		CPropTreeItem *pSonItem;
		//�Գƻ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "mirror datum point";
		lpInfo->m_strPropHelp = "mirror datum points in Senior mirror and specified plane mirror mirror.";
#else
		lpInfo->m_strPropName = "�Գƻ���";
		lpInfo->m_strPropHelp = "�߼��ԳƼ�ָ���澵��Գ�ʱ�ĶԳƻ��㡣";
#endif
		if (m_iShadowCreateStyle==0&&m_bAdvancedMir)
			pSonItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		else
			pSonItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
		pSonItem->m_idProp = GetPropID("paramcs.m_xMirInfo.origin");
		pSonItem->SetReadOnly();

		//�Գƻ���X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X";
		lpInfo->m_strPropHelp = "datum point's X coordinate component";
#else
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "�Գƻ���X���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pSonItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.m_xMirInfo.origin.x");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//�Գƻ���Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y";
		lpInfo->m_strPropHelp = "datum point's Y coordinate component";
#else
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "�Գƻ���Y���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pSonItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.m_xMirInfo.origin.y");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//�Գƻ���Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z";
		lpInfo->m_strPropHelp = "datum point's Z coordinate component";
#else
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "�Գƻ���Z���������";
#endif
		pLeafItem= m_xDesignInfoPropList.InsertItem(pSonItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.m_xMirInfo.origin.z");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
	}
	if (m_iShadowCreateStyle==1||m_iShadowCreateStyle==3)
	{
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		if (m_iShadowCreateStyle==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "offset direction";
			lpInfo->m_strPropHelp = "offset direction when specified offset value generates mapping part.";
#else
			lpInfo->m_strPropName = "ƫ�Ʒ���";
			lpInfo->m_strPropHelp = "ָ��ƫ��������Ӱ�乹��ʱ��ƫ�Ʒ���";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "mirror plane normal ";
			lpInfo->m_strPropHelp = "mirror plane normal direction when specified mirror plane generate mapping part.";
#else
			lpInfo->m_strPropName = "�����淨��";
			lpInfo->m_strPropHelp = "ָ���澵������Ӱ�乹��ʱ������ķ��߷���";
#endif
		}
		pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("_direction");
		pPropItem->SetReadOnly();

		//ƫ�Ʒ�������淨�߷���X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X";
		if (m_iShadowCreateStyle==1)
			lpInfo->m_strPropHelp = "offset direction X coordinate component.";
		else
			lpInfo->m_strPropHelp = "mirror plane normal direction X coordinate component";
#else
		lpInfo->m_strPropName = "X�������";
		if (m_iShadowCreateStyle==1)
			lpInfo->m_strPropHelp = "ƫ�Ʒ���X���������";
		else
			lpInfo->m_strPropHelp = "���淨�߷���X���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("_direction.x");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//ƫ�Ʒ�������淨�߷���Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y";
		if (m_iShadowCreateStyle==1)
			lpInfo->m_strPropHelp = "offset direction Y coordinate component";
		else
			lpInfo->m_strPropHelp = "mirror plane normal direction Y coordinate component";
#else
		lpInfo->m_strPropName = "Y�������";
		if (m_iShadowCreateStyle==1)
			lpInfo->m_strPropHelp = "ƫ�Ʒ���Y���������";
		else
			lpInfo->m_strPropHelp = "���淨�߷���Y���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("_direction.y");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//ƫ�Ʒ�������淨�߷���Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z";
		if (m_iShadowCreateStyle==1)
			lpInfo->m_strPropHelp = "offset direction Z coordinate component";
		else
			lpInfo->m_strPropHelp = "mirror plane normal direction Y coordinate component.";
#else
		lpInfo->m_strPropName = "Z�������";
		if (m_iShadowCreateStyle==1)
			lpInfo->m_strPropHelp = "ƫ�Ʒ���Z���������";
		else
			lpInfo->m_strPropHelp = "���淨�߷���Z���������";
#endif
		pLeafItem= m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("_direction.z");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//
		if (m_iShadowCreateStyle==1)
		{
			//ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "offset";
			lpInfo->m_strPropHelp = "offset value when specified offset value generates mapping parts.";
#else
			lpInfo->m_strPropName = "���ƫ����";
			lpInfo->m_strPropHelp = "ָ��ƫ��������Ӱ�乹��ʱ��ƫ������";
#endif
			pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = GetPropID("m_fOffsetDist");
			if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			//ƫ�Ƹ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "offset quantity";
			lpInfo->m_strPropHelp = "offset quantity when specified offset value generates mapping parts.";
#else
			lpInfo->m_strPropName = "ƫ�Ƹ���";
			lpInfo->m_strPropHelp = "ָ��ƫ��������Ӱ�乹��ʱ��ƫ�Ƹ�����";
#endif
			pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = GetPropID("m_nOffsetNum");
			if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->SetReadOnly(m_pShadowObj!=NULL);	//�鿴�޸�Ӱ�乹����Ϣʱ������Ϊֻ��
		}
	}
	if (m_iShadowCreateStyle==2)
	{
		//װ��ƽ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems=CParaCS::GetDatumPlaneCmbItems();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "datum plane";
		lpInfo->m_strPropHelp = "specify two axes direction��automatically calculate vertical and current plane's direction, modify the second axis direction with the first axis direction fixed, for example:X-Yplane,calculate axis Z direction according to X and Y axes, modify Y axis direction according to axes X and Z.";
#else
		lpInfo->m_strPropName = "��׼��";
		lpInfo->m_strPropHelp = "ָ�������᷽���Զ��������ֱ�뵱ǰ��ķ��򣬵�һ���᷽�򲻱������ڶ����᷽����:X-Y��,����X��Y�����Z�᷽����X��Z������Y�᷽��";
#endif
		pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("des_cs.m_eDatumPlane");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(m_iShadowCreateStyle!=2);
		//װ��λ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "positon";
		lpInfo->m_strPropHelp = "assembly datum point position.";
#else
		lpInfo->m_strPropName = "λ��";
		lpInfo->m_strPropHelp = "װ������λ�á�";
#endif
		pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("paramcs.desOrg");
		pPropItem->SetReadOnly(m_iShadowCreateStyle==0||m_iShadowCreateStyle==3);
		pPropItem->SetReadOnly();

		//װ������ϵԭ��λ��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X";
		lpInfo->m_strPropHelp = "assemble coordinate system origion X coordinate component.";
#else
		lpInfo->m_strPropName = "X����";
		lpInfo->m_strPropHelp = "װ������ϵԭ��λ��X���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.desOrg.x");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//װ������ϵԭ��λ��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y";
		lpInfo->m_strPropHelp = "assemble coordinate system origion Y coordinate component.";
#else
		lpInfo->m_strPropName = "Y����";
		lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Y���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.desOrg.y");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//װ������ϵԭ��λ��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z";
		lpInfo->m_strPropHelp = "assemble coordinate system origion Y coordinate component.";
#else
		lpInfo->m_strPropName = "Z����";
		lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Z���������";
#endif
		pLeafItem= m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.desOrg.z");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//װ��ʱX�᷽��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X axis";
		lpInfo->m_strPropHelp = "in X axis direction when assembling";
#else
		lpInfo->m_strPropName = "X�᷽��";
		lpInfo->m_strPropHelp = "װ��ʱX�᷽��";
#endif
		pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("paramcs.desAxisX");
		pPropItem->m_bHideChildren=TRUE;
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
		//װ������ϵX��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X";
		lpInfo->m_strPropHelp = "assemble coordinate system X axis X coordinate component.";
#else
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "װ������ϵX��X���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.desAxisX.x");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//װ������ϵX��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y";
		lpInfo->m_strPropHelp = "assemble coordinate system X axis Y coordinate component.";
#else
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "װ������ϵX��Y���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.desAxisX.y");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//װ������ϵX��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z";
		lpInfo->m_strPropHelp = "assemble coordinate system X axis Z coordinate component.";
#else
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "װ������ϵX��Z���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.desAxisX.z");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//װ��ʱY�᷽��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y axis";
		lpInfo->m_strPropHelp = "in Y axis direction when assembling";
#else
		lpInfo->m_strPropName = "Y�᷽��";
		lpInfo->m_strPropHelp = "װ��ʱY�᷽��";
#endif
		pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("paramcs.desAxisY");
		pPropItem->m_bHideChildren=TRUE;
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
		//װ������ϵY��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X";
		lpInfo->m_strPropHelp = "assemble coordinate system Y axis X coordinate component.";
#else
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "װ������ϵY��X���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.desAxisY.x");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//װ������ϵY��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y";
		lpInfo->m_strPropHelp = "assemble coordinate system Y axis Y coordinate component.";
#else
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "װ������ϵY��Y���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.desAxisY.y");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//װ������ϵY��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z";
		lpInfo->m_strPropHelp = "assemble coordinate system Y axis Z coordinate component.";
#else
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "װ������ϵY��Z���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.desAxisY.z");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//װ��ʱZ�᷽��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z axis";
		lpInfo->m_strPropHelp = "in Z axis direction when assembling";
#else
		lpInfo->m_strPropName = "Z�᷽��";
		lpInfo->m_strPropHelp = "װ��ʱZ�᷽��";
#endif
		pPropItem = m_xDesignInfoPropList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("paramcs.desAxisZ");
		pPropItem->m_bHideChildren=TRUE;
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();

		//װ������ϵZ��X�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X";
		lpInfo->m_strPropHelp = "assemble coordinate system Z axis X coordinate component.";
#else
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "װ������ϵZ��X���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.desAxisZ.x");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//װ������ϵZ��Y�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y";
		lpInfo->m_strPropHelp = "assemble coordinate system Z axis Y coordinate component.";
#else
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "װ������ϵZ��Y���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.desAxisZ.y");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
		//װ������ϵZ��Z�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z";
		lpInfo->m_strPropHelp = "assemble coordinate system Z axis Z coordinate component.";
#else
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "װ������ϵZ��Z���������";
#endif
		pLeafItem = m_xDesignInfoPropList.InsertItem(pPropItem,lpInfo, -1);
		pLeafItem->m_idProp = GetPropID("paramcs.desAxisZ.z");
		if(GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->SetReadOnly();
	}
	m_xDesignInfoPropList.Redraw();
}

void CDesignShadowDlg::DisplaySubPropList(CPropTreeItem *pItem,BOOL bUpdate/*=FALSE*/)
{
	if (!bUpdate)
		m_xSubPropList.CleanTree();
	m_xSubPropList.m_nObjClassTypeId = 0;
	//
	CLDSPoint *pPoint = NULL;
	CLDSVector *pVector = NULL;
	if (m_iCurDesParaType==0)
		pPoint=&datumPoint;
	else if (m_iCurDesParaType==1)
		pVector=&datumVector1;
	else if (m_iCurDesParaType==2)
		pVector=&datumVector2;
	if (pItem&&m_iCurDesParaType==0)	//�����
		CDisplayPointProperty::DisplayPropList(&m_xSubPropList,pPoint);
	else if (pItem&&m_iCurDesParaType==1||m_iCurDesParaType==2)	//�������
		CDisplayVectorProperty::DisplayPropList(&m_xSubPropList,pVector);
	else
		m_xSubPropList.CleanTree();
	m_xSubPropList.Redraw();
}

//����Ӱ�乹�������Ϣ
void CDesignShadowDlg::RefreshShadowDesignInfo()
{
	CParaCS::PARAMETRIC_INFO paramcs=des_cs.GetParamInfo();
	if (m_iShadowCreateStyle==0)
	{	//�Գ�����
		paramcs.m_xMirInfo.axis_flag=0;
		if(m_bMirAxisX)
			paramcs.m_xMirInfo.axis_flag|=0x01;
		if(m_bMirAxisY)
			paramcs.m_xMirInfo.axis_flag|=0x02;
		if(m_bMirAxisZ)
			paramcs.m_xMirInfo.axis_flag|=0x04;
		if(m_bMirRotation)
			paramcs.m_xMirInfo.axis_flag|=0x08;
		if (m_bAdvancedMir)
			paramcs.m_xMirInfo.origin=datumPoint.Position();
	}
	else if (m_iShadowCreateStyle==1)
	{	//ָ��ƫ��������
		normalize(datumVector1.vector);
		paramcs.desAxisX=datumVector1;
		paramcs.m_xMirInfo.origin.x=m_fOffsetDist;
	}
	else if (m_iShadowCreateStyle==2)
		des_cs.UpdateACS(console.GetActiveModel());
	else if (m_iShadowCreateStyle==3)
	{
		paramcs.m_xMirInfo.origin=datumPoint.Position();
		paramcs.m_xMirInfo.axis_flag|=0x10;
		paramcs.m_xMirInfo.mir_norm=datumVector1.vector;
	}
	des_cs.SetParamInfo(paramcs);
	if (m_pShadowObj)
	{
		m_pShadowObj->GetParaCS()->SetParamInfo(des_cs.GetParamInfo());
		ReDesignShadowObj();
	}
}

void CDesignShadowDlg::OnOK()
{
	CDialog::OnOK();
	RefreshShadowDesignInfo();
}

void CDesignShadowDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialog::OnCancel();
}

//���������Ϣ��������ǰѡ����
CPropTreeItem* CDesignShadowDlg::SetDesignInfoPropListCurSel()
{
	CPropTreeItem *pCurSelItem=NULL;
	if (m_iShadowCreateStyle==0||m_iShadowCreateStyle==3)
	{
		pCurSelItem=m_xDesignInfoPropList.FindItemByPropId(GetPropID("paramcs.m_xMirInfo.origin"),NULL);
		m_iCurDesParaType=0;
	}
	else if (m_iShadowCreateStyle==1)
	{
		pCurSelItem=m_xDesignInfoPropList.FindItemByPropId(GetPropID("_direction"),NULL);
		m_iCurDesParaType=1;
	}
	else if (m_iShadowCreateStyle==2)
	{
		pCurSelItem=m_xDesignInfoPropList.FindItemByPropId(GetPropID("paramcs.desOrg"),NULL);
		m_iCurDesParaType=0;
	}
	m_xDesignInfoPropList.SetFocus();
	if (pCurSelItem)
	{
		m_xDesignInfoPropList.SetCurSel(pCurSelItem->m_iIndex);
		SetEventPropId(pCurSelItem->m_idProp);
		SetDlgItemText(IDC_E_PROP_HELP_STR,pCurSelItem->m_lpNodeInfo->m_strPropHelp);
	}
	return pCurSelItem;
}

void CDesignShadowDlg::OnCbnSelchangeCmbShadowCreateStyle()
{
	UpdateData();
	CParaCS::PARAMETRIC_INFO paramcs=des_cs.GetParamInfo();
	if (m_iShadowCreateStyle==0||m_iShadowCreateStyle==3)
		paramcs.m_cDefStyle=CParaCS::CS_MIRTRANS;
	else if (m_iShadowCreateStyle==1)
		paramcs.m_cDefStyle=CParaCS::CS_TRANSLATE;
	else if (m_iShadowCreateStyle==2)
		paramcs.m_cDefStyle=CParaCS::CS_PARAM;
	des_cs.SetParamInfo(paramcs);
	DisplayShadowDesignPropList();
	DisplaySubPropList(SetDesignInfoPropListCurSel());
	CalAndRefreshItemValue(&m_xDesignInfoPropList);
	if (m_pShadowObj)
		RefreshShadowDesignInfo();
	UpdateData(FALSE);
}
