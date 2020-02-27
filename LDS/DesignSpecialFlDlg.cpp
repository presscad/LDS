// DesignSpecialFlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "DesignSpecialFlDlg.h"
#include "XhCharString.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "SortFunc.h"
#include "PropertyListOper.h"
#include "DesignVectorPropDlg.h"
#include "LicFuncDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDesignSpecialFlDlg dialog

int CDesignSpecialFlDlg::nScrLocationX=0;
int CDesignSpecialFlDlg::nScrLocationY=0;
CDesignSpecialFlDlg::CDesignSpecialFlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignSpecialFlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignSpecialFlDlg)
	//}}AFX_DATA_INIT
	m_arrPropGroupLabel.RemoveAll();
	m_pBaseNode=NULL;
	m_pCurLineTube=NULL;//当前钢管
	m_listBolt.AddColumnHeader("直径");
	m_listBolt.AddColumnHeader("半径R");
	m_listBolt.AddColumnHeader("偏移角");
	//
	m_listPlate.AddColumnHeader("编号");
	m_listPlate.AddColumnHeader("偏移角");
	m_listPlate.AddColumnHeader("类型");
	//
	m_nThick=20;
	m_iNormOffsetType=0;	//上平面
	m_fNormOffset=-20;
	axis_z.Set(0,0,-1);
	H=10;
	//
	m_fInnerRadius=0;
	m_fExternalRadius=0;
	m_fOddment=0;
	for(int i=0;i<4;i++)
	{
		m_pRayLinePart[i]=NULL;
		m_pNodePlate[i]=NULL;
		m_pCoverPlate[i]=NULL;
	}
	m_pCirPlate=NULL;
	m_pLinePartSet=NULL;
	m_bCreateRibPlate=FALSE;	//用来控制是否生成肋板	wht 11-05-26
	m_bCutDatumTube=TRUE;
}

void CDesignSpecialFlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignSpecialFlDlg)
	DDX_Control(pDX, IDC_PLATE_LIST, m_listPlate);
	DDX_Control(pDX, IDC_BOLT_LIST, m_listBolt);
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDesignSpecialFlDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignSpecialFlDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnSelchangeTabGroup)
	ON_BN_CLICKED(ID_CONFIRM_CREATE, OnConfirmCreate)
	ON_WM_PAINT()
	ON_NOTIFY(LVN_KEYDOWN, IDC_BOLT_LIST, OnKeydownBoltList)
	ON_NOTIFY(NM_CLICK, IDC_BOLT_LIST, OnClickBoltList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_PLATE_LIST, OnKeydownPlateList)
	ON_NOTIFY(NM_CLICK, IDC_PLATE_LIST, OnClickPlateList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PLATE_LIST, OnEndlabeleditList)
	//}}AFX_MSG_MAP
	ON_WM_MOVE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignSpecialFlDlg message handlers
static BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CItemInfo *lpInfo=NULL;
	CPropTreeItem *pFindItem=NULL,*pPropItem=NULL;
	CDesignSpecialFlDlg *pSpecialFlDlg=(CDesignSpecialFlDlg*)pPropList->GetParent();
	CXhChar100 tem_str;
	tem_str.Printf("%s",valueStr);
	CString sTempValue="";
	char sTextValue[100]="";
	BOOL bUpdateLayoutPara=FALSE;
	CPropertyListOper<CDesignSpecialFlDlg> oper(pPropList,pSpecialFlDlg);
	//基本信息
	if(CDesignSpecialFlDlg::GetPropID("sPartNo")==pItem->m_idProp)
	{
		pSpecialFlDlg->m_sPartNo.Copy(tem_str);
		ParsePartNo(pSpecialFlDlg->m_sPartNo,&pSpecialFlDlg->iSeg,NULL);
		sTempValue.Format("%s",(char*)pSpecialFlDlg->iSeg.ToString());
		pPropList->SetItemPropValue(CDesignSpecialFlDlg::GetPropID("iSeg"),sTempValue);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(valueStr,BOMPART::PLATE);
		if(pPart)
		{
			pSpecialFlDlg->m_cMaterial=pPart->cMaterial;
			pSpecialFlDlg->m_nThick=(int)pPart->thick;
			//
			CXhChar50 sValue("%.0f",pPart->thick);
			pPropList->SetItemPropValue(CDesignSpecialFlDlg::GetPropID("m_nPlateThick"),(char*)sValue);
			QuerySteelMatMark(pPart->cMaterial,sValue);
			pPropList->SetItemPropValue(CDesignSpecialFlDlg::GetPropID("cMaterial"),(char*)sValue);
		}
#endif
	}
	else if(CDesignSpecialFlDlg::GetPropID("iSeg")==pItem->m_idProp)
		pSpecialFlDlg->iSeg=SEGI(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("cMaterial")==pItem->m_idProp)
		pSpecialFlDlg->m_cMaterial=QuerySteelBriefMatMark(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("m_nPlateThick")==pItem->m_idProp)
	{
		pSpecialFlDlg->m_nThick=atoi(tem_str);
		pSpecialFlDlg->H=0.5*atoi(tem_str);
		pSpecialFlDlg->UpdateItemPropValue("H");
		if(pSpecialFlDlg->m_iNormOffsetType==0)		//0.外端面
			pSpecialFlDlg->m_fNormOffset=-pSpecialFlDlg->m_nThick;
		else if(pSpecialFlDlg->m_iNormOffsetType==1)	//1.内端面
			pSpecialFlDlg->m_fNormOffset=0;
		pSpecialFlDlg->UpdateItemPropValue("PlateNormOffset");
		pSpecialFlDlg->UpdateTubeOddment();
		pSpecialFlDlg->UpdateItemPropValue("m_fOddment");
	}
	else if(CDesignSpecialFlDlg::GetPropID("m_fInnerRadius")==pItem->m_idProp)
	{
		pSpecialFlDlg->m_fInnerRadius=atof(tem_str)*0.5;
		pSpecialFlDlg->ElbowPlatePara.B=ftoi(pSpecialFlDlg->m_fExternalRadius-pSpecialFlDlg->m_fInnerRadius);
		sTempValue.Format("%.f",pSpecialFlDlg->ElbowPlatePara.B);
		pPropList->SetItemPropValue(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.B"),sTempValue);
	}
	else if(CDesignSpecialFlDlg::GetPropID("m_fExternalRadius")==pItem->m_idProp)
	{
		pSpecialFlDlg->m_fExternalRadius=atof(tem_str)*0.5;
		pSpecialFlDlg->ElbowPlatePara.B=ftoi(pSpecialFlDlg->m_fExternalRadius-pSpecialFlDlg->m_fInnerRadius);
		sTempValue.Format("%.f",pSpecialFlDlg->ElbowPlatePara.B);
		pPropList->SetItemPropValue(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.B"),sTempValue);
	}
	//装配参数
	else if(CDesignSpecialFlDlg::GetPropID("m_bCutDatumTube")==pItem->m_idProp)
	{
		if(stricmp(tem_str,"是")==0)
			pSpecialFlDlg->m_bCutDatumTube=TRUE;
		else
			pSpecialFlDlg->m_bCutDatumTube=FALSE;
		pPropList->DeleteAllSonItems(pItem);
		if(pSpecialFlDlg->m_bCutDatumTube)
		{
			oper.InsertEditPropItem(pItem,"H","","",-1,TRUE);
			oper.InsertBtnEditPropItem(pItem,"m_fOddment","","",-1,TRUE);
		}
	}
	else if(CDesignSpecialFlDlg::GetPropID("H")==pItem->m_idProp)
		pSpecialFlDlg->H=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("m_fOddment")==pItem->m_idProp)
		pSpecialFlDlg->m_fOddment=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("m_iNormOffsetType")==pItem->m_idProp)
	{
		pSpecialFlDlg->m_iNormOffsetType=tem_str.At(0)-'0';
		if(pSpecialFlDlg->m_iNormOffsetType==0)			//0.外端面
			pSpecialFlDlg->m_fNormOffset=-pSpecialFlDlg->m_nThick;
		else if(pSpecialFlDlg->m_iNormOffsetType==1)	//1.内端面
			pSpecialFlDlg->m_fNormOffset=0;
		pSpecialFlDlg->UpdateItemPropValue("PlateNormOffset");
		pSpecialFlDlg->UpdateTubeOddment();
		pSpecialFlDlg->UpdateItemPropValue("m_fOddment");
	}
	else if(CDesignSpecialFlDlg::GetPropID("PlateNormOffset")==pItem->m_idProp)
	{
		pSpecialFlDlg->m_fNormOffset=atof(tem_str);
		pSpecialFlDlg->UpdateTubeOddment();
		pSpecialFlDlg->UpdateItemPropValue("m_fOddment");
	}
	else if(CDesignSpecialFlDlg::GetPropID("PlateNorm.x")==pItem->m_idProp)
		pSpecialFlDlg->axis_z.x=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("PlateNorm.y")==pItem->m_idProp)
		pSpecialFlDlg->axis_z.y=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("PlateNorm.z")==pItem->m_idProp)
		pSpecialFlDlg->axis_z.z=atof(tem_str);
	//肘板布置参数
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.A")==pItem->m_idProp)
		pSpecialFlDlg->ElbowPlatePara.A=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.B")==pItem->m_idProp)
		pSpecialFlDlg->ElbowPlatePara.B=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.C")==pItem->m_idProp)
		pSpecialFlDlg->ElbowPlatePara.C=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.D")==pItem->m_idProp)
		pSpecialFlDlg->ElbowPlatePara.D=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.E")==pItem->m_idProp)
		pSpecialFlDlg->ElbowPlatePara.E=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.F")==pItem->m_idProp)
		pSpecialFlDlg->ElbowPlatePara.F=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.G")==pItem->m_idProp)
		pSpecialFlDlg->ElbowPlatePara.G=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.H")==pItem->m_idProp)
	{
		pSpecialFlDlg->ElbowPlatePara.H=atof(tem_str);
		pPropList->DeleteAllSonItems(pItem);
		if(pSpecialFlDlg->ElbowPlatePara.H>0)
		{
			oper.InsertEditPropItem(pItem,"ElbowPlatePara.F","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"ElbowPlatePara.G","","",-1,TRUE);
		}
	}
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.m_iCutMode")==pItem->m_idProp)
		pSpecialFlDlg->ElbowPlatePara.m_iCutMode=tem_str[0]-'0';
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.cMaterial")==pItem->m_idProp)
		pSpecialFlDlg->ElbowPlatePara.cMaterial=QuerySteelBriefMatMark(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.sPartNo")==pItem->m_idProp)
	{
		bUpdateLayoutPara=TRUE;	//更新螺栓、肘板布置参数
		strncpy(pSpecialFlDlg->ElbowPlatePara.sPartNo,tem_str,16);
	}
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.m_fAngle")==pItem->m_idProp)
	{
		bUpdateLayoutPara=TRUE;	//更新螺栓、肘板布置参数
		pSpecialFlDlg->ElbowPlatePara.m_fAngle=atof(tem_str);
	}
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.m_nPlateNum")==pItem->m_idProp)
	{
		bUpdateLayoutPara=TRUE;	//更新螺栓、肘板布置参数
		pSpecialFlDlg->ElbowPlatePara.m_nPlateNum=atoi(tem_str);
	}
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.m_nThick")==pItem->m_idProp)
		pSpecialFlDlg->ElbowPlatePara.m_nThick=atoi(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.m_bObverse")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pSpecialFlDlg->ElbowPlatePara.m_bObverse=TRUE;
		else 
			pSpecialFlDlg->ElbowPlatePara.m_bObverse=FALSE;
	}
	else if(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.m_bVertical")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pSpecialFlDlg->ElbowPlatePara.m_bVertical=TRUE;
		else 
			pSpecialFlDlg->ElbowPlatePara.m_bVertical=FALSE;
	}
	//螺栓布置参数
	else if(CDesignSpecialFlDlg::GetPropID("m_nBoltNum")==pItem->m_idProp)
	{
		bUpdateLayoutPara=TRUE;	//更新螺栓、肘板布置参数
		pSpecialFlDlg->LayoutBoltPara.m_nBoltNum=atoi(tem_str);
		pSpecialFlDlg->ElbowPlatePara.m_nPlateNum=pSpecialFlDlg->LayoutBoltPara.m_nBoltNum;
		//TODO:更新显示 
	}
	else if(CDesignSpecialFlDlg::GetPropID("m_nBoltD")==pItem->m_idProp)
	{
		pSpecialFlDlg->LayoutBoltPara.m_nBoltD=atoi(tem_str);
		pSpecialFlDlg->LayoutBolt();	//重新布置螺栓
	}
	else if(CDesignSpecialFlDlg::GetPropID("m_iDriection")==pItem->m_idProp)
		pSpecialFlDlg->LayoutBoltPara.m_iDirection=tem_str[0]-'0';
	else if(CDesignSpecialFlDlg::GetPropID("m_iBoltSegI")==pItem->m_idProp)
		pSpecialFlDlg->LayoutBoltPara.m_iBoltSegI=atoi(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("m_iStartEdge")==pItem->m_idProp)
	{
		bUpdateLayoutPara=TRUE;	//更新螺栓、肘板布置参数
		pSpecialFlDlg->LayoutBoltPara.m_iStartEdge=tem_str[0]-'0';
	}
	else if(CDesignSpecialFlDlg::GetPropID("m_fOffsetAngle")==pItem->m_idProp)
	{
		bUpdateLayoutPara=TRUE;	//更新螺栓、肘板布置参数
		pSpecialFlDlg->LayoutBoltPara.m_fOffsetAngle=atof(tem_str);
		//螺栓数修改后更新法兰肘板数量以及首块肘板的偏移角度
		pSpecialFlDlg->ElbowPlatePara.m_fAngle=pSpecialFlDlg->LayoutBoltPara.m_fOffsetAngle
			+180.0/pSpecialFlDlg->ElbowPlatePara.m_nPlateNum;
		sTempValue.Format("%f",pSpecialFlDlg->ElbowPlatePara.m_fAngle);
		SimplifiedNumString(sTempValue);
		pPropList->SetItemPropValue(CDesignSpecialFlDlg::GetPropID("ElbowPlatePara.m_fAngle"),sTempValue);
	}
	else if(CDesignSpecialFlDlg::GetPropID("m_fBoltLayoutD")==pItem->m_idProp)
	{
		pSpecialFlDlg->LayoutBoltPara.m_fBoltLayoutR=atof(tem_str)*0.5;
		pSpecialFlDlg->LayoutBolt();	//重新布置螺栓
	}
	//封板参数
	else if(CDesignSpecialFlDlg::GetPropID("CoverPlatePara.cMaterial")==pItem->m_idProp)
		pSpecialFlDlg->CoverPlatePara.cMaterial=QuerySteelBriefMatMark(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("CoverPlatePara.sPartNo")==pItem->m_idProp)
		strncpy(pSpecialFlDlg->CoverPlatePara.sPartNo,tem_str,16);
	else if(CDesignSpecialFlDlg::GetPropID("CoverPlatePara.D")==pItem->m_idProp)
		pSpecialFlDlg->CoverPlatePara.D=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("CoverPlatePara.W")==pItem->m_idProp)
		pSpecialFlDlg->CoverPlatePara.W=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("CoverPlatePara.m_nThick")==pItem->m_idProp)
		pSpecialFlDlg->CoverPlatePara.m_nThick=atoi(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("CoverPlatePara.m_fAlfa")==pItem->m_idProp)
		pSpecialFlDlg->CoverPlatePara.m_fAlfa=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("CoverPlatePara.m_fBeta")==pItem->m_idProp)
		pSpecialFlDlg->CoverPlatePara.m_fBeta=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("CoverPlatePara.m_bUpPlate")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pSpecialFlDlg->CoverPlatePara.m_bUpPlate=TRUE;
		else
			pSpecialFlDlg->CoverPlatePara.m_bUpPlate=FALSE;
	}
	else if(CDesignSpecialFlDlg::GetPropID("CoverPlatePara.m_bDownPlate")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pSpecialFlDlg->CoverPlatePara.m_bDownPlate=TRUE;
		else
			pSpecialFlDlg->CoverPlatePara.m_bDownPlate=FALSE;
	}
	else if(CDesignSpecialFlDlg::GetPropID("CoverPlatePara.biHoleType")==pItem->m_idProp)
	{
		pSpecialFlDlg->CoverPlatePara.biHoleType=valueStr[0]-'0';
		pSpecialFlDlg->InitWaterleakHolePara();
		pSpecialFlDlg->UpdateWaterHolePropItem(pItem);
	}
	else if(CDesignSpecialFlDlg::GetPropID("CoverPlatePara.BH")==pItem->m_idProp)
	{
		double fValue=atof(valueStr);
		if(fValue<EPS)
		{
			AfxMessageBox("流水孔直径不能为0，请重新输入!");
			return FALSE;
		}
		else
			pSpecialFlDlg->CoverPlatePara.BH=fValue;
	}
	else if(CDesignSpecialFlDlg::GetPropID("hRelaPart1.R")==pItem->m_idProp)
		pSpecialFlDlg->CoverPlatePara.uiAttribute1=ftoi(atof(valueStr)*10);
	else if(CDesignSpecialFlDlg::GetPropID("hRelaPart1.BG")==pItem->m_idProp)
	{
		WORD* pw_BGorBS=(WORD*)&(pSpecialFlDlg->CoverPlatePara.uiAttribute1);
		*pw_BGorBS	  =atoi(valueStr);	
	}
	else if(CDesignSpecialFlDlg::GetPropID("hRelaPart1.BS")==pItem->m_idProp)
	{
	
		WORD* pw_BGorBS=(WORD*)&(pSpecialFlDlg->CoverPlatePara.uiAttribute1);
		*(pw_BGorBS+1)=atoi(valueStr);
	}
	else if(CDesignSpecialFlDlg::GetPropID("hRelaPart2.R")==pItem->m_idProp)
		pSpecialFlDlg->CoverPlatePara.uiAttribute2=ftoi(atof(valueStr)*10);
	else if(CDesignSpecialFlDlg::GetPropID("hRelaPart2.BG")==pItem->m_idProp)
	{
		WORD* pw_BGorBS=(WORD*)&(pSpecialFlDlg->CoverPlatePara.uiAttribute2);
		*pw_BGorBS	  =atoi(valueStr);	
	}
	else if(CDesignSpecialFlDlg::GetPropID("hRelaPart2.BS")==pItem->m_idProp)
	{

		WORD* pw_BGorBS=(WORD*)&(pSpecialFlDlg->CoverPlatePara.uiAttribute2);
		*(pw_BGorBS+1)=atoi(valueStr);
	}
	//肋板
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.cMaterial")==pItem->m_idProp)
		pSpecialFlDlg->RibPlatePara.cMaterial=QuerySteelBriefMatMark(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.sPartNo")==pItem->m_idProp)
		strncpy(pSpecialFlDlg->RibPlatePara.sPartNo,tem_str,16);
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.m_nThick")==pItem->m_idProp)
		pSpecialFlDlg->RibPlatePara.m_nThick=atoi(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.m_bUseH")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pSpecialFlDlg->RibPlatePara.m_bUseH=TRUE;
		else
			pSpecialFlDlg->RibPlatePara.m_bUseH=FALSE;
		//
		pPropList->DeleteItemByPropId(CDesignSpecialFlDlg::GetPropID("RibPlatePara.H"));
		pPropList->DeleteItemByPropId(CDesignSpecialFlDlg::GetPropID("RibPlatePara.D"));
		pFindItem=pPropList->FindItemByPropId(CDesignSpecialFlDlg::GetPropID("RibPlatePara"),NULL);
		if(pFindItem)
		{
			if(pSpecialFlDlg->RibPlatePara.m_bUseH)
				oper.InsertEditPropItem(pFindItem,"RibPlatePara.H","","",-1,TRUE);
			else
				oper.InsertEditPropItem(pFindItem,"RibPlatePara.D","","",-1,TRUE);
		}
	}
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.D")==pItem->m_idProp)
		pSpecialFlDlg->RibPlatePara.D=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.H")==pItem->m_idProp)
		pSpecialFlDlg->RibPlatePara.H=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.N")==pItem->m_idProp)
		pSpecialFlDlg->RibPlatePara.N=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.W")==pItem->m_idProp)
		pSpecialFlDlg->RibPlatePara.W=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.S")==pItem->m_idProp)
	{
		pSpecialFlDlg->RibPlatePara.S=atof(tem_str);
		pSpecialFlDlg->UpdateRibPlateWN();	//更新肋板参数W、N
	}
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.m_bTopCir")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pSpecialFlDlg->RibPlatePara.m_bTopCir=TRUE;
		else
			pSpecialFlDlg->RibPlatePara.m_bTopCir=FALSE;
		//更新示意图
		CStatic *pPic=(CStatic*)pSpecialFlDlg->GetDlgItem(IDC_S_PIC_PLATE_TYPE);
		CBitmap bmpPlate;
		bmpPlate.Detach();
		if(pSpecialFlDlg->RibPlatePara.m_bTopCir)
			bmpPlate.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE2);
		else
			bmpPlate.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE);
		pPic->SetBitmap(bmpPlate);
		//
		pPropList->DeleteItemByPropId(CDesignSpecialFlDlg::GetPropID("RibPlatePara.m_bUseH"));
		pPropList->DeleteItemByPropId(CDesignSpecialFlDlg::GetPropID("RibPlatePara.S"));
		pPropList->DeleteItemByPropId(CDesignSpecialFlDlg::GetPropID("RibPlatePara.W"));
		pPropList->DeleteItemByPropId(CDesignSpecialFlDlg::GetPropID("RibPlatePara.N"));
		pPropList->DeleteItemByPropId(CDesignSpecialFlDlg::GetPropID("RibPlatePara.H"));
		pPropList->DeleteItemByPropId(CDesignSpecialFlDlg::GetPropID("RibPlatePara.D"));
		pFindItem=pPropList->FindItemByPropId(CDesignSpecialFlDlg::GetPropID("RibPlatePara"),NULL);
		if(pFindItem)
		{
			if(!pSpecialFlDlg->RibPlatePara.m_bTopCir)
				oper.InsertCmbListPropItem(pFindItem,"RibPlatePara.m_bUseH","","","",-1,TRUE);
			//肋板偏移量S
			oper.InsertEditPropItem(pFindItem,"RibPlatePara.S","","",-1,TRUE);
			//封板宽度W
			oper.InsertEditPropItem(pFindItem,"RibPlatePara.W","","",-1,TRUE);
			if(!pSpecialFlDlg->RibPlatePara.m_bTopCir)
			{	
				//参数N
				oper.InsertEditPropItem(pFindItem,"RibPlatePara.N","","",-1,TRUE);
				if(pSpecialFlDlg->RibPlatePara.m_bUseH)
				{	//参数H
					oper.InsertEditPropItem(pFindItem,"RibPlatePara.H","","",-1,TRUE);
				}
				else
				{	//参数D
					oper.InsertEditPropItem(pFindItem,"RibPlatePara.D","","",-1,TRUE);
				}
			}
		}
	}
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.m_bOutterRibPlate")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pSpecialFlDlg->RibPlatePara.m_bOutterRibPlate=TRUE;
		else
			pSpecialFlDlg->RibPlatePara.m_bOutterRibPlate=FALSE;
	}
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.m_bMirCreate")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("是")==0)
			pSpecialFlDlg->RibPlatePara.m_bMirCreate=TRUE;
		else
			pSpecialFlDlg->RibPlatePara.m_bMirCreate=FALSE;
	}
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.norm.x")==pItem->m_idProp)
		pSpecialFlDlg->RibPlatePara.norm.x=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.norm.y")==pItem->m_idProp)
		pSpecialFlDlg->RibPlatePara.norm.y=atof(tem_str);
	else if(CDesignSpecialFlDlg::GetPropID("RibPlatePara.norm.z")==pItem->m_idProp)
		pSpecialFlDlg->RibPlatePara.norm.z=atof(tem_str);
	if(bUpdateLayoutPara)
	{	//更新螺栓、肘板布置参数
		pSpecialFlDlg->LayoutBolt();	
		pSpecialFlDlg->LayoutAttachPlate();
	}
	pSpecialFlDlg->ReDraw();
	return TRUE;
}

static BOOL ButtonClickItem(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	char valueStr[200]="";
	CPropTreeItem *pPropItem=NULL,*pFindItem=NULL;
	CDesignSpecialFlDlg *pSpecialFlDlg=(CDesignSpecialFlDlg*)pPropList->GetParent();
	if(pItem->m_idProp==CDesignSpecialFlDlg::GetPropID("m_fOddment"))
	{
		pSpecialFlDlg->UpdateTubeOddment();
		pSpecialFlDlg->ReDraw();
	}
	else if(pItem->m_idProp==CDesignSpecialFlDlg::GetPropID("PlateNorm"))
	{
		CDesignVectorPropDlg desvecDlg;
		desvecDlg.m_xVector.vector=pSpecialFlDlg->axis_z;
		if(desvecDlg.DoModal()==IDOK)
		{
			desvecDlg.m_xVector.UpdateVector(console.GetActiveModel());
			pSpecialFlDlg->axis_z=desvecDlg.m_xVector.vector;
		}
	}
	g_pSolidDraw->Draw();
	return TRUE;
}

const DWORD HASHTABLESIZE = 500;
IMPLEMENT_PROP_FUNC(CDesignSpecialFlDlg);
void CDesignSpecialFlDlg::InitPropHashtable()
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);
	//基本信息
	AddPropItem("PlateBasicInfo",PROPLIST_ITEM(id++,"基本信息"));
	AddPropItem("sPartNo",PROPLIST_ITEM(id++,"编号","环向板编号"));
	AddPropItem("iSeg",PROPLIST_ITEM(id++,"段号","环向板段号"));
	AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质","环向板材质",MakeMaterialMarkSetString()));
	AddPropItem("m_nPlateThick",PROPLIST_ITEM(id++,"板厚"));
	AddPropItem("m_fInnerRadius",PROPLIST_ITEM(id++,"内圆直径B","环向板内圆半径"));
	AddPropItem("m_fExternalRadius",PROPLIST_ITEM(id++,"外圆直径W","环向板外圆半径"));
	//装配参数
	AddPropItem("SetUpPara",PROPLIST_ITEM(id++,"装配参数","装配参数"));
	AddPropItem("m_iNormOffsetType",PROPLIST_ITEM(id++,"节点位置","基准节点在底板的位置","0.上平面|1.下平面|2.指定偏移"));
	AddPropItem("PlateNormOffset",PROPLIST_ITEM(id++,"偏移量","内端面(底面)相对于基准节点沿底板法线正向的偏移量"));
	AddPropItem("PlateNorm",PROPLIST_ITEM(id++,"法线方向","环向板法线方向"));
	AddPropItem("PlateNorm.x",PROPLIST_ITEM(id++,"X坐标","环向板法线方向X方向坐标分量"));
	AddPropItem("PlateNorm.y",PROPLIST_ITEM(id++,"Y坐标","环向板法线方向Y方向坐标分量"));
	AddPropItem("PlateNorm.z",PROPLIST_ITEM(id++,"Z坐标","环向板法线方向Z方向坐标分量"));
	AddPropItem("m_bCutDatumTube",PROPLIST_ITEM(id++,"基准钢管平面切割","","是|否"));
	AddPropItem("H",PROPLIST_ITEM(id++,"参数H","钢管端与环向板底面之间的距离"));
	AddPropItem("m_fOddment",PROPLIST_ITEM(id++,"终端正负头"));
	//螺栓布置参数
	AddPropItem("LayputBoltParam",PROPLIST_ITEM(id++,"螺栓布置参数","螺栓布置参数"));
	AddPropItem("m_nBoltD",PROPLIST_ITEM(id++,"螺栓直径","螺栓直径","12|16|20|24|26|30"));
	AddPropItem("m_nBoltNum",PROPLIST_ITEM(id++,"螺栓个数","螺栓个数"));
	AddPropItem("m_iDriection",PROPLIST_ITEM(id++,"螺栓朝向","螺栓朝向","0.朝外|1.朝内"));
	AddPropItem("m_iBoltSegI",PROPLIST_ITEM(id++,"螺栓段号","螺栓段号"));
	AddPropItem("m_iStartEdge",PROPLIST_ITEM(id++,"起始边","布置法兰螺栓时的起始边,右侧示意图中,绿色三角标志位基准边位置，黑色三角标注为焊道线位置","0.焊道线|1.基准边"));
	AddPropItem("m_fOffsetAngle",PROPLIST_ITEM(id++,"首螺栓偏移角","首螺栓相对于起始边的偏移角"));
	AddPropItem("m_fBoltLayoutD",PROPLIST_ITEM(id++,"螺栓所在圆直径","螺栓所在圆直径"));
	AddPropItem("tube_D",PROPLIST_ITEM(id++,"钢管直径","钢管直径"));
	//肘板参数
	AddPropItem("ElbowPlateProfile",PROPLIST_ITEM(id++,"外形参数"));
	AddPropItem("ElbowPlatePara.A",PROPLIST_ITEM(id++,"竖直高度A"));
	AddPropItem("ElbowPlatePara.B",PROPLIST_ITEM(id++,"底边宽度B"));
	AddPropItem("ElbowPlatePara.C",PROPLIST_ITEM(id++,"台阶高度C"));
	AddPropItem("ElbowPlatePara.D",PROPLIST_ITEM(id++,"顶部宽度D"));
	AddPropItem("ElbowPlatePara.E",PROPLIST_ITEM(id++,"切角宽度E"));
	AddPropItem("ElbowPlatePara.F",PROPLIST_ITEM(id++,"孔径间隙F"));
	AddPropItem("ElbowPlatePara.G",PROPLIST_ITEM(id++,"孔径间隙G"));
	AddPropItem("ElbowPlatePara.H",PROPLIST_ITEM(id++,"孔径H"));
	AddPropItem("ElbowPlatePara.m_iCutMode",PROPLIST_ITEM(id++,"倒角模式","倒角模式","0.切角|1.切弧"));
	AddPropItem("ElbowPlatePara.m_nThick",PROPLIST_ITEM(id++,"厚度","轴板厚度"));
	AddPropItem("ElbowPlateBasicInfo",PROPLIST_ITEM(id++,"基本参数"));
	AddPropItem("ElbowPlatePara.cMaterial",PROPLIST_ITEM(id++,"材质","",MakeMaterialMarkSetString()));
	AddPropItem("ElbowPlatePara.sPartNo",PROPLIST_ITEM(id++,"编号","肘板编号"));
	AddPropItem("ElbowPlatePara.m_fAngle",PROPLIST_ITEM(id++,"偏移角度","法兰肘板首块肋板相对于基准边的偏移角度"));
	AddPropItem("ElbowPlatePara.m_nPlateNum",PROPLIST_ITEM(id++,"肘板个数","法兰肘板个数"));
	AddPropItem("ElbowPlatePara.m_bObverse",PROPLIST_ITEM(id++,"",""));
	AddPropItem("ElbowPlatePara.m_bVertical",PROPLIST_ITEM(id++,"竖直且自动正位","肘板处置与基准钢板且自动正位","是|否"));
	//封板参数
	AddPropItem("CoverPlatePara",PROPLIST_ITEM(id++,"基本参数"));
	AddPropItem("CoverPlatePara.cMaterial",PROPLIST_ITEM(id++,"材质","封板材质",MakeMaterialMarkSetString()));
	AddPropItem("CoverPlatePara.sPartNo",PROPLIST_ITEM(id++,"编号","封板编号"));
	AddPropItem("CoverPlatePara.D",PROPLIST_ITEM(id++,"参数H"));
	AddPropItem("CoverPlatePara.W",PROPLIST_ITEM(id++,"参数W"));
	AddPropItem("CoverPlatePara.m_nThick",PROPLIST_ITEM(id++,"厚度"));
	AddPropItem("CoverPlatePara.m_fAlfa",PROPLIST_ITEM(id++,"偏移角α","偏移角α"));
	AddPropItem("CoverPlatePara.m_fBeta",PROPLIST_ITEM(id++,"扇形角β","扇形角β"));
	AddPropItem("CoverPlatePara.m_bUpPlate",PROPLIST_ITEM(id++,"上封板","","是|否"));
	AddPropItem("CoverPlatePara.m_bDownPlate",PROPLIST_ITEM(id++,"下封板","","是|否"));
	AddPropItem("CoverPlatePara.biHoleType",PROPLIST_ITEM(id++,"流水孔类型","关联钢板焊缝处的流水孔类型","0.圆孔|1.半圆孔|2.无"));
	AddPropItem("CoverPlatePara.BH",PROPLIST_ITEM(id++,"流水孔直径","关联钢板焊缝处的流水孔直径"));
	AddPropItem("WaterleakHole_RP1",PROPLIST_ITEM(id++,"关联基板1流水孔"));
	AddPropItem("hRelaPart1.R",PROPLIST_ITEM(id++,"半圆孔半径","第一块关联钢板处的流水孔半径"));
	AddPropItem("hRelaPart1.BG",PROPLIST_ITEM(id++,"流水孔间隙G","流水孔到管壁距离G"));
	AddPropItem("hRelaPart1.BS",PROPLIST_ITEM(id++,"流水孔间隙S","流水孔到立板壁距离S"));
	AddPropItem("WaterleakHole_RP2",PROPLIST_ITEM(id++,"关联基板2流水孔"));
	AddPropItem("hRelaPart2.R",PROPLIST_ITEM(id++,"半圆孔半径","第二块关联钢板处的流水孔半径"));
	AddPropItem("hRelaPart2.BG",PROPLIST_ITEM(id++,"流水孔间隙G","流水孔到管壁距离G"));
	AddPropItem("hRelaPart2.BS",PROPLIST_ITEM(id++,"流水孔间隙S","流水孔到立板壁距离S"));
	//肋板
	AddPropItem("RibPlatePara",PROPLIST_ITEM(id++,"基本参数"));
	AddPropItem("RibPlatePara.cMaterial",PROPLIST_ITEM(id++,"材质","肋板材质",MakeMaterialMarkSetString()));
	AddPropItem("RibPlatePara.sPartNo",PROPLIST_ITEM(id++,"编号","肋板编号"));
	AddPropItem("RibPlatePara.m_nThick",PROPLIST_ITEM(id++,"板厚"));
	AddPropItem("RibPlatePara.m_bUseH",PROPLIST_ITEM(id++,"启用参数","","是|否"));
	AddPropItem("RibPlatePara.D",PROPLIST_ITEM(id++,"参数D"));
	AddPropItem("RibPlatePara.H",PROPLIST_ITEM(id++,"参数H"));
	AddPropItem("RibPlatePara.N",PROPLIST_ITEM(id++,"参数N"));
	AddPropItem("RibPlatePara.W",PROPLIST_ITEM(id++,"参数W"));
	AddPropItem("RibPlatePara.S",PROPLIST_ITEM(id++,"偏移量S"));
	AddPropItem("RibPlatePara.m_bTopCir",PROPLIST_ITEM(id++,"顶部为圆弧","","是|否"));
	AddPropItem("RibPlatePara.m_bOutterRibPlate",PROPLIST_ITEM(id++,"",""));
	AddPropItem("RibPlatePara.m_bMirCreate",PROPLIST_ITEM(id++,"",""));
	AddPropItem("RibPlatePara.norm",PROPLIST_ITEM(id++,"法线方向"));
	AddPropItem("RibPlatePara.norm.x",PROPLIST_ITEM(id++,"X坐标分量"));
	AddPropItem("RibPlatePara.norm.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	AddPropItem("RibPlatePara.norm.z",PROPLIST_ITEM(id++,"Z坐标分量"));
}

int CDesignSpecialFlDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=false;
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CDesignSpecialFlDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}

BOOL CDesignSpecialFlDlg::OnInitDialog() 
{
	if(m_pCurLineTube==NULL||m_pBaseNode==NULL)
		return FALSE;
	CDialog::OnInitDialog();
	//所有控件在OnInitDialog之前都应默认处于显示状态，否则在初始化时有可能无法获取Hwnd导致异常，如需隐掉
	//应在OnInitDialog之后再隐掉 wjh-2011.8.21
	GetDlgItem(IDC_PLATE_LIST)->ShowWindow(SW_HIDE);
	m_ctrlPropGroup.SetMinTabWidth(40);	//设置TabCtrl最小宽度
	InitPropHashtable();	//初始化属性哈希表
	//初始化螺栓、钢板列表
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	long col_wide_arr[3]={50,70,70};
	m_listBolt.InitListCtrl(col_wide_arr);
	m_listPlate.InitListCtrl(col_wide_arr);
	//初始化
	m_fInnerRadius=m_pCurLineTube->GetDiameter()*0.5;
	m_fExternalRadius=m_pCurLineTube->GetDiameter();
	m_cMaterial=m_pCurLineTube->cMaterial;
	iSeg=m_pCurLineTube->iSeg;
	if(m_pCurLineTube->pEnd==m_pBaseNode)
		m_fOddment=m_pCurLineTube->endOdd();
	else if(m_pCurLineTube->pStart==m_pBaseNode)
		m_fOddment=m_pCurLineTube->startOdd();
	else
		return FALSE;
	//
	LayoutBoltPara.m_iStartEdge=m_pCurLineTube->m_bHasWeldRoad?0:1;
	LayoutBoltPara.m_nBoltD=20;
	LayoutBoltPara.m_fBoltLayoutR=m_fInnerRadius+0.5*(m_fExternalRadius-m_fInnerRadius);
	LayoutBoltPara.m_iBoltSegI=m_pCurLineTube->iSeg;
	LayoutBoltPara.m_nBoltNum=12;
	LayoutBoltPara.m_iDirection=1;
	LayoutBoltPara.m_fOffsetAngle=180.0/LayoutBoltPara.m_nBoltNum;
	//
	ElbowPlatePara.cMaterial=m_pCurLineTube->cMaterial;
	ElbowPlatePara.m_nPlateNum=LayoutBoltPara.m_nBoltNum;
	ElbowPlatePara.m_fAngle=LayoutBoltPara.m_fOffsetAngle+180.0/LayoutBoltPara.m_nBoltNum;
	ElbowPlatePara.m_nThick=10;
	ElbowPlatePara.A=150;
	ElbowPlatePara.B=m_fExternalRadius-m_fInnerRadius;
	ElbowPlatePara.C=30;
	ElbowPlatePara.D=30;
	ElbowPlatePara.E=25;
	ElbowPlatePara.F=30;
	ElbowPlatePara.G=20;
	ElbowPlatePara.H=0;
	ElbowPlatePara.m_iCutMode=1;
	ElbowPlatePara.E=_InitCutRadiusByTubeThick();
	//封版
	CoverPlatePara.cMaterial=m_pCurLineTube->cMaterial;
	CoverPlatePara.m_bUpPlate=TRUE;
	CoverPlatePara.m_bDownPlate=FALSE;
	CoverPlatePara.m_fAlfa=0;
	CoverPlatePara.m_fBeta=180;
	CoverPlatePara.m_nThick=10;
	CoverPlatePara.W=60;
	CoverPlatePara.D=30;
	InitWaterleakHolePara();
	//肋板
	RibPlatePara.cMaterial=m_pCurLineTube->cMaterial;
	RibPlatePara.m_nThick=10;
	if(m_pNodePlate[0])
		RibPlatePara.S=-ftoi(0.5*m_pNodePlate[0]->GetLength());
	else
		RibPlatePara.S=-100;
	RibPlatePara.m_bUseH=TRUE;
	RibPlatePara.W=RibPlatePara.N=60;
	RibPlatePara.H=50;
	RibPlatePara.m_bTopCir=FALSE;
	RibPlatePara.norm=m_pCurLineTube->End()-m_pCurLineTube->Start();
	normalize(RibPlatePara.norm);
	//显示相关设计参数
	DisplayPropertyList();
	//
	LayoutBolt();			//布置螺栓
	LayoutAttachPlate();	//布置肘板
	UpdateTubeOddment();	//平面切割后计算最小正负头
	UpdateRibPlateWN();		//更新环向肋板W、N参数
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
	int width = rect.Width();
	int height=rect.Height();
	rect.left=nScrLocationX;
	rect.top=nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect,TRUE);
	m_bEnableWindowMoveListen=true;
	return TRUE;  
}

bool CDesignSpecialFlDlg::UpdateItemPropValue(char* propStr)
{
	if(stricmp(propStr,"PlateNormOffset")==0)
	{
		CXhChar50 sText(m_fNormOffset);
		long prop_id=GetPropID("PlateNormOffset");
		m_propList.SetItemPropValue(prop_id,sText);
		m_propList.SetItemReadOnly(prop_id,m_iNormOffsetType!=2);
		return true;
	}
	else if(stricmp(propStr,"H")==0)
	{
		CXhChar50 sText(H);
		m_propList.SetItemPropValue(CDesignSpecialFlDlg::GetPropID("H"),sText);
		return true;
	}
	else if(stricmp(propStr,"m_fOddment")==0)
	{
		CXhChar50 sText(m_fOddment);
		m_propList.SetItemPropValue(CDesignSpecialFlDlg::GetPropID("m_fOddment"),sText);
		return true;
	}
	else
		return false;
}
CXhChar100 PointToString(const f3dPoint &point,bool b3dPt=TRUE);
int CDesignSpecialFlDlg::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	//基本信息
	if(CDesignSpecialFlDlg::GetPropID("sPartNo")==id)
		sText.Printf("%s",(char*)m_sPartNo);
	else if(CDesignSpecialFlDlg::GetPropID("iSeg")==id)
		sText.Copy(iSeg.ToString());
	else if(CDesignSpecialFlDlg::GetPropID("cMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(m_cMaterial,matStr))
			sText.Printf("%s",matStr);
	}
	else if(CDesignSpecialFlDlg::GetPropID("m_nPlateThick")==id)
		sText.Printf("%d",m_nThick);
	else if(GetPropID("m_fInnerRadius")==id)
	{
		sText.Printf("%f",m_fInnerRadius*2);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fExternalRadius")==id)
	{
		sText.Printf("%f",m_fExternalRadius*2);
		SimplifiedNumString(sText);
	}
	//装配参数
	else if(GetPropID("m_iNormOffsetType")==id)
	{
		if(m_iNormOffsetType==0)
			sText.Copy("0.上平面");
		else if(m_iNormOffsetType==1)
			sText.Copy("1.下平面");
		else //if(m_iNormOffsetType==2)
			sText.Copy("2.指定偏移");
	}
	else if(GetPropID("PlateNormOffset")==id)
	{
		sText.Printf("%f",m_fNormOffset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("PlateNorm")==id)
		sText.Copy(PointToString(axis_z));
	else if(GetPropID("PlateNorm.x")==id)
	{
		sText.Printf("%f",axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("PlateNorm.y")==id)
	{
		sText.Printf("%f",axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("PlateNorm.z")==id)
	{
		sText.Printf("%f",axis_z.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bCutDatumTube")==id)
	{
		if(m_bCutDatumTube)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("H")==id)
	{
		sText.Printf("%f",H);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fOddment")==id)
	{
		sText.Printf("%f",m_fOddment);
		SimplifiedNumString(sText);
	}
	//螺栓布置参数
	else if(GetPropID("m_nBoltNum")==id)
		sText.Printf("%d",LayoutBoltPara.m_nBoltNum);
	else if(GetPropID("m_nBoltD")==id)
		sText.Printf("%d",LayoutBoltPara.m_nBoltD);
	else if(GetPropID("m_iDriection")==id)
	{
		if(LayoutBoltPara.m_iDirection==0)
			sText.Copy("0.朝外");
		else
			sText.Copy("1.朝内");
	}
	else if(GetPropID("m_iBoltSegI")==id)
		sText.Printf("%d",LayoutBoltPara.m_iBoltSegI);
	else if(GetPropID("m_iStartEdge")==id)
	{
		if(LayoutBoltPara.m_iStartEdge==0)
			sText.Copy("0.焊道线");
		else
			sText.Copy("1.基准边");
	}
	else if(GetPropID("tube_D")==id)
	{
		sText.Printf("%f",m_pCurLineTube->GetDiameter());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fOffsetAngle")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_fOffsetAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fBoltLayoutD")==id)	
	{
		sText.Printf("%f",LayoutBoltPara.m_fBoltLayoutR*2);
		SimplifiedNumString(sText);
	}
	//肘板参数
	else if(GetPropID("ElbowPlatePara.A")==id)
	{
		sText.Printf("%f",ElbowPlatePara.A);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ElbowPlatePara.B")==id)
	{
		sText.Printf("%f",ElbowPlatePara.B);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ElbowPlatePara.C")==id)
	{
		sText.Printf("%f",ElbowPlatePara.C);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ElbowPlatePara.D")==id)
	{
		sText.Printf("%f",ElbowPlatePara.D);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ElbowPlatePara.E")==id)
	{
		sText.Printf("%f",ElbowPlatePara.E);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ElbowPlatePara.m_iCutMode")==id)
	{
		if(ElbowPlatePara.m_iCutMode==0)
			sText.Copy("0.切角");
		else
			sText.Copy("1.切弧");
	}
	else if(GetPropID("ElbowPlatePara.F")==id)
	{
		sText.Printf("%f",ElbowPlatePara.F);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ElbowPlatePara.G")==id)
	{
		sText.Printf("%f",ElbowPlatePara.G);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ElbowPlatePara.H")==id)
	{
		sText.Printf("%f",ElbowPlatePara.H);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ElbowPlatePara.cMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(ElbowPlatePara.cMaterial,matStr))
		sText.Printf("%s",matStr);
	}
	else if(GetPropID("ElbowPlatePara.sPartNo")==id)
		sText.Printf("%s",ElbowPlatePara.sPartNo);
	else if(GetPropID("ElbowPlatePara.m_fAngle")==id)
	{
		sText.Printf("%f",ElbowPlatePara.m_fAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ElbowPlatePara.m_nPlateNum")==id)
		sText.Printf("%d",ElbowPlatePara.m_nPlateNum);
	else if(GetPropID("ElbowPlatePara.m_nThick")==id)
		sText.Printf("%d",ElbowPlatePara.m_nThick);
	else if(GetPropID("ElbowPlatePara.m_bObverse")==id)
	{
		if(ElbowPlatePara.m_bObverse)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("ElbowPlatePara.m_bVertical")==id)
	{
		if(ElbowPlatePara.m_bVertical)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	//封板参数
	else if(GetPropID("CoverPlatePara.cMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(CoverPlatePara.cMaterial,matStr))
			sText.Printf("%s",matStr);
	}
	else if(GetPropID("CoverPlatePara.sPartNo")==id)
		sText.Printf("%s",CoverPlatePara.sPartNo);
	else if(GetPropID("CoverPlatePara.D")==id)
	{
		sText.Printf("%f",CoverPlatePara.D);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CoverPlatePara.W")==id)
	{
		sText.Printf("%f",CoverPlatePara.W);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CoverPlatePara.m_nThick")==id)
		sText.Printf("%d",CoverPlatePara.m_nThick);
	else if(GetPropID("CoverPlatePara.m_fAlfa")==id)
	{
		sText.Printf("%f",CoverPlatePara.m_fAlfa);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CoverPlatePara.m_fBeta")==id)
	{
		sText.Printf("%f",CoverPlatePara.m_fBeta);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CoverPlatePara.m_bUpPlate")==id)
	{
		if(CoverPlatePara.m_bUpPlate)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("CoverPlatePara.m_bDownPlate")==id)
	{
		if(CoverPlatePara.m_bDownPlate)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("CoverPlatePara.biHoleType")==id)
	{
		if(CoverPlatePara.biHoleType==0)
			sText.Copy("0.圆孔");
		else if(CoverPlatePara.biHoleType==1)
			sText.Copy("1.半圆孔");
		else //if(CoverPlatePara.biHoleType==2)
			sText.Copy("2.无");
	}
	else if(GetPropID("CoverPlatePara.BH")==id)
	{
		if(CoverPlatePara.biHoleType==0)
			sText.Printf("%g",CoverPlatePara.BH);
		else
			sText.Copy("0");
	}
	else if(GetPropID("hRelaPart1.R")==id)
	{
		double R=CoverPlatePara.uiAttribute1*0.1;
		sText.Printf("%.1f",R);
	}
	else if(GetPropID("hRelaPart1.BG")==id)
	{
		WORD* pw_BGorBS=(WORD*)&CoverPlatePara.uiAttribute1;
		WORD BG=*pw_BGorBS;
		sText.Printf("%d",BG);
	}
	else if(GetPropID("hRelaPart1.BS")==id)
	{
		WORD* pw_BGorBS=(WORD*)&CoverPlatePara.uiAttribute1;
		WORD BS=*(pw_BGorBS+1);
		sText.Printf("%d",BS);
	}
	else if(GetPropID("hRelaPart2.R")==id)
	{
		double R=CoverPlatePara.uiAttribute2*0.1;
		sText.Printf("%.1f",R);
	}
	else if(GetPropID("hRelaPart2.BG")==id)
	{
		WORD* pw_BGorBS=(WORD*)&CoverPlatePara.uiAttribute2;
		WORD BG=*pw_BGorBS;
		sText.Printf("%d",BG);
	}
	else if(GetPropID("hRelaPart2.BS")==id)
	{
		WORD* pw_BGorBS=(WORD*)&CoverPlatePara.uiAttribute2;
		WORD BS=*(pw_BGorBS+1);
		sText.Printf("%d",BS);
	}
	//肋板
	else if(GetPropID("RibPlatePara.cMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(RibPlatePara.cMaterial,matStr))
			sText.Printf("%s",matStr);
	}
	else if(GetPropID("RibPlatePara.sPartNo")==id)
		sText.Printf("%s",RibPlatePara.sPartNo);
	else if(GetPropID("RibPlatePara.m_nThick")==id)
		sText.Printf("%d",RibPlatePara.m_nThick);
	else if(GetPropID("RibPlatePara.m_bUseH")==id)
	{
		if(RibPlatePara.m_bUseH)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("RibPlatePara.D")==id)
	{
		sText.Printf("%f",RibPlatePara.D);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlatePara.H")==id)
	{
		sText.Printf("%f",RibPlatePara.H);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlatePara.N")==id)
	{
		sText.Printf("%f",RibPlatePara.N);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlatePara.W")==id)
	{
		sText.Printf("%f",RibPlatePara.W);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlatePara.S")==id)
	{
		sText.Printf("%f",RibPlatePara.S);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlatePara.m_bTopCir")==id)
	{
		if(RibPlatePara.m_bTopCir)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("RibPlatePara.m_bOutterRibPlate")==id)
	{
		if(RibPlatePara.m_bOutterRibPlate)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("RibPlatePara.m_bMirCreate")==id)
	{
		if(RibPlatePara.m_bMirCreate)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("RibPlatePara.norm.x")==id)
	{
		sText.Printf("%f",RibPlatePara.norm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlatePara.norm.y")==id)
	{
		sText.Printf("%f",RibPlatePara.norm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlatePara.norm.z")==id)
	{
		sText.Printf("%f",RibPlatePara.norm.z);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
void CDesignSpecialFlDlg::DisplayPropertyList(int iCurSel/*=0*/)
{
	const int GROUP_GENERAL=1,GROUP_LAYOUT_BOLT=2,GROUP_ELBOW_PLATE=3,GROUP_COVERT_PLATE=4,GROUP_RIB_PLATE=5;
	m_arrPropGroupLabel.RemoveAll();
	m_arrPropGroupLabel.SetSize(4);
	m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"常规");
	m_arrPropGroupLabel.SetAt(GROUP_LAYOUT_BOLT-1,"螺栓");
	m_arrPropGroupLabel.SetAt(GROUP_ELBOW_PLATE-1,"肋板");
	m_arrPropGroupLabel.SetAt(GROUP_COVERT_PLATE-1,"封板");
	//暂时不支持生成肋板 wht 11-05-26
	//m_arrPropGroupLabel.SetAt(GROUP_RIB_PLATE-1,"肋板");
	RefreshTabCtrl(iCurSel);
	//设置属性栏回调函数
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(ModifyItemValue);
	m_propList.SetButtonClickFunc(ButtonClickItem);
	CPropTreeItem *pSonPropItem,*pPropItem,*pGroupItem,*pRoot=m_propList.GetRootItem();
	CPropertyListOper<CDesignSpecialFlDlg> oper(&m_propList,this);
	//基本信息
	pGroupItem=oper.InsertPropItem(pRoot,"PlateBasicInfo");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	//
	oper.InsertEditPropItem(pGroupItem,"sPartNo");	
	//
	oper.InsertEditPropItem(pGroupItem,"iSeg");
	//
	oper.InsertCmbListPropItem(pGroupItem,"cMaterial");
	//
	oper.InsertEditPropItem(pGroupItem,"m_nPlateThick");
	//
	oper.InsertEditPropItem(pGroupItem,"m_fInnerRadius");
	//
	oper.InsertEditPropItem(pGroupItem,"m_fExternalRadius");
	//装配参数
	pGroupItem=oper.InsertPropItem(pRoot,"SetUpPara");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
	//
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_iNormOffsetType");
	pPropItem->m_bHideChildren=FALSE;
	//
	pSonPropItem=oper.InsertEditPropItem(pPropItem,"PlateNormOffset");
	pSonPropItem->SetReadOnly(m_iNormOffsetType!=2);
	//
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"PlateNorm");
	pPropItem->m_bHideChildren =TRUE;
	//X
	oper.InsertEditPropItem(pPropItem,"PlateNorm.x");
	//Y
	oper.InsertEditPropItem(pPropItem,"PlateNorm.y");
	//Z
	oper.InsertEditPropItem(pPropItem,"PlateNorm.z");
	//平焊法兰钢管端与法兰底面的距离
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bCutDatumTube");
	if(m_bCutDatumTube)
	{
		oper.InsertEditPropItem(pPropItem,"H");
		oper.InsertBtnEditPropItem(pPropItem,"m_fOddment");
	}
	//螺栓布置参数
	pGroupItem=oper.InsertPropItem(pRoot,"LayputBoltParam");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_LAYOUT_BOLT);
	//螺栓直径
	oper.InsertCmbEditPropItem(pGroupItem,"m_nBoltD");
	//螺栓朝向
	oper.InsertCmbListPropItem(pGroupItem,"m_iDriection");
	//段号
	oper.InsertEditPropItem(pGroupItem,"m_iBoltSegI");
	//钢管半径
	pPropItem=oper.InsertEditPropItem(pGroupItem,"tube_D");
	pPropItem->SetReadOnly(TRUE);
	//起始边
	if(m_pCurLineTube->m_bHasWeldRoad)
		oper.InsertCmbListPropItem(pGroupItem,"m_iStartEdge");
	//螺栓个数
	oper.InsertEditPropItem(pGroupItem,"m_nBoltNum");
	//首螺栓偏移角
	oper.InsertEditPropItem(pGroupItem,"m_fOffsetAngle");
	//螺栓所在圆半径
	oper.InsertEditPropItem(pGroupItem,"m_fBoltLayoutD");
	//肘板布置参数
	pGroupItem=oper.InsertPropItem(pRoot,"ElbowPlateBasicInfo");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_ELBOW_PLATE);
	//
	oper.InsertEditPropItem(pGroupItem,"ElbowPlatePara.sPartNo");
	//
	oper.InsertCmbListPropItem(pGroupItem,"ElbowPlatePara.cMaterial");
	//肘板个数
	oper.InsertEditPropItem(pGroupItem,"ElbowPlatePara.m_nPlateNum");
	//肘板偏移角
	oper.InsertEditPropItem(pGroupItem,"ElbowPlatePara.m_fAngle");
	//自动判断肘板位置即可
	oper.InsertCmbListPropItem(pGroupItem,"ElbowPlatePara.m_bVertical");
	//外形参数
	pGroupItem=oper.InsertPropItem(pRoot,"ElbowPlateProfile");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_ELBOW_PLATE);
	//
	oper.InsertEditPropItem(pGroupItem,"ElbowPlatePara.m_nThick");
	oper.InsertEditPropItem(pGroupItem,"ElbowPlatePara.A");
	oper.InsertEditPropItem(pGroupItem,"ElbowPlatePara.B");
	oper.InsertEditPropItem(pGroupItem,"ElbowPlatePara.C");
	oper.InsertEditPropItem(pGroupItem,"ElbowPlatePara.D");
	oper.InsertCmbListPropItem(pGroupItem,"ElbowPlatePara.m_iCutMode");
	oper.InsertEditPropItem(pGroupItem,"ElbowPlatePara.E");
	pPropItem=oper.InsertEditPropItem(pGroupItem,"ElbowPlatePara.H");
	if(ElbowPlatePara.H>0)
	{
		oper.InsertEditPropItem(pPropItem,"ElbowPlatePara.F");
		oper.InsertEditPropItem(pPropItem,"ElbowPlatePara.G");
	}
	//封板布置参数
	pGroupItem=oper.InsertPropItem(pRoot,"CoverPlatePara");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_COVERT_PLATE);
	//
	oper.InsertEditPropItem(pGroupItem,"CoverPlatePara.sPartNo");
	//
	oper.InsertCmbListPropItem(pGroupItem,"CoverPlatePara.cMaterial");
	//
	oper.InsertEditPropItem(pGroupItem,"CoverPlatePara.m_nThick");
	//封板宽度W
	oper.InsertEditPropItem(pGroupItem,"CoverPlatePara.W");
	//参数D
	oper.InsertEditPropItem(pGroupItem,"CoverPlatePara.D");
	//偏移角α
	oper.InsertEditPropItem(pGroupItem,"CoverPlatePara.m_fAlfa");
	//偏移角β
	oper.InsertEditPropItem(pGroupItem,"CoverPlatePara.m_fBeta");
	//上封板
	oper.InsertCmbListPropItem(pGroupItem,"CoverPlatePara.m_bUpPlate");
	//下封板
	oper.InsertCmbListPropItem(pGroupItem,"CoverPlatePara.m_bDownPlate");
	//工艺孔
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"CoverPlatePara.biHoleType");
	UpdateWaterHolePropItem(pPropItem);
	//肋板参数
	pGroupItem=oper.InsertPropItem(pRoot,"RibPlatePara");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_RIB_PLATE);
	//
	oper.InsertEditPropItem(pGroupItem,"RibPlatePara.sPartNo");
	//
	oper.InsertCmbListPropItem(pGroupItem,"RibPlatePara.cMaterial");
	//
	oper.InsertEditPropItem(pGroupItem,"RibPlatePara.m_nThick");
	//顶部圆弧连接
	oper.InsertCmbListPropItem(pGroupItem,"RibPlatePara.m_bTopCir");
	if(!RibPlatePara.m_bTopCir)
	{	//启用参数H
		oper.InsertCmbListPropItem(pGroupItem,"RibPlatePara.m_bUseH");
	}
	//肋板偏移量S
	oper.InsertEditPropItem(pGroupItem,"RibPlatePara.S");
	//封板宽度W
	oper.InsertEditPropItem(pGroupItem,"RibPlatePara.W");
	if(!RibPlatePara.m_bTopCir)
	{	
		oper.InsertEditPropItem(pGroupItem,"RibPlatePara.N");
		if(RibPlatePara.m_bUseH)
			oper.InsertEditPropItem(pGroupItem,"RibPlatePara.H");
		else
			oper.InsertEditPropItem(pGroupItem,"RibPlatePara.D");
	}
	//法线方向
	pGroupItem=oper.InsertPropItem(pRoot,"RibPlatePara.norm");
	pGroupItem->m_bHideChildren = FALSE;
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_RIB_PLATE);
	//X
	oper.InsertEditPropItem(pGroupItem,"RibPlatePara.norm.x");
	//Y
	oper.InsertEditPropItem(pGroupItem,"RibPlatePara.norm.y");
	//Z
	oper.InsertEditPropItem(pGroupItem,"RibPlatePara.norm.z");
	//
	m_propList.Redraw();
}
void CDesignSpecialFlDlg::UpdateWaterHolePropItem(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	CPropertyListOper<CDesignSpecialFlDlg> oper(&m_propList,this);
	m_propList.DeleteAllSonItems(pParentItem);
	if(CoverPlatePara.biHoleType==2)
		return;
	if(CoverPlatePara.biHoleType==0)
		oper.InsertEditPropItem(pParentItem,"CoverPlatePara.BH","","",-1,TRUE);
	CPropTreeItem* pPropItem=NULL;
	if(m_pNodePlate[0])
	{
		pPropItem=oper.InsertEditPropItem(pParentItem,"WaterleakHole_RP1","","",-1,TRUE);
		pPropItem->SetReadOnly();
		if(CoverPlatePara.biHoleType==1)	//环向封板钢板相应的流水孔半径
			oper.InsertEditPropItem(pPropItem,"hRelaPart1.R","","",-1,TRUE);
		else if(CoverPlatePara.biHoleType==0)
		{
			oper.InsertEditPropItem(pPropItem,"hRelaPart1.BG","","",-1,TRUE);
			oper.InsertEditPropItem(pPropItem,"hRelaPart1.BS","","",-1,TRUE);
		}
	}
	if(m_pNodePlate[1])
	{
		pPropItem=oper.InsertEditPropItem(pParentItem,"WaterleakHole_RP2","","",-1,TRUE);
		pPropItem->SetReadOnly();
		if(CoverPlatePara.biHoleType==1)	//环向封板钢板相应的流水孔半径
			oper.InsertEditPropItem(pPropItem,"hRelaPart2.R","","",-1,TRUE);
		else if(CoverPlatePara.biHoleType==0)
		{
			oper.InsertEditPropItem(pPropItem,"hRelaPart2.BG","","",-1,TRUE);
			oper.InsertEditPropItem(pPropItem,"hRelaPart2.BS","","",-1,TRUE);
		}
	}
}
void CDesignSpecialFlDlg::InitWaterleakHolePara()
{
	if(CoverPlatePara.biHoleType==2)
		return;
	if(m_pNodePlate[0])
	{
		if(CoverPlatePara.biHoleType==0)
		{	//圆孔
			WORD* pw_BGorBS=(WORD*)&(CoverPlatePara.uiAttribute1);
			*pw_BGorBS	  =25;
			*(pw_BGorBS+1)=25;
		}
		else	//半圆孔
			CoverPlatePara.uiAttribute1=_InitWaterHoleRadiusByThick(m_pNodePlate[0]->Thick);
	}
	if(m_pNodePlate[1])
	{
		if(CoverPlatePara.biHoleType==0)
		{	//圆孔
			WORD* pw_BGorBS=(WORD*)&(CoverPlatePara.uiAttribute2);
			*pw_BGorBS	  =25;
			*(pw_BGorBS+1)=25;
		}
		else	//半圆孔
			CoverPlatePara.uiAttribute2=_InitWaterHoleRadiusByThick(m_pNodePlate[1]->Thick);
	}
}
UINT CDesignSpecialFlDlg::_InitWaterHoleRadiusByThick(double fPlateThick)
{
	//焊接半圆孔半径(旧代码中半圆型流水孔的默认值一直都是25mm半径） wjh-2018.2.11
	UINT uiAttribute=250;
	if(fPlateThick<=12)
		uiAttribute=200;
	else if(fPlateThick<=16)
		uiAttribute=250;
	else if(fPlateThick>=18)
		uiAttribute=300;
	return uiAttribute;
}
UINT CDesignSpecialFlDlg::_InitCutRadiusByTubeThick()
{
	if(m_pCurLineTube)
		return 25;
	if(m_pCurLineTube->size_thick<=10)
		return 25;	//壁厚5mm＜t≤10mm时， 倒角圆弧半径R＝25mm
	else if(m_pCurLineTube->size_thick<=15)
		return 30;	//壁厚10mm＜t≤15mm时，倒角圆弧半径R＝30mm
	else if(m_pCurLineTube->size_thick<=20)
		return 35;	//壁厚15mm＜t≤20mm时，倒角圆弧半径R＝35mm
	else //if(pRelyRod->size_thick<=25)
		return 40;	//壁厚20mm＜t≤25mm时，倒角圆弧半径R＝40mm
}
void CDesignSpecialFlDlg::OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iCurSel=m_ctrlPropGroup.GetCurSel();
	m_propList.m_iPropGroup = iCurSel;
	m_propList.Redraw();
	
	//更新示意图
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	m_CurPlateBmp.Detach();
	if(iCurSel==1)	//螺栓
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_FL);
	else if(iCurSel==2)	//肘板
		m_CurPlateBmp.LoadBitmap(IDB_BMP_ELBOW_PLATE1);
	else if(iCurSel==3)	//封板
		m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_COVERPLATE2);
	else if(iCurSel==4)	//肋板
	{
		if(RibPlatePara.m_bTopCir)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE2);
		else
			m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE);
	}
	else //if(iCurSel==0)//外形
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_FL2);
	pPic->SetBitmap(m_CurPlateBmp);
	if(iCurSel==2)
	{	//显示肘板列表
		GetDlgItem(IDC_PLATE_LIST)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BOLT_LIST)->ShowWindow(SW_HIDE);
	}
	else
	{	//显示螺栓列表
		GetDlgItem(IDC_PLATE_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BOLT_LIST)->ShowWindow(SW_SHOW);
	}
	*pResult = 0;
}

void CDesignSpecialFlDlg::RefreshTabCtrl(int iCurSel)
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
	{
		RECT rcTabGroup;
		CWnd *pTabGroup = GetDlgItem(IDC_TAB_GROUP);
		if(pTabGroup)
			pTabGroup->GetWindowRect(&rcTabGroup);
		ScreenToClient(&rcTabGroup);
		if(m_arrPropGroupLabel.GetSize()>0)
			rcPropList.top=rcMainWnd.top+35;
		else 
			rcPropList.top=rcMainWnd.top+10;
		rcTabGroup.top=rcPropList.top-25;
		if(pTabGroup)
			pTabGroup->MoveWindow(&rcTabGroup);
	}
	if(pTopWnd)
		pTopWnd->MoveWindow(&rcPropList);
}

void CDesignSpecialFlDlg::OnOK() 
{
	//确认输入
}

void CDesignSpecialFlDlg::CutDatumTubeByPlane(CLDSPlate *pPlate)
{	//平面切割钢管
	if(pPlate==NULL)
		return;
	f3dPoint tube_len_vec;
	TUBEJOINT *pCurJoint=NULL;
	if(m_pBaseNode==m_pCurLineTube->pStart)
	{
		tube_len_vec=m_pCurLineTube->Start()-m_pCurLineTube->End();
		pCurJoint=&m_pCurLineTube->m_tJointStart;
		m_pCurLineTube->SetStartOdd(m_fOddment);
	}
	else
	{
		tube_len_vec=m_pCurLineTube->End()-m_pCurLineTube->Start();
		pCurJoint=&m_pCurLineTube->m_tJointEnd;
		m_pCurLineTube->SetEndOdd(m_fOddment);
	}
	normalize(tube_len_vec);
	pCurJoint->type=0;
	pCurJoint->cutPlaneNorm=pPlate->ucs.axis_z;
	if(tube_len_vec*pPlate->ucs.axis_z<0)
		pCurJoint->cutPlaneNorm*=-1.0;
	pCurJoint->cutPlanePos=pPlate->ucs.origin+pPlate->ucs.axis_z*H;
	//
	m_pCurLineTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),
		g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(m_pCurLineTube->GetSolidPartObject());
	g_pSolidDraw->Draw();
}

void CDesignSpecialFlDlg::OnConfirmCreate() 
{	//塔脚板
	m_pCirPlate=CreateCircularPlate(m_pCurLineTube); //生成环向底板 wjh-2011.8.21
	if(m_pCirPlate)
	{	//上侧水平法兰或异形法兰,以及螺栓肘板封板肋板
		CreateBolt(m_pCirPlate);	//螺栓
		CreateAttachPlate(m_pCirPlate,m_pCurLineTube);//肘板
		CreateCoverPlate(m_pNodePlate[0],m_pNodePlate[1],m_pCoverPlate[0],m_pCoverPlate[1]);	//封板
		if(m_bCreateRibPlate) //根据设置生成肋板 wht 11-05-26
			CreateRibPlate(m_pNodePlate[0],m_pNodePlate[1]);	//肋板
		if(m_bCutDatumTube)
			CutDatumTubeByPlane(m_pCirPlate);//平面切割钢管
	}
	CDialog::OnOK();
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

void CDesignSpecialFlDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rectDrawRgn; //当前绘图区域
	CWnd *pWnd = GetDlgItem(IDC_S_MAP);
	pWnd->GetWindowRect(&rectDrawRgn);
	ScreenToClient(&rectDrawRgn);
	int width=rectDrawRgn.Width();
	int height=rectDrawRgn.Height();
	double real_width=2*m_fExternalRadius+20,real_height=real_width,scale=1;
	double T=m_nThick;
	//
	UCS_2dSTRU draw_ucs;	//绘图坐标系
	draw_ucs.origin.Set(rectDrawRgn.TopLeft().x+width*0.5,rectDrawRgn.TopLeft().y+height*0.5);
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
	f2dPoint origin=draw_ucs.origin;
	int axis_y_len=ftoi(m_fExternalRadius*scale)+3;
	int axis_x_len=axis_y_len;
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
	//
	POINT topLeft,bottomRight;
	//法兰
	double fTubeR=m_fInnerRadius*scale;
	double fTubeThick=min(m_pCurLineTube->GetThick()*scale,8);
	for(int i=0;i<4;i++)
	{
		int nRadius=ftoi(m_fExternalRadius*scale);
		if(i==1)
			nRadius=ftoi(m_fInnerRadius*scale);
		else if(i==2)
		{
			pOldPen=dc.SelectObject(&green_dash_pen);
			nRadius=ftoi(LayoutBoltPara.m_fBoltLayoutR*scale);
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
		double cur_r=LayoutBoltPara.m_fBoltLayoutR;
		if(i==1)
			cur_r=m_fInnerRadius;
		else if(i==2)
			cur_r=m_fExternalRadius;
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
		//if(!m_pCurLineTube->m_bHasWeldRoad&&i==1)
		//	continue;
		int nInnerR=ftoi(fTubeR-fTubeThick);
		int nOutterR=ftoi(fTubeR);
		CPoint ptVertex[3];
		f2dPoint tempVertexArr[3];
		UCS_2dSTRU temp_ucs=draw_ucs;
		double fWeldRoadAngle=-m_pCurLineTube->CalWeldRoadAngle();
		if(LayoutBoltPara.m_iStartEdge==0)
			fWeldRoadAngle*=-1.0;
		f3dPoint rot_axis(0,0,1);
		if(m_pCurLineTube->pStart==m_pBaseNode)
			rot_axis*=-1.0;
		if(i==1)
		{	//焊道线
			f3dPoint axis(temp_ucs.axis_x.x,temp_ucs.axis_x.y,0);
			RotateVectorAroundVector(axis,fWeldRoadAngle,rot_axis);
			temp_ucs.axis_x.Set(axis.x,axis.y);
			axis.Set(temp_ucs.axis_y.x,temp_ucs.axis_y.y,0);
			RotateVectorAroundVector(axis,fWeldRoadAngle,rot_axis);
			temp_ucs.axis_y.Set(axis.x,axis.y);
		}
		tempVertexArr[0].Set(nInnerR,fTubeThick);
		tempVertexArr[1].Set(nInnerR,-fTubeThick);
		tempVertexArr[2].Set(nOutterR,0);
		for(int j=0;j<3;j++)
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
		if(LayoutBoltPara.m_iStartEdge==0)
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
	//绘制螺栓
	pOldPen=dc.SelectObject(&red_solid_pen);
	char sText[100]="";
	f3dPoint startPoint(LayoutBoltPara.m_fBoltLayoutR*scale,0,0);
	int nBoltR=ftoi(LayoutBoltPara.m_nBoltD*0.5*scale);
	//
	int iCurSel=-1;
	POSITION pos = m_listBolt.GetFirstSelectedItemPosition();
	if(pos!=NULL)
		iCurSel = m_listBolt.GetNextSelectedItem(pos);
	for(i=0;i<m_listBolt.GetItemCount();i++)
	{
		f3dPoint tempPoint=startPoint;
		m_listBolt.GetItemText(i,2,sText,100);
		double cur_angle=atof(sText)*RADTODEG_COEF;
		rotate_point_around_axis(tempPoint,cur_angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
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
	//生成肘板示意图
	iCurSel=-1;
	pos = m_listPlate.GetFirstSelectedItemPosition();
	if(pos!=NULL)
		iCurSel = m_listPlate.GetNextSelectedItem(pos);
	f3dPoint startPt(m_fInnerRadius*scale,0,0);
	f3dPoint endPt((m_fInnerRadius+ElbowPlatePara.B)*scale,0,0);
	double fPlateThick=ElbowPlatePara.m_nThick*scale;
	for(i=0;i<m_listPlate.GetItemCount();i++)
	{
		f2dPoint vertex_arr[4];
		f3dPoint tempStart=startPt;
		f3dPoint tempEnd=endPt;
		m_listPlate.GetItemText(i,1,sText,100);
		double cur_angle=atof(sText)*RADTODEG_COEF;
		rotate_point_around_axis(tempStart,cur_angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
		rotate_point_around_axis(tempEnd,cur_angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
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
	dc.SelectObject(pOldPen);
}

void CDesignSpecialFlDlg::ReDraw()
{
	Invalidate(FALSE);
}

void CDesignSpecialFlDlg::OnKeydownBoltList(NMHDR* pNMHDR, LRESULT* pResult) 
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
			m_listBolt.DeleteItem(iCurSel);
			pos = m_listBolt.GetFirstSelectedItemPosition();
		}
		else
			break;
	}
	Invalidate(FALSE);
	*pResult = 0;
}

void CDesignSpecialFlDlg::OnClickBoltList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	Invalidate(FALSE);
	*pResult = 0;
}

void CDesignSpecialFlDlg::OnKeydownPlateList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_listPlate.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_listPlate.GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_UP)
		{	//按一次UP键选中项会上移一位，但是这个操作是在按下UP键时触发的
			//此时还并未移动，所以手动调整选中项的位置
			if(iCurSel>=1)
				iCurSel--;
			if(iCurSel>=0)
				m_listPlate.SetItemState(iCurSel,LVIS_SELECTED,LVIS_SELECTED);
		}
		else if(pLVKeyDow->wVKey==VK_UP||pLVKeyDow->wVKey==VK_DOWN)
		{	//按一次UP键选中项会下移一位，但是这个操作是在按下DOWN键时触发的
			//此时还并未移动，所以手动调整选中项的位置
			if(iCurSel<m_listBolt.GetItemCount())
				iCurSel++;
			if(iCurSel>=0)
				m_listPlate.SetItemState(iCurSel,LVIS_SELECTED,LVIS_SELECTED);
		}
		else if(pLVKeyDow->wVKey==VK_DELETE)
		{
			m_listPlate.DeleteItem(iCurSel);
			pos = m_listPlate.GetFirstSelectedItemPosition();
		}
		else
			break;
	}
	Invalidate(FALSE);
	*pResult = 0;
}

void CDesignSpecialFlDlg::OnClickPlateList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	Invalidate(FALSE);
	POSITION pos = m_listPlate.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_listPlate.GetNextSelectedItem(pos);
		if(iCurSel>=0)
			m_listPlate.EditLabel(iCurSel);
	}
	*pResult = 0;
}

CLDSPlate *CDesignSpecialFlDlg::CreateCircularPlate(CLDSLineTube *pDatumTube)
{
	f3dLine baseline(pDatumTube->pStart->Position(false),pDatumTube->pEnd->Position(false));
	if(!baseline.PtInLine(m_pBaseNode->Position(false)))
	{
		AfxMessageBox("所选节点不在杆件上!");
		return FALSE;
	}
	baseline.startPt=pDatumTube->pStart->Position(true);
	baseline.endPt=pDatumTube->pEnd->Position(true);
	CUndoOperObject undo(&Ta);
	CLDSPlate *pCirPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	pCirPlate->iSeg=iSeg;
	pCirPlate->cMaterial=m_cMaterial;
	pCirPlate->SetPartNo(m_sPartNo);
	pCirPlate->Thick=m_nThick;
	pCirPlate->m_uStatMatCoef=pDatumTube->m_uStatMatCoef;
	pCirPlate->m_hPartWeldParent = pDatumTube->handle;
	pCirPlate->designInfo.m_hBaseNode=m_pBaseNode->handle;	//板的基准节点
	pCirPlate->designInfo.m_hBasePart=pDatumTube->handle;	//缺少基准杆件，对称时可能会报出警示信息
	pCirPlate->SetLayer(m_pBaseNode->layer());
	pCirPlate->cfgword=m_pBaseNode->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	pCirPlate->designInfo.origin.datum_pos_style=2;
	pCirPlate->designInfo.origin.des_para.RODNODE.hNode=m_pBaseNode->handle;
	pCirPlate->designInfo.origin.des_para.RODNODE.hRod =pDatumTube->handle;
	//法线设计参数
	pCirPlate->designInfo.norm.norm_style=0;
	pCirPlate->designInfo.norm.vector=axis_z;
	CDesignLjPartPara *pLjPara=pCirPlate->designInfo.partList.Add(pDatumTube->handle);
	pLjPara->hPart=pDatumTube->handle;
	f3dPoint tube_len_vec;
	if(m_pBaseNode==pDatumTube->pStart)
	{
		pLjPara->start0_end1=0;	//始端连接
		pCirPlate->designInfo.origin.des_para.RODNODE.direction=1;	//终-->始
		tube_len_vec=pDatumTube->Start()-pDatumTube->End();
		pCirPlate->ucs.origin=pDatumTube->Start();
	}
	else if(m_pBaseNode==pDatumTube->pEnd)
	{
		pLjPara->start0_end1=1;	//终端连接
		pCirPlate->designInfo.origin.des_para.RODNODE.direction=0;	//始-->终
		tube_len_vec=pDatumTube->End()-pDatumTube->Start();
		pCirPlate->ucs.origin=pDatumTube->End();
	}
	normalize(tube_len_vec);
	pCirPlate->ucs.axis_z=tube_len_vec;
	pCirPlate->ucs.axis_x=pDatumTube->ucs.axis_x;
	pCirPlate->ucs.axis_y=pCirPlate->ucs.axis_z^pCirPlate->ucs.axis_x;
	if(!pCirPlate->ucs.axis_z.IsEqual(axis_z))
	{
		f3dPoint rot_axis_vec=pCirPlate->ucs.axis_z^axis_z;
		double rot_ang=ACOS(pCirPlate->ucs.axis_z*axis_z);
		RotateVectorAroundVector(pCirPlate->ucs.axis_x,rot_ang,rot_axis_vec);
		RotateVectorAroundVector(pCirPlate->ucs.axis_y,rot_ang,rot_axis_vec);
		pCirPlate->ucs.axis_z=axis_z;
	}
	if(tube_len_vec*pCirPlate->ucs.axis_z>0)
		pCirPlate->designInfo.origin.des_para.RODNODE.len_offset_dist=m_fNormOffset;
	else
		pCirPlate->designInfo.origin.des_para.RODNODE.len_offset_dist=-m_fNormOffset;
	pCirPlate->designInfo.origin.UpdatePos(console.GetActiveModel());
	pCirPlate->ucs.origin=pCirPlate->designInfo.origin.Position();
	int piece_sector_n=4;
	double piece_sector_rad_angle=(2*Pi)/piece_sector_n;
	if(LayoutBoltPara.m_fOffsetAngle!=0)
		RotateVectorAroundVector(pCirPlate->ucs.axis_x,LayoutBoltPara.m_fOffsetAngle*RADTODEG_COEF,pCirPlate->ucs.axis_z);
	pCirPlate->ucs.axis_y=pCirPlate->ucs.axis_z^pCirPlate->ucs.axis_x;
	normalize(pCirPlate->ucs.axis_y);
	//环形加劲板
	PROFILE_VERTEX *pVertex=NULL;
	pCirPlate->m_fInnerRadius = m_fInnerRadius;
	pCirPlate->m_tInnerColumnNorm=tube_len_vec;
	f3dPoint vertex_origin,vertex_outter_start,vertex_outter_end;
	vertex_origin=pCirPlate->ucs.origin;
	vertex_outter_start=pCirPlate->ucs.origin+pCirPlate->ucs.axis_x*m_fExternalRadius;
	vertex_outter_start.feature=1;
	vertex_outter_start.ID=2;
	pVertex=pCirPlate->vertex_list.append(PROFILE_VERTEX(vertex_outter_start,1));
	pVertex->sector_angle = piece_sector_rad_angle;
	pVertex->center=pCirPlate->ucs.origin;
	vertex_outter_end=vertex_outter_start;
	for(int i=0;i<piece_sector_n;i++)
	{
		rotate_point_around_axis(vertex_outter_end,piece_sector_rad_angle,pCirPlate->ucs.origin,pCirPlate->ucs.origin+pCirPlate->ucs.axis_z*100);
		vertex_outter_end.feature=1;
		vertex_outter_end.ID=2+i;
		if(i<piece_sector_n-1)
		{
			pVertex=pCirPlate->vertex_list.append(PROFILE_VERTEX(vertex_outter_end,1));
			pVertex->sector_angle=piece_sector_rad_angle;
		}
		else
			pVertex=pCirPlate->vertex_list.append(PROFILE_VERTEX(vertex_outter_end,0));
		if(pVertex)
			pVertex->center=pCirPlate->ucs.origin;
	}
	for(pVertex=pCirPlate->vertex_list.GetFirst();pVertex;pVertex=pCirPlate->vertex_list.GetNext())
		coord_trans(pVertex->vertex,pCirPlate->ucs,FALSE);
	pCirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pCirPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return pCirPlate;
}

//布置肘板
int CDesignSpecialFlDlg::LayoutAttachPlate()
{
	//环形阵列
	int i=0;
	CStringArray str_arr;
	str_arr.SetSize(3);
	while(m_listPlate.GetItemCount()>0)
		m_listPlate.DeleteItem(0);
	double init_angle = ElbowPlatePara.m_fAngle;
	double piece_angle = 360.0/ElbowPlatePara.m_nPlateNum;
	//计算射线杆件连接的节点板所在角度
	//double fAngle[2]={0,0};
	BOOL bExistPlate[2]={FALSE,FALSE};
	m_pCurLineTube->BuildUCS();
	f3dPoint datum_line_vec=m_pCurLineTube->Start()-m_pCurLineTube->End();
	normalize(datum_line_vec);
	//for(i=0;i<2;i++)
	//{
	//	if(m_pRayLinePart[i]&&!m_plate.ucs.axis_z.IsZero())
	//	{	
	//		bExistPlate[i]=TRUE;
	//		f3dPoint ray_vec=m_pRayLinePart[i]->Start()-m_pRayLinePart[i]->End();
	//		normalize(ray_vec);
	//		if(m_pRayLinePart[i]->pStart==m_pBaseNode)
	//			ray_vec*=-1;
	//		f3dPoint axis_x=(datum_line_vec^ray_vec)^m_plate.ucs.axis_z;
	//		if(axis_x*ray_vec<0)
	//			axis_x*=-1.0;
	//		vector_trans(axis_x,m_pCurLineTube->ucs,FALSE);
	//		fAngle[i]=Cal2dLineAng(f2dPoint(0,0),f2dPoint(axis_x.x,axis_x.y))*DEGTORAD_COEF;
	//	}
	//}
	for(i=0;i<ElbowPlatePara.m_nPlateNum;i++)
	{
		str_arr[0] = ElbowPlatePara.sPartNo;	//规格
		double fCurAngle=init_angle+piece_angle*i;
		str_arr[1].Format("%f",fCurAngle);//方位角
		SimplifiedNumString(str_arr[1]);
		str_arr[2].Format("钢管肋板");
		SimplifiedNumString(str_arr[2]);
		//如下考虑不准确没考虑到0°与359°的近似一致，且综合考虑没有太大必要进行这样过滤,否则会带来混乱 wjh-2015.6.9
		//此处取3°为临界值
		//if((bExistPlate[0]&&fabs(fCurAngle-fAngle[0])<3)
		//	||(bExistPlate[1]&&fabs(fCurAngle-fAngle[1])<3))
		//	continue;
		m_listPlate.InsertItemRecord(-1,str_arr);
	}
	return 0;
}

//生成附加钢板
void CDesignSpecialFlDlg::CreateAttachPlate(CLDSPlate *pPlate,CLDSLineTube *pDatumTube)
{
	//根据钢管连接端以及基准板法线方向判断肘板加在基准板正面还是反面
	f3dPoint tube_len_vec=pDatumTube->End()-pDatumTube->Start();
	normalize(tube_len_vec);
	if(pDatumTube->pEnd==m_pBaseNode)
		tube_len_vec*=-1;
	if(tube_len_vec*pPlate->ucs.axis_z>0)
		ElbowPlatePara.m_bObverse=TRUE;
	else
		ElbowPlatePara.m_bObverse=FALSE;
	CUndoOperObject undo(&Ta);
	CXhChar100 sText;
	int nCount=m_listPlate.GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		CLDSParamPlate *pElbowPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pElbowPlate->m_iParamType = TYPE_ELBOWPLATE;
		pElbowPlate->m_hPartWeldParent = pPlate->handle;
		pElbowPlate->m_bDisplayHole=TRUE;
		pElbowPlate->cMaterial=ElbowPlatePara.cMaterial;	//设定材质
		pElbowPlate->iSeg=pPlate->iSeg;
		pElbowPlate->SetLayer(pPlate->layer());	//调整肘板的图层名
		pElbowPlate->cfgword=pPlate->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pElbowPlate->thick = ElbowPlatePara.m_nThick;
		pElbowPlate->SetDesignItemValue('A',ElbowPlatePara.A,"肘板参数A",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('B',ElbowPlatePara.B,"肘板参数B",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('C',ElbowPlatePara.C,"肘板参数C",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('D',ElbowPlatePara.D,"肘板参数D",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('E',ElbowPlatePara.E,"肘板参数E",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('F',ElbowPlatePara.F,"肘板参数F",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('G',ElbowPlatePara.G,"肘板参数G",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('H',ElbowPlatePara.H,"肘板参数H",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('e',ElbowPlatePara.m_iCutMode,"倒角模式",PARAM_SHAPE,1);
		pElbowPlate->SetDesignItemValue('L',pPlate->handle,"基准底板",PARAM_POS,1);
		if(LayoutBoltPara.m_iStartEdge==0)		//以焊道线为基准
			pPlate->m_bAttachPartByWeldRoad=TRUE;
		else //if(LayoutBoltPara.m_iStartEdge==1)//以钢管基准边为基准
			pPlate->m_bAttachPartByWeldRoad=FALSE;
		if(pDatumTube->ucs.axis_z*pDatumTube->ucs.axis_z>0)
			pElbowPlate->SetDesignItemValue('K',-pDatumTube->handle,"肘板所依靠的基准钢管",PARAM_POS,1);
		else
			pElbowPlate->SetDesignItemValue('K',pDatumTube->handle,"肘板所依靠的基准钢管",PARAM_POS,1);
		if(ElbowPlatePara.m_bObverse)	//在基准钢板正面布置肘板
			pElbowPlate->SetDesignItemValue('Z',1,"1表示在基准钢板正面布置钢板，-1表示在反面布置",PARAM_POS);
		else 
			pElbowPlate->SetDesignItemValue('Z',-1,"1表示在基准钢板正面布置钢板，-1表示在反面布置",PARAM_POS);
		//肘板垂直基准钢管或者肘板沿基准钢管方向
		pElbowPlate->SetDesignItemValue('V',ElbowPlatePara.m_bVertical,"0.沿基准钢管方向的肘板 1.竖直且带自动正为的肘板");
		CXhChar16 label;
		m_listPlate.GetItemText(i,0,label,16);	//编号
		pElbowPlate->SetPartNo(label);
		m_listPlate.GetItemText(i,1,sText,100);	//方位角
		pElbowPlate->SetDesignItemValue('Y',atof(sText),"肘板定位方向角°");
		pElbowPlate->SetDesignItemValue('b',1,"肘板定位的角度调整方式;0.旧版本以钢管为基准;1.新版本以依赖钢板为基准");
		//统一将肘板转换成肋板处理  wxc-2017.5.26
		pElbowPlate->ShiftElbowAndRibPlateSchema(TYPE_RIBPLATE);
		pElbowPlate->DesignPlate();
		pElbowPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),
			g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pElbowPlate->GetSolidPartObject());
		g_pSolidDraw->Draw();
	}
}

//布置螺栓
int CDesignSpecialFlDlg::LayoutBolt()
{
	//环形阵列
	int i=0;
	while(m_listBolt.GetItemCount()>0)
		m_listBolt.DeleteItem(0);
	CStringArray str_arr;
	str_arr.SetSize(3);
	CString sBoltSpec;
	sBoltSpec.Format("M%d",LayoutBoltPara.m_nBoltD);
	double init_angle = LayoutBoltPara.m_fOffsetAngle;
	double piece_angle = 360.0/LayoutBoltPara.m_nBoltNum;
	for(i=0;i<LayoutBoltPara.m_nBoltNum;i++)
	{
		str_arr[0] = sBoltSpec;						//规格
		str_arr[1].Format("%f",LayoutBoltPara.m_fBoltLayoutR);	//螺栓所在圆的半径
		SimplifiedNumString(str_arr[1]);
		str_arr[2].Format("%f",init_angle+piece_angle*i);
		SimplifiedNumString(str_arr[2]);			//方位角
		m_listBolt.InsertItemRecord(-1,str_arr);
	}
	return 0;
}

void CDesignSpecialFlDlg::CreateBolt(CLDSPlate *pPlate)
{	
	f3dPoint pos;
	char sText[100]="";
	DWORD dwRayNo = 0;
	pPlate->m_iLsArrayStyle = 1;	//环向布置
	if(LayoutBoltPara.m_iStartEdge==0)//以钢管焊道线为基准
		pPlate->m_bAttachPartByWeldRoad = TRUE;
	else
		pPlate->m_bAttachPartByWeldRoad = FALSE;
	CUndoOperObject undo(&Ta);
	//螺栓数据定义区
	int iInitRayNo=1;
	ATOM_LIST<RAYNO_RECORD>rayNoList;
	int nCount=m_listBolt.GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->iSeg = pPlate->iSeg;
		pBolt->SetLayer(pPlate->layer());	//调整螺栓的图层名
		pBolt->cfgword=pPlate->cfgword;	//调整螺栓配材号与基准构件配材号一致
		pBolt->des_base_pos.hPart = pPlate->handle;
		pBolt->des_base_pos.cLocationStyle=TRUE; 
		pBolt->des_work_norm.norm_style=3;	//构件法线
		pBolt->des_work_norm.hVicePart=pPlate->handle;
		pBolt->des_work_norm.direction=LayoutBoltPara.m_iDirection;
		pBolt->AddL0Thick(pPlate->handle,TRUE);//螺栓通厚
		if(LayoutBoltPara.m_iDirection==1)
			pBolt->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
		//环形阵列
		m_listBolt.GetItemText(i,0,sText,100);
		pBolt->set_d(atoi(sText+1));
		m_listBolt.GetItemText(i,2,sText,100);	//方位角
		double angle = atof(sText)*RADTODEG_COEF;	
		pBolt->des_base_pos.offset_angle = angle;	//螺栓相对于基准钢管的偏移角度
		m_listBolt.GetItemText(i,1,sText,100);		//螺栓距基准构件中心的距离
		pBolt->des_base_pos.R=atof(sText);			//该变量小于0表示螺栓为非环向布置
		CLsRef *pLsRef=pPlate->AppendLsRef(pBolt->GetLsRef());
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
	pPlate->GetLsRefList()->ResetBoltRefRayNo(pPlate);
}

//更新钢管正负头信息
void CDesignSpecialFlDlg::UpdateTubeOddment()
{	//自动计算钢管正负头---平面切割
	f3dPoint curPos,tube_len_vec;
	if(m_pCurLineTube->pEnd==m_pBaseNode)
	{
		curPos=m_pCurLineTube->End();
		tube_len_vec=m_pCurLineTube->End()-m_pCurLineTube->Start();	//始端==>终端
	}
	else
	{
		curPos=m_pCurLineTube->Start();
		tube_len_vec=m_pCurLineTube->Start()-m_pCurLineTube->End();	//终端==>始端
	}
	normalize(tube_len_vec);
	if(!axis_z.IsZero())
	{	//如果为平面切割自动计算一个最小的负头
		f3dPoint inters,temp_vec;
		f3dPoint cutPlaneOrigin=m_pBaseNode->Position();
		cutPlaneOrigin+=(H+m_fNormOffset)*axis_z;
		Int3dlf(inters,curPos,tube_len_vec,cutPlaneOrigin,axis_z);
		double min_dist=DISTANCE(inters,curPos);
		double angle=cal_angle_of_2vec(tube_len_vec,axis_z);
		double tana=fabs(tan(angle));
		//交点==>当前设计端位置
		temp_vec=curPos-inters;
		normalize(temp_vec);
		if(temp_vec*tube_len_vec>0)	//需要设置负头
			min_dist=-(min_dist-(m_pCurLineTube->GetDiameter()*0.5*tana));
		else
			min_dist+=(m_pCurLineTube->GetDiameter()*0.5*tana);
		m_fOddment=ftoi(min_dist);
	}
	CString sOddment="";
	sOddment.Format("%f",m_fOddment);
	SimplifiedNumString(sOddment);
	m_propList.SetItemPropValue(CDesignSpecialFlDlg::GetPropID("m_fOddment"),sOddment);
}

void CDesignSpecialFlDlg::OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM *pItem = &pDispInfo->item;
	// In case window is being destroyed
    if(!IsWindow(GetSafeHwnd()))
		return;
	if(pItem==NULL||(pItem&&pItem->pszText==NULL))
		return;
    // Only set as modified if (a) it actually was, and (b) ESC was not hit.
    if(pItem->lParam != VK_ESCAPE &&
		m_listPlate.GetItemText(pItem->iItem, pItem->iSubItem) != pItem->pszText)
	{
		m_listPlate.UpdateData();
		m_listPlate.SetItemText(pItem->iItem,pItem->iSubItem,pItem->pszText);
	}
	*pResult = 0;
}

//生成环向封版
void CDesignSpecialFlDlg::CreateCoverPlate(CLDSPlate *pNodePlate1,CLDSPlate *pNodePlate2,
										   CLDSParamPlate *&pCoverPlate1,CLDSParamPlate *&pCoverPlate2)
{
	if(pNodePlate1==NULL||pNodePlate2==NULL)
		return;
	CUndoOperObject undo(&Ta);
	if(CoverPlatePara.m_bUpPlate||CoverPlatePara.m_bDownPlate)
	{
		pCoverPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pCoverPlate1->m_iParamType = TYPE_CIRCOVERPLATE;
		pCoverPlate1->SetPartNo(CoverPlatePara.sPartNo);	//编号
		pCoverPlate1->iSeg = pNodePlate1->iSeg;	//段号
		pCoverPlate1->cMaterial = CoverPlatePara.cMaterial;	//材质
		pCoverPlate1->thick = CoverPlatePara.m_nThick;
		pCoverPlate1->m_hPartWeldParent = m_pCurLineTube->handle;
		pCoverPlate1->SetLayer(m_pCurLineTube->layer());
		pCoverPlate1->cfgword=m_pCurLineTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pCoverPlate1->SetDesignItemValue('A',CoverPlatePara.m_fAlfa,"封板旋转角度");
		pCoverPlate1->SetDesignItemValue('B',CoverPlatePara.m_fBeta,"扇形夹角",PARAM_SHAPE);
		pCoverPlate1->SetDesignItemValue('C',m_pCurLineTube->handle,"定位基准基准钢管",PARAM_POS,1);
		pCoverPlate1->SetDesignItemValue('E',m_pBaseNode->handle,"定位基准节点",PARAM_POS,1);
		if(CoverPlatePara.biHoleType==2)
		{	//无流水孔
			pCoverPlate1->SetDesignItemValue(KEY2C("BH"),-1);
			if(pNodePlate1)
				pCoverPlate1->SetDesignItemValue('F',pNodePlate1->handle,"第一块基准钢板",PARAM_POS,1);
			if(pNodePlate2)
				pCoverPlate1->SetDesignItemValue('G',pNodePlate2->handle,"第二块基准钢板",PARAM_POS,1);
		}
		else
		{	//有流水孔
			if(CoverPlatePara.biHoleType==0)		//0.圆孔
				pCoverPlate1->SetDesignItemValue(KEY2C("BH"),CoverPlatePara.BH);
			else if(CoverPlatePara.biHoleType==1)	//>0.半圆孔
				pCoverPlate1->SetDesignItemValue(KEY2C("BH"),0);
			CLDSGeneralPlate::DESIGN_PARAM para_item;
			if(pNodePlate1)
			{
				para_item.cValueType=3;
				para_item.value.hPart=pNodePlate1->handle;
				para_item.value.uiAttribute=CoverPlatePara.uiAttribute1;
				pCoverPlate1->SetDesignItem('F',para_item);
			}
			if(pNodePlate2)
			{
				para_item.cValueType=3;
				para_item.value.hPart=pNodePlate2->handle;
				para_item.value.uiAttribute=CoverPlatePara.uiAttribute2;
				pCoverPlate1->SetDesignItem('G',para_item);
			}
		}
		pCoverPlate1->SetDesignItemValue('D',CoverPlatePara.D,"环向封板参数D",PARAM_SHAPE);
		pCoverPlate1->SetDesignItemValue('W',CoverPlatePara.W,"环向封板参数W",PARAM_SHAPE);
		if(CoverPlatePara.m_bUpPlate)	//设计上封板
			pCoverPlate1->SetDesignItemValue('S',(double)-1,"设计上封板");
		else if(CoverPlatePara.m_bDownPlate)	//设计下封板
			pCoverPlate1->SetDesignItemValue('S',(double)1,"设计下封板");
		pCoverPlate1->SetDesignItemValue('R',m_pCurLineTube->GetDiameter()*0.5,"环向板内圆弧半径",PARAM_SHAPE);
		pCoverPlate1->SetDesignItemValue('M',(double)1,"M为-1时在贯通连板另外一侧加封板");
		pCoverPlate1->m_bDisplayHole=TRUE;
		pCoverPlate1->DesignPlate();
		pCoverPlate1->SetModified();
		pCoverPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pCoverPlate1->GetSolidPartObject());
		g_pSolidDraw->Draw();
	}
	if(CoverPlatePara.m_bUpPlate&&CoverPlatePara.m_bDownPlate)
	{	//上下封一起设计时,在此处设计下封板
		pCoverPlate2=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pCoverPlate2->CopyProperty(pCoverPlate1);
		pCoverPlate2->SetLayer(m_pCurLineTube->layer());
		pCoverPlate2->cfgword=m_pCurLineTube->cfgword;
		pCoverPlate2->SetDesignItemValue('S',(double)1,"设计下封板");
		pCoverPlate2->m_hPartWeldParent = m_pCurLineTube->handle;
		pCoverPlate2->m_bDisplayHole=TRUE;
		pCoverPlate2->DesignPlate();
		pCoverPlate2->SetModified();
		pCoverPlate2->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pCoverPlate2->GetSolidPartObject());
		g_pSolidDraw->Draw();
	}
	g_pSolidDraw->Draw();
	g_pSolidDraw->ReleaseSnapStatus();
}
//生成环向肋板
CLDSParamPlate* CDesignSpecialFlDlg::CreateRibPlate(CLDSPlate *pNodePlate1,CLDSPlate *pNodePlate2)
{
	if(pNodePlate1==NULL||pNodePlate2==NULL)
		return NULL;
	CUndoOperObject undo(&Ta);
	CLDSParamPlate *pCirRibPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pCirRibPlate->m_iParamType = TYPE_CIRRIBPLATE;
	pCirRibPlate->SetPartNo(RibPlatePara.sPartNo);	//编号
	pCirRibPlate->iSeg = pNodePlate1->iSeg;	//段号
	pCirRibPlate->layer(2)=m_pCurLineTube->layer(2);	//调整环向板的图层名
	pCirRibPlate->cMaterial = RibPlatePara.cMaterial;	//材质
	pCirRibPlate->thick = RibPlatePara.m_nThick;
	pCirRibPlate->m_hPartWeldParent = m_pCurLineTube->handle;
	pCirRibPlate->cfgword=m_pCurLineTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	//因修改环向肋板生成代码，此处需调整 wht 11-05-18
	pCirRibPlate->SetDesignItemValue('C',m_pCurLineTube->handle,"定位基准钢管",PARAM_POS,1);
	pCirRibPlate->SetDesignItemValue('P',m_pBaseNode->handle,"定位基准节点",PARAM_POS,1);
	pCirRibPlate->SetDesignItemValue('E',pNodePlate1->handle,"第一块基准钢板",PARAM_POS,1);
	
	double Y=0;
	if(pNodePlate2)	
	{
		pCirRibPlate->SetDesignItemValue('F',pNodePlate2->handle,"第二块基准钢板",PARAM_POS,1);
		pCirRibPlate->SetDesignItemValue('W',RibPlatePara.W,"第一块关联钢板的焊缝长",PARAM_SHAPE);
		pCirRibPlate->SetDesignItemValue('N',RibPlatePara.N,"第二块关联钢板或钢管的焊缝长",PARAM_SHAPE);
		pCirRibPlate->SetDesignItemValue('D',RibPlatePara.D,"基准钢管外壁到自由边距离，简称自由边距",PARAM_SHAPE);
		pCirRibPlate->SetDesignItemValue('H',RibPlatePara.H,"倒角高度",PARAM_SHAPE);
		double G=1;
		if(RibPlatePara.m_bTopCir)
			G=-1;	//顶部圆弧连接
		pCirRibPlate->SetDesignItemValue('G',G,"自由边类型(>0直线，<=0圆弧)");
	}
	else
	{	
		pCirRibPlate->SetDesignItemValue('F',0,"第二块基准钢板",PARAM_POS,1);
		pCirRibPlate->SetDesignItemValue('W',RibPlatePara.W,"第一块关联钢板的焊缝长",PARAM_SHAPE);
		pCirRibPlate->SetDesignItemValue('D',RibPlatePara.D,"基准钢管外壁到自由边距离，简称自由边距",PARAM_SHAPE);
		pCirRibPlate->SetDesignItemValue('H',RibPlatePara.H,"倒角高度",PARAM_SHAPE);
		if(RibPlatePara.m_bOutterRibPlate)
			Y=1;
		else 
			Y=-1;
	}
	pCirRibPlate->SetDesignItemValue('Y',Y,"半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)");
	pCirRibPlate->SetDesignItemValue('S',RibPlatePara.S,"沿基准钢管延伸方向的偏移距离。");
	pCirRibPlate->SetDesignItemValue('V',0,"加工间隙",PARAM_SHAPE);
	pCirRibPlate->ucs.axis_z=RibPlatePara.norm;
	normalize(pCirRibPlate->ucs.axis_z);
	//设置连接方向参数
	CONNECT_DIRECTION connectVec;
	connectVec.style=1;
	connectVec.serial=1;
	connectVec.direct.norm_style=0;
	connectVec.direct.vector=pCirRibPlate->ucs.axis_z;
	pCirRibPlate->ConnDirectList.SetValue(connectVec.serial,connectVec);
	pCirRibPlate->DesignPlate();
	if(pCirRibPlate->ucs.axis_z*RibPlatePara.norm<0)
	{
		pCirRibPlate->SetDesignItemValue('S',-1.0*RibPlatePara.S,"沿基准钢管延伸方向的偏移距离。");
		pCirRibPlate->DesignPlate();
	}
	pCirRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pCirRibPlate->GetSolidPartObject());
	if(RibPlatePara.m_bMirCreate)
	{
		CLDSParamPlate *pMirPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pMirPlate->CopyProperty(pCirRibPlate);
		pMirPlate->layer(2)=m_pCurLineTube->layer(2);
		pMirPlate->m_hPartWeldParent=m_pCurLineTube->handle;
		pMirPlate->SetDesignItemValue('Y',-Y,"半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)");
		pMirPlate->ucs.axis_z=pMirPlate->ucs.axis_z;
		//设置连接方向参数
		connectVec.direct.vector=pMirPlate->ucs.axis_z;
		pMirPlate->ConnDirectList.SetValue(connectVec.serial,connectVec);
		pMirPlate->DesignPlate();
		pMirPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pMirPlate->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return pCirRibPlate;
}

//更新肋板W、N参数 钢板需要延伸时计算出的W、N值不准确  wht 10-04-14
void CDesignSpecialFlDlg::UpdateRibPlateWN()
{
	UCS_STRU ucs;	//假定的肋板坐标系
	ucs.axis_z=RibPlatePara.norm;
	normalize(ucs.axis_z);
	ucs.origin = m_pBaseNode->Position(true);
	if(m_pBaseNode&&m_pCurLineTube)
	{
		m_pCurLineTube->BuildUCS();
		SnapPerp(&ucs.origin,m_pCurLineTube->Start(),m_pCurLineTube->End(),m_pBaseNode->Position(true));
		ucs.origin+=ucs.axis_z*RibPlatePara.S;
	}
	double r=0;
	if(m_pCurLineTube)
		r=m_pCurLineTube->GetDiameter()*0.5;
	//基准钢板在钢管垂直截面内的射线方向
	f3dPoint axis_x[2]; 
	for(int i=0;i<2;i++)
	{	//修正射线方向的正负号
		GEPOINT vertice,vertice_vec;
		if(m_pNodePlate[i]==NULL||(m_pNodePlate[i]&&m_pNodePlate[i]->vertex_list.GetNodeNum()<0))
			continue;
		axis_x[i]=ucs.axis_z^m_pNodePlate[i]->ucs.axis_z;
		normalize(axis_x[i]);
		vertice=m_pNodePlate[i]->vertex_list.GetFirst()->vertex;
		coord_trans(vertice,m_pNodePlate[i]->ucs,TRUE);
		vertice_vec=vertice-ucs.origin;
		if(vertice_vec*axis_x[i]<0)
			axis_x[i]*=-1.0;
		//计算肋板与第一块钢板相交边的长度
		ucs.axis_x = axis_x[i];
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);		
		f3dPoint inter_pos,base_pos;
		base_pos=ucs.origin;
		PROFILE_VERTEX *pVertex=NULL,*pPreVertex=NULL;
		pPreVertex=m_pNodePlate[i]->vertex_list.GetTail(); //钢板的第一个顶点的前一个顶点为最后一个顶点
		for(pVertex=m_pNodePlate[i]->vertex_list.GetFirst();pVertex;pVertex=m_pNodePlate[i]->vertex_list.GetNext())
		{
			if(pPreVertex&&pPreVertex!=pVertex)
			{
				f3dLine line(pPreVertex->vertex,pVertex->vertex);
				coord_trans(line.startPt,m_pNodePlate[i]->ucs,TRUE);
				coord_trans(line.endPt,m_pNodePlate[i]->ucs,TRUE);
				project_point(base_pos,line.startPt,m_pNodePlate[i]->ucs.axis_z);
				int ret=Int3dll(line.startPt,line.endPt,ucs.origin,ucs.origin+axis_x[i]*10000,inter_pos);
				if(ret>0)
				{
					double dist=DISTANCE(inter_pos,ucs.origin);
					if(dist>r)
					{
						if(i==0)
							RibPlatePara.W=dist-r;
						else
							RibPlatePara.N=dist-r;
						break;
					}
				}
				pPreVertex=pVertex;
			}
		}
	}
	//更新至属性栏显示
	CString sTextValue="";
	sTextValue.Format("%.f",RibPlatePara.W);
	m_propList.SetItemPropValue(GetPropID("RibPlatePara.W"),sTextValue);
	sTextValue.Format("%.f",RibPlatePara.N);
	m_propList.SetItemPropValue(GetPropID("RibPlatePara.N"),sTextValue);
}
#endif
#ifdef YIXINGFALAN	//这部分代码留在以后完善参数化异型法兰时使用 wjh-2015.8.7
typedef struct tagLinePartPara{
	BOOL bStart;
	CLDSLinePart *pLinePart;
	f3dPoint line_vec;
	CLDSParamPlate *pParamPlate;
}LINEPART_PARA;

static int CompareLinePartPara(const LINEPART_PARA &linePartPara1,const LINEPART_PARA &linePartPara2)
{
	double fAngle1=Cal2dLineAng(f2dPoint(0,0),f2dPoint(linePartPara1.line_vec.x,linePartPara1.line_vec.y));
	double fAngle2=Cal2dLineAng(f2dPoint(0,0),f2dPoint(linePartPara2.line_vec.x,linePartPara2.line_vec.y));
	if(fAngle1>fAngle2)
		return 1;
	else if(fAngle1<fAngle2)
		return -1;
	else
		return 0;
}
//生成异形法兰
CLDSPlate *CDesignSpecialFlDlg::CreateSpecialFl(LINEPARTSET *pLinePartSet,double fNormOffset/*=0*/)
{
	if(pLinePartSet==NULL)
		return NULL;
	if(pLinePartSet->GetNodeNum()<=0||m_pCoupleTube==NULL)
		return NULL;	//未指定对接钢管或者无射线杆件
	CLDSPlate *pSpecialFl=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	pSpecialFl->CopyProperty(&m_plate);
	pSpecialFl->cfgword=m_pBaseNode->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	pSpecialFl->face_N=1;
	pSpecialFl->m_hPartWeldParent=m_pCoupleTube->handle;
	pSpecialFl->designInfo.m_bEnableFlexibleDesign=FALSE;	//不启用柔性设计
	pSpecialFl->designInfo.iProfileStyle0123=3;				//不设计钢板外形
	pSpecialFl->designInfo.m_hBaseNode=m_pBaseNode->handle;	//板的基准节点
	//pSpecialFl->designInfo.m_hBasePart=m_pCoupleTube->handle;
	//基准点
	pSpecialFl->designInfo.origin.datum_pos_style=2;
	pSpecialFl->designInfo.origin.des_para.RODNODE.hNode=m_pBaseNode->handle;
	pSpecialFl->designInfo.origin.des_para.RODNODE.hLinePart=m_pCoupleTube->handle;
	//法线设计参数
	pSpecialFl->designInfo.norm.norm_style=0;
	pSpecialFl->designInfo.norm.vector=m_plate.ucs.axis_z;
	//建立坐标系
	f3dPoint tube_len_vec;
	if(m_pBaseNode==m_pCoupleTube->pStart)
	{
		pSpecialFl->designInfo.origin.des_para.RODNODE.direction=1;	//终-->始
		tube_len_vec=m_pCoupleTube->Start()-m_pCoupleTube->End();
	}
	else if(m_pBaseNode==m_pCoupleTube->pEnd)
	{
		pSpecialFl->designInfo.origin.des_para.RODNODE.direction=0;	//始-->终
		tube_len_vec=m_pCoupleTube->End()-m_pCoupleTube->Start();
	}
	normalize(tube_len_vec);
	pSpecialFl->ucs.axis_z=m_plate.ucs.axis_z;
	pSpecialFl->ucs.axis_x=m_pCoupleTube->ucs.axis_x;
	pSpecialFl->ucs.axis_y=pSpecialFl->ucs.axis_z^pSpecialFl->ucs.axis_x;
	normalize(pSpecialFl->ucs.axis_y);
	pSpecialFl->ucs.axis_x=pSpecialFl->ucs.axis_y^pSpecialFl->ucs.axis_z;
	normalize(pSpecialFl->ucs.axis_x);
	pSpecialFl->designInfo.origin.UpdatePos(console.GetActiveModel());
	pSpecialFl->ucs.origin=pSpecialFl->designInfo.origin.Position();
	pSpecialFl->m_fNormOffset=fNormOffset;
	//异形法兰内环
	pSpecialFl->m_tInnerColumnNorm=tube_len_vec;
	pSpecialFl->m_fInnerRadius=m_fInnerRadius;
	//分析射线杆件
	CArray<LINEPART_PARA,LINEPART_PARA&> linePartArr;
	CLDSLinePart *pLinePart=NULL;
	m_pCoupleTube->BuildUCS();
	for(pLinePart=pLinePartSet->GetFirst();pLinePart;pLinePart=pLinePartSet->GetNext())
	{
		LINEPART_PARA linePartPara;
		linePartPara.pLinePart=pLinePart;
		linePartPara.bStart=TRUE;
		linePartPara.line_vec=pLinePart->End()-pLinePart->Start();
		normalize(linePartPara.line_vec);
		if(pLinePart->pEnd==m_pBaseNode)
		{
			linePartPara.bStart=FALSE;
			linePartPara.line_vec*=-1.0;
		}
		if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
		{
			CLDSLineTube *pLineTube=(CLDSLineTube*)pLinePart;
			if(linePartPara.bStart)
				linePartPara.pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
			else
				linePartPara.pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		}
		//将射线方向转换到钢板坐标系下
		vector_trans(linePartPara.line_vec,pSpecialFl->ucs,FALSE);
		linePartArr.Add(linePartPara);
	}
	//按射线方向角度排序
	CHeapSort<LINEPART_PARA>::HeapSort(linePartArr.GetData(),linePartArr.GetSize(),CompareLinePartPara);
	for(int i=0;i<linePartArr.GetSize();i++)
	{
		if(i<0||i>linePartArr.GetUpperBound())
			continue;
		LINEPART_PARA linePartPara=linePartArr.GetAt(i);
		if(linePartPara.pParamPlate==NULL)
			continue;	//暂时仅处理插板连接方式
		CLsRef *pLsRef=NULL;
		int head_bolt_d=16,end_bolt_d=16;
		double minx=1000000,maxx=0,miny=0.5*H,maxy=-0.5*H;
		for(pLsRef=linePartPara.pParamPlate->GetFirstLsRef();pLsRef;pLsRef=linePartPara.pParamPlate->GetNextLsRef())
		{
			pSpecialFl->AppendLsRef(*pLsRef);	//添加螺栓引用
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			//钢管正负头修改后螺栓位置和法线方向都有可能变化
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			f3dPoint pos = pBolt->ucs.origin;
			coord_trans(pos,linePartPara.pParamPlate->ucs,FALSE);
			if(pos.x<minx)
			{
				head_bolt_d=pBolt->get_d();
				minx=pos.x;
			}
			if(pos.x>maxx)
			{
				end_bolt_d=pBolt->get_d();
				maxx=pos.x;
			}
			if(pos.y<miny)
				miny=pos.y;
			if(pos.y>maxy)
				maxy=pos.y;
		}
		if(minx>maxx)
			minx=maxx;
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,head_bolt_d);
		f3dPoint vertex_up,vertex_down;
		vertex_up.x=vertex_down.x=minx-LsSpace.EndSpace;
		double fPlateWidth=0;
		linePartPara.pParamPlate->GetDesignItemValue('H',&fPlateWidth);
		vertex_up.y=0.5*fPlateWidth;
		vertex_down.y=-vertex_up.y;
		coord_trans(vertex_up,linePartPara.pParamPlate->ucs,TRUE);
		coord_trans(vertex_down,linePartPara.pParamPlate->ucs,TRUE);
		coord_trans(vertex_up,pSpecialFl->ucs,FALSE);
		coord_trans(vertex_down,pSpecialFl->ucs,FALSE);
		vertex_down.z=vertex_up.z=0;
		f3dPoint verfiy_vec(-linePartPara.line_vec.y,linePartPara.line_vec.x);
		f3dPoint cur_vec=vertex_down-vertex_up;
		normalize(cur_vec);
		if(cur_vec*verfiy_vec<0)
		{	//交换上下顶点
			f3dPoint vertex_temp=vertex_up;
			vertex_up=vertex_down;
			vertex_down=vertex_temp;
		}
		vertex_down.feature=1;
		vertex_up.feature=1;
		pSpecialFl->vertex_list.append(PROFILE_VERTEX(vertex_up));
		pSpecialFl->vertex_list.append(PROFILE_VERTEX(vertex_down));
	}
	if(pSpecialFl->vertex_list.GetNodeNum()<3)
		return NULL;
	//求两切点
	f3dPoint start_tang_vertex,end_tang_vertex,mid_vertex;
	f3dPoint first_vertex=pSpecialFl->vertex_list.GetFirst()->vertex;
	f3dPoint tail_vertex=pSpecialFl->vertex_list.GetTail()->vertex;
	//
	f2dPoint retPt;
	f3dPoint pick_vec(first_vertex.y,-first_vertex.x,0),pickPt;
	normalize(pick_vec);
	pickPt=f3dPoint(0,0,0)+pick_vec*1.5*m_fExternalRadius;
	f2dCircle cir(m_fExternalRadius,f2dPoint(0,0));
	Tang2dpc(f2dPoint(first_vertex.x,first_vertex.y),cir,f2dPoint(pickPt.x,pickPt.y),retPt);
	start_tang_vertex.Set(retPt.x,retPt.y,0);
	//
	pick_vec.Set(-tail_vertex.y,tail_vertex.x);
	pickPt=f3dPoint(0,0,0)+pick_vec*1.5*m_fExternalRadius;
	Tang2dpc(f2dPoint(tail_vertex.x,tail_vertex.y),cir,f2dPoint(pickPt.x,pickPt.y),retPt);
	end_tang_vertex.Set(retPt.x,retPt.y,0);
	f3dPoint mid_vec=start_tang_vertex+end_tang_vertex;
	normalize(mid_vec);
	mid_vertex=f3dPoint(0,0,0)+mid_vec*m_fExternalRadius;
	mid_vertex.z=0;
	//添加顶点
	PROFILE_VERTEX *pVertex=pSpecialFl->vertex_list.append();
	pVertex->vertex=end_tang_vertex;
	pVertex->type=2;	//指定半径圆弧,center为相对坐标 wht 12-06-15
	pVertex->center.Set();
	pVertex->radius=m_fExternalRadius;
	pVertex->vertex.feature=1;
	//
	pVertex=pSpecialFl->vertex_list.append();
	pVertex->vertex=mid_vertex;
	pVertex->type=2;	//指定半径圆弧,center为相对坐标 wht 12-06-15
	pVertex->center.Set();
	pVertex->radius=m_fExternalRadius;
	pVertex->vertex.feature=1;
	//
	start_tang_vertex.feature=1;
	pSpecialFl->vertex_list.append(PROFILE_VERTEX(start_tang_vertex));
	pSpecialFl->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pSpecialFl->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return pSpecialFl;
}
#endif
