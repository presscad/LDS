#include "stdafx.h"
#include "LDS.h"
#include "I_DrawSolid.h"
#include "btc.h"
#include "WalkwayPropertyList.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "CallBackDialog.h"

///////////////////////////////////////////////////////////////////////////
#ifdef __COMPLEX_PART_INC_
CWalkWayPropertyList::CWalkWayPropertyList(void)
{
	ResetParams();
}
void CWalkWayPropertyList::ResetParams()
{
	params.m_ciWalkWayType=1;
	params.m_bCreateWalkWay=TRUE;
	params.m_fYA=600;
	params.m_fZH=300;
	params.m_fZh=300;
	params.m_fYB=150;
	params.m_fWidth=400;
	params.m_fLength=4000;
	params.m_nBoltMd=16;
	params.m_nBoltNum=2;
	params.m_fBoltSpace=80;
	params.m_fD1=40;
	params.m_fD2=30;
	params.m_sWalkWayJgSpec.Copy("100x6");
	params.m_sHolderJgSpec.Copy("70x5");
	params.m_cWalkWayJgMat='H';
	params.m_cHolderJgMat='H';
	params.m_ciHolderAngleLayoutStyle=0;	//相对当前视向默认支架角钢为外铁
	params.m_cPlateMat='H';
	params.m_nThick=8;
	params.para.A=90;
	params.para.B=180;
	params.para.H=60;
	params.para.W=30;
	//
	params.m_pBaseTube=NULL;
	params.m_pBaseNode=NULL;
	params.m_pOuterJg=params.m_pInnerJg=NULL;
	m_hBaseTube=m_hBaseNode=m_hOuterAngle=m_hInnerAngle=0;
	m_hOuterBeamDatumNode=m_hInnerBeamDatumNode=0;
	params.m_fZdOffDist=700;
	params.m_fZjOffDist=400;
}

CWalkWayPropertyList::~CWalkWayPropertyList(void)
{
}
bool CWalkWayPropertyList::VerifyRelaRodAndNodes()
{
	if((params.m_pBaseTube=(CLDSLineTube*)m_pBelongModel->FromRodHandle(m_hBaseTube))==NULL)
		return false;
	if((params.m_pBaseNode=m_pBelongModel->FromNodeHandle(m_hBaseNode))==NULL)
		return false;
	if((params.m_pOuterJg=(CLDSLineAngle*)m_pBelongModel->FromPartHandle(m_hOuterAngle,CLS_LINEANGLE))==NULL)
		return false;
	if((params.m_pInnerJg=(CLDSLineAngle*)m_pBelongModel->FromPartHandle(m_hInnerAngle,CLS_LINEANGLE))==NULL)
		return false;
	return true;
}
const DWORD HASHTABLESIZE = 500;
long CWalkWayPropertyList::InitPropHashtable(long initId/*=1*/,long initHashSize/*=50*/,long initStatusHashSize/*=50*/)	//返回下一空闲Id
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(initStatusHashSize);
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
	AddPropItem("ciHolderAngleLayoutStyle",PROPLIST_ITEM(id++,"横撑角钢朝向","","0.朝外|1.朝内"));
	AddPropItem("ciOtherAngleLayoutStyle", PROPLIST_ITEM(id++, "斜撑角钢朝向", "", "0.朝外|1.朝内"));
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
	return id;
}
long CWalkWayPropertyList::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("WalkWayType")==id)
	{
		if(params.m_ciWalkWayType==0)
			sText.Copy("0.钢板支架式");
		else if(params.m_ciWalkWayType==1)
			sText.Copy("1.三角支架式");
		else
			sText.Copy("2.台型支架式");
	}
	else if(GetPropID("CreateWalkWay")==id)
	{
		if(params.m_bCreateWalkWay)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
	else if(GetPropID("WalkWayWidth")==id)
	{
		sText.Printf("%f",params.m_fWidth);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("WalkWayLength")==id)
	{
		sText.Printf("%f",params.m_fLength);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("WalkWayJgSpec")==id)
		sText.Printf("%s",(char*)params.m_sWalkWayJgSpec);
	else if(GetPropID("WalkWayJgMat")==id)
		QuerySteelMatMark(params.m_cWalkWayJgMat,sText);
	else if(GetPropID("YA")==id)
	{
		sText.Printf("%f",params.m_fYA);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ZH")==id)
	{
		sText.Printf("%f",params.m_fZH);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("BoltSpace")==id)
	{
		sText.Printf("%g",params.m_fBoltSpace*0.5);
		//SimplifiedNumString(sText);
	}
	else if(GetPropID("BoltMd")==id)
		sText.Printf("%d",params.m_nBoltMd);
	else if(GetPropID("BoltNum")==id)
		sText.Printf("%d",params.m_nBoltNum);
	else if(GetPropID("HolderJgSpec")==id)
		sText.Printf("%s",(char*)params.m_sHolderJgSpec);
	else if(GetPropID("HolderJgMat")==id)
		QuerySteelMatMark(params.m_cHolderJgMat,sText);
	else if(GetPropID("ciHolderAngleLayoutStyle")==id)
	{
		if(params.m_ciHolderAngleLayoutStyle!=0)
			sText="1.朝内";
		else
			sText="0.朝外";
	}
	else if (GetPropID("ciOtherAngleLayoutStyle") == id)
	{
		if(params.m_ciOtherAngleLayoutStyle!=0)
			sText = "1.朝内";
		else
			sText = "0.朝外";
	}
	else if(GetPropID("PlateThick")==id)
		sText.Printf("%d",params.m_nThick);
	else if(GetPropID("PlateMat")==id)
		QuerySteelMatMark(params.m_cPlateMat,sText);
	else if(GetPropID("BaseTube")==id)
	{
		long hTube=params.m_pBaseTube?params.m_pBaseTube->handle:0;
		sText.Printf("0X%X",hTube);
	}
	else if(GetPropID("BaseNode")==id)
	{
		long hNode=params.m_pBaseNode?params.m_pBaseNode->handle:0;
		sText.Printf("0X%X",hNode);
	}
	else if(GetPropID("LinkAngle1")==id)
	{
		long hJg=params.m_pOuterJg?params.m_pOuterJg->handle:0;
		sText.Printf("0X%X",hJg);
	}
	else if(GetPropID("LinkAngle2")==id)
	{
		long hJg=params.m_pInnerJg?params.m_pInnerJg->handle:0;
		sText.Printf("0X%X",hJg);
	}
	else if(GetPropID("OffDistX+")==id)
	{
		sText.Printf("%f",params.m_fZjOffDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("OffDistX-")==id)
	{
		sText.Printf("%f",params.m_fZdOffDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.A")==id)
	{
		sText.Printf("%f",params.para.A);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.B")==id)
	{
		sText.Printf("%f",params.para.B);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("Zh")==id)
	{
		sText.Printf("%f",params.m_fZh);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("D1")==id)
	{
		sText.Printf("%f",params.m_fD1);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("D2")==id)
	{
		sText.Printf("%f",params.m_fD2);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("YB")==id)
	{
		sText.Printf("%f",params.m_fYB);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.W")==id)
	{
		sText.Printf("%f",params.para.W);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("para.H")==id)
	{
		sText.Printf("%f",params.para.H);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
BOOL CWalkWayPropertyList::HelpPromptProperty(CPropTreeItem* pItem)
{
	g_pSolidDraw->ReleaseSnapStatus();
	if(pItem->m_idProp==GetPropID("BaseTube"))
		g_pSolidDraw->SetEntSnapStatus(this->m_hBaseTube);
	else if(pItem->m_idProp==GetPropID("BaseNode"))
		g_pSolidDraw->SetEntSnapStatus(this->m_hBaseNode);
	else if(pItem->m_idProp==GetPropID("LinkAngle1"))
		g_pSolidDraw->SetEntSnapStatus(this->m_hOuterAngle);
	else if(pItem->m_idProp==GetPropID("LinkAngle2"))
		g_pSolidDraw->SetEntSnapStatus(this->m_hInnerAngle);
	return true;
}
#include "PropertyListOper.h"
bool CWalkWayPropertyList::ModifyValueFunc(CPropTreeItem* pItem, CString &valueStr,BOOL *lpRslt)
{
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	CNativePropListManager oper(this);//(pPropList,pDlg);
	if (pItem->m_idProp == GetPropID("WalkWayType"))
	{
		params.m_ciWalkWayType = tem_str[0] - '0';
		UpdateHolderProp(pItem);
		DisplaySlavePropertyList(pItem);
		UpdateSketchMap();
	}
	else if (pItem->m_idProp == GetPropID("YA"))
		params.m_fYA = atof(tem_str);
	else if (pItem->m_idProp == GetPropID("ZH"))
		params.m_fZH = atof(tem_str);
	else if (pItem->m_idProp == GetPropID("CreateWalkWay"))
	{
		if (valueStr.CompareNoCase("是") == 0)
			params.m_bCreateWalkWay = TRUE;
		else
			params.m_bCreateWalkWay = FALSE;
		//
		UpdateWalkWayProp(pItem);
		CPropTreeItem* pFindItem = FindItemByPropId(GetPropID("AssemblyInfo"), NULL);
		UpdateAssemblyProp(pFindItem);
	}
	else if (pItem->m_idProp == GetPropID("WalkWayWidth"))
		params.m_fWidth = atof(tem_str);
	else if (pItem->m_idProp == GetPropID("WalkWayLength"))
		params.m_fLength = atof(tem_str);
	else if (pItem->m_idProp == GetPropID("WalkWayJgSpec"))
		params.m_sWalkWayJgSpec.Copy(tem_str);
	else if (pItem->m_idProp == GetPropID("WalkWayJgMat"))
		params.m_cWalkWayJgMat = QuerySteelBriefMatMark(tem_str);
	else if (pItem->m_idProp == GetPropID("HolderJgSpec"))
		params.m_sHolderJgSpec.Copy(tem_str);
	else if (pItem->m_idProp == GetPropID("HolderJgMat"))
		params.m_cHolderJgMat = QuerySteelBriefMatMark(tem_str);
	else if (pItem->m_idProp == GetPropID("ciHolderAngleLayoutStyle"))
	{ 
		params.m_ciHolderAngleLayoutStyle = tem_str[0] - '0';
		params.m_ciOtherAngleLayoutStyle = params.m_ciHolderAngleLayoutStyle;
		oper.UpdatePropItemValue("ciOtherAngleLayoutStyle");
	}
	else if (pItem->m_idProp == GetPropID("ciOtherAngleLayoutStyle"))
		params.m_ciOtherAngleLayoutStyle = tem_str[0] - '0';
	else if(pItem->m_idProp==GetPropID("PlateThick"))
		params.m_nThick=atoi(tem_str);
	else if(pItem->m_idProp==GetPropID("PlateMat"))
		params.m_cPlateMat=QuerySteelBriefMatMark(tem_str);
	else if(pItem->m_idProp==GetPropID("BoltMd"))
		params.m_nBoltMd=atoi(tem_str);
	else if(pItem->m_idProp==GetPropID("BoltSpace"))
		params.m_fBoltSpace=atof(tem_str)*2;
	else if(pItem->m_idProp==GetPropID("BoltNum"))
		params.m_nBoltNum=atoi(tem_str);
	else if(pItem->m_idProp==GetPropID("para.A"))
		params.para.A=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("para.B"))
		params.para.B=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("para.W"))
		params.para.W=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("para.H"))
		params.para.H=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("Zh"))
		params.m_fZh=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("D1"))
		params.m_fD1=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("D2"))
		params.m_fD2=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("YB"))
		params.m_fYB=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("OffDistX+"))
		params.m_fZjOffDist=atof(tem_str);
	else if(pItem->m_idProp==GetPropID("OffDistX-"))
		params.m_fZdOffDist=atof(tem_str);
	return TRUE;
}
bool CWalkWayPropertyList::ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt)
{
	CCallBackDialog* pCallBackDlg=(CCallBackDialog*)GetParent();
	if(pItem->m_idProp==GetPropID("BaseTube"))
		pCallBackDlg->SelectObject(CLS_LINETUBE,pItem->m_idProp);
	else if(pItem->m_idProp==GetPropID("BaseNode"))
		pCallBackDlg->SelectObject(CLS_NODE,pItem->m_idProp);
	else if(pItem->m_idProp==GetPropID("LinkAngle1"))
		pCallBackDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);
	else if(pItem->m_idProp==GetPropID("LinkAngle2"))
		pCallBackDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);
	return TRUE;
}
GEPOINT CWalkWayPropertyList::CalNearVecX()
{
	GEPOINT nearVecX(1,0,0);
	if(params.m_pBaseTube==NULL)
		return nearVecX;
	GEPOINT ptS=params.m_pBaseTube->xPosStart,ptE=params.m_pBaseTube->xPosEnd;
	GEPOINT line_vec=(params.m_pBaseTube->End()-params.m_pBaseTube->Start()).normalized();
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
void CWalkWayPropertyList::UpdateTemplParams()
{
	//走道设计参数
	STDTEMPL_PARAM_ITEM* pParam=NULL;
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("W"));	//走道宽度
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.m_fWidth;
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("L"));	//走道长度
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.m_fLength;
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("YA"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.m_fYA;	//横向偏移
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("ZH"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.m_fZH;	//竖向高度偏移
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("Ls"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=(params.m_fZjOffDist-params.m_fZdOffDist);	//走道始端轴向偏移距离
	//支架设计参数
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("Zh"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.m_fZh;	//斜撑下侧Z向偏移
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("D1"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.m_fD1;	//斜撑下侧横向偏移
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("D2"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.m_fD2;	//斜撑上侧端点沿水平杆向内的横向偏移
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("YB"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.m_fYB;	//支架位置轴向偏移距离
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("X0"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.m_fZjOffDist;	//支架位置轴向偏移距离
	//
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("Md"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;
	pParam->value.liValue=params.m_nBoltMd;	//钢管上连接板上侧横螺直径
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("BN"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.m_nBoltNum;	//钢管上连接板上侧横螺栓数
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("BS"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.m_fBoltSpace;//钢管上连接板上侧横螺栓间距
	//走道板设计参数
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("PA"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.para.A;	//走道焊接板扇形夹角
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("PB"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.para.B;	//螺栓连接板扇形夹角
	pParam=m_xWalkWayTempl.listParams.Add(KEY4C("PW"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	pParam->value.fValue=params.para.W;	//
	//装配坐标系
	/*pParam=m_xWalkWayTempl.listParams.Add(KEY4C("acsO"));	//标准组件装配坐标系原点
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
	*/
}
bool CWalkWayPropertyList::UnifyParaAngleLayoutByAntiAxisX(CStdTemplParaRod* pParaRod)
{
	if(!pParaRod->IsAngleObject())
		return false;
	bool bToOutward = (params.m_ciHolderAngleLayoutStyle == 0)?true:false;
	CStdTemplParaAngle* pParaAngle=(CStdTemplParaAngle*)pParaRod;
	if(pParaAngle->normal.ciNormalDirection=='Z')
	{
		bool reverseXnY=true;
		if(pParaAngle->ciAngleLayout==1&& bToOutward)
			pParaAngle->ciAngleLayout=4;
		else if(pParaAngle->ciAngleLayout==2&& bToOutward)
			pParaAngle->ciAngleLayout=3;
		else if(pParaAngle->ciAngleLayout==4&&!bToOutward)
			pParaAngle->ciAngleLayout=1;
		else if(pParaAngle->ciAngleLayout==3&&!bToOutward)
			pParaAngle->ciAngleLayout=2;
		else
			reverseXnY=false;
		if( pParaRod->keyLabel==CTubeWalkWayStdComTempl::BRACE_UPPER_HORI_ANGLE_ID)
		{
			if(!bToOutward&&pParaAngle->ciAngleLayout==2)
			{
				pParaAngle->desStart.cTouchLineType=0;
				pParaAngle->desStart.cTouchLine=1;	//交于走道外主梁角钢Y肢心线
				pParaAngle->desStart.szXorCtrlWingOffset[0]='g';
				pParaAngle->desStart.szYorOthrWingOffset[0]='0';
				pParaAngle->desEnd.cTouchLineType = 3;//控制点
				pParaAngle->desEnd.coordCtrlType = 3;	//控制点投影
				pParaAngle->desEnd.szXorCtrlWingOffset[0]='g';
				pParaAngle->desEnd.szYorOthrWingOffset[0]='g';
			}
			else if(bToOutward&&pParaAngle->ciAngleLayout==3)
			{
				pParaAngle->desStart.cTouchLineType=0;
				pParaAngle->desStart.cTouchLine=1;	//交于走道外主梁角钢Y肢心线
				pParaAngle->desStart.szXorCtrlWingOffset[0]='0';
				pParaAngle->desStart.szYorOthrWingOffset[0]='g';
				pParaAngle->desEnd.cTouchLineType = 3;//控制点
				pParaAngle->desEnd.coordCtrlType = 3;	//控制点投影
				pParaAngle->desEnd.szXorCtrlWingOffset[0]='g';
				pParaAngle->desEnd.szYorOthrWingOffset[0]='g';
			}
		}
		for (WORD i = 0; reverseXnY&&i < pParaAngle->m_layoutBoltArr.Count; i++)
		{
			BOLT_LAYOUT* pLayout = pParaAngle->m_layoutBoltArr.GetAt(i);
			pLayout->ciDatumWing = 'X' + ('Y' - pLayout->ciDatumWing);
		}
	}
	else if (pParaAngle->normal.ciNormalDirection == 'x'&&pParaRod->keyLabel == CTubeWalkWayStdComTempl::BRACE_GRADIENT_ANGLE_ID)
	{	//斜撑角钢
		bool bOtherToOutward = (params.m_ciOtherAngleLayoutStyle == 0)?true:false;
		if (params.m_ciWalkWayType == 2||(params.m_ciWalkWayType == 1&&bOtherToOutward==bToOutward))
		{	//斜撑角钢与横撑角钢朝向一致
			if (pParaAngle->ciAngleLayout == 2 && bToOutward)
				pParaAngle->ciAngleLayout = 1;
			else if (pParaAngle->ciAngleLayout == 1 && !bToOutward)
				pParaAngle->ciAngleLayout = 2;
			if (bToOutward&&pParaAngle->ciAngleLayout == 1)
			{
				pParaAngle->desStart.cTouchLineType = 0;
				pParaAngle->desStart.cTouchLine = 1;
				pParaAngle->desStart.szXorCtrlWingOffset = "-R4#T";//"g";
				pParaAngle->desStart.szYorOthrWingOffset = "g";//"-g,-P2#T";
				pParaAngle->desEnd.cTouchLineType = 3;	//控制点
				pParaAngle->desEnd.coordCtrlType = 3;	//控制点投影
				pParaAngle->desEnd.szXorCtrlWingOffset = "g";
				pParaAngle->desEnd.szYorOthrWingOffset = "g";//"-g,-P2#T";
			}
			else if (!bToOutward&&pParaAngle->ciAngleLayout == 2)
			{
				pParaAngle->desStart.cTouchLineType = 0;
				pParaAngle->desStart.cTouchLine = 0;
				pParaAngle->desStart.szYorOthrWingOffset = "-R4#T";//"g";
				pParaAngle->desStart.szXorCtrlWingOffset = "g";//"-g,-P2#T";
				pParaAngle->desEnd.cTouchLineType = 3;	//控制点
				pParaAngle->desEnd.coordCtrlType = 3;	//控制点投影
				pParaAngle->desEnd.szXorCtrlWingOffset = "g";
				pParaAngle->desEnd.szYorOthrWingOffset = "g";//"-g,-P2#T";
			}
		}
		else
		{	//三角支架斜撑角钢与横撑角钢朝向不一致
			if (pParaAngle->ciAngleLayout == 2 && bOtherToOutward)
				pParaAngle->ciAngleLayout = 1;
			else if (pParaAngle->ciAngleLayout == 1 && !bOtherToOutward)
				pParaAngle->ciAngleLayout = 2;
			if (bOtherToOutward&&pParaAngle->ciAngleLayout == 1)
			{	//横财朝内，斜材超外
				pParaAngle->desStart.cTouchLineType = 0;
				pParaAngle->desStart.cTouchLine = 0;
				pParaAngle->desStart.szXorCtrlWingOffset ="0";
				pParaAngle->desStart.szYorOthrWingOffset ="g";
				pParaAngle->desEnd.cTouchLineType = 3;
				pParaAngle->desEnd.coordCtrlType = 3;
				pParaAngle->desEnd.szXorCtrlWingOffset = "-g,-P2#T";
				pParaAngle->desEnd.szYorOthrWingOffset = "g";
			}
			else if (!bOtherToOutward&&pParaAngle->ciAngleLayout == 2)
			{	//横财朝外，斜材超内
				pParaAngle->desStart.cTouchLineType = 0;
				pParaAngle->desStart.cTouchLine = 1;
				pParaAngle->desStart.szXorCtrlWingOffset = "g";
				pParaAngle->desStart.szYorOthrWingOffset = "0";
				pParaAngle->desEnd.cTouchLineType = 3;
				pParaAngle->desEnd.coordCtrlType = 3;
				pParaAngle->desEnd.szXorCtrlWingOffset = "g";
				pParaAngle->desEnd.szYorOthrWingOffset = "-g,-P2#T";
			}
		}
		for (WORD i = 0; i < pParaAngle->m_layoutBoltArr.Count; i++)
		{
			BOLT_LAYOUT* pLayout = pParaAngle->m_layoutBoltArr.GetAt(i);
			if (pParaAngle->ciAngleLayout == 1 || pParaAngle->ciAngleLayout == 3)
				pLayout->ciDatumWing = 'X';
			else if (pParaAngle->ciAngleLayout == 2 || pParaAngle->ciAngleLayout == 4)
				pLayout->ciDatumWing = 'Y';
		}
	}
	else if (pParaAngle->normal.ciNormalDirection == 'X'&& pParaRod->keyLabel == 7)
	{	//加劲角钢
		if (pParaAngle->ciAngleLayout == 3 && !bToOutward)
			pParaAngle->ciAngleLayout = 4;
		else if (pParaAngle->ciAngleLayout == 4 && bToOutward)
			pParaAngle->ciAngleLayout = 3;
		if (bToOutward&&pParaAngle->ciAngleLayout == 3)
		{
			pParaAngle->desStart.cTouchLineType = 0;
			pParaAngle->desStart.cTouchLine = 1;
			pParaAngle->desStart.szXorCtrlWingOffset = "-$4#T";
			pParaAngle->desStart.szYorOthrWingOffset = "g";
			pParaAngle->desEnd.cTouchLineType = 0;
			pParaAngle->desEnd.cTouchLine = 0;
			pParaAngle->desEnd.szXorCtrlWingOffset = "-$6#T";
			pParaAngle->desEnd.szYorOthrWingOffset = "g";
		}
		else if (!bToOutward&&pParaAngle->ciAngleLayout == 4)
		{
			pParaAngle->desStart.cTouchLineType = 0;
			pParaAngle->desStart.cTouchLine = 0;
			pParaAngle->desStart.szYorOthrWingOffset = "-$4#T";
			pParaAngle->desStart.szXorCtrlWingOffset = "g";
			pParaAngle->desEnd.cTouchLineType = 0;
			pParaAngle->desEnd.cTouchLine = 1;
			pParaAngle->desEnd.szXorCtrlWingOffset = "g";
			pParaAngle->desEnd.szYorOthrWingOffset = "-$6#T";
		}
		for (WORD i = 0; i < pParaAngle->m_layoutBoltArr.Count; i++)
		{
			BOLT_LAYOUT* pLayout = pParaAngle->m_layoutBoltArr.GetAt(i);
			if (pParaAngle->ciAngleLayout == 1 || pParaAngle->ciAngleLayout == 3)
				pLayout->ciDatumWing = 'X';
			else if (pParaAngle->ciAngleLayout == 2 || pParaAngle->ciAngleLayout == 4)
				pLayout->ciDatumWing = 'Y';
		}
	}
	else
		return false;
	return true;
}
bool CWalkWayPropertyList::CreateComTemplInstance()
{
	if(params.m_pBaseTube==NULL||params.m_pBaseNode==NULL)
	{
		AfxMessageBox("依赖钢管或依赖节点不能为空!");
		return FALSE;
	}
	if(params.m_pBaseNode->hFatherPart!=params.m_pBaseTube->handle)
	{
		AfxMessageBox("基准节点不在依赖钢管上!");
		return FALSE;
	}
	if(!params.m_bCreateWalkWay&&(params.m_pOuterJg==NULL||params.m_pInnerJg==NULL))
	{
		AfxMessageBox("支架需要有连接角钢！");
		return FALSE;
	}
	f3dPoint ptS=params.m_pBaseTube->Start(),ptE=params.m_pBaseTube->End();
	f3dPoint line_vec=(ptE-ptS).normalized();
	if(fabs(line_vec*f3dPoint(1,0,0))<0.707)
	{
		AfxMessageBox("设计非水平横担!");
		return FALSE;
	}
	//加载相应的走道及其支架组件模板
	IStdComTempl::TEMPL_SOURCE source(0,1,params.m_ciWalkWayType);
	m_xWalkWayTempl.InitComponentTempl(&source);
	//设定模板角钢及钢板的基本信息
	CStdTemplParaRod* pParaRod=NULL;
	if(params.m_bCreateWalkWay)
	{
		//内侧角钢
		pParaRod=m_xWalkWayTempl.listParaRods.GetValue(2);
		pParaRod->m_sGuige=params.m_sWalkWayJgSpec;
		pParaRod->cMaterial=params.m_cWalkWayJgMat;
		//外侧角钢
		pParaRod=m_xWalkWayTempl.listParaRods.GetValue(3);
		pParaRod->m_sGuige=params.m_sWalkWayJgSpec;
		pParaRod->cMaterial=params.m_cWalkWayJgMat;
	}
	else
	{	//不生成走道主梁时，需要实时根据基准节点与已有主梁计算m_fZdOffDist参数
		CLDSLinePart* pWayBeamRod=params.m_pInnerJg!=NULL?params.m_pInnerJg:params.m_pOuterJg;
		if(pWayBeamRod&&params.m_pBaseNode)
		{
			CLDSNode* pStartNode=pWayBeamRod->pStart;
			CLDSNode* pEndNode=pWayBeamRod->pEnd;
			GEPOINT lenStdVec=pEndNode->xOriginalPos-pStartNode->xOriginalPos;
			normalize(lenStdVec);
			params.m_fZdOffDist=(params.m_pBaseNode->xOriginalPos-pStartNode->xOriginalPos)*lenStdVec;
 			params.m_fZdOffDist+=params.m_fZjOffDist;
		}
	}
	//根据对话框界面设定模板在模型环境中的上下文参数
	this->UpdateTemplParams();
	if(params.m_ciWalkWayType>0)
	{	
		//上横撑角钢
		pParaRod=m_xWalkWayTempl.listParaRods.GetValue(4);
		pParaRod->m_sGuige=params.m_sHolderJgSpec;
		pParaRod->cMaterial=params.m_cHolderJgMat;
		UnifyParaAngleLayoutByAntiAxisX(pParaRod);
		//斜撑角钢
		pParaRod=m_xWalkWayTempl.listParaRods.GetValue(5);
		pParaRod->m_sGuige=params.m_sHolderJgSpec;
		pParaRod->cMaterial=params.m_cHolderJgMat;
		UnifyParaAngleLayoutByAntiAxisX(pParaRod);
		//下横撑角钢
		pParaRod=m_xWalkWayTempl.listParaRods.GetValue(6);
		if(pParaRod)
		{
			pParaRod->m_sGuige=params.m_sHolderJgSpec;
			pParaRod->cMaterial=params.m_cHolderJgMat;
			UnifyParaAngleLayoutByAntiAxisX(pParaRod);
		}
		//加劲角钢
		pParaRod=m_xWalkWayTempl.listParaRods.GetValue(7);
		if(pParaRod)
		{
			pParaRod->m_sGuige=params.m_sHolderJgSpec;
			pParaRod->cMaterial=params.m_cHolderJgMat;
			UnifyParaAngleLayoutByAntiAxisX(pParaRod);
		}
	}
	CStdTemplParaPlate* pParaPlate=m_xWalkWayTempl.listParaPlates.GetValue(1);
	if(pParaPlate)
	{
		pParaPlate->_thick=params.m_nThick;
		pParaPlate->cMaterial=params.m_cPlateMat;
	}
	pParaPlate=m_xWalkWayTempl.listParaPlates.GetValue(2);
	if(pParaPlate)
	{
		pParaPlate->_thick=params.m_nThick;
		pParaPlate->cMaterial=params.m_cPlateMat;
	}
	//匹配必要的装配杆件及节点
	GEPOINT nearVecX=CalNearVecX(),vec=(params.m_pBaseTube->End()-params.m_pBaseTube->Start()).normalized();
	CLDSNode* pTubeNodeS=(nearVecX*vec>0)?params.m_pBaseTube->pStart:params.m_pBaseTube->pEnd;
	CLDSNode* pTubeNodeE=(nearVecX*vec>0)?params.m_pBaseTube->pEnd:params.m_pBaseTube->pStart;
	m_xWalkWayTempl.hashMatchRodHid.SetValue(1,params.m_pBaseTube->handle);
	m_xWalkWayTempl.SetMappingNode(1,params.m_pBaseNode->handle);
	m_xWalkWayTempl.SetMappingNode(KEY4C("S"),pTubeNodeS->handle);
	m_xWalkWayTempl.SetMappingNode(KEY4C("E"),pTubeNodeE->handle);
	if(params.m_fZjOffDist==0)//此时支架在主管上的基点与params.m_pBaseNode重合
		m_xWalkWayTempl.SetMappingNode(KEY4C("BN"),params.m_pBaseNode->handle);
	if(params.m_pInnerJg)
	{
		m_xWalkWayTempl.hashMatchRodHid.SetValue(2,params.m_pInnerJg->handle);
		m_xWalkWayTempl.SetMappingNode(KEY4C("SI"),params.m_pInnerJg->pStart->handle);
		m_xWalkWayTempl.SetMappingNode(KEY4C("EI"),params.m_pInnerJg->pEnd->handle);
	}
	if(params.m_pOuterJg)
	{
		m_xWalkWayTempl.hashMatchRodHid.SetValue(3,params.m_pOuterJg->handle);
		m_xWalkWayTempl.SetMappingNode(KEY4C("SO"),params.m_pOuterJg->pStart->handle);
		m_xWalkWayTempl.SetMappingNode(KEY4C("EO"),params.m_pOuterJg->pEnd->handle);
	}	
	CLDSNode* pInnerBeamDatumNode=m_pBelongModel->FromNodeHandle(m_hInnerBeamDatumNode);
	CLDSNode* pOuterBeamDatumNode=m_pBelongModel->FromNodeHandle(m_hOuterBeamDatumNode);
	if(pInnerBeamDatumNode)
		m_xWalkWayTempl.SetMappingNode(KEY4C("DI0"),pInnerBeamDatumNode->handle);
	else
		m_xWalkWayTempl.DeleteMappingNode(KEY4C("DI0"));
	if(pOuterBeamDatumNode)
		m_xWalkWayTempl.SetMappingNode(KEY4C("DO0"),pOuterBeamDatumNode->handle);
	else
		m_xWalkWayTempl.DeleteMappingNode(KEY4C("DO0"));
	return TRUE;
}
bool CWalkWayPropertyList::CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt)
{
	return true;
}
BOOL CWalkWayPropertyList::FinishSelectObjOper(long hPickObj,long idEventProp)
{
	CCallBackDialog* pCallBackDlg=(CCallBackDialog*)GetParent();
	if(idEventProp==GetPropID("BaseTube"))
	{
		CLDSLineTube* pTube=NULL;
		if((pTube=(CLDSLineTube*)m_pBelongModel->FromPartHandle(hPickObj,CLS_LINETUBE))!=NULL)
		{
			params.m_pBaseTube=pTube;
			m_hBaseTube=hPickObj;
		}
	}
	else if(idEventProp==GetPropID("BaseNode"))
	{
		CLDSNode* pNode=NULL;
		if((pNode=m_pBelongModel->FromNodeHandle(hPickObj))!=NULL)
		{
			m_hBaseNode=hPickObj;
			params.m_pBaseNode=pNode;
		}
	}
	else if(idEventProp==GetPropID("LinkAngle1"))
	{
		CLDSLineAngle* pAngle=NULL;
		if((pAngle=(CLDSLineAngle*)m_pBelongModel->FromPartHandle(hPickObj,CLS_LINEANGLE))!=NULL)
		{
			m_hOuterAngle=hPickObj;
			params.m_pOuterJg=pAngle;
		}
	}
	else if(idEventProp==GetPropID("LinkAngle2"))
	{
		CLDSLineAngle* pAngle=NULL;
		if((pAngle=(CLDSLineAngle*)m_pBelongModel->FromPartHandle(hPickObj,CLS_LINEANGLE))!=NULL)
		{
			m_hInnerAngle=hPickObj;
			params.m_pInnerJg=pAngle;
		}
	}
	else
		return false;
	return true;
}
void CWalkWayPropertyList::DisplayPropertyList(int overwrite0_append1/*=0*/,DWORD dwPropGroup/*=1*/)
{
	CNativePropListManager oper(this);
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pRoot=GetRootItem();
	CleanTree();
	//m_propList.SetModifyValueFunc(ModifyProperty);
	//m_propList.SetButtonClickFunc(ButtonClick);
	//m_propList.SetPropHelpPromptFunc(HelpPromptProperty);
	//走道参数	
	pGroupItem=oper.InsertPropItem(pRoot,"BasicInfo");
	oper.InsertEditPropItem(pGroupItem,"OffDistX+");
	oper.InsertEditPropItem(pGroupItem,"OffDistX-");
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
	Redraw();
}
bool CWalkWayPropertyList::DisplaySlavePropertyList(CPropTreeItem* pItem,DWORD dwRelaInfo/*=0*/)
{
	if(this->m_pRelaSubPropList==NULL)
		return false;
	CNativePropListManager oper(m_pRelaSubPropList);
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pRoot=m_pRelaSubPropList->GetRootItem();
	m_pRelaSubPropList->CleanTree();
	//m_pRelaSubPropList->SetModifyValueFunc(ModifyProperty);
	//
	pGroupItem=oper.InsertPropItem(pRoot,"PlateInfo");
	oper.InsertEditPropItem(pGroupItem,"PlateThick");
	oper.InsertCmbListPropItem(pGroupItem,"PlateMat",MakeMaterialMarkSetString());
	if(params.m_ciWalkWayType==0)
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
	m_pRelaSubPropList->Redraw();
	return true;
}
void CWalkWayPropertyList::UpdateAssemblyProp(CPropTreeItem* pParentItem)
{
	CNativePropListManager oper(this);
	DeleteAllSonItems(pParentItem);
	//
	oper.InsertButtonPropItem(pParentItem,"BaseTube","","",-1,TRUE);
	oper.InsertButtonPropItem(pParentItem,"BaseNode","","",-1,TRUE);
	if(!this->params.m_bCreateWalkWay)
	{
		oper.InsertButtonPropItem(pParentItem,"LinkAngle1","","",-1,TRUE);
		oper.InsertButtonPropItem(pParentItem,"LinkAngle2","","",-1,TRUE);
	}
}
void CWalkWayPropertyList::UpdateWalkWayProp(CPropTreeItem* pParentItem)
{
	CNativePropListManager oper(this);
	DeleteAllSonItems(pParentItem);
	if(params.m_bCreateWalkWay)
	{
		oper.InsertEditPropItem(pParentItem,"OffDistX-","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"WalkWayWidth","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"WalkWayLength","","",-1,TRUE);
		oper.InsertCmbEditPropItem(pParentItem,"WalkWayJgSpec",MakeAngleSetString(),"","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"WalkWayJgMat",MakeMaterialMarkSetString(),"","",-1,TRUE);
	}
}
void CWalkWayPropertyList::UpdateHolderProp(CPropTreeItem* pParentItem)
{
	CNativePropListManager oper(this);
	DeleteAllSonItems(pParentItem);
	if(params.m_ciWalkWayType!=0)
	{
		oper.InsertEditPropItem(pParentItem,"Zh","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"D1","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"D2","","",-1,TRUE);
		if(params.m_ciWalkWayType==2)
			oper.InsertEditPropItem(pParentItem,"YB","","",-1,TRUE);
		oper.InsertCmbEditPropItem(pParentItem,"HolderJgSpec",MakeAngleSetString(),"","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"HolderJgMat",MakeMaterialMarkSetString(),"","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"ciHolderAngleLayoutStyle","0.朝外|1.朝内","","",-1,TRUE);
		if (params.m_ciWalkWayType == 1)
			oper.InsertCmbListPropItem(pParentItem, "ciOtherAngleLayoutStyle","0.朝外|1.朝内", "", "", -1, TRUE);
	}
	/*else
	{
		oper.InsertCmbEditPropItem(pParentItem,"BoltMd","12|16|20|24","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"BoltNum","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"BoltSpace","","",-1,TRUE);
	}*/
}
void CWalkWayPropertyList::UpdateSketchMap()
{
	if(m_pSketchBitmap==NULL)
		return;
	m_pSketchBitmap->Detach();
	if(this->params.m_ciWalkWayType==0)
	{	//钢板支架式
		m_pSketchBitmap->LoadBitmap(IDB_BMP_WALKWAY1);
	}
	else if(params.m_ciWalkWayType==1)
	{	//三角支架式
		m_pSketchBitmap->LoadBitmap(IDB_BMP_WALKWAY2);
	}
	else if(params.m_ciWalkWayType==2)
	{	//台型支教式
		m_pSketchBitmap->LoadBitmap(IDB_BMP_WALKWAY3);
	}
	//
	//CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_MAP);
	HBITMAP hBitmap=(HBITMAP)m_pSketchBitmap->GetSafeHandle();
	m_pSketchPanel->SetBitmap(hBitmap);
}
void CWalkWayPropertyList::DelWorkPlaneSketch()
{
	g_pSolidDraw->DelWorkPlaneSketch(1);
	g_pSolidDraw->DelWorkPlaneSketch(2);
	g_pSolidDraw->Draw();
}
void CWalkWayPropertyList::WorkPlaneSketch()
{
	if(params.m_pBaseNode==NULL || params.m_pBaseTube==NULL)
		return;
	DelWorkPlaneSketch();
	params.m_pBaseTube->BuildUCS();
	GEPOINT base_pos=params.m_pBaseNode->xOriginalPos;
	GEPOINT line_vec=CalNearVecX();
	if(line_vec*params.m_pBaseTube->ucs.axis_z>0)
		line_vec=params.m_pBaseTube->ucs.axis_z;
	else
		line_vec=params.m_pBaseTube->ucs.axis_z*(-1);
	int nLen=ftoi(params.m_pBaseTube->GetDiameter()*2);
	btc::SKETCH_PLANE sketch1(base_pos,line_vec,params.m_pBaseTube->ucs.axis_x,nLen);
	g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),sketch1.pVertexArr,sketch1.VertexCount,line_vec,"X+",200);
	g_pSolidDraw->NewWorkPlaneSketch(2,RGB(152,152,152),&base_pos,1,-1*line_vec,"X-",200);
	g_pSolidDraw->Draw();
}
bool CWalkWayPropertyList::InitializeUI(bool bInternalStart/*=false*/,DWORD dwParam/*=0*/)
{
	if(!bInternalStart)
	{
		InitPropHashtable();
		if(m_hInnerAngle<=0)
			m_hInnerAngle=(params.m_pInnerJg)?params.m_pInnerJg->handle:0;
		else
			params.m_pInnerJg=(CLDSLineAngle*)Ta.FromPartHandle(m_hInnerAngle,CLS_LINEANGLE);
		if(m_hOuterAngle<=0)
			m_hOuterAngle=(params.m_pOuterJg)?params.m_pOuterJg->handle:0;
		else
			params.m_pOuterJg=(CLDSLineAngle*)Ta.FromPartHandle(m_hOuterAngle,CLS_LINEANGLE);
		if(params.m_pInnerJg && params.m_pOuterJg)
			params.m_bCreateWalkWay=FALSE;
		else
			params.m_bCreateWalkWay=TRUE;
		if(m_hBaseTube<=0)
			m_hBaseTube=params.m_pBaseTube?params.m_pBaseTube->handle:0;
		else
			params.m_pBaseTube=(CLDSLineTube*)Ta.FromPartHandle(m_hBaseTube,CLS_LINETUBE);
		if(m_hBaseNode<=0)
			m_hBaseNode=params.m_pBaseNode?params.m_pBaseNode->handle:0;
		else
			params.m_pBaseNode=Ta.FromNodeHandle(m_hBaseNode);
		m_xWalkWayTempl.Empty();
	}
	UpdateSketchMap();
	WorkPlaneSketch();
	return true;
}
bool CWalkWayPropertyList::ConfirmOK()
{
	m_hBaseTube=params.m_pBaseTube?params.m_pBaseTube->handle:0;
	m_hBaseNode=params.m_pBaseNode?params.m_pBaseNode->handle:0;
	m_hInnerAngle=params.m_pInnerJg?params.m_pInnerJg->handle:0;
	m_hOuterAngle=params.m_pOuterJg?params.m_pOuterJg->handle:0;
	DelWorkPlaneSketch();
	return true;
}
bool CWalkWayPropertyList::CancelUI()
{
	DelWorkPlaneSketch();
	return true;
}
#endif