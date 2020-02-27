// DesignColumnRayRodDlg.cpp : ʵ���ļ�
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "DesignColumnRayRodDlg.h"
#include "DisplayPointProperty.h"
#include "DisplayVectorProperty.h"
#include "GlobalFunc.h"
#include "LmaDlg.h"
#include "dlg.h"
#include "LicFuncDef.h"

//<DEVELOP_PROCESS_MARK nameId="CDesignColumnRayRodDlg::FireRayRodDesignParaPropListFunc">
static BOOL FireRayRodDesignParaHelpStr(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDesignColumnRayRodDlg *pRayRodDlg=(CDesignColumnRayRodDlg*)pPropList->GetParent();
	//��ʾ������
	pRayRodDlg->DisplaySubObjPropList(pItem);	
	return TRUE;
}

static BOOL ModifyColumnIntersPointProperty(CPropertyList *pPropList,CPropTreeItem *pItem,
											CString &valueStr,CLDSPoint *pPoint,BOOL bStart)
{
	CDesignColumnRayRodDlg *pRayRodDlg=(CDesignColumnRayRodDlg*)pPropList->GetParent();
	if(pRayRodDlg==NULL)
		return FALSE;
	CString sPropIdPrefix = "m_xRodStartPos";
	if(!bStart)
		sPropIdPrefix = "m_xRodEndPos";
	if(CDesignColumnRayRodDlg::GetPropID("%s.hLineTube",sPropIdPrefix)==pItem->m_idProp)
	{
		sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.COLUMN_INTERS.hLineTube));
		//
		CLDSLineTube *pLineTube = (CLDSLineTube*)console.FromPartHandle(pPoint->des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
		pPropList->SetItemReadOnly(CDesignColumnRayRodDlg::GetPropID("%s.column_r",sPropIdPrefix),pLineTube!=NULL);
		//����Բ�ĵ�
		CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CDesignColumnRayRodDlg::GetPropID("%s.sect_center",sPropIdPrefix),NULL);
		if(pFindItem)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			CString sPropName = "Section Center";
			if(pLineTube)
				sPropName = "Section Pick Point";
			pPropList->SetItemPropName(pFindItem->m_idProp,sPropName);
			if(pLineTube)
				pFindItem->m_lpNodeInfo->m_strPropHelp = "Section pick point is to calculate section center.";
			else 
				pFindItem->m_lpNodeInfo->m_strPropHelp = "Section Center";
#else 
			CString sPropName = "����Բ��";
			if(pLineTube)
				sPropName = "����ʰȡ��";
			pPropList->SetItemPropName(pFindItem->m_idProp,sPropName);
			if(pLineTube)
				pFindItem->m_lpNodeInfo->m_strPropHelp = "����ʰȡ�����������Բ�ĵ�";
			else 
				pFindItem->m_lpNodeInfo->m_strPropHelp = "����Բ��";
#endif
		}
	}
	else if(CDesignColumnRayRodDlg::GetPropID("%s.column_r",sPropIdPrefix)==pItem->m_idProp)
		pPoint->des_para.COLUMN_INTERS.column_r=atof(valueStr);
	else if(CDesignColumnRayRodDlg::GetPropID("%s.ray_vec_style",sPropIdPrefix)==pItem->m_idProp)
	{	//���߷����巽ʽ
		pPoint->des_para.COLUMN_INTERS.ray_vec_style=valueStr[0]-'0';
		CXhChar100 sText;
		if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0) 
			pPropList->DeleteAllSonItems(pItem);
		else
		{
			CItemInfo* lpInfo;
			CPropTreeItem* pSubItem=pPropList->FindItemByPropId(CDesignColumnRayRodDlg::GetPropID("%s.ray_angle",sPropIdPrefix),NULL);
			if(pSubItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Start Angle";
				lpInfo->m_strPropHelp="The first ray rod's rotate angle relative to specified datum edge,unit is ��";
#else 
				lpInfo->m_strPropName="��ʼ�Ƕ�";
				lpInfo->m_strPropHelp="��һ�����߸˼������ָ����׼�ߵ���ת�Ƕ�,��λΪ��";
#endif
				pSubItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
				pSubItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("%s.ray_angle",sPropIdPrefix);
			}
			if(pRayRodDlg->GetPropValueStr(pSubItem->m_idProp,sText)>0)
				pSubItem->m_lpNodeInfo->m_strPropValue=sText;

			pSubItem=pPropList->FindItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_nRotateArrayN"),NULL);
			if(pSubItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Array Number";
				lpInfo->m_strPropHelp="The ray rods' number around datum axis array,rotate angle is first ray line's angle.";
#else 
				lpInfo->m_strPropName="��������";
				lpInfo->m_strPropHelp="�ƻ�׼����ת�����������߸˼�����������ת�Ƕ���ָ��һ�����ߵĽǶ�";
#endif
				pSubItem = pPropList->InsertItem(pItem,lpInfo,1,TRUE);
				pSubItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_nRotateArrayN");
			}
			if(pRayRodDlg->GetPropValueStr(pSubItem->m_idProp,sText)>0)
				pSubItem->m_lpNodeInfo->m_strPropValue=sText;
			if(pRayRodDlg->m_nRotateArrayN>1)
			{
				CPropTreeItem *pFindItem=pPropList->FindItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_fSectorAngle"),NULL);
				if(pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName="Rotate Angle";
					lpInfo->m_strPropHelp="It is two near ray rods's rotate angle when many ray rods rotate,unit is��";
	#else 
					lpInfo->m_strPropName="��ת�Ƕ�";
					lpInfo->m_strPropHelp="������߸˼���ת����ʱ�ڽ��������߸˼�����ת����Ƕ�,��λΪ��";
	#endif
					pFindItem = pPropList->InsertItem(pSubItem,lpInfo,-1,TRUE);
					pFindItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_fSectorAngle");
				}
				if(pRayRodDlg->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue=sText;
			}
			else
				pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_fSectorAngle"));
		}
		pRayRodDlg->DisplaySubObjPropList(pItem,FALSE);
	}
	else if(CDesignColumnRayRodDlg::GetPropID("%s.ray_angle",sPropIdPrefix)==pItem->m_idProp)
		pPoint->des_para.COLUMN_INTERS.ray_angle=atof(valueStr);
	else if(CDesignColumnRayRodDlg::GetPropID("%s.ray_para_offset",sPropIdPrefix)==pItem->m_idProp)
		pPoint->des_para.COLUMN_INTERS.ray_para_offset=atof(valueStr);
	else if(CDesignColumnRayRodDlg::GetPropID("%s.bSpecRo",sPropIdPrefix)==pItem->m_idProp)
	{
		pPoint->des_para.COLUMN_INTERS.bSpecRo=(valueStr[0]-'0')!=0;
		CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CDesignColumnRayRodDlg::GetPropID("%s.ray_offset_r",sPropIdPrefix),NULL);
		if(pFindItem)
		{
			if(pPoint->des_para.COLUMN_INTERS.bSpecRo)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pPropList->SetItemPropName(pFindItem->m_idProp,CString("Offset"));
				pFindItem->m_lpNodeInfo->m_strPropHelp="Base on intersection point of ray line and column ,the offset along ray line.";
#else 
				pPropList->SetItemPropName(pFindItem->m_idProp,CString("ƫ����"));
				pFindItem->m_lpNodeInfo->m_strPropHelp="�����������潻��Ϊ��׼�������߷����ƫ����";
#endif
			}
			else 
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pPropList->SetItemPropName(pFindItem->m_idProp,CString("Ancillary Ray Radius"));
				pFindItem->m_lpNodeInfo->m_strPropHelp="Base on column center,the offset along ray line.";
#else 
				pPropList->SetItemPropName(pFindItem->m_idProp,CString("�������߰뾶"));
				pFindItem->m_lpNodeInfo->m_strPropHelp="������Ϊ��׼�������߷����ƫ����";
#endif
			}
		}
	}
	else if(CDesignColumnRayRodDlg::GetPropID("%s.ray_offset_r",sPropIdPrefix)==pItem->m_idProp)
		pPoint->des_para.COLUMN_INTERS.ray_offset_r=atof(valueStr);
	else 
		return FALSE;
	return TRUE;
}

static void UpdateColumnIntersPointItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,
										CLDSPoint *pPoint,BOOL bUpdate,BOOL bStart);
static BOOL ModifyRayRodDesignParaProperty(CPropertyList *pPropList,CPropTreeItem *pItem,CString &valueStr)
{
	CXhChar500 sText;
	CPropTreeItem *pSonItem=NULL,*pPropItem=NULL;
	CDesignColumnRayRodDlg *pRayRodDlg=(CDesignColumnRayRodDlg*)pPropList->GetParent();
	//
	if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_sPartNo"))
	{
		pRayRodDlg->m_sPartNo.Copy(valueStr);
		if(ParsePartNo(pRayRodDlg->m_sPartNo,&pRayRodDlg->m_xSegI,NULL))
			pPropList->SetItemPropValue(CDesignColumnRayRodDlg::GetPropID("m_xSegI"),pRayRodDlg->m_xSegI.ToString());
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(pRayRodDlg->m_sPartNo);
		if(pPart)
		{	//�����ҵ���BOM��Ϣ�����¸øְ�Ļ�����Ϣ(�κš���񡢲���)
			if( pPart->cPartType==BOMPART::ANGLE||pPart->cPartType==BOMPART::TUBE||
				pPart->cPartType==BOMPART::SLOT||pPart->cPartType==BOMPART::FLAT)
			{
				pRayRodDlg->m_cMaterial=pPart->cMaterial;
				pRayRodDlg->m_sSpec.Printf("%.fx%.f",pPart->wide,pPart->thick);
				long idProp=CDesignColumnRayRodDlg::GetPropID("m_cMaterial");
				if(pRayRodDlg->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,sText);
				idProp=CDesignColumnRayRodDlg::GetPropID("m_sSpec");
				if(pRayRodDlg->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,sText);
			}
			//else
			//AfxMessageBox(CXhChar100("Bom��û�м���Ϊ#%s�ĸְ�",(char*)pPart->sPartNo));
		}
#endif
	}
	else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_xSegI"))
		pRayRodDlg->m_xSegI=SEGI(valueStr.GetBuffer());
	else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_cMaterial"))
	{
		for(int i=0;i<CSteelMatLibrary::GetCount();i++)
		{
			STEELMAT mat=CSteelMatLibrary::RecordAt(i);
			if(valueStr.CompareNoCase(mat.mark)==0)
			{
				pRayRodDlg->m_cMaterial=mat.cBriefMark;
				break;
			}
		}
	}
	else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_iRodType"))
	{
		pRayRodDlg->m_iRodType = valueStr[0]-'0';
		//���¹����
		CPropTreeItem *pSpecItem = pPropList->FindItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_sSpec"),NULL);
		if(pSpecItem)
		{
			if(pRayRodDlg->m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_ANGLE)
			{	//�Ǹ�
				pSpecItem->m_lpNodeInfo->m_cmbItems = MakeAngleSetString();
				pRayRodDlg->m_sSpec = "40X3";
			}
			else if(pRayRodDlg->m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_SLOT)
			{	//�۸�
				pSpecItem->m_lpNodeInfo->m_cmbItems = MakeSlotSetString();
				pRayRodDlg->m_sSpec = "5";
			}
			else if(pRayRodDlg->m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_FLAT)
			{	//����
				pSpecItem->m_lpNodeInfo->m_cmbItems = MakeFlatSetString();
				pRayRodDlg->m_sSpec = "40X3";
			}
			else if(pRayRodDlg->m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_TUBE)
			{	//�ֹ�
				pSpecItem->m_lpNodeInfo->m_cmbItems = MakeTubeSetString();
				pRayRodDlg->m_sSpec = "89X4";
			}
			if(pRayRodDlg->GetPropValueStr(pSpecItem->m_idProp,sText)>0)
				pPropList->SetItemPropValue(pSpecItem->m_idProp,sText);
		}
		//���·�����Ʋ���
		CPropTreeItem *pRoot = pPropList->GetRootItem();
		pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("workNormDesInfo"));
		//������Ʋ���
		CItemInfo *lpInfo = new CItemInfo();
		if(pRayRodDlg->m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_ANGLE)
		{
			lpInfo->m_controlType = PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Two Leg Infomation";
			lpInfo->m_strPropHelp = "Two leg's infomation";
#else 
			lpInfo->m_strPropName = "��֫������Ϣ";
			lpInfo->m_strPropHelp = "��֫������Ϣ";
#endif
		}
		else 
		{
			lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Plane Normal";
			lpInfo->m_strPropHelp = "Datum plane's normal line";
#else 
			lpInfo->m_strPropName = "��׼�淨��";
			lpInfo->m_strPropHelp = "��׼�淨��";
#endif
		}
		CPropTreeItem* pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1,TRUE);
		pGroupItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("workNormDesInfo");
		//
		if(pRayRodDlg->m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_ANGLE)
		{	//X֫���߲���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Leg Normal Direction";
			lpInfo->m_strPropHelp = "The normal direction of angle's x leg";
#else 
			lpInfo->m_strPropName = "X֫���߷���";
			lpInfo->m_strPropHelp = "�Ǹ�X֫���߷���";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_xDesNormX");
			if(pRayRodDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->SetReadOnly();
			//Y֫���߲���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Leg Normal Direction";
			lpInfo->m_strPropHelp = "The normal direction of angle's y leg";
#else 
			lpInfo->m_strPropName = "Y֫���߷���";
			lpInfo->m_strPropHelp = "�Ǹ�Y֫���߷���";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_xDesNormY");
			if(pRayRodDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->SetReadOnly();
		}
		//���ݸ˼������޸ĸ˼����췽��Ĭ��ֵ
		if(pRayRodDlg->m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_SLOT)
			pRayRodDlg->m_iEndPosType = 0;	//�۸�Ĭ���ظ˼�����
		else 
			pRayRodDlg->m_iEndPosType = 1;	//����˼�Ĭ�����������߷���
		if(pRayRodDlg->GetPropValueStr(CDesignColumnRayRodDlg::GetPropID("m_iEndPosType"),sText)>0)
			pPropList->SetItemPropValue(CDesignColumnRayRodDlg::GetPropID("m_iEndPosType"),sText);
	}
	else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_sSpec"))
		pRayRodDlg->m_sSpec.Printf("%s",valueStr);
	else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_fRayRodLen"))
		pRayRodDlg->m_fRayRodLen = atof(valueStr);
	else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_fSectorAngle"))
		pRayRodDlg->m_fSectorAngle=atof(valueStr);
	else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_nRotateArrayN"))
	{
		pRayRodDlg->m_nRotateArrayN=max(1,atoi(valueStr));
		if(pRayRodDlg->m_nRotateArrayN>1)
		{
			CPropTreeItem *pParent=NULL;
			CPropTreeItem *pFindItem=pPropList->FindItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_fSectorAngle"),&pParent);
			if(pFindItem==NULL)
			{
				CItemInfo* lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Rotate Angle";
				lpInfo->m_strPropHelp="It is two near ray rods's rotate angle when many ray rods rotate,unit is��";
#else 
				lpInfo->m_strPropName="��ת�Ƕ�";
				lpInfo->m_strPropHelp="������߸˼���ת����ʱ�ڽ��������߸˼�����ת����Ƕ�,��λΪ��";
#endif
				pParent=pPropList->FindItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_nRotateArrayN"),NULL);
				pFindItem = pPropList->InsertItem(pParent,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_fSectorAngle");
			}
			if(pRayRodDlg->GetPropValueStr(pFindItem->m_idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=sText;
		}
		else
			pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_fSectorAngle"));
	}
	else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_iEndPosType"))
	{
		pRayRodDlg->m_iEndPosType = valueStr[0]-'0';
		pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_iRodLenDefType"));
		pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_fRayRodLen"));
		CItemInfo *lpInfo=NULL;
		CPropTreeItem *pPropItem=NULL,*pParentItem=pItem->m_pParent;
		//�˼����ȶ��巽ʽ
		if(pRayRodDlg->m_iEndPosType==CDesignColumnRayRodDlg::ROD_ENDPOSTYPE_RAYVEC)
		{	
			CItemInfo *lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rod Length Define Type";
			lpInfo->m_strPropHelp = "Define type of rod's length :0.Specify rod's length 1.Specify end's ancillary ray radius";
			lpInfo->m_cmbItems="0.Specify Rod Length|1.Specify End Ancillary Ray Radius|";
#else 
			lpInfo->m_strPropName = "�˼����ȶ��巽ʽ";
			lpInfo->m_strPropHelp = "�˼����ȶ��巽ʽ:0.ָ���˼����� 1.ָ��ĩ�˸���뾶";
			lpInfo->m_cmbItems="0.ָ���˼�����|1.ָ��ĩ�˸���뾶|";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp=CDesignColumnRayRodDlg::GetPropID("m_iRodLenDefType");
			if(pRayRodDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		if(pRayRodDlg->m_iEndPosType==CDesignColumnRayRodDlg::ROD_ENDPOSTYPE_RAYVEC&&pRayRodDlg->m_iRodLenDefType==1)
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "End Ancillary Ray Radius";
		else 
			lpInfo->m_strPropName = "Rod Length";
#else 
			lpInfo->m_strPropName = "ĩ�˸���뾶";
		else 
			lpInfo->m_strPropName = "�˼�����";
#endif
		lpInfo->m_strPropHelp = lpInfo->m_strPropName;
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
		pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_fRayRodLen");
		if(pRayRodDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_iRodLenDefType"))
	{
		pRayRodDlg->m_iRodLenDefType = valueStr[0]-'0';
		//���ݸ˼�������������
		if(pRayRodDlg->m_iEndPosType==CDesignColumnRayRodDlg::ROD_ENDPOSTYPE_RAYVEC&&pRayRodDlg->m_iRodLenDefType==1)
#ifdef AFX_TARG_ENU_ENGLISH
			pPropList->SetItemPropName(CDesignColumnRayRodDlg::GetPropID("m_fRayRodLen"),CString("End Ancillary Ray Radius"));
		else 
			pPropList->SetItemPropName(CDesignColumnRayRodDlg::GetPropID("m_fRayRodLen"),CString("Rod Length"));
#else 
			pPropList->SetItemPropName(CDesignColumnRayRodDlg::GetPropID("m_fRayRodLen"),CString("ĩ�˸���뾶"));
		else 
			pPropList->SetItemPropName(CDesignColumnRayRodDlg::GetPropID("m_fRayRodLen"),CString("�˼�����"));
#endif
	}
	else 
	{
		BOOL bRet = ModifyColumnIntersPointProperty(pPropList,pItem,valueStr,&pRayRodDlg->m_xRodStartPos,TRUE);
		if(!bRet)
			ModifyColumnIntersPointProperty(pPropList,pItem,valueStr,&pRayRodDlg->m_xRodEndPos,FALSE);
	}
	return TRUE;
}

static BOOL ButtonColumnIntersPointPropClick(CPropertyList *pPropList,CPropTreeItem* pItem,CLDSPoint *pPoint,BOOL bStart)
{
	CString sPropIdPrefix = "m_xRodStartPos";
	if(!bStart)
		sPropIdPrefix = "m_xRodEndPos";
	CDesignColumnRayRodDlg *pRayRodDlg=(CDesignColumnRayRodDlg*)pPropList->GetParent();
	if(CDesignColumnRayRodDlg::GetPropID("%s.hLineTube",sPropIdPrefix)==pItem->m_idProp)
		pRayRodDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);//��׼�ֹ�
	else if(CDesignColumnRayRodDlg::GetPropID("%s.axis_vec",sPropIdPrefix)==pItem->m_idProp)
	{
		CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Axis Direction";
#else 
		vecDlg.m_sCaption = "���߷���";
#endif
		vecDlg.m_fVectorX = pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[0];
		vecDlg.m_fVectorY = pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[1];
		vecDlg.m_fVectorZ = pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[2];
		if(vecDlg.DoModal()==IDOK)
		{
			pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[0] = vecDlg.m_fVectorX;
			pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[1] = vecDlg.m_fVectorY;
			pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[2] = vecDlg.m_fVectorZ;
		}
	}
	else if(CDesignColumnRayRodDlg::GetPropID("%s.sect_center",sPropIdPrefix)==pItem->m_idProp)
	{
		CReportVectorDlg vecDlg;
		CLDSLineTube *pLineTube = (CLDSLineTube *)console.FromPartHandle(pPoint->des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
		if(pLineTube)
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Section Pick Point";
		else 
			vecDlg.m_sCaption = "Section Center";
#else 
			vecDlg.m_sCaption = "����ʰȡ��";
		else 
			vecDlg.m_sCaption = "����Բ��";
#endif
		vecDlg.m_fVectorX = pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[0];
		vecDlg.m_fVectorY = pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[1];
		vecDlg.m_fVectorZ = pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[2];
		if(vecDlg.DoModal()==IDOK)
		{
			pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[0] = vecDlg.m_fVectorX;
			pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[1] = vecDlg.m_fVectorY;
			pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[2] = vecDlg.m_fVectorZ;
		}
	}
	else if(CDesignColumnRayRodDlg::GetPropID("%s.sect_norm",sPropIdPrefix)==pItem->m_idProp)
	{
		CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Section Normal";
#else 
		vecDlg.m_sCaption = "���淨��";
#endif
		vecDlg.m_fVectorX = pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[0];
		vecDlg.m_fVectorY = pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[1];
		vecDlg.m_fVectorZ = pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[2];
		if(vecDlg.DoModal()==IDOK)
		{
			pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[0] = vecDlg.m_fVectorX;
			pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[1] = vecDlg.m_fVectorY;
			pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[2] = vecDlg.m_fVectorZ;
		}
	}
	else 
		return FALSE;
	return TRUE;
}
static BOOL ButtonRayRodDesignParaPlate(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CDesignColumnRayRodDlg *pRayRodDlg=(CDesignColumnRayRodDlg*)pPropList->GetParent();
	pRayRodDlg->SetEventPropId(0);		//��ʼ��������id��¼
	pRayRodDlg->m_idParentEventProp=0;	//��ʼ����������id��¼
	BOOL bRetCode = ButtonColumnIntersPointPropClick(pPropList,pItem,&pRayRodDlg->m_xRodStartPos,TRUE);
	if(bRetCode)
		bRetCode = ButtonColumnIntersPointPropClick(pPropList,pItem,&pRayRodDlg->m_xRodEndPos,FALSE);
	return bRetCode;
}
//</DEVELOP_PROCESS_MARK>

//<DEVELOP_PROCESS_MARK nameId="CDesignColumnRayRodDlg::FireSubObjPropListFunc">
static void UpdateWorkPlaneNormPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,BOOL bUpdate);
static BOOL ModifySubObjPropertyValue(CPropertyList *pPropList,CPropTreeItem *pItem,CString &valueStr)
{
	CDesignColumnRayRodDlg *pRayRodDlg=(CDesignColumnRayRodDlg*)pPropList->GetParent();
	if(pRayRodDlg==NULL)
		return FALSE;
	CPropTreeItem  *pParentEventItem = pRayRodDlg->m_xPropList.FindItemByPropId(pRayRodDlg->m_idParentEventProp,NULL);
	if(pParentEventItem==NULL)
		return FALSE;
	//����m_idParentEventProp��Ӧ������
	if(CDesignColumnRayRodDlg::GetPropID("m_xRodStartPos.ray_vec_style")==pParentEventItem->m_idProp||
		CDesignColumnRayRodDlg::GetPropID("m_xRodStartPos.sect_norm")==pParentEventItem->m_idProp||
		CDesignColumnRayRodDlg::GetPropID("m_xRodStartPos.axis_vec")==pParentEventItem->m_idProp||
		CDesignColumnRayRodDlg::GetPropID("m_xRodEndPos.ray_vec_style")==pParentEventItem->m_idProp||
		CDesignColumnRayRodDlg::GetPropID("m_xRodEndPos.sect_norm")==pParentEventItem->m_idProp||
		CDesignColumnRayRodDlg::GetPropID("m_xRodEndPos.axis_vec")==pParentEventItem->m_idProp)
		CDisplayVectorProperty::ModifyDatumVectorPropValue(pPropList,pItem,valueStr);
	else if(CDesignColumnRayRodDlg::GetPropID("m_xRodStartPos.sect_center")==pParentEventItem->m_idProp||
		CDesignColumnRayRodDlg::GetPropID("m_xRodEndPos.sect_center")==pParentEventItem->m_idProp)
		CDisplayPointProperty::ModifyDatumPointPropValue(pPropList,pItem,valueStr);
	else if(CDesignColumnRayRodDlg::GetPropID("workNormDesInfo")==pParentEventItem->m_idProp&&
		pRayRodDlg->m_iRodType!=CDesignColumnRayRodDlg::ROD_TYPE_ANGLE)
	{	//�ֹܡ��۸֡����������淨�߲���
		if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_iNormCalStyle"))
		{
			pRayRodDlg->m_iNormCalStyle = valueStr[0]-'0';

			pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_xWorkPlaneNorm.x"));
			pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_xWorkPlaneNorm.y"));
			pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_xWorkPlaneNorm.z"));
			pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_hWorkNormRefPart"));
			pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_xNearWorkPlaneNorm"));
			pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_xNearWorkPlaneNorm.x"));
			pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_xNearWorkPlaneNorm.y"));
			pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_xNearWorkPlaneNorm.z"));
			UpdateWorkPlaneNormPropItem(pPropList,pItem->m_pParent,TRUE);
		}
		else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_xNearWorkPlaneNorm.x"))
			pRayRodDlg->m_xNearWorkPlaneNorm.x = atof(valueStr);
		else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_xNearWorkPlaneNorm.y"))
			pRayRodDlg->m_xNearWorkPlaneNorm.y = atof(valueStr);
		else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_xNearWorkPlaneNorm.z"))
			pRayRodDlg->m_xNearWorkPlaneNorm.z = atof(valueStr);
		else if(pItem->m_idProp==CDesignColumnRayRodDlg::GetPropID("m_hWorkNormRefPart"))
			sscanf(valueStr.GetBuffer(1),"%X",&(pRayRodDlg->m_hWorkNormRefPart));
	}
	else if(CDesignColumnRayRodDlg::GetPropID("m_xDesNormX")==pParentEventItem->m_idProp)
	{
		CDisplayVectorProperty::ModifyDatumVectorPropValue(pPropList,pItem,valueStr);
		if(pRayRodDlg->m_xDesVectorX.norm_style==0)
		{	//ָ������
			pRayRodDlg->m_xDesNormX.bSpecific=TRUE;
			pRayRodDlg->m_xDesNormX.spec_norm=pRayRodDlg->m_xDesVectorX;
			pRayRodDlg->m_xNormX = pRayRodDlg->m_xDesVectorX.vector;
		}
		else if(pRayRodDlg->m_xDesVectorX.norm_style==6)//�ɽǸ���һ֫ȷ������
		{
			pRayRodDlg->m_xDesNormX.bSpecific=FALSE;
			pRayRodDlg->m_xDesNormX.bByOtherWing=TRUE;
		}
		else if(pRayRodDlg->m_xDesVectorX.norm_style==2)//�ɲ��սǸֽ�����ȷ��
		{
			pRayRodDlg->m_xDesNormX.bSpecific=FALSE;
			pRayRodDlg->m_xDesNormX.hViceJg=pRayRodDlg->m_xDesVectorX.hCrossPart;
			pRayRodDlg->m_xDesNormX.near_norm=pRayRodDlg->m_xDesVectorX.nearVector;
		}
		else
		{
			pRayRodDlg->m_xDesNormX.bSpecific=TRUE;
			pRayRodDlg->m_xDesNormX.spec_norm=pRayRodDlg->m_xDesVectorX;
		}
	}
	else if(CDesignColumnRayRodDlg::GetPropID("m_xDesNormY")==pParentEventItem->m_idProp)
	{
		CDisplayVectorProperty::ModifyDatumVectorPropValue(pPropList,pItem,valueStr);
		if(pRayRodDlg->m_xDesVectorY.norm_style==0)
		{	//ָ������
			pRayRodDlg->m_xDesNormY.bSpecific=TRUE;
			pRayRodDlg->m_xDesNormY.spec_norm=pRayRodDlg->m_xDesVectorY;
			pRayRodDlg->m_xNormY = pRayRodDlg->m_xDesVectorY.vector;
		}
		else if(pRayRodDlg->m_xDesVectorY.norm_style==6)//�ɽǸ���һ֫ȷ������
		{
			pRayRodDlg->m_xDesNormY.bSpecific=FALSE;
			pRayRodDlg->m_xDesNormY.bByOtherWing=TRUE;
		}
		else if(pRayRodDlg->m_xDesVectorX.norm_style==2)//�ɲ��սǸֽ�����ȷ��
		{
			pRayRodDlg->m_xDesNormY.bSpecific=FALSE;
			pRayRodDlg->m_xDesNormY.hViceJg=pRayRodDlg->m_xDesVectorX.hCrossPart;
			pRayRodDlg->m_xDesNormY.near_norm=pRayRodDlg->m_xDesVectorX.nearVector;
		}
		else
		{
			pRayRodDlg->m_xDesNormX.bSpecific=TRUE;
			pRayRodDlg->m_xDesNormX.spec_norm=pRayRodDlg->m_xDesVectorX;
		}
	}
	else 
		return FALSE;
	//�������������е�����ֵ
	pRayRodDlg->UpdateParentEventPropItem();
	//����������������ʾ
	CXhChar100 sText;
	if(pRayRodDlg->GetPropValueStr(pRayRodDlg->m_idParentEventProp,sText)>0)
		pRayRodDlg->m_xPropList.SetItemPropValue(pRayRodDlg->m_idParentEventProp,sText);
	return TRUE;
}

static BOOL ButtonSubObjPropertyClick(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CDesignColumnRayRodDlg *pRayRodDlg=(CDesignColumnRayRodDlg*)pPropList->GetParent();
	if(pRayRodDlg==NULL)
		return FALSE;
	CPropTreeItem  *pParentEventItem = pRayRodDlg->m_xPropList.FindItemByPropId(pRayRodDlg->m_idParentEventProp,NULL);
	if(pParentEventItem==NULL)
		return FALSE;
	if(CDesignColumnRayRodDlg::GetPropID("m_xRodStartPos.ray_vec_style")==pParentEventItem->m_idProp||
		CDesignColumnRayRodDlg::GetPropID("m_xRodStartPos.sect_norm")==pParentEventItem->m_idProp||
		CDesignColumnRayRodDlg::GetPropID("m_xRodStartPos.axis_vec")==pParentEventItem->m_idProp||
		CDesignColumnRayRodDlg::GetPropID("m_xRodEndPos.ray_vec_style")==pParentEventItem->m_idProp||
		CDesignColumnRayRodDlg::GetPropID("m_xRodEndPos.sect_norm")==pParentEventItem->m_idProp||
		CDesignColumnRayRodDlg::GetPropID("m_xRodEndPos.axis_vec")==pParentEventItem->m_idProp)
		CDisplayVectorProperty::VectorButtonClick(pPropList,pItem);
	else if(CDesignColumnRayRodDlg::GetPropID("m_xRodStartPos.sect_center")==pParentEventItem->m_idProp||
		CDesignColumnRayRodDlg::GetPropID("m_xRodEndPos.sect_center")==pParentEventItem->m_idProp)
		CDisplayPointProperty::DatumPointButtonClick(pPropList,pItem);
	else if(CDesignColumnRayRodDlg::GetPropID("workNormDesInfo")==pParentEventItem->m_idProp&&
		pRayRodDlg->m_iRodType!=CDesignColumnRayRodDlg::ROD_TYPE_ANGLE)
	{	//�ֹܡ��۸֡����������淨�߲���
		pRayRodDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);
	}
	else if(CDesignColumnRayRodDlg::GetPropID("m_xDesNormX")==pParentEventItem->m_idProp)
	{
		CDisplayVectorProperty::VectorButtonClick(pPropList,pItem);
		if(pRayRodDlg->m_xDesVectorX.norm_style==0)
		{	//ָ������
			pRayRodDlg->m_xDesNormX.bSpecific=TRUE;
			pRayRodDlg->m_xDesNormX.spec_norm=pRayRodDlg->m_xDesVectorX;
			pRayRodDlg->m_xNormX = pRayRodDlg->m_xDesVectorX.vector;
		}
		else if(pRayRodDlg->m_xDesVectorX.norm_style==6)//�ɽǸ���һ֫ȷ������
		{
			pRayRodDlg->m_xDesNormX.bSpecific=FALSE;
			pRayRodDlg->m_xDesNormX.bByOtherWing=TRUE;
		}
		else if(pRayRodDlg->m_xDesVectorX.norm_style==2)//�ɲ��սǸֽ�����ȷ��
		{
			pRayRodDlg->m_xDesNormX.bSpecific=FALSE;
			pRayRodDlg->m_xDesNormX.hViceJg=pRayRodDlg->m_xDesVectorX.hCrossPart;
			pRayRodDlg->m_xDesNormX.near_norm=pRayRodDlg->m_xDesVectorX.nearVector;
		}
		else
		{
			pRayRodDlg->m_xDesNormX.bSpecific=TRUE;
			pRayRodDlg->m_xDesNormX.spec_norm=pRayRodDlg->m_xDesVectorX;
		}
	}
	else if(CDesignColumnRayRodDlg::GetPropID("m_xDesNormY")==pParentEventItem->m_idProp)
	{
		CDisplayVectorProperty::VectorButtonClick(pPropList,pItem);
		if(pRayRodDlg->m_xDesVectorY.norm_style==0)
		{	//ָ������
			pRayRodDlg->m_xDesNormY.bSpecific=TRUE;
			pRayRodDlg->m_xDesNormY.spec_norm=pRayRodDlg->m_xDesVectorY;
			pRayRodDlg->m_xNormY = pRayRodDlg->m_xDesVectorY.vector;
		}
		else if(pRayRodDlg->m_xDesVectorY.norm_style==6)//�ɽǸ���һ֫ȷ������
		{
			pRayRodDlg->m_xDesNormY.bSpecific=FALSE;
			pRayRodDlg->m_xDesNormY.bByOtherWing=TRUE;
		}
		else if(pRayRodDlg->m_xDesVectorX.norm_style==2)//�ɲ��սǸֽ�����ȷ��
		{
			pRayRodDlg->m_xDesNormY.bSpecific=FALSE;
			pRayRodDlg->m_xDesNormY.hViceJg=pRayRodDlg->m_xDesVectorX.hCrossPart;
			pRayRodDlg->m_xDesNormY.near_norm=pRayRodDlg->m_xDesVectorX.nearVector;
		}
		else
		{
			pRayRodDlg->m_xDesNormX.bSpecific=TRUE;
			pRayRodDlg->m_xDesNormX.spec_norm=pRayRodDlg->m_xDesVectorX;
		}
	}
	return TRUE;
}
//</DEVELOP_PROCESS_MARK>

// CDesignColumnRayRodDlg �Ի���
int CDesignColumnRayRodDlg::nScrLocationX=0;
int CDesignColumnRayRodDlg::nScrLocationY=0;
IMPLEMENT_DYNAMIC(CDesignColumnRayRodDlg, CDialog)
CDesignColumnRayRodDlg::CDesignColumnRayRodDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignColumnRayRodDlg::IDD, pParent)
	, m_sSubObjName(_T(""))
{
	m_idParentEventProp = 0;
	m_cMaterial = CSteelMatLibrary::Q235BriefMark();
	m_iRodType = 0;
	m_iEndPosType = ROD_ENDPOSTYPE_RAYVEC;
	m_iRodLenDefType = 0;
	m_fRayRodLen = 150;
	m_iNormCalStyle = 0;
	m_hWorkNormRefPart = 0;
	m_pBaseLineTube = NULL;
	m_nRotateArrayN=1;
	m_fSectorAngle=90;	//��

	m_xDesNormX.hViceJg = 0;
	m_xDesNormX.bSpecific = FALSE;
	m_xDesNormX.bByOtherWing = FALSE;
	m_xDesNormY.hViceJg = 0;
	m_xDesNormY.bSpecific = FALSE;
	m_xDesNormY.bByOtherWing = TRUE;
	m_bEnableWindowMoveListen=FALSE;
}

CDesignColumnRayRodDlg::~CDesignColumnRayRodDlg()
{
}

void CDesignColumnRayRodDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_xPropList);
	DDX_Control(pDX, IDC_LIST_BOX_SUB, m_xSubPropList);
	DDX_Text(pDX, IDC_S_SUBOBJ_NAME, m_sSubObjName);
}


BEGIN_MESSAGE_MAP(CDesignColumnRayRodDlg, CDialog)
	ON_WM_MOVE()
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_BTN_CONFIRM, OnBnClickedBtnConfirm)
END_MESSAGE_MAP()

const DWORD HASHTABLESIZE = 500;
CStringKeyHashTable<long> CDesignColumnRayRodDlg::propHashtable;
void CDesignColumnRayRodDlg::InitPropHashtable()
{
	int id = 1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	propHashtable.SetValueAt("basicInfo",id++);
	propHashtable.SetValueAt("m_sPartNo",id++);
	propHashtable.SetValueAt("m_xSegI",id++);
	propHashtable.SetValueAt("m_cMaterial",id++);
	propHashtable.SetValueAt("m_iRodType",id++);
	propHashtable.SetValueAt("m_sSpec",id++);
	//ʼ��λ��
	propHashtable.SetValueAt("m_xRodStartPos",id++);
	propHashtable.SetValueAt("m_xRodStartPos.column",id++);
	propHashtable.SetValueAt("m_xRodStartPos.column_r",id++);		//����뾶
	propHashtable.SetValueAt("m_xRodStartPos.hLineTube",id++);		//�ֹܾ��
	propHashtable.SetValueAt("m_xRodStartPos.axis_vec",id++);		//��������������
	propHashtable.SetValueAt("m_xRodStartPos.sect_center",id++);	//����Բ�ĵ�
	propHashtable.SetValueAt("m_xRodStartPos.sect_norm",id++);		//���淨��NULL��ʾ����Ĵ�ֱ����
	propHashtable.SetValueAt("m_xRodStartPos.ray_vec_style",id++);	//���߷����巽ʽ
	propHashtable.SetValueAt("m_xRodStartPos.ray_vec",id++);		//���߷���	
	propHashtable.SetValueAt("m_xRodStartPos.ray_angle",id++);		//��һ�����߸˼�����ʼ��ת�Ƕ�
	propHashtable.SetValueAt("m_xRodStartPos.sect_angle",id++);		//�����߸˼���ת����Ƕ�
	propHashtable.SetValueAt("m_xRodStartPos.ray_para_offset",id++);//����ƽ���ƶ�����
	propHashtable.SetValueAt("m_xRodStartPos.bSpecRo",id++);		//�����뾶���巽ʽ
	propHashtable.SetValueAt("m_xRodStartPos.ray_offset_r",id++);	//�����뾶
	propHashtable.SetValueAt("m_xRodStartPos",id++);
	//�ն�λ��
	propHashtable.SetValueAt("m_iEndPosType",id++);
	propHashtable.SetValueAt("m_iRodLenDefType",id++);
	propHashtable.SetValueAt("m_fRayRodLen",id++);
	propHashtable.SetValueAt("m_xRodEndPos",id++);
	propHashtable.SetValueAt("m_xRodEndPos.column",id++);
	propHashtable.SetValueAt("m_xRodEndPos.column_r",id++);			//����뾶
	propHashtable.SetValueAt("m_xRodEndPos.hLineTube",id++);		//�ֹܾ��
	propHashtable.SetValueAt("m_xRodEndPos.axis_vec",id++);			//��������������
	propHashtable.SetValueAt("m_xRodEndPos.sect_center",id++);		//����Բ�ĵ�
	propHashtable.SetValueAt("m_xRodEndPos.sect_norm",id++);		//���淨��NULL��ʾ����Ĵ�ֱ����
	propHashtable.SetValueAt("m_xRodEndPos.ray_vec_style",id++);	//���߷����巽ʽ
	propHashtable.SetValueAt("m_xRodEndPos.ray_vec",id++);			//���߷���	
	propHashtable.SetValueAt("m_xRodEndPos.ray_angle",id++);		//��һ�����߸˼�����ʼ��ת�Ƕ�
	propHashtable.SetValueAt("m_fSectorAngle",id++);				//�����߸˼���ת����Ƕ�
	propHashtable.SetValueAt("m_nRotateArrayN",id++);				//��������
	propHashtable.SetValueAt("m_xRodEndPos.ray_para_offset",id++);	//����ƽ���ƶ�����
	propHashtable.SetValueAt("m_xRodEndPos.bSpecRo",id++);			//�����뾶���巽ʽ
	propHashtable.SetValueAt("m_xRodEndPos.ray_offset_r",id++);		//�����뾶
	//
	propHashtable.SetValueAt("workNormDesInfo",id++);
	//�ֹܡ��۸֡����������淨�߲���
	propHashtable.SetValueAt("m_xWorkPlaneNorm",id++);
	propHashtable.SetValueAt("m_xWorkPlaneNorm.x",id++);
	propHashtable.SetValueAt("m_xWorkPlaneNorm.y",id++);
	propHashtable.SetValueAt("m_xWorkPlaneNorm.z",id++);
	propHashtable.SetValueAt("m_iNormCalStyle",id++);
	propHashtable.SetValueAt("m_hWorkNormRefPart",id++);
	propHashtable.SetValueAt("m_xNearWorkPlaneNorm",id++);
	propHashtable.SetValueAt("m_xNearWorkPlaneNorm.x",id++);
	propHashtable.SetValueAt("m_xNearWorkPlaneNorm.y",id++);
	propHashtable.SetValueAt("m_xNearWorkPlaneNorm.z",id++);
	//�Ǹֹ����淨�߲���
	propHashtable.SetValueAt("m_xDesNormX",id++);
	propHashtable.SetValueAt("m_xDesNormY",id++);
}
long CDesignColumnRayRodDlg::GetPropID(char *sProp,...)
{	//�õ�����ID
	va_list ap;
	va_start(ap, sProp);
	CString sRealProp;
	sRealProp.FormatV(sProp,ap);
	va_end(ap);
	long id;
	if(propHashtable.GetValueAt(sRealProp.GetBuffer(1),id))
		return id;
	else
		return 0;
}

int CDesignColumnRayRodDlg::GetPropValueStr(long id, char *valueStr)
{	//��������ID�õ�����ֵ
	CXhChar500 sText;
	if(GetPropID("m_sPartNo")==id)
		sText.Copy(m_sPartNo);
	else if(GetPropID("m_xSegI")==id)
		sText.Copy(m_xSegI.ToString());
	else if(GetPropID("m_cMaterial")==id)
	{
		for(int i=0;i<CSteelMatLibrary::GetCount();i++)
		{
			STEELMAT mat=CSteelMatLibrary::RecordAt(i);
			if(m_cMaterial==mat.cBriefMark)
			{
				sText.Copy(mat.mark);
				break;
			}
		}
	}
	else if(GetPropID("m_iRodType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_ANGLE)
			sText.Copy("0.Angle");
		else if(m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_SLOT)
			sText.Copy("1.Slot Angle");
		else if(m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_FLAT)
			sText.Copy("2.Flat Iron");
		else if(m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_TUBE)
			sText.Copy("3.Tube");
#else 
		if(m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_ANGLE)
			sText.Copy("0.�Ǹ�");
		else if(m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_SLOT)
			sText.Copy("1.�۸�");
		else if(m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_FLAT)
			sText.Copy("2.����");
		else if(m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_TUBE)
			sText.Copy("3.�ֹ�");
#endif
	}
	else if(GetPropID("m_sSpec")==id)
		sText.Printf("%s",(char*)m_sSpec);
	//ʼ��
	else if(GetPropID("m_xRodStartPos.hLineTube")==id)		//�ֹܾ��
		sprintf(sText,"0X%X",m_xRodStartPos.des_para.COLUMN_INTERS.hLineTube);
	else if(GetPropID("m_xRodStartPos.axis_vec")==id)
	{
		f3dPoint axis_vec(m_xRodStartPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[0],m_xRodStartPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[1],
			m_xRodStartPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[2]);
		sText.ConvertFromPoint(axis_vec);
	}
	else if(GetPropID("m_xRodStartPos.sect_center")==id)
	{
		f3dPoint center(m_xRodStartPos.des_para.COLUMN_INTERS.SectCenter.sect_center[0],m_xRodStartPos.des_para.COLUMN_INTERS.SectCenter.sect_center[1],
			m_xRodStartPos.des_para.COLUMN_INTERS.SectCenter.sect_center[2]);
		sText.ConvertFromPoint(center);
	}
	else if(GetPropID("m_xRodStartPos.sect_norm")==id)
	{
		f3dPoint sect_norm(m_xRodStartPos.des_para.COLUMN_INTERS.SectNorm.sect_norm[0],m_xRodStartPos.des_para.COLUMN_INTERS.SectNorm.sect_norm[1],
			m_xRodStartPos.des_para.COLUMN_INTERS.SectNorm.sect_norm[2]);
		sText.ConvertFromPoint(sect_norm);
	}
	else if(GetPropID("m_xRodStartPos.ray_vec_style")==id)	//���߷����巽ʽ
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style==0)
			strcpy(sText,"0.Specify Directly");
		else if(m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style==1)
			strcpy(sText,"1.Specify Datum Edge");
		else if(m_xRodStartPos.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"2.Base On Tube Edge");
		else if(m_xRodStartPos.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"3.Base On Weld Line");
#else 
		if(m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style==0)
			strcpy(sText,"0.ֱ��ָ��");
		else if(m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style==1)
			strcpy(sText,"1.ָ����׼��");
		else if(m_xRodStartPos.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"2.�Ըֹܻ�׼��Ϊ��׼");
		else if(m_xRodStartPos.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"3.�Ըֹܺ�����Ϊ��׼");
#endif
	}
	else if(GetPropID("m_xRodStartPos.ray_angle")==id)		//��ת�Ƕ�
	{
		sprintf(sText,"%f",m_xRodStartPos.des_para.COLUMN_INTERS.ray_angle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_nRotateArrayN")==id)		//��������
	{
		sprintf(sText,"%d",m_nRotateArrayN);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fSectorAngle")==id)		//��ת�Ƕ�
		sprintf(sText,"%s",(char*)CXhChar16(m_fSectorAngle));
	else if(GetPropID("m_xRodStartPos.column_r")==id)		//����뾶
	{
		sprintf(sText,"%f",m_xRodStartPos.des_para.COLUMN_INTERS.column_r);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xRodStartPos.ray_para_offset")==id)//����ƽ���ƶ�����
	{
		sprintf(sText,"%f",m_xRodStartPos.des_para.COLUMN_INTERS.ray_para_offset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xRodStartPos.bSpecRo")==id)		//�����뾶���巽ʽ
	{
		if(!m_xRodStartPos.des_para.COLUMN_INTERS.bSpecRo)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Ray Line & Column Intersection Point");
		else
			strcpy(sText,"1.Column Center");
#else 
			strcpy(sText,"0.���������潻��");
		else
			strcpy(sText,"1.Բ������");
#endif
	}
	else if(GetPropID("m_xRodStartPos.ray_offset_r")==id)	//�����뾶
	{
		sprintf(sText,"%f",m_xRodStartPos.des_para.COLUMN_INTERS.ray_offset_r);
		SimplifiedNumString(sText);
	}
	//�ն�
	else if(GetPropID("m_iEndPosType")==id)
	{
		if(m_iEndPosType==ROD_ENDPOSTYPE_RAYVEC)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Along Column Ray Line");
		else //if(m_iEndPosType==ROD_ENDPOSTYPE_AXISVEC)
			sText.Copy("0.Along Column Axis");
#else 
			sText.Copy("1.����������");
		else //if(m_iEndPosType==ROD_ENDPOSTYPE_AXISVEC)
			sText.Copy("0.����������");
#endif
		//else if(m_iEndPosType==ROD_ENDPOSTYPE_USERDEF)
		//	sText.Copy("2.�Զ���");
	}
	else if(GetPropID("m_iRodLenDefType")==id)
	{
		if(m_iRodLenDefType==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Specify End Ancillary Ray Radius");
		else //if(m_iRodLenDefType==0)
			sText.Copy("0.Specify Rod Length");
#else 
			sText.Copy("1.ָ��ĩ�˸���뾶");
		else //if(m_iRodLenDefType==0)
			sText.Copy("0.ָ���˼�����");
#endif
	}
	else if(GetPropID("m_fRayRodLen")==id)
	{
		sText.Printf("%f",m_fRayRodLen);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xRodEndPos.hLineTube")==id)		//�ֹܾ��
		sprintf(sText,"0X%X",m_xRodEndPos.des_para.COLUMN_INTERS.hLineTube);
	else if(GetPropID("m_xRodEndPos.axis_vec")==id)
	{
		f3dPoint axis_vec(m_xRodEndPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[0],m_xRodEndPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[1],
			m_xRodEndPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[2]);
		sText.ConvertFromPoint(axis_vec);
	}
	else if(GetPropID("m_xRodEndPos.sect_center")==id)
	{
		f3dPoint center(m_xRodEndPos.des_para.COLUMN_INTERS.SectCenter.sect_center[0],m_xRodEndPos.des_para.COLUMN_INTERS.SectCenter.sect_center[1],
			m_xRodEndPos.des_para.COLUMN_INTERS.SectCenter.sect_center[2]);
		sText.ConvertFromPoint(center);
	}
	else if(GetPropID("m_xRodEndPos.sect_norm")==id)
	{
		f3dPoint sect_norm(m_xRodEndPos.des_para.COLUMN_INTERS.SectNorm.sect_norm[0],m_xRodEndPos.des_para.COLUMN_INTERS.SectNorm.sect_norm[1],
			m_xRodEndPos.des_para.COLUMN_INTERS.SectNorm.sect_norm[2]);
		sText.ConvertFromPoint(sect_norm);
	}
	else if(GetPropID("m_xRodEndPos.ray_vec_style")==id)	//���߷����巽ʽ
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style==0)
			strcpy(sText,"0.Specify Directly");
		else if(m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style==1)
			strcpy(sText,"1.Specify Datum Edge");
		else if(m_xRodEndPos.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"2.Base On Tube Edge");
		else if(m_xRodEndPos.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"3.Base On Weld Line");
#else 
		if(m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style==0)
			strcpy(sText,"0.ֱ��ָ��");
		else if(m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style==1)
			strcpy(sText,"1.ָ����׼��");
		else if(m_xRodEndPos.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"2.�Ըֹܻ�׼��Ϊ��׼");
		else if(m_xRodEndPos.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"3.�Ըֹܺ�����Ϊ��׼");
#endif
	}
	else if(GetPropID("m_xRodEndPos.ray_angle")==id)		//��ת�Ƕ�
	{
		sprintf(sText,"%f",m_xRodEndPos.des_para.COLUMN_INTERS.ray_angle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xRodEndPos.column_r")==id)		//����뾶
	{
		sprintf(sText,"%f",m_xRodEndPos.des_para.COLUMN_INTERS.column_r);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xRodEndPos.ray_para_offset")==id)//����ƽ���ƶ�����
	{
		sprintf(sText,"%f",m_xRodEndPos.des_para.COLUMN_INTERS.ray_para_offset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xRodEndPos.bSpecRo")==id)		//�����뾶���巽ʽ
	{
		if(!m_xRodEndPos.des_para.COLUMN_INTERS.bSpecRo)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Ray Line & Column Intersection Point");
		else
			strcpy(sText,"1.Column Center");
#else 
			strcpy(sText,"0.���������潻��");
		else
			strcpy(sText,"1.Բ������");
#endif
	}
	else if(GetPropID("m_xRodEndPos.ray_offset_r")==id)	//�����뾶
	{
		sprintf(sText,"%f",m_xRodEndPos.des_para.COLUMN_INTERS.ray_offset_r);
		SimplifiedNumString(sText);
	}
	//X֫���߲���
	else if(GetPropID("m_xDesNormX")==id)
	{
		if(m_xDesNormX.hViceJg>0x20)	//֫���߷���Ϊ���˼������淨��
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sText,499,"With 0X%X Coplanar",m_xDesNormX.hViceJg);
		else if(m_xDesNormX.bByOtherWing&&!m_xDesNormX.bSpecific)
			strcpy(sText,"Vertical To Another Leg");
		else 
			strcpy(sText,"Specify Direction");
#else 
			_snprintf(sText,499,"��0X%X����",m_xDesNormX.hViceJg);
		else if(m_xDesNormX.bByOtherWing&&!m_xDesNormX.bSpecific)
			strcpy(sText,"��ֱ����һ֫");
		else 
			strcpy(sText,"ָ������");
#endif
	}
	//Y֫���߲���
	else if(GetPropID("m_xDesNormY")==id)
	{
		if(m_xDesNormY.hViceJg>0x20)	//֫���߷���Ϊ���˼������淨��
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sText,499,"With 0X%X Coplanar",m_xDesNormY.hViceJg);
		else if(m_xDesNormY.bByOtherWing&&!m_xDesNormY.bSpecific)
			strcpy(sText,"Vertical To Another Leg");
		else 
			strcpy(sText,"Specify Direction");
#else 
			_snprintf(sText,499,"��0X%X����",m_xDesNormY.hViceJg);
		else if(m_xDesNormY.bByOtherWing&&!m_xDesNormY.bSpecific)
			strcpy(sText,"��ֱ����һ֫");
		else 
			strcpy(sText,"ָ������");
#endif
	}
	else if(GetPropID("m_iNormCalStyle")==id)
	{
		if(m_iNormCalStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Specify Directly");
		else
			strcpy(sText,"1.Reference Rod");
#else 
			strcpy(sText,"0.ֱ��ָ��");
		else
			strcpy(sText,"1.���ո˼�");
#endif
	}
	else if(GetPropID("m_hWorkNormRefPart")==id)
		sprintf(sText,"0X%X",m_hWorkNormRefPart);
	else if(GetPropID("m_xWorkPlaneNorm.x")==id)
	{
		sprintf(sText,"%f",m_xWorkPlaneNorm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xWorkPlaneNorm.y")==id)
	{
		sprintf(sText,"%f",m_xWorkPlaneNorm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xWorkPlaneNorm.z")==id)
	{
		sprintf(sText,"%f",m_xWorkPlaneNorm.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xNearWorkPlaneNorm.x")==id)
	{
		sprintf(sText,"%f",m_xNearWorkPlaneNorm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xNearWorkPlaneNorm.y")==id)
	{
		sprintf(sText,"%f",m_xNearWorkPlaneNorm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xNearWorkPlaneNorm.z")==id)
	{
		sprintf(sText,"%f",m_xNearWorkPlaneNorm.z);
		SimplifiedNumString(sText);
	}
	else 
		sText.Copy("");
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}

// CDesignColumnRayRodDlg ��Ϣ�������
BOOL CDesignColumnRayRodDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_xPropList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xPropList.SetDividerScale(0.5);
	m_xPropList.SetModifyValueFunc(ModifyRayRodDesignParaProperty);
	m_xPropList.SetButtonClickFunc(ButtonRayRodDesignParaPlate);
	m_xPropList.SetPropHelpPromptFunc(FireRayRodDesignParaHelpStr);
	//
	m_xSubPropList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xSubPropList.SetModifyValueFunc(ModifySubObjPropertyValue);
	m_xSubPropList.SetButtonClickFunc(ButtonSubObjPropertyClick);
	/*CLDSLineTube* pTube=Ta.Parts.FromHandle(m_xRodStartPos.des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
	if(pTube==NULL)
		m_xRodStartPos.des_para.COLUMN_INTERS.hLineTube=0;*/
	//��ʼ��id��ϣ��
	InitPropHashtable();
	if(m_bInernalStart)	//���ѡ�����ĺ�������
		FinishSelectObjOper();
	else				//��ʼ�����߸˼���Ʋ���
		InitRayRodDesginPara();
	DisplayRayRodPropList();
	if(m_bInernalStart)	
	{	//�ָ�ѡ����
		CPropTreeItem *pItem=m_xPropList.FindItemByPropId(m_idParentEventProp,NULL);
		m_xPropList.SetFocus();	//��������ý���
		if(pItem)
			m_xPropList.SetCurSel(pItem->m_iIndex);	//����ѡ����
		DisplaySubObjPropList(pItem,FALSE);			//��ʾ������
		CPropTreeItem *pSubItem=m_xSubPropList.FindItemByPropId(m_idEventProp,NULL);
		m_xSubPropList.SetFocus();
		if(pSubItem)	//ѡ��������
			m_xSubPropList.SetCurSel(pSubItem->m_iIndex);
	}
	else 
	{	//Ĭ��ѡ�н���Բ����
		CPropTreeItem *pItem=m_xPropList.FindItemByPropId(GetPropID("m_xRodStartPos.sect_center"),NULL);
		DisplaySubObjPropList(pItem);
	}
	//�ƶ��Ի������Ͻ�
	CRect rect;
	GetWindowRect(rect);
	int width=rect.Width();
	int height=rect.Height();
	rect.left = nScrLocationX;
	rect.top = nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect,TRUE);
	m_bEnableWindowMoveListen=TRUE;
	return TRUE;
}

void CDesignColumnRayRodDlg::InitRayRodDesginPara()
{
	m_sPartNo = "";
	m_sSpec = "40X3";
	if(m_pBaseLineTube)
		m_xSegI = m_pBaseLineTube->iSeg;
	m_cMaterial = CSteelMatLibrary::Q235BriefMark();
	//��ʼ����׼��
	m_xRodStartPos.datum_pos_style = 11;
	m_xRodStartPos.des_para.COLUMN_INTERS.hLineTube = 0;
	if(m_pBaseLineTube)
	{
		m_xRodStartPos.des_para.COLUMN_INTERS.hLineTube = m_pBaseLineTube->handle;
		f3dPoint axis_vec=m_pBaseLineTube->End()-m_pBaseLineTube->Start();
		normalize(axis_vec);
		m_xRodStartPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[0]=axis_vec.x;
		m_xRodStartPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[1]=axis_vec.y;
		m_xRodStartPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[2]=axis_vec.z;
		m_xRodStartPos.des_para.COLUMN_INTERS.column_r=m_pBaseLineTube->GetWidth()*0.5;
		m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style=2;	//�Ըֹܻ�׼��Ϊ��׼
	}
	m_xRodEndPos = m_xRodStartPos;
	//��ʼ�����߷���
	m_xNearWorkPlaneNorm=console.GetActiveView()->ucs.axis_z;
	if(m_pBaseLineTube)
	{
		m_xDesNormX.bSpecific = TRUE;
		m_xDesNormX.spec_norm.norm_style = 5;
		m_xDesNormX.spec_norm.hVicePart = m_pBaseLineTube->handle;
		m_xDesNormX.spec_norm.direction = 1;
		m_xDesNormY.bByOtherWing = TRUE;
		m_xDesNormY.bSpecific = FALSE;
		//
		m_hWorkNormRefPart=m_pBaseLineTube->handle;
		m_iNormCalStyle=1;
	}
}

void CDesignColumnRayRodDlg::OnBnClickedBtnConfirm()
{
	if(m_fRayRodLen<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Not set rod's length or end anxillary ray line's radius��");
#else 
		AfxMessageBox("δ���ø˼����Ȼ�ĩ�˸���뾶��");
#endif
		return;
	}
	f3dPoint sectCenter(m_xRodStartPos.des_para.COLUMN_INTERS.SectCenter.sect_center[0],
						m_xRodStartPos.des_para.COLUMN_INTERS.SectCenter.sect_center[1],
						m_xRodStartPos.des_para.COLUMN_INTERS.SectCenter.sect_center[2]);
	if(sectCenter.IsZero())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Not input section center or section pick point��");
#else 
		AfxMessageBox("δ�������Բ�ĵ�����ʰȡ�㣡");
#endif
		return;
	}
	CDialog::OnOK();
}

void CDesignColumnRayRodDlg::OnOK()
{
	
}

void CDesignColumnRayRodDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);
	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
int CDesignColumnRayRodDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=FALSE;
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
//<DEVELOP_PROCESS_MARK nameId="CDesignColumnRayRodDlg::DisplayRayRodPropList">
static void UpdateColumnIntersPointItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,
										CLDSPoint *pPoint,BOOL bUpdate,BOOL bStart)
{
	CXhChar200 sText;
	CPropTreeItem *pPropItem = NULL;
	CDesignColumnRayRodDlg *pRayRodDlg = (CDesignColumnRayRodDlg*)pPropList->GetParent();
	
	CString sPropIdPrefix = "m_xRodStartPos";
	if(!bStart)
		sPropIdPrefix = "m_xRodEndPos";

	CItemInfo *lpInfo= new CItemInfo();
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Datum Tube Handle";
	lpInfo->m_strPropHelp = "The tube handle is used to determine column plane";
#else 
	lpInfo->m_strPropName = "��׼�ֹܾ��";
	lpInfo->m_strPropHelp = "����ȷ������ĸֹܾ��";
#endif
	CPropTreeItem *pTubePropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
	pTubePropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("%s.hLineTube",sPropIdPrefix);
	if(pRayRodDlg->GetPropValueStr(pTubePropItem->m_idProp,sText)>0)
		pTubePropItem->m_lpNodeInfo->m_strPropValue = sText;
	pTubePropItem->m_bHideChildren = FALSE;
	
	CLDSLineTube *pLineTube = (CLDSLineTube*)console.FromPartHandle(pPoint->des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
	//����뾶	
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Column Radius";
	lpInfo->m_strPropHelp = "Column Radius";
#else 
	lpInfo->m_strPropName = "����뾶";
	lpInfo->m_strPropHelp = "����뾶";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
	pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("%s.column_r",sPropIdPrefix);
	if(pRayRodDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->SetReadOnly(pLineTube!=NULL);
	//��������������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Column Axis Front Direction";
	lpInfo->m_strPropHelp = "The front direction of column's axis.";
#else 
	lpInfo->m_strPropName = "��������������";
	lpInfo->m_strPropHelp = "��������������";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
	pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("%s.axis_vec",sPropIdPrefix);
	if(pRayRodDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->SetReadOnly();
	//����Բ�ĵ�
#ifdef AFX_TARG_ENU_ENGLISH
	CString sPropName = "Section Center";
	if(pLineTube)
		sPropName = "Section Pick Point";
#else 
	CString sPropName = "����Բ��";
	if(pLineTube)
		sPropName = "����ʰȡ��";
#endif
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName = sPropName;
	if(pLineTube)
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp = "Section pick point is to calculate section center.";
	else 
		lpInfo->m_strPropHelp = "Section Center";
#else 
		lpInfo->m_strPropHelp = "����ʰȡ�����������Բ�ĵ�";
	else 
		lpInfo->m_strPropHelp = "����Բ��";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
	pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("%s.sect_center",sPropIdPrefix);
	if(pRayRodDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->SetReadOnly();
	//���淨��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Section Normal";
	lpInfo->m_strPropHelp="Section normal:it shows vertical section plane when it is null.";
#else 
	lpInfo->m_strPropName="���淨��";
	lpInfo->m_strPropHelp="���淨�ߣ�Ϊ��ʱ��ʾΪ����Ĵ�ֱ����";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
	pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("%s.sect_norm",sPropIdPrefix);
	if(pRayRodDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->SetReadOnly();
	//�����巽ʽ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="0.Specify Directly|1.Specify Datum Edge|";
	if(pLineTube)
		lpInfo->m_cmbItems+="2.Base On Tube Edge|3.Base On Weld Line";
	lpInfo->m_strPropName="Ray Line Direction";
	lpInfo->m_strPropHelp="Ray line's direction type:0.Specify Directly,1~3 Input Rotate Angle(1.Specify Datum Edge,2.Base On Tube Edge,3.Base On Weld Line)";
#else 
	lpInfo->m_cmbItems="0.ֱ��ָ��|1.ָ����׼��|";
	if(pLineTube)
		lpInfo->m_cmbItems+="2.�Ըֹܻ�׼��Ϊ��׼|3.�Ըֹܺ�����Ϊ��׼";
	lpInfo->m_strPropName="���߷���";
	lpInfo->m_strPropHelp="���߷���������:0.ֱ��ָ��,1~3������ת�Ƕ�(1.ָ����׼��,2.�Ըֹܻ�׼��Ϊ��׼,3.�Ըֹܺ�����Ϊ��׼)";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
	pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("%s.ray_vec_style",sPropIdPrefix);
	if(pRayRodDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	if(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0)
	{	//��ת�Ƕ�
		CPropTreeItem* pSubItem=NULL;
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Start Angle";
		lpInfo->m_strPropHelp="The first ray rod's rotate angle relative to specified datum edge,unit is ��";
#else 
		lpInfo->m_strPropName="��ʼ�Ƕ�";
		lpInfo->m_strPropHelp="��һ�����߸˼������ָ����׼�ߵ���ת�Ƕ�,��λΪ��";
#endif
		pSubItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSubItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("%s.ray_angle",sPropIdPrefix);
		if(pRayRodDlg->GetPropValueStr(pSubItem->m_idProp,sText)>0)
			pSubItem->m_lpNodeInfo->m_strPropValue=sText;
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Array Number";
		lpInfo->m_strPropHelp="The ray rods' number around datum axis array,rotate angle is first ray line's angle.";
#else 
		lpInfo->m_strPropName="��������";
		lpInfo->m_strPropHelp="�ƻ�׼����ת�����������߸˼�����������ת�Ƕ���ָ��һ�����ߵĽǶ�";
#endif
		pSubItem = pPropList->InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSubItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_nRotateArrayN");
		if(pRayRodDlg->GetPropValueStr(pSubItem->m_idProp,sText)>0)
			pSubItem->m_lpNodeInfo->m_strPropValue=sText;
		if(pRayRodDlg->m_nRotateArrayN>1)
		{
			CPropTreeItem *pFindItem=pPropList->FindItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_fSectorAngle"),&pPropItem);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Rotate Angle";
				lpInfo->m_strPropHelp="It is two near ray rods's rotate angle when many ray rods rotate,unit is��";
#else 
				lpInfo->m_strPropName="��ת�Ƕ�";
				lpInfo->m_strPropHelp="������߸˼���ת����ʱ�ڽ��������߸˼�����ת����Ƕ�,��λΪ��";
#endif
				pFindItem = pPropList->InsertItem(pSubItem,lpInfo,-1,bUpdate);
				pFindItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_fSectorAngle");
			}
			if(pRayRodDlg->GetPropValueStr(pFindItem->m_idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=sText;
		}
		else
			pPropList->DeleteItemByPropId(CDesignColumnRayRodDlg::GetPropID("m_fSectorAngle"));
	}
	//����ƽ���ƶ�����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Ray Line Translational Distance";
	lpInfo->m_strPropHelp="Ray line translational distance��Vertical ray line parallel direction of movement,right hand spiral's front direction is positive,another side is negative��";
#else 
	lpInfo->m_strPropName="����ƽ�ƾ���";
	lpInfo->m_strPropHelp="����ƽ���ƶ����루��ֱ���߷������ƽ�����ƶ�,����������������ƶ�Ϊ������һ��Ϊ����";
#endif
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
	pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("%s.ray_para_offset",sPropIdPrefix);
	if(pRayRodDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//����Ҫ��ʾƫ�ƻ�׼�㼰����뾶
	/*
	//ƫ�ƻ�׼��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbItems="0.���������潻��|1.Բ������";
	lpInfo->m_strPropName="ƫ�ƻ�׼��";
	lpInfo->m_strPropHelp="0.������������Ľ���Ϊ��׼�������߷���ƫ��,1.������Ϊ��׼�������߷���ƫ��";
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
	pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("%s.bSpecRo",sPropIdPrefix);
	if(pRayRodDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//ƫ����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	if(pPoint->des_para.COLUMN_INTERS.bSpecRo)
	{
		lpInfo->m_strPropName="ƫ����";
		lpInfo->m_strPropHelp="�����������潻��Ϊ��׼�������߷����ƫ����";
	}
	else 
	{
		lpInfo->m_strPropName="����뾶";
		lpInfo->m_strPropHelp="������Ϊ��׼�������߷����ƫ����";
	}
	pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
	pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("%s.ray_offset_r",sPropIdPrefix);
	if(pRayRodDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;*/
}

void CDesignColumnRayRodDlg::DisplayRayRodPropList()
{
	m_xPropList.CleanTree();
	m_xPropList.m_nObjClassTypeId = 0;
	//
	CXhChar200 sText;
	CPropTreeItem *pGroupItem,*pPropItem,*pRoot=m_xPropList.GetRootItem();
	CItemInfo* lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Basic Information";
	lpInfo->m_strPropHelp = "Basic Information";
#else 
	lpInfo->m_strPropName = "������Ϣ";
	lpInfo->m_strPropHelp = "������Ϣ";
#endif
	pGroupItem = m_xPropList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp =GetPropID("basicInfo");

	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Part Label";
	lpInfo->m_strPropHelp = "Part Label";
#else 
	lpInfo->m_strPropName = "�������";
	lpInfo->m_strPropHelp = "�������";
#endif
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("m_sPartNo");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Segment No";
	lpInfo->m_strPropHelp = "Segment No";
#else 
	lpInfo->m_strPropName = "�κ�";
	lpInfo->m_strPropHelp = "�κ�";
#endif
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("m_xSegI");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Material";
	lpInfo->m_strPropHelp = "Material";
#else 
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "����";
#endif
	lpInfo->m_cmbItems=MakeMaterialMarkSetString();
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("m_cMaterial");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Rod Type";
	lpInfo->m_strPropHelp = "Rod Type:0.Angle 1.Slot Angle 2.Flat Iron 3.Tube";
	lpInfo->m_cmbItems="0.Angle|1.Slot Angle|2.Flat Iron|3.Tube";
#else 
	lpInfo->m_strPropName = "�˼�����";
	lpInfo->m_strPropHelp = "�˼�����:0.�Ǹ� 1.�۸� 2.���� 3.�ֹ�";
	lpInfo->m_cmbItems="0.�Ǹ�|1.�۸�|2.����|3.�ֹ�";
#endif
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("m_iRodType");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbType = CDT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Specification";
	lpInfo->m_strPropHelp = "Specification";
#else 
	lpInfo->m_strPropName = "���";
	lpInfo->m_strPropHelp = "���";
#endif
	if(m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_ANGLE)
		lpInfo->m_cmbItems=MakeAngleSetString();
	else if(m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_SLOT)
		lpInfo->m_cmbItems=MakeSlotSetString();
	else if(m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_FLAT)
		lpInfo->m_cmbItems=MakeFlatSetString();
	else if(m_iRodType==CDesignColumnRayRodDlg::ROD_TYPE_TUBE)
		lpInfo->m_cmbItems=MakeTubeSetString();
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("m_sSpec");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//�˼����췽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Rod Extend Direction";
	lpInfo->m_strPropHelp = "Rod Extend Direction:0.Along Column Axis 1.Along Column Ray Line";	// 2.�Զ���
	lpInfo->m_cmbItems="0.Along Column Axis|1.Along Column Ray Line";	//2.�Զ���|
#else 
	lpInfo->m_strPropName = "�˼����췽��";
	lpInfo->m_strPropHelp = "�˼����췽��:0.�������������� 1.��������������";	// 2.�Զ���
	lpInfo->m_cmbItems="0.����������|1.����������";	//2.�Զ���|
#endif
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("m_iEndPosType");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//�˼����ȶ��巽ʽ
	if(m_iEndPosType==ROD_ENDPOSTYPE_RAYVEC)
	{	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Rod Length Define Type";
		lpInfo->m_strPropHelp = "Define type of rod's length :0.Specify rod's length 1.Specify end's ancillary ray radius";
		lpInfo->m_cmbItems="0.Specify Rod Length|1.Specify End Ancillary Ray Radius|";
#else 
		lpInfo->m_strPropName = "�˼����ȶ��巽ʽ";
		lpInfo->m_strPropHelp = "�˼����ȶ��巽ʽ:0.ָ���˼����� 1.ָ��ĩ�˸���뾶";
		lpInfo->m_cmbItems="0.ָ���˼�����|1.ָ��ĩ�˸���뾶|";
#endif
		pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp =GetPropID("m_iRodLenDefType");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	if(m_iEndPosType==ROD_ENDPOSTYPE_RAYVEC&&m_iRodLenDefType==1)
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Ancillary Ray Radius";
	else 
		lpInfo->m_strPropName = "Rod Length";
#else 
		lpInfo->m_strPropName = "ĩ�˸���뾶";
	else 
		lpInfo->m_strPropName = "�˼�����";
#endif
	lpInfo->m_strPropHelp = lpInfo->m_strPropName;
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp =GetPropID("m_fRayRodLen");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//���涨��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Column Definition";
	lpInfo->m_strPropHelp = "Column Definition";
#else 
	lpInfo->m_strPropName = "���涨��";
	lpInfo->m_strPropHelp = "���涨��";
#endif
	pGroupItem = m_xPropList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp =GetPropID("m_xRodStartPos");
	//
	UpdateColumnIntersPointItem(&m_xPropList,pGroupItem,&m_xRodStartPos,FALSE,TRUE);
	//������Ʋ���
	lpInfo = new CItemInfo();
	if(m_iRodType==ROD_TYPE_ANGLE)
	{
		lpInfo->m_controlType = PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Two Leg Infomation";
		lpInfo->m_strPropHelp = "Two leg's infomation";
#else 
		lpInfo->m_strPropName = "��֫������Ϣ";
		lpInfo->m_strPropHelp = "��֫������Ϣ";
#endif
	}
	else 
	{
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Plane Normal";
		lpInfo->m_strPropHelp = "Datum plane's normal line";
#else 
		lpInfo->m_strPropName = "��׼�淨��";
		lpInfo->m_strPropHelp = "��׼�淨��";
#endif
	}
	pGroupItem = m_xPropList.InsertItem(pRoot,lpInfo, -1);
	pGroupItem->m_idProp =GetPropID("workNormDesInfo");
	//
	if(m_iRodType==ROD_TYPE_ANGLE)
	{	//X֫���߲���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Leg Normal Direction";
		lpInfo->m_strPropHelp = "The normal direction of angle's x leg";
#else 
		lpInfo->m_strPropName = "X֫���߷���";
		lpInfo->m_strPropHelp = "�Ǹ�X֫���߷���";
#endif
		pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_xDesNormX");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
		//Y֫���߲���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Leg Normal Direction";
		lpInfo->m_strPropHelp = "The normal direction of angle's y leg";
#else 
		lpInfo->m_strPropName = "Y֫���߷���";
		lpInfo->m_strPropHelp = "�Ǹ�Y֫���߷���";
#endif
		pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_xDesNormY");
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();
	}
	m_xPropList.Redraw();
}
//</DEVELOP_PROCESS_MARK>

//<DEVELOP_PROCESS_MARK nameId="CDesignColumnRayRodDlg::DisplaySubObjPropList">
static void UpdateWorkPlaneNormPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,BOOL bUpdate)
{
	CDesignColumnRayRodDlg *pRayRodDlg = (CDesignColumnRayRodDlg *)pPropList->GetParent();
	if(pRayRodDlg==NULL)
		return;
	CXhChar100 sText;
	CItemInfo *lpInfo = NULL;
	CPropTreeItem *pSonPropItem=NULL;
	if(pRayRodDlg->m_iNormCalStyle==1)
	{
		//��׼����Ʒ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Plane Near Normal";
		lpInfo->m_strPropHelp = "Datum plane's near normal usually specified by user��system can calculate datum plane's normal by near normal and axis��";
#else 
		lpInfo->m_strPropName = "��׼����Ʒ���";
		lpInfo->m_strPropHelp = "��׼����Ʒ���һ�����û�ָ����ϵͳ�ɸ��ݽ��Ʒ��߼����߾�ȷ�������׼�淨�ߡ�";
#endif
		pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_xNearWorkPlaneNorm");
		pSonPropItem->m_bHideChildren = TRUE;
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X";
		lpInfo->m_strPropHelp = "The x coordinate of datum plane's normal ";
#else 
		lpInfo->m_strPropName = "X�������";
		lpInfo->m_strPropHelp = "��׼�淨��X�᷽�������";
#endif
		CPropTreeItem *pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,bUpdate);
		pLeafPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_xNearWorkPlaneNorm.x");
		if(pRayRodDlg->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
			pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y";
		lpInfo->m_strPropHelp = "The y coordinate of datum plane's normal ";
#else 
		lpInfo->m_strPropName = "Y�������";
		lpInfo->m_strPropHelp = "��׼�淨��Y�᷽�������";
#endif
		pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,bUpdate);
		pLeafPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_xNearWorkPlaneNorm.y");
		if(pRayRodDlg->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
			pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Z";
		lpInfo->m_strPropHelp = "The z coordinate of datum plane's normal ";
#else 
		lpInfo->m_strPropName = "Z�������";
		lpInfo->m_strPropHelp = "��׼�淨��Z�᷽�������";
#endif
		pLeafPropItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,bUpdate);
		pLeafPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_xNearWorkPlaneNorm.z");
		if(pRayRodDlg->GetPropValueStr(pLeafPropItem->m_idProp,sText)>0)
			pLeafPropItem->m_lpNodeInfo->m_strPropValue = sText;

		//hWorkNormRefPart
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Normal Reference Rod";
		lpInfo->m_strPropHelp = "It is work plane's reference rod when calculate datum plane's normal��it represent start or end parent rod when it is 0X0(it isn't rod itself)";
#else 
		lpInfo->m_strPropName = "���߲��ո˼�";
		lpInfo->m_strPropHelp = "�����׼�淨��ʱ�Ĺ�������ո˼���0X0ʱĬ��Ϊʼ�˻��ն˽ڵ㸸�˼�(����Ϊ�˼�����)";
#endif
		pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_hWorkNormRefPart");
		if(pRayRodDlg->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	//X
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "The x coordinate of datum plane's normal ";
#else 
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "��׼�淨��X�᷽�������";
#endif
	pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
	pSonPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_xWorkPlaneNorm.x");
	if(pRayRodDlg->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly(pRayRodDlg->m_iNormCalStyle==1);
	//Y
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "The y coordinate of datum plane's normal ";
#else 
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "��׼�淨��Y�᷽�������";
#endif
	pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
	pSonPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_xWorkPlaneNorm.y");
	if(pRayRodDlg->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly(pRayRodDlg->m_iNormCalStyle==1);
	//Z
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "The z coordinate of datum plane's normal ";
#else 
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "��׼�淨��Z�᷽�������";
#endif
	pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
	pSonPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_xWorkPlaneNorm.z");
	if(pRayRodDlg->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->SetReadOnly(pRayRodDlg->m_iNormCalStyle==1);
}

void CDesignColumnRayRodDlg::DisplaySubObjPropList(CPropTreeItem *pItem,BOOL bUpdate/*=FALSE*/)
{
	if(pItem==NULL)
		return;
	CPropTreeItem* pRoot=m_xSubPropList.GetRootItem();
	if(GetPropID("workNormDesInfo")==pItem->m_idProp&&m_iRodType!=ROD_TYPE_ANGLE)
	{	//�ֹܡ��۸֡����������淨�߲���
		if(!bUpdate)
			m_xSubPropList.CleanTree();
		m_xSubPropList.m_nObjClassTypeId = 0;
		m_idParentEventProp = pItem->m_idProp;
		CXhChar100 sText;
		//��׼�淨��
		CItemInfo *lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Plane Normal";
		lpInfo->m_strPropHelp = "Datum plane's normal line";
#else 
		lpInfo->m_strPropName = "��׼�淨��";
		lpInfo->m_strPropHelp = "��׼�淨��";
#endif
		CPropTreeItem *pPropItem = m_xSubPropList.InsertItem(pRoot,lpInfo,-1,bUpdate);
		pPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_xWorkPlaneNorm");
		pPropItem->m_bHideChildren = FALSE;
		//����ָ����ʽ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Calculate Type";
		lpInfo->m_strPropHelp = "It is specify directly or by parent rod.";
		lpInfo->m_cmbItems = "0.Specify Directly|1.Reference Rod";
#else 
		lpInfo->m_strPropName = "���㷽ʽ";
		lpInfo->m_strPropHelp = "ֱ�Ӹ�������ͨ�����˼�����";
		lpInfo->m_cmbItems = "0.ֱ��ָ��|1.���ո˼�";
#endif
		CPropTreeItem *pSonPropItem = m_xSubPropList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp = CDesignColumnRayRodDlg::GetPropID("m_iNormCalStyle");
		pSonPropItem->m_bHideChildren = FALSE;
		if(GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		UpdateWorkPlaneNormPropItem(&m_xSubPropList,pPropItem,bUpdate);
		//
		m_sSubObjName=pItem->m_lpNodeInfo->m_strPropName;
	}
	else if(GetPropID("m_xDesNormX")==pItem->m_idProp)
	{	//�Ǹ�X֫�����淨�߲���
		if(!bUpdate)
			m_xSubPropList.CleanTree();
		m_xSubPropList.m_nObjClassTypeId = 0;
		m_idParentEventProp = pItem->m_idProp;
		if(m_xDesNormX.bSpecific)
		{	//ָ������
			m_xDesVectorX.norm_style=0;
			m_xDesVectorX=m_xDesNormX.spec_norm;
			m_xDesVectorX.vector=m_xNormX;
		}
		else if(m_xDesNormX.bByOtherWing)//�ɽǸ���һ֫ȷ������
		{
			m_xDesVectorX.norm_style=6;
			m_xDesVectorX.vector=m_xNormX;
			m_xDesVectorX.hVicePart=0;
		}
		else if(m_xDesNormX.hViceJg>0x20)//�ɲ��սǸֽ�����ȷ��
		{
			m_xDesVectorX.norm_style=2;
			m_xDesVectorX.hVicePart=0;
			m_xDesVectorX.hCrossPart=m_xDesNormX.hViceJg;
			m_xDesVectorX.nearVector=m_xDesNormX.near_norm;
		}
		m_xSubPropList.m_pObj = &m_xDesVectorX;
		CDisplayVectorProperty::DisplayPropList(&m_xSubPropList,&m_xDesVectorX,TRUE);
		m_sSubObjName=pItem->m_lpNodeInfo->m_strPropName;
	}
	else if(GetPropID("m_xDesNormY")==pItem->m_idProp)
	{	//�Ǹ�Y֫�����淨�߲���
		if(!bUpdate)
			m_xSubPropList.CleanTree();
		m_xSubPropList.m_nObjClassTypeId = 0;
		m_idParentEventProp = pItem->m_idProp;
		if(m_xDesNormY.bSpecific)
		{	//ָ������
			m_xDesVectorY.norm_style=0;
			m_xDesVectorY=m_xDesNormX.spec_norm;
			m_xDesVectorY.vector=m_xNormY;
		}
		else if(m_xDesNormY.bByOtherWing)//�ɽǸ���һ֫ȷ������
		{
			m_xDesVectorY.norm_style=6;
			m_xDesVectorY.vector=m_xNormY;
			m_xDesVectorY.hVicePart=0;
		}
		else if(m_xDesNormY.hViceJg>0x20)//�ɲ��սǸֽ�����ȷ��
		{
			m_xDesVectorY.norm_style=2;
			m_xDesVectorY.hVicePart=0;
			m_xDesVectorY.hCrossPart=m_xDesNormY.hViceJg;
			m_xDesVectorY.nearVector=m_xDesNormY.near_norm;
		}
		m_xSubPropList.m_pObj = &m_xDesVectorY;
		CDisplayVectorProperty::DisplayPropList(&m_xSubPropList,&m_xDesVectorY,TRUE);
		m_sSubObjName=pItem->m_lpNodeInfo->m_strPropName;
	}
	else 
	{	//�����׼�㡢��׼����
		CLDSPoint *pPoint=NULL;
		CLDSVector *pVector=NULL;
		//1.����pItem���ҵ�ǰ�༭������
		if(GetPropID("m_xRodStartPos.ray_vec_style")==pItem->m_idProp)
		{
			if(m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style==0||
				m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style==1)
				pVector = &m_xStartRayVec;
		}
		else if(GetPropID("m_xRodStartPos.sect_norm")==pItem->m_idProp)
			pVector = &m_xStartSectNorm;
		else if(GetPropID("m_xRodStartPos.sect_center")==pItem->m_idProp)
			pPoint = &m_xStartSectCenter;
		else if(GetPropID("m_xRodStartPos.axis_vec")==pItem->m_idProp)
			pVector = &m_xStartAxisVec;
		else if(GetPropID("m_xRodEndPos.ray_vec_style")==pItem->m_idProp)
		{
			if(m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style==0||
				m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style==1)
				pVector = &m_xEndRayVec;
		}
		else if(GetPropID("m_xRodEndPos.sect_norm")==pItem->m_idProp)
			pVector = &m_xEndSectNorm;
		else if(GetPropID("m_xRodEndPos.sect_center")==pItem->m_idProp)
			pPoint = &m_xEndSectCenter;
		else if(GetPropID("m_xRodEndPos.axis_vec")==pItem->m_idProp)
			pVector = &m_xEndAxisVec;
		else 
			return;
		m_sSubObjName = (pItem->m_lpNodeInfo->m_strPropName + ":");

		//2.��ʾ�Ӷ�������
		if(!bUpdate)
			m_xSubPropList.CleanTree();
		m_xSubPropList.m_nObjClassTypeId = 0;
		m_idParentEventProp = pItem->m_idProp;
		if(pPoint)
		{
			m_xSubPropList.m_pObj = pPoint;
			CDisplayPointProperty::DisplayPropList(&m_xSubPropList,pPoint);
		}
		else if(pVector)
		{
			m_xSubPropList.m_pObj = pVector;
			CDisplayVectorProperty::DisplayPropList(&m_xSubPropList,pVector);
		}
	}
	m_xSubPropList.Redraw();
	UpdateData(FALSE);
}
//</DEVELOP_PROCESS_MARK>

//<DEVELOP_PROCESS_MARK nameId="CDesignColumnRayRodDlg::FinishSelectObjOper">
void CDesignColumnRayRodDlg::FinishSelectObjOper()
{
	CXhChar100 sText;
	if(GetPropID("m_hWorkNormRefPart")==GetEventPropId())
	{
		CLDSLinePart *pRod=console.FromRodHandle(m_hPickObj);
		if(pRod)
			m_hWorkNormRefPart=pRod->handle;
	}
	else if(GetPropID("m_xRodStartPos.hLineTube")==GetEventPropId())
	{
		CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(m_hPickObj,CLS_LINETUBE);
		if(pLineTube)
		{
			m_xRodStartPos.des_para.RODNODE.hRod =pLineTube->handle;
			m_xRodStartPos.UpdatePos(console.GetActiveModel());
		}
		if(GetPropValueStr(m_idEventProp,sText)>0)
			m_xPropList.SetItemPropValue(m_idEventProp,sText);
		else 
			m_xPropList.SetItemPropValue(m_idEventProp,"0x0");
	}
	else if(GetPropID("m_xRodStartPos.hLineTube")==GetEventPropId())
	{
		CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(m_hPickObj,CLS_LINETUBE);
		if(pLineTube)
		{
			m_xRodEndPos.des_para.RODNODE.hRod =pLineTube->handle;
			m_xRodEndPos.UpdatePos(console.GetActiveModel());
		}
		if(GetPropValueStr(m_idEventProp,sText)>0)
			m_xPropList.SetItemPropValue(m_idEventProp,sText);
		else 
			m_xPropList.SetItemPropValue(m_idEventProp,"0x0");
	}
	else 
	{
		CLDSPoint *pPoint=NULL;
		CLDSVector *pVector=NULL;
		if(GetPropID("m_xRodStartPos.ray_vec_style")==m_idParentEventProp)
		{
			if(m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style==0)
				pVector = &m_xStartRayVec;
		}
		else if(GetPropID("m_xRodStartPos.sect_norm")==m_idParentEventProp)
			pVector = &m_xStartSectNorm;
		else if(GetPropID("m_xRodStartPos.sect_center")==m_idParentEventProp)
			pPoint = &m_xStartSectCenter;
		else if(GetPropID("m_xRodStartPos.axis_vec")==m_idParentEventProp)
			pVector = &m_xStartAxisVec;
		else if(GetPropID("m_xRodEndPos.ray_vec_style")==m_idParentEventProp)
		{
			if(m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style==0)
				pVector = &m_xEndRayVec;
		}
		else if(GetPropID("m_xRodEndPos.sect_norm")==m_idParentEventProp)
			pVector = &m_xEndSectNorm;
		else if(GetPropID("m_xRodEndPos.sect_center")==m_idParentEventProp)
			pPoint = &m_xEndSectCenter;
		else if(GetPropID("m_xRodEndPos.axis_vec")==m_idParentEventProp)
			pVector = &m_xEndAxisVec;
		if(pVector)
			CDisplayVectorProperty::FinishSelectObjOper(pVector,m_hPickObj,GetEventPropId());
		else if(pPoint)
			CDisplayPointProperty::FinishSelectObjOper(pPoint,m_hPickObj,GetEventPropId());
		else 
			return;
		UpdateParentEventPropItem();
	}
}
//</DEVELOP_PROCESS_MARK>

void CDesignColumnRayRodDlg::UpdateParentEventPropItem()
{
	//����m_idParentEventProp��Ӧ������
	if(CDesignColumnRayRodDlg::GetPropID("m_xRodStartPos.ray_vec_style")==m_idParentEventProp)
	{
		if(m_xStartRayVec.UpdateVector(console.GetActiveModel()))
		{
			m_xRodStartPos.des_para.COLUMN_INTERS.RayVector.ray_vec[0] = m_xStartRayVec.vector.x;
			m_xRodStartPos.des_para.COLUMN_INTERS.RayVector.ray_vec[1] = m_xStartRayVec.vector.y;
			m_xRodStartPos.des_para.COLUMN_INTERS.RayVector.ray_vec[2] = m_xStartRayVec.vector.z;
		}
	}
	else if(CDesignColumnRayRodDlg::GetPropID("m_xRodStartPos.sect_norm")==m_idParentEventProp)
	{
		if(m_xStartSectNorm.UpdateVector(console.GetActiveModel()))
		{
			m_xRodStartPos.des_para.COLUMN_INTERS.SectNorm.sect_norm[0] = m_xStartSectNorm.vector.x;
			m_xRodStartPos.des_para.COLUMN_INTERS.SectNorm.sect_norm[1] = m_xStartSectNorm.vector.y;
			m_xRodStartPos.des_para.COLUMN_INTERS.SectNorm.sect_norm[2] = m_xStartSectNorm.vector.z;
		}
	}
	else if(CDesignColumnRayRodDlg::GetPropID("m_xRodStartPos.sect_center")==m_idParentEventProp)
	{
		if(m_xStartSectCenter.UpdatePos())
		{
			m_xRodStartPos.des_para.COLUMN_INTERS.SectCenter.sect_center[0] = m_xStartSectCenter.Position().x;
			m_xRodStartPos.des_para.COLUMN_INTERS.SectCenter.sect_center[1] = m_xStartSectCenter.Position().y;
			m_xRodStartPos.des_para.COLUMN_INTERS.SectCenter.sect_center[2] = m_xStartSectCenter.Position().z;
		}
	}
	else if(CDesignColumnRayRodDlg::GetPropID("m_xRodStartPos.axis_vec")==m_idParentEventProp)
	{
		if(m_xStartAxisVec.UpdateVector(console.GetActiveModel()))
		{
			m_xRodStartPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[0] = m_xStartAxisVec.vector.x;
			m_xRodStartPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[1] = m_xStartAxisVec.vector.y;
			m_xRodStartPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[2] = m_xStartAxisVec.vector.z;
		}
	}
	else if(CDesignColumnRayRodDlg::GetPropID("m_xRodEndPos.ray_vec_style")==m_idParentEventProp)
	{
		if(m_xEndRayVec.UpdateVector(console.GetActiveModel()))
		{
			m_xRodEndPos.des_para.COLUMN_INTERS.RayVector.ray_vec[0] = m_xEndRayVec.vector.x;
			m_xRodEndPos.des_para.COLUMN_INTERS.RayVector.ray_vec[1] = m_xEndRayVec.vector.y;
			m_xRodEndPos.des_para.COLUMN_INTERS.RayVector.ray_vec[2] = m_xEndRayVec.vector.z;
		}
	}
	else if(CDesignColumnRayRodDlg::GetPropID("m_xRodEndPos.sect_norm")==m_idParentEventProp)
	{
		if(m_xEndSectNorm.UpdateVector(console.GetActiveModel()))
		{
			m_xRodEndPos.des_para.COLUMN_INTERS.SectNorm.sect_norm[0] = m_xEndSectNorm.vector.x;
			m_xRodEndPos.des_para.COLUMN_INTERS.SectNorm.sect_norm[1] = m_xEndSectNorm.vector.y;
			m_xRodEndPos.des_para.COLUMN_INTERS.SectNorm.sect_norm[2] = m_xEndSectNorm.vector.z;
		}
	}
	else if(CDesignColumnRayRodDlg::GetPropID("m_xRodEndPos.sect_center")==m_idParentEventProp)
	{
		if(m_xEndSectCenter.UpdatePos())
		{
			m_xRodEndPos.des_para.COLUMN_INTERS.SectCenter.sect_center[0] = m_xEndSectCenter.Position().x;
			m_xRodEndPos.des_para.COLUMN_INTERS.SectCenter.sect_center[1] = m_xEndSectCenter.Position().y;
			m_xRodEndPos.des_para.COLUMN_INTERS.SectCenter.sect_center[2] = m_xEndSectCenter.Position().z;
		}
	}	
	else if(CDesignColumnRayRodDlg::GetPropID("m_xRodEndPos.axis_vec")==m_idParentEventProp)
	{
		if(m_xEndAxisVec.UpdateVector(console.GetActiveModel()))
		{
			m_xRodEndPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[0] = m_xEndAxisVec.vector.x;
			m_xRodEndPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[1] = m_xEndAxisVec.vector.y;
			m_xRodEndPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[2] = m_xEndAxisVec.vector.z;
		}
	}
}
//<DEVELOP_PROCESS_MARK nameId="CDesignColumnRayRodDlg::CreateRayRod">
CLDSLinePart *CDesignColumnRayRodDlg::CreateRayRod()
{	//0.ȷ���˼�����
	int cls_id = 0;
	if(m_iRodType==ROD_TYPE_ANGLE)
		cls_id = CLS_LINEANGLE;
	else if(m_iRodType==ROD_TYPE_SLOT)
		cls_id = CLS_LINESLOT;
	else if(m_iRodType==ROD_TYPE_FLAT)
		cls_id = CLS_LINEFLAT;
	else if(m_iRodType==ROD_TYPE_TUBE)
		cls_id = CLS_LINETUBE;
	else 
		return NULL;
	CLDSLinePart *pNewRod=NULL;
	double init_sector_angle=0;
	BYTE init_rayvec_style= m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style;
	if(init_rayvec_style>0)
		init_sector_angle=m_xRodStartPos.des_para.COLUMN_INTERS.ray_angle;
	for(int i=0;i<m_nRotateArrayN;i++)
	{
		//1.ȷ��ʼ�ն˶�λ����
		if(m_iEndPosType==ROD_ENDPOSTYPE_AXISVEC)
		{
			m_xRodEndPos=m_xRodStartPos;
			f3dPoint axis_vec(m_xRodStartPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[0],
				m_xRodStartPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[1],
				m_xRodStartPos.des_para.COLUMN_INTERS.AxisVector.axis_vec[2]);
			normalize(axis_vec);
			f3dPoint sect_center(m_xRodStartPos.des_para.COLUMN_INTERS.SectCenter.sect_center[0],
				m_xRodStartPos.des_para.COLUMN_INTERS.SectCenter.sect_center[1],
				m_xRodStartPos.des_para.COLUMN_INTERS.SectCenter.sect_center[2]);
			sect_center += (axis_vec*m_fRayRodLen);
			m_xRodEndPos.des_para.COLUMN_INTERS.SectCenter.sect_center[0] = sect_center.x;
			m_xRodEndPos.des_para.COLUMN_INTERS.SectCenter.sect_center[1] = sect_center.y;
			m_xRodEndPos.des_para.COLUMN_INTERS.SectCenter.sect_center[2] = sect_center.z;
		}
		else if(m_iEndPosType==ROD_ENDPOSTYPE_RAYVEC)
		{
			m_xRodStartPos.des_para.COLUMN_INTERS.bSpecRo=FALSE;
			m_xRodStartPos.des_para.COLUMN_INTERS.ray_offset_r=0;
			m_xRodEndPos=m_xRodStartPos;
			if(m_iRodLenDefType==1)	//ָ��ĩ�˸���뾶
			{
				m_xRodEndPos.des_para.COLUMN_INTERS.bSpecRo=TRUE;
				m_xRodEndPos.des_para.COLUMN_INTERS.ray_offset_r = m_fRayRodLen;
			}
			else					//ָ���˼�����
				m_xRodEndPos.des_para.COLUMN_INTERS.ray_offset_r = m_fRayRodLen;
		}
		if(m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style>0)
			m_xRodStartPos.des_para.COLUMN_INTERS.ray_angle=init_sector_angle+i*m_fSectorAngle;
		else if(i>0)	//�ӵڶ������߸˼���ʼ���Ե�һ���˼������߷���Ϊ��ת��ʼ��
			m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style=1;
		if(m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style>0)
			m_xRodEndPos.des_para.COLUMN_INTERS.ray_angle=init_sector_angle+i*m_fSectorAngle;
		else if(i>0)	//�ӵڶ������߸˼���ʼ���Ե�һ���˼������߷���Ϊ��ת��ʼ��
			m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style=1;

		m_xRodStartPos.UpdatePos();
		m_xRodEndPos.UpdatePos();
		//2.����ʼ�ն˽ڵ�
		CLDSNode *pStartNode=NULL,*pEndNode=NULL;
		if(m_iRodType!=ROD_TYPE_SLOT||(m_iRodType==ROD_TYPE_SLOT&&(m_pBaseLineTube==NULL||m_iEndPosType!=0)))
		{	//�ظֹ����߷���Ĳ۸ֲ���Ҫ���ɽڵ�
			pStartNode=console.AppendNode();
			pEndNode=console.AppendNode();
			pStartNode->iSeg=m_xSegI;
			pEndNode->iSeg=m_xSegI;
			if(m_pBaseLineTube)
			{
				pStartNode->cfgword=m_pBaseLineTube->cfgword;
				pStartNode->SetLayer(m_pBaseLineTube->layer());
				pEndNode->cfgword=m_pBaseLineTube->cfgword;
				pEndNode->SetLayer(m_pBaseLineTube->layer());
			}
			pStartNode->m_cPosCalType=0;
			pEndNode->m_cPosCalType=0;
			pStartNode->SetPosition(m_xRodStartPos.Position());
			pEndNode->SetPosition(m_xRodEndPos.Position());
		}
		//3.���ɻ�׼�����ýڵ㶨λ����
		if(pStartNode)
		{
			pStartNode->m_cPosCalType=11;	//ָ����׼��
			CLDSPoint *pStartPoint = console.AppendPoint();
			long handle = pStartPoint->handle;
			*pStartPoint=m_xRodStartPos;
			pStartPoint->handle=handle;
			pStartNode->hDatumPoint=pStartPoint->handle;
		}
		if(pEndNode)
		{
			pEndNode->m_cPosCalType=11;		//ָ����׼��
			CLDSPoint *pEndPoint = console.AppendPoint();
			long handle = pEndPoint->handle;
			*pEndPoint=m_xRodEndPos;
			pEndPoint->handle=handle;
			pEndNode->hDatumPoint=pEndPoint->handle;
		}
		//4.���ɸ˼������ò���������Ϣ
		pNewRod=(CLDSLinePart*)console.AppendPart(cls_id);
		//���ýڵ㸸�˼�
		if(pStartNode&&pStartNode->hFatherPart<0x20)
		{
			pStartNode->hFatherPart = pNewRod->handle;
			pStartNode->SetModified();
		}
		if(pEndNode&&pEndNode->hFatherPart<0x20)
		{
			pEndNode->hFatherPart = pNewRod->handle;
			pEndNode->SetModified();
		}
		//���ø˼�������Ϣ
		pNewRod->cMaterial = m_cMaterial;
		pNewRod->SetPartNo(m_sPartNo);
		pNewRod->iSeg=m_xSegI;
		pNewRod->pStart = pStartNode;
		pNewRod->pEnd   = pEndNode;
		//�趨ͼ����
		pNewRod->SetLayer(theApp.env.layer);
		pNewRod->layer(1)='F';
		pNewRod->layer(2)='G';
		if(pStartNode)
			pStartNode->layer(2)='G';
		if(pEndNode)
			pEndNode->layer(2)='G';
		if(m_pBaseLineTube)
			pNewRod->SetLayer(m_pBaseLineTube->layer());
		//���ø˼����
		double wing_wide,wing_thick;
		restore_JG_guige(m_sSpec,wing_wide,wing_thick);
		pNewRod->size_wide=wing_wide;
		pNewRod->size_thick=wing_thick;
		//����ʼ�ն�λ��
		if(pNewRod->pStart)
			pNewRod->SetStart(pNewRod->pStart->ActivePosition());
		if(pNewRod->pEnd)
			pNewRod->SetEnd(pNewRod->pEnd->ActivePosition());
		//5.���ø˼���λ����
		if(pNewRod->IsAngle())
		{
			CLDSLineAngle *pLineAngle = (CLDSLineAngle*)pNewRod;
			//���ýǸַ��߲���
			if(m_xDesNormX.spec_norm.norm_style==2)
			{
				m_xDesNormX.hViceJg=pNewRod->handle;
				m_xDesNormX.spec_norm.hVicePart=pNewRod->handle;
			}
			if(m_xDesNormY.spec_norm.norm_style==2)
			{
				m_xDesNormY.hViceJg=pNewRod->handle;
				m_xDesNormY.spec_norm.hVicePart=pNewRod->handle;
			}
			pLineAngle->des_norm_x=m_xDesNormX;
			pLineAngle->des_norm_y=m_xDesNormY;
			//���ýǸֶ�λ����
			pLineAngle->desStartPos.SetToDatumPointStyle();			//ֱ��ָ������
			pLineAngle->desStartPos.datumPoint.datum_pos_style=8;	//��ڵ�ͬλ
			pLineAngle->desStartPos.datumPoint.des_para.hNode=pStartNode->handle;
			pLineAngle->desEndPos.SetToDatumPointStyle();			//ֱ��ָ������
			pLineAngle->desEndPos.datumPoint.datum_pos_style=8;		//��ڵ�ͬλ
			pLineAngle->desEndPos.datumPoint.des_para.hNode=pEndNode->handle;
			pLineAngle->ClearFlag();
			pLineAngle->CalPosition();
			pLineAngle->cal_x_wing_norm();
			pLineAngle->cal_y_wing_norm();
		}
		else 
		{
			if(pNewRod->GetClassTypeId()==CLS_LINESLOT)
			{
				CLDSLineSlot *pLineSlot = (CLDSLineSlot*)pNewRod;
				//���ò۸ֹ��
				_snprintf(pLineSlot->spec,MAX_CHAR_GUIGE_16,"%s",(char*)m_sSpec);
				SLOT_PARA_TYPE *pSlotPara=FindSlotType(m_sSpec);
				if(pSlotPara!=NULL)
				{
					pLineSlot->size_wide=pSlotPara->b;	//��
					pLineSlot->size_height=pSlotPara->h;//�����
					pLineSlot->size_thick=pSlotPara->t;	//��
				}
				//���ù����淨��
				pLineSlot->nearWorkPlaneNorm=m_xWorkPlaneNorm;
				pLineSlot->hWorkNormRefPart=m_hWorkNormRefPart;
				pLineSlot->m_iNormCalStyle=m_iNormCalStyle;
				pLineSlot->CalWorkPlaneNorm();
				if(m_pBaseLineTube&&m_iEndPosType==0)
				{	//��������������Ĳ۸�
					pLineSlot->hAttachTube=m_pBaseLineTube->handle;
					pLineSlot->desDatumPoint=m_xRodStartPos;
					pLineSlot->fLen=m_fRayRodLen;
				}
				//
				pLineSlot->desStartPos.endPosType=0;
				pLineSlot->desEndPos.endPosType=0;
				pLineSlot->CalPosition();
				pLineSlot->CalWorkPlaneNorm();
			}
			else if(pNewRod->GetClassTypeId()==CLS_LINEFLAT)
			{
				CLDSLineFlat *pLineFlat = (CLDSLineFlat*)pNewRod;
				//���ù����淨��
				pLineFlat->nearWorkPlaneNorm=m_xWorkPlaneNorm;
				pLineFlat->hWorkNormRefPart=m_hWorkNormRefPart;
				pLineFlat->m_iNormCalStyle=m_iNormCalStyle;
				pLineFlat->CalWorkPlaneNorm();
				//
				pLineFlat->desStartPos.endPosType=0;
				pLineFlat->desEndPos.endPosType=0;
			}
			else if(pNewRod->GetClassTypeId()==CLS_LINETUBE)
			{
				CLDSLineTube *pLineTube = (CLDSLineTube*)pNewRod;
				//���ù����淨��
				pLineTube->nearWorkPlaneNorm=m_xWorkPlaneNorm;
				pLineTube->hWorkNormRefPart=m_hWorkNormRefPart;
				pLineTube->m_iNormCalStyle=m_iNormCalStyle;
				pLineTube->CalWorkPlaneNorm();
				//
				pLineTube->desStartPos.endPosType=0;
				pLineTube->desEndPos.endPosType=0;
				//������
				if(pLineTube->GetDiameter()>g_sysPara.fTubeWeldRoadMinD)
					pLineTube->m_bHasWeldRoad=TRUE;
				else
					pLineTube->m_bHasWeldRoad=FALSE;
			}
			NewLinePart(pNewRod);
			pNewRod->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pNewRod->GetSolidPartObject());
			g_pSolidDraw->SetEntSnapStatus(pNewRod->handle);
		}
	}
	//��ԭ��ʼֵ
	m_xRodStartPos.des_para.COLUMN_INTERS.ray_vec_style=m_xRodEndPos.des_para.COLUMN_INTERS.ray_vec_style=init_rayvec_style;
	if(init_rayvec_style>0)
		m_xRodStartPos.des_para.COLUMN_INTERS.ray_angle=m_xRodEndPos.des_para.COLUMN_INTERS.ray_angle=init_sector_angle;
	g_pSolidDraw->Draw();
	return pNewRod;
}
//</DEVELOP_PROCESS_MARK>