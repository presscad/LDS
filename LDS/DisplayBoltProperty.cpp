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
#include "DatumPointDlg.h"
#include "RelativeObjectDlg.h"
#include "dlg.h"
#include "XhCharString.h"
#include "DatumPointPropertyDlg.h"
#include "DesignVectorPropDlg.h"
#include "DesignShadowDlg.h"
#include "BoltThroughThickDlg.h"
#include "PropertyListOper.h"
#include "UniWirePointModel.h"
#include "ElectricTower.h"
#include "UI.h"
#include "BoltHoleInfo.h"
#include "./StdPartSolid/Fitting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//from DisplayPropertyPage.cpp
BOOL SetPartMutiObjsProp(CLDSObject *pObj, char *propName,long idClassType=0);
void SetPropItemReadOnly(CLDSDbObject *pObj,char *propName,BOOL bReadOnly);

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL ModifyBoltItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSBolt::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

BOOL ModifyBoltProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr);
void Update_Bolt_Des_Pos_Param_TreeItem(CPropertyList *pPropList)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSBolt *pFirstBolt = (CLDSBolt*)pView->selectObjs.GetFirst();
	if(pFirstBolt==NULL)
		return;
	CPropTreeItem *pParentItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_base_pos"),NULL);
	if(pParentItem==NULL)
		return;
	CPropTreeItem *pLeafItem=NULL,*pFindItem=NULL;
	CPropertyListOper<CLDSBolt> oper(pPropList,pFirstBolt);
	//
	CSuperSmartPtr<CLDSPart>pDatumPart=console.FromPartHandle(pFirstBolt->des_base_pos.hPart);
	BOOL bReadOnly=FALSE;
	//��׼���������ϵ���˨������Ϊ��׼������λ���������������ж� wht 10-03-06
	CLDSParamPlate *pStdPart=NULL;
	if(pDatumPart.IsHasPtr()&&pDatumPart->GetClassTypeId()==CLS_PLATE)
		pStdPart=(CLDSParamPlate*)console.FromPartHandle(pDatumPart.PlatePointer()->designInfo.origin.des_para.hPart,CLS_PARAMPLATE);
	if((pDatumPart.IsHasPtr()&&pDatumPart->GetClassTypeId()==CLS_PARAMPLATE&&pDatumPart.ParamPlatePointer()->m_bStdPart)
		||(pStdPart&&pStdPart->m_bStdPart))	
		bReadOnly=TRUE;	//��׼�����ϵ���˨λ�ò��ɵ���
	if((pDatumPart.IsHasPtr()&&pDatumPart->GetClassTypeId()==CLS_LINEANGLE)||pFirstBolt->des_base_pos.R<=0)
	{	//�ǻ����õ���˨
		int index_offset=0;
		if((pDatumPart.IsHasPtr()&&pDatumPart->IsPlate())&&pFirstBolt->des_base_pos.R<=0)	//�ǻ����õ���˨
		{	//�ְ�Ϊ��׼��λ����
			//��λ������������
			pLeafItem=oper.InsertCmbListPropItem(pParentItem,"des_base_pos.cLocationStyle","","","",2,TRUE);
	#ifdef AFX_TARG_ENU_ENGLISH
			if(pFirstBolt->des_base_pos.cLocationStyle!=0)
				pLeafItem->m_lpNodeInfo->m_strPropValue="Local Coords";
			else
				pLeafItem->m_lpNodeInfo->m_strPropValue="Model Coords";
	#else 
			if(pFirstBolt->des_base_pos.cLocationStyle!=0)
				pLeafItem->m_lpNodeInfo->m_strPropValue="�������";
			else
				pLeafItem->m_lpNodeInfo->m_strPropValue="ģ������";
	#endif
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
			pLeafItem->SetReadOnly(!pFirstBolt->IsCanLocalCoordLocation()||pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			index_offset++;
		}
		else
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_base_pos.cLocationStyle"));
		if(pDatumPart.IsHasPtr()&&pDatumPart->GetClassTypeId()==CLS_PLATE&&pFirstBolt->des_base_pos.R<=0&&pDatumPart.PlatePointer()->face_N>1)
		{	//��λ��������ƽ��
			pLeafItem=oper.InsertCmbListPropItem(pParentItem,"des_base_pos.cResidePlane","","","",3,TRUE);
#ifdef AFX_TARG_ENU_ENGLISH
			pLeafItem->m_lpNodeInfo->m_cmbItems="Datum plane";
			if(pDatumPart.PlatePointer()->face_N>=2)
				pLeafItem->m_lpNodeInfo->m_cmbItems+="|Bending plane";
			if(pDatumPart.PlatePointer()->face_N>=3)
				pLeafItem->m_lpNodeInfo->m_cmbItems+="|2nd bending plane";
#else 
			pLeafItem->m_lpNodeInfo->m_cmbItems="��׼ƽ��";
			if(pDatumPart.PlatePointer()->face_N>=2)
				pLeafItem->m_lpNodeInfo->m_cmbItems+="|������";
			if(pDatumPart.PlatePointer()->face_N>=3)
				pLeafItem->m_lpNodeInfo->m_cmbItems+="|�ڶ�������";
#endif
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
			pLeafItem->SetReadOnly(bReadOnly||pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));	//��׼�����ϵ���˨λ�ò����޸�
			index_offset++;
		}
		else
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_base_pos.cResidePlane"));
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsLinePart()&&pDatumPart->IsAngle())
		{	//ƫ��֫
			pLeafItem=oper.InsertCmbListPropItem(pParentItem,"des_base_pos.offset_wing","","","",2+index_offset,TRUE);
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
			pLeafItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			index_offset++;
		}
		else
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_base_pos.offset_wing"));
		//����ƫ�Ʒ�ʽ
		bool lock_wingoffset=false;
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsLinePart()&&pDatumPart->IsAngle()&&
			(pFirstBolt->des_base_pos.datumPoint.datum_pos_style==1||pFirstBolt->des_base_pos.datumPoint.datum_pos_style==2))
		{
			pLeafItem=oper.InsertCmbListPropItem(pParentItem,"des_base_pos.m_biWingOffsetStyle","","","",2+index_offset,TRUE);
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
			index_offset++;
			lock_wingoffset=pFirstBolt->des_base_pos.m_biWingOffsetStyle!=4;
		}
		else
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_base_pos.m_biWingOffsetStyle"));
		//����ƫ����
		pLeafItem=oper.InsertEditPropItem(pParentItem,"des_base_pos.wing_offset_dist","","",2+index_offset,TRUE);
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		pLeafItem->SetReadOnly(lock_wingoffset||bReadOnly||pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));	//��׼�����ϵ���˨λ�ò����޸�
		if(pDatumPart.IsHasPtr()&&(pDatumPart->GetClassTypeId()==CLS_LINEANGLE||pDatumPart->GetClassTypeId()==CLS_LINESLOT))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pLeafItem->m_lpNodeInfo->m_strPropName = "Angle Lateral Offset";
			pLeafItem->m_lpNodeInfo->m_strPropHelp = "Bolt location point,the offset along working leg.";
#else 
			pLeafItem->m_lpNodeInfo->m_strPropName = "�Ǹֺ���ƫ����";
			pLeafItem->m_lpNodeInfo->m_strPropHelp = "��˨�Զ�λ���㣬�ؽǸֵ�ǰ����֫֫�����ƫ����";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pLeafItem->m_lpNodeInfo->m_strPropName = "Y Axial Direction Offset";
			pLeafItem->m_lpNodeInfo->m_strPropHelp = "Bolt location point,the offset along Y axis direction of the datum part's assembling CS.";
#else 
			pLeafItem->m_lpNodeInfo->m_strPropName = "Y�᷽��ƫ����";
			pLeafItem->m_lpNodeInfo->m_strPropHelp = "��˨�Զ�λ���㣬�ض�λ��׼������װ����ϵ��Y�᷽���ƫ������";
#endif
		}
		//����ƫ�Ʒ���
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsLinePart())
		{	//�˼�����ƫ�Ʒ���
			pLeafItem=oper.InsertCmbListPropItem(pParentItem,"des_base_pos.direction","","","",3+index_offset,TRUE);
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
			pLeafItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			index_offset++;
		}
		else
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_base_pos.direction"));
		//����ƫ����
		pLeafItem=oper.InsertEditPropItem(pParentItem,"des_base_pos.len_offset_dist","","",3+index_offset,TRUE);
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		pLeafItem->SetReadOnly(bReadOnly||pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));	//��׼�����ϵ���˨λ�ò����޸�
		if(pDatumPart.IsHasPtr()&&(pDatumPart->GetClassTypeId()==CLS_LINEANGLE||pDatumPart->GetClassTypeId()==CLS_LINESLOT))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pLeafItem->m_lpNodeInfo->m_strPropName = "Angle Vertical Offset";
			pLeafItem->m_lpNodeInfo->m_strPropHelp = "Bolt location point,the offset along length direction";
#else 
			pLeafItem->m_lpNodeInfo->m_strPropName = "�Ǹ�����ƫ����";
			pLeafItem->m_lpNodeInfo->m_strPropHelp = "��˨�Զ�λ���㣬�ؽǸ�����ƫ�Ʒ����ƫ����";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pLeafItem->m_lpNodeInfo->m_strPropName = "X Axial Direction Offset";
			pLeafItem->m_lpNodeInfo->m_strPropHelp = "Bolt location point,the offset along x axis direction of the datum part's assembling CS.";
#else 
			pLeafItem->m_lpNodeInfo->m_strPropName = "X�᷽��ƫ����";
			pLeafItem->m_lpNodeInfo->m_strPropHelp = "��˨�Զ�λ���㣬�ض�λ��׼������װ����ϵ��X�᷽���ƫ������";
#endif
		}
		//
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("RotationStartEdge"));
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_base_pos.offset_angle"));
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_base_pos.R"));
	}
	else
	{	//�����õ���˨
		pFindItem=oper.InsertCmbListPropItem(pParentItem,"RotationStartEdge","","","",1,TRUE);
		pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
		pFindItem->SetReadOnly(bReadOnly||pFirstBolt->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));//��׼�����ϵ���˨λ�ò����޸�
		//
		pFindItem=oper.InsertEditPropItem(pParentItem,"des_base_pos.offset_angle","","",2,TRUE);
		pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
		pFindItem->SetReadOnly(bReadOnly||pFirstBolt->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));//��׼�����ϵ���˨λ�ò����޸�
		//
		pFindItem=oper.InsertEditPropItem(pParentItem,"des_base_pos.R","","",3,TRUE);
		pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
		pFindItem->SetReadOnly(bReadOnly||pFirstBolt->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));	//��׼�����ϵ���˨λ�ò����޸�
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsArcPart())
		{	//����ƫ����
			pFindItem=oper.InsertEditPropItem(pParentItem,"des_base_pos.wing_offset_dist","","",-1,TRUE);
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
			pFindItem->SetReadOnly(bReadOnly||pFirstBolt->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));	//��׼�����ϵ���˨λ�ò����޸�
		}
		else
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_base_pos.wing_offset_dist"));
		//
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_base_pos.len_offset_dist"));
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_base_pos.offset_wing"));
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_base_pos.direction"));
	}
	//pPropList->Redraw();
}
void UpdateBoltDesPosProperty(CPropertyList *pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSBolt *pFirstBolt = (CLDSBolt*)pView->selectObjs.GetFirst();
	if(pFirstBolt==NULL)
		return;
	BYTE datum_pos_style=pFirstBolt->des_base_pos.datumPoint.datum_pos_style;
	CPropListOper<CLDSBolt,HASHOBJSET> oper(pPropList,pFirstBolt,&pView->selectObjs);
	pPropList->DeleteAllSonItems(pParentItem);
	CPropTreeItem *pPropItem=NULL,*pSonItem=NULL;
	if(pFirstBolt->m_bCanSimpleLocateMode&&datum_pos_style>0&&datum_pos_style<4)
	{	//��ģʽ��ʾ��λ����
		pPropItem=oper.InsertCmbListPropItem(pParentItem,"des_base_pos.datum_pos_style","","","",-1,TRUE);
		pPropItem->SetReadOnly();
		if(datum_pos_style==1)
		{	//�Ǹ���㶨λ
			pPropItem=oper.InsertButtonPropItem(pParentItem,"des_base_pos.hBasePart","","",-1,TRUE);
			pPropItem->m_bHideChildren=TRUE;
			if(CLDSBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSBolt::GetPropStatus(pPropItem->m_idProp);
			pSonItem=oper.InsertCmbListPropItem(pPropItem,"des_base_pos.iDirection","","","",-1,TRUE);
			pSonItem->SetReadOnly();
#ifdef AFX_TARG_ENU_ENGLISH
			pSonItem->m_lpNodeInfo->m_strPropName="Link Section";
			pSonItem->m_lpNodeInfo->m_cmbItems="Start|End";
#else
			pSonItem->m_lpNodeInfo->m_strPropName="��ǰ���Ӷ�";
			pSonItem->m_lpNodeInfo->m_cmbItems="ʼ��|�ն�";
#endif
			oper.InsertCmbListPropItem(pPropItem,"des_base_pos.iWorkWing","","","",-1,TRUE);
			oper.InsertCmbEditPropItem(pParentItem,"des_base_pos.fWingOffDist1","","","",-1,TRUE);
			pPropItem=oper.InsertEditPropItem(pParentItem,"des_base_pos.fLenOffDist","","",-1,TRUE);
			SetPartMutiObjsProp(pFirstBolt,"des_base_pos.fLenOffDist",CLS_BOLT);
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropName="End Distance";
#else
			pPropItem->m_lpNodeInfo->m_strPropName="����˾�";
#endif
		}
		else if(datum_pos_style==2)
		{	//�Ǹֽڵ㶨λ
			oper.InsertButtonPropItem(pParentItem,"des_base_pos.hBaseNode","","",-1,TRUE);
			pPropItem=oper.InsertButtonPropItem(pParentItem,"des_base_pos.hBasePart","","",-1,TRUE);
			pPropItem->m_bHideChildren=TRUE;
			if(CLDSBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSBolt::GetPropStatus(pPropItem->m_idProp);
			pSonItem=oper.InsertCmbListPropItem(pPropItem,"des_base_pos.iDirection","","","",-1,TRUE);
			oper.InsertCmbListPropItem(pPropItem,"des_base_pos.iWorkWing","","","",-1,TRUE);
			if(!pFirstBolt->des_base_pos.datumPoint.des_para.RODNODE.vPlaneNormal.IsZero())
				oper.InsertCmbEditPropItem(pParentItem,"des_base_pos.fWingOffDist3","","","",-1,TRUE);
			oper.InsertCmbEditPropItem(pParentItem,"des_base_pos.fWingOffDist1","","","",-1,TRUE);
			oper.InsertEditPropItem(pParentItem,"des_base_pos.fLenOffDist","","",-1,TRUE);
		}
		else// if(datum_pos_style==3)
		{	//���߽��㶨λ
			pPropItem=oper.InsertButtonPropItem(pParentItem,"des_base_pos.hCrossPart","","",-1,TRUE);
			pSonItem=oper.InsertCmbEditPropItem(pPropItem,"des_base_pos.fWingOffDist2","","","",-1,TRUE);
			pSonItem->m_lpNodeInfo->m_strPropName="�����ľ�";
			pPropItem=oper.InsertButtonPropItem(pParentItem,"des_base_pos.hBasePart","","",-1,TRUE);
			pSonItem=oper.InsertCmbEditPropItem(pPropItem,"des_base_pos.fWingOffDist1","","","",-1,TRUE);
			pSonItem->m_lpNodeInfo->m_strPropName="�����ľ�";
			oper.InsertEditPropItem(pPropItem,"des_base_pos.wing_offset_dist","","",-1,TRUE);
			oper.InsertCmbListPropItem(pPropItem,"des_base_pos.direction","","","",-1,TRUE);
			oper.InsertEditPropItem(pPropItem,"des_base_pos.len_offset_dist","","",-1,TRUE);
		}
	}
	else
	{	//�߼�ģʽ��ʾ��λ����
		//��λ��ʽ
		//0.��λ�������Ϣ1.���߽���;2.�Ǹ��������Ϊ��׼;
		//3.�Ǹ��Ͻڵ�Ϊ��׼;4.��˨���һ��˨��λ��;5.ָ����һָ�������ϵ��������
		pPropItem=oper.InsertButtonPropItem(pParentItem,"des_base_pos.datumPoint","","",-1,TRUE);
		pPropItem->m_bHideChildren=TRUE;
		if(CLDSBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSBolt::GetPropStatus(pPropItem->m_idProp);
		//����X�������
		pSonItem=oper.InsertEditPropItem(pPropItem,"des_base_pos.datumPoint.x","","",-1,TRUE);
		pSonItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
		pSonItem->SetReadOnly(pFirstBolt->des_base_pos.datumPoint.datum_pos_style!=0||pFirstBolt->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp));
		//����Y�������
		pSonItem=oper.InsertEditPropItem(pPropItem,"des_base_pos.datumPoint.y","","",-1,TRUE);
		pSonItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
		pSonItem->SetReadOnly(pFirstBolt->des_base_pos.datumPoint.datum_pos_style!=0||pFirstBolt->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp));
		//����Z�������
		pSonItem=oper.InsertEditPropItem(pPropItem,"des_base_pos.datumPoint.z","","",-1,TRUE);
		pSonItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
		pSonItem->SetReadOnly(pFirstBolt->des_base_pos.datumPoint.datum_pos_style!=0||pFirstBolt->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp));
		//��λ��׼����
		pPropItem=oper.InsertButtonPropItem(pParentItem,"des_base_pos.hPart","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		Update_Bolt_Des_Pos_Param_TreeItem(pPropList);
		//����ƫ��
		pPropItem=oper.InsertButtonPropItem(pParentItem,"des_base_pos.norm_offset","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	}
}
void Update_Bolt_Norm_CalStyle_TreeItem(CPropertyList *pPropList,CPropTreeItem *pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSBolt *pFirstBolt = (CLDSBolt*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem,*pFindItem,*pItem,*pLeafItem;
	if(pFirstBolt==NULL||pParentItem==NULL)
		return;
	char sText[MAX_TEMP_CHAR_100+1]="";
	CLDSVector facenorm;
	facenorm = pFirstBolt->des_work_norm;
	const int GROUP_GENERAL=1,GROUP_POS=2;
	DWORD dwPropGroup=GetSingleWord(GROUP_POS);
	CPropertyListOper<CLDSBolt> oper(pPropList,pFirstBolt);
	if(facenorm.norm_style!=0)
	{
		//��׼���߹���
		pPropItem=oper.InsertBtnEditPropItem(pParentItem,"des_work_norm.hVicePart","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs, pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=dwPropGroup;
		pPropItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		SetPropItemReadOnly(pFirstBolt,"work_norm.x",TRUE);
		SetPropItemReadOnly(pFirstBolt,"work_norm.y",TRUE);
		SetPropItemReadOnly(pFirstBolt,"work_norm.z",TRUE);
		if(facenorm.norm_style!=2||facenorm.norm_style!=4)
		{	//�ǽ���˼�ȷ�����߷�����������Ҫ���Ʒ��߷���
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.nearVector"));
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.nearVector.x"));
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.nearVector.y"));
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.nearVector.z"));
		}
		if(facenorm.norm_style!=5)
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.rot_ang"));
		if(facenorm.norm_style!=1)
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.norm_wing"));
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.hVicePart"));
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.norm_wing"));
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.direction"));
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.hCrossPart"));
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.nearVector"));
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.nearVector.x"));
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.nearVector.y"));
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.nearVector.z"));
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.rot_ang"));
		SetPropItemReadOnly(pFirstBolt,"work_norm.x",pFirstBolt->GetObjPropReadOnlyDefaultState(CLDSBolt::GetPropID("work_norm.x")));
		SetPropItemReadOnly(pFirstBolt,"work_norm.y",pFirstBolt->GetObjPropReadOnlyDefaultState(CLDSBolt::GetPropID("work_norm.y")));
		SetPropItemReadOnly(pFirstBolt,"work_norm.z",pFirstBolt->GetObjPropReadOnlyDefaultState(CLDSBolt::GetPropID("work_norm.z")));
	}
	if(facenorm.norm_style==2||facenorm.norm_style==4)
	{
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.direction"));	
		if(facenorm.norm_style==2)
		{
			pPropItem=oper.InsertEditPropItem(pParentItem,"des_work_norm.hCrossPart","","",-1,TRUE);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs, pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
			pPropItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
		else if(facenorm.norm_style==4)
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.hCrossPart"));

		pPropItem=oper.InsertButtonPropItem(pParentItem,"des_work_norm.nearVector","","",-1,TRUE);
		if(facenorm.norm_style==2)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropName = "Near Normal";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "It used to determine the normal when two cross rod calculates work normal.";
#else 
			pPropItem->m_lpNodeInfo->m_strPropName = "���ƹ�������";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "������˼���������ʱ����ȷ�����߷���";
#endif
		}
		else if(facenorm.norm_style==4)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem->m_lpNodeInfo->m_strPropName = "Relative Normal";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "The relative diretion of work normaline in datum part's coordinate.";
#else 
			pPropItem->m_lpNodeInfo->m_strPropName = "��Թ�������";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "���������ڻ�׼��������ϵ�µ���Է���";
#endif
		}
		if(CLDSBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSBolt::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=dwPropGroup;
		pPropItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//���ƹ�������X�������
		pLeafItem=oper.InsertEditPropItem(pPropItem,"des_work_norm.nearVector.x","","",-1,TRUE);
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=dwPropGroup;
		pLeafItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
		//���ƹ�������Y�������
		pLeafItem=oper.InsertEditPropItem(pPropItem,"des_work_norm.nearVector.y","","",-1,TRUE);
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=dwPropGroup;
		pLeafItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
		//���ƹ�������Z�������
		pLeafItem=oper.InsertEditPropItem(pPropItem,"des_work_norm.nearVector.z","","",-1,TRUE);
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=dwPropGroup;
		pLeafItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
	}
	else if(facenorm.norm_style!=0)
	{	
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.hCrossPart"));
		if(facenorm.norm_style==1)
		{
			pPropItem=oper.InsertCmbListPropItem(pParentItem,"des_work_norm.norm_wing","","","",-1,TRUE);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs, pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
			pPropItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
		CLDSPlate* pDatumPlate=(CLDSPlate*)Ta.Parts.FromHandle(pFirstBolt->des_work_norm.hVicePart,CLS_PLATE);
		if(pFirstBolt->des_work_norm.norm_style==3&&pDatumPlate&&pDatumPlate->face_N>1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pPropItem = oper.InsertCmbListPropItem(pParentItem, "des_work_norm.reside_plane", "Datum Plane", "", "", -1, TRUE);
			if(pDatumPlate->face_N>=2)
				pPropItem->m_lpNodeInfo->m_cmbItems+="|Bending plane";
			if(pDatumPlate->face_N>=3)
				pPropItem->m_lpNodeInfo->m_cmbItems+="|2nd bending plane";
#else 
			pPropItem = oper.InsertCmbListPropItem(pParentItem, "des_work_norm.reside_plane", "��׼ƽ��", "", "", -1, TRUE);
			if(pDatumPlate->face_N>=2)
				pPropItem->m_lpNodeInfo->m_cmbItems+="|������";
			if(pDatumPlate->face_N>=3)
				pPropItem->m_lpNodeInfo->m_cmbItems+="|�ڶ�������";
#endif
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs, pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
			pPropItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
		pPropItem=oper.InsertCmbListPropItem(pParentItem,"des_work_norm.direction","","","",-1,TRUE);

#ifdef AFX_TARG_ENU_ENGLISH
		if(facenorm.norm_style==5)
			pPropItem->m_lpNodeInfo->m_strPropName = "Diretion Type";
		else
			pPropItem->m_lpNodeInfo->m_strPropName = "Normal";
#else 
		if(facenorm.norm_style==5)
			pPropItem->m_lpNodeInfo->m_strPropName = "��������";
		else
			pPropItem->m_lpNodeInfo->m_strPropName = "���߳���";
#endif
		pPropItem->m_lpNodeInfo->m_strPropHelp = pFirstBolt->des_work_norm.GetDirectionHelpStr();
		pPropItem->m_lpNodeInfo->m_cmbItems = pFirstBolt->des_work_norm.GetDirectionCmbItems();
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs, pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=dwPropGroup;
		pPropItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		if(facenorm.norm_style==5)
		{
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_work_norm.rot_ang"),&pItem);
			bool bNeedRotAnglePara=false;
			CSuperSmartPtr<CLDSPart>pDatumPart=console.FromPartHandle(facenorm.hVicePart);
			if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			{
				if(facenorm.direction==2||facenorm.direction==3)
					bNeedRotAnglePara=true;
				else
				{
					pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.rot_ang"));
					return;
				}
			}
			else
			{
				if(pFindItem)
					pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.rot_ang"));
				return;
			}
			pPropItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_work_norm.direction"),&pItem);
			if(pFindItem==NULL&&bNeedRotAnglePara&&pPropItem)
			{
				CPropTreeItem* pSonItem=oper.InsertCmbEditPropItem(pPropItem,"des_work_norm.rot_ang","","","",-1,TRUE);
#ifdef AFX_TARG_ENU_ENGLISH
				if(facenorm.direction==2)
					pSonItem->m_lpNodeInfo->m_strPropHelp="The rotation angle from tube's datum plane around the axial line(��)";
				else 
					pSonItem->m_lpNodeInfo->m_strPropHelp="The rotation angle from tube's weld plane around the axial line(��)";
#else 
				if(facenorm.direction==2)
					pSonItem->m_lpNodeInfo->m_strPropHelp="�Ըֹܻ�׼��ƽ�淨�������ߵ���ת�Ƕ�(��)";
				else 
					pSonItem->m_lpNodeInfo->m_strPropHelp="�Ըֹܺ�����ƽ�淨�������ߵ���ת�Ƕ�(��)";
#endif
				pSonItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs, pPropItem->m_idProp);
				pSonItem->m_dwPropGroup=dwPropGroup;
				pSonItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp));
			}
		}
	}
}

BOOL BoltButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem, *pParentItem;
	CLDSBolt *pFirstBolt=NULL, *pBolt=NULL;
	CReportVectorDlg vecDlg;
	CCfgPartNoDlg cfgdlg;
	CString valueStr = _T("");

	if(pView->selectObjs.GetNodeNum()>0)
		pFirstBolt = (CLDSBolt*)pView->selectObjs.GetFirst();
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
	BOOL bReDesignRelaPlate=FALSE;	//�Ƿ���Ҫ������ƹ����ְ� wht 11-05-08
	BOOL bRenderSolid=TRUE;
	try{
		if(pItem->m_idProp==CLDSBolt::GetPropID("L0"))
		{
			CLockPropertyList lockPropertyList(pPropList);
			CBoltThroughThickDlg dlg;
			dlg.m_pBolt=pFirstBolt;
			if(IDOK!=dlg.DoModal())
				return FALSE;
			for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			{
				pBolt->SetL0KeyStr(dlg.m_sL0);
				pBolt->CalGuigeAuto();
				if(dlg.m_bUpdateZOffset)
				{
					pBolt->des_base_pos.norm_offset.key_str=dlg.m_sZOffsetStr;
					pBolt->correct_pos();
				}
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
			pItem->m_lpNodeInfo->m_strPropValue.Format("%.0f",pFirstBolt->L0);
		}
		else if(pItem->m_idProp==CLDSBolt::GetPropID("cfgword"))
		{
			//cfgdlg.cfg_style=CFG_WORD_NO;
			if(pFirstBolt->layer(0)!='L')
				cfgdlg.cfg_style=CFG_BODY_NO;
			else
				cfgdlg.cfg_style=CFG_LEG_NO;
			cfgdlg.cfgword = pFirstBolt->cfgword; 
			if(cfgdlg.DoModal()==IDOK)
			{
				for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
				{
					pBolt->cfgword = cfgdlg.cfgword;		//�ڴ˴����µ�һ���ڵ������Ϻ�
					char sText[MAX_TEMP_CHAR_100+1]="";
					if(pBolt->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
						pItem->m_lpNodeInfo->m_strPropValue=sText;
				}
			}
			bRenderSolid=FALSE;
		}
		/* Ŀǰ��˨���ߺ���������ͼģ���ȡ�ļ�ʱ�Զ����¼���,�ڴ��趨�������� wjh-2015.2.27
		else if(pItem->m_idProp==CLDSBolt::GetPropID("dwRayNo"))
		{
			cfgdlg.cfg_style=CFG_LSRAY_NO;
			cfgdlg.cfgword.flag.word[0]=pFirstBolt->dwRayNo;
			if(cfgdlg.DoModal()==IDOK)
			{
				for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
					pBolt->dwRayNo = cfgdlg.cfgword.flag.word[0];
				pItem->m_lpNodeInfo->m_strPropValue.Empty();	//�������ߺ�
				for(int i=0;i<32;i++)
				{
					if(cfgdlg.cfgword.flag.word[0]&GetSingleWord(i+1))
					{
						CString sTextStr;
						sTextStr.Format("%d",i+1);
						if(pItem->m_lpNodeInfo->m_strPropValue.GetLength()<=0)
							pItem->m_lpNodeInfo->m_strPropValue=sTextStr;
						else
							pItem->m_lpNodeInfo->m_strPropValue+=","+sTextStr;
					}
				}
			}
			bRenderSolid=FALSE;
		}
		*/
		else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.origin"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Bolt Assembling Position";
#else 
			vecDlg.m_sCaption = "��˨װ��λ��";
#endif
			vecDlg.m_fVectorX = pFirstBolt->ucs.origin.x;
			vecDlg.m_fVectorY = pFirstBolt->ucs.origin.y;
			vecDlg.m_fVectorZ = pFirstBolt->ucs.origin.z;
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("ucs.origin.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("ucs.origin.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("ucs.origin.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			f3dPoint old_origin=pFirstBolt->ucs.origin;
			if(vecDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("ucs.origin.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("ucs.origin.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("ucs.origin.z"),CString(sText));
				for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
					pBolt->ucs.origin.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				if(pFirstBolt->ucs.origin!=old_origin)
					bReDesignRelaPlate=TRUE;	//��˨λ�÷����仯��Ҫ���¼�������ְ�λ�� wht 11-05-08
			}
		}
		else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.axis_x"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "X Datum Axis";
#else 
			vecDlg.m_sCaption = "��˨װ��X��׼��";
#endif
			vecDlg.m_fVectorX = pFirstBolt->ucs.axis_x.x;
			vecDlg.m_fVectorY = pFirstBolt->ucs.axis_x.y;
			vecDlg.m_fVectorZ = pFirstBolt->ucs.axis_x.z;
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("ucs.axis_x.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("ucs.axis_x.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("ucs.axis_x.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("ucs.axis_x.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("ucs.axis_x.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("ucs.axis_x.z"),CString(sText));
				for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
					pBolt->ucs.axis_x.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
		else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.datumPoint"))
		{
			/*CDatumPointDlg datumPointDlg;
			datumPointDlg.m_pPoint = &pFirstBolt->des_base_pos.datumPoint;
			if(IDOK==datumPointDlg.DoModal())
			{
			char sText[MAX_TEMP_CHAR_100+1];
			CLDSPoint datumPoint=*datumPointDlg.m_pPoint;
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",datumPoint.pos.x);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint.x"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",datumPoint.pos.y);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint.y"),CString(sText));
			_snprintf(sText,MAX_TEMP_CHAR_100,"%f",datumPoint.pos.z);
			SimplifiedNumString(sText);
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint.z"),CString(sText));
			for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			{
			pBolt->des_base_pos.datumPoint=datumPoint;
			pBolt->correct_pos();
			}
			}*/
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstBolt;
			datumPointDlg.m_xPoint=pFirstBolt->des_base_pos.datumPoint;
			datumPointDlg.m_xPoint.UpdatePos(console.GetActiveModel());
			f3dPoint old_pos=datumPointDlg.m_xPoint.Position();
			if(datumPointDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				CLDSPoint datumPoint=datumPointDlg.m_xPoint;
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",datumPoint.Position().x);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint.x"),CString(sText));
				pFindItem=pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_base_pos.datumPoint.x"),NULL);
				if(pFindItem)
					pFindItem->SetReadOnly(datumPointDlg.m_xPoint.datum_pos_style!=0);
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",datumPoint.Position().y);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint.y"),CString(sText));
				pFindItem=pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_base_pos.datumPoint.y"),NULL);
				if(pFindItem)
					pFindItem->SetReadOnly(datumPointDlg.m_xPoint.datum_pos_style!=0);
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",datumPoint.Position().z);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint.z"),CString(sText));
				pFindItem=pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_base_pos.datumPoint.z"),&pItem);
				if(pFindItem)
					pFindItem->SetReadOnly(datumPointDlg.m_xPoint.datum_pos_style!=0);
				CUndoOperObject undo(&Ta,true);
				for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
				{
					pBolt->des_base_pos.datumPoint=datumPoint;
					pBolt->correct_pos();
				}
				if(pFirstBolt->des_base_pos.datumPoint.Position()!=old_pos)
					bReDesignRelaPlate=TRUE;	//��˨λ�÷����仯��Ҫ���¼�������ְ�λ�� wht 11-05-08
			}
		}
		else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.norm_offset"))
		{
			valueStr=pFirstBolt->des_base_pos.norm_offset.key_str;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the string of normal offset",&valueStr,'s');
#else 
			Input("���뷨��ƫ���ַ���",&valueStr,'s');
#endif
			CString sOffset;
			for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			{
				_snprintf(pBolt->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%s",valueStr);
				pBolt->correct_pos();
			}
			sOffset.Format("%d",pFirstBolt->des_base_pos.norm_offset.Thick(pFirstBolt->BelongModel()));
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_base_pos.norm_offset"),sOffset);
		}
		else if(pItem->m_idProp==CLDSBolt::GetPropID("work_norm"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Work Normal";
#else 
			vecDlg.m_sCaption = "��˨��������";
#endif
			vecDlg.m_fVectorX = pFirstBolt->get_norm().x;
			vecDlg.m_fVectorY = pFirstBolt->get_norm().y;
			vecDlg.m_fVectorZ = pFirstBolt->get_norm().z;
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("work_norm.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("work_norm.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("work_norm.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("work_norm.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("work_norm.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("work_norm.z"),CString(sText));
				for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
				{	//Ӧͬʱ������˨������Ʋ���������λ���߻����޸���ǰ��ֵ wht 09-09-02
					if(pBolt->des_work_norm.norm_style==0)
						pBolt->des_work_norm.vector.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					pBolt->set_norm(f3dPoint(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ));
				}
			}
		}
		else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm"))
		{	//��˨�����������
			CDesignVectorPropDlg vector_dlg;
			vector_dlg.m_hCurEditPart=pFirstBolt->handle;
			vector_dlg.m_xVector=pFirstBolt->des_work_norm;
			if(vector_dlg.DoModal()==IDOK)
			{
				for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
				{
					pBolt->des_work_norm=vector_dlg.m_xVector;
					pBolt->des_work_norm.UpdateVector(console.GetActiveModel());
					pBolt->correct_worknorm();
				}
				CXhChar100 sNormStyle;
				long idProp=CLDSBolt::GetPropID("des_work_norm.norm_style");
				if(pFirstBolt->GetPropValueStr(idProp,sNormStyle)>0)
					pPropList->SetItemPropValue(idProp,CString(sNormStyle));
				pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_work_norm"),&pParentItem);
				if(pFindItem)
					Update_Bolt_Norm_CalStyle_TreeItem(pPropList,pFindItem);
			}
		}
		else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.nearVector"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Work Normal Near Direction";
#else 
			vecDlg.m_sCaption = "��˨�������߽��Ʒ���";
#endif
			vecDlg.m_fVectorX = pFirstBolt->des_work_norm.nearVector.x;
			vecDlg.m_fVectorY = pFirstBolt->des_work_norm.nearVector.y;
			vecDlg.m_fVectorZ = pFirstBolt->des_work_norm.nearVector.z;
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_work_norm.nearVector.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_work_norm.nearVector.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_work_norm.nearVector.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				CString sTextStr;
				sTextStr.Format("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sTextStr);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_work_norm.nearVector.x"),sTextStr);
				sTextStr.Format("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sTextStr);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_work_norm.nearVector.y"),sTextStr);
				sTextStr.Format("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sTextStr);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_work_norm.nearVector.z"),sTextStr);
				for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
				{
					pBolt->des_work_norm.nearVector.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
					pBolt->SetModified();
					pBolt->correct_worknorm();
					pBolt->correct_pos();
				}
				//�������Կ�����ʾ�ķ��߷���
				SetPropItemReadOnly(pFirstBolt,"work_norm.x",FALSE);
				SetPropItemReadOnly(pFirstBolt,"work_norm.y",FALSE);
				SetPropItemReadOnly(pFirstBolt,"work_norm.z",FALSE);
				sTextStr.Format("%f",pFirstBolt->ucs.axis_z.x);
				SimplifiedNumString(sTextStr);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("work_norm.x"),sTextStr);
				sTextStr.Format("%f",pFirstBolt->ucs.axis_z.y);
				SimplifiedNumString(sTextStr);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("work_norm.y"),sTextStr);
				sTextStr.Format("%f",pFirstBolt->ucs.axis_z.z);
				SimplifiedNumString(sTextStr);
				pPropList->SetItemPropValue(CLDSBolt::GetPropID("work_norm.z"),sTextStr);
				SetPropItemReadOnly(pFirstBolt,"work_norm.x",TRUE);
				SetPropItemReadOnly(pFirstBolt,"work_norm.y",TRUE);
				SetPropItemReadOnly(pFirstBolt,"work_norm.z",TRUE);
			}
		}
		else if(pItem->m_idProp==CLDSBolt::GetPropID("relativeObjs"))
		{
			CRelativeObjectDlg relativeDlg;
			relativeDlg.m_pWorkObj = pFirstBolt;
			if(relativeDlg.DoModal()==IDOK)
			{
				CXhChar200 sText;
				pFirstBolt->GetPropValueStr(pItem->m_idProp,sText);
				pPropList->SetItemPropValue(pItem->m_idProp,sText);
			}
			bRenderSolid=FALSE;
		}
		else if(pItem->m_idProp==CLDSBolt::GetPropID("shadowInfo"))
		{
			CDesignShadowDlg shadowdlg;
			shadowdlg.SetShadowObj((CLDSObject*)pFirstBolt);
			shadowdlg.DoModal();
		}
		else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.hBaseNode"))
		{
			static CModifyPartHandleDlg modifyhdlg;
			modifyhdlg.m_sCaption="�޸���˨��λ�ڵ�";
			modifyhdlg.m_sTitle="ѡ��ڵ�:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstBolt->des_base_pos.datumPoint.des_para.RODNODE.hNode);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
			modifyhdlg.SetCmdPickPromptStr("��ѡ����˨��λ�ڵ�:");
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pNode)
				{	
					pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_base_pos.hBaseNode"),"0X%X",pNode->handle);
					g_pSolidDraw->ReleaseSnapStatus();
					for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
					{
						pBolt->des_base_pos.datumPoint.des_para.RODNODE.hNode=pNode->handle;
						g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
					}
				}
			}
		}
		else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.hPart")||
				pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.hBasePart")||
				pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.hCrossPart"))
		{
			//ͨ��ѡ�񹹼��޸Ķ�λ��׼����,һ�㲻��Ҫ�Գƹ��ܹ�֧��ֱ��ѡ�񹹼� wht 11-04-27
			CModifyPartHandleDlg modifyhdlg;
			bool bCurrCrossRod=false;
			long *phBaseRod=NULL,*phCrossRod=NULL;
			if(pFirstBolt->des_base_pos.datumPoint.datum_pos_style==3)
			{
				if(pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1==pFirstBolt->des_base_pos.hPart)
					phBaseRod=&pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
				else
					phBaseRod=&pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
			}
			if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.hCrossPart"))
			{
				bCurrCrossRod=true;
				if(pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1==pFirstBolt->des_base_pos.hPart)
					phCrossRod=&pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
				else
					phCrossRod=&pFirstBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify locating datum part of bolt";
			modifyhdlg.m_sTitle="locating datum part of bolt:";
#else
			modifyhdlg.m_sCaption=phCrossRod!=NULL?"�޸Ķ�λ��׼����":"�޸Ľ���˼�";
			modifyhdlg.m_sTitle=phCrossRod!=NULL?"��λ��׼����:":"��λ����˼�:";
#endif
			if(phCrossRod==NULL)
				modifyhdlg.m_sHandle.Format("0X%X",pFirstBolt->des_base_pos.hPart);
			else
				modifyhdlg.m_sHandle.Format("0X%X",*phCrossRod);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("select locating part of bolt:");
#else
			modifyhdlg.SetCmdPickPromptStr("ѡ����˨��λ����:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			modifyhdlg.m_iMirMsg=0;
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSDbObject *pObj=console.FromPartHandle(modifyhdlg.m_nHandle);
				if(pObj&&pObj->IsPart())
				{	
					pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_base_pos.hPart"),"0X%X",pObj->handle);
					g_pSolidDraw->ReleaseSnapStatus();
					for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
					{
						if(!bCurrCrossRod)
						{
							pBolt->des_base_pos.hPart=pObj->handle;
							if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)
							{
								if(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1==pBolt->des_base_pos.hPart)
									phBaseRod=&pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
								else
									phBaseRod=&pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
								*phBaseRod=pObj->handle;
							}
						}
						else if(bCurrCrossRod)
						{
							if(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1==pBolt->des_base_pos.hPart)
								phCrossRod=&pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
							else
								phCrossRod=&pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
							*phCrossRod=pObj->handle;
						}
						g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
					}
				}
				pPropList->SetCurSel(pItem->m_iIndex);
				pPropList->SetFocus();
			}
		}
		else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.hVicePart"))
		{
			//ͨ��ѡ�񹹼��޸Ķ�λ��׼����,һ�㲻��Ҫ�Գƹ��ܹ�֧��ֱ��ѡ�񹹼� wht 11-04-27
			//�ظ�ִ��GetPartsFromScr���ᵼ�¶���߳�ͬʱִ�е�����ͬʱ��ֹ������һ��selectObjs�ҵ����� wjh-2011.7.19
			CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify normal datum part of bolt";
			modifyhdlg.m_sTitle="normal datum part of bolt:";
#else
			modifyhdlg.m_sCaption="�޸Ļ�׼���߹���";
			modifyhdlg.m_sTitle="��׼���߹���:";
#endif
			modifyhdlg.m_sHandle.Format("0X%X",pFirstBolt->des_work_norm.hVicePart);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select the normal datum part of bolt:");
#else
			modifyhdlg.SetCmdPickPromptStr("��ѡ����˨���߻�׼����:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			modifyhdlg.m_iMirMsg=0;
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSDbObject *pObj=console.FromPartHandle(modifyhdlg.m_nHandle);
				if(pObj&&pObj->IsPart())
				{	
					pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_work_norm.hVicePart"),"0X%X",pObj->handle);
					g_pSolidDraw->ReleaseSnapStatus();
					for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
					{
						pBolt->des_work_norm.hVicePart=pObj->handle;
						g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
					}
					//ѡ���׼�����󣬸��ݹ������͸��·������������ݡ��������ݼ�������ʾ���� wht 12-09-26
					pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_work_norm"),&pParentItem);
					if(pFindItem)
						Update_Bolt_Norm_CalStyle_TreeItem(pPropList,pFindItem);			
				}
				pPropList->SetCurSel(pItem->m_iIndex);
				pPropList->SetFocus();
			}
		}
		else
			return FALSE;
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->SetModified();
			if(bRenderSolid)
			{
				HOLE_WALL* pHoleWall = console.GetHoleWall(pBolt);
				if(pBolt->is_visible)
				{
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
				else if(pHoleWall)
				{
					pHoleWall->UpdateSolid();
					g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
				}
			}
			//�޸���˨λ�û�����˨ֱ������˨���ڰ���Ҫ�������
			//if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.origin")||
			//	pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.datumPoint"))
			if(bReDesignRelaPlate)
			{
				CLDSPlate *pPlate=NULL;
				for(pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
				{
					if(pPlate->FindLsByHandle(pBolt->handle))
					{	//�����������˨��صĸְ�
						pPlate->SetModified();
						pPlate->DesignPlate();
						pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
						g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
					}
				}
			}
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

BOOL ModifyBoltPropItemHelpLiveString(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSObject *pObj=pView->selectObjs.GetFirst();
	if(pObj==NULL||pObj->GetClassTypeId()!=CLS_BOLT)
		return FALSE;
	g_pSolidDraw->DestroyAllMark();
	CLDSBolt *pCurrBolt = (CLDSBolt*)pObj;
	if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.hPart"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurrBolt->des_base_pos.hPart);
		return TRUE;
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.hBaseNode"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		if( pCurrBolt->des_base_pos.datumPoint.datum_pos_style==2)
			g_pSolidDraw->SetEntSnapStatus(pCurrBolt->des_base_pos.datumPoint.m_hNode);
		return TRUE;
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.hBasePart"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurrBolt->des_base_pos.hPart);
		if( pCurrBolt->des_base_pos.datumPoint.datum_pos_style==3&&(
			pCurrBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1!=pCurrBolt->des_base_pos.hPart&&
			pCurrBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2!=pCurrBolt->des_base_pos.hPart))
			g_pSolidDraw->SetEntSnapStatus(pCurrBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1);
		return TRUE;
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.hCrossPart"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		if( pCurrBolt->des_base_pos.datumPoint.datum_pos_style==3)
		{
			if(pCurrBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2==pCurrBolt->des_base_pos.hPart)
				g_pSolidDraw->SetEntSnapStatus(pCurrBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1);
			else //if(pCurrBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1==pCurrBolt->des_base_pos.hPart)
				g_pSolidDraw->SetEntSnapStatus(pCurrBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2);
		}
		return TRUE;
	}
	else
	{
		g_pSolidDraw->ReleaseSnapStatus();
		ARRAY_LIST<long> harr(0,pView->selectObjs.GetNodeNum());
		for(pObj=pView->selectObjs.GetFirst();pObj;pObj=pView->selectObjs.GetNext())
			harr.append(pObj->handle);
		g_pSolidDraw->SetEntSnapStatus(harr.m_pData,harr.GetSize());
		return TRUE;
	}
}
BOOL ModifyBoltProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSBolt *pFirstBolt = (CLDSBolt*)pView->selectObjs.GetFirst();
	CPropTreeItem *pFindItem, *pParentItem;
	CLDSBolt *pBolt = NULL;
	CString tempStr,tempStr_1;
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	int iTemp;
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	CPropertyListOper<CLDSBolt> oper(pPropList,pFirstBolt);
	const int GROUP_GENERAL=1,GROUP_POS=2;
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pItem->m_idProp==CLDSBolt::GetPropID("layer"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->SetLayer(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&8)
					continue;	//��ת�Գ�
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt)
					CalMirAtomLayer(pBolt->layer(),pMirBolt->layer(),pMirObj->mirInfo);
			}
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("m_hFamily"))
	{
		long hFamily=0;
		for(CLsFamily *pFamily=CLsLibrary::GetFirstFamily();pFamily;pFamily=CLsLibrary::GetNextFamily())
		{
			if(valueStr.CompareNoCase(pFamily->name)==0)
			{
				hFamily=pFamily->GetFamilyHandle();
				for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
				{
					//if(hFamily==CLsLibrary::GetFootNailFamily()&&hFamily!=pBolt->m_hFamily)
					if((hFamily==CLsLibrary::GetFootNailFamily()||pFamily->m_bFootNail)&&
						hFamily!=pBolt->m_hFamily)
						pBolt->set_oldL(ftoi(pBolt->L));	//��ֹ��˨�ĳɵĽŶ�����սŶ�ʱ����յ�
					CLsFamily *pOldFamily=CLsLibrary::FromHandle(pBolt->m_hFamily);
					pBolt->m_hFamily=hFamily;
					pBolt->CalGuigeAuto();
					if(pOldFamily!=NULL&&pOldFamily->m_bFootNail==pFamily->m_bFootNail)
					{	//�˴��������漰�Ŷ������޸�������Ŷ����Բ�֧��ֱ���޸ģ���������Ľ����ĶȽŶ��Գ� wjh-2019.3.28
						for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
						{
							CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
							if(pMirBolt)
							{
								pMirBolt->m_hFamily=hFamily;
								pMirBolt->CalGuigeAuto();
							}
						}
					}
				}
				strcpy(tem_str,"");
				long spec_propid=CLDSBolt::GetPropID("specification");
				if(pFirstBolt->GetPropValueStr(spec_propid,tem_str))
					pPropList->SetItemPropValue(spec_propid,tem_str);
				long grade_propid=CLDSBolt::GetPropID("grade");
				if(pFirstBolt->GetPropValueStr(grade_propid,tem_str))
					pPropList->SetItemPropValue(grade_propid,tem_str);
				break;
			}
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("grade"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{	
			pBolt->SetGrade(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt&&pMirBolt->m_hFamily==pBolt->m_hFamily)
					pMirBolt->SetGrade(tem_str);
				
			}
		}
		strcpy(tem_str,"");
		long spec_propid=CLDSBolt::GetPropID("specification");
		if(pFirstBolt->GetPropValueStr(spec_propid,tem_str))
			pPropList->SetItemPropValue(spec_propid,tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("specification"))
	{
		CLDSBolt bolt;
		restore_Ls_guige(tem_str,bolt);
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->SetGrade(bolt.Grade());
			pBolt->set_d(bolt.get_d());
			pBolt->set_L(bolt.get_L());
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt&&pMirBolt->m_hFamily==pBolt->m_hFamily)
				{	//���ӹ��ϵ����ͬ��������Ϊ�˱���,��һ����׽Ŷ�ʱ��һ�й����ԳƵ���˨Ҳ�����޸Ĺ�� wjh-2015.2.13
					pMirBolt->SetGrade(bolt.Grade());
					pMirBolt->set_d(bolt.get_d());
					pMirBolt->set_L(bolt.get_L());
				}
			}
		}
		strcpy(tem_str,"");
		long spec_propid=CLDSBolt::GetPropID("grade");
		if(pFirstBolt->GetPropValueStr(spec_propid,tem_str))
			pPropList->SetItemPropValue(spec_propid,tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("m_hBlock"))
	{
		CXhChar50 keyText(valueStr.GetString());
		char* key=strchr(keyText,'#');
		if(key)
			key++;
		else
			key=keyText;
		long hBlock=0;
		if(key)
			hBlock=HexStrToLong(key);
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->m_hBlock=hBlock;
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("L0"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->CalGuigeAuto();
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt)
					pMirBolt->CalGuigeAuto();
			}
		}
		_snprintf(tem_str,MAX_TEMP_CHAR_100,"%sM%dX%.0f",pFirstBolt->Grade(),pFirstBolt->get_d(),pFirstBolt->get_L());
		int idProp = CLDSBolt::GetPropID("specification");
		pPropList->SetItemPropValue(idProp,CString(tem_str));
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("iSeg"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->iSeg = SEGI(valueStr.GetBuffer());
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt)
					pMirBolt->iSeg=pBolt->iSeg;
			}
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("statSegStr"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			strncpy(pBolt->statSegStr,valueStr,23);
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt)
					strncpy(pMirBolt->statSegStr,pBolt->statSegStr,23);
			}
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("cfgword"))
	{	//ʹ�õ�һ����˨����ĺŸ���������˨����ĺ�
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->cfgword = pFirstBolt->cfgword;
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt)
					pMirBolt->cfgword=pBolt->cfgword;
			}
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("m_uStatMatCoef"))
	{	//ͳ��ϵ��
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->m_uStatMatCoef = atoi(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("hole_d_increment"))
	{	//��˨��ֱ������
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->hole_d_increment = (float)atof(tem_str);
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt)
					pMirBolt->hole_d_increment=pBolt->hole_d_increment;
			}
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("AutoMatchWasherThick"))
	{	//��Ȧ���
		CLDSModule* pModule=Ta.GetActiveModule();
#ifdef AFX_TARG_ENU_ENGLISH
		bool automatched=stricmp(tem_str,"Yes")==0;
#else
		bool automatched=stricmp(tem_str,"��")==0;
#endif
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->DianQuan.AutoMatchThick = automatched;
		//���µ�Ȧ��Ϣ�Լ���˨ͨ�� wht 11-01-10
		//tempStr.Format("-%dX%d", pFirstBolt->DianQuan.thick,pFirstBolt->DianQuan.N);
		//pPropList->SetItemPropValue(CLDSBolt::GetPropID("gasketInfo"),tempStr);
		//tempStr.Format("%.2f",pFirstBolt->get_L0(TRUE));
		//SimplifiedNumString(tempStr);
		//pPropList->SetItemPropValue(CLDSBolt::GetPropID("L0"),tempStr);
		//if(pPropList->GetItemPropValue(CLDSBolt::GetPropID("gasketOffset"),tempStr))
		//	pPropList->SetItemPropValue(CLDSBolt::GetPropID("gasketOffset"),tempStr);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("gasketThick"))
	{	//��Ȧ���
		CLDSModule* pModule=Ta.GetActiveModule();
		CLDSBolt* pFirstBolt=NULL;
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			int thick=atoi(tem_str);
			if(pBolt->DianQuan.thick != thick)
				pBolt->DianQuan.AutoMatchThick=false;
			if(pFirstBolt==NULL)
				pFirstBolt=pBolt;
			pBolt->DianQuan.thick = thick;
			PARTSET partset;
			pView->GetBoltActiveRelaPartSet(pBolt,partset,pModule);
			pBolt->CalGasketOffset(partset);
		}
		if(pFirstBolt&&pFirstBolt->DianQuan.AutoMatchThick)
			pItem->m_lpNodeInfo->SetCheck(1);	//�Զ�ƥ���Ȧ���
		else
			pItem->m_lpNodeInfo->SetCheck(2);	//�˹�ָ����Ȧ���
		//���µ�Ȧ��Ϣ�Լ���˨ͨ�� wht 11-01-10
		if(pFirstBolt->DianQuan.thick>0&&pFirstBolt->DianQuan.N>0)
			tempStr.Format("-%dX%d", pFirstBolt->DianQuan.thick,pFirstBolt->DianQuan.N);
		else
			tempStr="";
		pPropList->SetItemPropValue(CLDSBolt::GetPropID("gasketInfo"),tempStr);
		tempStr.Format("%.f",pFirstBolt->L0);
		SimplifiedNumString(tempStr);
		pPropList->SetItemPropValue(CLDSBolt::GetPropID("L0"),tempStr);
		if(pPropList->GetItemPropValue(CLDSBolt::GetPropID("gasketOffset"),tempStr))
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("gasketOffset"),tempStr);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("gasketN"))
	{	//��Ȧ����
		CLDSModule* pModule=Ta.GetActiveModule();
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->DianQuan.N = atoi(tem_str);
			PARTSET partset;
			pView->GetBoltActiveRelaPartSet(pBolt,partset,pModule);
			pBolt->CalGasketOffset(partset);
		}
		//���µ�Ȧ��Ϣ�Լ���˨ͨ�� wht 11-01-10
		if(pFirstBolt->DianQuan.thick>0&&pFirstBolt->DianQuan.N>0)
			tempStr.Format("-%dX%d", pFirstBolt->DianQuan.thick,pFirstBolt->DianQuan.N);
		else
			tempStr="";
		pPropList->SetItemPropValue(CLDSBolt::GetPropID("gasketInfo"),tempStr);
		tempStr.Format("%.f",pFirstBolt->L0);
		SimplifiedNumString(tempStr);
		pPropList->SetItemPropValue(CLDSBolt::GetPropID("L0"),tempStr);
		if(pPropList->GetItemPropValue(CLDSBolt::GetPropID("gasketOffset"),tempStr))
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("gasketOffset"),tempStr);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("gasketOffset"))
	{	//��Ȧλ��
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->DianQuan.offset = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("m_bVirtualPart"))
	{	//������˨
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pBolt->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pBolt->m_bVirtualPart = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pBolt->m_bVirtualPart = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt)
					pMirBolt->m_bVirtualPart=pBolt->m_bVirtualPart;
			}
		}
	}
	/*else if(pItem->m_idProp==CLDSBolt::GetPropID("m_bFootNail"))
	{	//�Ŷ�
	for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
	{
	if(valueStr.CompareNoCase("��")==0)
	pBolt->m_bFootNail = TRUE;
	else if(valueStr.CompareNoCase("��")==0)
	pBolt->m_bFootNail = FALSE;
	for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
	{
	CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
	if(pMirBolt)
	pMirBolt->m_bFootNail=pBolt->m_bFootNail;
	}
	}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("m_bTwoCapLs"))
	{	//˫ñ��˨
	for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
	{
	if(valueStr.CompareNoCase("��")==0)
	pBolt->m_bTwoCapLs = TRUE;
	else if(valueStr.CompareNoCase("��")==0)
	pBolt->m_bTwoCapLs = FALSE;
	for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
	{
	CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
	if(pMirBolt)
	pMirBolt->m_bTwoCapLs=pBolt->m_bTwoCapLs;
	}
	}
	}*/
	else if(pItem->m_idProp==CLDSBolt::GetPropID("m_bAntiLooseWasher"))
	{	//�����ɵ�
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			BOOL bSet;
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				bSet=TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				bSet=TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				bSet=FALSE;
			pBolt->m_dwFlag.SetFlagDword(CLDSBolt::ANTI_LOOSE,bSet);
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt)
					pMirBolt->m_dwFlag.SetFlagDword(CLDSBolt::ANTI_LOOSE,bSet);
			}
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("m_bAntiTheft"))
	{	//������˨
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			BOOL bSet;
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				bSet=TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				bSet=TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				bSet=FALSE;
			pBolt->m_dwFlag.SetFlagDword(CLDSBolt::ANTI_THEFT,bSet);
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt)
					pMirBolt->m_dwFlag.SetFlagDword(CLDSBolt::ANTI_THEFT,bSet);
			}
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("m_bUserSpecColor"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
				pBolt->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("No")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
				pBolt->m_bUserSpecColor = TRUE;
			else if(valueStr.CompareNoCase("��")==0)
#endif
				pBolt->m_bUserSpecColor = FALSE;
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt)
					pMirBolt->m_bUserSpecColor=pBolt->m_bUserSpecColor;
			}
		}
		pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("crMaterial"));
		if(pFirstBolt->m_bUserSpecColor)
			oper.InsertCmbColorPropItem(pItem,"crMaterial","","","",-1,TRUE);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("crMaterial"))
	{
		COLORREF crCurSel=0;
		memmove(tem_str, tem_str+3, 97);	//����RGB
		sscanf(tem_str,"%X",&crCurSel);
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->crMaterial = crCurSel;
			for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt)
					pMirBolt->crMaterial=pBolt->crMaterial;
			}
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("sNotes"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			_snprintf(pBolt->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
			//��ע���ù����޸� wht 10-10-22
			/*for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
			CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
			if(pMirBolt)
			strcpy(pMirBolt->sNotes,pBolt->sNotes);
			}*/
		}
	}
	else if (pItem->m_idProp == CLDSBolt::GetPropID("m_bCalL0IncGapByInters"))
	{
		bool bCalL0IncGapByInters = (valueStr[0] - '0')>0;
		double dfOldL0=pFirstBolt->L0;
		for (pBolt = (CLDSBolt*)pView->selectObjs.GetFirst(); pBolt; pBolt = (CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->blCalL0IncGapByInters = bCalL0IncGapByInters;
			pBolt->CalGuigeAuto();
			pBolt->SetModified();
			if (pBolt->is_visible)
			{
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
			//���ù����޸�
			/*for(RELATIVE_OBJECT *pMirObj=pBolt->relativeObjs.GetFirst();pMirObj;pMirObj=pBolt->relativeObjs.GetNext())
			{
				CLDSBolt *pMirBolt=(CLDSBolt*)pBolt->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_BOLT);
				if(pMirBolt)
					pMirBolt->blCalL0IncGapByInters = bCalL0IncGapByInters;
			}*/
		}
		if (fabs(dfOldL0-pFirstBolt->L0)>0.5)
		{
			CXhChar16 szL0("%.0f",pFirstBolt->L0);
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("L0"),szL0);
		}
	}
	//������
	else if(pItem->m_idProp==CLDSBolt::GetPropID("dfHoleDiameter")||
		pItem->m_idProp==CLDSBolt::GetPropID("siWaistLength")||
		pItem->m_idProp==CLDSBolt::GetPropID("hHoleResidePart")||
		pItem->m_idProp==CLDSBolt::GetPropID("ciHoleFuncType")||
		pItem->m_idProp==CLDSBolt::GetPropID("ciManuProcessType")||
		pItem->m_idProp==CLDSBolt::GetPropID("blEnlargeHole"))
	{
		for(QUEUENODE_HOLE* pHoleNode=hashBoltHoles.GetFirst();pHoleNode!=NULL;pHoleNode=hashBoltHoles.GetNext())
		{
			pHoleNode->xQueueNode.ModifyPropValueStr(pItem->m_idProp,valueStr);
			if (pItem->m_idProp == CLDSBolt::GetPropID("dfHoleDiameter") || pItem->m_idProp == CLDSBolt::GetPropID("siWaistLength"))
			{
				CLDSPart* pResidePart = pHoleNode->xQueueNode.pLsRef->pBolt->BelongModel()->FromPartHandle(pHoleNode->xQueueNode.hResidePart);
				if (pResidePart)
				{
					pResidePart->SetModified();
					pResidePart->Create3dSolidModel();
				}
				g_pSolidDraw->NewSolidPart(pResidePart->GetSolidPartObject());
			}
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos"))
	{
		bool bDisableSimpleLocateMode=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Simpleness Mode")==0)
			bDisableSimpleLocateMode=false;
		else if(valueStr.CompareNoCase("Senior Mode")==0)
#else 
		if(valueStr.CompareNoCase("��ģʽ")==0)
			bDisableSimpleLocateMode=false;
		else if(valueStr.CompareNoCase("�߼�ģʽ")==0)
#endif
			bDisableSimpleLocateMode=true;
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			if(pBolt->des_base_pos.datumPoint.datum_pos_style>=1&&pBolt->des_base_pos.datumPoint.datum_pos_style<=3)
				pBolt->m_bDisableSimpleLocateMode=bDisableSimpleLocateMode;
			else
				pBolt->m_bDisableSimpleLocateMode=true;
		}
		//
		UpdateBoltDesPosProperty(pPropList,pItem);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.datumPoint.x"))
	{
		pPropList->GetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint.y"), tempStr);
		pPropList->GetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint.z"), tempStr_1);
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->des_base_pos.datumPoint.SetPosition(atof(valueStr),atof(tempStr),atof(tempStr_1));
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.datumPoint.y"))
	{
		pPropList->GetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint.x"), tempStr);
		pPropList->GetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint.z"), tempStr_1);
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->des_base_pos.datumPoint.SetPosition(atof(tempStr),atof(valueStr),atof(tempStr_1));
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.datumPoint.z"))
	{
		pPropList->GetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint.x"), tempStr);
		pPropList->GetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint.y"), tempStr_1);
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->des_base_pos.datumPoint.SetPosition(atof(tempStr),atof(tempStr_1),atof(valueStr));
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.hPart"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			sscanf(tem_str,"%X",&pBolt->des_base_pos.hPart);
			pBolt->correct_pos();
		}
		Update_Bolt_Des_Pos_Param_TreeItem(pPropList);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.cLocationStyle"))
	{
		BYTE cLocationStyle=DESIGN_LS_POS::GLOBAL_COORD;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Local Coords")==0)
#else
		if(valueStr.Compare("�������")==0)
#endif
			cLocationStyle=DESIGN_LS_POS::LOCAL_COORD;
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			if(pBolt->des_base_pos.cLocationStyle==DESIGN_LS_POS::GLOBAL_COORD)
			{
				CLDSPart* pDatumPart=pBolt->BelongModel()->FromPartHandle(pBolt->des_base_pos.hPart);
				if(pDatumPart)
				{
					GEPOINT boltpos=pBolt->ucs.origin;
					coord_trans(boltpos,pDatumPart->ucs,FALSE,TRUE);
					pBolt->des_base_pos.len_offset_dist=boltpos.x;
					pBolt->des_base_pos.wing_offset_dist=boltpos.y;
				}
			}
			else if(pBolt->des_base_pos.cLocationStyle==DESIGN_LS_POS::LOCAL_COORD)
			{
				CLDSPart* pDatumPart=pBolt->BelongModel()->FromPartHandle(pBolt->des_base_pos.hPart);
				if(pDatumPart)
				{
					GEPOINT boltpos=pBolt->ucs.origin;
					coord_trans(boltpos,pDatumPart->ucs,FALSE,TRUE);
					pBolt->des_base_pos.datumPoint.datum_pos_style=9;
					pBolt->des_base_pos.datumPoint.des_para.hPart=pBolt->des_base_pos.hPart;
					pBolt->des_base_pos.datumPoint.datum_pos.Set(boltpos.x,boltpos.y);
					pBolt->des_base_pos.len_offset_dist=0;
					pBolt->des_base_pos.wing_offset_dist=0;
					if (pDatumPart->IsPlate())
					{	//�ֲ����궨λ����˨���䷨��һ������ͬλ��һ����ְ�Ĺ���ƽ�淨��һ�� wjh-2019.5.25
						pBolt->des_work_norm.norm_style = 3;	//������������
						pBolt->des_work_norm.reside_plane = pBolt->des_base_pos.reside_plane;
					}
				}
			}
			pBolt->des_base_pos.cLocationStyle=cLocationStyle;
		}
		if(cLocationStyle == DESIGN_LS_POS::LOCAL_COORD)
		{
			pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_work_norm"), &pParentItem);
			if (pFindItem)
			{
				CPropTreeItem* pNormalItem;
				pNormalItem = oper.InsertCmbListPropItem(pFindItem, "des_work_norm.norm_style");
				pNormalItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs, pNormalItem->m_idProp);
				pNormalItem->m_dwPropGroup = GetSingleWord(GROUP_POS);
				//TODO:��Ҫ���Ʋ�������������� wjh-2019.5.25
				pNormalItem = oper.InsertCmbListPropItem(pFindItem, "des_work_norm.reside_plane");
				pNormalItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(pView->selectObjs, pNormalItem->m_idProp);
				pNormalItem->m_dwPropGroup = GetSingleWord(GROUP_POS);
			}
			else
			{
				oper.UpdatePropItemValue("des_work_norm.norm_style");
				oper.UpdatePropItemValue("des_work_norm.reside_plane");
			}
		}
		Update_Bolt_Des_Pos_Param_TreeItem(pPropList);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.cResidePlane"))
	{
		int iResidePlane=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Bending plane")==0)
			iResidePlane=1;
		else if(valueStr.Compare("2nd bending plane")==0)
			iResidePlane=2;
#else
		if(valueStr.Compare("������")==0)
			iResidePlane=1;
		else if(valueStr.Compare("�ڶ�������")==0)
			iResidePlane=2;
#endif
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->des_base_pos.reside_plane = iResidePlane;
			CLDSPart* pDatumPart = NULL;
			if (pBolt->des_base_pos.cLocationStyle == DESIGN_LS_POS::LOCAL_COORD)
				pDatumPart = pBolt->BelongModel()->FromPartHandle(pBolt->des_base_pos.hPart); 
			if(pDatumPart!=NULL&& pDatumPart->IsPlate())
			{	//�ֲ����궨λ����˨���䷨��һ������ͬλ��һ����ְ�Ĺ���ƽ�淨��һ�� wjh-2019.5.25
				pBolt->des_work_norm.norm_style = 3;	//������������
				pBolt->des_work_norm.reside_plane = pBolt->des_base_pos.reside_plane;
			}
			pBolt->correct_pos();
		}
		oper.UpdatePropItemValue("des_work_norm.norm_style");
		oper.UpdatePropItemValue("des_work_norm.reside_plane");
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.reside_plane"))
	{
		int iResidePlane=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Bending plane")==0)
			iResidePlane=1;
		else if(valueStr.Compare("2nd bending plane")==0)
			iResidePlane=2;
#else
		if(valueStr.Compare("������")==0)
			iResidePlane=1;
		else if(valueStr.Compare("�ڶ�������")==0)
			iResidePlane=2;
#endif
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->des_work_norm.reside_plane = iResidePlane;
			pBolt->correct_worknorm();
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.len_offset_dist"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->des_base_pos.len_offset_dist = atof(valueStr);
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.wing_offset_dist"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->des_base_pos.wing_offset_dist = atof(valueStr);
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.m_biWingOffsetStyle"))
	{
		BYTE offset_style=4;
		if(valueStr.CompareNoCase("g")==0)
			offset_style=0;
		else if(valueStr.CompareNoCase("g1")==0)
			offset_style=1;
		else if(valueStr.CompareNoCase("g2")==0)
			offset_style=2;
		else if(valueStr.CompareNoCase("g3")==0)
			offset_style=3;
		else //if(valueStr.CompareNoCase("�Զ���")==0)
			offset_style=4;
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->des_base_pos.m_biWingOffsetStyle = offset_style;
			pBolt->correct_pos();
		}
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pFirstBolt->BelongModel()->FromPartHandle(pFirstBolt->des_base_pos.hPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pAngle)
		{
			JGZJ jgzj;
			if(pFirstBolt->des_base_pos.offset_wing==0)
				jgzj=pAngle->GetZhunJu('X');
			else
				jgzj=pAngle->GetZhunJu('Y');
			if(pFirstBolt->des_base_pos.datumPoint.datum_pos_style==2&&pAngle->handle==pFirstBolt->des_base_pos.datumPoint.m_hDatumRod)
			{	//֮ǰδ����ڵ�����׼�����,���׵��½Ǹֶ�ͷ����λ������˨��һ��,��������� wjh-2017.11.23
				CLDSNode* pDatumNode=pAngle->BelongModel()->FromNodeHandle(pFirstBolt->des_base_pos.datumPoint.m_hNode);
				jgzj.g=ftoi(pFirstBolt->des_base_pos.offset_wing==0?pAngle->GetNodeWingXG(pDatumNode):pAngle->GetNodeWingYG(pDatumNode));
			}

			long propid=CLDSBolt::GetPropID("des_base_pos.wing_offset_dist");
			CXhChar50 gstr("%d",jgzj.ByStyleG(offset_style,pFirstBolt->des_base_pos.wing_offset_dist));
			pPropList->SetItemPropValue(propid,gstr);
			pPropList->SetItemReadOnly(propid,offset_style!=4);
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("RotationStartEdge"))
	{
		int iStyle0Datum1WeldEdge=valueStr.GetAt(0)-'0';
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			CSuperSmartPtr<CLDSPart> pPlate=pBolt->BelongModel()->FromPartHandle(pBolt->des_base_pos.hPart);
			if(pPlate.IsHasPtr()&&pPlate->GetClassTypeId()==CLS_PARAMPLATE && pPlate.ParamPlatePointer()->m_iParamType==TYPE_FL)
				pPlate.ParamPlatePointer()->SetDesignItemValue('J',iStyle0Datum1WeldEdge,NULL,2);//TYPE_INTEGER
			else if(pPlate.IsHasPtr()&&pPlate->GetClassTypeId()==CLS_PLATE)
				pPlate.PlatePointer()->m_bAttachPartByWeldRoad=iStyle0Datum1WeldEdge;
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.offset_angle"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->des_base_pos.offset_angle = atof(valueStr)*RADTODEG_COEF;
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.R"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->des_base_pos.R = atof(valueStr);
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.offset_wing")||
			pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.iWorkWing"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("X Leg")==0)
				pBolt->des_base_pos.offset_wing = 0;
			else if(valueStr.CompareNoCase("Y Leg")==0)
#else 
			if(valueStr.CompareNoCase("X֫")==0)
				pBolt->des_base_pos.offset_wing = 0;
			else if(valueStr.CompareNoCase("Y֫")==0)
#endif
				pBolt->des_base_pos.offset_wing = 1;
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.fWingOffDist1"))
	{	//
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			int fDist=0;
			CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.FromPartHandle(pFirstBolt->des_base_pos.hPart);
			JGZJ jgzj=pAngle->GetZhunJu(pFirstBolt->des_base_pos.offset_wing==0?'X':'Y');
			if(valueStr.CompareNoCase("g")==0)
				fDist=jgzj.g;
			else if(valueStr.CompareNoCase("g1")==0)
				fDist=jgzj.g1;
			else if(valueStr.CompareNoCase("g2")==0)
				fDist=jgzj.g2;
			else if(valueStr.CompareNoCase("g3")==0)
				fDist=jgzj.g3;
			else
				fDist=atoi(valueStr);
			//
			if(pBolt->des_base_pos.datumPoint.datum_pos_style==1)
			{
				int datum_wing_offset_dist=jgzj.ByStyleG(pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style,pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist);
				pBolt->des_base_pos.wing_offset_dist=fDist-datum_wing_offset_dist;
			}
			else if(pBolt->des_base_pos.datumPoint.datum_pos_style==2)
			{
				int datum_wing_offset_dist=jgzj.ByStyleG(pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style,pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist);
				pBolt->des_base_pos.wing_offset_dist=fDist-datum_wing_offset_dist;
			}
			else if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)
			{
				if(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1==pBolt->des_base_pos.hPart)
				{
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1=fDist;
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=jgzj.MatchStyleG(fDist);
				}
				else
				{
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=fDist;
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=jgzj.MatchStyleG(fDist);
				}
			}
			if(pBolt->des_base_pos.datumPoint.datum_pos_style==1||pBolt->des_base_pos.datumPoint.datum_pos_style==2)
				pBolt->des_base_pos.m_biWingOffsetStyle=jgzj.MatchStyleG(pBolt->des_base_pos.wing_offset_dist);
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.fWingOffDist3"))
	{	//ƽ���ľ�
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			if(pBolt->des_base_pos.datumPoint.datum_pos_style!=2)
				continue;	//ֻ�и˼��ڵ㶨λʱ����ƽ���ľ�ĸ���
			int fDist=0;
			CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.FromPartHandle(pFirstBolt->des_base_pos.hPart);
			JGZJ jgzj=pAngle->GetZhunJu(pFirstBolt->des_base_pos.offset_wing==0?'X':'Y');
			if(valueStr.CompareNoCase("g")==0)
				fDist=jgzj.g;
			else if(valueStr.CompareNoCase("g1")==0)
				fDist=jgzj.g1;
			else if(valueStr.CompareNoCase("g2")==0)
				fDist=jgzj.g2;
			else if(valueStr.CompareNoCase("g3")==0)
				fDist=jgzj.g3;
			else
				fDist=atoi(valueStr);
			double fSumWingOffsetDist=pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist+pBolt->des_base_pos.wing_offset_dist;
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist=fDist;
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=jgzj.MatchStyleG(fDist);
			pBolt->des_base_pos.wing_offset_dist=fSumWingOffsetDist-fDist;	//�������ľ಻��
			pBolt->des_base_pos.m_biWingOffsetStyle=jgzj.MatchStyleG(pBolt->des_base_pos.wing_offset_dist);
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.fWingOffDist2"))
	{	//
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			if(pBolt->des_base_pos.datumPoint.datum_pos_style!=3)
				continue;
			int fDist=0;
			CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.FromPartHandle(pFirstBolt->des_base_pos.hPart);
			JGZJ jgzj=pAngle->GetZhunJu(pFirstBolt->des_base_pos.offset_wing==0?'X':'Y');
			if(valueStr.CompareNoCase("g")==0)
				fDist=jgzj.g;
			else if(valueStr.CompareNoCase("g1")==0)
				fDist=jgzj.g1;
			else if(valueStr.CompareNoCase("g2")==0)
				fDist=jgzj.g2;
			else if(valueStr.CompareNoCase("g3")==0)
				fDist=jgzj.g3;
			else
				fDist=atoi(valueStr);
			if(pBolt->des_base_pos.hPart==pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2)
			{
				pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1=fDist;
				pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=jgzj.MatchStyleG(fDist);
			}
			else
			{
				pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=fDist;
				pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=jgzj.MatchStyleG(fDist);
			}
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.fLenOffDist"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			double fDist=atof(valueStr);
			if(pBolt->des_base_pos.datumPoint.datum_pos_style==1)
			{
				if(pBolt->des_base_pos.datumPoint.des_para.RODEND.direction==pBolt->des_base_pos.direction)
					pBolt->des_base_pos.len_offset_dist=fDist-pBolt->des_base_pos.datumPoint.des_para.RODEND.len_offset_dist;
				else
					pBolt->des_base_pos.len_offset_dist=fDist+pBolt->des_base_pos.datumPoint.des_para.RODEND.len_offset_dist;
			}
			else if(pBolt->des_base_pos.datumPoint.datum_pos_style==2)
			{	
				if(pBolt->des_base_pos.datumPoint.des_para.RODNODE.direction==pBolt->des_base_pos.direction)
					pBolt->des_base_pos.len_offset_dist=fDist-pBolt->des_base_pos.datumPoint.des_para.RODNODE.len_offset_dist;
				else
					pBolt->des_base_pos.len_offset_dist=fDist+pBolt->des_base_pos.datumPoint.des_para.RODNODE.len_offset_dist;
			}
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.direction"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start->End")==0)
				pBolt->des_base_pos.direction = 0;
			else if(valueStr.CompareNoCase("End->Start")==0)
#else 
			if(valueStr.CompareNoCase("ʼ->��")==0)
				pBolt->des_base_pos.direction = 0;
			else if(valueStr.CompareNoCase("��->ʼ")==0)
#endif
				pBolt->des_base_pos.direction = 1;
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.iDirection"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Start->End")==0||valueStr.CompareNoCase("Start")==0)
				pBolt->des_base_pos.direction = 0;
			else if(valueStr.CompareNoCase("End->Start")==0||valueStr.CompareNoCase("End")==0)
#else 
			if(valueStr.CompareNoCase("ʼ->��")==0||valueStr.CompareNoCase("ʼ��")==0)
				pBolt->des_base_pos.direction = 0;
			else if(valueStr.CompareNoCase("��->ʼ")==0||valueStr.CompareNoCase("�ն�")==0)
#endif
				pBolt->des_base_pos.direction = 1;
			pBolt->correct_pos();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.norm_style"))
	{
		sscanf(valueStr,"%d",&iTemp);
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->des_work_norm.norm_style = iTemp;
		pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_work_norm"),&pParentItem);
		if(pFindItem)
			Update_Bolt_Norm_CalStyle_TreeItem(pPropList,pFindItem);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.hVicePart"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pBolt->des_work_norm.hVicePart);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.norm_wing"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("X leg normal")==0)
				pBolt->des_work_norm.norm_wing = 0;
			else if(valueStr.CompareNoCase("Y leg normal")==0)
#else 
			if(valueStr.CompareNoCase("X֫����")==0)
				pBolt->des_work_norm.norm_wing = 0;
			else if(valueStr.CompareNoCase("Y֫����")==0)
#endif
				pBolt->des_work_norm.norm_wing = 1;
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.direction"))
	{
		CLDSModule* pModule=Ta.GetActiveModule();
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			int iOldDirection=pBolt->des_work_norm.direction,iNewDirection=valueStr[0]-'0';
			if(iOldDirection!=iNewDirection && (iOldDirection/2==iNewDirection/2) &&
				pBolt->des_work_norm.ReverseDirection(false))
			{
				//������˨���߷���
				pBolt->correct_worknorm();
				//������˨����ƫ����
				PARTSET partset;
				pView->GetBoltActiveRelaPartSet(pBolt,partset,pModule);
				pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pBolt,partset);//��Ϊֱ����������Z��ƫ�Ʋ�����������֮ǰ�ķ�ʽ�׵��»��� wjh-2015.6.8
				//�жϽ�ʱ��˨�������ı�ʱ,Ӧ�趨����Ա㴦����˨λ�ø��������� wjh-2013.8.3
				//if(pFirstBolt->m_dwFlag.IsHasFlag(CLDSBolt::REVERSE_NORM))
				//	pBolt->m_dwFlag.SetFlagDword(CLDSBolt::REVERSE_NORM,FALSE);
				//else
				//	pBolt->m_dwFlag.SetFlagDword(CLDSBolt::REVERSE_NORM,TRUE);
			}
			else
				pBolt->des_work_norm.direction=iNewDirection;
			if(pBolt->des_work_norm.norm_style==5)
			{
				CSuperSmartPtr<CLDSPart>pDatumPart=console.FromPartHandle(pBolt->des_work_norm.hVicePart);
				if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
				{
					if(pFirstBolt->des_work_norm.direction<2||pFirstBolt->des_work_norm.direction>3)
						pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("des_work_norm.rot_ang"));
					else
					{
						CPropTreeItem* pSonItem=oper.InsertCmbListPropItem(pItem,"des_work_norm.rot_ang","","","",-1,TRUE);
#ifdef AFX_TARG_ENU_ENGLISH
						if(pFirstBolt->des_work_norm.direction==2)
							pSonItem->m_lpNodeInfo->m_strPropHelp="The rotation angle from tube's datum plane around the axial line(��)";
						else 
							pSonItem->m_lpNodeInfo->m_strPropHelp="The rotation angle from tube's weld plane around the axial line(��)";
#else 
						if(pFirstBolt->des_work_norm.direction==2)
							pSonItem->m_lpNodeInfo->m_strPropHelp="�Ըֹܻ�׼��ƽ�淨�������ߵ���ת�Ƕ�(��)";
						else 
							pSonItem->m_lpNodeInfo->m_strPropHelp="�Ըֹܺ�����ƽ�淨�������ߵ���ת�Ƕ�(��)";
#endif
					}
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.rot_ang"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{
			pBolt->des_work_norm.rot_ang = atof(valueStr);
			pBolt->correct_worknorm();
		}
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.hCrossPart"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			sscanf(tem_str,"%X",&pBolt->des_work_norm.hCrossPart);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.nearVector.x"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->des_work_norm.nearVector.x=atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.nearVector.y"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->des_work_norm.nearVector.y=atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.nearVector.z"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->des_work_norm.nearVector.z=atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.origin.x"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->ucs.origin.x = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.origin.y"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->ucs.origin.y = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.origin.z"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->ucs.origin.z = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.axis_x.x"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->ucs.axis_x.x = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.axis_x.y"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->ucs.axis_x.y = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.axis_x.z"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->ucs.axis_x.z = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.axis_y.x"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->ucs.axis_y.x = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.axis_y.y"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->ucs.axis_y.y = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.axis_y.z"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->ucs.axis_y.z = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.axis_z.x"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->ucs.axis_z.x = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.axis_z.y"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->ucs.axis_z.y = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("ucs.axis_z.z"))
	{
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->ucs.axis_z.z = atof(tem_str);
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("work_norm.x"))
	{
		CString sTemp, sTemp1;
		pPropList->GetItemPropValue(CLDSBolt::GetPropID("work_norm.y"),sTemp);
		pPropList->GetItemPropValue(CLDSBolt::GetPropID("work_norm.z"),sTemp1);
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->set_norm(f3dPoint(atof(valueStr),atof(sTemp),atof(sTemp1)));
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("work_norm.y"))
	{
		CString sTemp, sTemp1;
		pPropList->GetItemPropValue(CLDSBolt::GetPropID("work_norm.x"),sTemp);
		pPropList->GetItemPropValue(CLDSBolt::GetPropID("work_norm.z"),sTemp1);
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->set_norm(f3dPoint(atof(sTemp),atof(valueStr),atof(sTemp1)));
	}
	else if(pItem->m_idProp==CLDSBolt::GetPropID("work_norm.z"))
	{
		CString sTemp, sTemp1;
		pPropList->GetItemPropValue(CLDSBolt::GetPropID("work_norm.x"),sTemp);
		pPropList->GetItemPropValue(CLDSBolt::GetPropID("work_norm.y"),sTemp1);
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			pBolt->set_norm(f3dPoint(atof(sTemp),atof(sTemp1),atof(valueStr)));
	}
	else 
		return FALSE;
	if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.nearVector.x")||pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.nearVector.y")||
		pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.nearVector.z")||pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.hCrossPart")||
		pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.hVicePart")||pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.norm_style")||
		pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.direction")||pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.norm_wing"))
	{	
		for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
		{	//����������Ʋ��������仯ʱ���¼��㷨�߷������˨λ��
			pBolt->SetModified();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
		}
		//��ָ�����ߵ���� ����������Ʋ��������仯ʱ��Ҫ�������Կ�����ʾ�ķ��߷���
		if(pFirstBolt->des_work_norm.norm_style!=0)	
		{
			CString sTextStr;
			SetPropItemReadOnly(pFirstBolt,"work_norm.x",pFirstBolt->GetObjPropReadOnlyDefaultState(CLDSBolt::GetPropID("work_norm.x")));
			SetPropItemReadOnly(pFirstBolt,"work_norm.y",pFirstBolt->GetObjPropReadOnlyDefaultState(CLDSBolt::GetPropID("work_norm.y")));
			SetPropItemReadOnly(pFirstBolt,"work_norm.z",pFirstBolt->GetObjPropReadOnlyDefaultState(CLDSBolt::GetPropID("work_norm.z")));
			sTextStr.Format("%f",pFirstBolt->ucs.axis_z.x);
			SimplifiedNumString(sTextStr);
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("work_norm.x"),sTextStr);
			sTextStr.Format("%f",pFirstBolt->ucs.axis_z.y);
			SimplifiedNumString(sTextStr);
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("work_norm.y"),sTextStr);
			sTextStr.Format("%f",pFirstBolt->ucs.axis_z.z);
			SimplifiedNumString(sTextStr);
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("work_norm.z"),sTextStr);
			SetPropItemReadOnly(pFirstBolt,"work_norm.x",TRUE);
			SetPropItemReadOnly(pFirstBolt,"work_norm.y",TRUE);
			SetPropItemReadOnly(pFirstBolt,"work_norm.z",TRUE);
		}
		//��˨���߳������仯��Ҫ�ڡ���λ���㡿��������ʾ"��ͨ��"������������˨ԭ������
		if(pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.direction"))
		{
			CString sTextStr("");
			if(pFirstBolt->m_dwFlag.IsHasFlag(CLDSBolt::REVERSE_NORM))
#ifdef AFX_TARG_ENU_ENGLISH
				sTextStr.Format("-L0");
#else 
				sTextStr.Format("-ͨ��");
#endif
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("des_base_pos.datumPoint"),sTextStr);
			//������˨ԭ������
			sTextStr.Format("%f",pFirstBolt->ucs.origin.x);
			SimplifiedNumString(sTextStr);
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("ucs.origin.x"),sTextStr);
			sTextStr.Format("%f",pFirstBolt->ucs.origin.y);
			SimplifiedNumString(sTextStr);
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("ucs.origin.y"),sTextStr);
			sTextStr.Format("%f",pFirstBolt->ucs.origin.z);
			SimplifiedNumString(sTextStr);
			pPropList->SetItemPropValue(CLDSBolt::GetPropID("ucs.origin.z"),sTextStr);
			SetPropItemReadOnly(pFirstBolt,"ucs.origin.x",TRUE);
			SetPropItemReadOnly(pFirstBolt,"ucs.origin.y",TRUE);
			SetPropItemReadOnly(pFirstBolt,"ucs.origin.z",TRUE);
		}
	}
	for(pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
	{
		pBolt->SetModified();
		HOLE_WALL* pHoleWall = console.GetHoleWall(pBolt);
		if(pBolt->is_visible)
		{
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
		else if(pHoleWall)
		{
			pHoleWall->UpdateSolid();
			g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
		}
		//�޸���˨λ�û�����˨ֱ������˨���ڰ���Ҫ�������
		if(pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.hPart")||
			pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.m_biWingOffsetStyle")||
			pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.len_offset_dist")||
			pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.wing_offset_dist")||
			pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.offset_wing")||
			pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.cResidePlane")||
			pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.direction")||
			pItem->m_idProp==CLDSBolt::GetPropID("des_work_norm.reside_plane")||
			pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.fLenOffDist")||
			pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.fWingOffDist1")||
			pItem->m_idProp==CLDSBolt::GetPropID("des_base_pos.fWingOffDist2"))
		{
			CLDSPlate *pPlate=NULL;
			for(pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
			{
				if(pPlate->FindLsByHandle(pBolt->handle))
				{	//�����������˨��صĸְ�
					pPlate->SetModified();
					pPlate->DesignPlate();
					pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				}
			}
		}
	}
	g_pSolidDraw->Draw();
	return TRUE;
}
BOOL FireBoltPopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(pView->selectObjs.GetNodeNum()==0)
		return FALSE;
	CXhChar100 sText;
	BOOL bFinished=FALSE,bStart=TRUE;
	CLDSBolt *pFirstBolt = (CLDSBolt*)pView->selectObjs.GetFirst();
	if(pItem->m_idProp==CLDSBolt::GetPropID("gasketThick"))
	{
		pItem->m_lpNodeInfo->SetCheck(iMenu+1);
		if(iMenu==0)	//�Զ�ƥ��
		{
			for(CLDSBolt* pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
			{
				pBolt->DianQuan.AutoMatchThick=true;
				pBolt->CalGuigeAuto();
			}
			pItem->m_lpNodeInfo->m_bMutiObjsProp=TRUE;
		}
		else
		{
			for(CLDSBolt* pBolt=(CLDSBolt*)pView->selectObjs.GetFirst();pBolt;pBolt=(CLDSBolt*)pView->selectObjs.GetNext())
				pBolt->DianQuan.AutoMatchThick=false;
			pItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSBolt::IsPropEqual(pView->selectObjs,pItem->m_idProp);;
		}

		CXhChar100 valueStr;
		if(pFirstBolt->GetPropValueStr(pItem->m_idProp,valueStr)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
		return TRUE;
	}
	return FALSE;
}
CString GetBlockListStr();
void CLDSView::DisplayBoltProperty(BOOL bRebuild)
{
	for(CLDSObject* pSelObj=selectObjs.GetFirst();pSelObj;pSelObj=selectObjs.GetNext())
	{	//DisplayBoltProperty��ObjectSnappedProcess�е��õ����Ҳ��ܱ�֤selectObjs��ȫΪ��˨ �����ڴ�ɸѡ wjh-2016.6.15
		if(pSelObj->GetClassTypeId()!=CLS_BOLT)
			selectObjs.DeleteCursor(FALSE);
	}
	hashBoltHoles.Empty();
	CLDSBolt *pFirstBolt = (CLDSBolt*)selectObjs.GetFirst();
	if(pFirstBolt==NULL)
		return;
	QUEUENODE_HOLE* pFirstHoleNode = NULL;
	for(PROP_OBJ* pPropObj=hashSelectObjs.GetFirst();pPropObj;pPropObj=hashSelectObjs.GetNext())
	{
		BOLTHOLE holeobj;
		QUEUENODE_HOLE* pQueueNode=hashBoltHoles.Add(pPropObj->pBolt->handle);
		if (pFirstBolt->handle == pPropObj->pBolt->handle)
			pFirstHoleNode = pQueueNode;
		holeobj.hResidePart=pPropObj->hResidePart;
		CLDSPart* pResidePart=Ta.FromPartHandle(pPropObj->hResidePart);
		if(pResidePart!=NULL)
			holeobj.pLsRef=pResidePart->FindLsByHandle(pPropObj->pBolt->handle);
		if(holeobj.pLsRef!=NULL)
		{
			if (pQueueNode->xQueueNode.pLsRef == NULL)
				pQueueNode->xQueueNode = holeobj;
			else
				pQueueNode->AppendListNode(holeobj);
		}
	}
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pGroupItem,*pPropItem,*pLeafItem,*pRoot=pPropList->GetRootItem();
	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_POS=2,HOLE_PROPERTY=3;
	CPropertyListOper<CLDSBolt> oper(pPropList,pFirstBolt);
	if(bRebuild)
	{	//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(pFirstHoleNode!=NULL?3:2);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"Position");
		if(pFirstHoleNode!=NULL)
			pPropDlg->m_arrPropGroupLabel.SetAt(HOLE_PROPERTY-1,"Hole");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"λ��");
		if(pFirstHoleNode!=NULL)
			pPropDlg->m_arrPropGroupLabel.SetAt(HOLE_PROPERTY-1,"��˨��");
#endif
		pPropDlg->RefreshTabCtrl(CLDSBolt::m_iCurDisplayPropGroup);

		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = CLS_BOLT;
		pPropList->SetModifyValueFunc(ModifyBoltProperty);
		pPropList->SetButtonClickFunc(BoltButtonClick);
		pPropList->SetModifyStatusFunc(ModifyBoltItemStatus);
		pPropList->SetPopMenuClickFunc(FireBoltPopMenuClick);
		pPropList->SetPropHelpPromptFunc(ModifyBoltPropItemHelpLiveString);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstBolt->BelongModel()->IsTowerModel()||pFirstBolt->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//��˨������Ϣ
		pGroupItem=oper.InsertPropItem(pRoot,"basicInfo");
		if(CLDSBolt::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSBolt::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(HOLE_PROPERTY);
		//���
		pPropItem=oper.InsertEditPropItem(pGroupItem,"handle");
		pPropItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(HOLE_PROPERTY);
		//ͼ��
		pPropItem=oper.InsertEditPropItem(pGroupItem,"layer");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(HOLE_PROPERTY);
		//��ĺ�
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"cfgword");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(HOLE_PROPERTY);
		//�ֶκ�
		pPropItem=oper.InsertEditPropItem(pGroupItem,"iSeg");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
		//�κ�ͳ�Ʒ�Χ
		pPropItem=oper.InsertEditPropItem(pGroupItem,"statSegStr");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��˨���
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_hFamily");
		pPropItem->m_lpNodeInfo->m_cmbItems=MakeBoltFamilySetString();
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(HOLE_PROPERTY);
		if(selectObjs.GetNodeNum()>1)
		{//��˨����
			pLeafItem=oper.InsertCmbListPropItem(pPropItem,"grade");
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
		}
		//��˨���
		pLeafItem=oper.InsertCmbEditPropItem(pPropItem,"specification");
		pLeafItem->m_lpNodeInfo->m_cmbItems=MakeBoltSetString(pFirstBolt->m_hFamily);
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS)|GetSingleWord(HOLE_PROPERTY);
		//��Ԥͳ��
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bVirtualPart");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
		//�ݿ׼�϶
		pPropItem=oper.InsertEditPropItem(pGroupItem,"hole_d_increment");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
		/////////////////////////////////////////////////////////////////////////
		if (pFirstHoleNode != NULL)
		{	//������
			pGroupItem = oper.InsertPropItem(pRoot, "xHolePropGroup");
			if(CLDSBolt::GetPropStatus(pGroupItem->m_idProp)!=-1)
				pGroupItem->m_bHideChildren = CLDSBolt::GetPropStatus(pGroupItem->m_idProp);
			pGroupItem->m_dwPropGroup = GetSingleWord(HOLE_PROPERTY);
			//�׾�
			pPropItem = oper.InsertEditPropItem(pGroupItem, "dfHoleDiameter");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs, pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(HOLE_PROPERTY);
			//��Բ����
			pPropItem = oper.InsertEditPropItem(pGroupItem, "siWaistLength");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs, pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(HOLE_PROPERTY);
			//��������
			pPropItem = oper.InsertEditPropItem(pGroupItem, "hHoleResidePart");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs, pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(HOLE_PROPERTY);
			pPropItem->SetReadOnly();
			//�׹�������
			pPropItem = oper.InsertCmbListPropItem(pGroupItem, "ciHoleFuncType");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs, pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(HOLE_PROPERTY);
			//�׹�������
			pPropItem = oper.InsertCmbListPropItem(pGroupItem, "ciManuProcessType");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs, pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(HOLE_PROPERTY);
			//�׹�������
			pLeafItem = oper.InsertCmbListPropItem(pPropItem, "blEnlargeHole");
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs, pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup = GetSingleWord(HOLE_PROPERTY);
		}
		/////////////////////////////////////////////////////////////////////////
		//��˨ͨ����㷽ʽ wht 19-08-23
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "m_bCalL0IncGapByInters");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs, pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//ͨ�����
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"L0");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//ͳ��ϵ��
		pPropItem=oper.InsertEditPropItem(pGroupItem,"m_uStatMatCoef");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//��Ȧ��Ϣ
		pPropItem=oper.InsertEditPropItem(pGroupItem,"gasketInfo");
		pPropItem->SetReadOnly();
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSBolt::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
		//��Ȧ���
		pLeafItem=oper.InsertBtnPopMenuEditPropItem(pPropItem,"gasketThick");
		pLeafItem->m_lpNodeInfo->m_controlType = PIT_BUTTON;
		pLeafItem->m_lpNodeInfo->m_buttonType = BDT_POPMENUEDIT;
		if(pFirstBolt->DianQuan.AutoMatchThick)
			pLeafItem->m_lpNodeInfo->SetCheck(1);	//Ϊ����Ĭ��ֵ����������1��ʼ wjh-2016.12.22
		else
			pLeafItem->m_lpNodeInfo->SetCheck(2);
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = pFirstBolt->DianQuan.AutoMatchThick||!CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
		//��Ȧ����
		pLeafItem=oper.InsertEditPropItem(pPropItem,"gasketN");
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
		//��Ȧλ��
		pLeafItem=oper.InsertEditPropItem(pPropItem,"gasketOffset");
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_POS);
		//�����ɵ�
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bAntiLooseWasher");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//������˨
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bAntiTheft");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//�û�ָ��������ɫ
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bUserSpecColor");
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstBolt->m_bUserSpecColor)
		{	//������ɫ
			oper.InsertCmbColorPropItem(pPropItem,"crMaterial");
		}
		//
		if(pFirstBolt->IsShadowObject())
		{
			//Ӱ����Ϣ
			pPropItem=oper.InsertButtonPropItem(pGroupItem,"shadowInfo");
			pPropItem->m_bHideChildren = FALSE;
			if(CLDSBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSBolt::GetPropStatus(pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstBolt->GetShadowObjType()==CLDSObject::SHADOW_ISOLATED_RELA_LOCATION);	//������λ��˨�����޸�Ӱ�������Ϣ
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//Ӱ��ĸ����
			pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"m_hMotherObj");
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
			pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstBolt->IsBlockRefShadowObject())
			{
				//������������
				pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"m_hBlockRef");
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
				pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else 
		{
			//��������
			pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_hBlock");
			pPropItem->m_lpNodeInfo->m_cmbItems=GetBlockListStr();
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//��������
			pPropItem=oper.InsertButtonPropItem(pGroupItem,"relativeObjs");
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		//��ע
		pPropItem=oper.InsertEditPropItem(pGroupItem,"sNotes");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		//װ�䷽λ
		pGroupItem=oper.InsertPropItem(pRoot,"assemblePos");
		if(CLDSBolt::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSBolt::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ�����λ��
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"ucs.origin");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSBolt::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//װ������ϵԭ��λ��X�������
		pLeafItem=oper.InsertEditPropItem(pPropItem,"ucs.origin.x");
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		pLeafItem->SetReadOnly(pFirstBolt->des_base_pos.datumPoint.datum_pos_style!=0||pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
		//װ������ϵԭ��λ��Y�������
		pLeafItem=oper.InsertEditPropItem(pPropItem,"ucs.origin.y");
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		pLeafItem->SetReadOnly(pFirstBolt->des_base_pos.datumPoint.datum_pos_style!=0||pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
		//װ������ϵԭ��λ��Z�������
		pLeafItem=oper.InsertEditPropItem(pPropItem,"ucs.origin.z");
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		pLeafItem->SetReadOnly(pFirstBolt->des_base_pos.datumPoint.datum_pos_style!=0||pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
		//װ��λ����Ʋ���
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"des_base_pos");
		if(CLDSBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSBolt::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		UpdateBoltDesPosProperty(pPropList,pPropItem);
		//װ��ʱX�᷽��
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"ucs.axis_x");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSBolt::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//X�᷽��X�������
		oper.InsertEditPropItem(pPropItem,"ucs.axis_x.x");
		//X�᷽��Y�������
		oper.InsertEditPropItem(pPropItem,"ucs.axis_x.y");
		//X�᷽��Z�������
		oper.InsertEditPropItem(pPropItem,"ucs.axis_x.z");
		//װ��ʱ�������߷���
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"work_norm");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSBolt::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//��������X�������
		pLeafItem=oper.InsertEditPropItem(pPropItem,"work_norm.x");
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		if(pFirstBolt->des_work_norm.norm_style!=0)
			pLeafItem->SetReadOnly();
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//��������Y�������
		pLeafItem=oper.InsertEditPropItem(pPropItem,"work_norm.y");
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		if(pFirstBolt->des_work_norm.norm_style!=0)
			pLeafItem->SetReadOnly();
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//��������Z�������
		pLeafItem=oper.InsertEditPropItem(pPropItem,"work_norm.z");
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		if(pFirstBolt->des_work_norm.norm_style!=0)
			pLeafItem->SetReadOnly();
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//�������߲���
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"des_work_norm");
		if(CLDSBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSBolt::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		//�������߼��㷽ʽ
		pLeafItem=oper.InsertCmbListPropItem(pPropItem,"des_work_norm.norm_style");
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
		pLeafItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
		Update_Bolt_Norm_CalStyle_TreeItem(pPropList,pPropItem);
	}
	else
	{
		//����Ҫ�ؽ���������ҳ
		CPropTreeItem *pFindItem, *pParentItem = NULL;
		SetPartMutiObjsProp(pFirstBolt,"handle");
		SetPartMutiObjsProp(pFirstBolt,"layer");
		SetPartMutiObjsProp(pFirstBolt,"m_hFamily");
		pFindItem=pPropList->FindItemByPropId(CLDSBolt::GetPropID("grade"),&pParentItem);
		if(pFindItem==NULL&&selectObjs.GetNodeNum()>1)
		{
			pParentItem=pPropList->FindItemByPropId(CLDSBolt::GetPropID("m_hFamily"),NULL);
			pPropItem=oper.InsertCmbListPropItem(pParentItem,"grade","","","",0,TRUE);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
		else
			SetPartMutiObjsProp(pFirstBolt,"grade");
		SetPartMutiObjsProp(pFirstBolt,"specification");
		SetPartMutiObjsProp(pFirstBolt,"L0");
		SetPartMutiObjsProp(pFirstBolt,"iSeg");
		SetPartMutiObjsProp(pFirstBolt,"cfgword");
		SetPartMutiObjsProp(pFirstBolt,"dwRayNo");
		SetPartMutiObjsProp(pFirstBolt,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstBolt,"gasketInfo");
		SetPartMutiObjsProp(pFirstBolt,"gasketThick");
		SetPartMutiObjsProp(pFirstBolt,"gasketN");
		SetPartMutiObjsProp(pFirstBolt,"gasketOffset");
		SetPartMutiObjsProp(pFirstBolt,"m_bVirtualPart");
		SetPartMutiObjsProp(pFirstBolt,"hole_d_increment");
		if (pFirstHoleNode != NULL)
		{	//������
			if(pFirstHoleNode!=NULL&&pPropDlg->m_arrPropGroupLabel.GetSize()==2)
				pPropDlg->m_arrPropGroupLabel.Add("��˨��");
			else if(pFirstHoleNode==NULL&&pPropDlg->m_arrPropGroupLabel.GetSize()==3)
				pPropDlg->m_arrPropGroupLabel.RemoveAt(HOLE_PROPERTY-1);
			SetPartMutiObjsProp(pFirstBolt,"dfHoleDiameter");	//�׾�
			SetPartMutiObjsProp(pFirstBolt,"siWaistLength");	//��Բ����
			SetPartMutiObjsProp(pFirstBolt,"hHoleResidePart");	//��������
			SetPartMutiObjsProp(pFirstBolt,"ciHoleFuncType");	//�׹�������
			SetPartMutiObjsProp(pFirstBolt,"ciManuProcessType");//�׹�������
			SetPartMutiObjsProp(pFirstBolt,"blEnlargeHole");	//�Ƿ���Ҫ���׹���
		}
		//SetPartMutiObjsProp(pFirstBolt,"m_bFootNail");
		//SetPartMutiObjsProp(pFirstBolt,"m_bTwoCapLs");
		SetPartMutiObjsProp(pFirstBolt,"m_bAntiLooseWasher");
		SetPartMutiObjsProp(pFirstBolt,"m_bAntiTheft");
		SetPartMutiObjsProp(pFirstBolt,"m_bUserSpecColor");
		pFindItem=pPropList->FindItemByPropId(CLDSBolt::GetPropID("m_hBlock"),&pParentItem);
		if (pFindItem)
		{
			if (pFirstBolt->IsShadowObject())
			{	//����ͨ�����л���Ӱ�乹��
				pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("m_hBlock"));
				pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("relativeObjs"));
				//Ӱ����Ϣ
				pPropItem=oper.InsertButtonPropItem(pParentItem,"shadowInfo","","",-1,TRUE);
				if(CLDSBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSBolt::GetPropStatus(pPropItem->m_idProp);
				pPropItem->SetReadOnly(pFirstBolt->GetShadowObjType()==CLDSObject::SHADOW_ISOLATED_RELA_LOCATION);	//������λ��˨�����޸�Ӱ�������Ϣ
				//Ӱ��ĸ����
				pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"m_hMotherObj","","",-1,TRUE);
				pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
				pLeafItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
				if (pFirstBolt->IsBlockRefShadowObject())
				{
					//������������
					pLeafItem=oper.InsertBtnEditPropItem(pPropItem,"m_hBlockRef","","",-1,TRUE);
					pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
					pLeafItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstBolt,"m_hBlock");
				SetPartMutiObjsProp(pFirstBolt,"relativeObjs");
			}
		}
		pFindItem=pPropList->FindItemByPropId(CLDSBolt::GetPropID("shadowInfo"),&pParentItem);
		if (pFindItem)
		{
			if (!pFirstBolt->IsShadowObject())
			{	//��Ӱ�乹���л�����ͨ����
				pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("shadowInfo"));
				pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("m_hMotherObj"));
				pFindItem=pPropList->FindItemByPropId(CLDSBolt::GetPropID("m_hBlockRef"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("m_hBlockRef"));
				pFindItem=pPropList->FindItemByPropId(CLDSBolt::GetPropID("m_hBlock"),NULL);
				if(pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstBolt,"m_hBlock");
				else
				{	//��������
					pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_hBlock","","","",-1,TRUE);
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
					pPropItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
				pFindItem=pPropList->FindItemByPropId(CLDSBolt::GetPropID("relativeObjs"),NULL);
				if(pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstBolt,"relativeObjs");
				else
					oper.InsertButtonPropItem(pParentItem,"relativeObjs","","",-1,TRUE);
			}
			else
			{
				SetPartMutiObjsProp(pFirstBolt,"m_hMotherObj");
				pFindItem=pPropList->FindItemByPropId(CLDSBolt::GetPropID("m_hBlockRef"),&pParentItem);
				if (pFindItem&&pFirstBolt->IsBlockRefShadowObject())
				{
					SetPartMutiObjsProp(pFirstBolt,"m_hBlockRef");
					SetPartMutiObjsProp(pFirstBolt,"m_hBlock");
				}
				else if (pFindItem&&!pFirstBolt->IsBlockRefShadowObject())
				{
					pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("m_hBlockRef"));
					pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("m_hBlock"));
				}
				else if(pFindItem==NULL&&pFirstBolt->IsBlockRefShadowObject())
				{
					pPropList->FindItemByPropId(CLDSBolt::GetPropID("m_hMotherObj"),&pParentItem);
					//������������
					pLeafItem=oper.InsertBtnEditPropItem(pParentItem,"m_hBlockRef","","",-1,TRUE);
					pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSBolt::IsPropEqual(selectObjs,pLeafItem->m_idProp);
					pLeafItem->SetReadOnly(pFirstBolt->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
				}
			}
		}
		if(pFirstBolt->m_bUserSpecColor)
		{	
			if(!SetPartMutiObjsProp(pFirstBolt,"crMaterial"))
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("m_bUserSpecColor"),NULL);
				if(pFindItem)
					oper.InsertCmbColorPropItem(pFindItem,"crMaterial","","","",-1,TRUE);
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSBolt::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstBolt,"sNotes");
		SetPartMutiObjsProp(pFirstBolt,"m_bCalL0IncGapByInters");
		SetPartMutiObjsProp(pFirstBolt,"ucs.origin.x");
		SetPartMutiObjsProp(pFirstBolt,"ucs.origin.y");
		SetPartMutiObjsProp(pFirstBolt,"ucs.origin.z");
		SetPartMutiObjsProp(pFirstBolt,"des_base_pos");
		pFindItem=pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_base_pos"),&pParentItem);
		if(pFindItem)
			UpdateBoltDesPosProperty(pPropList,pFindItem);
		SetPartMutiObjsProp(pFirstBolt,"ucs.axis_x.x");
		SetPartMutiObjsProp(pFirstBolt,"ucs.axis_x.y");
		SetPartMutiObjsProp(pFirstBolt,"ucs.axis_x.z");
		SetPartMutiObjsProp(pFirstBolt,"work_norm.x");
		SetPartMutiObjsProp(pFirstBolt,"work_norm.y");
		SetPartMutiObjsProp(pFirstBolt,"work_norm.z");
		SetPartMutiObjsProp(pFirstBolt,"des_work_norm.norm_style");
		pFindItem = pPropList->FindItemByPropId(CLDSBolt::GetPropID("des_work_norm"),&pParentItem);
		if(pFindItem)
			Update_Bolt_Norm_CalStyle_TreeItem(pPropList,pFindItem);
	}
	pPropList->Redraw();
}
//////////////////////////////////////////////////////////////////////////
//��ʾ�ҵ�����
void UpdateLevelProperty(CPropertyList *pPropList,CPropTreeItem* pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	PROP_OBJ* pPropObj=pView->hashSelectObjs.GetFirst();
	if(pPropObj==NULL ||pPropObj->ciObjType!=PROP_OBJ::TYPE_WIREPOINTARROW)
		return;
	CXhWirePoint* pWirePt=gxWirePointModel.m_hashWirePoints.GetValue(pPropObj->obj.subobj.hObj);
	if(pWirePt==NULL)
		return;
	CPropertyListOper<CXhWirePoint> oper(pPropList,pWirePt);
	pPropList->DeleteAllSonItems(pParentItem);
	CPropTreeItem *pPropItem=NULL,*pSonItem=NULL;
	if(pWirePt->m_ciLevel==0)
		oper.InsertButtonPropItem(pParentItem,"RelaNode","","",-1,TRUE);
	else
	{
		oper.InsertEditPropItem(pParentItem,"HoleD","","",-1,TRUE);
		pPropItem=oper.InsertButtonPropItem(pParentItem,"HOLE1","","",-1,TRUE);
		pSonItem=oper.InsertButtonPropItem(pPropItem,"HOLE1.hBolt","","",-1,TRUE);
		pSonItem->SetReadOnly();
		pSonItem=oper.InsertButtonPropItem(pPropItem,"HOLE1.hResidePart","","",-1,TRUE);
		pSonItem->SetReadOnly();
		pPropItem=oper.InsertButtonPropItem(pParentItem,"HOLE2","","",-1,TRUE);
		pSonItem=oper.InsertButtonPropItem(pPropItem,"HOLE2.hBolt","","",-1,TRUE);
		pSonItem->SetReadOnly();
		pSonItem=oper.InsertButtonPropItem(pPropItem,"HOLE2.hResidePart","","",-1,TRUE);
		pSonItem->SetReadOnly();
	}
}
BOOL ModifyWirePtProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	PROP_OBJ* pPropObj=pView->hashSelectObjs.GetFirst();
	if(pPropObj==NULL ||pPropObj->ciObjType!=PROP_OBJ::TYPE_WIREPOINTARROW)
		return FALSE;
	CXhWirePoint* pWirePt=gxWirePointModel.m_hashWirePoints.GetValue(pPropObj->obj.subobj.hObj);
	if(pWirePt==NULL)
		return FALSE;
	CPropertyListOper<CXhWirePoint> oper(pPropList,pWirePt);
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pItem->m_idProp==CXhWirePoint::GetPropID("GimName"))
		pWirePt->m_sGimName.Copy(tem_str);
	else if(pItem->m_idProp==CXhWirePoint::GetPropID("WireType"))
	{
		if(stricmp(tem_str,"����")==0)
			pWirePt->m_xWirePlace.ciWireType='C';
		else if(stricmp(tem_str,"����")==0)
			pWirePt->m_xWirePlace.ciWireType='E';
		else if(stricmp(tem_str,"����")==0)
			pWirePt->m_xWirePlace.ciWireType='J';
		//
		pPropList->DeleteAllSonItems(pItem);
		if(pWirePt->m_xWirePlace.ciWireType!='E')
			oper.InsertEditPropItem(pItem,"CircuitSerial","","",-1,TRUE);		//��·��
	}
	else if(pItem->m_idProp==CXhWirePoint::GetPropID("PhaseSerial"))
		pWirePt->m_xWirePlace.ciPhaseSerial=atoi(tem_str);
	else if(pItem->m_idProp==CXhWirePoint::GetPropID("CircuitSerial"))
		pWirePt->m_xWirePlace.ciCircuitSerial=atoi(tem_str);
	else if(pItem->m_idProp==CXhWirePoint::GetPropID("CircuitDC"))
	{
		if(stricmp(tem_str,"����")==0)
			pWirePt->m_xWirePlace.blCircuitDC=false;
		else
			pWirePt->m_xWirePlace.blCircuitDC=true;
	}
	else if(pItem->m_idProp==CXhWirePoint::GetPropID("TensionType"))
	{
		if(stricmp(tem_str,"����")==0)
			pWirePt->m_xWirePlace.ciTensionType=1;
		else if(stricmp(tem_str,"����")==0)
			pWirePt->m_xWirePlace.ciTensionType=2;
		else
			pWirePt->m_xWirePlace.ciTensionType=3;
	}
	else if(pItem->m_idProp==CXhWirePoint::GetPropID("HangingStyle"))
	{
		if(stricmp(tem_str,"����")==0)
			pWirePt->m_xWirePlace.ciHangingStyle=0;
		else if(stricmp(tem_str,"˫��")==0)
			pWirePt->m_xWirePlace.ciHangingStyle='S';
		else if(stricmp(tem_str,"V��")==0)
			pWirePt->m_xWirePlace.ciHangingStyle='V';
	}
	else if(pItem->m_idProp==CXhWirePoint::GetPropID("Level"))
	{
		pWirePt->m_ciLevel=valueStr[0]-'0';
		//
		UpdateLevelProperty(pPropList,pItem);
	}
	return TRUE;
}
BOOL WirePtButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	PROP_OBJ* pPropObj=pView->hashSelectObjs.GetFirst();
	if(pPropObj==NULL ||pPropObj->ciObjType!=PROP_OBJ::TYPE_WIREPOINTARROW)
		return FALSE;
	CXhWirePoint* pWirePt=gxWirePointModel.m_hashWirePoints.GetValue(pPropObj->obj.subobj.hObj);
	if(pWirePt==NULL)
		return FALSE;
	CReportVectorDlg vecDlg;
	CPropertyListOper<CXhWirePoint> oper(pPropList,pWirePt);
	if(pItem->m_idProp==CXhWirePoint::GetPropID("RelaNode"))
	{
		static CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸Ľڵ�";
		modifyhdlg.m_sTitle="ѡ��ڵ�:";
		modifyhdlg.m_sHandle.Format("0X%X",pWirePt->m_hRelaNode);
		modifyhdlg.m_bDispSelectBtn=TRUE;
		modifyhdlg.m_bEnableMir=FALSE;
		modifyhdlg.SetPickObjTypeId(CLS_NODE);
		if(modifyhdlg.DoModal()==IDOK)
			pWirePt->m_hRelaNode=modifyhdlg.m_nHandle;
	}
	else if(pItem->m_idProp==CXhWirePoint::GetPropID("HOLE1")||pItem->m_idProp==CXhWirePoint::GetPropID("HOLE2"))
	{
		static CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸���˨��";
		modifyhdlg.m_sTitle="ѡ����˨��:";
		modifyhdlg.m_sHandle.Format("0X%X",pWirePt->holes[0].hBolt);
		modifyhdlg.m_bDispSelectBtn=TRUE;
		modifyhdlg.m_bEnableMir=FALSE;
		modifyhdlg.SetPickObjTypeId(CLS_HOLEWALL);
		if(modifyhdlg.DoModal()!=IDOK)
			return FALSE;
		CLDSBolt* pBolt=NULL;
		CLDSPart* pPart=NULL;
		if(modifyhdlg.m_nHandle<0)
		{
			OBJECT_HANDLER handler;
			g_pSolidDraw->GetSolidObject(modifyhdlg.m_nHandle,&handler);
			pBolt=(CLDSBolt*)console.FromPartHandle(handler.hiberid.masterId,CLS_BOLT);
			if(pBolt)
				pPart=console.FromPartHandle(handler.hiberid.HiberUpId(2));
		}
		else
		{
			pBolt=(CLDSBolt*)console.FromPartHandle(modifyhdlg.m_nHandle,CLS_BOLT);
			if(pBolt)
				pPart=console.FromPartHandle(pBolt->des_base_pos.hPart);
		}
		if(pItem->m_idProp==CXhWirePoint::GetPropID("HOLE1"))
		{
			pWirePt->holes[0].hBolt=pBolt?pBolt->handle:0;
			pWirePt->holes[0].hResidePart=pPart?pPart->handle:0;
			oper.UpdatePropItemValue("HOLE1.hBolt");
			oper.UpdatePropItemValue("HOLE1.hResidePart");
		}
		else if(pItem->m_idProp==CXhWirePoint::GetPropID("HOLE2"))
		{
			pWirePt->holes[1].hBolt=pBolt?pBolt->handle:0;
			pWirePt->holes[1].hResidePart=pPart?pPart->handle:0;
			oper.UpdatePropItemValue("HOLE2.hBolt");
			oper.UpdatePropItemValue("HOLE2.hResidePart");
		}
	}
	else if(pItem->m_idProp==CXhWirePoint::GetPropID("ucs.orgion"))
	{
		vecDlg.m_sCaption= "��˨װ��λ��";
		vecDlg.m_fVectorX=pWirePt->acs.origin.x;
		vecDlg.m_fVectorY=pWirePt->acs.origin.y;
		vecDlg.m_fVectorZ=pWirePt->acs.origin.z;
		if(vecDlg.DoModal()==IDOK)
		{
			pWirePt->acs.origin.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("ucs.orgion.x");
			oper.UpdatePropItemValue("ucs.orgion.y");
			oper.UpdatePropItemValue("ucs.orgion.z");
		}
	}
	else if(pItem->m_idProp==CXhWirePoint::GetPropID("ucs.axis_x"))
	{
		vecDlg.m_sCaption= "�ؽ���˨X��";
		vecDlg.m_fVectorX=pWirePt->acs.axis_x.x;
		vecDlg.m_fVectorY=pWirePt->acs.axis_x.y;
		vecDlg.m_fVectorZ=pWirePt->acs.axis_x.z;
		if(vecDlg.DoModal()==IDOK)
		{
			pWirePt->acs.axis_x.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("ucs.axis_x.x");
			oper.UpdatePropItemValue("ucs.axis_x.y");
			oper.UpdatePropItemValue("ucs.axis_x.z");
		}
	}
	else if(pItem->m_idProp==CXhWirePoint::GetPropID("ucs.axis_y"))
	{
		vecDlg.m_sCaption= "�ؽ���˨Y��";
		vecDlg.m_fVectorX=pWirePt->acs.axis_y.x;
		vecDlg.m_fVectorY=pWirePt->acs.axis_y.y;
		vecDlg.m_fVectorZ=pWirePt->acs.axis_y.z;
		if(vecDlg.DoModal()==IDOK)
		{
			pWirePt->acs.axis_y.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("ucs.axis_y.x");
			oper.UpdatePropItemValue("ucs.axis_y.y");
			oper.UpdatePropItemValue("ucs.axis_y.z");
		}
	}
	else if(pItem->m_idProp==CXhWirePoint::GetPropID("ucs.axis_z"))
	{
		vecDlg.m_sCaption= "�ؽ���˨Z��";
		vecDlg.m_fVectorX=pWirePt->acs.axis_z.x;
		vecDlg.m_fVectorY=pWirePt->acs.axis_z.y;
		vecDlg.m_fVectorZ=pWirePt->acs.axis_z.z;
		if(vecDlg.DoModal()==IDOK)
		{
			pWirePt->acs.axis_z.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("ucs.axis_z.x");
			oper.UpdatePropItemValue("ucs.axis_z.y");
			oper.UpdatePropItemValue("ucs.axis_z.z");
		}
	}
	return TRUE;
}
void CLDSView::DisplayWirePointProperty(BOOL bRebuild)
{
	PROP_OBJ* pPropObj=hashSelectObjs.GetFirst();
	if(pPropObj==NULL ||pPropObj->ciObjType!=PROP_OBJ::TYPE_WIREPOINTARROW)
		return;
	CXhWirePoint* pWirePt=gxWirePointModel.m_hashWirePoints.GetValue(pPropObj->obj.subobj.hObj);
	if(pWirePt==NULL)
		return;
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	const int GROUP_GENERAL=1,GROUP_POS=2;
	if(bRebuild)
	{	
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(2);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"Position");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS-1,"λ��");
#endif
		pPropDlg->RefreshTabCtrl(CXhWirePoint::m_iCurDisplayPropGroup);
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId=CLS_WIREARROW;
		pPropList->SetModifyValueFunc(ModifyWirePtProperty);
		pPropList->SetButtonClickFunc(WirePtButtonClick);
	}
	//
	CPropertyListOper<CXhWirePoint> oper(pPropList,pWirePt);
	CPropTreeItem *pItem=NULL,*pGroupItem=NULL,*pPropItem=NULL,*pRoot=pPropList->GetRootItem();
	pGroupItem=oper.InsertPropItem(pRoot,"BasicInfo");
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"WireType","","","",-1,TRUE);
	if(pWirePt->m_xWirePlace.ciWireType!='E')
	{
		oper.InsertEditPropItem(pPropItem,"CircuitSerial","","",-1,TRUE);		//��·��
		//oper.InsertCmbListPropItem(pGroupItem,"CircuitDC","","","",-1,TRUE);	//��·����
	}
	oper.InsertEditPropItem(pGroupItem,"PhaseSerial","","",-1,TRUE);
	oper.InsertCmbListPropItem(pGroupItem,"TensionType","","","",-1,TRUE);
	oper.InsertCmbListPropItem(pGroupItem,"HangingStyle","","","",-1,TRUE);
	oper.InsertEditPropItem(pGroupItem,"GimName","","",-1,TRUE);
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"Level","","","",-1,TRUE);
	UpdateLevelProperty(pPropList,pPropItem);
	//
	pGroupItem=oper.InsertPropItem(pRoot,"Location");
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_POS);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"ucs.orgion","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.orgion.x","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.orgion.y","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.orgion.z","","",-1,TRUE);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"ucs.axis_x","","",-1,TRUE);
	pPropItem->m_bHideChildren=TRUE;
	oper.InsertEditPropItem(pPropItem,"ucs.axis_x.x","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_x.y","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_x.z","","",-1,TRUE);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"ucs.axis_y","","",-1,TRUE);
	pPropItem->m_bHideChildren=TRUE;
	oper.InsertEditPropItem(pPropItem,"ucs.axis_y.x","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_y.y","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_y.z","","",-1,TRUE);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"ucs.axis_z","","",-1,TRUE);
	pPropItem->m_bHideChildren=TRUE;
	oper.InsertEditPropItem(pPropItem,"ucs.axis_z.x","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_z.y","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_z.z","","",-1,TRUE);
	//
	pPropList->Redraw();
}
//////////////////////////////////////////////////////////////////////////
//��ʾ�������
BOOL ModifyFittingProperty(CPropertyList *pPropList, CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSFitting* pFitting = (CLDSFitting*)pView->selectObjs.GetFirst();
	if (pFitting == NULL)
		return FALSE;
	CPropertyListOper<CLDSFitting> oper(pPropList, pFitting);
	if (pItem->m_idProp == CLDSFitting::GetPropID("FittingType"))
	{
		BYTE ciType = valueStr[0] - '0';
		if (ciType == 0)
		{
			pFitting->m_ciFittingType = CLDSFitting::TYPE_EB;
			pFitting->sizespec.Copy(FittingLibrary<FITTING_EB>()->pxSizeTbl[0].szSizeCode);
		}
		else if (ciType == 1)
		{
			pFitting->m_ciFittingType = CLDSFitting::TYPE_GD;
			pFitting->sizespec.Copy(FittingLibrary<FITTING_GD>()->pxSizeTbl[0].szSizeCode);
		}
		else if (ciType == 2)
			pFitting->m_ciFittingType = CLDSFitting::TYPE_UR;
		else if (ciType == 3)
		{
			pFitting->m_ciFittingType = CLDSFitting::TYPE_US;
			pFitting->sizespec.Copy(FittingLibrary<FITTING_US>()->pxSizeTbl[0].szSizeCode);
		}
		else if (ciType == 4)
		{
			pFitting->m_ciFittingType = CLDSFitting::TYPE_UB;
			pFitting->sizespec.Copy(FittingLibrary<FITTING_UB>()->pxSizeTbl[0].szSizeCode);
		}
		//
		CPropTreeItem* pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("SizeCode"), NULL);
		if (pFindItem)
		{
			pFindItem->m_lpNodeInfo->m_cmbItems = MakeFittingSetString(pFitting->m_ciFittingType);
			oper.UpdatePropItemValue("SizeCode");
		}
	}
	else if (pItem->m_idProp == CLDSFitting::GetPropID("iSeg"))
		pFitting->iSeg = SEGI(valueStr.GetBuffer());
	else if (pItem->m_idProp == CLDSFitting::GetPropID("layer"))
		pFitting->SetLayer(valueStr.GetBuffer());
	else if (pItem->m_idProp == CLDSFitting::GetPropID("SizeCode"))
	{
		pFitting->sizespec.Copy(valueStr);
		pFitting->MatchUring();
		oper.UpdatePropItemValue("URSizeCode");
		UI::UpdatePartSolid(pFitting);
	}
	else if (pItem->m_idProp == CLDSFitting::GetPropID("URSizeCode"))
	{
		pFitting->sizespecUR.Copy(valueStr);
		UI::UpdatePartSolid(pFitting);
	}
	else if (pItem->m_idProp == CLDSFitting::GetPropID("SwingAngle"))
	{
		pFitting->m_iLiveSwingAngle = atoi(valueStr);
		UI::UpdatePartSolid(pFitting);
	}
	else if (pItem->m_idProp == CLDSFitting::GetPropID("MinSwingAngle"))
	{
		pFitting->m_siMinSwingAngle = atoi(valueStr);
		UI::UpdatePartSolid(pFitting);
	}
	else if (pItem->m_idProp == CLDSFitting::GetPropID("MaxSwingAngle"))
	{
		pFitting->m_siMaxSwingAngle = atoi(valueStr);
		UI::UpdatePartSolid(pFitting);
	}
	else if (pItem->m_idProp == CLDSFitting::GetPropID("URSwingAngle"))
	{
		pFitting->m_iURLiveSwingAngle = atoi(valueStr);
		UI::UpdatePartSolid(pFitting);
	}
	else if (pItem->m_idProp == CLDSFitting::GetPropID("MinURSwingAngle"))
	{
		pFitting->m_siMinURSwingAngle = atoi(valueStr);
		UI::UpdatePartSolid(pFitting);
	}
	else if (pItem->m_idProp == CLDSFitting::GetPropID("MaxURSwingAngle"))
	{
		pFitting->m_siMaxURSwingAngle = atoi(valueStr);
		UI::UpdatePartSolid(pFitting);
	}
	//
	pFitting->DesignFitting();
	pFitting->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pFitting->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return TRUE;
}
BOOL FittingButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CReportVectorDlg vecDlg;
	CPropTreeItem *pFindItem = NULL;
	CCfgPartNoDlg cfgdlg;
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSBolt *pFirstBolt = (CLDSBolt*)pView->selectObjs.GetFirst();
	CLDSFitting* pFitting=(CLDSFitting*)pView->selectObjs.GetFirst();
	if(pFitting==NULL)
		return FALSE;
	CPropertyListOper<CLDSFitting> oper(pPropList,pFitting);
	if(pItem->m_idProp==CLDSFitting::GetPropID("Hole1")||pItem->m_idProp==CLDSFitting::GetPropID("Hole2"))
	{
		static CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�޸���˨��";
		modifyhdlg.m_sTitle="ѡ����˨��:";
		modifyhdlg.m_sHandle.Format("0X%X",pFitting->holes[0].hBolt);
		modifyhdlg.m_bDispSelectBtn=TRUE;
		modifyhdlg.m_bEnableMir=FALSE;
		modifyhdlg.SetPickObjTypeId(CLS_HOLEWALL);
		if(modifyhdlg.DoModal()!=IDOK)
			return FALSE;
		CLDSBolt *pBolt = NULL;
		CLDSPart* pPart=NULL;
		if(modifyhdlg.m_nHandle<0)
		{
			OBJECT_HANDLER handler;
			g_pSolidDraw->GetSolidObject(modifyhdlg.m_nHandle,&handler);
			pBolt=(CLDSBolt*)console.FromPartHandle(handler.hiberid.masterId,CLS_BOLT);
			if(pBolt)
				pPart=console.FromPartHandle(handler.hiberid.HiberUpId(2));
		}
		else
		{
			pBolt=(CLDSBolt*)console.FromPartHandle(modifyhdlg.m_nHandle,CLS_BOLT);
			if(pBolt)
				pPart=console.FromPartHandle(pBolt->des_base_pos.hPart);
		}
		if(pItem->m_idProp==CLDSFitting::GetPropID("Hole1"))
		{
			pFitting->holes[0].hBolt=pBolt?pBolt->handle:0;
			pFitting->holes[0].hResidePart=pPart?pPart->handle:0;
			oper.UpdatePropItemValue("Hole1.hBolt");
			oper.UpdatePropItemValue("Hole1.hPart");
		}
		else if(pItem->m_idProp==CLDSFitting::GetPropID("Hole2"))
		{
			pFitting->holes[1].hBolt=pBolt?pBolt->handle:0;
			pFitting->holes[1].hResidePart=pPart?pPart->handle:0;
			oper.UpdatePropItemValue("Hole2.hBolt");
			oper.UpdatePropItemValue("Hole2.hPart");
		}
	}
	else if (pItem->m_idProp == CLDSFitting::GetPropID("ucs.origin"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "Bolt Assembling Position";
#else 
		vecDlg.m_sCaption = "���λ��";
#endif
		vecDlg.m_fVectorX = pFitting->ucs.origin.x;
		vecDlg.m_fVectorY = pFitting->ucs.origin.y;
		vecDlg.m_fVectorZ = pFitting->ucs.origin.z;
		pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("ucs.origin.x"), NULL);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("ucs.origin.y"), NULL);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("ucs.origin.z"), NULL);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if (vecDlg.DoModal() == IDOK)
		{
			pFitting->ucs.origin.Set(vecDlg.m_fVectorX, vecDlg.m_fVectorY, vecDlg.m_fVectorZ);
			//ˢ�����Խ���
			oper.UpdatePropItemValue("ucs.origin.x");
			oper.UpdatePropItemValue("ucs.origin.y");
			oper.UpdatePropItemValue("ucs.origin.z");
		}
	}
	else if (pItem->m_idProp == CLDSFitting::GetPropID("ucs.axis_x"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "X Datum Axis";
#else 
		vecDlg.m_sCaption = "���׼��";
#endif
		vecDlg.m_fVectorX = pFitting->ucs.axis_x.x;
		vecDlg.m_fVectorY = pFitting->ucs.axis_x.y;
		vecDlg.m_fVectorZ = pFitting->ucs.axis_x.z;
		pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("ucs.axis_x.x"), NULL);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("ucs.axis_x.y"), NULL);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("ucs.axis_x.z"), NULL);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();

		if (vecDlg.DoModal() == IDOK)
		{
			pFitting->ucs.axis_x.Set(vecDlg.m_fVectorX, vecDlg.m_fVectorY, vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("ucs.axis_x.x");
			oper.UpdatePropItemValue("ucs.axis_x.y");
			oper.UpdatePropItemValue("ucs.axis_x.z");
		}
	}
	else if (pItem->m_idProp == CLDSFitting::GetPropID("ucs.axis_y"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "X Datum Axis";
#else 
		vecDlg.m_sCaption = "���׼��";
#endif
		vecDlg.m_fVectorX = pFitting->ucs.axis_y.x;
		vecDlg.m_fVectorY = pFitting->ucs.axis_y.y;
		vecDlg.m_fVectorZ = pFitting->ucs.axis_y.z;
		pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("ucs.axis_y.x"), NULL);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("ucs.axis_y.y"), NULL);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("ucs.axis_y.z"), NULL);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if (vecDlg.DoModal() == IDOK)
		{
			pFitting->ucs.axis_y.Set(vecDlg.m_fVectorX, vecDlg.m_fVectorY, vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("ucs.axis_y.x");
			oper.UpdatePropItemValue("ucs.axis_y.y");
			oper.UpdatePropItemValue("ucs.axis_y.z");
		}
	}
	else if (pItem->m_idProp == CLDSFitting::GetPropID("ucs.axis_z"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		vecDlg.m_sCaption = "X Datum Axis";
#else 
		vecDlg.m_sCaption = "���׼��";
#endif
		vecDlg.m_fVectorX = pFitting->ucs.axis_z.x;
		vecDlg.m_fVectorY = pFitting->ucs.axis_z.y;
		vecDlg.m_fVectorZ = pFitting->ucs.axis_z.z;
		pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("ucs.axis_z.x"), NULL);
		vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("ucs.axis_z.y"), NULL);
		vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
		pFindItem = pPropList->FindItemByPropId(CLDSFitting::GetPropID("ucs.axis_z.z"), NULL);
		vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
		if (vecDlg.DoModal() == IDOK)
		{
			pFitting->ucs.axis_z.Set(vecDlg.m_fVectorX, vecDlg.m_fVectorY, vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("ucs.axis_z.x");
			oper.UpdatePropItemValue("ucs.axis_z.y");
			oper.UpdatePropItemValue("ucs.axis_z.z");
		}
	}
	pFitting->DesignFitting();
	pFitting->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pFitting->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return TRUE;
}
BOOL FireFittingHelpStr(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSFitting* pFitting=(CLDSFitting*)pView->selectObjs.GetFirst();
	if(pFitting==NULL)
		return FALSE;
	CLDSView::PROMPT_LIFE lifeprompt(true);
	if(pItem->m_idProp==CLDSFitting::GetPropID("FittingType"))
	{
		if(pFitting->m_ciFittingType==CLDSFitting::TYPE_EB)
			lifeprompt.SetPromptPicture(IDB_BMP_EB);
		else if(pFitting->m_ciFittingType==CLDSFitting::TYPE_GD)
			lifeprompt.SetPromptPicture(IDB_BMP_GD);
	}
	return TRUE;
}
void CLDSView::DisplayFittingProperty(BOOL bRebuild)
{
	for(CLDSObject* pSelObj=selectObjs.GetFirst();pSelObj;pSelObj=selectObjs.GetNext())
	{	
		if(pSelObj->GetClassTypeId()!=CLS_FITTINGS)
			selectObjs.DeleteCursor(FALSE);
	}
	CLDSFitting* pFitting=(CLDSFitting*)selectObjs.GetFirst();
	if(pFitting==NULL)
		return;
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	const int GROUP_GENERAL=1, GROUP_POS =2;
	if(bRebuild)
	{	
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(2);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"Profile");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_POS -1,"λ��");
#endif
		pPropDlg->RefreshTabCtrl(CLDSFitting::m_iCurDisplayPropGroup);
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId=CLS_FITTINGS;
		pPropList->SetModifyValueFunc(ModifyFittingProperty);
		pPropList->SetButtonClickFunc(FittingButtonClick);
		pPropList->SetPropHelpPromptFunc(FireFittingHelpStr);
	}
	CPropertyListOper<CLDSFitting> oper(pPropList,pFitting);
	CPropTreeItem *pItem=NULL,*pPropItem=NULL,*pGroupItem=NULL, *pLeafItem, *pRoot=pPropList->GetRootItem();
	pGroupItem=oper.InsertPropItem(pRoot,"BasicInfo");
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	pPropItem = oper.InsertEditPropItem(pGroupItem,"handle","","",-1,TRUE);
	pPropItem->SetReadOnly();
	pPropItem = oper.InsertEditPropItem(pGroupItem, "layer");
	oper.InsertEditPropItem(pGroupItem, "iSeg", "", "",  -1, TRUE);
	oper.InsertCmbListPropItem(pGroupItem, "FittingType", "", "", "", -1, TRUE);
	oper.InsertCmbListPropItem(pGroupItem,"SizeCode",MakeFittingSetString(pFitting->m_ciFittingType),"","",-1,TRUE);
	
	if(pFitting->m_ciFittingType!=CLDSFitting::TYPE_UR)
	{
		pPropItem=oper.InsertEditPropItem(pGroupItem,"SwingAngle");
		pPropItem->m_bHideChildren = FALSE;
		oper.InsertEditPropItem(pPropItem,"MinSwingAngle");
		oper.InsertEditPropItem(pPropItem,"MaxSwingAngle");
		oper.InsertCmbListPropItem(pGroupItem,"URSizeCode",MakeFittingSetString(CLDSFitting::TYPE_UR),"","",-1,TRUE);
		pPropItem=oper.InsertEditPropItem(pGroupItem,"URSwingAngle");
		pPropItem->m_bHideChildren = FALSE;
		oper.InsertEditPropItem(pPropItem,"MinURSwingAngle");
		oper.InsertEditPropItem(pPropItem,"MaxURSwingAngle");
	}
	pPropItem = oper.InsertButtonPropItem(pGroupItem, "Hole1");
	pItem = oper.InsertButtonPropItem(pPropItem, "Hole1.hBolt", "", "", -1, TRUE);
	pItem->SetReadOnly();
	pItem = oper.InsertButtonPropItem(pPropItem, "Hole1.hPart", "", "", -1, TRUE);
	pItem->SetReadOnly();
	pPropItem = oper.InsertButtonPropItem(pGroupItem, "Hole2");
	pItem = oper.InsertButtonPropItem(pPropItem, "Hole2.hBolt", "", "", -1, TRUE);
	pItem->SetReadOnly();
	pItem = oper.InsertButtonPropItem(pPropItem, "Hole2.hPart", "", "", -1, TRUE);
	pItem->SetReadOnly();
	//�ߴ���Ϣ
	pGroupItem = oper.InsertPropItem(pRoot, "Profile");
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
	if (pFitting->m_ciFittingType == 0)
	{	//EB
		pPropItem = oper.InsertEditPropItem(pGroupItem, "eb.Thick", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "eb.ThroughS", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "eb.TrunnionD", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "eb.HoleD", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "eb.BoardL", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "eb.BoardH", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "eb.ChinR", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
	}
	else if (pFitting->m_ciFittingType == 1)
	{	//GD
		pPropItem = oper.InsertEditPropItem(pGroupItem, "gd.Thick", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "gd.ThroughS", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "gd.TrunnionD", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "gd.TrunnionL", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "gd.HoleD", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "gd.BoardL", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "gd.BoardH", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "gd.BoardH1", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
		pPropItem = oper.InsertEditPropItem(pGroupItem, "gd.BoardH2", "", "", -1, TRUE);
		pPropItem->SetReadOnly();
	}
	else
	{

	}
	//װ�䷽λ
	pGroupItem = oper.InsertPropItem(pRoot, "assemblePos");
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_POS);
	//װ�����λ��
	pPropItem = oper.InsertButtonPropItem(pGroupItem, "ucs.origin");
	
	pLeafItem = oper.InsertEditPropItem(pPropItem, "ucs.origin.x");
	pLeafItem->SetReadOnly();
	pLeafItem = oper.InsertEditPropItem(pPropItem, "ucs.origin.y");
	pLeafItem->SetReadOnly();
	pLeafItem = oper.InsertEditPropItem(pPropItem, "ucs.origin.z");
	pLeafItem->SetReadOnly();
	//װ��ʱX�᷽��
	pPropItem = oper.InsertButtonPropItem(pGroupItem, "ucs.axis_x");
	//X�᷽��X�������
	pLeafItem = oper.InsertEditPropItem(pPropItem, "ucs.axis_x.x");
	pLeafItem->SetReadOnly();
	//X�᷽��Y�������
	pLeafItem = oper.InsertEditPropItem(pPropItem, "ucs.axis_x.y");
	pLeafItem->SetReadOnly();
	//X�᷽��Z�������
	pLeafItem = oper.InsertEditPropItem(pPropItem, "ucs.axis_x.z");
	pLeafItem->SetReadOnly();
	//װ��ʱY�᷽��
	pPropItem = oper.InsertButtonPropItem(pGroupItem, "ucs.axis_y");
	//Y�᷽��X�������
	pLeafItem = oper.InsertEditPropItem(pPropItem, "ucs.axis_y.x");
	pLeafItem->SetReadOnly();
	//Y�᷽��Y�������
	pLeafItem = oper.InsertEditPropItem(pPropItem, "ucs.axis_y.y");
	pLeafItem->SetReadOnly();
	//Y�᷽��Z�������
	pLeafItem = oper.InsertEditPropItem(pPropItem, "ucs.axis_y.z");
	pLeafItem->SetReadOnly();
	//װ��ʱZ�᷽��
	pPropItem = oper.InsertButtonPropItem(pGroupItem, "ucs.axis_z");
	//Z�᷽��X�������
	pLeafItem = oper.InsertEditPropItem(pPropItem, "ucs.axis_z.x");
	pLeafItem->SetReadOnly();
	//Z�᷽��Y�������
	pLeafItem = oper.InsertEditPropItem(pPropItem, "ucs.axis_z.y");
	pLeafItem->SetReadOnly();
	//Z�᷽��Z�������
	pLeafItem = oper.InsertEditPropItem(pPropItem, "ucs.axis_z.z");
	pLeafItem->SetReadOnly();
	
	//
	pPropList->Redraw();
}
//////////////////////////////////////////////////////////////////////////
//��ʾ�ؽ���˨����
BOOL ModifyAnchorBoltProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSAnchorBolt* pAnchorBolt=(CLDSAnchorBolt*)pView->selectObjs.GetFirst();
	if(pAnchorBolt==NULL)
		return FALSE;
	if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("layer"))
		pAnchorBolt->SetLayer(valueStr.GetBuffer());
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("iSeg"))
		pAnchorBolt->iSeg=SEGI(valueStr.GetBuffer());
	else if (pItem->m_idProp==CLDSAnchorBolt::GetPropID("d"))
	{
		pAnchorBolt->set_d(atoi(valueStr));
		const ANCHOR_BOLT* pAnchorInfo=ANCHOR_BOLT::FromBoltD(pAnchorBolt->d);
		pAnchorBolt->washer.wiWidth=pAnchorInfo->wiWidth;	//���������
		pAnchorBolt->washer.wiThick=pAnchorInfo->wiThick;	//���������
		pAnchorBolt->washer.fHoleIncrement=pAnchorInfo->wiHoleIncrement;	//���׾���Եؽ���˨ֱ��������ֵ
		pAnchorBolt->hole_d_increment=pAnchorInfo->wiBasePlateHoleD-pAnchorInfo->d;	//�׽Ű��϶�Ӧ�׾�
		pAnchorBolt->m_fNutH=pAnchorInfo->m_fNutH;			//��ĸ�߶�
		pAnchorBolt->m_fNutTaper=pAnchorInfo->m_fNutTaper;	//��ĸ�������Բ׶����¶�h/w;��=15��~30��
		pAnchorBolt->m_fNutDe=pAnchorInfo->m_fNutDe;		//��ĸ���㵽�������
		pAnchorBolt->m_fNutDs=pAnchorInfo->m_fNutDs;		//��ĸ�ߵ��߾���
		pAnchorBolt->m_fLe=pAnchorInfo->m_fLe;				//��¶��(mm)
		//pAnchorBolt->m_fLa=pAnchorInfo->m_fLa;			//�޿۳�(mm) <��������
		//pAnchorBolt->m_fLr=?;	//��������֮�µ�����(root)
		CPropertyListOper<CLDSAnchorBolt> oper(pPropList,pAnchorBolt);
		oper.UpdatePropItemValue("Le");
		oper.UpdatePropItemValue("Lr");
		oper.UpdatePropItemValue("NutH");
		oper.UpdatePropItemValue("NutDe");
		oper.UpdatePropItemValue("NutDs");
		oper.UpdatePropItemValue("washer.Width");
		oper.UpdatePropItemValue("washer.Thick");
		oper.UpdatePropItemValue("washer.HoleIncrement");
	}
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("Le"))
		pAnchorBolt->m_fLe=(float)atof(valueStr);
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("Lr"))
		pAnchorBolt->m_fLr=(float)atof(valueStr);
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("NutH"))
		pAnchorBolt->m_fNutH=(float)atof(valueStr);
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("NutDe"))
		pAnchorBolt->m_fNutDe=(float)atof(valueStr);
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("NutDs"))
		pAnchorBolt->m_fNutDs=(float)atof(valueStr);
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("washer.Width"))
		pAnchorBolt->washer.wiWidth=atoi(valueStr);
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("washer.Thick"))
		pAnchorBolt->washer.wiThick=atoi(valueStr);
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("washer.HoleIncrement"))
		pAnchorBolt->washer.fHoleIncrement=(float)atof(valueStr);
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("Foundation.HeightSeria"))
		pAnchorBolt->Foundation.ciHeightSerial=atoi(valueStr);
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("Foundation.LegSerial"))
		pAnchorBolt->Foundation.ciLegSerial=atoi(valueStr);
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("Foundation.LegQuad"))
		pAnchorBolt->Foundation.ciLegQuad=atoi(valueStr);
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("Foundation.BoltSerial"))
		pAnchorBolt->Foundation.ciBoltSerial=atoi(valueStr);
	//
	pAnchorBolt->SetModified();
	pAnchorBolt->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pAnchorBolt->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return TRUE;
}
BOOL AnchorBoltButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSAnchorBolt* pAnchorBolt=(CLDSAnchorBolt*)pView->selectObjs.GetFirst();
	if(pAnchorBolt==NULL)
		return FALSE;
	CCfgPartNoDlg cfgdlg;
	CReportVectorDlg vecDlg;
	CPropertyListOper<CLDSAnchorBolt> oper(pPropList,pAnchorBolt);
	if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("cfgword"))
	{
		if(pAnchorBolt->layer(0)!='L')
			cfgdlg.cfg_style=CFG_BODY_NO;
		else
			cfgdlg.cfg_style=CFG_LEG_NO;
		cfgdlg.cfgword = pAnchorBolt->cfgword; 
		if(cfgdlg.DoModal()==IDOK)
		{
			pAnchorBolt->cfgword = cfgdlg.cfgword;
			oper.UpdatePropItemValue("cfgword");
		}
	}
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("ucs.orgion"))
	{
		vecDlg.m_sCaption= "��˨װ��λ��";
		vecDlg.m_fVectorX=pAnchorBolt->ucs.origin.x;
		vecDlg.m_fVectorY=pAnchorBolt->ucs.origin.y;
		vecDlg.m_fVectorZ=pAnchorBolt->ucs.origin.z;
		if(vecDlg.DoModal()==IDOK)
		{
			pAnchorBolt->ucs.origin.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("ucs.orgion.x");
			oper.UpdatePropItemValue("ucs.orgion.y");
			oper.UpdatePropItemValue("ucs.orgion.z");
		}
	}
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("ucs.axis_x"))
	{
		vecDlg.m_sCaption= "�ؽ���˨X��";
		vecDlg.m_fVectorX=pAnchorBolt->ucs.axis_x.x;
		vecDlg.m_fVectorY=pAnchorBolt->ucs.axis_x.y;
		vecDlg.m_fVectorZ=pAnchorBolt->ucs.axis_x.z;
		if(vecDlg.DoModal()==IDOK)
		{
			pAnchorBolt->ucs.axis_x.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("ucs.axis_x.x");
			oper.UpdatePropItemValue("ucs.axis_x.y");
			oper.UpdatePropItemValue("ucs.axis_x.z");
		}
	}
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("ucs.axis_y"))
	{
		vecDlg.m_sCaption= "�ؽ���˨Y��";
		vecDlg.m_fVectorX=pAnchorBolt->ucs.axis_y.x;
		vecDlg.m_fVectorY=pAnchorBolt->ucs.axis_y.y;
		vecDlg.m_fVectorZ=pAnchorBolt->ucs.axis_y.z;
		if(vecDlg.DoModal()==IDOK)
		{
			pAnchorBolt->ucs.axis_y.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("ucs.axis_y.x");
			oper.UpdatePropItemValue("ucs.axis_y.y");
			oper.UpdatePropItemValue("ucs.axis_y.z");
		}
	}
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("ucs.axis_z"))
	{
		vecDlg.m_sCaption= "�ؽ���˨Z��";
		vecDlg.m_fVectorX=pAnchorBolt->ucs.axis_z.x;
		vecDlg.m_fVectorY=pAnchorBolt->ucs.axis_z.y;
		vecDlg.m_fVectorZ=pAnchorBolt->ucs.axis_z.z;
		if(vecDlg.DoModal()==IDOK)
		{
			pAnchorBolt->ucs.axis_z.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("ucs.axis_z.x");
			oper.UpdatePropItemValue("ucs.axis_z.y");
			oper.UpdatePropItemValue("ucs.axis_z.z");
		}
	}
	else if(pItem->m_idProp==CLDSAnchorBolt::GetPropID("basePlate"))
	{
		static CModifyPartHandleDlg modifyhdlg;
		modifyhdlg.m_sCaption="�ؽŻ���";
		modifyhdlg.m_sTitle="ѡ��ؽŻ���:";
		modifyhdlg.m_sHandle.Format("0X%X",pAnchorBolt->baseplate.hBasePlate);
		modifyhdlg.m_bDispSelectBtn=TRUE;
		modifyhdlg.m_bEnableMir=FALSE;
		modifyhdlg.SetPickObjTypeId(CLS_PARAMPLATE);
		if(modifyhdlg.DoModal()!=IDOK)
			return FALSE;
		CLDSParamPlate* pBasePlate=(CLDSParamPlate*)console.FromPartHandle(modifyhdlg.m_nHandle,CLS_PARAMPLATE);
		if(pBasePlate && pBasePlate->m_iParamType==TYPE_BASEPLATE)
		{
			pAnchorBolt->baseplate.hBasePlate=pBasePlate->handle;
			pAnchorBolt->baseplate.wiBasePlateThick=(WORD)pBasePlate->thick;
			oper.UpdatePropItemValue("basePlate.Thick");
		}
	}
	//
	pAnchorBolt->SetModified();
	pAnchorBolt->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pAnchorBolt->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return TRUE;
}
CString MakeAnchorBoltSetString()
{
	CString szListSize;
	int i,nCount=ANCHOR_BOLT::GetCount();
	for(i=0;i<nCount;i++)
	{	
		const ANCHOR_BOLT *pAnchorBolt=ANCHOR_BOLT::RecordAt(i);
		if (i>0)
			szListSize.AppendChar('|');
		szListSize+=CXhChar16("%d",pAnchorBolt->d);
	}
	return szListSize;
}
void CLDSView::DisplayAnchorBoltProperty(BOOL bRebuild)
{
	for(CLDSObject* pSelObj=selectObjs.GetFirst();pSelObj;pSelObj=selectObjs.GetNext())
	{	
		if(pSelObj->GetClassTypeId()!=CLS_ANCHORBOLT)
			selectObjs.DeleteCursor(FALSE);
	}
	CLDSAnchorBolt* pAnchorBolt=(CLDSAnchorBolt*)selectObjs.GetFirst();
	if(pAnchorBolt==NULL)
		return;
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	const int GROUP_GENERAL=1,GROUP_LOCATION=2;
	if(bRebuild)
	{	
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(2);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_LOCATION,"Location");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_LOCATION-1,"��λ");
#endif
		pPropDlg->RefreshTabCtrl(CLDSAnchorBolt::m_iCurDisplayPropGroup);
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId=CLS_ANCHORBOLT;
		pPropList->SetModifyValueFunc(ModifyAnchorBoltProperty);
		pPropList->SetButtonClickFunc(AnchorBoltButtonClick);
	}
	CPropertyListOper<CLDSAnchorBolt> oper(pPropList,pAnchorBolt);
	CPropTreeItem *pItem=NULL,*pPropItem=NULL,*pGroupItem=NULL,*pRoot=pPropList->GetRootItem();
	pGroupItem=oper.InsertPropItem(pRoot,"BasicInfo");
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	pPropItem=oper.InsertEditPropItem(pGroupItem,"handle","","",-1,TRUE);
	pPropItem->SetReadOnly();
	oper.InsertEditPropItem(pGroupItem,"layer","","",-1,TRUE);
	oper.InsertButtonPropItem(pGroupItem,"cfgword","","",-1,TRUE);
	oper.InsertEditPropItem(pGroupItem,"iSeg","","",-1,TRUE);
	CString szCmbListItems=MakeAnchorBoltSetString();
	pPropItem=oper.InsertCmbEditPropItem(pGroupItem,"d",szCmbListItems,"","",-1,TRUE);
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSAnchorBolt::IsPropEqual(selectObjs,pPropItem->m_idProp);
	oper.InsertEditPropItem(pGroupItem,"Le","","",-1,TRUE);
	oper.InsertEditPropItem(pGroupItem,"Lr","","",-1,TRUE);
	oper.InsertEditPropItem(pGroupItem,"NutH","","",-1,TRUE);
	oper.InsertEditPropItem(pGroupItem,"NutDe","","",-1,TRUE);
	oper.InsertEditPropItem(pGroupItem,"NutDs","","",-1,TRUE);
	pPropItem=oper.InsertEditPropItem(pGroupItem,"washer","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"washer.Width","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"washer.Thick","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"washer.HoleIncrement","","",-1,TRUE);
	pGroupItem=oper.InsertPropItem(pRoot,"Location");
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_LOCATION);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"ucs.orgion","","",-1,TRUE);
	pPropItem->m_bHideChildren=TRUE;
	if (CLDSAnchorBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSAnchorBolt::GetPropStatus(pPropItem->m_idProp);
	oper.InsertEditPropItem(pPropItem,"ucs.orgion.x","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.orgion.y","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.orgion.z","","",-1,TRUE);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"ucs.axis_x","","",-1,TRUE);
	pPropItem->m_bHideChildren=TRUE;
	if (CLDSAnchorBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSAnchorBolt::GetPropStatus(pPropItem->m_idProp);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_x.x","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_x.y","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_x.z","","",-1,TRUE);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"ucs.axis_y","","",-1,TRUE);
	pPropItem->m_bHideChildren=TRUE;
	if (CLDSAnchorBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSAnchorBolt::GetPropStatus(pPropItem->m_idProp);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_y.x","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_y.y","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_y.z","","",-1,TRUE);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"ucs.axis_z","","",-1,TRUE);
	pPropItem->m_bHideChildren=TRUE;
	if (CLDSAnchorBolt::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren = CLDSAnchorBolt::GetPropStatus(pPropItem->m_idProp);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_z.x","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_z.y","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ucs.axis_z.z","","",-1,TRUE);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"basePlate","","",-1,TRUE);
	pItem=oper.InsertEditPropItem(pPropItem,"basePlate.Thick","","",-1,TRUE);
	pItem->SetReadOnly(TRUE);
	pPropItem=oper.InsertEditPropItem(pGroupItem,"Foundation","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"Foundation.HeightSeria","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"Foundation.LegSerial","","",-1,TRUE);
	oper.InsertCmbListPropItem(pPropItem,"Foundation.LegQuad","1|2|3|4","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"Foundation.BoltSerial","","",-1,TRUE);
	//
	pPropList->Redraw();
}
//////////////////////////////////////////////////////////////////////////
//��ʾ��������
BOOL ModifyFoundationProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSFoundation* pFoundation=(CLDSFoundation*)pView->selectObjs.GetFirst();
	if(pFoundation==NULL)
		return FALSE;
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if (pItem->m_idProp == CLDSFoundation::GetPropID("HeightSerial"))
		pFoundation->ciHeightSerial = valueStr[0] - '0';
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("LegSerial"))
		pFoundation->ciLegSerial = valueStr[0] - '0';
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("LegQuad"))
		pFoundation->ciLegQuad = valueStr[0] - '0';
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("Width"))
		pFoundation->wiWidth = atoi(valueStr);
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("Height"))
		pFoundation->wiHeight = atoi(valueStr);
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("location.x"))
		pFoundation->location.x = atof(valueStr);
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("location.y"))
		pFoundation->location.y = atof(valueStr);
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("location.z"))
		pFoundation->location.z = atof(valueStr);
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("WorkNormal.x"))
		pFoundation->vWorkNormal.x = atof(valueStr);
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("WorkNormal.y"))
		pFoundation->vWorkNormal.y = atof(valueStr);
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("WorkNormal.z"))
		pFoundation->vWorkNormal.z = atof(valueStr);
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("ProjAxisX.x"))
		pFoundation->vProjAxisX.x = atof(valueStr);
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("ProjAxisX.y"))
		pFoundation->vProjAxisX.y = atof(valueStr);
	else if (pItem->m_idProp == CLDSFoundation::GetPropID("ProjAxisX.z"))
		pFoundation->vProjAxisX.z = atof(valueStr);
	return TRUE;
}
BOOL FoundationButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSFoundation* pFoundation=(CLDSFoundation*)pView->selectObjs.GetFirst();
	if(pFoundation==NULL)
		return FALSE;
	CReportVectorDlg vecDlg;
	CPropertyListOper<CLDSFoundation> oper(pPropList,pFoundation);
	if (pItem->m_idProp == CLDSAnchorBolt::GetPropID("location"))
	{
		vecDlg.m_sCaption = "�����������ĵ�λ��";
		vecDlg.m_fVectorX = pFoundation->location.x;
		vecDlg.m_fVectorY = pFoundation->location.y;
		vecDlg.m_fVectorZ = pFoundation->location.z;
		if (vecDlg.DoModal() == IDOK)
		{
			pFoundation->location.Set(vecDlg.m_fVectorX, vecDlg.m_fVectorY, vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("location.x");
			oper.UpdatePropItemValue("location.y");
			oper.UpdatePropItemValue("location.z");
		}
	}
	else if (pItem->m_idProp == CLDSAnchorBolt::GetPropID("WorkNormal"))
	{
		vecDlg.m_sCaption = "������ƽ�淨��";
		vecDlg.m_fVectorX = pFoundation->vWorkNormal.x;
		vecDlg.m_fVectorY = pFoundation->vWorkNormal.y;
		vecDlg.m_fVectorZ = pFoundation->vWorkNormal.z;
		if (vecDlg.DoModal() == IDOK)
		{
			pFoundation->vWorkNormal.Set(vecDlg.m_fVectorX, vecDlg.m_fVectorY, vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("WorkNormal.x");
			oper.UpdatePropItemValue("WorkNormal.y");
			oper.UpdatePropItemValue("WorkNormal.z");
		}
	}
	else if (pItem->m_idProp == CLDSAnchorBolt::GetPropID("ProjAxisX"))
	{
		vecDlg.m_sCaption = "X��ˮƽͶӰ����";
		vecDlg.m_fVectorX = pFoundation->vProjAxisX.x;
		vecDlg.m_fVectorY = pFoundation->vProjAxisX.y;
		vecDlg.m_fVectorZ = pFoundation->vProjAxisX.z;
		if (vecDlg.DoModal() == IDOK)
		{
			pFoundation->vProjAxisX.Set(vecDlg.m_fVectorX, vecDlg.m_fVectorY, vecDlg.m_fVectorZ);
			oper.UpdatePropItemValue("ProjAxisX.x");
			oper.UpdatePropItemValue("ProjAxisX.y");
			oper.UpdatePropItemValue("ProjAxisX.z");
		}
	}
	return TRUE;
}
void CLDSView::DisplayFoundationProperty(BOOL bRebuild)
{
	for(CLDSObject* pSelObj=selectObjs.GetFirst();pSelObj;pSelObj=selectObjs.GetNext())
	{	
		if(pSelObj->GetClassTypeId()!=CLS_FOUNDATION)
			selectObjs.DeleteCursor(FALSE);
	}
	CLDSFoundation* pFoundation=(CLDSFoundation*)selectObjs.GetFirst();
	if(pFoundation==NULL)
		return;
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	const int GROUP_GENERAL=1;
	if(bRebuild)
	{	
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(1);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
#else 
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
#endif
		pPropDlg->RefreshTabCtrl(CLDSFoundation::m_iCurDisplayPropGroup);
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId=CLS_FOUNDATION;
		pPropList->SetModifyValueFunc(ModifyFoundationProperty);
		pPropList->SetButtonClickFunc(FoundationButtonClick);
	}
	CPropertyListOper<CLDSFoundation> oper(pPropList,pFoundation);
	CPropTreeItem *pItem=NULL,*pPropItem=NULL,*pGroupItem=NULL,*pRoot=pPropList->GetRootItem();
	pGroupItem=oper.InsertPropItem(pRoot,"BasicInfo");
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	pPropItem=oper.InsertEditPropItem(pGroupItem,"handle","","",-1,TRUE);
	pPropItem->SetReadOnly();
	oper.InsertCmbListPropItem(pGroupItem,"HeightSerial","","","",-1,TRUE);
	oper.InsertCmbListPropItem(pGroupItem,"LegSerial","","","",-1,TRUE);
	oper.InsertCmbListPropItem(pGroupItem,"LegQuad","","","",-1,TRUE);
	oper.InsertEditPropItem(pGroupItem,"Width","","",-1,TRUE);
	oper.InsertEditPropItem(pGroupItem,"Height","","",-1,TRUE);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"location","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"location.x","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"location.y","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"location.z","","",-1,TRUE);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"WorkNormal","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"WorkNormal.x","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"WorkNormal.y","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"WorkNormal.z","","",-1,TRUE);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"ProjAxisX","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ProjAxisX.x","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ProjAxisX.y","","",-1,TRUE);
	oper.InsertEditPropItem(pPropItem,"ProjAxisX.z","","",-1,TRUE);
	//
	pPropList->Redraw();
}
//////////////////////////////////////////////////////////////////////////
//��ʾ������
BOOL ModifyHoleWallProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	PROP_OBJ* pPropObj=pView->hashSelectObjs.GetFirst();
	if(pPropObj==NULL ||pPropObj->pBolt==NULL)
		return FALSE;
	//
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);

	return TRUE;
}
BOOL HoleWallButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	PROP_OBJ* pPropObj=pView->hashSelectObjs.GetFirst();
	if(pPropObj==NULL ||pPropObj->pBolt==NULL)
		return FALSE;
	return TRUE;
}
void CLDSView::DisplayHoleWallProperty(BOOL bRebuild)
{
	PROP_OBJ* pPropObj=hashSelectObjs.GetFirst();
	if(pPropObj==NULL ||pPropObj->pBolt==NULL)
		return;
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	if(bRebuild)
	{
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId=CLS_HOLEWALL;
		pPropList->SetModifyValueFunc(ModifyHoleWallProperty);
		pPropList->SetButtonClickFunc(HoleWallButtonClick);
		pPropList->Redraw();
	}
}
#endif