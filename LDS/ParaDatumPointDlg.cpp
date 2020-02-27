// ParaDatumPointDlg.cpp : 实现文件
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "ParaDatumPointDlg.h"
#include "LmaDlg.h"
#include "env_def.h"
#include "ScrTools.h"
#include "ReportPointDlg.h"
#include "DisplayPointProperty.h"


// CParaDatumPointDlg 对话框

IMPLEMENT_DYNAMIC(CParaDatumPointDlg, CDialog)

CParaDatumPointDlg::CParaDatumPointDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CParaDatumPointDlg::IDD, pParent)
{
	m_fPointX=0;
	m_fPointY=0;
	m_fPointZ=0;
}

CParaDatumPointDlg::~CParaDatumPointDlg()
{
}

void CParaDatumPointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Text(pDX, IDC_E_X, m_fPointX);
	DDX_Text(pDX, IDC_E_Y, m_fPointY);
	DDX_Text(pDX, IDC_E_Z, m_fPointZ);
}


BEGIN_MESSAGE_MAP(CParaDatumPointDlg, CDialog)
	ON_EN_CHANGE(IDC_E_X, OnEnChangeEdtX)
	ON_EN_CHANGE(IDC_E_Y, OnEnChangeEdtY)
	ON_EN_CHANGE(IDC_E_Z, OnEnChangeEdtZ)
END_MESSAGE_MAP()


static void SetEditState(CPropertyList *pPropList,CLDSPoint* pDatumPoint)
{
	CParaDatumPointDlg* pParaPointDlg=(CParaDatumPointDlg*)pPropList->GetParent();
	BOOL bReadOnly=FALSE;
	if(pDatumPoint->datum_pos_style!=BYTE(0))
		bReadOnly=TRUE;
	((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_X))->SetReadOnly(bReadOnly);
	((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Y))->SetReadOnly(bReadOnly);
	((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Z))->SetReadOnly(bReadOnly);
	if(pDatumPoint->datum_pos_style==5)
	{
		if(pDatumPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_X))->SetReadOnly(FALSE);
		else if(pDatumPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Y))->SetReadOnly(FALSE);
		else if(pDatumPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2)
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Z))->SetReadOnly(FALSE);
	}
	else if(pDatumPoint->datum_pos_style==6)
	{
		if(pDatumPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
		{
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Y))->SetReadOnly(FALSE);
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Z))->SetReadOnly(FALSE);
		}
		else if(pDatumPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
		{
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_X))->SetReadOnly(FALSE);
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Z))->SetReadOnly(FALSE);
		}
		else if(pDatumPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==2)
		{
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_X))->SetReadOnly(FALSE);
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Y))->SetReadOnly(FALSE);
		}
	}
}
static void UpdateDatumPointParaProperty(CPropertyList* pPropList,CPropTreeItem* pParentItme)
{
	CItemInfo* lpInfo=NULL;
	CXhChar100 valueStr;
	CParaDatumPointDlg* pParaPointDlg=(CParaDatumPointDlg*)pPropList->GetParent();
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL,*pRootItem=pParentItme;
	pPropList->DeleteAllSonItems(pParentItme);
	if(pParentItme==NULL)
		return;
	CLDSPoint* pDatumPoint=&pParaPointDlg->m_xPoint;
	if(pDatumPoint->datum_pos_style==0)		//直接指定坐标
	{	//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Datum point's X coordinate";
#else 
		lpInfo->m_strPropHelp="基准点X坐标";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("pos.x");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Datum point's Y coordinate";
#else 
		lpInfo->m_strPropHelp="基准点Y坐标";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("pos.y");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Datum point's Z coordinate";
#else 
		lpInfo->m_strPropHelp="基准点Z坐标";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("pos.z");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==1)	//角钢楞点
	{	//基准角钢句柄
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pDatumPoint->des_para.hPart,CLS_LINEPART);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
		if(pLinePart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Unknown Part Handle";
		else 
			lpInfo->m_strPropName.Format("Datum Part %s Handle",pLinePart->GetPartTypeName());
#else 
			lpInfo->m_strPropName="未知构件句柄";
		else 
			lpInfo->m_strPropName.Format("基准%s句柄",pLinePart->GetPartTypeName());
#endif
		lpInfo->m_strPropHelp=lpInfo->m_strPropName;
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.RODEND.hRod");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		if(pLinePart==NULL||pLinePart->IsAngle())
		{	//偏移肢
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Leg|1.Y Leg";
			lpInfo->m_strPropName="Offset Leg";
			lpInfo->m_strPropHelp="Offset Leg。";
#else 
			lpInfo->m_cmbItems="0.X肢|1.Y肢";
			lpInfo->m_strPropName="偏移肢";
			lpInfo->m_strPropHelp="偏移肢。";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.RODEND.offset_wing");
			if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//偏移类型  0.g;1.g1;2.g2;3.g3;4.自定义
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User Defined|";
			lpInfo->m_strPropName="Leg Offset Style";
			lpInfo->m_strPropHelp="Offset style is preconcert G or user-defining offset.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|自定义|";
			lpInfo->m_strPropName="肢向偏移类型";
			lpInfo->m_strPropHelp="偏移类型预定准距或自定义偏移距离。";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.RODEND.wing_offset_style");
			if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//偏移距离
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset";
			lpInfo->m_strPropHelp="Leg Offset。";
#else 
			lpInfo->m_strPropName="肢向偏移值";
			lpInfo->m_strPropHelp="肢向偏移值。";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.RODEND.wing_offset_dist");
			if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pDatumPoint->des_para.RODEND.wing_offset_style!=4);
		}
		//法向偏移量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="Normal Offset";
#else 
		lpInfo->m_strPropName="法向偏移量";
		lpInfo->m_strPropHelp="法向偏移量";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.RODEND.norm_offset");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//纵向偏移方向 0.始->终;1.终->始
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Length Offset Vertor";
		lpInfo->m_strPropHelp="Length offset vertor,along datum rod from start to end or from end to start offset.";
#else 
		lpInfo->m_cmbItems="0.始->终|1.终->始|";
		lpInfo->m_strPropName="纵向偏移方向";
		lpInfo->m_strPropHelp="纵向偏移方向,沿基准杆件始->终方向偏移或沿基准杆件终->始偏移。";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.RODEND.direction");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//纵向偏移值
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Length Offset";
		lpInfo->m_strPropHelp="Length Offset";
#else 
		lpInfo->m_strPropName="纵向偏移值";
		lpInfo->m_strPropHelp="纵向偏移值";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.RODEND.len_offset_dist");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//纵向偏移值计入正负头影响
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName="Odd Length Influence";
		lpInfo->m_strPropHelp="Whether or not included in odd length influence of vertical offset.";
#else 
		lpInfo->m_cmbItems="是|否";
		lpInfo->m_strPropName="正负头影响";
		lpInfo->m_strPropHelp="纵向偏移是否计入正负头影响。";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.RODEND.bIncOddEffect");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==2)	//杆件上节点定位
	{
		//基准杆件句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod Handle";
		lpInfo->m_strPropHelp="Datum rod's handle";
#else 
		lpInfo->m_strPropName="基准杆件句柄";
		lpInfo->m_strPropHelp="基准杆件句柄。";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.hLinePart");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//基准节点句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Node Handle";
		lpInfo->m_strPropHelp="Datum Node Handle。";
#else 
		lpInfo->m_strPropName="基准节点句柄";
		lpInfo->m_strPropHelp="基准节点句柄。";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.hNode");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(pDatumPoint->des_para.RODNODE.hRod,CLS_LINEPART);
		if(pLinePart&&pLinePart->IsAngle())
		{	//偏移肢
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Leg|1.Y Leg";
			lpInfo->m_strPropName="Offset Leg";
			lpInfo->m_strPropHelp="Offset Leg。";
#else 
			lpInfo->m_cmbItems="0.X肢|1.Y肢";
			lpInfo->m_strPropName="偏移肢";
			lpInfo->m_strPropHelp="偏移肢。";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.offset_wing");
			if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//偏移类型  0.g;1.g1;2.g2;3.g3;4.自定义
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User Defined|";
			lpInfo->m_strPropName="Leg Offset Style";
			lpInfo->m_strPropHelp="Offset style is preconcert G or user-defining offset.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|自定义|";
			lpInfo->m_strPropName="肢向偏移类型";
			lpInfo->m_strPropHelp="偏移类型预定准距或自定义偏移距离。";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_style");
			if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//偏移距离
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset";
			lpInfo->m_strPropHelp="Leg Offset。";
#else 
			lpInfo->m_strPropName="肢向偏移值";
			lpInfo->m_strPropHelp="肢向偏移值。";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_dist");
			if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pDatumPoint->des_para.RODNODE.wing_offset_style!=4);
		}
		//纵向偏移方向 0.始->终;1.终->始
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Length Offset Vertor";
		lpInfo->m_strPropHelp="Length offset vertor,along datum rod from start to end or from end to start offset.";
#else 
		lpInfo->m_cmbItems="0.始->终|1.终->始|";
		lpInfo->m_strPropName="纵向偏移方向";
		lpInfo->m_strPropHelp="纵向偏移方向,沿基准杆件始->终方向偏移或沿基准杆件终->始偏移。";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.direction");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//纵向偏移值
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Length Offset";
		lpInfo->m_strPropHelp="Length Offset";
#else 
		lpInfo->m_strPropName="纵向偏移值";
		lpInfo->m_strPropHelp="纵向偏移值";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.len_offset_dist");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//法向偏移量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="Normal Offset";
#else 
		lpInfo->m_strPropName="法向偏移量";
		lpInfo->m_strPropHelp="法向偏移量";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.norm_offset");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//平推
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName="Planar Location";
		lpInfo->m_strPropHelp="planar location";
#else 
		lpInfo->m_cmbItems="是|否";
		lpInfo->m_strPropName="平推";
		lpInfo->m_strPropHelp="平推";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.bFaceOffset");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		if(pDatumPoint->des_para.RODNODE.bFaceOffset)
		{	//平推面法线
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
			lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset Plane Normal";
			lpInfo->m_strPropHelp="The offset plane's normal";
#else 
			lpInfo->m_strPropName="平推面法线";
			lpInfo->m_strPropHelp="平推面法线";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset");
			if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			int vectorType=CStdVector::GetVectorType(pDatumPoint->des_para.RODNODE.vPlaneNormal);
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
			lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The x coordinate of offset plane's normal";
#else 
			lpInfo->m_strPropHelp="平推面法线X方向分量";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
			if(pParaPointDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
			lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The y coordinate of offset plane's normal";
#else 
			lpInfo->m_strPropHelp="平推面法线Y方向分量";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
			if(pParaPointDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
			lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="The z coordinate of offset plane's normal";
#else 
			lpInfo->m_strPropHelp="平推面法线Z方向分量";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
			if(pParaPointDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
		}
	}
	else if(pDatumPoint->datum_pos_style==3)	//杆件心线交点
	{
		//基准杆件句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod 1 Handle";
		lpInfo->m_strPropHelp="Datum rod1's handle.";
#else 
		lpInfo->m_strPropName="基准杆件1句柄";
		lpInfo->m_strPropHelp="基准杆件1句柄。";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.hDatum1");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		CLDSLinePart *pLinePart1=(CLDSLinePart*)console.FromPartHandle(pDatumPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEPART);
		if(pLinePart1&&pLinePart1->IsAngle())
		{	//偏移类型  0.g;1.g1;2.g2;3.g3;4.自定义
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User Defined|";
			lpInfo->m_strPropName="Leg Offset Style 1";
			lpInfo->m_strPropHelp="Datum angle offset style: preconcert stadia or user-defining offset。";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|自定义|";
			lpInfo->m_strPropName="肢向偏移类型1";
			lpInfo->m_strPropHelp="基准角钢1偏移类型:预定准距或自定义偏移距离。";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_style1");
			if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//偏移距离
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset 1";
			lpInfo->m_strPropHelp="Leg offset of datum angle 1.";
#else 
			lpInfo->m_strPropName="肢向偏移值1";
			lpInfo->m_strPropHelp="基准角钢1肢向偏移值。";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist1");
			if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pDatumPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
		}
		//基准杆件2句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod 2 Handle";
		lpInfo->m_strPropHelp="The handle of datum rod 2";
#else 
		lpInfo->m_strPropName="基准杆件2句柄";
		lpInfo->m_strPropHelp="基准杆件2句柄";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.hDatum2");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		CLDSLinePart *pLinePart2=(CLDSLinePart*)console.FromPartHandle(pDatumPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEPART);
		if(pLinePart2&&pLinePart2->IsAngle())
		{	//偏移类型  0.g;1.g1;2.g2;3.g3;4.自定义
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User Defined|";
			lpInfo->m_strPropName="Leg Offset Style 2";
			lpInfo->m_strPropHelp="Datum angle 2 offset style: preconcert stadia or user-defining offset。";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|自定义|";
			lpInfo->m_strPropName="肢向偏移类型2";
			lpInfo->m_strPropHelp="基准角钢2偏移类型:预定准距或自定义偏移距离。";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_style2");
			if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//偏移距离
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset 2";
			lpInfo->m_strPropHelp="Leg offset of datum angle 2";
#else 
			lpInfo->m_strPropName="肢向偏移值2";
			lpInfo->m_strPropHelp="基准角钢2肢向偏移值。";
#endif
			pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist2");
			if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(pDatumPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
		}
		//法向偏移量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="Normal Offset";
#else 
		lpInfo->m_strPropName="法向偏移量";
		lpInfo->m_strPropHelp="法向偏移量";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.norm_offset");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//近似法线
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Near Normal";
		lpInfo->m_strPropHelp="Near normal";
#else 
		lpInfo->m_strPropName="近似法线";
		lpInfo->m_strPropHelp="近似法线";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		int vectorType=CStdVector::GetVectorType(f3dPoint(pDatumPoint->des_para.AXIS_INTERS.near_vector));
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="X coordinate of near normal";
#else 
		lpInfo->m_strPropHelp="近似法线X方向分量";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]");
		if(pParaPointDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Y coordinate of near normal";
#else 
		lpInfo->m_strPropHelp="近似法线Y方向分量";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]");
		if(pParaPointDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Z coordinate of near normal";
#else 
		lpInfo->m_strPropHelp="近似法线Z方向分量";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]");
		if(pParaPointDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
	}
	else if(pDatumPoint->datum_pos_style==4)	//基准线交点
	{
		//基准线1句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Line 1 Handle";
		lpInfo->m_strPropHelp="Datum line 1 handle";
#else 
		lpInfo->m_strPropName="基准线1句柄";
		lpInfo->m_strPropHelp="基准线1句柄";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.hDatum1");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//基准线2句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Line 2 Handle";
		lpInfo->m_strPropHelp="Datum line 2 handle";
#else 
		lpInfo->m_strPropName="基准线2句柄";
		lpInfo->m_strPropHelp="基准线2句柄";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.hDatum2");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==5)	//基准线上轴向坐标不变点
	{
		//基准线句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Line Handle";
		lpInfo->m_strPropHelp="Datum line handle";
#else 
		lpInfo->m_strPropName="基准线句柄";
		lpInfo->m_strPropHelp="基准线句柄";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.DATUMLINE.hDatumLine");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//指定轴向
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.X Axis|1.Y Axis|2.Z Axis";
		lpInfo->m_strPropName="Specify Axis";
		lpInfo->m_strPropHelp="Specify axis";
#else 
		lpInfo->m_cmbItems="0.X轴|1.Y轴|2.Z轴";
		lpInfo->m_strPropName="指定轴向";
		lpInfo->m_strPropHelp="指定轴向";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==6)	//基准面上轴向坐标不变点
	{
		//基准面句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbItems=CDisplayPointProperty::GetPlaneHandleStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Plane Handle";
#else 
		lpInfo->m_strPropName="基准面句柄";
#endif
		lpInfo->m_strPropHelp=CDisplayPointProperty::GetSelectPlaneDesc(pDatumPoint);
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.DATUMPLANE.hDatumPlane");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//坐标对称变换操作
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="None|X Mirror|Y Mirror|Z Mirror";
		lpInfo->m_strPropName="Reference Point Mirror Style";
		lpInfo->m_strPropHelp="First according to current point create mirroring reference point,second intersects with datum plane,then mirrors to origin position。";
#else 
		lpInfo->m_cmbItems="无对称参照|X轴对称|Y轴对称|Z轴对称";
		lpInfo->m_strPropName="参照点对称方式";
		lpInfo->m_strPropHelp="先根据当前点生成对称的参照点，再与基准面进行线面求，然后再对称回原位置。";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.DATUMPLANE.cMirTransType");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//指定轴向
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.X Axis|1.Y Axis|2.Z Axis";
		lpInfo->m_strPropName="Specify Axis";
		lpInfo->m_strPropHelp="Specify axis";
#else 
		lpInfo->m_cmbItems="0.X轴|1.Y轴|2.Z轴";
		lpInfo->m_strPropName="指定轴向";
		lpInfo->m_strPropHelp="指定轴向";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==7)	//沿角钢两肢法向偏移的包铁楞点
	{
		//基准角钢句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Angle Handle";
		lpInfo->m_strPropHelp="Datum angle's handle";
#else 
		lpInfo->m_strPropName="基准角钢句柄";
		lpInfo->m_strPropHelp="基准角钢句柄。";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.RODEND.hRod");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//纵向偏移方向 0.始->终;1.终->始
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Length Offset Vertor";
		lpInfo->m_strPropHelp="Length offset vertor,along datum angle from start to end or from end to start offset.";
#else 
		lpInfo->m_cmbItems="0.始->终|1.终->始|";
		lpInfo->m_strPropName="纵向偏移方向";
		lpInfo->m_strPropHelp="纵向偏移方向,沿基准角钢始->终方向偏移或沿基准角钢终->始偏移。";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.RODEND.direction");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//纵向偏移值
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Length Offset";
		lpInfo->m_strPropHelp="Length Offset";
#else 
		lpInfo->m_strPropName="纵向偏移值";
		lpInfo->m_strPropHelp="纵向偏移值";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.RODEND.len_offset_dist");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//法向偏移量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="Normal Offset";
#else 
		lpInfo->m_strPropName="法向偏移量";
		lpInfo->m_strPropHelp="法向偏移量";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.RODEND.norm_offset");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==8)	//与节点同为
	{	//基准节点句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Node Handle";
		lpInfo->m_strPropHelp="Datum Node Handle。";
#else 
		lpInfo->m_strPropName="基准节点句柄";
		lpInfo->m_strPropHelp="基准节点句柄。";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.hNode");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pDatumPoint->datum_pos_style==9)	//指定构件上的相对坐标
	{
		//基准杆件句柄
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Part Handle";
		lpInfo->m_strPropHelp="Datum Part Handle。";
#else 
		lpInfo->m_strPropName="基准构件句柄";
		lpInfo->m_strPropHelp="基准构件句柄。";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.hPart");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//相对坐标
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Relative Coordinate";
		lpInfo->m_strPropHelp="Relative Coordinate";
#else 
		lpInfo->m_strPropName="相对坐标";
		lpInfo->m_strPropHelp="相对坐标";
#endif
		pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("datum_pos");
		if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The x component of relative coordinate";
#else 
		lpInfo->m_strPropHelp="相对坐标X方向分量";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CParaDatumPointDlg::GetPropID("datum_pos.x");
		if(pParaPointDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The y component of relative coordinate";
#else 
		lpInfo->m_strPropHelp="相对坐标Y方向分量";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CParaDatumPointDlg::GetPropID("datum_pos.y");
		if(pParaPointDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="The Z component of relative coordinate";
#else 
		lpInfo->m_strPropHelp="相对坐标Z方向分量";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = CParaDatumPointDlg::GetPropID("datum_pos.z");
		if(pParaPointDlg->GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
}
static BOOL ModifyParaPointProperty(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CParaDatumPointDlg *pParaPointDlg=(CParaDatumPointDlg*)pPropList->GetParent();
	CPropTreeItem* pFineItem=NULL,*pParentItem=NULL;
	BOOL bRefreshPtPara=FALSE;
	CLDSPoint *pPoint=&pParaPointDlg->m_xPoint;
	char value_str[100]="";
	if(CParaDatumPointDlg::GetPropID("datum_pos_desc")==pItem->m_idProp )
		strncpy(pPoint->description,valueStr,50);
	else if(CParaDatumPointDlg::GetPropID("hDatumPt")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("<New...>")==0)
#else 
		if(valueStr.Compare("<新建...>")==0)
#endif
		{
			CLDSPoint* pPt=new CLDSPoint();
			*pPoint=*pPt;
			pParaPointDlg->m_fPointX=0;
			pParaPointDlg->m_fPointY=0;
			pParaPointDlg->m_fPointZ=0;
			pParaPointDlg->UpdateData(FALSE);
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_X))->SetReadOnly(FALSE);
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Y))->SetReadOnly(FALSE);
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Z))->SetReadOnly(FALSE);
		}
		else
		{
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->handle));
			CLDSPoint* pPt=Ta.Point.FromHandle(pPoint->handle);
			*pPoint=*pPt;
			pPoint->UpdatePos(console.GetActiveModel());
			pParaPointDlg->m_fPointX=pPoint->Position().x;
			pParaPointDlg->m_fPointY=pPoint->Position().y;
			pParaPointDlg->m_fPointZ=pPoint->Position().z;
			pParaPointDlg->UpdateData(FALSE);
			SetEditState(pPropList,pPoint);
		}
		bRefreshPtPara=TRUE;
		CXhChar50 sText;
		pPropList->SetItemPropValue(CParaDatumPointDlg::GetPropID("datum_pos_desc"),pPoint->description);
		pParaPointDlg->GetPropValueStr(CParaDatumPointDlg::GetPropID("datum_pos_style"),sText);
		pPropList->SetItemPropValue(CParaDatumPointDlg::GetPropID("datum_pos_style"),sText);
	}
	else if(CParaDatumPointDlg::GetPropID("datum_pos_style")==pItem->m_idProp)
	{
		int nPosStyle=valueStr[0] - '0';
		if(pPoint->datum_pos_style!=nPosStyle)
		{
			pPoint->datum_pos_style=nPosStyle;
			bRefreshPtPara=TRUE;
		}
		SetEditState(pPropList,pPoint);
	}
	else if(pPoint->datum_pos_style==0)	//直接指定
	{
		if(CParaDatumPointDlg::GetPropID("pos.x")==pItem->m_idProp)
		{
			pPoint->SetPositionX(atof(valueStr));
			pParaPointDlg->m_fPointX=pPoint->Position().x;
			pParaPointDlg->UpdateData(FALSE);
		}
		else if(CParaDatumPointDlg::GetPropID("pos.y")==pItem->m_idProp)
		{
			pPoint->SetPositionY(atof(valueStr));
			pParaPointDlg->m_fPointY=pPoint->Position().y;
			pParaPointDlg->UpdateData(FALSE);
		}
		else if(CParaDatumPointDlg::GetPropID("pos.z")==pItem->m_idProp)
		{
			pPoint->SetPositionZ(atof(valueStr));
			pParaPointDlg->m_fPointZ=pPoint->Position().z;
			pParaPointDlg->UpdateData(FALSE);
		}
	}
	else if(pPoint->datum_pos_style==1)	//角钢楞点定位
	{
		if(CParaDatumPointDlg::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)	//角钢句柄
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODEND.hRod));
		else if(CParaDatumPointDlg::GetPropID("des_para.RODEND.direction")==pItem->m_idProp)	//0.基准角钢方向:始->终;1.终->始
			pPoint->des_para.RODEND.direction=valueStr[0]-'0';
		else if(CParaDatumPointDlg::GetPropID("des_para.RODEND.offset_wing")==pItem->m_idProp)//0.X肢;1.Y肢
			pPoint->des_para.RODEND.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CParaDatumPointDlg::GetPropID("des_para.RODEND.wing_offset_style")==pItem->m_idProp)	
		{	//0.g;1.g1;2.g2;3.g3;4.自定义
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
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.RODEND.hRod,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG&&pPoint->des_para.RODEND.offset_wing==0)
					jgzj=pLineAngle->xWingXZhunJu;	//X肢
				else if(pLineAngle->m_bEnableTeG&&pPoint->des_para.RODEND.offset_wing==1)
					jgzj=pLineAngle->xWingYZhunJu;	//X肢
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pPoint->des_para.RODEND.wing_offset_style==0)
					pPoint->des_para.RODEND.wing_offset_dist=jgzj.g;
				else if(pPoint->des_para.RODEND.wing_offset_style==1)
					pPoint->des_para.RODEND.wing_offset_dist=jgzj.g1;
				else if(pPoint->des_para.RODEND.wing_offset_style==2)
					pPoint->des_para.RODEND.wing_offset_dist=jgzj.g2;
				else if(pPoint->des_para.RODEND.wing_offset_style==3)
					pPoint->des_para.RODEND.wing_offset_dist=jgzj.g3;
			}
			long idProp=CParaDatumPointDlg::GetPropID("des_para.RODEND.wing_offset_dist");
			if(pParaPointDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.RODEND.wing_offset_style!=4);
		}
#endif
		else if(CParaDatumPointDlg::GetPropID("des_para.RODEND.wing_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.wing_offset_dist=atof(valueStr);
		else if(CParaDatumPointDlg::GetPropID("des_para.RODEND.len_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
		else if(CParaDatumPointDlg::GetPropID("des_para.RODEND.bIncOddEffect")==pItem->m_idProp)		
		{	//纵向偏移值计入正负头影响
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("是")==0)
#endif
				pPoint->des_para.RODEND.bIncOddEffect=TRUE;
			else 
				pPoint->des_para.RODEND.bIncOddEffect=FALSE;
		}
	}
	else if(pPoint->datum_pos_style==2)	//杆件节点定位
	{
		if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)	//角钢句柄
			sscanf(valueStr,"%X",&(pPoint->des_para.RODNODE.hRod));
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.hNode")==pItem->m_idProp)	//基准节点句柄
			sscanf(valueStr,"%X",&(pPoint->des_para.RODNODE.hNode));
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.direction")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.direction=valueStr[0]-'0';
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.offset_wing")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_style")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.自定义
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
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.RODNODE.hRod,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG&&pPoint->des_para.RODEND.offset_wing==0)
					jgzj=pLineAngle->xWingXZhunJu;	//X肢
				else if(pLineAngle->m_bEnableTeG&&pPoint->des_para.RODEND.offset_wing==1)
					jgzj=pLineAngle->xWingYZhunJu;	//X肢
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pPoint->des_para.RODNODE.wing_offset_style==0)
					pPoint->des_para.RODNODE.wing_offset_dist=jgzj.g;
				else if(pPoint->des_para.RODNODE.wing_offset_style==1)
					pPoint->des_para.RODNODE.wing_offset_dist=jgzj.g1;
				else if(pPoint->des_para.RODNODE.wing_offset_style==2)
					pPoint->des_para.RODNODE.wing_offset_dist=jgzj.g2;
				else if(pPoint->des_para.RODNODE.wing_offset_style==3)
					pPoint->des_para.RODNODE.wing_offset_dist=jgzj.g3;
			}
			long idProp=CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_dist");
			if(pParaPointDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.RODNODE.wing_offset_style!=4);
		}	
#endif
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.wing_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODNODE.wing_offset_dist=atof(valueStr);
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.len_offset_dist")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.len_offset_dist=atof(valueStr);
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.bFaceOffset")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("是")==0)
#endif
				pPoint->des_para.RODNODE.bFaceOffset=TRUE;
			else 
				pPoint->des_para.RODNODE.bFaceOffset=FALSE;
			pPropList->DeleteItemByPropId(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset"));
			pPropList->DeleteItemByPropId(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]"));
			pPropList->DeleteItemByPropId(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]"));
			pPropList->DeleteItemByPropId(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"));
			if(pPoint->des_para.RODNODE.bFaceOffset)
			{	//平推面法线
				CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL,*pRootItem=pPropList->GetRootItem();
				//
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
				lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();

#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Offset Plane Normal";
				lpInfo->m_strPropHelp="The offset plane's normal";
#else 
				lpInfo->m_strPropName="平推面法线";
				lpInfo->m_strPropHelp="平推面法线";
#endif
				pPropItem = pPropList->InsertItem(pRootItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset");
				if(pParaPointDlg->GetPropValueStr(pPropItem->m_idProp,value_str)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				int vectorType=CStdVector::GetVectorType(pPoint->des_para.RODNODE.vPlaneNormal);
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMONEDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The x coordinate of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="平推面法线X方向分量";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
				pSonPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
				if(pParaPointDlg->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMONEDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The y coordinate of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="平推面法线Y方向分量";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,TRUE);
				pSonPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
				if(pParaPointDlg->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMONEDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="The z coordinate of offset plane's normal";
#else 
				lpInfo->m_strPropHelp="平推面法线Z方向分量";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,TRUE);
				pSonPropItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
				if(pParaPointDlg->GetPropValueStr(pSonPropItem->m_idProp,value_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
			}
		}
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset")==pItem->m_idProp)
		{
			f3dPoint norm=CStdVector::GetVector(valueStr[0]-'0');
			pPoint->des_para.RODNODE.vPlaneNormal=norm;
			long idProp=CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.x);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.y);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.z);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');

		}
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.x=atof(valueStr);
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.y=atof(valueStr);
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.z=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==3)	//杆件心线交点
	{
		if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum1));
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum2));
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_style1")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.自定义
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
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE);
			CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG)
				{
					f3dPoint work_norm;
					if(pLineAngle&&pCrossAngle)
					{
						f3dPoint line_vec1=pLineAngle->End()-pLineAngle->Start();
						f3dPoint line_vec2=pCrossAngle->End()-pCrossAngle->Start();
						work_norm=line_vec1^line_vec2;
						normalize(work_norm);
					}
					if(fabs(work_norm*pLineAngle->get_norm_x_wing())>fabs(work_norm*pLineAngle->get_norm_y_wing()))
						jgzj=pLineAngle->xWingXZhunJu;	//X肢
					else 
						jgzj=pLineAngle->xWingYZhunJu;	//Y肢
				}
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==0)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==1)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g1;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==2)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g2;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==3)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g3;
			}
			long idProp=CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist1");
			if(pParaPointDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
		}
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_style2")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.自定义
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
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
			CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG)
				{
					f3dPoint work_norm;
					if(pLineAngle&&pCrossAngle)
					{
						f3dPoint line_vec1=pLineAngle->End()-pLineAngle->Start();
						f3dPoint line_vec2=pCrossAngle->End()-pCrossAngle->Start();
						work_norm=line_vec1^line_vec2;
						normalize(work_norm);
					}
					if(fabs(work_norm*pLineAngle->get_norm_x_wing())>fabs(work_norm*pLineAngle->get_norm_y_wing()))
						jgzj=pLineAngle->xWingXZhunJu;	//X肢
					else 
						jgzj=pLineAngle->xWingYZhunJu;	//Y肢
				}
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==0)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==1)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g1;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==2)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g2;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==3)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g3;
			}
			long idProp=CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist2");
			if(pParaPointDlg->GetPropValueStr(idProp,value_str)>0)
				pPropList->SetItemPropValue(idProp,CString(value_str));
			pPropList->SetItemReadOnly(idProp,pPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
		}
#endif
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist1")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.wing_offset_dist1=atol(valueStr);
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.wing_offset_dist2")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.wing_offset_dist2=atol(valueStr);
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector")==pItem->m_idProp)
		{
			f3dPoint vec=CStdVector::GetVector(valueStr[0]-'0');
			pPoint->des_para.AXIS_INTERS.near_vector[0]=vec.x;
			pPoint->des_para.AXIS_INTERS.near_vector[1]=vec.y;
			pPoint->des_para.AXIS_INTERS.near_vector[2]=vec.z;
			long idProp=CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[0]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[1]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]");
			pPropList->SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[2]);
			pPropList->SetItemReadOnly(idProp,valueStr[0]!='0');
		}
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[0]=atof(valueStr);
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[1]=atof(valueStr);
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[2]=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==4)	//基准线空间交点
	{
		if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum1));
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum2));
	}
	else if(pPoint->datum_pos_style==5)	//基准线上指定坐标分量点
	{
		if(CParaDatumPointDlg::GetPropID("des_para.DATUMLINE.hDatumLine")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.DATUMLINE.hDatumLine));
		else if(CParaDatumPointDlg::GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==pItem->m_idProp)
		{	
			pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=valueStr[0]-'0';
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_X))->SetReadOnly(TRUE);
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Y))->SetReadOnly(TRUE);
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Z))->SetReadOnly(TRUE);
			if(pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
				((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_X))->SetReadOnly(FALSE);
			else if(pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
				((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Y))->SetReadOnly(FALSE);
			else if(pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2)
				((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Z))->SetReadOnly(FALSE);
		}
	}
	else if(pPoint->datum_pos_style==6)	//基准面上指定轴向点
	{
		if(CParaDatumPointDlg::GetPropID("des_para.DATUMPLANE.hDatumPlane")==pItem->m_idProp)
		{
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.DATUMPLANE.hDatumPlane));
			CLDSPlane *pPlane=console.FromPlaneHandle(pPoint->des_para.DATUMPLANE.hDatumPlane);
			if(pPlane)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				CString sValue="Datum Plane：";
#else 
				CString sValue="基准面：";
#endif
				CString ss;
				ss.Format("%s_0X%X",pPlane->description,pPlane->handle); 
				sValue+=ss;
				pItem->m_lpNodeInfo->m_strPropHelp=sValue;
				pPropList->SetPromptString(sValue);
			}
		}
		else if(CParaDatumPointDlg::GetPropID("des_para.DATUMPLANE.cMirTransType")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("X Mirror")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='X';
			else if(valueStr.CompareNoCase("Y Mirror")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='Y';
			else if(valueStr.CompareNoCase("Z Mirror")==0)
#else 
			if(valueStr.CompareNoCase("X轴对称")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='X';
			else if(valueStr.CompareNoCase("Y轴对称")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='Y';
			else if(valueStr.CompareNoCase("Z轴对称")==0)
#endif
				pPoint->des_para.DATUMPLANE.cMirTransType='Z';
			else
				pPoint->des_para.DATUMPLANE.cMirTransType=0;
		}
		else if(CParaDatumPointDlg::GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==pItem->m_idProp)
		{	
			pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=valueStr[0]-'0';
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_X))->SetReadOnly(TRUE);
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Y))->SetReadOnly(TRUE);
			((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Z))->SetReadOnly(TRUE);
			if(pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
			{
				((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Y))->SetReadOnly(FALSE);
				((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Z))->SetReadOnly(FALSE);
			}
			else if(pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
			{
				((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_X))->SetReadOnly(FALSE);
				((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Z))->SetReadOnly(FALSE);
			}
			else if(pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==2)
			{
				((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_X))->SetReadOnly(FALSE);
				((CEdit*)pParaPointDlg->GetDlgItem(IDC_E_Y))->SetReadOnly(FALSE);
			}
		}
	}
	else if(pPoint->datum_pos_style==7)	//沿角钢两肢法向偏移的包铁楞点
	{
		if(CParaDatumPointDlg::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)	//角钢句柄
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODEND.hRod));
		else if(CParaDatumPointDlg::GetPropID("des_para.RODEND.direction")==pItem->m_idProp)	//0.基准角钢方向:始->终;1.终->始
			pPoint->des_para.RODEND.direction=valueStr[0]-'0';
		else if(CParaDatumPointDlg::GetPropID("des_para.RODEND.len_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==8)	//与节点同位
	{
		if(CParaDatumPointDlg::GetPropID("des_para.hNode")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.hNode));
	}
	else if(pPoint->datum_pos_style==9)	//指定构件上的相对坐标
	{
		if(CParaDatumPointDlg::GetPropID("des_para.hPart")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.hPart));
		else if(CParaDatumPointDlg::GetPropID("datum_pos.x")==pItem->m_idProp)
			pPoint->datum_pos.x=atof(valueStr);
		else if(CParaDatumPointDlg::GetPropID("datum_pos.y")==pItem->m_idProp)
			pPoint->datum_pos.y=atof(valueStr);
		else if(CParaDatumPointDlg::GetPropID("datum_pos.z")==pItem->m_idProp)
			pPoint->datum_pos.z=atof(valueStr);
	}
	if(bRefreshPtPara)
	{
		pFineItem=pPropList->FindItemByPropId(CParaDatumPointDlg::GetPropID("des_para"),&pParentItem);
		UpdateDatumPointParaProperty(pPropList,pFineItem);
	}
	return TRUE;
}
static BOOL ButtonClickParaPointLine(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CReportVectorDlg vecDlg;
	CPropTreeItem *pParentItem=NULL;
	CParaDatumPointDlg *pParaPointDlg=(CParaDatumPointDlg*)pPropList->GetParent();
	if(pParaPointDlg==NULL)
		return FALSE;
	CLDSPoint *pPoint=&pParaPointDlg->m_xPoint;
	CXhChar100 sText;
	if(pPoint->datum_pos_style==1)
	{	//角钢楞点定位
		if(CParaDatumPointDlg::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)
			pParaPointDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);//基准杆件
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CParaDatumPointDlg::GetPropID("des_para.RODEND.norm_offset")==pItem->m_idProp)
		{	//法向偏移量
			CString sThickStr;
			sThickStr=pPoint->des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("输入法向偏移量字符串",&sThickStr,'s');
#endif
			_snprintf(pPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
			if(pParaPointDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
	}
	else if(pPoint->datum_pos_style==2)
	{	//杆件上节点定位
		if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)
			pParaPointDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//基准杆件
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.hNode")==pItem->m_idProp)
			pParaPointDlg->SelectObject(CLS_NODE,pItem->m_idProp);		//基准节点
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.norm_offset")==pItem->m_idProp)
		{	//法向偏移量
			CString sThickStr;
			sThickStr=pPoint->des_para.RODNODE.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("输入法向偏移量字符串",&sThickStr,'s');
#endif
			_snprintf(pPoint->des_para.RODNODE.norm_offset,49,"%s",sThickStr);
			if(pParaPointDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		}
		else if(pItem->m_idProp==CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset")||
			pItem->m_idProp==CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]")||
			pItem->m_idProp==CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]")||
			pItem->m_idProp==CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"))

		{	
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Offset Plane Normal";
#else 
			vecDlg.m_sCaption = "平推面法线";
#endif
			vecDlg.m_fVectorX = pPoint->des_para.RODNODE.vPlaneNormal.x;
			vecDlg.m_fVectorY = pPoint->des_para.RODNODE.vPlaneNormal.y;
			vecDlg.m_fVectorZ = pPoint->des_para.RODNODE.vPlaneNormal.z;
			pItem = pPropList->FindItemByPropId(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]"),&pParentItem);
			vecDlg.m_bReadOnlyX = pItem->IsReadOnly();
			pItem = pPropList->FindItemByPropId(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]"),&pParentItem);
			vecDlg.m_bReadOnlyY = pItem->IsReadOnly();
			pItem = pPropList->FindItemByPropId(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CParaDatumPointDlg::GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"),CString(sText));
			}
		}
#endif
	}
	else if(pPoint->datum_pos_style==3)
	{	//角钢心线交点
		if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			pParaPointDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//基准杆件1
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			pParaPointDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//基准杆件2
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.norm_offset")==pItem->m_idProp)
		{	//法向偏移量
			CString sThickStr;
			sThickStr=pPoint->des_para.AXIS_INTERS.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("输入法向偏移量字符串",&sThickStr,'s');
#endif
			_snprintf(pPoint->des_para.AXIS_INTERS.norm_offset,49,"%s",sThickStr);
			if(pParaPointDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		}
		else if(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector")==pItem->m_idProp||
			CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]")==pItem->m_idProp||
			CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]")==pItem->m_idProp||
			CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Offset Plane Normal";
#else 
			vecDlg.m_sCaption = "平推面法线";
#endif
			vecDlg.m_fVectorX = pPoint->des_para.AXIS_INTERS.near_vector[0];
			vecDlg.m_fVectorY = pPoint->des_para.AXIS_INTERS.near_vector[1];
			vecDlg.m_fVectorZ = pPoint->des_para.AXIS_INTERS.near_vector[2];
			pItem = pPropList->FindItemByPropId(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]"),&pParentItem);
			vecDlg.m_bReadOnlyX = pItem->IsReadOnly();
			pItem = pPropList->FindItemByPropId(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]"),&pParentItem);
			vecDlg.m_bReadOnlyY = pItem->IsReadOnly();
			pItem = pPropList->FindItemByPropId(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[0]"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[1]"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CParaDatumPointDlg::GetPropID("des_para.AXIS_INTERS.near_vector[2]"),CString(sText));
			}
		}
#endif
	}
	else if(pPoint->datum_pos_style==7)
	{	//沿角钢两肢方向偏移的包铁楞点
		if(CParaDatumPointDlg::GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)
			pParaPointDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//基准角钢
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(CParaDatumPointDlg::GetPropID("des_para.RODEND.norm_offset")==pItem->m_idProp)
		{	//法向偏移量

			CString sThickStr;
			sThickStr=pPoint->des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input the offset string along normal",&sThickStr,'s');
#else 
			Input("输入法向偏移量字符串",&sThickStr,'s');
#endif
			_snprintf(pPoint->des_para.RODEND.norm_offset,49,"%s",sThickStr);
			if(pParaPointDlg->GetPropValueStr(pItem->m_idProp,sText)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
	}
	else if(pPoint->datum_pos_style==8)
	{	//与节点同位
		if(CParaDatumPointDlg::GetPropID("des_para.hNode")==pItem->m_idProp)
			pParaPointDlg->SelectObject(CLS_NODE,pItem->m_idProp);	//基准节点
	}
	else if(pPoint->datum_pos_style==9)
	{	//指定构件上的相对位置
		if(CParaDatumPointDlg::GetPropID("des_para.hPart")==pItem->m_idProp)
			pParaPointDlg->SelectObject(CLS_PART,pItem->m_idProp);	//基准构件
		else if(CParaDatumPointDlg::GetPropID("datum_pos")==pItem->m_idProp)
		{
			CReportPointDlg pointDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			pointDlg.m_sCaption = "Relative Coordinate";
#else 
			pointDlg.m_sCaption = "相对坐标";
#endif
			pointDlg.m_fPointX = pPoint->datum_pos.x;
			pointDlg.m_fPointY = pPoint->datum_pos.y;
			pointDlg.m_fPointZ = pPoint->datum_pos.z;
			if(pointDlg.DoModal()==IDOK)
			{
				CXhChar100 sText("%f",pointDlg.m_fPointX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CParaDatumPointDlg::GetPropID("datum_pos.x"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CParaDatumPointDlg::GetPropID("datum_pos.y"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CParaDatumPointDlg::GetPropID("datum_pos.z"),CString(sText));
				pPoint->datum_pos.Set(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ);
			}
		}
	}
	return TRUE;
}
// CParaDatumPointDlg 消息处理程序
BOOL CParaDatumPointDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//
	InitPropHashtable();
	m_propList.m_hPromptWnd = GetDlgItem(IDC_S_DATUM_LINE_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.5);
	m_propList.SetModifyValueFunc(ModifyParaPointProperty);
	m_propList.SetButtonClickFunc(ButtonClickParaPointLine);
	((CEdit*)GetDlgItem(IDC_E_X))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_Y))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_Z))->SetReadOnly(TRUE);
	if(m_bInernalStart)	
		FinishSelectObjOper();	//完成选择对象的后续操作
	//显示基准线信息
	DisplayParaPointProperty();
	if(m_bInernalStart)	
	{	//恢复选中项
		m_propList.Redraw();
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();
		m_propList.SetCurSel(pItem->m_iIndex);	//设置选中项	
	}
	UpdateData(FALSE);
	return TRUE;
}
void CParaDatumPointDlg::OnOK()
{
	UpdateData();
	CLDSPoint* pDatumPoint=NULL;
	if(m_xPoint.handle<0X20)
	{		//新增基准点
		pDatumPoint=console.AppendPoint();
		m_xPoint.handle=pDatumPoint->handle;
	}
	else	//更新数据
		pDatumPoint=Ta.Point.FromHandle(m_xPoint.handle);
	*pDatumPoint=m_xPoint;
	pDatumPoint->UpdatePos(console.GetActiveModel());
	CDialog::OnOK();
}
const DWORD HASHTABLESIZE1 = 500;
CHashStrTable<long> CParaDatumPointDlg::propHashtable;
void CParaDatumPointDlg::InitPropHashtable()
{
	int id=1;
	propHashtable.CreateHashTable(HASHTABLESIZE1);
	//基准顶点信息
	propHashtable.SetValueAt("basic",id++);
	propHashtable.SetValueAt("hDatumPt",id++);
	propHashtable.SetValueAt("datum_pos_desc",id++);	//基准点描述
	propHashtable.SetValueAt("datum_pos_style",id++);
	propHashtable.SetValueAt("des_para",id++);
	propHashtable.SetValueAt("pos.x",id++);
	propHashtable.SetValueAt("pos.y",id++);
	propHashtable.SetValueAt("pos.z",id++);
	//与节点同位
	propHashtable.SetValueAt("des_para.hNode",id++);
	//指定构件上的相对坐标
	propHashtable.SetValueAt("des_para.hPart",id++);
	propHashtable.SetValueAt("datum_pos",id++);
	propHashtable.SetValueAt("datum_pos.x",id++);
	propHashtable.SetValueAt("datum_pos.y",id++);
	propHashtable.SetValueAt("datum_pos.z",id++);
	//心线交点
	propHashtable.SetValueAt("des_para.AXIS_INTERS.hDatum1",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.hDatum2",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.wing_offset_style1",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.wing_offset_style2",id++);//0.g;1.g1;2.g2;3.g3;4.自定义
	propHashtable.SetValueAt("des_para.AXIS_INTERS.wing_offset_dist1",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.wing_offset_dist2",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.near_vector",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.near_vector[0]",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.near_vector[1]",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.near_vector[2]",id++);
	propHashtable.SetValueAt("des_para.AXIS_INTERS.norm_offset",id++);	//法向偏移量;
	//角钢楞点
	propHashtable.SetValueAt("des_para.RODEND.hRod",id++);				//角钢句柄
	propHashtable.SetValueAt("des_para.RODEND.direction",id++);			//0.基准角钢方向:始->终;1.终->始
	propHashtable.SetValueAt("des_para.RODEND.offset_wing",id++);		//0.X肢;1.Y肢
	propHashtable.SetValueAt("des_para.RODEND.wing_offset_style",id++);	//0.g;1.g1;2.g2;3.g3;4.自定义
	propHashtable.SetValueAt("des_para.RODEND.wing_offset_dist",id++);
	propHashtable.SetValueAt("des_para.RODEND.len_offset_dist",id++);	//肢向偏移值;纵向偏移值;
	propHashtable.SetValueAt("des_para.RODEND.bIncOddEffect",id++);		//纵向偏移值计入正负头影响
	propHashtable.SetValueAt("des_para.RODEND.norm_offset",id++);		//法向偏移量
	//杆件上节点为基准
	propHashtable.SetValueAt("des_para.LINEPARTNODE.hLinePart",id++);	//角钢句柄
	propHashtable.SetValueAt("des_para.LINEPARTNODE.hNode",id++);		//基准节点句柄
	propHashtable.SetValueAt("des_para.LINEPARTNODE.bFaceOffset",id++);
	propHashtable.SetValueAt("des_para.LINEPARTNODE.direction",id++);	//0.基准角钢方向:始->终;1.终->始
	propHashtable.SetValueAt("des_para.LINEPARTNODE.offset_wing",id++);	//0.X肢;1.Y肢
	propHashtable.SetValueAt("des_para.LINEPARTNODE.wing_offset_style",id++);//0.g;1.g1;2.g2;3.g3;4.自定义
	propHashtable.SetValueAt("des_para.LINEPARTNODE.wing_offset_dist",id++);
	propHashtable.SetValueAt("des_para.LINEPARTNODE.len_offset_dist",id++);	//肢向偏移值;纵向偏移值;
	propHashtable.SetValueAt("des_para.LINEPARTNODE.norm_offset",id++);		//法向偏移量;
	propHashtable.SetValueAt("des_para.LINEPARTNODE.face_norm_offset",id++);
	propHashtable.SetValueAt("des_para.LINEPARTNODE.face_norm_offset[0]",id++);
	propHashtable.SetValueAt("des_para.LINEPARTNODE.face_norm_offset[1]",id++);
	propHashtable.SetValueAt("des_para.LINEPARTNODE.face_norm_offset[2]",id++);
	//基准线上轴向坐标不变点
	propHashtable.SetValueAt("des_para.DATUMLINE.hDatumLine",id++);
	propHashtable.SetValueAt("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2",id++);
	//基准面上轴向坐标不变点
	propHashtable.SetValueAt("des_para.DATUMPLANE.hDatumPlane",id++);
	propHashtable.SetValueAt("des_para.DATUMPLANE.cMirTransType",id++);
	propHashtable.SetValueAt("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2",id++);
}
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
	sPointHandle+="<新建...>";
#endif
	return sPointHandle;
}
void CParaDatumPointDlg::DisplayParaPointProperty()
{
	m_propList.CleanTree();
	CPropTreeItem *pGroupItem,*pPropItem,*pRoot=m_propList.GetRootItem();
	CXhChar100 sText;
	CItemInfo* lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Basic Information";
#else 
	lpInfo->m_strPropName = "基本信息";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CParaDatumPointDlg::GetPropID("basic");
	pGroupItem->m_bHideChildren=FALSE;
	//
	lpInfo=new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbItems=GetPointHandleStr();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Datum Node Handle";
	lpInfo->m_strPropHelp="Datum node's handle";
#else 
	lpInfo->m_strPropName="句柄";
	lpInfo->m_strPropHelp="基准点的句柄";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp=CParaDatumPointDlg::GetPropID("hDatumPt");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	pPropItem->SetReadOnly(FALSE);
	//描述
	lpInfo=new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="Description";
	lpInfo->m_strPropHelp="Datum point's description";
#else 
	lpInfo->m_strPropName="描述";
	lpInfo->m_strPropHelp="基准点的描述";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp=CParaDatumPointDlg::GetPropID("datum_pos_desc");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//基准点定位方式
	lpInfo=new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="0.Manual Input|1.Angle Ridge Vertex|2.Rod Node Position|3.Bolt line Intersection Point|4.Datum Line Intersection|5.Coord. Specified On Datum Line|6.Coord. Specified  On Datum Plane|7.Angle Ridge Offset|8.Same Position With Node|9.Relative Coordinate On Datum Part";
	lpInfo->m_strPropName="Locate Type";
	lpInfo->m_strPropHelp="Position type of datum point";
#else 
	lpInfo->m_cmbItems="0.手动输入|1.角钢楞点定位点|2.杆件节点定位点|3.角钢心线交点|4.基准线空间交点|5.基准线上指定坐标分量点|6.基准面上指定轴向点|7.沿肢法向偏移的包铁楞点|8.与节点同位|9.基准构件上的相对坐标";
	lpInfo->m_strPropName="定位方式";
	lpInfo->m_strPropHelp="基准点的定位方式";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp=CParaDatumPointDlg::GetPropID("datum_pos_style");
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Parameter Information";
#else 
	lpInfo->m_strPropName = "参数信息";
#endif
	pGroupItem = m_propList.InsertItem(pRoot,lpInfo,-1);
	pGroupItem->m_idProp = CParaDatumPointDlg::GetPropID("des_para");
	pGroupItem->m_bHideChildren=FALSE;
	UpdateDatumPointParaProperty(&m_propList,pGroupItem);
	m_propList.Redraw();
}
int CParaDatumPointDlg::GetPropValueStr(long id,char *valueStr)
{
	CXhChar100 sText;
	if(GetPropID("hDatumPt")==id)
	{
		if(m_xPoint.handle>0X20)
			sText.Printf("0X%X",m_xPoint.handle);
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("<New...>");
#else 
			sText.Copy("<新建...>");
#endif
	}
	else if(GetPropID("datum_pos_desc")==id)
		sText.Printf("%s",m_xPoint.description);
	else if(GetPropID("datum_pos_style")==id)
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_xPoint.datum_pos_style==0)
			sText.Copy("0.Manual Input");
		else if(m_xPoint.datum_pos_style==1)
			sText.Copy("1.Angle Ridge Vertex");
		else if(m_xPoint.datum_pos_style==2)
			sText.Copy("2.Rod Node Position");
		else if(m_xPoint.datum_pos_style==3)
			sText.Copy("3.Bolt line Intersection Point");
		else if(m_xPoint.datum_pos_style==4)
			sText.Copy("4.Datum Line Intersection");
		else if(m_xPoint.datum_pos_style==5)
			sText.Copy("5.Coord. Specified On Datum Line");
		else if(m_xPoint.datum_pos_style==6)
			sText.Copy("6.Coord. Specified  On Datum Plane");
		else if(m_xPoint.datum_pos_style==7)
			sText.Copy("7.Angle Ridge Offset");
		else if(m_xPoint.datum_pos_style==8)
			sText.Copy("8.Same Position With Node");
		else if(m_xPoint.datum_pos_style==9)
			sText.Copy("9.Relative Coordinate On Datum Part");
#else 
		if(m_xPoint.datum_pos_style==0)
			sText.Copy("0.手动输入");
		else if(m_xPoint.datum_pos_style==1)
			sText.Copy("1.角钢楞点定位点");
		else if(m_xPoint.datum_pos_style==2)
			sText.Copy("2.杆件节点定位点");
		else if(m_xPoint.datum_pos_style==3)
			sText.Copy("3.角钢心线交点");
		else if(m_xPoint.datum_pos_style==4)
			sText.Copy("4.基准线空间交点");
		else if(m_xPoint.datum_pos_style==5)
			sText.Copy("5.基准线上指定坐标分量点");
		else if(m_xPoint.datum_pos_style==6)
			sText.Copy("6.基准面上指定轴向点");
		else if(m_xPoint.datum_pos_style==7)
			sText.Copy("7.沿肢法向偏移的包铁楞点");
		else if(m_xPoint.datum_pos_style==8)
			sText.Copy("8.与节点同位");
		else if(m_xPoint.datum_pos_style==9)
			sText.Copy("9.基准构件上的相对坐标");
#endif
	}
	if(m_xPoint.datum_pos_style==0)	//直接指定
	{
		if(GetPropID("pos.x")==id)
		{
			sText.Printf("%f",m_xPoint.Position().x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("pos.y")==id)
		{
			sText.Printf("%f",m_xPoint.Position().y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("pos.z")==id)
		{
			sText.Printf("%f",m_xPoint.Position().z);
			SimplifiedNumString(sText);
		}
	}
	else if(m_xPoint.datum_pos_style==1)	//角钢楞点定位
	{
		if(GetPropID("des_para.RODEND.hRod")==id)	//角钢句柄
			sText.Printf("0X%X",m_xPoint.des_para.RODEND.hRod);
		else if(GetPropID("des_para.RODEND.direction")==id)	
		{	//0.基准角钢方向:始->终;1.终->始
			if(m_xPoint.des_para.RODEND.direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("0.Start->End");
			else 
				sText.Copy("1.End->Start");
#else 
				sText.Copy("0.始->终");
			else 
				sText.Copy("1.终->始");
#endif
		}
		else if(GetPropID("des_para.RODEND.offset_wing")==id)//0.X肢;1.Y肢
		{	//0.X肢;1.Y肢
			if(m_xPoint.des_para.RODEND.offset_wing==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("0.X Leg");
			else 
				sText.Copy("1.Y Leg");
#else 
				sText.Copy("0.X肢");
			else 
				sText.Copy("1.Y肢");
#endif
		}
		else if(GetPropID("des_para.RODEND.wing_offset_style")==id)	
		{	//0.g;1.g1;2.g2;3.g3;4.自定义
			if(m_xPoint.des_para.RODEND.wing_offset_style==0)
				sText.Copy("g");
			else if(m_xPoint.des_para.RODEND.wing_offset_style==1)
				sText.Copy("g1");
			else if(m_xPoint.des_para.RODEND.wing_offset_style==2)
				sText.Copy("g2");
			else if(m_xPoint.des_para.RODEND.wing_offset_style==3)
				sText.Copy("g3");
			else 
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("User Defined");
#else 
				sText.Copy("自定义");
#endif
		}
		else if(GetPropID("des_para.RODEND.wing_offset_dist")==id)
		{
			sText.Printf("%f",m_xPoint.des_para.RODEND.wing_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.RODEND.len_offset_dist")==id)	
		{	//纵向偏移值;
			sText.Printf("%f",m_xPoint.des_para.RODEND.len_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.RODEND.bIncOddEffect")==id)		
		{	//纵向偏移值计入正负头影响
			if(m_xPoint.des_para.RODEND.bIncOddEffect)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else 
				sText.Copy("No");
#else 
				sText.Copy("是");
			else 
				sText.Copy("否");
#endif
		}
		else if(GetPropID("des_para.RODEND.norm_offset")==id)//法向偏移量
		{
			CDesignThickPara thick(m_xPoint.des_para.RODEND.norm_offset);
			sText.Printf("%d",thick.Thick(console.GetActiveModel()));
		}
	}
	else if(m_xPoint.datum_pos_style==2)	//杆件节点定位
	{
		if(GetPropID("des_para.LINEPARTNODE.hLinePart")==id)	//角钢句柄
			sText.Printf("0X%X",m_xPoint.des_para.RODNODE.hRod);
		else if(GetPropID("des_para.LINEPARTNODE.hNode")==id)	//基准节点句柄
			sText.Printf("0X%X",m_xPoint.des_para.RODNODE.hNode);
		else if(GetPropID("des_para.LINEPARTNODE.bFaceOffset")==id)
		{
			if(m_xPoint.des_para.RODNODE.bFaceOffset)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else 
				sText.Copy("No");
#else 
				sText.Copy("是");
			else 
				sText.Copy("否");
#endif
		}
		else if(GetPropID("des_para.LINEPARTNODE.direction")==id)	
		{	//0.基准角钢方向:始->终;1.终->始
			if(m_xPoint.des_para.RODNODE.direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("0.Start->End");
			else 
				sText.Copy("1.End->Start");
#else 
				sText.Copy("0.始->终");
			else 
				sText.Copy("1.终->始");
#endif
		}
		else if(GetPropID("des_para.LINEPARTNODE.offset_wing")==id)	
		{	//0.X肢;1.Y肢
			if(m_xPoint.des_para.RODNODE.offset_wing==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("0.X Leg");
			else 
				sText.Copy("1.Y Leg");
#else 
				sText.Copy("0.X肢");
			else 
				sText.Copy("1.Y肢");
#endif
		}
		else if(GetPropID("des_para.LINEPARTNODE.wing_offset_style")==id)
		{	//0.g;1.g1;2.g2;3.g3;4.自定义
			if(m_xPoint.des_para.RODNODE.wing_offset_style==0)
				sText.Copy("g");
			else if(m_xPoint.des_para.RODNODE.wing_offset_style==1)
				sText.Copy("g1");
			else if(m_xPoint.des_para.RODNODE.wing_offset_style==2)
				sText.Copy("g2");
			else if(m_xPoint.des_para.RODNODE.wing_offset_style==3)
				sText.Copy("g3");
			else
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("User Defined");
#else 
				sText.Copy("自定义");
#endif
		}	
		else if(GetPropID("des_para.LINEPARTNODE.wing_offset_dist")==id)
		{
			sText.Printf("%f",m_xPoint.des_para.RODNODE.wing_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.LINEPARTNODE.len_offset_dist")==id)	
		{	//纵向偏移值
			sText.Printf("%f",m_xPoint.des_para.RODNODE.len_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.LINEPARTNODE.norm_offset")==id)	//法向偏移量
		{
			CDesignThickPara thick(m_xPoint.des_para.RODNODE.norm_offset);
			sText.Printf("%d",thick.Thick(console.GetActiveModel()));
		}
		else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset")==id)
			sText.Printf("%s",CStdVector::GetVecTypeString(m_xPoint.des_para.RODNODE.vPlaneNormal));
		else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]")==id)
		{
			sText.Printf("%f",m_xPoint.des_para.RODNODE.vPlaneNormal.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]")==id)
		{
			sText.Printf("%f",m_xPoint.des_para.RODNODE.vPlaneNormal.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]")==id)
		{
			sText.Printf("%f",m_xPoint.des_para.RODNODE.vPlaneNormal.z);
			SimplifiedNumString(sText);
		}
	}
	else if(m_xPoint.datum_pos_style==3)	//杆件心线交点
	{
		if(GetPropID("des_para.AXIS_INTERS.hDatum1")==id)
			sText.Printf("0X%X",m_xPoint.des_para.AXIS_INTERS.hDatum1);
		else if(GetPropID("des_para.AXIS_INTERS.hDatum2")==id)
			sText.Printf("0X%X",m_xPoint.des_para.AXIS_INTERS.hDatum2);
		else if(GetPropID("des_para.AXIS_INTERS.wing_offset_style1")==id)
		{	//0.g;1.g1;2.g2;3.g3;4.自定义
			if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1==0)
				sText.Copy("g");
			else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1==1)
				sText.Copy("g1");
			else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1==2)
				sText.Copy("g2");
			else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1==3)
				sText.Copy("g3");
			else
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("User Defined");
#else 
				sText.Copy("自定义");
#endif
		}
		else if(GetPropID("des_para.AXIS_INTERS.wing_offset_style2")==id)
		{	//0.g;1.g1;2.g2;3.g3;4.自定义
			if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2==0)
				sText.Copy("g");
			else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2==1)
				sText.Copy("g1");
			else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2==2)
				sText.Copy("g2");
			else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2==3)
				sText.Copy("g3");
			else
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("User Defined");
#else 
				sText.Copy("自定义");
#endif
		}
		else if(GetPropID("des_para.AXIS_INTERS.wing_offset_dist1")==id)
		{
			sText.Printf("%d",m_xPoint.des_para.AXIS_INTERS.wing_offset_dist1);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.AXIS_INTERS.wing_offset_dist2")==id)
		{
			sText.Printf("%d",m_xPoint.des_para.AXIS_INTERS.wing_offset_dist2);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.AXIS_INTERS.near_vector")==id)
			sText.Printf("%s",CStdVector::GetVecTypeString(f3dPoint(m_xPoint.des_para.AXIS_INTERS.near_vector)));
		else if(GetPropID("des_para.AXIS_INTERS.near_vector[0]")==id)
		{
			sText.Printf("%f",m_xPoint.des_para.AXIS_INTERS.near_vector[0]);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.AXIS_INTERS.near_vector[1]")==id)
		{
			sText.Printf("%f",m_xPoint.des_para.AXIS_INTERS.near_vector[1]);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.AXIS_INTERS.near_vector[2]")==id)
		{
			sText.Printf("%f",m_xPoint.des_para.AXIS_INTERS.near_vector[2]);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.AXIS_INTERS.norm_offset")==id)	//法向偏移量;
		{
			CDesignThickPara thick(m_xPoint.des_para.AXIS_INTERS.norm_offset);
			sText.Printf("%d",thick.Thick(console.GetActiveModel()));
		}
	}
	else if(m_xPoint.datum_pos_style==4)	//基准线空间交点
	{
		if(GetPropID("des_para.AXIS_INTERS.hDatum1")==id)
			sText.Printf("0X%X",m_xPoint.des_para.AXIS_INTERS.hDatum1);
		else if(GetPropID("des_para.AXIS_INTERS.hDatum2")==id)
			sText.Printf("0X%X",m_xPoint.des_para.AXIS_INTERS.hDatum2);
	}
	else if(m_xPoint.datum_pos_style==5)	//基准线上指定坐标分量点
	{
		if(GetPropID("des_para.DATUMLINE.hDatumLine")==id)
			sText.Printf("0X%X",m_xPoint.des_para.DATUMLINE.hDatumLine);
		else if(GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
				sText.Printf("0.X Axis");
			else if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
				sText.Printf("1.Y Axis");
			else 
				sText.Printf("2.Z Axis");
#else 
			if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
				sText.Printf("0.X轴");
			else if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
				sText.Printf("1.Y轴");
			else 
				sText.Printf("2.Z轴");
#endif
		}
	}
	else if(m_xPoint.datum_pos_style==6)	//基准面上指定轴向点
	{
		if(GetPropID("des_para.DATUMPLANE.hDatumPlane")==id)
			sText.Printf("0X%X",m_xPoint.des_para.DATUMPLANE.hDatumPlane);
		else if(GetPropID("des_para.DATUMPLANE.cMirTransType")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_xPoint.des_para.DATUMPLANE.cMirTransType=='X')
				sText.Printf("X Mirror");
			else if(m_xPoint.des_para.DATUMPLANE.cMirTransType=='Y')
				sText.Printf("Y Mirror");
			else if(m_xPoint.des_para.DATUMPLANE.cMirTransType=='Z')
				sText.Printf("Z Mirror");
			else 
				sText.Printf("None");
#else 
			if(m_xPoint.des_para.DATUMPLANE.cMirTransType=='X')
				sText.Printf("X轴对称");
			else if(m_xPoint.des_para.DATUMPLANE.cMirTransType=='Y')
				sText.Printf("Y轴对称");
			else if(m_xPoint.des_para.DATUMPLANE.cMirTransType=='Z')
				sText.Printf("Z轴对称");
			else 
				sText.Printf("无对称参照");
#endif
		}
		else if(GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==id)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
				sText.Printf("0.X Axis");
			else if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
				sText.Printf("1.Y Axis");
			else 
				sText.Printf("2.Z Axis");
#else 
			if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
				sText.Printf("0.X轴");
			else if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
				sText.Printf("1.Y轴");
			else 
				sText.Printf("2.Z轴");
#endif
		}
	}
	else if(m_xPoint.datum_pos_style==7)	//沿角钢两肢法向偏移的包铁楞点
	{
		if(GetPropID("des_para.RODEND.hRod")==id)	//角钢句柄
			sText.Printf("0X%X",m_xPoint.des_para.RODEND.hRod);
		else if(GetPropID("des_para.RODEND.direction")==id)	
		{	//0.基准角钢方向:始->终;1.终->始
			if(m_xPoint.des_para.RODEND.direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("0.Start->End");
			else 
				sText.Copy("1.End->Start");
#else 
				sText.Copy("0.始->终");
			else 
				sText.Copy("1.终->始");
#endif
		}
		else if(GetPropID("des_para.RODEND.len_offset_dist")==id)	
		{	//纵向偏移值;
			sText.Printf("%f",m_xPoint.des_para.RODEND.len_offset_dist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("des_para.RODEND.norm_offset")==id)//法向偏移量
		{
			CDesignThickPara thick(m_xPoint.des_para.RODEND.norm_offset);
			sText.Printf("%d",thick.Thick(console.GetActiveModel()));
		}
	}
	else if(m_xPoint.datum_pos_style==8)	//与节点同位
	{
		if(GetPropID("des_para.hNode")==id)
			sText.Printf("0X%X",m_xPoint.des_para.hNode);
	}
	else if(m_xPoint.datum_pos_style==9)	//指定构件上的相对坐标
	{
		if(GetPropID("des_para.hPart")==id)
			sText.Printf("0X%X",m_xPoint.des_para.hPart);
		else if(GetPropID("datum_pos.x")==id)
		{
			sText.Printf("%f",m_xPoint.datum_pos.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("datum_pos.y")==id)
		{
			sText.Printf("%f",m_xPoint.datum_pos.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("datum_pos.z")==id)
		{
			sText.Printf("%f",m_xPoint.datum_pos.z);
			SimplifiedNumString(sText);
		}
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
long CParaDatumPointDlg::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}
//完成选择对象的后续操作
void CParaDatumPointDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(GetEventPropId()==GetPropID("des_para.RODEND.hRod"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
			m_xPoint.des_para.RODEND.hRod=pLinePart->handle;
	}
	else 
#endif
		if(GetEventPropId()==GetPropID("des_para.LINEPARTNODE.hLinePart"))
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
			if(pLinePart)
				m_xPoint.des_para.RODNODE.hRod=pLinePart->handle;
		}
		else if(GetEventPropId()==GetPropID("des_para.LINEPARTNODE.hNode"))
		{
			CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
			if(pNode)
				m_xPoint.des_para.RODNODE.hNode=pNode->handle;
		}
		else if(GetEventPropId()==GetPropID("des_para.AXIS_INTERS.hDatum1"))
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
			if(pLinePart)
				m_xPoint.des_para.AXIS_INTERS.hDatum1=pLinePart->handle;
		}
		else if(GetEventPropId()==GetPropID("des_para.AXIS_INTERS.hDatum2"))
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
			if(pLinePart)
				m_xPoint.des_para.AXIS_INTERS.hDatum2=pLinePart->handle;
		}
		else if(GetEventPropId()==GetPropID("des_para.hNode"))
		{
			CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
			if(pNode)
				m_xPoint.des_para.hNode=pNode->handle;
		}
		else if(GetEventPropId()==GetPropID("des_para.hPart"))
		{
			CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
			if(pPart)
				m_xPoint.des_para.hPart=pPart->handle;
		}
}
void CParaDatumPointDlg::OnEnChangeEdtX()
{
	UpdateData();
	if(m_xPoint.datum_pos_style==0||m_xPoint.datum_pos_style==5||m_xPoint.datum_pos_style==6)
	{	
		m_xPoint.SetPositionX(m_fPointX);
		CPropTreeItem* pFineItem;
		pFineItem=m_propList.FindItemByPropId(CParaDatumPointDlg::GetPropID("des_para"),NULL);
		UpdateDatumPointParaProperty(&m_propList,pFineItem);
	}
}
void CParaDatumPointDlg::OnEnChangeEdtY()
{
	UpdateData();
	if(m_xPoint.datum_pos_style==0||m_xPoint.datum_pos_style==5||m_xPoint.datum_pos_style==6)
	{	
		m_xPoint.SetPositionY(m_fPointY);
		CPropTreeItem* pFineItem;
		pFineItem=m_propList.FindItemByPropId(CParaDatumPointDlg::GetPropID("des_para"),NULL);
		UpdateDatumPointParaProperty(&m_propList,pFineItem);
	}
}
void CParaDatumPointDlg::OnEnChangeEdtZ()
{
	UpdateData();
	if(m_xPoint.datum_pos_style==0||m_xPoint.datum_pos_style==5||m_xPoint.datum_pos_style==6)
	{	
		m_xPoint.SetPositionZ(m_fPointZ);
		CPropTreeItem* pFineItem;
		pFineItem=m_propList.FindItemByPropId(CParaDatumPointDlg::GetPropID("des_para"),NULL);
		UpdateDatumPointParaProperty(&m_propList,pFineItem);
	}
}