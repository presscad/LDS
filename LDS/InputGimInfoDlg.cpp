// InstanceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "InputGimInfoDlg.h"
#include "PropertyListOper.h"

#ifdef __PART_DESIGN_INC_
//////////////////////////////////////////////////////////////////////////
//
BOOL ModifyProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CTowerGimInfo* pGim=(CTowerGimInfo*)pPropList->m_pObj;
	if(pGim==NULL)
		return FALSE;
	CPropertyListOper<CTowerGimInfo> oper(pPropList,pGim);
	BOOL bUpdateWireInfo=FALSE;
	if(CTowerGimInfo::GetPropID("Designer")==pItem->m_idProp)
		strcpy(pGim->m_xCurrGimFileHeader.m_sDesigner,valueStr);
	else if(CTowerGimInfo::GetPropID("Unit")==pItem->m_idProp)
		strcpy(pGim->m_xCurrGimFileHeader.m_sUnit,valueStr);
	else if(CTowerGimInfo::GetPropID("MajorVer")==pItem->m_idProp)
		strcpy(pGim->m_xCurrGimFileHeader.m_sMajorVersion,valueStr);
	else if(CTowerGimInfo::GetPropID("MinorVer")==pItem->m_idProp)
		strcpy(pGim->m_xCurrGimFileHeader.m_sMinorVersion,valueStr);
	else if(CTowerGimInfo::GetPropID("VoltGrade")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sVoltGrade,valueStr);
	else if(CTowerGimInfo::GetPropID("Type")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sType,valueStr);
	else if(CTowerGimInfo::GetPropID("Texture")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sTexture,valueStr);
	else if(CTowerGimInfo::GetPropID("FixedType")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sFixedType,valueStr);
	else if(CTowerGimInfo::GetPropID("TaType")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sTaType,valueStr);
	else if(CTowerGimInfo::GetPropID("Circuit")==pItem->m_idProp)
		pGim->m_xTowerGimPropFile.m_nCircuit=atoi(valueStr);
	else if(CTowerGimInfo::GetPropID("CWireSpec")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sCWireSpec,valueStr);
	else if(CTowerGimInfo::GetPropID("EWireSpec")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sEWireSpec,valueStr);
	else if(CTowerGimInfo::GetPropID("WindSpeed")==pItem->m_idProp)
		pGim->m_xTowerGimPropFile.m_fWindSpeed=atof(valueStr);
	else if(CTowerGimInfo::GetPropID("NiceThick")==pItem->m_idProp)
		pGim->m_xTowerGimPropFile.m_fNiceThick=atof(valueStr);
	else if(CTowerGimInfo::GetPropID("WindSpan")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sWindSpan,valueStr);
	else if(CTowerGimInfo::GetPropID("WeightSpan")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sWeightSpan,valueStr);
	else if(CTowerGimInfo::GetPropID("FrontRulingSpan")==pItem->m_idProp)
		pGim->m_xTowerGimPropFile.m_fFrontRulingSpan=atof(valueStr);
	else if(CTowerGimInfo::GetPropID("BackRulingSpan")==pItem->m_idProp)
		pGim->m_xTowerGimPropFile.m_fBackRulingSpan=atof(valueStr);
	else if(CTowerGimInfo::GetPropID("MaxSpan")==pItem->m_idProp)
		pGim->m_xTowerGimPropFile.m_fMaxSpan=atof(valueStr);
	else if(CTowerGimInfo::GetPropID("AngleRange")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sAngleRange,valueStr);
	else if(CTowerGimInfo::GetPropID("DesignKV")==pItem->m_idProp)
		pGim->m_xTowerGimPropFile.m_fDesignKV=atof(valueStr);
	else if(CTowerGimInfo::GetPropID("RatedHeight")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sRatedHeight,valueStr);
	else if(CTowerGimInfo::GetPropID("HeightRange")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sHeightRange,valueStr);
	else if(CTowerGimInfo::GetPropID("TowerWeight")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sTowerWeight,valueStr);
	else if(CTowerGimInfo::GetPropID("FrequencyRockAngle")==pItem->m_idProp)
		pGim->m_xTowerGimPropFile.m_fFrequencyRockAngle=atof(valueStr);
	else if(CTowerGimInfo::GetPropID("LightningRockAngle")==pItem->m_idProp)
		pGim->m_xTowerGimPropFile.m_fLightningRockAngle=atof(valueStr);
	else if(CTowerGimInfo::GetPropID("SwitchingRockAngle")==pItem->m_idProp)
		pGim->m_xTowerGimPropFile.m_fSwitchingRockAngle=atof(valueStr);
	else if(CTowerGimInfo::GetPropID("WorkingRockAngle")==pItem->m_idProp)
		pGim->m_xTowerGimPropFile.m_fWorkingRockAngle=atof(valueStr);
	else if(CTowerGimInfo::GetPropID("ManuFacturer")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sManuFacturer,valueStr);
	else if(CTowerGimInfo::GetPropID("MaterialCode")==pItem->m_idProp)
		strcpy(pGim->m_xTowerGimPropFile.m_sMaterialCode,valueStr);
	else
		return FALSE;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// CInstanceDlg 对话框

IMPLEMENT_DYNAMIC(CInputGimInfoDlg, CDialogEx)

CInputGimInfoDlg::CInputGimInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInputGimInfoDlg::IDD, pParent)
{
	m_pGimInfo =NULL;
}

CInputGimInfoDlg::~CInputGimInfoDlg()
{
}

void CInputGimInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_xPropList);
}


BEGIN_MESSAGE_MAP(CInputGimInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BN_IMPORT_FILE,OnImportFile)
	ON_BN_CLICKED(IDC_BN_EXPORT_FILE,OnExportFile)
END_MESSAGE_MAP()


// CInstanceDlg 消息处理程序
BOOL CInputGimInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//
	CTowerGimInfo::InitPropHashtable();
	CWnd *pWnd=GetDlgItem(IDC_E_PROP_HELP_STR);
	m_xPropList.m_hPromptWnd=pWnd->GetSafeHwnd();
	DisplayGimProperty();
	return TRUE;
}
void CInputGimInfoDlg::SetGimInfo(CTowerGimInfo* pGimInfo)
{
	m_pGimInfo = pGimInfo;
	if (m_pGimInfo == NULL)
		m_pGimInfo = &gxTowerGimInfo;
	m_pGimInfo->m_ciCodingType = g_sysPara.m_iModFileCodeType;
}
void CInputGimInfoDlg::DisplayGimProperty()
{
	m_xPropList.CleanCallBackFunc();
	m_xPropList.CleanTree();
	m_xPropList.m_nObjClassTypeId = 0;
	m_xPropList.m_pObj= m_pGimInfo;
	m_xPropList.SetModifyValueFunc(ModifyProperty);
	//
	CPropertyListOper<CTowerGimInfo> oper(&m_xPropList, m_pGimInfo);
	CPropTreeItem* pRoot=m_xPropList.GetRootItem();	
	CPropTreeItem *pGroupItem=oper.InsertPropItem(pRoot,"BasicInfo");
	oper.InsertEditPropItem(pGroupItem,"Designer");
	oper.InsertEditPropItem(pGroupItem,"Unit");
	oper.InsertEditPropItem(pGroupItem,"MajorVer");
	oper.InsertEditPropItem(pGroupItem,"MinorVer");
	//
	pGroupItem=oper.InsertPropItem(pRoot,"TowerInfo");
	oper.InsertEditPropItem(pGroupItem,"VoltGrade");
	oper.InsertEditPropItem(pGroupItem,"Type");
	oper.InsertCmbListPropItem(pGroupItem,"Texture");
	oper.InsertCmbListPropItem(pGroupItem,"FixedType");
	oper.InsertCmbListPropItem(pGroupItem,"TaType");
	oper.InsertEditPropItem(pGroupItem,"Circuit");
	oper.InsertEditPropItem(pGroupItem,"CWireSpec");
	oper.InsertEditPropItem(pGroupItem,"EWireSpec");
	oper.InsertEditPropItem(pGroupItem,"WindSpeed");
	oper.InsertEditPropItem(pGroupItem,"NiceThick");
	oper.InsertEditPropItem(pGroupItem,"WindSpan");
	oper.InsertEditPropItem(pGroupItem,"WeightSpan");
	oper.InsertEditPropItem(pGroupItem,"FrontRulingSpan");
	oper.InsertEditPropItem(pGroupItem,"BackRulingSpan");
	oper.InsertEditPropItem(pGroupItem,"MaxSpan");
	oper.InsertEditPropItem(pGroupItem,"AngleRange");
	oper.InsertEditPropItem(pGroupItem,"DesignKV");
	oper.InsertEditPropItem(pGroupItem,"RatedHeight");
	oper.InsertEditPropItem(pGroupItem,"HeightRange");
	oper.InsertEditPropItem(pGroupItem,"TowerWeight");
	oper.InsertEditPropItem(pGroupItem,"FrequencyRockAngle");
	oper.InsertEditPropItem(pGroupItem,"LightningRockAngle");
	oper.InsertEditPropItem(pGroupItem,"SwitchingRockAngle");
	oper.InsertEditPropItem(pGroupItem,"WorkingRockAngle");
	oper.InsertEditPropItem(pGroupItem,"ManuFacturer");
	oper.InsertEditPropItem(pGroupItem,"MaterialCode");
	//
	m_xPropList.Redraw();
}
void CInputGimInfoDlg::OnImportFile()
{
	CFileDialog dlg(TRUE,"fam","tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"铁塔公共属性(*.fam)|*.fam|所有文件(*.*)|*.*||");
	if(dlg.DoModal()!=IDOK)
		return;
	CString ext=dlg.GetFileExt();
	m_pGimInfo->ReadFamFile(dlg.GetPathName());
	//
	DisplayGimProperty();
}
void CInputGimInfoDlg::OnExportFile()
{
	CFileDialog dlg(FALSE,"fam","tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"铁塔公共属性(*.fam)|*.fam|所有文件(*.*)|*.*||");
	if(dlg.DoModal()!=IDOK)
		return;
	m_pGimInfo->WriteFamFile(dlg.GetPathName());
}
#endif