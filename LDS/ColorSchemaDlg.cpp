// ColorSchemeDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "ColorSchemaDlg.h"
#include "env_def.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "SpecialColorDlg.h"
#include "LdsPortalLocale.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorSchemeDlg dialog

CColorSchemeDlg::CColorSchemeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorSchemeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorSchemeDlg)
	m_iSchemeName = 2;
	//}}AFX_DATA_INIT
}


void CColorSchemeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorSchemeDlg)
	DDX_Control(pDX, IDC_COLORSET_LIST, m_propList);
	DDX_Control(pDX, IDC_LIST_EFFIC_COLOR_VARY, m_listcolor);
	DDX_CBIndex(pDX, IDC_SCHEME_NAME, m_iSchemeName);
	//}}AFX_DATA_MAP
}

const DWORD HASHTABLESIZE = 500;
CStringKeyHashTable<long> CColorSchemeDlg::propHashtable;
void CColorSchemeDlg::InitPropHashtable()
{
	int id = 2;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	propHashtable.SetValueAt("crPartMode.crNode",id++);
	propHashtable.SetValueAt("crPartMode.crControlNode",id++);
	propHashtable.SetValueAt("crPartMode.crDivideScaleNode",id++);
	propHashtable.SetValueAt("crPartMode.crAxisValFixNode",id++);
	propHashtable.SetValueAt("crPartMode.crOffsetNode",id++);
	propHashtable.SetValueAt("crPartMode.crLine",id++);
	propHashtable.SetValueAt("crPartMode.crPole",id++);
	propHashtable.SetValueAt("crPartMode.crAngleWingX",id++);
	propHashtable.SetValueAt("crPartMode.crAngleWingY",id++);
	propHashtable.SetValueAt("crPartMode.crAngleEdge",id++);
	propHashtable.SetValueAt("crPartMode.crSonAngle1WingX",id++);
	propHashtable.SetValueAt("crPartMode.crSonAngle1WingY",id++);
	propHashtable.SetValueAt("crPartMode.crSonAngle1Edge",id++);
	propHashtable.SetValueAt("crPartMode.crOtherBolt",id++);
	propHashtable.SetValueAt("crPartMode.crBolt12",id++);
	propHashtable.SetValueAt("crPartMode.crBolt16",id++);
	propHashtable.SetValueAt("crPartMode.crBolt20",id++);
	propHashtable.SetValueAt("crPartMode.crBolt24",id++);
	propHashtable.SetValueAt("crPartMode.crBolt27",id++);
	propHashtable.SetValueAt("crPartMode.crBolt30",id++);
	propHashtable.SetValueAt("crPartMode.crBolt36",id++);
	propHashtable.SetValueAt("crPartMode.crBolt39",id++);
	propHashtable.SetValueAt("crPartMode.crBolt42",id++);
	propHashtable.SetValueAt("crPartMode.crBolt48",id++);
	propHashtable.SetValueAt("crPartMode.crAntiTheftNut",id++);
	propHashtable.SetValueAt("crPartMode.crAntiLoosenNut",id++);
	propHashtable.SetValueAt("crPartMode.crPlate",id++);
	propHashtable.SetValueAt("crPartMode.crPlateS",id++);	//Q235板
	propHashtable.SetValueAt("crPartMode.crPlateH",id++);	//Q345板
	propHashtable.SetValueAt("crPartMode.crPlateG",id++);	//Q390板
	propHashtable.SetValueAt("crPartMode.crPlateP",id++);	//Q420板
	propHashtable.SetValueAt("crPartMode.crPlateT",id++);	//Q460板
	propHashtable.SetValueAt("crPartMode.crCutFaceOrPolyJoint",id++);
	propHashtable.SetValueAt("crPartMode.crShortAngle",id++);
	propHashtable.SetValueAt("crPartMode.crBackground",id++);
	propHashtable.SetValueAt("crPartMode.crPartBackground",id++);
	propHashtable.SetValueAt("crPartMode.crStdPart",id++);
	
	propHashtable.SetValueAt("crTsaMode.crNode",id++);
	propHashtable.SetValueAt("crTsaMode.crPostLine",id++);
	propHashtable.SetValueAt("crTsaMode.crLine",id++);
	propHashtable.SetValueAt("crTsaMode.crAuxLine",id++);
	propHashtable.SetValueAt("crTsaMode.crDimText",id++);
	propHashtable.SetValueAt("crTsaMode.crPole",id++);
	propHashtable.SetValueAt("crTsaMode.crLoadArrow",id++);
	propHashtable.SetValueAt("crTsaMode.crDeformedPoleLine",id++);
	propHashtable.SetValueAt("crTsaMode.crBackground",id++);
	propHashtable.SetValueAt("crTsaMode.crZeroStressElem",id++);
	propHashtable.SetValueAt("crTsaMode.bEfficColorLinearVary",id++);
	propHashtable.SetValueAt("crTsaMode.crOverflowElem",id++);
	
	propHashtable.SetValueAt("crMaterialMode.crBackground",id++);
	propHashtable.SetValueAt("crMaterialMode.crNode",id++);
	propHashtable.SetValueAt("crMaterialMode.crLine",id++);
	propHashtable.SetValueAt("crMaterialMode.crPole",id++);
	propHashtable.SetValueAt("crMaterialMode.crQ235",id++);
	propHashtable.SetValueAt("crMaterialMode.crQ345",id++);
	propHashtable.SetValueAt("crMaterialMode.crQ390",id++);
	propHashtable.SetValueAt("crMaterialMode.crQ420",id++);
	propHashtable.SetValueAt("crMaterialMode.crQ460",id++);
	propHashtable.SetValueAt("crMaterialMode.crOther",id++);

	propHashtable.SetValueAt("defaultColor",id++);
}
void CreatNewPropItem(CPropertyList *pPropList,CPropTreeItem* pItem,char* sProp)	
{
	if(sProp==NULL||strlen(sProp)<=0)
		return;
	CSpecialColorDlg dlg;
	strcpy(dlg.m_sProp,sProp);
	if(dlg.DoModal()!=IDOK)
		return;
	CColorSchemeDlg* pDlg=(CColorSchemeDlg*)pPropList->GetParent();
	COLORREF crDefault=g_sysPara.plateColRef.GetPlateColor(sProp);
	CStringArray strArr;
	strArr.SetSize(5);
	strArr[0] = "S";
	strArr[1] = "H";
	strArr[2] = "G";
	strArr[3] = "P";
	strArr[4] = "T";
	for(int i=0;i<5;i++)
	{
		CXhChar50 sSonPropStr,sPropStr;
		sprintf(sPropStr,"%s_%s",sProp,strArr[i]);
		pDlg->GetPropStr(pItem->m_idProp,sSonPropStr);
		sprintf(sSonPropStr,"%s%s",&sSonPropStr,strArr[i]);
		long idSonProp=CColorSchemeDlg::GetPropID(sSonPropStr);
		CPropTreeItem* pSonPropItem=pPropList->FindItemByPropId(idSonProp,NULL);
		if(!dlg.m_arrIsCanUse[i] || dlg.m_arrValue[i]==crDefault)
		{
			if(pSonPropItem)	//删除子属性
				pPropList->DeleteItemByPropId(pSonPropItem->m_idProp);
			g_sysPara.plateColRef.SetPlateColor(sPropStr,crDefault);
			continue;	
		}
		if(pSonPropItem==NULL)
		{	//添加子属性
			CItemInfo* lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType = CDT_COLOR;
			lpInfo->m_strPropName = pItem->m_lpNodeInfo->m_strPropName+strArr[i];
			lpInfo->m_strPropHelp = pItem->m_lpNodeInfo->m_strPropHelp+strArr[i];
			pSonPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = idSonProp;
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("RGB%X",dlg.m_arrValue[i]);
			pSonPropItem->m_dwPropGroup=pItem->m_dwPropGroup;
		}
		else//更新已有子属性值
			pPropList->SetItemPropValue(idSonProp,"RGB%X",dlg.m_arrValue[i]);
		g_sysPara.plateColRef.SetPlateColor(sPropStr,dlg.m_arrValue[i]);
	}
}
static BOOL ButtonClickColorItem(CPropertyList *pPropList,CPropTreeItem *pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if (pItem->m_idProp==CColorSchemeDlg::GetPropID("defaultColor"))
	{
		if (g_sysPara.iColorSchema == 3)
		{	//构件配色方案
			g_sysPara.crPartMode.crNode = RGB(255,255,255);
			g_sysPara.crPartMode.crControlNode = RGB(255,255,255);		  //控制节点
			g_sysPara.crPartMode.crDivideScaleNode = RGB(255,255,0);      //等分节点
			g_sysPara.crPartMode.crOffsetNode = RGB(128,128,0);   	      //偏移节点
			g_sysPara.crPartMode.crAxisValFixNode= RGB(255,80,0);		  //指定坐标分量节点
			g_sysPara.crPartMode.crLine = RGB(255,255,255);	//RGB(192,192,192);//(150,150,255);重新调整为银灰色更好看 wjh-2018.6.30
			g_sysPara.crPartMode.crPole = RGB(192,192,192);//(150,150,255);重新调整为银灰色更好看 wjh-2018.6.30
			g_sysPara.crPartMode.crAngleWingX = RGB(220,220,220);//RGB(192,192,192);//(150,150,255);重新调整为银灰色更好看 wjh-2018.6.30
			g_sysPara.crPartMode.crAngleWingY = RGB(220,220,220);//RGB(192,192,192);//(150,150,255);重新调整为银灰色更好看 wjh-2018.6.30
			g_sysPara.crPartMode.crAngleEdge = RGB(0,100,255);
			g_sysPara.crPartMode.crSonAngle1WingX = RGB(0,128,128);
			g_sysPara.crPartMode.crSonAngle1WingY = RGB(0,128,128);
			g_sysPara.crPartMode.crSonAngle1Edge = RGB(0,100,255);
			g_sysPara.crPartMode.crOtherBolt = RGB(128,0,255);
			g_sysPara.crPartMode.crBolt12 = RGB(255,0,255);
			LOCALE_PROPITEM* pLocaleItem = gxPortalLocalizer.GetLocaleItem("M16.Color");
			if (pLocaleItem != NULL)
				g_sysPara.crPartMode.crBolt16 = pLocaleItem->liVal;
			else
				g_sysPara.crPartMode.crBolt16 = RGB(128, 0, 64);
			pLocaleItem = gxPortalLocalizer.GetLocaleItem("M20.Color");
			if (pLocaleItem != NULL)
				g_sysPara.crPartMode.crBolt20 = pLocaleItem->liVal;
			else
				g_sysPara.crPartMode.crBolt20 = RGB(128, 0, 255);
			pLocaleItem = gxPortalLocalizer.GetLocaleItem("M24.Color");
			if (pLocaleItem != NULL)
				g_sysPara.crPartMode.crBolt24 = pLocaleItem->liVal;
			else
				g_sysPara.crPartMode.crBolt24 = RGB(46, 0, 91);
			g_sysPara.crPartMode.crPlate = RGB(159,120,50);	//RGB(0,255,128);
			g_sysPara.plateColRef.colorHashList.Empty();
			g_sysPara.crPartMode.crCutFaceOrPolyJoint = RGB(255,128,128);
			g_sysPara.crPartMode.crShortAngle = RGB(90,90,90);
			g_sysPara.crPartMode.crBackground = RGB(0,64,160);
			g_sysPara.crPartMode.crPartBackground = RGB(150,150,255);
			g_sysPara.crPartMode.crStdPart = RGB(100,100,100);
			CString clrStr; 
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crNode);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crNode"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crControlNode);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crControlNode"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crDivideScaleNode);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crDivideScaleNode"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crOffsetNode);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crOffsetNode"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAxisValFixNode);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crAxisValFixNode"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crLine);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crLine"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crPole);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crPole"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAngleWingX);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crAngleWingX"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAngleWingY);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crAngleWingY"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crAngleEdge);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crAngleEdge"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1WingX);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crSonAngle1WingX"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1WingY);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crSonAngle1WingY"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1Edge);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crSonAngle1Edge"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crOtherBolt);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crOtherBolt"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt12);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crBolt12"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt16);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crBolt16"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt20);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crBolt20"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBolt24);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crBolt24"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crPlate);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crPlate"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crCutFaceOrPolyJoint);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crCutFaceOrPolyJoint"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crShortAngle);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crShortAngle"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crBackground);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crBackground"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crPartBackground);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crPartBackground"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crPartMode.crStdPart);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crPartMode.crStdPart"),clrStr);
			if(pView->m_curAppType==TA_ASSEMBLE_ENV)
				g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crBackground);
			else
				g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crPartBackground);
		}
		else if (g_sysPara.iColorSchema == 2)
		{	//材质配色方案
			g_sysPara.crMaterialMode.crNode = RGB(255,255,255);
			g_sysPara.crMaterialMode.crPole = RGB(150,150,255);
			g_sysPara.crMaterialMode.crLine = RGB(150,150,255);
			g_sysPara.crMaterialMode.crBackground = RGB(0,64,160);
			g_sysPara.crMaterialMode.crQ235 = RGB(255,0,0);
			g_sysPara.crMaterialMode.crQ345 = RGB(0,255,0);
			g_sysPara.crMaterialMode.crQ390 = RGB(0,0,255);
			g_sysPara.crMaterialMode.crQ420 = RGB(255,127,0);
			g_sysPara.crMaterialMode.crQ460 = RGB(127,255,0);
			g_sysPara.crMaterialMode.crOther = RGB(0,255,127);
			//
			CString clrStr;
			clrStr.Format("RGB%X",g_sysPara.crMaterialMode.crNode);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crMaterialMode.crNode"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crMaterialMode.crLine);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crMaterialMode.crLine"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crMaterialMode.crPole);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crMaterialMode.crPole"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crMaterialMode.crQ235);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crMaterialMode.crQ235"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crMaterialMode.crQ345);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crMaterialMode.crQ345"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crMaterialMode.crQ390);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crMaterialMode.crQ390"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crMaterialMode.crQ420);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crMaterialMode.crQ420"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crMaterialMode.crQ460);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crMaterialMode.crQ460"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crMaterialMode.crOther);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crMaterialMode.crOther"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crMaterialMode.crBackground);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crMaterialMode.crBackground"),clrStr);
			g_pSolidSet->SetBkColor(g_sysPara.crMaterialMode.crBackground);
		}
		else if (g_sysPara.iColorSchema == 1)
		{	//TSA配色方案
			g_sysPara.crTsaMode.crNode = RGB(255,128,0);
			g_sysPara.crTsaMode.crPostLine = RGB(0,0,0);
			g_sysPara.crTsaMode.crPrimaryLine = RGB(128,65,50);
			g_sysPara.crTsaMode.crAuxLine = RGB(0,128,255);
			g_sysPara.crTsaMode.crDimText = RGB(0,0,0);
			g_sysPara.crTsaMode.crPole = RGB(150,150,255);
			g_sysPara.crTsaMode.crDeformedPoleLine = RGB(128,0,128);
			g_sysPara.crTsaMode.crLoadArrow = RGB(0,255,255);
			g_sysPara.crTsaMode.crBackground = RGB(192,192,192);
			g_sysPara.crTsaMode.crZeroStressElem =  RGB(0,0,0);
			g_sysPara.crTsaMode.crOverflowElem = RGB(255,0,0);
			CString clrStr; 
			clrStr.Format("RGB%X",g_sysPara.crTsaMode.crNode);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crTsaMode.crNode"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crTsaMode.crPostLine);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crTsaMode.crPostLine"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crTsaMode.crPrimaryLine);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crTsaMode.crLine"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crTsaMode.crAuxLine);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crTsaMode.crAuxLine"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crTsaMode.crPole);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crTsaMode.crPole"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crTsaMode.crDeformedPoleLine);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crTsaMode.crDimText"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crTsaMode.crDimText);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crTsaMode.crDeformedPoleLine"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crTsaMode.crLoadArrow);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crTsaMode.crLoadArrow"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crTsaMode.crBackground);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crTsaMode.crBackground"),clrStr);
			g_pSolidSet->SetBkColor(g_sysPara.crTsaMode.crBackground);
			clrStr.Format("RGB%X",g_sysPara.crTsaMode.crZeroStressElem);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crTsaMode.crZeroStressElem"),clrStr);
			clrStr.Format("RGB%X",g_sysPara.crTsaMode.crOverflowElem);
			pPropList->SetItemPropValue(CColorSchemeDlg::GetPropID("crTsaMode.crOverflowElem"),clrStr);
		}
	}
	else if(pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crPlate"))
		CreatNewPropItem(pPropList,pItem,"crPLATE");
	pView->RedrawAll();
	return true;
}
BOOL CColorSchemeDlg::Refresh(BOOL bJudge)
{
	CRect rect;
	CRect rectGrid;
	CRect rectColorList;
	GetDlgItem(IDC_LIST_EFFIC_COLOR_VARY)->GetWindowRect(&rectGrid);
	GetDlgItem(IDC_COLORSET_LIST)->GetWindowRect(&rectColorList);
	GetWindowRect(rect);
	if(bJudge)
	{
		if(rect.Width()<(rectColorList.Width()+rectGrid.Width()))
		{
			rect.right = rect.Width()+rectGrid.Width();
			rect.left = 0;
			rect.bottom = rect.Height();
			rect.top = 0;	
		}
		if(m_listcolor.GetItemCount()>0)
		m_listcolor.DeleteAllItems();
		CSuperGridCtrl::CTreeItem* pItem=NULL,*pSubItem=NULL;
		CString itemText;
		for (DWORD i=0;i<6;i++)
		{
			CListCtrlItemInfo* lpItemInfo= new CListCtrlItemInfo();
			lpItemInfo->SetSubItemColor(0,RGB(192,192,192));
			int k=i+1;
			itemText.Format("%d",k);
			lpItemInfo->AddSubItemText(itemText,TRUE);
			pItem=m_listcolor.InsertRootItem(lpItemInfo);
			pItem->m_idProp=i+1;
			itemText.Format("%d",g_sysPara.crTsaMode.crElemDivArr[i].efficPercent);
			lpItemInfo->AddSubItemText(itemText,FALSE);
			itemText.Format("RGB%X",g_sysPara.crTsaMode.crElemDivArr[i].crElemDivision);
			lpItemInfo->AddSubItemText(itemText,FALSE);
			lpItemInfo->SetControlType(2,GCT_CMB_COLOR);
		}
		m_listcolor.Redraw();
	}
	else
	{
		if(rect.Width()>(rectColorList.Width()+rectGrid.Width()))
		{
			rect.right = rect.Width()-rectGrid.Width();
			rect.left = 0;
			rect.bottom = rect.Height();
			rect.top = 0;	
		}
	}
	MoveWindow(rect,TRUE);
	return true;
}
static BOOL ButtonClickItemProperty(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	if(g_sysPara.iColorSchema==1)
	{
		if(pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.bEfficColorLinearVary"))
		{
			CColorSchemeDlg* pDlg=(CColorSchemeDlg*)pPropList->GetParent();
			pDlg->Refresh(TRUE);
		}
		else
		{
			CColorSchemeDlg* pDlg=(CColorSchemeDlg*)pPropList->GetParent();
			pDlg->Refresh(FALSE);
		}
	}
	return true;
}

static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	CColorSchemeDlg *pColorSchemeDlg=(CColorSchemeDlg*)pListCtrl->GetParent();
	if(sTextValue==""||sTextValue=="0")
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The data can't be empty!");
#else 
		AfxMessageBox("数据不能为空");
#endif
		return FALSE;
	}
	CString oldText = pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	if (oldText.CompareNoCase(sTextValue)==0)
		return FALSE;
	else if(pSelItem->m_idProp<=0)
		return FALSE;
	else if(g_sysPara.iColorSchema == 1&&pSelItem->m_idProp<=6)
	{
		COLORREF curClr = 0;
		char tem_str[MAX_TEMP_CHAR_100+1]="";
		_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",sTextValue);
		memmove(tem_str, tem_str+3, 97);	
		sscanf(tem_str,"%X",&curClr);
		if(iSubItem==1)
			g_sysPara.crTsaMode.crElemDivArr[pSelItem->m_idProp-1].efficPercent=atoi(sTextValue);
		if(iSubItem==2)
			g_sysPara.crTsaMode.crElemDivArr[pSelItem->m_idProp-1].crElemDivision=curClr;
		return true;
	}
	pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	return true;
}

static BOOL ModifyColorItemProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{	
	COLORREF curClr = 0;
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	memmove(tem_str, tem_str+3, 97);	
	sscanf(tem_str,"%X",&curClr);
	//
	if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crNode"))
	{
		if(g_sysPara.crPartMode.crNode!=curClr)
			g_sysPara.crPartMode.crNode=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crControlNode"))
	{
		if(g_sysPara.crPartMode.crControlNode!=curClr)
			g_sysPara.crPartMode.crControlNode=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crDivideScaleNode"))
	{
		if(g_sysPara.crPartMode.crDivideScaleNode !=curClr)
			g_sysPara.crPartMode.crDivideScaleNode =curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crOffsetNode"))
	{
		if(g_sysPara.crPartMode.crOffsetNode!=curClr)
			g_sysPara.crPartMode.crOffsetNode=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crAxisValFixNode"))
	{
		if(g_sysPara.crPartMode.crAxisValFixNode!=curClr)
			g_sysPara.crPartMode.crAxisValFixNode=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crLine"))
	{
		if(g_sysPara.crPartMode.crLine!=curClr)
			g_sysPara.crPartMode.crLine=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crPole"))
	{
		if(g_sysPara.crPartMode.crPole!=curClr)
			g_sysPara.crPartMode.crPole=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crAngleWingX"))
	{
		if(g_sysPara.crPartMode.crAngleWingX!=curClr)
			g_sysPara.crPartMode.crAngleWingX=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crAngleWingY"))
	{
		if(g_sysPara.crPartMode.crAngleWingY!=curClr)
			g_sysPara.crPartMode.crAngleWingY=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crAngleEdge"))
	{
		if(g_sysPara.crPartMode.crAngleEdge!=curClr)
			g_sysPara.crPartMode.crAngleEdge=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crSonAngle1WingX"))
	{
		if(g_sysPara.crPartMode.crSonAngle1WingX!=curClr)
			g_sysPara.crPartMode.crSonAngle1WingX=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crSonAngle1WingY"))
	{
		if(g_sysPara.crPartMode.crSonAngle1WingY!=curClr)
			g_sysPara.crPartMode.crSonAngle1WingY=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crSonAngle1Edge"))
	{
		if(g_sysPara.crPartMode.crSonAngle1Edge!=curClr)
			g_sysPara.crPartMode.crSonAngle1Edge=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crOtherBolt"))
	{
		if(g_sysPara.crPartMode.crOtherBolt!=curClr)
			g_sysPara.crPartMode.crOtherBolt=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crBolt12"))
	{
		if(g_sysPara.crPartMode.crBolt12!=curClr)
			g_sysPara.crPartMode.crBolt12=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crBolt16"))
	{
		if(g_sysPara.crPartMode.crBolt16!=curClr)
			g_sysPara.crPartMode.crBolt16=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crBolt20"))
	{
		if(g_sysPara.crPartMode.crBolt20!=curClr)
			g_sysPara.crPartMode.crBolt20=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crBolt24"))
	{
		if(g_sysPara.crPartMode.crBolt24!=curClr)
			g_sysPara.crPartMode.crBolt24=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crAntiTheftNut"))
	{
		if(g_sysPara.crPartMode.crAntiTheftNut!=curClr)
			g_sysPara.crPartMode.crAntiTheftNut=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crAntiLoosenNut"))
	{
		if(g_sysPara.crPartMode.crAntiLoosenNut!=curClr)
			g_sysPara.crPartMode.crAntiLoosenNut=curClr;
	}
	else if(pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crPlate"))
		g_sysPara.plateColRef.SetPlateColor("crPLATE",curClr);
	else if(pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crPlateS"))
		g_sysPara.plateColRef.SetPlateColor("crPLATE_S",curClr);
	else if(pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crPlateH"))
		g_sysPara.plateColRef.SetPlateColor("crPLATE_H",curClr);
	else if(pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crPlateG"))
		g_sysPara.plateColRef.SetPlateColor("crPLATE_G",curClr);
	else if(pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crPlateP"))
		g_sysPara.plateColRef.SetPlateColor("crPLATE_P",curClr);
	else if(pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crPlateT"))
		g_sysPara.plateColRef.SetPlateColor("crPLATE_T",curClr);
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crCutFaceOrPolyJoint"))
	{
		if(g_sysPara.crPartMode.crCutFaceOrPolyJoint!=curClr)
			g_sysPara.crPartMode.crCutFaceOrPolyJoint=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crShortAngle"))
	{
		if(g_sysPara.crPartMode.crShortAngle!=curClr)
			g_sysPara.crPartMode.crShortAngle=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crBackground"))
	{
		if(g_sysPara.crPartMode.crBackground!=curClr)
		{
			g_sysPara.crPartMode.crBackground=curClr;
			g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crBackground);
		}
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crPartBackground"))
	{
		if(g_sysPara.crPartMode.crPartBackground!=curClr)
			g_sysPara.crPartMode.crPartBackground=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crPartMode.crStdPart"))
	{
		if(g_sysPara.crPartMode.crStdPart!=curClr)
			g_sysPara.crPartMode.crStdPart=curClr;
	}
	//TSA
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.crNode"))
	{
		if(g_sysPara.crTsaMode.crNode!=curClr)
			g_sysPara.crTsaMode.crNode=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.crPostLine"))
	{
		if(g_sysPara.crTsaMode.crPostLine!=curClr)
			g_sysPara.crTsaMode.crPostLine=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.crLine"))
	{
		if(g_sysPara.crTsaMode.crPrimaryLine!=curClr)
			g_sysPara.crTsaMode.crPrimaryLine=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.crAuxLine"))
	{
		if(g_sysPara.crTsaMode.crAuxLine!=curClr)
			g_sysPara.crTsaMode.crAuxLine=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.crDimText"))
	{
		if(g_sysPara.crTsaMode.crDimText!=curClr)
			g_sysPara.crTsaMode.crDimText=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.crPole"))
	{
		if(g_sysPara.crTsaMode.crPole!=curClr)
			g_sysPara.crTsaMode.crPole=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.crLoadArrow"))
	{
		if(g_sysPara.crTsaMode.crLoadArrow!=curClr)
			g_sysPara.crTsaMode.crLoadArrow=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.crBackground"))
	{
		if(g_sysPara.crTsaMode.crBackground!=curClr)
		{
			g_sysPara.crTsaMode.crBackground=curClr;
			g_pSolidSet->SetBkColor(g_sysPara.crTsaMode.crBackground);
		}
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.crZeroStressElem"))
	{
		if(g_sysPara.crTsaMode.crZeroStressElem!=curClr)
		{
			g_sysPara.crTsaMode.crZeroStressElem=curClr;
		}
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.crOverflowElem"))
	{
		if(g_sysPara.crTsaMode.crOverflowElem!=curClr)
		{
			g_sysPara.crTsaMode.crOverflowElem=curClr;
		}
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.crDeformedPoleLine"))
	{
		if(g_sysPara.crTsaMode.crDeformedPoleLine!=curClr)
			g_sysPara.crTsaMode.crDeformedPoleLine=curClr;
	}
	else if(pItem->m_idProp==CColorSchemeDlg::GetPropID("crTsaMode.bEfficColorLinearVary"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Partition color")==0)
			g_sysPara.crTsaMode.bEfficColorLinearVary=false;
		else if(valueStr.CompareNoCase("Gradient color")==0)
			g_sysPara.crTsaMode.bEfficColorLinearVary=true;
#else 
		if(valueStr.CompareNoCase("分区配色")==0)
			g_sysPara.crTsaMode.bEfficColorLinearVary=false;
		else if(valueStr.CompareNoCase("渐变配色")==0)
			g_sysPara.crTsaMode.bEfficColorLinearVary=true;
#endif
	}
	//
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crMaterialMode.crNode"))
	{
		if(g_sysPara.crMaterialMode.crNode!=curClr)
			g_sysPara.crMaterialMode.crNode=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crMaterialMode.crLine"))
	{
		if(g_sysPara.crMaterialMode.crLine!=curClr)
			g_sysPara.crMaterialMode.crLine=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crMaterialMode.crPole"))
	{
		if(g_sysPara.crMaterialMode.crPole!=curClr)
			g_sysPara.crMaterialMode.crPole=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crMaterialMode.crBackground"))
	{
		if(g_sysPara.crMaterialMode.crBackground!=curClr)
		{
			g_sysPara.crMaterialMode.crBackground=curClr;
			g_pSolidSet->SetBkColor(g_sysPara.crMaterialMode.crBackground);
		}
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crMaterialMode.crQ235"))
	{
		if(g_sysPara.crMaterialMode.crQ235!=curClr)
			g_sysPara.crMaterialMode.crQ235=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crMaterialMode.crQ345"))
	{
		if(g_sysPara.crMaterialMode.crQ345!=curClr)
			g_sysPara.crMaterialMode.crQ345=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crMaterialMode.crQ390"))
	{
		if(g_sysPara.crMaterialMode.crQ390!=curClr)
			g_sysPara.crMaterialMode.crQ390=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crMaterialMode.crQ420"))
	{
		if(g_sysPara.crMaterialMode.crQ420!=curClr)
			g_sysPara.crMaterialMode.crQ420=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crMaterialMode.crQ460"))
	{
		if(g_sysPara.crMaterialMode.crQ460!=curClr)
			g_sysPara.crMaterialMode.crQ460=curClr;
	}
	else if (pItem->m_idProp==CColorSchemeDlg::GetPropID("crMaterialMode.crOther"))
	{
		if(g_sysPara.crMaterialMode.crOther!=curClr)
			g_sysPara.crMaterialMode.crOther=curClr;
	}
	return true;
}
void CColorSchemeDlg::InsertPropItem(CPropTreeItem *pPropItem)
{
	CStringArray strArr;
	strArr.SetSize(5);
	strArr[0] = "S";
	strArr[1] = "H";
	strArr[2] = "G";
	strArr[3] = "P";
	strArr[4] = "T";
	COLORREF crDefault=g_sysPara.plateColRef.GetPlateColor("crPLATE");
	CXhChar50 proStr;
	for(int i=0;i<5;i++)
	{
		COLORREF crValue;
		proStr.Printf("crPLATE_%s",strArr[i]);
		crValue=g_sysPara.plateColRef.GetPlateColor(proStr);
		if(crValue!=crDefault)
		{
			CItemInfo* lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType = CDT_COLOR;
			lpInfo->m_strPropName = pPropItem->m_lpNodeInfo->m_strPropName+strArr[i];
			lpInfo->m_strPropHelp = pPropItem->m_lpNodeInfo->m_strPropHelp+strArr[i];
			lpInfo->m_strPropValue.Format("RGB%X",crValue);
			CPropTreeItem* pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
			char sStr[50]="";
			GetPropStr(pPropItem->m_idProp,sStr);
			sprintf(sStr,"%s%s",sStr,strArr[i]);
			pSonPropItem->m_idProp = CColorSchemeDlg::GetPropID(sStr);
			pSonPropItem->m_dwPropGroup=pPropItem->m_dwPropGroup;
		}
	}	
}
void CColorSchemeDlg::DisplayPropertyListPart()
{
	m_propList.CleanTree();
	//设置属性栏回调函数
	m_propList.SetModifyValueFunc(ModifyColorItemProperty);
	m_propList.SetButtonClickFunc(ButtonClickColorItem);
	m_propList.SetPropHelpPromptFunc(ButtonClickItemProperty);
	CItemInfo* lpInfo;
	CPropTreeItem *pGroupItem,*pPropItem,*pRoot=m_propList.GetRootItem();
	if(m_iSchemeName == 0)
	{	//TSA配色方案
		lpInfo = new CItemInfo();	
		lpInfo->m_controlType = PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "TSA Color Scheme";
		lpInfo->m_strPropHelp = "TSA Color Scheme";
#else 
		lpInfo->m_strPropName = "TSA配色方案";
		lpInfo->m_strPropHelp = "TSA配色方案";
#endif
		pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Node";
		lpInfo->m_strPropHelp = "Node";
#else 
		lpInfo->m_strPropName = "节点";
		lpInfo->m_strPropHelp = "节点";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crTsaMode.crNode);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crTsaMode.crNode");

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Single Line";
		lpInfo->m_strPropHelp = "Single Line";
#else 
		lpInfo->m_strPropName = "主材单线";
		lpInfo->m_strPropHelp = "主材单线";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crTsaMode.crPostLine);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crTsaMode.crPostLine");

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Single Line";
		lpInfo->m_strPropHelp = "Single Line";
#else 
		lpInfo->m_strPropName = "单线";
		lpInfo->m_strPropHelp = "普通受力材单线";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crTsaMode.crPrimaryLine);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crTsaMode.crLine");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Ancillary Pole Single Line";
		lpInfo->m_strPropHelp = "ancillary pole Single Line";
#else 
		lpInfo->m_strPropName = "辅材单线";
		lpInfo->m_strPropHelp = "辅材单线";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crTsaMode.crAuxLine);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crTsaMode.crAuxLine");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Rod";
		lpInfo->m_strPropHelp = "Rod";
#else 
		lpInfo->m_strPropName = "杆件";
		lpInfo->m_strPropHelp = "杆件";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crTsaMode.crPole);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crTsaMode.crPole");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Text Dimension";
		lpInfo->m_strPropHelp = "Node label、rod and element information dimension";
#else 
		lpInfo->m_strPropName = "文字标注";
		lpInfo->m_strPropHelp = "节点号、杆件及单元信息标注";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crTsaMode.crDimText);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crTsaMode.crDimText");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Load Arrow";
		lpInfo->m_strPropHelp = "Load Arrow";
#else 
		lpInfo->m_strPropName = "负荷箭头";
		lpInfo->m_strPropHelp = "负荷箭头";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crTsaMode.crLoadArrow);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crTsaMode.crLoadArrow");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Backgroud";
		lpInfo->m_strPropHelp = "Backgroud";
#else 
		lpInfo->m_strPropName = "背景";
		lpInfo->m_strPropHelp = "背景";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crTsaMode.crBackground);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crTsaMode.crBackground");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part Corlor After Transmutation";
		lpInfo->m_strPropHelp = "Part Corlor After Transmutation";
#else 
		lpInfo->m_strPropName = "变形后杆件颜色";
		lpInfo->m_strPropHelp = "变形后杆件颜色";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crTsaMode.crDeformedPoleLine);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crTsaMode.crDeformedPoleLine");

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Elem Efficiency Color Scheme";
		lpInfo->m_strPropHelp = "Elem Efficiency Color Scheme";
		lpInfo->m_cmbItems= "Partition Color|Gradient Color";
#else 
		lpInfo->m_strPropName = "单元效率配色方案";
		lpInfo->m_strPropHelp = "单元效率配色方案";
		lpInfo->m_cmbItems= "分区配色|渐变配色";
#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crTsaMode.bEfficColorLinearVary");
#ifdef AFX_TARG_ENU_ENGLISH
		if(!g_sysPara.crTsaMode.bEfficColorLinearVary)
			lpInfo->m_strPropValue="Partition Color";
		else
			lpInfo->m_strPropValue="Gradient Color";
#else 
		if(!g_sysPara.crTsaMode.bEfficColorLinearVary)
			lpInfo->m_strPropValue="分区配色";
		else
			lpInfo->m_strPropValue="渐变配色";
#endif

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Zero Rod Elem";
		lpInfo->m_strPropHelp = "Zero rod elem";
#else 
		lpInfo->m_strPropName = "零杆单元";
		lpInfo->m_strPropHelp = "零杆单元";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crTsaMode.crZeroStressElem);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crTsaMode.crZeroStressElem");

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "OverLoad Elem";
		lpInfo->m_strPropHelp = "Overload elem";
#else 
		lpInfo->m_strPropName = "过载单元";
		lpInfo->m_strPropHelp = "过载单元";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crTsaMode.crOverflowElem);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crTsaMode.crOverflowElem");

	}
	else if (m_iSchemeName == 1)
	{	//材质配色方案
		lpInfo = new CItemInfo();	
		lpInfo->m_controlType = PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Material Color Scheme";
		lpInfo->m_strPropHelp = "Material Color Scheme";
#else 
		lpInfo->m_strPropName = "材质配色方案";
		lpInfo->m_strPropHelp = "材质配色方案";
#endif
		pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Background";
		lpInfo->m_strPropHelp = "Background";
#else 
		lpInfo->m_strPropName = "背景";
		lpInfo->m_strPropHelp = "背景";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crMaterialMode.crBackground);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crMaterialMode.crBackground");
		
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Node";
		lpInfo->m_strPropHelp = "Node";
#else 
		lpInfo->m_strPropName = "节点";
		lpInfo->m_strPropHelp = "节点";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crMaterialMode.crNode);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crMaterialMode.crNode");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Single Line";
		lpInfo->m_strPropHelp = "Single LIne";
#else 
		lpInfo->m_strPropName = "单线";
		lpInfo->m_strPropHelp = "单线";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crMaterialMode.crLine);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crMaterialMode.crLine");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Rod";
		lpInfo->m_strPropHelp = "Rod";
#else 
		lpInfo->m_strPropName = "杆件";
		lpInfo->m_strPropHelp = "杆件";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crMaterialMode.crPole);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crMaterialMode.crPole");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
		lpInfo->m_strPropName = "Q235";
		lpInfo->m_strPropHelp = "Q235";
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crMaterialMode.crQ235);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crMaterialMode.crQ235");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
		lpInfo->m_strPropName = "Q345";
		lpInfo->m_strPropHelp = "Q345";
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crMaterialMode.crQ345);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crMaterialMode.crQ345");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
		lpInfo->m_strPropName = "Q390";
		lpInfo->m_strPropHelp = "Q390";
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crMaterialMode.crQ390);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crMaterialMode.crQ390");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
		lpInfo->m_strPropName = "Q420";
		lpInfo->m_strPropHelp = "Q420";
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crMaterialMode.crQ420);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crMaterialMode.crQ420");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
		lpInfo->m_strPropName = "Q460";
		lpInfo->m_strPropHelp = "Q460";
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crMaterialMode.crQ460);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crMaterialMode.crQ460");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Others";
		lpInfo->m_strPropHelp = "Others";
#else 
		lpInfo->m_strPropName = "其他";
		lpInfo->m_strPropHelp = "其他";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crMaterialMode.crOther);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crMaterialMode.crOther");
	}
	else //if (m_iSchemeName == 2)
	{	//构件配色方案
		lpInfo = new CItemInfo();	
		lpInfo->m_controlType = PIT_STATIC;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part Color Scheme";
		lpInfo->m_strPropHelp = "Part Color Scheme";
#else 
		lpInfo->m_strPropName = "构件配色方案";
		lpInfo->m_strPropHelp = "构件配色方案";
#endif
		pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Node";
		lpInfo->m_strPropHelp = "Node";
#else 
		lpInfo->m_strPropName = "节点";
		lpInfo->m_strPropHelp = "节点";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crNode);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crNode");

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Control Node";
		lpInfo->m_strPropHelp = "Control Node";
#else 
		lpInfo->m_strPropName = "控制节点";
		lpInfo->m_strPropHelp = "控制节点";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crControlNode);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crControlNode");

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Divide Scale Node ";
		lpInfo->m_strPropHelp = "Divide Scale Node ";
#else 
		lpInfo->m_strPropName = "等分节点";
		lpInfo->m_strPropHelp = "等分节点";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crDivideScaleNode);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crDivideScaleNode");

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Offset Node";
		lpInfo->m_strPropHelp = "Offset Node";
#else 
		lpInfo->m_strPropName = "偏移节点";
		lpInfo->m_strPropHelp = "偏移节点";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crOffsetNode);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crOffsetNode");

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Coord Specifed Node";
		lpInfo->m_strPropHelp = "Coord  Specifed Node";
#else 
		lpInfo->m_strPropName = "指定坐标分量节点";
		lpInfo->m_strPropHelp = "指定坐标分量节点";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crAxisValFixNode);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crAxisValFixNode");

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Single Line";
		lpInfo->m_strPropHelp = "Single Line";
#else 
		lpInfo->m_strPropName = "单线";
		lpInfo->m_strPropHelp = "单线";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crLine);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crLine");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Rod";
		lpInfo->m_strPropHelp = "Rod";
#else 
		lpInfo->m_strPropName = "杆件";
		lpInfo->m_strPropHelp = "杆件";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crPole);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crPole");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Angle X Leg";
		lpInfo->m_strPropHelp = "Angle X Leg";
#else 
		lpInfo->m_strPropName = "角钢X肢";
		lpInfo->m_strPropHelp = "角钢X肢";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crAngleWingX);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crAngleWingX");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Angle Y Leg";
		lpInfo->m_strPropHelp = "Angle Y Leg";
#else 
		lpInfo->m_strPropName = "角钢Y肢";
		lpInfo->m_strPropHelp = "角钢Y肢";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crAngleWingY);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crAngleWingY");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Angle Leg Edge";
		lpInfo->m_strPropHelp = "Angle Leg Edge";
#else 
		lpInfo->m_strPropName = "角钢肢边";
		lpInfo->m_strPropHelp = "角钢肢边";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crAngleEdge);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crAngleEdge");

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "(Not No.1)Child Angle X Leg";
		lpInfo->m_strPropHelp = "In addition to the angle NO.1 ,other child angle's X leg color";
#else 
		lpInfo->m_strPropName = "(非1号)子角钢X肢";
		lpInfo->m_strPropHelp = "除1号子角钢外,其余子角钢的X肢颜色";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1WingX);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crSonAngle1WingX");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "(Not No.1)Child Angle Y Leg";
		lpInfo->m_strPropHelp = "In addition to the angle NO.1 ,other child angle's Y leg color";
#else 
		lpInfo->m_strPropName = "(非1号)子角钢Y肢";
		lpInfo->m_strPropHelp = "除1号子角钢外,其余子角钢的Y肢颜色";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1WingY);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crSonAngle1WingY");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "(Not No.1)Child Angle Leg Edge";
		lpInfo->m_strPropHelp = "In addition to the angle NO.1 ,other child angle's leg edge color";
#else 
		lpInfo->m_strPropName = "(非1号)子角钢肢边";
		lpInfo->m_strPropHelp = "除1号子角钢外,其余子角钢的肢边颜色";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crSonAngle1Edge);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crSonAngle1Edge");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Other Bolts";
		lpInfo->m_strPropHelp = "Other Bolts";
#else 
		lpInfo->m_strPropName = "其他螺栓";
		lpInfo->m_strPropHelp = "其他螺栓";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crOtherBolt);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crOtherBolt");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Bolt M12";
		lpInfo->m_strPropHelp = "Bolt M12";
#else 
		lpInfo->m_strPropName = "螺栓M12";
		lpInfo->m_strPropHelp = "螺栓M12";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crBolt12);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crBolt12");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Bolt M16";
		lpInfo->m_strPropHelp = "Bolt M16";
#else 
		lpInfo->m_strPropName = "螺栓M16";
		lpInfo->m_strPropHelp = "螺栓M16";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crBolt16);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crBolt16");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Bolt M20";
		lpInfo->m_strPropHelp = "Bolt M20";
#else 
		lpInfo->m_strPropName = "螺栓M20";
		lpInfo->m_strPropHelp = "螺栓M20";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crBolt20);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crBolt20");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Bolt M24";
		lpInfo->m_strPropHelp = "Bolt M24";
#else 
		lpInfo->m_strPropName = "螺栓M24";
		lpInfo->m_strPropHelp = "螺栓M24";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crBolt24);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crBolt24");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Anti-theft Nut";
		lpInfo->m_strPropHelp = "Anti-theft Nut";
#else 
		lpInfo->m_strPropName = "防盗螺母";
		lpInfo->m_strPropHelp = "防盗螺母";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crAntiTheftNut);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crAntiTheftNut");

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Anti-loosen Nut";
		lpInfo->m_strPropHelp = "Anti-loosen Nut";
#else 
		lpInfo->m_strPropName = "防松螺母";
		lpInfo->m_strPropHelp = "防松螺母";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crAntiLoosenNut);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crAntiLoosenNut");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Connect Plate";
		lpInfo->m_strPropHelp = "Connect Plate";
#else 
		lpInfo->m_strPropName = "连接板";
		lpInfo->m_strPropHelp = "连接板";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.plateColRef.GetPlateColor("crPLATE"));
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crPlate");
		InsertPropItem(pPropItem);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Bend/Cut Section";
		lpInfo->m_strPropHelp = "Bend/Cut Section ";
#else 
		lpInfo->m_strPropName = "火曲/切割段";
		lpInfo->m_strPropHelp = "火曲/切割段";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crCutFaceOrPolyJoint);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crCutFaceOrPolyJoint");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Short Angle";
		lpInfo->m_strPropHelp = "Short Angle";
#else 
		lpInfo->m_strPropName = "短角钢";
		lpInfo->m_strPropHelp = "短角钢";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crShortAngle);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crShortAngle");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Main Backgroud";
		lpInfo->m_strPropHelp = "Main Backgroud";
#else 
		lpInfo->m_strPropName = "主背景";
		lpInfo->m_strPropHelp = "主背景";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crBackground);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crBackground");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Ancillary Background";
		lpInfo->m_strPropHelp = "Ancillary background";
#else 
		lpInfo->m_strPropName = "辅背景";
		lpInfo->m_strPropHelp = "辅背景";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crPartBackground);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crPartBackground");
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Standard Part";
		lpInfo->m_strPropHelp = "Standard Part";
#else 
		lpInfo->m_strPropName = "标准构件";
		lpInfo->m_strPropHelp = "标准构件";
#endif
		lpInfo->m_strPropValue.Format("RGB%X",g_sysPara.crPartMode.crStdPart);
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CColorSchemeDlg::GetPropID("crPartMode.crStdPart");
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Default Color";
	lpInfo->m_strPropHelp = "Default Color";
#else 
	lpInfo->m_strPropName = "默认配色";
	lpInfo->m_strPropHelp = "默认配色";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CColorSchemeDlg::GetPropID("defaultColor");
	//
	m_propList.Redraw();
}

long CColorSchemeDlg::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}
int CColorSchemeDlg::GetPropStr(long id, char *propStr)
{
	char sInfo[50]="";
	if(GetPropID("crPartMode.crPlate")==id)
		strcpy(sInfo,"crPartMode.crPlate");
	if(propStr)
		strcpy(propStr,sInfo);
	return strlen(sInfo);
}
BOOL CColorSchemeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//当前配色方案
	if(g_sysPara.iColorSchema<=0||g_sysPara.iColorSchema>3)
		g_sysPara.iColorSchema=3;
	m_iSchemeName=g_sysPara.iColorSchema-1;
	UpdateData(FALSE);
	//
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	InitPropHashtable();
	DisplayPropertyListPart();
	//
	CRect rect;
	CRect rectGrid;
	GetWindowRect(rect);
	GetDlgItem(IDC_LIST_EFFIC_COLOR_VARY)->GetWindowRect(&rectGrid);
	rect.right = rect.Width()-rectGrid.Width();
	rect.left = 0;
	rect.bottom = rect.Height();
	rect.top = 0;
	MoveWindow(rect,TRUE);
	m_listcolor.DeleteAllItems();
#ifdef AFX_TARG_ENU_ENGLISH
	m_listcolor.AddColumnHeader("NO.");
	m_listcolor.AddColumnHeader("Boundary Line Percentage");
	m_listcolor.AddColumnHeader("Color");
#else 
	m_listcolor.AddColumnHeader("序号");
	m_listcolor.AddColumnHeader("分界线百分数");
	m_listcolor.AddColumnHeader("颜色");
#endif
	long col_wide_arr[3]={70,90,90};
	m_listcolor.InitListCtrl(col_wide_arr);
	m_listcolor.SetModifyValueFunc(FireValueModify);
	return TRUE;  
}

BEGIN_MESSAGE_MAP(CColorSchemeDlg, CDialog)
	//{{AFX_MSG_MAP(CColorSchemeDlg)
	ON_CBN_SELCHANGE(IDC_SCHEME_NAME, OnSelchangeSchemeName)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_ORIGINAL_STATE, OnBnClickedOriginalState)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorSchemeDlg message handlers

void CColorSchemeDlg::OnSelchangeSchemeName() 
{
	UpdateData();
	COLORREF curBackColor = 0; 
	g_pSolidSet->GetBkColor(&curBackColor);
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(m_iSchemeName==0)
	{	//TSA配色方案
		g_sysPara.iColorSchema = 1;
		if (curBackColor != g_sysPara.crTsaMode.crBackground)
			g_pSolidSet->SetBkColor(g_sysPara.crTsaMode.crBackground);
		g_pSolidSet->SetDisplayType(DISP_LINE);
	}
	else if (m_iSchemeName==1)
	{	//材质配色方案
		g_sysPara.iColorSchema = 2;
		if (curBackColor != g_sysPara.crPartMode.crBackground)
			g_pSolidSet->SetBkColor(g_sysPara.crPartMode.crBackground);
	}
	else //if(m_iSchemeName==2)
	{	//构件配色方案
		g_sysPara.iColorSchema = 3;
		if (curBackColor != g_sysPara.crMaterialMode.crBackground)
			g_pSolidSet->SetBkColor(g_sysPara.crMaterialMode.crBackground);
		g_pSolidSet->SetDisplayType(DISP_SOLID);
	}
	UpdateData(FALSE);
	DisplayPropertyListPart();
	pView->RedrawAll();
}

void CColorSchemeDlg::OnOK() 
{
	
}

void CColorSchemeDlg::OnClose() 
{
	UpdateData();
	g_sysPara.iColorSchema=m_iSchemeName+1;
	CDialog::OnOK();
}

void CColorSchemeDlg::OnBnClickedOriginalState()
{
	// TODO: 在此添加控件通知处理程序代码
	g_sysPara.crTsaMode.crElemDivArr[0].efficPercent=60;
	g_sysPara.crTsaMode.crElemDivArr[0].crElemDivision=RGB(0,128,255);
	g_sysPara.crTsaMode.crElemDivArr[1].efficPercent=80;
	g_sysPara.crTsaMode.crElemDivArr[1].crElemDivision=RGB(0,255,0);
	g_sysPara.crTsaMode.crElemDivArr[2].efficPercent=90;
	g_sysPara.crTsaMode.crElemDivArr[2].crElemDivision=RGB(200,250,140);
	g_sysPara.crTsaMode.crElemDivArr[3].efficPercent=95;
	g_sysPara.crTsaMode.crElemDivArr[3].crElemDivision=RGB(255,255,0);
	g_sysPara.crTsaMode.crElemDivArr[4].efficPercent=100;
	g_sysPara.crTsaMode.crElemDivArr[4].crElemDivision=RGB(255,145,0);
	g_sysPara.crTsaMode.crElemDivArr[5].efficPercent=255;	//>100终止不显示,为无效分界
	g_sysPara.crTsaMode.crElemDivArr[5].crElemDivision=RGB(0,255,0);
	//OnInitDialog();
	Refresh(true);
}
