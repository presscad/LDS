// DatumPlanePropertyDlg.cpp : ʵ���ļ�
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "DatumPlanePropertyDlg.h"
#include "LmaDlg.h"
#include "ScrTools.h"
#include "ReportPointDlg.h"
#include "DatumPointPropertyDlg.h"
#include "GlobalFunc.h"
#include "DisplayPointProperty.h"
#include "LogFile.h"

// CDatumPlanePropertyDlg �Ի���

IMPLEMENT_DYNAMIC(CDatumPlanePropertyDlg, CDialog)
CDatumPlanePropertyDlg::CDatumPlanePropertyDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDatumPlanePropertyDlg::IDD, pParent)
	, m_fPointX(0)
	, m_fPointY(0)
	, m_fPointZ(0)
{
	m_idEventProp=0;
	m_pActivePt=NULL;
	m_bNewPoint1=FALSE;
	m_bNewPoint2=FALSE;
	m_bNewPoint3=FALSE;
	m_datumPoint1.datum_pos_style=m_datumPoint2.datum_pos_style=m_datumPoint3.datum_pos_style=8;
}

CDatumPlanePropertyDlg::~CDatumPlanePropertyDlg()
{
}
void CDatumPlanePropertyDlg::UpdateEditPoint(CLDSPoint* pPt)
{
	m_fPointX =pPt->xRsltPosition.x;
	m_fPointY =pPt->xRsltPosition.y;
	m_fPointZ =pPt->xRsltPosition.z;
	UpdateData(FALSE);
}
void CDatumPlanePropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Text(pDX, IDC_EDT_X, m_fPointX);
	DDX_Text(pDX, IDC_EDT_Y, m_fPointY);
	DDX_Text(pDX, IDC_EDT_Z, m_fPointZ);
}


BEGIN_MESSAGE_MAP(CDatumPlanePropertyDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnTcnSelchangeTabGroup)
	ON_BN_CLICKED(ID_BN_UPDATE_PLANE, OnBnUpdatePlane)
	ON_BN_CLICKED(ID_BN_COPY_PLANE, OnBnCopyPlane)
	ON_EN_CHANGE(IDC_EDT_X, OnEnChangeEdtX)
	ON_EN_CHANGE(IDC_EDT_Y, OnEnChangeEdtY)
	ON_EN_CHANGE(IDC_EDT_Z, OnEnChangeEdtZ)
	ON_BN_CLICKED(ID_BN_COPY_NORM, OnBnCopyNorm)
END_MESSAGE_MAP()


//
static void SetEditState(CPropertyList *pPropList,CLDSPoint* pDatumPoint)
{
	CDatumPlanePropertyDlg* pPlanePropDlg=(CDatumPlanePropertyDlg*)pPropList->GetParent();
	((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(pDatumPoint->datum_pos_style!=0);
	((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(pDatumPoint->datum_pos_style!=0);
	((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(pDatumPoint->datum_pos_style!=0);
	if(pDatumPoint->datum_pos_style==5)
	{
		if(pDatumPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(FALSE);
		else if(pDatumPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(FALSE);
		else if(pDatumPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2)
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(FALSE);
	}
	else if(pDatumPoint->datum_pos_style==6)
	{
		if(pDatumPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
		{
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(FALSE);
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(FALSE);
		}
		else if(pDatumPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
		{
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(FALSE);
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(FALSE);
		}
		else if(pDatumPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==2)
		{
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(FALSE);
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(FALSE);
		}
	}
}
CString GetLineHandleStr(bool bIncNewItem=FALSE);	//From ParaPointPropertyList.cpp
static void UpdateDatumPointParaProperty(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CItemInfo* lpInfo=NULL;
	CXhChar100 valueStr;
	CDatumPlanePropertyDlg* pDatumPlaneDlg=(CDatumPlanePropertyDlg*)pPropList->GetParent();
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL,*pRootItem=pParentItem;
	CLDSPoint* pDatumPoint=pDatumPlaneDlg->m_pActivePt;
	if(pParentItem==NULL)
	{
		if(pDatumPoint==&pDatumPlaneDlg->m_datumPoint3)
			pParentItem=pPropList->FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_three"),NULL);
		else if(pDatumPoint==&pDatumPlaneDlg->m_datumPoint2)
			pParentItem=pPropList->FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_two"),NULL);
		else //if(pDatumPoint==&pDatumPlaneDlg->m_datumPoint1)
			pParentItem=pPropList->FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_one"),NULL);
		pRootItem=pParentItem;
	}
	if(pDatumPoint==NULL || pParentItem==NULL)
		return;
	pPropList->DeleteAllSonItems(pParentItem);
	pDatumPlaneDlg->UpdatePlane();
	if(pDatumPoint->datum_pos_style==0)		//ֱ��ָ������
	{	//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The x coordinate of datum point";
#else 
		lpInfo->m_strPropHelp="��׼��X����";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("pos.x");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The y coordinate of datum point";
#else 
		lpInfo->m_strPropHelp="��׼��Y����";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("pos.y");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The z coordinate of datum point";
#else 
		lpInfo->m_strPropHelp="��׼��Z����";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("pos.z");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==1)	//�Ǹ����
	{	//��׼�Ǹ־��
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pDatumPoint->des_para.hPart,CLS_LINEPART);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLinePart==NULL)
			lpInfo->m_strPropName="Unknown Part Handle";
		else 
			lpInfo->m_strPropName.Format("Datum %s handle",pLinePart->GetPartTypeName());
#else 
		if(pLinePart==NULL)
			lpInfo->m_strPropName="δ֪�������";
		else 
			lpInfo->m_strPropName.Format("��׼%s���",pLinePart->GetPartTypeName());
#endif
		lpInfo->m_strPropHelp=lpInfo->m_strPropName;
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.hRod");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		if(pLinePart==NULL||pLinePart->IsAngle())
		{	//ƫ��֫
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Leg|1.Y Leg";
			lpInfo->m_strPropName="Offset Leg";
			lpInfo->m_strPropHelp="Offset Leg��";
#else 
			lpInfo->m_cmbItems="0.X֫|1.Y֫";
			lpInfo->m_strPropName="ƫ��֫";
			lpInfo->m_strPropHelp="ƫ��֫��";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.offset_wing");
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|user specify |";
			lpInfo->m_strPropName="Leg Offset Type";
			lpInfo->m_strPropHelp="The offset stadia or user specified offset.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������";
			lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.wing_offset_style");
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset";
			lpInfo->m_strPropHelp="The offset of leg direction.";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ";
			lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.wing_offset_dist");
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pDatumPoint->des_para.RODEND.wing_offset_style!=4);
		}
		//����ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="Normal Offset";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.norm_offset");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Vertical Offset Direction";
		lpInfo->m_strPropHelp="Vertical to offset direction,along the direction of datum rod start to the end or along the opposite direction��";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�˼�ʼ->�շ���ƫ�ƻ��ػ�׼�˼���->ʼƫ�ơ�";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.direction");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Vertical Direction Offset";
		lpInfo->m_strPropHelp="The offset of vertical direction";
#else 
		lpInfo->m_strPropName="����ƫ��ֵ";
		lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.len_offset_dist");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ��ֵ��������ͷӰ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName="Odd Length Influence";
		lpInfo->m_strPropHelp="Whether or not included in odd length influence of vertical offset.";
#else 
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName="����ͷӰ��";
		lpInfo->m_strPropHelp="����ƫ���Ƿ��������ͷӰ�졣";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.bIncOddEffect");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==2)	//�˼��Ͻڵ㶨λ
	{
		//��׼�˼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod Handle";
		lpInfo->m_strPropHelp="Datum rod handle��";
#else 
		lpInfo->m_strPropName="��׼�˼����";
		lpInfo->m_strPropHelp="��׼�˼������";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.hLinePart");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//��׼�ڵ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Node Handle";
		lpInfo->m_strPropHelp="Datum node's handle.";
#else 
		lpInfo->m_strPropName="��׼�ڵ���";
		lpInfo->m_strPropHelp="��׼�ڵ�����";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.hNode");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pDatumPoint->des_para.RODNODE.hRod ,CLS_LINEPART);
		if(pLinePart&&pLinePart->IsAngle())
		{	//ƫ��֫
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Leg|1.Y Leg";
			lpInfo->m_strPropName="Offset Leg";
			lpInfo->m_strPropHelp="Offset Leg";
#else 
			lpInfo->m_cmbItems="0.X֫|1.Y֫";
			lpInfo->m_strPropName="ƫ��֫";
			lpInfo->m_strPropHelp="ƫ��֫��";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.offset_wing");
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3| user specify |";
			lpInfo->m_strPropName="Leg Offset Style";
			lpInfo->m_strPropHelp="The offset stadia or user specified offset.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������";
			lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_style");
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset";
			lpInfo->m_strPropHelp="The offset of leg direction.";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ";
			lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_dist");
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pDatumPoint->des_para.RODNODE.wing_offset_style!=4);
		}
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Vertical Offset Direction";
		lpInfo->m_strPropHelp="Vertical to offset direction,along the direction of datum rod start to the end or along the opposite direction��";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�˼�ʼ->�շ���ƫ�ƻ��ػ�׼�˼���->ʼƫ�ơ�";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.direction");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Vertical Direction Offset";
		lpInfo->m_strPropHelp="The offset of vertical direction";
#else 
		lpInfo->m_strPropName="����ƫ��ֵ";
		lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.len_offset_dist");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="Normal Offset";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.norm_offset");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ƽ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName="Offset Flatly";
		lpInfo->m_strPropHelp="Offset Flatly";
#else 
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName="ƽ��";
		lpInfo->m_strPropHelp="ƽ��";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.bFaceOffset");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		if(pDatumPoint->des_para.RODNODE.bFaceOffset)
		{	//ƽ���淨��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
			lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Plane Normal";
			lpInfo->m_strPropHelp="Offset flatly plane's normal";
#else 
			lpInfo->m_strPropName="ƽ���淨��";
			lpInfo->m_strPropHelp="ƽ���淨��";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset");
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			int vectorType=CStdVector::GetVectorType(pDatumPoint->des_para.RODNODE.vPlaneNormal);
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
			lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The x component of offset plane's normal";
#else 
			lpInfo->m_strPropHelp="ƽ���淨��X�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
			pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
			lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The y component of offset plane's normal";
#else 
			lpInfo->m_strPropHelp="ƽ���淨��Y�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
			pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
			lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The z component of offset plane's normal";
#else 
			lpInfo->m_strPropHelp="ƽ���淨��Z�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
			pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
		}
	}
	else if(pDatumPoint->datum_pos_style==3)	//�˼����߽���
	{
		//��׼�˼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod 1 Handle";
		lpInfo->m_strPropHelp="Datum rod 1 handle��";
#else 
		lpInfo->m_strPropName="��׼�˼�1���";
		lpInfo->m_strPropHelp="��׼�˼�1�����";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.hDatum1");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		CLDSLinePart *pLinePart1=(CLDSLinePart*)console.FromPartHandle(pDatumPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEPART);
		if(pLinePart1&&pLinePart1->IsAngle())
		{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3| user specify |";
			lpInfo->m_strPropName="Leg Offset Style 1";
			lpInfo->m_strPropHelp="The offset type of datum angle 1:Scheduled stadia or user specify offset distance��";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������1";
			lpInfo->m_strPropHelp="��׼�Ǹ�1ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_style1");
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset 1";
			lpInfo->m_strPropHelp="The leg offset of datum angle 1.";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ1";
			lpInfo->m_strPropHelp="��׼�Ǹ�1֫��ƫ��ֵ��";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist1");
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pDatumPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
		}
		//��׼�˼�2���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod 2 Handle";
		lpInfo->m_strPropHelp="Datum rod 2 handle";
#else 
		lpInfo->m_strPropName="��׼�˼�2���";
		lpInfo->m_strPropHelp="��׼�˼�2���";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.hDatum2");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		CLDSLinePart *pLinePart2=(CLDSLinePart*)console.FromPartHandle(pDatumPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEPART);
		if(pLinePart2&&pLinePart2->IsAngle())
		{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3| user specify |";
			lpInfo->m_strPropName="Leg Offset Style 2";
			lpInfo->m_strPropHelp="The offset type of datum angle 2:Scheduled stadia or user specify offset distance.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������2";
			lpInfo->m_strPropHelp="��׼�Ǹ�2ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_style2");
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset 2";
			lpInfo->m_strPropHelp="The leg offset of datum angle 2 .";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ2";
			lpInfo->m_strPropHelp="��׼�Ǹ�2֫��ƫ��ֵ��";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist2");
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pDatumPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
		}
		//����ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="Normal Offset";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.norm_offset");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//���Ʒ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Near Normal";
		lpInfo->m_strPropHelp="Near normal";
#else 
		lpInfo->m_strPropName="���Ʒ���";
		lpInfo->m_strPropHelp="���Ʒ���";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		int vectorType=CStdVector::GetVectorType(f3dPoint(pDatumPoint->des_para.AXIS_INTERS.near_vector));
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The X component of near normal";
#else 
		lpInfo->m_strPropHelp="���Ʒ���X�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]");
		pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The Y component of near normal";
#else 
		lpInfo->m_strPropHelp="���Ʒ���Y�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]");
		pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The Z component of near normal";
#else 
		lpInfo->m_strPropHelp="���Ʒ���Z�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]");
		pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
	}
	else if(pDatumPoint->datum_pos_style==4)	//��׼�߽���
	{
		//��׼��1���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbType=CDT_LIST;
		lpInfo->m_cmbItems=GetLineHandleStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Line 1 Handle";
		lpInfo->m_strPropHelp="Datum line 1 handle";
#else 
		lpInfo->m_strPropName="��׼��1���";
		lpInfo->m_strPropHelp="��׼��1���";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.hDatum1");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//��׼��2���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbType=CDT_LIST;
		lpInfo->m_cmbItems=GetLineHandleStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Line 2 Handle";
		lpInfo->m_strPropHelp="Datum line 2 handle";
#else 
		lpInfo->m_strPropName="��׼��2���";
		lpInfo->m_strPropHelp="��׼��2���";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.hDatum2");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==5)	//��׼�����������겻���
	{
		//��׼�߾��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbType=CDT_LIST;
		lpInfo->m_cmbItems=GetLineHandleStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Line Handle";
		lpInfo->m_strPropHelp="Datum line handle";
#else 
		lpInfo->m_strPropName="��׼�߾��";
		lpInfo->m_strPropHelp="��׼�߾��";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.DATUMLINE.hDatumLine");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ָ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.X axis |1.Y axis |2.Z axis ";
		lpInfo->m_strPropName="Specify Axial";
		lpInfo->m_strPropHelp="Specified axial";
#else 
		lpInfo->m_cmbItems="0.X��|1.Y��|2.Z��";
		lpInfo->m_strPropName="ָ������";
		lpInfo->m_strPropHelp="ָ������";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==6)	//��׼�����������겻���
	{
		//��׼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbItems=CDisplayPointProperty::GetPlaneHandleStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Plane Handle";
#else 
		lpInfo->m_strPropName="��׼����";
#endif
		lpInfo->m_strPropHelp=CDisplayPointProperty::GetSelectPlaneDesc(pDatumPoint);
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.DATUMPLANE.hDatumPlane");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ԳƱ任����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="No symmetrical reference|X axis symmetry|Y axis symmetry|Z axis symmetry";
		lpInfo->m_strPropName="Reference Point Symmetrical Type";
		lpInfo->m_strPropHelp="Symmetrical reference point was generated according to the current point first, then datum plane calculated with line plane, back to the original position��";
#else 
		lpInfo->m_cmbItems="�޶ԳƲ���|X��Գ�|Y��Գ�|Z��Գ�";
		lpInfo->m_strPropName="���յ�ԳƷ�ʽ";
		lpInfo->m_strPropHelp="�ȸ��ݵ�ǰ�����ɶԳƵĲ��յ㣬�����׼�����������Ȼ���ٶԳƻ�ԭλ�á�";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.DATUMPLANE.cMirTransType");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//ָ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.X axis |1.Y axis |2.Z axis ";
		lpInfo->m_strPropName="Specify Axial";
		lpInfo->m_strPropHelp="Specified axial";
#else 
		lpInfo->m_cmbItems="0.X��|1.Y��|2.Z��";
		lpInfo->m_strPropName="ָ������";
		lpInfo->m_strPropHelp="ָ������";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==7)	//�ؽǸ���֫����ƫ�Ƶİ������
	{
		//��׼�Ǹ־��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Angle Handle";
		lpInfo->m_strPropHelp="Datum angle handle��";
#else 
		lpInfo->m_strPropName="��׼�Ǹ־��";
		lpInfo->m_strPropHelp="��׼�Ǹ־����";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.hRod");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Vertical Offset Direction";
		lpInfo->m_strPropHelp="Vertical offset direction,along the direction of datum angle start to the end offset or along the opposite direction";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.direction");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Vertical Direction Offset";
		lpInfo->m_strPropHelp="The offset of vertical direction";
#else 
		lpInfo->m_strPropName="����ƫ��ֵ";
		lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.len_offset_dist");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//����ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="Normal offset";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.norm_offset");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==8)	//��ڵ�ͬΪ
	{	//��׼�ڵ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Node Handle";
		lpInfo->m_strPropHelp="Datum node handle��";
#else 
		lpInfo->m_strPropName="��׼�ڵ���";
		lpInfo->m_strPropHelp="��׼�ڵ�����";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.hNode");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==9)	//ָ�������ϵ��������
	{
		//��׼�˼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Part Handle";
		lpInfo->m_strPropHelp="Datum part handle��";
#else 
		lpInfo->m_strPropName="��׼�������";
		lpInfo->m_strPropHelp="��׼���������";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.hPart");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Relative Coordinates";
		lpInfo->m_strPropHelp="Relative coordinates";
#else 
		lpInfo->m_strPropName="�������";
		lpInfo->m_strPropHelp="�������";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("datum_pos");
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The x component of relative coordinates";
#else 
		lpInfo->m_strPropHelp="�������X�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("datum_pos.x");
		pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The y component of relative coordinates";
#else 
		lpInfo->m_strPropHelp="�������Y�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("datum_pos.y");
		pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The z component of relative coordinates";
#else 
		lpInfo->m_strPropHelp="�������Z�������";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("datum_pos.z");
		pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(pDatumPlaneDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
}
void InitDatumPointDesPara(CLDSPoint *pPoint);	//From ParaPointPropertyList.cpp
static BOOL ModifyDatumPlaneProperty(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CDatumPlanePropertyDlg *pPlanePropDlg=(CDatumPlanePropertyDlg*)pPropList->GetParent();
	if(pPlanePropDlg==NULL)
		return FALSE;
	CPropTreeItem* pFindItem=NULL,*pParentItem=NULL;
	BOOL bRefreshPtPara=FALSE;
	CLDSPoint *pPoint=&pPlanePropDlg->m_datumPoint1;
	if(pItem->m_dwPropGroup&0X08)
		pPoint=&pPlanePropDlg->m_datumPoint3;
	else if(pItem->m_dwPropGroup&0X04)
		pPoint=&pPlanePropDlg->m_datumPoint2;
	if(pPoint==NULL)
		return FALSE;
	pPlanePropDlg->UpdatePlane();
	pPlanePropDlg->m_pActivePt=pPoint;
	char value_str[100]="";
	if(CDatumPlanePropertyDlg::GetPropID("description")==pItem->m_idProp)
		strncpy(pPlanePropDlg->m_datumPlane.description,valueStr,100);
	else if(CDatumPlanePropertyDlg::GetPropID("datum_pos_desc1")==pItem->m_idProp||
		CDatumPlanePropertyDlg::GetPropID("datum_pos_desc2")==pItem->m_idProp||
		CDatumPlanePropertyDlg::GetPropID("datum_pos_desc3")==pItem->m_idProp)
		strncpy(pPoint->description,valueStr,50);
	else if(CDatumPlanePropertyDlg::GetPropID("hDatumPt1")==pItem->m_idProp||
		CDatumPlanePropertyDlg::GetPropID("hDatumPt2")==pItem->m_idProp ||
		CDatumPlanePropertyDlg::GetPropID("hDatumPt3")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("<New...>")==0)
#else 
		if(valueStr.Compare("<�½�...>")==0)
#endif
		{
			CLDSPoint* pPt=new CLDSPoint();
			*pPoint=*pPt;
			pPlanePropDlg->m_fPointX=0;
			pPlanePropDlg->m_fPointY=0;
			pPlanePropDlg->m_fPointZ=0;
			pPlanePropDlg->UpdateData(FALSE);
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(FALSE);
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(FALSE);
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(FALSE);
		}
		else
		{
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->handle));
			CLDSPoint* pPt=Ta.Point.FromHandle(pPoint->handle);
			*pPoint=*pPt;
			pPoint->UpdatePos(console.GetActiveModel());
			pPlanePropDlg->m_fPointX=pPoint->Position().x;
			pPlanePropDlg->m_fPointY=pPoint->Position().y;
			pPlanePropDlg->m_fPointZ=pPoint->Position().z;
			SetEditState(pPropList,pPoint);
			pPlanePropDlg->UpdateData(FALSE);
		}
		bRefreshPtPara=TRUE;
		CXhChar50 sText;
		if(CDatumPlanePropertyDlg::GetPropID("hDatumPt1")==pItem->m_idProp)
		{
			pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("datum_pos_desc1"),pPoint->description);
			pPlanePropDlg->GetPropValueStr(CDatumPlanePropertyDlg::GetPropID("datum_pos_style1"),sText);
			pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("datum_pos_style1"),sText);
		}
		else if(CDatumPlanePropertyDlg::GetPropID("hDatumPt2")==pItem->m_idProp)
		{
			pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("datum_pos_desc2"),pPoint->description);
			pPlanePropDlg->GetPropValueStr(CDatumPlanePropertyDlg::GetPropID("datum_pos_style2"),sText);
			pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("datum_pos_style2"),sText);
		}
		else
		{
			pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("datum_pos_desc3"),pPoint->description);
			pPlanePropDlg->GetPropValueStr(CDatumPlanePropertyDlg::GetPropID("datum_pos_style3"),sText);
			pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("datum_pos_style3"),sText);
		}
	}
	else if(CDatumPlanePropertyDlg::GetPropID("datum_pos_style1")==pItem->m_idProp ||
		CDatumPlanePropertyDlg::GetPropID("datum_pos_style2")==pItem->m_idProp||
		CDatumPlanePropertyDlg::GetPropID("datum_pos_style3")==pItem->m_idProp)
	{
		int nPosStyle=valueStr[0] - '0';
		if(pPoint->datum_pos_style!=nPosStyle)
		{
			pPoint->datum_pos_style=nPosStyle;
			InitDatumPointDesPara(pPoint);
			bRefreshPtPara=TRUE;
		}
		SetEditState(pPropList,pPoint);
	}
	else if(pPoint->datum_pos_style==0)	//ֱ��ָ��
	{
		if(CDatumPlanePropertyDlg::GetPropID("pos.x")==pItem->m_idProp)
		{
			pPoint->SetPositionX(atof(valueStr));
			pPlanePropDlg->m_fPointX=pPoint->Position().x;
			pPlanePropDlg->UpdateData(FALSE);
		}
		else if(CDatumPlanePropertyDlg::GetPropID("pos.y")==pItem->m_idProp)
		{
			pPoint->SetPositionY(atof(valueStr));
			pPlanePropDlg->m_fPointY=pPoint->Position().y;
			pPlanePropDlg->UpdateData(FALSE);
		}
		else if(CDatumPlanePropertyDlg::GetPropID("pos.z")==pItem->m_idProp)
		{
			pPoint->SetPositionZ(atof(valueStr));
			pPlanePropDlg->m_fPointZ=pPoint->Position().z;
			pPlanePropDlg->UpdateData(FALSE);
		}
	}
	else if(pPoint->datum_pos_style==1)	//�Ǹ���㶨λ
	{
		if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODEND.hRod));
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
			pPoint->des_para.RODEND.direction=valueStr[0]-'0';
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.offset_wing")==pItem->m_idProp)//0.X֫;1.Y֫
			pPoint->des_para.RODEND.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.wing_offset_style")==pItem->m_idProp)	
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pPoint->des_para.RODEND.wing_offset_style=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pPoint->des_para.RODEND.wing_offset_style=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pPoint->des_para.RODEND.wing_offset_style=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pPoint->des_para.RODEND.wing_offset_style=3;
			else 
				pPoint->des_para.RODEND.wing_offset_style=4;
			long idProp=CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.wing_offset_dist");
			if(pPlanePropDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.RODEND.wing_offset_style!=4);
		}
#endif
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.wing_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.wing_offset_dist=atof(valueStr);
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.len_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.bIncOddEffect")==pItem->m_idProp)		
		{	//����ƫ��ֵ��������ͷӰ��
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
#endif
			pPoint->des_para.RODEND.bIncOddEffect=TRUE;
			else 
			pPoint->des_para.RODEND.bIncOddEffect=FALSE;
		}
	}
	else if(pPoint->datum_pos_style==2)	//�˼��ڵ㶨λ
	{
		if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr,"%X",&(pPoint->des_para.RODNODE.hRod ));
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.hNode")==pItem->m_idProp)	//��׼�ڵ���
			sscanf(valueStr,"%X",&(pPoint->des_para.RODNODE.hNode));
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.direction")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.direction=valueStr[0]-'0';
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.offset_wing")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_style")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pPoint->des_para.RODNODE.wing_offset_style=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pPoint->des_para.RODNODE.wing_offset_style=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pPoint->des_para.RODNODE.wing_offset_style=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pPoint->des_para.RODNODE.wing_offset_style=3;
			else 
				pPoint->des_para.RODNODE.wing_offset_style=4;
			//
			long idProp=CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_dist");
			if(pPlanePropDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.RODNODE.wing_offset_style!=4);
		}	
#endif
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODNODE.wing_offset_dist=atof(valueStr);
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.len_offset_dist")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.len_offset_dist=atof(valueStr);
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.bFaceOffset")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
#endif
				pPoint->des_para.RODNODE.bFaceOffset=TRUE;
			else 
				pPoint->des_para.RODNODE.bFaceOffset=FALSE;
			pPropList->DeleteItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset"));
			pPropList->DeleteItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]"));
			pPropList->DeleteItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]"));
			pPropList->DeleteItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"));
			if(pPoint->des_para.RODNODE.bFaceOffset)
			{	//ƽ���淨��
				CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL,*pRootItem=pItem->m_pParent;
				//
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
				lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();

#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Offset Plane Normal";
				lpInfo->m_strPropHelp="Offset plane's normal";
#else 
				lpInfo->m_strPropName="ƽ���淨��";
				lpInfo->m_strPropHelp="ƽ���淨��";
#endif
				pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset");
				pPropItem->m_dwPropGroup=pRootItem->m_dwPropGroup;
				if(pPlanePropDlg->GetPropValueStr(pPropItem->m_idProp,value_str)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				int vectorType=CStdVector::GetVectorType(pPoint->des_para.RODNODE.vPlaneNormal);
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMONEDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The x component of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��X�������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
				pSonPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
				pSonPropItem->m_dwPropGroup=pPropItem->m_dwPropGroup;	
				if(pPlanePropDlg->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMONEDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The y component of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��Y�������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,TRUE);
				pSonPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
				pSonPropItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				if(pPlanePropDlg->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMONEDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The z component of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��Z�������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,TRUE);
				pSonPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
				pSonPropItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
				if(pPlanePropDlg->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
			}
		}
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset")==pItem->m_idProp)
		{
			f3dPoint norm=CStdVector::GetVector(valueStr[0]-'0');
			pPoint->des_para.RODNODE.vPlaneNormal=norm;
			long idProp=CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.x);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.y);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.z);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		}
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.x=atof(valueStr);
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.y=atof(valueStr);
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.z=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==3)	//�˼����߽���
	{
		if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum1));
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum2));
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_style1")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=3;
			else 
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=4;
			long idProp=CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist1");
			if(pPlanePropDlg->GetPropValueStr(idProp,value_str)>0)
			pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
		}
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_style2")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=3;
			else 
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=4;
			//
			long idProp=CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist2");
			if(pPlanePropDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
		}
#endif
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist1")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.wing_offset_dist1=atol(valueStr);
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist2")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.wing_offset_dist2=atol(valueStr);
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector")==pItem->m_idProp)
		{
			f3dPoint vec=CStdVector::GetVector(valueStr[0]-'0');
			pPoint->des_para.AXIS_INTERS.near_vector[0]=vec.x;
			pPoint->des_para.AXIS_INTERS.near_vector[1]=vec.y;
			pPoint->des_para.AXIS_INTERS.near_vector[2]=vec.z;
			long idProp=CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[0]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[1]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[2]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		}
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[0]=atof(valueStr);
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[1]=atof(valueStr);
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[2]=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==4)	//��׼�߿ռ佻��
	{
		if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum1));
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum2));
	}
	else if(pPoint->datum_pos_style==5)	//��׼����ָ�����������
	{
		if(CDatumPlanePropertyDlg::GetPropID("des_para.DATUMLINE.hDatumLine")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.DATUMLINE.hDatumLine));
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==pItem->m_idProp)
		{		
			pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=valueStr[0]-'0';
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(TRUE);
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(TRUE);
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(TRUE);
			if(pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
				((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(FALSE);
			else if(pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
				((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(FALSE);
			else if(pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2)
				((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(FALSE);
		}
	}
	else if(pPoint->datum_pos_style==6)	//��׼����ָ�������
	{
		if(CDatumPlanePropertyDlg::GetPropID("des_para.DATUMPLANE.hDatumPlane")==pItem->m_idProp)
		{
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.DATUMPLANE.hDatumPlane));
			CLDSPlane *pPlane=console.FromPlaneHandle(pPoint->des_para.DATUMPLANE.hDatumPlane);
			if(pPlane)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				CString sValue="Datum plane��";
#else 
				CString sValue="��׼�棺";
#endif
				CString ss;
				ss.Format("%s_0X%X",pPlane->description,pPlane->handle); 
				sValue+=ss;
				pItem->m_lpNodeInfo->m_strPropHelp=sValue;
				pPropList->SetPromptString(sValue);
			}
		}
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.DATUMPLANE.cMirTransType")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("X axis symmetry")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='X';
			else if(valueStr.CompareNoCase("Y axis symmetry")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='Y';
			else if(valueStr.CompareNoCase("Z axis symmetry")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='Z';
#else 
			if(valueStr.CompareNoCase("X��Գ�")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='X';
			else if(valueStr.CompareNoCase("Y��Գ�")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='Y';
			else if(valueStr.CompareNoCase("Z��Գ�")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='Z';
#endif
			else
				pPoint->des_para.DATUMPLANE.cMirTransType=0;
		}
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==pItem->m_idProp)
		{	
			pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=valueStr[0]-'0';
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(TRUE);
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(TRUE);
			((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(TRUE);
			if(pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
			{
				((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(FALSE);
				((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(FALSE);
			}
			else if(pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
			{
				((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(FALSE);
				((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Z))->SetReadOnly(FALSE);
			}
			else if(pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==2)
			{
				((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_X))->SetReadOnly(FALSE);
				((CEdit*)pPlanePropDlg->GetDlgItem(IDC_EDT_Y))->SetReadOnly(FALSE);
			}
		}
	}
	else if(pPoint->datum_pos_style==7)	//�ؽǸ���֫����ƫ�Ƶİ������
	{
		if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODEND.hRod));
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
			pPoint->des_para.RODEND.direction=valueStr[0]-'0';
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.len_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==8)	//��ڵ�ͬλ
	{
		if(CDatumPlanePropertyDlg::GetPropID("des_para.hNode")==pItem->m_idProp)
		sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.hNode));
	}
	else if(pPoint->datum_pos_style==9)	//ָ�������ϵ��������
	{
		if(CDatumPlanePropertyDlg::GetPropID("des_para.hPart")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.hPart));
		else if(CDatumPlanePropertyDlg::GetPropID("datum_pos.x")==pItem->m_idProp)
			pPoint->datum_pos.x=atof(valueStr);
		else if(CDatumPlanePropertyDlg::GetPropID("datum_pos.y")==pItem->m_idProp)
			pPoint->datum_pos.y=atof(valueStr);
		else if(CDatumPlanePropertyDlg::GetPropID("datum_pos.z")==pItem->m_idProp)
			pPoint->datum_pos.z=atof(valueStr);
	}
	if(bRefreshPtPara)
	{
		if(pItem->m_dwPropGroup&0X08)
			pFindItem=pPropList->FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_three"),&pParentItem);
		else if(pItem->m_dwPropGroup&0X04)
			pFindItem=pPropList->FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_two"),&pParentItem);
		else
			pFindItem=pPropList->FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_one"),&pParentItem);
		UpdateDatumPointParaProperty(pPropList,pFindItem);
	}
	pPoint->UpdatePos(console.GetActiveModel());
	pPlanePropDlg->UpdateEditPoint(pPoint);
	return TRUE;
}
//Button
static BOOL ButtonClickDatumPlane(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CReportVectorDlg vecDlg;
	CDatumPlanePropertyDlg *pPlanePropDlg=(CDatumPlanePropertyDlg*)pPropList->GetParent();
	CPropTreeItem *pParentItem=NULL;
	if(pPlanePropDlg==NULL)
		return FALSE;
	CLDSPoint *pPoint=&(pPlanePropDlg->m_datumPoint1);
	if(pItem->m_dwPropGroup&0X08)
		pPoint=&(pPlanePropDlg->m_datumPoint3);
	else if(pItem->m_dwPropGroup&0X04)
		pPoint=&(pPlanePropDlg->m_datumPoint2);
	if(pPoint==NULL)
		return FALSE;
	pPlanePropDlg->m_pActivePt=pPoint;
	CXhChar100 sText;
	pPlanePropDlg->UpdatePlane();
	if(pPoint->datum_pos_style==1)
	{	//�Ǹ���㶨λ
		if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)
			pPlanePropDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);//��׼�˼�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.norm_offset")==pItem->m_idProp)
		{	//����ƫ����
			CString sThickStr;
			sThickStr=pPoint->des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(pPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
			if(pPlanePropDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
	}
	else if(pPoint->datum_pos_style==2)
	{	//�˼��Ͻڵ㶨λ
		if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)
		pPlanePropDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.hNode")==pItem->m_idProp)
		pPlanePropDlg->SelectObject(CLS_NODE,pItem->m_idProp);		//��׼�ڵ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.norm_offset")==pItem->m_idProp)
		{	//����ƫ����
			CString sThickStr;
			sThickStr=pPoint->des_para.RODNODE.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			StrCopy(pPoint->des_para.RODNODE.norm_offset,sThickStr,51);
			//if(pPlanePropDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,sThickStr);//����GetPropValueStr��Ϊ�˱��⵱ǰ����㲻��pPoint��� wjh-2017.3.2
		}
		else if(pItem->m_idProp==CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset")||
		pItem->m_idProp==CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]")||
		pItem->m_idProp==CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]")||
		pItem->m_idProp==CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"))
		{	
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Offset Plane Normal";
#else 
			vecDlg.m_sCaption = "ƽ���淨��";
#endif
			vecDlg.m_fVectorX = pPoint->des_para.RODNODE.vPlaneNormal.x;
			vecDlg.m_fVectorY = pPoint->des_para.RODNODE.vPlaneNormal.y;
			vecDlg.m_fVectorZ = pPoint->des_para.RODNODE.vPlaneNormal.z;
			pItem = pPropList->FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]"),&pParentItem);
			vecDlg.m_bReadOnlyX = pItem->IsReadOnly();
			pItem = pPropList->FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]"),&pParentItem);
			vecDlg.m_bReadOnlyY = pItem->IsReadOnly();
			pItem = pPropList->FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"),CString(sText));
			}
		}
#endif
	}
	else if(pPoint->datum_pos_style==3)
	{	//�Ǹ����߽���
		if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
		pPlanePropDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�1
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
		pPlanePropDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�2
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.norm_offset")==pItem->m_idProp)
		{	//����ƫ����
			CString sThickStr;
			sThickStr=pPoint->des_para.AXIS_INTERS.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(pPoint->des_para.AXIS_INTERS.norm_offset,49,"%s",sThickStr);
			if(pPlanePropDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		}
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector")==pItem->m_idProp||
			CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]")==pItem->m_idProp||
			CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]")==pItem->m_idProp||
			CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Offset Plane Normal";
#else 
			vecDlg.m_sCaption = "ƽ���淨��";
#endif
			vecDlg.m_fVectorX = pPoint->des_para.AXIS_INTERS.near_vector[0];
			vecDlg.m_fVectorY = pPoint->des_para.AXIS_INTERS.near_vector[1];
			vecDlg.m_fVectorZ = pPoint->des_para.AXIS_INTERS.near_vector[2];
			pItem = pPropList->FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]"),&pParentItem);
			vecDlg.m_bReadOnlyX = pItem->IsReadOnly();
			pItem = pPropList->FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]"),&pParentItem);
			vecDlg.m_bReadOnlyY = pItem->IsReadOnly();
			pItem = pPropList->FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]"),CString(sText));
			}
		}
#endif
	}
	else if(pPoint->datum_pos_style==7)
	{	//�ؽǸ���֫����ƫ�Ƶİ������
		if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)
		pPlanePropDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//��׼�Ǹ�
		#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CDatumPlanePropertyDlg::GetPropID("des_para.RODEND.norm_offset")==pItem->m_idProp)
		{	//����ƫ����
			CString sThickStr;
			sThickStr=pPoint->des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(pPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
			if(pPlanePropDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
	}
	else if(pPoint->datum_pos_style==8)
	{	//��ڵ�ͬλ
		if(CDatumPlanePropertyDlg::GetPropID("des_para.hNode")==pItem->m_idProp)
		pPlanePropDlg->SelectObject(CLS_NODE,pItem->m_idProp);	//��׼�ڵ�
	}
	else if(pPoint->datum_pos_style==9)
	{	//ָ�������ϵ����λ��
		if(CDatumPlanePropertyDlg::GetPropID("des_para.hPart")==pItem->m_idProp)
		pPlanePropDlg->SelectObject(CLS_PART,pItem->m_idProp);	//��׼����
		else if(CDatumPlanePropertyDlg::GetPropID("datum_pos")==pItem->m_idProp)
		{
			CReportPointDlg pointDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			pointDlg.m_sCaption = "Relative Coordinates";
#else 
			pointDlg.m_sCaption = "�������";
#endif
			pointDlg.m_fPointX = pPoint->datum_pos.x;
			pointDlg.m_fPointY = pPoint->datum_pos.y;
			pointDlg.m_fPointZ = pPoint->datum_pos.z;
			if(pointDlg.DoModal()==IDOK)
			{
				CXhChar100 sText("%f",pointDlg.m_fPointX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("datum_pos.x"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("datum_pos.y"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("datum_pos.z"),CString(sText));
				pPoint->datum_pos.Set(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ);
			}
		}
	}
	pPoint->UpdatePos(console.GetActiveModel());
	pPlanePropDlg->UpdateEditPoint(pPoint);
	return TRUE;
}
// CDatumPlanePropertyDlg ��Ϣ�������
BOOL CDatumPlanePropertyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//
	logerr.ClearContents();
	InitPropHashtable();
#ifdef AFX_TARG_ENU_ENGLISH
	m_ctrlPropGroup.InsertItem(0,"General information");
	m_ctrlPropGroup.InsertItem(1,"Datum Point1");
	m_ctrlPropGroup.InsertItem(2,"Datum Point2");
	m_ctrlPropGroup.InsertItem(3,"Datum Point3");
#else 
	m_ctrlPropGroup.InsertItem(0,"������Ϣ");
	m_ctrlPropGroup.InsertItem(1,"��׼��1");
	m_ctrlPropGroup.InsertItem(2,"��׼��2");
	m_ctrlPropGroup.InsertItem(3,"��׼��3");
#endif
	m_ctrlPropGroup.SetCurSel(m_propList.m_iPropGroup);
	if(m_propList.m_iPropGroup==1)
		m_pActivePt=&m_datumPoint1;
	else if(m_propList.m_iPropGroup==2)
		m_pActivePt=&m_datumPoint2;
	else if(m_propList.m_iPropGroup==3)
		m_pActivePt=&m_datumPoint3;
	//
	m_propList.m_hPromptWnd = GetDlgItem(IDC_S_DATUM_PLANE_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.4);
	m_propList.SetModifyValueFunc(ModifyDatumPlaneProperty);
	m_propList.SetButtonClickFunc(ButtonClickDatumPlane);
	//
	((CEdit*)GetDlgItem(IDC_EDT_X))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_EDT_Y))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_EDT_Z))->SetReadOnly(TRUE);
	if(!m_bInernalStart)
	{
		if(m_datumPlane.handle>0x20)
		{
			CLDSPoint* pPoint=NULL;
			//
			if(m_datumPlane.hPointArr[0].bSimpleNodePoint)
			{	//ֱ��ָ���ڵ�
				m_datumPoint1.datum_pos_style=8;
				m_datumPoint1.des_para.hNode=m_datumPlane.hPointArr[0].handle;
				m_datumPoint1.UpdatePos();
			}
			else if(m_datumPlane.hPointArr[0].bRodTipStart)
			{
				m_datumPoint1.datum_pos_style=1;	//�˼��˵㶨λ
				m_datumPoint1.des_para.RODEND.hRod=m_datumPlane.hPointArr[0].handle;
				m_datumPoint1.des_para.RODEND.direction=0;
				m_datumPoint1.des_para.RODEND.wing_offset_style=4;
				m_datumPoint1.des_para.RODEND.wing_offset_dist=0;
				m_datumPoint1.des_para.RODEND.len_offset_dist=0;
				m_datumPoint1.UpdatePos();
			}
			else if(m_datumPlane.hPointArr[0].bRodTipEnd)
			{
				m_datumPoint1.datum_pos_style=1;	//�˼��˵㶨λ
				m_datumPoint1.des_para.RODEND.hRod=m_datumPlane.hPointArr[0].handle;
				m_datumPoint1.des_para.RODEND.direction=1;
				m_datumPoint1.des_para.RODEND.wing_offset_style=4;
				m_datumPoint1.des_para.RODEND.wing_offset_dist=0;
				m_datumPoint1.des_para.RODEND.len_offset_dist=0;
				m_datumPoint1.UpdatePos();
			}
			else
			{
				pPoint=Ta.Point.FromHandle(m_datumPlane.hPointArr[0]);
				if(pPoint)
					m_datumPoint1=*pPoint;
			}
			//
			if(m_datumPlane.hPointArr[1].bSimpleNodePoint)
			{	//ֱ��ָ���ڵ�
				m_datumPoint2.datum_pos_style=8;
				m_datumPoint2.des_para.hNode=m_datumPlane.hPointArr[1].handle;
				m_datumPoint2.UpdatePos();
			}
			else if(m_datumPlane.hPointArr[1].bRodTipStart)
			{
				m_datumPoint2.datum_pos_style=1;	//�˼��˵㶨λ
				m_datumPoint2.des_para.RODEND.hRod=m_datumPlane.hPointArr[1].handle;
				m_datumPoint2.des_para.RODEND.direction=0;
				m_datumPoint2.des_para.RODEND.wing_offset_style=4;
				m_datumPoint2.des_para.RODEND.wing_offset_dist=0;
				m_datumPoint2.des_para.RODEND.len_offset_dist=0;
				m_datumPoint2.UpdatePos();
			}
			else if(m_datumPlane.hPointArr[1].bRodTipEnd)
			{
				m_datumPoint2.datum_pos_style=1;	//�˼��˵㶨λ
				m_datumPoint2.des_para.RODEND.hRod=m_datumPlane.hPointArr[1].handle;
				m_datumPoint2.des_para.RODEND.direction=1;
				m_datumPoint2.des_para.RODEND.wing_offset_style=4;
				m_datumPoint2.des_para.RODEND.wing_offset_dist=0;
				m_datumPoint2.des_para.RODEND.len_offset_dist=0;
				m_datumPoint2.UpdatePos();
			}
			else
			{
				pPoint=Ta.Point.FromHandle(m_datumPlane.hPointArr[1]);
				if(pPoint)
					m_datumPoint2=*pPoint;
			}
			//
			if(m_datumPlane.hPointArr[2].bSimpleNodePoint)
			{	//ֱ��ָ���ڵ�
				m_datumPoint3.datum_pos_style=8;
				m_datumPoint3.des_para.hNode=m_datumPlane.hPointArr[2].handle;
				m_datumPoint3.UpdatePos();
			}
			else if(m_datumPlane.hPointArr[2].bRodTipStart)
			{
				m_datumPoint3.datum_pos_style=1;	//�˼��˵㶨λ
				m_datumPoint3.des_para.RODEND.hRod=m_datumPlane.hPointArr[2].handle;
				m_datumPoint3.des_para.RODEND.direction=0;
				m_datumPoint3.des_para.RODEND.wing_offset_style=4;
				m_datumPoint3.des_para.RODEND.wing_offset_dist=0;
				m_datumPoint3.des_para.RODEND.len_offset_dist=0;
				m_datumPoint3.UpdatePos();
			}
			else if(m_datumPlane.hPointArr[2].bRodTipEnd)
			{
				m_datumPoint3.datum_pos_style=1;	//�˼��˵㶨λ
				m_datumPoint3.des_para.RODEND.hRod=m_datumPlane.hPointArr[2].handle;
				m_datumPoint3.des_para.RODEND.direction=1;
				m_datumPoint3.des_para.RODEND.wing_offset_style=4;
				m_datumPoint3.des_para.RODEND.wing_offset_dist=0;
				m_datumPoint3.des_para.RODEND.len_offset_dist=0;
				m_datumPoint3.UpdatePos();
			}
			else
			{
				pPoint=Ta.Point.FromHandle(m_datumPlane.hPointArr[2]);
				if(pPoint)
					m_datumPoint3=*pPoint;
			}
			m_datumPlane.UpdatePlane(console.GetActiveModel());
		}
	}
	m_fPointX=m_datumPlane.GetBasePos().x;
	m_fPointY=m_datumPlane.GetBasePos().y;
	m_fPointZ=m_datumPlane.GetBasePos().z;
	if(m_bInernalStart)	
		FinishSelectObjOper();	//���ѡ�����ĺ�������
	//��ʾ��׼����Ϣ
	DisplayDatumPlaneProperty();
	if(m_bInernalStart)	
	{	//�ָ�ѡ����
		m_propList.Redraw();
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();
		m_propList.SetCurSel(pItem->m_iIndex);	//����ѡ����	
	}
	UpdateData(FALSE);
	if(logerr.IsHasContents())
		logerr.ShowToScreen();
	return TRUE;
}
void CDatumPlanePropertyDlg::OnTcnSelchangeTabGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_propList.m_iPropGroup = m_ctrlPropGroup.GetCurSel();
	if(m_propList.m_iPropGroup==0)
	{
		if(m_datumPlane.handle>0x20)
		{
			m_datumPlane.UpdatePlane(console.GetActiveModel());
			m_fPointX=m_datumPlane.GetBasePos().x;
			m_fPointY=m_datumPlane.GetBasePos().y;
			m_fPointZ=m_datumPlane.GetBasePos().z;
		}
		else 
		{	
			f3dPoint work_norm=m_datumPoint2.Position()-m_datumPoint1.Position()^m_datumPoint3.Position()-m_datumPoint2.Position();
			normalize(work_norm);
			m_propList.SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("norm.x"),CXhChar100(work_norm.x));
			m_propList.SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("norm.y"),CXhChar100(work_norm.y));
			m_propList.SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("norm.z"),CXhChar100(work_norm.z));
			m_fPointX=m_datumPoint2.Position().x;
			m_fPointY=m_datumPoint2.Position().y;
			m_fPointZ=m_datumPoint2.Position().z;
		}
		((CEdit*)GetDlgItem(IDC_EDT_X))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDT_Y))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EDT_Z))->SetReadOnly(TRUE);
	}
	else if(m_propList.m_iPropGroup==1)
	{
		m_fPointX=m_datumPoint1.Position().x;
		m_fPointY=m_datumPoint1.Position().y;
		m_fPointZ=m_datumPoint1.Position().z;
		SetEditState(&m_propList,&m_datumPoint1);
		m_pActivePt=&m_datumPoint1;
	}
	else if(m_propList.m_iPropGroup==2)
	{	
		m_fPointX=m_datumPoint2.Position().x;
		m_fPointY=m_datumPoint2.Position().y;
		m_fPointZ=m_datumPoint2.Position().z;
		SetEditState(&m_propList,&m_datumPoint2);
		m_pActivePt=&m_datumPoint2;
	}
	else if(m_propList.m_iPropGroup==3)
	{	
		m_fPointX=m_datumPoint3.Position().x;
		m_fPointY=m_datumPoint3.Position().y;
		m_fPointZ=m_datumPoint3.Position().z;
		SetEditState(&m_propList,&m_datumPoint3);
		m_pActivePt=&m_datumPoint3;
	}
	UpdateData(FALSE);
	if(m_propList.m_iPropGroup>0)
		UpdateDatumPointParaProperty(&m_propList,NULL);
	m_propList.Redraw();
	*pResult = 0;
}

void CDatumPlanePropertyDlg::OnOK()
{
	UpdateData();
	char sError[200]="";
	if(!m_datumPoint1.AssertValid(console.GetActiveModel(),sError))
	{
		AfxMessageBox(CXhChar200("��׼��1����������,%s",sError));
		return;
	}
	if(!m_datumPoint2.AssertValid(console.GetActiveModel(),sError))
	{
		AfxMessageBox(CXhChar200("��׼��2����������,%s",sError));
		return;
	}
	if(!m_datumPoint3.AssertValid(console.GetActiveModel(),sError))
	{
		AfxMessageBox(CXhChar200("��׼��3����������,%s",sError));
		return;
	}
	CLDSPoint* pDatumPoint=NULL;
	HANDLEPOINT hTempPointArr[3];
	hTempPointArr[0]=ToHandlePoint(&m_datumPoint1);
	if(hTempPointArr[0].handle<0x20)
	{	//������׼��
		m_bNewPoint1=TRUE;
		pDatumPoint=console.AppendPoint();
		m_datumPoint1.handle=pDatumPoint->handle;
		m_datumPoint1.dwPermission=pDatumPoint->dwPermission;
	}
	else if(hTempPointArr[0].ciHidType==0)
	{	//��������
		pDatumPoint=Ta.Point.FromHandle(m_datumPoint1.handle);	
		*pDatumPoint=m_datumPoint1;
		pDatumPoint->UpdatePos(console.GetActiveModel());
	}
	hTempPointArr[1]=ToHandlePoint(&m_datumPoint2);
	if(hTempPointArr[1].handle<0x20)
	{		//������׼��
		m_bNewPoint2=TRUE;
		pDatumPoint=console.AppendPoint();
		m_datumPoint2.handle=pDatumPoint->handle;
		m_datumPoint2.dwPermission=pDatumPoint->dwPermission;
	}
	else if(hTempPointArr[1].ciHidType==0)
	{	//��������
		pDatumPoint=Ta.Point.FromHandle(m_datumPoint2.handle);
		*pDatumPoint=m_datumPoint2;
		pDatumPoint->UpdatePos(console.GetActiveModel());
	}
	hTempPointArr[2]=ToHandlePoint(&m_datumPoint3);
	if(hTempPointArr[2].handle<0x20)
	{		//������׼��
		m_bNewPoint3=TRUE;
		pDatumPoint=console.AppendPoint();
		m_datumPoint3.handle=pDatumPoint->handle;
		m_datumPoint3.dwPermission=pDatumPoint->dwPermission;
	}
	else if(hTempPointArr[2].ciHidType==0)
	{	//��������
		pDatumPoint=Ta.Point.FromHandle(m_datumPoint3.handle);
		*pDatumPoint=m_datumPoint3;
		pDatumPoint->UpdatePos(console.GetActiveModel());
	}
	//
	__int64 idarr[3];
	memcpy(&idarr[0],&hTempPointArr[0],8);
	memcpy(&idarr[1],&hTempPointArr[1],8);
	memcpy(&idarr[2],&hTempPointArr[2],8);
	if(idarr[0]==idarr[1] || idarr[0]==idarr[2]||idarr[1]==idarr[2])
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The start or end of datum line is overlap��");
#else 
		AfxMessageBox("��׼�ߵ�ʼ�ն˻�׼���غϣ�");
#endif
		return;
	}
	m_datumPlane.hPointArr[0]=hTempPointArr[0];
	m_datumPlane.hPointArr[1]=hTempPointArr[1];
	m_datumPlane.hPointArr[2]=hTempPointArr[2];
	CDialog::OnOK();
}
//
const DWORD HASHTABLESIZE1 = 500;
CStringKeyHashTable<long> CDatumPlanePropertyDlg::propHashtable;
void CDatumPlanePropertyDlg::InitPropHashtable()
{
	int id=1;
	propHashtable.CreateHashTable(HASHTABLESIZE1);
	//��׼�߻�����Ϣ
	propHashtable.SetValueAt("basic",id++);
	propHashtable.SetValueAt("handle",id++);
	propHashtable.SetValueAt("description",id++);
	propHashtable.SetValueAt("norm",id++);
	propHashtable.SetValueAt("norm.x",id++);
	propHashtable.SetValueAt("norm.y",id++);
	propHashtable.SetValueAt("norm.z",id++);
	//��׼������Ϣ
	propHashtable.SetValueAt("des_para_one",id++);
	propHashtable.SetValueAt("hDatumPt1",id++);
	propHashtable.SetValueAt("datum_pos_desc1",id++);	//��׼������
	propHashtable.SetValueAt("datum_pos_style1",id++);
	propHashtable.SetValueAt("des_para_two",id++);
	propHashtable.SetValueAt("hDatumPt2",id++);
	propHashtable.SetValueAt("datum_pos_desc2",id++);	//��׼������
	propHashtable.SetValueAt("datum_pos_style2",id++);
	propHashtable.SetValueAt("des_para_three",id++);
	propHashtable.SetValueAt("hDatumPt3",id++);
	propHashtable.SetValueAt("datum_pos_desc3",id++);	//��׼������
	propHashtable.SetValueAt("datum_pos_style3",id++);
	//
	propHashtable.SetValueAt("pos.x",id++);
	propHashtable.SetValueAt("pos.y",id++);
	propHashtable.SetValueAt("pos.z",id++);
	//��ڵ�ͬλ
	propHashtable.SetValueAt("des_para.hNode",id++);
	//ָ�������ϵ��������
	propHashtable.SetValueAt("des_para.hPart",id++);
	propHashtable.SetValueAt("datum_pos",id++);
	propHashtable.SetValueAt("datum_pos.x",id++);
	propHashtable.SetValueAt("datum_pos.y",id++);
	propHashtable.SetValueAt("datum_pos.z",id++);
	//���߽���
	propHashtable.SetValueAt("des_para.AXIS_INTERS.hDatum1",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.hDatum2",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.wing_offset_style1",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.wing_offset_style2",id++);//0.g;1.g1;2.g2;3.g3;4.�Զ���
	propHashtable.SetValueAt("des_para.AXIS_INTERS.wing_offset_dist1",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.wing_offset_dist2",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.near_vector",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.near_vector[0]",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.near_vector[1]",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.near_vector[2]",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.norm_offset",id++);	//����ƫ����;
	//�Ǹ����
	propHashtable.SetValueAt("des_para.RODEND.hRod",id++);				//�Ǹ־��
	propHashtable.SetValueAt("des_para.RODEND.direction",id++);			//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	propHashtable.SetValueAt("des_para.RODEND.offset_wing",id++);		//0.X֫;1.Y֫
	propHashtable.SetValueAt("des_para.RODEND.wing_offset_style",id++);	//0.g;1.g1;2.g2;3.g3;4.�Զ���
	propHashtable.SetValueAt("des_para.RODEND.wing_offset_dist",id++);
	propHashtable.SetValueAt("des_para.RODEND.len_offset_dist",id++);	//֫��ƫ��ֵ;����ƫ��ֵ;
	propHashtable.SetValueAt("des_para.RODEND.bIncOddEffect",id++);		//����ƫ��ֵ��������ͷӰ��
	propHashtable.SetValueAt("des_para.RODEND.norm_offset",id++);		//����ƫ����
	//�˼��Ͻڵ�Ϊ��׼
	propHashtable.SetValueAt("des_para.LINEPARTNODE.hLinePart",id++);	//�Ǹ־��
	propHashtable.SetValueAt("des_para.LINEPARTNODE.hNode",id++);		//��׼�ڵ���
	propHashtable.SetValueAt("des_para.LINEPARTNODE.bFaceOffset",id++);
	propHashtable.SetValueAt("des_para.LINEPARTNODE.direction",id++);	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	propHashtable.SetValueAt("des_para.LINEPARTNODE.offset_wing",id++);	//0.X֫;1.Y֫
	propHashtable.SetValueAt("des_para.LINEPARTNODE.wing_offset_style",id++);//0.g;1.g1;2.g2;3.g3;4.�Զ���
	propHashtable.SetValueAt("des_para.LINEPARTNODE.wing_offset_dist",id++);
	propHashtable.SetValueAt("des_para.LINEPARTNODE.len_offset_dist",id++);	//֫��ƫ��ֵ;����ƫ��ֵ;
	propHashtable.SetValueAt("des_para.LINEPARTNODE.norm_offset",id++);		//����ƫ����;
	propHashtable.SetValueAt("des_para.LINEPARTNODE.face_norm_offset",id++);
	propHashtable.SetValueAt("des_para.LINEPARTNODE.face_norm_offset[0]",id++);
	propHashtable.SetValueAt("des_para.LINEPARTNODE.face_norm_offset[1]",id++);
	propHashtable.SetValueAt("des_para.LINEPARTNODE.face_norm_offset[2]",id++);
	//��׼�����������겻���
	propHashtable.SetValueAt("des_para.DATUMLINE.hDatumLine",id++);
	propHashtable.SetValueAt("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2",id++);
	//��׼�����������겻���
	propHashtable.SetValueAt("des_para.DATUMPLANE.hDatumPlane",id++);
	propHashtable.SetValueAt("des_para.DATUMPLANE.cMirTransType",id++);
	propHashtable.SetValueAt("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2",id++);
}
//
static CString GetPointHandleStr()
{
	CString  sPointHandle="";
	CString sValue;
	int nPointNum=Ta.Point.GetNodeNum();
	for(int i=0;i<nPointNum;i++)
	{
		CLDSPoint Point=Ta.Point[i];
		sValue.Format("0X%X|",Point.handle);
		sPointHandle+=sValue;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	sPointHandle+="<New...>";
#else 
	sPointHandle+="<�½�...>";
#endif
	return sPointHandle;
}
void CDatumPlanePropertyDlg::DisplayDatumPlaneProperty()
{
	CPropTreeItem *pPropItem,*pGroupItem,*pRoot=m_propList.GetRootItem();
	CXhChar100 sText;
	m_propList.CleanTree();
	m_propList.m_nObjClassTypeId = 0;
	const int GROUP_DATUMPLANE=1,GROUP_DATUMPT1=2,GROUP_DATUMPT2=3,GROUP_DATUMPT3=4;
	UpdatePlane();
	//������Ϣ
	//m_pActivePt=&m_datumPoint1;
	CItemInfo*lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Basic Information";
#else 
	lpInfo->m_strPropName = "������Ϣ";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("basic");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPLANE); 
	//���
	lpInfo= new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Handle";
	lpInfo->m_strPropHelp = "Datum plane handle";
#else 
	lpInfo->m_strPropName = "���";
	lpInfo->m_strPropHelp = "��׼����";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("handle");
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPLANE);
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->SetReadOnly();
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Description";
	lpInfo->m_strPropHelp = "Datum plane description";
#else 
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "�Ի�׼���������";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("description");
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPLANE);
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//��׼�淨��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Datum Plane Normal";
	lpInfo->m_strPropHelp="The normal coordinates of datum plane";
#else 
	lpInfo->m_strPropName="��׼�淨��";
	lpInfo->m_strPropHelp="��׼�淨������";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("norm");
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPLANE);
	pPropItem->m_bHideChildren=FALSE;
	//X
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="The x normal coordinates of datum plane";
#else 
	lpInfo->m_strPropHelp="��׼�淨��X����";
#endif
	CPropTreeItem* pSonItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("norm.x");
	pSonItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPLANE);
	if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue=sText;
	pSonItem->SetReadOnly();
	//Y
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="The y normal coordinates of datum plane";
#else 
	lpInfo->m_strPropHelp="��׼�淨��Y����";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("norm.y");
	pSonItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPLANE);
	if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue=sText;
	pSonItem->SetReadOnly();
	//Z
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
	lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp="The z normal coordinates of datum plane";
#else 
	lpInfo->m_strPropHelp="��׼�淨��Z����";
#endif
	pSonItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("norm.z");
	pSonItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPLANE);
	if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
		pSonItem->m_lpNodeInfo->m_strPropValue=sText;
	pSonItem->SetReadOnly();
	//��׼��1
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Datum Plane Basic Information";
#else 
	lpInfo->m_strPropName = "������Ϣ";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("basic");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPT1);
	//��׼��1���
	lpInfo=new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbItems=GetPointHandleStr();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Handle";
	lpInfo->m_strPropHelp="The collected handle of existing datum points";
#else 
	lpInfo->m_strPropName="���";
	lpInfo->m_strPropHelp="���л�׼��������";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp=CDatumPlanePropertyDlg::GetPropID("hDatumPt1");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPT1);
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	pPropItem->SetReadOnly(FALSE);
	//����
	lpInfo=new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Description";
	lpInfo->m_strPropHelp="The description of the first datum point";
#else 
	lpInfo->m_strPropName="����";
	lpInfo->m_strPropHelp="��һ����׼�������";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp=CDatumPlanePropertyDlg::GetPropID("datum_pos_desc1");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPT1);
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//��׼�㶨λ��ʽ
	lpInfo=new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="0.Manual Input|1.Angle Ridge Vertex Locating Point|2.Rod Node Locating Point|3.Angle Center Line Intersection Point|4.Baseline Space Intersection Point|5.Specified Component Point Of Datum Line|6.Specified Component Point Of Datum Plane|7.Angle Ridge Offset|8.With The Node In The Same Position|9.Datum Rod Relative Coordinate";
	lpInfo->m_strPropName="Locate Mode";
	lpInfo->m_strPropHelp="The locate mode of the first datum point";
#else 
	lpInfo->m_cmbItems="0.�ֶ�����|1.�Ǹ���㶨λ��|2.�˼��ڵ㶨λ��|3.�Ǹ����߽���|4.��׼�߿ռ佻��|5.��׼����ָ�����������|6.��׼����ָ�������|7.��֫����ƫ�Ƶİ������|8.��ڵ�ͬλ|9.��׼�����ϵ��������";
	lpInfo->m_strPropName="��λ��ʽ";
	lpInfo->m_strPropHelp="��һ����׼��Ķ�λ��ʽ";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp=CDatumPlanePropertyDlg::GetPropID("datum_pos_style1");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPT1);
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//������Ϣ
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Parameter Information";
#else 
	lpInfo->m_strPropName = "������Ϣ";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para_one");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPT1);
	//UpdateDatumPointParaProperty(&m_propList,pGroupItem);
	//��׼��2
	//m_pActivePt=&m_datumPoint2;
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Basic Information";
#else 
	lpInfo->m_strPropName = "������Ϣ";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("basic");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPT2);
	//��׼����
	lpInfo=new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbItems=GetPointHandleStr();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Handle";
	lpInfo->m_strPropHelp="The collected handle of existing datum points";
#else 
	lpInfo->m_strPropName="���";
	lpInfo->m_strPropHelp="���л�׼��ľ������";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp=CDatumPlanePropertyDlg::GetPropID("hDatumPt2");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPT2);
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	pPropItem->SetReadOnly(FALSE);
	//����
	lpInfo=new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Description";
	lpInfo->m_strPropHelp="The description of the second datum point";
#else 
	lpInfo->m_strPropName="����";
	lpInfo->m_strPropHelp="�ڶ�����׼�������";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp=CDatumPlanePropertyDlg::GetPropID("datum_pos_desc2");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPT2);
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//��׼�㶨λ��ʽ
	lpInfo=new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="0.Manual Input|1.Angle Ridge Vertex Locating Point|2.Rod Node Locating Point|3.Angle Center Line Intersection Point|4.Baseline Space Intersection Point|5.Specified Component Point Of Datum Line|6.Specified Component Point Of Datum Plane|7.Angle Ridge Offset|8.With The Node In The Same Position|9.Datum Rod Relative Coordinate";
	lpInfo->m_strPropName="Locate Mode";
	lpInfo->m_strPropHelp="The locate mode of the second datum point ";
#else 
	lpInfo->m_cmbItems="0.�ֶ�����|1.�Ǹ���㶨λ��|2.�˼��ڵ㶨λ��|3.�Ǹ����߽���|4.��׼�߿ռ佻��|5.��׼����ָ�����������|6.��׼����ָ�������|7.��֫����ƫ�Ƶİ������|8.��ڵ�ͬλ|9.��׼�����ϵ��������";
	lpInfo->m_strPropName="��λ��ʽ";
	lpInfo->m_strPropHelp="�ڶ�����׼��Ķ�λ��ʽ";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp=CDatumPlanePropertyDlg::GetPropID("datum_pos_style2");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPT2);
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Parameter Information";
#else 
	lpInfo->m_strPropName = "������Ϣ";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para_two");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPT2);
	//UpdateDatumPointParaProperty(&m_propList,pGroupItem);
	//��׼��3
	//m_pActivePt=&m_datumPoint3;
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Basic Information";
#else 
	lpInfo->m_strPropName = "������Ϣ";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("basic");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPT3);
	//��׼����
	lpInfo=new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbItems=GetPointHandleStr();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Handle";
	lpInfo->m_strPropHelp="The collected handle of existing datum points";
#else 
	lpInfo->m_strPropName="���";
	lpInfo->m_strPropHelp="���л�׼��ľ������";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp=CDatumPlanePropertyDlg::GetPropID("hDatumPt3");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPT3);
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	pPropItem->SetReadOnly(FALSE);
	//����
	lpInfo=new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Description";
	lpInfo->m_strPropHelp="The description of the third datum point";
#else 
	lpInfo->m_strPropName="����";
	lpInfo->m_strPropHelp="��������׼�������";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp=CDatumPlanePropertyDlg::GetPropID("datum_pos_desc3");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPT3);
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//��׼�㶨λ��ʽ
	lpInfo=new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="0.Manual Input|1.Angle Ridge Vertex Locating Point|2.Rod Node Locating Point|3.Angle Center Line Intersection Point|4.Baseline Space Intersection Point|5.Specified Component Point Of Datum Line|6.Specified Component Point Of Datum Plane|7.Angle Ridge Offset|8.With The Node In The Same Position|9.Datum Rod Relative Coordinate";
	lpInfo->m_strPropName="Locate Mode";
	lpInfo->m_strPropHelp="The locate mode of third datum point ";
#else 
	lpInfo->m_cmbItems="0.�ֶ�����|1.�Ǹ���㶨λ��|2.�˼��ڵ㶨λ��|3.�Ǹ����߽���|4.��׼�߿ռ佻��|5.��׼����ָ�����������|6.��׼����ָ�������|7.��֫����ƫ�Ƶİ������|8.��ڵ�ͬλ|9.��׼�����ϵ��������";
	lpInfo->m_strPropName="��λ��ʽ";
	lpInfo->m_strPropHelp="��������׼��Ķ�λ��ʽ";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp=CDatumPlanePropertyDlg::GetPropID("datum_pos_style3");
	pPropItem->m_dwPropGroup=GetSingleWord(GROUP_DATUMPT3);
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Parameter Information";
#else 
	lpInfo->m_strPropName = "������Ϣ";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CDatumPlanePropertyDlg::GetPropID("des_para_three");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DATUMPT3);
	//
	UpdateDatumPointParaProperty(&m_propList,NULL);
	m_propList.Redraw();
}

int GetDatumPointWingOffset(CLDSPoint *pPoint,bool bWingOffset2=false)
{
	long wing_x0_y1=0,wing_offset_dist=0,wing_offset_style=0;
	CLDSLineAngle *pLineAngle=NULL;
	if(pPoint->datum_pos_style==1)
	{	//1.�Ǹ��������Ϊ��׼
		pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.RODEND.hRod,CLS_LINEANGLE);
		wing_x0_y1=pPoint->des_para.RODEND.offset_wing;
		wing_offset_dist=(int)pPoint->des_para.RODEND.wing_offset_dist;
		wing_offset_style=pPoint->des_para.RODEND.wing_offset_style;
	}
	else if(pPoint->datum_pos_style==2)
	{	//2.�˼��Ͻڵ�Ϊ��׼
		pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.RODNODE.hRod ,CLS_LINEANGLE);
		wing_x0_y1=pPoint->des_para.RODNODE.offset_wing;
		wing_offset_dist=(int)pPoint->des_para.RODNODE.wing_offset_dist;
		wing_offset_style=pPoint->des_para.RODNODE.wing_offset_style;
	}
	else if(pPoint->datum_pos_style==3)
	{	//3.�Ǹ����߽���
		wing_offset_dist=pPoint->des_para.AXIS_INTERS.wing_offset_dist1;
		wing_offset_style=pPoint->des_para.AXIS_INTERS.wing_offset_style1;
		CLDSLineAngle *pOtherAngle=NULL;
		pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE);
		pOtherAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
		if(bWingOffset2)
		{
			CLDSLineAngle *pTempAngle=pOtherAngle;
			pOtherAngle=pLineAngle;
			pLineAngle=pTempAngle;
			wing_offset_dist=pPoint->des_para.AXIS_INTERS.wing_offset_dist2;
			wing_offset_style=pPoint->des_para.AXIS_INTERS.wing_offset_style2;
		}
		if(pLineAngle&&pOtherAngle&&wing_offset_style<4)
		{
			f3dPoint line_vec1=pLineAngle->End()-pLineAngle->Start();
			f3dPoint line_vec2=pOtherAngle->End()-pOtherAngle->Start();
			f3dPoint work_norm=line_vec1^line_vec2;
			normalize(work_norm);
			if(fabs(work_norm*pLineAngle->get_norm_x_wing())>fabs(work_norm*pLineAngle->get_norm_y_wing()))
				wing_x0_y1=0;
			else
				wing_x0_y1=1;
		}
	}
	if(pLineAngle&&wing_offset_style<4)
	{
		if(wing_x0_y1==0)
			wing_offset_dist=pLineAngle->GetWingXG(wing_offset_style);
		else if(wing_x0_y1==1)
			wing_offset_dist=pLineAngle->GetWingYG(wing_offset_style);
	}
	return wing_offset_dist;
}
int CDatumPlanePropertyDlg::GetPropValueStr(long id,char *valueStr)
{
	CXhChar100 sText;
	if(GetPropID("handle")==id)
		sText.Printf("0X%X",m_datumPlane.handle);
	else if(GetPropID("description")==id)
		sText.Printf("%s",m_datumPlane.description);
	else if(GetPropID("norm.x")==id)
	{
		sText.Printf("%f",m_datumPlane.GetWorkNorm().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("norm.y")==id)
	{
		sText.Printf("%f",m_datumPlane.GetWorkNorm().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("norm.z")==id)
	{
		sText.Printf("%f",m_datumPlane.GetWorkNorm().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("hDatumPt1")==id ||GetPropID("hDatumPt2")==id||GetPropID("hDatumPt3")==id)
	{
		CLDSPoint *pCurPoint=NULL;
		if(GetPropID("hDatumPt1")==id)
			pCurPoint=&m_datumPoint1;
		else if(GetPropID("hDatumPt2")==id)
			pCurPoint=&m_datumPoint2;
		else
			pCurPoint=&m_datumPoint3;
		if(pCurPoint->handle>0X20)
			sText.Printf("0X%X",pCurPoint->handle);
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("<New...>");
#else 
			sText.Copy("<�½�...>");
#endif
	}
	else if(GetPropID("datum_pos_desc1")==id||GetPropID("datum_pos_desc2")==id||GetPropID("datum_pos_desc3")==id)
	{
		if(GetPropID("datum_pos_desc1")==id)
			sText.Printf("%s",m_datumPoint1.description);
		else if(GetPropID("datum_pos_desc2")==id)
			sText.Printf("%s",m_datumPoint2.description);
		else
			sText.Printf("%s",m_datumPoint3.description);
	}
	else if(GetPropID("datum_pos_style1")==id||GetPropID("datum_pos_style2")==id||GetPropID("datum_pos_style3")==id)
	{	
		CLDSPoint *pCurPoint=NULL;
		if(GetPropID("datum_pos_style1")==id)
			pCurPoint=&m_datumPoint1;
		else if(GetPropID("datum_pos_style2")==id)
			pCurPoint=&m_datumPoint2;
		else
			pCurPoint=&m_datumPoint3;
#ifdef AFX_TARG_ENU_ENGLISH
		if(pCurPoint->datum_pos_style==0)
			sText.Copy("0.Manual Input");
		else if(pCurPoint->datum_pos_style==1)
			sText.Copy("1.Angle Ridge Vertex Locating Point");
		else if(pCurPoint->datum_pos_style==2)
			sText.Copy("2.Rod Node Locating Point");
		else if(pCurPoint->datum_pos_style==3)
			sText.Copy("3.Angle Center Line Intersection Point");
		else if(pCurPoint->datum_pos_style==4)
			sText.Copy("4.Baseline Space Intersection Point");
		else if(pCurPoint->datum_pos_style==5)
			sText.Copy("5.Specified Component Point Of Datum Line");
		else if(pCurPoint->datum_pos_style==6)
			sText.Copy("6.Specified Component Point Of Datum Plane");
		else if(pCurPoint->datum_pos_style==7)
			sText.Copy("7.Angle Ridge Offset");
		else if(pCurPoint->datum_pos_style==8)
			sText.Copy("8.With The Node In The Same Position");
		else if(pCurPoint->datum_pos_style==9)
			sText.Copy("9.Datum Rod Relative Coordinate");
#else 
		if(pCurPoint->datum_pos_style==0)
			sText.Copy("0.�ֶ�����");
		else if(pCurPoint->datum_pos_style==1)
			sText.Copy("1.�Ǹ���㶨λ��");
		else if(pCurPoint->datum_pos_style==2)
			sText.Copy("2.�˼��ڵ㶨λ��");
		else if(pCurPoint->datum_pos_style==3)
			sText.Copy("3.�Ǹ����߽���");
		else if(pCurPoint->datum_pos_style==4)
			sText.Copy("4.��׼�߿ռ佻��");
		else if(pCurPoint->datum_pos_style==5)
			sText.Copy("5.��׼����ָ�����������");
		else if(pCurPoint->datum_pos_style==6)
			sText.Copy("6.��׼����ָ�������");
		else if(pCurPoint->datum_pos_style==7)
			sText.Copy("7.��֫����ƫ�Ƶİ������");
		else if(pCurPoint->datum_pos_style==8)
			sText.Copy("8.��ڵ�ͬλ");
		else if(pCurPoint->datum_pos_style==9)
			sText.Copy("9.��׼�����ϵ��������");
#endif
	}
	if(m_pActivePt&&m_pActivePt->datum_pos_style==0)	//ֱ��ָ��
	{
		if(GetPropID("pos.x")==id)
		{
			sText.Printf("%f",m_pActivePt->Position().x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("pos.y")==id)
		{
			sText.Printf("%f",m_pActivePt->Position().y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("pos.z")==id)
		{
			sText.Printf("%f",m_pActivePt->Position().z);
			SimplifiedNumString(sText);
		}
	}
	else if(m_pActivePt&&m_pActivePt->datum_pos_style==1)	//�Ǹ���㶨λ
	{
		if(GetPropID("des_para.RODEND.hRod")==id)	//�Ǹ־��
			sText.Printf("0X%X",m_pActivePt->des_para.RODEND.hRod);
		else if(GetPropID("des_para.RODEND.direction")==id)	
		{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_pActivePt->des_para.RODEND.direction==0)
				sText.Copy("0.Start->End");
			else 
				sText.Copy("1.End->Start");
#else 
			if(m_pActivePt->des_para.RODEND.direction==0)
				sText.Copy("0.ʼ->��");
			else 
				sText.Copy("1.��->ʼ");
#endif
		}
		else if(GetPropID("des_para.RODEND.offset_wing")==id)//0.X֫;1.Y֫
		{	//0.X֫;1.Y֫
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_pActivePt->des_para.RODEND.offset_wing==0)
				sText.Copy("0.X Leg");
			else 
				sText.Copy("1.Y Leg");
#else 
			if(m_pActivePt->des_para.RODEND.offset_wing==0)
				sText.Copy("0.X֫");
			else 
				sText.Copy("1.Y֫");
#endif
		}
		else if(GetPropID("des_para.RODEND.wing_offset_style")==id)	
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(m_pActivePt->des_para.RODEND.wing_offset_style==0)
				sText.Copy("g");
			else if(m_pActivePt->des_para.RODEND.wing_offset_style==1)
				sText.Copy("g1");
			else if(m_pActivePt->des_para.RODEND.wing_offset_style==2)
				sText.Copy("g2");
			else if(m_pActivePt->des_para.RODEND.wing_offset_style==3)
				sText.Copy("g3");
			else 
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("user specify");
#else 
				sText.Copy("�Զ���");
#endif
		}
		else if(GetPropID("des_para.RODEND.wing_offset_dist")==id)
		{
			sText.Printf("%d",GetDatumPointWingOffset(m_pActivePt));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.RODEND.len_offset_dist")==id)	
		{	//����ƫ��ֵ;
			sText.Printf("%f",m_pActivePt->des_para.RODEND.len_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.RODEND.bIncOddEffect")==id)		
		{	//����ƫ��ֵ��������ͷӰ��
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_pActivePt->des_para.RODEND.bIncOddEffect)
				sText.Copy("Yes");
			else 
				sText.Copy("No");
#else 
			if(m_pActivePt->des_para.RODEND.bIncOddEffect)
				sText.Copy("��");
			else 
				sText.Copy("��");
#endif
		}
		else if(GetPropID("des_para.RODEND.norm_offset")==id)//����ƫ����
		{
			CDesignThickPara thick(m_pActivePt->des_para.RODEND.norm_offset);
			sText.Printf("%d",thick.Thick(console.GetActiveModel()));
		}
	}
	else if(m_pActivePt&&m_pActivePt->datum_pos_style==2)	//�˼��ڵ㶨λ
	{
		if(GetPropID("des_para.LINEPARTNODE.hLinePart")==id)	//�Ǹ־��
			sText.Printf("0X%X",m_pActivePt->des_para.RODNODE.hRod );
		else if(GetPropID("des_para.LINEPARTNODE.hNode")==id)	//��׼�ڵ���
			sText.Printf("0X%X",m_pActivePt->des_para.RODNODE.hNode);
		else if(GetPropID("des_para.LINEPARTNODE.bFaceOffset")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_pActivePt->des_para.RODNODE.bFaceOffset)
				sText.Copy("Yes");
			else 
				sText.Copy("No");
#else 
			if(m_pActivePt->des_para.RODNODE.bFaceOffset)
				sText.Copy("��");
			else 
				sText.Copy("��");
#endif
		}
		else if(GetPropID("des_para.LINEPARTNODE.direction")==id)	
		{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_pActivePt->des_para.RODNODE.direction==0)
				sText.Copy("0.Start->End");
			else 
				sText.Copy("1.End->Start");
#else 
			if(m_pActivePt->des_para.RODNODE.direction==0)
				sText.Copy("0.ʼ->��");
			else 
				sText.Copy("1.��->ʼ");
#endif
		}
		else if(GetPropID("des_para.LINEPARTNODE.offset_wing")==id)	
		{	//0.X֫;1.Y֫
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_pActivePt->des_para.RODNODE.offset_wing==0)
				sText.Copy("0.X Leg");
			else 
				sText.Copy("1.Y Leg");
#else 
			if(m_pActivePt->des_para.RODNODE.offset_wing==0)
				sText.Copy("0.X֫");
			else 
				sText.Copy("1.Y֫");
#endif
		}
		else if(GetPropID("des_para.LINEPARTNODE.wing_offset_style")==id)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(m_pActivePt->des_para.RODNODE.wing_offset_style==0)
				sText.Copy("g");
			else if(m_pActivePt->des_para.RODNODE.wing_offset_style==1)
				sText.Copy("g1");
			else if(m_pActivePt->des_para.RODNODE.wing_offset_style==2)
				sText.Copy("g2");
			else if(m_pActivePt->des_para.RODNODE.wing_offset_style==3)
				sText.Copy("g3");
			else
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("user specify");
#else 
				sText.Copy("�Զ���");
#endif
		}	
		else if(GetPropID("des_para.LINEPARTNODE.wing_offset_dist")==id)
		{
			sText.Printf("%d",GetDatumPointWingOffset(m_pActivePt));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.LINEPARTNODE.len_offset_dist")==id)	
		{	//����ƫ��ֵ
			sText.Printf("%f",m_pActivePt->des_para.RODNODE.len_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.LINEPARTNODE.norm_offset")==id)	//����ƫ����
		{
			CDesignThickPara thick(m_pActivePt->des_para.RODNODE.norm_offset);
			sText.Printf("%d",thick.Thick(console.GetActiveModel()));
		}
		else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset")==id)
			sText.Printf("%s",CStdVector::GetVecTypeString(m_pActivePt->des_para.RODNODE.vPlaneNormal));
		else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]")==id)
		{
			sText.Printf("%f",m_pActivePt->des_para.RODNODE.vPlaneNormal.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]")==id)
		{
			sText.Printf("%f",m_pActivePt->des_para.RODNODE.vPlaneNormal.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]")==id)
		{
			sText.Printf("%f",m_pActivePt->des_para.RODNODE.vPlaneNormal.z);
			SimplifiedNumString(sText);
		}
	}
	else if(m_pActivePt&&m_pActivePt->datum_pos_style==3)	//�˼����߽���
	{
		if(GetPropID("des_para.AXIS_INTERS.hDatum1")==id)
			sText.Printf("0X%X",m_pActivePt->des_para.AXIS_INTERS.hDatum1);
		else if(GetPropID("des_para.AXIS_INTERS.hDatum2")==id)
			sText.Printf("0X%X",m_pActivePt->des_para.AXIS_INTERS.hDatum2);
		else if(GetPropID("des_para.AXIS_INTERS.wing_offset_style1")==id)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(m_pActivePt->des_para.AXIS_INTERS.wing_offset_style1==0)
				sText.Copy("g");
			else if(m_pActivePt->des_para.AXIS_INTERS.wing_offset_style1==1)
				sText.Copy("g1");
			else if(m_pActivePt->des_para.AXIS_INTERS.wing_offset_style1==2)
				sText.Copy("g2");
			else if(m_pActivePt->des_para.AXIS_INTERS.wing_offset_style1==3)
				sText.Copy("g3");
			else
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("user specify");
#else 
				sText.Copy("�Զ���");
#endif
		}
		else if(GetPropID("des_para.AXIS_INTERS.wing_offset_style2")==id)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(m_pActivePt->des_para.AXIS_INTERS.wing_offset_style2==0)
				sText.Copy("g");
			else if(m_pActivePt->des_para.AXIS_INTERS.wing_offset_style2==1)
				sText.Copy("g1");
			else if(m_pActivePt->des_para.AXIS_INTERS.wing_offset_style2==2)
				sText.Copy("g2");
			else if(m_pActivePt->des_para.AXIS_INTERS.wing_offset_style2==3)
				sText.Copy("g3");
			else
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("user specify");
#else 
				sText.Copy("�Զ���");
#endif
		}
		else if(GetPropID("des_para.AXIS_INTERS.wing_offset_dist1")==id)
		{
			sText.Printf("%d",GetDatumPointWingOffset(m_pActivePt));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.AXIS_INTERS.wing_offset_dist2")==id)
		{
			sText.Printf("%d",GetDatumPointWingOffset(m_pActivePt,true));
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.AXIS_INTERS.near_vector")==id)
			sText.Printf("%s",CStdVector::GetVecTypeString(f3dPoint(m_pActivePt->des_para.AXIS_INTERS.near_vector)));
		else if(GetPropID("des_para.AXIS_INTERS.near_vector[0]")==id)
		{
			sText.Printf("%f",m_pActivePt->des_para.AXIS_INTERS.near_vector[0]);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.AXIS_INTERS.near_vector[1]")==id)
		{
			sText.Printf("%f",m_pActivePt->des_para.AXIS_INTERS.near_vector[1]);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.AXIS_INTERS.near_vector[2]")==id)
		{
			sText.Printf("%f",m_pActivePt->des_para.AXIS_INTERS.near_vector[2]);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.AXIS_INTERS.norm_offset")==id)	//����ƫ����;
		{
			CDesignThickPara thick(m_pActivePt->des_para.AXIS_INTERS.norm_offset);
			sText.Printf("%d",thick.Thick(console.GetActiveModel()));
		}
	}
	else if(m_pActivePt&&m_pActivePt->datum_pos_style==4)	//��׼�߿ռ佻��
	{
		if(GetPropID("des_para.AXIS_INTERS.hDatum1")==id)
			sText.Printf("0X%X",m_pActivePt->des_para.AXIS_INTERS.hDatum1);
		else if(GetPropID("des_para.AXIS_INTERS.hDatum2")==id)
			sText.Printf("0X%X",m_pActivePt->des_para.AXIS_INTERS.hDatum2);
	}
	else if(m_pActivePt&&m_pActivePt->datum_pos_style==5)	//��׼����ָ�����������
	{
		if(GetPropID("des_para.DATUMLINE.hDatumLine")==id)
			sText.Printf("0X%X",m_pActivePt->des_para.DATUMLINE.hDatumLine);
		else if(GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_pActivePt->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
				sText.Printf("0.X axis");
			else if(m_pActivePt->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
				sText.Printf("1.Y axis");
			else 
				sText.Printf("2.Z axis");
#else 
			if(m_pActivePt->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
				sText.Printf("0.X��");
			else if(m_pActivePt->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
				sText.Printf("1.Y��");
			else 
				sText.Printf("2.Z��");
#endif
		}
	}
	else if(m_pActivePt&&m_pActivePt->datum_pos_style==6)	//��׼����ָ�������
	{
		if(GetPropID("des_para.DATUMPLANE.hDatumPlane")==id)
			sText.Printf("0X%X",m_pActivePt->des_para.DATUMPLANE.hDatumPlane);
		else if(GetPropID("des_para.DATUMPLANE.cMirTransType")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_pActivePt->des_para.DATUMPLANE.cMirTransType=='X')
				sText.Printf("X axis symmetry");
			else if(m_pActivePt->des_para.DATUMPLANE.cMirTransType=='Y')
				sText.Printf("Y axis symmetry");
			else if(m_pActivePt->des_para.DATUMPLANE.cMirTransType=='Z')
				sText.Printf("Z axis symmetry");
			else 
				sText.Printf("No symmetrical reference");
#else 
			if(m_pActivePt->des_para.DATUMPLANE.cMirTransType=='X')
				sText.Printf("X��Գ�");
			else if(m_pActivePt->des_para.DATUMPLANE.cMirTransType=='Y')
				sText.Printf("Y��Գ�");
			else if(m_pActivePt->des_para.DATUMPLANE.cMirTransType=='Z')
				sText.Printf("Z��Գ�");
			else 
				sText.Printf("�޶ԳƲ���");
#endif
		}
		else if(GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_pActivePt->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
				sText.Printf("0.X axis");
			else if(m_pActivePt->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
				sText.Printf("1.Y axis");
			else 
				sText.Printf("2.Z axis");
#else 
			if(m_pActivePt->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
				sText.Printf("0.X��");
			else if(m_pActivePt->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
				sText.Printf("1.Y��");
			else 
				sText.Printf("2.Z��");
#endif
		}
	}
	else if(m_pActivePt&&m_pActivePt->datum_pos_style==7)	//�ؽǸ���֫����ƫ�Ƶİ������
	{
		if(GetPropID("des_para.RODEND.hRod")==id)	//�Ǹ־��
			sText.Printf("0X%X",m_pActivePt->des_para.RODEND.hRod);
		else if(GetPropID("des_para.RODEND.direction")==id)	
		{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_pActivePt->des_para.RODEND.direction==0)
				sText.Copy("0.Start->End");
			else 
				sText.Copy("1.End->Start");
#else 
			if(m_pActivePt->des_para.RODEND.direction==0)
				sText.Copy("0.ʼ->��");
			else 
				sText.Copy("1.��->ʼ");
#endif
		}
		else if(GetPropID("des_para.RODEND.len_offset_dist")==id)	
		{	//����ƫ��ֵ;
			sText.Printf("%f",m_pActivePt->des_para.RODEND.len_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.RODEND.norm_offset")==id)//����ƫ����
		{
			CDesignThickPara thick(m_pActivePt->des_para.RODEND.norm_offset);
			sText.Printf("%d",thick.Thick(console.GetActiveModel()));
		}
	}
	else if(m_pActivePt&&m_pActivePt->datum_pos_style==8)	//��ڵ�ͬλ
	{
		if(GetPropID("des_para.hNode")==id)
			sText.Printf("0X%X",m_pActivePt->des_para.hNode);
	}
	else if(m_pActivePt&&m_pActivePt->datum_pos_style==9)	//ָ�������ϵ��������
	{
		if(GetPropID("des_para.hPart")==id)
			sText.Printf("0X%X",m_pActivePt->des_para.hPart);
		else if(GetPropID("datum_pos.x")==id)
		{
			sText.Printf("%f",m_pActivePt->datum_pos.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("datum_pos.y")==id)
		{
			sText.Printf("%f",m_pActivePt->datum_pos.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("datum_pos.z")==id)
		{
			sText.Printf("%f",m_pActivePt->datum_pos.z);
			SimplifiedNumString(sText);
		}
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
long CDatumPlanePropertyDlg::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}
//���ѡ�����ĺ�������
void CDatumPlanePropertyDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	CLDSPoint* pPoint=&m_datumPoint1;
	if(m_propList.m_iPropGroup==2)
		pPoint=&m_datumPoint2;
	else if(m_propList.m_iPropGroup==3)
		pPoint=&m_datumPoint3;
	CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(GetEventPropId()==GetPropID("des_para.RODEND.hRod"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
			pPoint->des_para.RODEND.hRod=pLinePart->handle;
	}
	else 
	{
#endif
		if(GetEventPropId()==GetPropID("des_para.LINEPARTNODE.hLinePart"))
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
			if(pLinePart)
				pPoint->des_para.RODNODE.hRod =pLinePart->handle;
		}
		else if(GetEventPropId()==GetPropID("des_para.LINEPARTNODE.hNode"))
		{
			CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
			if(pNode)
				pPoint->des_para.RODNODE.hNode=pNode->handle;
		}
		else if(GetEventPropId()==GetPropID("des_para.AXIS_INTERS.hDatum1"))
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
			if(pLinePart)
				pPoint->des_para.AXIS_INTERS.hDatum1=pLinePart->handle;
		}
		else if(GetEventPropId()==GetPropID("des_para.AXIS_INTERS.hDatum2"))
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
			if(pLinePart)
				pPoint->des_para.AXIS_INTERS.hDatum2=pLinePart->handle;
		}
		else if(GetEventPropId()==GetPropID("des_para.hNode"))
		{
			CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
			if(pNode)
				pPoint->des_para.hNode=pNode->handle;
		}
		else if(GetEventPropId()==GetPropID("des_para.hPart"))
		{
			CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
			if(pPart)
				pPoint->des_para.hPart=pPart->handle;
		}
	}
	pPoint->UpdatePos(console.GetActiveModel());
	m_fPointX=pPoint->Position().x;
	m_fPointY=pPoint->Position().y;
	m_fPointZ=pPoint->Position().z;
}

HANDLEPOINT CDatumPlanePropertyDlg::ToHandlePoint(CLDSPoint* pDatumPoint)
{
	HANDLEPOINT hPointEx;//=&m_datumPlane.hPointArr[serial-1];
	if(pDatumPoint->handle>0x20)
	{
		hPointEx.handle=pDatumPoint->handle;
		hPointEx.ciHidType=0;
		return hPointEx;
	}
	if(pDatumPoint->datum_pos_style==8)
	{
		hPointEx.ciHidType=1;
		hPointEx.handle=pDatumPoint->des_para.hNode;
	}
	else if(pDatumPoint->datum_pos_style==1&&pDatumPoint->handle<0x20&&
		(pDatumPoint->des_para.RODEND.len_offset_dist==0&&pDatumPoint->des_para.RODEND.wing_offset_style==4&&pDatumPoint->des_para.RODEND.wing_offset_dist==0))
	{
		hPointEx.handle=pDatumPoint->des_para.RODEND.hRod;
		if(pDatumPoint->des_para.RODEND.direction==0)
			hPointEx.ciHidType='S';
		else
			hPointEx.ciHidType='E';
	}
	else
	{
		hPointEx.ciHidType=0;
		hPointEx.handle=0;
	}
	return hPointEx;
}
void CDatumPlanePropertyDlg::OnBnUpdatePlane()
{
	UpdateData();
	CLDSPlane plane;
	CLDSPoint* pDatumPoint=NULL;
	plane.hPointArr[0]=ToHandlePoint(&m_datumPoint1);
	if(plane.hPointArr[0].handle<0x20)
	{	//������׼��
		m_bNewPoint1=TRUE;
		pDatumPoint=console.AppendPoint();
		m_datumPoint1.handle=pDatumPoint->handle;
		m_datumPoint1.dwPermission=pDatumPoint->dwPermission;
	}
	else if(plane.hPointArr[0].ciHidType==0)	//��������
	{
		pDatumPoint=Ta.Point.FromHandle(m_datumPoint1.handle);	
		*pDatumPoint=m_datumPoint1;
	}
	plane.hPointArr[1]=ToHandlePoint(&m_datumPoint2);
	if(plane.hPointArr[1].handle<0x20)
	{		//������׼��
		m_bNewPoint2=TRUE;
		pDatumPoint=console.AppendPoint();
		m_datumPoint2.handle=pDatumPoint->handle;
		m_datumPoint2.dwPermission=pDatumPoint->dwPermission;
	}
	else if(plane.hPointArr[1].ciHidType==0)	//��������
	{
		pDatumPoint=Ta.Point.FromHandle(m_datumPoint2.handle);
		*pDatumPoint=m_datumPoint2;
	}
	plane.hPointArr[2]=ToHandlePoint(&m_datumPoint3);
	if(plane.hPointArr[2].handle<0x20)
	{		//������׼��
		m_bNewPoint3=TRUE;
		pDatumPoint=console.AppendPoint();
		m_datumPoint3.handle=pDatumPoint->handle;
		m_datumPoint3.dwPermission=pDatumPoint->dwPermission;
	}
	else if(plane.hPointArr[2].ciHidType==0)	//��������
	{
		pDatumPoint=Ta.Point.FromHandle(m_datumPoint3.handle);
		*pDatumPoint=m_datumPoint3;
	}

	if(plane.UpdatePlane(console.GetActiveModel()))
	{
		strncpy(plane.description,m_datumPlane.description,100);
		long hcopy=m_datumPlane.handle;
		m_datumPlane=plane;
		m_datumPlane.handle=hcopy;
		m_fPointX=plane.GetBasePos().x;//GetWorkNorm().x;
		m_fPointY=plane.GetBasePos().y;
		m_fPointZ=plane.GetBasePos().z;
		CXhChar16 sText;
		if(GetPropValueStr(CDatumPlanePropertyDlg::GetPropID("norm.x"),sText)>0)
			m_propList.SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("norm.x"),sText);
		if(GetPropValueStr(CDatumPlanePropertyDlg::GetPropID("norm.y"),sText)>0)
			m_propList.SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("norm.y"),sText);
		if(GetPropValueStr(CDatumPlanePropertyDlg::GetPropID("norm.z"),sText)>0)
			m_propList.SetItemPropValue(CDatumPlanePropertyDlg::GetPropID("norm.z"),sText);
		UpdateData(FALSE);
	}
}

void CDatumPlanePropertyDlg::OnBnCopyPlane()
{
	CLDSPlane plane;
	UpdateData();
	plane.hPointArr[0]=m_datumPoint1.handle;
	plane.hPointArr[1]=m_datumPoint2.handle;
	plane.hPointArr[2]=m_datumPoint3.handle;
	if( console.FromPointHandle(plane.hPointArr[0])==NULL||
		console.FromPointHandle(plane.hPointArr[1])==NULL||
		console.FromPointHandle(plane.hPointArr[2])==NULL)
		return;
	if(plane.UpdatePlane(console.GetActiveModel()))
	{
		m_fPointX=plane.GetBasePos().x;
		m_fPointY=plane.GetBasePos().y;
		m_fPointZ=plane.GetBasePos().z;
		WritePlaneToClip(plane.GetBasePos(),plane.GetWorkNorm());
		UpdateData(FALSE);
	}
}

void CDatumPlanePropertyDlg::OnBnCopyNorm()
{
	CLDSPlane plane;
	UpdateData();
	plane.hPointArr[0]=m_datumPoint1.handle;
	plane.hPointArr[1]=m_datumPoint2.handle;
	plane.hPointArr[2]=m_datumPoint3.handle;
	if( console.FromPointHandle(plane.hPointArr[0])==NULL||
		console.FromPointHandle(plane.hPointArr[1])==NULL||
		console.FromPointHandle(plane.hPointArr[2])==NULL)
		return;
	if(plane.UpdatePlane(console.GetActiveModel()))
	{
		//m_fPointX=plane.GetBasePos().x;
		//m_fPointY=plane.GetBasePos().y;
		//m_fPointZ=plane.GetBasePos().z;
		WritePointToClip(plane.GetWorkNorm());
		UpdateData(FALSE);
	}
}

void CDatumPlanePropertyDlg::OnEnChangeEdtX()
{
	UpdateData();
	if(m_pActivePt&&(m_pActivePt->datum_pos_style==0||m_pActivePt->datum_pos_style==5||m_pActivePt->datum_pos_style==6))
	{
		m_pActivePt->SetPositionX(m_fPointX);
		CPropTreeItem* pFineItem;
		if(m_propList.m_iPropGroup==3)
			pFineItem=m_propList.FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_three"),NULL);
		else if(m_propList.m_iPropGroup==2)
			pFineItem=m_propList.FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_two"),NULL);
		else
			pFineItem=m_propList.FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_one"),NULL);
		UpdateDatumPointParaProperty(&m_propList,pFineItem);
	}
}

void CDatumPlanePropertyDlg::OnEnChangeEdtY()
{
	UpdateData();
	if(m_pActivePt&&(m_pActivePt->datum_pos_style==0||m_pActivePt->datum_pos_style==5||m_pActivePt->datum_pos_style==6))
	{	
		m_pActivePt->SetPositionY(m_fPointY);
		CPropTreeItem* pFineItem;
		if(m_propList.m_iPropGroup==3)
			pFineItem=m_propList.FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_three"),NULL);
		else if(m_propList.m_iPropGroup==2)
			pFineItem=m_propList.FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_two"),NULL);
		else
			pFineItem=m_propList.FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_one"),NULL);
		UpdateDatumPointParaProperty(&m_propList,pFineItem);
	}
}

void CDatumPlanePropertyDlg::OnEnChangeEdtZ()
{
	UpdateData();
	if(m_pActivePt&&(m_pActivePt->datum_pos_style==0||m_pActivePt->datum_pos_style==5||m_pActivePt->datum_pos_style==6))
	{	
		m_pActivePt->SetPositionZ(m_fPointZ);
		CPropTreeItem* pFineItem;
		if(m_propList.m_iPropGroup==3)
			pFineItem=m_propList.FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_three"),NULL);
		else if(m_propList.m_iPropGroup==2)
			pFineItem=m_propList.FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_two"),NULL);
		else
			pFineItem=m_propList.FindItemByPropId(CDatumPlanePropertyDlg::GetPropID("des_para_one"),NULL);
		UpdateDatumPointParaProperty(&m_propList,pFineItem);
	}
}

BOOL CDatumPlanePropertyDlg::UpdatePlane()
{
	if(m_datumPlane.handle>0x20)
		return m_datumPlane.UpdatePlane(console.GetActiveModel());
	else
		return FALSE;
}
