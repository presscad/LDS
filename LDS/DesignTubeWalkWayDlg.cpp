#include "stdafx.h"
#include "Lds.h"
#include "GlobalFunc.h"
#include "PropertyListOper.h"
#include "DesignTubeWalkWayDlg.h"
#include "btc.h"
//////////////////////////////////////////////////////////////////////////
//回调函数处理
BOOL HelpPromptProperty(CPropertyList *pPropList,CPropTreeItem* pItem)
{
	CDesignTubeWalkWayDlg* pDlg=(CDesignTubeWalkWayDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	g_pSolidDraw->ReleaseSnapStatus();
	if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("BaseTube"))
		g_pSolidDraw->SetEntSnapStatus(pDlg->m_hBaseTube);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("BaseNode"))
		g_pSolidDraw->SetEntSnapStatus(pDlg->m_hBaseNode);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("LinkAngle1"))
		g_pSolidDraw->SetEntSnapStatus(pDlg->m_hOuterAngle);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("LinkAngle2"))
		g_pSolidDraw->SetEntSnapStatus(pDlg->m_hInnerAngle);
	return TRUE;
}
BOOL ModifyProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CDesignTubeWalkWayDlg* pDlg=(CDesignTubeWalkWayDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	CPropertyListOper<CDesignTubeWalkWayDlg> oper(pPropList,pDlg);
	if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("WalkWayType"))
	{
		pDlg->m_ciWalkWayType=tem_str[0]-'0';
		pDlg->UpdateHolderProp(pItem);
		pDlg->DisplaySubPropList();
		pDlg->UpdateSketchMap();
	}
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("YA"))
		pDlg->m_fYA=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("ZH"))
		pDlg->m_fZH=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("CreateWalkWay"))
	{
		if(valueStr.CompareNoCase("是")==0)
			pDlg->m_bCreateWalkWay=TRUE;
		else
			pDlg->m_bCreateWalkWay=FALSE;
		//
		pDlg->UpdateWalkWayProp(pItem);
		CPropTreeItem* pFindItem=pPropList->FindItemByPropId(CDesignTubeWalkWayDlg::GetPropID("AssemblyInfo"),NULL);
		pDlg->UpdateAssemblyProp(pFindItem);		
	}
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("WalkWayWidth"))
		pDlg->m_fWidth=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("WalkWayLength"))
		pDlg->m_fLength=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("WalkWayJgSpec"))
		pDlg->m_sWalkWayJgSpec.Copy(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("WalkWayJgMat"))
		pDlg->m_cWalkWayJgMat=QuerySteelBriefMatMark(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("HolderJgSpec"))
		pDlg->m_sHolderJgSpec.Copy(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("HolderJgMat"))
		pDlg->m_cHolderJgMat=QuerySteelBriefMatMark(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("ciHolderAngleLayoutStyle"))
		pDlg->m_ciHolderAngleLayoutStyle=tem_str[0]-'0';
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("PlateThick"))
		pDlg->m_nThick=atoi(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("PlateMat"))
		pDlg->m_cPlateMat=QuerySteelBriefMatMark(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("BoltMd"))
		pDlg->m_nBoltMd=atoi(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("BoltSpace"))
		pDlg->m_fBoltSpace=atof(tem_str)*2;
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("BoltNum"))
		pDlg->m_nBoltNum=atoi(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("para.A"))
		pDlg->para.A=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("para.B"))
		pDlg->para.B=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("para.W"))
		pDlg->para.W=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("para.H"))
		pDlg->para.H=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("Zh"))
		pDlg->m_fZh=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("D1"))
		pDlg->m_fD1=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("D2"))
		pDlg->m_fD2=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("YB"))
		pDlg->m_fYB=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("OffDistX+"))
		pDlg->m_fZjOffDist=atof(tem_str);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("OffDistX-"))
		pDlg->m_fZdOffDist=atof(tem_str);
	return TRUE;
}
BOOL ButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDesignTubeWalkWayDlg* pDlg=(CDesignTubeWalkWayDlg*)pPropList->GetParent();
	if(pDlg==NULL)
		return FALSE;
	if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("BaseTube"))
		pDlg->SelectObject(CLS_LINETUBE,pItem->m_idProp);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("BaseNode"))
		pDlg->SelectObject(CLS_NODE,pItem->m_idProp);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("LinkAngle1"))
		pDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);
	else if(pItem->m_idProp==CDesignTubeWalkWayDlg::GetPropID("LinkAngle2"))
		pDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// CDesWireNodeDlg 对话框
IMPLEMENT_DYNAMIC(CDesignTubeWalkWayDlg, CDialogEx)
int CDesignTubeWalkWayDlg::nScrLocationX=0;
int CDesignTubeWalkWayDlg::nScrLocationY=0;
CDesignTubeWalkWayDlg::CDesignTubeWalkWayDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignTubeWalkWayDlg::IDD, pParent)
{
	m_ciWalkWayType=1;
	m_bCreateWalkWay=TRUE;
	m_fYA=600;
	m_fZH=300;
	m_fZh=300;
	m_fYB=200;
	m_fWidth=400;
	m_fLength=4000;
	m_nBoltMd=16;
	m_nBoltNum=2;
	m_fBoltSpace=80;
	m_fD1=40;
	m_fD2=30;
	m_sWalkWayJgSpec.Copy("100x6");
	m_sHolderJgSpec.Copy("70x5");
	m_cWalkWayJgMat='H';
	m_cHolderJgMat='H';
	m_ciHolderAngleLayoutStyle=0;	//相对当前视向默认支架角钢为外铁
	m_cPlateMat='H';
	m_nThick=8;
	para.A=90;
	para.B=180;
	para.H=60;
	para.W=30;
	//
	m_pBaseTube=NULL;
	m_pBaseNode=NULL;
	m_pOuterJg=m_pInnerJg=NULL;
	m_hBaseTube=m_hBaseNode=m_hOuterAngle=m_hInnerAngle=0;
	m_hOuterBeamDatumNode=m_hInnerBeamDatumNode=0;
	m_fZdOffDist=700;
	m_fZjOffDist=400;
}

CDesignTubeWalkWayDlg::~CDesignTubeWalkWayDlg()
{
}

void CDesignTubeWalkWayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_LIST_BOX_SUB,m_propSubList);
}


BEGIN_MESSAGE_MAP(CDesignTubeWalkWayDlg, CDialog)
	ON_WM_MOVE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CDesWireNodeDlg 消息处理程序
BOOL CDesignTubeWalkWayDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitPropHashtable();
	if(m_bInernalStart)
	{	//内部启动
		FinishSelectObjOper(m_hPickObj,GetEventPropId());
		DisplayPropList();
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();	//属性栏获得焦点
		if(pItem)
			m_propList.SetCurSel(pItem->m_iIndex);	//设置选中项
	}
	else
	{
		long hInnerJg=(m_pInnerJg)?m_pInnerJg->handle:0;
		long hOuterJg=(m_pOuterJg)?m_pOuterJg->handle:0;
		m_pInnerJg=(CLDSLineAngle*)Ta.FromPartHandle(hInnerJg,CLS_LINEANGLE);
		m_pOuterJg=(CLDSLineAngle*)Ta.FromPartHandle(hOuterJg,CLS_LINEANGLE);
		if(m_pInnerJg && m_pOuterJg)
			m_bCreateWalkWay=FALSE;
		else
			m_bCreateWalkWay=TRUE;
		m_xWalkWayTempl.Empty();
		m_propList.SetDividerScale(0.5);
		m_propSubList.SetDividerScale(0.5);
		m_propList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
		m_propSubList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
		DisplayPropList();
	}
	DisplaySubPropList();
	UpdateSketchMap();
	WorkPlaneSketch();
	//移动对话框到左上角
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
	UpdateData(FALSE);
	return TRUE;
}
int CDesignTubeWalkWayDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=FALSE;
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
void CDesignTubeWalkWayDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);
	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
const DWORD HASHTABLESIZE = 500;
IMPLEMENT_PROP_FUNC(CDesignTubeWalkWayDlg);
void CDesignTubeWalkWayDlg::InitPropHashtable()
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);
	//模板信息
	AddPropItem("BasicInfo",PROPLIST_ITEM(id++,"走道参数"));
	AddPropItem("YA",PROPLIST_ITEM(id++,"横向偏移YA","走道外侧角钢距钢管的横向偏移量"));
	AddPropItem("ZH",PROPLIST_ITEM(id++,"竖向偏移ZH","走道外侧角钢距钢管的竖向偏移量"));
	AddPropItem("CreateWalkWay",PROPLIST_ITEM(id++,"生成走道主梁","","是|否"));
	AddPropItem("WalkWayWidth",PROPLIST_ITEM(id++,"走道宽度YW"));
	AddPropItem("WalkWayLength",PROPLIST_ITEM(id++,"走道长度L"));
	AddPropItem("WalkWayJgSpec",PROPLIST_ITEM(id++,"角钢规格"));
	AddPropItem("WalkWayJgMat",PROPLIST_ITEM(id++,"角钢材质"));
	//支架参数
	AddPropItem("WalkWayType",PROPLIST_ITEM(id++,"走道支架类型","","0.钢板支架式|1.三角支架式|2.台型支架式"));
	AddPropItem("HolderJgSpec",PROPLIST_ITEM(id++,"角钢规格"));
	AddPropItem("HolderJgMat",PROPLIST_ITEM(id++,"角钢材质"));
	AddPropItem("ciHolderAngleLayoutStyle",PROPLIST_ITEM(id++,"肢朝向","","0.朝外|1.朝内"));
	AddPropItem("BoltMd", PROPLIST_ITEM(id++,"螺栓直径 M"));
	AddPropItem("BoltNum",PROPLIST_ITEM(id++,"螺栓数量"));
	AddPropItem("BoltSpace",PROPLIST_ITEM(id++,"螺栓间距N"));
	AddPropItem("Zh",PROPLIST_ITEM(id++,"偏移距离Zh","斜撑下侧竖向偏移"));
	AddPropItem("D1",PROPLIST_ITEM(id++,"偏移距离D1","斜撑下侧横向偏移"));
	AddPropItem("D2",PROPLIST_ITEM(id++,"偏移距离D2","斜撑上侧端点沿水平杆向内的横向偏移"));
	AddPropItem("YB",PROPLIST_ITEM(id++,"偏移距离YB","竖撑的横向偏移距"));
	//装配信息
	AddPropItem("AssemblyInfo",PROPLIST_ITEM(id++,"装配信息"));
	AddPropItem("BaseTube",PROPLIST_ITEM(id++,"基准钢管"));
	AddPropItem("BaseNode",PROPLIST_ITEM(id++,"基准节点"));
	AddPropItem("LinkAngle1",PROPLIST_ITEM(id++,"外侧角钢","走道外侧角钢"));
	AddPropItem("LinkAngle2",PROPLIST_ITEM(id++,"内侧角钢","走道内侧角钢"));
	AddPropItem("OffDistX+",PROPLIST_ITEM(id++,"轴向偏移X0","支架设计点自基准点沿走道纵向的偏移距离"));
	AddPropItem("OffDistX-",PROPLIST_ITEM(id++,"走道始端X1","走道角钢起始点距起始支架的距离"));
	//走道板参数
	AddPropItem("PlateInfo",PROPLIST_ITEM(id++,"走道板信息"));
	AddPropItem("PlateThick",PROPLIST_ITEM(id++,"钢板厚度"));
	AddPropItem("PlateMat",PROPLIST_ITEM(id++,"钢板材质"));
	AddPropItem("para.A",PROPLIST_ITEM(id++,"扇形夹角B","走道焊接板的扇形夹角"));
	AddPropItem("para.B",PROPLIST_ITEM(id++,"扇形夹角B","螺栓连接板的扇形夹角"));
	AddPropItem("para.W",PROPLIST_ITEM(id++,"环形带宽W"));
	AddPropItem("para.H",PROPLIST_ITEM(id++,"台口高度H"));
}
int CDesignTubeWalkWayDlg::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)	//根据属性ID得到属性值字符串
{
	CXhChar100 sText;
	if(GetPropID("WalkWayType")==id)
	{
		if(m_ciWalkWayType==0)
			sText.Copy("0.钢板支架式");
		else if(m_ciWalkWayType==1)
			sText.Copy("1.三角支架式");
		else
			sText.Copy("2.台型支架式");
	}
	else if(GetPropID("CreateWalkWay")==id)
	{
		if(m_bCreateWalkWay)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("WalkWayWidth")==id)
	{
		sText.Printf("%f",m_fWidth);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("WalkWayLength")==id)
	{
		sText.Printf("%f",m_fLength);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("WalkWayJgSpec")==id)
		sText.Printf("%s",(char*)m_sWalkWayJgSpec);
	else if(GetPropID("WalkWayJgMat")==id)
		QuerySteelMatMark(m_cWalkWayJgMat,sText);
	else if(GetPropID("YA")==id)
	{
		sText.Printf("%f",m_fYA);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ZH")==id)
	{
		sText.Printf("%f",m_fZH);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("BoltSpace")==id)
	{
		sText.Printf("%g",m_fBoltSpace*0.5);
		//SimplifiedNumString(sText);
	}
	else if(GetPropID("BoltMd")==id)
		sText.Printf("%d",m_nBoltMd);
	else if(GetPropID("BoltNum")==id)
		sText.Printf("%d",m_nBoltNum);
	else if(GetPropID("HolderJgSpec")==id)
		sText.Printf("%s",(char*)m_sHolderJgSpec);
	else if(GetPropID("HolderJgMat")==id)
		QuerySteelMatMark(m_cHolderJgMat,sText);
	else if(GetPropID("ciHolderAngleLayoutStyle")==id)
	{
		if(m_ciHolderAngleLayoutStyle!=0)
			sText="1.朝内";
		else
			sText="0.朝外";
	}
	else if(GetPropID("PlateThick")==id)
		sText.Printf("%d",m_nThick);
	else if(GetPropID("PlateMat")==id)
		QuerySteelMatMark(m_cPlateMat,sText);
	else if(GetPropID("BaseTube")==id)
	{
		long hTube=m_pBaseTube?m_pBaseTube->handle:0;
		sText.Printf("0X%X",hTube);
	}
	else if(GetPropID("BaseNode")==id)
	{
		long hNode=m_pBaseNode?m_pBaseNode->handle:0;
		sText.Printf("0X%X",hNode);
	}
	else if(GetPropID("LinkAngle1")==id)
	{
		long hJg=m_pOuterJg?m_pOuterJg->handle:0;
		sText.Printf("0X%X",hJg);
	}
	else if(GetPropID("LinkAngle2")==id)
	{
		long hJg=m_pInnerJg?m_pInnerJg->handle:0;
		sText.Printf("0X%X",hJg);
	}
	else if(GetPropID("OffDistX+")==id)
	{
		sText.Printf("%f",m_fZjOffDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("OffDistX-")==id)
	{
		sText.Printf("%f",m_fZdOffDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.A")==id)
	{
		sText.Printf("%f",para.A);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.B")==id)
	{
		sText.Printf("%f",para.B);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("Zh")==id)
	{
		sText.Printf("%f",m_fZh);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("D1")==id)
	{
		sText.Printf("%f",m_fD1);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("D2")==id)
	{
		sText.Printf("%f",m_fD2);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("YB")==id)
	{
		sText.Printf("%f",m_fYB);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.W")==id)
	{
		sText.Printf("%f",para.W);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.H")==id)
	{
		sText.Printf("%f",para.H);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
void CDesignTubeWalkWayDlg::DisplayPropList()
{
	CPropertyListOper<CDesignTubeWalkWayDlg> oper(&m_propList,this);
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pRoot=m_propList.GetRootItem();
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(ModifyProperty);
	m_propList.SetButtonClickFunc(ButtonClick);
	m_propList.SetPropHelpPromptFunc(HelpPromptProperty);
	//走道参数	
	pGroupItem=oper.InsertPropItem(pRoot,"BasicInfo");
	oper.InsertEditPropItem(pGroupItem,"OffDistX+");
	oper.InsertEditPropItem(pGroupItem,"YA");
	oper.InsertEditPropItem(pGroupItem,"ZH");
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"CreateWalkWay");
	UpdateWalkWayProp(pPropItem);
	//支架参数
	pGroupItem=oper.InsertCmbListPropItem(pRoot,"WalkWayType");	//走道类型
	UpdateHolderProp(pGroupItem);
	//装配参数
	pGroupItem=oper.InsertGrayPopMenuItem(pRoot,"AssemblyInfo");
	UpdateAssemblyProp(pGroupItem);
	//
	m_propList.Redraw();
}
void CDesignTubeWalkWayDlg::DisplaySubPropList()
{
	CPropertyListOper<CDesignTubeWalkWayDlg> oper(&m_propSubList,this);
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pRoot=m_propSubList.GetRootItem();
	m_propSubList.CleanTree();
	m_propSubList.SetModifyValueFunc(ModifyProperty);
	//
	pGroupItem=oper.InsertPropItem(pRoot,"PlateInfo");
	oper.InsertEditPropItem(pGroupItem,"PlateThick");
	oper.InsertCmbListPropItem(pGroupItem,"PlateMat",MakeMaterialMarkSetString());
	if(m_ciWalkWayType==0)
		oper.InsertEditPropItem(pGroupItem,"para.A");
	else
	{
		oper.InsertEditPropItem(pGroupItem,"para.B");
		oper.InsertEditPropItem(pGroupItem,"para.W");
		//oper.InsertEditPropItem(pGroupItem,"para.H");
		//
		oper.InsertCmbEditPropItem(pGroupItem,"BoltMd","12|16|20|24","","",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"BoltNum","","",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"BoltSpace","","",-1,TRUE);
	}
	//
	m_propSubList.Redraw();
}
void CDesignTubeWalkWayDlg::UpdateAssemblyProp(CPropTreeItem* pParentItem)
{
	CPropertyListOper<CDesignTubeWalkWayDlg> oper(&m_propList,this);
	m_propList.DeleteAllSonItems(pParentItem);
	//
	oper.InsertButtonPropItem(pParentItem,"BaseTube","","",-1,TRUE);
	oper.InsertButtonPropItem(pParentItem,"BaseNode","","",-1,TRUE);
	if(!m_bCreateWalkWay)
	{
		oper.InsertButtonPropItem(pParentItem,"LinkAngle1","","",-1,TRUE);
		oper.InsertButtonPropItem(pParentItem,"LinkAngle2","","",-1,TRUE);
	}
}
void CDesignTubeWalkWayDlg::UpdateWalkWayProp(CPropTreeItem* pParentItem)
{
	CPropertyListOper<CDesignTubeWalkWayDlg> oper(&m_propList,this);
	m_propList.DeleteAllSonItems(pParentItem);
	if(m_bCreateWalkWay)
	{
		oper.InsertEditPropItem(pParentItem,"OffDistX-","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"WalkWayWidth","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"WalkWayLength","","",-1,TRUE);
		oper.InsertCmbEditPropItem(pParentItem,"WalkWayJgSpec",MakeAngleSetString(),"","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"WalkWayJgMat",MakeMaterialMarkSetString(),"","",-1,TRUE);
	}
}
void CDesignTubeWalkWayDlg::UpdateHolderProp(CPropTreeItem* pParentItem)
{
	CPropertyListOper<CDesignTubeWalkWayDlg> oper(&m_propList,this);
	m_propList.DeleteAllSonItems(pParentItem);
	if(m_ciWalkWayType!=0)
	{
		oper.InsertEditPropItem(pParentItem,"Zh","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"D1","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"D2","","",-1,TRUE);
		if(m_ciWalkWayType==2)
			oper.InsertEditPropItem(pParentItem,"YB","","",-1,TRUE);
		oper.InsertCmbEditPropItem(pParentItem,"HolderJgSpec",MakeAngleSetString(),"","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"HolderJgMat",MakeMaterialMarkSetString(),"","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"ciHolderAngleLayoutStyle","0.朝外|1.朝内","","",-1,TRUE);
	}
	else
	{
		oper.InsertCmbEditPropItem(pParentItem,"BoltMd","12|16|20|24","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"BoltNum","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"BoltSpace","","",-1,TRUE);
	}
}
BOOL CDesignTubeWalkWayDlg::FinishSelectObjOper(long hPickObj,long idEventProp)
{
	CLDSNode* pNode;
	CLDSPart* pPart;
	if(idEventProp==CDesignTubeWalkWayDlg::GetPropID("BaseNode"))
	{
		if((pNode=console.FromNodeHandle(hPickObj))!=NULL)
			m_pBaseNode=pNode;
	}
	else if(idEventProp==CDesignTubeWalkWayDlg::GetPropID("BaseTube"))
	{
		if((pPart=console.FromPartHandle(hPickObj))!=NULL)
			m_pBaseTube=(CLDSLineTube*)pPart;
	}
	else if(idEventProp==CDesignTubeWalkWayDlg::GetPropID("LinkAngle1"))
		m_pOuterJg=(CLDSLineAngle*)console.FromPartHandle(hPickObj);
	else if(idEventProp==CDesignTubeWalkWayDlg::GetPropID("LinkAngle2"))
		m_pInnerJg=(CLDSLineAngle*)console.FromPartHandle(hPickObj);
	return TRUE;
}
void CDesignTubeWalkWayDlg::UpdateSketchMap()
{
	m_xWalkWayBmp.Detach();
	if(m_ciWalkWayType==0)
	{	//钢板支架式
		m_xWalkWayBmp.LoadBitmap(IDB_BMP_WALKWAY1);
	}
	else if(m_ciWalkWayType==1)
	{	//三角支架式
		m_xWalkWayBmp.LoadBitmap(IDB_BMP_WALKWAY2);
	}
	else if(m_ciWalkWayType==2)
	{	//台型支教式
		m_xWalkWayBmp.LoadBitmap(IDB_BMP_WALKWAY3);
	}
	//
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_MAP);
	pPic->SetBitmap(m_xWalkWayBmp);
}
GEPOINT CDesignTubeWalkWayDlg::CalNearVecX()
{
	GEPOINT nearVecX(1,0,0);
	if(m_pBaseTube==NULL)
		return nearVecX;
	GEPOINT ptS=m_pBaseTube->xPosStart,ptE=m_pBaseTube->xPosEnd;
	GEPOINT line_vec=(m_pBaseTube->End()-m_pBaseTube->Start()).normalized();
	if(fabs(line_vec.x)>fabs(line_vec.y))
	{	//X轴负向走道
		if(ptS.x>0&&ptE.x>0)		//右侧横担
			nearVecX.Set(1,0,0);
		else if(ptS.x<0&&ptE.x<0)	//左侧横担
			nearVecX.Set(-1,0,0);
		else if((ptS.x<0&&ptE.x>0)||(ptS.x>0&&ptE.x<0))	//塔身横隔面
			nearVecX=(ptS.y>0)?GEPOINT(1,0,0):GEPOINT(-1,0,0);
	}
	else
	{	//Y轴负向走道
		if(ptS.y>0&&ptE.y>0)		//前侧横担
			nearVecX.Set(0,1,0);
		else if(ptS.y<0&&ptE.y<0)	//后侧横担
			nearVecX.Set(0,-1,0);
		else if((ptS.y<0&&ptE.y>0)||(ptS.y>0&&ptE.y<0))	//塔身横隔面
			nearVecX=(ptS.x>0)?GEPOINT(0,-1,0):GEPOINT(0,1,0);
	}
	return nearVecX;
}
void CDesignTubeWalkWayDlg::WorkPlaneSketch()
{
	if(m_pBaseNode==NULL || m_pBaseTube==NULL)
		return;
	DelWorkPlaneSketch();
	m_pBaseTube->BuildUCS();
	GEPOINT base_pos=m_pBaseNode->xOriginalPos;
	GEPOINT line_vec=CalNearVecX();
	if(line_vec*m_pBaseTube->ucs.axis_z>0)
		line_vec=m_pBaseTube->ucs.axis_z;
	else
		line_vec=m_pBaseTube->ucs.axis_z*(-1);
	int nLen=ftoi(m_pBaseTube->GetDiameter()*2);
	btc::SKETCH_PLANE sketch1(base_pos,line_vec,m_pBaseTube->ucs.axis_x,nLen);
	g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),sketch1.pVertexArr,sketch1.VertexCount,line_vec,"X+",200);
	g_pSolidDraw->NewWorkPlaneSketch(2,RGB(152,152,152),&base_pos,1,-1*line_vec,"X-",200);
	g_pSolidDraw->Draw();
}
void CDesignTubeWalkWayDlg::DelWorkPlaneSketch()
{
	g_pSolidDraw->DelWorkPlaneSketch(1);
	g_pSolidDraw->DelWorkPlaneSketch(2);
	g_pSolidDraw->Draw();
}
//初始化走道模板的设计参数
void CDesignTubeWalkWayDlg::InitTemplParams()
{
	//走道设计参数
	STDTEMPL_PARAM_ITEM* pParam=NULL;
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("W"));	//走道宽度
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=this->m_fWidth;
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("L"));	//走道长度
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=this->m_fLength;
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("YA"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=m_fYA;	//横向偏移
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("ZH"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=m_fZH;	//竖向高度偏移
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("Ls"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=(m_fZjOffDist-m_fZdOffDist);	//走道始端轴向偏移距离
	//支架设计参数
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("Zh"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=m_fZh;	//斜撑下侧Z向偏移
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("D1"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=m_fD1;	//斜撑下侧横向偏移
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("D2"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=m_fD2;	//斜撑上侧端点沿水平杆向内的横向偏移
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("YB"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=m_fYB;	//支架位置轴向偏移距离
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("X0"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=m_fZjOffDist;	//支架位置轴向偏移距离
	//
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("Md"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;
	pParam->value.liValue=this->m_nBoltMd;	//钢管上连接板上侧横螺直径
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("BN"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=this->m_nBoltNum;	//钢管上连接板上侧横螺栓数
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("BS"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=this->m_fBoltSpace;//钢管上连接板上侧横螺栓间距
	//走道板设计参数
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("PA"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=para.A;	//走道焊接板扇形夹角
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("PB"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=para.B;	//螺栓连接板扇形夹角
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("PW"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=para.W;	//
	//装配坐标系
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("acsO"));	//标准组件装配坐标系原点
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_COORD3D;
	pParam->value.objtype.wiPropValue=0;
	//strcpy(pParam->exprstr,"Node_Pos(1)");
	strcpy(pParam->exprstr,"RodNode_Pos(1,1)");	//改为杆件上节点定位可有效避免端头为槽型插板时的偏移量问题 wjh-2018.10.8
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("acsX"));	//标准组件装配坐标系X轴方向（未指定终点方向时，根据起始基准点由中心向外延伸）
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_COORD3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"Rod_LenV(1)");
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("acsY"));	//标准组件装配坐标系Y轴方向
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_COORD3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"auto");	//自动根据X轴及基准钢管的位置计算
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("acsZ"));	//标准组件装配坐标系Z轴方向
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//三维点或矢量类型EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_COORD3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"auto");	//自动根据X轴计算(保证向上即与模型坐标系的-Z方向一致)
}
static bool UnifyParaAngleLayoutByAntiAxisX(CStdTemplParaRod* pParaRod,bool bToOutward=true)
{
	if(!pParaRod->IsAngleObject())
		return false;
	CStdTemplParaAngle* pParaAngle=(CStdTemplParaAngle*)pParaRod;
	if(pParaAngle->ciNormalDirection=='Z')
	{
		bool reverseXnY=true;
		if(pParaAngle->ciAngleLayout==1&&bToOutward)
			pParaAngle->ciAngleLayout=4;
		else if(pParaAngle->ciAngleLayout==2&&bToOutward)
			pParaAngle->ciAngleLayout=3;
		else if(pParaAngle->ciAngleLayout==4&&!bToOutward)
			pParaAngle->ciAngleLayout=1;
		else if(pParaAngle->ciAngleLayout==3&&!bToOutward)
			pParaAngle->ciAngleLayout=2;
		else
			reverseXnY=false;
		if( pParaRod->keyLabel==CTubeWalkWayStdComTempl::BRACE_UPPER_HORI_ANGLE_ID||
			pParaRod->keyLabel==CTubeWalkWayStdComTempl::BRACE_DOWN_HORI_ANGLE_ID)
		{
			if(!bToOutward&&pParaAngle->ciAngleLayout==2)
			{
				//pParaAngle->desStart.cTouchLineType=0;
				//pParaAngle->desStart.cTouchLine=1;	//交于走道外主梁角钢Y肢心线
				//pParaAngle->desEnd.cTouchLineType=3;//控制点
				//pParaAngle->desStart.coordCtrlType=pParaJg1->desEnd.coordCtrlType=3;	//控制点投影
				//pParaAngle->desStart.szXorCtrlWingOffset[0]='0';//'g';
				//pParaAngle->desStart.szYorOthrWingOffset[0]='0';
				pParaAngle->desEnd.szXorCtrlWingOffset[0]='0';//'g';
				pParaAngle->desEnd.szYorOthrWingOffset[0]='g';//'0';
			}
			else if(!bToOutward&&pParaAngle->ciAngleLayout==1)
			{
				//pParaAngle->desStart.cTouchLineType=pParaAngle->desEnd.cTouchLineType=3;//控制点
				//pParaAngle->desStart.coordCtrlType=pParaAngle->desEnd.coordCtrlType=3;	//控制点投影
				pParaAngle->desStart.szXorCtrlWingOffset[0]='0';//'g';
				pParaAngle->desStart.szYorOthrWingOffset[0]='g';//'0';
				pParaAngle->desEnd.szXorCtrlWingOffset[0]='0';//'g';
				pParaAngle->desEnd.szYorOthrWingOffset[0]='g';//'0';
			}
			else if(bToOutward&&pParaAngle->ciAngleLayout==3)
			{
				//pParaAngle->desStart.cTouchLineType=0;
				//pParaAngle->desStart.cTouchLine=1;	//交于走道外主梁角钢Y肢心线
				//pParaAngle->desEnd.cTouchLineType=3;//控制点
				//pParaAngle->desStart.coordCtrlType=pParaJg1->desEnd.coordCtrlType=3;	//控制点投影
				pParaAngle->desStart.szXorCtrlWingOffset[0]='0';//'g';
				pParaAngle->desStart.szYorOthrWingOffset[0]='0';
				pParaAngle->desEnd.szXorCtrlWingOffset[0]='g';
				pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
			}
			else if(bToOutward&&pParaAngle->ciAngleLayout==4)
			{
				//pParaAngle->desStart.cTouchLineType=pParaJg3->desEnd.cTouchLineType=3;//控制点
				//pParaAngle->desStart.coordCtrlType=pParaJg3->desEnd.coordCtrlType=3;	//控制点投影
				pParaAngle->desStart.szXorCtrlWingOffset[0]='g';
				pParaAngle->desStart.szYorOthrWingOffset[0]='0';
				pParaAngle->desEnd.szXorCtrlWingOffset[0]='g';
				pParaAngle->desEnd.szYorOthrWingOffset[0]='0';
			}
			for(WORD i=0;reverseXnY&&i<pParaAngle->m_layoutBoltArr.Count;i++)
			{
				BOLT_LAYOUT* pLayout=pParaAngle->m_layoutBoltArr.GetAt(i);
				pLayout->ciDatumWing='X'+('Y'-pLayout->ciDatumWing);
			}
		}
	}
	else if(pParaAngle->ciNormalDirection=='x')
	{
		if(pParaAngle->ciAngleLayout==2&&bToOutward)
			pParaAngle->ciAngleLayout=1;
		else if(pParaAngle->ciAngleLayout==3&&bToOutward)
			pParaAngle->ciAngleLayout=4;
		else if(pParaAngle->ciAngleLayout==1&&!bToOutward)
			pParaAngle->ciAngleLayout=2;
		else if(pParaAngle->ciAngleLayout==4&&!bToOutward)
			pParaAngle->ciAngleLayout=3;
		if(pParaRod->keyLabel==CTubeWalkWayStdComTempl::BRACE_GRADIENT_ANGLE_ID)
		{
			if(bToOutward&&pParaAngle->ciAngleLayout==1)
			{
				pParaAngle->desStart.cTouchLineType=0;
				pParaAngle->desStart.cTouchLine=1;
				pParaAngle->desStart.coordCtrlType=3;
				pParaAngle->desStart.szXorCtrlWingOffset="-R4#T";//"g";
				pParaAngle->desStart.szYorOthrWingOffset="g";//"-g,-P2#T";
				pParaAngle->desEnd.cTouchLineType=3;	//控制点
				pParaAngle->desEnd.coordCtrlType=3;	//控制点投影
				pParaAngle->desEnd.szXorCtrlWingOffset="g";
				pParaAngle->desEnd.szYorOthrWingOffset="g";//"-g,-P2#T";
			}
			else if(!bToOutward&&pParaAngle->ciAngleLayout==2)
			{
				pParaAngle->desStart.cTouchLineType=0;
				pParaAngle->desStart.cTouchLine=0;
				pParaAngle->desStart.coordCtrlType=3;
				pParaAngle->desStart.szYorOthrWingOffset="-R4#T";//"g";
				pParaAngle->desStart.szXorCtrlWingOffset="g";//"-g,-P2#T";
				pParaAngle->desEnd.cTouchLineType=3;	//控制点
				pParaAngle->desEnd.coordCtrlType=3;	//控制点投影
				pParaAngle->desEnd.szXorCtrlWingOffset="g";
				pParaAngle->desEnd.szYorOthrWingOffset="g";//"-g,-P2#T";
			}
		}
		for(WORD i=0;i<pParaAngle->m_layoutBoltArr.Count;i++)
		{
			BOLT_LAYOUT* pLayout=pParaAngle->m_layoutBoltArr.GetAt(i);
			if(pParaAngle->ciAngleLayout==1||pParaAngle->ciAngleLayout==3)
				pLayout->ciDatumWing='X';
			else if(pParaAngle->ciAngleLayout==2||pParaAngle->ciAngleLayout==4)
				pLayout->ciDatumWing='Y';
		}
	}
	else
		return false;
	return true;
}
BOOL CDesignTubeWalkWayDlg::CreateTubeWalkWay()
{
	if(m_pBaseTube==NULL||m_pBaseNode==NULL)
	{
		AfxMessageBox("依赖钢管或依赖节点不能为空!");
		return FALSE;
	}
	if(m_pBaseNode->hFatherPart!=m_pBaseTube->handle)
	{
		AfxMessageBox("基准节点不在依赖钢管上!");
		return FALSE;
	}
	if(!m_bCreateWalkWay&&(m_pOuterJg==NULL||m_pInnerJg==NULL))
	{
		AfxMessageBox("支架需要有连接角钢！");
		return FALSE;
	}
	f3dPoint ptS=m_pBaseTube->Start(),ptE=m_pBaseTube->End();
	f3dPoint line_vec=(ptE-ptS).normalized();
	if(fabs(line_vec*f3dPoint(1,0,0))<0.707)
	{
		AfxMessageBox("设计非水平横担!");
		return FALSE;
	}
	//加载相应的走道及其支架组件模板
	IStdComTempl::TEMPL_SOURCE source(0,1,m_ciWalkWayType);
	m_xWalkWayTempl.InitComponentTempl(&source);
	//根据对话框界面设定模板在模型环境中的上下文参数
	InitTemplParams();
	//设定模板角钢及钢板的基本信息
	CStdTemplParaRod* pParaRod=NULL;
	if(m_bCreateWalkWay)
	{
		//内侧角钢
		pParaRod=m_xWalkWayTempl.listParaRods.GetValue(2);
		pParaRod->m_sGuige=m_sWalkWayJgSpec;
		pParaRod->m_cMaterial=m_cWalkWayJgMat;
		//外侧角钢
		pParaRod=m_xWalkWayTempl.listParaRods.GetValue(3);
		pParaRod->m_sGuige=m_sWalkWayJgSpec;
		pParaRod->m_cMaterial=m_cWalkWayJgMat;
	}
	if(m_ciWalkWayType>0)
	{	
		//上横撑角钢
		pParaRod=m_xWalkWayTempl.listParaRods.GetValue(4);
		pParaRod->m_sGuige=m_sHolderJgSpec;
		pParaRod->m_cMaterial=m_cHolderJgMat;
		UnifyParaAngleLayoutByAntiAxisX(pParaRod,m_ciHolderAngleLayoutStyle==0);
		//斜撑角钢
		pParaRod=m_xWalkWayTempl.listParaRods.GetValue(5);
		pParaRod->m_sGuige=m_sHolderJgSpec;
		pParaRod->m_cMaterial=m_cHolderJgMat;
		UnifyParaAngleLayoutByAntiAxisX(pParaRod,m_ciHolderAngleLayoutStyle==0);
		//下横撑角钢
		pParaRod=m_xWalkWayTempl.listParaRods.GetValue(6);
		if(pParaRod)
		{
			pParaRod->m_sGuige=m_sHolderJgSpec;
			pParaRod->m_cMaterial=m_cHolderJgMat;
			UnifyParaAngleLayoutByAntiAxisX(pParaRod,m_ciHolderAngleLayoutStyle==0);
		}
		//加劲角钢
		pParaRod=m_xWalkWayTempl.listParaRods.GetValue(7);
		if(pParaRod)
		{
			pParaRod->m_sGuige=m_sHolderJgSpec;
			pParaRod->m_cMaterial=m_cHolderJgMat;
		}
	}
	CStdTemplParaPlate* pParaPlate=m_xWalkWayTempl.listParaPlates.GetValue(1);
	if(pParaPlate)
	{
		pParaPlate->_thick=m_nThick;
		pParaPlate->cMaterial=m_cPlateMat;
	}
	pParaPlate=m_xWalkWayTempl.listParaPlates.GetValue(2);
	if(pParaPlate)
	{
		pParaPlate->_thick=m_nThick;
		pParaPlate->cMaterial=m_cPlateMat;
	}
	//匹配必要的装配杆件及节点
	GEPOINT nearVecX=CalNearVecX(),vec=(m_pBaseTube->End()-m_pBaseTube->Start()).normalized();
	CLDSNode* pTubeNodeS=(nearVecX*vec>0)?m_pBaseTube->pStart:m_pBaseTube->pEnd;
	CLDSNode* pTubeNodeE=(nearVecX*vec>0)?m_pBaseTube->pEnd:m_pBaseTube->pStart;
	m_xWalkWayTempl.hashMatchRodHid.SetValue(1,m_pBaseTube->handle);
	m_xWalkWayTempl.hashMatchNodeHid.SetValue(1,m_pBaseNode->handle);
	m_xWalkWayTempl.hashMatchNodeHid.SetValue(KEY4C("S"),pTubeNodeS->handle);
	m_xWalkWayTempl.hashMatchNodeHid.SetValue(KEY4C("E"),pTubeNodeE->handle);
	if(m_fZjOffDist==0)
		m_xWalkWayTempl.hashMatchNodeHid.SetValue(KEY4C("BN"),m_pBaseNode->handle);
	if(m_pInnerJg)
	{
		m_xWalkWayTempl.hashMatchRodHid.SetValue(2,m_pInnerJg->handle);
		m_xWalkWayTempl.hashMatchNodeHid.SetValue(KEY4C("SI"),m_pInnerJg->pStart->handle);
		m_xWalkWayTempl.hashMatchNodeHid.SetValue(KEY4C("EI"),m_pInnerJg->pEnd->handle);
	}
	if(m_pOuterJg)
	{
		m_xWalkWayTempl.hashMatchRodHid.SetValue(3,m_pOuterJg->handle);
		m_xWalkWayTempl.hashMatchNodeHid.SetValue(KEY4C("SO"),m_pOuterJg->pStart->handle);
		m_xWalkWayTempl.hashMatchNodeHid.SetValue(KEY4C("EO"),m_pOuterJg->pEnd->handle);
	}	
	CLDSNode* pInnerBeamDatumNode=Ta.FromNodeHandle(this->m_hInnerBeamDatumNode);
	CLDSNode* pOuterBeamDatumNode=Ta.FromNodeHandle(this->m_hOuterBeamDatumNode);
	if(pInnerBeamDatumNode)
		m_xWalkWayTempl.hashMatchNodeHid.SetValue(KEY4C("DI0"),pInnerBeamDatumNode->handle);
	else
		m_xWalkWayTempl.hashMatchNodeHid.DeleteNode(KEY4C("DI0"));
	if(pOuterBeamDatumNode)
		m_xWalkWayTempl.hashMatchNodeHid.SetValue(KEY4C("DO0"),pOuterBeamDatumNode->handle);
	else
		m_xWalkWayTempl.hashMatchNodeHid.DeleteNode(KEY4C("DO0"));
	return TRUE;
}

bool CDesignTubeWalkWayDlg::VerifyRelaRodAndNodes()
{
	if((m_pBaseTube=(CLDSLineTube*)Ta.FromRodHandle(m_hBaseTube))==NULL)
		return false;
	if((m_pBaseNode=Ta.FromNodeHandle(m_hBaseNode))==NULL)
		return false;
	if((m_pOuterJg=(CLDSLineAngle*)Ta.FromPartHandle(m_hOuterAngle,CLS_LINEANGLE))==NULL)
		return false;
	if((m_pInnerJg=(CLDSLineAngle*)Ta.FromPartHandle(m_hInnerAngle,CLS_LINEANGLE))==NULL)
		return false;
	return true;
}
void CDesignTubeWalkWayDlg::OnOK()
{
	if(!CreateTubeWalkWay())
		return;
	m_hBaseTube=m_pBaseTube?m_pBaseTube->handle:0;
	m_hBaseNode=m_pBaseNode?m_pBaseNode->handle:0;
	DelWorkPlaneSketch();
	return CDialog::OnOK();
}
void CDesignTubeWalkWayDlg::OnCancel()
{
	DelWorkPlaneSketch();
	return CDialog::OnCancel();
}
