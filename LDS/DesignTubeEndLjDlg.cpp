// DesignTubeEndLjDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "LDSDoc.h"
#include "DesignTubeEndLjDlg.h"
#include "XhCharString.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "DesignXEndRibPlateDlg.h"
#include "OddmentDesignDlg.h"
#include "LibraryQuery.h"
#include "ReportPointDlg.h"
#include "LmaDlg.h"
#include "PartsDbDlg.h"
#include "PropertyListOper.h"
#include "LayAngleBoltDlg.h"
#include "PlateBasicParaDlg.h"
#include "LocalFeatureDef.h"
#include "DataCompare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CDesignTubeEndLjDlg dialog
CDesignTubeEndLjDlg::tagAttachPlatePara::tagAttachPlatePara()
{
	memset(this,0,sizeof(tagAttachPlatePara));
	cMaterial=CSteelMatLibrary::Q235BriefMark();
	if(cMaterial==0)	//全局参数时PartLib还未初始化
		cMaterial='S';
	m_nThick=8;
}
CDesignTubeEndLjDlg::tagNodePlatePara::tagNodePlatePara()
{
	memset(this,0,sizeof(tagNodePlatePara));
	cMaterial=CSteelMatLibrary::Q235BriefMark();
	if(cMaterial==0)	//全局参数时PartLib还未初始化
		cMaterial='S';
	m_nThick=8;
}
int CDesignTubeEndLjDlg::nScrLocationX=0;
int CDesignTubeEndLjDlg::nScrLocationY=0;
CDesignTubeEndLjDlg::CDesignTubeEndLjDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignTubeEndLjDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignTubeEndLjDlg)
	m_iTubeEndLjStyle = 4;
	//}}AFX_DATA_INIT
	m_pFlDParam=NULL;			//对焊法兰
	m_pFlDParam2=NULL;			//对接的对焊法兰
	m_pFlPParam=NULL;			//平焊法兰
	m_pFlPParam2=NULL;			//对接的平焊法兰
	m_pRigidFlParam=NULL;		//刚性法兰
	m_pRigidFlParam2=NULL;		//对接的刚性法兰
	m_pFlexibleFlParam=NULL;	//柔性法兰
	m_pFlexibleFlParam2=NULL;	//对接的柔性法兰
	m_pInsertPlateParam=NULL;	//标准插板
	m_arrPropGroupLabel.RemoveAll();
	m_bStartEnd=TRUE;
	m_bCoupleFLCreate=true;
	m_pBaseNode=NULL;
	m_pCurLineTube=NULL;//当前钢管
	m_pRelatedTube=NULL;
	m_iPartType=0;		//0.普通构件 1.标准构件
	strcpy(sCoupleFlPlateNo,"");	//(普通法兰)不等径对接时对接法兰的编号
	strcpy(sWeldPlatePartNo,"");	//十字插板垂板编号
	strcpy(sWrapPlatePartNo,"");	//十字插板包板编号
	//strcpy(sCodeName,"");			//标准构件代号
	m_cFlLevel1='1';
	m_cFlLevel2='1';
	m_iStdPartLevel=1;
	m_bFaceToFace = FALSE;			//对接相贯
	m_hDatumTube1=m_hDatumTube2=m_hWeldLinkObj=0;
	m_bSpecEndLjStyle=FALSE;
	m_bHasExistEndLj=FALSE;
	m_pParamPlate=m_pSrcParamPlate=NULL;
	m_bCreateAttachPlate=FALSE;
	m_bSpecialFlRayPole=FALSE;	//异形法兰连接的射线杆件
	m_iWrapPlateType=0;			//0.钢板 1.角钢
	m_bDesignNodePlate=FALSE;
	work_norm.Set();
	m_bCreateFillPlate=FALSE;
	setupInfo.m_fWeldHeight=20;
	setupInfo.workNorm.norm_style=5;
	setupInfo.workNorm.direction=2;	//以基准钢管的基准线平面为旋转基准
	setupInfo.workNorm.rot_ang=0;
	setupInfo.workNormUpVert.norm_style=4;	//母板装配坐标系中的相对方向
	setupInfo.workNormUpVert.nearVector.Set(0,-1);
	setupInfo.workNormDownVert.norm_style=4;	//母板装配坐标系中的相对方向
	setupInfo.workNormDownVert.nearVector.Set(0,1);
	m_bEnableWindowMoveListen=FALSE;
}

void CDesignTubeEndLjDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignTubeEndLjDlg)
	DDX_Control(pDX, IDC_LIST_BOX_CHILD, m_xSubPropList);
	DDX_Control(pDX, IDC_BOLT_LIST, m_listBolt);
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_BOLT_PARA_LIST,m_listBoltParam);
	DDX_CBIndex(pDX, ID_TUBE_ENDLJ_STYLE, m_iTubeEndLjStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignTubeEndLjDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignTubeEndLjDlg)
	ON_CBN_SELCHANGE(ID_TUBE_ENDLJ_STYLE, OnSelchangeTubeEndljStyle)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnSelchangeTabGroup)
	ON_BN_CLICKED(ID_CONFIRM_CREATE, OnConfirmCreate)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_DEL_CUR_ENDLJ, OnDelCurEndlj)
	ON_BN_CLICKED(IDC_BTN_PARTS_DB, OnPartsDb)
	ON_NOTIFY(LVN_KEYDOWN, IDC_BOLT_LIST, OnKeydownBoltList)
	ON_NOTIFY(NM_CLICK, IDC_BOLT_LIST, OnClickBoltList)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_WM_MOVE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//静态及回调函数处理
static BOOL FireItemChanged(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,NM_LISTVIEW* pNMListView)
{	//修改螺栓位置后重绘预览图 wht 10-09-13
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pListCtrl->GetParent();
	if(pTubeEndLjDlg==NULL || pTubeEndLjDlg->m_iTubeEndLjStyle<3)
		return FALSE;
	CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA *pSelLsPara=(CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA*)pItem->m_idProp;
	if(pSelLsPara==NULL)
		return FALSE;
	//取消所有选中项的选中状态
	int nCount=pTubeEndLjDlg->m_listBolt.GetItemCount();
	pTubeEndLjDlg->m_listBolt.SetItemState(-1,0,LVNI_SELECTED);
	for(int i=0;i<nCount;i++)
	{	//
		DWORD dwFlag=pTubeEndLjDlg->m_listBolt.GetItemData(i);	//螺栓标志位
		int h=*((WORD*)&dwFlag);
		if(h>=100)
			h=h/100;
		if(h==pItem->GetIndex()+1)
			pTubeEndLjDlg->m_listBolt.SetItemState(i,LVNI_SELECTED|LVNI_FOCUSED,LVNI_SELECTED|LVNI_FOCUSED);
	}
	//更新属性栏显示
	pTubeEndLjDlg->LayoutBoltPara=*pSelLsPara;
	CPropTreeItem* pGroupItem=pTubeEndLjDlg->m_propList.FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("LayoutBoltParam"),NULL);
	if(pGroupItem)
		pTubeEndLjDlg->UpdateBoltProperty(pGroupItem);
	pTubeEndLjDlg->Redraw();
	return TRUE;
}
static BOOL FireModifyValue(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{	//修改螺栓位置后重绘预览图
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pListCtrl->GetParent();
	if(pTubeEndLjDlg==NULL)
		return FALSE;
	CString sOldValue=_T("");
	if(pSelItem&&pSelItem->m_lpNodeInfo)
		sOldValue=pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	//当前单元格内容已修改，更改构件为编辑状态
	if(sOldValue.CompareNoCase(sTextValue)!=0)
	{
		CXhChar100 sText;
		if(pTubeEndLjDlg->m_iTubeEndLjStyle>2)
		{	//插板
			CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA *pLsPara=(CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA*)pSelItem->m_idProp;
			if(pLsPara==NULL)
				return FALSE;
			if(iSubItem==0)		//螺栓总数
				pLsPara->m_nBoltNum=atoi(sTextValue);
			else if(iSubItem==1)//行数
				pLsPara->m_nRowNum=atoi(sTextValue);
			else if(iSubItem==2)//列数
				pLsPara->m_nColNum=atoi(sTextValue);
			else if(iSubItem==3)//直径
				pLsPara->m_nBoltD=atoi(sTextValue);
			else if(iSubItem==4)//行距
				pLsPara->m_fRowSpace=atof(sTextValue);
			else if(iSubItem==5)//列距
				pLsPara->m_fColSpace=atof(sTextValue);
			else if(iSubItem==6)//X
				pLsPara->m_posFirstBolt.x=atof(sTextValue);
			else if(iSubItem==7)//Y
				pLsPara->m_posFirstBolt.y=atof(sTextValue);
		}
		else if(pTubeEndLjDlg->m_iTubeEndLjStyle<=2)
		{
			int iCurSel=pSelItem->GetIndex();
			if(iCurSel>0)
				return FALSE;
			if(iSubItem==0)		//螺栓直径
				pTubeEndLjDlg->LayoutBoltPara.m_nBoltD=atoi(sTextValue);
			else if(iSubItem==1)//总数
				pTubeEndLjDlg->LayoutBoltPara.m_nBoltNum=atoi(sTextValue);
			else if(iSubItem==2)//螺栓所在直径
				pTubeEndLjDlg->LayoutBoltPara.m_fBoltLayoutD=atof(sTextValue);
			//更新属性栏螺栓信息显示
			CPropTreeItem* pGroupItem=pTubeEndLjDlg->m_propList.FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("LayoutBoltParam"),NULL);
			if(pGroupItem)
				pTubeEndLjDlg->UpdateBoltProperty(pGroupItem);
		}
		//重新布置螺栓,刷新对话框显示
		pTubeEndLjDlg->RefreshBoltList();
		pTubeEndLjDlg->Redraw();
	}
	return TRUE;
}
static BOOL FireDeleteItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem)
{	
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pListCtrl->GetParent();
	if(pTubeEndLjDlg==NULL || pTubeEndLjDlg->m_iTubeEndLjStyle<3 ||pTubeEndLjDlg->GetPartType()>0)
		return FALSE;	//pTubeEndLjDlg->GetPartType()>0表示标准件，此时不允许删除螺栓孔 wjh-2018.3.9
	CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA *pLsPara=NULL,*pSelLsPara=(CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA*)pItem->m_idProp;
	if(pSelLsPara==NULL)
		return FALSE;
	for(pLsPara=pTubeEndLjDlg->LayoutBoltParaList.GetFirst();pLsPara;pLsPara=pTubeEndLjDlg->LayoutBoltParaList.GetNext())
	{
		if(pLsPara==pSelLsPara)
		{
			pTubeEndLjDlg->LayoutBoltParaList.DeleteCursor();
			break;
		}
	}
	pTubeEndLjDlg->LayoutBoltParaList.Clean();
	pTubeEndLjDlg->RefreshBoltList();	
	pTubeEndLjDlg->Invalidate(FALSE);
	return TRUE;
}

static void NewRibPlate(CLDSParamPlate *pParamPlate,CDesignTubeEndLjDlg::ATTACHPLATE_PARA *pAttachPlateParam,
						CLDSPlate *pDatumPlate,BOOL bUp,CLDSPlate *pWeldParentPlate)
{
	if(pDatumPlate==NULL)
		return;
	CLDSPlate *pRibPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	pRibPlate->SetLayer(pParamPlate->layer()); //调整图层名
	pRibPlate->cfgword=pParamPlate->cfgword;	//附加肋板配材号应与十字插板配材号一致 
	//十字插板肋板焊接父构件应为十字插板关联的节点板 wht 10-08-02
	if(pWeldParentPlate)
		pRibPlate->m_hPartWeldParent=pWeldParentPlate->handle;
	pRibPlate->iSeg=pParamPlate->iSeg;
	pRibPlate->cMaterial=pAttachPlateParam->cMaterial;
	pRibPlate->SetPartNo(pAttachPlateParam->sPartNo);
	pRibPlate->Thick=pAttachPlateParam->m_nThick;
	pRibPlate->layer(2)=pParamPlate->layer(2);
	pRibPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
	pRibPlate->m_fNormOffset=-0.5*pAttachPlateParam->m_nThick;
	pRibPlate->jdb_style=5;	//相对于基准构件不动的钢板
	//建立肋板坐标系
	CLsRef *pLsRef=NULL;
	double min_bolt_x=100000;
	int start1_end2_mid0=0;
	if(bUp)	//Z正方向且不在立板上的螺栓
		start1_end2_mid0=32;
	else	//Z负方向且不在立板上的螺栓
		start1_end2_mid0=42;
	for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{
		if(pLsRef->start1_end2_mid0!=start1_end2_mid0)
			continue;
		pRibPlate->AppendLsRef(*pLsRef);				//引入螺栓
		(*pLsRef)->AddL0Thick(pRibPlate->handle,TRUE);	//更新螺栓通厚
		(*pLsRef)->CalGuigeAuto();
		double len_offset=(*pLsRef)->des_base_pos.len_offset_dist;
		if(len_offset<min_bolt_x)
			min_bolt_x=len_offset;
	}
	pRibPlate->ucs=pDatumPlate->ucs;
	pRibPlate->ucs.origin+=(pRibPlate->ucs.axis_x*(min_bolt_x-pAttachPlateParam->E));
	//钢板设计位置及法线参数
	pRibPlate->designInfo.m_bEnableFlexibleDesign=TRUE;	//启用柔性化设计
	pRibPlate->designInfo.m_hBasePart=pDatumPlate->handle;
	pRibPlate->designInfo.norm.norm_style=4;	//基准构件上的相对方向
	pRibPlate->designInfo.norm.hVicePart=pDatumPlate->handle;
	pRibPlate->designInfo.norm.nearVector.Set(0,0,1);
	pRibPlate->designInfo.origin.datum_pos_style=9;//基准构件上的相对坐标
	pRibPlate->designInfo.origin.des_para.hPart=pDatumPlate->handle;
	pRibPlate->designInfo.origin.datum_pos.Set(min_bolt_x-pAttachPlateParam->E,0,0);
	//钢板轮廓点
	pRibPlate->designInfo.iProfileStyle0123=3;	//不设计钢板外形
	PROFILE_VERTEX *pVertex=pRibPlate->vertex_list.append();
	pVertex->vertex.Set(0,0,0);
	pVertex->vertex.feature=1;
	pVertex=pRibPlate->vertex_list.append();
	pVertex->vertex.Set(pAttachPlateParam->B,0,0);
	pVertex->vertex.feature=1;
	pVertex=pRibPlate->vertex_list.append();
	pVertex->vertex.Set(pAttachPlateParam->B,pAttachPlateParam->C,0);
	pVertex->vertex.feature=1;
	pVertex=pRibPlate->vertex_list.append();
	pVertex->vertex.Set(pAttachPlateParam->B-pAttachPlateParam->D,pAttachPlateParam->A,0);
	pVertex->vertex.feature=1;
	pVertex=pRibPlate->vertex_list.append();
	pVertex->vertex.Set(0,pAttachPlateParam->A,0);
	pVertex->vertex.feature=1;
	pRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pRibPlate->GetSolidPartObject());
}

BOOL ButtonClickItem(CPropertyList* pPropList,CPropTreeItem* pItem);
BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr);
//修改附加板相关参数 
static BOOL ModifAttachPlateValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{	//附加板布置参数
	CXhChar100 tem_str;
	tem_str.Printf("%s",valueStr);
	CString sTempValue="";
	CPropTreeItem *pFindItem=NULL;
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pPropList->GetParent();
	CPropertyListOper<CDesignTubeEndLjDlg> oper(pPropList,pTubeEndLjDlg);
 	if(CDesignTubeEndLjDlg::GetPropID("m_bCreateAttachPlate")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pTubeEndLjDlg->m_bCreateAttachPlate=TRUE;
		else 
			pTubeEndLjDlg->m_bCreateAttachPlate=FALSE;
		pTubeEndLjDlg->UpdateAttachPlateProperty(pItem);	//更新附加板参数
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.A")==pItem->m_idProp)
	{
		pTubeEndLjDlg->AttachPlatePara.A=atof(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5)
		{	//十字插板附加肋板两种生成方式 输入角度时需更新长度 wht 10-09-06
			if(pTubeEndLjDlg->AttachPlatePara.m_bUseAngle)
			{	//根据角度更新D
				double D=(pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C)
					*tan(pTubeEndLjDlg->AttachPlatePara.m_fAngle*RADTODEG_COEF);
				pTubeEndLjDlg->AttachPlatePara.D=ftol(D*10)/10.0;	//保留小数点后一位
				sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.D);
				SimplifiedNumString(sTempValue);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.D"),sTempValue);
			}
			else
			{	//根据A,D,C更新角度
				double tana=pTubeEndLjDlg->AttachPlatePara.A/(pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C);
				pTubeEndLjDlg->AttachPlatePara.m_fAngle=atan(tana)*DEGTORAD_COEF;
				sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.m_fAngle);
				SimplifiedNumString(sTempValue);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_fAngle"),sTempValue);
			}
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.B")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.B=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.C")==pItem->m_idProp)
	{
		pTubeEndLjDlg->AttachPlatePara.C=atof(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5)
		{	//十字插板附加肋板两种生成方式 输入角度时需更新长度 wht 10-09-06
			if(pTubeEndLjDlg->AttachPlatePara.m_bUseAngle)
			{	//根据角度更新D
				double D=(pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C)
					*tan(pTubeEndLjDlg->AttachPlatePara.m_fAngle*RADTODEG_COEF);
				pTubeEndLjDlg->AttachPlatePara.D=ftol(D*10)/10.0;	//保留小数点后一位
				sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.D);
				SimplifiedNumString(sTempValue);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.D"),sTempValue);
			}
			else
			{	//根据A,D,C更新角度
				double tana=pTubeEndLjDlg->AttachPlatePara.A/(pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C);
				pTubeEndLjDlg->AttachPlatePara.m_fAngle=atan(tana)*DEGTORAD_COEF;
				sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.m_fAngle);
				SimplifiedNumString(sTempValue);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_fAngle"),sTempValue);
			}
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.D")==pItem->m_idProp)
	{
		pTubeEndLjDlg->AttachPlatePara.D=atof(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5)
		{	//十字插板附加肋板两种生成方式 输入角度时需更新长度 wht 10-09-06
			//根据A,D,C更新角度
			double tana=pTubeEndLjDlg->AttachPlatePara.A/(pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C);
			pTubeEndLjDlg->AttachPlatePara.m_fAngle=atan(tana)*DEGTORAD_COEF;
			sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.m_fAngle);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_fAngle"),sTempValue);
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.E")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.E=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.F")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.F=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.G")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.G=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.H")==pItem->m_idProp)
	{
		pTubeEndLjDlg->AttachPlatePara.H=atof(tem_str);
		if(pTubeEndLjDlg->AttachPlatePara.H>0)
		{
			oper.InsertEditPropItem(pItem,"AttachPlatePara.F","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"AttachPlatePara.G","","",-1,TRUE);
		}
		else
			pPropList->DeleteAllSonItems(pItem);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_iCutMode")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.m_iCutMode=tem_str[0]-'0';
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.cMaterial")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.cMaterial=QuerySteelBriefMatMark(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.sPartNo")==pItem->m_idProp)
	{
		strcpy(pTubeEndLjDlg->AttachPlatePara.sPartNo,tem_str);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(pTubeEndLjDlg->AttachPlatePara.sPartNo);
		if(pPart)
		{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
			if(pPart->cPartType==BOMPART::PLATE)
			{
				pTubeEndLjDlg->AttachPlatePara.cMaterial=pPart->cMaterial;
				pTubeEndLjDlg->AttachPlatePara.m_nThick=(int)pPart->thick;
				char sTemp[50]="";
				long idProp=CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_nThick");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
				idProp=CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.cMaterial");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
			}
			//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
		}
#endif
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_fAngle")==pItem->m_idProp)
	{
		pTubeEndLjDlg->AttachPlatePara.m_fAngle=atof(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5)
		{	//十字插板附加肋板两种生成方式 输入角度时需更新长度 wht 10-09-06
			double D=(pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C)
				*tan(pTubeEndLjDlg->AttachPlatePara.m_fAngle*RADTODEG_COEF);
			pTubeEndLjDlg->AttachPlatePara.D=ftol(D*10)/10.0;	//保留小数点后一位
			sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.D);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.D"),sTempValue);
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_nPlateNum")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.m_nPlateNum=atoi(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_nThick")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.m_nThick=atoi(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_bUseAngle")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pTubeEndLjDlg->AttachPlatePara.m_bUseAngle=TRUE;
		else 
			pTubeEndLjDlg->AttachPlatePara.m_bUseAngle=FALSE;
		//
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.D"),NULL);
		if(pFindItem)
			pFindItem->SetReadOnly(pTubeEndLjDlg->AttachPlatePara.m_bUseAngle);
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_fAngle"),NULL);
		if(pFindItem)
			pFindItem->SetReadOnly(!pTubeEndLjDlg->AttachPlatePara.m_bUseAngle);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_bMirCreate")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pTubeEndLjDlg->AttachPlatePara.m_bMirCreate=TRUE;
		else 
			pTubeEndLjDlg->AttachPlatePara.m_bMirCreate=FALSE;
	}
	//填板参数
	else if(CDesignTubeEndLjDlg::GetPropID("m_bCreateFillPlate")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pTubeEndLjDlg->m_bCreateFillPlate=TRUE;
		else 
			pTubeEndLjDlg->m_bCreateFillPlate=FALSE;
		pTubeEndLjDlg->UpdateFillPlateProperty(pItem);	//更新填板参数
	}
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.cMat")==pItem->m_idProp)
		pTubeEndLjDlg->FillPlatePara.cMat=QuerySteelBriefMatMark(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.sPartNo")==pItem->m_idProp)
	{
		strcpy(pTubeEndLjDlg->FillPlatePara.sPartNo,tem_str);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(pTubeEndLjDlg->FillPlatePara.sPartNo);
		if(pPart)
		{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
			if(pPart->cPartType==BOMPART::PLATE)
			{
				pTubeEndLjDlg->FillPlatePara.cMat=pPart->cMaterial;
				pTubeEndLjDlg->FillPlatePara.nThick=(int)pPart->thick;
				char sTemp[50]="";
				long idProp=CDesignTubeEndLjDlg::GetPropID("FillPlatePara.nThick");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
				idProp=CDesignTubeEndLjDlg::GetPropID("FillPlatePara.cMat");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
			}
			//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
		}
#endif
	}
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.nThick")==pItem->m_idProp)
		pTubeEndLjDlg->FillPlatePara.nThick=atoi(tem_str);
	else
		return FALSE;
	pTubeEndLjDlg->Invalidate(FALSE);
	return TRUE;
}

static BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	if(pItem->m_lpNodeInfo->m_strPropValue.CompareNoCase(valueStr)==0)
		return TRUE;	//值未发生变化时不进行以下操作 wht 11-01-26
	CPropTreeItem *pFindItem=NULL,*pPropItem=NULL,*pRoot=pPropList->GetRootItem();
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pPropList->GetParent();
	//最初if(pTubeEndLjDlg->IsHasExistEndLj())是为了避免修改已有设计，后来2010年海涛改偏了，现在纠正 wjh-2017.12.28
	//if(pTubeEndLjDlg->IsHasExistEndLj()&&ModifAttachPlateValue(pPropList,pItem,valueStr))
	if(ModifAttachPlateValue(pPropList,pItem,valueStr))
		return TRUE;	//已存在端连接设计
	CXhChar100 tem_str;
	tem_str.Printf("%s",valueStr);
	CXhChar100 sCurPara;
	CString sTempValue="";
	char sTextValue[100]="";
	BOOL bUpdateLayoutBoltPara=FALSE;
	int strLen=pTubeEndLjDlg->GetPropStr(pItem->m_idProp,sCurPara);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(pPropList,pTubeEndLjDlg);
	if(strLen==1&&((sCurPara[0]>='A'&&sCurPara[0]<='Z')||(sCurPara[0]>='a'&&sCurPara[0]<='z')))
	{	//参数化板设计参数 为 A~Z、a~z之间的字符
		DESIGN_PLATE_ITEM *pDesignItem=pTubeEndLjDlg->m_paramPlate.GetDesignItem(sCurPara[0]);
		if(pDesignItem)
		{
			CXhChar200 sNotes;
			sNotes.Copy(pDesignItem->sNotes);
			if(pDesignItem->dataType==1)
			{
				long handle=0;
				sscanf(tem_str,"%X",&handle);
				pTubeEndLjDlg->m_paramPlate.SetDesignItemValue(pDesignItem->key,handle,sNotes,pDesignItem->paramType,1);
			}
			else
				pTubeEndLjDlg->m_paramPlate.SetDesignItemValue(pDesignItem->key,atof(tem_str),sNotes,pDesignItem->paramType);
		}
		//pTubeEndLjDlg->UpdateTubeOddment();	//更新钢管正负头
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5&&(sCurPara[0]=='H'||sCurPara[0]=='S'||sCurPara[0]=='W'))
		{	//更新十字插板附加板外形参数
			double H=0,T=0,S=0,W=0;
			T=pTubeEndLjDlg->m_paramPlate.thick;
			pTubeEndLjDlg->m_paramPlate.GetDesignItemValue('H',&H);
			pTubeEndLjDlg->m_paramPlate.GetDesignItemValue('S',&S);
			pTubeEndLjDlg->m_paramPlate.GetDesignItemValue('W',&W);
			pTubeEndLjDlg->AttachPlatePara.A=0.5*(H-T);
			pTubeEndLjDlg->AttachPlatePara.B=S;
			pTubeEndLjDlg->AttachPlatePara.m_nThick=ftoi(T);
			pTubeEndLjDlg->AttachPlatePara.D=pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C;
			sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.A);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.A"),sTempValue);
			sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.B);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.B"),sTempValue);
			sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.D);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.D"),sTempValue);
			sTempValue.Format("%d",pTubeEndLjDlg->AttachPlatePara.m_nThick);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_nThick"),sTempValue);
		}
		else if(pTubeEndLjDlg->m_iTubeEndLjStyle==2&&pTubeEndLjDlg->GetPartType()==0)
		{	
			if(sCurPara[0]=='R')
			{	//普通法兰半径，属性栏显示为法兰直径方便用户查看修改
				double fInnerD=atof(tem_str);
				pTubeEndLjDlg->m_paramPlate.SetDesignItemValue('R',fInnerD*0.5);
			}
			else if(sCurPara[0]=='W')
			{	//更新法兰肘板宽度B
				double R=0,W=0;
				pTubeEndLjDlg->m_paramPlate.GetDesignItemValue('R',&R);
				pTubeEndLjDlg->m_paramPlate.GetDesignItemValue('W',&W);
				pTubeEndLjDlg->AttachPlatePara.B=ftoi(0.5*W-R);
				sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.B);
				SimplifiedNumString(sTempValue);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.B"),sTempValue);
			}
		}
	}
	//构件类型
	else if(CDesignTubeEndLjDlg::GetPropID("m_iPartType")==pItem->m_idProp)
	{
		pTubeEndLjDlg->SetPartType(tem_str[0]-'0');
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2)
		{
			int iPartType=pTubeEndLjDlg->GetPartType();
			if(iPartType==1||iPartType==2)
				pTubeEndLjDlg->m_arrPropGroupLabel.SetSize(3);
			else
			{
				pTubeEndLjDlg->m_arrPropGroupLabel.SetSize(4);
				pTubeEndLjDlg->m_arrPropGroupLabel.SetAt(3,"附加板");
				pPropItem=oper.InsertCmbListPropItem(pRoot,"m_bCreateAttachPlate");
				pPropItem->m_dwPropGroup=GetSingleWord(4);
				pTubeEndLjDlg->UpdateAttachPlateProperty(pPropItem);
			}
			pTubeEndLjDlg->RefreshTabCtrl(0);	//刷新TabCtrl wht 12-05-16
		}
		//重新查找标准构件参数,更新设计信息
		pTubeEndLjDlg->LoadStdPlateParaByLevel();
		pTubeEndLjDlg->UpdateDesignParam(FALSE);
		//
		pTubeEndLjDlg->UpdatePartTypeProperty(pItem);
		//更新基本参数
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("basicInfo"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateBaseInfoProperty(pFindItem);
		//更新钢管端连接位置
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("TubeEndPos"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateTubeEndProperty(pFindItem);
		//更新装配参数
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("SetUpPara"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateSetUpProperty(pFindItem);
		//
		pTubeEndLjDlg->UpdateSketchMap();
		pTubeEndLjDlg->UpdateTubeOddment();	//更新钢管正负头
		bUpdateLayoutBoltPara=TRUE;			//更新螺栓信息
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_iWrapPlateType")==pItem->m_idProp)
		pTubeEndLjDlg->m_iWrapPlateType=tem_str[0]-'0';
	//标准构件级别
	else if(CDesignTubeEndLjDlg::GetPropID("m_iStdPartLevel")==pItem->m_idProp)
	{
   		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2)
			pTubeEndLjDlg->m_cFlLevel1=tem_str[0];
		else if(pTubeEndLjDlg->m_iTubeEndLjStyle>2)
			pTubeEndLjDlg->m_iStdPartLevel=atoi(tem_str);
		//修改标准插板级别 重新查找标准构件参数,更新设计信息
		pTubeEndLjDlg->LoadStdPlateParaByLevel();
		pTubeEndLjDlg->UpdateDesignParam(FALSE);
		//更新基本参数
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("basicInfo"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateBaseInfoProperty(pFindItem);
		pTubeEndLjDlg->UpdateTubeOddment();	//更新钢管正负头
		bUpdateLayoutBoltPara=TRUE;	//更新螺栓信息
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_iStdPartLevel2")==pItem->m_idProp)
	{
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2)
			pTubeEndLjDlg->m_cFlLevel2=tem_str[0];
		//修改标准插板级别 重新查找标准构件参数,更新设计信息
		pTubeEndLjDlg->LoadStdPlateParaByLevel();
		pTubeEndLjDlg->UpdateDesignParam(FALSE);
		//更新基本参数
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("basicInfo"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateBaseInfoProperty(pFindItem);
	}
	//标准构件材质
	else if(CDesignTubeEndLjDlg::GetPropID("m_cStdPartMat")==pItem->m_idProp)
	{	//根据材质查找标找插板
		char cMat=QuerySteelBriefMatMark(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle>2)
		{
			int xLevelArr[9]={0};
			INSERT_PLATE_PARAM *pChaBanParam=pTubeEndLjDlg->QueryStdInsertPlateByMat(cMat,xLevelArr);
			if(pChaBanParam==NULL)
			{
				AfxMessageBox(CXhChar100("标准插板库中没有找到钢管材质为%s的插板",(char*)tem_str));
				return FALSE;
			}
			pTubeEndLjDlg->m_arrStdPartLevel.RemoveAll();
			for(int i=0;i<9;i++)
			{
				if(xLevelArr[i]==0)
					break;
				pTubeEndLjDlg->m_arrStdPartLevel.Add(CXhChar16("%d",xLevelArr[i]));
			}
			pTubeEndLjDlg->m_iStdPartLevel=xLevelArr[0];
		}
		else if(pTubeEndLjDlg->m_iTubeEndLjStyle==2)
		{
			int xLevelArr[9]={0};
			FL_PARAM *pFlDPara=pTubeEndLjDlg->QueryStdFlDParaByMat(cMat,xLevelArr);
			if(pFlDPara==NULL)
			{
				AfxMessageBox(CXhChar100("标准对焊法兰库中没有找到材质为%s的对焊法兰",(char*)tem_str));
				return FALSE;
			}
			pTubeEndLjDlg->m_arrStdFlDLevelM.RemoveAll();
			for(int i=0;i<9;i++)
			{
				if(xLevelArr[i]==0)
					break;
				pTubeEndLjDlg->m_arrStdFlDLevelM.Add(CXhChar16("%d",xLevelArr[i]));
			}
			pTubeEndLjDlg->m_cFlLevel1=pFlDPara->cLevelMark;
		}
		pTubeEndLjDlg->m_cStdPartMat=cMat;
		pTubeEndLjDlg->LoadStdPlateParaByLevel();
		pTubeEndLjDlg->UpdateDesignParam(FALSE);
		//更新基本参数
		oper.UpdatePropItemValue("m_iStdPartLevel");
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("basicInfo"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateBaseInfoProperty(pFindItem);
		pTubeEndLjDlg->UpdateTubeOddment();	//更新钢管正负头
		bUpdateLayoutBoltPara=TRUE;	//更新螺栓信息
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_bCoupleFLCreate")==pItem->m_idProp)
	{
		pTubeEndLjDlg->m_bCoupleFLCreate=tem_str[0]!='0';
		//更新基本参数
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("basicInfo"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateBaseInfoProperty(pFindItem);
	}
	//平面切割
	else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.x")
		||pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.y")
		||pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.z")
		||pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.x")
		||pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.y")
		||pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.z"))
	{
		if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.x"))
			pTubeEndLjDlg->cutPlaneOrigin.x = atof(valueStr);
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.y"))
			pTubeEndLjDlg->cutPlaneOrigin.y = atof(valueStr);
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.z"))
			pTubeEndLjDlg->cutPlaneOrigin.z = atof(valueStr);
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.x"))
			pTubeEndLjDlg->cutPlaneNorm.x = atof(valueStr);
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.y"))
			pTubeEndLjDlg->cutPlaneNorm.y = atof(valueStr);
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.z"))
			pTubeEndLjDlg->cutPlaneNorm.z = atof(valueStr);
		pTubeEndLjDlg->UpdateTubeOddment();	//更新钢管正负头
	}
	//钢管相贯
	else if(CDesignTubeEndLjDlg::GetPropID("m_bFaceToFace")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pTubeEndLjDlg->m_bFaceToFace=TRUE;
		else
			pTubeEndLjDlg->m_bFaceToFace=FALSE;
		pPropList->DeleteItemByPropId(CDesignTubeEndLjDlg::GetPropID("m_hDatumTube2"));
		if(!pTubeEndLjDlg->m_bFaceToFace)
		{	//相贯钢管2
			pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("TubeTransect"),NULL);
			if(pFindItem)
				oper.InsertBtnEditPropItem(pFindItem,"m_hDatumTube2","","",-1,TRUE);
		}
		pTubeEndLjDlg->UpdateTubeOddment();	//更新钢管正负头 wht 11-05-10
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_hWeldLinkObj")==pItem->m_idProp)
		pTubeEndLjDlg->m_hWeldLinkObj=HexStrToLong(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_hDatumTube1")==pItem->m_idProp)
	{
		sscanf(tem_str,"%X",&pTubeEndLjDlg->m_hDatumTube1);
		pTubeEndLjDlg->UpdateTubeOddment();	//更新钢管正负头 wht 11-05-10
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_hDatumTube2")==pItem->m_idProp)
	{
		sscanf(tem_str,"%X",&pTubeEndLjDlg->m_hDatumTube2);
		pTubeEndLjDlg->UpdateTubeOddment();	//更新钢管正负头 wht 11-05-10
	}
	//基本信息
	else if(CDesignTubeEndLjDlg::GetPropID("sPartNo")==pItem->m_idProp)
	{
		pTubeEndLjDlg->m_paramPlate.SetPartNo(tem_str);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(pTubeEndLjDlg->m_paramPlate.Label);
		if(pPart)
		{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
			if(pPart->cPartType==BOMPART::PLATE)
			{
				pTubeEndLjDlg->m_paramPlate.cMaterial=pPart->cMaterial;
				pTubeEndLjDlg->m_paramPlate.thick=(int)pPart->thick;
				char sTemp[50]="";
				long idProp=CDesignTubeEndLjDlg::GetPropID("thick");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
				idProp=CDesignTubeEndLjDlg::GetPropID("cMaterial");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
			}
			//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
		}
#endif
	}
	else if(CDesignTubeEndLjDlg::GetPropID("sCoupleFlPlateNo")==pItem->m_idProp)
		strcpy(pTubeEndLjDlg->sCoupleFlPlateNo,tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("sWeldPlatePartNo")==pItem->m_idProp)
		strcpy(pTubeEndLjDlg->sWeldPlatePartNo,tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("sWrapPlatePartNo")==pItem->m_idProp)
		strcpy(pTubeEndLjDlg->sWrapPlatePartNo,tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("iSeg")==pItem->m_idProp)
		pTubeEndLjDlg->m_paramPlate.iSeg=SEGI(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("cMaterial")==pItem->m_idProp)
		pTubeEndLjDlg->m_paramPlate.cMaterial=QuerySteelBriefMatMark(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_uStatMatCoef")==pItem->m_idProp)
		pTubeEndLjDlg->m_paramPlate.m_uStatMatCoef=atoi(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("thick")==pItem->m_idProp)
	{
		pTubeEndLjDlg->m_paramPlate.thick=atoi(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2&&pTubeEndLjDlg->GetPartType()==0)
		{	//普通法兰
			double H=ftoi(0.5*pTubeEndLjDlg->m_paramPlate.thick);
			pTubeEndLjDlg->m_paramPlate.SetDesignItemValue('H',H);
			sTempValue.Format("%f",H);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("H"),sTempValue);
		}
	}
	//装配参数
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm")==pItem->m_idProp)
	{
		if(valueStr[0]=='0')	//自定义方向
		{
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x"),
				pTubeEndLjDlg->setupInfo.workNorm.norm_style!=0);
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.y"),
				pTubeEndLjDlg->setupInfo.workNorm.norm_style!=0);
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.z"),
				pTubeEndLjDlg->setupInfo.workNorm.norm_style!=0);
		}
		else
		{
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x"));
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.y"));
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.z"));
			f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
			pTubeEndLjDlg->setupInfo.workNorm.norm_style=0;	//指定法丝
			pTubeEndLjDlg->setupInfo.workNorm.vector=vector;
			CString sText;
			sText.Format("%.f",vector.x);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x"),sText);
			sText.Format("%.f",vector.y);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.y"),sText);
			sText.Format("%.f",vector.z);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.z"),sText);
		}
		pTubeEndLjDlg->DisplayChildPropertyList(pItem,FALSE);
		return TRUE;
	}
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNorm.vector.x=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNorm.vector.y=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNorm.vector.z=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm")==pItem->m_idProp)
	{
		if(valueStr[0]=='0')	//自定义方向
		{
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x"),
				pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style!=0);
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.y"),
				pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style!=0);
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.z"),
				pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style!=0);
		}
		else
		{
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x"));
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.y"));
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.z"));
			f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
			pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style=0;	//指定法丝
			pTubeEndLjDlg->setupInfo.workNormUpVert.vector=vector;
			CString sText;
			sText.Format("%.f",vector.x);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x"),sText);
			sText.Format("%.f",vector.y);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.y"),sText);
			sText.Format("%.f",vector.z);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.z"),sText);
		}
		pTubeEndLjDlg->DisplayChildPropertyList(pItem,FALSE);
		return TRUE;
	}
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNormUpVert.vector.x=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNormUpVert.vector.y=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNormUpVert.vector.z=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm")==pItem->m_idProp)
	{
		if(valueStr[0]=='0')	//自定义方向
		{
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x"),
				pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style!=0);
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.y"),
				pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style!=0);
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.z"),
				pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style!=0);
		}
		else
		{
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x"));
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.y"));
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.z"));
			f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
			pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style=0;	//指定法丝
			pTubeEndLjDlg->setupInfo.workNormDownVert.vector=vector;
			CString sText;
			sText.Format("%.f",vector.x);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x"),sText);
			sText.Format("%.f",vector.y);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.y"),sText);
			sText.Format("%.f",vector.z);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.z"),sText);
		}
		pTubeEndLjDlg->DisplayChildPropertyList(pItem,FALSE);
		return TRUE;
	}
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNormDownVert.vector.x=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNormDownVert.vector.y=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNormDownVert.vector.z=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.norm_style")==pItem->m_idProp)
	{
		if(pPropList->m_nObjClassTypeId==0)
		{
			//转换近似法向
			pTubeEndLjDlg->setupInfo.workNorm.norm_style=valueStr[0]-'0';
			if(pTubeEndLjDlg->setupInfo.workNorm.norm_style==2)	//相对转绝对
				pTubeEndLjDlg->setupInfo.workNorm.nearVector=pTubeEndLjDlg->setupInfo.workNorm.vector;
			else if(pTubeEndLjDlg->setupInfo.workNorm.norm_style==4)	//绝对转相对
			{
				CLDSPart* pDatumPart=NULL;
				if(pTubeEndLjDlg->setupInfo.workNorm.hVicePart>0x20)
					pDatumPart=console.FromPartHandle(pTubeEndLjDlg->setupInfo.workNorm.hVicePart);
				if(pDatumPart==NULL)
					pDatumPart=&pTubeEndLjDlg->m_paramPlate;
				pTubeEndLjDlg->setupInfo.workNorm.nearVector=pTubeEndLjDlg->setupInfo.workNorm.vector;
				vector_trans(pTubeEndLjDlg->setupInfo.workNorm.nearVector,pDatumPart->ucs,FALSE);
			}
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x"),valueStr[0]!='0');
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.y"),valueStr[0]!='0');
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.z"),valueStr[0]!='0');
		}
		else if(pPropList->m_nObjClassTypeId==1)
		{
			//转换近似法向
			pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style=valueStr[0]-'0';
			if(pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style==2)	//相对转绝对
				pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector=pTubeEndLjDlg->setupInfo.workNormUpVert.vector;
			else if(pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style==4)	//绝对转相对
			{
				CLDSPart* pDatumPart=NULL;
				if(pTubeEndLjDlg->setupInfo.workNormUpVert.hVicePart>0x20)
					pDatumPart=console.FromPartHandle(pTubeEndLjDlg->setupInfo.workNormUpVert.hVicePart);
				if(pDatumPart==NULL)
					pDatumPart=&pTubeEndLjDlg->m_paramPlate;
				pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector=pTubeEndLjDlg->setupInfo.workNormUpVert.vector;
				vector_trans(pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector,pDatumPart->ucs,FALSE);
			}
			pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style=valueStr[0]-'0';
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x"),valueStr[0]!='0');
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.y"),valueStr[0]!='0');
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.z"),valueStr[0]!='0');
		}
		else if(pPropList->m_nObjClassTypeId==2)
		{
			//转换近似法向
			pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style=valueStr[0]-'0';
			if(pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style==2)	//相对转绝对
				pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector=pTubeEndLjDlg->setupInfo.workNormDownVert.vector;
			else if(pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style==4)	//绝对转相对
			{
				CLDSPart* pDatumPart=NULL;
				if(pTubeEndLjDlg->setupInfo.workNormDownVert.hVicePart>0x20)
					pDatumPart=console.FromPartHandle(pTubeEndLjDlg->setupInfo.workNormDownVert.hVicePart);
				if(pDatumPart==NULL)
					pDatumPart=&pTubeEndLjDlg->m_paramPlate;
				pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector=pTubeEndLjDlg->setupInfo.workNormDownVert.vector;
				vector_trans(pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector,pDatumPart->ucs,FALSE);
			}
			pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style=valueStr[0]-'0';
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x"),valueStr[0]!='0');
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.y"),valueStr[0]!='0');
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.z"),valueStr[0]!='0');
		}
		pTubeEndLjDlg->DisplayChildPropertyList(NULL,TRUE);
		return TRUE;
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.norm_wing")==pItem->m_idProp)
	{
		if(pPropList->m_nObjClassTypeId==0)
			pTubeEndLjDlg->setupInfo.workNorm.norm_wing=valueStr[0]-'0';
		else if(pPropList->m_nObjClassTypeId==1)
			pTubeEndLjDlg->setupInfo.workNormUpVert.norm_wing=valueStr[0]-'0';
		else if(pPropList->m_nObjClassTypeId==2)
			pTubeEndLjDlg->setupInfo.workNormDownVert.norm_wing=valueStr[0]-'0';
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.direction")==pItem->m_idProp)
	{
		if(pPropList->m_nObjClassTypeId==0)
			pTubeEndLjDlg->setupInfo.workNorm.direction=valueStr[0]-'0';
		else if(pPropList->m_nObjClassTypeId==1)
			pTubeEndLjDlg->setupInfo.workNormUpVert.direction=valueStr[0]-'0';
		else if(pPropList->m_nObjClassTypeId==2)
			pTubeEndLjDlg->setupInfo.workNormDownVert.direction=valueStr[0]-'0';
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.rot_ang")==pItem->m_idProp)
	{
		double fRotAngle=0;
		f3dPoint view_norm=pTubeEndLjDlg->GetWorkNorm();
		f3dPoint tube_work_norm=pTubeEndLjDlg->m_pCurLineTube->WorkPlaneNorm();
		if(strcmp(tem_str,"朝外")==0)
		{	
			if(tube_work_norm*view_norm>0)
				fRotAngle=180;	//工作面法线方向与钢管基准面法线方向相同
			else 
				fRotAngle=0;
		}
		else if(strcmp(tem_str,"朝内")==0)
		{
			if(tube_work_norm*view_norm>0)
				fRotAngle=0;	//工作面法线方向与钢管基准面法线方向相同
			else 
				fRotAngle=180;
		}
		else
			fRotAngle=atof(tem_str);
		if(pPropList->m_nObjClassTypeId==0)
			pTubeEndLjDlg->setupInfo.workNorm.rot_ang=fRotAngle;
		else if(pPropList->m_nObjClassTypeId==1)
			pTubeEndLjDlg->setupInfo.workNormUpVert.rot_ang=fRotAngle;
		else if(pPropList->m_nObjClassTypeId==2)
			pTubeEndLjDlg->setupInfo.workNormDownVert.rot_ang=fRotAngle;
		DESIGN_PLATE_ITEM *pDesignItem=pTubeEndLjDlg->m_paramPlate.GetDesignItem('D');
		pTubeEndLjDlg->m_paramPlate.SetDesignItemValue('D',fRotAngle,pDesignItem->sNotes,pDesignItem->paramType);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVectorType")==pItem->m_idProp)
	{
		if(valueStr[0]-'0'==0)	//自定义方向
		{
			CXhChar100 sText;
			CItemInfo *lpInfo;
			CPropTreeItem *pSonItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.x"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "X分量";
				lpInfo->m_strPropHelp = "X分量";
				pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.x");
				if(pTubeEndLjDlg->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			
			pSonItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.y"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Y分量";
				lpInfo->m_strPropHelp = "Y分量";
				pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.y");
				if(pTubeEndLjDlg->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			
			pSonItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.z"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Z分量";
				lpInfo->m_strPropHelp = "Z分量";
				pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.z");
				if(pTubeEndLjDlg->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
		{
			pPropList->DeleteItemByPropId(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.x"));
			pPropList->DeleteItemByPropId(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.y"));
			pPropList->DeleteItemByPropId(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.z"));
			if(pPropList->m_nObjClassTypeId==0)
				pTubeEndLjDlg->setupInfo.workNorm.nearVector=CStdVector::GetVector(valueStr[0]-'0');
			else if(pPropList->m_nObjClassTypeId==1)
				pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector=CStdVector::GetVector(valueStr[0]-'0');
			else if(pPropList->m_nObjClassTypeId==2)
				pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector=CStdVector::GetVector(valueStr[0]-'0');
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.x")==pItem->m_idProp)
	{
		if(pPropList->m_nObjClassTypeId==0)
			pTubeEndLjDlg->setupInfo.workNorm.nearVector.x=atof(valueStr);
		else if(pPropList->m_nObjClassTypeId==1)
			pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector.x=atof(valueStr);
		else if(pPropList->m_nObjClassTypeId==2)
			pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector.x=atof(valueStr);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.y")==pItem->m_idProp)
	{
		if(pPropList->m_nObjClassTypeId==0)
			pTubeEndLjDlg->setupInfo.workNorm.nearVector.y=atof(valueStr);
		else if(pPropList->m_nObjClassTypeId==1)
			pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector.y=atof(valueStr);
		else if(pPropList->m_nObjClassTypeId==2)
			pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector.y=atof(valueStr);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.z")==pItem->m_idProp)
	{
		if(pPropList->m_nObjClassTypeId==0)
			pTubeEndLjDlg->setupInfo.workNorm.nearVector.z=atof(valueStr);
		else if(pPropList->m_nObjClassTypeId==1)
			pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector.z=atof(valueStr);
		else if(pPropList->m_nObjClassTypeId==2)
			pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector.z=atof(valueStr);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("H1")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.H1=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_fNormOffset")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.m_fNormOffset=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_fOddment")==pItem->m_idProp)
	{
		pTubeEndLjDlg->setupInfo.m_fOddment=atof(tem_str);
		//用户手动输入钢管正负头以后，不在支持自动计算正负头 wht 11-05-09
		if(pTubeEndLjDlg->m_bStartEnd)
			pTubeEndLjDlg->m_pCurLineTube->desStartOdd.m_iOddCalStyle=2;	//手动输入
		else 
			pTubeEndLjDlg->m_pCurLineTube->desEndOdd.m_iOddCalStyle=2;		//手动输入
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fWeldHeight")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.m_fWeldHeight=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_fTubeNormOffset")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.m_fTubeNormOffset=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_fTubeEccentricDist")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.m_fTubeEccentricDist=atof(tem_str);
	//节点板参数
	else if(CDesignTubeEndLjDlg::GetPropID("NodePlatePara.cMaterial")==pItem->m_idProp)
		pTubeEndLjDlg->NodePlatePara.cMaterial=QuerySteelBriefMatMark(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("NodePlatePara.sPartNo")==pItem->m_idProp)
	{
		strcpy(pTubeEndLjDlg->NodePlatePara.sPartNo,tem_str);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(pTubeEndLjDlg->NodePlatePara.sPartNo);
		if(pPart)
		{	//根据找到的BOM信息，更新该钢板的基本信息(段号、规格、材质)
			if(pPart->cPartType==BOMPART::PLATE)
			{
				pTubeEndLjDlg->NodePlatePara.cMaterial=pPart->cMaterial;
				pTubeEndLjDlg->NodePlatePara.m_nThick=(int)pPart->thick;
				char sTemp[50]="";
				long idProp=CDesignTubeEndLjDlg::GetPropID("NodePlatePara.m_nThick");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
				idProp=CDesignTubeEndLjDlg::GetPropID("NodePlatePara.cMaterial");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
			}
			//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的钢板",(char*)pPart->sPartNo));
		}
#endif
	}
	else if(CDesignTubeEndLjDlg::GetPropID("NodePlatePara.m_nRollHeight")==pItem->m_idProp)
		pTubeEndLjDlg->NodePlatePara.m_nRollHeight=atoi(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("NodePlatePara.m_nThick")==pItem->m_idProp)
	{
		pTubeEndLjDlg->NodePlatePara.m_nThick=atoi(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5)
		{	//更新节点板厚度 wht 11-05-11
			pTubeEndLjDlg->FillPlatePara.nNodePlateThick=atoi(tem_str);
			char sThick[50]="";
			long idProp=CDesignTubeEndLjDlg::GetPropID("FillPlatePara.nNodePlateThick");
			if(pTubeEndLjDlg->GetPropValueStr(idProp,sThick)>0)
				pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sThick);
		}
	}
	//螺栓布置参数
	else if(CDesignTubeEndLjDlg::GetPropID("m_nBoltD")==pItem->m_idProp)
	{
		pTubeEndLjDlg->LayoutBoltPara.m_nBoltD=atoi(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2&&pTubeEndLjDlg->GetPartType()==0)
			pTubeEndLjDlg->UpdateLayoutBoltPara();
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_iDriection")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_iDirection=tem_str[0]-'0';
	else if(CDesignTubeEndLjDlg::GetPropID("m_iStartEdge")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_iStartEdge=tem_str[0]-'0';
	else if(CDesignTubeEndLjDlg::GetPropID("m_fOffsetAngle")==pItem->m_idProp)
	{
		pTubeEndLjDlg->LayoutBoltPara.m_fOffsetAngle=atof(tem_str);
		//普通构件直接调整每个螺栓的偏移角，标准构件调整法兰X轴偏移角度 wht 11-05-23
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2&&pTubeEndLjDlg->GetPartType()==0)
		{	//螺栓数修改后更新法兰肘板数量以及首块肘板的偏移角度
			pTubeEndLjDlg->AttachPlatePara.m_fAngle=pTubeEndLjDlg->LayoutBoltPara.m_fOffsetAngle
				+180.0/pTubeEndLjDlg->AttachPlatePara.m_nPlateNum;
			sTempValue.Format("%f",pTubeEndLjDlg->AttachPlatePara.m_fAngle);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_fAngle"),sTempValue);
			pTubeEndLjDlg->UpdateLayoutBoltPara();
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fBoltLayoutD")==pItem->m_idProp)
	{
		pTubeEndLjDlg->LayoutBoltPara.m_fBoltLayoutD=atof(tem_str);
		pTubeEndLjDlg->UpdateLayoutBoltPara();
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_nBoltNum")==pItem->m_idProp)
	{	//法兰上的螺栓数量
		int nBoltNum=atoi(tem_str);
		if(nBoltNum==pTubeEndLjDlg->LayoutBoltPara.m_nBoltNum)
			return FALSE;
		pTubeEndLjDlg->LayoutBoltPara.m_nBoltNum=nBoltNum;
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2&&pTubeEndLjDlg->GetPartType()==0)
		{
			pTubeEndLjDlg->UpdateLayoutBoltPara();
			//螺栓数修改后更新法兰肘板数量以及首块肘板的偏移角度
			pTubeEndLjDlg->AttachPlatePara.m_nPlateNum=nBoltNum;
			pTubeEndLjDlg->AttachPlatePara.m_fAngle=pTubeEndLjDlg->LayoutBoltPara.m_fOffsetAngle+180.0/nBoltNum;
			oper.UpdatePropItemValue("AttachPlatePara.m_nPlateNum");
			oper.UpdatePropItemValue("AttachPlatePara.m_fAngle");
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_bStaggerLayout")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pTubeEndLjDlg->LayoutBoltPara.m_bStaggerLayout=TRUE;
		else
			pTubeEndLjDlg->LayoutBoltPara.m_bStaggerLayout=FALSE;
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_bMirLayout")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pTubeEndLjDlg->LayoutBoltPara.m_bMirLayout=TRUE;
		else
			pTubeEndLjDlg->LayoutBoltPara.m_bMirLayout=FALSE;
		pPropList->DeleteItemByPropId(CDesignTubeEndLjDlg::GetPropID("m_fSpecialSpace"));
		if(pTubeEndLjDlg->LayoutBoltPara.m_bMirLayout)
			oper.InsertEditPropItem(pItem,"m_fSpecialSpace","","",-1,TRUE);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fSpecialSpace")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_fSpecialSpace=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_nRowNum")==pItem->m_idProp)
	{
		pTubeEndLjDlg->LayoutBoltPara.m_nRowNum=atoi(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5)
		{
			if(pTubeEndLjDlg->LayoutBoltPara.m_nRowNum!=1)
				pTubeEndLjDlg->LayoutBoltPara.m_bStaggerLayout=FALSE;
			pPropItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("m_bStaggerLayout"),NULL);
			if(pPropItem)
			{	//交错布置水平面与竖直面螺栓
				oper.UpdatePropItemValue("m_bStaggerLayout");
				pPropItem->SetReadOnly(pTubeEndLjDlg->LayoutBoltPara.m_nRowNum!=1);
			}
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_nColNum")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_nColNum=atoi(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_fRowSpace")==pItem->m_idProp)
	{
		pTubeEndLjDlg->LayoutBoltPara.m_fRowSpace=atof(tem_str);
		pTubeEndLjDlg->LayoutBoltPara.m_posFirstBolt.y=-0.5*atof(tem_str);
		oper.UpdatePropItemValue("m_posFirstBolt.y");
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fColSpace")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_fColSpace=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_posFirstBolt.x")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_posFirstBolt.x=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_posFirstBolt.y")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_posFirstBolt.y=atof(tem_str);
	//厚度或者槽型插板朝向发生变化时更新钢管法向偏移量
	if(pTubeEndLjDlg->m_iTubeEndLjStyle==4&&pTubeEndLjDlg->NodePlatePara.m_nThick>0
		&&(CDesignTubeEndLjDlg::GetPropID("T")==pItem->m_idProp
		||CDesignTubeEndLjDlg::GetPropID("design_axis_z.rot_ang")==pItem->m_idProp
		||CDesignTubeEndLjDlg::GetPropID("NodePlatePara.m_nThick")==pItem->m_idProp))	//槽型插板
	{
		pTubeEndLjDlg->setupInfo.m_fTubeNormOffset=-(pTubeEndLjDlg->NodePlatePara.m_nThick+pTubeEndLjDlg->m_paramPlate.thick)*0.5;
		if(fabs(fabs(pTubeEndLjDlg->setupInfo.workNorm.rot_ang)-180)<EPS)	//槽型插板旋转180度
			pTubeEndLjDlg->setupInfo.m_fTubeNormOffset*=-1.0;
		else if(fabs(pTubeEndLjDlg->setupInfo.workNorm.rot_ang)>EPS)
			pTubeEndLjDlg->setupInfo.m_fTubeNormOffset=0;	//非0°和180°时偏移量归零	
		sTempValue.Format("%f",pTubeEndLjDlg->setupInfo.m_fTubeNormOffset);
		SimplifiedNumString(sTempValue);
		pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("m_fTubeNormOffset"),sTempValue);
	}
	if(bUpdateLayoutBoltPara)
	{
		//更新螺栓布置参数
		pTubeEndLjDlg->UpdateLayoutBoltPara();
		//更新外形属性栏
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("Profile"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateProfileProperty(pFindItem);
		//螺栓布置属性栏
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("LayoutBoltParam"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateBoltProperty(pFindItem);
	}
	pTubeEndLjDlg->Invalidate(FALSE);
	return TRUE;
}

static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pListCtrl->GetParent();
	if(pTubeEndLjDlg==NULL)
		return FALSE;
	if(pTubeEndLjDlg->GetPartType()>0)
		return FALSE;	//不准构件不支持添加删除螺栓操作
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	pMenu->AppendMenu(MF_STRING,ID_DELETE_BOLT_ITEM,"删除螺栓");
	pMenu->AppendMenu(MF_STRING,ID_NEW_BOLT_ITEM,"添加螺栓");
	pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"删除全部螺栓");
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pTubeEndLjDlg);
	return TRUE;
}
static BOOL PropHelpPrompt(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pPropList->GetParent();
	pTubeEndLjDlg->RefreshCtrlState(TRUE);
	if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm"))
		pTubeEndLjDlg->DisplayChildPropertyList(pItem,FALSE);
	else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm"))
		pTubeEndLjDlg->DisplayChildPropertyList(pItem,FALSE);
	else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm"))
		pTubeEndLjDlg->DisplayChildPropertyList(pItem,FALSE);
	return TRUE;
}
static BOOL ButtonClickItem(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	char valueStr[200]="";
	CPropTreeItem *pFindItem=NULL;
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pPropList->GetParent();
	if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("design_axis_z.hVicePart"))
		pTubeEndLjDlg->SelectObject(CLS_PART,pItem->m_idProp);		//选择基准构件
	else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("design_axis_z.hCrossPart"))
		pTubeEndLjDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//选择基准杆件
	else if(!pTubeEndLjDlg->IsHasExistEndLj())	//不存在端连接设计
	{
		if(CDesignTubeEndLjDlg::GetPropID("m_hDatumTube1")==pItem->m_idProp
			||CDesignTubeEndLjDlg::GetPropID("m_hDatumTube2")==pItem->m_idProp)
			pTubeEndLjDlg->SelectObject(CLS_LINETUBE,pItem->m_idProp);	//选择基准钢管
		if(CDesignTubeEndLjDlg::GetPropID("m_hWeldLinkObj")==pItem->m_idProp)
			pTubeEndLjDlg->SelectObject(CLS_LINETUBE,pItem->m_idProp);	//选择基准钢管
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("m_fWeldHeight"))
		{	//根据焊缝高度重新计算钢管正负头 wht 11-05-09
			CLDSLineTube *pLineTube=pTubeEndLjDlg->m_pCurLineTube;
			DESIGNODDMENT * pCurOddment=&(pLineTube->desEndOdd);
			if(pTubeEndLjDlg->m_bStartEnd)
				pCurOddment=&(pLineTube->desStartOdd);
			pCurOddment->m_fCollideDist=pTubeEndLjDlg->setupInfo.m_fWeldHeight+g_sysPara.VertexDist;
			BOOL bRetCode=TRUE;
			if(console.FromPartHandle(pCurOddment->m_hRefPart1)==NULL
				&&console.FromPartHandle(pCurOddment->m_hRefPart2)==NULL)
			{	//无碰撞基准杆件时提示用户使用基准节点父杆件为碰撞基准杆件 wht 11-05-11
				if(AfxMessageBox("未设置碰撞参照杆件,无法计算钢管正负头,是否以基准节点父杆件为碰撞基准杆件?",MB_YESNO)==IDYES)
				{
					if(pTubeEndLjDlg->m_pBaseNode->hFatherPart!=pTubeEndLjDlg->m_pCurLineTube->handle)
						pCurOddment->m_hRefPart1=pTubeEndLjDlg->m_pBaseNode->hFatherPart;
				}
				else
					bRetCode=FALSE;
			}
			if(bRetCode)
			{	//根据碰撞计算正负头,此处由用户点击按钮触发故不用判断判断正负头计算方式 
				pTubeEndLjDlg->setupInfo.m_fOddment=
					pLineTube->CalOddment(pTubeEndLjDlg->m_bStartEnd,&pTubeEndLjDlg->m_paramPlate,true);
				char sText[100]="";
				long idProp=CDesignTubeEndLjDlg::GetPropID("m_fOddment");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,sText);
			}
		}
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("m_fOddment"))
		{
			COddmentDesignDlg oddDlg;
			DESIGNODDMENT *pCurDesOddment=NULL;
			if(pTubeEndLjDlg->m_bStartEnd)
				pCurDesOddment=&pTubeEndLjDlg->m_pCurLineTube->desStartOdd;
			else
				pCurDesOddment=&pTubeEndLjDlg->m_pCurLineTube->desEndOdd;
			oddDlg.m_hRefPart1.Format("0X%X",pCurDesOddment->m_hRefPart1);
			oddDlg.m_hRefPart2.Format("0X%X",pCurDesOddment->m_hRefPart2);
			oddDlg.m_fCollideDist = pCurDesOddment->m_fCollideDist;
			oddDlg.m_iOddCalStyle = pCurDesOddment->m_iOddCalStyle;
			oddDlg.m_bDispSelectBtn=FALSE;	//
			if(oddDlg.DoModal()==IDOK)
			{
				char sText[100]="";
				_snprintf(sText,99,oddDlg.m_hRefPart1);
				sscanf(sText,"%X",&pCurDesOddment->m_hRefPart1);
				_snprintf(sText,99,oddDlg.m_hRefPart2);
				sscanf(sText,"%X",&pCurDesOddment->m_hRefPart2);
				pCurDesOddment->m_fCollideDist = oddDlg.m_fCollideDist;
				pCurDesOddment->m_iOddCalStyle = oddDlg.m_iOddCalStyle;
				if(pCurDesOddment->m_iOddCalStyle==0)
				{	//根据碰撞计算钢管正负头 wht 11-05-09
					pTubeEndLjDlg->setupInfo.m_fOddment=
						pTubeEndLjDlg->m_pCurLineTube->CalOddment(pTubeEndLjDlg->m_bStartEnd,&pTubeEndLjDlg->m_paramPlate,true);
					char valueStr[50]="";
					if(pTubeEndLjDlg->GetPropValueStr(pItem->m_idProp,valueStr)>0)
						pPropList->SetItemPropValue(pItem->m_idProp,CString(valueStr));
				}
			}
		}
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin"))
		{
			CReportPointDlg pointDlg;
			pointDlg.m_sCaption = "钢管切割面基点坐标";
			pointDlg.m_fPointX = pTubeEndLjDlg->cutPlaneOrigin.x;
			pointDlg.m_fPointY = pTubeEndLjDlg->cutPlaneOrigin.y;
			pointDlg.m_fPointZ = pTubeEndLjDlg->cutPlaneOrigin.z;
			if(pointDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.z"),CString(sText));
				pTubeEndLjDlg->cutPlaneOrigin.Set(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ);
				pTubeEndLjDlg->UpdateTubeOddment();	//更新钢管正负头
			}
		}
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm"))
		{
			CReportVectorDlg vecDlg;
			vecDlg.m_sCaption = "钢管切割面法线坐标";
			vecDlg.m_fVectorX = pTubeEndLjDlg->cutPlaneNorm.x;
			vecDlg.m_fVectorY = pTubeEndLjDlg->cutPlaneNorm.y;
			vecDlg.m_fVectorZ = pTubeEndLjDlg->cutPlaneNorm.z;
			pFindItem = pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.x"),NULL);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.y"),NULL);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.z"),NULL);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.z"),CString(sText));
				pTubeEndLjDlg->cutPlaneNorm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				pTubeEndLjDlg->UpdateTubeOddment();	//更新钢管正负头
			}
		}
	}
	pTubeEndLjDlg->Invalidate(FALSE);
	g_pSolidDraw->Draw();
	return TRUE;
}
BOOL FirePopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pPropList->GetParent();
	if(pTubeEndLjDlg==NULL)
		return FALSE;
	if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("LayoutBoltParam"))
	{
		pItem->m_lpNodeInfo->SetCheck(iMenu+1);
		if(iMenu==0)
		{	//添加螺栓布置参数
			int nBoltNum=pTubeEndLjDlg->LayoutBoltPara.m_nBoltNum;
			if(pTubeEndLjDlg->m_iTubeEndLjStyle==5&&pTubeEndLjDlg->LayoutBoltPara.m_bMirLayout)
			{	//对称布置十字插板螺栓,保证输入的螺栓数为偶数个　wht 11-05-03
				if(nBoltNum%2!=0)
				{
					AfxMessageBox("输入的螺栓数部是偶数个，不支持对称布置螺栓!");
					return FALSE;
				}
				nBoltNum/=2;
			}
			CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA* pLayoutPara=NULL,*pMirLayoutPara=NULL;
			pLayoutPara=pTubeEndLjDlg->LayoutBoltParaList.append(pTubeEndLjDlg->LayoutBoltPara);
			if(pTubeEndLjDlg->m_iTubeEndLjStyle==5&&pTubeEndLjDlg->LayoutBoltPara.m_bMirLayout)
			{	//十字插板，默认按单行布置
				double fColSpace=pTubeEndLjDlg->LayoutBoltPara.m_fColSpace;
				double fSpecialSpace=pTubeEndLjDlg->LayoutBoltPara.m_fSpecialSpace;
				pMirLayoutPara=pTubeEndLjDlg->LayoutBoltParaList.append(*pLayoutPara);
				pMirLayoutPara->m_posFirstBolt.x+=((nBoltNum-1)*fColSpace+fSpecialSpace);
			}
		}
		else	
		{	//更新螺栓布置参数
			int iCurSel=pTubeEndLjDlg->m_listBoltParam.GetSelectedItem();
			CSuperGridCtrl::CTreeItem *pCurItem=pTubeEndLjDlg->m_listBoltParam.GetTreeItem(iCurSel);
			if(pCurItem==NULL)
				return FALSE;
			CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA *pSelLsPara=(CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA*)pCurItem->m_idProp;
			if(pSelLsPara==NULL)
			{
				AfxMessageBox("没有选中螺栓布置参数，更新失败!");
				return FALSE;
			}
			*pSelLsPara=pTubeEndLjDlg->LayoutBoltPara;
		}
		pTubeEndLjDlg->UpdateLayoutBoltPara();
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
// CDesignTubeEndLjDlg message handlers
IMPLEMENT_PROP_FUNC(CDesignTubeEndLjDlg);
const DWORD HASHTABLESIZE = 500;
void CDesignTubeEndLjDlg::InitPropHashtable()
{
	int i,id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);
	//外形参数
	AddPropItem("m_iPartType",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	AddPropItem("m_iStdPartLevel",PROPLIST_ITEM(id++,"法兰列表","级别对应板应力系数，1表示应力系数为100%,5表示应力系数为%50，7表示应力系数为70%。"));
	AddPropItem("m_iStdPartLevel2",PROPLIST_ITEM(id++,"对接法兰列表","级别对应板应力系数，1表示应力系数为100%,5表示应力系数为%50，7表示应力系数为70%。"));
	AddPropItem("m_cStdPartMat",PROPLIST_ITEM(id++,"选择材质","插板的选择材质(钢管材质)",MakeMaterialMarkSetString()));
	AddPropItem("m_bCoupleFLCreate",PROPLIST_ITEM(id++,"组对设计","是否设计对接法兰","0.单侧法兰|1.成对设计"));
	AddPropItem("m_iWrapPlateType",PROPLIST_ITEM(id++,"包板类型","十字插板包板类型","0.钢板|1.角钢"));
	AddPropItem("Profile",PROPLIST_ITEM(id++,"外形参数","外形参数"));
	for(i=0;i<26;i++)
	{
		AddPropItem(CXhChar16("%c",'A'+i),PROPLIST_ITEM(id++,"构件类型","构件类型"));
		AddPropItem(CXhChar16("%c",'a'+i),PROPLIST_ITEM(id++,"构件类型","构件类型"));
	}
	//平面切割
	AddPropItem("m_hWeldLinkObj",PROPLIST_ITEM(id++,"端头焊接构件","与钢管切割端面直接焊接的构件"));
	AddPropItem("cutPlaneOrigin",PROPLIST_ITEM(id++,"切割面基点","钢管切割面基点坐标"));
	AddPropItem("cutPlaneOrigin.x",PROPLIST_ITEM(id++,"X坐标","钢管切割面基点X轴方向坐标"));
	AddPropItem("cutPlaneOrigin.y",PROPLIST_ITEM(id++,"Y坐标","钢管切割面基点Y轴方向坐标"));
	AddPropItem("cutPlaneOrigin.z",PROPLIST_ITEM(id++,"Z坐标","钢管切割面基点Z轴方向坐标"));
	AddPropItem("cutPlaneNorm",PROPLIST_ITEM(id++,"切割面法向","钢管切割面法线方向"));
	AddPropItem("cutPlaneNorm.x",PROPLIST_ITEM(id++,"X坐标","钢管切割面法线方向X轴方向坐标"));
	AddPropItem("cutPlaneNorm.y",PROPLIST_ITEM(id++,"Y坐标","钢管切割面法线方向Y轴方向坐标"));
	AddPropItem("cutPlaneNorm.z",PROPLIST_ITEM(id++,"Z坐标","钢管切割面法线方向Z轴方向坐标"));
	//钢管相贯
	AddPropItem("TubeTransect",PROPLIST_ITEM(id++,"钢管相贯","钢管相贯"));
	AddPropItem("m_bFaceToFace",PROPLIST_ITEM(id++,"对接相贯","对接相贯","是|否"));
	AddPropItem("m_hDatumTube1",PROPLIST_ITEM(id++,"相贯钢管1","相贯钢管1"));
	AddPropItem("m_hDatumTube2",PROPLIST_ITEM(id++,"相贯钢管2","相贯钢管2"));
	//基本信息
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"基本信息","基本信息"));
	AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号","构件编号"));
	AddPropItem("sCodeName",PROPLIST_ITEM(id++,"插板代号",""));
	AddPropItem("sCoupleFlPlateNo",PROPLIST_ITEM(id++,"对接法兰编号","对接法兰编号"));
	AddPropItem("sWeldPlatePartNo",PROPLIST_ITEM(id++,"立板编号","立板编号"));
	AddPropItem("sWrapPlatePartNo",PROPLIST_ITEM(id++,"包板编号","包板编号"));
	AddPropItem("iSeg",PROPLIST_ITEM(id++,"段号"));
	AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质","材质",MakeMaterialMarkSetString()));
	AddPropItem("thick",PROPLIST_ITEM(id++,"厚度","法兰底板或钢管插板厚度,mm"));
	AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数","统材系数"));
	//钢管端连接参数
	AddPropItem("TubeEndPos",PROPLIST_ITEM(id++,"钢管端连接位置"));
	AddPropItem("m_fWeldHeight",PROPLIST_ITEM(id++,"焊缝高度","焊缝高度"));
	AddPropItem("m_fNormOffset",PROPLIST_ITEM(id++,"工作面法向偏移量","工作面法向偏移量"));
	AddPropItem("m_fOddment",PROPLIST_ITEM(id++,"正负头","钢管始端正负头,端连接为法兰时钢管正负头=钢管初始化正负头-法兰高出钢管部分的高度"));
	AddPropItem("m_fTubeNormOffset",PROPLIST_ITEM(id++,"法向偏移量","钢管法向偏移量"));
	AddPropItem("m_fTubeEccentricDist",PROPLIST_ITEM(id++,"偏心距","钢管偏心距"));
	//装配参数
	AddPropItem("SetUpPara",PROPLIST_ITEM(id++,"装配参数","装配参数"));
	AddPropItem("H1",PROPLIST_ITEM(id++,"H1","平焊法兰钢管端与法兰底面的距离"));
	AddPropItem("MotherPlateNorm",PROPLIST_ITEM(id++,"基板工作法向","基板工作法向计算方式",CStdVector::MakeVectorSetStr()));
	AddPropItem("MotherPlateNorm.x",PROPLIST_ITEM(id++,"X坐标分量","基板法向X坐标分量"));
	AddPropItem("MotherPlateNorm.y",PROPLIST_ITEM(id++,"Y坐标分量","基板法向Y坐标分量"));
	AddPropItem("MotherPlateNorm.z",PROPLIST_ITEM(id++,"Z坐标分量","基板法向Z坐标分量"));
	AddPropItem("UpVertPlateNorm",PROPLIST_ITEM(id++,"1号立板工作法向","垂直于基板的1号立板工作法向计算方式",CStdVector::MakeVectorSetStr()));
	AddPropItem("UpVertPlateNorm.x",PROPLIST_ITEM(id++,"X坐标分量","1号立板法向X坐标分量"));
	AddPropItem("UpVertPlateNorm.y",PROPLIST_ITEM(id++,"Y坐标分量","1号立板法向Y坐标分量"));
	AddPropItem("UpVertPlateNorm.z",PROPLIST_ITEM(id++,"Z坐标分量","1号立板法向Z坐标分量"));
	AddPropItem("DownVertPlateNorm",PROPLIST_ITEM(id++,"2号立板工作法向","垂直于基板的2号立板工作法向计算方式",CStdVector::MakeVectorSetStr()));
	AddPropItem("DownVertPlateNorm.x",PROPLIST_ITEM(id++,"X坐标分量","2号立板法向X坐标分量"));
	AddPropItem("DownVertPlateNorm.y",PROPLIST_ITEM(id++,"Y坐标分量","2号立板法向Y坐标分量"));
	AddPropItem("DownVertPlateNorm.z",PROPLIST_ITEM(id++,"Z坐标分量","2号立板法向Z坐标分量"));
	//法线设计参数
	AddPropItem("design_axis_z",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	AddPropItem("design_axis_z.norm_style",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	AddPropItem("design_axis_z.hVicePart",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	AddPropItem("design_axis_z.hCrossPart",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	AddPropItem("design_axis_z.norm_wing",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	AddPropItem("design_axis_z.direction",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	AddPropItem("design_axis_z.rot_ang",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	//AddPropItem("design_axis_z.VectorType",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	//AddPropItem("design_axis_z.vector.x",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	//AddPropItem("design_axis_z.vector.y",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	//AddPropItem("design_axis_z.vector.z",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	AddPropItem("design_axis_z.nearVectorType",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	AddPropItem("design_axis_z.nearVector.x",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	AddPropItem("design_axis_z.nearVector.y",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	AddPropItem("design_axis_z.nearVector.z",PROPLIST_ITEM(id++,"构件类型","构件类型"));
	//螺栓布置参数
	AddPropItem("LayoutBoltParam",PROPLIST_ITEM(id++,"螺栓布置信息","","添加螺栓矩阵|更新螺栓矩阵"));
	AddPropItem("m_nBoltD",PROPLIST_ITEM(id++,"螺栓直径","螺栓直径","12|16|20|24|27|30|"));
	AddPropItem("m_iDriection",PROPLIST_ITEM(id++,"螺栓朝向","螺栓朝向","0.朝外|1.朝内"));
	AddPropItem("m_fTubeR",PROPLIST_ITEM(id++,"钢管半径","钢管半径"));
	AddPropItem("m_iStartEdge",PROPLIST_ITEM(id++,"起始边","布置法兰螺栓时的起始边,右侧示意图中,绿色三角标志位基准边位置，黑色三角标注为焊道线位置","0.基准边|1.焊道线"));
	AddPropItem("m_fOffsetAngle",PROPLIST_ITEM(id++,"首螺栓偏移角","首螺栓相对起始边的偏移角"));
	AddPropItem("m_fBoltLayoutD",PROPLIST_ITEM(id++,"所在圆直径","螺栓所在圆直径D"));
	AddPropItem("m_nBoltNum",PROPLIST_ITEM(id++,"螺栓个数","螺栓个数"));
	AddPropItem("m_bStaggerLayout",PROPLIST_ITEM(id++,"交叉布置","布置双排螺栓时交错布置水平面与竖直面螺栓","是|否"));
	AddPropItem("m_bMirLayout",PROPLIST_ITEM(id++,"对称布置","对称布置特殊间距两侧螺栓","是|否"));
	AddPropItem("m_fSpecialSpace",PROPLIST_ITEM(id++,"特殊间距","特殊间距"));
	AddPropItem("m_nRowNum",PROPLIST_ITEM(id++,"螺栓行数","螺栓行数"));
	AddPropItem("m_nColNum",PROPLIST_ITEM(id++,"螺栓列数","螺栓列数"));
	AddPropItem("m_fRowSpace",PROPLIST_ITEM(id++,"螺栓行距","螺栓行距"));
	AddPropItem("m_fColSpace",PROPLIST_ITEM(id++,"螺栓列距","螺栓列距"));
	AddPropItem("m_posFirstBolt.x",PROPLIST_ITEM(id++,"首螺栓X坐标","首螺栓X坐标"));
	AddPropItem("m_posFirstBolt.y",PROPLIST_ITEM(id++,"首螺栓Y坐标","首螺栓Y坐标"));
	//附加板参数
	AddPropItem("m_bCreateAttachPlate",PROPLIST_ITEM(id++,"生成肋板","是否生成肋板","是|否"));
	AddPropItem("AttachPlateProfile",PROPLIST_ITEM(id++,"肋板外形参数","肋板外形参数"));
	AddPropItem("AttachPlatePara.A",PROPLIST_ITEM(id++,"竖直高度A"));
	AddPropItem("AttachPlatePara.B",PROPLIST_ITEM(id++,"底边宽度B"));
	AddPropItem("AttachPlatePara.C",PROPLIST_ITEM(id++,"台阶高度C"));
	AddPropItem("AttachPlatePara.D",PROPLIST_ITEM(id++,"顶部宽度D"));
	AddPropItem("AttachPlatePara.E",PROPLIST_ITEM(id++,"切角宽度E"));
	AddPropItem("AttachPlatePara.F",PROPLIST_ITEM(id++,"孔径间隙F"));
	AddPropItem("AttachPlatePara.G",PROPLIST_ITEM(id++,"孔径间隙G"));
	AddPropItem("AttachPlatePara.H",PROPLIST_ITEM(id++,"孔径H"));
	AddPropItem("AttachPlatePara.m_iCutMode",PROPLIST_ITEM(id++,"倒角模式","","0.切角|1.切弧"));
	AddPropItem("AttachPlatePara.m_nThick",PROPLIST_ITEM(id++,"厚度","肋板厚度"));
	AddPropItem("AttachPlateBasicInfo",PROPLIST_ITEM(id++,"肋板基本信息","肋板基本信息"));
	AddPropItem("AttachPlatePara.cMaterial",PROPLIST_ITEM(id++,"材质","肋板材质",MakeMaterialMarkSetString()));
	AddPropItem("AttachPlatePara.sPartNo",PROPLIST_ITEM(id++,"编号","肋板编号"));
	AddPropItem("AttachPlatePara.m_fAngle",PROPLIST_ITEM(id++,"偏移角度","法兰肋板首块肋板相对于基准边的偏移角度"));
	AddPropItem("AttachPlatePara.m_nPlateNum",PROPLIST_ITEM(id++,"肋板个数","肋板个数"));
	AddPropItem("AttachPlatePara.m_bUseAngle",PROPLIST_ITEM(id++,"启动角度参数α","启动角度参数α","是|否"));
	AddPropItem("AttachPlatePara.m_bMirCreate",PROPLIST_ITEM(id++,"对称生成","对称生成是自插板另一侧的肋板","是|否"));
	//填板参数
	AddPropItem("m_bCreateFillPlate",PROPLIST_ITEM(id++,"生成垫板","与十字插板相连的节点板厚度大于插板基板厚度，需要生成垫板","是|否"));
	AddPropItem("FillPlateBasicInfo",PROPLIST_ITEM(id++,"垫板参数","垫板参数"));
	AddPropItem("FillPlatePara.cMat",PROPLIST_ITEM(id++,"材质","垫板材质",MakeMaterialMarkSetString()));
	AddPropItem("FillPlatePara.sPartNo",PROPLIST_ITEM(id++,"编号","构垫板编号"));
	AddPropItem("FillPlatePara.nThick",PROPLIST_ITEM(id++,"厚度","垫板厚度"));
	AddPropItem("FillPlatePara.nNodePlateThick",PROPLIST_ITEM(id++,"厚度","与十字插板相连的节点板厚度"));
	//节点板参数
	AddPropItem("NodePlateBasicInfo",PROPLIST_ITEM(id++,"基本参数","基本参数"));
	AddPropItem("NodePlatePara.cMaterial",PROPLIST_ITEM(id++,"材质","节点板材质"));
	AddPropItem("NodePlatePara.sPartNo",PROPLIST_ITEM(id++,"编号","节点板编号"));
	AddPropItem("NodePlatePara.m_nThick",PROPLIST_ITEM(id++,"厚度","节点板厚度"));
	AddPropItem("NodePlatePara.m_nRollHeight",PROPLIST_ITEM(id++,"构件类型","构件类型"));
}
int CDesignTubeEndLjDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=FALSE;
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
BOOL CDesignTubeEndLjDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_pCurLineTube==NULL||m_pBaseNode==NULL)
		return FALSE;
	//初始化属性栏
	InitPropHashtable();	//初始化属性哈希表
	m_propList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xSubPropList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	//
	m_listBoltParam.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_listBoltParam.SetDblclkDisplayCellCtrl(TRUE);	//设置为双击显示可编辑控件
	m_listBoltParam.SetModifyValueFunc(FireModifyValue);
	m_listBoltParam.SetItemChangedFunc(FireItemChanged);
	m_listBoltParam.SetDeleteItemFunc(FireDeleteItem);
	m_listBoltParam.SetImageWidth(0);	//无缩进
	//
	m_paramPlate.SetBelongModel(console.GetActiveModel());	
	if(work_norm.IsZero())	//未设置工作面法线时默认工作面法线为钢管的工作面法线
		work_norm=m_pCurLineTube->WorkPlaneNorm();
	TUBEJOINT *pCurJoint=NULL;
	DESIGNODDMENT* pCurOddment=NULL;
	if(m_pCurLineTube->pStart==m_pBaseNode)
	{
		m_bStartEnd=TRUE;
		m_pRelatedTube=FindTubeInLine(m_pCurLineTube,m_bStartEnd);
		setupInfo.m_fTubeEccentricDist=m_pCurLineTube->desStartPos.fEccentricDist;
		setupInfo.m_fTubeNormOffset=m_pCurLineTube->desStartPos.fNormOffset;
		setupInfo.m_fOddment=m_pCurLineTube->startOdd(true);
		pCurJoint=&m_pCurLineTube->m_tJointStart;
		pCurOddment=&m_pCurLineTube->desStartOdd;
	}
	else 
	{
		m_bStartEnd=FALSE;
		m_pRelatedTube=FindTubeInLine(m_pCurLineTube,m_bStartEnd);
		setupInfo.m_fTubeEccentricDist=m_pCurLineTube->desEndPos.fEccentricDist;
		setupInfo.m_fTubeNormOffset=m_pCurLineTube->desEndPos.fNormOffset;
		setupInfo.m_fOddment=m_pCurLineTube->endOdd(true);
		pCurJoint=&m_pCurLineTube->m_tJointEnd;
		pCurOddment=&m_pCurLineTube->desEndOdd;
	}
	if(!m_bInernalStart)
	{	//判断当前设计端是否已存在端连接
		m_pParamPlate=NULL;
		m_bHasExistEndLj=FALSE;
		m_bCreateFillPlate=FALSE;
		m_bFaceToFace=FALSE;
		if(pCurJoint->hViceLinkObj<0)
			m_bFaceToFace=TRUE;
		cutPlaneNorm=pCurJoint->cutPlaneNorm;
		cutPlaneOrigin=pCurJoint->cutPlanePos;
		m_hWeldLinkObj=(pCurJoint->type==0)?pCurJoint->hLinkObj:0;	//当前连接端为平面切割时赋值焊接
		m_hDatumTube1=pCurJoint->hLinkObj;
		if(pCurJoint->hViceLinkObj>0)
			m_hDatumTube2=pCurJoint->hViceLinkObj;
		if(pCurJoint->hLinkObj>0)
		{
			if(pCurJoint->type>1)
				m_pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pCurJoint->hLinkObj,CLS_PARAMPLATE);
			else 
				m_bHasExistEndLj=TRUE;	
			if(m_pParamPlate)
				m_bHasExistEndLj=TRUE;
		}
		if(m_bHasExistEndLj)
			m_iTubeEndLjStyle=pCurJoint->type;	//端连接类型
		UpdateDesignParam();	//初始化标准构件设计参数
		if(m_iTubeEndLjStyle==5)	//十字插板
		{
			setupInfo.workNorm.norm_style=5;
			setupInfo.workNorm.rot_ang=0;
			setupInfo.workNorm.hVicePart=m_pCurLineTube->handle;
			setupInfo.workNorm.UpdateVector(console.GetActiveModel());
			setupInfo.workNormUpVert.norm_style=setupInfo.workNormDownVert.norm_style=4;
			setupInfo.workNormUpVert.nearVector.Set(0,-1);
			setupInfo.workNormDownVert.nearVector.Set(0,1);
			setupInfo.workNormUpVert.vector=setupInfo.workNormUpVert.nearVector;
			setupInfo.workNormDownVert.vector=setupInfo.workNormDownVert.nearVector;
			if(m_pCurLineTube->pStart==m_pBaseNode)
				m_paramPlate.ucs.axis_x=m_pCurLineTube->Start()-m_pCurLineTube->End();
			else
				m_paramPlate.ucs.axis_x=m_pCurLineTube->End()-m_pCurLineTube->Start();
			m_paramPlate.ucs.axis_z=setupInfo.workNorm.vector;
			m_paramPlate.ucs.axis_y=m_paramPlate.ucs.axis_z^m_paramPlate.ucs.axis_x;
			normalize(m_paramPlate.ucs.axis_y);
			m_paramPlate.ucs.axis_x=m_paramPlate.ucs.axis_y^m_paramPlate.ucs.axis_z;
			normalize(m_paramPlate.ucs.axis_x);
			vector_trans(setupInfo.workNormUpVert.vector,m_paramPlate.ucs,TRUE);
			vector_trans(setupInfo.workNormDownVert.vector,m_paramPlate.ucs,TRUE);
		}
		if(m_iTubeEndLjStyle>=3&&m_iTubeEndLjStyle<=5&&
			pCurOddment->m_iOddCalStyle==0&&!IsHasExistEndLj())
		{
			if(pCurOddment->m_hRefPart1==0&&m_pCurLineTube->handle!=m_pBaseNode->hFatherPart)
				pCurOddment->m_hRefPart1=m_pBaseNode->hFatherPart;
			pCurOddment->m_fCollideDist=setupInfo.m_fWeldHeight+g_sysPara.VertexDist;
			setupInfo.m_fOddment=m_pCurLineTube->CalOddment(m_bStartEnd,&m_paramPlate,true);
		}
		//已存在端连接且端连接为十字插板时搜索与该十字插板关联的节点板
		//生成十字插板附加肋板时，肋板的焊接父构件为十字插板关联的节点板 wht 10-08-02
		if(m_pParamPlate&&m_pParamPlate->m_iParamType==TYPE_XEND)
		{
			CLDSPlate *pWeldPlate=NULL;
			CLDSModule *pActiveModule=Ta.GetActiveModule();
			for(pWeldPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pWeldPlate;pWeldPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
			{	
				if(pActiveModule&&!pActiveModule->IsSonPart(pWeldPlate))
					continue;	//非当前模型中的构件 
				CDesignLjPartPara *pLjPara=pWeldPlate->designInfo.FromHandle(m_pCurLineTube->handle);
				if(pLjPara==NULL)
					continue;
				if((m_bStartEnd&&pLjPara->start0_end1!=0)||
					(!m_bStartEnd&&pLjPara->start0_end1!=1))
					continue;
				AttachPlatePara.m_pWeldParentPlate=pWeldPlate;
				break;
			}
		}
		UpdateLayoutBoltPara();
		DisplayPropertyList();
		RefreshCtrlState();
	}
	else
	{	//内部启动，完成选择对象的后续操作
		FinishSelectObjOper();
		DisplayPropertyList();
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		if(pItem)
		{
			m_propList.SetFocus();	//属性栏获得焦点
			m_propList.SetCurSel(pItem->m_iIndex);	//设置选中项
		}
		else
		{
			RefreshCtrlState(FALSE);
			pItem=m_xSubPropList.FindItemByPropId(m_idEventProp,NULL);
			m_xSubPropList.SetFocus();
			m_xSubPropList.SetCurSel(pItem->m_iIndex);
			DisplayChildPropertyList(NULL);
			if(m_xSubPropList.m_nObjClassTypeId==0)
			{
				pItem=m_propList.FindItemByPropId(GetPropID("MotherPlateNorm"),NULL);
				if(pItem)
					m_propList.SetCurSel(pItem->m_iIndex);
			}
			else if(m_xSubPropList.m_nObjClassTypeId==1)
			{
				pItem=m_propList.FindItemByPropId(GetPropID("UpVertPlateNorm"),NULL);
				if(pItem)
					m_propList.SetCurSel(pItem->m_iIndex);
			}
			else if(m_xSubPropList.m_nObjClassTypeId==1)
			{
				pItem=m_propList.FindItemByPropId(GetPropID("DownVertPlateNorm"),NULL);
				if(pItem)
					m_propList.SetCurSel(pItem->m_iIndex);
			}
		}
	}
	UpdateSketchMap();
	//生成画笔
	if(solid_pen.GetSafeHandle()==0)
		solid_pen.CreatePen(PS_SOLID,1,RGB(0,0,200));
	if(dash_pen.GetSafeHandle()==0)
		dash_pen.CreatePen(PS_DASH,1,RGB(0,0,0));
	if(dash_dot_pen.GetSafeHandle()==0)
		dash_dot_pen.CreatePen(PS_DASHDOT,1,RGB(255,0,0));
	if(green_solid_pen.GetSafeHandle()==0)
		green_solid_pen.CreatePen(PS_SOLID,2,RGB(0,255,0));
	if(green_dash_pen.GetSafeHandle()==0)
		green_dash_pen.CreatePen(PS_DASH,1,RGB(0,255,0));
	if(red_solid_pen.GetSafeHandle()==0)
		red_solid_pen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	if(blue_solid_pen.GetSafeHandle()==0)
		blue_solid_pen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	if(purple_solid_pen.GetSafeHandle()==0)	//紫色画笔
		purple_solid_pen.CreatePen(PS_SOLID,1,RGB(128,0,255));
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
	return TRUE;  
}
INSERT_PLATE_PARAM* CDesignTubeEndLjDlg::QueryStdInsertPlate(int iLevel,char cMat)
{
	if(m_iTubeEndLjStyle==3)
		return FindUEndParam(m_pCurLineTube->GetDiameter(),m_pCurLineTube->GetThick(),cMat,iLevel);
	else if(m_iTubeEndLjStyle==4)
		return FindRollEndParam(m_pCurLineTube->GetDiameter(),m_pCurLineTube->GetThick(),cMat,iLevel);
	else if(m_iTubeEndLjStyle==5)
		return FindXEndParam(m_pCurLineTube->GetDiameter(),m_pCurLineTube->GetThick(),cMat,iLevel);
	else
		return NULL;
}
INSERT_PLATE_PARAM* CDesignTubeEndLjDlg::QueryStdInsertPlateByMat(char cMat,int* pLevelArr/*=NULL*/)
{
	INSERT_PLATE_PARAM* pInsertPlateParam=NULL;
	int index=0;
	for(int i=1;i<=9;i++)
	{	
		INSERT_PLATE_PARAM *pChaBanParam=QueryStdInsertPlate(i,cMat);
		if(pChaBanParam==NULL)
			continue;
		if(pLevelArr)
			pLevelArr[index++]=pChaBanParam->iLevelMark;
		if(pInsertPlateParam==NULL||i>pInsertPlateParam->iLevelMark)
			pInsertPlateParam=pChaBanParam;	//优先用小级别插板(板子小）
	}
	return pInsertPlateParam;
}
FL_PARAM* CDesignTubeEndLjDlg::QueryStdFlDParaByMat(char cMat,int* pLevelArr/*=NULL*/)
{
	FL_PARAM* pFlDPara=NULL;
	double tube_d_m=m_pCurLineTube->GetDiameter();
	double tube_d_c=m_pRelatedTube?m_pRelatedTube->GetDiameter():tube_d_m;
	if(tube_d_m>tube_d_c)
		tube_d_c=tube_d_m;
	int index=0;
	for(int i=1;i<9;i++)
	{
		char cLevel='0'+i;
		FL_PARAM* pFindFld=FindFlDParam(tube_d_m,tube_d_c,cMat,cLevel);
		if(pFindFld==NULL)
			continue;
		if(pLevelArr)
			pLevelArr[index++]=i;
		if(pFlDPara==NULL||pFindFld->cLevelMark>pFlDPara->cLevelMark)
			pFlDPara=pFindFld;
	}
	return pFlDPara;
}
//初始化标准构件设计参数 
void CDesignTubeEndLjDlg::InitStdPartDesignParam()
{
	m_pFlDParam=m_pFlPParam=NULL;
	m_pFlDParam2=m_pFlPParam2=NULL;
	m_pRigidFlParam=m_pFlexibleFlParam=NULL;
	m_pRigidFlParam2=m_pFlexibleFlParam2=NULL;
	m_pInsertPlateParam=NULL;
	m_arrStdPartLevel.RemoveAll();
	m_arrStdFlDLevelM.RemoveAll();
	m_arrStdFlDLevelC.RemoveAll();
	m_bCoupleFLCreate=true;
	if(m_iTubeEndLjStyle==2)
	{	//法兰
		double tube_d=m_pCurLineTube->GetDiameter();
		double couple_d=m_pRelatedTube?m_pRelatedTube->GetDiameter():tube_d;
		FL_COUPLE couple;
		if(QueryFlCouple(tube_d,couple_d,'D',&couple))
		{
			m_pFlDParam=FindFlDParam(couple.mainFlName);
			m_pFlDParam2=FindFlDParam(couple.coupleFlName);
		}
		if(QueryFlCouple(tube_d,couple_d,'P',&couple))
		{	
			m_pFlPParam=FindFlPParam(couple.mainFlName);
			m_pFlPParam2=FindFlPParam(couple.coupleFlName);
		}
		if(QueryFlCouple(tube_d,couple_d,'G',&couple))
		{	
			m_pRigidFlParam=FindFlGParam(couple.mainFlName);
			m_pRigidFlParam2=FindFlGParam(couple.coupleFlName);
		}
		if(QueryFlCouple(tube_d,couple_d,'R',&couple))
		{	
			m_pFlexibleFlParam=FindFlRParam(couple.mainFlName);
			m_pFlexibleFlParam2=FindFlRParam(couple.coupleFlName);
		}
		if(m_pFlDParam!=NULL)
			m_iPartType=2;
		else if(m_pFlPParam!=NULL)
			m_iPartType=1;
		else if(m_pRigidFlParam!=NULL)
			m_iPartType=3;
		else if(m_pFlexibleFlParam!=NULL)
			m_iPartType=4;
		else if(m_pFlPParam==NULL&&m_pFlDParam==NULL&&m_pRigidFlParam==NULL&&m_pFlexibleFlParam==NULL)
			m_iPartType=0;	//未找到合理标准构件时 构件类型初始化为0
		//设计对焊法兰时，获取对焊级别
		if(m_iPartType==2)
		{	
			double tube_d_m=0,tube_d_c=0;
			//统计主管对焊法兰的级别
			m_cFlLevel1=m_pFlDParam->cLevelMark;
			m_cStdPartMat=m_pFlDParam->cMaterial;
			tube_d_m=m_pCurLineTube->GetDiameter();
			tube_d_c=m_pRelatedTube?m_pRelatedTube->GetDiameter():tube_d_m;
			if(tube_d_m>tube_d_c)
				tube_d_c=tube_d_m;
			for(int i=1;i<9;i++)
			{
				char cLevel='0'+i;
				if(FindFlDParam(tube_d_m,tube_d_c,m_cStdPartMat,cLevel))
					m_arrStdFlDLevelM.Add(CXhChar16("%C",cLevel));
			}
			//统计对接对焊法兰的级别
			if(m_pRelatedTube && m_pFlDParam2)
			{
				m_cFlLevel2=m_pFlDParam2->cLevelMark;
				tube_d_m=m_pRelatedTube->GetDiameter();
				tube_d_c=m_pCurLineTube->GetDiameter();
				if(tube_d_m>tube_d_c)
					tube_d_c=tube_d_m;
				for(int i=1;i<9;i++)
				{
					char cLevel='0'+i;
					if(FindFlDParam(tube_d_m,tube_d_c,cLevel))
						m_arrStdFlDLevelC.Add(CXhChar16("%C",cLevel));
				}
			}
		}
	}
	else if(m_iTubeEndLjStyle==3||m_iTubeEndLjStyle==4||m_iTubeEndLjStyle==5)
	{
		int xLevelArr[9]={0};
		m_pInsertPlateParam=QueryStdInsertPlateByMat(m_pCurLineTube->cMaterial,xLevelArr);
		if(m_pInsertPlateParam)	
		{	//当前标准构件级别
			m_iPartType=1;
			m_iStdPartLevel=m_pInsertPlateParam->iLevelMark;
			m_cStdPartMat=m_pInsertPlateParam->cTubeMat;	//查找标准插板依赖的是钢管材质
			//统计插板级别
			for(int i=0;i<9;i++)
			{
				if(xLevelArr[i]==0)
					break;
				m_arrStdPartLevel.Add(CXhChar16("%d",xLevelArr[i]));
			}
		}
		else
			m_iPartType=0;	//未找到合理标准构件时 构件类型初始化为0
	}
	if(m_pParamPlate&&m_bHasExistEndLj)
	{	//已存在端连接,更新标准构件级别
		m_bCoupleFLCreate=false;
		CLDSStdPartModel stdPartModel;
		if(m_pParamPlate->GetStdPartModel(&stdPartModel))
		{
			if(m_pParamPlate->m_iParamType==TYPE_FLG)
			{
				m_iPartType=3;
				m_pRigidFlParam=FindFlGParam(m_pParamPlate->GetPartNo());
			}
			else if(m_pParamPlate->m_iParamType==TYPE_FLR) 
			{
				m_iPartType=4;
				m_pFlexibleFlParam=FindFlRParam(m_pParamPlate->GetPartNo());
			}
			else if(m_pParamPlate->m_iParamType==TYPE_FLD)
			{
				m_iPartType=2;
				m_cFlLevel1=stdPartModel.param.fl_param.cLevelMark;
				m_pFlDParam=FindFlDParam(m_pParamPlate->GetPartNo());
			}
			else if(m_pParamPlate->m_iParamType==TYPE_FLP)
			{
				m_iPartType=1;
				m_pFlPParam=FindFlPParam(m_pParamPlate->GetPartNo());
			}
			else
			{	
				m_iPartType=1;
				m_iStdPartLevel=stdPartModel.param.insert_plate_param.iLevelMark;
				m_cStdPartMat=stdPartModel.param.insert_plate_param.cTubeMat;	//查找标准插板依赖的是钢管材质
				if(m_pParamPlate->m_iParamType==TYPE_ROLLEND)
					m_pInsertPlateParam=FindRollEndParam(m_pParamPlate->GetPartNo());
				else if(m_pParamPlate->m_iParamType==TYPE_UEND)
					m_pInsertPlateParam=FindUEndParam(m_pParamPlate->GetPartNo());
				else if(m_pParamPlate->m_iParamType==TYPE_XEND)
					m_pInsertPlateParam=FindXEndParam(m_pParamPlate->GetPartNo());
			}
		}
		else
			m_iPartType=0;
	}
}

//初始化设计参数
void CDesignTubeEndLjDlg::UpdateDesignParam(BOOL bInitPartType/*=TRUE*/)
{
	CLogErrorLife loglife;
	if(bInitPartType)
		InitStdPartDesignParam();
	//初始化插板工作平面旋转角度，应放在前边初始化 wht 10-11-09 
	setupInfo.workNorm.rot_ang=0;
	if(m_iTubeEndLjStyle==4&&!m_bSpecialFlRayPole)
	{	//保证槽型插板卷边方向朝外 wht 10-10-21
		if(work_norm*m_pCurLineTube->WorkPlaneNorm()>0)
			setupInfo.workNorm.rot_ang=180;	//工作面法线方向与钢管基准面法线方向相同
		else 
			setupInfo.workNorm.rot_ang=0;		//相反
	}
	//初始化外形参数 wht 10-08-02
	if(m_bHasExistEndLj&&m_pParamPlate)	//已存在端连接
		UpdateDesignParamBySrcPlate(m_pParamPlate);
	else
	{
		double A=10,R=10,W=40,H=0,D=0,L=520,M=260,N=40,S=20,T=10;
		double fTubeD = m_pCurLineTube->GetDiameter();
		m_paramPlate.m_bStdPart=FALSE;	
		m_paramPlate.iSeg=m_pCurLineTube->iSeg;
		m_paramPlate.cMaterial=m_pCurLineTube->cMaterial;
		m_paramPlate.thick=(long)T;
		m_paramPlate.SetPartNo("");
		strcpy(sCoupleFlPlateNo,"");
		m_paramPlate.EmptyDesignItems();//清空设计参数		
		//
		LayoutBoltParaList.Empty();
		LayoutBoltPara.m_iStartEdge=m_pCurLineTube->m_bHasWeldRoad?1:0;
		LayoutBoltPara.m_fOffsetAngle=0;
		LayoutBoltPara.m_iDirection=0;
		if(m_bDesignNodePlate)
		{
			NodePlatePara.cMaterial=m_pCurLineTube->cMaterial;
			NodePlatePara.m_nThick=m_paramPlate.thick;
		}
		if(m_iTubeEndLjStyle==2)	//法兰
		{
			if(m_iPartType==0)
			{	//普通法兰
				m_paramPlate.m_iParamType=TYPE_FL;
				H = 0.5*T;
				W=fTubeD+80;
				R=fTubeD*0.5+1;	//法兰内半径
				//螺栓信息
				if(m_pRigidFlParam)
				{	//刚性法兰其实就是部分标准化的普通带肋法兰，可以考虑作为默认值 wjh-2017.12.28
					W=m_pRigidFlParam->fOuterD;
					LayoutBoltPara.m_fBoltLayoutD=m_pRigidFlParam->fBoltCirD;
					LayoutBoltPara.m_nBoltD=m_pRigidFlParam->nBoltD;
					LayoutBoltPara.m_nBoltNum=m_pRigidFlParam->nBoltNum;
				}
				else
				{
					LayoutBoltPara.m_fBoltLayoutD=fTubeD+20;
					LayoutBoltPara.m_nBoltD=20;
					LayoutBoltPara.m_nBoltNum=8;
				}
				//附加肋板信息
				AttachPlatePara.cMaterial=m_pCurLineTube->cMaterial;
				AttachPlatePara.m_nThick=m_paramPlate.thick;
				AttachPlatePara.m_nPlateNum=LayoutBoltPara.m_nBoltNum;
				AttachPlatePara.A=150;
				AttachPlatePara.B=0.5*W-R;
				AttachPlatePara.C=30;
				AttachPlatePara.D=30;
				AttachPlatePara.E=25;
				AttachPlatePara.F=30;
				AttachPlatePara.G=20;
				AttachPlatePara.H=0;
				AttachPlatePara.m_iCutMode=1;
				AttachPlatePara.m_fAngle=LayoutBoltPara.m_fOffsetAngle+180.0/LayoutBoltPara.m_nBoltNum;
			}
			else if(m_iPartType==1&&m_pFlPParam)
			{
				m_paramPlate.m_iParamType=TYPE_FLP;
				m_paramPlate.SetPartNo(m_pFlPParam->codeName);
				if(m_pFlPParam2)
					strcpy(sCoupleFlPlateNo,m_pFlPParam2->codeName);
				m_paramPlate.m_bStdPart=true;
				m_paramPlate.thick=(long)m_pFlPParam->C;
				m_paramPlate.cMaterial=m_pFlPParam->cMaterial;
				R=m_pFlPParam->B*0.5;
				W=m_pFlPParam->D;
				LayoutBoltPara.m_fBoltLayoutD=m_pFlPParam->K;
				LayoutBoltPara.m_nBoltD=m_pFlPParam->nBoltD;
				LayoutBoltPara.m_nBoltNum=m_pFlPParam->nBoltNum;
			}
			else if(m_iPartType==2&&m_pFlDParam)
			{
				m_paramPlate.m_iParamType=TYPE_FLD;
				FL_PARAM tem_fld=*m_pFlDParam;
				tem_fld.t=m_pCurLineTube->GetThick();
				tem_fld.UpdateCodeName();
				m_paramPlate.SetPartNo(tem_fld.codeName);
				if(m_pFlDParam2)
				{
					FL_PARAM tem_fld2=*m_pFlDParam2;
					tem_fld2.t=m_pRelatedTube?m_pRelatedTube->GetThick():tem_fld.t;
					tem_fld2.UpdateCodeName();
					strcpy(sCoupleFlPlateNo,tem_fld2.codeName);
				}
				m_paramPlate.m_bStdPart=true;
				m_paramPlate.thick=(long)m_pFlDParam->C;
				m_paramPlate.cMaterial=m_pFlDParam->cMaterial;
				R=m_pFlDParam->B*0.5;
				W=m_pFlDParam->D;
				LayoutBoltPara.m_fBoltLayoutD=m_pFlDParam->K;
				LayoutBoltPara.m_nBoltD=m_pFlDParam->nBoltD;
				LayoutBoltPara.m_nBoltNum=m_pFlDParam->nBoltNum;
			}
			else if((m_iPartType==3&&m_pRigidFlParam)||(m_iPartType==4&&m_pFlexibleFlParam))
			{
				FL_PARAM2 *pParam=NULL;
				if(m_iPartType==3)
				{
					m_paramPlate.m_iParamType=TYPE_FLG;
					pParam=m_pRigidFlParam;
					if(m_pRigidFlParam2)
						strcpy(sCoupleFlPlateNo,m_pRigidFlParam2->codeName);
				}
				else if(m_iPartType==4)
				{
					m_paramPlate.m_iParamType=TYPE_FLR;
					pParam=m_pFlexibleFlParam;
					if(m_pFlexibleFlParam2)
						strcpy(sCoupleFlPlateNo,m_pFlexibleFlParam2->codeName);
				}
				m_paramPlate.SetPartNo(pParam->codeName);
				m_paramPlate.m_bStdPart=true;
				m_paramPlate.thick=(long)pParam->fThick;
				m_paramPlate.cMaterial=pParam->cMaterial;
				R=pParam->fInnerD*0.5;
				W=pParam->fOuterD;
				LayoutBoltPara.m_fBoltLayoutD=pParam->fBoltCirD;
				LayoutBoltPara.m_nBoltD=pParam->nBoltD;
				LayoutBoltPara.m_nBoltNum=pParam->nBoltNum;
				//初始化肋板参数
				if(pParam->nRibPlateNum>0)
				{
					m_bCreateAttachPlate=TRUE;
					AttachPlatePara.m_nPlateNum=pParam->nRibPlateNum;
					AttachPlatePara.A=pParam->para.A;
					AttachPlatePara.B=pParam->para.B;
					AttachPlatePara.C=pParam->para.C;
					AttachPlatePara.D=pParam->para.D;
					AttachPlatePara.E=pParam->para.E;
					AttachPlatePara.F=pParam->para.F;
					AttachPlatePara.G=pParam->para.G;
					AttachPlatePara.H=pParam->para.H;
					AttachPlatePara.m_fAngle=LayoutBoltPara.m_fOffsetAngle+180.0/pParam->nBoltNum;
				}
				else 
					m_bCreateAttachPlate=FALSE;
			}
			//添加法兰设计参数
			m_paramPlate.SetDesignItemValue('A',360,"填充角度",PARAM_SHAPE); //法兰填充角度 wht 11-05-09
			m_paramPlate.SetDesignItemValue('R',R,"法兰内环半径",PARAM_SHAPE);
			m_paramPlate.SetDesignItemValue('W',W,"法兰外环直径",PARAM_SHAPE);
			m_paramPlate.SetDesignItemValue('H',H,"法兰高出钢管的高度",PARAM_SHAPE);
			m_paramPlate.SetDesignItemValue('D',setupInfo.workNorm.rot_ang,"基准X轴相对钢管X轴旋转角度");
			m_paramPlate.SetDesignItemValue('J',LayoutBoltPara.m_iStartEdge,"法兰基准边");
		}
		else if(m_iTubeEndLjStyle>=3&&m_iTubeEndLjStyle<=5)
		{
			if(m_pInsertPlateParam)
			{	//外形参数
				A=m_pInsertPlateParam->A;
				R=m_pInsertPlateParam->R;
				W=m_pInsertPlateParam->W;
				N=m_pInsertPlateParam->N;
				S=m_pInsertPlateParam->S;
				H=m_pInsertPlateParam->H;
				L=m_pInsertPlateParam->L;
				M=m_pInsertPlateParam->M;
				//基本信息
				m_paramPlate.thick=m_pInsertPlateParam->Thick;
				m_paramPlate.cMaterial=m_pInsertPlateParam->cMaterial;
				m_paramPlate.SetPartNo(m_iPartType!=0?m_pInsertPlateParam->codeName:"");
				if(m_iTubeEndLjStyle==5&&m_iPartType!=0)
				{
					m_paramPlate.SetPartNo(m_pInsertPlateParam->datumPlatePartNo);
					strcpy(sWeldPlatePartNo,m_pInsertPlateParam->weldPlatePartNo);
					strcpy(sWrapPlatePartNo,m_pInsertPlateParam->wrapPlatePartNo);
					strcpy(sCodeName,m_pInsertPlateParam->codeName);
				}
				m_paramPlate.m_bStdPart=(m_iPartType!=0)?true:false;
				//螺栓信息
				LayoutBoltPara.m_nBoltD=m_pInsertPlateParam->ls_param[0].iBoltD;
				for(int i=0;i<m_pInsertPlateParam->iBoltRect;i++)
				{
					LAYOUT_BOLT_PARA *pBoltPara=LayoutBoltParaList.append();
					pBoltPara->m_nBoltD=m_pInsertPlateParam->ls_param[i].iBoltD;
					pBoltPara->m_nBoltNum=m_pInsertPlateParam->ls_param[i].iBoltNum;
					pBoltPara->m_nColNum=m_pInsertPlateParam->ls_param[i].iColumn;
					pBoltPara->m_nRowNum=m_pInsertPlateParam->ls_param[i].iRow;
					pBoltPara->m_fColSpace=m_pInsertPlateParam->ls_param[i].fHorzSpace;
					pBoltPara->m_fRowSpace=m_pInsertPlateParam->ls_param[i].fVertSpace;
					pBoltPara->m_posFirstBolt.x=m_pInsertPlateParam->ls_param[i].fBoltPosX;
					pBoltPara->m_posFirstBolt.y=m_pInsertPlateParam->ls_param[i].fBoltPosY;
					pBoltPara->m_fSpecialSpace=pBoltPara->m_fColSpace;	//特殊间距初始值应等于列距 
					if(m_pInsertPlateParam->ls_param[i].iColumn==-1
						||m_pInsertPlateParam->ls_param[i].iRow==-1)
						pBoltPara->m_bStaggerLayout=TRUE;
					else
						pBoltPara->m_bStaggerLayout=FALSE;
					pBoltPara->m_bMirLayout=FALSE;
				}
			}
			else 
			{	
				m_paramPlate.SetPartNo("");
				H = fTubeD+40;
				if(m_iTubeEndLjStyle==5)
				{   //普通十字插板
					W = T;
					A = 0.5*(T+W);
					R = 0.5*T+W;
					S = L;
					N = (int)(0.5*H);
				}
				//初始化螺栓信息
				double fHalfRowSpace=ftoi(fTubeD*0.8*0.5);
				LAYOUT_BOLT_PARA *pBoltPara=LayoutBoltParaList.append();
				pBoltPara->m_nBoltD=20;
				pBoltPara->m_nBoltNum=4;
				pBoltPara->m_nColNum=2;
				pBoltPara->m_nRowNum=2;
				pBoltPara->m_fColSpace=(L-M-80);
				pBoltPara->m_fRowSpace=fHalfRowSpace*2;
				pBoltPara->m_posFirstBolt.x=M+10+30;
				pBoltPara->m_posFirstBolt.y=-fHalfRowSpace;
				pBoltPara->m_posFirstBolt.z=0;
				if(m_iTubeEndLjStyle==5)
				{	//十字插板螺栓
					pBoltPara->m_bMirLayout=FALSE;
					pBoltPara->m_bStaggerLayout=FALSE;
					pBoltPara->m_fRowSpace=60;
					pBoltPara->m_fSpecialSpace=80;
					pBoltPara->m_posFirstBolt.x=30;
					pBoltPara->m_posFirstBolt.y=30;
					pBoltPara->m_posFirstBolt.z=0;
				}
			}
			if(m_iTubeEndLjStyle==3)	//U型端板
			{
				if(NodePlatePara.m_nThick>0)	//U型插板钢管偏移量归零
					setupInfo.m_fTubeNormOffset=0;
				m_paramPlate.SetDesignItemValue('W',W,"U型插板上下两面的间距",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('L',L,"板长",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('H',H,"板宽",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('M',M,"U型插板与钢管相交的长度",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('N',N,"切角长度",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('S',S,"切角宽度",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('D',setupInfo.workNorm.rot_ang,"工作面相对钢管基准面旋转角度");
				m_paramPlate.m_iParamType=TYPE_UEND;
			}
			else if(m_iTubeEndLjStyle==4)	//槽型插板
			{
				if(NodePlatePara.m_nThick>0)	//槽型插板
				{
					setupInfo.m_fTubeNormOffset=-0.5*(m_paramPlate.thick+NodePlatePara.m_nThick);
					if(fabs(fabs(setupInfo.workNorm.rot_ang)-180)<EPS)	//槽型插板旋转180度
						setupInfo.m_fTubeNormOffset=(NodePlatePara.m_nThick+m_paramPlate.thick)*0.5;
					else if(fabs(setupInfo.workNorm.rot_ang)>EPS)
						setupInfo.m_fTubeNormOffset=0;	//非0°和180°时偏移量归零
				}
				m_paramPlate.SetDesignItemValue('W',W,"卷边的高度",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('L',L,"板长",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('H',H,"板宽",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('M',M,"板与钢管相交的长度",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('N',N,"切角长度",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('S',S,"切角宽度",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('D',setupInfo.workNorm.rot_ang,"工作面相对钢管基准面旋转角度");
				m_paramPlate.m_iParamType=TYPE_ROLLEND;
				//
				AttachPlatePara.A=L;
				AttachPlatePara.B=H*0.5;
				AttachPlatePara.C=30;
				AttachPlatePara.D=30;
				AttachPlatePara.E=30;
				AttachPlatePara.F=30;
			}
			else if(m_iTubeEndLjStyle==5)	//X型插板
			{	
				if(NodePlatePara.m_nThick>0)	//十字插板钢管偏移量归零
					setupInfo.m_fTubeNormOffset=0;
				m_paramPlate.SetDesignItemValue('L',L,"板长",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('H',H,"板宽",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('M',M,"板与钢管相交的长度",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('W',W,"十字型插板上附加板的厚度",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('N',N,"十字型插板上附加板的宽度",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('S',S,"十字型插板上附加板的长度",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('A',A,"十字型插板上附加板的定位参数A",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('R',R,"十字型插板上附加板的定位参数R",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('D',setupInfo.workNorm.rot_ang,"工作面相对钢管基准面旋转角度");
				m_paramPlate.m_iParamType=TYPE_XEND;
				//
				AttachPlatePara.m_fAngle=45;
				AttachPlatePara.A=0.5*(H-T);
				AttachPlatePara.B=S;
				AttachPlatePara.C=30;
				AttachPlatePara.D=AttachPlatePara.A-30;
				AttachPlatePara.E=30;
			}
			DESIGNODDMENT * pCurOddment=&m_pCurLineTube->desEndOdd;
			if(m_bStartEnd)
				pCurOddment=&(m_pCurLineTube->desStartOdd);
			if(pCurOddment->m_iOddCalStyle==0)//根据碰撞计算正负头 wht 05-09
				setupInfo.m_fOddment=m_pCurLineTube->CalOddment(m_bStartEnd,&m_paramPlate,true);
		}
		else if(m_iTubeEndLjStyle==0||m_iTubeEndLjStyle==1)
		{	//初始化平面切割参数以及钢管相贯参数
			if(m_pCurLineTube->pStart==m_pBaseNode)
			{	//始端
				if(m_pCurLineTube->desStartPos.endPosType==1)
					m_hDatumTube1=m_pCurLineTube->desStartPos.hDatumPart;
			}
			else
			{	//终端
				if(m_pCurLineTube->desEndPos.endPosType==1)
					m_hDatumTube1=m_pCurLineTube->desEndPos.hDatumPart;
			}
			UpdateTubeOddment();	//平面切割或钢管相贯 wht 11-05-10
		}
	}
}
//根据所选参数板的类型显示合适的图片
void CDesignTubeEndLjDlg::UpdateSketchMap(int iCurSel/*=0*/)
{
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	m_CurPlateBmp.Detach();
	if(m_iTubeEndLjStyle==2)
	{
		if(m_iPartType==1&&m_pFlPParam)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_STD_PART_FLP);
		else if(m_iPartType==2&&m_pFlDParam)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_STD_PART_FLD);
		else //if(m_iPartType==0)
		{	//普通法兰
			if(iCurSel==0||iCurSel==1)	//常规与外形
				m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_FL2);
			else if(iCurSel==2)
				m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_FL1);
			else if(iCurSel==3)	//螺栓与附加板
				m_CurPlateBmp.LoadBitmap(IDB_BMP_ELBOW_PLATE1);
		}
	}
	else if(m_iTubeEndLjStyle==3)		//U型端板
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_UEND_BOLT);
	else if(m_iTubeEndLjStyle==4)	//槽型插板
	{
		if(iCurSel<3)	//外形 螺栓
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_ROLLEND_BOLT);
		else if(iCurSel==3)			//附加板GROUP_ATTACH_PLATE-1
			m_CurPlateBmp.LoadBitmap(IDB_BMP_XEND_RIB_PLATE);
	}
	else if(m_iTubeEndLjStyle==5)	//X型插板
	{
		if(iCurSel<3)	//外形 螺栓
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_XEND_BOLT);
		else if(iCurSel==3)			//附加板GROUP_ATTACH_PLATE-1
			m_CurPlateBmp.LoadBitmap(IDB_BMP_XEND_RIB_PLATE1);
	}
	else 
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_NONE);
	pPic->SetBitmap(m_CurPlateBmp);
}
int CDesignTubeEndLjDlg::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText,sCurParam;
	GetPropStr(id,sCurParam);
	CLDSVector xVector;
	if(m_xSubPropList.m_nObjClassTypeId==0)
		xVector=setupInfo.workNorm;
	else if(m_xSubPropList.m_nObjClassTypeId==1)
		xVector=setupInfo.workNormUpVert;
	else if(m_xSubPropList.m_nObjClassTypeId==2)
		xVector=setupInfo.workNormDownVert;
	//外形参数
	if(sCurParam.GetLength()==1&&((sCurParam[0]>='A'&&sCurParam[0]<='Z')
		||(sCurParam[0]>='a'&&sCurParam[0]<='z')))
	{
		if(m_iTubeEndLjStyle==2&&((m_iPartType==1&&m_pFlPParam)
			||(m_iPartType==2&&m_pFlDParam)))
		{	//高颈法兰对应参数 wht 11-01-21
			FL_PARAM fl_param;
			if(m_iPartType==1)
				fl_param=*m_pFlPParam;
			else 
				fl_param=*m_pFlDParam;
			if(sCurParam[0]=='A')
				sText.Printf("%f",fl_param.A);
			else if(sCurParam[0]=='B')
				sText.Printf("%f",fl_param.B);
			else if(sCurParam[0]=='C')
				sText.Printf("%f",fl_param.C);
			else if(sCurParam[0]=='D')
				sText.Printf("%f",fl_param.D);
			else if(sCurParam[0]=='K')
				sText.Printf("%f",fl_param.K);
			else if(sCurParam[0]=='L')
				sText.Printf("%f",fl_param.L);
			else if(sCurParam[0]=='H')
				sText.Printf("%f",fl_param.H);
			else if(sCurParam[0]=='R')
				sText.Printf("%f",fl_param.R);
			else if(sCurParam[0]=='N')
				sText.Printf("%f",fl_param.N);
			else if(sCurParam[0]=='S')
				sText.Printf("%f",fl_param.S);
			else if(sCurParam[0]=='d')
				sText.Printf("%f",fl_param.d);
			else if(sCurParam[0]=='t')
				sText.Printf("%f",fl_param.t);
		}
		else if(m_iTubeEndLjStyle==2&&((m_iPartType==3&&m_pRigidFlParam)
			||(m_iPartType==4&&m_pFlexibleFlParam)))
		{	//刚性法兰或柔性法兰对应参数 wht 12-05-15
			FL_PARAM2 fl_param;
			if(m_iPartType==3)
				fl_param=*m_pRigidFlParam;
			else 
				fl_param=*m_pFlexibleFlParam;
			if(sCurParam[0]=='A')
				sText.Printf("%f",fl_param.para.A);
			else if(sCurParam[0]=='B')
				sText.Printf("%f",fl_param.para.B);
			else if(sCurParam[0]=='C')
				sText.Printf("%f",fl_param.para.C);
			else if(sCurParam[0]=='D')
				sText.Printf("%f",fl_param.para.D);
			else if(sCurParam[0]=='E')
				sText.Printf("%f",fl_param.para.E);
			else if(sCurParam[0]=='F')
				sText.Printf("%f",fl_param.para.F);
			else if(sCurParam[0]=='G')
				sText.Printf("%f",fl_param.para.G);
			else if(sCurParam[0]=='H')
				sText.Printf("%f",fl_param.para.H);
			else if(sCurParam[0]=='d')
				sText.Printf("%f",fl_param.d);
			else if(sCurParam[0]=='T')
				sText.Printf("%f",fl_param.para.T);
			//法兰外形参数
			else if(sCurParam[0]=='R')
				sText.Printf("%f",fl_param.fInnerD);
			else if(sCurParam[0]=='W')
				sText.Printf("%f",fl_param.fOuterD);
			else if(sCurParam[0]=='t')
				sText.Printf("%f",fl_param.fThick);
		}
		else if(m_iTubeEndLjStyle==2&&m_iPartType==0&&sCurParam[0]=='R')
		{
			DESIGN_PLATE_ITEM *pItem=m_paramPlate.GetDesignItem(sCurParam[0]);
			if(pItem)
				sText.Printf("%f",pItem->value.fValue*2);
		}
		else 
		{
			DESIGN_PLATE_ITEM *pItem=m_paramPlate.GetDesignItem(sCurParam[0]);
			if(pItem)
			{
				if(pItem->dataType==1)
				{
					if(pItem->value.hPart<0)
						sText.Printf("-0X%X",abs(pItem->value.hPart));
					else 
						sText.Printf("0X%X",pItem->value.hPart);
				}
				else
					sText.Printf("%f",pItem->value.fValue);
			}
		}
		SimplifiedNumString(sText);
	}
	//构件类型
	else if(CDesignTubeEndLjDlg::GetPropID("m_iPartType")==id) 
	{
		if(m_iTubeEndLjStyle==2)
		{
			if(m_iPartType==0)
				sText.Copy("0.普通法兰");
			else if(m_iPartType==1)
				sText.Copy("1.平焊法兰");
			else if(m_iPartType==2)
				sText.Copy("2.对焊法兰");
			else if(m_iPartType==3)
				sText.Copy("3.刚性法兰");
			else if(m_iPartType==4)
				sText.Copy("4.柔性法兰");
		}
		else if(m_iTubeEndLjStyle>2)
		{
			if(m_iPartType==0)
				sText.Copy("0.自定义插板");
			else if(m_iPartType==1)
				sText.Copy("1.标准插板");
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_iWrapPlateType")==id) 
	{	//十字插板包板类型
		if(m_iWrapPlateType==0)
			sText.Copy("0.钢板");
		else if(m_iWrapPlateType==1)
			sText.Copy("1.角钢");
	}
	//标准构件级别
	else if(CDesignTubeEndLjDlg::GetPropID("m_iStdPartLevel")==id)
	{
		if(m_iTubeEndLjStyle==2)
			sText.Printf("%C",m_cFlLevel1);
		else 
			sText.Printf("%d",m_iStdPartLevel);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_iStdPartLevel2")==id)
	{
		if(m_iTubeEndLjStyle==2)
			sText.Printf("%C",m_cFlLevel2);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_cStdPartMat")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(m_cStdPartMat,matStr))
			sText.Printf("%s",matStr);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_bCoupleFLCreate")==id)
	{
		if(!m_bCoupleFLCreate)
			sText.Printf("0.单侧法兰");
		else
			sText.Printf("1.成对设计");
	}
	//平面切割
	else if(CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.x")==id)
	{
		sText.Printf("%f",cutPlaneOrigin.x);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.y")==id)	
	{
		sText.Printf("%f",cutPlaneOrigin.y);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.z")==id)
	{
		sText.Printf("%f",cutPlaneOrigin.z);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.x")==id)
	{
		sText.Printf("%f",cutPlaneNorm.x);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.y")==id)
	{
		sText.Printf("%f",cutPlaneNorm.y);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.z")==id)
	{
		sText.Printf("%f",cutPlaneNorm.z);
		SimplifiedNumString(sText);
	}
	//钢管相贯
	else if(CDesignTubeEndLjDlg::GetPropID("m_bFaceToFace")==id)
	{
		if(m_bFaceToFace)
			sText.Copy("是");
		else 
			sText.Copy("否");
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_hWeldLinkObj")==id)
		sText.Printf("0X%X",m_hWeldLinkObj);
	else if(CDesignTubeEndLjDlg::GetPropID("m_hDatumTube1")==id)
		sText.Printf("0X%X",m_hDatumTube1);
	else if(CDesignTubeEndLjDlg::GetPropID("m_hDatumTube2")==id)
		sText.Printf("0X%X",m_hDatumTube2);
	//基本信息
	else if(CDesignTubeEndLjDlg::GetPropID("sPartNo")==id)
		sText.Printf("%s",(char*)m_paramPlate.GetPartNo());
	else if(CDesignTubeEndLjDlg::GetPropID("sCoupleFlPlateNo")==id)
		sText.Printf("%s",sCoupleFlPlateNo);
	else if(CDesignTubeEndLjDlg::GetPropID("sWeldPlatePartNo")==id)
		sText.Printf("%s",sWeldPlatePartNo);
	else if(CDesignTubeEndLjDlg::GetPropID("sWrapPlatePartNo")==id)
		sText.Printf("%s",sWrapPlatePartNo);
	else if(CDesignTubeEndLjDlg::GetPropID("sCodeName")==id)
		sText.Printf("%s",sCodeName);
	else if(CDesignTubeEndLjDlg::GetPropID("iSeg")==id)
		sText.Copy(m_paramPlate.iSeg.ToString());
	else if(CDesignTubeEndLjDlg::GetPropID("cMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(m_paramPlate.cMaterial,matStr))
			sText.Printf("%s",matStr);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_uStatMatCoef")==id)
		sText.Printf("%d",m_paramPlate.m_uStatMatCoef);
	else if(CDesignTubeEndLjDlg::GetPropID("thick")==id)
		sText.Printf("%d",m_paramPlate.thick);
	//装配参数
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.rot_ang")==id)
	{
		sText.Printf("%f",setupInfo.workNorm.rot_ang);
		SimplifiedNumString(sText);
		if(m_iTubeEndLjStyle==4)
		{	//槽型插板需要显示卷边朝向 wht 10-10-29
			f3dPoint tube_work_norm=m_pCurLineTube->WorkPlaneNorm();
			if(ftol(setupInfo.workNorm.rot_ang)==0)
			{
				if(tube_work_norm*work_norm>0)
					strcpy(sText,"朝内"); //当前工作面法线与钢管工作面法线同向
				else 
					strcpy(sText,"朝外"); //当前工作面法线与钢管工作面法线反向
			}
			else if(ftol(setupInfo.workNorm.rot_ang)==180)
			{
				if(tube_work_norm*work_norm>0)
					strcpy(sText,"朝外"); //当前工作面法线与钢管工作面法线同向
				else 
					strcpy(sText,"朝内"); //当前工作面法线与钢管工作面法线反向
			}
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(setupInfo.workNorm.vector));
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x")==id)
	{
		sText.Printf("%f",setupInfo.workNorm.vector.x);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.y")==id)
	{
		sText.Printf("%f",setupInfo.workNorm.vector.y);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.z")==id)
	{
		sText.Printf("%f",setupInfo.workNorm.vector.z);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(setupInfo.workNormUpVert.vector));
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x")==id)
	{
		sText.Printf("%f",setupInfo.workNormUpVert.vector.x);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.y")==id)
	{
		sText.Printf("%f",setupInfo.workNormUpVert.vector.y);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.z")==id)
	{
		sText.Printf("%f",setupInfo.workNormUpVert.vector.z);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(setupInfo.workNormDownVert.vector));
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x")==id)
	{
		sText.Printf("%f",setupInfo.workNormDownVert.vector.x);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.y")==id)
	{
		sText.Printf("%f",setupInfo.workNormDownVert.vector.y);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.z")==id)
	{
		sText.Printf("%f",setupInfo.workNormDownVert.vector.z);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("H1")==id)
	{
		sText.Printf("%f",setupInfo.H1);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fNormOffset")==id)
	{
		sText.Printf("%f",setupInfo.m_fNormOffset);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fOddment")==id)
	{
		sText.Printf("%f",setupInfo.m_fOddment);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fWeldHeight")==id)
	{
		sText.Printf("%f",setupInfo.m_fWeldHeight);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fTubeNormOffset")==id)
	{
		sText.Printf("%f",setupInfo.m_fTubeNormOffset);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fTubeEccentricDist")==id)
	{
		sText.Printf("%f",setupInfo.m_fTubeEccentricDist);
		SimplifiedNumString(sText);
	}
	//法线设计参数
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.norm_style")==id)
	{
		if(xVector.norm_style==0)
			sText.Copy("0.指定方向");
		else if(xVector.norm_style==1)
			sText.Copy("1.角钢肢法线");
		else if(xVector.norm_style==2)
			sText.Copy("2.杆件交叉面法线");
		else if(xVector.norm_style==3)
			sText.Copy("3.构件工作法线");
		else if(xVector.norm_style==4)
			sText.Copy("4.构件上的相对方向");
		else if(xVector.norm_style==5)
			sText.Copy("5.杆件上的特征方向");
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.hVicePart")==id)
		sText.Printf("0X%X",xVector.hVicePart);
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.hCrossPart")==id)
		sText.Printf("0X%X",xVector.hCrossPart);
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.norm_wing")==id)
	{
		if(xVector.norm_wing==0)
			sText.Printf("%s","0.X肢法线");
		else
			sText.Printf("%s","1.Y肢法线");
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.direction")==id)
	{
		if(xVector.norm_style==1)
		{
			if(xVector.direction==0)
				sText.Printf("%s","0.朝外");
			else
				sText.Printf("%s","1.朝里");
		}
		else if(xVector.norm_style==5)
		{
			if(xVector.direction==0)
				sText.Copy("0.始->终");
			else if(xVector.direction==1)
				sText.Copy("1.终->始");
			else if(xVector.direction==2)
				sText.Copy("2.自基准面旋转");
			else if(xVector.direction==3)
				sText.Copy("3.自焊道面旋转");
		}
		else
		{
			if(xVector.direction==0)
				sText.Printf("%s","0.正向");
			else
				sText.Printf("%s","1.反向");
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.rot_ang")==id)
	{
		sText.Printf("%f",xVector.rot_ang);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVectorType")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(xVector.nearVector));
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.x")==id)
	{
		sText.Printf("%lf",xVector.nearVector.x);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.y")==id)
	{
		sText.Printf("%lf",xVector.nearVector.y);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.z")==id)
	{
		sText.Printf("%lf",xVector.nearVector.z);
		SimplifiedNumString(sText);
	}
	//螺栓布置参数
	else if(GetPropID("m_nBoltD")==id)
		sText.Printf("%d",LayoutBoltPara.m_nBoltD);
	else if(GetPropID("m_iDriection")==id)
	{
		if(LayoutBoltPara.m_iDirection==0)
			sText.Copy("0.朝外");
		else
			sText.Copy("1.朝内");
	}
	else if(GetPropID("m_iStartEdge")==id)
	{
		if(LayoutBoltPara.m_iStartEdge==0)
			sText.Copy("0.基准边");
		else
			sText.Copy("1.焊道线");
	}
	else if(GetPropID("m_fOffsetAngle")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_fOffsetAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fBoltLayoutD")==id)	
	{
		sText.Printf("%f",LayoutBoltPara.m_fBoltLayoutD);
		SimplifiedNumString(sText);
	}
	//附加板参数
	else if(CDesignTubeEndLjDlg::GetPropID("m_bCreateAttachPlate")==id)
	{
		if(m_bCreateAttachPlate)
			sText.Copy("是");
		else 
			sText.Copy("否");
	}
	else if(GetPropID("AttachPlatePara.A")==id)
	{
		sText.Printf("%f",AttachPlatePara.A);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.B")==id)
	{
		sText.Printf("%f",AttachPlatePara.B);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.C")==id)
	{
		sText.Printf("%f",AttachPlatePara.C);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.D")==id)
	{
		sText.Printf("%f",AttachPlatePara.D);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.E")==id)
	{
		sText.Printf("%f",AttachPlatePara.E);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.m_iCutMode")==id)
	{
		if(AttachPlatePara.m_iCutMode==0)
			sText.Copy("0.切角");
		else
			sText.Copy("1.切弧");
	}
	else if(GetPropID("AttachPlatePara.F")==id)
	{
		sText.Printf("%f",AttachPlatePara.F);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.G")==id)
	{
		sText.Printf("%f",AttachPlatePara.G);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.H")==id)
	{
		sText.Printf("%f",AttachPlatePara.H);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.cMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(AttachPlatePara.cMaterial,matStr))
		sText.Printf("%s",matStr);
	}
	else if(GetPropID("AttachPlatePara.sPartNo")==id)
		sText.Printf("%s",AttachPlatePara.sPartNo);
	else if(GetPropID("AttachPlatePara.m_fAngle")==id)
	{
		sText.Printf("%f",AttachPlatePara.m_fAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.m_nPlateNum")==id)
		sText.Printf("%d",AttachPlatePara.m_nPlateNum);
	else if(GetPropID("AttachPlatePara.m_nThick")==id)
		sText.Printf("%d",AttachPlatePara.m_nThick);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_bMirCreate")==id)
	{
		if(AttachPlatePara.m_bMirCreate)
			sText.Copy("是");
		else 
			sText.Copy("否");
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_bUseAngle")==id)
	{
		if(AttachPlatePara.m_bUseAngle)
			sText.Copy("是");
		else 
			sText.Copy("否");
	}
	//填板参数
	else if(CDesignTubeEndLjDlg::GetPropID("m_bCreateFillPlate")==id)
	{
		if(m_bCreateFillPlate)
			sText.Copy("是");
		else 
			sText.Copy("否");
	}
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.cMat")==id)
	{
		char matStr[20]="";
		if(QuerySteelMatMark(FillPlatePara.cMat,matStr))
			sText.Printf("%s",matStr);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.sPartNo")==id)
		sText.Printf("%s",FillPlatePara.sPartNo);
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.nThick")==id)
		sText.Printf("%d",FillPlatePara.nThick);
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.nNodePlateThick")==id)
		sText.Printf("%d",FillPlatePara.nNodePlateThick);
	//节点板
	else if(GetPropID("NodePlatePara.cMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(NodePlatePara.cMaterial,matStr))
			sText.Printf("%s",matStr);
	}
	else if(GetPropID("NodePlatePara.sPartNo")==id)
		sText.Printf("%s",NodePlatePara.sPartNo);
	else if(GetPropID("NodePlatePara.m_nRollHeight")==id)
		sText.Printf("%d",NodePlatePara.m_nRollHeight);
	else if(GetPropID("NodePlatePara.m_nThick")==id)
		sText.Printf("%d",NodePlatePara.m_nThick);
	else if(GetPropID("m_nBoltNum")==id)
		sText.Printf("%d",LayoutBoltPara.m_nBoltNum);
	else if(GetPropID("m_bStaggerLayout")==id)
	{
		if(LayoutBoltPara.m_bStaggerLayout)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("m_bMirLayout")==id)
	{
		if(LayoutBoltPara.m_bMirLayout)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("m_fSpecialSpace")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_fSpecialSpace);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_nRowNum")==id)
		sText.Printf("%d",LayoutBoltPara.m_nRowNum);
	else if(GetPropID("m_nColNum")==id)
		sText.Printf("%d",LayoutBoltPara.m_nColNum);
	else if(GetPropID("m_fRowSpace")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_fRowSpace);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fColSpace")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_fColSpace);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_posFirstBolt.x")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_posFirstBolt.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_posFirstBolt.y")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_posFirstBolt.y);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
void CDesignTubeEndLjDlg::UpdateProfileProperty(CPropTreeItem *pParentItem)
{
	if(pParentItem==NULL)
		return;
	CString sPartName=GetTubeEndLjName();
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	m_propList.DeleteAllSonItems(pParentItem);
	//
	int iPartType=GetPartType();
	CPropTreeItem *pPropItem=NULL;
	if(m_iTubeEndLjStyle==2&&iPartType>2)
	{	//刚性法兰柔性法兰外形参数
		//法兰内径
		pPropItem=oper.InsertEditPropItem(pParentItem,"R","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "内径B";
		pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s内径B",sPartName);
		pPropItem->SetReadOnly(TRUE);
		//法兰外径
		pPropItem=oper.InsertEditPropItem(pParentItem,"W","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "外径W";
		pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s外径W",sPartName);
		pPropItem->SetReadOnly(TRUE);
		//法兰厚度
		pPropItem=oper.InsertEditPropItem(pParentItem,"t","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "厚度T";
		pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s厚度T",sPartName);
		pPropItem->SetReadOnly(TRUE);
	}
	else if(m_iTubeEndLjStyle==2&&GetPartType()>0)
	{	//焊端外径/钢管外直径
		pPropItem=oper.InsertEditPropItem(pParentItem,"A","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "A";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "A=焊端外径/钢管外直径";
		pPropItem->SetReadOnly(TRUE);
		//法兰内径
		pPropItem=oper.InsertEditPropItem(pParentItem,"B","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "B";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "B=法兰内径";
		pPropItem->SetReadOnly(TRUE);
		//法兰外径
		pPropItem=oper.InsertEditPropItem(pParentItem,"D","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "D";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "D=法兰外径";
		pPropItem->SetReadOnly(TRUE);
		//法兰厚度
		pPropItem=oper.InsertEditPropItem(pParentItem,"C","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "C";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "C=法兰厚度";
		pPropItem->SetReadOnly(TRUE);
		//法兰高度
		pPropItem=oper.InsertEditPropItem(pParentItem,"H","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "H";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "H=法兰高度";
		pPropItem->SetReadOnly(TRUE);
		//法兰根部直径，包含R尺寸
		pPropItem=oper.InsertEditPropItem(pParentItem,"N","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "N";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "N=法兰根部直径(包括R尺寸)";
		pPropItem->SetReadOnly(TRUE);
		//法兰顶部厚度 //对焊法兰时S记录S相对于主管厚度的增量 wht 10-01-20
		pPropItem=oper.InsertEditPropItem(pParentItem,"S","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "S";
		pPropItem->m_lpNodeInfo->m_strPropHelp =(GetPartType()==2)?"S=法兰顶部厚度-主管厚度的差值":"S=法兰顶部厚度";
		pPropItem->SetReadOnly(TRUE);
		//法兰颈部倒角半径
		pPropItem=oper.InsertEditPropItem(pParentItem,"R","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "R";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "R=法兰颈部倒角半径";
		pPropItem->SetReadOnly(TRUE);
		//螺栓孔径
		pPropItem=oper.InsertEditPropItem(pParentItem,"L","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "L";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "L=螺栓孔径";
		pPropItem->SetReadOnly(TRUE);
	}
	else if(m_iTubeEndLjStyle==2&&GetPartType()==0)
	{
		oper.InsertEditPropItem(pParentItem,"R","内径B","法兰内径",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"W","外径W","法兰外径",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"H","参数H","钢管插入到法兰的深度",-1,TRUE);
	}
	else if(m_iTubeEndLjStyle>2&&m_iTubeEndLjStyle<6)
	{	//插板外形参数
		//插板长度L
		pPropItem=oper.InsertEditPropItem(pParentItem,"L","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "插板长度L";
		pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s长度L",sPartName);
		pPropItem->SetReadOnly(iPartType>0);
		//焊接长度
		pPropItem=oper.InsertEditPropItem(pParentItem,"M","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "焊接长度M";
		pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s焊接长度M",sPartName);
		pPropItem->SetReadOnly(iPartType>0);
		//N
		pPropItem=oper.InsertEditPropItem(pParentItem,"N","","",-1,TRUE);
		if(m_iTubeEndLjStyle==3||m_iTubeEndLjStyle==4)	
		{	//U型插板 槽型插板
			pPropItem->m_lpNodeInfo->m_strPropName = "切角长度N";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s切角长度N",sPartName);
		}
		else
		{	//十字插板
			pPropItem->m_lpNodeInfo->m_strPropName = "包板宽度N";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s包板宽度N",sPartName);
		}
		pPropItem->SetReadOnly(iPartType>0);
		//S
		pPropItem=oper.InsertEditPropItem(pParentItem,"S","","",-1,TRUE);
		if(m_iTubeEndLjStyle==3||m_iTubeEndLjStyle==4)	//U型插板 槽型插板
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "切角宽度S";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s切角宽度S",sPartName);
		}
		else	//十字插板
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "包板长度S";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s包板长度S",sPartName);
		}
		pPropItem->SetReadOnly(iPartType>0);
		//H
		pPropItem=oper.InsertEditPropItem(pParentItem,"H","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "插板宽度H";
		pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s宽度H",sPartName);
		pPropItem->SetReadOnly(iPartType>0);
		//W
		pPropItem=oper.InsertEditPropItem(pParentItem,"W","","",-1,TRUE);
		if(m_iTubeEndLjStyle==3)	//U型插板
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "U型槽高度W";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s上下两面间的距离W",sPartName);
		}
		else if(m_iTubeEndLjStyle==4) //卷边插板
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "卷边高度W";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s卷边高度W",sPartName);
		}
		else	//十字插板	
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "包板厚度W";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s包板厚度W",sPartName);
		}
		pPropItem->SetReadOnly(iPartType>0);
		if(m_iTubeEndLjStyle==5)
		{	//十字插板
			//A
			pPropItem=oper.InsertEditPropItem(pParentItem,"A","","",-1,TRUE);
			pPropItem->m_lpNodeInfo->m_strPropName = "包板定位参数A";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s包板定位参数A,包板与钢管端之间的距离",sPartName);
			pPropItem->SetReadOnly(iPartType>0);
			//R
			pPropItem=oper.InsertEditPropItem(pParentItem,"R","","",-1,TRUE);
			pPropItem->m_lpNodeInfo->m_strPropName = "包板定位参数R";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s包板定位参数R,包板到其垂直的立板或基板之间的距离",sPartName);
			pPropItem->SetReadOnly(iPartType>0);
		}
	}
}
//更新附加板参数
void CDesignTubeEndLjDlg::UpdateFillPlateProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	//附加板布置参数
	if(m_bCreateFillPlate&&m_iTubeEndLjStyle==5)
	{	
		//节点板厚度
		if(FillPlatePara.nNodePlateThick>0)
		{
			CPropTreeItem* pItem=oper.InsertEditPropItem(pParentItem,"FillPlatePara.nNodePlateThick","","",-1,TRUE);
			pItem->SetReadOnly();
		}
		//
		oper.InsertEditPropItem(pParentItem,"FillPlatePara.sPartNo","","",-1,TRUE);
		//
		oper.InsertCmbListPropItem(pParentItem,"FillPlatePara.cMat","","","",-1,TRUE);
		//
		oper.InsertEditPropItem(pParentItem,"FillPlatePara.nThick","","",-1,TRUE);
	}
}
//更新附加板参数
void CDesignTubeEndLjDlg::UpdateAttachPlateProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	CPropTreeItem* pPropItem=NULL;
	if(!m_bCreateAttachPlate)
		return;
	//附加板布置参数
	if(m_iTubeEndLjStyle==2)
	{	//法兰
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.sPartNo","","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.cMaterial","","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.m_nThick","","",-1,TRUE);
		//法兰肋板个数
		pPropItem=oper.InsertEditPropItem(pParentItem,"AttachPlatePara.m_nPlateNum","","",-1,TRUE);
		pPropItem->SetReadOnly();
		//肘板偏移角
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.m_fAngle","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.A","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.B","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.C","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.D","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.E","","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.m_iCutMode","","","",-1,TRUE);
		pPropItem=oper.InsertEditPropItem(pParentItem,"AttachPlatePara.H","","",-1,TRUE);
		if(AttachPlatePara.H>0)
		{
			oper.InsertEditPropItem(pPropItem,"AttachPlatePara.F","","",-1,TRUE);
			oper.InsertEditPropItem(pPropItem,"AttachPlatePara.G","","",-1,TRUE);
		}
	}
	else if(m_iTubeEndLjStyle==4)
	{
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.sPartNo","","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.cMaterial","","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.m_nThick","","",-1,TRUE);
		//
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.m_bMirCreate","","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.A","长度L","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.B","宽度W","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.C","切角长L1","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.D","切角宽H1","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.E","切角长L2","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.F","切角宽H2","",-1,TRUE);
	}
	else if(m_iTubeEndLjStyle==5)
	{
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.sPartNo","","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.cMaterial","","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.m_nThick","","",-1,TRUE);
		//
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.m_bMirCreate","","","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.m_bUseAngle","","","",-1,TRUE);
		pPropItem=oper.InsertEditPropItem(pParentItem,"AttachPlatePara.m_fAngle","","",-1,TRUE);
		pPropItem->SetReadOnly(!AttachPlatePara.m_bUseAngle);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.A","参数A","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.B","参数B","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.C","参数C","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.D","参数D","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.E","参数E","",-1,TRUE);
	}
}
//显示子属性
void CDesignTubeEndLjDlg::DisplayChildPropertyList(CPropTreeItem *pItem,BOOL bUpdate/*=FALSE*/)
{
	RefreshCtrlState(FALSE);
	if(!bUpdate)
		m_xSubPropList.CleanTree();
	if(pItem)
	{
		if(pItem->m_idProp==GetPropID("MotherPlateNorm"))
			m_xSubPropList.m_nObjClassTypeId = 0;
		else if(pItem->m_idProp==GetPropID("UpVertPlateNorm"))
			m_xSubPropList.m_nObjClassTypeId = 1;
		else if(pItem->m_idProp==GetPropID("DownVertPlateNorm"))
			m_xSubPropList.m_nObjClassTypeId = 2;
	}
	CLDSVector xVector;
	if(m_xSubPropList.m_nObjClassTypeId==0)
		xVector=setupInfo.workNorm;
	else if(m_xSubPropList.m_nObjClassTypeId==1)
		xVector=setupInfo.workNormUpVert;
	else if(m_xSubPropList.m_nObjClassTypeId==2)
		xVector=setupInfo.workNormDownVert;
	else
		return;
	m_xSubPropList.SetModifyValueFunc(ModifyItemValue);
	m_xSubPropList.SetButtonClickFunc(ButtonClickItem);
	//
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pSonItem=NULL,*pRoot=m_xSubPropList.GetRootItem();
	char sText[MAX_TEMP_CHAR_100+1]="";
	CItemInfo* lpInfo = NULL;
	pGroupItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z"),NULL);
	if(pGroupItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_STATIC;
		lpInfo->m_strPropName = "工作法向设计参数";
		lpInfo->m_strPropHelp = "工作法向设计参数";
		pGroupItem = m_xSubPropList.InsertItem(pRoot,lpInfo, -1,bUpdate);
		pGroupItem->m_idProp =GetPropID("design_axis_z");
	}
	//法线计算方式
	pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.norm_style"),NULL);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = "0.指定方向|1.单角钢肢法线|2.杆件交叉面法线|3.构件工作法线|4.构件上的相对方向|5.杆件上的特征方向";
		lpInfo->m_strPropName = "法向计算方式";
		lpInfo->m_strPropHelp = lpInfo->m_strPropName;
		pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("design_axis_z.norm_style");
	}
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;

	//单角钢肢法线
	if(xVector.norm_style==0)
	{
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.hVicePart"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.hCrossPart"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.norm_wing"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.direction"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.rot_ang"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVectorType"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.x"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.y"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.z"));
	}
	else if(xVector.norm_style==1)
	{
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.hCrossPart"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.rot_ang"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVectorType"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.x"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.y"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.z"));
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.hVicePart"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "基准角钢";
			lpInfo->m_strPropHelp = "基准角钢句柄";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.hVicePart");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.norm_wing"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbItems ="0.X肢法线|1.Y肢法线";
			lpInfo->m_strPropName = "基准肢";
			lpInfo->m_strPropHelp = "基准肢";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.norm_wing");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.direction"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.direction");
		}
		pPropItem->m_lpNodeInfo->m_cmbItems ="0.朝外|1.朝里";
		pPropItem->m_lpNodeInfo->m_strPropName = "朝向";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "朝向";
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else if(xVector.norm_style==2)
	{	//交叉杆件平面定向
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.norm_wing"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.direction"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.rot_ang"));
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.hVicePart"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "基准杆件";
			lpInfo->m_strPropHelp = "基准杆件句柄";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.hVicePart");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.hCrossPart"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "交叉参照杆件";
			lpInfo->m_strPropHelp = "交叉参照杆件句柄";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo,2,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.hCrossPart");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVectorType"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.nearVectorType");
		}
		pPropItem->m_lpNodeInfo->m_strPropName = "近似方向";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "近似方向用于区别计算方向为交叉面的正法线还是负法线方向";
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		int nearVectorType=CStdVector::GetVectorType(xVector.nearVector);
		if(nearVectorType==0)	//自定义
		{
			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVector.x"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "X分量";
				lpInfo->m_strPropHelp = "X分量";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp =GetPropID("design_axis_z.nearVector.x");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVector.y"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Y分量";
				lpInfo->m_strPropHelp = "Y分量";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp =GetPropID("design_axis_z.nearVector.y");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVector.z"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Z分量";
				lpInfo->m_strPropHelp = "Z分量";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp =GetPropID("design_axis_z.nearVector.z");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		else
		{
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.x"));
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.y"));
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.z"));
		}
	}
	else if(xVector.norm_style==3)
	{	//构件工作法线
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.hCrossPart"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.norm_wing"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.rot_ang"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVectorType"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.x"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.y"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.z"));
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.hVicePart"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "基准构件";
			lpInfo->m_strPropHelp = "基准构件";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.hVicePart");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.direction"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.direction");
		}
		pPropItem->m_lpNodeInfo->m_cmbItems ="0.正向|1.反向";
		pPropItem->m_lpNodeInfo->m_strPropName = "朝向";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "正向是指基准构件的正工作法线方向；反向是指基准构件的负工作法线方向";
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else if(xVector.norm_style==4)
	{	//构件上相对方向
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.hCrossPart"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.norm_wing"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.direction"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.rot_ang"));
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.hVicePart"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "基准构件";
			lpInfo->m_strPropHelp = "基准构件,十字插板时0x0表示基准构件是插板的基板";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.hVicePart");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVectorType"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.nearVectorType");
		}
		pPropItem->m_lpNodeInfo->m_strPropName = "相对方向";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "基准构件装配坐标系下的相对方向";
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		int nearVectorType=CStdVector::GetVectorType(xVector.nearVector);
		if(nearVectorType==0)	//自定义
		{
			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVector.x"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "X分量";
				lpInfo->m_strPropHelp = "X分量";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp =GetPropID("design_axis_z.nearVector.x");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVector.y"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Y分量";
				lpInfo->m_strPropHelp = "Y分量";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp =GetPropID("design_axis_z.nearVector.y");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVector.z"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Z分量";
				lpInfo->m_strPropHelp = "Z分量";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp =GetPropID("design_axis_z.nearVector.z");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		else
		{
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.x"));
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.y"));
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.z"));
		}
	}
	else if(xVector.norm_style==5)
	{	//5.基准杆件上的特征方向
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.hCrossPart"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.norm_wing"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVectorType"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.x"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.y"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.z"));
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.hVicePart"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "基准杆件";
			lpInfo->m_strPropHelp = "基准杆件句柄";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.hVicePart");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		
		//方向类型
		bool bNeedRotAnglePara=false;
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(xVector.hVicePart,CLS_LINEPART);
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.direction"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.direction");
		}
		pPropItem->m_lpNodeInfo->m_strPropName = "方向类型";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "0.杆件始->终方向,1.杆件终->始方向,2.角钢X肢法线方向,3.角钢X肢法线负方向,4.角钢Y肢法线方向,5.角钢Y肢法线负方向";
		if(pLinePart&&pLinePart->IsAngle())
			pPropItem->m_lpNodeInfo->m_cmbItems ="0.始->终|1.终->始|2.X肢法线+|3.X肢法线-|4.Y肢法线+|5.Y肢法线-";
		else if(pLinePart&&pLinePart->IsTube())
		{
			pPropItem->m_lpNodeInfo->m_cmbItems = "0.始->终|1.终->始|2.自基准面旋转|3.自焊道面旋转";
			if(xVector.direction>=2)
				bNeedRotAnglePara=true;
		}
		else 
			pPropItem->m_lpNodeInfo->m_cmbItems ="0.始->终|1.终->始";
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bNeedRotAnglePara)
		{
			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.rot_ang"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType=CDT_EDIT;
				lpInfo->m_cmbItems = "0|90|180|270";
				lpInfo->m_strPropName = "旋转角度";
				if(xVector.direction==2)
					lpInfo->m_strPropHelp="自钢管基准线平面法向绕轴线的旋转角度°";
				else 
					lpInfo->m_strPropHelp="自钢管焊道线平面法向绕轴线的旋转角度°";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp = GetPropID("design_axis_z.rot_ang");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		else
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.rot_ang"));
	}
	if(!bUpdate)
		m_xSubPropList.Redraw();
}
CString CDesignTubeEndLjDlg::GetTubeEndLjName()
{
	CString sPartName;
	if(m_iTubeEndLjStyle==0)
		sPartName = "平面切割";
	else if(m_iTubeEndLjStyle==1)
		sPartName = "钢管相贯";
	if(m_iTubeEndLjStyle==2)
		sPartName = "法兰";
	else if(m_iTubeEndLjStyle==3)
		sPartName = "U型插板";
	else if(m_iTubeEndLjStyle==4)
		sPartName = "槽型插板";
	else if(m_iTubeEndLjStyle==5)
		sPartName = "十字插板";
	return sPartName;
}
void CDesignTubeEndLjDlg::UpdateBaseInfoProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	//
	CPropTreeItem* pPropItem=NULL;
	if(m_iTubeEndLjStyle==5)
	{	
		if(m_iPartType>0)
		{	//标准插板代号
			pPropItem=oper.InsertEditPropItem(pParentItem,"sCodeName","插板代号","",-1,TRUE);
			pPropItem->SetReadOnly(TRUE);
		}
		//基板编号
		pPropItem=oper.InsertEditPropItem(pParentItem,"sPartNo","基板编号","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType>0);
		//立板编号
		pPropItem=oper.InsertEditPropItem(pParentItem,"sWeldPlatePartNo","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType>0);
		//包板编号
		pPropItem=oper.InsertEditPropItem(pParentItem,"sWrapPlatePartNo","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType>0);
	}
	else
	{
		pPropItem=oper.InsertEditPropItem(pParentItem,"sPartNo","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType>0);
		//对接法兰编号 wht 10-09-29
		if(m_iTubeEndLjStyle==2&&m_bCoupleFLCreate&&m_pRelatedTube)
		{
			pPropItem=oper.InsertEditPropItem(pParentItem,"sCoupleFlPlateNo","","",-1,TRUE);
			pPropItem->SetReadOnly(m_iPartType>0);
		}
	}
	//
	oper.InsertEditPropItem(pParentItem,"iSeg","","",-1,TRUE);
	//
	pPropItem=oper.InsertCmbListPropItem(pParentItem,"cMaterial","","","",-1,TRUE);
	pPropItem->SetReadOnly(m_iPartType>0);
	//厚度	
	pPropItem=oper.InsertEditPropItem(pParentItem,"thick","","",-1,TRUE);
	pPropItem->SetReadOnly(m_iPartType>0);
	//
	oper.InsertEditPropItem(pParentItem,"m_uStatMatCoef","","",-1,TRUE);
}
void CDesignTubeEndLjDlg::UpdateTubeEndProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	//
	oper.InsertBtnEditPropItem(pParentItem,"m_fWeldHeight","","",-1,TRUE);
	//
	CPropTreeItem* pPropItem=oper.InsertBtnEditPropItem(pParentItem,"m_fOddment","","",-1,TRUE);
	if(m_bStartEnd)
	{	//钢管始端正负头
		pPropItem->m_lpNodeInfo->m_strPropName = "始端正负头";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "钢管始端正负头,端连接为法兰时钢管正负头=钢管初始化正负头-法兰高出钢管部分的高度。";
	}
	else
	{	//钢管终端正负头
		pPropItem->m_lpNodeInfo->m_strPropName = "终端正负头";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "钢管终端正负头,端连接为法兰时钢管正负头=钢管初始化正负头-法兰高出钢管部分的高度。";
	}
	pPropItem->SetReadOnly(IsHasExistEndLj());
	//钢管法向偏移量
	oper.InsertEditPropItem(pParentItem,"m_fTubeNormOffset","","",-1,TRUE);
	//钢管偏心距
	oper.InsertEditPropItem(pParentItem,"m_fTubeEccentricDist","","",-1,TRUE);
}
void CDesignTubeEndLjDlg::UpdateSetUpProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	//
	CString sPartName=GetTubeEndLjName();
	CPropTreeItem* pPropItem=NULL;
	if(m_iTubeEndLjStyle==2)
	{	//H1

	}
	else if(m_iTubeEndLjStyle>2&&m_iTubeEndLjStyle<5)
	{	//法兰暂时不显示X轴旋转角度以及法向偏移量 wht 10-07-01
		if(m_iTubeEndLjStyle==4)
		{
			pPropItem=oper.InsertCmbListPropItem(pParentItem,"design_axis_z.rot_ang","","","",-1,TRUE);
			pPropItem->m_lpNodeInfo->m_cmbItems="朝外|朝内|90|270|";
			pPropItem->m_lpNodeInfo->m_strPropName = "卷边方向";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("朝外朝内表示槽型插板卷边朝向(相对于当前视图而言)，角度表示插板工作面相对与钢管基准面之间的旋转角度");
		}
		else
		{	//工作面旋转角度
			pPropItem=oper.InsertCmbEditPropItem(pParentItem,"design_axis_z.rot_ang","","","",-1,TRUE);
			pPropItem->m_lpNodeInfo->m_cmbItems="0|90|180|270|";
			pPropItem->m_lpNodeInfo->m_strPropName = "工作面旋转角度";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s工作面相对与钢管基准面旋转角度",sPartName);
		}
		//工作面法向偏移量
		oper.InsertEditPropItem(pParentItem,"m_fNormOffset","","",-1,TRUE);
	}
	else if(m_iTubeEndLjStyle==5)
	{	//十字插板
		//基板工作法向
		pPropItem=oper.InsertCmbListPropItem(pParentItem,"MotherPlateNorm","","","",-1,TRUE);
		pPropItem->m_bHideChildren=TRUE;
		//基板法向X坐标分量
		CPropTreeItem* pSonPropItem=oper.InsertEditPropItem(pPropItem,"MotherPlateNorm.x","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//基板法向Y坐标分量
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"MotherPlateNorm.y","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//基板法向Z坐标分量
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"MotherPlateNorm.z","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//1号(上)立板工作法向
		pPropItem=oper.InsertCmbListPropItem(pParentItem,"UpVertPlateNorm","","","",-1,TRUE);
		pPropItem->m_bHideChildren=TRUE;
		//1号(上)立板法向X坐标分量
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"UpVertPlateNorm.x","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//1号(上)立板法向Y坐标分量
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"UpVertPlateNorm.y","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//1号(上)立板法向Z坐标分量
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"UpVertPlateNorm.z","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//2号(下)立板工作法向
		pPropItem=oper.InsertCmbListPropItem(pParentItem,"DownVertPlateNorm","","","",-1,TRUE);
		pPropItem->m_bHideChildren=TRUE;
		//2号(下)立板法向X坐标分量
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"DownVertPlateNorm.x","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//2号(下)立板法向Y坐标分量
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"DownVertPlateNorm.y","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//2号(下)立板法向Z坐标分量
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"DownVertPlateNorm.z","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
	}
}
void CDesignTubeEndLjDlg::UpdateBoltProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	CPropTreeItem* pGroupItem=NULL,*pPropItem=NULL;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	//螺栓基本信息
	if(m_iTubeEndLjStyle==2)
	{	//钢管半径
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_fTubeR","钢管外径","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropValue.Format("%g",m_pCurLineTube->GetDiameter());
		pPropItem->SetReadOnly(TRUE);
	}
	//螺栓布置参数
	if(m_iTubeEndLjStyle==2)
	{
		//螺栓直径
		pPropItem=oper.InsertCmbEditPropItem(pParentItem,"m_nBoltD","","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType!=0);
		//螺栓个数
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_nBoltNum","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType!=0);
		//螺栓所在圆半径
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_fBoltLayoutD","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType!=0);
		//螺栓朝向
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_iDriection","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType!=0);
		//起始边
		if(m_pCurLineTube->m_bHasWeldRoad&&m_iPartType==0)
			oper.InsertCmbListPropItem(pParentItem,"m_iStartEdge","","","",-1,TRUE);
		//首螺栓偏移角 
		//环向布置螺栓时螺栓偏移角度指相对于定位基准构件X轴的偏移
		//此处将首螺栓偏移角度保存至'X'参数中 wht 11-05-26
		if(m_iPartType==0)
			oper.InsertEditPropItem(pParentItem,"m_fOffsetAngle","","",-1,TRUE);
	}
	else 
	{	//插板螺栓布置参数
		pPropItem=oper.InsertCmbEditPropItem(pParentItem,"m_nBoltD","","","",-1,TRUE);	//螺栓直径
		pPropItem->SetReadOnly(m_iPartType>0);
		//螺栓个数
		if(m_iTubeEndLjStyle==5)
			pPropItem=oper.InsertEditPropItem(pParentItem,"m_nBoltNum","1/4螺栓数","1/4螺栓数",-1,TRUE);
		else
			pPropItem=oper.InsertEditPropItem(pParentItem,"m_nBoltNum","螺栓个数","螺栓个数",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType>0);
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_nRowNum","","",-1,TRUE);	//螺栓行数
		pPropItem->SetReadOnly(m_iPartType>0);
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_nColNum","","",-1,TRUE);	//螺栓列数
		pPropItem->SetReadOnly(m_iPartType>0);
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_fRowSpace","","",-1,TRUE);	//螺栓行距
		pPropItem->SetReadOnly(m_iPartType>0);
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_fColSpace","","",-1,TRUE);	//螺栓列距
		pPropItem->SetReadOnly(m_iPartType>0);
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_posFirstBolt.x","","",-1,TRUE);	//首螺栓X坐标
		pPropItem->SetReadOnly(m_iPartType>0);
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_posFirstBolt.y","","",-1,TRUE);	//首螺栓Y坐标
		pPropItem->SetReadOnly(m_iPartType>0);
		if(m_iTubeEndLjStyle==5)
		{	//十字插板
			pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_bStaggerLayout","单行布置时支持交错布置","","",-1,TRUE);	//交错布置水平面与竖直面螺栓
			pPropItem->SetReadOnly(m_iPartType>0||LayoutBoltPara.m_nRowNum>1);
			//pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_bMirLayout","","","",-1,TRUE);	//对称布置
			//pPropItem->SetReadOnly(m_iPartType>0);
			//if(LayoutBoltPara.m_bMirLayout)
				//oper.InsertEditPropItem(pPropItem,"m_fSpecialSpace","","",-1,TRUE);	//特殊间距	
		}
		pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_iDriection","","","",-1,TRUE);	//螺栓朝向
		pPropItem->SetReadOnly(m_iPartType!=0);
	}
}
void CDesignTubeEndLjDlg::UpdatePartTypeProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	//
	CPropTreeItem* pPropItem=NULL;
	//普通十字插板也应该支持包钢代包板的形式 wht 10-10-18
	if(m_iTubeEndLjStyle==5)	//十字插板包板类型 
		oper.InsertCmbListPropItem(pParentItem,"m_iWrapPlateType","","","",-1,TRUE);
	if(m_iTubeEndLjStyle>2&&m_iPartType>0)	//插板选择材质
		oper.InsertCmbListPropItem(pParentItem,"m_cStdPartMat","","","",-1,TRUE);
	if(m_iTubeEndLjStyle==2&&!m_bHasExistEndLj)
		oper.InsertCmbListPropItem(pParentItem,"m_bCoupleFLCreate","","","",-1,TRUE);
	//级别
	if((m_iTubeEndLjStyle==2&&m_iPartType==2)||m_iTubeEndLjStyle>2)
	{
		if(m_iTubeEndLjStyle==2)
		{
			oper.InsertCmbListPropItem(pParentItem,"m_cStdPartMat","","选择材质","法兰的选择材质(法兰材质)",-1,TRUE);
			if(m_arrStdFlDLevelM.GetSize()>0)
			{
				CXhChar100 sLevelArr;
				pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_iStdPartLevel","","主管法兰级别","",-1,TRUE);
				for(int i=0;i<m_arrStdFlDLevelM.GetSize();i++)
				{
					CXhChar50 sLevel;
					sLevel.Printf("%s|",m_arrStdFlDLevelM.GetAt(i));
					sLevelArr.Append(sLevel);
				}
				pPropItem->m_lpNodeInfo->m_cmbItems.Format("%s",(char*)sLevelArr);
			}
			if(m_arrStdFlDLevelC.GetSize()>0)
			{
				CXhChar100 sLevelArr;
				pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_iStdPartLevel2","","对接法兰级别","",-1,TRUE);
				pParentItem->SetReadOnly(!m_bCoupleFLCreate);
				for(int i=0;i<m_arrStdFlDLevelC.GetSize();i++)
				{
					CXhChar50 sLevel;
					sLevel.Printf("%s|",m_arrStdFlDLevelC.GetAt(i));
					sLevelArr.Append(sLevel);
				}
				pPropItem->m_lpNodeInfo->m_cmbItems.Format("%s",(char*)sLevelArr);
			}
		}
		else
		{
			CXhChar100 sLevelArr;
			pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_iStdPartLevel","","插板级别","",-1,TRUE);
			for(int i=0;i<m_arrStdPartLevel.GetSize();i++)
			{
				CXhChar50 sLevel;
				sLevel.Printf("%s|",m_arrStdPartLevel.GetAt(i));
				sLevelArr.Append(sLevel);
			}
			pPropItem->m_lpNodeInfo->m_cmbItems.Format("%s",(char*)sLevelArr);
		}
	}
}
void CDesignTubeEndLjDlg::DisplayPropertyList(int iCurSel/*=0*/)
{
	const int GROUP_GENERAL=1,GROUP_PROFILE=2,GROUP_LAYOUT_BOLT=3,GROUP_ATTACH_PLATE=4;
	m_arrPropGroupLabel.RemoveAll();
	if(m_iTubeEndLjStyle<2)
		m_arrPropGroupLabel.SetSize(0);
	else //if(m_iTubeEndLjStyle>=2)
	{
		if(m_iTubeEndLjStyle==2)
		{
			if(!IsHasExistEndLj()&&(m_iPartType==0||m_iPartType==3||m_iPartType==4))
				m_arrPropGroupLabel.SetSize(4);
			else 
				m_arrPropGroupLabel.SetSize(3);
			m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"常规");
			m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"外形");
			m_arrPropGroupLabel.SetAt(GROUP_LAYOUT_BOLT-1,"螺栓");
			if(!IsHasExistEndLj()&&(m_iPartType==0||m_iPartType==3||m_iPartType==4))
				m_arrPropGroupLabel.SetAt(GROUP_ATTACH_PLATE-1,"附加板");
		}
		else 
		{
			int nGroupCount=0;
			if(!IsHasExistEndLj()&&(m_iTubeEndLjStyle==4||m_iTubeEndLjStyle==5))
				nGroupCount=4;
			else
				nGroupCount=3;
			if(!IsHasExistEndLj()&&m_bDesignNodePlate)
				nGroupCount++;
			m_arrPropGroupLabel.SetSize(nGroupCount);
			m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"常规");
			m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"外形");
			m_arrPropGroupLabel.SetAt(GROUP_LAYOUT_BOLT-1,"螺栓");
			if(!IsHasExistEndLj()&&(m_iTubeEndLjStyle==4||m_iTubeEndLjStyle==5))	//卷边插板|十字插板需要设计附加板
				m_arrPropGroupLabel.SetAt(GROUP_ATTACH_PLATE-1,"附加板");
			if(!IsHasExistEndLj()&&m_bDesignNodePlate)		//节点板属性位于最后一个属性组 wht 11-04-12
				m_arrPropGroupLabel.SetAt(nGroupCount-1,"节点板");
		}	
	}
	RefreshTabCtrl(iCurSel);
	//设置属性栏回调函数
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(ModifyItemValue);
	m_propList.SetButtonClickFunc(ButtonClickItem);
	m_propList.SetPropHelpPromptFunc(PropHelpPrompt);
	m_propList.SetPopMenuClickFunc(FirePopMenuClick);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	CPropTreeItem *pPropItem,*pGroupItem,*pRoot=m_propList.GetRootItem();
	CString sPartName=GetTubeEndLjName();
	if(m_iTubeEndLjStyle<=1)
	{	
		if(m_iTubeEndLjStyle==0)
		{	//平面切割
			oper.InsertBtnEditPropItem(pRoot,"m_hWeldLinkObj");
			pGroupItem=oper.InsertGrayButtonPropItem(pRoot,"cutPlaneOrigin");
			pGroupItem->m_bHideChildren = FALSE;
			//X
			oper.InsertEditPropItem(pGroupItem,"cutPlaneOrigin.x");
			//Y
			oper.InsertEditPropItem(pGroupItem,"cutPlaneOrigin.y");
			//Z
			oper.InsertEditPropItem(pGroupItem,"cutPlaneOrigin.z");
			//法线方向
			pGroupItem=oper.InsertGrayButtonPropItem(pRoot,"cutPlaneNorm");
			pGroupItem->m_bHideChildren = FALSE;
			//X
			oper.InsertEditPropItem(pGroupItem,"cutPlaneNorm.x");
			//Y
			oper.InsertEditPropItem(pGroupItem,"cutPlaneNorm.y");
			//Z
			oper.InsertEditPropItem(pGroupItem,"cutPlaneNorm.z");
		}
		else
		{	//钢管相贯
			pGroupItem=oper.InsertPropItem(pRoot,"TubeTransect");
			pGroupItem->m_bHideChildren = FALSE;
			//对接相贯
			oper.InsertCmbListPropItem(pGroupItem,"m_bFaceToFace");
			//相贯钢管1
			oper.InsertBtnEditPropItem(pGroupItem,"m_hDatumTube1");
			if(!m_bFaceToFace)	//相贯钢管2
				oper.InsertBtnEditPropItem(pGroupItem,"m_hDatumTube2");
		}
		//钢管端连接位置
		pGroupItem=oper.InsertPropItem(pRoot,"TubeEndPos");
		pGroupItem->m_bHideChildren=FALSE;
		//正负头
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"m_fOddment");
		pPropItem->m_lpNodeInfo->m_strPropName=(m_bStartEnd)?"始端正负头":"终端正负头";
		//钢管偏心距
		if(m_iTubeEndLjStyle==0)
			oper.InsertEditPropItem(pGroupItem,"m_fTubeEccentricDist");
	}
	else
	{
		if((m_iTubeEndLjStyle==2&&(m_pFlDParam||m_pFlPParam||m_pFlexibleFlParam||m_pRigidFlParam))
			||(m_iTubeEndLjStyle>2&&m_pInsertPlateParam))
		{	//存在标准构件
			pGroupItem=oper.InsertCmbListPropItem(pRoot,"m_iPartType");
			pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if(m_iTubeEndLjStyle==2)
			{
				CXhChar100 strItem("0.普通法兰|");
				if(m_pFlPParam)
					strItem.Append("1.平焊法兰|");
				if(m_pFlDParam)
					strItem.Append("2.对焊法兰|");
				if(m_pRigidFlParam)
					strItem.Append("3.刚性法兰|");
				if(m_pFlexibleFlParam)
					strItem.Append("4.柔性法兰|");
				pGroupItem->m_lpNodeInfo->m_cmbItems.Format("%s",(char*)strItem);
			}
			else 
				pGroupItem->m_lpNodeInfo->m_cmbItems = "0.自定义插板|1.标准插板";
			//
			UpdatePartTypeProperty(pGroupItem);
		}
		//基本信息
		pGroupItem=oper.InsertPropItem(pRoot,"basicInfo");
		pGroupItem->m_bHideChildren=FALSE;
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		UpdateBaseInfoProperty(pGroupItem);
		//钢管端连接位置
		pGroupItem=oper.InsertPropItem(pRoot,"TubeEndPos");
		pGroupItem->m_bHideChildren=FALSE;
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		UpdateTubeEndProperty(pGroupItem);
		//装配参数
		pGroupItem=oper.InsertPropItem(pRoot,"SetUpPara");
		pGroupItem->m_bHideChildren=FALSE;
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		UpdateSetUpProperty(pGroupItem);
		//外形参数
		pGroupItem=oper.InsertPropItem(pRoot,"Profile");
		pGroupItem->m_bHideChildren=FALSE;//(m_iPartType==0);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		UpdateProfileProperty(pGroupItem);
		//螺栓布置参数
		if(m_iTubeEndLjStyle>2)
			pGroupItem=oper.InsertGrayPopMenuItem(pRoot,"LayoutBoltParam");
		else
			pGroupItem=oper.InsertPropItem(pRoot,"LayoutBoltParam");
		pGroupItem->m_bHideChildren=FALSE;
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_LAYOUT_BOLT);
		UpdateBoltProperty(pGroupItem);
		//附加板参数
		if((m_iTubeEndLjStyle==2&&(m_iPartType==0||m_iPartType==3||m_iPartType==4))||
			m_iTubeEndLjStyle==5||m_iTubeEndLjStyle==4)
		{
			pGroupItem=oper.InsertCmbListPropItem(pRoot,"m_bCreateAttachPlate");
			pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_ATTACH_PLATE);
			pGroupItem->SetReadOnly(GetPartType()>2);	//刚性法兰及柔性法兰均不允许修改
			UpdateAttachPlateProperty(pGroupItem);
			//填板参数 wht 11-05-11
			if(m_iTubeEndLjStyle==5)
			{
				pGroupItem=oper.InsertCmbListPropItem(pRoot,"m_bCreateFillPlate");
				pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_ATTACH_PLATE);
				//显示填板参数
				UpdateFillPlateProperty(pGroupItem);
			}
		}
		//
		if(m_bDesignNodePlate)
		{	//节点板参数
			pGroupItem=oper.InsertPropItem(pRoot,"NodePlateBasicInfo");
			pGroupItem->m_bHideChildren=FALSE;
			pGroupItem->m_dwPropGroup=GetSingleWord(m_arrPropGroupLabel.GetSize());
			//
			oper.InsertEditPropItem(pGroupItem,"NodePlatePara.sPartNo");
			//
			oper.InsertEditPropItem(pGroupItem,"NodePlatePara.cMaterial");
			//
			oper.InsertEditPropItem(pGroupItem,"NodePlatePara.m_nThick");
		}
	}
	m_propList.Redraw();
}

void CDesignTubeEndLjDlg::OnSelchangeTubeEndljStyle() 
{
	int iOldStyle=m_iTubeEndLjStyle;
	UpdateData();
	if(m_bDesignNodePlate&&m_iTubeEndLjStyle<3)
	{	//射线杆件仅允许布置插板
		m_iTubeEndLjStyle=iOldStyle;
		AfxMessageBox("射线杆件端连接样式只允许为插板！");
		UpdateData(FALSE);
	}
	UpdateDesignParam();
	if(m_iTubeEndLjStyle==5&&setupInfo.workNorm.hVicePart==0)	//十字插板
	{
		setupInfo.workNorm.norm_style=5;
		setupInfo.workNorm.rot_ang=0;
		setupInfo.workNorm.hVicePart=m_pCurLineTube->handle;
		setupInfo.workNorm.UpdateVector(console.GetActiveModel());
		setupInfo.workNormUpVert.norm_style=setupInfo.workNormDownVert.norm_style=4;
		setupInfo.workNormUpVert.vector=setupInfo.workNormUpVert.nearVector;
		setupInfo.workNormDownVert.vector=setupInfo.workNormDownVert.nearVector;
		if(m_pCurLineTube->pStart==m_pBaseNode)
			m_paramPlate.ucs.axis_x=m_pCurLineTube->Start()-m_pCurLineTube->End();
		else
			m_paramPlate.ucs.axis_x=m_pCurLineTube->End()-m_pCurLineTube->Start();
		m_paramPlate.ucs.axis_z=setupInfo.workNorm.vector;
		m_paramPlate.ucs.axis_y=m_paramPlate.ucs.axis_z^m_paramPlate.ucs.axis_x;
		normalize(m_paramPlate.ucs.axis_y);
		m_paramPlate.ucs.axis_x=m_paramPlate.ucs.axis_y^m_paramPlate.ucs.axis_z;
		normalize(m_paramPlate.ucs.axis_x);
		vector_trans(setupInfo.workNormUpVert.vector,m_paramPlate.ucs,TRUE);
		vector_trans(setupInfo.workNormDownVert.vector,m_paramPlate.ucs,TRUE);
	}
	UpdateLayoutBoltPara();
	DisplayPropertyList();
	UpdateSketchMap();
	//
	RECT rc,rc1,rc2;
	GetDlgItem(IDC_S_PIC_PLATE_TYPE)->GetWindowRect(&rc1);
	GetDlgItem(IDC_S_MAP)->GetWindowRect(&rc2);
	//向四周各扩5个像素，用于刷新因文字标注超出的部分
	rc.left = min(rc1.left,rc2.left)-5;
	rc.top = min(rc1.top,rc2.top)-5;
	rc.right=max(rc1.right,rc2.right)+5;
	rc.bottom=max(rc1.bottom,rc2.bottom)+5;
	ScreenToClient(&rc);
	UpdateData(FALSE);
	InvalidateRect(&rc,TRUE);
}

void CDesignTubeEndLjDlg::OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iCurSel=m_ctrlPropGroup.GetCurSel();
	m_propList.m_iPropGroup = iCurSel;
	m_propList.Redraw();
	//更新示意图
	UpdateSketchMap(iCurSel);
	*pResult = 0;
}

void CDesignTubeEndLjDlg::RefreshTabCtrl(int iCurSel)
{
	m_ctrlPropGroup.DeleteAllItems();
	for(int i=0;i<m_arrPropGroupLabel.GetSize();i++)
		m_ctrlPropGroup.InsertItem(i,m_arrPropGroupLabel[i]);
	if(m_arrPropGroupLabel.GetSize()>0)
	{
		m_ctrlPropGroup.SetCurSel(iCurSel);
		m_propList.m_iPropGroup=iCurSel;
	}
	else //不需要分组显示
		m_propList.m_iPropGroup=0;
	
	//根据分组数调整窗口位置
	RECT rcPropList,rcMainWnd;
	GetClientRect(&rcMainWnd);
	CWnd *pTopWnd = GetDlgItem(IDC_LIST_BOX);
	if(pTopWnd)
		pTopWnd->GetWindowRect(&rcPropList);
	ScreenToClient(&rcPropList);
	if(m_arrPropGroupLabel.GetSize()>0)
		rcPropList.top=rcMainWnd.top+60;
	else 
		rcPropList.top=rcMainWnd.top+35;
	if(pTopWnd)
		pTopWnd->MoveWindow(&rcPropList);
}
void CDesignTubeEndLjDlg::Redraw()
{
	CRect rect;
	GetDlgItem(IDC_S_MAP)->GetWindowRect(&rect);
	rect.top-=100;
	InvalidateRect(rect,FALSE);
}
void CDesignTubeEndLjDlg::RefreshCtrlState(BOOL bShowBolt/*=TRUE*/)
{
	RECT rc;
	GetDlgItem(IDC_WORK_PANEL)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_xSubPropList.MoveWindow(&rc);
	//
	m_listBoltParam.ShowWindow(bShowBolt);
	m_listBolt.ShowWindow(bShowBolt);
	m_xSubPropList.ShowWindow(!bShowBolt);
	GetDlgItem(IDC_BTN_PARTS_DB)->ShowWindow(!m_bHasExistEndLj);
	GetDlgItem(IDC_BTN_DEL_CUR_ENDLJ)->ShowWindow(m_bHasExistEndLj);
	GetDlgItem(IDC_CMB_TUBE_ENDLJ_STYLE)->EnableWindow(!m_bSpecEndLjStyle&&!m_bHasExistEndLj);
}
void CDesignTubeEndLjDlg::RefreshBoltListColumn()
{
	//刷新标题列
	m_listBolt.m_arrHeader.RemoveAll();
	if(m_iTubeEndLjStyle==2)
	{	//
		m_listBolt.AddColumnHeader("直径");
		m_listBolt.AddColumnHeader("R");
		m_listBolt.AddColumnHeader("角度");
		long col_wide_arr[3]={50,100,100};
		m_listBolt.InitListCtrl(col_wide_arr);
	}
	else
	{
		m_listBolt.AddColumnHeader("直径");
		m_listBolt.AddColumnHeader("X");
		m_listBolt.AddColumnHeader("Y");
		m_listBolt.AddColumnHeader("Z");
		long col_wide_arr[4]={50,70,70,60};
		m_listBolt.InitListCtrl(col_wide_arr);
	}
}
void CDesignTubeEndLjDlg::RefreshBoltParaListColumn()
{
	m_listBoltParam.EmptyColumnHeader();
	if(m_iTubeEndLjStyle==2)
	{	//法兰标题列
		m_listBoltParam.AddColumnHeader("螺栓直径");
		m_listBoltParam.AddColumnHeader("螺栓个数");
		m_listBoltParam.AddColumnHeader("所在圆直径");
		long para_col_wide_arr[3]={92,92,92};
		m_listBoltParam.InitListCtrl(para_col_wide_arr);
	}
	else
	{	//插板标题列
		m_listBoltParam.AddColumnHeader("S");
		m_listBoltParam.AddColumnHeader("R");
		m_listBoltParam.AddColumnHeader("C");
		m_listBoltParam.AddColumnHeader("直径");
		m_listBoltParam.AddColumnHeader("行距");
		m_listBoltParam.AddColumnHeader("列距");
		m_listBoltParam.AddColumnHeader("X");
		m_listBoltParam.AddColumnHeader("Y");
		long para_col_wide_arr[8]={25,30,30,40,40,40,45,45};
		m_listBoltParam.InitListCtrl(para_col_wide_arr);
	}
	
}
void CDesignTubeEndLjDlg::RefreshBoltParamList()
{
	RefreshBoltParaListColumn();
	//
	CString sTextValue;
	m_listBoltParam.DeleteAllItems();
	if(m_iTubeEndLjStyle==2)
	{	//法兰
		CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
		//直径
		sTextValue.Format("%d",LayoutBoltPara.m_nBoltD);
		lpInfo->AddSubItemText(sTextValue);
		lpInfo->SetControlType(0,GCT_CMB_EDIT);
		lpInfo->SetListItemsStr(0,"12|16|20|24|27|30|");
		lpInfo->SetSubItemReadOnly(0,m_iPartType>0);
		//螺栓总数
		sTextValue.Format("%d",LayoutBoltPara.m_nBoltNum);		
		lpInfo->AddSubItemText(sTextValue);
		lpInfo->SetSubItemReadOnly(1,m_iPartType>0);
		//螺栓所在圆直径
		sTextValue.Format("%f",LayoutBoltPara.m_fBoltLayoutD);
		SimplifiedNumString(sTextValue);
		lpInfo->AddSubItemText(sTextValue);
		lpInfo->SetSubItemReadOnly(2,m_iPartType>0);
		//
		m_listBoltParam.InsertRootItem(lpInfo);
	}
	else if(m_iTubeEndLjStyle>2)
	{	//插板
		for(LAYOUT_BOLT_PARA *pBoltPara=LayoutBoltParaList.GetFirst();pBoltPara;pBoltPara=LayoutBoltParaList.GetNext())
		{
			CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
			//螺栓总数
			sTextValue.Format("%d",pBoltPara->m_nBoltNum);		
			lpInfo->AddSubItemText(sTextValue);	
			lpInfo->SetSubItemHelpText(0,"S表示螺栓总数");
			lpInfo->SetSubItemReadOnly(0,m_iPartType>0);
			//螺栓行数
			sTextValue.Format("%d",pBoltPara->m_nRowNum);		
			lpInfo->AddSubItemText(sTextValue,m_iTubeEndLjStyle==5&&sTextValue.CompareNoCase("-1")==0);
			lpInfo->SetSubItemHelpText(1,"R==-1或者C==-1表示水平面与竖直面交错布置螺栓");
			lpInfo->SetSubItemReadOnly(1,m_iPartType>0);
			//螺栓列数
			sTextValue.Format("%d",pBoltPara->m_nColNum);		
			lpInfo->AddSubItemText(sTextValue,m_iTubeEndLjStyle==5&&sTextValue.CompareNoCase("-1")==0);
			lpInfo->SetSubItemHelpText(2,"R==-1或者C==-1表示水平面与竖直面交错布置螺栓");
			lpInfo->SetSubItemReadOnly(2,m_iPartType>0);
			//直径
			sTextValue.Format("%d",pBoltPara->m_nBoltD);
			lpInfo->AddSubItemText(sTextValue);
			lpInfo->SetControlType(3,GCT_CMB_EDIT);
			lpInfo->SetListItemsStr(3,"12|16|20|24|27|30|");
			lpInfo->SetSubItemReadOnly(3,m_iPartType>0);
			//行距
			sTextValue.Format("%.f",pBoltPara->m_fRowSpace);
			lpInfo->AddSubItemText(sTextValue);
			lpInfo->SetSubItemHelpText(4,"行距");
			lpInfo->SetSubItemReadOnly(4,m_iPartType>0);
			//列距
			sTextValue.Format("%.f",pBoltPara->m_fColSpace);		
			lpInfo->AddSubItemText(sTextValue);
			lpInfo->SetSubItemHelpText(5,"列距");
			lpInfo->SetSubItemReadOnly(5,m_iPartType>0);
			//X
			sTextValue.Format("%.f",pBoltPara->m_posFirstBolt.x);		
			lpInfo->AddSubItemText(sTextValue);
			lpInfo->SetSubItemHelpText(6,"首螺栓X坐标");
			lpInfo->SetSubItemReadOnly(6,m_iPartType>0);
			//Y
			sTextValue.Format("%.f",pBoltPara->m_posFirstBolt.y);
			lpInfo->AddSubItemText(sTextValue);
			lpInfo->SetSubItemHelpText(7,"首螺栓Y坐标");
			lpInfo->SetSubItemReadOnly(7,m_iPartType>0);
			//
			CSuperGridCtrl::CTreeItem* pItem=m_listBoltParam.InsertRootItem(lpInfo);
			if(pItem)
				pItem->m_idProp=(DWORD)pBoltPara;;
		}
	}
	m_listBoltParam.Redraw();
}
void CDesignTubeEndLjDlg::RefreshBoltList()
{
	RefreshBoltListColumn();
	//
	m_listBolt.DeleteAllItems();//清空螺栓列表
	CStringArray str_arr;
	if(m_iTubeEndLjStyle==2)
	{	//法兰
		str_arr.SetSize(3);
		double fInitAngle=(m_iPartType==0)?LayoutBoltPara.m_fOffsetAngle:0;;
		double fPieceAngle=360.0/LayoutBoltPara.m_nBoltNum;
		for(int i=0;i<LayoutBoltPara.m_nBoltNum;i++)
		{
			str_arr[0].Format("M%d",LayoutBoltPara.m_nBoltD);
			str_arr[1].Format("%f",LayoutBoltPara.m_fBoltLayoutD*0.5);
			SimplifiedNumString(str_arr[1]);
			str_arr[2].Format("%f",fInitAngle+fPieceAngle*i);
			SimplifiedNumString(str_arr[2]);
			m_listBolt.InsertItemRecord(-1,str_arr);
		}
	}
	else if(m_iTubeEndLjStyle>2)
	{	//插板
		//根据螺栓布置参数计算螺栓坐标
		ATOM_LIST<f3dPoint> boltPts;
		LayoutBolt(boltPts);
		//
		str_arr.SetSize(4);
		int nBoltNum=0;
		for(f3dPoint *pPt=boltPts.GetFirst();pPt;pPt=boltPts.GetNext())
		{
			nBoltNum++;
			//根据螺栓数判断螺栓属于第几组参数
			DWORD dwFlag=0;
			int nSumBolt=0,index=0;
			for(index=0;index<LayoutBoltParaList.GetNodeNum();index++)
			{
				if(m_iTubeEndLjStyle==5)	//十字插板
					nSumBolt+=LayoutBoltParaList[index].m_nBoltNum*4;
				else 
					nSumBolt+=LayoutBoltParaList[index].m_nBoltNum;
				if(nBoltNum<=nSumBolt)
				{	//从1开始,因为0没有正负,此处需要使用正负号,表示第几组螺栓
					dwFlag=index+1;
					break;
				}
			}
			if(m_iTubeEndLjStyle==5)	//十字插板,X-Y面上的螺栓为正，X-Z面上的螺栓为负
			{
				WORD* pwFlg=(WORD*)&dwFlag;
				if(pPt->feature==31||pPt->feature==41)
					(*pwFlg)*=100;
				else
					(*pwFlg)*=1;
				*(pwFlg+1)=(WORD)pPt->feature;
			}
			//
			str_arr[0].Format("M%d",LayoutBoltParaList[index].m_nBoltD);
			str_arr[1].Format("%f",pPt->x);
			SimplifiedNumString(str_arr[1]);
			str_arr[2].Format("%f",pPt->y);
			SimplifiedNumString(str_arr[2]);
			str_arr[3].Format("%f",pPt->z);
			SimplifiedNumString(str_arr[3]);
			int iItem=m_listBolt.InsertItemRecord(-1,str_arr);
			m_listBolt.SetItemData(iItem,dwFlag);		//螺栓位置标志
		}
	}
}
//生成附加钢板
void CDesignTubeEndLjDlg::CreateAttachPlate(CLDSParamPlate *pParamPalte,CLDSLineTube *pDatumTube,BOOL bFlCoupleDesign/*=FALSE*/)
{
	CXhChar100 sText;
	int nCount=AttachPlatePara.m_nPlateNum;
	double sector_angle=360.0/nCount;
	for(int i=0;i<nCount;i++)
	{
		CLDSParamPlate *pElbowPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pElbowPlate->SetLayer(pDatumTube->layer()); //调整图层名
		pElbowPlate->cfgword=pDatumTube->cfgword;
		pElbowPlate->m_iParamType = TYPE_ELBOWPLATE;
		pElbowPlate->m_hPartWeldParent = pParamPalte->handle;
		pElbowPlate->cfgword=pParamPalte->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pElbowPlate->SetDesignItemValue('A',AttachPlatePara.A,"肘板参数A",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('B',AttachPlatePara.B,"肘板参数B",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('C',AttachPlatePara.C,"肘板参数C",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('D',AttachPlatePara.D,"肘板参数D",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('E',AttachPlatePara.E,"肘板参数E",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('F',AttachPlatePara.F,"肘板参数F",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('G',AttachPlatePara.G,"肘板参数G",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('H',AttachPlatePara.H,"肘板参数H",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('e',AttachPlatePara.m_iCutMode,"",PARAM_SHAPE,1);
		if(pParamPalte->ucs.axis_z*pDatumTube->ucs.axis_z>0)
			pElbowPlate->SetDesignItemValue('K',-pDatumTube->handle,"肘板所依靠的基准钢管",PARAM_POS,1);
		else
			pElbowPlate->SetDesignItemValue('K',pDatumTube->handle,"肘板所依靠的基准钢管",PARAM_POS,1);
		pElbowPlate->SetDesignItemValue('L',pParamPalte->handle,"基准底板",PARAM_POS,1);
		pElbowPlate->m_bDisplayHole=TRUE;
		//不等径对接时调整肋板宽度 wht 10-09-29
		double offset_r=0.5*(m_pCurLineTube->GetDiameter()-pDatumTube->GetDiameter());
		if(offset_r>EPS2)
		{
			DESIGN_PLATE_ITEM *pItem=pElbowPlate->GetDesignItem('B');
			if(pItem)	//肋板宽度
				pElbowPlate->SetDesignItemValue(pItem->key,pItem->value.fValue+offset_r,pItem->sNotes,pItem->paramType);
		}
		pElbowPlate->SetPartNo(AttachPlatePara.sPartNo);	//设置法兰肋板编号 wht 12-08-24
		//设定材质
		pElbowPlate->cMaterial=AttachPlatePara.cMaterial;
		//方位角
		double angle=AttachPlatePara.m_fAngle+i*sector_angle;
		if(bFlCoupleDesign)
			angle*=-1;
		pElbowPlate->SetDesignItemValue('Y',angle,"肘板定位方向角°");
		pElbowPlate->SetDesignItemValue('b',1,"肘板定位的角度调整方式;0.旧版本以钢管为基准;1.新版本以依赖钢板为基准");
		pElbowPlate->iSeg=pParamPalte->iSeg;
		pElbowPlate->SetLayer(pParamPalte->layer());	//调整肘板的图层名
		pElbowPlate->thick = AttachPlatePara.m_nThick;
		//统一将肘板转换成肋板处理  wxc-2017.5.26
		pElbowPlate->ShiftElbowAndRibPlateSchema(TYPE_RIBPLATE);
		pElbowPlate->DesignPlate();
		double scale_user2scr=g_pSolidOper->GetScaleUserToScreen();
		double sample_len=g_pSolidSet->GetArcSamplingLength();
		pElbowPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,scale_user2scr,sample_len,g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pElbowPlate->GetSolidPartObject());
		g_pSolidDraw->Draw();
		pParamPalte->AppendAttachPart(pElbowPlate);
	}
}

void CDesignTubeEndLjDlg::OnConfirmCreate() 
{
	if((m_iTubeEndLjStyle==3||m_iTubeEndLjStyle==4||m_iTubeEndLjStyle==5)&&!m_bHasExistEndLj)
	{	//判断正负头，如果碰撞提示用户 wht 11-05-11
		DESIGNODDMENT * pCurOddment=&(m_pCurLineTube->desEndOdd);
		if(m_bStartEnd)
			pCurOddment=&(m_pCurLineTube->desStartOdd);
		pCurOddment->m_fCollideDist=setupInfo.m_fWeldHeight+g_sysPara.VertexDist;
		long hOldRefPart1=pCurOddment->m_hRefPart1;	//记录旧的碰撞基准杆件
		if(console.FromPartHandle(pCurOddment->m_hRefPart1)==NULL
			&&console.FromPartHandle(pCurOddment->m_hRefPart2)==NULL)
		{	
			if(m_pBaseNode->hFatherPart!=m_pCurLineTube->handle)
				pCurOddment->m_hRefPart1=m_pBaseNode->hFatherPart;
		}
		//根据碰撞计算正负头,此处由用户点击按钮触发故不用判断判断正负头计算方式 
		double fOddment=m_pCurLineTube->CalOddment(m_bStartEnd,&m_paramPlate,true);
		pCurOddment->m_hRefPart1=hOldRefPart1;
		if(setupInfo.m_fOddment>fOddment)
		{
			if(AfxMessageBox("插板与钢管可能发生碰撞,是否通过【焊缝高度】后的按钮计算负头?",MB_YESNO)==IDYES)
			{	//根据焊缝高度直接计算正负头
				setupInfo.m_fOddment=m_pCurLineTube->CalOddment(m_bStartEnd,&m_paramPlate,true);
			}
		}
	}
	if(m_iTubeEndLjStyle==5)
	{
		if(m_bDesignNodePlate)	//设计节点板
			FillPlatePara.nNodePlateThick=NodePlatePara.m_nThick;
		if(FillPlatePara.nNodePlateThick>m_paramPlate.thick&&!m_bCreateFillPlate
			&&AfxMessageBox("节点板厚度大于十字插板基板厚度，是否需要在附加板属性页中输入填板参数?",MB_YESNO)==IDYES)
		{	//切换到附加板属性页，方便输入填板参数 wht 11-05-11
			FillPlatePara.nThick=(int)(0.5*FillPlatePara.nNodePlateThick-0.5*m_paramPlate.thick);
			m_ctrlPropGroup.SetCurSel(2);
			m_propList.m_iPropGroup = 2;
			m_propList.Redraw();
			//更新示意图
			CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
			m_CurPlateBmp.Detach();
			m_CurPlateBmp.LoadBitmap(IDB_BMP_XEND_RIB_PLATE1); //附加板
			pPic->SetBitmap(m_CurPlateBmp);
			//更新输入界面
			m_bCreateFillPlate=TRUE;
			CPropTreeItem* pFindItem=m_propList.FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("m_bCreateFillPlate"),NULL);
			if(pFindItem)
			{
				m_propList.SetItemPropValue(pFindItem->m_idProp,"是");
				UpdateFillPlateProperty(pFindItem);
			}
			return;
		}
	}
	//异形法兰射线杆件 自动调整钢管工作面法线方向
	if(m_bSpecialFlRayPole&&(m_iTubeEndLjStyle>=3||m_iTubeEndLjStyle<=5))
	{
		m_pCurLineTube->m_iNormCalStyle=0;	//直接指定工作面法线
		m_pCurLineTube->SetWorkPlaneNorm(f3dPoint(0,0,1));
	}
	if(m_iTubeEndLjStyle==0)
	{	//平面切割
		TUBEJOINT *pCurJoint=NULL;
		if(m_bStartEnd)
		{
			m_pCurLineTube->SetStartOdd(setupInfo.m_fOddment);
			m_pCurLineTube->desStartPos.fNormOffset=setupInfo.m_fNormOffset;
			m_pCurLineTube->desStartPos.fEccentricDist=setupInfo.m_fTubeEccentricDist;
			pCurJoint=&m_pCurLineTube->m_tJointStart;
		}
		else
		{
			m_pCurLineTube->SetEndOdd(setupInfo.m_fOddment);
			m_pCurLineTube->desEndPos.fNormOffset=setupInfo.m_fNormOffset;
			m_pCurLineTube->desEndPos.fEccentricDist=setupInfo.m_fTubeEccentricDist;
			pCurJoint=&m_pCurLineTube->m_tJointEnd;
		}
		pCurJoint->type=m_iTubeEndLjStyle;//平面切割
		pCurJoint->cutPlanePos=cutPlaneOrigin;
		pCurJoint->cutPlaneNorm=cutPlaneNorm;
		pCurJoint->hLinkObj=m_hWeldLinkObj;
	}
	else if(m_iTubeEndLjStyle==1)
	{	//钢管相贯
		long hOldLinkTube=0,hOldViceLinkTube=0,hLinkObj=0,hViceLinkObj=0;
		TUBEJOINT *pCurrentJoint=NULL;
		f3dPoint cur_tube_vec,link_tube_vec,cutPlanePos;
		//删除钢管对接时更新对接钢管的端连接
		if(m_bStartEnd)
		{
			hOldLinkTube = m_pCurLineTube->m_tJointStart.hLinkObj;
			hOldViceLinkTube = m_pCurLineTube->m_tJointStart.hViceLinkObj;
			cur_tube_vec=m_pCurLineTube->End()-m_pCurLineTube->Start();
			pCurrentJoint=&m_pCurLineTube->m_tJointStart;
			cutPlanePos=m_pCurLineTube->Start();
		}
		else
		{
			hOldLinkTube = m_pCurLineTube->m_tJointEnd.hLinkObj;
			hOldViceLinkTube = m_pCurLineTube->m_tJointEnd.hViceLinkObj;
			cur_tube_vec=m_pCurLineTube->Start()-m_pCurLineTube->End();
			pCurrentJoint=&m_pCurLineTube->m_tJointEnd;
			cutPlanePos=m_pCurLineTube->End();
		}
		normalize(cur_tube_vec);
		hLinkObj=m_hDatumTube1;
		hViceLinkObj=m_hDatumTube2;	
		CLDSLineTube *pOldTube=(CLDSLineTube*)console.FromPartHandle(hOldLinkTube,CLS_LINETUBE);
		if(pOldTube&&hOldViceLinkTube<0)
		{	//修改前是钢管对接,当前非对接相贯
			if(!m_bFaceToFace||(hLinkObj==0))	//删除钢管对接
			{	
				if(pOldTube->pStart==m_pBaseNode)
				{
					pOldTube->m_tJointStart.type=0;
					pOldTube->m_tJointStart.hLinkObj=0;
					pOldTube->m_tJointStart.hViceLinkObj=0;
					pOldTube->m_tJointStart.cutPlanePos.Set();
					pOldTube->m_tJointStart.cutPlaneNorm.Set();
					pOldTube->SetStartOdd(0);
				}
				else if(pOldTube->pEnd==m_pBaseNode)
				{
					pOldTube->m_tJointEnd.type=0;
					pOldTube->m_tJointEnd.hLinkObj=0;
					pOldTube->m_tJointEnd.hViceLinkObj=0;
					pOldTube->m_tJointEnd.cutPlanePos.Set();
					pOldTube->m_tJointEnd.cutPlaneNorm.Set();
					pOldTube->SetEndOdd(0);
				}
			}
		}
		//判断输入的对接钢管是否合理
		CLDSLineTube *pLineTube = (CLDSLineTube*)console.FromPartHandle(hLinkObj,CLS_LINETUBE);
		CLDSLineTube *pViceLineTube = (CLDSLineTube*)console.FromPartHandle(hViceLinkObj,CLS_LINETUBE);
		if((hLinkObj&&!pLineTube)||(hViceLinkObj&&!pViceLineTube))	
		{	//输入句柄不为0且输入的钢管不存在
			MessageBox("输入错误不存在该钢管,请重新输入!");
			return;
		}
		if(m_bFaceToFace&&pLineTube)//对接相贯且输入了对接钢管句柄
		{
			if(m_pBaseNode!=pLineTube->pStart&&m_pBaseNode!=pLineTube->pEnd)
			{
				MessageBox("两钢管无连接不能实现对接,请重新输入!");
				return;	
			}
			if(pLineTube->GetDiameter()!=m_pCurLineTube->GetDiameter()
				||pLineTube->GetThick()!=m_pCurLineTube->GetThick())
			{	//钢管规格不同
				MessageBox("两钢管规格不同不能实现对接,请重新输入!");
				return;
			}
			hViceLinkObj=-1;	//对接相贯
		}
		//设置钢管相贯相关参数
		if(pCurrentJoint)
		{
			if(hLinkObj<0)
			{
				pCurrentJoint->cutPlaneNorm.Set();
				pCurrentJoint->cutPlanePos.Set();
				pCurrentJoint->type=0;
			}
			pCurrentJoint->type=1;
			pCurrentJoint->hLinkObj=hLinkObj;
			pCurrentJoint->hViceLinkObj=hViceLinkObj;
			m_pCurLineTube->m_hPartWeldParent=hLinkObj;
			//设置钢管正负头 wht 11-05-10
			if(m_bStartEnd)
				m_pCurLineTube->SetStartOdd(setupInfo.m_fOddment);
			else 
				m_pCurLineTube->SetEndOdd(setupInfo.m_fOddment);
		}
		if(m_bFaceToFace&&pLineTube&&pCurrentJoint)
		{
			//将对接钢管的相应端设置为对接相贯
			if(pLineTube->pStart==m_pBaseNode)
			{
				pLineTube->m_tJointStart.type=1;
				pLineTube->m_tJointStart.hLinkObj=m_pCurLineTube->handle;
				pLineTube->m_tJointStart.hViceLinkObj=-1;
				link_tube_vec=pLineTube->End()-pLineTube->Start();
			}
			else if(pLineTube->pEnd==m_pBaseNode)
			{
				pLineTube->m_tJointEnd.type=1;
				pLineTube->m_tJointEnd.hLinkObj=m_pCurLineTube->handle;
				pLineTube->m_tJointEnd.hViceLinkObj=-1;
				link_tube_vec=pLineTube->Start()-pLineTube->End();
			}
			normalize(link_tube_vec);
			//设置对接相贯的切割平面
			f3dPoint mid_vec,vec,cutPlaneNorm;
			mid_vec=cur_tube_vec+link_tube_vec;
			normalize(mid_vec);
			vec=cur_tube_vec^link_tube_vec;
			normalize(vec);
			cutPlaneNorm=mid_vec^vec;
			normalize(cutPlaneNorm);
			//计算出一个最小的正头
			double angle=cal_angle_of_2vec(cur_tube_vec,cutPlaneNorm);
			double tana=fabs(tan(angle));
			int min_dist=int(pLineTube->GetDiameter()*0.5*tana+1);
			//设置当前钢管切割面
			if(cutPlaneNorm*cur_tube_vec>0)
				cutPlaneNorm*=-1.0;
			pCurrentJoint->cutPlaneNorm=cutPlaneNorm;
			pCurrentJoint->cutPlanePos=cutPlanePos;
			//设置正负头
			if(m_bStartEnd)
				m_pCurLineTube->SetStartOdd(min_dist);
			else
				m_pCurLineTube->SetEndOdd(min_dist);
			//设置对接钢管的切割面
			if(m_pBaseNode==pLineTube->pStart)
			{
				pLineTube->SetStartOdd(min_dist);	//设置正头
				pLineTube->m_tJointStart.cutPlaneNorm = -1.0*cutPlaneNorm;
				pLineTube->m_tJointStart.cutPlanePos = pLineTube->Start();
			}
			else if(m_pBaseNode==pLineTube->pEnd)
			{
				pLineTube->SetEndOdd(min_dist);	//设置正头
				pLineTube->m_tJointEnd.cutPlaneNorm = -1.0*cutPlaneNorm;
				pLineTube->m_tJointEnd.cutPlanePos = pLineTube->End();
			}
		}
	}
	else if(m_iPartType>0&&!m_bHasExistEndLj)
	{	//标准构件
		if(m_iTubeEndLjStyle==2)
		{
			CLDSNode *pNode=m_bStartEnd?m_pCurLineTube->pStart:m_pCurLineTube->pEnd;
			if(m_pRelatedTube&&m_bCoupleFLCreate)
			{	//存在合法对接钢管
				CLDSPart *pPart=NULL;
				if(m_pRelatedTube->pStart==pNode&&(m_pRelatedTube->m_tJointStart.hLinkObj>0X20||!m_pRelatedTube->m_tJointStart.hLinkObj))
					pPart = console.FromPartHandle(m_pRelatedTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
				else if(m_pRelatedTube->pEnd==pNode&&(m_pRelatedTube->m_tJointEnd.hLinkObj>0X20||!m_pRelatedTube->m_tJointEnd.hLinkObj))
					pPart = console.FromPartHandle(m_pRelatedTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
				if(pPart)
				{
					m_bCoupleFLCreate=false;
					AfxMessageBox(CXhChar100("0x%X钢管的对接钢管端头已有连接构件,不能重复设计对接法兰!",m_pCurLineTube->handle));
				}
			}
			CLDSStdPartModel stdPartModel,coupleStdPartModel;
			globalLibrary.GetStdPartModel((char*)m_paramPlate.GetPartNo(),m_paramPlate.m_iParamType,&stdPartModel);
			stdPartModel.m_fRelateTubeThick=m_pCurLineTube->GetThick();
			if(m_bCoupleFLCreate&&m_pRelatedTube!=NULL)
			{
				globalLibrary.GetStdPartModel(sCoupleFlPlateNo,m_paramPlate.m_iParamType,&coupleStdPartModel);
				coupleStdPartModel.m_fRelateTubeThick=m_pRelatedTube->GetThick();
				NewStandardPart(&stdPartModel,&coupleStdPartModel);	
			}
			else
				NewStandardPart(&stdPartModel);	
		}
		else if(m_iTubeEndLjStyle>2&&m_pInsertPlateParam)
		{	//插板
			CLDSStdPartModel stdPartModel;
			if(globalLibrary.GetStdPartModel(m_pInsertPlateParam->codeName,m_paramPlate.m_iParamType,&stdPartModel))
				NewStandardPart(&stdPartModel);
		}
	}
	else if(m_iTubeEndLjStyle>=2&&!m_bHasExistEndLj)
	{	//普通构件
		m_pParamPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);	
		m_pParamPlate->CopyProperty(&m_paramPlate);
		m_pParamPlate->workNorm=setupInfo.workNorm;
		m_pParamPlate->m_hPartWeldParent=m_pCurLineTube->handle;
		m_pParamPlate->SetLayer(m_pCurLineTube->layer()); //调整图层名
		m_pParamPlate->cfgword=m_pCurLineTube->cfgword;
		if(m_bStartEnd)
		{
			m_pCurLineTube->m_tJointStart.type=m_iTubeEndLjStyle;
			m_pCurLineTube->m_tJointStart.hLinkObj=m_pParamPlate->handle;
			m_pCurLineTube->SetStartOdd(setupInfo.m_fOddment,true);
			m_pCurLineTube->desStartPos.fNormOffset=setupInfo.m_fTubeNormOffset;	//钢管基准面法向偏移量
			m_pCurLineTube->DesignStartJoint();
		}
		else
		{
			m_pCurLineTube->m_tJointEnd.type=m_iTubeEndLjStyle;
			m_pCurLineTube->m_tJointEnd.hLinkObj=m_pParamPlate->handle;
			m_pCurLineTube->SetEndOdd(setupInfo.m_fOddment,true);
			m_pCurLineTube->desEndPos.fNormOffset=setupInfo.m_fTubeNormOffset;	//钢管基准面法向偏移量
			m_pCurLineTube->DesignEndJoint();
		}
		double fOddment=0;
		BOOL bCanFlCoupleDesign = FALSE,bFlCoupleDesign=FALSE;
		if(m_iTubeEndLjStyle==2)	//法兰连接
		{
			double H=0;
			m_pParamPlate->GetDesignItemValue('H',&H);
			CLDSNode *pNode;
			if(m_bStartEnd)
			{
				pNode=m_pCurLineTube->pStart;
				fOddment=m_pCurLineTube->startOdd();
			}
			else 
			{
				pNode=m_pCurLineTube->pEnd;
				fOddment=m_pCurLineTube->endOdd();
			}
			if(m_pRelatedTube&&m_pRelatedTube->m_bVirtualPart)//pRelatedTube->GetDiameter()>m_pCurLineTube->GetDiameter())
				m_pRelatedTube=NULL;	//仅支持基准钢管直接大于对接钢管直径的不等径法兰对接 wht 10-09-29
			if(m_pRelatedTube)
			{
				CLDSParamPlate *pTmpPlate=NULL;
				if(m_pRelatedTube->pStart==pNode&&m_pRelatedTube->m_tJointStart.hLinkObj>0X20)
					pTmpPlate = (CLDSParamPlate*)console.FromPartHandle(m_pRelatedTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
				else if(m_pRelatedTube->pEnd==pNode&&m_pRelatedTube->m_tJointEnd.hLinkObj>0X20)
					pTmpPlate = (CLDSParamPlate*)console.FromPartHandle(m_pRelatedTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
				if(pTmpPlate==NULL)
					bCanFlCoupleDesign=TRUE;
			}
			if(bCanFlCoupleDesign)
				bFlCoupleDesign=(AfxMessageBox("是否设计对接法兰Y/N?",MB_YESNO)==IDYES);
			//生成法兰上的螺栓
			CreateBolt(m_pParamPlate);
			//生成法兰上的肘板
			if(m_bCreateAttachPlate)
				CreateAttachPlate(m_pParamPlate,m_pCurLineTube);
			if(bFlCoupleDesign)	//有共线衔接钢管,可进行成对法兰设计
			{
				if(m_pRelatedTube->pStart==pNode)
					m_pRelatedTube->SetStartOdd(-2*H-fOddment);
				else
					m_pRelatedTube->SetEndOdd(-2*H-fOddment);
				CLDSParamPlate *pCoupleFlPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pCoupleFlPlate->CopyProperty(m_pParamPlate);
				pCoupleFlPlate->m_hPartWeldParent = m_pRelatedTube->handle;
				pCoupleFlPlate->SetLayer(m_pRelatedTube->layer()); //调整图层名
				pCoupleFlPlate->cfgword=m_pRelatedTube->cfgword;
				//不等径对接时调整法兰内环半径 wht 10-09-29
				double offset_r=0.5*(m_pCurLineTube->GetDiameter()-m_pRelatedTube->GetDiameter());
				if(offset_r>EPS2)
				{
					pCoupleFlPlate->SetPartNo(sCoupleFlPlateNo);	//对接法兰编号 
					DESIGN_PLATE_ITEM *pItem=pCoupleFlPlate->GetDesignItem('R');
					if(pItem)	//修改法兰内环半径
						pCoupleFlPlate->SetDesignItemValue(pItem->key,pItem->value.fValue-offset_r,pItem->sNotes,pItem->paramType);
				}
				if(m_pRelatedTube->pStart==pNode)
				{
					m_pRelatedTube->SetStartOdd(-2*H-fOddment);
					m_pRelatedTube->m_tJointStart.type=TYPE_FL;
					m_pRelatedTube->m_tJointStart.hLinkObj=pCoupleFlPlate->handle;
					m_pRelatedTube->DesignStartJoint();
				}
				else
				{
					m_pRelatedTube->SetEndOdd(-2*H-fOddment);
					m_pRelatedTube->m_tJointEnd.type=TYPE_FL;
					m_pRelatedTube->m_tJointEnd.hLinkObj=pCoupleFlPlate->handle;
					m_pRelatedTube->DesignEndJoint();
				}
				//修改螺栓通厚重新计算规格
				for(CLsRef *pLsRef=m_pParamPlate->GetFirstLsRef();pLsRef;pLsRef=m_pParamPlate->GetNextLsRef())
				{
					pCoupleFlPlate->AppendLsRef(*pLsRef);
					pLsRef->GetLsPtr()->AddL0Thick(pCoupleFlPlate->handle,TRUE);
					pLsRef->GetLsPtr()->CalGuigeAuto();
					(*pLsRef)->SetModified();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
				//生成成对的肘板
				if(m_bCreateAttachPlate)
					CreateAttachPlate(pCoupleFlPlate,m_pRelatedTube,bFlCoupleDesign);
				m_pRelatedTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				pCoupleFlPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pCoupleFlPlate->GetSolidPartObject());
				g_pSolidDraw->NewSolidPart(m_pRelatedTube->GetSolidPartObject());
			}
		}
		else
		{
			//生成U型板/卷边板上的螺栓
			CreateBolt(m_pParamPlate);
			m_pParamPlate->ResetSonBoltRayNo();		//布置插板上螺栓射线号
			m_pParamPlate->GetLsRefList()->ResetBoltRefRayNo(m_pParamPlate);	//布置插板上螺栓孔射线号
		}
		//生成十字插板上的附加板
		if(m_pParamPlate&&m_pParamPlate->m_iParamType==TYPE_XEND)
			LayoutXEndSubPlate(m_pParamPlate);
	}
	//已存在插板时在此处添加十字插板的加劲板
	if(m_bHasExistEndLj&&m_bCreateAttachPlate)
	{	
		CLDSPlate *pVertPlate[2]={NULL,NULL};
		ATTACHPART_ITEM *pItem=NULL;
		double M=0;
		m_pParamPlate->GetDesignItemValue('M',&M);
		for(pItem=m_pParamPlate->GetFirstAttachPart();pItem;pItem=m_pParamPlate->GetNextAttachPart())
		{
			CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pItem->h,CLS_PLATE);
			if(pPlate==NULL)
				continue;
			f3dPoint plate_pos=pPlate->ucs.origin;
			//转换到十字插板坐标系下
			coord_trans(plate_pos,m_pParamPlate->ucs,FALSE);
			if(plate_pos.x>M)
				continue;
			if(plate_pos.x<EPS2&&plate_pos.z>0)
				pVertPlate[0]=pPlate;	//上侧立板
			else if(plate_pos.x<EPS2&&plate_pos.z<0)
				pVertPlate[1]=pPlate;	//下侧立板
		}
		for(int i=0;i<2;i++)
		{
			if(pVertPlate[i]==NULL)
				continue;
			if(i==0)	//十字插板Z正方向肋板
				NewRibPlate(m_pParamPlate,&AttachPlatePara,pVertPlate[i],TRUE,AttachPlatePara.m_pWeldParentPlate);
			else if(AttachPlatePara.m_bMirCreate)//对称生成十字插板Z负方向肋板
				NewRibPlate(m_pParamPlate,&AttachPlatePara,pVertPlate[i],FALSE,AttachPlatePara.m_pWeldParentPlate);
			if(m_bStartEnd)
				m_pCurLineTube->m_tJointStart.bVertPlate=TRUE;
			else
				m_pCurLineTube->m_tJointEnd.bVertPlate=TRUE;
		}
	}
	//在此处设计十字插板填板 wht 11-05-11
	if(m_pParamPlate&&m_pParamPlate->m_iParamType==TYPE_XEND&&m_bCreateFillPlate)
		DesignXEndFillPlate(m_pParamPlate);
	//在此处设计槽型插板的加劲板
	if(m_pParamPlate&&m_pParamPlate->m_iParamType==TYPE_ROLLEND&&m_bCreateAttachPlate)
		DesignREndAttachPlate(m_pParamPlate);
	//计算钢管位置
	m_pCurLineTube->ClearFlag();	//清空标志
	m_pCurLineTube->CalPosition();
	m_pCurLineTube->SetModified();
	m_pCurLineTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(m_pCurLineTube->GetSolidPartObject());
	if(m_pParamPlate)
	{	//重新设计插板坐标系
		if(m_bStartEnd)
			m_pCurLineTube->DesignStartJointUCS(m_pParamPlate);
		else 
			m_pCurLineTube->DesignEndJointUCS(m_pParamPlate);
		m_pParamPlate->DesignPlate();
		m_pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(m_pParamPlate->GetSolidPartObject());
	}
	g_pSolidDraw->ReleaseSnapStatus();
	m_pSrcParamPlate=NULL;	//重新初始化克隆钢板时的源钢板 wht 10-07-06
	CDialog::OnOK();	
}

BOOL CDesignTubeEndLjDlg::NewStandardPart(CLDSStdPartModel *pStdPartModel,CLDSStdPartModel *pCoupleStdPartModel/*=NULL*/)
{
	UpdateData();
	if(pStdPartModel==NULL)
		return FALSE;
	Ta.BeginUndoListen();
	m_pParamPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	//拷贝属性时会冲掉正确的配材号,此处先将配材号赋值给临时参数化板 wht 10-08-02
	m_paramPlate.cfgword=m_pParamPlate->cfgword;	
	m_pParamPlate->CopyProperty(&m_paramPlate);
	m_pParamPlate->m_bStdPart=TRUE;	//标准构件
	m_pParamPlate->m_hPartWeldParent=m_pCurLineTube->handle;
	m_pParamPlate->m_fNormOffset=setupInfo.m_fNormOffset;//工作面法向偏心距
	m_pParamPlate->SetLayer(m_pCurLineTube->layer()); //调整图层名
	m_pParamPlate->cfgword=m_pCurLineTube->cfgword;
	m_pParamPlate->m_iParamType=pStdPartModel->m_iStdPartType;
	m_pParamPlate->workNorm=setupInfo.workNorm;
	if(m_iTubeEndLjStyle==2)	//标准构件法兰
	{
		//设置工作面相对于钢管基准面的旋转角度 wht 10-01-21
		//用字母'X'表示旋转角度，D与标准法兰参数重名 
		//法兰未公开setupInfo.workNorm.rot_ang参数,将首螺栓偏移角度保存至'X'参数中 wht 11-05-26
		m_pParamPlate->SetDesignItemValue('X',LayoutBoltPara.m_fOffsetAngle,"基准X轴相对钢管X轴旋转角度");	//setupInfo.workNorm.rot_ang
		if(LayoutBoltPara.m_iStartEdge==1)	//以焊道线为基准 m_iStartEdge与J值相反容易出错，以后需调整一致 wht 11-05-26
			m_pParamPlate->SetDesignItemValue('J',1,"环向布置螺栓时:1表示以焊道线为基准，0表示以基准边为基准。");
		else								//以基准边为基准
			m_pParamPlate->SetDesignItemValue('J',0,"环向布置螺栓时:1表示以焊道线为基准，0表示以基准边为基准。");
		if(m_pParamPlate->m_iParamType==TYPE_FLP)	
			m_pParamPlate->SetDesignItemValue('O',setupInfo.H1,"钢管端与法兰底面的距离");
		if(m_pParamPlate->m_iParamType==TYPE_FLD||m_pParamPlate->m_iParamType==TYPE_FLP)
			m_pParamPlate->cMaterial=pStdPartModel->param.fl_param.cMaterial;	//法兰材质
		else if(m_pParamPlate->m_iParamType==TYPE_FLG||m_pParamPlate->m_iParamType==TYPE_FLR)
			m_pParamPlate->cMaterial=pStdPartModel->param.fl_param2.cMaterial;	//法兰材质
		if(m_bStartEnd)
			m_pCurLineTube->m_tJointStart.hLinkObj=m_pParamPlate->handle;
		else 
			m_pCurLineTube->m_tJointEnd.hLinkObj=m_pParamPlate->handle;
		m_pParamPlate->DesignStdPartUcs();
		m_pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		pStdPartModel->CreateBoltInPlate(m_pParamPlate);//布置法兰上的螺栓
		//生成对接法兰
		CLDSNode *pNode=NULL;
		if(m_bStartEnd)
			pNode=m_pCurLineTube->pStart;
		else
			pNode=m_pCurLineTube->pEnd;
		if(pCoupleStdPartModel&&pStdPartModel)
		{	//设计对接法兰
			double oddment=-setupInfo.m_fOddment;
			CLDSParamPlate *pCoupleFlPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pCoupleFlPlate->CopyProperty(m_pParamPlate);
			pCoupleFlPlate->m_hPartWeldParent = m_pRelatedTube->handle;
			pCoupleFlPlate->SetLayer(m_pRelatedTube->layer()); //调整图层名
			pCoupleFlPlate->cfgword=m_pRelatedTube->cfgword; //调整法兰的图层名
			if(m_pParamPlate->m_iParamType==TYPE_FLD||m_pParamPlate->m_iParamType==TYPE_FLP)
			{
				pCoupleFlPlate->cMaterial=pCoupleStdPartModel->param.fl_param.cMaterial;	//标准法兰材质
				pCoupleFlPlate->SetPartNo(pCoupleStdPartModel->param.fl_param.codeName,true);
			}
			else //if(m_pParamPlate->m_iParamType==TYPE_FLG||m_pParamPlate->m_iParamType==TYPE_FLR)
			{
				pCoupleFlPlate->cMaterial=pCoupleStdPartModel->param.fl_param2.cMaterial;	//标准法兰材质
				pCoupleFlPlate->SetPartNo(pCoupleStdPartModel->param.fl_param2.codeName,true);
			}
			if(m_pRelatedTube->pStart==pNode)
			{
				m_pRelatedTube->m_tJointStart.type=m_iTubeEndLjStyle;
				m_pRelatedTube->m_tJointStart.hLinkObj=pCoupleFlPlate->handle;
				//必须置于最后,否则会因m_tJointStart.type未赋值,导致正负头无法按到法兰端面赋值 wjh-2015.3.20
				m_pRelatedTube->SetStartOdd(oddment,true);
			}
			else
			{
				m_pRelatedTube->m_tJointEnd.type=m_iTubeEndLjStyle;
				m_pRelatedTube->m_tJointEnd.hLinkObj=pCoupleFlPlate->handle;
				m_pRelatedTube->SetEndOdd(oddment,true);
			}
			//修改螺栓通厚重新计算规格
			for(CLsRef *pLsRef=m_pParamPlate->GetFirstLsRef();pLsRef;pLsRef=m_pParamPlate->GetNextLsRef())
			{
				pCoupleFlPlate->AppendLsRef(*pLsRef);
				pLsRef->GetLsPtr()->AddL0Thick(pCoupleFlPlate->handle,TRUE);
				pLsRef->GetLsPtr()->CalGuigeAuto();
				(*pLsRef)->SetModified();
				(*pLsRef)->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
			}
			//
			pCoupleFlPlate->DesignStdPartUcs();
			m_pRelatedTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			pCoupleFlPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pCoupleFlPlate->GetSolidPartObject());
			g_pSolidDraw->NewSolidPart(m_pRelatedTube->GetSolidPartObject());	
			//生成对接钢管上的刚性法兰或者柔性法兰上的肘板
			if(m_bCreateAttachPlate&&m_iPartType>=3)
				CreateAttachPlate(pCoupleFlPlate,m_pRelatedTube);
		}
	}
	else
	{	
		//设置工作面相对于钢管基准面的旋转角度 wht 10-01-21
		//用字母'X'表示旋转角度，D与标准法兰参数重名 
		m_pParamPlate->SetDesignItemValue('X',setupInfo.workNorm.rot_ang,"工作面相对钢管基准面旋转角度");
		m_pParamPlate->cMaterial=pStdPartModel->param.insert_plate_param.cMaterial;	//插板材质
		if(m_pParamPlate->m_iParamType==TYPE_XEND)	//十字插板基板编号与十字插板代号不相同 wht 10-01-21
			m_pParamPlate->SetPartNo(pStdPartModel->param.insert_plate_param.datumPlatePartNo,true);
	}
	if(m_bStartEnd)
	{
		m_pCurLineTube->m_tJointStart.type=m_iTubeEndLjStyle;
		m_pCurLineTube->m_tJointStart.hLinkObj=m_pParamPlate->handle;
		m_pCurLineTube->desStartPos.fNormOffset = setupInfo.m_fTubeNormOffset;	//钢管基准面法向偏移量
		m_pCurLineTube->SetStartOdd(setupInfo.m_fOddment,true);
	}
	else
	{
		m_pCurLineTube->m_tJointEnd.type=m_iTubeEndLjStyle;
		m_pCurLineTube->m_tJointEnd.hLinkObj=m_pParamPlate->handle;
		m_pCurLineTube->desEndPos.fNormOffset = setupInfo.m_fTubeNormOffset;	//钢管基准面法向偏移量
		m_pCurLineTube->SetEndOdd(setupInfo.m_fOddment,true);
	}
	m_pCurLineTube->ClearFlag();	//清空标志
	m_pCurLineTube->CalPosition();	//计算钢管位置
	m_pParamPlate->DesignStdPartUcs();
	m_pParamPlate->DesignPlate();
	m_pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(m_pParamPlate->GetSolidPartObject());
	if(m_iTubeEndLjStyle>2)		//法兰螺栓已单独布置过	wht 09-11-25
		pStdPartModel->CreateBoltInPlate(m_pParamPlate);//布置螺栓U/C/X型插板上的螺栓
	if(m_pParamPlate->m_iParamType==TYPE_XEND)
		LayoutXEndSubPlate(m_pParamPlate);
	//生成刚性法兰或者柔性法兰上的肘板(钢管正负头设置之后，再生成附加板，否则位置不对) 
	if(m_pParamPlate->IsFL()&&m_bCreateAttachPlate&&m_iPartType>=3)
		CreateAttachPlate(m_pParamPlate,m_pCurLineTube);
	//绘制标准构件上的螺栓
	for(CLsRef *pLsRef=m_pParamPlate->GetFirstLsRef();pLsRef;pLsRef=m_pParamPlate->GetNextLsRef())
	{	
		(*pLsRef)->correct_worknorm();
		(*pLsRef)->correct_pos();
		(*pLsRef)->CalGuigeAuto();
		(*pLsRef)->SetModified();
		(*pLsRef)->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	m_pParamPlate->ResetSonBoltRayNo();		//布置插板上螺栓射线号
	m_pParamPlate->GetLsRefList()->ResetBoltRefRayNo(m_pParamPlate);	//布置插板上螺栓孔射线号
	UpdateData(FALSE);
	Ta.EndUndoListen();
	return TRUE;
}

//根据标准构件类型 级别重新获得标准构件参数
void CDesignTubeEndLjDlg::LoadStdPlateParaByLevel()
{
	if(m_iTubeEndLjStyle==2 && m_iPartType==2)
	{	//对焊法兰
		double tube_d_m=0,tube_d_c=0;
		//主管法兰
		tube_d_m=m_pCurLineTube->GetDiameter();
		tube_d_c=m_pRelatedTube?m_pRelatedTube->GetDiameter():tube_d_m;
		if(tube_d_m>tube_d_c)
			tube_d_c=tube_d_m;
		m_pFlDParam=FindFlDParam(tube_d_m,tube_d_c,m_cStdPartMat,m_cFlLevel1);
		//对接法兰
		if(m_pRelatedTube)
		{
			tube_d_m=m_pRelatedTube->GetDiameter();
			tube_d_c=m_pCurLineTube->GetDiameter();
			if(tube_d_m>tube_d_c)
				tube_d_c=tube_d_m;
			m_pFlDParam2=FindFlDParam(tube_d_m,tube_d_c,m_cStdPartMat,m_cFlLevel2);
		}
	}
	else	//标准插板
		m_pInsertPlateParam=QueryStdInsertPlate(m_iStdPartLevel,m_cStdPartMat);
}

//布置十字插板上的附加板
void CDesignTubeEndLjDlg::LayoutXEndSubPlate(CLDSParamPlate *pParamPlate,BOOL bIsCloneBolt/*=FALSE*/)
{
	if(pParamPlate==NULL)
		return;
	if(pParamPlate->m_iParamType!=TYPE_XEND)
		return;	//非十字插板
	CString sPartNo;
	double A=0,M=0,R=0,x=0,y=0,z=0,N=0,S=0,W=0,L=0,H=0;
	if(pParamPlate->m_bStdPart)
	{	//标准构件
		CLDSStdPartModel stdPartModel;
		if(!pParamPlate->GetStdPartModel(&stdPartModel))
			return;
		A=stdPartModel.param.insert_plate_param.A;
		R=stdPartModel.param.insert_plate_param.R;
		N=stdPartModel.param.insert_plate_param.N;
		S=stdPartModel.param.insert_plate_param.S;
		W=stdPartModel.param.insert_plate_param.W;
		H=stdPartModel.param.insert_plate_param.H;
		L=stdPartModel.param.insert_plate_param.L;
		M=stdPartModel.param.insert_plate_param.M;
	}
	else
	{
		pParamPlate->GetDesignItemValue('N',&N);
		pParamPlate->GetDesignItemValue('S',&S);
		pParamPlate->GetDesignItemValue('W',&W);
		pParamPlate->GetDesignItemValue('M',&M);
		pParamPlate->GetDesignItemValue('R',&R);
		pParamPlate->GetDesignItemValue('A',&A);
		pParamPlate->GetDesignItemValue('L',&L);
		pParamPlate->GetDesignItemValue('H',&H);
	}
	x=A+M;
	y=R;
	z=0.5*pParamPlate->GetThick();
	
	//十字插板需要布置立板 标准十字插板的立板也单独生成 wht 10-01-24
	CLDSPlate *pUpVertPlate=NULL,*pDownVertPlate=NULL; //Z正方向立板  Z负方向立板
	//十字插板立板法线设计基准构件为基板			//wht 10-03-06
	//X-Y平面上的包板及螺栓法线设计基准构件为基板
	//X-Z平面上的包板及螺栓法线设计基准构件为对应的立板,立板法线修改后包板及螺栓法线能及时更新
	int i;
	for(i=0;i<2;i++)
	{
		CLDSPlate *pVertPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		pVertPlate->iSeg=pParamPlate->iSeg;
		pVertPlate->cfgword=pParamPlate->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pVertPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
		pVertPlate->cMaterial = pParamPlate->cMaterial;
		pVertPlate->SetLayer(pParamPlate->layer()); //调整图层名
		pVertPlate->cfgword=pParamPlate->cfgword; //调整法兰的图层名
		pVertPlate->m_hPartWeldParent=pParamPlate->handle;//根据父构件的参数设计包板
		pVertPlate->Thick=ftoi(pParamPlate->GetThick());
		pVertPlate->SetPartNo(sWeldPlatePartNo);
		pVertPlate->jdb_style=5;	//基准构件上相对位置不变的附加板
		//.................................................
		//此处有问题尽量不去使用jdb_style 稍后修改 
		//.................................................
		pVertPlate->designInfo.m_hBasePart=pParamPlate->handle;
		pVertPlate->designInfo.origin.datum_pos_style=9;//基准构件上的相对坐标
		pVertPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
		pVertPlate->m_fNormOffset=-z;
		if(i==0)
		{	//Z正方向的立板
			pUpVertPlate=pVertPlate;	//记录Z正方向立板
			pVertPlate->designInfo.norm=setupInfo.workNormUpVert;
			if(setupInfo.workNormUpVert.norm_style==4&&setupInfo.workNormUpVert.hVicePart<=0X20)
				pVertPlate->designInfo.norm.hVicePart=pParamPlate->handle;
			//pVertPlate->designInfo.norm.nearVector.Set(0,-1,0);
			pVertPlate->designInfo.origin.datum_pos.Set(0,0,z);
			//引用相应的螺栓到立板上
			for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			{
				if(pLsRef->start1_end2_mid0==31)//引用螺栓到Z正方向立板上
				{	
					pVertPlate->AppendLsRef(*pLsRef);
					(*pLsRef)->AddL0Thick(pVertPlate->handle,TRUE);
				}
			}
		}
		else if(i==1)
		{	//Z负方向的立板
			pDownVertPlate=pVertPlate;	//记录Z负方向立板
			pVertPlate->designInfo.origin.datum_pos.Set(0,0,-z);
			pVertPlate->designInfo.norm=setupInfo.workNormDownVert;
			if(setupInfo.workNormDownVert.norm_style==4&&setupInfo.workNormDownVert.hVicePart<=0X20)
				pVertPlate->designInfo.norm.hVicePart=pParamPlate->handle;
			//pVertPlate->designInfo.norm.nearVector.Set(0,1,0);
			//引用相应的螺栓到立板上
			for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			{
				if(pLsRef->start1_end2_mid0==41)//引用螺栓到Z负方向立板上
				{
					pVertPlate->AppendLsRef(*pLsRef);
					(*pLsRef)->AddL0Thick(pVertPlate->handle,TRUE);
				}
			}
		}
		pVertPlate->DesignSetupUcs();
		pVertPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
		pVertPlate->vertex_list.append(PROFILE_VERTEX(L,0,0));
		pVertPlate->vertex_list.append(PROFILE_VERTEX(L,0.5*H-z,0));
		pVertPlate->vertex_list.append(PROFILE_VERTEX(0,0.5*H-z,0));
		pVertPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pVertPlate->GetSolidPartObject());
		//十字插板加劲板
		if(m_bCreateAttachPlate)
		{	
			if(i==0)	//十字插板Z正方向肋板
				NewRibPlate(pParamPlate,&AttachPlatePara,pVertPlate,TRUE,AttachPlatePara.m_pWeldParentPlate);
			else if(AttachPlatePara.m_bMirCreate)//对称生成十字插板Z负方向肋板
				NewRibPlate(pParamPlate,&AttachPlatePara,pVertPlate,FALSE,AttachPlatePara.m_pWeldParentPlate);
			if(m_bStartEnd)
				m_pCurLineTube->m_tJointStart.bVertPlate=TRUE;
			else
				m_pCurLineTube->m_tJointEnd.bVertPlate=TRUE;
		}
		//将立板添加到附加构件列表中
		pParamPlate->AppendAttachPart(pVertPlate);
	}
	if(m_iWrapPlateType==1)
	{	//用短角钢代替包板
		double fAngleY=0.5*pParamPlate->GetThick();	//短角钢定位Y坐标
		for(i=0;i<4;i++)
		{	//生成四根短角钢代替八块包板
			CLDSLineAngle *pShortJg=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
			pShortJg->iSeg=pParamPlate->iSeg;
			pShortJg->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
			pShortJg->cMaterial = pParamPlate->cMaterial;
			pShortJg->SetLayer(pParamPlate->layer()); //调整图层名
			pShortJg->cfgword=pParamPlate->cfgword; //调整包板的图层名
			////包板为螺栓连接，不应该有焊接父构件	wht 10-08-02
			//pShortJg->m_hPartWeldParent=pParamPlate->handle;//根据父构件的参数设计包板
			pShortJg->set_wing_thick(W);	//肢厚
			pShortJg->set_wing_wide(N+R-0.5*pParamPlate->GetThick());//肢宽
			pShortJg->SetPartNo(sWrapPlatePartNo);
			//位置参数
			pShortJg->desStartPos.SetToDatumPointStyle();
			//pShortJg->desStartPos.datum_pos_style=15;	//直接指定基点
			pShortJg->desStartPos.datumPoint.datum_pos_style=9;	//指定构件上的相对坐标
			pShortJg->desStartPos.datumPoint.des_para.hPart=pParamPlate->handle;
			pShortJg->desEndPos.SetToDatumPointStyle();
			//pShortJg->desEndPos.datum_pos_style=15;	//直接指定基点
			pShortJg->desEndPos.datumPoint.datum_pos_style=9;	//指定构件上的相对坐标
			pShortJg->desEndPos.datumPoint.des_para.hPart=pParamPlate->handle;
			//法线参数
			pShortJg->des_norm_x.spec_norm.norm_style=4;//指定构件上的相对方向
			pShortJg->des_norm_x.bByOtherWing=FALSE;
			pShortJg->des_norm_x.bSpecific=TRUE;
			pShortJg->des_norm_y.spec_norm.norm_style=4;//指定构件上的相对方向
			pShortJg->des_norm_y.bByOtherWing=FALSE;
			pShortJg->des_norm_y.bSpecific=TRUE;
			if(i==0)
			{	//X-Y平面: Y负Z上
				pShortJg->desStartPos.datumPoint.datum_pos.Set(x,-fAngleY,z);
				pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,-fAngleY,z);
				//X肢法线方向随Z正方向侧立板 Y肢法线方向随基板
				pShortJg->des_norm_x.spec_norm.hVicePart=pUpVertPlate->handle;
				pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
				pShortJg->des_norm_y.spec_norm.hVicePart=pParamPlate->handle;
				pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,-1);
				//引入螺栓
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Y平面:Y负方向一侧的螺栓 X-Z平面:Z正方向一侧的螺栓
					if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22
						||pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
					{
						pShortJg->AppendMidLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
					}
				}
			}
			else if(i==1)
			{	//X-Y平面: Y正Z上
				pShortJg->desStartPos.datumPoint.datum_pos.Set(x,fAngleY,z);
				pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,fAngleY,z);
				//Y肢法线方向随Z负方向侧立板 X肢法线方向随基板
				pShortJg->des_norm_y.spec_norm.hVicePart=pUpVertPlate->handle;
				pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
				pShortJg->des_norm_x.spec_norm.hVicePart=pParamPlate->handle;
				pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
				//引入螺栓
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Y平面:Y正方向一侧的螺栓 X-Z平面:Z正方向一侧的螺栓
					if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12
						||pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
					{
						pShortJg->AppendMidLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
					}
				}
			}
			else if(i==2)
			{	//X-Y平面: Y负Z下
				pShortJg->desStartPos.datumPoint.datum_pos.Set(x,-fAngleY,-z);
				pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,-fAngleY,-z);
				//X肢法线方向随Z负方向侧立板 Y肢法线方向随基板
				pShortJg->des_norm_y.spec_norm.hVicePart=pDownVertPlate->handle;
				pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
				pShortJg->des_norm_x.spec_norm.hVicePart=pParamPlate->handle;
				pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,1);
				//引入螺栓
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Y平面:Y负方向一侧的螺栓 X-Z平面:Z负方向一侧的螺栓
					if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22
						||pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
					{
						pShortJg->AppendMidLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
					}
				}
			} 
			else if(i==3)
			{	//X-Y平面: Y正Z下
				pShortJg->desStartPos.datumPoint.datum_pos.Set(x,fAngleY,-z);
				pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,fAngleY,-z);
				//Y肢法线方向随Z正方向侧立板 X肢法线方向随基板
				pShortJg->des_norm_x.spec_norm.hVicePart=pDownVertPlate->handle;
				pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
				pShortJg->des_norm_y.spec_norm.hVicePart=pParamPlate->handle;
				pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
				//引入螺栓
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Y平面:Y正方向一侧的螺栓 X-Z平面:Z负方向一侧的螺栓
					if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12
						||pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
					{
						pShortJg->AppendMidLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
					}
				}
			}
			pShortJg->ClearFlag();
			pShortJg->CalPosition();
			pShortJg->cal_x_wing_norm();
			pShortJg->cal_y_wing_norm();
			pShortJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pShortJg->GetSolidPartObject());
			//将包板添加到附加构件列表中
			pParamPlate->AppendAttachPart(pShortJg);
		}
	}
	else 
	{	//包板为钢板
		for(i=0;i<8;i++)
		{
			CLDSPlate *pPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
			pPlate->iSeg=pParamPlate->iSeg;
			pPlate->SetLayer(pParamPlate->layer()); //调整图层名
			pPlate->cfgword=pParamPlate->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
			pPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
			pPlate->cMaterial = pParamPlate->cMaterial;
			//包板为螺栓连接，不应该有焊接父构件	wht 10-08-02
			//pPlate->m_hPartWeldParent=pParamPlate->handle;//根据父构件的参数设计包板
			pPlate->Thick=ftoi(W);
			pPlate->SetPartNo(sWrapPlatePartNo);
			pPlate->jdb_style=5;	//基准构件上相对位置不变的附加板
			pPlate->designInfo.m_hBasePart=pParamPlate->handle;
			pPlate->designInfo.norm.norm_style=4;	//基准构件上的相对方向
			pPlate->designInfo.origin.datum_pos_style=9;//基准构件上的相对坐标
			if(i==0)
			{	//X-Y平面: Y负Z上
				pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
				pPlate->designInfo.norm.nearVector.Set(0,0,-1);
				pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
				pPlate->designInfo.origin.datum_pos.Set(x,-y,z);
				pPlate->m_fNormOffset=-W;
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Y平面:Y负方向一侧的螺栓 
					if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==1)
			{	//X-Y平面: Y负Z下
				pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
				pPlate->designInfo.norm.nearVector.Set(0,0,-1);
				pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
				pPlate->designInfo.origin.datum_pos.Set(x,-y,-z);
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Y平面:Y负方向一侧的螺栓
					if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==2)
			{	//X-Y平面: Y正Z上
				pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
				pPlate->designInfo.norm.nearVector.Set(0,0,1);
				pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
				pPlate->designInfo.origin.datum_pos.Set(x,y,z);
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Y平面:Y正方向一侧的螺栓
					if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==3)
			{	//X-Y平面: Y正Z下
				pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
				pPlate->designInfo.norm.nearVector.Set(0,0,1);
				pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
				pPlate->designInfo.origin.datum_pos.Set(x,y,-z);
				pPlate->m_fNormOffset=-W;
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Y平面:Y正方向一侧的螺栓
					if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==4)
			{	//X-Z平面: Y下Z正
				pPlate->designInfo.norm.hVicePart=pUpVertPlate->handle;			//上侧立板
				pPlate->designInfo.norm.nearVector.Set(0,0,1);
				pPlate->designInfo.origin.des_para.hPart=pUpVertPlate->handle;	//上侧立板
				pPlate->designInfo.origin.datum_pos.Set(x,y-z,z);				//y=R z=半个板厚
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Z平面:Z正方向一侧的螺栓
					if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==5)
			{	//X-Z平面: Y上Z正
				pPlate->designInfo.norm.hVicePart=pUpVertPlate->handle;			//上侧立板
				pPlate->designInfo.norm.nearVector.Set(0,0,1);
				pPlate->designInfo.origin.des_para.hPart=pUpVertPlate->handle;	//上侧立板
				pPlate->designInfo.origin.datum_pos.Set(x,y-z,-z);				//y=R z=半个板厚
				pPlate->m_fNormOffset=-W;
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Z平面:Z正方向一侧的螺栓
					if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==6)
			{	//X-Z平面: Y下Z负
				pPlate->designInfo.norm.hVicePart=pDownVertPlate->handle;		//下侧立板
				pPlate->designInfo.norm.nearVector.Set(0,0,1);
				pPlate->designInfo.origin.des_para.hPart=pDownVertPlate->handle;//下侧立板
				pPlate->designInfo.origin.datum_pos.Set(x,y-z,-z);				//y=R z=半个板厚
				pPlate->m_fNormOffset=-W;
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Z平面:Z负方向一侧的螺栓
					if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==7)
			{	//X-Z平面: Y上Z负
				pPlate->designInfo.norm.hVicePart=pDownVertPlate->handle;		//下侧立板
				pPlate->designInfo.norm.nearVector.Set(0,0,1);
				pPlate->designInfo.origin.des_para.hPart=pDownVertPlate->handle;//下侧立板
				pPlate->designInfo.origin.datum_pos.Set(x,y-z,z);				//y=R z=半个板厚
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Z平面:Z负方向一侧的螺栓
					if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			pPlate->DesignSetupUcs();
			pPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
			pPlate->vertex_list.append(PROFILE_VERTEX(S,0,0));
			pPlate->vertex_list.append(PROFILE_VERTEX(S,N,0));
			pPlate->vertex_list.append(PROFILE_VERTEX(0,N,0));
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			//将包板添加到附加构件列表中
			pParamPlate->AppendAttachPart(pPlate);
		}
	}
	if(bIsCloneBolt)
		z=0;	//通过以后插板克隆而来的螺栓不需要调整螺栓偏移值 wht 11-05-27
	for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{	
		//将螺栓引入立板/包板后调整螺栓法向偏移量
		if(m_pSrcParamPlate==NULL)	//通过零件库生成插板时不用调整螺栓法向偏移量 wht 10-07-06
			(*pLsRef)->des_base_pos.norm_offset.AddThick(-(long)W);
		//布置螺栓时螺栓所有螺栓均以基板为定位基准构件进行布置，
		//生成立板后,立板上的螺栓均以立板为定位基准构件故猜出需做以下调整
		//调整十字插板螺栓法线设计参数以及位置设计参数	wht 10-03-06
		if(pUpVertPlate&&pDownVertPlate)
		{	//X-Z平面:Z正方向一侧的螺栓
			if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
			{
				(*pLsRef)->des_work_norm.norm_style=4;	//基准构件上的相对方向
				(*pLsRef)->des_work_norm.nearVector.Set(0,0,1);
				(*pLsRef)->des_work_norm.hVicePart=pUpVertPlate->handle;
				//调整位置参数
				(*pLsRef)->des_base_pos.hPart=pUpVertPlate->handle;
				double wing_offset_dist=fabs((*pLsRef)->des_base_pos.wing_offset_dist);
				double len_offset_dist=fabs((*pLsRef)->des_base_pos.len_offset_dist);
				(*pLsRef)->des_base_pos.wing_offset_dist=wing_offset_dist-z; // z=半个板厚
				(*pLsRef)->des_base_pos.len_offset_dist=len_offset_dist;
			}
			//X-Z平面:Z负方向一侧的螺栓
			else if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
			{
				(*pLsRef)->des_work_norm.norm_style=4;	//基准构件上的相对方向
				(*pLsRef)->des_work_norm.nearVector.Set(0,0,1);
				(*pLsRef)->des_work_norm.hVicePart=pDownVertPlate->handle;
				//调整位置参数
				(*pLsRef)->des_base_pos.hPart=pDownVertPlate->handle;
				double wing_offset_dist=fabs((*pLsRef)->des_base_pos.wing_offset_dist);
				double len_offset_dist=fabs((*pLsRef)->des_base_pos.len_offset_dist);
				(*pLsRef)->des_base_pos.wing_offset_dist=wing_offset_dist-z; // z=半个板厚
				(*pLsRef)->des_base_pos.len_offset_dist=len_offset_dist;
			}
		}
		(*pLsRef)->SetModified();
		(*pLsRef)->correct_worknorm();
		(*pLsRef)->correct_pos();
		(*pLsRef)->CalGuigeAuto();
		(*pLsRef)->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
	}
}
void CDesignTubeEndLjDlg::DesignREndAttachPlate(CLDSParamPlate *pParamPlate)
{
	for(int i=0;i<2;i++)
	{
		if(i==1 && !AttachPlatePara.m_bMirCreate)
			break;
		double fOffZ=0.5*pParamPlate->GetThick();
		CLDSPlate *pVertPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		pVertPlate->iSeg=pParamPlate->iSeg;
		pVertPlate->cfgword=pParamPlate->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pVertPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
		pVertPlate->cMaterial = pParamPlate->cMaterial;
		pVertPlate->SetLayer(pParamPlate->layer()); //调整图层名
		pVertPlate->m_hPartWeldParent=pParamPlate->handle;//根据父构件的参数设计包板
		pVertPlate->Thick=ftoi(AttachPlatePara.m_nThick);
		pVertPlate->SetPartNo(AttachPlatePara.sPartNo);
		pVertPlate->m_fNormOffset=-fOffZ;
		pVertPlate->jdb_style=5;
		pVertPlate->designInfo.m_hBasePart=pParamPlate->handle;
		pVertPlate->designInfo.origin.datum_pos_style=9;//基准构件上的相对坐标
		pVertPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
		pVertPlate->designInfo.norm.norm_style=4;
		pVertPlate->designInfo.norm.hVicePart=pParamPlate->handle;
		if(i==0)
		{
			pVertPlate->designInfo.origin.datum_pos.Set(0,0,-fOffZ);
			pVertPlate->designInfo.norm.nearVector.Set(0,1,0);
		}
		else
		{
			pVertPlate->designInfo.origin.datum_pos.Set(0,0,fOffZ);
			pVertPlate->designInfo.norm.nearVector.Set(0,-1,0);
		}
		pVertPlate->DesignSetupUcs();
		//初始化轮廓点
		double L=AttachPlatePara.A;
		double H=AttachPlatePara.B;
		pVertPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
		pVertPlate->vertex_list.append(PROFILE_VERTEX(L,0,0));
		if(AttachPlatePara.F>0 && AttachPlatePara.F<H)
			pVertPlate->vertex_list.append(PROFILE_VERTEX(L,H-AttachPlatePara.F,0));
		pVertPlate->vertex_list.append(PROFILE_VERTEX(L-AttachPlatePara.E,H,0));
		pVertPlate->vertex_list.append(PROFILE_VERTEX(0+AttachPlatePara.C,H,0));
		if(AttachPlatePara.D>0 && AttachPlatePara.D<H)
			pVertPlate->vertex_list.append(PROFILE_VERTEX(0,H-AttachPlatePara.D,0));
		pVertPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pVertPlate->GetSolidPartObject());
		//将立板添加到附加构件列表中
		pParamPlate->AppendAttachPart(pVertPlate);
	}
}
//布置螺栓
void CDesignTubeEndLjDlg::LayoutBolt(ATOM_LIST<f3dPoint>& bolt_pt_arr)
{
	f3dPoint bolt_pos;
	if(m_iTubeEndLjStyle==2)	//环形阵列
	{	//法兰
		double init_angle=(m_iPartType!=0)?0:LayoutBoltPara.m_fOffsetAngle;
		double piece_angle=2*Pi/LayoutBoltPara.m_nBoltNum;
		for(int i=0;i<LayoutBoltPara.m_nBoltNum;i++)
		{
			double angle=init_angle*RADTODEG_COEF+i*piece_angle;
			bolt_pos.x=0.5*LayoutBoltPara.m_fBoltLayoutD*cos(angle);
			bolt_pos.y=0.5*LayoutBoltPara.m_fBoltLayoutD*sin(angle);
			bolt_pos.z=0;
			bolt_pt_arr.append(bolt_pos);
		}
	}
	else if(m_iTubeEndLjStyle>2)	//矩形阵列
	{	//插板
		//根据输入的第一个螺栓的位置,阵列数,以及矩阵(行X列),间距布置螺栓
		//十字插板上布置螺栓时可能需要布置双排螺栓		wht 09-06-06
		//双排螺栓分两种方式：第一种就是默认的每个小包板上螺栓布置相同；
		//					  第二种就是水平面包板上的螺栓与竖直面上的螺栓呈交错布置
		for(LAYOUT_BOLT_PARA *pLayoutPara=LayoutBoltParaList.GetFirst();pLayoutPara;pLayoutPara=LayoutBoltParaList.GetNext())
		{
			if(m_paramPlate.m_iParamType==TYPE_XEND)
			{	//在包板坐标系下布置十字插板螺栓
				double H=0,N=0,A=0,M=0,R=0;
				m_paramPlate.GetDesignItemValue('H',&H);
				m_paramPlate.GetDesignItemValue('N',&N);
				m_paramPlate.GetDesignItemValue('A',&A);
				m_paramPlate.GetDesignItemValue('M',&M);
				m_paramPlate.GetDesignItemValue('R',&R);
				double fBoltPosX=pLayoutPara->m_posFirstBolt.x+A+M;
				double fBoltPosY=pLayoutPara->m_posFirstBolt.y+R;
				double fBoltPosZ=pLayoutPara->m_posFirstBolt.z;
				//布置十字插板螺栓时用户输入的为相对包板的坐标此处需要将相对包板的坐标转换到十字插板坐标系下
				//水平面与竖直面交错布置时的螺栓Y坐标与H无关 wht 10-01-22
				double Y=R+(N-pLayoutPara->m_posFirstBolt.y);	//交错布置时的螺栓Y坐标
				double offset_dist=0;
				for(int i=0;i<pLayoutPara->m_nBoltNum;i++)
				{
					//X-Y平面: Y正方向一侧的螺栓
					bolt_pos.x=fBoltPosX+pLayoutPara->m_fColSpace*(i/abs(pLayoutPara->m_nRowNum));
					bolt_pos.y=fBoltPosY+pLayoutPara->m_fRowSpace*(i%abs(pLayoutPara->m_nRowNum));
					bolt_pos.z=fBoltPosZ;
					bolt_pos.feature=11;
					bolt_pt_arr.append(bolt_pos);
					//X-Y平面: Y负方向一侧的螺栓
					bolt_pos.x=fBoltPosX+pLayoutPara->m_fColSpace*(i/abs(pLayoutPara->m_nRowNum));
					bolt_pos.y=-(fBoltPosY+pLayoutPara->m_fRowSpace*(i%abs(pLayoutPara->m_nRowNum)));
					bolt_pos.z=fBoltPosZ;
					bolt_pos.feature=21;
					bolt_pt_arr.append(bolt_pos);
					if(pLayoutPara->m_bStaggerLayout)	//水平面和竖直面交错布置螺栓
					{	
						//X-Z平面: Z正方向一侧的螺栓
						bolt_pos.x=fBoltPosX+pLayoutPara->m_fColSpace*(i/abs(pLayoutPara->m_nRowNum));
						bolt_pos.y=0;
						bolt_pos.z=Y;
						bolt_pos.feature=31;
						bolt_pt_arr.append(bolt_pos);
						//X-Z平面: Z负方向一侧的螺栓
						bolt_pos.x=fBoltPosX+pLayoutPara->m_fColSpace*(i/abs(pLayoutPara->m_nRowNum));
						bolt_pos.y=0;
						bolt_pos.z=-Y;
						bolt_pos.feature=41;
						bolt_pt_arr.append(bolt_pos);
					}
					else
					{
						//X-Z平面: Z正方向一侧的螺栓
						bolt_pos.x=fBoltPosX+pLayoutPara->m_fColSpace*(i/abs(pLayoutPara->m_nRowNum));
						bolt_pos.y=0;
						bolt_pos.z=fBoltPosY+pLayoutPara->m_fRowSpace*(i%abs(pLayoutPara->m_nRowNum));
						bolt_pos.feature=31;
						bolt_pt_arr.append(bolt_pos);
						//X-Z平面: Z负方向一侧的螺栓
						bolt_pos.x=fBoltPosX+pLayoutPara->m_fColSpace*(i/abs(pLayoutPara->m_nRowNum));
						bolt_pos.y=0;
						bolt_pos.z=-(fBoltPosY+pLayoutPara->m_fRowSpace*(i%abs(pLayoutPara->m_nRowNum)));
						bolt_pos.feature=41;
						bolt_pt_arr.append(bolt_pos);
					}
				}
			}
			else
			{
				for(int i=0;i<pLayoutPara->m_nBoltNum;i++)
				{
					bolt_pos.x=pLayoutPara->m_posFirstBolt.x+pLayoutPara->m_fColSpace*(i/pLayoutPara->m_nRowNum);
					bolt_pos.y=pLayoutPara->m_posFirstBolt.y+pLayoutPara->m_fRowSpace*(i%pLayoutPara->m_nRowNum);
					bolt_pos.z=pLayoutPara->m_posFirstBolt.z;
					bolt_pt_arr.append(bolt_pos);
				}
				int nLast=pLayoutPara->m_nBoltNum%pLayoutPara->m_nRowNum;
				if(nLast>0)	
				{	//最后一个螺栓位于中线上
					int index=0;
					for (f3dPoint *pPt =bolt_pt_arr.GetTail();index<nLast;index++,pPt=bolt_pt_arr.GetPrev())
						pPt->y += 0.5*(pLayoutPara->m_nRowNum-nLast)*pLayoutPara->m_fRowSpace;
				}
			}
		}
	}
}

void CDesignTubeEndLjDlg::CreateBolt(CLDSParamPlate *pParamPlate)
{	
	f3dPoint pos;
	char sText[100];
	DWORD dwRayNo = 0;
	if(m_iPartType>0)
		return;	//标准构件螺栓不调用此函数
	if(m_iTubeEndLjStyle==2)	//环向布置螺栓
	{
		if(LayoutBoltPara.m_iStartEdge==1) //以钢管焊道线为基准
			pParamPlate->SetDesignItemValue('J',1,"环向布置肘板或者螺栓时:1表示以焊道线为基准，0表示以基准边为基准。");
		else //if(LayoutBoltPara.m_iStartEdge==0) //以钢管基准边为基准
			pParamPlate->SetDesignItemValue('J',0,"环向布置肘板或者螺栓时:1表示以焊道线为基准，0表示以基准边为基准。");
	}
	//螺栓数据定义区
	ATOM_LIST<RAYNO_RECORD>rayNoList;
	int iInitRayNo=1;
	for(int i=0;i<m_listBolt.GetItemCount();i++)
	{
		m_listBolt.GetItemText(i,0,sText,100);
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		if(m_iTubeEndLjStyle==2)
			pBolt->m_hFamily=4;	//法兰上应选用法兰螺栓规格系列
		pBolt->set_d(atoi(sText+1));
		pBolt->iSeg = pParamPlate->iSeg;
		pBolt->SetLayer(pParamPlate->layer());	//调整螺栓的图层名
		pBolt->cfgword=pParamPlate->cfgword;	//调整螺栓配材号与基准构件配材号一致
		pBolt->des_base_pos.hPart = pParamPlate->handle;
		pBolt->des_base_pos.cLocationStyle=TRUE; 
		pBolt->des_work_norm.norm_style=3;	//构件法线
		pBolt->des_work_norm.hVicePart=pParamPlate->handle;
		pBolt->des_work_norm.direction=LayoutBoltPara.m_iDirection;
		if(m_iTubeEndLjStyle>2)		//矩形阵列
		{
			m_listBolt.GetItemText(i,1,sText,100);
			pos.x=atof(sText);
			m_listBolt.GetItemText(i,2,sText,100);
			pos.y=atof(sText);
			m_listBolt.GetItemText(i,3,sText,100);
			pos.z=atof(sText);
			pBolt->des_base_pos.len_offset_dist=pos.x;
			double L=0;
			if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
			{	//十字插板	
				DWORD dwFlag=m_listBolt.GetItemData(i);	//螺栓标志位
				WORD* pw_h=(WORD*)&dwFlag;
				pos.feature=*(pw_h+1);
				pParamPlate->GetDesignItemValue('L',&L);
				pBolt->des_base_pos.wing_offset_dist=pos.y;
				if(pos.feature==11)
				{	//X-Y平面: Y正方向一侧的螺栓
					if(L>pos.x)//Y正方向一侧,连接插板与包板的螺栓
						pBolt->feature=11;
					else
						pBolt->feature=12;	//Y正方向一侧,连接包板与其他连板的螺栓
					pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.z);
				}
				else if(pos.feature==21)
				{	//X-Y平面: Y负方向一侧的螺栓
					if(L>pos.x)//Y负方向一侧,连接插板与包板的螺栓
						pBolt->feature=21;
					else
						pBolt->feature=22;	//Y负方向一侧,连接包板与其他连板的螺栓
					pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.z);
				}
				else if(pos.feature==31)	
				{	//X-Z平面: Z正方向一侧
					if(L>pos.x)
						pBolt->feature=31;	//Z正方向一侧，连接立板与其他连板的螺栓
					else
						pBolt->feature=32;	//Z正方向一侧，连接包板与其他连板的螺栓
					pBolt->des_work_norm.norm_style=4;
					pBolt->des_work_norm.nearVector.Set(0,1,0);
					pBolt->des_base_pos.wing_offset_dist = pos.z;
					pBolt->ucs.axis_z=pParamPlate->ucs.axis_y;
					//通过零件库生成十字插板时，转换坐标到基准钢板坐标系下，
					//Z正方向立板法向方向为(0,-1,0) 法向偏移量应为负值 //wht 2010-07-06 此处为暂时添加 
					if(m_pSrcParamPlate==NULL)
						pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.y);
					else
						pBolt->des_base_pos.norm_offset.AddThick(-(LONG)pos.y);
				}
				else if(pos.feature==41)	
				{	//X-Z平面: Z负方向一侧
					if(L>pos.x)
						pBolt->feature=41;	//Z负方向一侧，连接立板与其他连板的螺栓
					else
						pBolt->feature=42;	//Z负方向一侧，连接包板与其他连板的螺栓
					pBolt->des_work_norm.norm_style=4;
					pBolt->des_work_norm.nearVector.Set(0,1,0);
					pBolt->des_base_pos.wing_offset_dist = pos.z;
					pBolt->ucs.axis_z=pParamPlate->ucs.axis_y;
					pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.y);
				}
				if(pBolt->feature==11||pBolt->feature==21)	//螺栓在十字插板范围内修改螺栓通厚
					pBolt->AddL0Thick(pParamPlate->handle,TRUE);
			}
			else
			{
				pBolt->AddL0Thick(pParamPlate->handle,TRUE);
				pBolt->des_base_pos.wing_offset_dist = pos.y;
				pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.z);
			}
			pBolt->des_work_norm.direction=LayoutBoltPara.m_iDirection;	//0-朝外 1-朝内
			//coord_trans(pos,pPart->ucs,TRUE);
			RAYNO_RECORD *pRayNo;
			for(pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
			{
				if(pRayNo->yCoord==pos.y)
				{	
					pBolt->dwRayNo=pRayNo->dwRayNo;
					break;
				}
			}
			if(pRayNo==NULL)
			{
				pRayNo=rayNoList.append();
				pRayNo->dwRayNo=GetSingleWord(iInitRayNo);
				pRayNo->yCoord=pos.y;
				pBolt->dwRayNo=pRayNo->dwRayNo;
				pRayNo->hPart=pParamPlate->handle;
				iInitRayNo++;
			}
		}
		else	//环形阵列
		{
			pBolt->AddL0Thick(pParamPlate->handle,TRUE);//螺栓通厚
			m_listBolt.GetItemText(i,2,sText,100);		//方位角
			double angle = atof(sText)*RADTODEG_COEF;	
			pBolt->des_base_pos.offset_angle = angle;	//螺栓相对于基准钢管的偏移角度
			m_listBolt.GetItemText(i,1,sText,100);		//螺栓距基准构件中心的距离
			pBolt->des_base_pos.R=atof(sText);			//该变量小于0表示螺栓为非环向布置
			pBolt->des_work_norm.direction=0;			//环向布置的螺栓默认朝外
		}
		CLsRef *pLsRef=NULL;
		pLsRef=pParamPlate->AppendLsRef(pBolt->GetLsRef());
		if(pLsRef)
			pLsRef->start1_end2_mid0=(BYTE)pBolt->feature;	//标记螺栓所在位置
		pBolt->SetModified();
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	//设置螺栓引用射线号
	if(m_iTubeEndLjStyle>2)	//在参数化板上矩形阵列布置螺栓
		pParamPlate->GetLsRefList()->ResetBoltRefRayNo(pParamPlate);
	g_pSolidDraw->Draw();
}

//标注构件尺寸 
//iOffsetType -1.X轴负方向 -2.Y轴负方向 1.X轴正方向 2.Y轴正方向
static void DimText(CPaintDC *pDc,f2dPoint start,f2dPoint end,double fLen,
					int iOffsetType,int nOffsetDist=0)
{
	CString sLen;
	sLen.Format("%f",fLen);
	SimplifiedNumString(sLen);
	CSize sizeText=pDc->GetTextExtent(sLen);
	int nSpaceDist=5;	//标注文字与轮廓边之间的间隙值
	f2dPoint dim_pos,dim_start,dim_end,mid_start,mid_end,ext_start,ext_end;
	dim_start=start; 
	dim_end=end;
	if(abs(iOffsetType)==1)
	{	//垂直标注
		if(nOffsetDist==0)	//标注位置相对于标注点的偏移距离
			nOffsetDist=ftoi(sizeText.cx*0.5)+4;
		dim_start.x+=(iOffsetType*nOffsetDist);
		dim_end.x+=(iOffsetType*nOffsetDist);
		//
		ext_start=dim_start;
		ext_end=dim_end;
		ext_start.x+=(iOffsetType*sizeText.cy*0.25);
		ext_end.x+=(iOffsetType*sizeText.cy*0.25);
		dim_pos.Set(dim_start.x,0.5*(dim_start.y+dim_end.y));
		//增加两个标注点空出文字空间
		int nFlag=1;
		if(dim_start.y<dim_end.y)
			nFlag=-1;
		mid_start.Set(dim_start.x,dim_pos.y+0.6*sizeText.cy*nFlag);
		mid_end.Set(dim_start.x,dim_pos.y-0.6*sizeText.cy*nFlag);
	}
	else if(abs(iOffsetType)==2)
	{	//水平标注
		iOffsetType/=2;	//除以2
		if(nOffsetDist==0)	//标注位置相对于标注点的偏移距离
			nOffsetDist=ftoi(sizeText.cy*0.6);
		dim_start.y+=(iOffsetType*nOffsetDist);
		dim_end.y+=(iOffsetType*nOffsetDist);
		dim_pos.Set(0.5*(dim_start.x+dim_end.x),dim_start.y);
		//
		ext_start=dim_start;
		ext_end=dim_end;
		ext_start.y+=(iOffsetType*sizeText.cy*0.25);
		ext_end.y+=(iOffsetType*sizeText.cy*0.25);
		//增加两个标注点空出文字空间
		int nFlag=1;
		if(dim_start.x<dim_end.x)
			nFlag=-1;
		mid_start.Set(dim_pos.x+0.6*sizeText.cx*nFlag,dim_start.y);
		mid_end.Set(dim_pos.x-0.6*sizeText.cx*nFlag,dim_start.y);
		iOffsetType*=2;	//乘以2
	}
	//绘制标注线
	pDc->MoveTo(ftoi(start.x),ftoi(start.y));
	pDc->LineTo(ftoi(ext_start.x),ftoi(ext_start.y));
	pDc->MoveTo(ftoi(end.x),ftoi(end.y));
	pDc->LineTo(ftoi(ext_end.x),ftoi(ext_end.y));
	//
	int nDimSpace=(int)DISTANCE(f3dPoint(dim_start.x,dim_start.y,0),f3dPoint(dim_end.x,dim_end.y,0));
	if(nDimSpace<sizeText.cx)
	{
		pDc->MoveTo(ftoi(dim_start.x),ftoi(dim_start.y));
		pDc->LineTo(ftoi(dim_end.x),ftoi(dim_end.y));
		//绘制标注内容
		dim_pos.x+=sizeText.cx;
		pDc->TextOut(ftoi(dim_pos.x-0.5*sizeText.cx),ftoi(dim_pos.y-sizeText.cy*0.5),sLen);
	}
	else
	{
		pDc->MoveTo(ftoi(dim_start.x),ftoi(dim_start.y));
		pDc->LineTo(ftoi(mid_start.x),ftoi(mid_start.y));
		pDc->MoveTo(ftoi(mid_end.x),ftoi(mid_end.y));
		pDc->LineTo(ftoi(dim_end.x),ftoi(dim_end.y));
		//绘制标注内容
		pDc->TextOut(ftoi(dim_pos.x-0.5*sizeText.cx),ftoi(dim_pos.y-sizeText.cy*0.5),sLen);
	}
}

void CDesignTubeEndLjDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rectDrawRgn; //当然绘图区域
	CWnd *pWnd = GetDlgItem(IDC_S_MAP);
	pWnd->GetWindowRect(&rectDrawRgn);
	ScreenToClient(&rectDrawRgn);
	int width=rectDrawRgn.Width();
	int height=rectDrawRgn.Height();
	//
	double W=0,L=0,H=0,N=0,S=0,R=0,M=0,T=0,A=0;
	double real_width=0,real_height=0,scale=1;
	UCS_2dSTRU draw_ucs;	//绘图坐标系
	T=m_paramPlate.GetThick();
	//
	if(m_iTubeEndLjStyle==2)
	{	//法兰
		if(!m_paramPlate.m_bStdPart||(m_paramPlate.m_bStdPart&&(m_iPartType==3||m_iPartType==4)))
		{	//普通|刚性|柔性法兰
			m_paramPlate.GetDesignItemValue('W',&W);
			m_paramPlate.GetDesignItemValue('R',&R);
		}
		else 
		{	//平焊|对焊法兰
			m_paramPlate.GetDesignItemValue('D',&W);
			m_paramPlate.GetDesignItemValue('B',&R);
			R*=0.5;
		}
		real_width=real_height=W+10;
		draw_ucs.origin.Set(rectDrawRgn.TopLeft().x+width*0.5,rectDrawRgn.TopLeft().y+height*0.5);
	}
	else if(m_iTubeEndLjStyle>2)
	{	//插板
		m_paramPlate.GetDesignItemValue('L',&L);
		m_paramPlate.GetDesignItemValue('H',&H);
		m_paramPlate.GetDesignItemValue('M',&M);
		m_paramPlate.GetDesignItemValue('S',&S);
		m_paramPlate.GetDesignItemValue('N',&N);
		m_paramPlate.GetDesignItemValue('W',&W);
		m_paramPlate.GetDesignItemValue('A',&A);
		m_paramPlate.GetDesignItemValue('R',&R);
		double offset_dist_x=0,offset_dist_y=0.5*height;
		if(m_iTubeEndLjStyle==3)
		{	//U型板
			real_width=L+3*W;
			real_height=H+8*W;
			offset_dist_x=((2*W)/(L+3*W))*width;
			offset_dist_y-=(W/(H+8*W))*height;
		}
		else if(m_iTubeEndLjStyle==4)
		{	//卷边板
			real_width=L+4*W;
			real_height=H+2*W;
			offset_dist_x=(W/(L+4*W))*width;
		}
		else if(m_iTubeEndLjStyle==5)
		{	//十字插板
			real_width=M+S+5*N;
			real_height=H+3*N;
			offset_dist_x=(N/(M+S+5*N))*width;
			offset_dist_y+=(N/(H+3*N))*height;
		}
		draw_ucs.origin.Set(rectDrawRgn.TopLeft().x+offset_dist_x,rectDrawRgn.TopLeft().y+offset_dist_y);
	}
	draw_ucs.axis_x.Set(1,0);
	draw_ucs.axis_y.Set(0,-1);
	double scale_x=width/real_width;
	double scale_y=height/real_height;
	scale=min(scale_x,scale_y);
	dc.SelectStockObject(LTGRAY_BRUSH); 
	dc.Rectangle(rectDrawRgn);
	dc.SelectStockObject(HOLLOW_BRUSH);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0,0,0));
	//绘制坐标系
	CPen *pOldPen=dc.SelectObject(&red_solid_pen);
	if(m_iTubeEndLjStyle>=2)
	{
		f2dPoint origin=draw_ucs.origin;
		int axis_y_len=ftoi(0.3*height);
		int axis_x_len=axis_y_len;
		//法兰坐标系X、Y轴长度取0.8半径 因为要绘制基准边以及焊道线标记
		if(m_iTubeEndLjStyle==2) 
			axis_x_len=axis_y_len=ftoi(W*0.5*scale)+3;
		//X轴
		dc.MoveTo(ftoi(origin.x),ftoi(origin.y));
		dc.LineTo(ftoi(origin.x+axis_x_len),ftoi(origin.y));
		//绘制箭头
		dc.LineTo(ftoi(origin.x+axis_x_len-8),ftoi(origin.y-4));
		dc.MoveTo(ftoi(origin.x+axis_x_len),ftoi(origin.y));
		dc.LineTo(ftoi(origin.x+axis_x_len-8),ftoi(origin.y+4));
		//Y轴
		dc.SelectObject(&green_solid_pen);
		dc.MoveTo(ftoi(origin.x),ftoi(origin.y));
		dc.LineTo(ftoi(origin.x),ftoi(origin.y-axis_y_len));
		//绘制箭头
		dc.LineTo(ftoi(origin.x-4),ftoi(origin.y-axis_y_len+8));
		dc.MoveTo(ftoi(origin.x),ftoi(origin.y-axis_y_len));
		dc.LineTo(ftoi(origin.x+4),ftoi(origin.y-axis_y_len+8));
		dc.SelectObject(pOldPen);
	}
	POINT topLeft,bottomRight;
	if(m_iTubeEndLjStyle==2)
	{	//法兰
		double fTubeR=R*scale-1;
		double fTubeThick=min(m_pCurLineTube->GetThick()*scale,8);
		int i;
		for(i=0;i<4;i++)
		{
			int nRadius=ftoi(0.5*W*scale);
			if(i==1)
				nRadius=ftoi(R*scale);
			else if(i==2)
			{
				pOldPen=dc.SelectObject(&green_dash_pen);
				nRadius=ftoi(LayoutBoltPara.m_fBoltLayoutD*0.5*scale);
			}
			//绘制钢管示意图
			else if(i==3)
				nRadius=ftoi(fTubeR-fTubeThick);
			topLeft.x=ftoi(draw_ucs.origin.x)-nRadius;
			topLeft.y=ftoi(draw_ucs.origin.y)-nRadius;
			bottomRight.x=ftoi(draw_ucs.origin.x)+nRadius;
			bottomRight.y=ftoi(draw_ucs.origin.y)+nRadius;
			dc.Ellipse(topLeft.x,topLeft.y,bottomRight.x,bottomRight.y);
			if(i==2)
				dc.SelectObject(pOldPen);
		}
		//标注法兰尺寸
		pOldPen=dc.SelectObject(&purple_solid_pen);
		double tube_r=m_pCurLineTube->GetDiameter()*0.5;
		for(i=0;i<3;i++)
		{
			f2dPoint dim_start,dim_end;
			double cur_r=LayoutBoltPara.m_fBoltLayoutD*0.5;
			if(i==1)
				cur_r=R;
			else if(i==2)
				cur_r=W*0.5;
			dim_start.Set(0,cur_r*scale);
			dim_end.Set(0,-cur_r*scale);
			coord_trans(dim_start,draw_ucs,TRUE);
			coord_trans(dim_end,draw_ucs,TRUE);
			if(i==0)
				DimText(&dc,dim_start,dim_end,2*cur_r,-1,ftoi(cur_r*scale*1.2));
			else if(i==2)
				DimText(&dc,dim_start,dim_end,2*cur_r,-1,ftoi(cur_r*scale*1.5));
			else
				DimText(&dc,dim_start,dim_end,2*cur_r,-1,ftoi(cur_r*0.5*scale));
		}
		dc.SelectObject(pOldPen);
		//绘制钢管基准边与焊道线标记
		for(i=0;i<2;i++)
		{
			if(!m_pCurLineTube->m_bHasWeldRoad&&i==1)
				continue;
			int nInnerR=ftoi(fTubeR-fTubeThick);
			int nOutterR=ftoi(fTubeR);
			CPoint ptVertex[3];
			f2dPoint tempVertexArr[3];
			UCS_2dSTRU temp_ucs=draw_ucs;
			f3dPoint rot_axis(0,0,1);
			if(m_bStartEnd)
				rot_axis*=-1.0;
			double fOffsetAngle=0;
			if(m_iPartType!=0&&m_iTubeEndLjStyle==2)	//按指定角度调整标准法兰X轴方向 wht 11-05-26
				fOffsetAngle=LayoutBoltPara.m_fOffsetAngle*RADTODEG_COEF;
			if(i==1)
			{	//焊道线
				double fWeldRoadAngle=-m_pCurLineTube->CalWeldRoadAngle();
				if(LayoutBoltPara.m_iStartEdge==1)
					fWeldRoadAngle*=-1.0;
				fOffsetAngle+=fWeldRoadAngle;
			}
			if(fabs(fOffsetAngle)>EPS)
			{	//绕指定轴旋转指定角度
				f3dPoint axis(temp_ucs.axis_x.x,temp_ucs.axis_x.y,0);
				RotateVectorAroundVector(axis,fOffsetAngle,rot_axis);
				temp_ucs.axis_x.Set(axis.x,axis.y);
				axis.Set(temp_ucs.axis_y.x,temp_ucs.axis_y.y,0);
				RotateVectorAroundVector(axis,fOffsetAngle,rot_axis);
				temp_ucs.axis_y.Set(axis.x,axis.y);
			}
			tempVertexArr[0].Set(nInnerR,fTubeThick);
			tempVertexArr[1].Set(nInnerR,-fTubeThick);
			tempVertexArr[2].Set(nOutterR,0);
			int j;
			for(j=0;j<3;j++)
			{
				coord_trans(tempVertexArr[j],temp_ucs,TRUE);
				ptVertex[j].x=ftoi(tempVertexArr[j].x);
				ptVertex[j].y=ftoi(tempVertexArr[j].y);
			}
			for(j=0;j<3;j++)
			{
				dc.MoveTo(ftoi(tempVertexArr[j].x),ftoi(tempVertexArr[j].y));
				dc.LineTo(ftoi(tempVertexArr[(j+1)%3].x),ftoi(tempVertexArr[(j+1)%3].y));
			}
			CRgn rgn;
			rgn.CreatePolygonRgn(ptVertex,3,ALTERNATE);
			CBrush curBrush;
			if(LayoutBoltPara.m_iStartEdge==1)
			{	//以焊道线为起始边
				if(i==0) //焊道线标志
					curBrush.CreateSolidBrush(RGB(0,0,0));
				else	 //基准边标志
					curBrush.CreateSolidBrush(RGB(0,255,0));
			}
			else
			{	//以基准边为起始边
				if(i==0) //基准边标志	
					curBrush.CreateSolidBrush(RGB(0,255,0));
				else	 //焊道线标志
					curBrush.CreateSolidBrush(RGB(0,0,0));
			}
			dc.FillRgn(&rgn,&curBrush);
		}
		//绘制螺栓 按螺栓列表中的参数绘制螺栓 wht 11-05-09
		pOldPen=dc.SelectObject(&red_solid_pen);
		int iCurSel=-1;
		POSITION pos = m_listBolt.GetFirstSelectedItemPosition();
		if(pos!=NULL)
			iCurSel = m_listBolt.GetNextSelectedItem(pos);
		int nCount=m_listBolt.GetItemCount();
		for(i=0;i<nCount;i++)
		{
			char sText[100]="";
			m_listBolt.GetItemText(i,0,sText,100);
			int nBoltR=ftoi(atoi(sText+1)*0.5*scale);
			m_listBolt.GetItemText(i,2,sText,100);		//方位角
			double angle = atof(sText)*RADTODEG_COEF;	//螺栓相对于基准钢管的偏移角度
			m_listBolt.GetItemText(i,1,sText,100);		//螺栓距基准构件中心的距离
			double R=atof(sText)*scale;			
			f3dPoint tempPoint(R,0,0);
			rotate_point_around_axis(tempPoint,angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
			f2dPoint curPoint(tempPoint.x,tempPoint.y);
			coord_trans(curPoint,draw_ucs,TRUE);
			topLeft.x=ftoi(curPoint.x)-nBoltR;
			topLeft.y=ftoi(curPoint.y)-nBoltR;
			bottomRight.x=ftoi(curPoint.x)+nBoltR;
			bottomRight.y=ftoi(curPoint.y)+nBoltR;
			if(iCurSel==i)	//当前选中的螺栓
				dc.SelectObject(&green_solid_pen);
			else
				dc.SelectObject(&red_solid_pen);
			dc.Ellipse(topLeft.x,topLeft.y,bottomRight.x,bottomRight.y);
		}
		dc.SelectObject(pOldPen);
		//生成附加板示意图
		double sector_angle=(2*Pi)/LayoutBoltPara.m_nBoltNum;
		if(m_bCreateAttachPlate&&(m_iPartType==0||m_iPartType>2))
		{	
			double start_angle=AttachPlatePara.m_fAngle*RADTODEG_COEF;
			f3dPoint startPt(R*scale,0,0);
			f3dPoint endPt((R+AttachPlatePara.B)*scale,0,0);
			double fPlateThick=AttachPlatePara.m_nThick*scale;
			for(i=0;i<AttachPlatePara.m_nPlateNum;i++)
			{
				f2dPoint vertex_arr[4];
				f3dPoint tempStart=startPt;
				f3dPoint tempEnd=endPt;
				rotate_point_around_axis(tempStart,start_angle+i*sector_angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
				rotate_point_around_axis(tempEnd,start_angle+i*sector_angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
				f3dPoint len_vec=tempEnd-tempStart;
				normalize(len_vec);
				f3dPoint offset_vec(-len_vec.y,len_vec.x,0);
				f3dPoint temp_vertex=tempStart+offset_vec*fPlateThick*0.5;
				vertex_arr[0].Set(temp_vertex.x,temp_vertex.y);
				temp_vertex=tempStart-offset_vec*fPlateThick*0.5;
				vertex_arr[1].Set(temp_vertex.x,temp_vertex.y);
				temp_vertex=tempEnd-offset_vec*fPlateThick*0.5;
				vertex_arr[2].Set(temp_vertex.x,temp_vertex.y);
				temp_vertex=tempEnd+offset_vec*fPlateThick*0.5;
				vertex_arr[3].Set(temp_vertex.x,temp_vertex.y);
				for(int j=0;j<4;j++)
					coord_trans(vertex_arr[j],draw_ucs,TRUE);
				if(iCurSel==i)
					pOldPen=dc.SelectObject(&green_solid_pen);
				for(j=0;j<4;j++)
				{
					dc.MoveTo(ftoi(vertex_arr[j].x),ftoi(vertex_arr[j].y));
					dc.LineTo(ftoi(vertex_arr[(j+1)%4].x),ftoi(vertex_arr[(j+1)%4].y));
				}
				if(iCurSel==i)
					dc.SelectObject(pOldPen);
			}	
		}
	}
	else if(m_iTubeEndLjStyle>2)
	{	//槽型插板、U型插板、十字插板
		int i=0;
		f2dPoint dim_start,dim_end;	
		double fTubeR=m_pCurLineTube->GetDiameter()*0.5;
		double fTubeThick=max(m_pCurLineTube->GetThick(),8);
		f2dPoint vertex_arr[8];
		double fCutAngleLen=N,fCutAngleWidth=S;
		if(m_iTubeEndLjStyle==5)
			fCutAngleLen=fCutAngleWidth=0;
		vertex_arr[0].Set(0,0.5*H*scale);
		vertex_arr[1].Set(0,-0.5*H*scale);
		vertex_arr[2].Set((L-fCutAngleLen)*scale,-0.5*H*scale);
		vertex_arr[3].Set(L*scale,(fCutAngleWidth-0.5*H)*scale);
		vertex_arr[4].Set(L*scale,(-fCutAngleWidth+0.5*H)*scale);
		vertex_arr[5].Set((L-fCutAngleLen)*scale,0.5*H*scale);
		//钢管端点
		vertex_arr[6].Set(M*scale,fTubeR*scale);
		vertex_arr[7].Set(M*scale,-fTubeR*scale);
		for(i=0;i<8;i++)
			coord_trans(vertex_arr[i],draw_ucs,TRUE);
		BOOL bOutside=FALSE;
		if((work_norm*m_pCurLineTube->WorkPlaneNorm()>0&&ftol(setupInfo.workNorm.rot_ang)==180)
			||(work_norm*m_pCurLineTube->WorkPlaneNorm()<0&&ftol(setupInfo.workNorm.rot_ang)==0))
			bOutside=TRUE;
		//绘制构件轮廓图
		for(i=0;i<6;i++)
		{
			f2dPoint start=vertex_arr[i];
			f2dPoint end=vertex_arr[(i+1)%6];
			dc.MoveTo(ftoi(start.x),ftoi(start.y));
			dc.LineTo(ftoi(end.x),ftoi(end.y));
			if(m_iTubeEndLjStyle==4&&(i==1||i==5))
			{	//绘制卷边
				int nFlag=1;
				if(i==1)
					nFlag=-1;
				if(!bOutside)
					pOldPen=dc.SelectObject(&dash_pen);
				dc.MoveTo(ftoi(start.x),ftoi(start.y+nFlag*T*scale));
				dc.LineTo(ftoi(end.x),ftoi(end.y+nFlag*T*scale));
				if(!bOutside)
					dc.SelectObject(pOldPen);
			}
		}
		//标注构件轮廓边尺寸
		pOldPen=dc.SelectObject(&purple_solid_pen);
		if(m_iTubeEndLjStyle>2)
		{	//H
			if(m_iTubeEndLjStyle==5)
				fCutAngleLen=A+S-(L-M);
			dim_start.Set(vertex_arr[2].x+fCutAngleLen*scale,vertex_arr[2].y);
			dim_end.Set(vertex_arr[5].x+fCutAngleLen*scale,vertex_arr[5].y);
			DimText(&dc,dim_start,dim_end,H,1);	
			//L
			dim_start.Set(vertex_arr[0].x,vertex_arr[0].y);
			dim_end.Set(vertex_arr[4].x,vertex_arr[4].y-fCutAngleWidth*scale);
			DimText(&dc,dim_start,dim_end,L,-2);
			//M
			dim_start.Set(vertex_arr[1].x,vertex_arr[1].y);
			dim_end.Set(vertex_arr[1].x+M*scale,vertex_arr[1].y);
			DimText(&dc,dim_start,dim_end,M,2);
			if(m_iTubeEndLjStyle==3||m_iTubeEndLjStyle==4)
			{	//N
				dim_start.Set(vertex_arr[2].x,vertex_arr[2].y);
				dim_end.Set(vertex_arr[2].x+N*scale,vertex_arr[2].y);
				DimText(&dc,dim_start,dim_end,N,2);
				//S
				dim_start.Set(vertex_arr[5].x,vertex_arr[5].y);
				dim_end.Set(vertex_arr[5].x,vertex_arr[5].y+S*scale);
				DimText(&dc,dim_start,dim_end,S,-1);
			}
			else if(m_iTubeEndLjStyle==5)
			{	//A
				dim_start.Set(vertex_arr[1].x+M*scale,vertex_arr[1].y);
				dim_end.Set(vertex_arr[1].x+(A+M)*scale,vertex_arr[1].y);
				DimText(&dc,dim_start,dim_end,A,2);	
			}
			//标注钢管直径
			dim_start.Set(0,fTubeR*scale);
			dim_end.Set(0,-fTubeR*scale);
			coord_trans(dim_start,draw_ucs,TRUE);
			coord_trans(dim_end,draw_ucs,TRUE);
			DimText(&dc,dim_start,dim_end,2*fTubeR,-1);
		}
		dc.SelectObject(pOldPen);
		//绘制钢管示意图
		double tube_offset_len=0.5*W;
		if(m_iTubeEndLjStyle==5)
			tube_offset_len=0.5*N;
		dc.MoveTo(ftoi(vertex_arr[6].x),ftoi(vertex_arr[6].y));
		dc.LineTo(ftoi(vertex_arr[7].x),ftoi(vertex_arr[7].y));
		dc.MoveTo(ftoi(vertex_arr[6].x),ftoi(vertex_arr[6].y));
		dc.LineTo(ftoi(vertex_arr[6].x-(M+tube_offset_len)*scale),ftoi(vertex_arr[6].y));
		dc.MoveTo(ftoi(vertex_arr[7].x),ftoi(vertex_arr[7].y));
		dc.LineTo(ftoi(vertex_arr[7].x-(M+tube_offset_len)*scale),ftoi(vertex_arr[7].y));
		pOldPen=dc.SelectObject(&green_dash_pen);
		dc.MoveTo(ftoi(vertex_arr[6].x),ftoi(vertex_arr[6].y+fTubeThick*scale));
		dc.LineTo(ftoi(vertex_arr[6].x-(M+tube_offset_len)*scale),ftoi(vertex_arr[6].y+fTubeThick*scale));
		dc.MoveTo(ftoi(vertex_arr[7].x),ftoi(vertex_arr[7].y-fTubeThick*scale));
		dc.LineTo(ftoi(vertex_arr[7].x-(M+tube_offset_len)*scale),ftoi(vertex_arr[7].y-fTubeThick*scale));
		dc.SelectObject(pOldPen);
		
		if(m_iTubeEndLjStyle==3)
		{	//绘制U型插板侧面图
			vertex_arr[0].Set((0.5*W+T)*scale,(0.5*W+T)*scale);
			vertex_arr[1].Set(L*scale,(0.5*W+T)*scale);
			vertex_arr[2].Set(L*scale,0.5*W*scale);
			vertex_arr[3].Set((0.5*W+T)*scale,0.5*W*scale);
			vertex_arr[4].Set((0.5*W+T)*scale,-0.5*W*scale);
			vertex_arr[5].Set(L*scale,-0.5*W*scale);
			vertex_arr[6].Set(L*scale,-(0.5*W+T)*scale);
			vertex_arr[7].Set((0.5*W+T)*scale,-(0.5*W+T)*scale);
			//临时坐标系
			UCS_2dSTRU temp_ucs=draw_ucs;
			temp_ucs.origin.y+=(0.5*H+3*W)*scale;
			for(i=0;i<8;i++)
				coord_trans(vertex_arr[i],temp_ucs,TRUE);
			for(i=0;i<8;i++)
			{
				f2dPoint start=vertex_arr[i];
				f2dPoint end=vertex_arr[(i+1)%8];
				dc.MoveTo(ftoi(start.x),ftoi(start.y));
				dc.LineTo(ftoi(end.x),ftoi(end.y));
			}
			f2dPoint center((0.5*W+T)*scale,0);
			coord_trans(center,temp_ucs,TRUE);
			for(i=0;i<8;i++)
			{
				f2dPoint start=vertex_arr[i];
				f2dPoint end=vertex_arr[(i+1)%8];
				if(i==3)
					dc.AngleArc(ftoi(center.x),ftoi(center.y),ftoi(0.5*W*scale),90,180);
				else if(i==7)
					dc.AngleArc(ftoi(center.x),ftoi(center.y),ftoi((0.5*W+T)*scale),90,180);
				else
				{
					dc.MoveTo(ftoi(start.x),ftoi(start.y));
					dc.LineTo(ftoi(end.x),ftoi(end.y));
				}
			}
			//标注W尺寸
			pOldPen=dc.SelectObject(&purple_solid_pen);
			dim_start.Set(vertex_arr[2].x,vertex_arr[2].y);
			dim_end.Set(vertex_arr[2].x,vertex_arr[2].y+W*scale);
			DimText(&dc,dim_start,dim_end,W,1);
			dc.SelectObject(pOldPen);
		}
		else if(m_iTubeEndLjStyle==4) 
		{	//槽型插板卷边示意图
			vertex_arr[0].Set(0,0.5*H*scale);
			vertex_arr[1].Set(0,-0.5*H*scale);
			vertex_arr[2].Set(W*scale,-0.5*H*scale);
			vertex_arr[3].Set(W*scale,(T-0.5*H)*scale);
			vertex_arr[4].Set(T*scale,(T-0.5*H)*scale);
			vertex_arr[5].Set(T*scale,(-T+0.5*H)*scale);
			vertex_arr[6].Set(W*scale,(-T+0.5*H)*scale);
			vertex_arr[7].Set(W*scale,0.5*H*scale);
			//临时坐标系
			UCS_2dSTRU temp_ucs=draw_ucs;
			temp_ucs.origin.x+=(L+1.5*W)*scale;
			for(i=0;i<8;i++)
				coord_trans(vertex_arr[i],temp_ucs,TRUE);
			for(i=0;i<8;i++)
			{
				f2dPoint start=vertex_arr[i];
				f2dPoint end=vertex_arr[(i+1)%8];
				dc.MoveTo(ftoi(start.x),ftoi(start.y));
				dc.LineTo(ftoi(end.x),ftoi(end.y));
			}
			//标注W尺寸
			pOldPen=dc.SelectObject(&purple_solid_pen);
			dim_start.Set(vertex_arr[1].x,vertex_arr[1].y);
			dim_end.Set(vertex_arr[1].x+W*scale,vertex_arr[1].y);
			DimText(&dc,dim_start,dim_end,W,2);
			dc.SelectObject(pOldPen);
		}
		else 
		{	//十字插板包板立板
			double offset_x=M+A,offset_y=R;
			for(int j=0;j<16;j++)
			{
				//临时坐标系
				UCS_2dSTRU temp_ucs=draw_ucs;
				if(j==0)
				{	//立板
					vertex_arr[0].Set(0,0.5*T*scale);
					vertex_arr[1].Set(0,-0.5*T*scale);
					vertex_arr[2].Set(L*scale,-0.5*T*scale);
					vertex_arr[3].Set(L*scale,0.5*T*scale);
				}
				else if(j==1||j==2)
				{	//水平包板
					int nFlag=1;
					if(j==2)
						nFlag=-1;
					vertex_arr[0].Set(offset_x*scale,nFlag*offset_y*scale);
					vertex_arr[1].Set((offset_x+S)*scale,nFlag*offset_y*scale);
					vertex_arr[2].Set((offset_x+S)*scale,nFlag*(offset_y+N)*scale);
					vertex_arr[3].Set(offset_x*scale,nFlag*(offset_y+N)*scale);
				}
				else if(j==3||j==4)
				{	//竖直包板
					int nFlag=1;
					if(j==4)
						nFlag=-1;
					vertex_arr[0].Set(offset_x*scale,nFlag*0.5*T*scale);
					vertex_arr[1].Set((offset_x+S)*scale,nFlag*0.5*T*scale);
					vertex_arr[2].Set((offset_x+S)*scale,nFlag*(0.5*T+W)*scale);
					vertex_arr[3].Set(offset_x*scale,nFlag*(0.5*T+W)*scale);
				}
				else if(j==5)
				{	//包板详图
					vertex_arr[0].Set(offset_x*scale,offset_y*scale);
					vertex_arr[1].Set((offset_x+S)*scale,offset_y*scale);
					vertex_arr[2].Set((offset_x+S)*scale,(offset_y+N)*scale);
					vertex_arr[3].Set(offset_x*scale,(offset_y+N)*scale);
					//调整坐标系
					temp_ucs.origin.y-=2*N*scale;
				}
				//十字插板端面图
				else
				{	//调整坐标系
					temp_ucs.origin.x+=(M+A+S+2.5*N)*scale;
					if(j==6)
					{	
						vertex_arr[0].Set(-0.5*T*scale,0.5*H*scale);
						vertex_arr[1].Set(0.5*T*scale,0.5*H*scale);
						vertex_arr[2].Set(0.5*T*scale,-0.5*H*scale);
						vertex_arr[3].Set(-0.5*T*scale,-0.5*H*scale);
					}
					else if(j==7)
					{
						vertex_arr[0].Set(-0.5*H*scale,0.5*T*scale);
						vertex_arr[1].Set(0.5*H*scale,0.5*T*scale);
						vertex_arr[2].Set(0.5*H*scale,-0.5*T*scale);
						vertex_arr[3].Set(-0.5*H*scale,-0.5*T*scale);
					}
					else if(j>7&&j<12)
					{
						int nFlagX=1,nFlagY=1;
						if(j==9)
						{
							nFlagX=-1;
							nFlagY=1;
						}
						else if(j==10)
						{
							nFlagX=-1;
							nFlagY=-1;
						}
						else if(j==11)
						{
							nFlagX=1;
							nFlagY=-1;
						}
						vertex_arr[0].Set(nFlagX*0.5*T*scale,nFlagY*R*scale);
						vertex_arr[1].Set(nFlagX*(0.5*T+W)*scale,nFlagY*R*scale);
						vertex_arr[2].Set(nFlagX*(0.5*T+W)*scale,nFlagY*(R+N)*scale);
						vertex_arr[3].Set(nFlagX*0.5*T*scale,nFlagY*(R+N)*scale);
					}
					else
					{
						int nFlagX=1,nFlagY=1;
						if(j==13)
						{
							nFlagX=-1;
							nFlagY=1;
						}
						else if(j==14)
						{
							nFlagX=-1;
							nFlagY=-1;
						}
						else if(j==15)
						{
							nFlagX=1;
							nFlagY=-1;
						}
						vertex_arr[0].Set(nFlagX*R*scale,nFlagY*0.5*T*scale);
						vertex_arr[1].Set(nFlagX*R*scale,nFlagY*(0.5*T+W)*scale);
						vertex_arr[2].Set(nFlagX*(R+N)*scale,nFlagY*(0.5*T+W)*scale);
						vertex_arr[3].Set(nFlagX*(R+N)*scale,nFlagY*0.5*T*scale);
					}
				}
				for(i=0;i<4;i++)
					coord_trans(vertex_arr[i],temp_ucs,TRUE);
				for(i=0;i<4;i++)
				{
					f2dPoint start=vertex_arr[i];
					f2dPoint end=vertex_arr[(i+1)%4];
					dc.MoveTo(ftoi(start.x),ftoi(start.y));
					dc.LineTo(ftoi(end.x),ftoi(end.y));
				}
				//尺寸标注
				pOldPen=dc.SelectObject(&purple_solid_pen);
				if(j==5)
				{	//标注S尺寸
					dim_start.Set(vertex_arr[3].x,vertex_arr[3].y);
					dim_end.Set(vertex_arr[3].x+S*scale,vertex_arr[3].y);
					DimText(&dc,dim_start,dim_end,S,-2);
					//标注N尺寸
					dim_start.Set(vertex_arr[2].x,vertex_arr[2].y);
					dim_end.Set(vertex_arr[2].x,vertex_arr[2].y+N*scale);
					DimText(&dc,dim_start,dim_end,N,1);
				}
				else if(j==6)
				{	//标注R尺寸
					dim_start.Set(0,(0.5*T+W)*scale);
					dim_end.Set(R*scale,(0.5*T+W)*scale);
					coord_trans(dim_start,temp_ucs,TRUE);
					coord_trans(dim_end,temp_ucs,TRUE);
					DimText(&dc,dim_start,dim_end,R,-2,ftoi(0.5*H*scale)+10);
				}
				dc.SelectObject(pOldPen);
			}
		}
		//绘制螺栓
		pOldPen=dc.SelectObject(&red_solid_pen);
		int nCount=m_listBolt.GetItemCount();
		for(i=0;i<nCount;i++)
		{
			char sText[100]="";
			DWORD dwFlag=m_listBolt.GetItemData(i);	//螺栓标志位
			WORD h=*((WORD*)&dwFlag);
			BOOL bHighLight=m_listBolt.GetItemState(i,LVNI_ALL|LVNI_SELECTED);
			m_listBolt.GetItemText(i,0,sText,100);
			int nBoltR=ftoi(atoi(sText+1)*0.5*scale);
			f3dPoint pos;
			m_listBolt.GetItemText(i,1,sText,100);
			pos.x=atof(sText);
			m_listBolt.GetItemText(i,2,sText,100);
			pos.y=atof(sText);
			m_listBolt.GetItemText(i,3,sText,100);
			pos.z=atof(sText);
			if(m_paramPlate.m_iParamType==TYPE_XEND&&h>=100)
				continue;	//仅生成十字插板XY面螺栓
			f2dPoint curPoint(pos.x*scale,pos.y*scale);
			if(m_paramPlate.m_iParamType==TYPE_XEND&&pos.y>0)
			{	//临时坐标系
				UCS_2dSTRU temp_ucs=draw_ucs;
				temp_ucs.origin.y-=2*N*scale;
				coord_trans(curPoint,temp_ucs,TRUE);
				topLeft.x=ftoi(curPoint.x)-nBoltR;
				topLeft.y=ftoi(curPoint.y)-nBoltR;
				bottomRight.x=ftoi(curPoint.x)+nBoltR;
				bottomRight.y=ftoi(curPoint.y)+nBoltR;
				if(bHighLight)
				{	//当前选中的螺栓
					pOldPen=dc.SelectObject(&green_solid_pen);
					dc.Ellipse(topLeft.x,topLeft.y,bottomRight.x,bottomRight.y);
					dc.SelectObject(pOldPen);
				}
				else
					dc.Ellipse(topLeft.x,topLeft.y,bottomRight.x,bottomRight.y);
			}
			curPoint.Set(pos.x*scale,pos.y*scale);	//重新赋值
			coord_trans(curPoint,draw_ucs,TRUE);
			topLeft.x=ftoi(curPoint.x)-nBoltR;
			topLeft.y=ftoi(curPoint.y)-nBoltR;
			bottomRight.x=ftoi(curPoint.x)+nBoltR;
			bottomRight.y=ftoi(curPoint.y)+nBoltR;
			if(bHighLight)
			{	//当前选中的螺栓
				pOldPen=dc.SelectObject(&green_solid_pen);
				dc.Ellipse(topLeft.x,topLeft.y,bottomRight.x,bottomRight.y);
				dc.SelectObject(pOldPen);
			}
			else
				dc.Ellipse(topLeft.x,topLeft.y,bottomRight.x,bottomRight.y);
		}
		dc.SelectObject(pOldPen);
	}
	dc.SelectObject(pOldPen);
}
//更新钢管正负头信息
void CDesignTubeEndLjDlg::UpdateTubeOddment()
{	//自动计算钢管正负头
	if(m_iTubeEndLjStyle==0)
	{	//平面切割
		f3dPoint curPos,tube_len_vec;
		if(m_bStartEnd)
		{
			curPos=m_pCurLineTube->Start();
			tube_len_vec=m_pCurLineTube->Start()-m_pCurLineTube->End();	//终端==>始端
		}
		else
		{
			curPos=m_pCurLineTube->End();
			tube_len_vec=m_pCurLineTube->End()-m_pCurLineTube->Start();	//始端==>终端
		}
		normalize(tube_len_vec);
		if(!cutPlaneNorm.IsZero()&&tube_len_vec*cutPlaneNorm>0)
		{	//如果为平面切割自动计算一个最小的负头
			f3dPoint inters,temp_vec;
			Int3dlf(inters,curPos,tube_len_vec,cutPlaneOrigin,cutPlaneNorm);
			double min_dist=DISTANCE(inters,curPos);
			double angle=cal_angle_of_2vec(tube_len_vec,cutPlaneNorm);
			double tana=fabs(tan(angle));
			temp_vec=curPos-inters;
			normalize(temp_vec);
			if(temp_vec*tube_len_vec>0)	//需要设置负头
				min_dist=-(min_dist-(m_pCurLineTube->GetDiameter()*0.5*tana));
			else
				min_dist+=(m_pCurLineTube->GetDiameter()*0.5*tana);
			setupInfo.m_fOddment=ftoi(min_dist);
		}
	}
	else if(m_iTubeEndLjStyle==1)
	{	//钢管相贯
		if(m_bFaceToFace)
			setupInfo.m_fOddment=0;	//对接相贯正负头为0
		else 
		{	//非对接相贯时计算钢管正负头 wht 11-05-10
			double min_dist=0;
			double fSamplingLength=2.5;
			int n = ftoi(Pi*m_pCurLineTube->GetDiameter()/fSamplingLength);
			int uSlices=max(n,8);
			uSlices=min(n,288);
			double piece_angle=2*Pi/uSlices;
			double radius=m_pCurLineTube->GetDiameter()*0.5;
			f3dPoint line_pick_start=m_pCurLineTube->End()+m_pCurLineTube->ucs.axis_x*radius;
			f3dPoint line_vec=m_pCurLineTube->End()-m_pCurLineTube->Start();
			f3dPoint end_pos=m_pCurLineTube->End();
			if(m_bStartEnd)
			{
				line_pick_start=m_pCurLineTube->Start()+m_pCurLineTube->ucs.axis_x*radius;
				line_vec=m_pCurLineTube->Start()-m_pCurLineTube->End();
				end_pos=m_pCurLineTube->Start();
			}
			normalize(line_vec);
			//分别根据两根相贯基准钢管计算当前钢管负头 
			long hanld_arr[2]={m_hDatumTube1,m_hDatumTube2};
			for(int j=0;j<2;j++)
			{
				CLDSLineTube *pTransTube=(CLDSLineTube*)console.FromPartHandle(hanld_arr[j],CLS_LINETUBE);
				if(pTransTube==NULL)
					continue;
				double trans_radius=pTransTube->GetDiameter()*0.5;
				for(int i=0;i<uSlices;i++)
				{
					f3dPoint line_pick=line_pick_start;
					if(i>0)
						rotate_point_around_axis(line_pick,piece_angle*i,m_pCurLineTube->Start(),m_pCurLineTube->End());
					f3dPoint inters1,inters2;
					if(Int3dlc(&inters1,&inters2,line_pick,line_vec,pTransTube->Start(),pTransTube->End(),trans_radius,trans_radius)>0)
					{
						f3dPoint vec1=inters1-end_pos;
						f3dPoint vec2=inters2-end_pos;
						double dd1=vec1*line_vec;
						double dd2=vec2*line_vec;
						if(min_dist>=0)
							min_dist=dd1<dd2?dd1:dd2;
						else 
						{
							if(dd1<dd2&&dd1<0&&dd1>min_dist)
								min_dist=dd1;	//第一个交点有效
							else if(dd1>dd2&&dd2<0&&dd2>min_dist)
								min_dist=dd2;	//第二个交点有效
						}
					}
				}
			}
			//按5圆整再+10mm加工误差 wht 11-05-10
			if(min_dist<0)
				setupInfo.m_fOddment=min(0,(ftoi(min_dist)/5)*5+10);
		}
	}
	else if(m_iTubeEndLjStyle==2)
	{	//更新法兰正负头 wht 11-01-23
		/* 因目前该对话框中显示正负头值已改为至法兰端面的正负头,法兰板厚发生修改时已不影响正负头显示值,故此注释掉以下代码 wjh-2015.3.14
		double fOddmentOffset=0;
		if(GetPartType()==0)		//普通法兰
			m_paramPlate.GetDesignItemValue('H',&fOddmentOffset);
		else if(GetPartType()==1)	//平焊法兰
			fOddmentOffset=m_pFlPParam->H1;
		else if(GetPartType()==2)	//对焊法兰
			fOddmentOffset=m_pFlDParam->H;
		else if(GetPartType()==3)
			fOddmentOffset=m_pRigidFlParam->fH;
		else if(GetPartType()==4)
			fOddmentOffset=m_pFlexibleFlParam->fH;
		*/
		if(m_bStartEnd)
			setupInfo.m_fOddment=m_pCurLineTube->startOdd(true);//-fOddmentOffset;
		else
			setupInfo.m_fOddment=m_pCurLineTube->endOdd(true);//-fOddmentOffset;		
	}
	else if(m_iTubeEndLjStyle>2)
	{
		DESIGNODDMENT * pCurOddment=&(m_pCurLineTube->desEndOdd);
		if(m_bStartEnd)
			pCurOddment=&(m_pCurLineTube->desStartOdd);
		//不应该自动判断钢管碰撞基准杆件,某些情况用户可能不希望自动计算正负头 wht 11-05-09
		//long hNodeFather=m_pBaseNode->hFatherPart;
		//if(pCurOddment->m_hRefPart1<0x20&&hNodeFather!=m_pCurLineTube->handle)
		//	pCurOddment->m_hRefPart1=hNodeFather;
		if(pCurOddment->m_iOddCalStyle==0)	//根据碰撞计算正负头
			setupInfo.m_fOddment=m_pCurLineTube->CalOddment(m_bStartEnd,&m_paramPlate);
	}
	CString sOddment="";
	sOddment.Format("%f",setupInfo.m_fOddment);
	SimplifiedNumString(sOddment);
	m_propList.SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("m_fOddment"),sOddment);
}

void CDesignTubeEndLjDlg::OnDelCurEndlj() 
{
	if(MessageBox( "需要立即删除端连接(板)吗(Y/N)?","LatticeTower",MB_YESNO)!=IDYES)
		return;
	if(m_pParamPlate)
	{
		BOLTSET boltSet;
		for(CLsRef *pLsRef=m_pParamPlate->GetFirstLsRef();pLsRef;pLsRef=m_pParamPlate->GetNextLsRef())
			boltSet.append(pLsRef->GetLsPtr());
		for(CLDSBolt *pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
			Ta.DelBoltAndRelativeBoltRef(pBolt->handle);
		Ta.DelTubeEndLj(m_pParamPlate->handle);
		//g_pSolidDraw->BuildDisplayList();
		g_pSolidDraw->Draw();
	}
	if(m_iTubeEndLjStyle==1)
	{	//钢管相贯
		m_hDatumTube1=m_hDatumTube2=0;
		m_propList.SetItemPropValue(GetPropID("m_hDatumTube1"),CString("0X0"));
		if(!m_bFaceToFace)
			m_propList.SetItemPropValue(GetPropID("m_hDatumTube2"),CString("0X0"));
		else
			m_bFaceToFace=FALSE;
	}
	else if(m_iTubeEndLjStyle==0)
	{	//钢管相贯
		m_hWeldLinkObj=0;
		m_propList.SetItemPropValue(GetPropID("m_hWeldLinkObj"),CString("0X0"));
	}
	else
	{
		if(m_bStartEnd)
		{
			m_pCurLineTube->m_tJointStart.hLinkObj=0;
			m_pCurLineTube->m_tJointStart.hViceLinkObj=0;
		}
		else
		{
			m_pCurLineTube->m_tJointEnd.hLinkObj=0;
			m_pCurLineTube->m_tJointEnd.hViceLinkObj=0;
		}
	}
	m_pParamPlate = NULL;
	m_bHasExistEndLj=FALSE;
	m_propList.SetItemReadOnly(GetPropID("m_fOddment"),FALSE);
	m_pInsertPlateParam=NULL;
	m_pFlDParam=m_pFlDParam=NULL;
	m_pFlexibleFlParam=m_pRigidFlParam=NULL;
	GetDlgItem(IDC_BTN_DEL_CUR_ENDLJ)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_PARTS_DB)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CMB_TUBE_ENDLJ_STYLE)->EnableWindow(!m_bSpecEndLjStyle&&!m_bHasExistEndLj);
	//
	UpdateDesignParam();
	UpdateLayoutBoltPara();
	DisplayPropertyList();
	UpdateSketchMap();
}
//根据零件库中的参数化板或选中的参数化板提取信息
void CDesignTubeEndLjDlg::UpdateDesignParamBySrcPlate(CLDSParamPlate* pSrcParamPlate)
{
	if(pSrcParamPlate==NULL)
		return;
	//获取标准构件设计参数
	CLDSStdPartModel stdPartModel;
	if(pSrcParamPlate->m_bStdPart&&pSrcParamPlate->GetStdPartModel(&stdPartModel))
	{
		if(pSrcParamPlate->m_iParamType==TYPE_FLG)
			m_pRigidFlParam=FindFlGParam(pSrcParamPlate->GetPartNo());
		else if(pSrcParamPlate->m_iParamType==TYPE_FLR) 
			m_pFlexibleFlParam=FindFlRParam(pSrcParamPlate->GetPartNo());
		else if(pSrcParamPlate->m_iParamType==TYPE_FLD)
			m_pFlDParam=FindFlDParam(pSrcParamPlate->GetPartNo());
		else if(pSrcParamPlate->m_iParamType==TYPE_FLP)
			m_pFlPParam=FindFlPParam(pSrcParamPlate->GetPartNo());
		else
		{	
			m_iStdPartLevel=stdPartModel.param.insert_plate_param.iLevelMark;
			if(pSrcParamPlate->m_iParamType==TYPE_ROLLEND)
				m_pInsertPlateParam=FindRollEndParam(pSrcParamPlate->GetPartNo());
			else if(pSrcParamPlate->m_iParamType==TYPE_UEND)
				m_pInsertPlateParam=FindUEndParam(pSrcParamPlate->GetPartNo());
			else if(pSrcParamPlate->m_iParamType==TYPE_XEND)
				m_pInsertPlateParam=FindXEndParam(pSrcParamPlate->GetPartNo());
		}
	}
	//
	m_paramPlate.CopyProperty(pSrcParamPlate);
	if(m_iTubeEndLjStyle==2&&pSrcParamPlate->m_iParamType==TYPE_FL)	//普通法兰
	{	//获取螺栓信息
		long J=0;
		pSrcParamPlate->GetDesignItemValue('J',&J);
		LayoutBoltPara.m_iStartEdge=J;
		LayoutBoltPara.m_nBoltNum=pSrcParamPlate->GetLsCount();
		CLsRef *pFirstLsRef=pSrcParamPlate->GetFirstLsRef();
		if(pFirstLsRef)
		{
			double fLayoutR=DistOf3dPtLine(pFirstLsRef->GetLsPtr()->ucs.origin,
				pSrcParamPlate->ucs.origin,pSrcParamPlate->ucs.origin+pSrcParamPlate->ucs.axis_z*100);
			LayoutBoltPara.m_nBoltD=pFirstLsRef->GetLsPtr()->get_d();	//获取螺栓直径
			LayoutBoltPara.m_fBoltLayoutD=fLayoutR*2;
			//获取首螺栓偏移角度(即取所有螺栓中最小的偏移角度)
			LayoutBoltPara.m_fOffsetAngle=pFirstLsRef->GetLsPtr()->des_base_pos.offset_angle;
			for(CLsRef *pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
			{
				if(pLsRef->GetLsPtr()->des_base_pos.R<=0)
					continue;
				if(LayoutBoltPara.m_fOffsetAngle > pLsRef->GetLsPtr()->des_base_pos.offset_angle)
					LayoutBoltPara.m_fOffsetAngle = pLsRef->GetLsPtr()->des_base_pos.offset_angle;
			}
			LayoutBoltPara.m_fOffsetAngle*=DEGTORAD_COEF;	//转换为角度制
		}
		//获取附加肋板信息
		m_bCreateAttachPlate=FALSE;
		CLDSParamPlate *pAttachPlate=NULL;
		for(ATTACHPART_ITEM *pItem=pSrcParamPlate->GetFirstAttachPart();pItem;pItem=pSrcParamPlate->GetNextAttachPart())
		{
			if(pItem->idClsType==CLS_PARAMPLATE)
			{
				pAttachPlate=(CLDSParamPlate*)console.FromPartHandle(pItem->h,CLS_PARAMPLATE);
				break;
			}
		}
		if(pAttachPlate)
		{
			m_bCreateAttachPlate=TRUE;
			strcpy(AttachPlatePara.sPartNo,pAttachPlate->GetPartNo());
			AttachPlatePara.cMaterial=pAttachPlate->cMaterial;
			AttachPlatePara.m_nThick=pAttachPlate->thick;
			AttachPlatePara.m_nPlateNum=LayoutBoltPara.m_nBoltNum;
			CLDSParamPlate::RIBPLATE_ELBOW elbow;
			elbow.LoadParams(pAttachPlate);//需要重新审定elbow.LoadParams对下面代码的替代性 wjh-2018.3.12
			AttachPlatePara.A=elbow.A.ToDouble();
			AttachPlatePara.B=elbow.B.ToDouble();
			AttachPlatePara.C=elbow.C.ToDouble();
			AttachPlatePara.D=elbow.D.ToDouble();
			AttachPlatePara.E=elbow.E.ToDouble();
			AttachPlatePara.F=elbow.F.ToDouble();
			AttachPlatePara.G=elbow.G.ToDouble();
			AttachPlatePara.H=elbow.H.ToDouble();
			//pAttachPlate->GetDesignItemValue('A', &AttachPlatePara.A);
			//pAttachPlate->GetDesignItemValue('B', &AttachPlatePara.B);
			//pAttachPlate->GetDesignItemValue('C', &AttachPlatePara.C);
			//pAttachPlate->GetDesignItemValue('D', &AttachPlatePara.D);
			//pAttachPlate->GetDesignItemValue('E', &AttachPlatePara.E);
			//pAttachPlate->GetDesignItemValue('F', &AttachPlatePara.F);
			//pAttachPlate->GetDesignItemValue('G', &AttachPlatePara.G);
			//pAttachPlate->GetDesignItemValue('H', &AttachPlatePara.H);
			//根据螺栓偏移角度计算,首肋板偏移角度
			AttachPlatePara.m_fAngle=LayoutBoltPara.m_fOffsetAngle+180.0/AttachPlatePara.m_nPlateNum;
		}
	}
	else if(m_iTubeEndLjStyle==5)
	{	//立板、包板编号 wht 11-01-23
		ATTACHPART_ITEM *pItem=NULL;
		strcpy(sWeldPlatePartNo,"");
		strcpy(sWrapPlatePartNo,"");
		strcpy(sCodeName,"");
		double M=0,A=0;
		if(m_pInsertPlateParam)
		{
			M=m_pInsertPlateParam->M;
			A=m_pInsertPlateParam->A;
			strcpy(sWeldPlatePartNo,m_pInsertPlateParam->weldPlatePartNo);
			strcpy(sWrapPlatePartNo,m_pInsertPlateParam->wrapPlatePartNo);
			strcpy(sCodeName,m_pInsertPlateParam->codeName);
		}
		else
		{
			pSrcParamPlate->GetDesignItemValue('M',&M);
			pSrcParamPlate->GetDesignItemValue('A',&A);
			for(pItem=pSrcParamPlate->GetFirstAttachPart();pItem;pItem=pSrcParamPlate->GetNextAttachPart())
			{
				CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pItem->h,CLS_PLATE);
				if(pPlate==NULL)
					continue;
				f3dPoint plate_pos=pPlate->ucs.origin;
				//转换到十字插板坐标系下
				coord_trans(plate_pos,pSrcParamPlate->ucs,FALSE);
				if(plate_pos.x<EPS2&&fabs(plate_pos.z)>0&&strlen(sWeldPlatePartNo)==0)
					strcpy(sWeldPlatePartNo,pPlate->GetPartNo());	//立板编号
				else if(fabs(plate_pos.x-M-A)<EPS2&&strlen(sWrapPlatePartNo)==0)
					strcpy(sWrapPlatePartNo,pPlate->GetPartNo());
				if(strlen(sWeldPlatePartNo)>0&&strlen(sWrapPlatePartNo)>0)
					break;
			}
		}
	}
	//普通插板，提取插板上的螺栓布置参数
	if(m_iTubeEndLjStyle>2 && !pSrcParamPlate->m_bStdPart)
	{
		if(m_iTubeEndLjStyle<5)
		{	//U型插板|槽型插板
			LayoutBoltParaList.Empty();	//清空原有螺栓布置参数
			for(CLsRef* pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
			{
				CLDSBolt* pBolt=pLsRef->GetLsPtr();
				//
				LAYOUT_BOLT_PARA *pBoltPara=LayoutBoltParaList.append();
				pBoltPara->m_nBoltD=pBolt->get_d();
				pBoltPara->m_nBoltNum=1;
				pBoltPara->m_nColNum=1;
				pBoltPara->m_nRowNum=1;
				pBoltPara->m_fColSpace=0;
				pBoltPara->m_fRowSpace=0;
				pBoltPara->m_posFirstBolt.x=pBolt->des_base_pos.len_offset_dist;
				pBoltPara->m_posFirstBolt.y=pBolt->des_base_pos.wing_offset_dist;
				pBoltPara->m_iDirection=pBolt->des_work_norm.direction;	
				pBoltPara->m_fSpecialSpace=pBoltPara->m_fColSpace;	//特殊间距初始值应等于列距 
				pBoltPara->m_bStaggerLayout=FALSE;
				pBoltPara->m_bMirLayout=FALSE;
			}
		}
		else if(m_iTubeEndLjStyle==5)
		{	//十字型插板
			BOOL bStaggerLayout=FALSE;
			CLDSBolt *pBolt=NULL,*pXYBolt=NULL,*pXZBolt=NULL;
			for(CLsRef* pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
			{
				pBolt=pLsRef->GetLsPtr();
				if(pBolt->feature==11)
					pXYBolt=pBolt;
				if(pBolt->feature==31)
					pXZBolt=pBolt;
				if(pXYBolt && pXZBolt)
				{
					double fY1=pXYBolt->des_base_pos.wing_offset_dist;
					double fY2=pXZBolt->des_base_pos.wing_offset_dist;
					if(fabs(fY1-fY2)>1)
						bStaggerLayout=TRUE;
					break;
				}
			}
			//提取十字插板螺栓布置参数
			LayoutBoltParaList.Empty();	//清空原有螺栓布置参数
			double H=0,N=0,A=0,M=0,R=0;
			pSrcParamPlate->GetDesignItemValue('A',&A);
			pSrcParamPlate->GetDesignItemValue('M',&M);
			pSrcParamPlate->GetDesignItemValue('R',&R);
			for(CLsRef* pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
			{
				pBolt=pLsRef->GetLsPtr();
				if(pBolt->feature!=11&&pBolt->feature!=12)
					continue;
				double fBoltPosX=pBolt->des_base_pos.len_offset_dist-A-M;
				double fBoltPosY=pBolt->des_base_pos.wing_offset_dist-R;
				//
				LAYOUT_BOLT_PARA *pBoltPara=LayoutBoltParaList.append();
				pBoltPara->m_nBoltD=pBolt->get_d();
				pBoltPara->m_nBoltNum=1;
				pBoltPara->m_nColNum=1;
				pBoltPara->m_nRowNum=1;
				pBoltPara->m_fColSpace=0;
				pBoltPara->m_fRowSpace=0;
				pBoltPara->m_posFirstBolt.x=fBoltPosX;
				pBoltPara->m_posFirstBolt.y=fBoltPosY;
				pBoltPara->m_bStaggerLayout=bStaggerLayout;
				pBoltPara->m_bMirLayout=FALSE;
			}
		}
	}
}
void CDesignTubeEndLjDlg::OnPartsDb() 
{
	CPartsDbDlg partsDlg;
	partsDlg.m_bRefreshDlgCtrl = TRUE;
	if(m_iTubeEndLjStyle==2)
		partsDlg.m_bIncParamFl = TRUE;
	else if(m_iTubeEndLjStyle==3)
		partsDlg.m_bIncParamUend = TRUE;
	else if(m_iTubeEndLjStyle==4)
		partsDlg.m_bIncParamRollEnd = TRUE;
	else if(m_iTubeEndLjStyle==5)
		partsDlg.m_bIncParamXEnd = TRUE;
	if(partsDlg.DoModal()==IDOK)
	{
		m_pSrcParamPlate=(CLDSParamPlate*)partsDlg.m_pSelectPart;
		if(!m_pSrcParamPlate)
			return;
		if(m_pSrcParamPlate->m_bStdPart&&(m_pSrcParamPlate->IsFL()))
			return;
		CLDSStdPartModel stdPartModel;
		if(m_pSrcParamPlate->m_bStdPart&&!m_pSrcParamPlate->GetStdPartModel(&stdPartModel))
			return;
		m_iPartType=0;	//普通构件
		UpdateDesignParamBySrcPlate(m_pSrcParamPlate);
		setupInfo.m_fNormOffset=m_pSrcParamPlate->m_fNormOffset;//法向偏距
		if(m_pCurLineTube)
			m_paramPlate.iSeg=m_pCurLineTube->iSeg;	//插板段号应该与基准钢管段号保持一致
		UpdateTubeOddment();	//更新钢管正负头
		UpdateLayoutBoltPara();
		DisplayPropertyList(m_ctrlPropGroup.GetCurSel());	//更新属性栏
		UpdateSketchMap(m_ctrlPropGroup.GetCurSel());		//更新图片
		Invalidate(FALSE);
	}
}

void CDesignTubeEndLjDlg::OnKeydownBoltList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_listBolt.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_listBolt.GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_UP)
		{	//按一次UP键选中项会上移一位，但是这个操作是在按下UP键时触发的
			//此时还并未移动，所以手动调整选中项的位置
			if(iCurSel>=1)
				iCurSel--;
			if(iCurSel>=0)
				m_listBolt.SetItemState(iCurSel,LVIS_SELECTED,LVIS_SELECTED);
		}
		else if(pLVKeyDow->wVKey==VK_UP||pLVKeyDow->wVKey==VK_DOWN)
		{	//按一次UP键选中项会下移一位，但是这个操作是在按下DOWN键时触发的
			//此时还并未移动，所以手动调整选中项的位置
			if(iCurSel<m_listBolt.GetItemCount())
				iCurSel++;
			if(iCurSel>=0)
				m_listBolt.SetItemState(iCurSel,LVIS_SELECTED,LVIS_SELECTED);
		}
		else if(pLVKeyDow->wVKey==VK_DELETE)
		{
			if(m_iPartType>0||m_iTubeEndLjStyle==2)
				return;	//法兰或标准构件螺栓不可在列表中删除
			m_listBolt.DeleteItem(iCurSel);
			pos = m_listBolt.GetFirstSelectedItemPosition();
		}
		else
			break;
	}
	Invalidate(FALSE);
	*pResult = 0;
}

//更新螺栓布置参数
void CDesignTubeEndLjDlg::UpdateLayoutBoltPara()
{
	if(m_iTubeEndLjStyle==2)
	{	//填充螺栓布置参数
		if((m_iPartType==1)&&m_pFlPParam)//||m_iPartType==0
		{	//平焊法兰
			LayoutBoltPara.m_nBoltD=m_pFlPParam->nBoltD;
			LayoutBoltPara.m_nBoltNum=m_pFlPParam->nBoltNum;
			LayoutBoltPara.m_fBoltLayoutD=m_pFlPParam->K;
		}
		else if((m_iPartType==2)&&m_pFlDParam)//||m_iPartType==0
		{	//对焊法兰
			LayoutBoltPara.m_nBoltD=m_pFlDParam->nBoltD;
			LayoutBoltPara.m_nBoltNum=m_pFlDParam->nBoltNum;
			LayoutBoltPara.m_fBoltLayoutD=m_pFlDParam->K;
		}
		else if(m_iPartType==3&&m_pRigidFlParam)
		{	//刚性法兰
			LayoutBoltPara.m_nBoltD=m_pRigidFlParam->nBoltD;
			LayoutBoltPara.m_nBoltNum=m_pRigidFlParam->nBoltNum;
			LayoutBoltPara.m_fBoltLayoutD=m_pRigidFlParam->fBoltCirD;
		}
		else if(m_iPartType==4&&m_pFlexibleFlParam)
		{	//柔性法兰
			LayoutBoltPara.m_nBoltD=m_pFlexibleFlParam->nBoltD;
			LayoutBoltPara.m_nBoltNum=m_pFlexibleFlParam->nBoltNum;
			LayoutBoltPara.m_fBoltLayoutD=m_pFlexibleFlParam->fBoltCirD;
		}
	}
	else
	{	//插板
		if(m_iPartType>0&&m_pInsertPlateParam)
		{	//标准插板
			LayoutBoltParaList.Empty();	//清空原有螺栓布置参数
			LayoutBoltPara.m_nBoltD=m_pInsertPlateParam->ls_param[0].iBoltD;
			for(int i=0;i<m_pInsertPlateParam->iBoltRect;i++)
			{
				CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA *pBoltPara=LayoutBoltParaList.append();
				pBoltPara->m_nBoltD=m_pInsertPlateParam->ls_param[i].iBoltD;
				pBoltPara->m_nBoltNum=m_pInsertPlateParam->ls_param[i].iBoltNum;
				pBoltPara->m_nColNum=m_pInsertPlateParam->ls_param[i].iColumn;
				pBoltPara->m_nRowNum=m_pInsertPlateParam->ls_param[i].iRow;
				pBoltPara->m_fColSpace=m_pInsertPlateParam->ls_param[i].fHorzSpace;
				pBoltPara->m_fRowSpace=m_pInsertPlateParam->ls_param[i].fVertSpace;
				pBoltPara->m_posFirstBolt.x=m_pInsertPlateParam->ls_param[i].fBoltPosX;
				pBoltPara->m_posFirstBolt.y=m_pInsertPlateParam->ls_param[i].fBoltPosY;
				pBoltPara->m_fSpecialSpace=pBoltPara->m_fColSpace;	//特殊间距初始值应等于列距 
				if(m_pInsertPlateParam->ls_param[i].iColumn==-1
					||m_pInsertPlateParam->ls_param[i].iRow==-1)
					pBoltPara->m_bStaggerLayout=TRUE;
				else
					pBoltPara->m_bStaggerLayout=FALSE;
				pBoltPara->m_bMirLayout=FALSE;
			}
		}
	}
	//刷新螺栓参数列表
	RefreshBoltParamList();
	RefreshBoltList();	
	Invalidate(FALSE);
}

//判断是否已经存在端连接
BOOL CDesignTubeEndLjDlg::IsHasExistEndLj()
{
	return (m_pParamPlate&&m_bHasExistEndLj);
}

//得到构件类型
int CDesignTubeEndLjDlg::GetPartType() const
{
	if(m_iTubeEndLjStyle==2)
	{	//端头法兰连接
		if(m_iPartType==1&&m_pFlPParam)
			return 1;
		else if(m_iPartType==2&&m_pFlDParam)
			return 2;
		else if(m_iPartType==3&&m_pRigidFlParam)
			return 3;
		else if(m_iPartType==4&&m_pFlexibleFlParam)
			return 4;
	}
	else if(m_iTubeEndLjStyle>2)
	{	//端头插板连接
		if(m_iPartType==1&&m_pInsertPlateParam)
			return 1;
	}
	return 0;	//普通法兰或插板连接
}
//设置构件类型
void CDesignTubeEndLjDlg::SetPartType(int iPartType)
{
	if(iPartType!=0&&(iPartType>4&&iPartType<1))
		iPartType=0;
	m_iPartType=iPartType;
}

void CDesignTubeEndLjDlg::OnClickBoltList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	Invalidate(FALSE);
	*pResult = 0;
}

void CDesignTubeEndLjDlg::DesignLineAngle(CLDSLineAngle* pRayJg,CLDSPlate* pNodePlate)
{
	CDesignLjPartPara* pLinePartPara=pNodePlate->designInfo.partList.Add(pRayJg->handle);
	pLinePartPara->hPart=pRayJg->handle;
	pLinePartPara->m_nClassTypeId=CLS_LINEANGLE;
	pLinePartPara->iFaceNo=1;
	pLinePartPara->angle.bEndLjJg=0;
	//计算角钢正负头坐标
	double fNormOffset=-0.5*pNodePlate->GetThick();
	int x_wing0_y_wing1=0;
	BOOL bInsideJg=IsInsideJg(pRayJg,pNodePlate->ucs.axis_z,&x_wing0_y_wing1);
	int iLinkToStart0End1=1;
	if(pRayJg->pStart==m_pBaseNode)
	{
		iLinkToStart0End1=0;
		pRayJg->desStartOdd.m_hRefPart1=pNodePlate->designInfo.m_hBasePart;
		pRayJg->desStartOdd.m_fCollideDist=g_sysPara.VertexDist;
		pRayJg->CalStartOddment();
		if(x_wing0_y_wing1==0)
		{
			pRayJg->desStartPos.wing_x_offset.gStyle=4;
			pRayJg->desStartPos.wing_x_offset.offsetDist=fNormOffset;
		}
		else
		{
			pRayJg->desStartPos.wing_y_offset.gStyle=4;
			pRayJg->desStartPos.wing_y_offset.offsetDist=fNormOffset;
		}
		pRayJg->CalPosition();
	}
	else if(pRayJg->pEnd==m_pBaseNode)
	{
		iLinkToStart0End1=1;
		pRayJg->desEndOdd.m_hRefPart1=pNodePlate->designInfo.m_hBasePart;
		pRayJg->desEndOdd.m_fCollideDist=g_sysPara.VertexDist;
		pRayJg->CalEndOddment();
		if(x_wing0_y_wing1==0)
		{
			pRayJg->desEndPos.wing_x_offset.gStyle=4;
			pRayJg->desEndPos.wing_x_offset.offsetDist=fNormOffset;
		}
		else
		{
			pRayJg->desEndPos.wing_y_offset.gStyle=4;
			pRayJg->desEndPos.wing_y_offset.offsetDist=fNormOffset;
		}
		pRayJg->CalPosition();
	}
	pLinePartPara->start0_end1=iLinkToStart0End1;
	//布置角钢螺栓
	CLayAngleBoltDlg dlg;
	dlg.m_pNode=m_pBaseNode;
	dlg.m_hPlate=pNodePlate->handle;
	dlg.m_pLinePart=pRayJg;
	dlg.m_xiPreferSegI=pNodePlate->iSeg;//pNodePlate->iSeg.ToString();
	dlg.viewNorm=pNodePlate->ucs.axis_z;
	dlg.m_bMirLayLs = FALSE;
	dlg.m_bRayEndFaceOffset=FALSE;
	dlg.m_bTwoEdgeProfile=FALSE;
	if(bInsideJg)	//里铁
		dlg.m_tBoltNormOffset.AddThick(-pRayJg->handle,TRUE);
	else
		dlg.m_tBoltNormOffset.AddThick(-pNodePlate->handle,TRUE);
	dlg.m_iBoltNorm=x_wing0_y_wing1;
	pLinePartPara->angle.cur_wing_x0_y1=(BYTE)x_wing0_y_wing1;
	if(iLinkToStart0End1==0)
	{
		dlg.m_iOddCalStyle=pRayJg->desStartOdd.m_iOddCalStyle;
		dlg.m_fOddment=pRayJg->startOdd();
		dlg.m_sExtraOdd.Format("%.0f",pRayJg->GetStartExtraOdd(dlg.m_iBoltNorm));
		dlg.m_nLsNum=pRayJg->BoltsNumOn1Wing(TRUE);
		dlg.m_iBoltRows=pRayJg->connectStart.rows-1;
		dlg.m_sBoltSize.Format("%d",pRayJg->connectStart.d);
	}
	else if(iLinkToStart0End1==1)
	{
		dlg.m_iOddCalStyle=pRayJg->desEndOdd.m_iOddCalStyle;
		dlg.m_fOddment=pRayJg->endOdd();
		dlg.m_sExtraOdd.Format("%.0f",pRayJg->GetEndExtraOdd(dlg.m_iBoltNorm));
		dlg.m_nLsNum=pRayJg->BoltsNumOn1Wing(FALSE);
		dlg.m_iBoltRows=pRayJg->connectEnd.rows-1;
		dlg.m_sBoltSize.Format("%d",pRayJg->connectEnd.d);
	}
	ATOM_LIST<CDesignLsPara>boltList;
	dlg.m_pLsList=&boltList;
	dlg.m_bIncPlateProfilePara=TRUE;
	if(dlg.DoModal()==IDOK)
	{
		if(iLinkToStart0End1==0)	//始端连接
		{
			if(x_wing0_y_wing1==0)
			{
				pRayJg->desStartPos.wing_x_offset.gStyle=4;
				pRayJg->desStartPos.wing_x_offset.offsetDist=dlg.m_fAngleEndNormOffset;
			}
			else
			{
				pRayJg->desStartPos.wing_y_offset.gStyle=4;
				pRayJg->desStartPos.wing_y_offset.offsetDist=dlg.m_fAngleEndNormOffset;
			}
		}
		else	//终端连接
		{
			if(x_wing0_y_wing1==0)
			{
				pRayJg->desEndPos.wing_x_offset.gStyle=4;
				pRayJg->desEndPos.wing_x_offset.offsetDist=dlg.m_fAngleEndNormOffset;
			}
			else
			{
				pRayJg->desEndPos.wing_y_offset.gStyle=4;
				pRayJg->desEndPos.wing_y_offset.offsetDist=dlg.m_fAngleEndNormOffset;
			}
		}
		pRayJg->CalPosition();
		//
		pLinePartPara->angle.cbSpaceFlag=dlg.cbSpaceFlag;
		pLinePartPara->ber_space=dlg.m_fBerSpace;
		pLinePartPara->wing_space=dlg.m_fWingSpace;
		pLinePartPara->end_space=dlg.m_fPlateEdgeSpace;
		pLinePartPara->angle.bTwoEdge=dlg.m_bTwoEdgeProfile;
		if(pLinePartPara->start0_end1==0)
		{
			pRayJg->desStartOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
			pRayJg->SetStartOdd(dlg.m_fOddment);
		}
		else if(pLinePartPara->start0_end1==1)
		{
			pRayJg->desEndOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
			pRayJg->SetEndOdd(dlg.m_fOddment);
		}
		//int iInitRayNo=1;
		ATOM_LIST<RAYNO_RECORD>rayNoList;
		CObjNoGroup NoGroup(32);
		BOLTSET boltSet;
		for(CDesignLsPara *pBoltPara=boltList.GetFirst();pBoltPara;pBoltPara=boltList.GetNext())
		{
			CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg = pBoltPara->iSeg;
			pBoltPara->DesignLs(pBolt);
			pBolt->CopyModuleInstanceInfo(pRayJg);
			pBolt->AddL0Thick(pNodePlate->handle,TRUE);
			pBolt->AddL0Thick(pRayJg->handle,TRUE);
			if(pLinePartPara->start0_end1==0)
			{
				pRayJg->connectStart.d=atoi(dlg.m_sBoltSize);
				pRayJg->SetBoltsNumOn1Wing(dlg.m_nLsNum,TRUE);
				pRayJg->connectStart.rows=dlg.m_iBoltRows+1;
				pRayJg->AppendStartLsRef(pBolt->GetLsRef());
			}
			else if(pLinePartPara->start0_end1==1)
			{
				pRayJg->connectEnd.d=atoi(dlg.m_sBoltSize);
				pRayJg->SetBoltsNumOn1Wing(dlg.m_nLsNum,FALSE);
				pRayJg->connectEnd.rows=dlg.m_iBoltRows+1;
				pRayJg->AppendEndLsRef(pBolt->GetLsRef());
			}
			
			pNodePlate->AppendLsRef(pBolt->GetLsRef());
			pBolt->CalGuigeAuto();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
		if(pLinePartPara->start0_end1==0&&pRayJg->desStartOdd.m_iOddCalStyle==1)
		{
			pRayJg->CalStartOddment();
			pRayJg->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
		}
		else if(pLinePartPara->start0_end1==1&&pRayJg->desEndOdd.m_iOddCalStyle==1)
		{
			pRayJg->CalEndOddment();
			pRayJg->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
		}
	}
}
void CDesignTubeEndLjDlg::InitPlateBasePara(CLDSPlate* pNodePlate,CLDSLineTube *pDatumLineTube)
{
	static CPlateBasicParaDlg basicParaDlg;
	pNodePlate->Thick=basicParaDlg.m_nPlankThick=8;
	basicParaDlg.m_iMaterial=QuerySteelMatIndex(pDatumLineTube->cMaterial);
	basicParaDlg.m_sSegI=pDatumLineTube->iSeg.ToString();
	basicParaDlg.m_bAdjustWeldLen=FALSE;
	if(basicParaDlg.DoModal()==IDOK)
	{
		pNodePlate->cMaterial=CSteelMatLibrary::RecordAt(basicParaDlg.m_iMaterial).cBriefMark;
		pNodePlate->SetPartNo(basicParaDlg.m_sPartNo);
		pNodePlate->iSeg=SEGI(basicParaDlg.m_sSegI.GetBuffer());
		pNodePlate->Thick=basicParaDlg.m_nPlankThick;
		pNodePlate->m_fNormOffset=-0.5*basicParaDlg.m_nPlankThick;
		pNodePlate->designInfo.weld_width=basicParaDlg.m_nWeldWidth;
		strcpy(pNodePlate->layer(),pDatumLineTube->layer());
		pNodePlate->layer(1)='P';
		pNodePlate->cfgword=pDatumLineTube->cfgword;
		pNodePlate->m_hPartWeldParent=pDatumLineTube->handle;
	}
}
CLDSPlate *CDesignTubeEndLjDlg::CreateNodePlate(CLDSLineTube *pDatumLineTube,CLDSLineAngle* pRayLineAngle)
{
	if(pDatumLineTube==NULL || pRayLineAngle==NULL)
		return NULL;
	CLDSPlate *pNodePlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	//
	InitPlateBasePara(pNodePlate,pDatumLineTube);
	//设计信息
	pNodePlate->designInfo.iProfileStyle0123=1;
	pNodePlate->designInfo.m_bEnableFlexibleDesign=TRUE;
	pNodePlate->designInfo.m_hBaseNode=m_pBaseNode->handle;
	pNodePlate->designInfo.m_hBasePart=pDatumLineTube->handle;
	//基准点
	pNodePlate->designInfo.origin.datum_pos_style = 2;
	pNodePlate->designInfo.origin.des_para.RODNODE.hNode =m_pBaseNode->handle;
	pNodePlate->designInfo.origin.des_para.RODNODE.hRod  =pDatumLineTube->handle;
	//法线
	if(pDatumLineTube->pEnd==m_pBaseNode)
		pNodePlate->ucs.axis_y = pDatumLineTube->Start()-pDatumLineTube->End();
	else
		pNodePlate->ucs.axis_y = pDatumLineTube->End()-pDatumLineTube->Start();
	pNodePlate->ucs.axis_x = pRayLineAngle->End()-pRayLineAngle->Start();
	pNodePlate->ucs.axis_z = pNodePlate->ucs.axis_x^pNodePlate->ucs.axis_y;
	normalize(pNodePlate->ucs.axis_z);
	normalize(pNodePlate->ucs.axis_y);
	if(pNodePlate->ucs.axis_z*console.GetActiveView()->ucs.axis_z<0)
		pNodePlate->ucs.axis_z*=-1.0;
	pNodePlate->designInfo.norm.norm_style=2;	//杆件交叉面法线
	pNodePlate->designInfo.norm.hVicePart=pDatumLineTube->handle;
	pNodePlate->designInfo.norm.hCrossPart=pRayLineAngle->handle;
	pNodePlate->designInfo.norm.vector=pNodePlate->ucs.axis_z;
	//连接杆件
	CDesignLjPartPara *pLjPart=NULL;
	pLjPart=pNodePlate->designInfo.partList.Add(pDatumLineTube->handle);
	pLjPart->m_nClassTypeId=CLS_LINETUBE;
	pLjPart->hPart=pDatumLineTube->handle;
	pLjPart->iFaceNo=1;
	pLjPart->start0_end1=2;
	//
	DesignLineAngle(pRayLineAngle,pNodePlate);
	//
	pNodePlate->DesignPlate();
	pNodePlate->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pNodePlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return pNodePlate;
}
//设计节点板
CLDSPlate *CDesignTubeEndLjDlg::CreateNodePlate(CLDSLineTube *pDatumLineTube,
												CLDSLineTube *pRayLineTube/*=NULL*/,
												CLDSParamPlate *pParamPlate/*=NULL*/,
												CLDSLineTube *pHorizontalTube/*=NULL*/)	//横材
{
	if(pParamPlate==NULL)
		pParamPlate=m_pParamPlate;
	if(pRayLineTube==NULL)
		pRayLineTube=m_pCurLineTube;
	if(pDatumLineTube==NULL||pRayLineTube==NULL||pParamPlate==NULL)
		return NULL;
	int start0_end1=(pRayLineTube->pStart==m_pBaseNode)?0:1;
	f3dPoint  ray_line_vec=(pRayLineTube->Start()-pRayLineTube->End()).normalized();
	if(start0_end1==0)
		ray_line_vec*=-1;
	f3dPoint base_line_vec;
	if(pDatumLineTube->pEnd==m_pBaseNode)
		base_line_vec=(pDatumLineTube->Start()-pDatumLineTube->End()).normalized();
	else
		base_line_vec=(pDatumLineTube->End()-pDatumLineTube->Start()).normalized();
	//
	CLDSPlate *pNodePlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	pNodePlate->iSeg=pParamPlate->iSeg;
	pNodePlate->cMaterial=NodePlatePara.cMaterial;
	pNodePlate->SetPartNo(NodePlatePara.sPartNo);
	pNodePlate->Thick=NodePlatePara.m_nThick;
	strcpy(pNodePlate->layer(),pParamPlate->layer());
	pNodePlate->layer(1)='P';
	pNodePlate->cfgword=pParamPlate->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	pNodePlate->m_hPartWeldParent=pDatumLineTube->handle;
	pNodePlate->m_fNormOffset=-0.5*NodePlatePara.m_nThick;	//钢板法线偏移量
	//pNodePlate->cfgword=m_pBaseNode->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	//
	pNodePlate->designInfo.iProfileStyle0123=1;	//定制外形
	pNodePlate->designInfo.m_bEnableFlexibleDesign=TRUE;//不启用连板柔性设计
	pNodePlate->designInfo.m_hBaseNode = m_pBaseNode->handle;
	pNodePlate->designInfo.m_hBasePart = pDatumLineTube->handle;
	//基准点
	pNodePlate->designInfo.origin.datum_pos_style = 2;
	pNodePlate->designInfo.origin.des_para.RODNODE.hNode =m_pBaseNode->handle;
	pNodePlate->designInfo.origin.des_para.RODNODE.hRod  =pDatumLineTube->handle;
	//法线
	pNodePlate->ucs.axis_y = base_line_vec;
	pNodePlate->ucs.axis_x = ray_line_vec;
	pNodePlate->ucs.axis_z = pNodePlate->ucs.axis_x^pNodePlate->ucs.axis_y;
	normalize(pNodePlate->ucs.axis_z);
	normalize(pNodePlate->ucs.axis_y);
	if(pNodePlate->ucs.axis_z*console.GetActiveView()->ucs.axis_z<0)
		pNodePlate->ucs.axis_z*=-1.0;
	pNodePlate->designInfo.norm.norm_style=2;	//杆件交叉面法线
	pNodePlate->designInfo.norm.hVicePart=pDatumLineTube->handle;
	pNodePlate->designInfo.norm.hCrossPart=pRayLineTube->handle;
	pNodePlate->designInfo.norm.nearVector = pNodePlate->ucs.axis_z;
	//连接杆件列表
	CDesignLjPartPara *pLjPart=NULL;
	pLjPart=pNodePlate->designInfo.partList.Add(pDatumLineTube->handle);
	pLjPart->iFaceNo=1;
	pLjPart->start0_end1=2;
	pLjPart->ber_space=0;
	pLjPart=pNodePlate->designInfo.partList.Add(pRayLineTube->handle);
	pLjPart->iFaceNo=1;
	pLjPart->start0_end1=start0_end1;
	pLjPart->ber_space=0;
	//添加横材及横材关联插板上的关联螺栓 wht 11-06-07
	CLsRef *pLsRef=NULL;
	if(pHorizontalTube)
	{
		pLjPart=pNodePlate->designInfo.partList.Add(pHorizontalTube->handle);
		pLjPart->iFaceNo=1;
		if(pHorizontalTube->pStart==m_pBaseNode)
			start0_end1=0;
		else if(pHorizontalTube->pEnd==m_pBaseNode)
			start0_end1=1;
		pLjPart->start0_end1=start0_end1;
		pLjPart->ber_space=0;
		CLDSParamPlate *pPlate=pHorizontalTube->GetParamPlate(start0_end1);
		if(pPlate&&pPlate->m_iParamType==TYPE_XEND)
		{
			f3dPoint work_norm=pNodePlate->ucs.axis_z;
			CLDSPlate *pUpPlate=NULL,*pDownPlate=NULL,*pCurVertPlate=NULL;
			pPlate->GetVertPlateInXEndPlate(&pUpPlate,&pDownPlate);
			if(fabs(work_norm*pPlate->ucs.axis_z)<fabs(work_norm*pUpPlate->ucs.axis_z))
			{	//立板与节点板共面时，需调整立板的法线参数	 wxc-2018.3.17
				if(pUpPlate->ucs.axis_y*base_line_vec>pDownPlate->ucs.axis_y*base_line_vec)
					pCurVertPlate=pUpPlate;
				else
					pCurVertPlate=pDownPlate;
			}
			//
			for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			{	//跳过十字插板上不合理螺栓
				double L=0;
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				f3dPoint ls_pos=pBolt->ucs.origin;
				coord_trans(ls_pos,pPlate->ucs,FALSE);
				pPlate->GetDesignItemValue('L',&L);
				if(fabs(pBolt->get_norm()*pNodePlate->ucs.axis_z)<EPS_COS2)
					continue;
				if(ls_pos.x<L)
					continue;
				if(pCurVertPlate&&((pCurVertPlate==pUpPlate&&ls_pos.z<0)
					||(pCurVertPlate==pDownPlate&&ls_pos.z>0)))
					continue;
				pBolt->AddL0Thick(pNodePlate->handle,TRUE,TRUE);
				pBolt->SetModified();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->CalGuigeAuto();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				pNodePlate->AppendLsRef(*pLsRef);		
			}
			if(pCurVertPlate)
			{
				//调整立板法线与所在坡面法线方向一致
				pCurVertPlate->designInfo.norm=pNodePlate->designInfo.norm;
				pCurVertPlate->designInfo.norm.nearVector=pCurVertPlate->ucs.axis_z;
				//重新设计以立板定位的包板
				ATTACHPART_ITEM *pItem=NULL;
				for(pItem=pPlate->GetFirstAttachPart();pItem;pItem=pPlate->GetNextAttachPart())
				{
					CLDSPlate *pSonPlate=(CLDSPlate*)console.FromPartHandle(pItem->h,CLS_PLATE);
					if(pPlate==NULL)
						continue;
					if((pSonPlate->designInfo.norm.hVicePart==pCurVertPlate->handle
						&&pSonPlate->designInfo.norm.norm_style>0)||pSonPlate==pCurVertPlate)
					{	
						pSonPlate->DesignSetupUcs();
						pSonPlate->DesignPlate();
						pSonPlate->SetModified();
						pSonPlate->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pSonPlate->GetSolidPartObject());
					}
				}
				//重新设计参数板时将其附带的螺栓重新设计
				for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					CLDSBolt *pBolt=pLsRef->GetLsPtr();
					pBolt->correct_worknorm();
					pBolt->correct_pos();
					pBolt->CalGuigeAuto();
					pBolt->SetModified();
					pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
		}
		else if(pPlate)
		{
			for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				if(fabs(pBolt->get_norm()*pNodePlate->ucs.axis_z)<EPS_COS2)
					continue;
				pBolt->AddL0Thick(pNodePlate->handle,TRUE,TRUE);
				if(pBolt->des_base_pos.direction==0)	//法向偏移量
				{
					pBolt->des_base_pos.norm_offset.EmptyThick();
					pBolt->des_base_pos.norm_offset.AddThick(-pNodePlate->handle,TRUE);
				}
				pBolt->SetModified();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->CalGuigeAuto();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				pNodePlate->AppendLsRef(*pLsRef);
			}
		}
	}
	//添加螺栓引用
	if(pParamPlate->m_iParamType==TYPE_XEND)
	{
		for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
		{	//跳过十字插板上不合理螺栓
			double L=0;
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			f3dPoint ls_pos=pBolt->ucs.origin;
			coord_trans(ls_pos,pParamPlate->ucs,FALSE);
			pParamPlate->GetDesignItemValue('L',&L);
			if(fabs(pBolt->get_norm()*pNodePlate->ucs.axis_z)<EPS_COS2)
				continue;
			if(ls_pos.x<L)
				continue;
			pBolt->AddL0Thick(pNodePlate->handle,TRUE,TRUE);
			pBolt->SetModified();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			pNodePlate->AppendLsRef(*pLsRef);		
		}
	}
	else
	{
		for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(fabs(pBolt->get_norm()*pNodePlate->ucs.axis_z)<EPS_COS2)
				continue;
			pBolt->AddL0Thick(pNodePlate->handle,TRUE,TRUE);
			if(pBolt->des_base_pos.direction==0)	//法向偏移量
			{
				pBolt->des_base_pos.norm_offset.EmptyThick();
				pBolt->des_base_pos.norm_offset.AddThick(-pNodePlate->handle,TRUE);
			}
			pBolt->SetModified();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			pNodePlate->AppendLsRef(*pLsRef);
		}
	}
	//设计钢板
	pNodePlate->DesignPlate();
	pNodePlate->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pNodePlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return pNodePlate;
}

//完成选择对象的后续操作
void CDesignTubeEndLjDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	char sValueText[100];
	if(GetPropID("m_hDatumTube1")==GetEventPropId())
	{
		if(console.FromPartHandle(m_hPickObj,CLS_LINETUBE))
			m_hDatumTube1=m_hPickObj;
		UpdateTubeOddment(); //更新钢管正负头
		if(GetPropValueStr(GetPropID("m_hDatumTube1"),sValueText))
			m_propList.SetItemPropValue(GetPropID("m_hDatumTube1"),sValueText);
	}
	else if(GetPropID("m_hDatumTube2")==GetEventPropId())
	{
		if(console.FromPartHandle(m_hPickObj,CLS_LINETUBE))
			m_hDatumTube2=m_hPickObj;	
		UpdateTubeOddment(); //更新钢管正负头
		if(GetPropValueStr(GetPropID("m_hDatumTube2"),sValueText))
			m_propList.SetItemPropValue(GetPropID("m_hDatumTube2"),sValueText);
	}
	else if(GetPropID("m_hWeldLinkObj")==GetEventPropId())
	{	//根据焊接连接板，自动计算平面切割点和法向
		if(console.FromPartHandle(m_hPickObj))
			m_hWeldLinkObj=m_hPickObj;
		f3dPoint tube_vec=(m_pCurLineTube->End()-m_pCurLineTube->Start()).normalized();
		tube_vec*=m_bStartEnd?1:-1;
		CLDSGeneralPlate* pWeldPlate=(CLDSGeneralPlate*)Ta.FromPartHandle(m_hWeldLinkObj);
		double fCosa=pWeldPlate?pWeldPlate->ucs.axis_z*tube_vec:0;
		if(fabs(fCosa)>EPS_COS2)
		{
			cutPlaneNorm=pWeldPlate->ucs.axis_z;
			if(fCosa>0)
				cutPlaneNorm*=-1;
			cutPlaneOrigin=pWeldPlate->ucs.origin;
			if(fCosa>0)
				cutPlaneOrigin+=pWeldPlate->ucs.axis_z*pWeldPlate->Thick;
			UpdateTubeOddment();	//更新钢管正负头
		}
		if(GetPropValueStr(GetPropID("m_hWeldLinkObj"),sValueText))
			m_propList.SetItemPropValue(GetPropID("m_hWeldLinkObj"),sValueText);
		if(GetPropValueStr(GetPropID("cutPlaneOrigin.x"),sValueText))
			m_propList.SetItemPropValue(GetPropID("cutPlaneOrigin.x"),sValueText);
		if(GetPropValueStr(GetPropID("cutPlaneOrigin.y"),sValueText))
			m_propList.SetItemPropValue(GetPropID("cutPlaneOrigin.y"),sValueText);
		if(GetPropValueStr(GetPropID("cutPlaneOrigin.z"),sValueText))
			m_propList.SetItemPropValue(GetPropID("cutPlaneOrigin.z"),sValueText);
		if(GetPropValueStr(GetPropID("cutPlaneNorm.x"),sValueText))
			m_propList.SetItemPropValue(GetPropID("cutPlaneNorm.x"),sValueText);
		if(GetPropValueStr(GetPropID("cutPlaneNorm.y"),sValueText))
			m_propList.SetItemPropValue(GetPropID("cutPlaneNorm.y"),sValueText);
		if(GetPropValueStr(GetPropID("cutPlaneNorm.z"),sValueText))
			m_propList.SetItemPropValue(GetPropID("cutPlaneNorm.z"),sValueText);
	}
	else if(GetPropID("design_axis_z.hVicePart")==GetEventPropId())
	{
		if(console.FromPartHandle(m_hPickObj)==NULL)
			m_hPickObj=0;
		if(m_xSubPropList.m_nObjClassTypeId==0)
			setupInfo.workNorm.hVicePart=m_hPickObj;	
		else if(m_xSubPropList.m_nObjClassTypeId==1)
			setupInfo.workNormUpVert.hVicePart=m_hPickObj;	
		else if(m_xSubPropList.m_nObjClassTypeId==2)
			setupInfo.workNormDownVert.hVicePart=m_hPickObj;	
		if(GetPropValueStr(GetPropID("design_axis_z.hVicePart"),sValueText))
			m_xSubPropList.SetItemPropValue(GetPropID("design_axis_z.hVicePart"),sValueText);
	}
	else if(GetPropID("design_axis_z.hCrossPart")==GetEventPropId())
	{
		if(console.FromPartHandle(m_hPickObj,CLS_LINEPART)==NULL)
			m_hPickObj=0;
		if(m_xSubPropList.m_nObjClassTypeId==0)
			setupInfo.workNorm.hCrossPart=m_hPickObj;	
		else if(m_xSubPropList.m_nObjClassTypeId==1)
			setupInfo.workNormUpVert.hCrossPart=m_hPickObj;	
		else if(m_xSubPropList.m_nObjClassTypeId==2)
			setupInfo.workNormDownVert.hCrossPart=m_hPickObj;	
		if(GetPropValueStr(GetPropID("design_axis_z.hCrossPart"),sValueText))
			m_xSubPropList.SetItemPropValue(GetPropID("design_axis_z.hCrossPart"),sValueText);
	}
}
//设计十字插板填板,使用包钢代替包板时暂不支持设计填板(该情况一般不会出现) wht 11-05-10
void CDesignTubeEndLjDlg::DesignXEndFillPlate(CLDSParamPlate *pParamPlate)
{
	if(pParamPlate==NULL||pParamPlate->m_iParamType!=TYPE_XEND)
		return;
	if(FillPlatePara.nThick<=0)
		return;
	double S=0,N=0,L=0,M=0,A=0;
	pParamPlate->GetDesignItemValue('S',&S);
	pParamPlate->GetDesignItemValue('N',&N);
	pParamPlate->GetDesignItemValue('L',&L);
	pParamPlate->GetDesignItemValue('M',&M);
	pParamPlate->GetDesignItemValue('A',&A);
	CLsRef *pLsRef=NULL;
	for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
		(*pLsRef)->feature=0;	//初始化螺栓feature=0,用来标记螺栓是否已调整过法向偏移量
	ATTACHPART_ITEM *pItem=NULL;
	int nCount=0;	//填板个数
	CLDSPlate *plate_ptr_arr[4]={NULL};	//记录填板指针，最后添加到十字插板附加板链表中
	for(pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
	{
		CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pItem->h,CLS_PLATE);
		if(pPlate==NULL)
			continue;
		if(fabs(pPlate->ucs.axis_z*pParamPlate->ucs.axis_z)<EPS_COS)
			continue;	//法线与基板法线不同
		double fLen=pPlate->GetLength();
		double fWidth=pPlate->GetWidth();
		if(!(fabs(fLen-S)<EPS&&fabs(fWidth-N)<EPS)
			&&!(fabs(fWidth-S)<EPS&&fabs(fLen-N)<EPS))
			continue;	//跳过长宽与包板长宽不同的附加板
		//1.生成填板
		plate_ptr_arr[nCount]=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		plate_ptr_arr[nCount]->CopyProperty(pPlate);
		plate_ptr_arr[nCount]->cMaterial=FillPlatePara.cMat;		//材质
		plate_ptr_arr[nCount]->SetPartNo(FillPlatePara.sPartNo);	//编号
		plate_ptr_arr[nCount]->Thick=FillPlatePara.nThick;			//厚度
		if(pPlate->m_fNormOffset<0)							//法向偏移量
			plate_ptr_arr[nCount]->m_fNormOffset=-FillPlatePara.nThick;
		//定位基准点
		plate_ptr_arr[nCount]->designInfo.origin=pPlate->designInfo.origin;
		//法线设计参数
		plate_ptr_arr[nCount]->designInfo.norm=pPlate->designInfo.norm;
		//坐标系
		plate_ptr_arr[nCount]->ucs=pPlate->ucs;
		//填充钢板轮廓点
		plate_ptr_arr[nCount]->DesignSetupUcs();
		plate_ptr_arr[nCount]->vertex_list.append(PROFILE_VERTEX(0,0,0));
		plate_ptr_arr[nCount]->vertex_list.append(PROFILE_VERTEX(L-M-A,0,0));
		plate_ptr_arr[nCount]->vertex_list.append(PROFILE_VERTEX(L-M-A,N,0));
		plate_ptr_arr[nCount]->vertex_list.append(PROFILE_VERTEX(0,N,0));
		plate_ptr_arr[nCount]->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(plate_ptr_arr[nCount]->GetSolidPartObject());
		//2.调整包板的位置
		if(pPlate->m_fNormOffset<0)
			pPlate->m_fNormOffset-=FillPlatePara.nThick;
		else 
			pPlate->m_fNormOffset+=FillPlatePara.nThick;
		pPlate->SetModified();
		pPlate->Create3dSolidModel(); //调整包板位置后重新生成实体
		g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		//
		CLsRef *pLsRef=NULL;
		for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			//3.调整螺栓的法向偏移量
			if(pBolt->feature==0)
			{
				pBolt->des_base_pos.norm_offset.AddThick(-plate_ptr_arr[nCount]->handle,TRUE);
				pBolt->feature=1;	//表示螺栓已设置过法向偏移量
			}
			f3dPoint pos=pBolt->ucs.origin;
			coord_trans(pos,pParamPlate->ucs,FALSE);
			if(pos.x<L)
			{	//4.自动引入相关螺栓并更新螺栓通厚
				plate_ptr_arr[nCount]->AppendLsRef(pBolt->GetLsRef());
				pBolt->AddL0Thick(plate_ptr_arr[nCount]->handle,TRUE,TRUE);
			}
			pBolt->CalGuigeAuto();
			pBolt->correct_pos();
			pBolt->SetModified();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
	}
	//5.将填板添加到十字插板附加板中
	for(int i=0;i<nCount;i++)
		pParamPlate->AppendAttachPart(plate_ptr_arr[i]);
	g_pSolidDraw->Draw();
}
BOOL  CDesignTubeEndLjDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	CXhChar200 helpStr;
	if(m_iTubeEndLjStyle==0)
		sprintf(helpStr,"%s::/命令参考/menu_cmd_list.htm#平面切割",theApp.m_pszHelpFilePath);
	else if(m_iTubeEndLjStyle==1)
		sprintf(helpStr,"%s::/命令参考/menu_cmd_list.htm#钢管相贯",theApp.m_pszHelpFilePath);
	else if(m_iTubeEndLjStyle==3)
		sprintf(helpStr,"%s::/命令参考/menu_cmd_list.htm#U型插板",theApp.m_pszHelpFilePath);
	else if(m_iTubeEndLjStyle==4)
		sprintf(helpStr,"%s::/命令参考/menu_cmd_list.htm#槽型插板",theApp.m_pszHelpFilePath);
	else if(m_iTubeEndLjStyle==5)
		sprintf(helpStr,"%s::/命令参考/menu_cmd_list.htm#十字型插板",theApp.m_pszHelpFilePath);
	else
	{
		sprintf(helpStr,"%s::/命令参考/menu_cmd_list.htm",theApp.m_pszHelpFilePath);
		AfxMessageBox("暂缺少该属性的帮助主题，请直接与软件供应商联系！");
	}
	HtmlHelp((DWORD_PTR)(char*)helpStr,HH_DISPLAY_TOPIC);
	return TRUE;
}

void CDesignTubeEndLjDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);

	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
#endif
