// DesignBushTubeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignBushTubeDlg.h"
#include "afxdialogex.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "MainFrm.h"
#include "CmdLineDlg.h"
#include "PropertyListOper.h"

//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_PROP_FUNC(CBushTube);
const DWORD HASHTABLESIZE1 = 500;
void CBushTube::InitPropHashtable()
{
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE1);
	propStatusHashtable.CreateHashTable(HASHTABLESIZE1);
	int id = 1;
	AddPropItem("Basic",PROPLIST_ITEM(id++,"套管信息","套管基本信息"));
	AddPropItem("bush_tube.sPartNo",PROPLIST_ITEM(id++,"编号","套管编号"));
	AddPropItem("bush_tube.cMaterial",PROPLIST_ITEM(id++,"材质","套管材质",CXhChar100(MakeMaterialMarkSetString())));
	AddPropItem("bush_tube.fLength",PROPLIST_ITEM(id++,"长度","套管长度"));
	AddPropItem("bush_tube.fOuterD",PROPLIST_ITEM(id++,"外径","套管外径"));
	AddPropItem("bush_tube.fInterD",PROPLIST_ITEM(id++,"内径","套管内径"));
	AddPropItem("bush_tube.fThick",PROPLIST_ITEM(id++,"厚度","套管厚度"));
	AddPropItem("bush_tube.hResidePlate",PROPLIST_ITEM(id++,"焊接钢板","套管所在钢板"));
	AddPropItem("bush_tube.hRelyBolt",PROPLIST_ITEM(id++,"依赖螺栓","套管依赖螺栓孔"));
	AddPropItem("bush_tube.bRingPlate",PROPLIST_ITEM(id++,"生成环板","是否生成附加环板","是|否"));
	AddPropItem("bush_tube.ring_plate.sPartNo",PROPLIST_ITEM(id++,"环板编号","附加环板编号"));
	AddPropItem("bush_tube.ring_plate.cMaterial",PROPLIST_ITEM(id++,"环板材质","环板材质",CXhChar100(MakeMaterialMarkSetString())));
	AddPropItem("bush_tube.ring_plate.fThick",PROPLIST_ITEM(id++,"环板厚度","附加环板厚度"));
	AddPropItem("bush_tube.ring_plate.fOuterD",PROPLIST_ITEM(id++,"环板外径","附加环板的外环直径"));
	AddPropItem("bush_tube.ring_plate.fInnerD",PROPLIST_ITEM(id++,"环板内径","附加环板的内环直径"));
}
int CBushTube::GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("bush_tube.sPartNo")==id)
		sText.Copy(sPartNo);
	else if(GetPropID("bush_tube.cMaterial")==id)
	{
		STEELMAT *pSteelMat=QuerySteelMatByBriefMark(cMaterial);
		if(pSteelMat)
			sText.Copy(pSteelMat->mark);
	}
	else if(GetPropID("bush_tube.fLength")==id)
	{
		sText.Printf("%f",fLength);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("bush_tube.fOuterD")==id)
	{
		sText.Printf("%f",fOuterD);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("bush_tube.fInterD")==id)
	{
		sText.Printf("%f",fInnerD);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("bush_tube.fThick")==id)
	{
		fThick=(fOuterD*0.5)-(fInnerD*0.5);
		sText.Printf("%f",fThick);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("bush_tube.hResidePlate")==id)
		sText.Printf("0X%X",hResidePlate);
	else if(GetPropID("bush_tube.hRelyBolt")==id)
		sText.Printf("0X%X",hRelyBolt);
	else if(GetPropID("bush_tube.bRingPlate")==id)
	{
		if(bRingPlate)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("bush_tube.ring_plate.sPartNo")==id)
		sText.Copy(ring_plate.sPartNo);
	else if(GetPropID("bush_tube.ring_plate.cMaterial")==id)
	{
		STEELMAT *pSteelMat=QuerySteelMatByBriefMark(ring_plate.cMaterial);
		if(pSteelMat)
			sText.Copy(pSteelMat->mark);
	}
	else if(GetPropID("bush_tube.ring_plate.fThick")==id)
	{
		sText.Printf("%f",ring_plate.fThick);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("bush_tube.ring_plate.fOuterD")==id)
	{
		sText.Printf("%f",ring_plate.fOuterD);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("bush_tube.ring_plate.fInnerD")==id)
	{
		sText.Printf("%f",ring_plate.fInnerD);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
BOOL CBushTube::ToXmlTemplFile()
{
	CXhChar100 templfile("%stemplate.bt",APP_PATH);
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("BUSH-TUBE");
	xml.AddAttrib("类型","普通钢板");
	//添加套管信息节间
	xml.IntoElem();
	xml.AddElem("TubeParams");
	xml.AddAttrib("PartNo",(char*)sPartNo);
	xml.AddAttrib("Material",(char*)CXhChar16("%c",cMaterial));
	xml.AddAttrib("Length",(char*)CXhChar16("%f",fLength));
	xml.AddAttrib("OuterD",(char*)CXhChar16("%f",fOuterD));
	xml.AddAttrib("InnerD",(char*)CXhChar16("%f",fInnerD));
	xml.AddAttrib("Thick",(char*)CXhChar16("%f",fThick));
	xml.OutOfElem();
	//添加环板信息节间
	xml.IntoElem();
	xml.AddElem("RingPlateParams");
	xml.AddAttrib("PartNo",(char*)ring_plate.sPartNo);
	xml.AddAttrib("Material",(char*)CXhChar16("%c",ring_plate.cMaterial));
	xml.AddAttrib("OuterD",(char*)CXhChar16("%f",ring_plate.fOuterD));
	xml.AddAttrib("InnerD",(char*)CXhChar16("%f",ring_plate.fInnerD));
	xml.AddAttrib("Thick",(char*)CXhChar16("%f",ring_plate.fThick));
	xml.OutOfElem();		//跳出</primaryAngle>
	return xml.Save((char*)templfile);
}
BOOL CBushTube::FromXmlTemplFile()
{
	CXhChar100 templfile("%stemplate.bt",APP_PATH);
	CMarkup xml;
	if(!xml.Load((char*)templfile))
		return FALSE;
	xml.ResetMainPos();
	ImportFootDataFromXML(xml,"TubeParams");
	ImportFootDataFromXML(xml,"RingPlateParams");
	return TRUE;
}
bool CBushTube::ImportFootDataFromXML(CMarkup xml,const char* sElemName)
{
	CString sValue=_T("");
	while(xml.FindChildElem(sElemName))   
	{ 
		xml.IntoElem();
		if(stricmp(sElemName,"TubeParams")==0)
		{
			sValue=xml.GetAttrib("PartNo");
			if(sValue.GetLength()>0)
				sPartNo.Copy(sValue);
			sValue=xml.GetAttrib("Material");
			if(sValue.GetLength()>0)
				cMaterial=sValue[0];
			sValue=xml.GetAttrib("Length");
			fLength=atof(sValue);
			sValue=xml.GetAttrib("OuterD");
			fOuterD=atof(sValue);
			sValue=xml.GetAttrib("InnerD");
			fInnerD=atof(sValue);
			sValue=xml.GetAttrib("Thick");
			fThick=atof(sValue);
		}
		else if(stricmp(sElemName,"RingPlateParams")==0)
		{
			sValue=xml.GetAttrib("PartNo");
			if(sValue.GetLength()>0)
				ring_plate.sPartNo.Copy(sValue);
			sValue=xml.GetAttrib("Material");
			if(sValue.GetLength()>0)
				ring_plate.cMaterial=sValue[0];
			sValue=xml.GetAttrib("OuterD");
			ring_plate.fOuterD=atof(sValue);
			sValue=xml.GetAttrib("InnerD");
			ring_plate.fInnerD=atof(sValue);
			sValue=xml.GetAttrib("Thick");
			ring_plate.fThick=atof(sValue);
		}
		xml.OutOfElem();
	}
	return true;
}
void CBushTube::CloneProp(CBushTube* pSrcBush)
{
	cMaterial=pSrcBush->cMaterial;
	sPartNo=pSrcBush->sPartNo;
	fLength=pSrcBush->fLength;
	fOuterD=pSrcBush->fOuterD;
	fInnerD=pSrcBush->fInnerD;
	fThick=pSrcBush->fThick;
	bRingPlate = pSrcBush->bRingPlate;
	if (bRingPlate)
	{
		ring_plate.cMaterial=pSrcBush->ring_plate.cMaterial;
		ring_plate.sPartNo = pSrcBush->ring_plate.sPartNo;
		ring_plate.fThick = pSrcBush->ring_plate.fThick;
		ring_plate.fInnerD = pSrcBush->ring_plate.fInnerD;
		ring_plate.fOuterD = pSrcBush->ring_plate.fOuterD;
	}
}
//////////////////////////////////////////////////////////////////////////
//回调函数处理
static BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CDesignBushTubeDlg *pBushTubeDlg=(CDesignBushTubeDlg*)pPropList->GetParent();
	if(pBushTubeDlg==NULL)
		return FALSE;
	CXhChar50 sText(valueStr);
	CPropertyListOper<CBushTube> oper(pPropList,&(pBushTubeDlg->m_xBush));
	//
	if(CBushTube::GetPropID("bush_tube.fLength")==pItem->m_idProp)
		pBushTubeDlg->m_xBush.fLength=atof(valueStr);
	else if(CBushTube::GetPropID("bush_tube.sPartNo")==pItem->m_idProp)
		pBushTubeDlg->m_xBush.sPartNo.Copy(valueStr);
	else if(CBushTube::GetPropID("bush_tube.cMaterial")==pItem->m_idProp)
		pBushTubeDlg->m_xBush.cMaterial=QuerySteelBriefMatMark(sText);
	else if(CBushTube::GetPropID("bush_tube.fOuterD")==pItem->m_idProp)
	{
		pBushTubeDlg->m_xBush.fOuterD=atof(valueStr);
		oper.InsertEditPropItem(pItem->m_pParent,"bush_tube.fThick","","",-1,TRUE);
	}
	else if(CBushTube::GetPropID("bush_tube.fInterD")==pItem->m_idProp)
	{	
		pBushTubeDlg->m_xBush.fInnerD=atof(valueStr);
		oper.InsertEditPropItem(pItem->m_pParent,"bush_tube.fThick","","",-1,TRUE);
	}
	else if(CBushTube::GetPropID("bush_tube.bRingPlate")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pBushTubeDlg->m_xBush.bRingPlate=TRUE;
		else
			pBushTubeDlg->m_xBush.bRingPlate=FALSE;
		pPropList->DeleteAllSonItems(pItem);
		if(pBushTubeDlg->m_xBush.bRingPlate)
		{
			oper.InsertEditPropItem(pItem,"bush_tube.ring_plate.sPartNo","","",-1,TRUE);
			oper.InsertCmbListPropItem(pItem,"bush_tube.ring_plate.cMaterial","","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"bush_tube.ring_plate.fThick","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"bush_tube.ring_plate.fOuterD","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"bush_tube.ring_plate.fInnerD","","",-1,TRUE);
		}
	}
	else if(CBushTube::GetPropID("bush_tube.ring_plate.fThick")==pItem->m_idProp)
		pBushTubeDlg->m_xBush.ring_plate.fThick=atof(valueStr);
	else if(CBushTube::GetPropID("bush_tube.ring_plate.fOuterD")==pItem->m_idProp)
		pBushTubeDlg->m_xBush.ring_plate.fOuterD=atof(valueStr);
	else if(CBushTube::GetPropID("bush_tube.ring_plate.fInnerD")==pItem->m_idProp)
		pBushTubeDlg->m_xBush.ring_plate.fInnerD=atof(valueStr);
	else if(CBushTube::GetPropID("bush_tube.ring_plate.sPartNo")==pItem->m_idProp)
		pBushTubeDlg->m_xBush.ring_plate.sPartNo.Copy(valueStr);
	else if(CBushTube::GetPropID("bush_tube.ring_plate.cMaterial")==pItem->m_idProp)
		pBushTubeDlg->m_xBush.ring_plate.cMaterial=QuerySteelBriefMatMark(sText);
	return TRUE;
}
static BOOL ButtonClickItem(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDesignBushTubeDlg *pBushTubeDlg=(CDesignBushTubeDlg*)pPropList->GetParent();
	if(pBushTubeDlg==NULL)
		return FALSE;
	if(CBushTube::GetPropID("bush_tube.hResidePlate")==pItem->m_idProp)
		pBushTubeDlg->SelectObject(CLS_PLATE,pItem->m_idProp);
	else if(CBushTube::GetPropID("bush_tube.hRelyBolt")==pItem->m_idProp)
		pBushTubeDlg->SelectObject(CLS_BOLT,pItem->m_idProp);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// DesignBushTubeDlg 对话框

IMPLEMENT_DYNAMIC(CDesignBushTubeDlg, CDialog)

CDesignBushTubeDlg::CDesignBushTubeDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignBushTubeDlg::IDD, pParent)
{

}

CDesignBushTubeDlg::~CDesignBushTubeDlg()
{
}

void CDesignBushTubeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
}


BEGIN_MESSAGE_MAP(CDesignBushTubeDlg, CDialog)
	ON_COMMAND(IDC_BTN_IMPORT_DATA, &CDesignBushTubeDlg::OnImportData)
END_MESSAGE_MAP()

bool CDesignBushTubeDlg::m_bValidBushParam = false;
CBushTube CDesignBushTubeDlg::m_xBushParam;
// DesignBushTubeDlg 消息处理程序
BOOL CDesignBushTubeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//初始化螺栓列表以及属性ID
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xBush.InitPropHashtable();	//初始化属性哈希表
	if(!m_bInernalStart)
	{
		CLDSBolt* pBolt=(CLDSBolt*)Ta.FromPartHandle(m_xBush.hRelyBolt);
		if(pBolt)
			m_xBush.fOuterD=pBolt->get_d()+pBolt->hole_d_increment;
		m_xBush.fInnerD=m_xBush.fOuterD-m_xBush.fThick;
		m_xBush.ring_plate.fInnerD=m_xBush.fOuterD;
		m_xBush.ring_plate.cMaterial=m_xBush.cMaterial;
		if (m_bValidBushParam)
			m_xBush.CloneProp(&m_xBushParam);
	}
	else
	{
		FinishSelectObjOper();	//完成选择对象的后续操作
		//恢复选中项
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();
		if(pItem)
			m_propList.SetCurSel(pItem->m_iIndex);	//设置选中项	
		m_bInernalStart=FALSE;
	}
	DisplayPropertyList();
	return TRUE;
}
void CDesignBushTubeDlg::DisplayPropertyList()
{
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(ModifyItemValue);
	m_propList.SetButtonClickFunc(ButtonClickItem);
	//
	CPropertyListOper<CBushTube> oper(&m_propList,&m_xBush);
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pRoot=m_propList.GetRootItem();
	pGroupItem=oper.InsertPropItem(pRoot,"Basic");
	pGroupItem->m_bHideChildren = FALSE;
	oper.InsertEditPropItem(pGroupItem,"bush_tube.sPartNo");		//构件编号
	oper.InsertCmbListPropItem(pGroupItem,"bush_tube.cMaterial");	//材质	
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"bush_tube.hResidePlate");	//焊接钢板
	pPropItem->SetReadOnly(TRUE);
	oper.InsertButtonPropItem(pGroupItem,"bush_tube.hRelyBolt");	//依赖螺栓
	oper.InsertEditPropItem(pGroupItem,"bush_tube.fOuterD");		//套管外径
	oper.InsertEditPropItem(pGroupItem,"bush_tube.fInterD");		//套管内径
	pPropItem=oper.InsertEditPropItem(pGroupItem,"bush_tube.fThick");//套管厚度
	pPropItem->SetReadOnly(TRUE);
	oper.InsertEditPropItem(pGroupItem,"bush_tube.fLength");	//套管长度
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"bush_tube.bRingPlate");
	if(m_xBush.bRingPlate)
	{
		oper.InsertEditPropItem(pPropItem,"bush_tube.ring_plate.sPartNo");
		oper.InsertCmbListPropItem(pPropItem,"bush_tube.ring_plate.cMaterial");
		oper.InsertEditPropItem(pPropItem,"bush_tube.ring_plate.fThick");
		oper.InsertEditPropItem(pPropItem,"bush_tube.ring_plate.fOuterD");
		oper.InsertEditPropItem(pPropItem,"bush_tube.ring_plate.fInnerD");
	}
	m_propList.Redraw();
}
void CDesignBushTubeDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	long hPart=0;
	if(GetEventPropId()==CBushTube::GetPropID("bush_tube.hResidePlate"))
	{
		CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
		if(pPart&&pPart->GetClassTypeId()==CLS_PLATE)
			m_xBush.hResidePlate=pPart->handle;
	}
	else if(GetEventPropId()==CBushTube::GetPropID("bush_tube.hRelyBolt"))
	{
		CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
		if(pPart&&pPart->GetClassTypeId()==CLS_BOLT)
		{
			m_xBush.hRelyBolt=pPart->handle;
			CLDSBolt* pBolt=(CLDSBolt*)pPart;
			m_xBush.fOuterD=pBolt->get_d()+pBolt->hole_d_increment;
		}
	}
}
void CDesignBushTubeDlg::OnImportData()
{
	if(m_xBush.FromXmlTemplFile())
		DisplayPropertyList();
}
void CDesignBushTubeDlg::OnOK()
{
	m_xBush.ToXmlTemplFile();
	m_bValidBushParam = true;
	m_xBushParam.CloneProp(&m_xBush);
	return CDialog::OnOK();
}