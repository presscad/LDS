//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include ".\displayvectorproperty.h"
#include "CallBackDialog.h"
#include "LmaDlg.h"

CDisplayVectorProperty::CDisplayVectorProperty(void)
{
}

CDisplayVectorProperty::~CDisplayVectorProperty(void)
{
}

//<DEVELOP_PROCESS_MARK nameId="CDisplayVectorProperty.CPP">
BOOL CDisplayVectorProperty::VectorButtonClick(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CCallBackDialog *pCallBackDlg = (CCallBackDialog *)pPropList->GetParent();
	if(pCallBackDlg==NULL)
		return FALSE;
	CLDSVector *pVector=(CLDSVector*)pPropList->m_pObj;
	char sText[100]="";
	if(CLDSVector::GetPropID("vector")==pItem->m_idProp)
	{
		CReportVectorDlg vecDlg;
		vecDlg.m_fVectorX=pVector->vector.x;
		vecDlg.m_fVectorY=pVector->vector.y;
		vecDlg.m_fVectorZ=pVector->vector.z;
		vecDlg.m_bReadOnlyX=(pVector->norm_style!=0);
		vecDlg.m_bReadOnlyY=(pVector->norm_style!=0);
		vecDlg.m_bReadOnlyZ=(pVector->norm_style!=0);
		if(vecDlg.DoModal()==IDOK)
		{
			pVector->vector.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			normalize(pVector->vector);
			long idProp=CLDSVector::GetPropID("vector.x");
			if(pVector->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CLDSVector::GetPropID("vector.y");
			if(pVector->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CLDSVector::GetPropID("vector.z");
			if(pVector->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);	
			idProp=CLDSVector::GetPropID("VectorType");
			if(pVector->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);	
		}
	}
	else if(CLDSVector::GetPropID("hVicePart")==pItem->m_idProp
		||CLDSVector::GetPropID("hCrossPart")==pItem->m_idProp)
	{
		if(pVector->norm_style==1)
			pCallBackDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);
		else if(pVector->norm_style==2||pVector->norm_style==5)
			pCallBackDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);
		else 
			pCallBackDlg->SelectObject(CLS_PART,pItem->m_idProp);
	}
	else 
		return FALSE;
	return TRUE;
}

BOOL CDisplayVectorProperty::ModifyDatumVectorPropValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CLDSVector *pVector=(CLDSVector*)pPropList->m_pObj;
	if(pVector==NULL)
		return FALSE;
	if(pItem->m_idProp==CLDSVector::GetPropID("norm_style"))
	{
		pVector->norm_style=valueStr[0]-'0';
		UpdateDatumVectorPropItem(pPropList,pItem,pVector,TRUE);
	}
	else if (pItem->m_idProp==CLDSVector::GetPropID("vector.x"))
		pVector->vector.x=atof(valueStr);
	else if (pItem->m_idProp==CLDSVector::GetPropID("vector.y"))
		pVector->vector.y=atof(valueStr);
	else if (pItem->m_idProp==CLDSVector::GetPropID("vector.z"))
		pVector->vector.z=atof(valueStr);
	else if(CLDSVector::GetPropID("hVicePart")==pItem->m_idProp)//��׼�˼����
		sscanf(valueStr,"%d",&(pVector->hVicePart));
	else if(CLDSVector::GetPropID("hCrossPart")==pItem->m_idProp)
		sscanf(valueStr,"%d",&(pVector->hCrossPart));
	else if(CLDSVector::GetPropID("norm_wing")==pItem->m_idProp)
		pVector->norm_wing=valueStr[0]-'0';
	else if(CLDSVector::GetPropID("direction")==pItem->m_idProp)
	{
		pVector->direction=valueStr[0]-'0';
		if(pVector->norm_style==5)
		{
			CSuperSmartPtr<CLDSPart>pDatumPart=console.FromPartHandle(pVector->hVicePart);
			if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			{
				if(pVector->direction<2)
					pPropList->DeleteItemByPropId(CLDSVector::GetPropID("rot_ang"));
				else
				{
					int idProp=CLDSVector::GetPropID("rot_ang");
					if(pPropList->FindItemByPropId(idProp,NULL)==NULL)
					{
						CItemInfo* lpInfo = new CItemInfo();
						lpInfo->m_controlType = PIT_COMBO;
						lpInfo->m_cmbType=CDT_EDIT;
						lpInfo->m_cmbItems = "0|90|180|270";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName = "Rotation Angle";
						if(pVector->direction==2)
							lpInfo->m_strPropHelp="The rotation angle from tube's datum plane around the axial line(��)";
						else 
							lpInfo->m_strPropHelp="The rotation angle from tube's weld plane around the axial line(��)";
#else 
						lpInfo->m_strPropName = "��ת�Ƕ�";
						if(pVector->direction==2)
							lpInfo->m_strPropHelp="�Ըֹܻ�׼��ƽ�淨�������ߵ���ת�Ƕ�(��)";
						else 
							lpInfo->m_strPropHelp="�Ըֹܺ�����ƽ�淨�������ߵ���ת�Ƕ�(��)";
#endif
						CPropTreeItem* pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
						pSonItem->m_idProp = idProp;
						char item_text[100]="";
						if(pVector->GetPropValueStr(pSonItem->m_idProp,item_text)>0)
							pSonItem->m_lpNodeInfo->m_strPropValue = item_text;
					}
				}
			}
		}
	}
	else if(CLDSVector::GetPropID("rot_ang")==pItem->m_idProp)
	{
		pVector->rot_ang = atof(valueStr);
	}
	else if(CLDSVector::GetPropID("VectorType")==pItem->m_idProp)
	{
		if(valueStr[0]=='0')	//�Զ��巽��
		{
			pPropList->SetItemReadOnly(CLDSVector::GetPropID("vector.x"),pVector->norm_style!=0);
			pPropList->SetItemReadOnly(CLDSVector::GetPropID("vector.y"),pVector->norm_style!=0);
			pPropList->SetItemReadOnly(CLDSVector::GetPropID("vector.z"),pVector->norm_style!=0);
		}
		else
		{
			pPropList->SetItemReadOnly(CLDSVector::GetPropID("vector.x"),TRUE);
			pPropList->SetItemReadOnly(CLDSVector::GetPropID("vector.y"),TRUE);
			pPropList->SetItemReadOnly(CLDSVector::GetPropID("vector.z"),TRUE);
			f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
			pVector->vector=vector;
			CString sText;
			sText.Format("%.f",vector.x);
			pPropList->SetItemPropValue(CLDSVector::GetPropID("vector.x"),sText);
			sText.Format("%.f",vector.y);
			pPropList->SetItemPropValue(CLDSVector::GetPropID("vector.y"),sText);
			sText.Format("%.f",vector.z);
			pPropList->SetItemPropValue(CLDSVector::GetPropID("vector.z"),sText);
		}
	}
	else if(CLDSVector::GetPropID("nearVectorType")==pItem->m_idProp)
	{
		if(valueStr[0]-'0'==0)	//�Զ��巽��
		{
			CXhChar100 sText;
			CItemInfo *lpInfo;
			CPropTreeItem *pSonItem,*pFindItem=pPropList->FindItemByPropId(CLDSVector::GetPropID("nearVector.x"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "X Component";
				lpInfo->m_strPropHelp = "X Component";
#else 
				lpInfo->m_strPropName = "X����";
				lpInfo->m_strPropHelp = "X����";
#endif
				pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CLDSVector::GetPropID("nearVector.x");
				if(pVector->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}

			pFindItem=pPropList->FindItemByPropId(CLDSVector::GetPropID("nearVector.y"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Y Component";
				lpInfo->m_strPropHelp = "Y Component";
#else 
				lpInfo->m_strPropName = "Y����";
				lpInfo->m_strPropHelp = "Y����";
#endif
				pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CLDSVector::GetPropID("nearVector.y");
				if(pVector->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}

			pFindItem=pPropList->FindItemByPropId(CLDSVector::GetPropID("nearVector.z"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Z Component";
				lpInfo->m_strPropHelp = "Z Component";
#else 
				lpInfo->m_strPropName = "Z����";
				lpInfo->m_strPropHelp = "Z����";
#endif
				pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CLDSVector::GetPropID("nearVector.z");
				if(pVector->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSVector::GetPropID("nearVector.x"));
			pPropList->DeleteItemByPropId(CLDSVector::GetPropID("nearVector.y"));
			pPropList->DeleteItemByPropId(CLDSVector::GetPropID("nearVector.z"));
			pVector->nearVector=CStdVector::GetVector(valueStr[0]-'0');
		}
	}
	else if(CLDSVector::GetPropID("nearVector.x")==pItem->m_idProp)
		pVector->nearVector.x=atof(valueStr);
	else if(CLDSVector::GetPropID("nearVector.y")==pItem->m_idProp)
		pVector->nearVector.y=atof(valueStr);
	else if(CLDSVector::GetPropID("nearVector.z")==pItem->m_idProp)
		pVector->nearVector.z=atof(valueStr);
	else
		return FALSE;
	if(pVector->norm_style!=0)
	{
		char value_str[100]="";
		pVector->UpdateVector(console.GetActiveModel());
		long idProp=CLDSVector::GetPropID("vector.x");
		if(pVector->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,value_str);
		idProp=CLDSVector::GetPropID("vector.y");
		if(pVector->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,value_str);
		idProp=CLDSVector::GetPropID("vector.z");
		if(pVector->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,value_str);
	}
	return TRUE;
}

void CDisplayVectorProperty::UpdateDatumVectorPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem, CLDSVector *pVector,
													   BOOL bUpdate,BOOL bSpecAngleWingNormStyle/*=FALSE*/,long hCurAngle/*=0*/)
{
	if(pParentItem==NULL)
		return;
	//
	CItemInfo* lpInfo=NULL;
	CXhChar100 sText;
	CPropTreeItem *pPropItem=NULL,*pSonItem=NULL;
	if(bUpdate)
	{
		pPropList->DeleteItemByPropId(CLDSVector::GetPropID("hVicePart"));
		pPropList->DeleteItemByPropId(CLDSVector::GetPropID("hCrossPart"));
		pPropList->DeleteItemByPropId(CLDSVector::GetPropID("norm_wing"));
		pPropList->DeleteItemByPropId(CLDSVector::GetPropID("direction"));
		pPropList->DeleteItemByPropId(CLDSVector::GetPropID("VectorType"));
		pPropList->DeleteItemByPropId(CLDSVector::GetPropID("nearVectorType"));
		pPropList->DeleteItemByPropId(CLDSVector::GetPropID("nearVector.x"));
		pPropList->DeleteItemByPropId(CLDSVector::GetPropID("nearVector.y"));
		pPropList->DeleteItemByPropId(CLDSVector::GetPropID("nearVector.z"));
	}
	if(pVector->norm_style!=0)
	{
		pPropList->SetItemReadOnly(CLDSVector::GetPropID("vector.x"),TRUE);
		pPropList->SetItemReadOnly(CLDSVector::GetPropID("vector.y"),TRUE);
		pPropList->SetItemReadOnly(CLDSVector::GetPropID("vector.z"),TRUE);
	}
	else 
	{
		int nVectorType=CStdVector::GetVectorType(pVector->vector);
		pPropList->SetItemReadOnly(CLDSVector::GetPropID("vector.x"),nVectorType!=0);
		pPropList->SetItemReadOnly(CLDSVector::GetPropID("vector.y"),nVectorType!=0);
		pPropList->SetItemReadOnly(CLDSVector::GetPropID("vector.z"),nVectorType!=0);
	}
	//ָ������
	if(pVector->norm_style==0)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specify Direction";
		lpInfo->m_strPropHelp = "Specified Direction";
#else 
		lpInfo->m_strPropName = "ָ������";
		lpInfo->m_strPropHelp = "ָ������";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("VectorType");
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	//���Ǹ�֫����
	else if(pVector->norm_style==1)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Angle";
		lpInfo->m_strPropHelp = "Datum angle's handle";
#else 
		lpInfo->m_strPropName = "��׼�Ǹ�";
		lpInfo->m_strPropHelp = "��׼�Ǹ־��";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("hVicePart");
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems ="0.X Leg Normal|1.Y Leg Norma";
		lpInfo->m_strPropName = "Datum Leg";
		lpInfo->m_strPropHelp = "Datum Leg";
#else 
		lpInfo->m_cmbItems ="0.X֫����|1.Y֫����";
		lpInfo->m_strPropName = "��׼֫";
		lpInfo->m_strPropHelp = "��׼֫";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("norm_wing");
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems ="0.Outward|1.Inward";
		lpInfo->m_strPropName = "Direction";
		lpInfo->m_strPropHelp = "Direction";
#else 
		lpInfo->m_cmbItems ="0.����|1.����";
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("direction");
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else if(pVector->norm_style==2)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Rod";
		lpInfo->m_strPropHelp = "Datum rod's handle";
#else 
		lpInfo->m_strPropName = "��׼�˼�";
		lpInfo->m_strPropHelp = "��׼�˼����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("hVicePart");
		if(bSpecAngleWingNormStyle)
		{	//��ƽǸ�֫���߲���ʱ��Ĭ�ϸ�������Ϊ��ǰ�Ǹ�
			pVector->hVicePart = hCurAngle;
			pPropItem->SetReadOnly();
		}
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Cross-bracing Reference Rod";
		lpInfo->m_strPropHelp = "Cross-bracing reference rod's handle";
#else 
		lpInfo->m_strPropName = "������ո˼�";
		lpInfo->m_strPropHelp = "������ո˼����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("hCrossPart");
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Near Direction";
		lpInfo->m_strPropHelp = "The near direction distinguish whether or not forward normal direction";
#else 
		lpInfo->m_strPropName = "���Ʒ���";
		lpInfo->m_strPropHelp = "���Ʒ�������������㷽��Ϊ������������߻��Ǹ����߷���";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("nearVectorType");
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		int nearVectorType=CStdVector::GetVectorType(pVector->nearVector);
		if(nearVectorType==0)	//�Զ���
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Component";
			lpInfo->m_strPropHelp = "X Component";
#else 
			lpInfo->m_strPropName = "X����";
			lpInfo->m_strPropHelp = "X����";
#endif
			pSonItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =CLDSVector::GetPropID("nearVector.x");
			if(pVector->GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Component";
			lpInfo->m_strPropHelp = "Y Component";
#else 
			lpInfo->m_strPropName = "Y����";
			lpInfo->m_strPropHelp = "Y����";
#endif
			pSonItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =CLDSVector::GetPropID("nearVector.y");
			if(pVector->GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Component";
			lpInfo->m_strPropHelp = "Z Component";
#else 
			lpInfo->m_strPropName = "Z����";
			lpInfo->m_strPropHelp = "Z����";
#endif
			pSonItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =CLDSVector::GetPropID("nearVector.z");
			if(pVector->GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
	else if(pVector->norm_style==3)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Part";
		lpInfo->m_strPropHelp = "Datum part";
#else 
		lpInfo->m_strPropName = "��׼����";
		lpInfo->m_strPropHelp = "��׼����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("hVicePart");
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems ="0.Forward direction|1.Reverse direction";
		lpInfo->m_strPropName = "Direction";
		lpInfo->m_strPropHelp = "Forward direction is datum part's plus normal ��reverse direction is opposite direction";
#else 
		lpInfo->m_cmbItems ="0.����|1.����";
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "������ָ��׼���������������߷��򣻷�����ָ��׼�����ĸ��������߷���";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("direction");
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else if(pVector->norm_style==4)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Part";
		lpInfo->m_strPropHelp = "Datum Part";
#else 
		lpInfo->m_strPropName = "��׼����";
		lpInfo->m_strPropHelp = "��׼����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("hVicePart");
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Relative Direction";
		lpInfo->m_strPropHelp = "The relative direction of datum part on assemble coordinate system";
#else 
		lpInfo->m_strPropName = "��Է���";
		lpInfo->m_strPropHelp = "��׼����װ������ϵ�µ���Է���";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("nearVectorType");
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		int nearVectorType=CStdVector::GetVectorType(pVector->nearVector);
		if(nearVectorType==0)	//�Զ���
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Component";
			lpInfo->m_strPropHelp = "X Component";
#else 
			lpInfo->m_strPropName = "X����";
			lpInfo->m_strPropHelp = "X����";
#endif
			pSonItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =CLDSVector::GetPropID("nearVector.x");
			if(pVector->GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Component";
			lpInfo->m_strPropHelp = "Y Component";
#else 
			lpInfo->m_strPropName = "Y����";
			lpInfo->m_strPropHelp = "Y����";
#endif
			pSonItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =CLDSVector::GetPropID("nearVector.y");
			if(pVector->GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z Component";
			lpInfo->m_strPropHelp = "Z Component";
#else 
			lpInfo->m_strPropName = "Z����";
			lpInfo->m_strPropHelp = "Z����";
#endif
			pSonItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp =CLDSVector::GetPropID("nearVector.z");
			if(pVector->GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
	else if(pVector->norm_style==5)
	{	//5.��׼�˼��ϵ���������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Rod";
		lpInfo->m_strPropHelp = "Datum rod's handle";
#else 
		lpInfo->m_strPropName = "��׼�˼�";
		lpInfo->m_strPropHelp = "��׼�˼����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("hVicePart");
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		//��������
		bool bNeedRotAnglePara=false;
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pVector->hVicePart,CLS_LINEPART);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		if(pLinePart&&pLinePart->IsAngle())
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems ="0.Start->End|1.End->Start|2.X Leg Normal+|3.X Leg Normal-|4.Y Leg Normal+|5.Y Leg Normal-";
#else 
			lpInfo->m_cmbItems ="0.ʼ->��|1.��->ʼ|2.X֫����+|3.X֫����-|4.Y֫����+|5.Y֫����-";
#endif
		else if(pLinePart&&pLinePart->IsTube())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems = "0.Start->End|1.End->Start|2.Rotate Along Datum Plane|3.Rotate Along Weld Road Plane";
#else 
			lpInfo->m_cmbItems = "0.ʼ->��|1.��->ʼ|2.�Ի�׼����ת|3.�Ժ�������ת";
#endif
			if(pVector->direction>=2)
				bNeedRotAnglePara=true;
		}
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems ="0.Start->End|1.End->Start";
#else 
			lpInfo->m_cmbItems ="0.ʼ->��|1.��->ʼ";
#endif
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Direction Type";
		lpInfo->m_strPropHelp = "0.Rod start->end direction,1.Rod end->start direction,2.Angle X-leg  normal,3.Angle X-leg normal reverse direction,4.Angle Y-leg  normal,5.Angle Y-leg normal reverse direction";
#else 
		lpInfo->m_strPropName = "��������";
		lpInfo->m_strPropHelp = "0.�˼�ʼ->�շ���,1.�˼���->ʼ����,2.�Ǹ�X֫���߷���,3.�Ǹ�X֫���߸�����,4.�Ǹ�Y֫���߷���,5.�Ǹ�Y֫���߸�����";
#endif
		pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =CLDSVector::GetPropID("direction");
		if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bNeedRotAnglePara)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=CDT_EDIT;
			lpInfo->m_cmbItems = "0|90|180|270";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rotation Angle";
			if(pVector->direction==2)
				lpInfo->m_strPropHelp="The rotation angle from tube's datum plane around the axial line��";
			else 
				lpInfo->m_strPropHelp="The rotation angle from tube's weld plane around the axial line��";
#else 
			lpInfo->m_strPropName = "��ת�Ƕ�";
			if(pVector->direction==2)
				lpInfo->m_strPropHelp="�Ըֹܻ�׼��ƽ�淨�������ߵ���ת�Ƕȡ�";
			else 
				lpInfo->m_strPropHelp="�Ըֹܺ�����ƽ�淨�������ߵ���ת�Ƕȡ�";
#endif
			pSonItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonItem->m_idProp = CLDSVector::GetPropID("rot_ang");
			if(pVector->GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		}
	}
}

void CDisplayVectorProperty::DisplayPropList(CPropertyList *pPropList,CLDSVector *pVector,
											 BOOL bSpecAngleWingNormStyle/*=FALSE*/,long hCurAngle/*=0*/)
{
	CPropTreeItem *pGroupItem,*pPropItem,*pRoot=pPropList->GetRootItem();
	char sText[MAX_TEMP_CHAR_100+1]="";
	CItemInfo* lpInfo = NULL;

	pPropList->m_pObj = pVector;
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Vector Coordinate";
	lpInfo->m_strPropHelp="Vector Coordinate";
#else 
	lpInfo->m_strPropName="��������";
	lpInfo->m_strPropHelp="��������";
#endif
	pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CLDSVector::GetPropID("vector");
	if(pVector->GetPropValueStr(pGroupItem->m_idProp,sText)>0)
		pGroupItem->m_lpNodeInfo->m_strPropValue=sText;
	pGroupItem->m_bHideChildren=FALSE;
	//X
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="Normal's x component";
#else 
	lpInfo->m_strPropHelp="���߷���X�������";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSVector::GetPropID("vector.x");
	if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	pPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
	//Y
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="Normal's y component";
#else 
	lpInfo->m_strPropHelp="���߷���Y�������";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSVector::GetPropID("vector.y");
	if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	pPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
	//Z
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="Normal's z component";
#else 
	lpInfo->m_strPropHelp="���߷���Z�������";
#endif
	pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CLDSVector::GetPropID("vector.z");
	if(pVector->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	pPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
	//��׼�㶨λ��ʽ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="0.Specify Direction|1.Single Angle Leg Normal|2.Two Rod Cross-bracing Normal|3.Part Work Normal|4.Part Relative Direction|5.Rod Characteristic Direction";
	if(bSpecAngleWingNormStyle)
		lpInfo->m_cmbItems+="|6.Determined By Another leg|";
	lpInfo->m_strPropName="Normal Locate Mode";
	lpInfo->m_strPropHelp="Normal locate mode";
#else 
	lpInfo->m_cmbItems="0.ָ������|1.���Ǹ�֫����|2.���˼������淨��|3.������������|4.�����ϵ���Է���|5.�˼��ϵ���������";
	if(bSpecAngleWingNormStyle)
		lpInfo->m_cmbItems+="|6.����һ֫ȷ��|";
	lpInfo->m_strPropName="���߶�λ��ʽ";
	lpInfo->m_strPropHelp="���߶�λ��ʽ";
#endif
	pGroupItem = pPropList->InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CLDSVector::GetPropID("norm_style");
	if(pVector->GetPropValueStr(pGroupItem->m_idProp,sText)>0)
		pGroupItem->m_lpNodeInfo->m_strPropValue=sText;
	pGroupItem->m_bHideChildren=FALSE;
	//��������
	UpdateDatumVectorPropItem(pPropList,pGroupItem,pVector,FALSE,bSpecAngleWingNormStyle,hCurAngle);
}

BOOL CDisplayVectorProperty::FinishSelectObjOper(CLDSVector *pVector,long hPickObj,long idEventProp)
{
	if(CLDSVector::GetPropID("hVicePart")==idEventProp)
	{
		CLDSPart *pPart=NULL;
		if(pVector->norm_style==1)
			pPart=console.FromPartHandle(hPickObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		else if(pVector->norm_style==2||pVector->norm_style==5)
			pPart=console.FromPartHandle(hPickObj,CLS_LINEPART);
		else 
			pPart=console.FromPartHandle(hPickObj);
		if(pPart)
		{
			pVector->hVicePart=pPart->handle;
			pVector->UpdateVector(console.GetActiveModel());
		}
	}
	else if(pVector->norm_style==2&&CLDSVector::GetPropID("hCrossPart")==idEventProp)
	{
		CLDSPart *pPart=console.FromPartHandle(hPickObj);
		if(pPart)
		{
			pVector->hCrossPart=pPart->handle;
			pVector->UpdateVector(console.GetActiveModel());
		}
	}
	else 
		return FALSE;
	return TRUE;
}
//</DEVELOP_PROCESS_MARK>