//<LOCALE_TRANSLATE BY hxr /> 2015-04-20
#include "stdafx.h"
#include "stdio.h"
#include "Tower.h"
#include "CreateFace.h"
#include "SortFunc.h"
#include "btc.h"
#include "VertexsLifeObject.h"
#include "DbObjPropId.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

CLDSParamPlate::CLDSParamPlate()
{
	m_nClassTypeId=CLS_PARAMPLATE;
	//strcpy(m_sClassName,"CLDSParamPlate");
	thick=8;
	is_data_modified = TRUE;
	_material = 'S';
	pSolidBody = NULL;
	strcpy(sPartNo,"");
	m_bDisplayHole = false;
	m_uStatMatCoef = 1;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
	m_iParamType=TYPE_ROLLEND;
	m_iStyle=0;
	m_iLsRayNo=0;
	m_bAutoCalParamPlateUcs = TRUE;	//TRUE:自动计算参数板坐标系 FALSE:指定参数板坐标系
	m_bStdPart = FALSE; //TRUE:标准构件 FALSE:非标准构件
	m_fNormOffset=0;
	crMaterial=GetColor(CLS_PARAMPLATE,-1,'S');	//crPole
	design.inited=true;	//在FromBuffer时赋值false
}
CLDSParamPlate::~CLDSParamPlate()
{
	if(pSolidBody)
	{
		delete pSolidBody;
		pSolidBody=NULL;
	}
}
IMPLEMENT_GET(CLDSParamPlate, Thick){
	CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
	if(pMotherObj==this)
		return _thick;
	else
		return pMotherObj->Thick;
	return 0;
}
IMPLEMENT_SET(CLDSParamPlate, Thick){
	if(value>=0)
	{
		CLDSParamPlate* pMotherObj=(CLDSParamPlate*)MotherObject();
		bool modifed=(_thick!=ftoi(value));
		_thick = ftoi(value);
		if(pMotherObj!=this)
			pMotherObj->Thick=value;
		if(modifed)
			SetModified();
		//designItemList.GetNodeNum()>0防止参数不全时进行厚度赋值,系统提前设计参数化基座板
		DESIGN_PARAM paramNormOffset;
		if(modifed&&_iParamType==TYPE_BASEPLATE&&GetDesignItemValue('C',&paramNormOffset)&&
			(paramNormOffset.value.iValue>=0&&paramNormOffset.value.iValue<=2))
			DesignPlate();	//底座板有时随板厚改变,钢板位置会发生变化 wjh-2016.8.31
	}
}
int GetWaterShieldedPlateValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetCircularCoverPlateValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetCircularRibPlateValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetBasePlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetShoePlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetAngleRibPlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetStayWirePlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetRibPlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetStationConnPlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetStationShoePlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetStationLinkPlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetElbowPlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetStationCrossPlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetFLPlateValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr);
int GetPedalPlateValueStr(CLDSParamPlate* pParamPlate, long id, char* valueStr);
int CLDSParamPlate::GetPropValueStr(long id, char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[100]="";
	CXhString szText(sText,100);
	if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("designInfo.hNode")==id)
	{
		CLDSNode* pNode=GetBaseNode();
		if(pNode==NULL)
			strcpy(sText,"0X0");
		else
			sprintf(sText,"0X%X",pNode->handle);
	}
	else if(GetPropID("designInfo.hRod")==id)
		sprintf(sText,"0X%X",m_hBaseRod);
	else if(GetPropID("layer")==id)
		strcpy(sText,layer());
	else if(GetPropID("cMaterial")==id)	
	{
		char matStr[20];
		if(QuerySteelMatMark(cMaterial,matStr))
			sprintf(sText,"%s",matStr);
	}
	else if(GetPropID("m_cQualityLevel")==id)
	{
		if(!this->blDefaultQualityLevel&&isalpha(cQualityLevel))
			sprintf(sText,"%C",cQualityLevel);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Default");
#else
			strcpy(sText,"默认");
#endif
	}
	else if(GetPropID("thick")==id)
		sprintf(sText,"%d",ftol(GetThick()));
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		sprintf(sText,"%s",(char*)GetPartNo());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
	{
		if(layer(0)=='L')	//接腿构件
			return cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_bDisplayHole")==id)
	{
		if(m_bDisplayHole)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
#endif
	}
	else if(GetPropID("m_cPickedDisplayMode")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_cPickedDisplayMode==0)
			strcpy(sText,"None");
		else if(m_cPickedDisplayMode==1)
			strcpy(sText,"Actual Profile");
		else if(m_cPickedDisplayMode==2)
			strcpy(sText,"Spread Profile");
#else 
		if(m_cPickedDisplayMode==0)
			strcpy(sText,"0.不显示");
		else if(m_cPickedDisplayMode==1)
			strcpy(sText,"1.轮廓边");
		else if(m_cPickedDisplayMode==2)
			strcpy(sText,"2.轮廓边(展开)");
#endif
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bVirtualPart)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bVirtualPart)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
		if(m_bUserSpecColor)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		sprintf(sText,"%s",sNotes);
	else if(GetPropID("m_fNormOffset")==id)
	{
		sprintf(sText,"%f",GetNormOffset());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.x")==id)
	{
		sprintf(sText,"%f",ucs.origin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.y")==id)
	{
		sprintf(sText,"%f",ucs.origin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.z")==id)
	{
		sprintf(sText,"%f",ucs.origin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_iParamType")==id)
	{
		if(m_iParamType==TYPE_FL)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Flange");
		else if(m_iParamType==TYPE_UEND)
			strcpy(sText,"1.U Insert Plate");
		else if(m_iParamType==TYPE_ROLLEND)
			strcpy(sText,"2.Slot Insert Plate");
		else if(m_iParamType==TYPE_ELBOWPLATE)
			strcpy(sText,"3.Elbow Plate");
		else if(m_iParamType==TYPE_VERTPLATE)
			strcpy(sText,"4.Vertical Strong Plate");
		else if(m_iParamType==TYPE_ENDPLATE)
			strcpy(sText,"5.Over End Plate"); 
		else if(m_iParamType==TYPE_CIRRIBPLATE)
			strcpy(sText,"6.Circular Rib Plate"); 
		else if(m_iParamType==TYPE_CIRCOVERPLATE)
			strcpy(sText,"7.Circular Over Plate"); 
		else if(m_iParamType==TYPE_TUBERIBPLATE)
			strcpy(sText,"8.Tube Rib Plate"); 
		else if(m_iParamType==TYPE_LINEANGLERIBPLATE)
			strcpy(sText,"9.Angle Rib Plate"); 
		else if(m_iParamType==TYPE_XEND)
			strcpy(sText,"10.Cross Insert Flange");
		else if(m_iParamType==TYPE_FLP)
			strcpy(sText,"11.Plane Weld Flange");
		else if(m_iParamType==TYPE_FLD)
			strcpy(sText,"12.Relative Weld Flange");
		else if(m_iParamType==TYPE_WATERSHIELDEDPLATE)
			strcpy(sText,"13.Water Shielded Plate");
		else if(m_iParamType==TYPE_RIBPLATE)
			strcpy(sText,"14.Rib Plate");
		else if(m_iParamType==TYPE_FLG)
			strcpy(sText,"15.Rigid Flange");
		else if(m_iParamType==TYPE_FLR)
			strcpy(sText,"16.Flexible Flange");
		else if(m_iParamType==TYPE_BASEPLATE)
			strcpy(sText,"17.Base Plate");
		else if(m_iParamType==TYPE_SHOEPLATE)
			strcpy(sText,"18.Shoe Plate");
		else if(m_iParamType==TYPE_STATIONCONNPLATE)
			strcpy(sText,"19.Station Connect Plate");
		else if(m_iParamType==TYPE_STATIONSHOEPLATE)
			strcpy(sText,"20.Station Shoe Plate");
		else if(m_iParamType==TYPE_STATIONCROSSPLATE)
			strcpy(sText,"21.Station Cross Plate");
		else if(m_iParamType==TYPE_STATIONLINKPLATE)
			strcpy(sText,"22.Station Link Plate");
		else if (m_iParamType == TYPE_PEDALPLATE)
			strcpy(sText, "23.Pedal Plate");
#else
			strcpy(sText,"0.法兰盘");
		else if(m_iParamType==TYPE_UEND)
			strcpy(sText,"1.U型插板");
		else if(m_iParamType==TYPE_ROLLEND)
			strcpy(sText,"2.槽型插板");
		else if(m_iParamType==TYPE_ELBOWPLATE)
			strcpy(sText,"3.肘板");
		else if(m_iParamType==TYPE_VERTPLATE)
			strcpy(sText,"4.垂向加劲板");
		else if(m_iParamType==TYPE_ENDPLATE)
			strcpy(sText,"5.封头板"); 
		else if(m_iParamType==TYPE_CIRRIBPLATE)
			strcpy(sText,"6.环向肋板"); 
		else if(m_iParamType==TYPE_CIRCOVERPLATE)
			strcpy(sText,"7.环向封板"); 
		else if(m_iParamType==TYPE_TUBERIBPLATE)
			strcpy(sText,"8.钢管肋板"); 
		else if(m_iParamType==TYPE_LINEANGLERIBPLATE)
			strcpy(sText,"9.角钢肋板"); 
		else if(m_iParamType==TYPE_XEND)
			strcpy(sText,"10.十字插板");
		else if(m_iParamType==TYPE_FLP)
			strcpy(sText,"11.平焊法兰");
		else if(m_iParamType==TYPE_FLD)
			strcpy(sText,"12.对焊法兰");
		else if(m_iParamType==TYPE_WATERSHIELDEDPLATE)
			strcpy(sText,"13.遮水板");
		else if(m_iParamType==TYPE_RIBPLATE)
			strcpy(sText,"14.肋板");
		else if(m_iParamType==TYPE_FLG)
			strcpy(sText,"15.刚性法兰");
		else if(m_iParamType==TYPE_FLR)
			strcpy(sText,"16.柔性法兰");
		else if(m_iParamType==TYPE_BASEPLATE)
			strcpy(sText,"17.底座板");
		else if(m_iParamType==TYPE_SHOEPLATE)
			strcpy(sText,"18.靴板");
		else if(m_iParamType==TYPE_STATIONCONNPLATE)
			strcpy(sText,"19.端板");
		else if(m_iParamType==TYPE_STATIONSHOEPLATE)
			strcpy(sText,"20.立柱夹板");
		else if(m_iParamType==TYPE_STATIONCROSSPLATE)
			strcpy(sText,"21.端撑十字板");
		else if(m_iParamType==TYPE_STATIONLINKPLATE)
			strcpy(sText,"22.端撑连板");
		else if (m_iParamType == TYPE_PEDALPLATE)
			strcpy(sText, "23.脚踏板");
#endif
		else
			strcpy(sText,"");
	}
	else if(GetPropID("m_cRollProcessType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(_cRollProcessType==2)
			strcpy(sText,"2.Processing Priority 2");
		else if(_cRollProcessType==1)
			strcpy(sText,"1.Processing Priority 1");
		else if(_cRollProcessType==0)
			strcpy(sText,"0.Mechanical Priority");
		else// if (_cRollProcessType == 0xFF)
			strcpy(sText,"3.Default");
#else
		if(_cRollProcessType==2)
			strcpy(sText,"2.最少刀数2");
		else if(_cRollProcessType==1)
			strcpy(sText,"1.最少刀数1");
		else if(_cRollProcessType==0)
			strcpy(sText,"0.最佳受力");
		else //if (_cRollProcessType == 0xFF)
			strcpy(sText,"3.默认值");
#endif
	}
	else if(GetPropID("m_cRollOffsetType")==id)
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_ciRollOffsetType==0)
			strcpy(sText,"0.Outside");
		else if(m_ciRollOffsetType==1)
			strcpy(sText,"1.Inside");
		else if(m_ciRollOffsetType==2)
			strcpy(sText,"2.Default");
#else
		if(m_ciRollOffsetType==0)
			strcpy(sText,"0.外侧");
		else if(m_ciRollOffsetType==1)
			strcpy(sText,"1.内侧");
		else if(m_ciRollOffsetType==2)
			strcpy(sText,"2.默认值");
#endif
	}
	else if(GetPropID("m_cDeformType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_ciDeformType==0)
			strcpy(sText,"0.Assembly Plane");
		else if(m_ciDeformType==1)
			strcpy(sText,"1.Middle Plane");
		else if(m_ciDeformType==2)
			strcpy(sText,"2.Raised Plane");
		else if(m_ciDeformType==3)
			strcpy(sText,"3.Sunk Plane");
		else if(m_ciDeformType==4)
			strcpy(sText,"4.Default");
#else
		if(m_ciDeformType==0)
			strcpy(sText,"0.装配面");
		else if(m_ciDeformType==1)
			strcpy(sText,"1.中性层");
		else if(m_ciDeformType==2)
			strcpy(sText,"2.凸面");
		else if(m_ciDeformType==3)
			strcpy(sText,"3.凹面");
		else if(m_ciDeformType==4)
			strcpy(sText,"4.默认值");
#endif
	}
	else if(GetPropID("m_fDeformCoef")==id)
	{
		sprintf(sText,"%f",m_fDeformCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("relativeObjs")==id)
	{
		for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
		{
			if(strlen(sText)==0)
				_snprintf(sText,99,"0X%X",pRelaObj->hObj);
			else
				_snprintf(sText,99,"%s,0X%X",sText,pRelaObj->hObj);
		}
	}
	else if(GetPropID("shadowInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("m_hMotherObj")==id)
	{
		CLDSParamPlate *pMotherPlate=(CLDSParamPlate*)MotherObject(false);
		if(pMotherPlate!=this)
			sprintf(sText,"0X%X",pMotherPlate->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_bSyncShadowPartNo")==id)
	{
		if(m_bSyncShadowPartNo)
			strcpy(sText,"是");
		else 
			strcpy(sText,"否");
	}
	else if(GetPropID("m_hBlockRef")==id)
	{
		CBlockParameter blockPara;
		if (GetBlockParameter(blockPara))
			sprintf(sText,"0X%X",blockPara.m_pBlockRef->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_hBlock")==id)
		strcpy(sText,GetBlockIdName());
	else if(GetPropID("wcs.origin")==id)
	{
		PLATEEDIT_STATE* pEditState=m_hashDisplayPlateStates.Add(handle);
		szText.Printf("(%.1f,%.1f)",pEditState->m_fLocalWcsOrgX,pEditState->m_fLocalWcsOrgY);
	}
	else if(GetPropID("wcs.origin.x")==id)
	{
		PLATEEDIT_STATE* pEditState=m_hashDisplayPlateStates.Add(handle);
		szText.Printf("%g",pEditState->m_fLocalWcsOrgX);
	}
	else if(GetPropID("wcs.origin.y")==id)
	{
		PLATEEDIT_STATE* pEditState=m_hashDisplayPlateStates.Add(handle);
		szText.Printf("%g",pEditState->m_fLocalWcsOrgY);
	}
	else if(GetPropID("wcs.axis_x")==id)
	{
		PLATEEDIT_STATE* pEditState=m_hashDisplayPlateStates.Add(handle);
		szText.Printf("(%.1f,%.1f)",pEditState->m_fLocalWcsAxisX_x,pEditState->m_fLocalWcsAxisX_y);
	}
	else if(GetPropID("wcs.axis_x.a")==id)
	{
		PLATEEDIT_STATE* pEditState=m_hashDisplayPlateStates.Add(handle);
		double alfa=Cal2dLineAng(0,0,pEditState->m_fLocalWcsAxisX_x,pEditState->m_fLocalWcsAxisX_y)*DEGTORAD_COEF;
		szText.Printf("%.2f",alfa);
		SimplifiedNumString(szText);
	}
	else if(GetPropID("wcs.axis_y")==id)
	{
		PLATEEDIT_STATE* pEditState=m_hashDisplayPlateStates.Add(handle);
		szText.Printf("(%.1f,%.1f)",pEditState->m_fLocalWcsAxisY_x,pEditState->m_fLocalWcsAxisY_y);
	}
	else if(m_iParamType==TYPE_BASEPLATE)
		return GetBasePlatePropValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_SHOEPLATE)
		return GetShoePlatePropValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_LINEANGLERIBPLATE)
		return GetAngleRibPlatePropValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_TUBESTAYWIREPLATE)
		return GetStayWirePlatePropValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_RIBPLATE)
		return GetRibPlatePropValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_STATIONCONNPLATE)
		return GetStationConnPlatePropValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_STATIONSHOEPLATE)
		return GetStationShoePlatePropValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_STATIONLINKPLATE)
		return GetStationLinkPlatePropValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_ELBOWPLATE)
		return GetElbowPlatePropValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_STATIONCROSSPLATE)
		return GetStationCrossPlatePropValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_WATERSHIELDEDPLATE)
		return GetWaterShieldedPlateValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_CIRCOVERPLATE)
		return GetCircularCoverPlateValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_CIRRIBPLATE||m_iParamType==TYPE_TUBERIBPLATE)
		return GetCircularRibPlateValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_FL)
		return GetFLPlateValueStr(this,id,valueStr);
	else if(m_iParamType==TYPE_PEDALPLATE)
		return GetPedalPlateValueStr(this, id, valueStr);
#ifdef AFX_TARG_ENU_ENGLISH
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("0X%X Plate's property##d string value out of length",handle,id);
#else
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("参数化钢板0X%X属性#%d字符串超出长度限制",handle,id);
#endif
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
BOOL CLDSParamPlate::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSObject *pFirstObj = selectObjs.GetFirst();
	CLDSParamPlate *pPlate=NULL, *pFirstPlate = (CLDSParamPlate*)pFirstObj;
	pFirstPlate->GetPropValueStr(idProp,valueStr);
	for(pPlate=(CLDSParamPlate*)selectObjs.GetNext();pPlate;pPlate=(CLDSParamPlate*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pPlate->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////////
//定义参数化板参数属性项标识函数
long DYNA_PROPITEM_GROUP::GetPropID(const char* propStr)
{
	PROPLIST_ITEM *pItem=propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
		if(CLDSObject::log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			CLDSObject::log2file->Log("Don't find property Id named '%s'",propStr);
#else
			CLDSObject::log2file->Log("没找到名为'%s'的属性Id",propStr);
#endif
		return 0;
	}
}
int DYNA_PROPITEM_GROUP::GetPropStatus(long id)
{
	DWORD dwStatus;
	if(propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}
BOOL DYNA_PROPITEM_GROUP::SetPropStatus(long id,BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return propStatusHashtable.SetValueAt(id,dwStatus);
}
PROPLIST_ITEM* DYNA_PROPITEM_GROUP::GetPropItem(long id)
{
	return hashPropItemById.GetValue(id);
}
PROPLIST_ITEM* DYNA_PROPITEM_GROUP::AddPropItem(char* propKey,PROPLIST_ITEM item)
{
	PROPLIST_ITEM* pItem=propHashtable.SetValue(propKey,item);
	pItem->propKeyStr.Copy(propKey);
	return hashPropItemById.SetValue(item.id,pItem);
}
PROPLIST_ITEM* DYNA_PROPITEM_GROUP::UpdatePropItem(char* propKey,char* propName/*=NULL*/)
{
	PROPLIST_ITEM *pItem=propHashtable.GetValue(propKey);
	if(pItem!=NULL&&propName!=NULL)
		pItem->name.Copy(propName);
	return pItem;
}
int InitBasePlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("locationParams",PROPLIST_ITEM(id++,"Location Parameter"));
	pItemPropGroup->AddPropItem("S",PROPLIST_ITEM(id++,"Shape Type","Square,Circular","0.Square|1.Circular"));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"Radius"));
	pItemPropGroup->AddPropItem("XA",PROPLIST_ITEM(id++,"X Positive Width"));
	pItemPropGroup->AddPropItem("XB",PROPLIST_ITEM(id++,"X Negative Width"));
	pItemPropGroup->AddPropItem("YA",PROPLIST_ITEM(id++,"Y Positive Height"));
	pItemPropGroup->AddPropItem("YB",PROPLIST_ITEM(id++,"Y Negative Height"));
	pItemPropGroup->AddPropItem("X0",PROPLIST_ITEM(id++,"X","Chamfer Width"));
	pItemPropGroup->AddPropItem("Y0",PROPLIST_ITEM(id++,"Y","Chamfer Width"));
	pItemPropGroup->AddPropItem("X1",PROPLIST_ITEM(id++,"X","Chamfer Width"));
	pItemPropGroup->AddPropItem("Y1",PROPLIST_ITEM(id++,"Y","Chamfer Width"));
	pItemPropGroup->AddPropItem("X2",PROPLIST_ITEM(id++,"X","Chamfer Width"));
	pItemPropGroup->AddPropItem("Y2",PROPLIST_ITEM(id++,"Y","Chamfer Width"));
	pItemPropGroup->AddPropItem("X3",PROPLIST_ITEM(id++,"X","Chamfer Width"));
	pItemPropGroup->AddPropItem("Y3",PROPLIST_ITEM(id++,"Y","Chamfer Width"));
#else
	pItemPropGroup->AddPropItem("locationParams",PROPLIST_ITEM(id++,"定位参数"));
	pItemPropGroup->AddPropItem("S",PROPLIST_ITEM(id++,"外形类型","方形,圆形","0.方形|1.圆形"));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"半径"));
	pItemPropGroup->AddPropItem("XA",PROPLIST_ITEM(id++,"X+向宽"));
	pItemPropGroup->AddPropItem("XB",PROPLIST_ITEM(id++,"X-向宽"));
	pItemPropGroup->AddPropItem("X0",PROPLIST_ITEM(id++,"Ⅳ象限X向切角","第四象限水平切割长度"));
	pItemPropGroup->AddPropItem("X2",PROPLIST_ITEM(id++,"Ｉ象限X向切角","第一象限水平切割长度"));
	pItemPropGroup->AddPropItem("X1",PROPLIST_ITEM(id++,"Ⅲ象限X向切角","第三象限水平切割长度"));
	pItemPropGroup->AddPropItem("X3",PROPLIST_ITEM(id++,"Ⅱ象限X向切角","第二象限水平切割长度"));
	pItemPropGroup->AddPropItem("YA",PROPLIST_ITEM(id++,"Y+向宽"));
	pItemPropGroup->AddPropItem("YB",PROPLIST_ITEM(id++,"Y-向宽"));
	pItemPropGroup->AddPropItem("Y0",PROPLIST_ITEM(id++,"Ⅳ象限Y向切角","第四象限竖直切割长度"));
	pItemPropGroup->AddPropItem("Y2",PROPLIST_ITEM(id++,"Ｉ象限Y向切角","第一象限竖直切割长度"));
	pItemPropGroup->AddPropItem("Y1",PROPLIST_ITEM(id++,"Ⅲ象限Y向切角","第三象限竖直切割长度"));
	pItemPropGroup->AddPropItem("Y3",PROPLIST_ITEM(id++,"Ⅱ象限Y向切角","第二象限竖直切割长度"));
#endif
	return id;
}
int InitShoePlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("F",PROPLIST_ITEM(id++,"Shape Type","Shape Type,Through Base Plate,Leg Single Side,Ridge Single Side","0.Through Base Plate|1.Leg Single Side|2.Ridge Single Side"));
	pItemPropGroup->AddPropItem("CS",PROPLIST_ITEM(id++,"Connection Method","Boots plate and depend on the Angle of the connection"));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"Shoe Plate Direction","Coordinate axis of shoe plate relative to base plate","0.X|1.Y"));
	pItemPropGroup->AddPropItem("AL",PROPLIST_ITEM(id++,"Wrap Length","The length of butt-joint shoe plate's weldline project to dependant angle ridge line"));
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"Step Hatch Height","Step hatch height, the height from shoe plate's front vertex to base plate"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"Top Remain","Top Remain,the width from top front vertex to angle ridge"));
	pItemPropGroup->AddPropItem("IA",PROPLIST_ITEM(id++,"Inside Step Hatch Height",""));
	pItemPropGroup->AddPropItem("IB",PROPLIST_ITEM(id++,"Inside Top Remain",""));
	pItemPropGroup->AddPropItem("IC",PROPLIST_ITEM(id++,"param IC"));
	pItemPropGroup->AddPropItem("ID",PROPLIST_ITEM(id++,"param ID"));
	pItemPropGroup->AddPropItem("M",PROPLIST_ITEM(id++,"Extend Remainder","0:Extend to base plate edge, else: the remainder to edge"));
	pItemPropGroup->AddPropItem("AL",PROPLIST_ITEM(id++,"Extend Length Of Angle Direction","Extend Length Of Datum Angle Direction，0:calculate the length by plate's bolts"));
	pItemPropGroup->AddPropItem("CM",PROPLIST_ITEM(id++,"Cut Angle Mode","","0.Cut Angle|1.Cut Arc"));
	pItemPropGroup->AddPropItem("L",PROPLIST_ITEM(id++,"Cut Angle Width"));
	pItemPropGroup->AddPropItem("NL",PROPLIST_ITEM(id++,"Offset Dist Of Norm Direction"));
#else
	pItemPropGroup->AddPropItem("F",PROPLIST_ITEM(id++,"外形类别","外形类别,贯穿底板,肢翼单侧,楞线单侧","0.贯穿底板|1.肢翼单侧|2.楞线单侧"));
	pItemPropGroup->AddPropItem("CS",PROPLIST_ITEM(id++,"连接方式","连接方式,螺栓连接,对焊连接","0.螺栓连接|1.对焊连接"));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"靴板方向","靴板相对底板的坐标轴向","0.X轴|1.Y轴"));
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"外侧台阶高A","即靴板前侧顶点距底板的高度"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"外侧延伸量B","顶部余量,顶部前侧顶点到角钢楞线的宽度"));
	pItemPropGroup->AddPropItem("IA",PROPLIST_ITEM(id++,"内侧台阶高IA","即靴板后侧顶点距底板的高度"));
	pItemPropGroup->AddPropItem("IB",PROPLIST_ITEM(id++,"内侧延伸量IB","顶部余量,顶部后侧顶点到角钢肢翼线的宽度"));
	pItemPropGroup->AddPropItem("IC",PROPLIST_ITEM(id++,"斜材延伸量IC","靴板顶点在斜材楞线侧的延伸量"));
	pItemPropGroup->AddPropItem("ID",PROPLIST_ITEM(id++,"斜材延伸量ID","靴板顶点在斜材肢翼侧的延伸量"));
	pItemPropGroup->AddPropItem("M",PROPLIST_ITEM(id++,"底部延伸量M","靴板延伸至底板边缘时的间隙值M==0表示延伸至底板边缘"));
	pItemPropGroup->AddPropItem("AL",PROPLIST_ITEM(id++,"高度延伸量AL","靴板非连接侧角钢长度方向延伸量，默认根据角钢螺栓计算"));
	pItemPropGroup->AddPropItem("CM",PROPLIST_ITEM(id++,"倒角模式","","0.切角|1.切弧"));
	pItemPropGroup->AddPropItem("L",PROPLIST_ITEM(id++,"倒角宽度"));
	pItemPropGroup->AddPropItem("NL",PROPLIST_ITEM(id++,"法线方向偏移量","靴板沿法线方向的偏移距离"));
#endif
	return id;
}
int InitAngleRibPlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	//角钢肋板: A.角钢+基准节点
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"Datum Angle"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"Locate Node(Bolt)"));
	pItemPropGroup->AddPropItem("I",PROPLIST_ITEM(id++,"Rib Plate Type","Rib plate type：inside rib plate,outside rib plate","0.Inside Rib Plate|1.Outside Rib Plate"));
	pItemPropGroup->AddPropItem("L",PROPLIST_ITEM(id++,"L","Parameter L"));
	pItemPropGroup->AddPropItem("Ly",PROPLIST_ITEM(id++,"Ly","Parameter Lr,Top Remain"));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"W","Parameter W"));
	pItemPropGroup->AddPropItem("Wy",PROPLIST_ITEM(id++,"Wy","Parameter Wy"));
	pItemPropGroup->AddPropItem("S",PROPLIST_ITEM(id++,"S","Parameter S"));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"D","Parameter D"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"H","Parameter H"));
	pItemPropGroup->AddPropItem("Hy",PROPLIST_ITEM(id++,"Hy","Parameter Hy"));
	pItemPropGroup->AddPropItem("C",PROPLIST_ITEM(id++,"Bevel Type","Bevel type of inside rib plate:straight,circular","0.Straight|1.Circular"));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"R","Parameter R:Circular Radius"));
	pItemPropGroup->AddPropItem("d",PROPLIST_ITEM(id++,"d","Parameter d:Hole diameter"));
	pItemPropGroup->AddPropItem("P",PROPLIST_ITEM(id++,"P","Parameter P"));
#else
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"基准角钢"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"定位节点(螺栓)"));
	pItemPropGroup->AddPropItem("I",PROPLIST_ITEM(id++,"肋板类型","肋板类型：内侧肋板,外侧肋板","0.内侧肋板|1.外侧肋板"));
	pItemPropGroup->AddPropItem("L",PROPLIST_ITEM(id++,"肋板长度Lx","肋板在X肢上的长度"));
	pItemPropGroup->AddPropItem("Lr",PROPLIST_ITEM(id++,"Lr","参数Lr,顶部余量"));
	pItemPropGroup->AddPropItem("Ly",PROPLIST_ITEM(id++,"肋板长度Ly","肋板在Y肢上的长度"));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"台口高度Wx","在X肢上的台口高度"));
	pItemPropGroup->AddPropItem("Wy",PROPLIST_ITEM(id++,"台口高度Wy","在Y肢上的台口高度"));
	pItemPropGroup->AddPropItem("E", PROPLIST_ITEM(id++, "倒角类型", "内侧肋板的倒角方式","0.直角|1.圆弧"));
	pItemPropGroup->AddPropItem("S",PROPLIST_ITEM(id++,"倒角宽度S","倒角宽度S"));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"法向偏移量D","法线方向的偏移距离"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"肋板长度Hx","紧贴X肢的长度"));
	pItemPropGroup->AddPropItem("Hy",PROPLIST_ITEM(id++,"肋板长度Hy","紧贴Y肢的长度"));
	pItemPropGroup->AddPropItem("C",PROPLIST_ITEM(id++,"斜边类型","内侧肋板的斜边类型","0.直线|1.圆弧"));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"圆弧半径R","圆弧边半径"));
	pItemPropGroup->AddPropItem("d",PROPLIST_ITEM(id++,"中心孔径d","参数d:孔直径"));
	pItemPropGroup->AddPropItem("P",PROPLIST_ITEM(id++,"中心孔距P","参数P"));
#endif
	return id;
}
int InitStayWirePlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	//钢管拉线板
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"Datum Tube"));
	pItemPropGroup->AddPropItem("C",PROPLIST_ITEM(id++,"Datum Plate"));
	pItemPropGroup->AddPropItem("L",PROPLIST_ITEM(id++,"Plate Len"));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"Plate Width"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"Weld Len"));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"Cur Radius"));
	pItemPropGroup->AddPropItem("N",PROPLIST_ITEM(id++,"Hole Number"));
	pItemPropGroup->AddPropItem("d",PROPLIST_ITEM(id++,"Hole Dim"));
	pItemPropGroup->AddPropItem("P",PROPLIST_ITEM(id++,"Hole Space"));
#else
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"基准钢管"));
	pItemPropGroup->AddPropItem("C",PROPLIST_ITEM(id++,"依赖钢板"));
	pItemPropGroup->AddPropItem("L",PROPLIST_ITEM(id++,"肋板长度L"));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"肋板宽度W"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"焊接宽度H"));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"倒角圆弧R"));
	pItemPropGroup->AddPropItem("N",PROPLIST_ITEM(id++,"拉线孔数"));
	pItemPropGroup->AddPropItem("d",PROPLIST_ITEM(id++,"拉线孔径D"));
	pItemPropGroup->AddPropItem("P",PROPLIST_ITEM(id++,"拉线孔距P"));
#endif
	return id;
}
int InitRibPlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	//肋板(角肋板，槽肋板，角钢肋板，钢板肋板，钢管肋板，钢管夹板) wht 11-03-21
	//钢管肋板:	B.钢管+基准节点
	//钢板肋板: C.钢板+基准螺栓+方向构件
	//  角肋板: D.钢板+辅助钢板+定位螺栓
	//			E.钢管+辅助钢板(旧:钢管肘板)
	//			F.钢管+钢管(旧:钢管夹板)	
	//  槽肋板: G.钢板+2个辅助钢板+定位螺栓
	//			H.钢管+2个辅助钢板
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
	CXhChar100 sPropHelp,sPropCmbStr;
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"Datum Part","Datum part for rib plate locate"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"Datum Node","Datum node for rib plate locate"));
	pItemPropGroup->AddPropItem("C",PROPLIST_ITEM(id++,"Ancillary Part 1","Ancillary part 1 for rib plate locate"));	//辅助构件1
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"Ancillary Part 2","Ancillary part 2 for rib plate locate"));	//辅助构件2
	pItemPropGroup->AddPropItem("O",PROPLIST_ITEM(id++,"Datum Point Locate Bolt"));	//基点定位螺栓
	pItemPropGroup->AddPropItem("V",PROPLIST_ITEM(id++,"Direction Part"));		//X轴或Y轴方向基准杆件
	pItemPropGroup->AddPropItem("Ox",PROPLIST_ITEM(id++,"X","X coordinate of rib plate UCS's origin(Relative coordinate under datum plate)"));
	pItemPropGroup->AddPropItem("Oy",PROPLIST_ITEM(id++,"Y","Y coordinate of rib plate UCS's origin(Relative coordinate under datum plate)"));
	pItemPropGroup->AddPropItem("Xx",PROPLIST_ITEM(id++,"X","X coordinate of rib plate UCS's X axis(Relative coordinate under datum plate)"));
	pItemPropGroup->AddPropItem("Xy",PROPLIST_ITEM(id++,"Y","Y coordinate of rib plate UCS's X axis(Relative coordinate under datum plate)"));
	pItemPropGroup->AddPropItem("Ga",PROPLIST_ITEM(id++,"Plate Lean Angle","Plate lean angle(degree measure),Z axis of datum plate as initial direction,rotate specify angle around X axis to confirm Y axis."));
	pItemPropGroup->AddPropItem("X",PROPLIST_ITEM(id++,"X Axis Offset ΔX"));	//X方向偏移量
	pItemPropGroup->AddPropItem("Z",PROPLIST_ITEM(id++,"Z Axis Offset ΔZ"));	//Z方向偏移量
	pItemPropGroup->AddPropItem("dx",PROPLIST_ITEM(id++,"X Axis Offset δX"));	//X方向偏移量
	pItemPropGroup->AddPropItem("dz",PROPLIST_ITEM(id++,"Z Axis Offset δZ"));	//Z方向偏移量
	pItemPropGroup->AddPropItem("El",PROPLIST_ITEM(id++,"Side edge rule","","0.length|1.height"));
	pItemPropGroup->AddPropItem("E",PROPLIST_ITEM(id++,"Rib Plate Height"));
	pItemPropGroup->AddPropItem("F",PROPLIST_ITEM(id++,"Step Height"));	
	pItemPropGroup->AddPropItem("G",PROPLIST_ITEM(id++,"Rib Plate Length"));	
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"Top Width"));	
	pItemPropGroup->AddPropItem("e",PROPLIST_ITEM(id++,"Cutting Model","","0.cut-angle|1.cut_arc"));
	pItemPropGroup->AddPropItem("I",PROPLIST_ITEM(id++,"Cut Angle Width"));
	pItemPropGroup->AddPropItem("I2",PROPLIST_ITEM(id++,"Cut Angle Width"));
	pItemPropGroup->AddPropItem("Rd",PROPLIST_ITEM(id++,"Ray Distance"));
	pItemPropGroup->AddPropItem("RibPlate.plate.bOnDatumPlateTopFace",PROPLIST_ITEM(id++,"On Datum Plate Front","Yes:Rib plate on top of datum plate,No:Rib plate on bottom of datum plate.","Yes|No"));
	pItemPropGroup->AddPropItem("RibPlate.plate.bSpecOrg",PROPLIST_ITEM(id++,"Datum Position","Specify the origin for rib plate UCS，the inputting coordinate is relative coordinate under datum plate UCS.","0.Specify Datum Part|1.Input"));
	pItemPropGroup->AddPropItem("RibPlate.plate.bSpecAxisX",PROPLIST_ITEM(id++,"X Axis","Specify X axis for rib plate UCS,the inputting coordinate is relative coordinate under datum plate UCS.","0.Auto Calculate|1.Input"));	
	pItemPropGroup->AddPropItem("RibPlate.plate.bSpecAxisY",PROPLIST_ITEM(id++,"Y Axis","Through rod of Y axis direction or lean angle(degree measure) inputted to confirm Y axis.","0.Direction Rod|1.Lean Angle"));	
	//pItemPropGroup->AddPropItem("RibPlate.plate.bSpecAxisY",PROPLIST_ITEM(id++,"Y Axis","Through lean angle(degree measure) imputted to confirm Y axis.","0.Vertical Datum Plate|1.Lean Angle"));	
	pItemPropGroup->AddPropItem("RibPlate.plate.bAlongVecPart",PROPLIST_ITEM(id++,"Same Direction With Direction Rod","Y axis of rib plate is same with direction rod.","Yes|No"));	
	pItemPropGroup->AddPropItem("RibPlate.plate.bOnRelaPlateTopFace",PROPLIST_ITEM(id++,"On Ancillary Plate Front","Rib plate on top of ancillary plate.","Yes|No"));
	//X+方向顶部边角类型
	sPropHelp.Copy("rib plate's top edge angle in X+ direction type，define the appearance of plate");
	sPropCmbStr.Copy("0.Right-Angle|1.Chamfer Angle|2.Arc");
	pItemPropGroup->AddPropItem("Rt",PROPLIST_ITEM(id++,"type of the top edge angle in X+ direction",sPropHelp,sPropCmbStr));
	//X+切角宽度/切弧半径
	sPropHelp.Copy("width of top cutting arc in X+ direction");
	pItemPropGroup->AddPropItem("Rw",PROPLIST_ITEM(id++,"cutting angle width",sPropHelp));	
	sPropHelp.Copy("height of top cutting arc in X+ direction");
	pItemPropGroup->AddPropItem("Rh",PROPLIST_ITEM(id++,"cutting angle width",sPropHelp));	
	//X-方向顶部边角类型
	sPropHelp.Copy("rib plate's top edge angle type in X- direction,define the appearance of plate");
	sPropCmbStr.Copy("0.Right-Angle|1.Chamfer Angle|2.Arc");
	pItemPropGroup->AddPropItem("Lt",PROPLIST_ITEM(id++,"type of the top edge angle in X- direction",sPropHelp,sPropCmbStr));
	//X-切角宽度/切弧半径
	sPropHelp.Copy("width of top cutting angle in X- direction");
	pItemPropGroup->AddPropItem("Lw",PROPLIST_ITEM(id++,"cutting angle width",sPropHelp));	
	sPropHelp.Copy("height of top cutting angle in X- direction");
	pItemPropGroup->AddPropItem("Lh",PROPLIST_ITEM(id++,"cutting angle width",sPropHelp));	
#else
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"基准构件","肋板定位基准构件"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"基准节点","肋板定位基准节"));
	pItemPropGroup->AddPropItem("C",PROPLIST_ITEM(id++,"辅助构件1","肋板定位辅助构件1"));	//辅助构件1
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"辅助构件2","肋板定位辅助构件2"));	//辅助构件2
	pItemPropGroup->AddPropItem("O",PROPLIST_ITEM(id++,"基点定位螺栓"));	//基点定位螺栓
	pItemPropGroup->AddPropItem("V",PROPLIST_ITEM(id++,"方向构件"));		//X轴或Y轴方向基准杆件
	pItemPropGroup->AddPropItem("Ox",PROPLIST_ITEM(id++,"X","肋板坐标系原点X坐标分量(基准钢板下的相对坐标"));
	pItemPropGroup->AddPropItem("Oy",PROPLIST_ITEM(id++,"Y","肋板坐标系原点Y坐标分量(基准钢板下的相对坐标)"));
	pItemPropGroup->AddPropItem("Xx",PROPLIST_ITEM(id++,"X","肋板坐标系X轴X坐标分量(基准钢板下的相对坐标)"));
	pItemPropGroup->AddPropItem("Xy",PROPLIST_ITEM(id++,"Y","肋板坐标系X轴Y坐标分量(基准钢板下的相对坐标)"));
	pItemPropGroup->AddPropItem("Ga",PROPLIST_ITEM(id++,"钢板倾斜角度","钢板倾斜角度(角度制),以基准钢板Z轴方向为初始方向，绕X轴旋转指定角度确定Y轴方向。"));
	pItemPropGroup->AddPropItem("X",PROPLIST_ITEM(id++,"X向偏移ΔX","钢板坐标系原点与基准点之间的X轴方向上的偏移距离"));	//X方向偏移量
	pItemPropGroup->AddPropItem("Z",PROPLIST_ITEM(id++,"Z向偏移ΔZ","钢板坐标系原点与基准点之间的Z轴方向上的偏移距离"));	//Z方向偏移量
	pItemPropGroup->AddPropItem("dx",PROPLIST_ITEM(id++,"制弯面X向偏移δX"));	//X方向偏移量
	pItemPropGroup->AddPropItem("dz",PROPLIST_ITEM(id++,"制弯面Z向偏移δZ"));	//Z方向偏移量
	pItemPropGroup->AddPropItem("El",PROPLIST_ITEM(id++,"侧边计算依据","","0.边长|1.高度"));
	pItemPropGroup->AddPropItem("E",PROPLIST_ITEM(id++,"侧边高度A"));
	pItemPropGroup->AddPropItem("G",PROPLIST_ITEM(id++,"底边宽度B"));	
	pItemPropGroup->AddPropItem("F",PROPLIST_ITEM(id++,"台阶高度C"));	
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"顶部宽度D"));	
	pItemPropGroup->AddPropItem("e",PROPLIST_ITEM(id++,"倒角模式","倒角模式","0.切角|1.切弧"));
	pItemPropGroup->AddPropItem("I",PROPLIST_ITEM(id++,"切角宽度E"));
	pItemPropGroup->AddPropItem("I2",PROPLIST_ITEM(id++,"切角宽度E"));
	pItemPropGroup->AddPropItem("MH",PROPLIST_ITEM(id++,"孔径H"));	
	pItemPropGroup->AddPropItem("MF",PROPLIST_ITEM(id++,"孔间隙F"));	
	pItemPropGroup->AddPropItem("MG",PROPLIST_ITEM(id++,"孔间隙G"));	
	pItemPropGroup->AddPropItem("Rd",PROPLIST_ITEM(id++,"射线距离"));
	pItemPropGroup->AddPropItem("RibPlate.plate.bOnDatumPlateTopFace",PROPLIST_ITEM(id++,"位于基准钢板顶侧","是:肋板在基准钢板顶面一侧,否:肋板在基准钢板底面一侧。","是|否"));
	pItemPropGroup->AddPropItem("RibPlate.plate.bSpecOrg",PROPLIST_ITEM(id++,"基准位置","指定肋板坐标系原点位置，输入坐标为基准钢板坐标系下的相对坐标。","0.指定定位螺栓|1.直接输入"));
	pItemPropGroup->AddPropItem("RibPlate.plate.bSpecAxisX",PROPLIST_ITEM(id++,"X轴方向","指定肋板坐标系X轴方向，输入坐标为基准钢板坐标系下的相对坐标。","0.自动计算|1.直接输入"));	
	pItemPropGroup->AddPropItem("RibPlate.plate.bSpecAxisY",PROPLIST_ITEM(id++,"Y轴方向","通过Y轴方向杆件或输入的倾斜角度(角度制)，确定Y轴方向。","0.方向杆件|1.倾斜角度"));	
	//pItemPropGroup->AddPropItem("RibPlate.plate.bSpecAxisY",PROPLIST_ITEM(id++,"Y轴方向","通过输入的倾斜角度(角度制)，确定Y轴方向。","0.垂直基板|1.倾斜角度"));	
	pItemPropGroup->AddPropItem("RibPlate.plate.bAlongVecPart",PROPLIST_ITEM(id++,"与方向杆件同向","肋板Y轴方向与方向杆件同向。","是|否"));	
	pItemPropGroup->AddPropItem("RibPlate.plate.bOnRelaPlateTopFace",PROPLIST_ITEM(id++,"位于辅助钢板正侧","肋板在辅助钢板顶面一侧。","是|否"));
	pItemPropGroup->AddPropItem("K",PROPLIST_ITEM(id++,"初始偏转角"));		//定位方向角
	pItemPropGroup->AddPropItem("J",PROPLIST_ITEM(id++,"定位基准线","钢板定位基准线或直接指定方向","0.基准边|1.焊道线|2.指定方向"));
	pItemPropGroup->AddPropItem("Nt",PROPLIST_ITEM(id++,"工作面类型","钢板工作法线的定义基准方式","0.钢管轴面|1.钢管截面|2.指定方向"));
	pItemPropGroup->AddPropItem("Norm",PROPLIST_ITEM(id++,"法线方向"));	
	pItemPropGroup->AddPropItem("Nx",PROPLIST_ITEM(id++,"X"));	
	pItemPropGroup->AddPropItem("Ny",PROPLIST_ITEM(id++,"Y"));
	pItemPropGroup->AddPropItem("Nz",PROPLIST_ITEM(id++,"Z"));
	pItemPropGroup->AddPropItem("AxisY",PROPLIST_ITEM(id++,"展开方向","肋板展开方向",CStdVector::MakeVectorSetStr()));
	pItemPropGroup->AddPropItem("Wx",PROPLIST_ITEM(id++,"X分量"));
	pItemPropGroup->AddPropItem("Wy",PROPLIST_ITEM(id++,"Y分量"));
	pItemPropGroup->AddPropItem("Wz",PROPLIST_ITEM(id++,"Z分量"));
	pItemPropGroup->AddPropItem("DM",PROPLIST_ITEM(id++,"轴向偏移量"));
	pItemPropGroup->AddPropItem("DX",PROPLIST_ITEM(id++,"X向偏移量"));
	pItemPropGroup->AddPropItem("Rt",PROPLIST_ITEM(id++,"X+向顶部边角类型","肋板X+向顶部边角类型，确定板的外形。","0.直角|1.倒角|2.圆弧"));
	pItemPropGroup->AddPropItem("Rw",PROPLIST_ITEM(id++,"X向切角量","X+向顶部切角宽度"));	
	pItemPropGroup->AddPropItem("Rh",PROPLIST_ITEM(id++,"Y向切角量","X+向顶部切角高度"));	
	pItemPropGroup->AddPropItem("Lt",PROPLIST_ITEM(id++,"X-向顶部边角类型","肋板X-向顶部边角类型，确定板的外形。","0.直角|1.倒角|2.圆弧"));
	pItemPropGroup->AddPropItem("Lw",PROPLIST_ITEM(id++,"X向切角量","X-向顶部切角宽度"));
	pItemPropGroup->AddPropItem("Lh",PROPLIST_ITEM(id++,"Y向切角量","X-向顶部切角高度"));
#endif
	return id;
}
int InitStationConnPlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{	//钢管端板
	int id=iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("LP",PROPLIST_ITEM(id++,"Datum base palte",""));
	pItemPropGroup->AddPropItem("I",PROPLIST_ITEM(id++,"Datum Vector","","0.X+|1.X-|2.Y+|3.Y-"));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"D","Parameter D"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"H","Parameter H"));
	pItemPropGroup->AddPropItem("C",PROPLIST_ITEM(id++,"C","Parameter C"));
	pItemPropGroup->AddPropItem("r",PROPLIST_ITEM(id++,"r","Parameter r"));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"R","Parameter R"));
#else
	pItemPropGroup->AddPropItem("LP",PROPLIST_ITEM(id++,"定位基板","端板的定位基准钢板"));
	pItemPropGroup->AddPropItem("I",PROPLIST_ITEM(id++,"定位方向","端板在基板的位置","0.X+|1.X-|2.Y+|3.Y-"));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"偏移距D","端板沿定位方向的偏移距离"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"孔距H","销钉孔到板底的距离"));
	pItemPropGroup->AddPropItem("C",PROPLIST_ITEM(id++,"根高C","端板根高"));
	pItemPropGroup->AddPropItem("r",PROPLIST_ITEM(id++,"孔半径r","销钉孔半径"));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"弧半径R","圆弧半径"));
#endif
	return id;
}
int InitStationShoePlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{	//立柱夹板
	int id=iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("LB",PROPLIST_ITEM(id++,"Datunm Plate",""));
	pItemPropGroup->AddPropItem("LE",PROPLIST_ITEM(id++,"Assist Plate",""));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"R","Parameter R"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"H","Parameter H"));
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"C","Parameter C"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"B","Parameter B"));
	pItemPropGroup->AddPropItem("CR",PROPLIST_ITEM(id++,"CR","Parameter CR"));
#else
	pItemPropGroup->AddPropItem("LB",PROPLIST_ITEM(id++,"定位基板","夹板的定位基准钢板"));
	pItemPropGroup->AddPropItem("LE",PROPLIST_ITEM(id++,"辅助端板","夹板的连接端板"));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"弧半径R","圆弧半径"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"板高H","夹板高度"));
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"根高C","夹板根高"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"倒角宽B","倒角宽度"));
	pItemPropGroup->AddPropItem("CR",PROPLIST_ITEM(id++,"倒角半径CR","倒角半径"));
#endif
	return id;
}
int InitStationLinkPlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"Datum Tube",""));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"Datum Node",""));
	pItemPropGroup->AddPropItem("C",PROPLIST_ITEM(id++,"Assist Angle1",""));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"Assist Angle2",""));
	pItemPropGroup->AddPropItem("OW",PROPLIST_ITEM(id++,"distance on wing",""));
	pItemPropGroup->AddPropItem("OB",PROPLIST_ITEM(id++,"distance on ber",""));
#else
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"基准钢管","端撑连板的端撑钢管"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"定位节点","端撑连板的定位节点"));
	pItemPropGroup->AddPropItem("C",PROPLIST_ITEM(id++,"连接角钢1","端撑连板的连接角钢1"));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"连接角钢1","端撑连板的连接角钢2"));
	pItemPropGroup->AddPropItem("OW",PROPLIST_ITEM(id++,"肢翼侧余量","端撑连板在角钢肢翼侧的偏移距"));
	pItemPropGroup->AddPropItem("OB",PROPLIST_ITEM(id++,"楞线侧余量","端撑连板在角钢楞线侧的偏移距"));
#endif
	return id;
}
int InitElbowPlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"A","Parameter A"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"B","Parameter B"));
	pItemPropGroup->AddPropItem("C",PROPLIST_ITEM(id++,"C","Parameter C"));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"D","Parameter D"));
	pItemPropGroup->AddPropItem("e",PROPLIST_ITEM(id++,"e","Parameter e"));
	pItemPropGroup->AddPropItem("E",PROPLIST_ITEM(id++,"E","Parameter E"));
	pItemPropGroup->AddPropItem("F",PROPLIST_ITEM(id++,"F","Parameter F"));
	pItemPropGroup->AddPropItem("G",PROPLIST_ITEM(id++,"G","Parameter G"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"H","Parameter H"));
#else
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"肘板高度A","肘板与基准钢管相连的边长"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"底部宽度B","肘板与辅助钢板相连的边长"));
	pItemPropGroup->AddPropItem("C",PROPLIST_ITEM(id++,"台阶高度C","台阶高度"));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"顶部宽度D","肘板顶部宽度"));
	pItemPropGroup->AddPropItem("e",PROPLIST_ITEM(id++,"倒角模式","倒角模式","0.切角|1.切弧"));
	pItemPropGroup->AddPropItem("E",PROPLIST_ITEM(id++,"倒角宽度E","肘板倒角宽度"));
	pItemPropGroup->AddPropItem("F",PROPLIST_ITEM(id++,"孔距F","引流孔到底边的距离"));
	pItemPropGroup->AddPropItem("G",PROPLIST_ITEM(id++,"孔距G","引流孔到侧边的距离"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"孔径H","引流孔孔径"));
#endif
	return id;
}
int InitStationCrossPlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("LP",PROPLIST_ITEM(id++,"base_palte",""));
	pItemPropGroup->AddPropItem("I",PROPLIST_ITEM(id++,"I",""));
	pItemPropGroup->AddPropItem("LA",PROPLIST_ITEM(id++,"LA",""));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"H",""));
#else
	pItemPropGroup->AddPropItem("LP",PROPLIST_ITEM(id++,"基准钢板","插板连接的基准钢板"));
	pItemPropGroup->AddPropItem("I",PROPLIST_ITEM(id++,"定位类型","插板定位类型"));
	pItemPropGroup->AddPropItem("LA",PROPLIST_ITEM(id++,"辅助插板","辅助插板"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"插板高度","插板高度"));
#endif
	return id;
}
int InitWaterShieldedPlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("E",PROPLIST_ITEM(id++,"Locating Datum Edge","",":0.Datum Edge|1.Weld Line"));
	pItemPropGroup->AddPropItem("S",PROPLIST_ITEM(id++,"Normal Offset",""));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"Outer circle diameter",""));
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"The two water tank's angle",""));
	pItemPropGroup->AddPropItem("O",PROPLIST_ITEM(id++,"The offset angle between two water tank's bisector and datum edge",""));
	pItemPropGroup->AddPropItem("L",PROPLIST_ITEM(id++,"Water tank length"));
	pItemPropGroup->AddPropItem("K",PROPLIST_ITEM(id++,"Water tank width"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"Water tank depth"));
#else
	pItemPropGroup->AddPropItem("E",PROPLIST_ITEM(id++,"定位基准边","遮水板的定位基准边","0.基准边|1.焊道线"));
	pItemPropGroup->AddPropItem("S",PROPLIST_ITEM(id++,"法向偏移量","遮水板沿法线方向的偏移距离"));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"外环直径","遮水板的外环直径"));
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"流水槽夹角","两流水槽之间的夹角"));
	pItemPropGroup->AddPropItem("O",PROPLIST_ITEM(id++,"流水槽定位角","两流水槽角平分线与基准边之间的偏移角"));
	pItemPropGroup->AddPropItem("L",PROPLIST_ITEM(id++,"流水槽长度","流水槽长度"));
	pItemPropGroup->AddPropItem("K",PROPLIST_ITEM(id++,"流水槽宽度","流水槽宽度"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"流水槽深度","流水槽深度"));
#endif
	return id;
}
int InitCircularCoverPlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"Sealing Plate Rotate Angle",""));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"Section Angle",""));
	pItemPropGroup->AddPropItem("N",PROPLIST_ITEM(id++,"Parameter N",""));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"Step Hatch Height",""));
	pItemPropGroup->AddPropItem("",PROPLIST_ITEM(id++,"Circular Band Width",""));
#else
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"定位偏移角","环向封板的定位偏移角"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"扇形夹角β","环向封板的扇形夹角"));
	pItemPropGroup->AddPropItem("N",PROPLIST_ITEM(id++,"延伸距离N","外形参数N"));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"台口高度H",""));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"环形带宽W",""));
#endif
	return id;
}
int InitCircularRibPlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(20);
	pItemPropGroup->propStatusHashtable.CreateHashTable(20);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"Sector Start Angle",""));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"Section Angle",""));
	pItemPropGroup->AddPropItem("S",PROPLIST_ITEM(id++,"Offset along datum tube",""));
	pItemPropGroup->AddPropItem("T",PROPLIST_ITEM(id++,"Offset along circular plate's normal",""));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"Circular Plate Outer Arc Radius",""));
	pItemPropGroup->AddPropItem("V",PROPLIST_ITEM(id++,"Maching Gap",""));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"First relative plate's weld length",""));
	pItemPropGroup->AddPropItem("N",PROPLIST_ITEM(id++,"second relative plate's or tube's weld length"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"Step Hatch Height",""));
	pItemPropGroup->AddPropItem("h",PROPLIST_ITEM(id++,"N Side Step Hatch Height",""));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"free margin",""));
	pItemPropGroup->AddPropItem("y",PROPLIST_ITEM(id++,"Chamfer Angle",""));
	pItemPropGroup->AddPropItem("ES",PROPLIST_ITEM(id++,"Edge Space",""));
	pItemPropGroup->AddPropItem("r",PROPLIST_ITEM(id++,"Inner Radius"));
#else
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"定位起始角","普通环向板的定位起始角"));
	pItemPropGroup->AddPropItem("B",PROPLIST_ITEM(id++,"扇形夹角β","普通环向板的扇形夹角"));
	pItemPropGroup->AddPropItem("S",PROPLIST_ITEM(id++,"轴向偏移量","沿钢管轴向偏移距离"));
	pItemPropGroup->AddPropItem("T",PROPLIST_ITEM(id++,"法向偏移量","沿环板的法向偏移距离"));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"外环半径R",""));
	pItemPropGroup->AddPropItem("V",PROPLIST_ITEM(id++,"加工间隙V",""));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"焊缝长度W","第一块关联钢板的焊缝长"));
	pItemPropGroup->AddPropItem("N",PROPLIST_ITEM(id++,"焊缝长度N","第二块关联钢板或钢管的焊缝长"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"台口高度Hw",""));
	pItemPropGroup->AddPropItem("h",PROPLIST_ITEM(id++,"台口高度Hn",""));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"自由边距D",""));
	pItemPropGroup->AddPropItem("y",PROPLIST_ITEM(id++,"倒角角度γ",""));
	pItemPropGroup->AddPropItem("ES",PROPLIST_ITEM(id++,"螺栓边距",""));
	pItemPropGroup->AddPropItem("r",PROPLIST_ITEM(id++,"内圆半径r"));
#endif
	return id;
}
int InitTubeRibPlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("S",PROPLIST_ITEM(id++,"Offset along datum tube",""));
	pItemPropGroup->AddPropItem("T",PROPLIST_ITEM(id++,"Offset along circular plate's normal",""));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"First relative plate's weld length",""));
	pItemPropGroup->AddPropItem("N",PROPLIST_ITEM(id++,"second relative plate's or tube's weld length"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"Step Hatch Height",""));
	pItemPropGroup->AddPropItem("h",PROPLIST_ITEM(id++,"N Side Step Hatch Height",""));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"free margin",""));
#else
	pItemPropGroup->AddPropItem("S",PROPLIST_ITEM(id++,"轴向偏移量","沿钢管轴向偏移距离"));
	pItemPropGroup->AddPropItem("T",PROPLIST_ITEM(id++,"法向偏移量","沿环板的法向偏移距离"));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"焊缝长度W","第一块关联钢板的焊缝长"));
	pItemPropGroup->AddPropItem("N",PROPLIST_ITEM(id++,"焊缝长度N","第二块关联钢板或钢管的焊缝长"));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"台口高度H",""));
	pItemPropGroup->AddPropItem("h",PROPLIST_ITEM(id++,"N侧台口高",""));
	pItemPropGroup->AddPropItem("D",PROPLIST_ITEM(id++,"自由边距D",""));
#endif
	return id;
}
int InitFlPlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup,long iInitId)
{
	int id=iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(5);
	pItemPropGroup->propStatusHashtable.CreateHashTable(5);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"angle",""));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"offset distance H",""));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"inner circle radius",""));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"outer circle diameter",""));
#else
	pItemPropGroup->AddPropItem("A",PROPLIST_ITEM(id++,"扇形角度",""));
	pItemPropGroup->AddPropItem("H",PROPLIST_ITEM(id++,"偏移距H","法兰底部到钢管连接端的距离"));
	pItemPropGroup->AddPropItem("R",PROPLIST_ITEM(id++,"内环半径","普通法兰的内环半径"));
	pItemPropGroup->AddPropItem("W",PROPLIST_ITEM(id++,"外环直径","普通法兰的外环直径"));
#endif
	return id;
}
int InitPedalPlateParamItemPropHashtable(DYNA_PROPITEM_GROUP* pItemPropGroup, long iInitId)
{
	int id = iInitId;
	pItemPropGroup->propHashtable.SetHashTableGrowSize(10);
	pItemPropGroup->propStatusHashtable.CreateHashTable(10);
#ifdef AFX_TARG_ENU_ENGLISH
	pItemPropGroup->AddPropItem("A", PROPLIST_ITEM(id++, "Datum LineAngle", ""));
	pItemPropGroup->AddPropItem("V", PROPLIST_ITEM(id++, "Extend Vec", "", "0.Wing|1.Ber"));
	pItemPropGroup->AddPropItem("L", PROPLIST_ITEM(id++, "pram L", ""));
	pItemPropGroup->AddPropItem("W", PROPLIST_ITEM(id++, "pram W", ""));
	pItemPropGroup->AddPropItem("H", PROPLIST_ITEM(id++, "pram H", ""));
	pItemPropGroup->AddPropItem("S", PROPLIST_ITEM(id++, "pram S", ""));
	pItemPropGroup->AddPropItem("D", PROPLIST_ITEM(id++, "pram D", ""));
#else
	pItemPropGroup->AddPropItem("A", PROPLIST_ITEM(id++, "基准角钢", ""));
	pItemPropGroup->AddPropItem("V", PROPLIST_ITEM(id++, "延伸方向", "延伸方向方式", "0.肢翼侧|1.楞线侧"));
	pItemPropGroup->AddPropItem("C", PROPLIST_ITEM(id++, "外形样式", "","0.外形I|1.外形II"));
	pItemPropGroup->AddPropItem("S", PROPLIST_ITEM(id++, "连接宽度S", "S=0,默认延伸到轮廓边"));
	pItemPropGroup->AddPropItem("L", PROPLIST_ITEM(id++, "踏板长度L", ""));
	pItemPropGroup->AddPropItem("W", PROPLIST_ITEM(id++, "踏板宽度W", ""));
	pItemPropGroup->AddPropItem("H", PROPLIST_ITEM(id++, "挡板高度H", ""));
	pItemPropGroup->AddPropItem("D", PROPLIST_ITEM(id++, "台口高度D", ""));
#endif
	return id;
}
int GetPedalPlateValueStr(CLDSParamPlate* pParamPlate, long id, char* valueStr)
{
	KEY2C key = CLDSParamPlate::GetPropKey(id, pParamPlate->m_iParamType);
	if (key.wItem == 0)
		return 0;
	CXhChar100 sText;
	DESIGN_PLATE_ITEM *pItem = pParamPlate->GetDesignItem(key);
	if (pItem == NULL)
		return 0;
	if (KEY2C("C") == key)
	{
		long iType = pItem->value.iType;
		if (iType == 0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.profile I");
		else
			sText.Copy("1.profile II");
#else
			sText.Copy("0.外形I");
		else
			sText.Copy("1.外形II");
#endif
	}
	else if (KEY2C("V") == key)
	{
		long iType = pItem->value.iType;
		if (iType == 0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Wing Side");
		else
			sText.Copy("1.Ber Side");
#else
			sText.Copy("0.肢翼侧");
		else
			sText.Copy("1.楞线侧");
#endif
	}
	else
	{
		if (0 == pItem->dataType)
		{
			sText.Printf("%f", pItem->value.fValue);
			SimplifiedNumString(sText);
		}
		else if (1 == pItem->dataType)
			sText.Printf("0x%X", pItem->value.hPart);
		else if (2 == pItem->dataType)
			sText.Printf("%d", pItem->value.iType);
	}
	if (valueStr)
		strcpy(valueStr, sText);
	return sText.GetLength();
}
int GetFLPlateValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	CXhChar100 sText;
	DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
	if(pItem==NULL)
		return 0;
	if(0==pItem->dataType)
	{
		sText.Printf("%f",pItem->value.fValue);
		SimplifiedNumString(sText);
	}
	else if(1==pItem->dataType)
		sText.Printf("0x%X",pItem->value.hPart);
	else if(2==pItem->dataType)
		sText.Printf("%d",pItem->value.iType);
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetWaterShieldedPlateValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	CXhChar100 sText;
	DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
	if(pItem==NULL)
		return 0;
	if(KEY2C("E")==key)
	{	
		long iType=0;
		if(pItem->dataType==0)
			iType=ftoi(pItem->value.fValue);
		else 
			iType=pItem->value.iType;
		if(iType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Datum Edge");
		else
			sText.Copy("1.Weld Line");
#else
			sText.Copy("0.基准边");
		else
			sText.Copy("1.焊道线");
#endif
	}
	else 
	{
		if(0==pItem->dataType)
		{
			sText.Printf("%f",pItem->value.fValue);
			SimplifiedNumString(sText);
		}
		else if(1==pItem->dataType)
			sText.Printf("0x%X",pItem->value.hPart);
		else if(2==pItem->dataType)
			sText.Printf("%d",pItem->value.iType);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetCircularCoverPlateValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	CXhChar100 sText;
	DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
	if(pItem==NULL)
		return 0;
	if(0==pItem->dataType)
	{
		sText.Printf("%f",pItem->value.fValue);
		SimplifiedNumString(sText);
	}
	else if(1==pItem->dataType)
		sText.Printf("0x%X",pItem->value.hPart);
	else if(2==pItem->dataType)
		sText.Printf("%d",pItem->value.iType);
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetCircularRibPlateValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	CXhChar100 sText;
	DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
	if(pItem==NULL)
	{
		if(key=='h')//环向肋板'h'参数缺少表示与'H'参数一致处理 wjh-2017.5.26
			pItem=pParamPlate->GetDesignItem('H');
		if(pItem==NULL)
			return 0;
	}
	if(key=='S')
	{	//沿轴向偏移量，由于历史原因实际上偏移正方向是与钢板法线一致的，因此显示给用户时需要正负号一致性处理　wjh-2018.1.2
		CLDSLinePart* pRod=pParamPlate->BelongModel()->FromRodHandle(pParamPlate->m_hBaseRod);
		if(pRod)
		{
			GEPOINT vDatumAxis=pRod->End()-pRod->Start();
			if(vDatumAxis*pParamPlate->ucs.axis_z<0)
				sText.Printf("%g",-pItem->fValue);
			else
				sText.Printf("%g",pItem->fValue);
			if(valueStr)
				strcpy(valueStr,sText);
			return sText.GetLength();
		}
	}
	if(0==pItem->dataType)
	{
		if(key=='R')	//暂保守只处理环向板外圆半径参数为整数显示 wjh-2018.2.9
			sText.Printf("%.0f",pItem->value.fValue);
		else
			sText.Printf("%.1f",pItem->value.fValue);
		SimplifiedNumString(sText);
	}
	else if(1==pItem->dataType)
		sText.Printf("0x%X",pItem->value.hPart);
	else if(2==pItem->dataType)
		sText.Printf("%d",pItem->value.iType);
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetBasePlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)	//根据属性ID得到属性值
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	CXhChar100 sText;
	DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
	if(pItem==NULL)
		return 0;
	if(KEY2C("S")==key)
	{	//塔脚板底板外形类型
		long iType=0;
		if(pItem->dataType==0)
			iType=ftoi(pItem->value.fValue);
		else 
			iType=pItem->value.iType;
		if(iType==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Circular");
		else
			sText.Copy("0.Square");
#else
			sText.Copy("1.圆形");
		else
			sText.Copy("0.方形");
#endif
	}
	else 
	{
		if(0==pItem->dataType)
		{
			sText.Printf("%f",pItem->value.fValue);
			SimplifiedNumString(sText);
		}
		else if(1==pItem->dataType)
			sText.Printf("0x%X",pItem->value.hPart);
		else if(2==pItem->dataType)
			sText.Printf("%d",pItem->value.iType);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetShoePlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)	//根据属性ID得到属性值
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	CXhChar100 sText;
	long iType=0;
	if(KEY2C("F")==key)
	{	//塔脚板靴板外形类型
		pParamPlate->GetDesignItemValue(key,&iType);
		if(1==iType)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Leg Single Side");
		else if(2==iType)
			sText.Copy("2.Ridge Single Side");
		else 
			sText.Copy("0.Through Base Plate");
#else
			sText.Copy("1.肢翼单侧");
		else if(2==iType)
			sText.Copy("2.楞线单侧");
		else 
			sText.Copy("0.贯穿底板");
#endif
	}
	else if(KEY2C("CM")==key)
	{	//塔脚板靴板倒角模式
		pParamPlate->GetDesignItemValue(key,&iType);
		if(iType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Cut Angle");
		else
			sText.Copy("1.Cut Arc");
#else
			sText.Copy("0.切角");
		else
			sText.Copy("1.切弧");
#endif
	}
	else if(KEY2C("CS")==key)
	{	//塔脚板靴板外形类型
		pParamPlate->GetDesignItemValue(key,&iType);
		if(0==iType)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Bolt Connect");
		else 
			sText.Copy("1.Weld Connect");
#else
			sText.Copy("0.螺栓连接");
		else 
			sText.Copy("1.对焊连接");
#endif
	}
	else if(KEY2C("W")==key)
	{
		pParamPlate->GetDesignItemValue(key,&iType);
		if(1==iType)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("1.Y");
		else 
			sText.Printf("0.X");
#else
			sText.Printf("1.Y轴");
		else 
			sText.Printf("0.X轴");
#endif
	}
	else 
	{	//新加设计参数在旧版本的靴板参数哈希表中不存在，导致属性栏不显示值，故将查找DESIGN_PLATE_ITEM移到此处
		DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
		if(pItem==NULL)
			return 0;
		if(0==pItem->dataType)
		{
			sText.Printf("%f",pItem->value.fValue);
			SimplifiedNumString(sText);
		}
		else if(1==pItem->dataType)
			sText.Printf("0x%X",pItem->value.hPart);
		else if(2==pItem->dataType)
			sText.Printf("%d",pItem->value.iType);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetAngleRibPlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	CXhChar100 sText;
	if(CLDSParamPlate::GetPropID("Lr",pParamPlate->m_iParamType)==id)
	{
		long hAngle=0;
		double L=0;
		sText.Copy("0");
		if(pParamPlate->GetDesignItemValue(KEY2C("A"),&hAngle)&&
			pParamPlate->GetDesignItemValue(KEY2C("L"),&L))
		{
			CLDSLineAngle *pAngle=(CLDSLineAngle*)pParamPlate->BelongModel()->FromPartHandle(hAngle,CLS_LINEANGLE);
			if(pAngle)
				sText.Printf("%.f",pAngle->GetWidth()-L);
		}
	}
	else if(key==KEY2C("Ly")||key==KEY2C("Wy")||key==KEY2C("Hy"))
	{
		DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
		if(pItem==NULL)
		{
			if(key==KEY2C("Ly"))
				pItem=pParamPlate->GetDesignItem(KEY2C("L"));
			else if(key==KEY2C("Wy"))
				pItem=pParamPlate->GetDesignItem(KEY2C("W"));
			else
				pItem=pParamPlate->GetDesignItem(KEY2C("H"));	
		}
		if(pItem)
		{
			sText.Printf("%f",pItem->value.fValue);
			SimplifiedNumString(sText);
		}
	}
	else
	{
		DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
		if(pItem==NULL)
			return 0;
		long iType=0;
		if(KEY2C("I")==key)
		{
			pParamPlate->GetDesignItemValue(key,&iType);
			if(1==iType)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("1.Outside Rib Plate");
			else 
				sText.Printf("0.Inside Rib Plate");
#else
				sText.Printf("1.外侧肋板");
			else 
				sText.Printf("0.内侧肋板");
#endif
		}
		else if(KEY2C("C")==key)
		{
			long C = 0;
			pParamPlate->GetDesignItemValue(key,&C);
			if(1==C)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("1.Circular");
			else 
				sText.Printf("0.Straight");
#else
				sText.Printf("1.圆弧");
			else 
				sText.Printf("0.直线");
#endif
		}
		else if (KEY2C("E") == key)
		{
			long E = 0;
			pParamPlate->GetDesignItemValue(key, &E);
			if (1 == E)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("1.Circular");
			else
				sText.Printf("0.Straight");
#else
				sText.Printf("1.圆弧");
			else
				sText.Printf("0.直角");
#endif
		}
		else 
		{
			if(0==pItem->dataType)
			{
				sText.Printf("%f",pItem->value.fValue);
				SimplifiedNumString(sText);
			}
			else if(1==pItem->dataType)
				sText.Printf("0x%X",pItem->value.hPart);
			else if(2==pItem->dataType)
				sText.Printf("%d",pItem->value.iType);
		}
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetStayWirePlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
	if(pItem==NULL)
		return 0;
	CXhChar100 sText;
	if(0==pItem->dataType)
	{
		sText.Printf("%f",pItem->value.fValue);
		SimplifiedNumString(sText);
	}
	else if(1==pItem->dataType)
		sText.Printf("0x%X",pItem->value.hPart);
	else if(2==pItem->dataType)
		sText.Printf("%d",pItem->value.iType);
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetRibPlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)
{
	long h=0;
	CXhChar100 sText;
	if(CLDSParamPlate::GetPropID("RibPlate.plate.bOnDatumPlateTopFace",pParamPlate->m_iParamType)==id)
	{
		pParamPlate->GetDesignItemValue('A',&h);
		if(h>0)
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
	else if(CLDSParamPlate::GetPropID("RibPlate.plate.bSpecOrg",pParamPlate->m_iParamType)==id)
	{
		double fValue=0;
		BOOL bSpecOrg=TRUE;
		bSpecOrg=bSpecOrg&&pParamPlate->GetDesignItemValue(KEY2C("Ox"),&fValue);
		bSpecOrg=bSpecOrg&&pParamPlate->GetDesignItemValue(KEY2C("Oy"),&fValue);
		if(bSpecOrg)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Input");
		else 
			sText.Copy("0.Specify Location Bolt");
#else
			sText.Copy("1.直接输入");
		else 
			sText.Copy("0.指定定位螺栓");
#endif
	}
	else if(CLDSParamPlate::GetPropID("RibPlate.plate.bSpecAxisX",pParamPlate->m_iParamType)==id)
	{
		double fValue=0;
		BOOL bSpecAxisX=TRUE;
		bSpecAxisX=bSpecAxisX&&pParamPlate->GetDesignItemValue(KEY2C("Xx"),&fValue);
		bSpecAxisX=bSpecAxisX&&pParamPlate->GetDesignItemValue(KEY2C("Xy"),&fValue);
		if(bSpecAxisX)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Input");
		else 
			sText.Copy("0.Auto Calculate");
#else
			sText.Copy("1.直接输入");
		else 
			sText.Copy("0.自动计算");
#endif
	}
	else if(CLDSParamPlate::GetPropID("RibPlate.plate.bSpecAxisY",pParamPlate->m_iParamType)==id)
	{
		double fValue=0;
		long hRelaPart1=0,hRelaPart2=0;
		BOOL bSpecAxisY=TRUE;
		bSpecAxisY=bSpecAxisY&&pParamPlate->GetDesignItemValue(KEY2C("Ga"),&fValue);
		pParamPlate->GetDesignItemValue('C',&hRelaPart1);
		pParamPlate->GetDesignItemValue('D',&hRelaPart2);
		CLDSPart *pRelaPart1=pParamPlate->BelongModel()->FromPartHandle(abs(hRelaPart1));
		CLDSPart *pRelaPart2=pParamPlate->BelongModel()->FromPartHandle(abs(hRelaPart2));
		if(pRelaPart1==NULL&&pRelaPart2==NULL)
		{
			if(bSpecAxisY)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("1.Lean Angle");
			else 
				sText.Printf("0.Vertical Datum Plate");
#else
				sText.Printf("1.倾斜角度");
			else 
				sText.Printf("0.垂直基板");
#endif
		}
		else 
		{
			if(bSpecAxisY)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("1.Lean Angle");
			else 
				sText.Printf("0.Direction Rod");
#else
				sText.Printf("1.倾斜角度");
			else 
				sText.Printf("0.方向杆件");
#endif
		}
	}
	else if(CLDSParamPlate::GetPropID("RibPlate.plate.bAlongVecPart",pParamPlate->m_iParamType)==id)
	{
		pParamPlate->GetDesignItemValue('V',&h);
		if(h>0)
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
	else if(CLDSParamPlate::GetPropID("RibPlate.plate.bOnRelaPlateTopFace",pParamPlate->m_iParamType)==id)
	{
		long hDatumPart=0,hRelaPart1=0,hRelaPart2=0;
		pParamPlate->GetDesignItemValue('A',&hDatumPart);
		pParamPlate->GetDesignItemValue('C',&hRelaPart1);
		pParamPlate->GetDesignItemValue('D',&hRelaPart2);
		CLDSPart *pDatumPart=pParamPlate->BelongModel()->FromPartHandle(abs(hDatumPart));
		CLDSPart *pRelaPart1=pParamPlate->BelongModel()->FromPartHandle(abs(hRelaPart1));
		CLDSPart *pRelaPart2=pParamPlate->BelongModel()->FromPartHandle(abs(hRelaPart2));
		if(pRelaPart1==NULL&&pRelaPart2==NULL)
		{
			if(pDatumPart&&pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			{
				if(hDatumPart>0)
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
		}
		else if(pRelaPart1)
		{
			if(hRelaPart1>0)
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
	}
	else if(CLDSParamPlate::GetPropID("Lt",pParamPlate->m_iParamType)==id||
			CLDSParamPlate::GetPropID("Rt",pParamPlate->m_iParamType)==id)
	{
		long iType=0;
		if(CLDSParamPlate::GetPropID("Lt",pParamPlate->m_iParamType)==id)
			pParamPlate->GetDesignItemValue(KEY2C("Lt"),&iType);
		else
			pParamPlate->GetDesignItemValue(KEY2C("Rt"),&iType);
#ifdef AFX_TARG_ENU_ENGLISH
		if(iType==1)
			sText.Copy("1.Chamfer Angle");
		else if(iType==2)
			sText.Copy("2.Arc");
		else
			sText.Copy("0.Right-Angle");
#else
		if(iType==1)
			sText.Copy("1.倒角");
		else if(iType==2)
			sText.Copy("2.圆弧");
		else
			sText.Copy("0.直角");
#endif
	}
	else if(CLDSParamPlate::GetPropID("e",pParamPlate->m_iParamType)==id)
	{
		long iType=0;
		pParamPlate->GetDesignItemValue(KEY2C("e"),&iType);
		if(iType==0)
			sText.Copy("0.切角");
		else if(iType==1)
			sText.Copy("1.切弧");
	}
	else if(CLDSParamPlate::GetPropID("I",pParamPlate->m_iParamType)==id)
	{
		long iType=0;
		pParamPlate->GetDesignItemValue(KEY2C("I"),&iType);
		sText.Printf("%d",iType);
	}
	else if(CLDSParamPlate::GetPropID("J",pParamPlate->m_iParamType)==id)
	{
		long iType=0;
		pParamPlate->GetDesignItemValue(KEY2C("J"),&iType);
		if(pParamPlate->GetRibPlateSubType()==CLDSParamPlate::TYPE_RIB_PLATE_ON_TUBE)
		{
			if(iType==0)
				sText.Copy("0.基准边");
			else if(iType==1)
				sText.Copy("1.焊道线");
			else
				sText.Copy("2.指定方向");
		}
		else
		{
			if(iType==0)
				sText.Copy("0.基准边");
			else if(iType==1)
				sText.Copy("1.焊道线");
		}
	}
	else if(CLDSParamPlate::GetPropID("AxisY",pParamPlate->m_iParamType)==id)
	{
		int nVecType=CStdVector::GetVectorType(pParamPlate->ucs.axis_y);
		sText.Printf("%s",CStdVector::GetVecTypeString(nVecType));
	}
	else if(CLDSParamPlate::GetPropID("Wx",pParamPlate->m_iParamType)==id)
	{
		sText.Printf("%f",pParamPlate->ucs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if(CLDSParamPlate::GetPropID("Wy",pParamPlate->m_iParamType)==id)
	{
		sText.Printf("%f",pParamPlate->ucs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if(CLDSParamPlate::GetPropID("Wz",pParamPlate->m_iParamType)==id)
	{
		sText.Printf("%f",pParamPlate->ucs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if (CLDSParamPlate::GetPropID("El",pParamPlate->m_iParamType)==id)
	{
		KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
		if (key.wItem==0)
			return 0;
		DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
		if (pItem==NULL||pItem->iValue==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.length");
		else
			sText.Copy("1.height");
#else
			sText.Copy("0.边长");
		else
			sText.Copy("1.高度");
#endif
	}
	else
	{
		KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
		if(key.wItem==0)
			return 0;
		DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
		if (pItem == NULL&&key==KEY2C("Ga")&&valueStr!=NULL)
		{
			strcpy(valueStr,"90");
			return 2;
		}
		else if(pItem == NULL)
			return 0;
		if( KEY2C("A")==key||KEY2C("B")==key||KEY2C("C")==key||
			KEY2C("D")==key||KEY2C("O")==key||KEY2C("V")==key)
		{
			long hPart=pItem->value.hPart;
			pParamPlate->GetDesignItemValue(key,&hPart);
			sText.Printf("0x%X",abs(hPart));
		}
		else 
		{
			if(0==pItem->dataType)
			{
				sText.Printf("%f",pItem->value.fValue);
				SimplifiedNumString(sText);
			}
			else if(1==pItem->dataType)
				sText.Printf("0x%X",abs(pItem->value.hPart));
			else if(2==pItem->dataType)
				sText.Printf("%d",pItem->value.iType);
		}
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetStationConnPlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	CXhChar100 sText;
	DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
	if(pItem==NULL)
		return 0;
	long iType=0;
	if(KEY2C("I")==key)
	{
		pParamPlate->GetDesignItemValue(key,&iType);
		if(iType==0)
			sText.Printf("0.X+");
		else if(iType==1)
			sText.Printf("1.X-");
		else if(iType==2)
			sText.Printf("2.Y+");
		else if(iType==3)
			sText.Printf("3.Y-");
	}
	else 
	{
		if(0==pItem->dataType)
		{
			sText.Printf("%f",pItem->value.fValue);
			SimplifiedNumString(sText);
		}
		else if(1==pItem->dataType)
			sText.Printf("0x%X",pItem->value.hPart);
		else if(2==pItem->dataType)
			sText.Printf("%d",pItem->value.iType);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetStationShoePlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	CXhChar100 sText;
	DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
	if(pItem==NULL)
		return 0;
	if(0==pItem->dataType)
	{
		sText.Printf("%f",pItem->value.fValue);
		SimplifiedNumString(sText);
	}
	else if(1==pItem->dataType)
		sText.Printf("0x%X",pItem->value.hPart);
	else if(2==pItem->dataType)
		sText.Printf("%d",pItem->value.iType);
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetStationLinkPlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	CXhChar100 sText;
	DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
	if(pItem==NULL)
		return 0;
	if(0==pItem->dataType)
	{
		sText.Printf("%f",pItem->value.fValue);
		SimplifiedNumString(sText);
	}
	else if(1==pItem->dataType)
		sText.Printf("0x%X",pItem->value.hPart);
	else if(2==pItem->dataType)
		sText.Printf("%d",pItem->value.iType);
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetElbowPlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	CXhChar100 sText;
	if(CLDSParamPlate::GetPropID("e",pParamPlate->m_iParamType)==id)
	{
		long iType=0;
		pParamPlate->GetDesignItemValue(key,&iType);
		if(iType==0)
			sText.Copy("0.切角");
		else if(iType==1)
			sText.Copy("1.切弧");
	}
	else
	{
		DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
		if(pItem==NULL)
			return 0;
		if(0==pItem->dataType)
		{
			sText.Printf("%f",pItem->value.fValue);
			SimplifiedNumString(sText);
		}
		else if(1==pItem->dataType)
			sText.Printf("0x%X",pItem->value.hPart);
		else if(2==pItem->dataType)
			sText.Printf("%d",pItem->value.iType);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
int GetStationCrossPlatePropValueStr(CLDSParamPlate* pParamPlate,long id,char* valueStr)
{
	KEY2C key=CLDSParamPlate::GetPropKey(id,pParamPlate->m_iParamType);
	if(key.wItem==0)
		return 0;
	CXhChar100 sText;
	DESIGN_PLATE_ITEM *pItem=pParamPlate->GetDesignItem(key);
	if(pItem==NULL)
		return 0;
	if(KEY2C("I")==key)
	{
		long iType=0;
		pParamPlate->GetDesignItemValue(key,&iType);
		if(iType==0)
			sText.Printf("0.X轴通板");
		else if(iType==1)
			sText.Printf("1.X+向板");
		else if(iType==2)
			sText.Printf("2.X-向板");
		else if(iType==3)
			sText.Printf("3.Y轴通板");
		else if(iType==4)
			sText.Printf("4.Y+向板");
		else if(iType==5)
			sText.Printf("5.Y-向板");
	}
	else
	{
		if(0==pItem->dataType)
		{
			sText.Printf("%f",pItem->value.fValue);
			SimplifiedNumString(sText);
		}
		else if(1==pItem->dataType)
			sText.Printf("0x%X",pItem->value.hPart);
		else if(2==pItem->dataType)
			sText.Printf("%d",pItem->value.iType);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return sText.GetLength();
}
void CLDSParamPlate::InitObjParamItemsPropHashTable(long idInitial)
{
	InitBasePlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_BASEPLATE),idInitial);
	InitShoePlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_SHOEPLATE),idInitial);
	InitRibPlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_RIBPLATE),idInitial);
	InitAngleRibPlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_LINEANGLERIBPLATE),idInitial);
	InitStayWirePlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_TUBESTAYWIREPLATE),idInitial);
	InitStationConnPlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_STATIONCONNPLATE),idInitial);
	InitStationShoePlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_STATIONSHOEPLATE),idInitial);
	InitStationLinkPlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_STATIONLINKPLATE),idInitial);
	InitElbowPlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_ELBOWPLATE),idInitial);
	InitStationCrossPlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_STATIONCROSSPLATE),idInitial);
	InitWaterShieldedPlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_WATERSHIELDEDPLATE),idInitial);
	InitCircularCoverPlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_CIRCOVERPLATE),idInitial);
	InitCircularRibPlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_CIRRIBPLATE),idInitial);
	InitTubeRibPlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_TUBERIBPLATE),idInitial);
	InitFlPlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_FL),idInitial);
	InitPedalPlateParamItemPropHashtable(hashParamItemPropGroup.Add(TYPE_PEDALPLATE), idInitial);
}
long CLDSParamPlate::GetPropID(const char* propStr,int iParamType)
{
	BYTE level=Log2File()->WarningLevel();
	Log2File()->SetWarningLevel(CLogFile::WARNING_LEVEL0_NONE);
	int id=GetPropID(propStr);
	Log2File()->SetWarningLevel(level);
	if(0==id)
	{
		DYNA_PROPITEM_GROUP* pParamPropGroup=hashParamItemPropGroup.GetValue(iParamType);
		if(pParamPropGroup!=NULL)
			return pParamPropGroup->GetPropID(propStr);
		else
		{
			if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("Don't find property Id named '%s'",propStr);
#else
				log2file->Log("没找到名为'%s'的属性Id",propStr);
#endif
			return 0;
		}
	}
	else
		return id;
}
KEY2C CLDSParamPlate::GetPropKey(long id,int iParamType)
{
	DYNA_PROPITEM_GROUP *pPropItemGroup=CLDSParamPlate::hashParamItemPropGroup.GetValue(iParamType);
	if(pPropItemGroup)
	{
		PROPLIST_ITEM *pPropItem=pPropItemGroup->GetPropItem(id);
		if(pPropItem)
			return KEY2C(pPropItem->propKeyStr);
	}
	return KEY2C();
}
long CLDSParamPlate::GetDbPropId(KEY2C paramKey,int iParamType)
{
	long idDbProp=0;
	if(iParamType==TYPE_STATIONCONNPLATE)
	{
		if(KEY2C("r")==paramKey)
			idDbProp=PLATE_PROPID::STATIONCONNPLATE::PROP_HOLE_R;
		else if(KEY2C("H")==paramKey)
			idDbProp=PLATE_PROPID::STATIONCONNPLATE::PROP_HOLE_H;
	}
	else if(iParamType==TYPE_BASEPLATE)
	{
		if(KEY2C("R")==paramKey)
			idDbProp=PLATE_PROPID::BASE::R;
		else if(KEY2C("XA")==paramKey||KEY2C("XB")==paramKey||KEY2C("YA")==paramKey||KEY2C("YB")==paramKey)
			idDbProp=DBOBJ_PROPID::PROP_PROFILE;
	}
	else if(iParamType==TYPE_STATIONCROSSPLATE)
	{
		if(KEY2C("H")==paramKey)
			idDbProp=PLATE_PROPID::STATIONCROSSPLATE::PROP_PARA_H;
	}
	return idDbProp;
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CLDSParamPlate::GetObjPropReadOnlyDefaultState(long id)
{
	return GetObjPropReadOnlyDefaultState(id,ShadowMode());
}
BOOL CLDSParamPlate::GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode)
{
	if (shadow_mode==CLDSObject::SHADOWMODE_COMMON)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else if(GetPropID("cMaterial")==id)
			return FALSE;
		else if(GetPropID("m_cQualityLevel")==id)
			return FALSE;
		else if(GetPropID("m_hPartWeldParent")==id)
			return FALSE;
		else if(GetPropID("sPartNo")==id)
			return FALSE;
		else if(GetPropID("cfgword")==id)
			return FALSE;
		else if(GetPropID("m_uStatMatCoef")==id)
			return FALSE;
		else if(GetPropID("sNotes")==id)
			return FALSE;
		else if(GetPropID("relativeObjs")==id)
			return FALSE;
		else
			return TRUE;
	}
	else if(shadow_mode==CLDSObject::SHADOWMODE_BLOCK)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else if(GetPropID("m_hPartWeldParent")==id)
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(GetPropID("handle")==id)
			return TRUE;
		else
			return FALSE;
	}
}
ATTACHPART_ITEM* CLDSParamPlate::AppendAttachPart(CLDSDbObject *pObj)
{
	if(pObj==NULL)
		return NULL;
	for(ATTACHPART_ITEM* pItem=AttachPart.GetFirst();pItem;pItem=AttachPart.GetNext())
	{	//避免重复添加同一构件
		if(pItem->h==pObj->handle)
			return pItem;
	}
	ATTACHPART_ITEM  *pPartItem=AttachPart.Add(pObj->handle);
	pPartItem->idClsType = pObj->GetClassTypeId();
	if(pObj->IsPart())
		strcpy(pPartItem->sPartNo,((CLDSPart*)pObj)->GetPartNo());
	return pPartItem;
}

//查找指定构件是否在附加构件列表中
BOOL CLDSParamPlate::IsInAttachPartList(long h)
{
	if(h<0X20)
		return FALSE;
	for(ATTACHPART_ITEM* pItem=AttachPart.GetFirst();pItem;pItem=AttachPart.GetNext())
	{	
		if(pItem->h==h)
			return TRUE;
	}
	return FALSE;
}

int CLDSParamPlate::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	char steelmark[20]="";
	char tmp_spec[200];
	if(bIncMat&&cMaterial!='S')
		QuerySteelMatMark(cMaterial,steelmark);
	int plate_thick=(int)GetThick();
	sprintf(tmp_spec,"%s-%d",steelmark,plate_thick);
	SimplifiedNumString(tmp_spec);
	if(spec)
		strcpy(spec,tmp_spec);
	return strlen(tmp_spec);
}

bool CLDSParamPlate::IsZhiWan()
{
	CLDSParamPlate* pMotherPlate=(CLDSParamPlate*)MotherPart();
	if(pMotherPlate!=this)
		return pMotherPlate->IsZhiWan();
	if(m_iParamType==TYPE_UEND||m_iParamType==TYPE_ROLLEND)
		return true;
	else if(m_iParamType==TYPE_CIRRIBPLATE||m_iParamType==TYPE_TUBERIBPLATE||m_iParamType==TYPE_RIBPLATE)
	{
		BOOL bPush=vertex_list.push_stack();
		for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pVertex->vertex.feature>1)
			{
				if(bPush)
					vertex_list.pop_stack();
				return true;
			}
		}
		if(bPush)
			vertex_list.pop_stack();
	}
	return false;
}
int CLDSParamPlate::m_iCirclePlankAreaType=2;
double CLDSParamPlate::GetWidth()
{
	CLDSParamPlate* pMotherPlate=(CLDSParamPlate*)MotherPart();
	if(pMotherPlate!=this)
		return pMotherPlate->GetWidth();
	if(m_bStdPart)
	{	//标准构件
		CLDSStdPartModel stdPartModel;
		if(GetStdPartModel(&stdPartModel))
		{
			if(m_iParamType==TYPE_UEND||m_iParamType==TYPE_XEND)	
				return stdPartModel.param.insert_plate_param.H;	//U/X型插板
			else if(m_iParamType==TYPE_ROLLEND)
				return stdPartModel.param.insert_plate_param.H+2*stdPartModel.param.insert_plate_param.W
				-8*stdPartModel.param.insert_plate_param.Thick/3;//以板厚1/3为板弯曲中性层
			else if(m_iParamType==TYPE_FLD||m_iParamType==TYPE_FLP)
				return (stdPartModel.param.fl_param.D-stdPartModel.param.fl_param.B)*0.5;//对/平焊法兰
			else if(m_iParamType==TYPE_FLG||m_iParamType==TYPE_FLR)
				return (stdPartModel.param.fl_param2.fOuterD-stdPartModel.param.fl_param2.fInnerD)*0.5;
		}
		else
			return 0;
	}
	//非标准构件
	if(m_iParamType==TYPE_FL)
	{
		double W,R;
		GetDesignItemValue('W',&W);
		GetDesignItemValue('R',&R);
		if(m_iCirclePlankAreaType==2)
			return W*0.5-R;
		else	//包络矩形
		{
			f2dRect rect=GetMinWrapRect();
			if(rect.Width()<rect.Height())
				return rect.Width();
			else
				return rect.Height();
		}
	}
	else if(m_iParamType==TYPE_UEND||m_iParamType==TYPE_XEND)
	{
		double H;
		GetDesignItemValue('H',&H);
		return H;
	}
	else if(m_iParamType==TYPE_ROLLEND)
	{
		double H,W;
		GetDesignItemValue('H',&H);
		GetDesignItemValue('W',&W);
		return H+2*W;
	}
	else if(m_iParamType==TYPE_ELBOWPLATE)
	{
		double B;
		GetDesignItemValue('B',&B);
		return B;
	}
	else if(m_iParamType==TYPE_CIRCOVERPLATE)
	{	//环向封板
		double B,R,W;
		GetDesignItemValue('B',&B);
		GetDesignItemValue('R',&R);
		GetDesignItemValue('W',&W);
		if(m_iCirclePlankAreaType==2)
			return W;
		else
		{
			f2dRect rect=GetMinWrapRect();
			if(rect.Width()<rect.Height())
				return rect.Width();
			else
				return rect.Height();
		}
	}
	else if(m_iParamType==TYPE_LINEANGLERIBPLATE)
	{
		double L=0;
		GetDesignItemValue('L',&L);
		return L;
	}
	else
	{
		SCOPE_STRU scope;
		if(vertex_list.GetNodeNum()==0)
			DesignPlate();
		for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			scope.VerifyVertex(pVertex->vertex);
		if(scope.wide()>0)
			return scope.wide();
	}
	return 0;
}

double CLDSParamPlate::GetLength()
{
	CLDSParamPlate* pMotherPlate=(CLDSParamPlate*)MotherPart();
	if(pMotherPlate!=this)
		return pMotherPlate->GetLength();
	if(m_bStdPart)
	{	//标准构件
		CLDSStdPartModel stdPartModel;
		if(library!=NULL&&library->GetStdPartModel(sPartNo,m_iParamType,&stdPartModel))
		{
			if(m_iParamType==TYPE_ROLLEND||m_iParamType==TYPE_XEND)//X/C型插板	
				return stdPartModel.param.insert_plate_param.L;	
			else if(m_iParamType==TYPE_UEND)	//以板厚1/2为板弯曲中性层
				return (stdPartModel.param.insert_plate_param.L-stdPartModel.param.insert_plate_param.Thick)*2
				+(Pi-2)*(stdPartModel.param.insert_plate_param.Thick+stdPartModel.param.insert_plate_param.W)/2;
			else if(m_iParamType==TYPE_FLD||m_iParamType==TYPE_FLP)	//对/平焊法兰
			{
				double R0=stdPartModel.param.fl_param.D*0.5;
				double R=stdPartModel.param.fl_param.B*0.5;
				return Pi*(R0*R0-R*R)/(R0-R);	
			}
			else if(m_iParamType==TYPE_FLG||m_iParamType==TYPE_FLR)	
			{
				double R0=stdPartModel.param.fl_param2.fOuterD*0.5;
				double R=stdPartModel.param.fl_param2.fInnerD*0.5;
				return Pi*(R0*R0-R*R)/(R0-R);	
			}
		}
		else
			return 0;	
	}
	//非标准构件
	if(m_iParamType==TYPE_FL)
	{
		double A=360,W=0,R=0;
		GetDesignItemValue('A',&A);
		GetDesignItemValue('W',&W);
		GetDesignItemValue('R',&R);
		if(m_iCirclePlankAreaType==2)
			return (A/360)*Pi*(0.25*W*W-R*R)/(0.5*W-R);
		else
		{
			f2dRect rect=GetMinWrapRect();
			if(rect.Width()<rect.Height())
				return rect.Height();
			else
				return rect.Width();
		}
	}
	else if(m_iParamType==TYPE_UEND)
	{
		double L,W;
		GetDesignItemValue('L',&L);
		GetDesignItemValue('W',&W);
		return (L-thick)*2+(Pi-2)*(thick+W)/2;	//以板厚1/2为板弯曲中性层
	}
	else if(m_iParamType==TYPE_ROLLEND)
	{
		double L;
		GetDesignItemValue('L',&L);
		return L;	
	}
	else if(m_iParamType==TYPE_ELBOWPLATE)
	{
		double A;
		GetDesignItemValue('A',&A);
		return A;
	}
	else if(m_iParamType==TYPE_LINEANGLERIBPLATE||m_iParamType==TYPE_XEND)
	{
		double L=0;
		GetDesignItemValue('L',&L);
		return L;
	}
	else if(m_iParamType==TYPE_CIRCOVERPLATE)
	{	//环向封板
		double B,R,W;
		GetDesignItemValue('B',&B);
		GetDesignItemValue('R',&R);
		GetDesignItemValue('W',&W);
		if(m_iCirclePlankAreaType==2)
			return (B/360)*Pi*((R+W)*(R+W)-R*R)/W;
		else
		{
			f2dRect rect=GetMinWrapRect();
			if(rect.Width()<rect.Height())
				return rect.Height();
			else
				return rect.Width();
		}
	}
	else
	{
		SCOPE_STRU scope;
		if(vertex_list.GetNodeNum()==0)
			DesignPlate();
		for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			scope.VerifyVertex(pVertex->vertex);
		if(scope.high()>0)
			return scope.high();
	}
	return 0;
}
double CLDSParamPlate::GetPrjDrawingWidth()	//获得投影图宽度
{
	CLDSParamPlate* pMotherPlate=(CLDSParamPlate*)MotherPart();
	if(pMotherPlate!=this)
		return pMotherPlate->GetPrjDrawingWidth();
	if(m_iParamType==TYPE_ROLLEND)
	{
		if(m_bStdPart)
		{	//标准构件
			CLDSStdPartModel stdPartModel;
			if(library!=NULL&&library->GetStdPartModel(sPartNo,m_iParamType,&stdPartModel))
				return stdPartModel.param.insert_plate_param.H;	
			else
				return 0;
		}
		else
		{
			double H;
			GetDesignItemValue('H',&H);
			return H;
		}
	}
	else
		return GetWidth();
}
double CLDSParamPlate::GetPrjDrawingLength()	//获得投影图长度
{
	CLDSParamPlate* pMotherPlate=(CLDSParamPlate*)MotherPart();
	if(pMotherPlate!=this)
		return pMotherPlate->GetPrjDrawingLength();
	if(m_iParamType==TYPE_UEND)
	{
		if(m_bStdPart)
		{	//标准构件
			CLDSStdPartModel stdPartModel;
			if(library!=NULL&&library->GetStdPartModel(sPartNo,m_iParamType,&stdPartModel))
				return stdPartModel.param.insert_plate_param.L;	
			else
				return 0;
		}
		else
		{
			double L;
			GetDesignItemValue('L',&L);
			return L;
		}
	}
	else
		return GetLength();
}

double CLDSParamPlate::GetThick()
{
	CLDSParamPlate* pMotherPlate=(CLDSParamPlate*)MotherPart();
	if(pMotherPlate!=this)
		return pMotherPlate->GetThick();
	if(m_bStdPart)
	{	//标准构件
		CLDSStdPartModel stdPartModel;
		if(library!=NULL&&library->GetStdPartModel(sPartNo,m_iParamType,&stdPartModel))
		{
			if(m_iParamType==TYPE_UEND||m_iParamType==TYPE_ROLLEND||m_iParamType==TYPE_XEND)	
				return stdPartModel.param.insert_plate_param.Thick;	//U/X/C型插板
			else if(m_iParamType==TYPE_FLD||m_iParamType==TYPE_FLP)
				return stdPartModel.param.fl_param.C;				//对/平焊法兰
			else if(m_iParamType==TYPE_FLG||m_iParamType==TYPE_FLR)
				return stdPartModel.param.fl_param2.fThick;			//刚性/柔性法兰
		}
		else
			return 0;
	}	
	else //非标准构件
		return thick;
	return 0;
}

double CLDSParamPlate::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	CLDSParamPlate* pMotherPlate=(CLDSParamPlate*)MotherPart();
	if(pMotherPlate!=this)
		return pMotherPlate->GetWeight(bSolidLength);
	if(m_bStdPart)
	{	//标准构件
		CLDSStdPartModel stdPartModel;
		if(library!=NULL&&library->GetStdPartModel(sPartNo,m_iParamType,&stdPartModel))
		{
			if(m_iParamType==TYPE_UEND||m_iParamType==TYPE_ROLLEND||m_iParamType==TYPE_XEND)	
				return stdPartModel.param.insert_plate_param.theroy_weight;	//U/X/C型插板
			else if(m_iParamType==TYPE_FLD||m_iParamType==TYPE_FLP)	//对/平焊法兰
			{
				stdPartModel.param.fl_param.t=GetThick();
				return stdPartModel.param.fl_param.Weight();
			}
			else if(m_iParamType==TYPE_FLR||m_iParamType==TYPE_FLG)
				return stdPartModel.param.fl_param2.theroy_weight;
		}
		else
			return 0;
	}
	else //非标准构件
		return GetWidth()*GetLength()*thick*7.85e-6;//CalSteelPlankWei(GetWidth()*GetLength()*thick);
	return 0;
}

void CLDSParamPlate::CopyProperty(CLDSDbObject *pObj)
{
	if(pObj->GetClassTypeId()!=CLS_PARAMPLATE)
		return;
	CLDSParamPlate *pPlate=(CLDSParamPlate*)pObj;
	thick=(long)pPlate->GetThick();			//板厚
	//m_fNormOffset=pPlate->m_fNormOffset;	//装配时法向位移量，设此参数便于钢管上焊接板的位置设定
	//m_fInnerRadius=pPlate->m_fInnerRadius;//内圆弧半径
	cMaterial=pPlate->cMaterial;
	cbVirginQualityLevel = pPlate->cbVirginQualityLevel;
	cfgword=pPlate->cfgword;
	iSeg=pPlate->iSeg;
	m_uStatMatCoef=pPlate->m_uStatMatCoef;
	m_bDisplayHole=pPlate->m_bDisplayHole;
	sPartNo.Copy(pPlate->sPartNo);
	strcpy(sNotes,pPlate->sNotes);
	m_iParamType=pPlate->m_iParamType;
	m_bStdPart=pPlate->m_bStdPart;
	m_fNormOffset=pPlate->m_fNormOffset;//工作面法向偏心距
	params.Empty();
	for(DESIGN_PLATE_ITEM *pItem=pPlate->params.GetFirst();pItem;pItem=pPlate->params.GetNext())
		params.SetValue(pItem->key,*pItem);
}
void CLDSParamPlate::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( version==0||
		(doc_type==1&&version>=4020005)||	//TMA(V4.2.0.5)
		(doc_type==2&&version>=1020005)||	//LMA(V1.2.0.5)
		(doc_type==3&&version>=2000005)||	//TSA(V2.0.0.5)
		(doc_type==4&&version>=1020005)||	//LDS(V1.2.0.5)
		(doc_type==5&&version>=1020005))	//TDA(V1.2.0.5)
	{
		buffer.ReadDword(&dwPermission);
		CfgwordFromBuffer(cfgword,buffer,version,doc_type);
		BelongModel()->EvolveCfgword(cfgword,version,doc_type);
		buffer.ReadString(sPartNo);
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadInteger(&_hPartWeldParent);
		buffer.ReadString(layer_tag,4);
		buffer.ReadString(sNotes,51);
		if(version==0||
			(doc_type==1&&version>=5000010)||
			(doc_type==2&&version>=2000005)||
			(doc_type==4&&version>=1030005)||
			(doc_type==5&&version>=1030005))
		{
			buffer.ReadBooleanThin(&_bSyncShadowPartNo);
			if(!_bSyncShadowPartNo)
			{
				buffer.ReadInteger(&_iSeg.iSeg);
				buffer.ReadString(sPartNo,16);
			}
		}
		workNorm.FromBuffer(buffer,version,doc_type);
		buffer.ReadPoint(ucs.origin);
		buffer.ReadPoint(ucs.axis_x);
		buffer.ReadPoint(ucs.axis_y);
		buffer.ReadPoint(ucs.axis_z);
		buffer.ReadDouble(&m_fNormOffset);
		int n=0;
		AttachPart.Empty();
		buffer.ReadInteger(&n);
		for(int i=0;i<n;i++)
		{
			long h=0;
			buffer.ReadInteger(&h);
			ATTACHPART_ITEM *pAttachPart=AttachPart.Add(h);
			buffer.ReadInteger(&pAttachPart->idClsType);
			buffer.ReadString(pAttachPart->sPartNo);
		}
		buffer.ReadInteger(&n);
		vertex_list.Empty();
		for(int ii=0;ii<n;ii++)
		{
			f3dPoint vertex;
			buffer.ReadPoint(vertex);
			PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertex));
			buffer.ReadInteger(&pVertex->vertex.feature);
			buffer.ReadInteger(&pVertex->vertex.ID);
			buffer.ReadByte(&pVertex->type);
			buffer.ReadBooleanThin(&pVertex->m_bWeldEdge);
			buffer.ReadBooleanThin(&pVertex->m_bRollEdge);
			if(pVertex->type==2||pVertex->type==3)	//指定圆弧半径
			{
				buffer.ReadDouble(&pVertex->radius);
				buffer.ReadPoint(pVertex->center); 
			}
			if(pVertex->type==1)	//指定圆弧夹角
				buffer.ReadDouble(&pVertex->sector_angle);
			buffer.ReadWord(&pVertex->manu_space);
			if(pVertex->type==3)	//椭圆弧
				buffer.ReadPoint(pVertex->column_norm);
		}
		buffer.ReadInteger(&n);
		for(int j=0;j<n;j++)
		{
			RELATIVE_OBJECT *pMirObj=relativeObjs.Add(buffer.ReadInteger());
			buffer.ReadByte(&pMirObj->mirInfo.axis_flag);
			buffer.ReadPoint(pMirObj->mirInfo.origin);
			if((pMirObj->mirInfo.axis_flag&0x08)>0)
			{
				buffer.ReadWord(&pMirObj->mirInfo.rotate_angle);
				buffer.ReadByte(&pMirObj->mirInfo.array_num);
			}
			if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
				buffer.ReadPoint(pMirObj->mirInfo.mir_norm);
		}
	}
	pLsRefList->FromBuffer(buffer,BelongModel(),version,doc_type);
}
void CLDSParamPlate::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if( version==0||
		(doc_type==1&&version>=4020005)||	//TMA(V4.2.0.5)
		(doc_type==2&&version>=1020005)||	//LMA(V1.2.0.5)
		(doc_type==3&&version>=2000005)||	//TSA(V2.0.0.5)
		(doc_type==4&&version>=1020005)||	//LDS(V1.2.0.5)
		(doc_type==5&&version>=1020005))	//TDA(V1.2.0.5)
	{
		buffer.WriteDword(dwPermission);
		CfgwordToBuffer(cfgword,buffer);
		buffer.WriteString(sPartNo);
		buffer.WriteInteger(m_uStatMatCoef);
		buffer.WriteInteger(m_hPartWeldParent);
		buffer.WriteString(layer_tag);
		buffer.WriteString(sNotes);
		if(version==0||
			(doc_type==1&&version>=5000010)||
			(doc_type==2&&version>=2000005)||
			(doc_type==4&&version>=1030005)||
			(doc_type==5&&version>=1030005))
		{
			buffer.WriteBooleanThin(_bSyncShadowPartNo);
			if(!_bSyncShadowPartNo)
			{
				buffer.WriteInteger(_iSeg.iSeg);
				buffer.WriteString(sPartNo,16);
			}
		}
		workNorm.ToBuffer(buffer,version,doc_type);
		buffer.WritePoint(ucs.origin);
		buffer.WritePoint(ucs.axis_x);
		buffer.WritePoint(ucs.axis_y);
		buffer.WritePoint(ucs.axis_z);
		buffer.WriteDouble(m_fNormOffset);//法向偏距
		buffer.WriteInteger(AttachPart.GetNodeNum());
		BOOL bPush=AttachPart.push_stack();
		for(ATTACHPART_ITEM *pAttachPart=AttachPart.GetFirst();pAttachPart;pAttachPart=AttachPart.GetNext())
		{
			buffer.WriteInteger(pAttachPart->h);
			buffer.WriteInteger(pAttachPart->idClsType);
			buffer.WriteString(pAttachPart->sPartNo);
		}
		if(bPush)
			AttachPart.pop_stack();
		buffer.WriteInteger(vertex_list.GetNodeNum());
		bPush=vertex_list.push_stack();
		for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			buffer.WritePoint(pVertex->vertex);
			buffer.WriteInteger(pVertex->vertex.feature);
			buffer.WriteInteger(pVertex->vertex.ID);
			buffer.WriteByte(pVertex->type);
			buffer.WriteBooleanThin(pVertex->m_bWeldEdge);
			buffer.WriteBooleanThin(pVertex->m_bRollEdge);
			if(pVertex->type==2||pVertex->type==3)	//指定圆弧半径
			{
				buffer.WriteDouble(pVertex->radius);
				buffer.WritePoint(pVertex->center); 
			}
			if(pVertex->type==1)	//指定圆弧夹角
				buffer.WriteDouble(pVertex->sector_angle);
			buffer.WriteWord(pVertex->manu_space);
			if(pVertex->type==3)	//椭圆弧
				buffer.WritePoint(pVertex->column_norm);
		}
		if(bPush)
			vertex_list.pop_stack();
		buffer.WriteInteger(relativeObjs.GetNodeNum());
		for(RELATIVE_OBJECT *pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
		{
			buffer.WriteInteger(pMirObj->hObj);
			buffer.WriteByte(pMirObj->mirInfo.axis_flag);
			buffer.WritePoint(pMirObj->mirInfo.origin);
			if((pMirObj->mirInfo.axis_flag&8)>0)
			{
				buffer.WriteWord(pMirObj->mirInfo.rotate_angle);
				buffer.WriteByte(pMirObj->mirInfo.array_num);
			}
			if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
				buffer.WritePoint(pMirObj->mirInfo.mir_norm);
		}
	}
	//连接板螺栓区
	pLsRefList->ToBuffer(buffer,version,doc_type);
}
void CLDSParamPlate::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropFromBuffer(buffer,version,doc_type);
		return;
	}
	DWORD dw;
	buffer.ReadDword(&dwPermission);
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	if( version==0||
	   (version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
	   (version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
		mpcl.FromBuffer(&buffer,version,doc_type);
	buffer.ReadString(sPartNo);
	buffer.ReadDword(&m_uStatMatCoef);
	buffer.ReadInteger(&_hPartWeldParent);
	buffer.ReadString(layer(),4);
	buffer.ReadInteger(&_iSeg.iSeg);
	if(  version==0||
		(doc_type==1&&version>=5000200)||	//TMA V5.0.2.0
		(doc_type==2&&version>=2000200)||	//LMA V2.0.2.0
		(doc_type==3&&version>=2010100)||	//LMA V2.1.1.0
		(doc_type==4&&version>=1030200)||	//LDS V1.3.2.0
		(doc_type==5&&version>=1030200))	//TDA V1.3.2.0
		buffer.ReadBooleanThin(&_bVirtualPart);
	buffer.ReadByte(&_material);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.ReadByte(&_cQualityLevel);
	if(CTower::IsPrevDeflateVersion(version,doc_type))
	{
		buffer.ReadInteger(&dw);//显示螺栓孔及切割面
		m_bDisplayHole=(dw!=0);
	}
	else
		buffer.ReadBooleanThin(&m_bDisplayHole);
	int iTm=0;
	buffer.ReadInteger(&iTm);
	m_iParamType=PARAM_PLATE_TYPE(iTm);//TYPE_FL;
	if(  version==0|| doc_type==5||
		(doc_type==1&&version>=4000105)||(doc_type==2&&version>=1000105)||
		(doc_type==3&&version>=1070305)||(doc_type==4&&version>=1000305))
		workNorm.FromBuffer(buffer,version,doc_type);
	buffer.ReadPoint(ucs.origin);
	buffer.ReadPoint(ucs.axis_x);
	buffer.ReadPoint(ucs.axis_y);
	buffer.ReadPoint(ucs.axis_z);
	design.inited=false;
	workNorm.vector=ucs.axis_z;
	buffer.ReadInteger(&_thick);
	buffer.ReadInteger(&_bStdPart);
	if(	version==0||
		(doc_type==1&&version>=5010500)||	//TMA V5.1.5.0
		(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&version>=1030700)||	//LDS V1.3.7.0
		(doc_type==5&&version>=1030700))	//TDA V1.3.7.0
		buffer.ReadByte(&m_cReverseAxisVec);
	buffer.ReadInteger(&iTm);
	params.Empty();
	DESIGN_PLATE_ITEM *pItem;
	int i=0;
	for(i=0;i<iTm;i++)
	{
		KEY2C key;
		if(  version==0||
			(doc_type==1&&version>=4020006)||(doc_type==2&&version>=1020006)||
			(doc_type==3&&version>=2000006)||(doc_type==4&&version>=1020006)||(doc_type==5&&version>=1020006))
			buffer.ReadWord(&key.wItem);
		else
			buffer.ReadByte((char*)&key);
		pItem=params.Add(key);
		buffer.ReadByte(&pItem->dataType);
		if(pItem->dataType==1)
			buffer.ReadInteger(&pItem->value.hPart);
		else if(pItem->dataType==2)
			buffer.ReadInteger(&pItem->value.iType);
		else if(pItem->dataType==3)
		{
			buffer.ReadInteger(&pItem->value.hObj);
			buffer.ReadInteger(&pItem->value.uiAttribute);
		}
		else
			buffer.ReadDouble(&pItem->value.fValue);
		if(_iParamType==TYPE_ELBOWPLATE&&pItem->key.IsEqual(KEY2C("Y"))&&pItem->dataType!=0)
			pItem->dataType=DESIGN_PARAM_ITEM::TYPE_INTEGER;	//肘板中'Y'参数表示旋转定位角,个别情况此值的类型可能因未明原因写成了句柄 wjh-2015.8.7
		buffer.ReadString(pItem->sNotes);
		buffer.ReadByte(&pItem->paramType);
		//自钢管基准面旋转角度现转到workNorm中，wjh-2011.7.6
		if(m_iParamType==TYPE_XEND&&pItem->key=='D')
		{
			workNorm.norm_style=5;
			workNorm.hVicePart=m_hPartWeldParent;
			workNorm.direction=2;
			workNorm.rot_ang=pItem->value.fValue;
		}
	}
	long hDatumPart=0;
	if(  version>0&&m_iParamType==TYPE_RIBPLATE&&GetDesignItemValue('A',&hDatumPart)&&(
		(doc_type==1&&version<4020006)||(doc_type==2&&version<1020006)||//(doc_type==3&&version<2000006)||
		(doc_type==4&&version<1020006)||(doc_type==5&&version<1020006)))
	{
		if((pItem=GetDesignItem('Y'))!=NULL)
		{
			pItem->key=KEY2C("DM");
			if(hDatumPart<0)
				pItem->value.fValue*=-1.0;	//因偏移量统一为基准杆件延利方向，因此此值需要反号　wjh-2013.4.5
		}
		hDatumPart*=-1;
	}
	buffer.ReadDouble(&m_fNormOffset);
	bool bBeforVersion101=false;//xxxx101版本之前
//#if defined(__TMA_FILE_)||defined(__TMA_)
	if(doc_type==1&&version>0&&version<=4000101)
		bBeforVersion101=true;
//#elif defined(__LMA_FILE_)||defined(__LMA_)
	else if(doc_type==2&&version>0&&version<=1000101)
		bBeforVersion101=true;
//#elif defined(__LDS_FILE_)||defined(__LDS_)
	else if(doc_type==4&&version>0&&version<=1000301)
		bBeforVersion101=true;
//#elif defined(__TDA_FILE_)
	else if(doc_type==5)
		bBeforVersion101=false;
//#endif
	CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
	if(bBeforVersion101&&pDatumTube
		&&(m_iParamType==TYPE_CIRRIBPLATE||m_iParamType==TYPE_TUBERIBPLATE))
	{	//转换以前版本的环向板参数为当前版本参数 wht 11-04-20
		long C=0,P=0,E=0,F=0,Y=-1,G=-1,hVecPole=0;
		double S=0,T=0,V=0,W=0,N=0,H=0,D=0,B=0;
		if(m_iParamType==TYPE_CIRRIBPLATE)
		{	//环向肋板
			long O=-1;
			double R=0;
			GetDesignItemValue('A',&P,NULL);	//基准节点
			GetDesignItemValue('B',&E,NULL);	//第一块基准钢板
			GetDesignItemValue('C',&F,NULL);	//第二块基准钢板
			GetDesignItemValue('O',&O,NULL);	//顶部圆弧 >0表示顶部为圆弧 <0为直线
			G=(O>0)?-1:1;	
			GetDesignItemValue('Y',&Y,NULL);	//>=0外侧,<0内侧
			//外形参数
			GetDesignItemValue('W',&W,NULL);	
			GetDesignItemValue('N',&N,NULL);
			GetDesignItemValue('H',&H,NULL);
			GetDesignItemValue('S',&S,NULL);	//法向偏距
			GetDesignItemValue('R',&R,NULL);	//环向板内圆弧半径包括加工间隙
			V=R-pDatumTube->GetDiameter()*0.5;
			GetDesignItemValue('D',&D,NULL);	//一块基准钢板且顶部为圆弧时D表示扇形角
			if(O>0&&F<0x20)	//仅有一块基准钢板
				B=D;
		}
		else if(m_iParamType==TYPE_TUBERIBPLATE)
		{	//跨越连板
			long A=-1;
			GetDesignItemValue('P',&P,NULL);	//基准节点
			GetDesignItemValue('B',&E,NULL);	//第一块基准钢板
			GetDesignItemValue('C',&F,NULL);	//第二块基准钢板
			GetDesignItemValue('A',&A,NULL);	//顶部圆弧 >0表示顶部为圆弧 <0为直线
			G=(A>0)?-1:1;
			GetDesignItemValue('Y',&Y,NULL);	//>=0外侧,<0内侧
			//GetDesignItemValue('F',&F,NULL);	//>=0套在钢管上的环形板
			GetDesignItemValue('S',&hVecPole,NULL);	//确定X轴方向的杆件
			//
			GetDesignItemValue('W',&W,NULL);	
			GetDesignItemValue('N',&N,NULL);
			GetDesignItemValue('H',&H,NULL);
			GetDesignItemValue('V',&V,NULL);	//加工间隙
			GetDesignItemValue('D',&S,NULL);	//法向偏距
			double R=0;
			GetDesignItemValue('R',&R,NULL);	//环向板外圆弧半径
			D=R-pDatumTube->GetDiameter()*0.5;
		}
		//
		params.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
		SetDesignItemValue('C',pDatumTube->handle,"Locate datum tube",PARAM_POS,1);
		SetDesignItemValue('P',P,"Locate datum node",PARAM_POS,1);
		SetDesignItemValue('E',E,"First datum plate",PARAM_POS,1);
		SetDesignItemValue('F',F,"Second datum plate",PARAM_POS,1);
		SetDesignItemValue('G',G,"Free edge type(>0 Straight，<=0 Circular)");
		SetDesignItemValue('Y',Y,"Semicircle plate position(>0 Outside of datum plate;=0 Auto;<0 Inside of datum plate)",PARAM_SHAPE);
#else
		SetDesignItemValue('C',pDatumTube->handle,"定位基准钢管",PARAM_POS,1);
		SetDesignItemValue('P',P,"定位基准节点",PARAM_POS,1);
		SetDesignItemValue('E',E,"第一块基准钢板",PARAM_POS,1);
		SetDesignItemValue('F',F,"第二块基准钢板",PARAM_POS,1);
		SetDesignItemValue('G',G,"自由边类型(>0直线，<=0圆弧)");
		SetDesignItemValue('Y',Y,"半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)",PARAM_SHAPE);
#endif
		//保存基准面法线
		ConnDirectList.Empty();
		if(m_iParamType==TYPE_CIRRIBPLATE)
		{
			CONNECT_DIRECTION *pNewVec=ConnDirectList.Add(1);
			pNewVec->serial=1;
			pNewVec->style=1;
			pNewVec->direct.norm_style=0;
			pNewVec->direct.vector=ucs.axis_z;
		}
		else if(m_iParamType==TYPE_TUBERIBPLATE
				&&BelongModel()->FromPartHandle(hVecPole,CLS_LINETUBE))
		{	//跨越连板X轴方向
			CONNECT_DIRECTION *pNewVec=ConnDirectList.Add(1);
			pNewVec->style=0;
			pNewVec->direct.norm_style=5;
			pNewVec->direct.hVicePart=hVecPole;
		}
		if(m_iParamType==TYPE_TUBERIBPLATE)
		{	//判断跨越连板的W,N参数是否需要互换
			f3dPoint axis_x=GetRelaConnVec(1,NULL);
			if(ucs.axis_x*axis_x>0)
			{
				double temp=W;
				W=N;
				N=temp;
			}
		}
#ifdef AFX_TARG_ENU_ENGLISH
		SetDesignItemValue('S',S,"The offset along extension direction of datum tube.",PARAM_POS);
		SetDesignItemValue('V',V,"Process space",PARAM_SHAPE);
		SetDesignItemValue('W',W,"Weld seam length of the first relative plate",PARAM_SHAPE);
		SetDesignItemValue('N',N,"Weld seam length of the second relative plate of tube",PARAM_SHAPE);
		SetDesignItemValue('D',D,"Distance from outer wall of datum tube to free edge,free space for short",PARAM_SHAPE);
		SetDesignItemValue('H',H,"Chamfer Height",PARAM_SHAPE);
#else
		SetDesignItemValue('S',S,"沿基准钢管延伸方向的偏移距离。",PARAM_POS);
		SetDesignItemValue('V',V,"加工间隙",PARAM_SHAPE);
		SetDesignItemValue('W',W,"第一块关联钢板的焊缝长",PARAM_SHAPE);
		SetDesignItemValue('N',N,"第二块关联钢板或钢管的焊缝长",PARAM_SHAPE);
		SetDesignItemValue('D',D,"基准钢管外壁到自由边距离，简称自由边距",PARAM_SHAPE);
		SetDesignItemValue('H',H,"倒角高度",PARAM_SHAPE);
#endif
	}

/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version==0||version>=4000028)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version==0||version>=1000083)
#elif defined(__TDA_FILE_)
	if(true)
#else
	if(version==0||version>=1000010)
#endif*/
	if( version==0||doc_type==5||((doc_type==1&&version>=4000028)||
		(doc_type==2&&version>=1000083)||(doc_type>2&&version>=1000010)))
	{	
		BOOL bBeforVersion86=TRUE;	//LMA86版本以前 wht 10-08-27
/*#if defined(__TMA_FILE_)||defined(__TMA_)
		if(version==0||version>=4000031)
#elif defined(__LMA_FILE_)||defined(__LMA_)
		if(version==0||version>=1000086)
#elif defined(__TDA_FILE_)
		if(true)
#else
		if(version==0||version>=1000014)
#endif*/
		if( version==0||doc_type==5||((doc_type==1&&version>=4000031)||
			(doc_type==2&&version>=1000086)||(doc_type>2&&version>=1000014)))
			bBeforVersion86=FALSE;

		if(!bBeforVersion101)
		{	//读取连接方向参数 wht 11-04-12
			ConnDirectList.Empty();
			iTm=0;
			buffer.ReadInteger(&iTm);
			for(i=0;i<iTm;i++)
			{
				CONNECT_DIRECTION connvec;
				buffer.ReadByte(&connvec.style);
				buffer.ReadByte(&connvec.serial);
				buffer.ReadWord(&connvec.high_space);
				buffer.ReadWord(&connvec.end_space);
				CONNECT_DIRECTION *pVec=ConnDirectList.SetValue(connvec.serial,connvec,true);
				pVec->direct.FromBuffer(buffer,version,doc_type);
				pVec->direct.UpdateVector(BelongModel());
			}
		}
		//
		if(	version==0||
			(doc_type==1&&version>=5010300)||	//TMA V5.1.3.0
			(doc_type==2&&version>=2000600)||	//LMA V2.0.6.0
			(doc_type==4&&version>=1030600)||	//LDS V1.3.6.0
			(doc_type==5&&version>=1030600))	//TDA V1.3.6.0
			buffer.ReadByte(&_cRollProcessType);
		if(	version==0||
			(doc_type==1&&version>=5010500)||	//TMA V5.1.5.0
			(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0
			(doc_type==4&&version>=1030700)||	//LDS V1.3.7.0
			(doc_type==5&&version>=1030700))	//TDA V1.3.7.0
		{
			buffer.ReadByte(&_ciRollOffsetType);
			buffer.ReadByte(&_ciDeformType);
		}
		if(	version==0||
			(doc_type==1&&version>=5010600)||	//TMA V5.1.6.0
			(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0
			(doc_type==4&&version>=1030800)||	//LDS V1.3.8.0
			(doc_type==5&&version>=1030800))	//TDA V1.3.8.0
		{
			buffer.ReadDouble(&_fDeformCoef);
			buffer.ReadDouble(&_fHuoquR);
		}
		buffer.ReadInteger(&iTm);
		vertex_list.Empty();
		BOOL bReadHuoquNorm=FALSE;	//是否需要读取火曲面法线
		for(int ii=0;ii<iTm;ii++)
		{
			PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX());
			pVertex->FromBuffer(buffer,version,doc_type,true,&ucs);
			/* 以下代码移至PROFILE_VERTEX::FromBuffer函数
			f3dPoint vertex;
			buffer.ReadPoint(vertex);
			buffer.ReadInteger(&pVertex->vertex.feature);
			buffer.ReadInteger(&pVertex->vertex.ID);
			buffer.ReadByte(&pVertex->type);
			buffer.ReadBooleanThin(&pVertex->m_bWeldEdge);
			buffer.ReadBooleanThin(&pVertex->m_bRollEdge);
			if(pVertex->type==2||pVertex->type==3)	//指定圆弧半径
			{
				buffer.ReadDouble(&pVertex->radius);
				if(!bBeforVersion86)
				{
					buffer.ReadPoint(pVertex->center); //存储圆心 wht 10-08-27
					if(bBeforVersion101&&pVertex->type==2&&(fabs(pVertex->center.z)>EPS||
						fabs(pVertex->center.x)>1000||fabs(pVertex->center.y)>1000))
					{	//type==2的圆弧，center由绝对坐标系转到板相对坐标系
						coord_trans(pVertex->center,ucs,FALSE);
					}
				}
			}
			if(pVertex->type==1)	//指定圆弧夹角
				buffer.ReadDouble(&pVertex->sector_angle);
			buffer.ReadWord(&pVertex->manu_space);
			if(pVertex->type==3)	//椭圆弧
			{
				if(bBeforVersion86)
					buffer.ReadPoint(pVertex->center);
				buffer.ReadPoint(pVertex->column_norm);
			}
			*/
			if(pVertex->vertex.feature>10)
				bReadHuoquNorm=TRUE;	//存在火曲点需要读取火曲线信息
		}
		
		if(bReadHuoquNorm&&!bBeforVersion101)	
		{	//在xxxx101版本之后且存在火曲点时需要读取火曲面法线 wht 11-04-12
			buffer.ReadPoint(keyvecset.huoqufacenorm);
			buffer.ReadPoint(keyvecset.huoquaxis_vec);
		}
	}
	AttachPart.Empty();
	buffer.ReadInteger(&iTm);
	for(i=0;i<iTm;i++)
	{
		long h=0;
		buffer.ReadInteger(&h);
		ATTACHPART_ITEM *pAttachPart=AttachPart.Add(h);
		buffer.ReadInteger(&pAttachPart->idClsType);
		buffer.ReadString(pAttachPart->sPartNo);
	}
/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version==0||version>=4000021)//version==NULL||compareVersion(version,"4.0.0.21")>=0)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version==0||version>=1000076)//version==NULL||compareVersion(version,"1.0.0.76")>=0)
#elif defined(__TDA_FILE_)
	if(true)//version==NULL||compareVersion(version,"1.0.0.76")>=0)
#endif*/
	if( version==0||((doc_type==1&&version>=4000021)||(doc_type==2&&version>=1000076))||doc_type>2)
	{
		if(CTower::IsPrevDeflateVersion(version,doc_type))
		{
			buffer.ReadInteger(&dw);
			m_bUserSpecColor=(dw!=0);
		}
		else
			buffer.ReadBooleanThin(&m_bUserSpecColor);
		if(m_bUserSpecColor)
			buffer.ReadDword(&crMaterial);
	}
	//连接板螺栓区
	pLsRefList->FromBuffer(buffer,BelongModel(),version,doc_type);
	buffer.ReadString(sNotes,51);
	//关联构件列表
	int nn=0;
	buffer.ReadInteger(&nn);
	RELATIVE_OBJECT *pRelaObj=NULL;
	relativeObjs.Empty();
	for(i=0;i<nn;i++)
	{
		pRelaObj=relativeObjs.Add(buffer.ReadInteger());
		if(CTower::IsPrevDeflateVersion(version,doc_type))
		{
			buffer.ReadDword(&dw);
			pRelaObj->mirInfo.axis_flag=(BYTE)dw;
			buffer.ReadInteger(&dw);
			pRelaObj->mirInfo.array_num=(BYTE)dw;
			buffer.ReadPoint(pRelaObj->mirInfo.origin);
			double ff;
			buffer.ReadDouble(&ff);
			pRelaObj->mirInfo.rotate_angle=(short)ff;
		}
		else
		{
			buffer.ReadByte(&pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin;
			buffer.ReadBooleanThin(&bSpecOrigin);
			if(bSpecOrigin)
				buffer.ReadPoint(pRelaObj->mirInfo.origin);
			if(pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.ReadByte(&pRelaObj->mirInfo.array_num);
				buffer.ReadWord(&pRelaObj->mirInfo.rotate_angle);
			}
		}
	}
	SetModified();
}

void CLDSParamPlate::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropToBuffer(buffer,version,doc_type);
		return;
	}
	buffer.WriteDword(dwPermission);
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	if( version==0||
	   (version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
	   (version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
		mpcl.ToBuffer(&buffer,version,doc_type);
	buffer.WriteString(sPartNo);
	buffer.WriteDword(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	buffer.WriteString(layer());
	buffer.WriteInteger(iSeg);
	if(  version==0||
		(doc_type==1&&version>=5000200)||	//TMA V5.0.2.0
		(doc_type==2&&version>=2000200)||	//LMA V2.0.2.0
		(doc_type==3&&version>=2010100)||	//LMA V2.1.1.0
		(doc_type==4&&version>=1030200)||	//LDS V1.3.2.0
		(doc_type==5&&version>=1030200))	//TDA V1.3.2.0
		buffer.WriteBooleanThin(m_bVirtualPart);
	buffer.WriteByte(cMaterial);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.WriteByte(m_cQualityLevel);
	buffer.WriteBooleanThin(m_bDisplayHole);
	buffer.WriteInteger((int)m_iParamType);
	workNorm.ToBuffer(buffer,version,doc_type);
	buffer.WritePoint(ucs.origin);
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_y);
	buffer.WritePoint(ucs.axis_z);
	buffer.WriteInteger(_thick);
	buffer.WriteInteger(m_bStdPart);
	if(	version==0||
		(doc_type==1&&version>=5010500)||	//TMA V5.1.5.0
		(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&version>=1030700)||	//LDS V1.3.7.0
		(doc_type==5&&version>=1030700))	//TDA V1.3.7.0
		buffer.WriteByte(m_cReverseAxisVec);
	BUFFERPOP stack(&buffer,params.GetNodeNum());
	buffer.WriteInteger(params.GetNodeNum());
	for(DESIGN_PLATE_ITEM *pItem=params.GetFirst();pItem;pItem=params.GetNext())
	{
		if(  version==0||
			(doc_type==1&&version>=4020006)||
			(doc_type==2&&version>=1020006)||
			(doc_type==3&&version>=2000006)||
			(doc_type==4&&version>=1020006)||
			(doc_type==5&&version>=1020006))
			buffer.WriteWord(pItem->key);
		else
			buffer.WriteByte((BYTE)pItem->key);
		buffer.WriteByte(pItem->dataType);
		if(pItem->dataType==1)
			buffer.WriteInteger(pItem->value.hPart);
		else if(pItem->dataType==2)
			buffer.WriteInteger(pItem->value.iType);
		else if(pItem->dataType==3)
		{
			buffer.WriteInteger(pItem->value.hObj);
			buffer.WriteInteger(pItem->value.uiAttribute);
		}
		else
			buffer.WriteDouble(pItem->value.fValue);
		buffer.WriteString(pItem->sNotes);
		buffer.WriteByte(pItem->paramType);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of parametric plate is wrong!");
#else
		CLDSObject::Log2File()->Log("钢板设计参数记录数出现错误!");
#endif

	buffer.WriteDouble(m_fNormOffset);//法向偏距

	//保存连接方向参数 wht 11-04-12
    stack.Initialize(&buffer,ConnDirectList.GetNodeNum());
	buffer.WriteInteger(ConnDirectList.GetNodeNum());
	for(CONNECT_DIRECTION *pVec=ConnDirectList.GetFirst();pVec;pVec=ConnDirectList.GetNext())
	{
		buffer.WriteByte(pVec->style);
		buffer.WriteByte(pVec->serial);
		buffer.WriteWord(pVec->high_space);
		buffer.WriteWord(pVec->end_space);
		pVec->direct.ToBuffer(buffer,version,doc_type);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of connect direction is wrong!");
#else
		CLDSObject::Log2File()->Log("连接方向记录数出现错误!");
#endif

	BOOL bSaveHuoquNorm=FALSE;
	if(	version==0||
		(doc_type==1&&version>=5010300)||	//TMA V5.1.3.0
		(doc_type==2&&version>=2000600)||	//LMA V2.0.6.0
		(doc_type==4&&version>=1030600)||	//LDS V1.3.6.0
		(doc_type==5&&version>=1030600))	//TDA V1.3.6.0
		buffer.WriteByte(_cRollProcessType);
	if(	version==0||
		(doc_type==1&&version>=5010500)||	//TMA V5.1.5.0
		(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&version>=1030700)||	//LDS V1.3.7.0
		(doc_type==5&&version>=1030700))	//TDA V1.3.7.0
	{
		buffer.WriteByte(_ciRollOffsetType);
		buffer.WriteByte(_ciDeformType);
	}
	if(	version==0||
		(doc_type==1&&version>=5010600)||	//TMA V5.1.6.0
		(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&version>=1030800)||	//LDS V1.3.8.0
		(doc_type==5&&version>=1030800))	//TDA V1.3.8.0
	{
		buffer.WriteDouble(_fDeformCoef);
		buffer.WriteDouble(_fHuoquR);
	}
	stack.Initialize(&buffer,vertex_list.GetNodeNum());
	buffer.WriteInteger(vertex_list.GetNodeNum());
	BOOL bPush=vertex_list.push_stack();
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->vertex.feature>10)
			bSaveHuoquNorm=TRUE;	//存在火曲点需要保存火曲线信息 wht 11-04-12
		pVertex->ToBuffer(buffer,version,doc_type,true);
		/* 以下代码移至PROFILE_VERTEX::ToBuffer函数
		buffer.WritePoint(pVertex->vertex);
		buffer.WriteInteger(pVertex->vertex.feature);
		buffer.WriteInteger(pVertex->vertex.ID);
		buffer.WriteByte(pVertex->type);
		buffer.WriteBooleanThin(pVertex->m_bWeldEdge);
		buffer.WriteBooleanThin(pVertex->m_bRollEdge);
		if(pVertex->type==2||pVertex->type==3)	//指定圆弧半径
		{
			buffer.WriteDouble(pVertex->radius);
			buffer.WritePoint(pVertex->center); //保存圆心 wht 10-08-27
		}
		if(pVertex->type==1)	//指定圆弧夹角
			buffer.WriteDouble(pVertex->sector_angle);
		buffer.WriteWord(pVertex->manu_space);
		if(pVertex->type==3)	//椭圆弧
			buffer.WritePoint(pVertex->column_norm);
		*/
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of profile vetex is wrong!");
#else
		CLDSObject::Log2File()->Log("轮廓点记录数出现错误!");
#endif

	if(bPush)
		vertex_list.pop_stack();
	if(bSaveHuoquNorm)	
	{	//保存火曲面法线 wht 11-04-12
		buffer.WritePoint(keyvecset.huoqufacenorm);
		buffer.WritePoint(keyvecset.huoquaxis_vec);
	}
	stack.Initialize(&buffer,AttachPart.GetNodeNum());
	buffer.WriteInteger(AttachPart.GetNodeNum());
	bPush=AttachPart.push_stack();
	for(ATTACHPART_ITEM *pAttachPart=AttachPart.GetFirst();pAttachPart;pAttachPart=AttachPart.GetNext())
	{
		buffer.WriteInteger(pAttachPart->h);
		buffer.WriteInteger(pAttachPart->idClsType);
		buffer.WriteString(pAttachPart->sPartNo);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of attach part is wrong!");
#else
		CLDSObject::Log2File()->Log("钢板附属构件记录数出现错误!");
#endif

	if(bPush)
		AttachPart.pop_stack();
	buffer.WriteBooleanThin(m_bUserSpecColor);
	if(m_bUserSpecColor)
		buffer.WriteDword(crMaterial);
	//连接板螺栓区
	pLsRefList->ToBuffer(buffer,version,doc_type);
	buffer.WriteString(sNotes);
	//关联构件列表
	RELATIVE_OBJECT *pRelaObj=NULL;
	stack.Initialize(&buffer,relativeObjs.GetNodeNum());
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pRelaObj->hObj);
		buffer.WriteByte(pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		buffer.WriteBooleanThin(bSpecOrigin);
		if(bSpecOrigin)
			buffer.WritePoint(pRelaObj->mirInfo.origin);
		if(pRelaObj->mirInfo.axis_flag&0x08)
		{
			buffer.WriteByte(pRelaObj->mirInfo.array_num);
			buffer.WriteWord(pRelaObj->mirInfo.rotate_angle);
		}
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of relative object is wrong!");
#else
		CLDSObject::Log2File()->Log("关联构件记录数出现错误!");
#endif

}
void CLDSParamPlate::ToXmlFile(FILE* fp,DWORD schema)
{
	int i=0;
	char sText[500]="";
	fprintf(fp,"<参数化板信息 handle=\"0x%X\" dwPermission=\"%d\">\n",handle,dwPermission);
	fprintf(fp,"<配材号 cfgword=\"%d,%d,%d,%d\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3]);
	fprintf(fp,"<构件编号 sPartNo=\"%s\"/>\n",(char*)sPartNo);
	fprintf(fp,"<统材系数 StatMatCoef=\"%d\"/>\n",m_uStatMatCoef);
	fprintf(fp,"<焊接父构件 PartWeldParent=\"0x%x\"/>\n",m_hPartWeldParent);
	fprintf(fp,"<所在图层 layer=\"%s\"/>\n",layer());
	fprintf(fp,"<段号 iSeg=\"%d\"/>\n",iSeg);
	fprintf(fp,"<材质 cMaterial=\"%c\"/>\n",cMaterial);
	if(m_bDisplayHole)
		strcpy(sText,"true");
	else
		strcpy(sText,"false");
	fprintf(fp,"<是否显示孔 bDisplayHole=\"%s\"/>\n",sText);
	fprintf(fp,"<参数化板样式 ParamType=\"%d\"/>\n",(int)m_iParamType);
	workNorm.ToXmlFile(fp,schema);
	fprintf(fp,"<原点坐标 origin=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.origin.x,ucs.origin.y,ucs.origin.z);
	fprintf(fp,"<X轴方向 axis_x=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_x.x,ucs.axis_x.y,ucs.axis_x.z);
	fprintf(fp,"<Y轴方向 axis_y=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_y.x,ucs.axis_y.y,ucs.axis_y.z);
	fprintf(fp,"<Z轴方向 axis_z=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_z.x,ucs.axis_z.y,ucs.axis_z.z);
	fprintf(fp,"<板厚 thick=\"%d\"/>\n",thick);
	if(m_bStdPart==1)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<是否为标准构件 bStdPart=\"%s\"/>\n",sText);
	fprintf(fp,"<参数设计项 num=\"%d\"/>\n",params.GetNodeNum());
	for(DESIGN_PLATE_ITEM *pItem=params.GetFirst();pItem;pItem=params.GetNext())
	{
		fprintf(fp,"<设计项信息 key=\"%s\" dataType=\"%d\">\n",pItem->key.Label(),pItem->dataType);
		if(pItem->dataType==1)
			fprintf(fp,"<值类型 valueType=\"%d\"/>\n",pItem->value.hPart);
		else if(pItem->dataType==2)
			fprintf(fp,"<值类型 valueType=\"%d\"/>\n",pItem->value.iType);
		else
			fprintf(fp,"<值类型 valueType=\"%d\"/>\n",pItem->value.fValue);
		fprintf(fp,"<注释 Notes=\"%s\"/>\n",(char*)pItem->sNotes);
		fprintf(fp,"<参数类型 paramType=\"%d\"/>\n",pItem->paramType);
		fprintf(fp,"</设计项信息>\n");
	}
	sprintf(sText,"%f",m_fNormOffset);
	SimplifiedNumString(sText);
	fprintf(fp,"<法向偏距 NormOffset=\"%s\"/>\n",sText);
	fprintf(fp,"<连接方向列表 num=\"%d\"/>\n",ConnDirectList.GetNodeNum());
	for(CONNECT_DIRECTION *pVec=ConnDirectList.GetFirst();pVec;pVec=ConnDirectList.GetNext())
	{
		fprintf(fp,"<连接方向信息 style=\"%d\" serial=\"%d\" high_space=\"%d\" end_space=\"%d\"/>\n",pVec->style,pVec->serial,pVec->high_space,pVec->end_space);
		pVec->direct.ToXmlFile(fp,schema);
	}
	BOOL bSaveHuoquNorm=FALSE;
	fprintf(fp,"<顶点列表 num=\"%d\"/>\n",vertex_list.GetNodeNum());
	BOOL bPush=vertex_list.push_stack();
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->vertex.feature>10)
			bSaveHuoquNorm=TRUE;	//存在火曲点需要保存火曲线信息 wht 11-04-12
		fprintf(fp,"<顶点信息>\n");
		fprintf(fp,"<顶点坐标 vertex=\"(%.3f,%.3f,%.3f)\"/>\n",pVertex->vertex.x,pVertex->vertex.y,pVertex->vertex.z);
		fprintf(fp,"<顶点特征 feature=\"%d\"/>\n",pVertex->vertex.feature);
		fprintf(fp,"<顶点ID ID=\"%d\"/>\n",pVertex->vertex.ID);
		fprintf(fp,"<顶点类型 type=\"%d\"/>\n",pVertex->type);
		if(pVertex->m_bWeldEdge)
			strcpy(sText,"true");
		else 
			strcpy(sText,"false");
		fprintf(fp,"<是否需焊缝边 bWeldEdge=\"%s\"/>\n",sText);
		if(pVertex->m_bRollEdge)
			strcpy(sText,"true");
		else 
			strcpy(sText,"false");
		fprintf(fp,"<是否需卷边 bRollEdge=\"%s\"/>\n",sText);
		if(pVertex->type==2||pVertex->type==3)	//指定圆弧半径
		{
			sprintf(sText,"%f",pVertex->radius);
			fprintf(fp,"<指定圆弧半径 radius=\"%s\"/>\n",sText);
			fprintf(fp,"<圆心坐标 center=\"(%.3f,%.3f,%.3f)\"/>\n",pVertex->center.x,pVertex->center.y,pVertex->center.z);
		}
		if(pVertex->type==1)	//指定圆弧夹角
			fprintf(fp,"<圆弧夹角 sector_angle=\"%.2f\"/>\n",pVertex->sector_angle);
		fprintf(fp,"<预留间隙或卷边高度 manu_space=\"%d\"/>\n",pVertex->manu_space);
		if(pVertex->type==3)	//椭圆弧
			fprintf(fp,"<法线方向 norm=\"(%.3f,%.3f,%.3f)\"/>\n",pVertex->column_norm.x,pVertex->column_norm.y,pVertex->column_norm.z);
		fprintf(fp,"</顶点信息>\n");
	}
	if(bPush)
		vertex_list.pop_stack();
	if(bSaveHuoquNorm)	
	{
		fprintf(fp,"<火曲面法向 huoqufacenorm=\"(%.3f,%.3f,%.3f)\"/>\n",keyvecset.huoqufacenorm.x,keyvecset.huoqufacenorm.y,keyvecset.huoqufacenorm.z);
		fprintf(fp,"<火曲线方向 huoquxis_vec=\"(%.3f,%.3f,%.3f)\"/>\n",keyvecset.huoquaxis_vec.x,keyvecset.huoquaxis_vec.y,keyvecset.huoquaxis_vec.z);
	}
	fprintf(fp,"<附属构件 num=\"%d\"/>\n",AttachPart.GetNodeNum());
	bPush=AttachPart.push_stack();
	for(ATTACHPART_ITEM *pAttachPart=AttachPart.GetFirst();pAttachPart;pAttachPart=AttachPart.GetNext())
		fprintf(fp,"<附件信息 h=\"%d\" idClsType=\"%d\" sPartNo=\"%s\"/>\n",pAttachPart->h,pAttachPart->idClsType,pAttachPart->sPartNo);
	if(bPush)
		AttachPart.pop_stack();
	if(m_bUserSpecColor)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<是否用户指定颜色 bUserSpecColor=\"%s\"/>\n",sText);
	if(m_bUserSpecColor)
		fprintf(fp,"<构件颜色 crMaterial=\"%d\"/>\n",crMaterial);
	//连接板螺栓区
	pLsRefList->ToXmlFile(fp,schema);
	fprintf(fp,"<备注 sNotes=\"%s\"/>\n",sNotes);
	RELATIVE_OBJECT *pRelaObj=NULL;
	fprintf(fp,"<关联构件 ObjsNum=\"%d\"/>\n",relativeObjs.GetNodeNum());
	for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		fprintf(fp,"<关联构件信息 hObj=\"0x%x\">\n",pRelaObj->hObj);
		fprintf(fp,"<对称方式 axis_flag=\"%x\"/>\n",pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		if(bSpecOrigin)
			strcpy(sText,"true");
		else 
			strcpy(sText,"false");
		fprintf(fp,"<是否有对称基点 bSpecOrigin=\"%s\"/>\n",sText);
		if(bSpecOrigin)
			fprintf(fp,"<对称基点坐标 origin=\"(%.3f,%.3f,%.3f)\"/>\n",pRelaObj->mirInfo.origin.x,pRelaObj->mirInfo.origin.y,pRelaObj->mirInfo.origin.z);
		if(pRelaObj->mirInfo.axis_flag&0x08)
		{
			fprintf(fp,"<对称旋转信息>\n");
			fprintf(fp,"<旋转对称次数 array_num=\"%d\"/>\n",pRelaObj->mirInfo.array_num);
			fprintf(fp,"<旋转角度 rotate_angle=\"%d\"/>\n",pRelaObj->mirInfo.rotate_angle);
			fprintf(fp,"</对称旋转信息>\n");
		}
		fprintf(fp,"</关联构件信息>\n");
	}
	fprintf(fp,"</参数化板信息>\n");
}
void CLDSParamPlate::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//已调用过)
		return;
	_console->AddToObjSetBuf(this);
	if(!bAddRelaObj)
		return;
	bAddRelaObj=bAddSubLevelRelaObj;
	//TODO:参数化板中的关联构件未考虑　wjh-2017.12.11
	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		pLsRef->GetLsPtr()->AddToConsoleObjBuf(bAddRelaObj);
}

BOOL CLDSParamPlate::IsHasDesignItemValue(WORD wKey)
{
	return params.GetValue(wKey)!=NULL;
}
BOOL CLDSParamPlate::DeleteDesignItem(WORD wKey)
{
	return params.DeleteNode(wKey);
}

BOOL CLDSParamPlate::DeleteAttachPartItem(long handle)
{
	for(ATTACHPART_ITEM *pItem=AttachPart.GetFirst();pItem;pItem=AttachPart.GetNext())
	{
		if(pItem->h==handle)
		{
			AttachPart.DeleteCursor(TRUE);
			return TRUE;
		}
	}
	return FALSE;
}

DESIGN_PLATE_ITEM *CLDSGeneralPlate::GetDesignItem(WORD wKey)
{
	CLDSGeneralPlate* pMotherPlate=(CLDSGeneralPlate*)MotherPart();
	if (pMotherPlate!=this)
	{
		DESIGN_PLATE_ITEM *pMotherItem = pMotherPlate->GetDesignItem(wKey);
		if(pMotherItem&&pMotherItem->paramType==1)	//外形参数
			return pMotherItem;
	}
	return params.GetValue(wKey);
}

BOOL CLDSGeneralPlate::GetDesignItemValue(WORD wKey, DESIGN_PARAM *pParamItem,CHashTable<DESIGN_PLATE_ITEM*>*pTbl/*=NULL*/)
{
	CLDSGeneralPlate* pMotherPlate=(CLDSGeneralPlate*)MotherPart();
	if (pMotherPlate!=this)
	{
		DESIGN_PLATE_ITEM *pMotherItem = pMotherPlate->GetDesignItem(wKey);
		if(pMotherItem&&pMotherItem->paramType==1)	//外形参数
			return pMotherPlate->GetDesignItemValue(wKey,pParamItem,pTbl);
	}
	DESIGN_PLATE_ITEM *pItem=NULL;
	if(pTbl)
		pTbl->GetValueAt(wKey,pItem);
	else
		pItem=params.GetValue(wKey);
	if(pItem)
	{
		pParamItem->bExist=true;
		pParamItem->cValueType=pItem->dataType;
		pParamItem->value.fValue=pItem->value.fValue;
		pParamItem->cParamType=pItem->paramType;
		pParamItem->wItemKey=pItem->key;
	}
	else
		pParamItem->bExist=false;
	return pItem!=NULL;
}
BOOL CLDSGeneralPlate::GetDesignItemValue(WORD wKey, long *phPart,CHashTable<DESIGN_PLATE_ITEM*>*pTbl/*=NULL*/)
{
	CLDSParamPlate* pMotherPlate=(CLDSParamPlate*)MotherPart();
	if (pMotherPlate!=this)
	{
		DESIGN_PLATE_ITEM *pMotherItem = pMotherPlate->GetDesignItem(wKey);
		if(pMotherItem&&pMotherItem->paramType==1)	//外形参数
			return pMotherPlate->GetDesignItemValue(wKey,phPart,pTbl);
	}
	DESIGN_PLATE_ITEM *pItem=NULL;
	if(pTbl)
		pTbl->GetValueAt(wKey,pItem);
	else
		pItem=params.GetValue(wKey);
	if(pItem)
	{
		if(pItem->dataType==0)
		{
			if(phPart)
				*phPart = ftoi(pItem->value.fValue);
			return TRUE;
		}
		else if(pItem->dataType==1||pItem->dataType==3)
		{
			if(phPart)
				*phPart = pItem->value.hPart;
			return TRUE;
		}
		else if(pItem->dataType==2)
		{
			if(phPart)
				*phPart = pItem->value.iType;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CLDSGeneralPlate::GetDesignItemValue(WORD wKey, double *pfValue,CHashTable<DESIGN_PLATE_ITEM*>*pTbl/*=NULL*/)
{
	//D与标准法兰相关参数同名,用X表示旋转角度  暂时如此处理 wht 10-01-21
	//O表示平焊法兰地面与钢管端头之间的距离 H1
	CLDSParamPlate* pMotherPlate=(CLDSParamPlate*)MotherPart();
	if (pMotherPlate!=this)
	{
		DESIGN_PLATE_ITEM *pMotherItem = pMotherPlate->GetDesignItem(wKey);
		if(pMotherItem&&pMotherItem->paramType==1)	//外形参数
			return pMotherPlate->GetDesignItemValue(wKey,pfValue,pTbl);
	}
	if(wiType>0&&GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate* pParamPlate=(CLDSParamPlate*)this;
		if(pParamPlate->m_bStdPart&&wKey!='X'&&!(wKey=='O'&&wiType==TYPE_FLP))	//m_iParamType
			return pParamPlate->GetStdPartParamValue(wKey,pfValue);
	}
	DESIGN_PLATE_ITEM *pItem=NULL;
	if(pTbl)
		pTbl->GetValueAt(wKey,pItem);
	else
		pItem=params.GetValue(wKey);
	if(pItem)
	{
		if(pItem->dataType==0)
		{
			if(pfValue)
				*pfValue = pItem->value.fValue;
			return TRUE;
		}
		else if(pItem->dataType>=1)
		{
			if(pfValue)
				*pfValue = pItem->value.hPart;
			return TRUE;
		}
	}
	return FALSE;
}

char* CLDSParamPlate::GetDesignItemNotes(WORD wKey)
{
	CLDSParamPlate* pMotherPlate=(CLDSParamPlate*)MotherPart();
	if (pMotherPlate!=this)
	{
		DESIGN_PLATE_ITEM *pMotherItem = pMotherPlate->GetDesignItem(wKey);
		if(pMotherItem&&pMotherItem->paramType==1)	//外形参数
			return pMotherItem->sNotes;
	}
	DESIGN_PLATE_ITEM *pItem=params.GetValue(wKey);
	return (pItem!=NULL) ? (char*)pItem->sNotes : NULL;
}

/*
BOOL CLDSParamPlate::SetDesignItemValue(char cKey,double fValue,char *sNotes)
{
	for(DESIGN_PLATE_ITEM *pItem=designItemList.GetFirst();pItem;pItem=designItemList.GetNext())
	{
		if(pItem->cItem==cKey)
		{
			pItem->value.fValue=fValue;
			pItem->dataType = 0;
			break;
		}
	}
	if(pItem==NULL)
	{
		pItem=designItemList.append();
		pItem->cItem=cKey;
		pItem->value.fValue=fValue;
		pItem->dataType=0;
		strcpy(pItem->sNotes,"");
		sprintf(pItem->sNotes,"%s",sNotes);
	}
	return TRUE;
}
*/

BOOL CLDSParamPlate::SetDesignItem(WORD wKey,DESIGN_PARAM& param,char *sNotes/*=NULL*/,
										int iParamType/*=PARAM_POS*/,BOOL bModify/*=FALSE*/)
{
	DESIGN_PLATE_ITEM *pItem=params.Add(wKey);
	if(pItem==NULL)
		return FALSE;
	pItem->dataType=param.cValueType;
	pItem->value.fValue=param.value.fValue;
	pItem->sNotes=(sNotes!=NULL)?sNotes:"";
	pItem->paramType=iParamType;
	pItem->bModify=bModify;
	return TRUE;
}
BOOL CLDSParamPlate::SetDesignItemValue(WORD wKey,double fValue,char *sNotes/*=NULL*/,
										int iParamType/*=PARAM_POS*/,int iDataType/*=0*/,BOOL bModify/*=FALSE*/)
{
	CLDSParamPlate* pMotherPlate=(CLDSParamPlate*)MotherPart();
	if (pMotherPlate!=this&&iParamType==1)	//外形参数
		pMotherPlate->SetDesignItemValue(wKey,fValue,sNotes,iParamType,iDataType,bModify);
	DESIGN_PLATE_ITEM *pItem;
	for(pItem=params.GetFirst();pItem;pItem=params.GetNext())
	{
		if(pItem->key==wKey&&iDataType==0)
		{
			pItem->paramType=iParamType;
			pItem->bModify=bModify;
			pItem->value.fValue = fValue;
			pItem->dataType = 0;
			pItem->sNotes.Copy(sNotes);
			break;
		}
		else if(pItem->key==wKey&&iDataType==1)
		{
			pItem->paramType=iParamType;
			pItem->bModify=bModify;
			pItem->value.hPart = ftoi(fValue);
			pItem->dataType = 1;
			pItem->sNotes.Copy(sNotes);
			break;
		}
		else if(pItem->key==wKey&&iDataType==2)
		{
			pItem->paramType=iParamType;
			pItem->bModify=bModify;
			pItem->value.iType = ftoi(fValue);
			pItem->dataType = 2;
			pItem->sNotes.Copy(sNotes);
			break;
		}
	}
	if(pItem==NULL)
	{
		pItem = params.Add(wKey);
		if(iDataType==0)
		{
			pItem->value.fValue = fValue;
			pItem->dataType = 0;
		}
		else if(iDataType==1)
		{
			pItem->value.hPart = (long)fValue;
			pItem->dataType = 1;
		}
		else if(iDataType==2)
		{
			pItem->value.iType = (long)fValue;
			pItem->dataType = 2;
		}
		pItem->sNotes.Copy(sNotes);
		pItem->bModify=bModify;
		pItem->paramType=iParamType;
	}
	return TRUE;
}

/*BOOL CLDSParamPlate::DesignBolts()
{
	pLsRefList->Empty();
	for(CDesignPlateBoltPara *pDesignBolt=designBoltList.GetFirst();pDesignBolt;pDesignBolt=designBoltList.GetNext())
	{
		if(pDesignBolt->scope_angle<EPS)
		{	//逐个定制
			CLDSBolt *pBolt=(CLDSBolt*)BelongModel()->FromPartHandle(pDesignBolt->linkBoltArr(0),CLS_BOLT);
			if(pBolt==NULL)
			{
				pBolt=(CLDSBolt*)SuperiorTower()->Parts.append(CLS_BOLT);
				pDesignBolt->linkBoltArr(0)=pBolt->handle;
			}
			pBolt->set_d(pDesignBolt->d);
			pBolt->L0=pDesignBolt->L0;
			f3dPoint pos(pDesignBolt->x,pDesignBolt->y);
			coord_trans(pos,ucs,TRUE);
			pBolt->set_norm(ucs.axis_z);
			pos+=ucs.axis_z*pDesignBolt->z;
			pBolt->set_pos(pos);
			FindLsGuiGe(pBolt,pDesignBolt->d,pBolt->L0);
			if(pLsRefList->FromHandle(pBolt->handle)==NULL)
				pLsRefList->append(pBolt->GetLsRef(TRUE));
		}
		else //if(pDesignBolt->scope_angle>EPS)
		{	//环形陈列
			double L0=pDesignBolt->L0;
			double norm_offset=pDesignBolt->z;
			for(int i=0;i<pDesignBolt->linkBoltArr.Length();i++)
			{
				CLDSBolt *pBolt=(CLDSBolt*)BelongModel()->FromPartHandle(pDesignBolt->linkBoltArr(i),CLS_BOLT);
				if(pBolt==NULL)
				{
					pBolt=(CLDSBolt*)SuperiorTower()->Parts.append(CLS_BOLT);
					pDesignBolt->linkBoltArr(i)=pBolt->handle;
				}
				pBolt->set_d(pDesignBolt->d);
				pBolt->L0=L0;
				f3dPoint pos(pDesignBolt->x,pDesignBolt->y);
				double piece_angle=pDesignBolt->scope_angle/pDesignBolt->array_num*RADTODEG_COEF;
				coord_trans(pos,ucs,TRUE);
				rotate_point_around_axis(pos,piece_angle*i,ucs.origin,ucs.origin+ucs.axis_z);
				pBolt->set_norm(ucs.axis_z);
				pos+=ucs.axis_z*norm_offset;
				pBolt->set_pos(pos);
				FindLsGuiGe(pBolt,pDesignBolt->d,pBolt->L0);
				if(pLsRefList->FromHandle(pBolt->handle)==NULL)
					pLsRefList->append(pBolt->GetLsRef(TRUE));
			}
		}
	}
	return TRUE;
}*/

CLDSPart *CLDSParamPlate::GetMirPart(MIRMSG mirmsg,const double *pPickPos/*=NULL*/,BOOL bCheckProfile/*=TRUE*/)
{
	for(RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
	{	//先在关联构件列表中查找
		CLDSParamPlate *pParamPlate=NULL;
		if(pItem->mirInfo.IsSameMirMsg(mirmsg))
		{
			pParamPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(pItem->hObj,CLS_PARAMPLATE);
			if( pParamPlate&&pParamPlate->m_iParamType==m_iParamType&&
				ModuleTogetherWith(pParamPlate)&&IsEqualMirWith(pParamPlate,mirmsg))
				return pParamPlate;
			else
				break;
		}
	}
	f3dPoint mir_origin=GetMirPos(ucs.origin,mirmsg);
	f3dPoint axis_z=GetMirVector(ucs.axis_z,mirmsg);
	f3dPoint axis_y=GetMirVector(ucs.axis_y,mirmsg);
	//非标准构件有对称关系时，设计参数必须一致
	double dx=-1,dy=1;
	CLDSPart *pFatherPart=NULL,*pMirFatherPart=NULL;
	long Y=0,L=0,l=1,y=1,b=0,a=0,A=1,B=-1,E=-1,F=-1,G=-1;
	double K=0,k=0;
	f3dPoint geom_center;
	if(!m_bStdPart)
	{
		if(m_iParamType==TYPE_ELBOWPLATE)
		{
			GetDesignItemValue('L',&L);
			MirPartHandle(BelongModel(),&b,L,mirmsg);
			GetDesignItemValue('Y',&Y);
		}
		else if(m_iParamType==TYPE_RIBPLATE)
		{
			GetDesignItemValue('B',&B);
			MirPartHandle(BelongModel(),&b,B,mirmsg);
			GetDesignItemValue('K',&K);
		}
		/* 之前一直未计算F、G对称钢板句柄，现在使用IsGeomMir()来判断两钢板是否存在几何对称关系 wht 15-12-22
		else if(m_iParamType==TYPE_CIRCOVERPLATE)
		{
			GetDesignItemValue('F',&F);	//第一块基准钢板
			GetDesignItemValue('G',&G);	//第二块基准钢板
			GetDesignItemValue('a',&a);	//偏移角
		}*/
		else if(m_iParamType==TYPE_VERTPLATE||m_iParamType==TYPE_ENDPLATE)
		{	//垂向加劲板(封头板)原点坐标与父构件均相同且Z轴方向相对与父构件的Z轴方向一致则认为为同一构件
			pFatherPart=BelongModel()->FromPartHandle(m_hPartWeldParent);
			pMirFatherPart=pFatherPart->GetMirPart(mirmsg);
			if(pFatherPart)
				dx=pFatherPart->ucs.axis_z*ucs.axis_z; //垂向加劲板(封头板)的Z轴与其父构件的Z轴是否同向
		}
		else if(m_iParamType==TYPE_CIRRIBPLATE)
		{
			GetDesignItemValue('E',&E);	//第一块基准钢板 'B'改为'E' wht 11-05-20
			MirPartHandle(BelongModel(),&E,E,mirmsg);
			GetDesignItemValue('F',&F);	//第二块基准钢板 'C'改为'F' wht 11-05-20
			MirPartHandle(BelongModel(),&F,F,mirmsg);
			GetDesignItemValue('Y',&Y);	//外/内侧
		}
		geom_center=CLDSPlate::GetGeomCenter(this);
	}
	CSuperSmartPtr<CLDSPart>pMirPart;
	BOOL pushed=m_pModel->PushPartStack(); //遍历构件列表前应先压栈 wht 12-03-05
	for(pMirPart=m_pModel->EnumPartFirst(CLS_PARAMPLATE);pMirPart.IsHasPtr();pMirPart=m_pModel->EnumPartNext(CLS_PARAMPLATE))
	{
		CLDSParamPlate *pMirPlate=pMirPart.ParamPlatePointer();
		if(m_iParamType!=pMirPlate->m_iParamType)
			continue;	//参数化板子类型不同
		if(pMirPlate==this)
			continue;	//不支持自对称
		if(m_bStdPart!=pMirPlate->m_bStdPart)
			continue;	//有对称关系的构件必须同是标准构件或非标准构件
		else if(!ModuleTogetherWith(pMirPlate))
			continue;
		if(m_bStdPart&&stricmp(sPartNo,pMirPart->GetPartNo())!=0)
			continue;	//有外形对称关系的标准构件肯定具有同样的件号
		//对法兰来讲是成对的，如果不区分法线正负将无法判断出具体对称哪一个法兰 wjh-2014.8.23
		double ddz=pMirPart->ucs.axis_z*axis_z;
		if(pMirPart->ucs.origin!=mir_origin||fabs(ddz)<EPS_COS||fabs(pMirPart->ucs.axis_y*axis_y)<EPS_COS)
			continue;	//有几何位置对称关系的构件肯定具有同样的件号
		if(IsFL()&&ddz<0)
			continue;
		if(m_bStdPart)	//位置对称编号相同的标准构件为对称构件
			break;		
		else //if(!m_bStdPart)
		{	//非标准构件
			if(pMirPlate->thick!=thick)
				continue;	//板厚不一致
			if(pMirPlate->m_iParamType==TYPE_ELBOWPLATE)
			{	//基准构件相同且偏移角度相同则为同一个肘板
				pMirPlate->GetDesignItemValue('Y',&y);
				if(y!=Y)
					continue;
				pMirPlate->GetDesignItemValue('L',&l);
				if(b!=l)
					continue;
			}
			else if(m_iParamType==TYPE_RIBPLATE)
			{	//比较肋板基准构件及偏移角度
				pMirPlate->GetDesignItemValue('B',&B);
				if(B!=b)
					continue;
				pMirPlate->GetDesignItemValue('K',&k);
				if(fabs(k-K)>0)
					continue;
			}
			/*else if(pMirPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			{
				long f=-1,g=-1;
				pMirPlate->GetDesignItemValue('F',&f);	//第一块基准钢板
				pMirPlate->GetDesignItemValue('G',&g);	//第二块基准钢板
				pMirPlate->GetDesignItemValue('A',&A);
				if(a!=A||f!=F||g!=G)
					continue;
			}*/
			else if(m_iParamType==TYPE_VERTPLATE||m_iParamType==TYPE_ENDPLATE)
			{
				CLDSPart *pPart=BelongModel()->FromPartHandle(pMirPlate->m_hPartWeldParent);
				if(pPart)
					dy=pPart->ucs.axis_z*pMirPart->ucs.axis_z; //垂向加劲板(封头板)的Z轴与其父构件的Z轴是否同向		
				if(pMirFatherPart->handle!=pMirPlate->m_hPartWeldParent||(dx*dy<=0))
					continue;
			}
			//else if(m_iParamType==TYPE_TUBERIBPLATE)
			//	;
			else if(m_iParamType==TYPE_CIRRIBPLATE)
			{
				long y=-1,e=-1,f=-1;
				pMirPlate->GetDesignItemValue('Y',&y);
				pMirPlate->GetDesignItemValue('E',&e);	//'B'改为'E' wht 11-05-20
				pMirPlate->GetDesignItemValue('F',&f);	//'C'改为'F' wht 11-05-20
				if(F==0)//仅有一块基准钢板的情况
				{
					if(0!=f||y!=Y)	//TODO：这里的‘F’参数代表什么，是否逻辑有问题wjh-2018.3.12
						continue;
				}
				else	//有两块基准钢板的情况
				{
					if(E!=e&&F!=f)
						continue;
				}
			}

			if(vertex_list.GetNodeNum()!=pMirPart.ParamPlatePointer()->vertex_list.GetNodeNum())
				continue;	//有外形对称关系的构件顶点数肯定相同
			ATOM_LIST<PROFILE_VERTEX> *pMirVertexList=&pMirPart.ParamPlatePointer()->vertex_list;
			bool diffrent=false;
			for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				PROFILE_VERTEX* pNextVertex=NULL,*pMirNextVertex=NULL;
				if(pVertex->type>0)
				{
					BOOL pushed=vertex_list.push_stack();
					pNextVertex=vertex_list.GetNext();
					if(pNextVertex==NULL)
						pNextVertex=vertex_list.GetFirst();
					vertex_list.pop_stack(pushed);
				}
				PROFILE_VERTEX *pMirVertex=NULL;
				for(pMirVertex=pMirVertexList->GetFirst();pMirVertex;pMirVertex=pMirVertexList->GetNext())
				{
					pMirNextVertex=NULL;
					if(pVertex->type>0&&pMirVertex->type>0)
					{
						pushed=pMirVertexList->push_stack();
						pMirNextVertex=pMirVertexList->GetNext();
						if(pMirNextVertex==NULL)
							pMirNextVertex=pMirVertexList->GetFirst();
						pMirVertexList->pop_stack(pushed);
					}
					if( pVertex->IsEqualWith(pMirVertex,pNextVertex,pMirNextVertex)&&
						pVertex->manu_space==pMirVertex->manu_space)
						break;
				}
				if(pMirVertex==NULL)
				{	//在对称构件顶点列表中未找到对称顶点，外形不同 wht 12-03-19
					diffrent=true;
					break;
				}
			}
			if(diffrent)	//与理想对称参数化板有差异，不完全一致
				continue;
			else 
			{
				if(!IsGeomMir(pMirPlate,mirmsg,FALSE))
					continue;
				break;		//找到对称的参数化板跳出循环
			}
		}
	}
	m_pModel->PopPartStack(pushed);
	if(pMirPart.IsHasPtr())
		return pMirPart.ParamPlatePointer();
	return NULL;
}
bool CLDSParamPlate::IsBelongTo(int iClassTypeId)const
{
	if(iClassTypeId==this->m_nClassTypeId||iClassTypeId==CLS_PART||iClassTypeId==CLS_DBOBJECT)//||iClassTypeId==CLS_GENERALPLATE)
		return true;
	else
		return false;
}
BOOL CLDSParamPlate::ValidateProfile(double epsilon/*=1.0*/,EDGEBOLT *endBoltInfoArr/*=NULL*/,
							   int *count/*=NULL*/,BYTE cbValidateFlag/*=0xff*/)
{
	return CLDSGeneralPlate::ValidateProfile(epsilon,endBoltInfoArr,count,cbValidateFlag);
}
//0.钢板外形相同 1.外形不同 2.螺栓信息不同 3.线属性卷边方向不同或者卷边高度不同
int CLDSParamPlate::IsEqualProfile(CLDSGeneralPlate* pMirPlate,BOOL bCompareLs/*=TRUE*/,double epsilon/*=1.0*/)
{
	if(GetClassTypeId()!=pMirPlate->GetClassTypeId())
		return 1;	//暂时要求两钢板类型必须一致 wht 15-12-22
	struct ve_item{
		f3dPoint vertex;
		double len;
		double cosine_angle;
	};
	int i,n,vice_n;
	f3dLine std_line,vice_std_line;
	f3dPoint vec_cur,vec_next;
	UCS_STRU tm_ucs,tm_vice_ucs;
	n = vertex_list.GetNodeNum();
	vice_n = pMirPlate->vertex_list.GetNodeNum();
	if(n!=vice_n||n<3||vice_n<3)
		return 1;	//外形不合理或不同
	struct ve_item* item_arr = new struct ve_item[n];
	struct ve_item* vice_item_arr = new struct ve_item[vice_n];
	for(i=0;i<n;i++)
	{
		item_arr[i].vertex = vertex_list[i].vertex;
		vec_cur = vertex_list[(i+1)%n].vertex-item_arr[i].vertex;
		vec_next= vertex_list[(i+2)%n].vertex-vertex_list[(i+1)%n].vertex;
		item_arr[i].len = vec_cur.mod();
		item_arr[i].cosine_angle = vec_cur.normalized()*vec_next.normalized();
	}
	for(i=0;i<vice_n;i++)
	{
		vice_item_arr[i].vertex = pMirPlate->vertex_list[i].vertex;
		vec_cur = pMirPlate->vertex_list[(i+1)%n].vertex-vice_item_arr[i].vertex;
		vec_next= pMirPlate->vertex_list[(i+2)%n].vertex-pMirPlate->vertex_list[(i+1)%n].vertex;
		vice_item_arr[i].len = vec_cur.mod();
		vice_item_arr[i].cosine_angle = vec_cur.normalized()*vec_next.normalized();
	}
	int base_i=0;
	for(i=0;i<n;i++)
	{
		if( fabs(item_arr[i].len-vice_item_arr[(base_i+i)%n].len)>epsilon
		 || fabs(item_arr[i].cosine_angle-vice_item_arr[(base_i+i)%n].cosine_angle)>epsilon*1e-2)
		{
			base_i++;
			if(base_i>=n)
				goto REVERSE_ORDER;
			i=-1;
			continue;
		}
	}
	std_line.startPt = vertex_list[0].vertex;
	std_line.endPt   = vertex_list[1].vertex;
	vice_std_line.startPt = vice_item_arr[base_i].vertex;
	vice_std_line.endPt   = vice_item_arr[(base_i+1)%n].vertex;
	if(bCompareLs)	//对照螺栓孔位置及大小
	{
		if(GetLsCount()!=pMirPlate->GetLsCount())
		{
			delete []item_arr;
			delete []vice_item_arr;
			return 2;
		}
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			BOOL bFindEqualLs = FALSE;
			f3dPoint pos;
			SpreadLsPos(pLsRef->GetLsPtr(),pos);
			pos.z = 0;
			double dist_start = DISTANCE(pos,std_line.startPt);
			double dist_end   = DISTANCE(pos,std_line.endPt);
			for(CLsRef *pViceLsRef=pMirPlate->GetFirstLsRef();pViceLsRef;pViceLsRef=pMirPlate->GetNextLsRef())
			{
				pMirPlate->SpreadLsPos(pViceLsRef->GetLsPtr(),pos);
				pos.z = 0;
				double vice_dist_start = DISTANCE(pos,vice_std_line.startPt);
				double vice_dist_end   = DISTANCE(pos,vice_std_line.endPt);
				if( (*pLsRef)->get_d()+(*pLsRef)->hole_d_increment!=
					(*pViceLsRef)->get_d()+(*pViceLsRef)->hole_d_increment)
					continue;
				else if(fabs(dist_start-vice_dist_start)>epsilon||fabs(dist_end-vice_dist_end)>epsilon)
					continue;
				else
				{
					bFindEqualLs = TRUE;
					break;
				}
			}
			if(!bFindEqualLs)	//螺栓孔不相同
			{
				delete []item_arr;
				delete []vice_item_arr;
				return 2;	
			}
		}
	}
	delete []item_arr;
	delete []vice_item_arr;
	return 0;
REVERSE_ORDER:
	for(i=0;i<vice_n;i++)
	{
		vice_item_arr[i].vertex = pMirPlate->vertex_list[vice_n-1-i].vertex;
		vec_cur = pMirPlate->vertex_list[(n+vice_n-i-2)%n].vertex-vice_item_arr[i].vertex;
		vec_next= pMirPlate->vertex_list[(n+vice_n-i-3)%n].vertex-pMirPlate->vertex_list[(n+vice_n-i-2)%n].vertex;
		vice_item_arr[i].len = vec_cur.mod();
		vice_item_arr[i].cosine_angle = vec_cur.normalized()*vec_next.normalized();
	}
	base_i=0;
	for(i=0;i<n;i++)
	{
		if( fabs(item_arr[i].len-vice_item_arr[(base_i+i)%n].len)>epsilon
		 || fabs(item_arr[i].cosine_angle-vice_item_arr[(base_i+i)%n].cosine_angle)>epsilon*1e-2)
		{
			base_i++;
			if(base_i>=n)
			{	//外形不相同
				delete []item_arr;
				delete []vice_item_arr;
				return 1;	//外形不合理或不同
			}
			i=-1;
			continue;
		}
	}
	std_line.startPt = vertex_list[0].vertex;
	std_line.endPt   = vertex_list[1].vertex;
	vice_std_line.startPt = vice_item_arr[base_i].vertex;
	vice_std_line.endPt   = vice_item_arr[(base_i+1)%n].vertex;
	if(bCompareLs)	//对照螺栓孔位置及大小
	{
		if(GetLsCount()!=pMirPlate->GetLsCount())
		{
			delete []item_arr;
			delete []vice_item_arr;
			return 2;
		}
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			BOOL bFindEqualLs = FALSE;
			f3dPoint pos;
			SpreadLsPos(pLsRef->GetLsPtr(),pos);
			pos.z = 0;
			double dist_start = DISTANCE(pos,std_line.startPt);
			double dist_end   = DISTANCE(pos,std_line.endPt);
			for(CLsRef *pViceLsRef=pMirPlate->GetFirstLsRef();pViceLsRef;pViceLsRef=pMirPlate->GetNextLsRef())
			{
				pMirPlate->SpreadLsPos(pViceLsRef->GetLsPtr(),pos);
				pos.z = 0;
				double vice_dist_start = DISTANCE(pos,vice_std_line.startPt);
				double vice_dist_end   = DISTANCE(pos,vice_std_line.endPt);
				if( (*pLsRef)->get_d()+(*pLsRef)->hole_d_increment!=
					(*pViceLsRef)->get_d()+(*pViceLsRef)->hole_d_increment)
					continue;
				else if(fabs(dist_start-vice_dist_start)>epsilon||fabs(dist_end-vice_dist_end)>epsilon)
					continue;
				else
				{
					bFindEqualLs = TRUE;
					break;
				}
			}
			if(!bFindEqualLs)	//螺栓孔不相同
			{
				delete []item_arr;
				delete []vice_item_arr;
				return 2;	
			}
		}
	}
	delete []item_arr;
	delete []vice_item_arr;
	return 0;
}
bool CLDSParamPlate::IsEqual(CLDSParamPlate* pMirPlate,double epsilon/*=1.0*/, char *error_info/*=NULL*/,
						   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)//error_info>50 characters
{
	//构件编号相同的标准构件一定相同，因为标准构件的参数是靠编号来唯一确定的
	if(m_bStdPart&&pMirPlate->m_bStdPart)
		return true;
	CLsRef *pLsRef=NULL;
	int i,nLsCount=0,nMirLsCount=0;
	if(pMirPlate->m_iParamType==TYPE_XEND)
	{	//统计十字插板上的有效螺栓个数	wht 10-01-15
		for(i=0;i<2;i++)
		{
			CLDSParamPlate *pCurPlate=NULL;
			if(i==0) 
				pCurPlate=this;
			else	
				pCurPlate=pMirPlate;
			if(pCurPlate==NULL)
				continue;
			for(pLsRef=pCurPlate->GetFirstLsRef();pLsRef;pLsRef=pCurPlate->GetNextLsRef())
			{
				if(pLsRef->start1_end2_mid0>0)
				{	
					if(pLsRef->start1_end2_mid0!=11&&pLsRef->start1_end2_mid0!=21)
						continue;	//跳过不在插板范围内的螺栓
				}
				else 
				{	//用start1_end2_mid0表示螺栓位置之前,添加的十字插板
					double L=0;
					pCurPlate->GetDesignItemValue('L',&L);
					if(fabs((*pLsRef)->get_norm()*pCurPlate->ucs.axis_z)<EPS_COS2)
						continue;
					if((*pLsRef)->des_base_pos.len_offset_dist>L)
						continue; //不统计十字插板范围外的螺栓孔
				}
				if(i==0)
					nLsCount++;
				else
					nMirLsCount++;
			}
		}
	}
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHK_PLATE_BASICINFO)
	{
		bool bRet=true;
		if(m_iParamType!=pMirPlate->m_iParamType)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} type is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}类型不同",(char*)sPartNo);
#endif
			bRet=false;
		}		
		else if(cMaterial!=pMirPlate->cMaterial)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} material is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}材质不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(thick!=pMirPlate->thick)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} plate's thickness is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}板厚不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		if(!bRet)
		{
			if(pErrorInfo)
				*pErrorInfo=CHK_PLATE_BASICINFO;
			return false;
		}
	}
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHK_PLATE_PROFILEANDBOLTINFO)
	{
		bool bRet=true;
		//1、比较螺栓个数,位置及规格
		if((m_iParamType==TYPE_XEND&&nLsCount!=nMirLsCount)
			||(m_iParamType!=TYPE_XEND&&GetLsCount()!=pMirPlate->GetLsCount()))
		{	//比较螺栓个数
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s} bolt's quantity is different",(char*)sPartNo);
#else
				sprintf(error_info,"{%s}螺栓数不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		/*for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{	//比较螺栓位置、规格
			long ls_d=pLsRef->GetLsPtr()->get_d();
			f3dPoint ls_pos=pLsRef->GetLsPtr()->ucs.origin;
			project_point(ls_pos,ucs.origin,ucs.axis_z);
			coord_trans(ls_pos,ucs,FALSE);
			CLsRef *pMirLsRef=NULL;
			if(m_iParamType==TYPE_UEND||m_iParamType==TYPE_ROLLEND||m_iParamType==TYPE_CIRCOVERPLATE||m_iParamType==TYPE_FL)
			{	//该类构件建立坐标系时的规则相同，可以根据螺栓的相对位置查找螺栓
				pMirLsRef=pMirPlate->FindLsByUcsPos(ls_pos,pMirPlate->ucs,2);
				if(pMirLsRef==NULL)
				{
					if(error_info)
					{
						sprintf(error_info,"{%s}螺栓位置不同",(char*)sPartNo);
						bRet=false;
						break;
					}
				}
				else if(pMirLsRef)
				{
					if(pMirLsRef->GetLsPtr()->get_d()!=ls_d)
					{
						if(error_info)
						{
							sprintf(error_info,"{%s}螺栓螺栓规格不同",(char*)sPartNo);
							bRet=false;
							break;
						}
					}
				}
			}
			//else 	//其它类型参数化板暂不做螺栓信息比较
			//法兰附带的肘板或者环向肋板上的孔信息可不做比较 
		}
		if(!bRet)
		{
			if(pErrorInfo)
				*pErrorInfo=CHK_PLATE_PROFILEANDBOLTINFO;
			return false;
		}*/
		//2、比较参数及外形信息 
		//首先比较参数是否相同(主要用于提示用户外形不同可能是由于参数不同而导致)，然后比较外形是否相同 wht 10-03-19
		BOOL bHasSameParam=TRUE;
		if(m_iParamType==TYPE_UEND||m_iParamType==TYPE_ROLLEND||m_iParamType==TYPE_XEND)
		{
			double W[2]={0},L[2]={0},H[2]={0},M[2]={0},N[2]={0},S[2]={0},A[2]={0},R[2]={0};
			GetDesignItemValue('W',&W[0]);
			GetDesignItemValue('L',&L[0]);
			GetDesignItemValue('H',&H[0]);
			GetDesignItemValue('M',&M[0]);
			GetDesignItemValue('N',&N[0]);
			GetDesignItemValue('S',&S[0]);
			GetDesignItemValue('A',&A[0]);
			GetDesignItemValue('R',&R[0]);
			pMirPlate->GetDesignItemValue('W',&W[1]);
			pMirPlate->GetDesignItemValue('L',&L[1]);
			pMirPlate->GetDesignItemValue('H',&H[1]);
			pMirPlate->GetDesignItemValue('M',&M[1]);
			pMirPlate->GetDesignItemValue('N',&N[1]);
			pMirPlate->GetDesignItemValue('S',&S[1]);
			pMirPlate->GetDesignItemValue('A',&A[1]);
			pMirPlate->GetDesignItemValue('R',&R[1]);
			if(m_iParamType==TYPE_XEND)
			{
				if(A[0]!=A[1] || R[0]!=R[1])
					bHasSameParam=FALSE; //参数不同
			}
			if(W[0]!=W[1] || L[0]!=L[1] || H[0]!=H[1] || M[0]!=M[1] || N[0]!=N[1] || S[0]!=S[1])
				bHasSameParam=FALSE; //参数不同
		}
		else if(m_iParamType==TYPE_FL)
		{
			double W[2]={0},R[2]={0};//,A[2];
			GetDesignItemValue('R',&R[0]);
			GetDesignItemValue('W',&W[0]);
			//法兰一定是整圆,不用判断圆弧扇形角度 wjh-2012.3.30
			//GetDesignItemValue('A',&A[0]);
			pMirPlate->GetDesignItemValue('R',&R[1]);
			pMirPlate->GetDesignItemValue('W',&W[1]);
			//pMirPlate->GetDesignItemValue('A',&A[1]);
			if(R[0]!=R[1] || W[0]!=W[1])// || A[0]!=A[1])
				bHasSameParam=FALSE; //参数不同
		}
		else if(m_iParamType==TYPE_ELBOWPLATE)
		{
			double A[2]={0},B[2]={0},C[2]={0},D[2]={0},E[2]={0},F[2]={0},G[2]={0},H[2]={0},Y[2]={0};//K[2]
			GetDesignItemValue('A',&A[0]);
			GetDesignItemValue('B',&B[0]);
			GetDesignItemValue('C',&C[0]);
			GetDesignItemValue('D',&D[0]);
			GetDesignItemValue('E',&E[0]);
			GetDesignItemValue('F',&F[0]);
			GetDesignItemValue('G',&G[0]);
			GetDesignItemValue('H',&H[0]);
			pMirPlate->GetDesignItemValue('A',&A[1]);
			pMirPlate->GetDesignItemValue('B',&B[1]);
			pMirPlate->GetDesignItemValue('C',&C[1]);
			pMirPlate->GetDesignItemValue('D',&D[1]);
			pMirPlate->GetDesignItemValue('E',&E[1]);
			pMirPlate->GetDesignItemValue('F',&F[1]);
			pMirPlate->GetDesignItemValue('G',&G[1]);
			pMirPlate->GetDesignItemValue('H',&H[1]);
			if(A[0]!=A[1] || B[0]!=B[1] || C[0]!=C[1] || D[0]!=D[1] || E[0]!=E[1] || F[0]!=F[1] || 
				G[0]!=G[1] || H[0]!=H[1])
				bHasSameParam=FALSE; //参数不同
		}
		else if(m_iParamType==TYPE_CIRRIBPLATE)
		{
			double W[2]={0},N[2]={0},D[2]={0},R[2]={0},H[2]={0};
			GetDesignItemValue('W',&W[0]);
			GetDesignItemValue('N',&N[0]);
			GetDesignItemValue('H',&H[0]);
			GetDesignItemValue('R',&R[0]);
			GetDesignItemValue('D',&D[0]);
			pMirPlate->GetDesignItemValue('W',&W[1]);
			pMirPlate->GetDesignItemValue('N',&N[1]);
			pMirPlate->GetDesignItemValue('H',&H[1]);
			pMirPlate->GetDesignItemValue('R',&R[1]);
			pMirPlate->GetDesignItemValue('D',&D[1]);
			if(fabs(H[0]-H[1])>EPS || fabs(D[0]-D[1])>EPS || fabs(R[0]-R[1])>EPS || !((fabs(W[0]-W[1])<=EPS&&fabs(N[0]-N[1])<=EPS)||(fabs(W[0]-N[1])<=EPS&&fabs(N[0]-W[1])<=EPS)))
				bHasSameParam=FALSE;//参数不同	//W,N值对应相等或者互换相等，均可认为参数相同
		}
		else if(m_iParamType==TYPE_CIRCOVERPLATE)
		{
			double W[2]={0},B[2]={0},D[2]={0},R[2]={0};
			GetDesignItemValue('W',&W[0]);
			GetDesignItemValue('B',&B[0]);
			GetDesignItemValue('D',&D[0]);
			GetDesignItemValue('R',&R[0]);
			pMirPlate->GetDesignItemValue('W',&W[1]);
			pMirPlate->GetDesignItemValue('B',&B[1]);
			pMirPlate->GetDesignItemValue('D',&D[1]);
			pMirPlate->GetDesignItemValue('R',&R[1]);
			if(W[0]!=W[1] || B[0]!=B[1] || D[0]!=D[1] || R[0]!=R[1])
				bHasSameParam=FALSE; //参数不同
		}
		else if(m_iParamType==TYPE_TUBERIBPLATE)
		{
			long hTube=0,hMirTube=0;
			double A[2]={0},W[2]={0},H[2]={0},R[2]={0},N[2]={0},D[2]={0},V[2]={0};
			GetDesignItemValue('A',&A[0]);
			GetDesignItemValue('W',&W[0]);
			GetDesignItemValue('H',&H[0]);
			GetDesignItemValue('R',&R[0]);
			GetDesignItemValue('N',&N[0]);
			GetDesignItemValue('D',&D[0]);
			GetDesignItemValue('V',&V[0]);
			GetDesignItemValue('C',&hTube);
			pMirPlate->GetDesignItemValue('A',&A[1]);
			pMirPlate->GetDesignItemValue('W',&W[1]);
			pMirPlate->GetDesignItemValue('H',&H[1]);
			pMirPlate->GetDesignItemValue('R',&R[1]);
			pMirPlate->GetDesignItemValue('N',&N[1]);
			pMirPlate->GetDesignItemValue('D',&D[1]);
			pMirPlate->GetDesignItemValue('V',&V[1]);
			pMirPlate->GetDesignItemValue('C',&hMirTube);
			//
			CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(hTube,CLS_LINETUBE);
			CLDSLineTube *pMirTube=(CLDSLineTube*)BelongModel()->FromPartHandle(hMirTube,CLS_LINETUBE);
			if(pDatumTube&&pMirTube)
			{
				if(R[0]>0)	//异型法兰或其余跨越环向板中指定了外圆弧半径绝对值
					D[0]=R[0]-pDatumTube->GetDiameter()*0.5-V[0];
				else if(D[0]>0)
					R[0]=pDatumTube->GetDiameter()*0.5+D[0]+V[0];
				if(R[1]>0)	//异型法兰或其余跨越环向板中指定了外圆弧半径绝对值
					D[1]=R[1]-pMirTube->GetDiameter()*0.5-V[1];
				else if(D[1]>0)
					R[1]=pMirTube->GetDiameter()*0.5+D[1]+V[1];
				if(A[0]!=A[1] || H[0]!=H[1] || R[0]!=R[1] 
					|| !((W[0]==W[1]&&N[0]==N[1])||(W[0]==N[1]&&N[0]==W[1])))
					bHasSameParam=FALSE; //参数不同
			}
		}
		else if(m_iParamType==TYPE_LINEANGLERIBPLATE)
		{
			double W[2]={0},L[2]={0},S[2]={0};
			GetDesignItemValue('W',&W[0]);
			GetDesignItemValue('L',&L[0]);
			GetDesignItemValue('S',&S[0]);
			pMirPlate->GetDesignItemValue('W',&W[1]);
			pMirPlate->GetDesignItemValue('L',&L[1]);
			pMirPlate->GetDesignItemValue('S',&S[1]);
			if(W[0]!=W[1] || L[0]!=L[1] || S[0]!=S[1])
				bHasSameParam=FALSE; //参数不同
		}
		else if(m_iParamType==TYPE_WATERSHIELDEDPLATE)
		{	//遮水板
			double D[2]={0},W[2]={0},S[2]={0},K[2]={0},L[2]={0},H[2]={0},A[2]={0};
			GetDesignItemValue('D',&D[0]);
			GetDesignItemValue('W',&W[0]);
			GetDesignItemValue('S',&S[0]);
			GetDesignItemValue('K',&K[0]);
			GetDesignItemValue('L',&L[0]);
			GetDesignItemValue('H',&H[0]);
			GetDesignItemValue('A',&A[0]);
			pMirPlate->GetDesignItemValue('D',&D[1]);
			pMirPlate->GetDesignItemValue('W',&W[1]);
			pMirPlate->GetDesignItemValue('S',&S[1]);
			pMirPlate->GetDesignItemValue('K',&K[1]);
			pMirPlate->GetDesignItemValue('L',&L[1]);
			pMirPlate->GetDesignItemValue('H',&H[1]);
			pMirPlate->GetDesignItemValue('A',&A[1]);
			if(D[0]!=D[1] || W[0]!=W[1] || S[0]!=S[1] || K[0]!=K[1] || L[0]!=L[1] || H[0]!=H[1] || A[0]!=A[1])
				bHasSameParam=FALSE; //参数不同
		}
		if(bHasSameParam)
		{
			if(vertex_list.GetNodeNum()==0)
				DesignPlate();
			if(pMirPlate->vertex_list.GetNodeNum()==0)
				pMirPlate->DesignPlate();
			if(IsEqualProfile(pMirPlate,TRUE,epsilon)==0)
				return true;
			else
			{
				char sError[100]="";
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(error_info,"{%s} plate shape is different %s",(char*)sPartNo,sError);
#else
					sprintf(error_info,"{%s}板外形不同%s",(char*)sPartNo,sError);
#endif
				bRet=false;
			}
		}
		else
		{
			char sError[100]="";
			if(!bHasSameParam)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"<Parameter is different>");
			if(error_info)
				sprintf(error_info,"{%s} plate shape is different%s",(char*)sPartNo,sError);
#else
				sprintf(sError,"<参数不同>");
			if(error_info)
				sprintf(error_info,"{%s}板外形不同%s",(char*)sPartNo,sError);
#endif
			bRet=false;
		}
		if(!bRet)
		{
			if(pErrorInfo)
				*pErrorInfo=CHK_PLATE_PROFILEANDBOLTINFO;
			return false;
		}
	}
	return true;
}

//已知一组平行线的求平行线与椭圆的切点
/*
	lien_vec		//直线方向
	majorAxisLen	//椭圆长半轴长度
	minAxisLen		//椭圆短半轴长度
	tangPt1			//求得的切点1
	tangPt2			//求得的切点2
*/
int Tang2dle(f3dPoint line_vec,double majorAxisLen,double minAxisLen,f3dPoint &tangPt1,f3dPoint &tangPt2)
{
	/*
	//椭圆方程
	x=majorAxisLen*cosa;
	y=minAxisLen*sina;
	//相切条件
	k=△y/△x=(-majorAxisLen*sina)/(minAxisLen*cosa)
	tana=-k*(majorAxisLen/minAxisLen);
	sina=sqrt(tana*tana/(tana*tana+1));
	cosa=sqrt(1/(tana*tana+1));
	可求得sina,cosa 进而求得x，y
	*/
	if(majorAxisLen<eps2||minAxisLen<eps2||line_vec.IsZero())
		return 0;
	double k=line_vec.y/line_vec.x;	//直线斜率
	if(fabs(line_vec.y)<eps)
	{	//平行于椭圆长轴
		tangPt1.Set(minAxisLen,0);
		tangPt2.Set(-minAxisLen,0);
		return 1;
	}
	else if(fabs(line_vec.x)<eps)
	{	//平行于椭圆短轴
		tangPt1.Set(majorAxisLen,0);
		tangPt2.Set(-majorAxisLen,0);
		return 1;
	}
	double x=0,y=0;					//切点X,Y坐标
	double tana=-k*(majorAxisLen/minAxisLen);
	double sina=sqrt(tana*tana/(tana*tana+1));
	double cosa=sqrt(1.0/(tana*tana+1));
	x=sina*majorAxisLen;
	y=cosa*minAxisLen;
	//根据x，y坐标可以得到四个点，舍去其中不合适的点
	f3dPoint vec1,vec2;
	vec1.Set(x,y,0);
	vec2.Set(x,-y,0);
	normalize(vec1);
	normalize(vec2);
	if(fabs(line_vec*vec1)<fabs(line_vec*vec2))
	{
		tangPt1.Set(x,y,0);
		tangPt2.Set(-tangPt1.x,-tangPt1.y,0);
		return 1;
	}
	else if(fabs(line_vec*vec1)>fabs(line_vec*vec2))
	{
		tangPt1.Set(x,-y,0);
		tangPt2.Set(-tangPt1.x,-tangPt1.y,0);
		return 1;
	}
	return 0;
}
//计算始终端顶点到相应挂线孔圆弧的切点
BOOL CalStartAndEndTangentPt(f3dPoint startPt,f3dPoint endPt,double radius,f3dPoint center,PROFILE_VERTEX& startTangPt,PROFILE_VERTEX& endTangPt)
{
	f3dPoint vec,pick_vec,pick_pos;
	vec=endPt-startPt;
	normalize(vec);
	pick_vec.Set(vec.y,-vec.x,0);
	//始端顶点与始端处挂线孔所在圆弧求切点
	f2dPoint tan_pt;
	f2dCircle circle;
	circle.radius=radius;
	circle.centre.Set(center.x,center.y);
	pick_pos=center+pick_vec*10;
	if(Tang2dpc(f2dPoint(startPt.x,startPt.y),circle,pick_pos,tan_pt))
	{
		startTangPt.vertex.Set(tan_pt.x,tan_pt.y,0);
		startTangPt.type=2;
		startTangPt.center=center;
		startTangPt.radius=radius;
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Start vertex on arc inside of the hole for hanging wire,failed design！");
#else
		logerr.Log("始端顶点在挂线孔圆弧内部,设计失败！");
#endif
		return FALSE;
	}
	//终端顶点与终端处挂线孔所在圆弧求切点
	circle.radius=radius;
	circle.centre.Set(center.x,center.y);
	pick_pos=center+pick_vec*10;
	if(Tang2dpc(f2dPoint(endPt.x,endPt.y),circle,pick_pos,tan_pt))
		endTangPt.vertex.Set(tan_pt.x,tan_pt.y,0);
	else
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("End vertex on arc inside of the hole for hanging wire,failed design！");
#else
		logerr.Log("终端顶点在挂线孔圆弧内部,设计失败！");
#endif
		return FALSE;
	}
	return TRUE;
}
//在某些情况下螺栓是单独设计，为加快速度不需要重新设计螺栓位置
BOOL CLDSParamPlate::DesignPlate(bool bDesignBolt/*=true*/)
{
	if(IsShadowObject())
	{
		ShadowDesign();
		return TRUE;
	}
 	f3dPoint vertice;
	CVertexsLifeObject manageVertexs(vertex_list);
	vertex_list.Empty();
	SetModified();
	if(m_bStdPart)	//标准构件不用进行设计
	{	//填充标准构件顶点列表 出组焊图调整长度时需要	wht 09-07-10
		if(m_iParamType==TYPE_UEND||m_iParamType==TYPE_ROLLEND||m_iParamType==TYPE_XEND)	
		{
			double L,N,H,W,S;
			CLDSStdPartModel stdPartModel;
			if(!GetStdPartModel(&stdPartModel))
				return FALSE;
			L=stdPartModel.param.insert_plate_param.L;
			N=stdPartModel.param.insert_plate_param.N;
			H=stdPartModel.param.insert_plate_param.H;
			W=stdPartModel.param.insert_plate_param.W;
			S=stdPartModel.param.insert_plate_param.S;
			if(m_iParamType==TYPE_UEND)
			{
				vertice.Set(0,0.5*H,0);
				vertex_list.append(vertice);
				vertice.Set(0,-0.5*H,0);
				vertex_list.append(vertice);
				vertice.Set(L-N,-0.5*H,0);
				vertex_list.append(vertice);
				vertice.Set(L,-0.5*H+S,0);
				vertex_list.append(vertice);
				vertice.Set(L,0.5*H-S,0);
				vertex_list.append(vertice);
				vertice.Set(L-N,0.5*H,0);
				vertex_list.append(vertice);
			}
			else if(m_iParamType==TYPE_ROLLEND)
			{
				vertice.Set(L,0.5*H-S);
				vertice.feature=1;
				vertex_list.append(vertice);
				vertice.Set(L-N,0.5*H);
				vertice.feature=1;//12;
				vertex_list.append(vertice);
				vertice.y+=W;
				vertice.feature=2;
				//vertex_list.append(vertice);
				vertice.x=0;
				//vertex_list.append(vertice);
				vertice.y=0.5*H;
				vertice.feature=1;//12;
				vertex_list.append(vertice);
				vertice.y=-0.5*H;
				vertice.feature=1;//13;
				vertex_list.append(vertice);
				vertice.y-=W;
				vertice.feature=3;
				//vertex_list.append(vertice);
				vertice.x=L-N;
				//vertex_list.append(vertice);
				vertice.y=-0.5*H;
				vertice.feature=1;//13;
				vertex_list.append(vertice);
				vertice.Set(L,-0.5*H+S);
				vertice.feature=1;
				vertex_list.append(vertice);
			}
			else //if(m_iParamType==TYPE_XEND)
			{
				vertice.Set(0,-0.5*H,0);
				vertex_list.append(vertice);
				vertice.Set(L,-0.5*H,0);
				vertex_list.append(vertice);
				vertice.Set(L,0.5*H,0);
				vertex_list.append(vertice);
				vertice.Set(0,0.5*H,0);
				vertex_list.append(vertice);
			}
		}
		else if(m_iParamType==TYPE_FLD||m_iParamType==TYPE_FLP)
		{
			CLDSStdPartModel stdPartModel;
			if(library==NULL||!library->GetStdPartModel(sPartNo,m_iParamType,&stdPartModel))
				return FALSE;
			double R=0.5*stdPartModel.param.fl_param.D;
			PROFILE_VERTEX vertex;
			//vertex.type=1;
			//vertex.center=ucs.origin;
			vertex.type=2;	//指定圆弧半径便于转换为普通钢板时修外形 wjh-2013.10.11
			vertex.radius=R;
			vertex.sector_angle = Pi/2.0;	//四等分圆弧
			vertex_list.Empty();
			vertex.vertex.Set(R,0,0);
			vertex_list.append(vertex);
			vertex.vertex.Set(0,R,0);
			vertex_list.append(vertex);
			vertex.vertex.Set(-R,0,0);
			vertex_list.append(vertex);
			vertex.vertex.Set(0,-R,0);
			vertex_list.append(vertex);
		}
		else if(m_iParamType==TYPE_FLG||m_iParamType==TYPE_FLR)
		{
			CLDSStdPartModel stdPartModel;
			if(library==NULL||!library->GetStdPartModel(sPartNo,m_iParamType,&stdPartModel))
				return FALSE;
			double R=0.5*stdPartModel.param.fl_param2.fOuterD;
			PROFILE_VERTEX vertex;
			//vertex.type=1;
			//vertex.center=ucs.origin;
			vertex.type=2;	//指定圆弧半径便于转换为普通钢板时修外形 wjh-2013.10.11
			vertex.radius=R;
			vertex.sector_angle = Pi/2.0;	//四等分圆弧
			vertex_list.Empty();
			vertex.vertex.Set(R,0,0);
			vertex_list.append(vertex);
			vertex.vertex.Set(0,R,0);
			vertex_list.append(vertex);
			vertex.vertex.Set(-R,0,0);
			vertex_list.append(vertex);
			vertex.vertex.Set(0,-R,0);
			vertex_list.append(vertex);
		}
	}
	else
	{
		CHashTable<DESIGN_PLATE_ITEM*>itemHashTbl;
		itemHashTbl.CreateHashTable(params.GetNodeNum());
		for(DESIGN_PLATE_ITEM* pItem=params.GetFirst();pItem;pItem=params.GetNext())
			itemHashTbl.SetValueAt(pItem->key,pItem);
		if(m_iParamType==TYPE_UEND)
		{
			double W,L,H,M,N,S;
			if(!GetDesignItemValue('W',&W,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('L',&L,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('H',&H,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('M',&M,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('N',&N,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('S',&S,&itemHashTbl))
				return FALSE;
			vertice.Set(0,0.5*H,0);
			vertex_list.append(vertice);
			vertice.Set(0,-0.5*H,0);
			vertex_list.append(vertice);
			vertice.Set(L-N,-0.5*H,0);
			vertex_list.append(vertice);
			vertice.Set(L,-0.5*H+S,0);
			vertex_list.append(vertice);
			vertice.Set(L,0.5*H-S,0);
			vertex_list.append(vertice);
			vertice.Set(L-N,0.5*H,0);
			vertex_list.append(vertice);
		}
		else if(m_iParamType==TYPE_ROLLEND)
		{
			double H,L,W,M,N,S;
			if(!GetDesignItemValue('H',&H,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('L',&L,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('W',&W,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('M',&M,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('N',&N,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('S',&S,&itemHashTbl))
				return FALSE;
			vertice.Set(L,0.5*H-S);
			vertice.feature=1;
			vertex_list.append(vertice);
			vertice.Set(L-N,0.5*H);
			vertice.feature=1;//12;
			vertex_list.append(vertice);
			vertice.y+=W;
			vertice.feature=2;
			//vertex_list.append(vertice);
			vertice.x=0;
			//vertex_list.append(vertice);
			vertice.y=0.5*H;
			vertice.feature=1;//12;
			vertex_list.append(vertice);
			vertice.y=-0.5*H;
			vertice.feature=1;//13;
			vertex_list.append(vertice);
			vertice.y-=W;
			vertice.feature=3;
			//vertex_list.append(vertice);
			vertice.x=L-N;
			//vertex_list.append(vertice);
			vertice.y=-0.5*H;
			vertice.feature=1;//13;
			vertex_list.append(vertice);
			vertice.Set(L,-0.5*H+S);
			vertice.feature=1;
			vertex_list.append(vertice);
		}
		else if(m_iParamType==TYPE_XEND)
		{	
			double W,L,H,M,N,S;
			if(!GetDesignItemValue('L',&L,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('H',&H,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('M',&M,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('W',&W,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('N',&N,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('S',&S,&itemHashTbl))
				return FALSE;
			vertice.Set(0,0.5*H,0);
			vertex_list.append(vertice);
			vertice.Set(0,-0.5*H,0);
			vertex_list.append(vertice);
			vertice.Set(L,-0.5*H,0);
			vertex_list.append(vertice);
			vertice.Set(L,0.5*H,0);
			vertex_list.append(vertice);
		}
		else if(m_iParamType==TYPE_CIRRIBPLATE||m_iParamType==TYPE_TUBERIBPLATE)//||m_iParamType==TYPE_CIRCOVERPLATE)
		{
			if(!DesignCircularPlate(&itemHashTbl))
				return FALSE;
		}
		else if(m_iParamType==TYPE_CIRCOVERPLATE)
		{
			if(!DesignCircularCoverPlate(&itemHashTbl))
				return FALSE;
		}
		else if(m_iParamType==TYPE_WATERSHIELDEDPLATE)
		{	//遮水板
			if(!DesignWaterShieldedPlate(&itemHashTbl))
				return FALSE;
		}
		else if(m_iParamType==TYPE_FL)
		{
			double W,R;//,A;
			if(!GetDesignItemValue('R',&R,&itemHashTbl))
				return FALSE;
			if(!GetDesignItemValue('W',&W,&itemHashTbl))
				return FALSE;
			//if(!GetDesignItemValue('A',&A))
			//	return FALSE;
			double RO=W*0.5;
			PROFILE_VERTEX vertex;
			//vertex.type=1;
			//vertex.center=ucs.origin;
			vertex.type=2;	//指定圆弧半径便于转换为普通钢板时修外形 wjh-2013.10.11
			vertex.radius=RO;
			vertex.sector_angle = Pi*0.5;	//四等分圆弧
			vertex_list.Empty();
			vertex.vertex.Set(RO,0,0);
			vertex_list.append(vertex);
			vertex.vertex.Set(0,RO,0);
			vertex_list.append(vertex);
			vertex.vertex.Set(-RO,0,0);
			vertex_list.append(vertex);
			vertex.vertex.Set(0,-RO,0);
			vertex_list.append(vertex);
		}
		else if(m_iParamType==TYPE_ELBOWPLATE)
		{
			RIBPLATE_ELBOW elbow;
			elbow.LoadParams(this,&itemHashTbl);
			CSuperSmartPtr<CLDSPart> pBottomPlate=BelongModel()->FromPartHandle(abs(elbow.hBtmPlate.hValue),CLS_PLATE,CLS_PARAMPLATE);
			CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(abs(elbow.hDatumTube.hValue),CLS_LINETUBE);
			if(!DesignRibElbowPlate(pDatumTube,pBottomPlate.GeneralPlatePointer(),elbow))
				return FALSE;
		}
		else if(m_iParamType==TYPE_LINEANGLERIBPLATE)
		{
			CLDSParamPlate::RIBPLATE_ANGLE anglerib;
			anglerib.LoadParams(this,&itemHashTbl);
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(anglerib.A.ToLong(),CLS_LINEANGLE);
			if(!DesignRibPlateOnAngle(pLineAngle,anglerib))
				return FALSE;
		}
		else if(m_iParamType==TYPE_TUBESTAYWIREPLATE)
		{
			CLDSParamPlate::RIBPLATE_STAY_WIRE stay_wire;
			stay_wire.LoadParams(this,&itemHashTbl);
			if(!DesignTubeStayWirePlate(stay_wire))
				return FALSE;
		}
		else if(m_iParamType==TYPE_RIBPLATE)
		{	//普通角肋板
			if(!DesignRibPlate2(&itemHashTbl))
				return FALSE;
		}
		else if(m_iParamType==TYPE_BASEPLATE)
		{	//塔脚底板
			if(!DesignBasePlate(&itemHashTbl))
				return FALSE;
		}
		else if(m_iParamType==TYPE_SHOEPLATE)
		{	//塔脚靴板
			if(!DesignShoePlate(&itemHashTbl))
				return FALSE;
		}
		else if (m_iParamType == TYPE_PEDALPLATE)
		{	//脚踏板
			if (!DesignPedalPlate(&itemHashTbl))
				return FALSE;
		}
		else if(m_iParamType==TYPE_STATIONCONNPLATE)
		{	//端撑端板
			if(!DesignStationConnPlate(&itemHashTbl))
				return FALSE;
		}
		else if(m_iParamType==TYPE_STATIONSHOEPLATE)
		{	//立柱夹板
			if(!DesignStationShoePlate(&itemHashTbl))
				return FALSE;
		}
		else if(m_iParamType==TYPE_STATIONCROSSPLATE)
		{	//端撑插板
			if(!DesignStationCrossPlate(&itemHashTbl))
				return FALSE;
		}
		else if(m_iParamType==TYPE_STATIONLINKPLATE)
		{	//端撑连板
			if(!DesignStationLinkPlate(&itemHashTbl))
				return FALSE;
		}
		else if(m_iParamType==TYPE_STATIONNODEPLATE)
		{	//立柱连板
			if(!DesignStationNodePlate(&itemHashTbl))
				return FALSE;
		}
		else if(m_iParamType==TYPE_WALKWAYCIRPLATE)
		{	//走道的加劲环板
			if(!DesignWalkWayCirPlate(&itemHashTbl))
				return FALSE;
		}
		else if(m_iParamType==TYPE_VERTPLATE||m_iParamType==TYPE_ENDPLATE)
		{
			double S,L,W,H,M,E=0;
			CLDSParamPlate *pParamPlate = (CLDSParamPlate*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_PARAMPLATE);
			if(!pParamPlate)
			{
				//AfxMessageBox("垂向加劲板无焊接父构件重新设计失败!");
				return FALSE;
			}
			pParamPlate->GetDesignItemValue('W',&W,&itemHashTbl);
			pParamPlate->GetDesignItemValue('M',&M,&itemHashTbl);
			GetDesignItemValue('E',&E,&itemHashTbl);
			if(pParamPlate->ucs.axis_z*ucs.axis_y<0)
			{
				ucs = pParamPlate->ucs;
				ucs.axis_y*=-1.0;
				ucs.axis_z*=-1.0;
			}	
			else
				ucs = pParamPlate->ucs;
			ucs.axis_x*=-1.0;	
			ucs.axis_z=ucs.axis_y;
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			ucs.origin-=ucs.axis_x*M;
			ucs.origin+=ucs.axis_z*E;
			GetDesignItemValue('S',&S,&itemHashTbl);	//起始点X坐标(起始点Y坐标)
			GetDesignItemValue('L',&L,&itemHashTbl);	//终止点X坐标(终止点Y坐标)
			GetDesignItemValue('W',&W,&itemHashTbl);	//宽
			GetDesignItemValue('H',&H,&itemHashTbl);	//加劲板所依附的板厚度的一半
			if(m_iParamType==TYPE_VERTPLATE)
			{
				vertice.Set(S,W+H,0);
				vertex_list.append(vertice);
				vertice.Set(L,W+H,0);
				vertex_list.append(vertice);
				vertice.Set(L,H,0);
				vertex_list.append(vertice);
				vertice.Set(S,H,0);
				vertex_list.append(vertice);
			}
			else if(m_iParamType==TYPE_ENDPLATE)
			{
				vertice.Set(0,W+H,S);
				vertex_list.append(vertice);
				vertice.Set(0,W+H,L);
				vertex_list.append(vertice);
				vertice.Set(0,H,L);
				vertex_list.append(vertice);
				vertice.Set(0,H,S);
				vertex_list.append(vertice);
			}
		}
		if(bDesignBolt)
		{
			for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			{
				if(pLsRef->GetLsPtr())
				{
					pLsRef->GetLsPtr()->CalGuigeAuto();
					pLsRef->GetLsPtr()->correct_worknorm();
					pLsRef->GetLsPtr()->correct_pos();
				}
			}
		}
	}
	//根据原有的轮廓点进行修正(保留用户自定义的顶点)
	ATOM_LIST<PROFILE_VERTEX> src_vertex_list;
	manageVertexs.RestoreVertexList(src_vertex_list);
	UpdateProfileVertex(src_vertex_list);
	manageVertexs.DisableBackup();	//设计成功不需要复原钢板外形
	return TRUE;
}
struct MODIFY_RELATION{
	BYTE biRelaState;	//0.替换 1.插入 2.添加
	int idModify;
	int idRelate;
	MODIFY_RELATION(){idModify=idRelate=0;biRelaState=0;}
};
BOOL CLDSParamPlate::UpdateProfileVertex(ATOM_LIST<PROFILE_VERTEX> &src_vertex_list)
{
	if(IsShadowObject())	//影像构件
		return FALSE;
	if(vertex_list.GetNodeNum()<=3||src_vertex_list.GetNodeNum()<=3)
		return FALSE;
	if(!IsCanManualModify())
		return FALSE;	//不支持用户修改
	//查找原始参数化顶点相关联的顶点
	CHashList<MODIFY_RELATION> xModifyRelList;
	int nParamVertex=0,nModifyVertex=0;
	for(int i=0;i<src_vertex_list.GetNodeNum();i++)
	{
		if(src_vertex_list[i].m_cPosDesignType!=PROFILE_VERTEX::DT_PARAMETRIC)
			nModifyVertex++;
		else
		{
			nParamVertex++;
			//
			int idRelaVertex=-1;
			for(int j=0;j<vertex_list.GetNodeNum();j++)
			{
				if(src_vertex_list[i].m_hRod==vertex_list[j].m_hRod)
				{
					idRelaVertex=j;
					break;
				}
			}
			if(idRelaVertex>-1)
			{	//找到关联顶点
				MODIFY_RELATION* pRela=xModifyRelList.Add(i+1);
				pRela->biRelaState=0;
				pRela->idModify=i;
				pRela->idRelate=idRelaVertex;
			}
		}
	}
	if(nModifyVertex<=0||nParamVertex<=0)
		return FALSE;	//都是自定义点或都是参数化点
	if(xModifyRelList.GetNodeNum()!=nParamVertex)
		return FALSE;	//没有找到与原始参数化顶点相关联的顶点
	//初始化原始自定义顶点的插入位置
	for(int i=0;i<src_vertex_list.GetNodeNum();i++)
	{
		if(src_vertex_list[i].m_cPosDesignType==PROFILE_VERTEX::DT_PARAMETRIC)
			continue;
		int idInsertVer=-1;
		for(int j=i+1;j<src_vertex_list.GetNodeNum();j++)
		{
			if(src_vertex_list[j].m_cPosDesignType==PROFILE_VERTEX::DT_PARAMETRIC)
			{
				idInsertVer=j;
				break;
			}
		}
		MODIFY_RELATION* pRela=xModifyRelList.Add(i+1);
		pRela->idModify=i;
		pRela->biRelaState=2;
		MODIFY_RELATION* pInsertRela=xModifyRelList.GetValue(idInsertVer+1);
		if(pInsertRela)
		{
			pRela->biRelaState=1;	
			pRela->idRelate=pInsertRela->idRelate;
		}
	}
	//根据当前的轮廓顶点更新原始外形顶点坐标
	for(int i=0;i<src_vertex_list.GetNodeNum();i++)
	{
		MODIFY_RELATION* pRela=xModifyRelList.GetValue(i+1);
		if(pRela&&pRela->biRelaState==0)
			src_vertex_list[i].vertex=vertex_list[pRela->idRelate].vertex;
	}
	vertex_list.Empty();
	for(PROFILE_VERTEX* pVertex=src_vertex_list.GetFirst();pVertex;pVertex=src_vertex_list.GetNext())
		vertex_list.append(*pVertex);
	return TRUE;
}
BOOL CLDSParamPlate::IsCanManualModify()
{
	if(m_iParamType==TYPE_BASEPLATE)
		return TRUE;
	else if(m_iParamType==TYPE_SHOEPLATE)
		return TRUE;
	else
		return FALSE;
}
static void CreateLsHole(CLDSParamPlate *pParamPlate,fBody *pBody,BOOL bDisplayHole,f3dPolyFace *pTopFace,f3dPolyFace *pBottomFace)
{
	if(pParamPlate==NULL)
		return;
	CHoleMaker holeMaker;
	UCS_STRU hole_ucs;
	for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{
		if(!pParamPlate->m_bDisplayHole&&!bDisplayHole&&!pLsRef->GetLsPtr()->m_bVirtualPart)
			continue;
		if(pParamPlate->m_iParamType==TYPE_XEND&&pLsRef->start1_end2_mid0>0)
		{	
			if(pLsRef->start1_end2_mid0!=11&&pLsRef->start1_end2_mid0!=21)
				continue;	//跳过不在插板范围内的螺栓
		}
		else if(pParamPlate->m_iParamType==TYPE_XEND)
		{	//用start1_end2_mid0表示螺栓位置之前做的十字插板
			double L=0;
			pParamPlate->GetDesignItemValue('L',&L);
			if(fabs((*pLsRef)->get_norm()*pParamPlate->ucs.axis_z)<EPS_COS2)
				continue;
			if((*pLsRef)->des_base_pos.len_offset_dist>L)
				continue; //不生成十字插板范围外的螺栓孔
		}
		hole_ucs.origin=(*pLsRef)->ucs.origin;
		hole_ucs.axis_z=(*pLsRef)->get_norm();
		if(hole_ucs.axis_x.IsZero())
			hole_ucs.axis_x=btc::CalLocalCS_X_AXIS(hole_ucs.axis_z);
		hole_ucs.axis_x=(*pLsRef)->ucs.axis_x;
		//hole_ucs.axis_x=inters_vec(hole_ucs.axis_z);
		//hole_ucs.axis_y=hole_ucs.axis_z^hole_ucs.axis_x;
		//hole_ucs.axis_x=hole_ucs.axis_y^hole_ucs.axis_z;
		COLORREF crLsHole=CLDSPart::GetColor(CLS_BOLT,(*pLsRef)->get_d(),' ');
		holeMaker.Create(hole_ucs,(*pLsRef)->get_d()+(*pLsRef)->hole_d_increment,pLsRef->waistLen,pTopFace,pBottomFace,pBody,crLsHole);
	}
}

double CLDSParamPlate::GetNormOffset()
{
	if(m_iParamType==TYPE_FL||m_iParamType==TYPE_FLR||m_iParamType==TYPE_FLG)
	{	//法兰盘
		return m_fNormOffset;
	}
	else if(m_iParamType==TYPE_UEND)
	{	//U型插板
		return m_fNormOffset;
	}
	else if(m_iParamType==TYPE_ROLLEND)
	{	//卷边插板
		return m_fNormOffset-GetThick()*0.5;
	}
	else if(m_iParamType==TYPE_ELBOWPLATE)
	{	//肘板
		return m_fNormOffset-GetThick()*0.5;
	}
	else if(m_iParamType==TYPE_VERTPLATE||m_iParamType==TYPE_ENDPLATE)
	{	//垂向加劲板、封头板
		return m_fNormOffset-GetThick()*0.5;
	}
	else if(m_iParamType==TYPE_CIRRIBPLATE||m_iParamType==TYPE_CIRCOVERPLATE||m_iParamType==TYPE_TUBERIBPLATE)
	{	//环向肋板、环向封板、钢管肋板(跨越连板)
		if(IsZhiWan())
			return m_fNormOffset;
		else 
			return -GetThick()*0.5;
	}
	else if(m_iParamType==TYPE_XEND)
	{	//十字插板
		return m_fNormOffset-GetThick()*0.5;
	}
	else if(m_iParamType==TYPE_FLD||m_iParamType==TYPE_FLP)
	{	//对焊法兰、平焊法兰
		return m_fNormOffset;
	}
	else if(m_iParamType==TYPE_WATERSHIELDEDPLATE)
	{	//遮水板
		return m_fNormOffset;
	}
	else if(m_iParamType==TYPE_LINEANGLERIBPLATE||m_iParamType==TYPE_RIBPLATE)
	{	//角钢肋板、普通肋板(角肋板，槽肋板，角钢肋板，钢板肋板，钢管肋板，钢管夹板) wht 11-03-21
		return m_fNormOffset-GetThick()*0.5;
	}
	else
		return m_fNormOffset;
}
long CLDSParamPlate::get_BaseNode()	//返回连接板的依赖节点
{
	CLDSNode* pBaseNode=GetBaseNode();
	if(pBaseNode)
		return pBaseNode->handle;
	else
		return 0;
}
long CLDSParamPlate::get_BaseRod()	//返回钢板基准节点  wht 09-12-12
{
	long hBaseRod=0;
	CLDSNode *pBaseNode=NULL;
	if(IsFL()||m_iParamType==TYPE_UEND||m_iParamType==TYPE_ROLLEND||m_iParamType==TYPE_XEND)
	{	//钢管端连接
		CLDSLineTube *pLineTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
		return (pLineTube)?pLineTube->handle:0;
	}	
	else if(m_iParamType==TYPE_CIRCOVERPLATE||m_iParamType==TYPE_CIRRIBPLATE||
		m_iParamType==TYPE_TUBERIBPLATE||m_iParamType==TYPE_WATERSHIELDEDPLATE)		//环向肋板
		GetDesignItemValue('C',&hBaseRod);
	else if(m_iParamType==TYPE_LINEANGLERIBPLATE)//角钢肋板
		GetDesignItemValue('A',&hBaseRod);
	else if(m_iParamType==TYPE_ELBOWPLATE)		//肘板
		GetDesignItemValue('K',&hBaseRod);
	//else if(m_iParamType==TYPE_RIBPLATE)	//普通肋板情况复杂暂未处理
	else if(m_iParamType==TYPE_BASEPLATE)
		GetDesignItemValue('L',&hBaseRod);	
	else if(m_iParamType==TYPE_SHOEPLATE)
		GetDesignItemValue('D',&hBaseRod);	
	return hBaseRod;
}
void CLDSParamPlate::SetNormOffset(double fNormOffset)
{
	if(m_iParamType==TYPE_FL||m_iParamType==TYPE_FLG||m_iParamType==TYPE_FLR)
	{	//法兰盘
		m_fNormOffset=fNormOffset;
	}
	else if(m_iParamType==TYPE_UEND)
	{	//U型插板
		m_fNormOffset=fNormOffset;
	}
	else if(m_iParamType==TYPE_ROLLEND)
	{	//卷边插板
		m_fNormOffset=fNormOffset+GetThick()*0.5;
	}
	else if(m_iParamType==TYPE_ELBOWPLATE)
	{	//肘板
		m_fNormOffset=fNormOffset+GetThick()*0.5;
	}
	else if(m_iParamType==TYPE_VERTPLATE||m_iParamType==TYPE_ENDPLATE)
	{	//垂向加劲板、封头板
		m_fNormOffset=fNormOffset+GetThick()*0.5;
	}
	else if(m_iParamType==TYPE_CIRRIBPLATE||m_iParamType==TYPE_CIRCOVERPLATE||m_iParamType==TYPE_TUBERIBPLATE)
	{	//环向肋板、环向封板、钢管肋板(跨越连板)
		m_fNormOffset=fNormOffset;
	}
	else if(m_iParamType==TYPE_XEND)
	{	//十字插板
		m_fNormOffset=fNormOffset+GetThick()*0.5;
	}
	else if(m_iParamType==TYPE_FLD||m_iParamType==TYPE_FLP)
	{	//对焊法兰、平焊法兰
		m_fNormOffset=fNormOffset;
	}
	else if(m_iParamType==TYPE_WATERSHIELDEDPLATE)
	{	//遮水板
		m_fNormOffset=fNormOffset;
	}
	else if(m_iParamType==TYPE_LINEANGLERIBPLATE||m_iParamType==TYPE_RIBPLATE)
	{	//角钢肋板、普通肋板(角肋板，槽肋板，角钢肋板，钢板肋板，钢管肋板，钢管夹板) wht 11-03-21
		m_fNormOffset=fNormOffset+GetThick()*0.5;
	}
	else
		m_fNormOffset=fNormOffset;
}
//生成矩形立体面
void CreateRectSolidFaces(CCreateFace& createFace,fBody* pBody)
{
	int nVertex=(int)(pBody->vertex.GetNodeNum()*0.5);
	//顶面
	f3dPolyFace* pTopFace=pBody->faceList.append();
	for(int i=0;i<nVertex;i++)
	{
		if(i==0)
			createFace.NewOutterEdgeLine(pTopFace,((i+1)%nVertex)*2+1,1);
		else
			createFace.NewOutterEdgeLine(pTopFace,((i+1)%nVertex)*2+1);
	}
	//底面
	f3dPolyFace* pBtmFace=pBody->faceList.append();
	for(int i=0;i<nVertex;i++)
	{
		if(i==0)
			createFace.NewOutterEdgeLine(pBtmFace,(nVertex-1)*2,0);
		else
			createFace.NewOutterEdgeLine(pBtmFace,(nVertex-1-i)*2);
	}
	//侧面
	f3dPolyFace* pFace=NULL;
	for(int i=0; i<nVertex; i++)
	{
		pFace=pBody->faceList.append();
		createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2,i*2);
		createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2);
	}
}
typedef f3dPoint* f3dPointPtr;
BOOL CLDSParamPlate::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;
	fBody body;
	fBody* pBody=&body;
	if(pSolidBody!=NULL && !IsSolidModified())	//数据未修改
		return TRUE;
	else
	{
		if(pSolidBody==NULL)
			pSolidBody = new CSolidBody;
		else
			pSolidBody->Empty();
	}
	SetModified(FALSE);
	//标准构件
	if(m_bStdPart)	//拷贝标准构件实体数据
	{
		BOOL bRet=FALSE;
		CLDSStdPartModel stdPartModel;
		if(GetStdPartModel(&stdPartModel))
			bRet=stdPartModel.Create3dSolidModel(pBody,m_bDisplayHole||bDisplayHole,scale_of_user2scr,sample_len,smoothness,m_cRollProcessType);
		if(bRet)
		{
			for(f3dPoint* pVertex=pBody->vertex.GetFirst();pVertex;pVertex=pBody->vertex.GetNext())
				coord_trans(*pVertex,ucs,TRUE,TRUE);
			for(f3dPolyFace* pFace=pBody->faceList.GetFirst();pFace;pFace=pBody->faceList.GetNext())
			{
				if(!pFace->poly_norm.IsZero())
					vector_trans(pFace->poly_norm,ucs,TRUE,TRUE);
				/*
				if(m_iParamType==TYPE_UEND)//临时用来修改U型板的曲面法线
				{
					for(f3dAtomLine *pLine=pFace->outer_edge.GetFirst();pLine;pLine=pFace->outer_edge.GetNext())
					{
						if(pLine->SectorAngle()>0)
						{
							f3dPoint lineNorm = pLine->WorkNorm();
							vector_trans(lineNorm,ucs,TRUE);
							pLine->CreateArcLine(lineNorm,pLine->SectorAngle());
						}
					}
				}*/
			}
			is_data_modified = FALSE;
			pSolidBody->ConvertFrom(pBody);
		}
		return bRet;
	}
	if(vertex_list.GetNodeNum()<3)
		DesignPlate(false);
	int i=0,nVertex=vertex_list.GetNodeNum();
	//非标准构件
	if(nVertex<3)
		return FALSE;
	is_data_modified = FALSE;
	PROFILE_VERTEX *pVertex;
	CHashTable<PROFILE_VERTEX>vertex_tbl;
	vertex_tbl.CreateHashTable(vertex_list.GetNodeNum());
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		i++;
		pVertex->vertex.ID = i;
		vertex_tbl.SetValueAt(i,*pVertex);
	}
	
	f3dPolyFace *pFace=NULL;
	fLoop* pLoop=NULL;
	f3dPoint vertice;
	COLORREF plank_color=GetColor(CLS_PARAMPLATE,-1,cMaterial);	//默认板颜色
	if(m_bUserSpecColor)	//用户指定构件颜色
		plank_color=crMaterial;
	if(m_iParamType==TYPE_FL)
	{
		double W,fInnerRadius;
		GetDesignItemValue('W',&W);
		GetDesignItemValue('R',&fInnerRadius);
		int slices= CalArcResolution(0.5*W,2*Pi,scale_of_user2scr,sample_len,smoothness);
		slices=min(slices,200);	//防止后面临时建立的outter_down_vertex_arr数组越界
		double coshalf_alfa = cos(Pi/slices);	//半角余弦
		double RO=0.5*W/coshalf_alfa;			//法兰外环显示半径
		double RI=fInnerRadius/coshalf_alfa;	//法兰内环显示半径
		double scaleRI=RI/RO;	//内环显示半径占外环显示半径的比例值
		//生成法兰不含螺栓孔在内的外轮廓顶点
		f3dPoint vertex_arr[200];
		GetCircleSimuPolyVertex(RO,vertex_arr,slices);
		for(i=0;i<slices;i++)
		{
			pBody->vertex.append(vertex_arr[i]);	//外侧底部,i*4
			vertex_arr[i].z=thick;
			pBody->vertex.append(vertex_arr[i]);	//外侧顶部,i*4+1
			vertex_arr[i]*=scaleRI;
			vertex_arr[i].z=thick;
			pBody->vertex.append(vertex_arr[i]);	//内侧顶部,i*4+2
			vertex_arr[i].z=0;
			pBody->vertex.append(vertex_arr[i]);	//内侧底部,i*4+3
		}
		f3dPoint *pVertice;
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//顶平
		f3dPolyFace *pTopFace=pBody->faceList.append();
		pTopFace->poly_norm=ucs.axis_z;
		pTopFace->material = plank_color;
		if(fInnerRadius>0)
			pLoop=pTopFace->inner_loop.append();
		for(i=0;i<slices;i++)
		{
			createFace.NewOutterEdgeLine(pTopFace,i*4+1,((i+1)%slices)*4+1);
			if(fInnerRadius>0)
				createFace.NewInnerLoopEdgeLine(pLoop,((slices-i)%slices)*4+2,(slices-i-1)*4+2);
		}
		//底平面
		f3dPolyFace *pBottomFace=pBody->faceList.append();
		pBottomFace->poly_norm=-ucs.axis_z;
		pBottomFace->material = plank_color;
		if(fInnerRadius>0)
			pLoop=pBottomFace->inner_loop.append();
		for(i=0;i<slices;i++)
		{
			createFace.NewOutterEdgeLine(pBottomFace,((slices-i)%slices)*4,(slices-i-1)*4);
			if(fInnerRadius>0)
				createFace.NewInnerLoopEdgeLine(pLoop,i*4+3,((i+1)%slices)*4+3);
		}
		//外圆柱侧面
		for(i=0;i<slices;i++)
		{
			pFace=pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4,i*4);
			createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+1);
			createFace.NewOutterEdgeLine(pFace,i*4+1);
			createFace.NewOutterEdgeLine(pFace,i*4);
		}
		//内圆柱侧面
		for(i=0;i<slices;i++)
		{
			pFace=pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace,i*4+2,i*4+3);
			createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+2);
			createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+3);
			createFace.NewOutterEdgeLine(pFace,i*4+3);
		}
		//坐标系由相对坐标转换到结构体的绝对坐标下
		for(pVertice=pBody->vertex.GetFirst(),i=0;pVertice;pVertice=pBody->vertex.GetNext(),i++)
		{
			if(coord_trans(*pVertice,ucs,TRUE,TRUE)!=TRUE)
				return FALSE;
		}
		//定义螺栓孔
		CreateLsHole(this,pBody,bDisplayHole,pTopFace,pBottomFace);
	}
	else if(m_iParamType==TYPE_UEND)
	{
		double W,H;
		GetDesignItemValue('W',&W);
		GetDesignItemValue('H',&H);
		int nVertex=vertex_list.GetNodeNum();
		f3dLine baseline;
		baseline.startPt=vertex_list.GetFirst()->vertex;
		baseline.endPt=vertex_list.GetNext()->vertex;
		f3dPoint base_vec=baseline.endPt-baseline.startPt;
		base_vec.z=0;
		normalize(base_vec);
		f3dPoint base_normoffset_vec=f3dPoint(0,0,1)^base_vec;
		baseline.startPt+=base_normoffset_vec*(thick+0.5*W);
		baseline.endPt+=base_normoffset_vec*(thick+0.5*W);
		baseline.startPt.z=thick+0.5*W;
		pBody->vertex.append(baseline.startPt);
		baseline.startPt.z=0.5*W;
		pBody->vertex.append(baseline.startPt);
		baseline.startPt.z=-0.5*W;
		pBody->vertex.append(baseline.startPt);
		baseline.startPt.z=-thick-0.5*W;
		pBody->vertex.append(baseline.startPt);
		
		baseline.endPt.z=thick+0.5*W;
		pBody->vertex.append(baseline.endPt);
		baseline.endPt.z=0.5*W;
		pBody->vertex.append(baseline.endPt);
		baseline.endPt.z=-0.5*W;
		pBody->vertex.append(baseline.endPt);
		baseline.endPt.z=-thick-0.5*W;
		pBody->vertex.append(baseline.endPt);
		for(i=2;i<nVertex;i++)
		{
			f3dPoint vertice=vertex_list.GetNext()->vertex;
			vertice.z= 0.5*W+thick;
			pBody->vertex.append(vertice);
			vertice.z= 0.5*W;
			pBody->vertex.append(vertice);
			vertice.z=-0.5*W;
			pBody->vertex.append(vertice);
			vertice.z=-0.5*W-thick;
			pBody->vertex.append(vertice);
		}
		//用来显示曲形面 有八个点和以前的重复 为了以后添加面方便
		int uSlices = CalArcResolution(0.5*W+thick,Pi,scale_of_user2scr,sample_len,smoothness);
		uSlices = min(uSlices,200);
		f3dPoint arc_vertex_arr[200],arc_vertex_arr_inner[200];
		double outter_radius = 0.5*W+thick;
		double inner_radius = 0.5*W;
		f3dArcLine arcLine;
		f3dPoint arcLineNorm,arcLineStart,arcLineEnd,arcLineOringin;
		arcLineNorm.Set(0,0,1);
		arcLineOringin.Set(0,0.5*W+thick,0);
		arcLineStart.Set(0.5*W+thick,0.5*W+thick,0);
		arcLineEnd.Set(0.5*W+thick,-(0.5*W+thick),0);
		//外环
		arcLine.CreateMethod3(arcLineStart,arcLineEnd,arcLineNorm,outter_radius,arcLineOringin);
		CLDSPart::GetArcSimuPolyVertex(&arcLine,arc_vertex_arr,uSlices);
		//内环
		arcLineStart.Set(0.5*W+thick,0.5*W,0);
		arcLineEnd.Set(0.5*W+thick,-0.5*W,0);
		arcLine.CreateMethod3(arcLineStart,arcLineEnd,arcLineNorm,inner_radius,arcLineOringin);
		CLDSPart::GetArcSimuPolyVertex(&arcLine,arc_vertex_arr_inner,uSlices);
		for(i=0;i<=uSlices;i++)
		{
			vertice.x = arc_vertex_arr[i].x;
			vertice.z = arc_vertex_arr[i].y;
			vertice.y = 0.5*H;
			pBody->vertex.append(vertice);//i*4+nVertex*4
			vertice.y = -0.5*H;
			pBody->vertex.append(vertice);//i*4+nVertex*4+1
			vertice.x = arc_vertex_arr_inner[i].x;
			vertice.z = arc_vertex_arr_inner[i].y;
			vertice.y = 0.5*H;
			pBody->vertex.append(vertice);//i*4+nVertex*4+2
			vertice.y = -0.5*H;
			pBody->vertex.append(vertice);//i*4+nVertex*4+3
		}
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//坐标系由相对坐标转换到结构体的绝对坐标下
		for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		{
			if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
				return FALSE;
		}
		//顶平面
		f3dPolyFace *pTopFace=pBody->faceList.append();
		pTopFace->material = plank_color;
		for(i=0;i<nVertex;i++)
			createFace.NewOutterEdgeLine(pTopFace,((i+1)%nVertex)*4,i*4);
		//中间层上平面
		f3dPolyFace *pMidUpFace=pBody->faceList.append();
		pMidUpFace->material = plank_color;
		for(i=nVertex;i>0;i--)
			createFace.NewOutterEdgeLine(pMidUpFace,(i-1)*4+1,(i%nVertex)*4+1);
		//中间层下平面
		f3dPolyFace *pMidDownFace=pBody->faceList.append();
		pMidDownFace->material = plank_color;
		for(i=0;i<nVertex;i++)
			createFace.NewOutterEdgeLine(pMidDownFace,((i+1)%nVertex)*4+2,i*4+2);
		//底平面
		f3dPolyFace *pBottomFace=pBody->faceList.append();
		pBottomFace->material = plank_color;
		for(i=nVertex;i>0;i--)
			createFace.NewOutterEdgeLine(pBottomFace,(i-1)*4+3,(i%nVertex)*4+3);
		//定义螺栓孔
		CreateLsHole(this,pBody,bDisplayHole,pTopFace,pMidUpFace);
		CreateLsHole(this,pBody,bDisplayHole,pMidDownFace,pBottomFace);
		
		vector_trans(base_vec,ucs,TRUE);
		//显示四个曲面
		int iArcStart = nVertex*4;
		for(i=0;i<uSlices;i++)
		{	//制弯外圆柱面
			pFace=pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart,i*4+iArcStart);
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart+1);
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart+1);
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart);
			//制弯内圆柱面
			pFace=pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart+2,(i+1)*4+iArcStart+2);
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart+3);
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart+3);
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart+2);
			//正半环形面
			pFace=pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart,(i+1)*4+iArcStart);
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart+2);
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart+2);
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart);
			//负半环形面
			pFace=pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart+1,i*4+iArcStart+1);
			createFace.NewOutterEdgeLine(pFace,(i+1)*4+iArcStart+3);
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart+3);
			createFace.NewOutterEdgeLine(pFace,i*4+iArcStart+1);
		}
		//各上层矩形侧面
		for(i=0;i<nVertex;i++)
		{
			if(i==0)
				continue;
			pFace=pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace,i*4+1,i*4);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*4+1);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*4);
			createFace.NewOutterEdgeLine(pFace,i*4);
		}
		//各下层矩形侧面
		for(i=0;i<nVertex;i++)
		{
			if(i==0)
				continue;
			pFace=pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace,i*4+3,i*4+2);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*4+3);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*4+2);
			createFace.NewOutterEdgeLine(pFace,i*4+2);
		}
	}
	else if(m_iParamType==TYPE_ROLLEND)
	{
		double H,L,W,N,M,S;
		GetDesignItemValue('H',&H);
		GetDesignItemValue('L',&L);
		GetDesignItemValue('W',&W);
		GetDesignItemValue('M',&M);
		GetDesignItemValue('N',&N);
		GetDesignItemValue('S',&S);
		//根据卷边处理方式设置卷边显示样式
		//UINT nRollProcessType = CLDSPart::library->GetRollProcessType();
		double fOuterOffsetLen=0.0,fInnerOffsetLen=0.0;	//
		if(m_cRollProcessType==1)
		{	//最少刀数1 保证卷边外侧边长度为L-N wht 11-04-18
			if((S+W)>0)
				fInnerOffsetLen=(S*N)/(S+W);
		}
		else if(m_cRollProcessType==2)
		{	//最少刀数2 保证卷边内侧边长度为L-N
			if(S>0)
				fOuterOffsetLen=W*N/S;
		}
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			vertice = pVertex->vertex;
			if(fabs(vertice.x-L+N)<EPS)	//调整X坐标为(L-N)的顶点坐标 wht 11-04-18
				vertice.x+=fInnerOffsetLen;	
			vertice.z=-0.5*thick;
			pBody->vertex.append(vertice);
			vertice.z=0.5*thick;
			pBody->vertex.append(vertice);
		}
		if(W>0)
		{	//上侧卷边  wht 08-02-27 
			vertice.Set(L-N+fInnerOffsetLen,0.5*H,-0.5*thick);
			pBody->vertex.append(vertice);
			vertice.Set(L-N+fInnerOffsetLen,0.5*H-thick,-0.5*thick);
			pBody->vertex.append(vertice);
			vertice.Set(0,0.5*H-thick,-0.5*thick);
			pBody->vertex.append(vertice);
			vertice.Set(0,0.5*H,-0.5*thick);
			pBody->vertex.append(vertice);

			vertice.Set(L-N-fOuterOffsetLen,0.5*H,0.5*thick-W);
			pBody->vertex.append(vertice);
			vertice.Set(L-N-fOuterOffsetLen,0.5*H-thick,0.5*thick-W);
			pBody->vertex.append(vertice);
			vertice.Set(0,0.5*H-thick,0.5*thick-W);
			pBody->vertex.append(vertice);
			vertice.Set(0,0.5*H,0.5*thick-W);
			pBody->vertex.append(vertice);

			//下侧卷边
			vertice.Set(0,-0.5*H+thick,-0.5*thick);
			pBody->vertex.append(vertice);
			vertice.Set(L-N+fInnerOffsetLen,-0.5*H+thick,-0.5*thick);
			pBody->vertex.append(vertice);
			vertice.Set(L-N+fInnerOffsetLen,-0.5*H,-0.5*thick);
			pBody->vertex.append(vertice);
			vertice.Set(0,-0.5*H,-0.5*thick);
			pBody->vertex.append(vertice);
			
			vertice.Set(0,-0.5*H+thick,0.5*thick-W);
			pBody->vertex.append(vertice);
			vertice.Set(L-N-fOuterOffsetLen,-0.5*H+thick,0.5*thick-W);
			pBody->vertex.append(vertice);
			vertice.Set(L-N-fOuterOffsetLen,-0.5*H,0.5*thick-W);
			pBody->vertex.append(vertice);
			vertice.Set(0,-0.5*H,0.5*thick-W);
			pBody->vertex.append(vertice);
		}
		//坐标系由相对坐标转换到结构体的绝对坐标下
		for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		{
			if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
				return FALSE;
		}
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//基准面顶面
		f3dPolyFace *pTopDatumFace=pBody->faceList.append();
		pTopDatumFace->poly_norm=ucs.axis_z;
		pTopDatumFace->material=plank_color;
		for(i=0;i<nVertex;i++)
			createFace.NewOutterEdgeLine(pTopDatumFace,((i+1)%nVertex)*2+1,i*2+1);
		//基准面底面
		f3dPolyFace *pBottomDatumFace=pBody->faceList.append();
		pBottomDatumFace->poly_norm=-ucs.axis_z;
		pBottomDatumFace->material=plank_color;
		for(i=nVertex-1;i>=0;i--)
			createFace.NewOutterEdgeLine(pBottomDatumFace,i*2,((i+1)%nVertex)*2);
		//基准面侧面
		pFace;
		for(i=0;i<nVertex;i++)
		{
			pFace=pBody->faceList.append();
			pFace->material=plank_color;
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2,i*2);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2+1);
			createFace.NewOutterEdgeLine(pFace,i*2+1);
			createFace.NewOutterEdgeLine(pFace,i*2);
		}
		if(W>0)
		{
			nVertex*=2;
			//上卷边
			pFace=pBody->faceList.append();
			pFace->material=plank_color;
			for(i=0;i<4;i++)
				createFace.NewOutterEdgeLine(pFace,nVertex+4+(i+1)%4,nVertex+4+i);
			pFace=pBody->faceList.append();
			pFace->material=plank_color;
			createFace.NewOutterEdgeLine(pFace,nVertex+1,nVertex);
			createFace.NewOutterEdgeLine(pFace,nVertex+5);
			createFace.NewOutterEdgeLine(pFace,nVertex+4);
			createFace.NewOutterEdgeLine(pFace,nVertex);
			pFace=pBody->faceList.append();
			pFace->material=plank_color;
			createFace.NewOutterEdgeLine(pFace,nVertex+2,nVertex+1);
			createFace.NewOutterEdgeLine(pFace,nVertex+6);
			createFace.NewOutterEdgeLine(pFace,nVertex+5);
			createFace.NewOutterEdgeLine(pFace,nVertex+1);
			pFace=pBody->faceList.append();
			pFace->material=plank_color;
			createFace.NewOutterEdgeLine(pFace,nVertex+3,nVertex+2);
			createFace.NewOutterEdgeLine(pFace,nVertex+7);
			createFace.NewOutterEdgeLine(pFace,nVertex+6);
			createFace.NewOutterEdgeLine(pFace,nVertex+2);
			pFace=pBody->faceList.append();
			pFace->material=plank_color;
			createFace.NewOutterEdgeLine(pFace,nVertex,nVertex+3);
			createFace.NewOutterEdgeLine(pFace,nVertex+4);
			createFace.NewOutterEdgeLine(pFace,nVertex+7);
			createFace.NewOutterEdgeLine(pFace,nVertex+3);
			//下卷边
			pFace=pBody->faceList.append();
			pFace->material=plank_color;
			for(i=0;i<4;i++)
				createFace.NewOutterEdgeLine(pFace,nVertex+12+(i+1)%4,nVertex+12+i);
			pFace=pBody->faceList.append();
			pFace->material=plank_color;
			createFace.NewOutterEdgeLine(pFace,nVertex+1+8,nVertex+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+5+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+4+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+8);
			pFace=pBody->faceList.append();
			pFace->material=plank_color;
			createFace.NewOutterEdgeLine(pFace,nVertex+2+8,nVertex+1+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+6+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+5+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+1+8);
			pFace=pBody->faceList.append();
			pFace->material=plank_color;
			createFace.NewOutterEdgeLine(pFace,nVertex+3+8,nVertex+2+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+7+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+6+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+2+8);
			pFace=pBody->faceList.append();
			pFace->material=plank_color;
			createFace.NewOutterEdgeLine(pFace,nVertex+8,nVertex+3+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+4+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+7+8);
			createFace.NewOutterEdgeLine(pFace,nVertex+3+8);
		}
		//定义螺栓孔
		CreateLsHole(this,pBody,bDisplayHole,pTopDatumFace,pBottomDatumFace);
	}
	else if(m_iParamType==TYPE_CIRRIBPLATE||m_iParamType==TYPE_CIRCOVERPLATE||m_iParamType==TYPE_TUBERIBPLATE)
	{
		f3dAtomLine* pAtomLine=NULL;
		CLDSLineTube *pDatumTube = (CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
		/*if(pDatumTube==NULL)
		{
		CString sError;
		sError.Format("0X%X参数化板缺少焊接父构件，实体创建失败!",handle);
		AfxMessageBox(sError);
		return FALSE;
		}*/
		double huoqu_cosa=1,huoqu_alfa=0;
		f3dPoint huoqufacenorm=keyvecset.huoqufacenorm;
		f3dPoint huoquaxis_vec=keyvecset.huoquaxis_vec;
		if(!huoqufacenorm.IsZero())
			huoqu_cosa=ucs.axis_z*huoqufacenorm;
		f3dLine huoquline;
		PROFILE_VERTEX *pVertex=NULL;
		if(fabs(huoqu_cosa)<EPS_COS)	//需要制弯环向板
		{
			vector_trans(huoqufacenorm,ucs,FALSE);
			vector_trans(huoquaxis_vec,ucs,FALSE);
			normalize(huoqufacenorm);
			normalize(huoquaxis_vec);
			huoquline.startPt.Set();
			huoquline.endPt=huoquaxis_vec;
			huoqu_alfa=ACOS(huoqu_cosa);
			//
			double offset_dist=0.5;
			f3dPoint face_pick;
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				if(pVertex->vertex.feature==2)
				{
					face_pick=pVertex->vertex;
					break;
				}
			}
			rotate_point_around_axis(face_pick,huoqu_alfa,huoquline);
			if(face_pick.z>0)	//向工作面法向内凹,此时需要重新考虑火曲变形区
				offset_dist=GetThick()*tan(fabs(huoqu_alfa)/2.0)+0.5;
			//计算两火曲面角平分线方向
			f3dPoint sum_norm;
			Add_Pnt(sum_norm,f3dPoint(0,0,1),huoqufacenorm);
			double angle = cal_angle_of_2vec(sum_norm,huoqufacenorm);
			normalize(sum_norm);
			sum_norm = sum_norm*(1.0/cos(angle));
			f3dPoint main_norm(0,0,1);
			//建立连接板底面顶点表
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				f3dPoint vertice=pVertex->vertex;
				if(vertice.feature==1)
				{
					//vertice -= main_norm*thick*0.5;
					vertice.ID=pVertex->vertex.ID;
					pBody->vertex.append(vertice);
				}
				else if(vertice.feature==2)
				{	//将第二面的点旋转到实际位置
					rotate_point_around_axis(vertice,huoqu_alfa,huoquline);
					//vertice -= huoqufacenorm*thick*0.5;
					vertice.ID=pVertex->vertex.ID;
					pBody->vertex.append(vertice);
				}
				//feature大于10的表示火曲线上的点,十位和个位分别表示其所在的两个面
				else if(vertice.feature>10)
				{
					double dd;	//偏移方向判子
					f3dPoint direct,pick;
					direct = cross_prod(main_norm,huoquaxis_vec);
					normalize(direct);
					//前移一位获取前一顶点
					pVertex = vertex_list.GetPrev();
					if(pVertex==NULL)	//第一个顶点就在火曲线上
					{
						pVertex = vertex_list.GetTail();
						vertex_list.GetFirst();
					}
					else
						vertex_list.GetNext();	//返回原位
					pick = pVertex->vertex;
					vertice.ID*=-1;				//当前节点ID设为负值 wht 2009-08-14
					pick.ID=abs(vertice.ID);	//设置插入点的ID
					f3dPoint vertice_vec=vertice-pick;
					normalize(vertice_vec);
					dd = DistOf2dPtLine(pick,huoquline.startPt,huoquline.endPt);
					if(dd<0)//前一点位于火曲线右侧	偏移offset_dist模拟真实变形
						Sub_Pnt(pick,vertice,offset_dist*direct);
					else 
						Add_Pnt(pick,vertice,offset_dist*direct);
					f3dPoint pick_vec=pick-vertice;
					normalize(pick_vec);
					if(pick_vec*vertice_vec<0)
						pick.ID*=-1;
					if(pVertex->vertex.feature==2)
					{
						rotate_point_around_axis(pick,huoqu_alfa,huoquline);
						//pick -= huoqufacenorm*thick*0.5;
					}
					//else 
						//pick -= main_norm*thick*0.5;
					if(pVertex->vertex.feature>10)
						pick.feature = 1;
					else
						pick.feature = pVertex->vertex.feature;
					pBody->vertex.append(pick);
					f3dPoint pt=vertice;
					//pt-=sum_norm*thick*0.5;
					pBody->vertex.append(pt);
					//后移一位获取后一顶点
					pVertex = vertex_list.GetNext();
					if(pVertex==NULL)
					{
						pVertex = vertex_list.GetFirst();
						vertex_list.GetTail();
					}
					else
						vertex_list.GetPrev();	//返回原位
					pick = pVertex->vertex;
					pick.ID=abs(vertice.ID);	//设置插入点的ID
					vertice_vec=pick-vertice;
					normalize(vertice_vec);
					dd = DistOf2dPtLine(pick,huoquline.startPt,huoquline.endPt);
					if(dd<0)//后一点位于火曲线右侧	偏移offset_dist模拟真实变形
						Sub_Pnt(pick,vertice,offset_dist*direct);
					else
						Add_Pnt(pick,vertice,offset_dist*direct);
					pick_vec=pick-vertice;
					normalize(pick_vec);
					if(pick_vec*vertice_vec<0)
						pick.ID*=-1;
					if(pVertex->vertex.feature==2)
					{
						rotate_point_around_axis(pick,huoqu_alfa,huoquline);
					//	pick -= huoqufacenorm*thick*0.5;
					}
					//else 
					//	pick -= main_norm*thick*0.5;
					if(pVertex->vertex.feature>10)
						pick.feature = 1;
					else
						pick.feature = pVertex->vertex.feature;
					pBody->vertex.append(pick);
				}
			}
			//建立连接板底面顶点表
			i=0;
			int n = pBody->vertex.GetNodeNum();
			f3dPoint *pnt_arr = new f3dPoint[n];
			f3dPoint *pPoint=NULL;
			for(pPoint=pBody->vertex.GetFirst();pPoint;pPoint=pBody->vertex.GetNext())
			{
				if(pPoint->feature==1)
					Add_Pnt(pnt_arr[i],*pPoint,main_norm*GetThick());
				else if(pPoint->feature==2)
					Add_Pnt(pnt_arr[i],*pPoint,huoqufacenorm*GetThick());
				else	//feature>10此时点在火曲线上同属于两节点面
					Add_Pnt(pnt_arr[i],*pPoint,sum_norm*GetThick());
				pnt_arr[i].feature = pPoint->feature;
				pnt_arr[i].ID = pPoint->ID;
				i++;
			}
			for(i=0;i<n;i++)
				pBody->vertex.append(pnt_arr[i]);
			delete []pnt_arr;
			//连接板板厚底面创建
			int k;
			f3dPolyFace *pFace,*pUpFace[2],*pDownFace[2];
			f3dPoint *pStart,*pEnd;
			for(k=0;k<2;k++)
			{
				pDownFace[k] = pFace = pBody->faceList.append();
				pFace->material = plank_color;
				for(i=n-1;i>=0;i--)
				{
					f3dAtomLine line;
					line.is_positive_line = FALSE;
					pStart = &pBody->vertex[i];
					if( pStart->feature!=k+1
						&&(pStart->feature%10)!=k+1
						&&(pStart->feature/10)!=k+1)
						continue;
					while(1)
					{
						if(i>0)
						{
							line.is_positive_line = FALSE;
							pEnd = &pBody->vertex[i-1];
							if( pEnd->feature!=k+1
								&&(pEnd->feature%10)!=k+1
								&&(pEnd->feature/10)!=k+1)
							{	//不是当前面顶点,跳过
								i--;
								continue;
							}
							break;
						}
						else
						{
							line.is_positive_line = TRUE;
							if(k==1)	//双面板
							{
								pEnd = pFace->outer_edge[0].pStart;
								break;
							}
							else
							{
								pEnd = &pBody->vertex[n-1];
								break;
							}
						}
					}
					line.pStart = pStart;
					line.pEnd = pEnd;
					pFace->outer_edge.append(line);
				}
			}
			//连接板板厚顶面创建
			for(k=0;k<2;k++)
			{
				pUpFace[k] = pFace = pBody->faceList.append();
				pFace->material = plank_color;
				BOOL bStartEdge3Face = FALSE;
				for(i=n;i<2*n;i++)
				{
					f3dAtomLine line;
					pStart = &pBody->vertex[i];
					if( pStart->feature!=k+1
						&&(pStart->feature%10)!=k+1
						&&(pStart->feature/10)!=k+1)
						continue;//不是当前面,跳过
					while(1)
					{
						if(i<2*n-1)
						{
							line.is_positive_line = TRUE;
							pEnd = &pBody->vertex[i+1];
							if( pEnd->feature!=k+1
								&&(pEnd->feature%10)!=k+1
								&&(pEnd->feature/10)!=k+1)
							{		//不是当前面,跳过
								i++;
								continue;
							}
							break;
						}
						else
						{
							line.is_positive_line = FALSE;
							if(k==1)	//双面板
							{
								pEnd = pFace->outer_edge[0].pStart;
								break;
							}
							else
							{
								pEnd = &pBody->vertex[n];
								break;
							}
						}
					}
					line.pStart = pStart;
					line.pEnd	 = pEnd;
					pFace->outer_edge.append(line);
				}
			}
			//连接板板厚侧面创建
			for(i=0;i<n;i++)
			{
				int k = i+1;
				f3dAtomLine line;
				pFace = pBody->faceList.append();
				pFace->material = plank_color;
				line.pStart = &pBody->vertex[i];
				if(k==n)
				{
					k=0;
					line.is_positive_line = FALSE;
					line.pEnd = &pBody->vertex[k];
				}
				else
				{
					line.is_positive_line = TRUE;
					line.pEnd = &pBody->vertex[i+1];
				}
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[k];
				line.pEnd = &pBody->vertex[k+n];
				line.is_positive_line = TRUE;
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[k+n];
				line.pEnd = &pBody->vertex[i+n];
				if(i+n>k+n)
					line.is_positive_line = TRUE;
				else
					line.is_positive_line = FALSE;
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[i+n];
				line.pEnd   = &pBody->vertex[i];
				line.is_positive_line = FALSE;
				pFace->outer_edge.append(line);
			}
			for(f3dPoint *pPnt=pBody->vertex.GetFirst();pPnt;pPnt=pBody->vertex.GetNext())
			{
				coord_trans(*pPnt,ucs,TRUE);
				*pPnt+=(m_fNormOffset-0.5*thick)*ucs.axis_z;
			}
			//定义螺栓孔
			if(m_bDisplayHole || bDisplayHole)
			{
				UCS_STRU hole_ucs;
				CHoleMaker holeMaker;
				CXhSimpleList<HOLE> listWaterleakHoles;
				AddMiscellaneousHoles(&listWaterleakHoles);
				for(HOLE* pWaterHole=listWaterleakHoles.EnumObjectFirst();pWaterHole;pWaterHole=listWaterleakHoles.EnumObjectNext())
				{
					hole_ucs.origin=pWaterHole->xMcsPosition;
					COLORREF crLsHole=CLDSPart::GetColor(CLS_BOLT,ftoi(pWaterHole->fHoleD),' ');
					if(fabs(hole_ucs.axis_z*ucs.axis_z)>fabs(hole_ucs.axis_z*keyvecset.huoqufacenorm))
					{
						hole_ucs.axis_z=ucs.axis_z;
						hole_ucs.axis_x=btc::CalLocalCS_X_AXIS(hole_ucs.axis_z);
						holeMaker.Create(hole_ucs,pWaterHole->fHoleD,pWaterHole->wWaistLen,pUpFace[0],pDownFace[0],pBody,crLsHole);
					}
					else
					{
						hole_ucs.axis_z=keyvecset.huoqufacenorm;
						hole_ucs.axis_x=btc::CalLocalCS_X_AXIS(hole_ucs.axis_z);
						holeMaker.Create(hole_ucs,pWaterHole->fHoleD,pWaterHole->wWaistLen,pUpFace[1],pDownFace[1],pBody,crLsHole);
					}
				}
			}
		}
		else
		{
			int nVertex = vertex_list.GetNodeNum();  
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				vertice = pVertex->vertex;
				vertice.z = -thick*0.5;
				pBody->vertex.append(vertice);
				vertice.z = thick*0.5;
				pBody->vertex.append(vertice);
			}
			//初始化链表
			CCreateFace createFace;
			createFace.InitVertexList(pBody);
			//上层多边形面
			f3dPolyFace* pTopFace=pBody->faceList.append();
			pTopFace->material = plank_color;
			pTopFace->poly_norm=ucs.axis_z;
			for(i=0;i<nVertex;i++)
			{
				//pAtomLine=pBody->NewEdgeLine(pTopFace,((i+1)%nVertex)*2+1,i*2+1);
				createFace.NewOutterEdgeLine(pTopFace,((i+1)%nVertex)*2+1,i*2+1);
				//if(vertex_list[i].type>0)
				//	CreateArcLine(pAtomLine,ucs.axis_z,vertex_list[i]);
			}
			//下层多边形面
			f3dPolyFace *pBottomFace=pBody->faceList.append();
			pBottomFace->material = plank_color;
			pBottomFace->poly_norm=-ucs.axis_z;
			for(i=nVertex-1;i>=0;i--)
			{
				//pAtomLine=pBody->NewEdgeLine(pBottomFace,((i+nVertex-1)%nVertex)*2,i*2);
				createFace.NewOutterEdgeLine(pBottomFace,((i+nVertex-1)%nVertex)*2,i*2);
				//if(vertex_list[(i+nVertex-1)%nVertex].type>0)
				//	CreateArcLine(pAtomLine,-ucs.axis_z,vertex_list[(i+nVertex-1)%nVertex]);
			}
			//各矩形侧面
			for(i=0; i<nVertex; i++)
			{
				pFace=pBody->faceList.append();
				pFace->material = plank_color;
				//pAtomLine=pBody->NewEdgeLine(pFace,((i+1)%nVertex)*2,i*2);
				createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2,i*2);
				//if(vertex_list[i].type>0)
				//	CreateArcLine(pAtomLine,ucs.axis_z,vertex_list[i]);
				//pAtomLine=pBody->NewEdgeLine(pFace,((i+1)%nVertex)*2+1);
				createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2+1);
				//pAtomLine=pBody->NewEdgeLine(pFace,i*2+1);
				createFace.NewOutterEdgeLine(pFace,i*2+1);
				//if(vertex_list[i].type>0)
				//	CreateArcLine(pAtomLine,-ucs.axis_z,vertex_list[i]);
				//pAtomLine=pBody->NewEdgeLine(pFace,i*2);
				createFace.NewOutterEdgeLine(pFace,i*2);
			}
			//坐标系由相对坐标转换到结构体的绝对坐标下
			for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
			{
				if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
					return FALSE;
			}
			//显示环向板内圆 wht 11-05-27
			double inner_r=0;	//内圆半径
			f3dPoint col_axis;	//圆柱法线方向
			if(GetInnerCirRadius(inner_r,col_axis))
			{	//暂时均按圆显示实体,出图时按椭圆处理
				CHoleMaker holeMaker;
				int slices=CalArcResolution(inner_r,2*Pi,scale_of_user2scr,sample_len);
				holeMaker.Create(ucs,2*inner_r,0,pTopFace,pBottomFace,pBody,plank_color,slices);
			}
			//定义螺栓孔
			CreateLsHole(this,pBody,bDisplayHole,pTopFace,pBottomFace);
			if(m_bDisplayHole||bDisplayHole)
			{
				UCS_STRU hole_ucs;
				CHoleMaker holeMaker;
				CXhSimpleList<HOLE> listWaterleakHoles;
				AddMiscellaneousHoles(&listWaterleakHoles);
				for(HOLE* pWaterHole=listWaterleakHoles.EnumObjectFirst();pWaterHole;pWaterHole=listWaterleakHoles.EnumObjectNext())
				{
					hole_ucs.origin=pWaterHole->xMcsPosition;
					hole_ucs.axis_z=pWaterHole->vHoleAxisVec;
					hole_ucs.axis_x=btc::CalLocalCS_X_AXIS(hole_ucs.axis_z);
					COLORREF crLsHole=CLDSPart::GetColor(CLS_BOLT,ftoi(pWaterHole->fHoleD),' ');
					holeMaker.Create(hole_ucs,pWaterHole->fHoleD,0,pTopFace,pBottomFace,pBody,crLsHole);
				}
			}
		}
	}
	else if(m_iParamType==TYPE_RIBPLATE)
	{	//肋板
		double huoqu_cosa=1;
		f3dPoint huoquFaceNorm=keyvecset.huoqufacenorm;
		f3dPoint huoquAxisVec=keyvecset.huoquaxis_vec;
		if(!huoquFaceNorm.IsZero())
			huoqu_cosa=ucs.axis_z*huoquFaceNorm;
		if(fabs(huoqu_cosa)<EPS_COS)
		{	//双面肋板
			vector_trans(huoquFaceNorm,ucs,FALSE);
			normalize(huoquFaceNorm);
			double huoqu_angle=0;
			f3dPoint face_pick,huoqu_start,huoqu_end;
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				if(pVertex->vertex.feature==2)
					face_pick=pVertex->vertex;
				if(pVertex->vertex.feature==12)
				{
					if(huoqu_start.IsZero())
						huoqu_start=pVertex->vertex;
					else
						huoqu_end=pVertex->vertex;
				}
				if(!face_pick.IsZero()&&!huoqu_end.IsZero())
					break;
			}
			if(!huoqu_start.IsZero()&&!huoqu_end.IsZero())
			{
				double justify=DistOf2dPtLine(face_pick,huoqu_start,huoqu_end);
				if(justify>0)
				{
					f3dPoint point=huoqu_start;
					huoqu_start=huoqu_end;
					huoqu_end=point;
				}
				huoquAxisVec=huoqu_end-huoqu_start;
				huoquAxisVec.z=0;
				normalize(huoquAxisVec);
				GECS huoqucs;
				huoqucs.axis_x.Set(huoquAxisVec.y,-huoquAxisVec.x);
				huoqucs.axis_z=huoquAxisVec;
				huoqucs.axis_y.Set(0,0,-1);
				GEPOINT extend_vec=huoqucs.TransVToCS(huoquFaceNorm);
				RotateVectorAroundVector(extend_vec,1,0,GEPOINT(0,0,1));
				huoqu_angle=Cal2dLineAng(0,0,extend_vec.x,extend_vec.y);
				if(huoqu_angle>Pi)
					huoqu_angle-=2*Pi;
			}
			//计算火曲变形偏移距
			double offset_dist=0.5;
			f3dLine huoquLine(huoqu_start,huoqu_end);
			rotate_point_around_axis(face_pick,huoqu_angle,huoquLine);
			if(face_pick.z>0)	//向工作面法向内凹,此时需要重新考虑火曲变形区
				offset_dist=GetThick()*tan(fabs(huoqu_angle)/2.0)+0.5;
			//计算两火曲面角平分线方向
			f3dPoint sum_norm,main_norm(0,0,1);
			Add_Pnt(sum_norm,main_norm,huoquFaceNorm);
			double angle=cal_angle_of_2vec(sum_norm,huoquFaceNorm);
			normalize(sum_norm);
			sum_norm = sum_norm*(1.0/cos(angle));
			//建立连接板底面顶点表
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				f3dPoint vertice=pVertex->vertex;
				if(vertice.feature==1)
				{	//基准面的点直接插入实体模型
					vertice.ID=pVertex->vertex.ID;
					pBody->vertex.append(vertice);
				}
				else if(vertice.feature==2)
				{	//将第二面的点旋转到实际位置
					rotate_point_around_axis(vertice,huoqu_angle,huoquLine);
					vertice.ID=pVertex->vertex.ID;
					pBody->vertex.append(vertice);
				}
				else if(vertice.feature>10)
				{	//火曲线上的点(变形过度需要三个点)
					f3dPoint direct=cross_prod(main_norm,huoquAxisVec);
					normalize(direct);
					for(int i=0;i<3;i++)
					{
						if(i==1)
						{
							pBody->vertex.append(vertice);
							continue;
						}
						else if(i==0)
						{
							pVertex = vertex_list.GetPrev();
							if(pVertex==NULL)	//第一个顶点就在火曲线上
							{
								pVertex = vertex_list.GetTail();
								vertex_list.GetFirst();
							}
							else
								vertex_list.GetNext();	//返回原位
						}
						else //if(i==2)
						{
							pVertex = vertex_list.GetNext();
							if(pVertex==NULL)
							{
								pVertex = vertex_list.GetFirst();
								vertex_list.GetTail();
							}
							else
								vertex_list.GetPrev();	//返回原位
						}
						//计算变形过度点坐标
						f3dPoint pick,vertice_vec,pick_vec;
						pick=pVertex->vertex;
						pick.ID=abs(vertice.ID);
						vertice_vec=pick-vertice;
						normalize(vertice_vec);
						double dd=DistOf2dPtLine(pick,huoquLine.startPt,huoquLine.endPt);
						if(dd<0)//后一点位于火曲线右侧	偏移offset_dist模拟真实变形
							Sub_Pnt(pick,vertice,offset_dist*direct);
						else
							Add_Pnt(pick,vertice,offset_dist*direct);
						pick_vec=pick-vertice;
						normalize(pick_vec);
						if(pick_vec*vertice_vec<0)
							pick.ID*=-1;
						if(pVertex->vertex.feature==2)
							rotate_point_around_axis(pick,huoqu_angle,huoquLine);
						pBody->vertex.append(pick);
					}	
				}
			}
			//向实体模型中填充点
			i=0;
			int n=pBody->vertex.GetNodeNum();
			f3dPoint *pnt_arr = new f3dPoint[n];
			f3dPoint *pPoint=NULL;
			for(pPoint=pBody->vertex.GetFirst();pPoint;pPoint=pBody->vertex.GetNext())
			{
				if(pPoint->feature==1)
					Add_Pnt(pnt_arr[i],*pPoint,main_norm*GetThick());
				else if(pPoint->feature==2)
					Add_Pnt(pnt_arr[i],*pPoint,huoquFaceNorm*GetThick());
				else
					Add_Pnt(pnt_arr[i],*pPoint,sum_norm*GetThick());
				pnt_arr[i].feature = pPoint->feature;
				pnt_arr[i].ID = pPoint->ID;
				i++;
			}
			for(i=0;i<n;i++)
				pBody->vertex.append(pnt_arr[i]);
			delete []pnt_arr;
			//连接板板厚底面创建
			int k;
			f3dPolyFace *pFace,*pUpFace[2],*pDownFace[2];
			f3dPoint *pStart,*pEnd;
			for(k=0;k<2;k++)
			{
				pDownFace[k] = pFace = pBody->faceList.append();
				pFace->material = plank_color;
				for(i=n-1;i>=0;i--)
				{
					f3dAtomLine line;
					line.is_positive_line = FALSE;
					pStart = &pBody->vertex[i];
					if( pStart->feature!=k+1&&(pStart->feature%10)!=k+1&&(pStart->feature/10)!=k+1)
						continue;
					while(1)
					{
						if(i>0)
						{
							line.is_positive_line = FALSE;
							pEnd = &pBody->vertex[i-1];
							if(pEnd->feature!=k+1&&(pEnd->feature%10)!=k+1&&(pEnd->feature/10)!=k+1)
							{	//不是当前面顶点,跳过
								i--;
								continue;
							}
							break;
						}
						else
						{
							line.is_positive_line = TRUE;
							if(k==1)	//双面板
							{
								pEnd = pFace->outer_edge[0].pStart;
								break;
							}
							else
							{
								pEnd = &pBody->vertex[n-1];
								break;
							}
						}
					}
					line.pStart = pStart;
					line.pEnd = pEnd;
					pFace->outer_edge.append(line);
				}
			}
			//连接板板厚顶面创建
			for(k=0;k<2;k++)
			{
				pUpFace[k] = pFace = pBody->faceList.append();
				pFace->material = plank_color;
				BOOL bStartEdge3Face = FALSE;
				for(i=n;i<2*n;i++)
				{
					f3dAtomLine line;
					pStart = &pBody->vertex[i];
					if( pStart->feature!=k+1&&(pStart->feature%10)!=k+1&&(pStart->feature/10)!=k+1)
						continue;//不是当前面,跳过
					while(1)
					{
						if(i<2*n-1)
						{
							line.is_positive_line = TRUE;
							pEnd = &pBody->vertex[i+1];
							if( pEnd->feature!=k+1&&(pEnd->feature%10)!=k+1&&(pEnd->feature/10)!=k+1)
							{		//不是当前面,跳过
								i++;
								continue;
							}
							break;
						}
						else
						{
							line.is_positive_line = FALSE;
							if(k==1)	//双面板
							{
								pEnd = pFace->outer_edge[0].pStart;
								break;
							}
							else
							{
								pEnd = &pBody->vertex[n];
								break;
							}
						}
					}
					line.pStart = pStart;
					line.pEnd	 = pEnd;
					pFace->outer_edge.append(line);
				}
			}
			//连接板板厚侧面创建
			for(i=0;i<n;i++)
			{
				pFace=pBody->faceList.append();
				pFace->material = plank_color;
				int k = i+1;
				f3dAtomLine line;
				pFace = pBody->faceList.append();
				pFace->material = plank_color;
				//
				line.pStart = &pBody->vertex[i];
				if(k==n)
				{
					k=0;
					line.is_positive_line = FALSE;
					line.pEnd = &pBody->vertex[k];
				}
				else
				{
					line.is_positive_line = TRUE;
					line.pEnd = &pBody->vertex[i+1];
				}
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[k];
				line.pEnd = &pBody->vertex[k+n];
				line.is_positive_line = TRUE;
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[k+n];
				line.pEnd = &pBody->vertex[i+n];
				if(i+n>k+n)
					line.is_positive_line = TRUE;
				else
					line.is_positive_line = FALSE;
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[i+n];
				line.pEnd   = &pBody->vertex[i];
				line.is_positive_line = FALSE;
				pFace->outer_edge.append(line);
			}
			for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
			{
				if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
					return FALSE;
			}
			//定义螺栓孔
			if(m_bDisplayHole || bDisplayHole)
			{
				CHoleMaker holeMaker;
				UCS_STRU hole_ucs;
				for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
				{
					hole_ucs.origin=(*pLsRef)->ucs.origin;
					hole_ucs.axis_z=(*pLsRef)->get_norm();
					hole_ucs.axis_x=(*pLsRef)->ucs.axis_x;
					if(hole_ucs.axis_x.IsZero())
						hole_ucs.axis_x=btc::CalLocalCS_X_AXIS(hole_ucs.axis_z);
					COLORREF crLsHole=CLDSPart::GetColor(CLS_BOLT,(*pLsRef)->get_d(),' ');
					double hole_d=(*pLsRef)->get_d()+(*pLsRef)->hole_d_increment;
					if(fabs(hole_ucs.axis_z*ucs.axis_z)>fabs(hole_ucs.axis_z*huoquFaceNorm))
						holeMaker.Create(hole_ucs,hole_d,pLsRef->waistLen,pUpFace[0],pDownFace[0],pBody,crLsHole);
					else 
						holeMaker.Create(hole_ucs,hole_d,pLsRef->waistLen,pUpFace[1],pDownFace[1],pBody,crLsHole);
				}
			}
		}
		else	//单面肋板
		{
			int nVertex = vertex_list.GetNodeNum();  
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				vertice = pVertex->vertex;
				vertice.z = -thick*0.5;
				pBody->vertex.append(vertice);
				vertice.z = thick*0.5;
				pBody->vertex.append(vertice);
			}
			CCreateFace createFace;
			createFace.InitVertexList(pBody);
			//顶面
			f3dPolyFace *pTopFace=pBody->faceList.append();
			pTopFace->material = plank_color;
			for(i=0;i<nVertex;i++)
			{
				if(i==0)
					createFace.NewOutterEdgeLine(pTopFace,((i+1)%nVertex)*2+1,1);
				else
					createFace.NewOutterEdgeLine(pTopFace,((i+1)%nVertex)*2+1);
			}
			//底面
			f3dPolyFace *pBtmFace=pBody->faceList.append();
			pBtmFace->material = plank_color;
			for(i=0;i<nVertex;i++)
			{
				if(i==0)
					createFace.NewOutterEdgeLine(pBtmFace,(nVertex-1)*2,0);
				else
					createFace.NewOutterEdgeLine(pBtmFace,(nVertex-1-i)*2);
			}
			//侧面
			for(int i=0; i<nVertex; i++)
			{
				pFace=pBody->faceList.append();
				pFace->material = plank_color;
				createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2,i*2);
				createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2+1);
				createFace.NewOutterEdgeLine(pFace,i*2+1);
				createFace.NewOutterEdgeLine(pFace,i*2);
			}

			//坐标系由相对坐标转换到结构体的绝对坐标下
			for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
			{
				if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
					return FALSE;
			}
			//定义螺栓孔
			CreateLsHole(this,pBody,bDisplayHole,pTopFace,pBtmFace);
			double H=0,G=0,F=0;
			GetDesignItemValue(KEY2C("MH"),&H);
			if(H>0)// && (m_bDisplayHole || bDisplayHole))	
			{
				GetDesignItemValue(KEY2C("MG"),&G);
				GetDesignItemValue(KEY2C("MF"),&F);
				//
				f3dPoint along_axisvec;
				long hDatumPart=0,hAssisPlate1=0;
				GetDesignItemValue('A',&hDatumPart);			//基准构件
				GetDesignItemValue(KEY2C("C"),&hAssisPlate1);	//辅助钢板1
				CLDSPart* pDatumPart=BelongModel()->FromPartHandle(abs(hDatumPart));
				CLDSPart* pAssistantPart1=BelongModel()->FromPartHandle(abs(hAssisPlate1));
				if(pDatumPart&&pAssistantPart1)
				{	//角肋板上的通孔绘制
					along_axisvec=pAssistantPart1->ucs.axis_z^ucs.axis_z;	//沿1号辅板的角焊缝走向
					if(along_axisvec*ucs.axis_y<0)
						along_axisvec*=-1.0;
					vector_trans(along_axisvec,ucs,FALSE);
					normalize(along_axisvec);
					double k=0;
					if(along_axisvec.y>eps)
						k=along_axisvec.x/along_axisvec.y;
					CHoleMaker holeMaker;
					UCS_STRU hole_ucs;
					hole_ucs.origin.Set(G+F*k,F);
					coord_trans(hole_ucs.origin,ucs,TRUE);
					hole_ucs.axis_z=ucs.axis_z;
					hole_ucs.axis_x=inters_vec(ucs.axis_z);
					hole_ucs.axis_y=hole_ucs.axis_z^hole_ucs.axis_x;
					hole_ucs.axis_x=hole_ucs.axis_y^hole_ucs.axis_z;
					holeMaker.Create(hole_ucs,H,0,pTopFace,pBtmFace,pBody,GetColor(CLS_BOLT,0,' '));
				}
			}
		}
	}
	else if(m_iParamType==TYPE_BASEPLATE ||m_iParamType==TYPE_SHOEPLATE)
	{
		double T=thick;
		int nVertex = vertex_list.GetNodeNum();  
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			vertice = pVertex->vertex;
			pBody->vertex.append(vertice);
			vertice.z = T;
			pBody->vertex.append(vertice);
		}
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//顶面
		f3dPolyFace *pTopFace=pBody->faceList.append();
		pTopFace->material = plank_color;
		for(i=0;i<nVertex;i++)
		{
			if(i==0)
				createFace.NewOutterEdgeLine(pTopFace,((i+1)%nVertex)*2+1,1);
			else
				createFace.NewOutterEdgeLine(pTopFace,((i+1)%nVertex)*2+1);
		}
		//底面
		f3dPolyFace *pBtmFace=pBody->faceList.append();
		pBtmFace->material = plank_color;
		for(i=0;i<nVertex;i++)
		{
			if(i==0)
				createFace.NewOutterEdgeLine(pBtmFace,(nVertex-1)*2,0);
			else
				createFace.NewOutterEdgeLine(pBtmFace,(nVertex-1-i)*2);
		}
		//侧面
		for(int i=0; i<nVertex; i++)
		{
			pFace=pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2,i*2);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2+1);
			createFace.NewOutterEdgeLine(pFace,i*2+1);
			createFace.NewOutterEdgeLine(pFace,i*2);
		}
		//坐标系由相对坐标转换到结构体的绝对坐标下
		for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		{
			if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
				return FALSE;
		}
		//定义螺栓孔
		CreateLsHole(this,pBody,bDisplayHole,pTopFace,pBtmFace);
	}
	else if (m_iParamType == TYPE_PEDALPLATE)
	{
		long A = 0;
		GetDesignItemValue('A', &A);
		CLDSLineAngle* pDatumJg = (CLDSLineAngle*)BelongModel()->FromPartHandle(A, CLS_LINEANGLE);
		if (pDatumJg == NULL)
			return FALSE;
		//绘制正面
		double T = thick;
		int nVertex = vertex_list.GetNodeNum();
		for (pVertex = vertex_list.GetFirst(); pVertex; pVertex = vertex_list.GetNext())
		{
			vertice = pVertex->vertex;
			pBody->vertex.append(vertice);
			vertice.z = T;
			pBody->vertex.append(vertice);
		}
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//顶面
		f3dPolyFace *pTopFace = pBody->faceList.append();
		pTopFace->material = plank_color;
		for (int i = 0; i < nVertex; i++)
			createFace.NewOutterEdgeLine(pTopFace, ((i + 1) % nVertex) * 2 + 1, i * 2 + 1);
		//底面
		f3dPolyFace *pBtmFace = pBody->faceList.append();
		pBtmFace->material = plank_color;
		for (int i = nVertex - 1; i >= 0; i--)
			createFace.NewOutterEdgeLine(pBtmFace, ((nVertex + i - 1) % nVertex) * 2, i * 2);
		//侧面
		for (int i = 0; i < nVertex; i++)
		{
			pFace = pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace, ((i + 1) % nVertex) * 2, i * 2);
			createFace.NewOutterEdgeLine(pFace, ((i + 1) % nVertex) * 2 + 1);
			createFace.NewOutterEdgeLine(pFace, i * 2 + 1);
			createFace.NewOutterEdgeLine(pFace, i * 2);
		}
		//绘制踏板卷边面
		long V = 0;
		GetDesignItemValue('V', &V);
		f3dPoint spreadVec, linePtS, linePtE;
		if (fabs(ucs.axis_z*pDatumJg->vxWingNorm) > fabs(ucs.axis_z*pDatumJg->vyWingNorm))
			spreadVec = pDatumJg->GetWingVecX();
		else
			spreadVec = pDatumJg->GetWingVecY();
		spreadVec *= (V == 0) ? 1 : -1;
		vector_trans(spreadVec, ucs, FALSE);
		for (pVertex = vertex_list.GetFirst(); pVertex; pVertex = vertex_list.GetNext())
		{
			if (pVertex->vertex.feature == PROFILE_VERTEX::DT_BEND_DATUM_POINT)
				linePtS = pVertex->vertex;
			else if (pVertex->vertex.feature == PROFILE_VERTEX::DT_BEND_EXTEND_POINT)
				linePtE = pVertex->vertex;
		}
		double W = 0, H = 0;
		GetDesignItemValue('W', &W);
		GetDesignItemValue('H', &H);
		f3dPoint face_norm, offset_norm, line_vec;
		for (int ii = 0; ii < 2; ii++)
		{
			f3dLine roll_line;
			double roll_len = 0;
			if (ii == 0)
			{	//踏板面
				roll_len = W;
				if (spreadVec*f3dPoint(1, 0, 0) < 0)
				{
					roll_line.startPt = linePtS;
					roll_line.endPt = linePtE;
				}
				else
				{
					roll_line.startPt = linePtE;
					roll_line.endPt = linePtS;
				}
				line_vec = (roll_line.endPt - roll_line.startPt).normalized();
				face_norm.Set(0, 0, 1);
				offset_norm = face_norm ^ line_vec;
				normalize(offset_norm);
				if (offset_norm*f3dPoint(0, 1, 0) < 0)
					offset_norm *= -1;	//向内卷边
			}
			else
			{	//挡板面
				roll_len = H;
				if (spreadVec*f3dPoint(1, 0, 0) < 0)
				{
					roll_line.startPt = linePtE;
					roll_line.endPt = linePtE + f3dPoint(0, 0, 1) * W;
				}
				else
				{
					roll_line.startPt = linePtE + f3dPoint(0, 0, 1) * W;
					roll_line.endPt = linePtE;
				}
				line_vec = (roll_line.endPt - roll_line.startPt).normalized();
				face_norm = offset_norm;
				offset_norm = face_norm ^ line_vec;
				normalize(offset_norm);
				if (offset_norm*spreadVec < 0)
					offset_norm *= -1;
			}
			f3dPoint cube_vert_arr[4];
			cube_vert_arr[0] = roll_line.startPt;
			cube_vert_arr[1] = roll_line.endPt;
			cube_vert_arr[2] = roll_line.endPt + face_norm * roll_len;
			cube_vert_arr[3] = roll_line.startPt + face_norm * roll_len;
			int nIndex = pBody->vertex.GetNodeNum();
			for (int i = 0; i < 4; i++)
			{
				pBody->vertex.append(cube_vert_arr[i]);
				pBody->vertex.append(cube_vert_arr[i] + offset_norm * thick);
			}
			createFace.InitVertexList(pBody);
			pFace = pBody->faceList.append();
			pFace->material = plank_color;
			for (int i = 0; i < 4; i++)
				createFace.NewOutterEdgeLine(pFace, nIndex + ((i + 1) % 4) * 2 + 1, nIndex + i * 2 + 1);
			pFace = pBody->faceList.append();
			pFace->material = plank_color;
			for (int i = 3; i >= 0; i--)
				createFace.NewOutterEdgeLine(pFace, nIndex + ((3 + i) % 4) * 2, nIndex + i * 2);
			for (int i = 0; i < 4; i++)
			{
				pFace = pBody->faceList.append();
				pFace->material = plank_color;
				createFace.NewOutterEdgeLine(pFace, nIndex + ((i + 1) % 4) * 2, nIndex + i * 2);
				createFace.NewOutterEdgeLine(pFace, nIndex + ((i + 1) % 4) * 2 + 1);
				createFace.NewOutterEdgeLine(pFace, nIndex + i * 2 + 1);
				createFace.NewOutterEdgeLine(pFace, nIndex + i * 2);
			}
		}
		//坐标系由相对坐标转换到结构体的绝对坐标下
		for (f3dPoint *pVertice = pBody->vertex.GetFirst(); pVertice; pVertice = pBody->vertex.GetNext())
		{
			if (coord_trans(*pVertice, ucs, TRUE) != TRUE)
				return FALSE;
		}
		//定义螺栓孔
		CreateLsHole(this, pBody, bDisplayHole, pTopFace, pBtmFace);
	}
	else if(m_iParamType==TYPE_STATIONCONNPLATE)
	{
		double T=thick;
		int nVertex = vertex_list.GetNodeNum();  
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			vertice = pVertex->vertex;
			pBody->vertex.append(vertice);
			vertice.z = T;
			pBody->vertex.append(vertice);
		}
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//绘制矩形立体面
		CreateRectSolidFaces(createFace,pBody);
		int i=0;
		f3dPolyFace *pTopFace=NULL,*pBtmFace=NULL;
		for(f3dPolyFace* pFace=pBody->faceList.GetFirst();pFace;pFace=pBody->faceList.GetNext(),i++)
		{
			pFace->material = plank_color;
			if(i==0)
				pTopFace=pFace;
			if(i==1)
				pBtmFace=pFace;
		}
		//坐标系由相对坐标转换到结构体的绝对坐标下
		for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		{
			if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
				return FALSE;
		}
		//绘制端板内圆环
		double inner_r=0,H=0;
		GetDesignItemValue('r',&inner_r);
		GetDesignItemValue('H',&H);
		UCS_STRU hole_ucs;
		hole_ucs.origin.Set(0,H,0);
		coord_trans(hole_ucs.origin,ucs,TRUE);
		hole_ucs.axis_z=ucs.axis_z;
		hole_ucs.axis_x=inters_vec(ucs.axis_z);
		hole_ucs.axis_y=hole_ucs.axis_z^hole_ucs.axis_x;
		hole_ucs.axis_x=hole_ucs.axis_y^hole_ucs.axis_z;
		int slices=CalArcResolution(inner_r,2*Pi,scale_of_user2scr,sample_len);
		CHoleMaker holeMaker;
		holeMaker.Create(hole_ucs,2*inner_r,0,pTopFace,pBtmFace,pBody,plank_color,slices);
	}
	else if(m_iParamType==TYPE_STATIONSHOEPLATE)
	{
		double T=thick;
		int nVertex = vertex_list.GetNodeNum();  
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			vertice = pVertex->vertex;
			vertice.z =-0.5*T;
			pBody->vertex.append(vertice);
			vertice.z = 0.5*T;
			pBody->vertex.append(vertice);
		}
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//绘制矩形立体面
		CreateRectSolidFaces(createFace,pBody);
		int i=0;
		f3dPolyFace *pTopFace=NULL,*pBtmFace=NULL;
		for(f3dPolyFace* pFace=pBody->faceList.GetFirst();pFace;pFace=pBody->faceList.GetNext(),i++)
		{
			pFace->material = plank_color;
			if(i==0)
				pTopFace=pFace;
			if(i==1)
				pBtmFace=pFace;
		}
		//坐标系由相对坐标转换到结构体的绝对坐标下
		for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		{
			if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
				return FALSE;
		}
		//绘制端板内圆环
		long hEndPlate=0;
		GetDesignItemValue(KEY2C("LE"),&hEndPlate);
		CLDSParamPlate* pEndPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(hEndPlate,CLS_PARAMPLATE);
		if(pEndPlate&&pEndPlate->m_iParamType==TYPE_STATIONCONNPLATE)	
		{
			double inner_r=0,H=0;
			pEndPlate->GetDesignItemValue('r',&inner_r);
			pEndPlate->GetDesignItemValue('H',&H);
			UCS_STRU hole_ucs;
			hole_ucs.origin.Set(0,H,0);
			coord_trans(hole_ucs.origin,pEndPlate->ucs,TRUE);
			hole_ucs.axis_z=ucs.axis_z;
			hole_ucs.axis_x=inters_vec(ucs.axis_z);
			hole_ucs.axis_y=hole_ucs.axis_z^hole_ucs.axis_x;
			hole_ucs.axis_x=hole_ucs.axis_y^hole_ucs.axis_z;
			int slices=CalArcResolution(inner_r,2*Pi,scale_of_user2scr,sample_len);
			CHoleMaker holeMaker;
			holeMaker.Create(hole_ucs,2*inner_r,0,pTopFace,pBtmFace,pBody,plank_color,slices);
		}
	}
	else if(m_iParamType==TYPE_STATIONCROSSPLATE||m_iParamType==TYPE_STATIONLINKPLATE||m_iParamType==TYPE_STATIONNODEPLATE
				||m_iParamType==TYPE_WALKWAYCIRPLATE)
	{
		double T=thick;
		int nVertex = vertex_list.GetNodeNum();  
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			vertice = pVertex->vertex;
			vertice.z =-0.5*T;
			pBody->vertex.append(vertice);
			vertice.z = 0.5*T;
			pBody->vertex.append(vertice);
		}
		//绘制矩形立体面
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		CreateRectSolidFaces(createFace,pBody);
		for(f3dPolyFace* pFace=pBody->faceList.GetFirst();pFace;pFace=pBody->faceList.GetNext(),i++)
			pFace->material = plank_color;
		//坐标系由相对坐标转换到结构体的绝对坐标下
		for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		{
			if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
				return FALSE;
		}
	}
	else if(m_iParamType==TYPE_ELBOWPLATE)
	{
		double T=thick;
		int nVertex = vertex_list.GetNodeNum();  
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			vertice = pVertex->vertex;
			vertice.z = -T*0.5;
			pBody->vertex.append(vertice);
			vertice.z = T*0.5;
			pBody->vertex.append(vertice);
		}
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//顶面
		f3dPolyFace *pTopFace=pBody->faceList.append();
		pTopFace->material = plank_color;
		for(i=0;i<nVertex;i++)
		{
			if(i==0)
				createFace.NewOutterEdgeLine(pTopFace,((i+1)%nVertex)*2+1,1);
			else
				createFace.NewOutterEdgeLine(pTopFace,((i+1)%nVertex)*2+1);
		}
		//底面
		f3dPolyFace *pBtmFace=pBody->faceList.append();
		pBtmFace->material = plank_color;
		for(i=0;i<nVertex;i++)
		{
			if(i==0)
				createFace.NewOutterEdgeLine(pBtmFace,(nVertex-1)*2,0);
			else
				createFace.NewOutterEdgeLine(pBtmFace,(nVertex-1-i)*2);
		}
		//侧面
		for(int i=0; i<nVertex; i++)
		{
			pFace=pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2,i*2);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2+1);
			createFace.NewOutterEdgeLine(pFace,i*2+1);
			createFace.NewOutterEdgeLine(pFace,i*2);
		}
		
		//坐标系由相对坐标转换到结构体的绝对坐标下
		for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		{
			if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
				return FALSE;
		}
		//定义孔
		long L=0,K=0,V=0;
		double H,G=0,F=0;
		GetDesignItemValue('H',&H);
		if(H>0)	//肘板带孔
		{
			GetDesignItemValue('G',&G);
			GetDesignItemValue('F',&F);
			if(!GetDesignItemValue('V',&V))
				V=0;	//是否为竖直带自动正位的肋板
			f3dPoint line_vec;
			CLDSPart *pPlate=NULL;
			CLDSLineTube *pDatumTube=NULL;
			if(GetDesignItemValue('L',&L))	//获得基准底板
				pPlate=BelongModel()->FromPartHandle(L,CLS_PLATE,CLS_PARAMPLATE);
			if(GetDesignItemValue('K',&K))	//基准钢管
			{
				pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(abs(K),CLS_LINETUBE);
				if(pDatumTube&&K>0)
					line_vec=pDatumTube->End()-pDatumTube->Start();
				else if(pDatumTube&&K<0)
					line_vec=pDatumTube->Start()-pDatumTube->End();
			}
			else
			{
				GetDesignItemValue('x', &line_vec.x);
				GetDesignItemValue('y', &line_vec.y);
				GetDesignItemValue('z', &line_vec.z);
			}
			normalize(line_vec);
			vector_trans(line_vec,ucs,FALSE);
			line_vec.z=0;
			if(m_bDisplayHole || bDisplayHole)
			{
				CHoleMaker holeMaker;
				UCS_STRU hole_ucs;
				if(V&&pPlate&&pDatumTube)
				{
					f3dPoint vertex=vertex_list.GetTail()->vertex;
					line_vec.Set(vertex.x,vertex.y,0);
					normalize(line_vec);
				}
				double k=0;
				if(line_vec.y>eps)//防止line_vec.y=0 
					k=line_vec.x/line_vec.y;
				hole_ucs.origin.Set(G+F*k,F);
				coord_trans(hole_ucs.origin,ucs,TRUE);
				hole_ucs.axis_z=ucs.axis_z;
				hole_ucs.axis_x=inters_vec(ucs.axis_z);
				hole_ucs.axis_y=hole_ucs.axis_z^hole_ucs.axis_x;
				hole_ucs.axis_x=hole_ucs.axis_y^hole_ucs.axis_z;
				holeMaker.Create(hole_ucs,H,0,pTopFace,pBtmFace,pBody,GetColor(CLS_BOLT,0,' '));
			}
		}
	}
	else if(m_iParamType==TYPE_VERTPLATE||m_iParamType==TYPE_ENDPLATE)
	{
		int nVertex = vertex_list.GetNodeNum();  
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(m_iParamType==TYPE_VERTPLATE)
			{
				vertice = pVertex->vertex;
				vertice.z = thick*0.5;
				pBody->vertex.append(vertice);
				vertice.z = -thick*0.5;
				pBody->vertex.append(vertice);
			}
			else if(m_iParamType==TYPE_ENDPLATE)
			{
				vertice = pVertex->vertex;
				vertice.x = 0;
				pBody->vertex.append(vertice);
				vertice.x = -thick;
				pBody->vertex.append(vertice);
			}
		}
		//坐标系由相对坐标转换到结构体的绝对坐标下
		for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		{
			if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
				return FALSE;
		}
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//前面
		f3dPolyFace *pFrontFace=pBody->faceList.append();
		pFrontFace->material = plank_color;
		createFace.NewOutterEdgeLine(pFrontFace,0,1*2);
		createFace.NewOutterEdgeLine(pFrontFace,3*2);
		createFace.NewOutterEdgeLine(pFrontFace,2*2);
		createFace.NewOutterEdgeLine(pFrontFace,1*2);
		//后面
		f3dPolyFace *pBackFace=pBody->faceList.append();
		pBackFace->material = plank_color;
		createFace.NewOutterEdgeLine(pBackFace,0+1,3*2+1);
		createFace.NewOutterEdgeLine(pBackFace,1*2+1);
		createFace.NewOutterEdgeLine(pBackFace,2*2+1);
		createFace.NewOutterEdgeLine(pBackFace,3*2+1);
		//侧面
		for(i=0; i<4; i++)
		{
			pFace=pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace,0+2*i,1+2*i);
			createFace.NewOutterEdgeLine(pFace,(2+2*i)%8);
			createFace.NewOutterEdgeLine(pFace,(3+2*i)%8);
			createFace.NewOutterEdgeLine(pFace,1+2*i);
		}
		//定义螺栓孔
		CreateLsHole(this,pBody,bDisplayHole,pFrontFace,pBackFace);
	}
	else if(m_iParamType==TYPE_LINEANGLERIBPLATE || m_iParamType==TYPE_TUBESTAYWIREPLATE)
	{
		int nVertex = vertex_list.GetNodeNum();  
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			vertice = pVertex->vertex;
			vertice.z = -thick*0.5;
			pBody->vertex.append(vertice);
			vertice.z = thick*0.5;
			pBody->vertex.append(vertice);
		}
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//坐标系由相对坐标转换到结构体的绝对坐标下
		for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		{
			if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
				return FALSE;
		}
		//前面
		f3dPolyFace *pBottomFace=pBody->faceList.append();
		pBottomFace->material = plank_color;
		for(i=0;i<nVertex;i++)
			createFace.NewOutterEdgeLine(pBottomFace,((nVertex-i)%nVertex)*2,((nVertex-i+1)%nVertex)*2);
		//之前含内凹角多边形(如外侧角钢肋板)在自动计算法线时存在易反向有错误故添加此代码,现已改正 wjh-2015.9.25
		pBottomFace->poly_norm=-ucs.axis_z;
		//后面
		f3dPolyFace *pTopFace=pBody->faceList.append();
		pTopFace->material = plank_color;
		for(i=0;i<nVertex;i++)
			createFace.NewOutterEdgeLine(pTopFace,2*i+1,2*((nVertex+i-1)%nVertex)+1);
		pTopFace->poly_norm=ucs.axis_z;
		//侧面
		for(i=0; i<nVertex; i++)
		{
			pFace=pBody->faceList.append();
			pFace->material = plank_color;
			createFace.NewOutterEdgeLine(pFace,0+2*i,1+2*i);
			createFace.NewOutterEdgeLine(pFace,(2+2*i)%(2*nVertex));
			createFace.NewOutterEdgeLine(pFace,(3+2*i)%(2*nVertex));
			createFace.NewOutterEdgeLine(pFace,1+2*i);
		}

		//定义螺栓孔
		if(m_iParamType==TYPE_LINEANGLERIBPLATE)
		{
			long I=0;
			double d=0,P=0,R=0;
			GetDesignItemValue('I',&I);
			GetDesignItemValue('d',&d);
			GetDesignItemValue('P',&P);
			GetDesignItemValue('R',&R);
			if(P>0&&d>0&&I==0)
			{	//内侧角钢肋板的螺栓孔
				long A;
				GetDesignItemValue('A',&A);
				CLDSLineAngle *pLineAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(A,CLS_LINEANGLE);
				if(pLineAngle)
				{
					double alfa=pLineAngle->GetRadWingAngle();
					double sina=sin(alfa);
					double cosa=cos(alfa);
					double c=0;
					P+=pLineAngle->GetThick();
					if(fabs(cosa)>EPS_COS)
					{	//以防出现除0
						c=P;
						P=0;
					}
					else
						c=P*sina/(1-cosa);
					UCS_STRU hole_ucs;
					f3dPoint datum_pos,final_pos;
					f3dPoint line_vec=pLineAngle->End()-pLineAngle->Start();
					normalize(line_vec);
					Int3dlf(datum_pos,pLineAngle->Start(),line_vec,ucs.origin,ucs.axis_z);
					hole_ucs.origin=datum_pos+pLineAngle->GetWingVecY()*c-pLineAngle->get_norm_y_wing()*P;
					//coord_trans(hole_ucs.origin,ucs,TRUE);
					hole_ucs.axis_z=ucs.axis_z;
					//vector_trans(hole_ucs.axis_z,ucs,FALSE);
					hole_ucs.axis_x=inters_vec(ucs.axis_z);
					hole_ucs.axis_y=hole_ucs.axis_z^hole_ucs.axis_x;
					hole_ucs.axis_x=hole_ucs.axis_y^hole_ucs.axis_z;
					CHoleMaker holeMaker;
					holeMaker.Create(hole_ucs,d,0,pTopFace,pBottomFace,pBody,GetColor(CLS_BOLT,0,' '));
				}
			}
			else if(d>0 && R>d*0.5 && I==2)
			{	//纵向角钢肋板的螺栓孔
				double L=0,W=0;
				GetDesignItemValue('L',&L);
				GetDesignItemValue('W',&W);
				UCS_STRU hole_ucs;
				for(int i=0;i<2;i++)
				{
					int sign=(i==0)?1:-1;
					hole_ucs.origin=ucs.origin+ucs.axis_x*sign*(L*0.5-R)+ucs.axis_y*(W-R);
					hole_ucs.axis_z=ucs.axis_z;
					hole_ucs.axis_x=inters_vec(ucs.axis_z);
					hole_ucs.axis_y=hole_ucs.axis_z^hole_ucs.axis_x;
					hole_ucs.axis_x=hole_ucs.axis_y^hole_ucs.axis_z;
					CHoleMaker holeMaker;
					holeMaker.Create(hole_ucs,d,0,pTopFace,pBottomFace,pBody,GetColor(CLS_BOLT,0,' '));
				}
			}
		}
		else //if(m_iParamType==TYPE_TUBESTAYWIREPLATE)
		{
			long N=0;
			double d=0,P=0,R=0,L=0,W=0;
			GetDesignItemValue('N',&N);
			GetDesignItemValue('d',&d);
			GetDesignItemValue('R',&R);
			GetDesignItemValue('P',&P);
			GetDesignItemValue('L',&L);
			GetDesignItemValue('W',&W);
			f3dPoint pt(W,L-R,0);
			coord_trans(pt,ucs,TRUE);
			UCS_STRU hole_ucs;
			for(int i=0;i<N;i++)
			{
				hole_ucs.origin=pt-ucs.axis_y*P*i;
				hole_ucs.axis_z=ucs.axis_z;
				hole_ucs.axis_x=inters_vec(ucs.axis_z);
				hole_ucs.axis_y=hole_ucs.axis_z^hole_ucs.axis_x;
				hole_ucs.axis_x=hole_ucs.axis_y^hole_ucs.axis_z;
				CHoleMaker holeMaker;
				holeMaker.Create(hole_ucs,d,0,pTopFace,pBottomFace,pBody,GetColor(CLS_BOLT,0,' '));
			}
		}
	}
	else if(m_iParamType==TYPE_XEND)
	{
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			vertice = pVertex->vertex;
			vertice.z=-0.5*thick;
			pBody->vertex.append(vertice);
			vertice.z=0.5*thick;
			pBody->vertex.append(vertice);
		}
		//坐标系由相对坐标转换到结构体的绝对坐标下
		for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		{
			if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
				return FALSE;
		}
		//初始化链表
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//基准面顶面
		f3dPolyFace *pTopDatumFace=pBody->faceList.append();
		pTopDatumFace->poly_norm=ucs.axis_z;
		pTopDatumFace->material=plank_color;
		for(i=0;i<nVertex;i++)
			createFace.NewOutterEdgeLine(pTopDatumFace,((i+1)%nVertex)*2+1,i*2+1);
		//基准面底面
		f3dPolyFace *pBottomDatumFace=pBody->faceList.append();
		pBottomDatumFace->poly_norm=-ucs.axis_z;
		pBottomDatumFace->material=plank_color;
		for(i=nVertex-1;i>=0;i--)
			createFace.NewOutterEdgeLine(pBottomDatumFace,i*2,((i+1)%nVertex)*2);
		//基准面侧面
		pFace;
		for(i=0;i<nVertex;i++)
		{
			pFace=pBody->faceList.append();
			pFace->material=plank_color;
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2,i*2);
			createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2+1);
			createFace.NewOutterEdgeLine(pFace,i*2+1);
			createFace.NewOutterEdgeLine(pFace,i*2);
		}
		//定义螺栓孔
		CreateLsHole(this,pBody,bDisplayHole,pTopDatumFace,pBottomDatumFace);
	}
	else if(m_iParamType==TYPE_WATERSHIELDEDPLATE)
	{	//遮水板
		COLORREF slot_color=GetColor(CLS_BOLT,0,' ');	//默认流水槽颜色
		double A,W,H=0,K=0,L=0;
		if(!GetDesignItemValue('A',&A))	//V型槽夹角
			return FALSE;
		if(!GetDesignItemValue('W',&W))	//外圆直径
			return FALSE;
		if(!GetDesignItemValue('H',&H))	//槽深
			return FALSE;
		if(!GetDesignItemValue('K',&K))	//槽宽
			return FALSE;
		if(!GetDesignItemValue('L',&L))	//槽长
			return FALSE;
		f3dPolyFace *pTopFace,*pBottomFace,*pTopFace2;
		BOOL bHasWaterSolt=TRUE;
		if(fabs(A)<EPS||fabs(H)<EPS||fabs(K)<EPS||fabs(L)<EPS)
			bHasWaterSolt=FALSE;	//两流水槽夹角、长度、深度或宽度为0时表示无流水槽 wht 11-07-01
		if(bHasWaterSolt)
		{	//带流水槽
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				vertice = pVertex->vertex;
				vertice.z=0;
				pBody->vertex.append(vertice);
				vertice.z= thick;
				pBody->vertex.append(vertice);
			}
			if(H>0&&K>0)
			{
				vertice = vertex_list.GetFirst()->vertex;
				vertice.z=thick-H;
				pBody->vertex.append(vertice);	//14
				vertice = vertex_list.GetNext()->vertex;
				vertice.z=thick-H;
				pBody->vertex.append(vertice);	//15
				vertice = vertex_list.GetTail()->vertex;
				vertice.z=thick-H;	
				pBody->vertex.append(vertice);	//16
				vertice = vertex_list.GetPrev()->vertex;
				vertice.z=thick-H;
				pBody->vertex.append(vertice);	//17
				double x=K/sin(0.5*A*RADTODEG_COEF);
				vertice.ID=-10;	//ID为-10统一生成圆弧时不做处理，特殊情况单独处理
				vertice.Set(0,0,thick-H);
				pBody->vertex.append(vertice);	//18
				vertice.Set(x,0,thick-H);
				pBody->vertex.append(vertice);	//19
				vertice.Set(0,0,thick);
				pBody->vertex.append(vertice);	//20
				vertice.Set(x,0,thick);
				pBody->vertex.append(vertice);	//21
				vertice.Set(W*0.5,0,thick);
				pBody->vertex.append(vertice);	//22
			}
			//初始化链表
			CCreateFace createFace;
			createFace.InitVertexList(pBody);
			//坐标系由相对坐标转换到结构体的绝对坐标下
			for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
			{
				if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
					return FALSE;
			}
			int a=pBody->vertex.GetNodeNum();
			double arc_angle=360*RADTODEG_COEF;	//遮水板均为整圆
			double R=0.5*W;
			PROFILE_VERTEX vertex;
			//顶平面(不在两流水槽之间的顶面)
			pTopFace=pBody->faceList.append();
			pTopFace->poly_norm=ucs.axis_z;
			pTopFace->material = plank_color;
			for(i=0;i<7;i++)
			{
				if(i>4||i==0)
					continue;
				if(i==1)
					createFace.NewOutterEdgeLine(pTopFace,i*2+1,20);
				createFace.NewOutterEdgeLine(pTopFace,(i*2+3)%14,i*2+1);
				if(i==4)
					createFace.NewOutterEdgeLine(pTopFace,20,(i*2+3)%14);
			}
			//两流水槽之间的顶面
			pTopFace2=pBody->faceList.append();
			pTopFace2->poly_norm=ucs.axis_z;
			pTopFace2->material = plank_color;
			createFace.NewOutterEdgeLine(pTopFace2,22,1);
			createFace.NewOutterEdgeLine(pTopFace2,13);
			createFace.NewOutterEdgeLine(pTopFace2,21);
			createFace.NewOutterEdgeLine(pTopFace2,1);
			//流水槽底面
			pFace=pBody->faceList.append();
			pFace->material = slot_color;
			createFace.NewOutterEdgeLine(pFace,19,18);
			createFace.NewOutterEdgeLine(pFace,14);
			createFace.NewOutterEdgeLine(pFace,15);
			createFace.NewOutterEdgeLine(pFace,18);
			pFace=pBody->faceList.append();
			pFace->material = slot_color;
			createFace.NewOutterEdgeLine(pFace,18,19);
			createFace.NewOutterEdgeLine(pFace,17);
			createFace.NewOutterEdgeLine(pFace,16);
			createFace.NewOutterEdgeLine(pFace,19);
			//流水槽底面
			pFace=pBody->faceList.append();
			pFace->material = slot_color;
			createFace.NewOutterEdgeLine(pFace,18,20);
			createFace.NewOutterEdgeLine(pFace,15);
			createFace.NewOutterEdgeLine(pFace,3);
			createFace.NewOutterEdgeLine(pFace,20);
			pFace=pBody->faceList.append();
			pFace->material = slot_color;
			createFace.NewOutterEdgeLine(pFace,21,19);
			createFace.NewOutterEdgeLine(pFace,1);
			createFace.NewOutterEdgeLine(pFace,14);
			createFace.NewOutterEdgeLine(pFace,19);
			pFace=pBody->faceList.append();
			pFace->material = slot_color;
			createFace.NewOutterEdgeLine(pFace,20,18);
			createFace.NewOutterEdgeLine(pFace,11);
			createFace.NewOutterEdgeLine(pFace,17);
			createFace.NewOutterEdgeLine(pFace,18);
			pFace=pBody->faceList.append();
			pFace->material = slot_color;
			createFace.NewOutterEdgeLine(pFace,19,21);
			createFace.NewOutterEdgeLine(pFace,16);
			createFace.NewOutterEdgeLine(pFace,13);
			createFace.NewOutterEdgeLine(pFace,21);
			//底平面
			pBottomFace=pBody->faceList.append();
			pBottomFace->poly_norm=-ucs.axis_z;
			pBottomFace->material = plank_color;
			for(i=6;i>=0;i--)
				createFace.NewOutterEdgeLine(pBottomFace,i*2,(i*2+2)%14);
			//外圆柱侧面
			for(i=0;i<7;i++)
			{
				pFace=pBody->faceList.append();
				pFace->material = plank_color;
				createFace.NewOutterEdgeLine(pFace,((i+1)*2)%14,i*2);
				if(i==0)
				{	//流水槽
					createFace.NewOutterEdgeLine(pFace,15);
					createFace.NewOutterEdgeLine(pFace,14);
				}
				else if(i==5)
				{	//流水槽
					createFace.NewOutterEdgeLine(pFace,16);
					createFace.NewOutterEdgeLine(pFace,17);
				}
				else
				{
					createFace.NewOutterEdgeLine(pFace,((i+1)*2)%14+1);
					createFace.NewOutterEdgeLine(pFace,i*2+1);
				}
				createFace.NewOutterEdgeLine(pFace,i*2);
			}
		}
		else 
		{	//不带流水槽
			int nVertex = vertex_list.GetNodeNum();  
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				vertice = pVertex->vertex;
				pBody->vertex.append(vertice);
				vertice.z = thick;
				pBody->vertex.append(vertice);
			}
			//初始化链表
			CCreateFace createFace;
			createFace.InitVertexList(pBody);
			//顶平
			pTopFace=pBody->faceList.append();
			pTopFace->poly_norm=ucs.axis_z;
			pTopFace->material = plank_color;
			for(i=0;i<nVertex;i++)
				createFace.NewOutterEdgeLine(pTopFace,((i+1)%nVertex)*2+1,i*2+1);
			//底平面
			pBottomFace=pBody->faceList.append();
			pBottomFace->poly_norm=-ucs.axis_z;
			pBottomFace->material = plank_color;
			for(i=nVertex-1;i>=0;i--)
				createFace.NewOutterEdgeLine(pBottomFace,((i+nVertex-1)%nVertex)*2,i*2);
			//外圆柱侧面
			for(i=0;i<nVertex;i++)
			{
				pFace=pBody->faceList.append();
				pFace->material = plank_color;
				createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2,i*2);
				createFace.NewOutterEdgeLine(pFace,((i+1)%nVertex)*2+1);
				createFace.NewOutterEdgeLine(pFace,i*2+1);
				createFace.NewOutterEdgeLine(pFace,i*2);
			}
			//坐标系由相对坐标转换到结构体的绝对坐标下
			for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
			{
				if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
					return FALSE;
			}
		}
		//定义螺栓孔
		CreateLsHole(this,pBody,bDisplayHole,pTopFace,pBottomFace);
	}
	PROFILE_VERTEX curVertex,nextVertex;
	for(pFace=pBody->faceList.GetFirst();pFace;pFace=pBody->faceList.GetNext())
	{
		for(f3dAtomLine* pLine=pFace->outer_edge.GetFirst();pLine;pLine=pFace->outer_edge.GetNext())
		{
			f3dPoint *pFeatureNode,*pNextNode;
			//if(pLine->pEnd->ID==-10||pLine->pStart->ID==-10)
			//	continue;	//特殊情况单独处理
			int sub_id=abs(pLine->pEnd->ID)-abs(pLine->pStart->ID);
			if(sub_id==0)
				continue;
			int sign=1;
			if(sub_id==1||sub_id==1-vertex_list.GetNodeNum())
			{
				pFeatureNode=pLine->pStart;
				pNextNode=pLine->pEnd;
			}
			else
			{
				pFeatureNode=pLine->pEnd;
				pNextNode=pLine->pStart;
				sign = -1;
			}
			if(pFeatureNode->ID<0)
				continue;
			if(!vertex_tbl.GetValueAt(abs(pFeatureNode->ID),curVertex) || !vertex_tbl.GetValueAt(abs(pNextNode->ID),nextVertex))
				continue;
			if (curVertex.vertex.feature>= 10 && nextVertex.vertex.feature>= 10)
				continue;
			if(curVertex.type>=1)
			{
				int iFace=0;
				if(curVertex.vertex.feature<10)
					iFace=curVertex.vertex.feature-1;
				else if (curVertex.vertex.feature>10 && curVertex.vertex.feature<20
					&& nextVertex.vertex.feature == 2)
					iFace=curVertex.vertex.feature%10-1;
				double sec_angle=0;
				if(curVertex.type==1)	//指定圆弧夹角(自动计算圆弧夹角的情况现在已自动计算出精确值)
				{
					sec_angle=curVertex.sector_angle;
					if(sec_angle<0)
					{
						sign=-sign;
						sec_angle=-sec_angle;
					}
				}
				if(curVertex.radius<0)
					sign=-sign;
				f3dPoint worknorm;
				if(pFeatureNode->feature==2||curVertex.vertex.feature==2) //生成火曲面圆弧时应使用火曲法线 wht 11-04-12
					worknorm=sign*keyvecset.huoqufacenorm;
				else
					worknorm=sign*ucs.axis_z;
				if(curVertex.type==1)
					pLine->CreateArcLine(worknorm,sec_angle);
				else if(curVertex.type==2)	//指定R
				{
					if(*pLine->pStart==*pLine->pEnd)
						continue;	//直线始端终端顶点重合无法形成圆弧 wht 11-06-03
					f3dPoint center_pick=curVertex.center;
					coord_trans(center_pick,ucs,TRUE);
					pLine->CreateArcLine2(worknorm,fabs(curVertex.radius),center_pick);
				}
				else //椭圆弧
				{
					//f3dPoint s=*pLine->pStart,e=*pLine->pEnd;
					//coord_trans(s,ucs,FALSE);
					//coord_trans(e,ucs,FALSE);
					if(worknorm.IsZero())
						continue;	//工作法线为NULL时死机,暂未查明原因(跨越连板) wht 11-05-13
					//pLine->CreateArcLine2(worknorm,fabs(curVertex.radius),curVertex.center); //测试用以后删除
					pLine->CreateEllipseArcLine(curVertex.center,curVertex.column_norm,worknorm,fabs(curVertex.radius));
					//椭圆时有可能需要调整始终端点位置
					f3dPoint start=pLine->PositionInAngle(0);
					f3dPoint end = pLine->PositionInAngle(pLine->SectorAngle());
					pLine->pStart->Set(start.x,start.y,start.z);
					pLine->pEnd->Set(end.x,end.y,end.z);
				}
			}
		}
	}
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
}

//计算外形顶点根据连接方向
BOOL CLDSParamPlate::CalProfileVertexByConnVector(CONNECT_DIRECTION *pVec,CLDSNode *pBaseNode)
{
	if(!pVec->IsProfilePole())
		return FALSE;	//非外形杆件
	long hPart=pVec->direct.hVicePart;
	CLDSLinePart *pLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(hPart,CLS_LINETUBE,CLS_LINEANGLE);
	if(pLinePart==NULL)
		return FALSE;
	if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
		return FALSE;
	//首先根据基准基点判断杆件始终端连接,如未连接在基准节点上根据direction参数进行判断
	int start0_end1=0;
	if(pLinePart->pStart==pBaseNode)
		start0_end1=0;
	else if(pLinePart->pEnd==pBaseNode)
		start0_end1=1;
	else if(pVec->direct.norm_style==5&&pVec->direct.direction==0)
		start0_end1=0;
	else if(pVec->direct.norm_style==5&&pVec->direct.direction==1)
		start0_end1=1;
	else
		return FALSE;
	//根据螺栓计算关键点
	f3dLine ber_line;
	PROFILE_VERTEX *pVertex=NULL;
	f3dPoint vertex_up,vertex_down,bolt_up,bolt_down;
	f3dPoint base_pos,ls_pos,ls_offset_vec,vice_vec,wing_vec;
	if(start0_end1==0)	//始端连接
		vice_vec=pLinePart->End()-pLinePart->Start();
	else				//终端连接
		vice_vec=pLinePart->Start()-pLinePart->End();
	normalize(vice_vec);
	//
	LSSPACE_STRU LsSpace;
	int head_bolt_d=20,end_bolt_d=20;
	if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)	//角钢构件一般有连接螺栓
	{
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
		//if(pLineAngle->GetLsCount()==0)
		//	return FALSE;	//射线杆件上无螺栓
		int cur_wing_x0_y1=0;
		if(fabs(pLineAngle->get_norm_x_wing()*ucs.axis_z)
			>fabs(pLineAngle->get_norm_y_wing()*ucs.axis_z))
			cur_wing_x0_y1=0;
		else 
			cur_wing_x0_y1=1;
		ber_line.startPt=pLineAngle->Start();
		ber_line.endPt=pLineAngle->End();
		if(start0_end1==0)	//终端节点不是当前设计节点
			base_pos=pLineAngle->Start();
		else
			base_pos=pLineAngle->End();
		double max_ber_dist=0,min_ber_dist=pLineAngle->GetWidth();
		double head_len=0,tail_len=0;		//头->尾方向与射线方向一致
		if(start0_end1==0)					//终端节点不是当前设计节点
			head_len=tail_len=-pLineAngle->startOdd();
		else
			head_len=tail_len=-pLineAngle->endOdd();
		BOOL bFirstLs=TRUE;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(pBolt&&pLineAngle->FindLsByHandle(pBolt->handle))
			{
				//角钢正负头修改后螺栓位置和法线都有可能修改，所以需要再次更新
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				ls_pos=pBolt->ucs.origin;
				if(cur_wing_x0_y1==0)
					project_point(ls_pos,base_pos,pLineAngle->get_norm_x_wing());
				else 
					project_point(ls_pos,base_pos,pLineAngle->get_norm_y_wing());
				double dist=0;
				SnapPerp(&ls_pos,ber_line,ls_pos,&dist);
				if(bFirstLs)
					max_ber_dist=min_ber_dist=dist;
				else if(dist>max_ber_dist)
					max_ber_dist=dist;
				else if(dist<min_ber_dist)
					min_ber_dist=dist;
				ls_offset_vec = ls_pos-base_pos;
				if(ls_offset_vec*vice_vec>0)
					dist = ls_offset_vec.mod();
				else
					dist = -ls_offset_vec.mod();
				if(bFirstLs)
				{
					head_len=tail_len=dist;
					head_bolt_d=end_bolt_d=pBolt->get_d();
				}
				else if(dist>tail_len)
				{
					tail_len=dist;
					end_bolt_d=pBolt->get_d();
				}
				else if(dist<head_len)
				{
					head_len=dist;
					head_bolt_d=pBolt->get_d();
				}
				bFirstLs=FALSE;
			}
		}
		if(bFirstLs)	//射线杆件上无螺栓
		{
			if(cur_wing_x0_y1==0)
				max_ber_dist=min_ber_dist=pLineAngle->GetWingXG(0);
			else
				max_ber_dist=min_ber_dist=pLineAngle->GetWingYG(0);
			//return FALSE;
		}
		
		if(cur_wing_x0_y1==0)
			wing_vec=pLineAngle->GetWingVecX();
		else
			wing_vec=pLineAngle->GetWingVecY();
		//
		GetLsSpace(LsSpace,end_bolt_d);
		//根据用户设置的端距和边距设计板外形，前提是要保证最小端距及边距 wht 11-04-25
		//LsSpace.PlateEdgeSpace=max(LsSpace.PlateEdgeSpace,pVec->high_space);
		LsSpace.EndSpace=max(LsSpace.EndSpace,pVec->end_space);
		f3dPoint vertice,vv,vertice1;
		vv=base_pos+vice_vec*(tail_len+LsSpace.EndSpace);
		vertice=vv+wing_vec*(min_ber_dist-pVec->high_space);
		vertice1=vv+wing_vec*(max_ber_dist+pVec->tip_space);
		coord_trans(vertice,ucs,FALSE);
		coord_trans(vertice1,ucs,FALSE);
		vertice.feature=vertice1.feature=1;
		vertice.z=vertice1.z=0;
		vertex_list.append(PROFILE_VERTEX(vertice));
		vertex_list.append(PROFILE_VERTEX(vertice1));
	}
	else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube *pLineTube=(CLDSLineTube*)pLinePart;
		CLDSParamPlate *pParamPlate=pLineTube->GetParamPlate(start0_end1);
		if(pParamPlate==NULL)
			return FALSE;	//无端板不进行设计
		if(pParamPlate->m_iParamType!=TYPE_UEND
			&&pParamPlate->m_iParamType!=TYPE_ROLLEND
			&&pParamPlate->m_iParamType!=TYPE_XEND)
			return FALSE;	//不是U型插板、槽型插板或十字插板不进行设计
		double L=0,H=0;
		if(pParamPlate->m_bStdPart)
		{	//标准构件
			CLDSStdPartModel stdPartModel;
			if(!pParamPlate->GetStdPartModel(&stdPartModel))
				return FALSE;
			L=stdPartModel.param.insert_plate_param.L;
			H=stdPartModel.param.insert_plate_param.H;
		}
		else
		{	//非标准构件
			pParamPlate->GetDesignItemValue('L',&L);
			pParamPlate->GetDesignItemValue('H',&H);
		}
		double minx=1000000,maxx=0,miny=0.5*H,maxy=-0.5*H;
		for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(pBolt&&pParamPlate->FindLsByHandle(pBolt->handle))
			{	//钢管正负头修改后螺栓位置和法线方向都有可能变化
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				f3dPoint pos = (*pLsRef)->ucs.origin;
				coord_trans(pos,pParamPlate->ucs,FALSE);
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
		}
		if(minx>maxx)
			minx=maxx;
		GetLsSpace(LsSpace,head_bolt_d);
		//根据用户设置的端距，边距设计板外形，前提是要保证最小端距 wht 11-04-25
		LsSpace.PlateEdgeSpace=max(LsSpace.PlateEdgeSpace,pVec->high_space);
		LsSpace.EndSpace=max(LsSpace.EndSpace,pVec->end_space);
		vertex_up.x=vertex_down.x=minx-LsSpace.EndSpace;
		vertex_up.y=0.5*H;
		vertex_down.y=-vertex_up.y;
		coord_trans(vertex_up,pParamPlate->ucs,TRUE);
		coord_trans(vertex_down,pParamPlate->ucs,TRUE);
		coord_trans(vertex_up,ucs,FALSE);
		coord_trans(vertex_down,ucs,FALSE);
		vertex_up.z = vertex_down.z = 0;
		vertex_up.feature = vertex_down.feature = 1;
		vertex_list.append(PROFILE_VERTEX(vertex_up));
		vertex_list.append(PROFILE_VERTEX(vertex_down));
	}
	return TRUE;
}

static int CompareVertex(const PROFILE_VERTEX &vertex1,const PROFILE_VERTEX &vertex2)
{
	double angle1=Cal2dLineAng(f2dPoint(0,0),f2dPoint(vertex1.vertex.x,vertex1.vertex.y));
	double angle2=Cal2dLineAng(f2dPoint(0,0),f2dPoint(vertex2.vertex.x,vertex2.vertex.y));
	if(angle1>angle2)
		return 1;
	else if(angle1<angle2)
		return -1;
	else 
		return 0;
}
//计算钢板顶面或底面拾取点(参数化钢板或普通钢板)
static f3dPoint GetPlateDatumFacePickPt(CLDSPart *pDatumPlate,BOOL bTopFace)
{
	f3dPoint face_pick;
	if(!pDatumPlate->IsPlate())
		return face_pick;
	face_pick=pDatumPlate->ucs.origin;
	if(pDatumPlate->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate=(CLDSPlate*)pDatumPlate;
		face_pick+=(pPlate->ucs.axis_z*pPlate->m_fNormOffset);
		if(bTopFace)
			face_pick+=(pPlate->ucs.axis_z*pPlate->GetThick());
	}
	else if(pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE) 
	{
		if(bTopFace)
			face_pick+=pDatumPlate->ucs.axis_z*(pDatumPlate->GetThick()+((CLDSParamPlate*)pDatumPlate)->GetNormOffset());
		else
			face_pick+=pDatumPlate->ucs.axis_z*((CLDSParamPlate*)pDatumPlate)->GetNormOffset();
		/*double offset=((CLDSParamPlate*)pDatumPlate)->m_fNormOffset;
		if(bTopFace)
			offset+=pDatumPlate->GetThick()*0.5;
		else
			offset-=pDatumPlate->GetThick()*0.5;
		if(((CLDSParamPlate*)pDatumPlate)->m_iParamType==TYPE_FL||
			((CLDSParamPlate*)pDatumPlate)->m_iParamType==TYPE_FLG||
			((CLDSParamPlate*)pDatumPlate)->m_iParamType==TYPE_FLR||
			((CLDSParamPlate*)pDatumPlate)->m_iParamType==TYPE_WATERSHIELDEDPLATE)	//法兰实体底部在原点
			offset+=pDatumPlate->GetThick()*0.5;
		face_pick+=pDatumPlate->ucs.axis_z*offset;
		*/
	}
	return face_pick;
}
//<DEVELOP_PROCESS_MARK nameId="CLDSParamPlate::DesignCircularPlate">
/*	各字符含义
	A:α,环向定位角(定位参数)
	B:β,环向扇形角(外形参数)
	C:定位基准钢管(球体)句柄(定位参数)
	D:基准钢管外壁到自由边距离，简称自由边距
	E:第一块关联基准钢板句柄
	F:第二块关联基准钢板句柄，或焊接关联钢管句柄(关联钢管是指与环向板有焊缝的钢管，不含上面的插板螺栓连接钢管，因此不可能出现两根)
	G:自由边类型(>0直线，<=0圆弧)
	H:倒角高度
	I:第一根连接方向钢管句柄(指明有此钢管方向的连接意图，即插板螺栓连接或无连接制弯)
	J:第二根连接方向钢管句柄(指明有此钢管方向的连接意图，即插板螺栓连接或无连接制弯)
	P:定位基准节点句柄
	R:环向板外圆弧半径
	r:环向板内圆半径(异形法兰或360的环向板使用 r<0根据钢管生成内圆 r=0无内圆 r>0按指定半径生成内圆) wht 11-05-27
	S:自定位基准节点向定位原点的轴向偏移距离(S<0上封板;S>0下封板)
	T:自定位基准节点向定位原点的轴向偏移距离在环向板法线上的投影距离
	V:加工间隙(基准钢管外壁到环向板内环的预留间隙)
	Y:半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)
	W:第一块关联钢板的焊缝长
	N:第二块关联钢板或钢管的焊缝长
*/
BOOL CLDSParamPlate::DesignCircularPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	long C=0,E=0,F=0,G=0,I=0,J=0,K=0,P=0;
	double A=0,B=2*Pi,W=0,H=0,h=0,D=0,N=0,S=0,T=0,Y=0,gama=45,V=0,r=0,j=0;
	double Ri=0,Ro=0;
	double Hw=0,Hn=0;	//自动根据螺栓计算得出的始末端倒角高
	DESIGN_PARAM::DUAL_VALUE rela_part1,rela_part2;
	DESIGN_PARAM itemparam;
	rela_part1.uiAttribute=rela_part2.uiAttribute=0;
	f3dPoint inters,face_pick,vertice,verify_vec,datum_axis_z;
	if(GetDesignItemValue('A',&A,pItemHashTbl))	//A:α,环向定位角(定位参数)
		A*=RADTODEG_COEF;
	if(GetDesignItemValue('B',&B,pItemHashTbl))	//B:β,环向扇形角(外形参数)
		B*=RADTODEG_COEF;
	GetDesignItemValue('C',&C,pItemHashTbl);	//C:定位基准钢管(球体)句柄(定位参数)
	GetDesignItemValue('D',&D,pItemHashTbl);	//D:基准钢管外壁到自由边距离，简称自由边距
	//GetDesignItemValue('E',&E,pItemHashTbl);	//E:第一块关联基准钢板句柄
	if(!GetDesignItemValue('E',&itemparam,pItemHashTbl))
		rela_part1.hPart=E=0;
	else
	{
		if(itemparam.cValueType==3)	//句柄+数值
		{
			rela_part1.hPart=E=itemparam.value.hPart;
			rela_part1.uiAttribute=itemparam.value.uiAttribute;
		}
		else //if(item_param.cValueType)
			rela_part1.hPart=E=itemparam.ToLong();
	}
	//GetDesignItemValue('F',&F,pItemHashTbl);	//F:第二块关联基准钢板句柄，或焊接关联钢管句柄(关联钢管是指与环向板有焊缝的钢管，不含上面的插板螺栓连接钢管，因此不可能出现两根)
	if(!GetDesignItemValue('F',&itemparam,pItemHashTbl))
		rela_part2.hPart=F=0;
	else
	{
		if(itemparam.cValueType==3)	//句柄+数值
		{
			rela_part2.hPart=F=itemparam.value.hPart;
			rela_part2.uiAttribute=itemparam.value.uiAttribute;
		}
		else //if(item_param.cValueType)
			rela_part2.hPart=F=itemparam.ToLong();
	}
	GetDesignItemValue('G',&G,pItemHashTbl);	//G:自由边类型(>0直线，<=0圆弧)
	GetDesignItemValue('H',&H,pItemHashTbl);	//H:台口高度
	if(!GetDesignItemValue('h',&h,pItemHashTbl))//h:N侧台口高度,无此参数时h=H
		h=H;
	GetDesignItemValue('I',&I,pItemHashTbl);	//I:第一根连接方向钢管句柄(指明有此钢管方向的连接意图，即插板螺栓连接或无连接制弯)
	GetDesignItemValue('J',&J,pItemHashTbl);	//J:第二根连接方向钢管句柄(指明有此钢管方向的连接意图，即插板螺栓连接或无连接制弯)
	GetDesignItemValue('P',&P,pItemHashTbl);	//P:定位基准节点句柄
	GetDesignItemValue('R',&Ro,pItemHashTbl);	//R:环向板外圆弧半径
	//'r'参数已被后来的'V'加工间隙取代 wjh-2017.5.24
	//GetDesignItemValue('r',&r,pItemHashTbl);	//r:环向板内圆半径(异形法兰或360的环向板使用 r<0根据钢管生成内圆 r=0无内圆 r>0按指定半径生成内圆) wht 11-05-27
	GetDesignItemValue('S',&S,pItemHashTbl);	//S:自定位基准节点向定位原点的轴向偏移距离
	GetDesignItemValue('T',&T,pItemHashTbl);	//T:自定位基准节点向定位原点的轴向偏移距离在环向板法线上的投影距离
	GetDesignItemValue('V',&V,pItemHashTbl);	//V:加工间隙(基准钢管外壁到环向板内环的预留间隙)
	GetDesignItemValue('Y',&Y,pItemHashTbl);	//Y:半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)
	GetDesignItemValue('y',&gama,pItemHashTbl);	//y:γ,2WA型环向肋板自由侧的倒角角度,单位°
	GetDesignItemValue('W',&W,pItemHashTbl);	//W:第一块关联钢板的焊缝长
	GetDesignItemValue('N',&N,pItemHashTbl);	//N:第二块关联钢板或钢管的焊缝长
	GetDesignItemValue('j',&j,pItemHashTbl);	//j:起始边：0.基准线|1.焊道线
	//
	CLDSSphere *pDatumSphere=NULL;
	CLDSLineTube *pDatumTube=NULL,*pRelaTube=NULL,*pDirectTube1=NULL,*pDirectTube2=NULL;
	CLDSPart *pPart=m_pModel->FromPartHandle(C,CLS_LINETUBE,CLS_SPHERE);
	if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
		pDatumTube=(CLDSLineTube*)pPart;
	else if(pPart)
		pDatumSphere=(CLDSSphere*)pPart;
	if((pDatumSphere==NULL&&pDatumTube==NULL))
		return FALSE;
	CLDSNode *pBaseNode=m_pModel->FromNodeHandle(P);
	CLDSPart *pRelaPlate1=NULL,*pRelaPlate2=NULL;
	CLDSLineAngle* pRelaJg=NULL;
	pPart=m_pModel->FromPartHandle(E,CLS_PLATE,CLS_PARAMPLATE,CLS_LINEANGLE);
	if(pPart && pPart->IsPlate())
		pRelaPlate1=pPart;
	else if(pPart && pPart->IsAngle())
		pRelaJg=(CLDSLineAngle*)pPart;
	pPart=m_pModel->FromPartHandle(F,CLS_PLATE,CLS_PARAMPLATE,CLS_LINETUBE);
	if(pPart &&pPart->IsPlate())
		pRelaPlate2=pPart;
	else if(pPart && pPart->GetClassTypeId()==CLS_LINETUBE)
		pRelaTube=(CLDSLineTube*)pPart;
	CLDSPlate *pTransPlate=NULL;	//贯通板,第一块关联钢板为贯通钢板时该变量可用 wht 11-06-03
	if(pRelaPlate1&&pRelaPlate1->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pRelaPlate1)->IsTransPlate())
		pTransPlate=(CLDSPlate*)pRelaPlate1;
	Ri=pDatumTube->GetDiameter()*0.5+V;	//内圆弧半径
	if(Ro>0)		//异型法兰或其余跨越环向板中指定了外圆弧半径绝对值
		D=Ro-Ri;	//应考虑加工间隙 wht 11-05-17
	else if(D>0)
		Ro=Ri+D;
	else if(pRelaPlate1&&pTransPlate==NULL&&pRelaPlate2==NULL&&pRelaTube==NULL)
		Ro=W+Ri;	//2WA 仅有一块基准钢板且自由边为圆弧 wht 11-05-20
	BOOL bIsCirRib3WA=FALSE;	//3WA类型的环向肋板 wht 11-05-24 猜测应为自由边仅为一条圆弧的三焊缝扇形肋板 wjh-2017.5.27
	if(H<EPS&&W<EPS&&N<EPS&&fabs(Ro-D-Ri)<EPS&&G<=0&&pRelaPlate1&&pRelaPlate2)
		bIsCirRib3WA=TRUE;
	//计算Z轴方向
	ucs.axis_z=GetRelaConnPlaneNorm(1,pItemHashTbl);
	if(ucs.axis_z.IsZero())
		return FALSE;
	//设定环向肋板坐标系原点及法线标准化
	if(pBaseNode&&pDatumTube)
	{
		datum_axis_z=pDatumTube->End()-pDatumTube->Start();
		normalize(datum_axis_z);
		if(pBaseNode->m_cPosCalType==4)	//交叉点应取交叉关联杆件的实际交点（角钢为心线交点，钢管为轴线交点）
			pBaseNode->GetIntersZhun(&ucs.origin);
		else
			ucs.origin=pDatumTube->GetDatumPos(pBaseNode);
		if(pRelaTube==NULL)	//有关联钢管时S参数无效
		{
			if(fabs(T)>0)
			{
				double cosa=fabs(datum_axis_z*ucs.axis_z);
				if(cosa>EPS) 
					S=T/cosa;
			}
		}
	}
	if(pDatumTube&&pRelaJg)
	{	//环向走道板
		if(GetLsCount()<=0)
			return FALSE;
		datum_axis_z=(pDatumTube->End()-pDatumTube->Start()).normalized();
		f3dPoint line_vec=(pRelaJg->End()-pRelaJg->Start()).normalized();
		if (fabs(line_vec*datum_axis_z) > EPS2)
		{
			Log2File()->LevelLog(CLogFile::WARNING_LEVEL2_COMMON, "0x%X走道板依赖角钢0x%X与钢管0x%x不垂直，设计失败", handle, pRelaJg->handle, pDatumTube->handle);
			return FALSE;	//
		}
		char cWorkWing='X';
		if(fabs(pRelaJg->vxWingNorm*datum_axis_z)<fabs(pRelaJg->vyWingNorm*datum_axis_z))
			cWorkWing='Y';
		f3dPoint work_norm=(cWorkWing=='X')?pRelaJg->vxWingNorm:pRelaJg->vyWingNorm;
		f3dPoint wing_vec=(cWorkWing=='X')?pRelaJg->GetWingVecX():pRelaJg->GetWingVecY();
		f3dPoint line_ptS=pRelaJg->Start();
		f3dPoint line_ptE=pRelaJg->End();
		//计算坐标系原点
		f3dPoint pt,ptS,ptE,inter_pt;
		f3dPoint extend_vec=datum_axis_z^line_vec;
		Int3dlf(ptS,line_ptS,extend_vec,pDatumTube->Start(),extend_vec);
		Int3dlf(ptE,line_ptE,extend_vec,pDatumTube->Start(),extend_vec);
		Int3dll(ptS,ptE,pDatumTube->Start(),pDatumTube->End(),pt);
		ucs.origin=pt;
		//计算坐标系各轴向
		CLDSLineAngle* pRelaJg2=(CLDSLineAngle*)m_pModel->FromPartHandle(F,CLS_LINEANGLE);
		SnapPerp(&inter_pt,line_ptS,line_ptE,ucs.origin);
		ucs.axis_x=inter_pt-ucs.origin;
		if(pRelaJg2)
		{
			SnapPerp(&inter_pt,pRelaJg2->Start(),pRelaJg2->End(),ucs.origin);
			ucs.axis_x=ucs.origin-inter_pt;
		}
		normalize(ucs.axis_x);
		ucs.axis_z=datum_axis_z;
		ucs.axis_z*=(work_norm*datum_axis_z<0)?-1:1;
		normalize(ucs.axis_z);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_y);
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		ucs.origin+=ucs.axis_z*Thick*0.5;
		//计算螺栓边距
		CLDSBolt *pFirBolt=GetFirstLsRef()->GetLsPtr();
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,pFirBolt->get_d());
		double edge_space=0;
		GetDesignItemValue(KEY2C("ES"),&edge_space,pItemHashTbl);
		if(edge_space<=0)
		{
			JGZJ jgzj=pRelaJg->GetAngleZJ(pRelaJg->GetWidth());
			if(edge_space==0)		//标准边距
				edge_space=LsSpace.PlateEdgeSpace;
			else if(edge_space==-1)	//延伸到边
			{
				if(ucs.axis_x*wing_vec>0)
					edge_space=pRelaJg->GetWidth()-ftoi(jgzj.g);
				else
					edge_space=ftoi(jgzj.g);
			}
			SetDesignItemValue(KEY2C("ES"),edge_space);
		}
		//计算角钢上螺栓特征点
		f3dPoint bolt_pt1,bolt_pt2;
		double head_len=0,tail_len=0,dist=0,ber_space=0;
		BOOL bInStart=(DISTANCE(pt,ptS)<DISTANCE(pt,ptE))?TRUE:FALSE;
		f3dPoint vice_vec=bInStart?(line_vec*1):(line_vec*-1);
		f3dPoint base_pos=bInStart?line_ptS:line_ptE;
		if(GetLsCount()>1)
		{
			for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				if(pBolt==NULL||pRelaJg->FindLsByHandle(pBolt->handle)==NULL)
					continue;
				f3dPoint ls_pos=pBolt->ucs.origin;
				project_point(ls_pos,base_pos,work_norm);
				SnapPerp(&ls_pos,f3dLine(line_ptS,line_ptE),ls_pos,&ber_space);
				f3dPoint ls_offset_vec = ls_pos-base_pos;
				if(ls_offset_vec*vice_vec>0)
					dist=ls_offset_vec.mod();
				else
					dist=-ls_offset_vec.mod();
				if(head_len==0 && fabs(dist)>0)
					head_len=tail_len=dist;
				else if(dist>tail_len)
					tail_len=dist;
				else if(dist<head_len)
					head_len=dist;
			}
			bolt_pt1=base_pos+wing_vec*ber_space+vice_vec*(tail_len+LsSpace.PlateEdgeSpace);
			bolt_pt2=base_pos+wing_vec*ber_space+vice_vec*(head_len-LsSpace.PlateEdgeSpace);
			bolt_pt1+=ucs.axis_x*edge_space;
			bolt_pt2+=ucs.axis_x*edge_space;
			coord_trans(bolt_pt1,ucs,FALSE);
			coord_trans(bolt_pt2,ucs,FALSE);
			bolt_pt1.z=bolt_pt2.z=0;
		}
		else if(GetLsCount()==1)
		{	//只有一个螺栓时,根据AXIS_Y计算螺栓处的轮廓点
			f3dPoint ls_pos=pFirBolt->ucs.origin;
			project_point(ls_pos,base_pos,work_norm);
			coord_trans(ls_pos,ucs,FALSE);
			double fY=fabs(ls_pos.y)+LsSpace.PlateEdgeSpace;
			bolt_pt1.Set(ls_pos.x+edge_space,fY);
			bolt_pt2.Set(ls_pos.x+edge_space,-fY);
		}
		if(bolt_pt1.y>bolt_pt2.y)
		{
			f3dPoint tem_pt=bolt_pt1;
			bolt_pt1=bolt_pt2;
			bolt_pt2=tem_pt;
		}
		//计算外形轮廓点
		BOOL bCalTan=FALSE;
		double angle_start=-B*0.5,angle_end=B*0.5,radius=Ri+W;
		f3dPoint vertex_S,vertex_E,tan_pt1,tan_pt2;
		vertex_S.Set(Ri*cos(angle_start),Ri*sin(angle_start));
		vertex_E.Set(Ri*cos(angle_end),Ri*sin(angle_end));
		if(fabs(H)>0)
		{
			tan_pt1.Set(vertex_S.x+H,vertex_S.y-W);
			tan_pt2.Set(vertex_E.x+H,vertex_E.y+W);
		}
		else
		{
			f2dPoint tan_pt;
			if(Tang2dpc(f2dPoint(bolt_pt1.x,bolt_pt1.y),f2dCircle(radius,0,0),f2dPoint(vertex_S.x,vertex_S.y),tan_pt))
				tan_pt1.Set(tan_pt.x,tan_pt.y,0);
			if(Tang2dpc(f2dPoint(bolt_pt2.x,bolt_pt2.y),f2dCircle(radius,0,0),f2dPoint(vertex_E.x,vertex_E.y),tan_pt))
				tan_pt2.Set(tan_pt.x,tan_pt.y,0);
			bCalTan=TRUE;
		}
		//初始化轮廓列表
		PROFILE_VERTEX* pVertex=NULL;
		//第1个
		pVertex=vertex_list.append(PROFILE_VERTEX(vertex_E,2));
		pVertex->radius=-Ri;
		pVertex->m_bWeldEdge=TRUE;
		//第2个
		vertex_list.append(PROFILE_VERTEX(vertex_S));
		//第3个
		pVertex=vertex_list.append(PROFILE_VERTEX(f3dPoint(vertex_S.x,vertex_S.y-W,0)));
		if(bCalTan)
		{
			pVertex->type=2;
			pVertex->radius=radius;
		}
		//第4,5,6个
		vertex_list.append(PROFILE_VERTEX(tan_pt1));
		vertex_list.append(PROFILE_VERTEX(bolt_pt1));
		vertex_list.append(PROFILE_VERTEX(bolt_pt2));
		//第7个
		pVertex=vertex_list.append(PROFILE_VERTEX(tan_pt2));
		if(bCalTan)
		{
			pVertex->type=2;
			pVertex->radius=radius;
		}
		//第8个
		vertex_list.append(PROFILE_VERTEX(f3dPoint(vertex_E.x,vertex_E.y+W)));
	}
	else if(pDatumTube&&pRelaPlate1==NULL)
	{	//异型法兰
		//ucs.axis_z应为输入值,一般是水平面
		ucs.axis_x=pDatumTube->ucs.axis_x;
		project_vector(ucs.axis_x,ucs.axis_z);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		normalize(ucs.axis_z);
		//确定坐标系后再根据偏移量计算坐标原点位置 wht 11-05-13
		if(fabs(S)>0)
		{
			if(datum_axis_z*ucs.axis_z>0)
				ucs.origin+=datum_axis_z*S;
			else 
				ucs.origin-=datum_axis_z*S;
		}
		//1.根据连接方向计算关键点
		vertex_list.Empty();
		CONNECT_DIRECTION *pVec=NULL;
		for(pVec=ConnDirectList.GetFirst();pVec;pVec=ConnDirectList.GetNext())
			CalProfileVertexByConnVector(pVec,pBaseNode);
		int nVertex=vertex_list.GetNodeNum();
		if(nVertex>0)
		{
			//2.关键点逆时针排序
			PROFILE_VERTEX *pPntArr = new PROFILE_VERTEX[nVertex];
			for(int i=0;i<nVertex;i++)
			{
				pPntArr[i] = vertex_list[i];
				pPntArr[i].vertex.z = 0;
			}
			CQuickSort<PROFILE_VERTEX>::QuickSort(pPntArr,nVertex,CompareVertex);
			for(i=0;i<nVertex;i++)
				vertex_list[i] = pPntArr[i];
			delete []pPntArr;
			//3.关键点与异形法兰求得合理切点
			f2dCircle retPt,cir(Ro,f2dPoint(0,0));
			for(i=0;i<nVertex;i++)
			{
				f2dPoint tan_pt;
				PROFILE_VERTEX *pVertex=&vertex_list[i];
				PROFILE_VERTEX *pNextVertex=&vertex_list[(i+1)%nVertex];
				PROFILE_VERTEX *pPreVertex=&vertex_list[(i-1+nVertex)%nVertex];
				if(pVertex->type>0)
					continue;	//当前轮廓点位圆弧边上的点
				f3dPoint vertice=pVertex->vertex,next_v=pNextVertex->vertex;
				if(pPreVertex&&pPreVertex->type>0)
				{	
					//前一点为圆弧点，计算顺时针方向侧的切点
					if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,f2dPoint(vertice.y,-vertice.x),tan_pt))
					{
						vertex_list.insert(PROFILE_VERTEX(tan_pt.x,tan_pt.y,0),i);//在当前轮廓点之前插入切点
						nVertex++;	//轮廓点总数+1
						i++;		//当前轮廓点索引+1
					}
				}
				if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,f2dPoint(-vertice.y,vertice.x),tan_pt))
				{
					BOOL bInsertVertex=FALSE;
					if(pNextVertex->type==0)
					{
						f3dPoint next_vec=next_v.normalized();	//圆心到下一轮廓点之间的方向
						f3dPoint cur_norm(-vertice.y,vertice.x);//圆心到当前轮廓点逆时针旋转90度的方向
						normalize(cur_norm);
						if(next_vec*cur_norm<EPS)
							bInsertVertex=TRUE;	//下一轮廓点在当前轮廓点与圆心连线的另一侧，该切点合法
						else 
						{
							double dist=0;
							f2dPoint perp;
							SnapPerp(&perp,f2dPoint(vertice.x,vertice.y),f2dPoint(next_v.x,next_v.y),f2dPoint(0,0),&dist);
							if(dist>0&&dist<cir.radius)	//圆心到当前轮廓点与下一轮廓点的垂直距离小于求切圆的半径，该切点合法
								bInsertVertex=TRUE;
						}
					}
					else 
						bInsertVertex=TRUE;
					if(bInsertVertex)
					{	//在当前顶点后插入一个圆弧点
						PROFILE_VERTEX *pNewVertex=vertex_list.insert(PROFILE_VERTEX(tan_pt.x,tan_pt.y,0),i+1);
						pNewVertex->type=2;
						pNewVertex->radius=Ro;
						nVertex++;	//轮廓点总数+1
						i++;		//当前轮廓点索引+1
					}
				}
			}
			f2dPoint tan_pt;
			PROFILE_VERTEX *pTailVertex=vertex_list.GetTail();
			PROFILE_VERTEX *pFirstVertex=vertex_list.GetFirst();
			if(pTailVertex&&pTailVertex->type>0&&pFirstVertex)
			{	//最后一点为圆弧点需要计算第一点与顺时针方向一侧的切点 wht 11-05-16
				f3dPoint vertice=pFirstVertex->vertex;
				if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,f2dPoint(vertice.y,-vertice.x),tan_pt))
					vertex_list.append(PROFILE_VERTEX(tan_pt.x,tan_pt.y,0));//在最后插入一个轮廓点
			}
		}
		else 
		{	//普通环向板
			if(B>2*Pi||fabs(B-2*Pi)<EPS)
			{	//整圆
				PROFILE_VERTEX vertex;
				vertex.type=1;
				vertex.center=ucs.origin;
				vertex.sector_angle = Pi*0.5;	//四等分圆弧
				vertex_list.Empty();
				vertex.vertex.Set(Ro,0,0);
				vertex_list.append(vertex);
				vertex.vertex.Set(0,Ro,0);
				vertex_list.append(vertex);
				vertex.vertex.Set(-Ro,0,0);
				vertex_list.append(vertex);
				vertex.vertex.Set(0,-Ro,0);
				vertex_list.append(vertex);
			}
			else 
			{	//非正圆时通过指定角度生成扇形板 wht 11-05-27
				//根据输入的定位偏移角调整坐标系
				double rotAngle=A;
				if(pDatumTube->m_bHasWeldRoad&&j!=0)	//指定焊道边
					rotAngle+=pDatumTube->CalWeldRoadAngle();
				RotateVectorAroundVector(ucs.axis_x,rotAngle,ucs.axis_z);
				ucs.axis_y=ucs.axis_z^ucs.axis_x;
				normalize(ucs.axis_y);
				//轮廓点
				f3dPoint vertex_start,vertex_end;
				vertex_start.Set(Ri,0,0);
				vertex_start.feature=1;
				vertex_start.ID=1;
				vertex_list.append(PROFILE_VERTEX(vertex_start));
				//
				vertex_start.Set(Ro,0,0);
				vertex_start.feature=1;
				vertex_start.ID=2;
				PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertex_start,1));
				pVertex->sector_angle = B;
				pVertex->center.Set();
				//
				vertex_end=vertex_start;
				rotate_point_around_axis(vertex_end,B,f3dPoint(0,0,0),f3dPoint(0,0,100));
				vertex_end.feature=1;
				vertex_end.ID=3;
				vertex_list.append(PROFILE_VERTEX(vertex_end));
				//
				vertex_end.Set(Ri,0,0);
				rotate_point_around_axis(vertex_end,B,f3dPoint(0,0,0),f3dPoint(0,0,100));
				vertex_end.feature=1;
				vertex_end.ID=4;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertex_end,1));
				if(fabs(datum_axis_z*ucs.axis_z)<EPS_COS)	
				{	//椭圆弧
					pVertex->type=3;
					pVertex->center=ucs.origin;
					if(ucs.axis_z*datum_axis_z>0)
						pVertex->column_norm=datum_axis_z;
					else 
						pVertex->column_norm=-1.0*datum_axis_z;
					pVertex->radius=-Ri;
					if(B>Pi)
					{	//大于180度的椭圆弧 分两段来生成
						vertice.Set(0,Ri);
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=-Ri;
					}
				}
				else 
				{	//圆弧
					pVertex->sector_angle=-B;
					pVertex->center.Set();
				}
			}
		}
	}
	else
	{
		//一.确认含关联板的环向板装配坐标系
		//ucs.axis_z=GetRelaConnPlaneNorm(1,pItemHashTbl);
		keyvecset.huoqufacenorm=GetRelaConnPlaneNorm(2,pItemHashTbl);
		f3dPoint conn_wingvec[2];//两个关联连接面的翼展方向(与基准杆件垂直)
		conn_wingvec[0]=keyvecset.weldvec_w=GetRelaConnVec(1,pItemHashTbl);
		conn_wingvec[1]=keyvecset.weldvec_n=GetRelaConnVec(2,pItemHashTbl);
		if(!conn_wingvec[0].IsZero())
			project_vector(conn_wingvec[0],datum_axis_z);
		if(!conn_wingvec[1].IsZero())
			project_vector(conn_wingvec[1],datum_axis_z);
		//计算钢板Ｙ轴方向，即半圆型环向板肋板的位置
		//双关联钢板时，以凹角侧为内侧，凸角侧为外侧；
		//双关联钢板共面或单块关联钢板时，第一块关联钢板工作法向侧为外侧，
		if(Y>=0)
		{	//跨越环向板在关联钢板外侧
			if(Y>0)	//基板正侧
				verify_vec=pRelaPlate1->ucs.axis_z;
			else
			{	//自动判断
				if(pRelaPlate2==NULL||fabs(conn_wingvec[0]*conn_wingvec[1])>EPS_COS)
					verify_vec=pRelaPlate1->ucs.axis_z;
				else
				{
					f3dPoint bi_vec=conn_wingvec[0]+conn_wingvec[1];
					normalize(bi_vec);
					if(pRelaPlate1->ucs.axis_z*bi_vec>0)	//第一块钢板工作法向指凹角内侧
						verify_vec= pRelaPlate1->ucs.axis_z;
					else					//第一块钢板工作法向指凹角外侧，需反号后再赋值
						verify_vec= -pRelaPlate1->ucs.axis_z;
				}
			}
		}
		else
		{	//跨越环向板在关联钢板内侧
			verify_vec=-pRelaPlate1->ucs.axis_z;
			//以下代码用于自动判断内外侧，此处用户已指定在基板内侧无需自动判断 wht 11-05-17
			/*if(pRelaPlate2==NULL||fabs(conn_wingvec[0]*conn_wingvec[1])>EPS_COS2)
				verify_vec=-pRelaPlate1->ucs.axis_z;
			else
			{
				f3dPoint bi_vec=conn_wingvec[0]+conn_wingvec[1];
				if(pRelaPlate1->ucs.axis_z*bi_vec>0)	//第一块钢板工作法向指凹角内侧
					verify_vec= pRelaPlate1->ucs.axis_z;
				else									//第一块钢板工作法向指凹角外侧，需反号后再赋值
					verify_vec=-pRelaPlate1->ucs.axis_z;
			}*/
		}
		ucs.axis_x=keyvecset.weldvec_w;
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		if(ucs.axis_y*verify_vec<0)
		{
			ucs.axis_y*=-1.0;
			ucs.axis_z*=-1.0;
		}
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		normalize(ucs.axis_z);
		//确定坐标系后再根据偏移量计算坐标原点位置 wht 11-05-13
		if(fabs(S)>0)
		{
			if(datum_axis_z*ucs.axis_z>0)
				ucs.origin+=datum_axis_z*S;
			else 
				ucs.origin-=datum_axis_z*S;
		}
		//二.计算需要制弯时的制弯信息
		f3dLine huoquline;		//制弯线
		//fabs(huoqu_cosa)<EPS_COS表示用户分别指定了两块关联钢板的连接方向(焊接走向),需要制弯环向板
		double huoqu_cosa=1,huoqu_alfa=0;
		double ellipse_cosa_w=fabs(ucs.axis_z*datum_axis_z);//基面与基准钢管夹角余弦
		double ellipse_cosa_n=ellipse_cosa_w;				//制弯面与基准管夹角余弦
		if(!keyvecset.huoqufacenorm.IsZero())
			huoqu_cosa=ucs.axis_z*keyvecset.huoqufacenorm;
		if(fabs(huoqu_cosa)<EPS_COS)
		{	//需要制弯环向板
			keyvecset.huoquaxis_vec=ucs.axis_z^keyvecset.huoqufacenorm;	//预算火曲线方向
			f3dPoint weldvec1=keyvecset.weldvec_w;			//向第一块关联板侧的焊缝走向
			f3dPoint weldvec2=keyvecset.weldvec_n;
			project_vector(weldvec1,keyvecset.huoquaxis_vec);
			project_vector(weldvec2,keyvecset.huoquaxis_vec);
			f3dPoint justify_vec1=ucs.axis_z^keyvecset.huoqufacenorm;
			f3dPoint justify_vec2=weldvec1^weldvec2;
			if(justify_vec1*justify_vec2>0)
			{	//需要翻转制弯面工作法向
				keyvecset.huoqufacenorm*=-1.0;
				keyvecset.huoquaxis_vec*=-1.0;
				huoqu_cosa*=-1.0;
			}
			ellipse_cosa_n=fabs(keyvecset.huoqufacenorm*datum_axis_z);	//制弯面与基准管夹角余弦
			normalize(keyvecset.huoquaxis_vec);
			huoquline.startPt=ucs.origin;
			huoquline.endPt=ucs.origin+keyvecset.huoquaxis_vec;
			bIsCirRib3WA=FALSE;	//需要制弯的钢板不可能为3WA型
		}
		//三.计算不同关联构件情况下的内圆弧及制弯时内圆弧上的制弯点
		if(pDatumTube&&pRelaPlate1)
		{
			//计算钢板基准面拾取点 wht 11-06-03
			face_pick=GetPlateDatumFacePickPt(pRelaPlate1,ucs.axis_y*pRelaPlate1->ucs.axis_z>0);
			f3dPoint line_pick,line_vec=pRelaPlate1->ucs.axis_z;
			//将关联板法线投影到环向板内形成环向板内垂直于焊缝的一方向
			project_vector(line_vec,ucs.axis_z);
			//计算第一块关联板焊缝上的拾取点
			Int3dlf(line_pick,ucs.origin,line_vec,face_pick,pRelaPlate1->ucs.axis_z);
			Int3dlc(&vertexset.inner_arc_end,&inters,line_pick,ucs.axis_x,pDatumTube->Start(),pDatumTube->End(),Ri,Ri);
			verify_vec=vertexset.inner_arc_end-ucs.origin;
			if(verify_vec*ucs.axis_x<0)
				vertexset.inner_arc_end=inters;
			if(fabs(huoqu_cosa)<EPS_COS)
			{	//需要制弯
				Int3dlc(&vertexset.inner_arc_midhuoqu,&inters,ucs.origin,keyvecset.huoquaxis_vec,pDatumTube->Start(),pDatumTube->End(),Ri,Ri);
				verify_vec=vertexset.inner_arc_midhuoqu-ucs.origin;
				if(verify_vec*ucs.axis_y<0)
					vertexset.inner_arc_midhuoqu=inters;
				huoqu_alfa=ACOS(huoqu_cosa);
			}
			if(bIsCirRib3WA)
			{	//3WA型环向肋板在此处计算外圆弧起点 wht 11-05-24
				Int3dlc(&vertexset.weld_outer_w_arc,&inters,line_pick,ucs.axis_x,
					ucs.origin-ucs.axis_z*100,ucs.origin+ucs.axis_z*100,Ro,Ro);
				verify_vec=vertexset.weld_outer_w_arc-ucs.origin;
				if(verify_vec*ucs.axis_x<0)
					vertexset.weld_outer_w_arc=inters;
				vertexset.weld_outer_w=vertexset.weld_outer_w_h=vertexset.weld_outer_w_arc;
			}
		}
		if(pDatumTube&&(pRelaPlate2||pRelaTube
			||(pRelaPlate2==NULL&&pRelaTube==NULL&&pTransPlate)))
		{	//存在第二个关联构件或者不存在第二个关联构件但第一块关联钢板为贯通板 wht 11-05-20
			keyvecset.weldvec_nw=keyvecset.weldvec_n;	//制弯面焊缝走向在基面内的展开方向
			RotateVectorAroundVector(keyvecset.weldvec_nw,-huoqu_alfa,keyvecset.huoquaxis_vec);	//将制弯面焊缝走向绕火曲线展开至基面
			keyvecset.weldvec_nw_vert=keyvecset.weldvec_nw;
			RotateVectorAroundVector(keyvecset.weldvec_nw_vert,-0.5*Pi,ucs.axis_z);	//在基面内将展开到基面的制弯面焊缝逆时针转90°指向环向板面内
			keyvecset.weldvec_n_vert=keyvecset.weldvec_nw_vert;
			RotateVectorAroundVector(keyvecset.weldvec_n_vert, huoqu_alfa,keyvecset.huoquaxis_vec);	//将基面内展开的制弯面法线绕火曲线复面到制弯面
			f3dPoint line_pick;
			if(pRelaPlate2)
			{
				face_pick=pRelaPlate2->ucs.origin;
				//计算钢板基准面拾取点 wht 11-06-03
				face_pick=GetPlateDatumFacePickPt(pRelaPlate2,keyvecset.weldvec_n_vert*pRelaPlate2->ucs.axis_z>0);
				f3dPoint line_vec=pRelaPlate2->ucs.axis_z;
				//将关联板法线投影到环向板制弯面内形成环向板内垂直于焊缝的一方向
				project_vector(line_vec,keyvecset.huoqufacenorm);
				//计算第二块关联板焊缝上的拾取点
				Int3dlf(line_pick,ucs.origin,line_vec,face_pick,pRelaPlate2->ucs.axis_z);
			}
			else if(pRelaTube)
			{
				//f3dPoint top_direct=keyvecset.huoqufacenorm^keyvecset.weldvec_n;
				//top_direct计算不准确，直接使用keyvecset.weldvec_n_vert即可 wth 11-05-19
				line_pick=pRelaTube->Start()+keyvecset.weldvec_n_vert*(pRelaTube->GetDiameter()*0.5);
			}
			else //if(pRelaPlate2==NULL&&pRelaTube==NULL&&pTransPlate)
			{	//不存在第二个关联构件但第一块关联钢板为贯通板 wht 11-05-20
				//计算钢板基准面拾取点 wht 11-06-03
				face_pick=GetPlateDatumFacePickPt(pRelaPlate1,ucs.axis_y*pRelaPlate1->ucs.axis_z>0);
				f3dPoint line_vec=pRelaPlate1->ucs.axis_z;
				//将关联板法线投影到环向板内形成环向板内垂直于焊缝的一方向
				project_vector(line_vec,ucs.axis_z);
				//计算第一块关联板焊缝上的拾取点
				Int3dlf(line_pick,ucs.origin,line_vec,face_pick,pRelaPlate1->ucs.axis_z);
			}
			Int3dlc(&vertexset.inner_arc_start,&inters,line_pick,keyvecset.weldvec_n,pDatumTube->Start(),pDatumTube->End(),Ri,Ri);
			verify_vec=vertexset.inner_arc_start-ucs.origin;
			if(verify_vec*keyvecset.weldvec_n<0)
				vertexset.inner_arc_start=inters;
			if(fabs(huoqu_cosa)<EPS_COS)//将顶点坐标由制弯面转换到基面上
				rotate_point_around_axis(vertexset.inner_arc_start,-huoqu_alfa,huoquline);
			if(bIsCirRib3WA&&pRelaPlate2)
			{	//3WA型环向肋板在此处计算外圆弧起点 wht 11-05-24
				Int3dlc(&vertexset.weld_outer_n_arc,&inters,line_pick,keyvecset.weldvec_n,
					ucs.origin-ucs.axis_z*100,ucs.origin+ucs.axis_z*100,Ro,Ro);
				verify_vec=vertexset.weld_outer_n_arc-ucs.origin;
				if(verify_vec*keyvecset.weldvec_n<0)
					vertexset.weld_outer_n_arc=inters;
				vertexset.weld_outer_n=vertexset.weld_outer_n_h=vertexset.weld_outer_n_arc;
			}
		}
		else if(pDatumTube&&pTransPlate&&N>0)
		{	//第一块钢板为贯通板且N>0 该代码暂时可能执行不到，之前已经处理过第一块钢板为贯通板的情况 wht 11-06-03
			vertexset.inner_arc_start=ucs.origin*2-vertexset.inner_arc_end;
			keyvecset.weldvec_n=keyvecset.weldvec_nw=-ucs.axis_x;
		}
		else if(pRelaPlate2==NULL&&pRelaTube==NULL)
		{	//两条焊接边的环向肋板
			if(G>0)	//肋板2WL
			{	//自由边为直线
				if(D<=0&&B>0)
				{	//扇形角定制外形
					vertexset.inner_arc_start=vertexset.inner_arc_end;
					rotate_point_around_axis(vertexset.inner_arc_start,B,ucs.origin,ucs.origin+ucs.axis_z*100);
				}
				else if(D>0&&D<Ri)
				{	//自由边距定制外形
					double cos_len=sqrt(Ri*Ri-D*D);
					vertexset.inner_arc_start=ucs.origin+ucs.axis_x*cos_len+ucs.axis_y*D;
				}
				else
					vertexset.inner_arc_start=ucs.origin+ucs.axis_y*Ri;
			}
			else	//肋板2WA
			{	//自由边为圆弧
				vertexset.inner_arc_start=vertexset.inner_arc_end;
				rotate_point_around_axis(vertexset.inner_arc_start,B,ucs.origin,ucs.origin+ucs.axis_z*100);
			}
		}
		if(fabs(huoqu_cosa)>=EPS_COS) //不需要制弯
			vertexset.inner_arc_midhuoqu=vertexset.inner_arc_start;
		CONNECT_DIRECTION *pDirect=GetRelaConnInfo(1);
		if(pDirect&&!keyvecset.weldvec_w.IsZero())
		{
			double max_weld_dist=0,max_vert_dist=0,weld_dist=0,vert_dist=0;	//相关连接螺栓距离原点的焊缝方向及焊缝垂直方向的距离投影
			for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			{
				if((*pLsRef)->m_bVirtualPart)
					continue;	//不考虑虚拟螺栓，虚拟螺栓只为生成螺栓孔不影响外形 wht 11-07-08
				double dd=(*pLsRef)->ucs.axis_z*ucs.axis_z;
				f3dPoint perp,weld_vec,vert_vec;
				if(fabs(dd)>EPS_COS)		//该面上的连接螺栓
				{
					Int3dlf(inters,(*pLsRef)->ucs.origin,(*pLsRef)->ucs.axis_z,ucs.origin,ucs.axis_z);
					SnapPerp(&perp,ucs.origin,ucs.origin+ucs.axis_x,inters);
					vert_vec=inters-perp;
					vert_dist=vert_vec*ucs.axis_y;
					weld_vec=perp-vertexset.inner_arc_end;
					weld_dist=weld_vec*ucs.axis_x;
					if(vert_dist>max_vert_dist&&vert_dist<200)
						max_vert_dist=vert_dist;
					if(weld_dist>max_weld_dist) //计算最大焊缝长度 wht 11-05-30
						max_weld_dist=weld_dist;
				}
			}
			if(max_vert_dist>0)
				Hw=max_vert_dist+pDirect->high_space-(vertexset.inner_arc_end-ucs.origin)*ucs.axis_y;
			if(max_weld_dist>0&&max_weld_dist+pDirect->end_space>W)
				W=max_weld_dist+pDirect->end_space;
		}
		pDirect=GetRelaConnInfo(2);
		if(pDirect&&!keyvecset.weldvec_n.IsZero())
		{
			double max_weld_dist=0,max_vert_dist=0,weld_dist=0,vert_dist=0;	//相关连接螺栓距离原点的焊缝方向及焊缝垂直方向的距离投影
			for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			{
				if((*pLsRef)->m_bVirtualPart)
					continue;	//不考虑虚拟螺栓，虚拟螺栓只为生成螺栓孔不影响外形 wht 11-07-08
				f3dPoint perp,weld_vec,vert_vec;
				f3dPoint plane_norm=ucs.axis_z;
				double dd=(*pLsRef)->ucs.axis_z*ucs.axis_z;
				if(fabs(huoqu_cosa)<EPS_COS)	//需要制弯
				{
					dd=(*pLsRef)->ucs.axis_z*keyvecset.huoqufacenorm;
					plane_norm=keyvecset.huoqufacenorm;
				}
				if(fabs(dd)>EPS_COS)	//该面上的连接螺栓
				{
					Int3dlf(inters,(*pLsRef)->ucs.origin,(*pLsRef)->ucs.axis_z,ucs.origin,plane_norm);
					SnapPerp(&perp,ucs.origin,ucs.origin+keyvecset.weldvec_n,inters);
					vert_vec=inters-perp;
					vert_dist=vert_vec*keyvecset.weldvec_n_vert;
					if(fabs(huoqu_cosa)<EPS_COS)	//需要制弯
						perp=SpreadTransV(perp,true);
					weld_vec=perp-vertexset.inner_arc_start;
					weld_dist=weld_vec*keyvecset.weldvec_nw;
					if(vert_dist>max_vert_dist&&vert_dist<200)
						max_vert_dist=vert_dist;
					if(weld_dist>max_weld_dist)
						max_weld_dist=weld_dist;
				}
			}
			if(max_vert_dist>0)
				Hn=max_vert_dist+pDirect->high_space-(vertexset.inner_arc_start-ucs.origin)*keyvecset.weldvec_nw_vert;
			if(max_weld_dist>0&&max_weld_dist+pDirect->end_space>N)
				N=max_weld_dist+pDirect->end_space;
		}
		if(!bIsCirRib3WA)
		{	//四.计算关联焊缝线顶点、自由边（顶部直线或圆弧）上的顶点(含制弯点)
			f3dPoint mid_vec=keyvecset.weldvec_w+keyvecset.weldvec_n;
			normalize(mid_vec);
			BOOL bNeedJudgeOuterCirTanPt=FALSE;	//存在两个关联构件且为内侧肋板时需要判断外圆弧切点是否合法 wht 11-05-23
			if(pRelaPlate1&&(pRelaPlate2||pRelaTube)&&ucs.axis_y*mid_vec>0	//存在两个关联构件且为内侧环向肋板
				&&fabs(keyvecset.weldvec_w*keyvecset.weldvec_n)<EPS_COS		//两基准构件延伸方向夹角不等于180°
				&&fabs(huoqu_cosa)>EPS_COS)									//不需要制弯时才需判断切点是否合法 wht 11-05-31	
				bNeedJudgeOuterCirTanPt=TRUE;
			if(pDatumTube&&pRelaPlate1)
			{	//不是异型法兰
				vertexset.weld_outer_w=vertexset.inner_arc_end+ucs.axis_x*W;
				keyvecset.weldvec_w_vert=ucs.axis_x;
				RotateVectorAroundVector(keyvecset.weldvec_w_vert,0.5*Pi,ucs.axis_z);
				//计算环向板的各关键顶点
				if(H<=0&&Hw<=0&&D>EPS)
				{	//未指定倒角高度
					if(G<=0)
					{	//顶部为圆弧连接
						vertexset.weld_outer_w_h=vertexset.weld_outer_w;
						if(W-D>EPS)
						{	//自由边外圆弧自焊缝线端顶点向外圆弧的公切点开始
							f2dCircle retPt,cir(Ro,f2dPoint(0,0));
							f3dPoint vertice=vertexset.weld_outer_w;
							coord_trans(vertice,ucs,FALSE);
							f2dPoint pickPt(vertice.x,vertice.y+30),tanPt;
							if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,pickPt,tanPt))					
							{
								vertexset.weld_outer_w_arc.Set(tanPt.x,tanPt.y);
								coord_trans(vertexset.weld_outer_w_arc,ucs,TRUE);
								//存在两个关联构件且为内侧肋板时需要判断外圆弧切点是否合法 wht 11-05-23
								if(bNeedJudgeOuterCirTanPt)
								{	//
									f3dPoint w_arc_vec=vertexset.weld_outer_w_arc-vertexset.weld_outer_w_h;
									normalize(w_arc_vec);
									if(mid_vec*w_arc_vec<0) //切点不合理,顶部圆弧连接自动退化为直线连接
										vertexset.weld_outer_w_arc=vertexset.weld_outer_w_h;
								}
							}
							else
								vertexset.weld_outer_w_arc=vertexset.weld_outer_w_h;
						}
						else
						{	
							vertexset.weld_outer_w_arc=vertexset.weld_outer_w_h;
							vertexset.weld_outer_huoqu=vertexset.weld_outer_w_arc;
						}
					}
					else //启用参数D
					{	//顶部为直边
						//计算两关联板焊缝的倒角高Hw,Hn(若需要强制Hw=Hn,最好直接指定H,否则不能保证)
						keyvecset.freevec=(vertexset.inner_arc_end-vertexset.inner_arc_start).normalized();
						keyvecset.freevec_vert=keyvecset.freevec;
						RotateVectorAroundVector(keyvecset.freevec_vert,Pi/2.0,ucs.axis_z);
						//
						f3dPoint free_edge_pick;
						Int3dlc(&free_edge_pick,&inters,ucs.origin,keyvecset.freevec_vert,pDatumTube->Start(),pDatumTube->End(),Ri+D,Ri+D);
						if((free_edge_pick-ucs.origin)*keyvecset.freevec_vert<0)
							free_edge_pick=inters;
						Int3dll(free_edge_pick,free_edge_pick+keyvecset.freevec,
							vertexset.weld_outer_w,vertexset.weld_outer_w+keyvecset.weldvec_w_vert,vertexset.weld_outer_w_h);
						vertexset.weld_outer_w_arc=vertexset.weld_outer_w_h;
					}
				}
				else //if(H>0)	//启用参数H
				{	//顶部为直线连接
					Hw=max(Hw,H);
					if(G<=0)
					{	//顶部为圆弧连接
						if(pRelaPlate1&&pTransPlate==NULL		  //非贯通板
							&&pRelaPlate2==NULL&&pRelaTube==NULL) //2WA W侧不考虑H参数 wht 11-05-20
							vertexset.weld_outer_w_arc=vertexset.weld_outer_w_h=vertexset.weld_outer_w;
						else 
						{
							vertexset.weld_outer_w_h=vertexset.weld_outer_w+keyvecset.weldvec_w_vert*Hw;
							f3dPoint basePt=vertexset.weld_outer_w_h;
							coord_trans(basePt,ucs,FALSE);	//转换到钢板坐标系下
							f2dPoint pickPt(basePt.x,basePt.y+Hw),tanPt;
							f2dCircle cir(Ro,f2dPoint(0,0));
							if(Tang2dpc(f2dPoint(basePt.x,basePt.y),cir,pickPt,tanPt)>0)
							{
								vertexset.weld_outer_w_arc.Set(tanPt.x,tanPt.y);
								coord_trans(vertexset.weld_outer_w_arc,ucs,TRUE);
								//存在两个关联构件且为内侧肋板时需要判断外圆弧切点是否合法 wht 11-05-23
								if(bNeedJudgeOuterCirTanPt)
								{	//
									f3dPoint w_arc_vec=vertexset.weld_outer_w_arc-vertexset.weld_outer_w_h;
									normalize(w_arc_vec);
									if(mid_vec*w_arc_vec<0) //切点不合理,顶部圆弧连接自动退化为直线连接
										vertexset.weld_outer_w_arc=vertexset.weld_outer_w_h; 
								}
							}
							else
								vertexset.weld_outer_w_arc=vertexset.weld_outer_w_h;
						}
					}
					else	//H决定D
					{	
						vertexset.weld_outer_w_h=vertexset.weld_outer_w+keyvecset.weldvec_w_vert*Hw;
						vertexset.weld_outer_w_arc=vertexset.weld_outer_w_h;
					}
				}
			}
			if(!keyvecset.weldvec_n.IsZero())
			{	//有第二个关联方向
				if(fabs(huoqu_cosa)>=EPS_COS)	//不需要制弯
					keyvecset.weldvec_nw=keyvecset.weldvec_n;
				vertexset.weld_outer_n=vertexset.inner_arc_start+keyvecset.weldvec_nw*N;
				//计算环向板的各关键顶点
				if(h<=0&&Hn<=0&&D>EPS)
				{	//未指定倒角高度
					if(G<=0)
					{	//顶部为圆弧连接
						vertexset.weld_outer_n_h=vertexset.weld_outer_n;	//H<=0 weld_outer_n_h与weld_outer_n重合 wht 11-05-20
						if(N-D>EPS)
						{	//自由边外圆弧自焊缝线端顶点向外圆弧的公切点开始
							f2dCircle retPt,cir(Ro,f2dPoint(0,0));
							f3dPoint vertice=vertexset.weld_outer_n;
							f3dPoint pickPt=vertice+keyvecset.weldvec_nw_vert*30;
							coord_trans(vertice,ucs,FALSE);
							coord_trans(pickPt,ucs,FALSE);
							f2dPoint tanPt;
							if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,f2dPoint(pickPt.x,pickPt.y),tanPt))
							{
								vertexset.weld_outer_n_arc.Set(tanPt.x,tanPt.y);
								coord_trans(vertexset.weld_outer_n_arc,ucs,TRUE);
								//存在两个关联构件且为内侧肋板时需要判断外圆弧切点是否合法 wht 11-05-23
								if(bNeedJudgeOuterCirTanPt)
								{
									f3dPoint n_arc_vec=vertexset.weld_outer_n_arc-vertexset.weld_outer_n_h;
									normalize(n_arc_vec);
									if(mid_vec*n_arc_vec<0) //切点不合理,顶部圆弧连接自动退化为直线连接
										vertexset.weld_outer_n_arc=vertexset.weld_outer_n_h; 
								}
							}
							else
								vertexset.weld_outer_n_arc=vertexset.weld_outer_n_h;
						}
						else
							vertexset.weld_outer_n_arc=vertexset.weld_outer_n_h;
					}
					else //启用参数D
					{	//顶部为直边
						f3dPoint free_edge_pick;
						Int3dlc(&free_edge_pick,&inters,ucs.origin,keyvecset.freevec_vert,pDatumTube->Start(),pDatumTube->End(),Ri+D,Ri+D);
						if((free_edge_pick-ucs.origin)*keyvecset.freevec_vert<0)
							free_edge_pick=inters;
						Int3dll(free_edge_pick,free_edge_pick+keyvecset.freevec,
							vertexset.weld_outer_n,vertexset.weld_outer_n+keyvecset.weldvec_nw_vert,vertexset.weld_outer_n_h);
						vertexset.weld_outer_n_arc=vertexset.weld_outer_n_h;
					}
				}
				else //if(H>0)	//启用参数H
				{	//顶部为直线连接
					Hn=max(Hn,h);
					if(G<=0)
					{	//顶部为圆弧连接
						vertexset.weld_outer_n_h=vertexset.weld_outer_n+keyvecset.weldvec_nw_vert*Hn;
						f3dPoint basePt=vertexset.weld_outer_n_h;
						f3dPoint pickPt=vertexset.weld_outer_n_h+keyvecset.weldvec_nw_vert*Hn;
						coord_trans(basePt,ucs,FALSE);	//转换到钢板坐标系下
						coord_trans(pickPt,ucs,FALSE);
						f2dPoint tanPt;
						f2dCircle cir(Ro,f2dPoint(0,0));
						if(Tang2dpc(f2dPoint(basePt.x,basePt.y),cir,f2dPoint(pickPt.x,pickPt.y),tanPt))
						{
							vertexset.weld_outer_n_arc.Set(tanPt.x,tanPt.y);
							coord_trans(vertexset.weld_outer_n_arc,ucs,TRUE);
							//存在两个关联构件且为内侧肋板时需要判断外圆弧切点是否合法 wht 11-05-23
							if(bNeedJudgeOuterCirTanPt)
							{
								f3dPoint n_arc_vec=vertexset.weld_outer_n_arc-vertexset.weld_outer_n_h;
								normalize(n_arc_vec);
								if(mid_vec*n_arc_vec<0) //切点不合理,顶部圆弧连接自动退化为直线连接
									vertexset.weld_outer_n_arc=vertexset.weld_outer_n_h; 
							}
						}
						else
							vertexset.weld_outer_n_arc=vertexset.weld_outer_n_h;
					}
					else	//H决定D
					{	
						vertexset.weld_outer_n_h=vertexset.weld_outer_n+keyvecset.weldvec_nw_vert*Hn;
						vertexset.weld_outer_n_arc=vertexset.weld_outer_n_h;
					}
				}
			}
			else
			{	//没有指定第二个关联方向，即单关联板单侧环向肋板(不支持火曲)
				if(G>0)
				{	//自由边为直线
					if(D==0&&B>0)	//扇形角定制外形
					{
						keyvecset.weldvec_n=ucs.axis_x;
						RotateVectorAroundVector(keyvecset.weldvec_n,B,ucs.axis_z);
						//keyvecset.weldvec_n_vert=keyvecset.weldvec_n;
						//RotateVectorAroundVector(keyvecset.weldvec_n_vert,-0.25*Pi,ucs.axis_z);
						vertexset.weld_outer_n=vertexset.inner_arc_start+keyvecset.weldvec_n*h;
						vertexset.weld_outer_w_arc=vertexset.weld_outer_huoqu=vertexset.weld_outer_n_arc=
							vertexset.weld_outer_n_h=vertexset.weld_outer_n;
					}
					else
						vertexset.weld_outer_w_arc=vertexset.weld_outer_huoqu=vertexset.weld_outer_n_arc=
							vertexset.weld_outer_n_h=vertexset.weld_outer_n=vertexset.inner_arc_start;
				}
				else
				{	//自由边为圆弧
					keyvecset.weldvec_n=ucs.axis_x;
					RotateVectorAroundVector(keyvecset.weldvec_n,B,ucs.axis_z);
					//2WA weld_outer_n wht 11-05-20
					//根据用户反馈H含义由原倒角高改为台口高,倒角高+台口高=W wjh-2011.11.22
					vertexset.weld_outer_n=vertexset.inner_arc_start+keyvecset.weldvec_n*h;//(W-H);
					if(h<W)//H>0)	//有倒角存在 N<Ro-Ri 
					{
						keyvecset.weldvec_n_vert=keyvecset.weldvec_n;
						RotateVectorAroundVector(keyvecset.weldvec_n_vert,-gama*RADTODEG_COEF,ucs.axis_z);	//-0.25*Pi
						//通过与圆求交点计算 vertexset.weld_outer_n_h wht 11-05-20
						f3dPoint cut_angle_vec=keyvecset.weldvec_n_vert;
						vector_trans(cut_angle_vec,ucs,FALSE);
						f3dPoint weld_outer_n=vertexset.weld_outer_n;
						coord_trans(weld_outer_n,ucs,FALSE);
						cut_angle_vec.z=weld_outer_n.z=0;
						f3dLine cut_line;
						cut_line.startPt=weld_outer_n;
						cut_line.endPt=weld_outer_n+cut_angle_vec*10000;
						f2dLine line;
						line.startPt.Set(cut_line.startPt.x,cut_line.startPt.y);
						line.endPt.Set(cut_line.endPt.x,cut_line.endPt.y);
						f3dPoint inters_pt1,inters_pt2;
						if(Int2dlc(line,f2dCircle(Ro,0,0),inters_pt1.x,inters_pt1.y,inters_pt2.x,inters_pt2.y)>0)
						{
							f3dPoint vec1=inters_pt1-weld_outer_n;
							if(vec1*cut_angle_vec>0)
								vertexset.weld_outer_n_arc=inters_pt1;
							else 
								vertexset.weld_outer_n_arc=inters_pt2;
							coord_trans(vertexset.weld_outer_n_arc,ucs,TRUE);
							vertexset.weld_outer_n_h=vertexset.weld_outer_n_arc;
						}
						else 
							vertexset.weld_outer_n_arc=vertexset.weld_outer_n_h=vertexset.weld_outer_n;
						//double offset=sqrt(Ro*Ro-(Ri+H)*(Ri+H)*80.5)-0.707106781*(Ri+H);
						//vertexset.weld_outer_n_arc=vertexset.weld_outer_n_h=vertexset.weld_outer_n+keyvecset.weldvec_n_vert*offset;
					}
					else
						vertexset.weld_outer_n_arc=vertexset.weld_outer_n_h=vertexset.weld_outer_n;
				}
			}
		}
		//计算自由边上的制弯点
		BYTE cOuterHuoquPos=0;	//0.圆弧内 1.W侧直线边 2.N侧直线边
		if(fabs(huoqu_cosa)<EPS_COS)
		{
			if(G<=0&&Ro>0)
			{	//自由边外轮廓为圆弧
				/*double cosa= keyvecset.huoquaxis_vec*ucs.axis_x;
				if(keyvecset.huoquaxis_vec.y<0)
					cosa*=-1.0;
				double sina=sqrt(max(0,1-cosa*cosa));
				vertexset.weld_outer_huoqu=ucs.origin+(Ro*cosa)*ucs.axis_x+(Ro*sina)*ucs.axis_y;*/
				//计算外侧圆弧上的火曲点 wht 11-05-31
				f3dPoint outer_huoqu_vec=vertexset.inner_arc_midhuoqu-ucs.origin;
				normalize(outer_huoqu_vec);
				f3dPoint vec_arr[3]={vertexset.weld_outer_w_arc-ucs.origin,	//X轴方向为W侧焊接方向
									 vertexset.weld_outer_n_arc-ucs.origin,
									 outer_huoqu_vec};
				normalize(vec_arr[0]);
				normalize(vec_arr[1]);
				double angleArr[3]={0,0,0};
				for(int i=0;i<3;i++)
				{
					vector_trans(vec_arr[i],ucs,FALSE);
					angleArr[i]=Cal2dLineAng(0,0,vec_arr[i].x,vec_arr[i].y);
				}
				if(angleArr[2]<angleArr[0])
				{	//制弯点在W侧(X+方向)直线边上
					cOuterHuoquPos=1;
					if(Int3dpl(f3dLine(vertexset.weld_outer_w_h,vertexset.weld_outer_w_arc),huoquline,inters)>0)
						vertexset.weld_outer_huoqu=inters;
					//else	//制弯线不合理
				}
				else if(angleArr[2]>angleArr[1])
				{	//制弯点在N侧(X-方向)直线边上
					cOuterHuoquPos=2;
					if(Int3dpl(f3dLine(vertexset.weld_outer_n_h,vertexset.weld_outer_n_arc),huoquline,inters)>0)
						vertexset.weld_outer_huoqu=inters;
					//else	//制弯线不合理
				}
				else //制弯点在圆弧边上
					vertexset.weld_outer_huoqu=ucs.origin+outer_huoqu_vec*Ro;
			}
			else
			{	//自由边为直线，保证最小自由边距
				f3dLine free_edge_line(vertexset.weld_outer_w_h,vertexset.weld_outer_n_h);
				if(Int3dpl(free_edge_line,huoquline,inters)>0)
				{
					vertexset.weld_outer_huoqu=inters;
					vertexset.weld_outer_n_arc=vertexset.weld_outer_w_arc=vertexset.weld_outer_huoqu;
				}
				//else	//制弯线不合理
			}
		}
		else	//没有制弯
			vertexset.weld_outer_huoqu=vertexset.weld_outer_w_arc;
		//五.根据上面计算出的外形关键点生成环向的外形数据
		//第一个顶点
		vertice=vertexset.inner_arc_end;
		coord_trans(vertice,ucs,FALSE);
		vertice.z=0;
		vertice.feature=1;
		PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
		pVertex->m_bWeldEdge=TRUE;
		if(pRelaPlate1)
			pVertex->m_hRod=rela_part1.hPart;	//
		//第二个顶点
		vertice=vertexset.weld_outer_w;
		coord_trans(vertice,ucs,FALSE);
		vertice.z=0;
		vertice.feature=1;
		pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
		if(G<=0&&vertexset.weld_outer_w_h==vertexset.weld_outer_w
			&&vertexset.weld_outer_w_arc==vertexset.weld_outer_w_h)
		{	
			//if(fabs(huoqu_cosa)>EPS_COS&&vertexset.weld_outer_huoqu==vertexset.weld_outer_w_arc) 
			if(bIsCirRib3WA) //仅3WA型钢板判断w_r==n_r wht 11-06-07
			{	//自由边为圆弧且仅有四个顶点 wht 11-04-20 
				double w_r=vertice.mod();
				vertice=vertexset.weld_outer_n_arc;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				double n_r=vertice.mod();
				if(fabs(w_r-n_r)<EPS&&fabs(w_r-Ro)<EPS)
				{	//两点到圆心的距离相等且等于Ro时才可能为圆弧 wht 11-05-24
					pVertex->radius=Ro;
					pVertex->type=2;
				}
			}
			else 
			{	//不用判断w_r等于n_r wht 11-06-07
				pVertex->radius=Ro;
				pVertex->type=2;
			}
		}
		//第三个顶点
		if(vertexset.weld_outer_w_h!=vertexset.weld_outer_w)
		{
			vertice=vertexset.weld_outer_w_h;
			coord_trans(vertice,ucs,FALSE);
			vertice.z=0;
			vertice.feature=1;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
		}
		//制弯点在圆弧边上
		if(0==cOuterHuoquPos)
		{	//第四个顶点
			if(vertexset.weld_outer_w_arc!=vertexset.weld_outer_w_h)
			{
				vertice=vertexset.weld_outer_w_arc;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				//if(vertexset.weld_outer_w_arc!=vertexset.weld_outer_huoqu)
				if(G<=0)
				{	//自由边为圆弧
					pVertex->radius=Ro;
					pVertex->type=2;
				}
				else if(fabs(huoqu_cosa)<EPS_COS)
					pVertex->vertex.feature=12;	//外自由边为直线，圆弧起止点与制弯点重合
			}
			//第五个顶点(外自由边上制弯点)
			if(vertexset.weld_outer_huoqu!=vertexset.weld_outer_w_arc)
			{	//有制弯点时,暂不支持自由边为圆弧
				vertice=vertexset.weld_outer_huoqu;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				if(fabs(huoqu_cosa)<EPS_COS)
					vertice.feature=12;
				else
					vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				if(vertexset.weld_outer_huoqu!=vertexset.weld_outer_n_arc)
				{
					pVertex->radius=Ro;
					pVertex->type=2;
				}
			}
			//第六个顶点
			if(vertexset.weld_outer_n_arc!=vertexset.weld_outer_huoqu)
			{	
				vertice=vertexset.weld_outer_n_arc;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				if(fabs(huoqu_cosa)<EPS_COS)
					vertice.feature=2;	//制弯面上顶点
				else
					vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
			}
		}
		//制弯点在W侧(X-方向)直线边上，只有钢板需要制弯时cOuterHuoquPos才会等于1或等于2 wht 15-12-24
		else if(1==cOuterHuoquPos)
		{	//第四个顶点(外自由边上制弯点)
			if(vertexset.weld_outer_huoqu!=vertexset.weld_outer_w_h)
			{	//有制弯点时,暂不支持自由边为圆弧
				vertice=vertexset.weld_outer_huoqu;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				if(fabs(huoqu_cosa)<EPS_COS)
					vertice.feature=12;
				else
					vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
			}
			//第五个顶点
			if(vertexset.weld_outer_w_arc!=vertexset.weld_outer_huoqu)
			{
				vertice=vertexset.weld_outer_w_arc;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=2;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				if(G<=0)
				{	//自由边为圆弧
					pVertex->radius=Ro;
					pVertex->type=2;
				}
			}
			//第六个顶点
			if(vertexset.weld_outer_n_arc!=vertexset.weld_outer_w_arc)
			{	
				vertice=vertexset.weld_outer_n_arc;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=2;	//制弯面上顶点
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
			}
		}
		//制弯点在N侧(X+方向)直线边上
		else if(2==cOuterHuoquPos)
		{
			//第四个顶点
			if(vertexset.weld_outer_w_arc!=vertexset.weld_outer_w_h)
			{
				vertice=vertexset.weld_outer_w_arc;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				if(G<=0)
				{	//自由边为圆弧
					pVertex->radius=Ro;
					pVertex->type=2;
				}
			}
			//第五个顶点
			if(vertexset.weld_outer_n_arc!=vertexset.weld_outer_w_arc)
			{	
				vertice=vertexset.weld_outer_n_arc;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
			}
			//第六个顶点(外自由边上制弯点)
			if(vertexset.weld_outer_huoqu!=vertexset.weld_outer_n_arc)
			{	
				vertice=vertexset.weld_outer_huoqu;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=12;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
			}
			
		}
		//第七个顶点
		if(vertexset.weld_outer_n_h!=vertexset.weld_outer_n_arc)
		{
			vertice=vertexset.weld_outer_n_h;
			coord_trans(vertice,ucs,FALSE);
			vertice.z=0;
			if(fabs(huoqu_cosa)<EPS_COS)
				vertice.feature=2;	//需要制弯
			else
				vertice.feature=1;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
			if(vertexset.weld_outer_n_h==vertexset.weld_outer_n)
				pVertex->m_bWeldEdge=TRUE;
		}
		//第八个顶点
		if(vertexset.weld_outer_n!=vertexset.weld_outer_n_h
			&&vertexset.weld_outer_n!=vertexset.inner_arc_start)
		{
			vertice=vertexset.weld_outer_n;
			coord_trans(vertice,ucs,FALSE);
			vertice.z=0;
			if(fabs(huoqu_cosa)<EPS_COS)
				vertice.feature=2;	//需要制弯
			else
				vertice.feature=1;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
			pVertex->m_bWeldEdge=TRUE;
		}
		//第九个顶点(内圆弧起点)
		vertice=vertexset.inner_arc_start;
		coord_trans(vertice,ucs,FALSE);
		vertice.z=0;
		if(fabs(huoqu_cosa)<EPS_COS)
			vertice.feature=2;	//需要制弯
		else
			vertice.feature=1;
		pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
		pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
		pVertex->radius=-Ri;
		if(pRelaPlate2 || pRelaTube)
			pVertex->m_hRod=rela_part2.hPart;
		else if(pTransPlate)
			pVertex->m_hRod=rela_part1.hPart;
		if(ellipse_cosa_n>=EPS_COS)
			pVertex->type=2;
		else
		{	//椭圆弧
			pVertex->type=3;
			pVertex->center=ucs.origin;
			pVertex->column_norm=datum_axis_z;
		}
		//第十个顶点
		if(vertexset.inner_arc_midhuoqu!=vertexset.inner_arc_start)
		{
			vertice=vertexset.inner_arc_midhuoqu;
			coord_trans(vertice,ucs,FALSE);
			vertice.z=0;
			if(fabs(huoqu_cosa)<EPS_COS)
				vertice.feature=12;	//需要制弯
			else
				vertice.feature=1;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
			pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
			pVertex->radius=-Ri;
			if(ellipse_cosa_w>=EPS_COS)	//此处应使用基面与基准钢管夹角余弦ellipse_cosa_w wht 11-05-31
				pVertex->type=2;
			else
			{	//椭圆弧
				pVertex->type=3;
				pVertex->center=ucs.origin;
				pVertex->column_norm=datum_axis_z;
			}
		}
		//处理环向肋板的工艺流水孔（含半圆孔或圆孔两种类型）
		double BH=0;	//BH:流水孔直径(<0.无孔;0.半圆孔;1.圆孔)
		if(GetDesignItemValue(KEY2C("BH"),&BH,pItemHashTbl)&&fabs(BH)<EPS)
		{
			while(1)
			{
				int iSelVertex=0,num=vertex_list.GetNodeNum();
				for(iSelVertex=0;iSelVertex<num;iSelVertex++)
				{
					if(vertex_list[iSelVertex].m_hRod>0X20)
						break;
				}
				if(iSelVertex<0||iSelVertex>=num)
					break;
				//对选中轮廓点进行倒角操作
				double fCutR=(vertex_list[iSelVertex].m_hRod==pRelaPlate1->handle)?rela_part1.uiAttribute*0.1:rela_part2.uiAttribute*0.1;
				f3dPoint prePt =vertex_list[(iSelVertex-1+num)%num].vertex;
				f3dPoint cutPt =vertex_list[iSelVertex].vertex;
				f3dPoint nextPt=vertex_list[(iSelVertex+1)%num].vertex;
				f3dPoint preVec =(prePt-cutPt).normalized();
				f3dPoint nextVec=(nextPt-cutPt).normalized();
				//f3dPoint offVec =(preVec+nextVec).normalized();
				if(DISTANCE(prePt,cutPt)>fCutR && DISTANCE(nextPt,cutPt)>fCutR)
				{
					f3dPoint cutPt1,cutPt2;
					//计算第一个倒角点
					if(vertex_list[(iSelVertex-1+num)%num].type==0)
						cutPt1=cutPt+preVec*fCutR;
					else
					{
						f3dArcLine arcline;
						if(vertex_list[(iSelVertex-1+num)%num].type<3)
							vertex_list[(iSelVertex-1+num)%num].RetrieveArcLine(arcline,cutPt,&ucs);
						else
						{
							int sign=vertex_list[(iSelVertex-1+num)%num].radius<0?-1:1;
							f3dPoint columnNorm=vertex_list[(iSelVertex-1+num)%num].column_norm;
							f3dPoint center=vertex_list[(iSelVertex-1+num)%num].center;
							coord_trans(center,ucs,FALSE);
							vector_trans(columnNorm,ucs,FALSE);
							if(prePt.feature%10==2&&cutPt.feature%10==2)
							{	//制弯面上的椭圆弧，首先应将圆心标定点及柱面法线旋转展开至基面上再进行坐标系转换
								center=SpreadTransV(center);
								columnNorm=SpreadTransV(columnNorm,false);
							}
							arcline.CreateEllipse(center,prePt,cutPt,columnNorm,f3dPoint(0,0,sign),sign*vertex_list[(iSelVertex-1+num)%num].radius);
						}
						double R=arcline.Radius(),fLen=SQRT(R*R-fCutR*fCutR*0.25);
						double fAngle=arcline.SectorAngle()-ACOS(fLen/R)*2;
						cutPt1=arcline.PositionInAngle(fAngle);
						cutPt1.z=0;
					}
					//计算第二个倒角点
					if(vertex_list[iSelVertex].type==0)
						cutPt2=cutPt+nextVec*fCutR;
					else
					{
						f3dArcLine arcline;
						if(vertex_list[iSelVertex].type<3)
							vertex_list[iSelVertex].RetrieveArcLine(arcline,nextPt,&ucs);
						else
						{
							int sign=vertex_list[iSelVertex].radius<0?-1:1;
							f3dPoint columnNorm=vertex_list[iSelVertex].column_norm;
							f3dPoint center=vertex_list[iSelVertex].center;
							coord_trans(center,ucs,FALSE);
							vector_trans(columnNorm,ucs,FALSE);
							if(cutPt.feature%10==2&&nextPt.feature%10==2)
							{	//制弯面上的椭圆弧，首先应将圆心标定点及柱面法线旋转展开至基面上再进行坐标系转换
								center=SpreadTransV(center);
								columnNorm=SpreadTransV(columnNorm,false);
							}
							arcline.CreateEllipse(center,cutPt,nextPt,columnNorm,f3dPoint(0,0,sign),sign*vertex_list[iSelVertex].radius);
						}
						double R=arcline.Radius(),fLen=SQRT(R*R-fCutR*fCutR*0.25);
						double fAngle=ACOS(fLen/R)*2;
						cutPt2=arcline.PositionInAngle(fAngle);
						cutPt2.z=0;
					}
					//更新当前轮廓点
					PROFILE_VERTEX cut_vertex=vertex_list[iSelVertex];
					bool bWeld=cut_vertex.m_bWeldEdge;
					cutPt1.feature=cutPt2.feature=cut_vertex.vertex.feature;
					vertex_list[iSelVertex].vertex=cutPt1;
					vertex_list[iSelVertex].m_bWeldEdge=false;
					vertex_list[iSelVertex].m_hRod=0;
					vertex_list[iSelVertex].type=2;
					vertex_list[iSelVertex].radius=-fCutR;
					vertex_list[iSelVertex].center=cutPt;
					//插入一个新的轮廓点
					PROFILE_VERTEX* pVertex=vertex_list.insert(cut_vertex,(iSelVertex+1)%num);
					pVertex->vertex=cutPt2;
					pVertex->m_bWeldEdge=bWeld;
					pVertex->m_hRod=0;
				}
				else
					vertex_list[iSelVertex].m_hRod=0;
			}
		}
	}
	return TRUE;
}
//</DEVELOP_PROCESS_MARK>
BOOL CLDSParamPlate::DesignCircularRibPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	long h;
	GetDesignItemValue('A',&h,pItemHashTbl);	//定位基准节点
	CLDSNode *pBaseNode=BelongModel()->FromNodeHandle(h);
	GetDesignItemValue('B',&h,pItemHashTbl);		//第一块定位基准钢板
	CLDSPlate *pDatumPlate1=(CLDSPlate*)BelongModel()->FromPartHandle(h,CLS_PLATE);
	h=0;
	GetDesignItemValue('C',&h,pItemHashTbl);		//第二块定位基准钢板
	CLDSPlate *pDatumPlate2=(CLDSPlate*)BelongModel()->FromPartHandle(h,CLS_PLATE);
	CLDSPart *pPart=BelongModel()->FromPartHandle(m_hPartWeldParent);
	if(pBaseNode==NULL||pDatumPlate1==NULL||pPart==NULL)//||pDatumPlate2==NULL
		return FALSE;
	CLDSLineTube *pDatumTube=NULL;
	CLDSSphere *pDatumSphere=NULL;
	if(pPart->GetClassTypeId()==CLS_LINETUBE)
		pDatumTube=(CLDSLineTube*)pPart;
	else if(pPart->GetClassTypeId()==CLS_SPHERE)
		pDatumSphere=(CLDSSphere*)pPart;
	double W,D,S,R,N,H=-1;
	GetDesignItemValue('W',&W,pItemHashTbl);
	if(!GetDesignItemValue('N',&N,pItemHashTbl))
		N=W;
	GetDesignItemValue('D',&D,pItemHashTbl);
	GetDesignItemValue('S',&S,pItemHashTbl);
	GetDesignItemValue('R',&R,pItemHashTbl);
	GetDesignItemValue('H',&H,pItemHashTbl);
	long O=-1;
	BOOL bCir=FALSE;
	GetDesignItemValue('O',&O,pItemHashTbl);
	if(O>0)	//顶部为圆弧连接
	{
		bCir=TRUE;
		if(pDatumPlate1&&pDatumPlate2)	//两块基准钢板时要保证W==N
			N=W;	//保证N、W值相等顶部才可能为圆弧连接
	}
	//设定环向肋板坐标系原点及法线标准化
	normalize(ucs.axis_z);
	ucs.origin = pBaseNode->Position(true);
	f3dPoint datum_offset_vec=ucs.axis_z;
	if(pBaseNode&&pDatumTube)
	{
		pDatumTube->BuildUCS();
		SnapPerp(&ucs.origin,pDatumTube->Start(),pDatumTube->End(),pBaseNode->Position(true));	
		datum_offset_vec=pDatumTube->End()-pDatumTube->Start();
		normalize(datum_offset_vec);
		ucs.origin+=datum_offset_vec*S;
	}	
	//基准钢板在钢管垂直截面内的射线方向
	f3dPoint axis_x_1,axis_x_2;
	axis_x_1=ucs.axis_z^pDatumPlate1->ucs.axis_z;
	normalize(axis_x_1);
	if(pDatumPlate2)
	{
		axis_x_2=ucs.axis_z^pDatumPlate2->ucs.axis_z;
		normalize(axis_x_2);
	}
	//修正射线方向的正负号
	f3dPoint vertice_vec,vertice;
	if(pDatumPlate1&&pDatumPlate1->vertex_list.GetNodeNum()>0)
	{	//判断基准钢板是否为贯通连板 在钢板X轴一侧生成肋板 wht 10-03-22
		if(pDatumPlate1->IsTransPlate())	//贯通钢管的连板
			vertice_vec=pDatumPlate1->ucs.axis_x;
		else
		{
			vertice=pDatumPlate1->vertex_list.GetFirst()->vertex;
			coord_trans(vertice,pDatumPlate1->ucs,TRUE);
			vertice_vec=vertice-ucs.origin;
		}
		if(vertice_vec*axis_x_1<0)
			axis_x_1*=-1.0;
	}
	if(pDatumPlate2&&pDatumPlate2->vertex_list.GetNodeNum()>0)
	{	//判断基准钢板是否为贯通连板 在钢板X轴负方向一侧生成肋板 wht 10-03-22
		if(pDatumPlate2->IsTransPlate())	//贯通钢管的连板
			vertice_vec=-pDatumPlate2->ucs.axis_x;
		else
		{
			vertice=pDatumPlate2->vertex_list.GetFirst()->vertex;
			coord_trans(vertice,pDatumPlate2->ucs,TRUE);
			vertice_vec=vertice-ucs.origin;
		}
		if(vertice_vec*axis_x_2<0)
			axis_x_2*=-1.0;
	}
	if(pDatumPlate2&&pDatumTube)	//有两块基准钢板的环向肋板
	{
		//沿逆时针方向旋转排序两个基准钢板
		f3dPoint axis_z_verify=axis_x_1^axis_x_2;
		f3dPoint face_pick[2],face_norm1,face_norm2;
		BOOL bSwapDatumPlate=FALSE;
		if(axis_z_verify*ucs.axis_z<0)
		{	//第一块基准钢板在后，对调两个外形定位基准面
			face_norm1=ucs.axis_z^axis_x_2;
			normalize(face_norm1);
			face_pick[0]=pDatumPlate2->ucs.origin;
			if(face_norm1*pDatumPlate2->ucs.axis_z>0)
				face_pick[0]+=pDatumPlate2->ucs.axis_z*(pDatumPlate2->GetThick()+pDatumPlate2->m_fNormOffset);
			else
				face_pick[0]+=pDatumPlate2->ucs.axis_z*pDatumPlate2->m_fNormOffset;
			face_norm2=axis_x_1^ucs.axis_z;
			normalize(face_norm2);
			face_pick[1]=pDatumPlate1->ucs.origin;
			if(face_norm2*pDatumPlate1->ucs.axis_z>0)
				face_pick[1]+=pDatumPlate1->ucs.axis_z*(pDatumPlate1->GetThick()+pDatumPlate1->m_fNormOffset);
			else
				face_pick[1]+=pDatumPlate1->ucs.axis_z*pDatumPlate1->m_fNormOffset;
			//前边计算得到的face_norm1 face_norm2 仅用来确定方向	wht 09-06-15
			if(face_norm1*pDatumPlate2->ucs.axis_z<0)
				face_norm1=-pDatumPlate2->ucs.axis_z;
			else
				face_norm1=pDatumPlate2->ucs.axis_z;
			if(face_norm2*pDatumPlate1->ucs.axis_z<0)
				face_norm2=-pDatumPlate1->ucs.axis_z;
			else
				face_norm2=pDatumPlate1->ucs.axis_z;
			vertice=axis_x_1;
			axis_x_1=axis_x_2;
			axis_x_2=vertice;
			double tempW=W;	//对调W N 的值 确保W-对应第一块板，N-对应第二块板 
			W=N;
			N=tempW;
			bSwapDatumPlate=TRUE;
		}
		else
		{	//第一块基准钢板在前
			face_norm1=ucs.axis_z^axis_x_1;
			normalize(face_norm1);
			face_pick[0]=pDatumPlate1->ucs.origin;
			if(face_norm1*pDatumPlate1->ucs.axis_z>0)
				face_pick[0]+=pDatumPlate1->ucs.axis_z*(pDatumPlate1->GetThick()+pDatumPlate1->m_fNormOffset);
			else
				face_pick[0]+=pDatumPlate1->ucs.axis_z*pDatumPlate1->m_fNormOffset;
			face_norm2=axis_x_2^ucs.axis_z;
			normalize(face_norm2);
			face_pick[1]=pDatumPlate2->ucs.origin;
			if(face_norm2*pDatumPlate2->ucs.axis_z>0)
				face_pick[1]+=pDatumPlate2->ucs.axis_z*(pDatumPlate2->GetThick()+pDatumPlate2->m_fNormOffset);
			else
				face_pick[1]+=pDatumPlate2->ucs.axis_z*pDatumPlate2->m_fNormOffset;
			//前边计算得到的face_norm1 face_norm2 仅仅用来确定方向	wht 09-06-15
			if(face_norm1*pDatumPlate1->ucs.axis_z<0)
				face_norm1=-pDatumPlate1->ucs.axis_z;
			else
				face_norm1=pDatumPlate1->ucs.axis_z;
			if(face_norm2*pDatumPlate2->ucs.axis_z<0)
				face_norm2=-pDatumPlate2->ucs.axis_z;
			else
				face_norm2=pDatumPlate2->ucs.axis_z;
		}
		ucs.axis_x = axis_x_1;
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		f3dPoint inner_arc_start,inner_arc_end;
		double radius=pDatumTube->GetDiameter()*0.5;
		f3dPoint line_pick=ucs.origin+axis_x_1*100+axis_x_2*100;	//肯定在当前板平面内
		f3dPoint line_vec=axis_x_1;
		RotateVectorAroundVector(line_vec,Pi*0.5,ucs.axis_z);	//垂直于相交边的方向
		Int3dlf(line_pick,line_pick,line_vec,face_pick[0],face_norm1);
		Int3dlc(&inner_arc_end,&vertice,line_pick,axis_x_1,pDatumTube->Start(),pDatumTube->End(),radius,radius);
		vertice_vec=inner_arc_end-ucs.origin;
		if(vertice_vec*axis_x_1<0)
			inner_arc_end=vertice;
		if(pDatumPlate2)
		{
			line_pick=ucs.origin+axis_x_1*100+axis_x_2*100;	//肯定在当前板平面内
			line_vec=axis_x_2;
			RotateVectorAroundVector(line_vec,Pi*0.5,ucs.axis_z);	//垂直于相交边的方向
			Int3dlf(line_pick,line_pick,line_vec,face_pick[1],face_norm2);
			Int3dlc(&inner_arc_start,&vertice,line_pick,axis_x_2,pDatumTube->Start(),pDatumTube->End(),radius,radius);
			vertice_vec=inner_arc_start-ucs.origin;
			if(vertice_vec*axis_x_2<0)
				inner_arc_start=vertice;
		}
		//第一个顶点
		vertice=inner_arc_end;
		coord_trans(vertice,ucs,FALSE);
		vertice.z=0;
		vertice.feature=1;
		PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
		pVertex->m_bWeldEdge=TRUE;
		pVertex->weldline.hWeldMotherPart=bSwapDatumPlate?pDatumPlate2->handle:pDatumPlate1->handle;
		//第二个顶点
		vertice.x+=W;
		pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
		f3dPoint vertex2=vertice;
		if(bCir)
		{	//顶部为圆弧连接
			pVertex->type=2;
			pVertex->radius=R+W;
			double cosa=fabs(ucs.axis_z*pDatumTube->ucs.axis_z);
			if(cosa<EPS_COS&&cosa>EPS)	//内圆弧为椭圆弧
				pVertex->radius=(R+W)/cosa;
		}
		else
		{	//顶部为直线连接
			if(H<0)	//启用参数D
			{	
				//第三个顶点
				f3dPoint inters,axis_x_mid=axis_x_1+axis_x_2;
				normalize(axis_x_mid);
				Int3dlc(&inters,&vertice,ucs.origin,axis_x_mid,pDatumTube->Start(),pDatumTube->End(),radius,radius);
				vertice_vec=inters-ucs.origin;
				if(vertice_vec*axis_x_mid<0)
					inters=vertice;
				vertice=inters+axis_x_mid*D;	//第三点和第四点间边线的中点
				coord_trans(vertice,ucs,FALSE);
				f3dPoint mid_vertex(vertice.x,vertice.y);
				f3dPoint edge_vec(-vertice.y,vertice.x);
				normalize(edge_vec);
				vertice.Set(vertex2.x,vertice.y+(vertex2.x-vertice.x)*edge_vec.y/edge_vec.x,0);
				vertice.feature=1;
				vertex_list.append(PROFILE_VERTEX(vertice,0));
				//第四个顶点
				vertice+=(mid_vertex-vertice)*2;
				vertice.feature=1;
				vertex_list.append(PROFILE_VERTEX(vertice,0));
			}
			else if(H>0)	//启用参数H
			{	
				//第三个顶点	
				vertice.Set(vertex2.x,vertex2.y+H,0);
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				//第四个顶点
				vertice=inner_arc_start;
				vertice+=axis_x_2*N+face_norm2*H;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
			}
		}
		//第五个顶点
		vertice=inner_arc_start;
		vertice+=axis_x_2*N;
		coord_trans(vertice,ucs,FALSE);
		vertice.z=0;
		vertice.feature=1;
		pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
		pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
		pVertex->weldline.hWeldMotherPart=bSwapDatumPlate?pDatumPlate1->handle:pDatumPlate2->handle;
		f3dPoint vertex5=vertice;
		//第六个顶点，为内圆弧起始点
		vertice=inner_arc_start;
		coord_trans(vertice,ucs,FALSE);
		vertice.z=0;
		vertice.feature=1;
		pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
		if(fabs(ucs.axis_z*pDatumTube->ucs.axis_z)>EPS_COS)	//圆弧
		{	//圆弧
			pVertex->type=2;
			pVertex->radius=-R;
		}
		else
		{	//椭圆弧
			pVertex->type=3;
			pVertex->center=ucs.origin;
			if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
				pVertex->column_norm=pDatumTube->ucs.axis_z;
			else
				pVertex->column_norm=-pDatumTube->ucs.axis_z;
			pVertex->radius=R;
		}
		pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
	}
	else if(pDatumPlate2==NULL)//仅有一块基准钢板的环向肋板
	{	//if(pDatumTube)基准构件为钢管; if(pDatumSphere)基准构件球体;	
		double Y=0,radius=0;
		GetDesignItemValue('Y',&Y);
		ucs.axis_x = axis_x_1;
		if(Y>=0)
			ucs.axis_y=pDatumPlate1->ucs.axis_z;
		else
			ucs.axis_y=-pDatumPlate1->ucs.axis_z;
		ucs.axis_x = ucs.axis_y^ucs.axis_z; 
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		normalize(ucs.axis_z);
		f3dPoint line_pick=pDatumPlate1->ucs.origin;
		if(Y>=0)
			line_pick+=pDatumPlate1->ucs.axis_z*(pDatumPlate1->GetThick()+pDatumPlate1->m_fNormOffset);
		else
			line_pick+=pDatumPlate1->ucs.axis_z*pDatumPlate1->m_fNormOffset;
		f3dPoint inner_arc_end,arc_vertex;
		project_point(line_pick,ucs.origin,ucs.axis_z);
		if(pDatumTube)	//基准构件为钢管
		{
			radius=pDatumTube->GetDiameter()*0.5;
			Int3dlc(&inner_arc_end,&vertice,line_pick,axis_x_1,pDatumTube->Start(),pDatumTube->End(),radius,radius);
			vertice_vec=inner_arc_end-ucs.origin;
			if(vertice_vec*axis_x_1<0)
				inner_arc_end=vertice;
			arc_vertex=inner_arc_end;
			coord_trans(arc_vertex,ucs,FALSE);
			arc_vertex.z=0;
		}
		else if(pDatumSphere)	//基准构件为球体
		{	//延伸面经过球原点的肋板
			radius=pDatumSphere->D*0.5;
			double y_offset_dist=pDatumPlate1->m_fNormOffset;
			if(Y>=0)
				y_offset_dist+=pDatumPlate1->GetThick();
			double x_offset_dist=sqrt(radius*radius-y_offset_dist*y_offset_dist);
			if(axis_x_1*ucs.axis_x>0)
				arc_vertex.Set(x_offset_dist,fabs(y_offset_dist));
			else
				arc_vertex.Set(-x_offset_dist,fabs(y_offset_dist));
			
			if(S!=0)
			{
				double r=sqrt(x_offset_dist*x_offset_dist-S*S);
				x_offset_dist=sqrt(r*r-y_offset_dist*y_offset_dist);
				if(axis_x_1*ucs.axis_x>0)
					arc_vertex.Set(x_offset_dist,fabs(y_offset_dist));
				else
					arc_vertex.Set(-x_offset_dist,fabs(y_offset_dist));
				radius=r;
			}
		}
		if(bCir)
		{	//顶部圆弧连接
			f3dPoint inters2;
			double fArcAngle=D*RADTODEG_COEF;	//扇形角度
			if(axis_x_1*ucs.axis_x>0)	//板在Х轴正向
			{
				double cosa=cos(fArcAngle);
				double sina=sin(fArcAngle);
				//第一个顶点
				vertice=arc_vertex;
				vertice.feature=1;
				PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->m_bWeldEdge=TRUE;
				//第二个顶点 为外圆弧起点
				vertice.x+=W;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->type=2;
				pVertex->radius=R+W;
				//
				if(fabs(W-N)<EPS)	//无切角
				{	
					//第三个顶点
					vertice.Set(cosa*(R+W),sina*(R+W));
					vertex_list.append(PROFILE_VERTEX(vertice,0));
					//第四个顶点，为内圆弧起始点
					vertice.Set(cosa*R,sina*R);
					vertice.feature=1;
					pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
					pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
					if(pDatumTube)
					{	
						if(fabs(ucs.axis_z*pDatumTube->ucs.axis_z)>EPS_COS)	//圆弧
						{	//圆弧
							pVertex->type=2;
							pVertex->radius=-R;
						}
						else
						{	//椭圆弧
							pVertex->type=3;
							pVertex->center=ucs.origin;
							if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
								pVertex->column_norm=pDatumTube->ucs.axis_z;
							else
								pVertex->column_norm=-pDatumTube->ucs.axis_z;
							pVertex->radius=R;
						}
					}
				}
				else//存在切角
				{	//第三个顶点
					f2dLine line2d;
					line2d.startPt.Set(cosa*(R+N),sina*(R+N));
					line2d.endPt.Set(line2d.startPt.x+(R+N)*cos(fArcAngle-0.25*Pi),line2d.startPt.y+(R+N)*sin(fArcAngle-0.25*Pi));
					f2dArc arc;
					arc.startPt.Set(R+W,0);
					arc.angle=fArcAngle;
					Int2dla(line2d,arc,vertice.x,vertice.y,inters2.x,inters2.y);
					vertice.feature=1;
					vertice.ID=3;
					vertex_list.append(PROFILE_VERTEX(vertice,0));
					//第四个顶点
					vertice.Set(cosa*(R+N),sina*(R+N));
					vertex_list.append(PROFILE_VERTEX(vertice,0));
					//第五个顶点，为内圆弧起始点
					vertice.Set(cosa*R,sina*R);
					vertice.feature=1;
					pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
					pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
					if(pDatumTube)
					{	
						if(fabs(ucs.axis_z*pDatumTube->ucs.axis_z)>EPS_COS)	//圆弧
						{	//圆弧
							pVertex->type=2;
							pVertex->radius=-R;
						}
						else
						{	//椭圆弧
							pVertex->type=3;
							pVertex->center=ucs.origin;
							if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
								pVertex->column_norm=pDatumTube->ucs.axis_z;
							else
								pVertex->column_norm=-pDatumTube->ucs.axis_z;
							pVertex->radius=R;
						}
					}
				}
			}
			else	//板在Х轴反向	
			{
				double cosa=cos(Pi-fArcAngle);
				double sina=sin(Pi-fArcAngle);
				//第一个顶点
				vertice.Set(cosa*R,sina*R);
				vertice.feature=1;
				vertex_list.append(PROFILE_VERTEX(vertice,0));
				if(fabs(W-N)<EPS)	//无切角
				{	//第二个顶点 为外圆弧起点
					vertice.Set(cosa*(R+W),sina*(R+W));
					vertice.feature=1;
					PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
					pVertex->type=2;
					pVertex->radius=R+W;
					//第三个顶点
					vertice.Set(arc_vertex.x-W,arc_vertex.y);
					pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
					pVertex->m_bWeldEdge=TRUE;
					//第四个顶点，为内圆弧起始点
					vertice=arc_vertex;
					vertice.feature=1;
					pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
					pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
					if(pDatumTube)
					{	
						if(fabs(ucs.axis_z*pDatumTube->ucs.axis_z)>EPS_COS)	//圆弧
						{	//圆弧
							pVertex->type=2;
							pVertex->radius=-R;
						}
						else
						{	//椭圆弧
							pVertex->type=3;
							pVertex->center=ucs.origin;
							if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
								pVertex->column_norm=pDatumTube->ucs.axis_z;
							else
								pVertex->column_norm=-pDatumTube->ucs.axis_z;
							pVertex->radius=R;
						}
					}
				}
				else
				{
					//第二个顶点
					vertice.Set(cosa*(R+N),sina*(R+N));
					vertex_list.append(PROFILE_VERTEX(vertice,0));
					//第三个顶点
					f2dLine line2d;
					line2d.startPt.Set(cosa*(R+N),sina*(R+N));
					line2d.endPt.Set(vertice.x+(R+N)*cos(Pi-fArcAngle+0.25*Pi),vertice.y+(R+N)*sin(Pi-fArcAngle+0.25*Pi));
					f2dArc arc;
					arc.startPt.Set(cosa*(R+W),sina*(R+W));
					arc.angle=fArcAngle;
					Int2dla(line2d,arc,vertice.x,vertice.y,inters2.x,inters2.y);
					vertice.feature=1;
					vertice.ID=3;
					PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,2));
					//指定半径圆弧,center为相对坐标 wht 12-06-15
					pVertex->center.Set();
					//pVertex->center=ucs.origin;
					pVertex->radius=R+W;
					//第四个顶点
					vertice.Set(arc_vertex.x-W,arc_vertex.y);
					pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
					pVertex->m_bWeldEdge=TRUE;
					//第五个顶点，为内圆弧起始点
					vertice=arc_vertex;
					vertice.feature=1;
					pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
					pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
					if(pDatumTube)
					{	
						if(fabs(ucs.axis_z*pDatumTube->ucs.axis_z)>EPS_COS)	//圆弧
						{	//圆弧
							pVertex->type=2;
							pVertex->radius=-R;
						}
						else
						{	//椭圆弧
							pVertex->type=3;
							pVertex->center=ucs.origin;
							if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
								pVertex->column_norm=pDatumTube->ucs.axis_z;
							else
								pVertex->column_norm=-pDatumTube->ucs.axis_z;
							pVertex->radius=R;
						}
					}
				}
			}
		}
		else
		{	//顶部非圆弧连接
			if(axis_x_1*ucs.axis_x>0)	//板在Х轴正向
			{
				//第一个顶点
				vertice=arc_vertex;
				vertice.feature=1;
				PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->m_bWeldEdge=TRUE;
				//第二个顶点
				vertice.x+=W;
				vertex_list.append(PROFILE_VERTEX(vertice,0));
				//第三个顶点
				vertice.y+=H;
				vertex_list.append(PROFILE_VERTEX(vertice,0));
				//第四个顶点，为内圆弧起始点
				vertice.Set(sqrt(radius*radius-D*D),D);
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
				pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
				if(pDatumTube)
				{	
					if(fabs(ucs.axis_z*pDatumTube->ucs.axis_z)>EPS_COS)	//圆弧
					{	//圆弧
						pVertex->type=2;
						pVertex->radius=-R;
					}
					else
					{	//椭圆弧
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=R;
					}
				}
				else if(pDatumSphere)
				{
					pVertex->type=1;
					pVertex->center=ucs.origin;
					pVertex->radius=-R;
					PROFILE_VERTEX *pPreVertex=vertex_list.GetFirst();
					double half_len = 0.5*DISTANCE(pVertex->vertex,pPreVertex->vertex);
					if(fabs(half_len/pVertex->radius)<=1)
						pVertex->sector_angle = 2*asin(half_len/pVertex->radius);
				}
			}
			else	//板在Х轴负向	
			{
				//第一个顶点
				vertice.Set(-sqrt(radius*radius-D*D),D);
				vertice.feature=1;
				vertex_list.append(PROFILE_VERTEX(vertice,0));
				//第二个顶点
				vertice.Set(arc_vertex.x-W,arc_vertex.y+H);
				vertex_list.append(PROFILE_VERTEX(vertice,0));
				//第三个顶点
				vertice.Set(arc_vertex.x-W,arc_vertex.y);
				PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->m_bWeldEdge=TRUE;
				//第四个顶点，为内圆弧起始点
				vertice=arc_vertex;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
				pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
				if(pDatumTube)
				{	
					if(fabs(ucs.axis_z*pDatumTube->ucs.axis_z)>EPS_COS)	//圆弧
					{	//圆弧
						pVertex->type=2;
						pVertex->radius=-R;
					}
					else
					{	//椭圆弧
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=R;
					}
				}
				else if(pDatumSphere)
				{
					pVertex->type=1;
					pVertex->center=ucs.origin;
					pVertex->radius=-R;
					PROFILE_VERTEX *pPreVertex=vertex_list.GetFirst();
					double half_len = 0.5*DISTANCE(pVertex->vertex,pPreVertex->vertex);
					if(fabs(half_len/pVertex->radius)<=1)
						pVertex->sector_angle = 2*asin(half_len/pVertex->radius);
				}
			}
		}
	}
	return TRUE;
}
BOOL CLDSParamPlate::DesignBasePlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)//设计底板
{
	long h,baseType=0,shapeType=0;
	GetDesignItemValue('A',&h,pItemHashTbl);	//定位基准节点
	CLDSNode *pBaseNode=BelongModel()->FromNodeHandle(h);
	//1.计算底板的装配坐标系
	BOOL bJgFootBasePlate=FALSE;
	long hBaseRod=0,nNormOffset=0,cNormOffsetType=0;
	if(GetDesignItemValue('C',&cNormOffsetType,pItemHashTbl))	//法向偏移类型
	{
		if(cNormOffsetType==0)		//上平面
			nNormOffset=-thick;
		else if(cNormOffsetType==1)	//下平面
			nNormOffset=0;
		else if(cNormOffsetType==2)	//中间平面
			nNormOffset=-(short)thick/2;
		else 
			GetDesignItemValue('N',&nNormOffset,pItemHashTbl);
	}
	else
		GetDesignItemValue('N',&nNormOffset,pItemHashTbl);		//指定法向偏移
	GetDesignItemValue(KEY2C('L'),&hBaseRod,pItemHashTbl);
	CLDSLinePart* pBaseRod=(CLDSLinePart*)BelongModel()->FromPartHandle(hBaseRod,CLS_LINEPART);
	if(pBaseRod==NULL)
	{	//初始化普通基板坐标系
		long iAxisXType=0,iAxisZType=0;
		GetDesignItemValue('Z',&iAxisZType,pItemHashTbl);
		//GetDesignItemValue('X',&iAxisXType,pItemHashTbl);
		if(iAxisZType==0)
		{
			workNorm.UpdateVector(BelongModel());
			ucs.axis_z=workNorm.vector;
		}
		ucs.axis_z=CStdVector::GetVector(iAxisZType);
		if(ucs.axis_z.IsZero())
			ucs.axis_z.Set(0,0,-1);
		ucs.axis_x=btc::CalLocalCS_X_AXIS(ucs.axis_z);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		if(!is_ucs_valid(ucs))
			return FALSE;
		if(design.inited)//ucs.origin.IsZero())
		{	//计算底板坐标原点时考虑指定法线偏移距离
			design.origin.UpdatePos(BelongModel());
			ucs.origin=design.origin.Position();
			ucs.origin+=nNormOffset*ucs.axis_z;
		}
	}
	else if(pBaseRod->GetClassTypeId()==CLS_LINETUBE)
	{	//初始化钢管端头基板坐标系
		CLDSLineTube* pBaseTube=(CLDSLineTube*)pBaseRod;
		pBaseTube->BuildUCS();
		f3dPoint lenVec,axisVecX;
		if(pBaseTube->pStart==pBaseNode)
			lenVec=pBaseTube->Start()-pBaseTube->End();
		else //if(pBaseTube->pEnd==pBaseNode)
			lenVec=pBaseTube->End()-pBaseTube->Start();
		normalize(lenVec);
		long iAxisXType=0,iAxisZType=0;
		GetDesignItemValue('Z',&iAxisZType,pItemHashTbl);
		if(iAxisZType==0)
		{
			workNorm.UpdateVector(BelongModel());
			ucs.axis_z=workNorm.vector;
		}
		else
			ucs.axis_z=CStdVector::GetVector(iAxisZType);
		if(ucs.axis_z.IsZero())
			ucs.axis_z=lenVec;//.Set(0,0,-1);
		ucs.axis_x=btc::CalLocalCS_X_AXIS(ucs.axis_z);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		if(!is_ucs_valid(ucs))
			return FALSE;
		long iOrigType=0;
		GetDesignItemValue('O',&iOrigType,pItemHashTbl);
		if(iOrigType==1)
		{	//基板在钢管端头
			f3dPoint startPt,endPt,line_vec;
			startPt=pBaseTube->Start();
			endPt=pBaseTube->End();
			line_vec=endPt-startPt;
			normalize(line_vec);
			if(pBaseTube->pStart->handle=pBaseNode->handle)
				ucs.origin=startPt-line_vec*pBaseTube->startOdd();
			else
				ucs.origin=endPt+line_vec*pBaseTube->endOdd();	
		}
		else
		{	//计算底板坐标原点时考虑指定法线偏移距离
			if(design.inited)//ucs.origin.IsZero())
			{
				design.origin.UpdatePos(BelongModel());
				ucs.origin=design.origin.Position();
				ucs.origin+=nNormOffset*ucs.axis_z;
			}
		}
	}
	else if(pBaseRod->IsAngle())
	{	//初始化角钢塔脚底板坐标系
		bJgFootBasePlate=TRUE;
		CLDSLineAngle* pBaseAngle=(CLDSLineAngle*)pBaseRod;
		f3dPoint lenVec,axisVecX;
		if(pBaseAngle->pStart==pBaseNode)
		{
			lenVec=pBaseAngle->End()-pBaseAngle->Start();
			if(lenVec.z<0)	//基准角钢在底板上侧
				axisVecX=-pBaseAngle->GetWingVecY();
			else
				axisVecX=-pBaseAngle->GetWingVecX();
		}
		else if(pBaseAngle->pEnd==pBaseNode)
		{
			lenVec=pBaseAngle->Start()-pBaseAngle->End();
			if(lenVec.z<0)	//基准角钢在底板上侧
				axisVecX=-pBaseAngle->GetWingVecX();
			else
				axisVecX=-pBaseAngle->GetWingVecY();
		}
		else
			return FALSE;
		if(ucs.axis_z.IsZero())
			ucs.axis_z.Set(0,0,-1);
		else if(ucs.axis_z.z>0)
			ucs.axis_z*=-1.0;
		if(design.inited)//ucs.origin.IsZero())
		{	//计算底板坐标原点时考虑指定法线偏移距离
			design.origin.UpdatePos(BelongModel());
			ucs.origin=design.origin.Position();
			ucs.origin+=nNormOffset*ucs.axis_z;
			if(nNormOffset!=0&&!ucs.axis_z.IsEqual(f3dPoint(0,0,-1))&&!ucs.axis_z.IsEqual(f3dPoint(0,0,1)))
				Int3dlf(ucs.origin,design.origin.Position(),GEPOINT(0,0,1),ucs.origin,ucs.axis_z);	//斜塔脚
		}
		//由于存在异型布置的组合角钢,不宜根据角钢肢向确定底板坐标系,故改为按象限确定 wjh-2015.4.6
		if(ucs.origin.x>0&&ucs.origin.y>0)
			ucs.axis_x.Set(1,0,0);	//1号位
		else if(ucs.origin.x<0&&ucs.origin.y>0)
			ucs.axis_x.Set(0,1,0);	//2号位
		else if(ucs.origin.x>0&&ucs.origin.y<0)
			ucs.axis_x.Set(0,-1,0);	//3号位
		else if(ucs.origin.x<0&&ucs.origin.y<0)
			ucs.axis_x.Set(-1,0,0);	//4号位
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		if(!ucs.axis_z.IsEqual(f3dPoint(0,0,-1)))
		{	//非典型塔脚板，需要以主材角钢的角分线为基准将误差均分，不可以其中一肢法线为基准　wjh-2016.4.8
			GEPOINT inters,vMidVec=pBaseAngle->vxWingNorm+pBaseAngle->vyWingNorm;
			Int3dlf(inters,vMidVec,ucs.origin+lenVec,ucs.origin,ucs.axis_z);
			GEPOINT bivec=inters-GEPOINT(ucs.origin);
			normalize(bivec);
			double cosa45=0.70710678118654752440084436210485;
			double sina45=cosa45;
			GEPOINT axisx=bivec,axisy=bivec;
			RotateVectorAroundVector(axisx,sina45,cosa45,ucs.axis_z);
			RotateVectorAroundVector(axisy,-sina45,cosa45,ucs.axis_z);
			double ddyx=ucs.axis_y*axisx;
			double ddyy=ucs.axis_y*axisy;
			if(fabs(ddyx)>fabs(ddyy))
				ucs.axis_y=ddyx>0?axisx:-axisx;
			else if(fabs(ddyx)<fabs(ddyy))
				ucs.axis_y=ddyy>0?axisy:-axisy;
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
			normalize(ucs.axis_x);
			normalize(ucs.axis_y);
		}
	}
	//2.定义底板外形
	//GetDesignItemValue(KEY2C("BT"),&baseType,pItemHashTbl);	//底板类型:塔脚板或牛蹄板
	GetDesignItemValue('S',&shapeType,pItemHashTbl);			//底板外形：0.方形 1.圆形
	PROFILE_VERTEX *pVertex=NULL;
	if(shapeType==0)
	{
		double XA=0,XB=0,YA=0,YB=0,X[4]={0},Y[4]={0};
		GetDesignItemValue(KEY2C("XA"),&XA,pItemHashTbl);
		GetDesignItemValue(KEY2C("XB"),&XB,pItemHashTbl);
		GetDesignItemValue(KEY2C("YA"),&YA,pItemHashTbl);
		GetDesignItemValue(KEY2C("YB"),&YB,pItemHashTbl);
		int i;
		for(i=0;i<4;i++)
		{
			GetDesignItemValue(KEY2C(CXhChar16("X%d",i)),&X[i],pItemHashTbl);
			GetDesignItemValue(KEY2C(CXhChar16("Y%d",i)),&Y[i],pItemHashTbl);
		}
		/*if(bJgFootBasePlate)
		{	//角钢塔脚板的倒角数据不应该在无数据时都统一初始化 wjh-2016.9.18
			if(XB<=0)
				XB=XA;
			if(YB<=0)
				YB=YA;
			if(X[1]<=0)	//(X+Y+)侧宽默认与(X+Y-)侧相同
				X[1]=X[0];
			if(X[2]<=0)	//(X+Y+)侧宽默认与(X+Y-)侧相同
				X[2]=X[1];
			if(X[3]<=0)	//(X-Y-)侧宽默认与(X+Y+)对角侧相同
				X[3]=X[0];
			if(Y[1]<=0)	//(X+Y+)侧宽默认与(X+Y-)侧相同
				Y[1]=Y[0];
			if(Y[2]<=0)	//(X+Y+)侧宽默认与(X+Y-)侧相同
				Y[2]=Y[1];
			if(Y[3]<=0)	//(X-Y-)侧宽默认与(X+Y+)对角侧相同
				Y[3]=Y[0];
		}*/
		pVertex=vertex_list.append(PROFILE_VERTEX(XA-X[0],-YB,0,0));
		pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("YB"));
		if(Y[0]>0)
		{
			pVertex=vertex_list.append(PROFILE_VERTEX(XA,-YB+Y[0],0,0));
			pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("Y0"));
		}
		pVertex=vertex_list.append(PROFILE_VERTEX(XA,YA-Y[2],0,0));
		pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("XA"));
		if(X[2]>0)
		{
			pVertex=vertex_list.append(PROFILE_VERTEX(XA-X[2],YA,0,0));
			pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("X2"));
		}
		pVertex=vertex_list.append(PROFILE_VERTEX(-XB+X[3],YA,0,0));
		pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("YA"));
		if(Y[3]>0)
		{
			pVertex=vertex_list.append(PROFILE_VERTEX(-XB,YA-Y[3],0,0));
			pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("Y3"));
		}
		pVertex=vertex_list.append(PROFILE_VERTEX(-XB,-YB+Y[1],0,0));
		pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("XB"));
		if(X[1]>0)
		{
			pVertex=vertex_list.append(PROFILE_VERTEX(-XB+X[1],-YB,0,0));
			pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("X1"));
		}
	}
	else
	{
		double R=0;
		GetDesignItemValue(KEY2C("R"),&R,pItemHashTbl);
		vertex_list.append(PROFILE_VERTEX(0,-R,0));
		vertex_list.append(PROFILE_VERTEX(R,0,0));
		vertex_list.append(PROFILE_VERTEX(0,R,0));
		vertex_list.append(PROFILE_VERTEX(-R,0,0));
		long hDesPara=1;
		for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			pVertex->type=1;
			pVertex->sector_angle=Pi/2;
			pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,hDesPara++);
		}
	}
	for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		pLsRef->GetLsPtr()->correct_worknorm();
		pLsRef->GetLsPtr()->correct_pos();
	}
	//同时重新设计基板上的附加板
	for(ATTACHPART_ITEM* pItem=AttachPart.GetFirst();pItem;pItem=AttachPart.GetNext())
	{
		CLDSParamPlate* pAttachPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(pItem->h,CLS_PARAMPLATE);//pItem->idClsType);
		if(pAttachPlate)
			pAttachPlate->DesignPlate();
	}
	return TRUE;
}
GEPOINT MoveVertexByMidPlane(double halfThick,const double* bottom_vertex,const double* axis_x,const double* axis_z,const double* bottom_normal)
{
	GEPOINT xBtmVertex(bottom_vertex),vPlateAxisX(axis_x),vPlateAxisZ(axis_z),vBtmPlaneNormal(bottom_normal);
	GEPOINT vPlateAxisY=vPlateAxisZ^vPlateAxisX;
	double cosa=vPlateAxisZ*vBtmPlaneNormal;
	double sina=SQRT(1-cosa*cosa);
	if(fabs(cosa)<EPS)	//靴板与底板垂直，不需要重新计算
		return xBtmVertex;
	else if(fabs(sina)<EPS)
		return xBtmVertex;	//靴板与底板平行,这种情况属于异常,一般不会出现
	int sign=1; 
	if(vPlateAxisY*vBtmPlaneNormal<0)
		sign=-1;
	//if(cosa>0)	//靴板法向与底板朝靴板一侧法向一致,底边应延长至中性层与底板平面相交
	double extendY=halfThick*cosa/sina;
	xBtmVertex.y-=sign*extendY;
	return xBtmVertex;
}
//DesignShoePlate2未来会取代DesignShoePlate()函数,该函数是在自由定制各类型靴板过程中添加的 wjh-2017.1.17
BOOL CLDSParamPlate::DesignShoePlate2(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)//设计靴板
{
	long iConnectStyle=0,iCutMode=0;
	long hConnectBasePlate=0,hConnectShoePlate=0,hDependentAngle=0,hLinkHoriAngle=0,hLinkXieAngle=0,work_direct=0,style_bolt0_weld1=0;
	long profile_side=0,A=0,B=0,bolt_layoutstyle=0,normOffDist=0,IA=0,IB=0,IC=0,ID=0;
	double fCutAngleW=0,fMargin=0,fAlongVecLen=0;
	//work_direct=='X'表示靴板在连接基准角钢的X肢，'Y'表示在Y肢
	if(!GetDesignItemValue('W',&work_direct,pItemHashTbl))	//靴板相对底板的坐标轴向0.X轴;1.Y轴
#ifdef AFX_TARG_ENU_ENGLISH
		Log2File()->Log("0X%X shoe plate lacks parameter of extending axis direction,defaults along X axis of base plate!",handle);
#else
		Log2File()->Log("0X%X靴板缺少延展轴向参数,默认沿底板X轴方向!",handle);
#endif
	if(work_direct!='X'&&work_direct!='Y')
		return FALSE;
	GetDesignItemValue('A',&A,pItemHashTbl);//台口高
	GetDesignItemValue('B',&B,pItemHashTbl);//顶部余量
	GetDesignItemValue(KEY2C("IA"),&IA,pItemHashTbl);//内侧台口高
	GetDesignItemValue(KEY2C("IB"),&IB,pItemHashTbl);//顶部内侧余量
	GetDesignItemValue('M',&fMargin,pItemHashTbl);//靴板延伸至底板边缘时的间隙值
	GetDesignItemValue(KEY2C("CS"),&iConnectStyle,pItemHashTbl);	//与依赖角钢间的连接方式, 0.螺栓连接;1.对焊连接
	if(iConnectStyle==1)	//焊接靴板默认覆盖长度为300,　以免默认板过小　wjh-2018.1.25
		fAlongVecLen=300;
	GetDesignItemValue(KEY2C("AL"),&fAlongVecLen,pItemHashTbl);		//靴板延杆件长度方向延伸量(默认该延伸量根据螺栓计算)
	GetDesignItemValue(KEY2C("CM"),&iCutMode,pItemHashTbl);		//倒角模式 0.切角 1.切弧
	GetDesignItemValue('L',&fCutAngleW,pItemHashTbl);	//切角宽度
	GetDesignItemValue(KEY2C("IC"),&IC,pItemHashTbl);//斜材hLinkXieAngle楞线侧延伸量
	GetDesignItemValue(KEY2C("ID"),&ID,pItemHashTbl);//斜材hLinkXieAngle肢翼侧延伸量
	GetDesignItemValue(KEY2C("LX"),&hLinkXieAngle,pItemHashTbl);	//连接角钢
	GetDesignItemValue(KEY2C("LH"),&hLinkHoriAngle,pItemHashTbl);	//连接角钢
	GetDesignItemValue(KEY2C("NL"),&normOffDist,pItemHashTbl);		//靴板沿法线方向偏移距离
	if(!GetDesignItemValue('C',&hConnectBasePlate,pItemHashTbl))//连接底板
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X shoe plate lacks parameter of connecting base plate,failed design!",handle);
#else
			log2file->Log("0X%X靴板缺少连接底板参数,设计失败!",handle);
#endif
		return FALSE;
	}
	if(!GetDesignItemValue('D',&hDependentAngle,pItemHashTbl))	//依赖角钢
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X parametric plate lacks parameter of reference angle,failed design!",handle);
#else
			log2file->Log("0X%X参数化钢板缺少依赖角钢参数,设计失败!",handle);
#endif
		return FALSE;
	}
	if(!GetDesignItemValue('F',&profile_side,pItemHashTbl)&&log2file)		//外形类别:0.贯穿底板;1.肢翼单侧;2.楞线单侧
#ifdef AFX_TARG_ENU_ENGLISH
		log2file->Log("0X%X shoe plate lacks parameter of shape type,defaults through base plate!",handle);
#else
		log2file->Log("0X%X靴板缺少外形类别参数,默认为贯穿底板的靴板!",handle);
#endif
	if(!GetDesignItemValue('G',&hConnectShoePlate,pItemHashTbl)&&profile_side==2&&iConnectStyle==0)		//连接靴板
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X shoe plate lacks parameter of connecting shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X靴板缺少靠接靴板参数,设计失败!",handle);
#endif
		return FALSE;
	}
	GetDesignItemValue('H',&bolt_layoutstyle,pItemHashTbl);	//螺栓布置型式:0.左高右低;1.左低右高
	CLDSGeneralPlate* pBasePlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(hConnectBasePlate,CLS_PLATE,CLS_PARAMPLATE);
	if(pBasePlate==NULL)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X missing the connecting base plate specified for shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X靴板的指定连接底板丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	CLDSGroupLineAngle* pBaseGroupAngle=NULL;
	CLDSLineAngle* pBaseAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(hDependentAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	DYN_ARRAY<CLDSLineAngle*> arrSonBaseAnglePtrs(4);
	arrSonBaseAnglePtrs.Clear();
	if(pBaseAngle&&pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		pBaseGroupAngle=(CLDSGroupLineAngle*)pBaseAngle;
		//TODO:Task 1 wjh-2018.1.25
		arrSonBaseAnglePtrs[0]=pBaseAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pBaseGroupAngle->son_jg_h[0]);
		for(int k=1;k<4;k++)
			arrSonBaseAnglePtrs[k]=(CLDSLineAngle*)BelongModel()->FromPartHandle(pBaseGroupAngle->son_jg_h[k]);
	}
	if(pBaseAngle==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Log2File()->Log("0X%X missing the reference angle specified for shoe plate,failed design!",handle);
#else
		Log2File()->Log("0X%X靴板指定依赖角钢丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	//1.计算靴板装配坐标系
	long hBaseNode=pBasePlate->m_hBaseNode;
	f3dPoint lenVec;
	if(pBaseAngle->pStart&&pBaseAngle->pStart->handle==hBaseNode)
		lenVec=pBaseAngle->End()-pBaseAngle->Start();
	else if(pBaseAngle->pEnd&&pBaseAngle->pEnd->handle==hBaseNode)
		lenVec=pBaseAngle->Start()-pBaseAngle->End();
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Log2File()->Log("0X%X without connection between reference angle specified for shoe and node that base plate depends,failed design!",handle);
#else
		Log2File()->Log("0X%X靴板指定依赖角钢与底板依赖节点无连接,设计失败!",handle);
#endif
		return FALSE;
	}
	normalize(lenVec);
	//TODO:未考虑组合角钢的情况，应与下文中的outwardvec一并考虑 wjh-2016.9.5
	ucs.axis_z=(work_direct=='X') ? pBaseAngle->vxWingNorm : pBaseAngle->vyWingNorm;
	/* 暂对于手工设计的靴板调用的DesignShoePlate2不启用如下代码
	if(pBaseGroupAngle&&pBaseGroupAngle->wSectStyle!=1&&lenVec.z*ucs.axis_z.z<0)
	{	//异型组合角钢塔脚旋转或X/Y对称时很可能由于靴板的底面不同导致外形出现细微差异，实际应取中性层面，现暂时临时取法向与主材射向一致来避免 wjh-2017.6.6
		if(pBaseGroupAngle->wSectStyle==0)
			pBaseAngle=arrSonBaseAnglePtrs[1];
		else if(pBaseGroupAngle->wSectStyle==2)
			pBaseAngle=arrSonBaseAnglePtrs[2];
		if(pBaseAngle!=arrSonBaseAnglePtrs[0])
			ucs.axis_z*=-1.0;	//变换基准子角钢成功
	}*/
	ucs.axis_x=pBasePlate->ucs.axis_z^ucs.axis_z;
	normalize(ucs.axis_x);
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	if(ucs.axis_y*lenVec<0)
	{	//确保Y轴与依赖主材角钢射线方向一致
		ucs.axis_x*=-1.0;
		ucs.axis_y*=-1.0;
	}
	BYTE ciOldShemaWorkDirect=0;
	if( (work_direct=='X'&&(ucs.axis_x*pBaseAngle->vyWingNorm<0))||
		(work_direct=='Y'&&(ucs.axis_x*pBaseAngle->vxWingNorm<0)))
		ciOldShemaWorkDirect=1;
	f3dPoint datumPlaneOrg=pBasePlate->ucs.origin;
	if(lenVec*pBasePlate->ucs.axis_z>0)		//基准角钢在底板上侧
		datumPlaneOrg+=pBasePlate->ucs.axis_z*(pBasePlate->GetNormOffset()+pBasePlate->GetThick());
	else //if(lenVec*pBasePlate->ucs.axis_z<0)	//基准角钢在底板下侧
		datumPlaneOrg+=pBasePlate->ucs.axis_z*pBasePlate->GetNormOffset();
	//计算由组合角钢间隙大于靴板厚带来的靴板法向位移偏差 wjh-2015.9.16
	double norm_offset=0;
	if(iConnectStyle==0&&pBaseAngle->group_father_jg_h>0X20)
		pBaseGroupAngle=(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(pBaseAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
	if(pBaseGroupAngle&&pBaseGroupAngle->jg_space!=GetThick())
		norm_offset=0.5*(pBaseGroupAngle->jg_space-GetThick());
	f3dPoint pick=pBaseAngle->Start();
	if(fabs(norm_offset)>EPS)
		pick+=ucs.axis_z*norm_offset;
	bool bButtWeld=false;	//靴板与依赖角钢肢实际以对接形式焊接
	if(iConnectStyle==1)
	{
		pick-=ucs.axis_z*thick;	//单角钢对焊连接
		bButtWeld=true;
	}
	if(bButtWeld && abs(normOffDist)>0)	//靴板沿法线方向偏移距离
		pick+=ucs.axis_z*normOffDist;
	Int3dlf(ucs.origin,pick,lenVec,datumPlaneOrg,pBasePlate->ucs.axis_z);
	//2.设计靴板外形
	f3dPoint origin=ucs.origin;
	f3dPoint axisx=ucs.axis_x;
	coord_trans(origin,pBasePlate->ucs,FALSE,TRUE);
	vector_trans(axisx,pBasePlate->ucs,FALSE,TRUE);
	origin.z=0;
	f3dArcLine arcline;
	GEPOINT inters,inters2;
	CMaxDouble left,right;
	PROFILE_VERTEX* pVertex=NULL,*pPrevVertex=pBasePlate->vertex_list.GetTail();
	for(pVertex=pBasePlate->vertex_list.GetFirst();pVertex;pVertex=pBasePlate->vertex_list.GetNext())
	{
		pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&pBasePlate->ucs);
		if(arcline.SectorAngle()==0)	//直线
		{
			if(Int2dll(f2dLine(pPrevVertex->vertex,pVertex->vertex),f2dLine(origin-axisx*10000,origin+axisx*10000),inters.x,inters.y)>0)
			{
				GEPOINT distvec(inters.x-origin.x,inters.y-origin.y);
				double dist =distvec*axisx;
				if(dist<0)
					left.Update(-dist);
				if(dist>0)
					right.Update(dist);
			}
		}
		else //if(arcline.ColumnNorm().IsZero()||fabs(arcline.ColumnNorm()*ucs.axis_z)>EPS_COS)
		{	//普通圆弧(外轮廓一般不会是椭圆故暂未特别处理椭圆)
			f2dArc arc;
			arc.centre=arcline.Center();
			arc.startPt=arcline.PositionInAngle(0);
			arc.angle=arcline.SectorAngle();
			int count=Int2dla(f2dLine(origin-axisx*10000,origin+axisx*10000),arc,inters.x,inters.y,inters2.x,inters2.y);
			if(count>0)
			{
				GEPOINT distvec(inters.x-origin.x,inters.y-origin.y),distvec2;
				if(count==2)
					distvec2.Set(inters2.x-origin.x,inters2.y-origin.y);	//第2个交点最近
				double dist =distvec*axisx;
				if(dist<0)
					left.Update(-dist);
				if(dist>0)
					right.Update(dist);
				dist =distvec2*axisx;
				if(dist<0)
					left.Update(-dist);
				if(dist>0)
					right.Update(dist);	
			}
		}
		pPrevVertex=pVertex;
	}
	bool find_vertex[2]={false,false};
	if(!(find_vertex[0]= left.IsInited()))
		left.Update(0);
	if(!(find_vertex[1]=right.IsInited()))
		right.Update(0);
	if(!find_vertex[0]||!find_vertex[1])
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X shoe plate,doesn't find the enough intersection point with base plate,failed design",handle);
#else
			log2file->Log("0X%X靴板,未找到与底板间的足够交点, 设计失败",handle);
#endif
		return false;
	}
	//靴板底板顶点
	f3dPoint xueban_btm_vertex[2];	//-X->+X或-Y->+Y顺序
	xueban_btm_vertex[0].Set(origin.x+axisx.x*-left.number,origin.y+axisx.y*-left.number);
	xueban_btm_vertex[1].Set(origin.x+axisx.x*right.number,origin.y+axisx.y*right.number);
	xueban_btm_vertex[0]+=axisx*fMargin;
	xueban_btm_vertex[1]-=axisx*fMargin;
	coord_trans(xueban_btm_vertex[0],pBasePlate->ucs,TRUE);
	coord_trans(xueban_btm_vertex[1],pBasePlate->ucs,TRUE);
	if(lenVec*pBasePlate->ucs.axis_z>0)		//基准角钢在底板上侧
	{
		xueban_btm_vertex[0]+=pBasePlate->ucs.axis_z*(pBasePlate->GetNormOffset()+pBasePlate->GetThick());
		xueban_btm_vertex[1]+=pBasePlate->ucs.axis_z*(pBasePlate->GetNormOffset()+pBasePlate->GetThick());
	}
	else if(lenVec*pBasePlate->ucs.axis_z<0)	//基准角钢在底板下侧
	{
		xueban_btm_vertex[0]+=pBasePlate->ucs.axis_z*pBasePlate->GetNormOffset();
		xueban_btm_vertex[1]+=pBasePlate->ucs.axis_z*pBasePlate->GetNormOffset();
	}
	coord_trans(xueban_btm_vertex[0],ucs,FALSE);
	coord_trans(xueban_btm_vertex[1],ucs,FALSE);
	//3.计算基准依赖角钢连接螺栓端形成的轮廓顶点
	CLDSGeneralPlate* pBoltShoePlate=this;	//获取一块与依赖角钢有螺栓连接靴板
	if(profile_side!=0&&iConnectStyle==0)
		pBoltShoePlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(abs(hConnectShoePlate),CLS_PLATE,CLS_PARAMPLATE);
	if(pBoltShoePlate==NULL)
		return FALSE;
	CLsRef* pLsRef,*pEndLsRef=NULL;
	CMaxDouble max_base_len;
	for(pLsRef=pBoltShoePlate->GetFirstLsRef();pLsRef;pLsRef=pBoltShoePlate->GetNextLsRef())
	{
		if( pBaseAngle->FindLsByHandle(pLsRef->GetLsPtr()->handle)||
			(arrSonBaseAnglePtrs[1]&&arrSonBaseAnglePtrs[1]->FindLsByHandle(pLsRef->GetLsPtr()->handle))||
			(arrSonBaseAnglePtrs[2]&&arrSonBaseAnglePtrs[2]->FindLsByHandle(pLsRef->GetLsPtr()->handle))||
			(arrSonBaseAnglePtrs[3]&&arrSonBaseAnglePtrs[3]->FindLsByHandle(pLsRef->GetLsPtr()->handle)))
		{
			f3dPoint len_vec=pLsRef->GetLsPtr()->ucs.origin-ucs.origin;
			max_base_len.Update(len_vec*lenVec,pLsRef->GetLsPtr());
		}
	}
	if(!max_base_len.IsInited())
		max_base_len.Update(0);	//靴板上无螺栓时，max_base_len需手动进行初始化
	//处理依赖主角钢上两肢错孔情况
	//TODO:未处理脚钉距靴板较近时的干扰 wjh-2015.5.21
	//TODO:Task 2 wjh-2018.1.25
	for(pLsRef=pBaseAngle->GetFirstLsRef();pLsRef;pLsRef=pBaseAngle->GetNextLsRef())
	{
		f3dPoint len_vec=pLsRef->GetLsPtr()->ucs.origin-ucs.origin;
		double len_dist=len_vec*lenVec;
		if(len_dist<max_base_len.number+60)
		{
			CLDSBolt* pNearBolt=pLsRef->pBolt;
			for(THICK_ITEM item=pNearBolt->GetFirstL0ThickItem();!item.IsFinalKey();item=pNearBolt->GetNextL0ThickItem())
			{
				if(!item.IsHandle()||abs(item.val)==pBaseAngle->handle)
					continue;
				SmartPartPtr pL0Part=BelongModel()->FromPartHandle(item.val);
				if(pL0Part.IsHasPtr()&&pL0Part->IsPlate())
				{
					max_base_len.Update(len_dist,pLsRef->GetLsPtr());
					break;
				}
			}
		}
	}
	LSSPACE_STRU LsSpace;
	CLDSBolt* pEndBolt=(CLDSBolt*)max_base_len.m_pRelaObj;
	if(pEndBolt)
		GetLsSpace(LsSpace,pEndBolt->get_d());
	else
		LsSpace.EndSpace=30;
	max_base_len.number+=LsSpace.EndSpace;
	max_base_len.Update(fAlongVecLen);	//取设定长度与根据螺栓计算长度的较大值 wht 15-07-04
	f3dPoint alongVec=lenVec;	//依赖主材角钢的纵向延伸方向
	vector_trans(alongVec,ucs,FALSE);
	f3dPoint alongVertVec=lenVec;//垂直于主材纵向朝外的偏移方向(X+或Y-)
	RotateVectorAroundVector(alongVertVec,1,0,ucs.axis_z);
	GEPOINT outwardvec;	//朝靴板外侧的验证方向
	if(work_direct=='X')
		outwardvec=pBaseAngle->vyWingNorm;
	else //if(work_direct=='Y')
		outwardvec=pBaseAngle->vxWingNorm;
	project_vector(outwardvec,pBasePlate->ucs.axis_z);
	if(alongVertVec*outwardvec<0)
		alongVertVec*=-1;
	vector_trans(alongVertVec,ucs,FALSE);
	alongVertVec.z=0;
	f3dPoint baseAngleEdgeVertexArr[4];	//依次为前侧顶点十字交叉焊点,角钢楞点,角钢肢翼顶点
	baseAngleEdgeVertexArr[2]=alongVec*(max_base_len.number);	//+LsSpace.EndSpace已考虑到max_base_len中 wht 15-07-02
	if(pBaseGroupAngle)
		B+=ftoi(pBaseGroupAngle->GetWidth()+pBaseGroupAngle->jg_space);
	IB+=ftoi(pBaseAngle->GetWidth());
	baseAngleEdgeVertexArr[1]=baseAngleEdgeVertexArr[2]+alongVertVec*(pBoltShoePlate->Thick+norm_offset);
	baseAngleEdgeVertexArr[0]=baseAngleEdgeVertexArr[2]+alongVertVec*B;
	baseAngleEdgeVertexArr[3]=baseAngleEdgeVertexArr[2]-alongVertVec*IB;
	baseAngleEdgeVertexArr[2]+=alongVertVec*norm_offset;
	//TODO:此处应根据另一肢靠接贯通靴板的轮廓进行修正
	if(pBoltShoePlate!=this)
	{
		f3dPoint localLenVec=lenVec;
		f3dPoint pick=baseAngleEdgeVertexArr[2];
		coord_trans(pick,ucs,TRUE);
		vector_trans(localLenVec,pBoltShoePlate->ucs,FALSE);
		coord_trans(pick,pBoltShoePlate->ucs,FALSE);
		f2dLine line(pick,pick+localLenVec*10000);
		double inters_x,inters_y;
		PROFILE_VERTEX* pPrvVertex=pBoltShoePlate->vertex_list.GetTail();
		for(pVertex=pBoltShoePlate->vertex_list.GetFirst();pVertex;pVertex=pBoltShoePlate->vertex_list.GetNext())
		{
			if(Int2dll(line,f2dLine(pPrvVertex->vertex,pVertex->vertex),inters_x,inters_y)>0)
				break;
			pPrvVertex=pVertex;
		}
		if(pVertex)
		{
			pick.Set(inters_x,inters_y,pick.z);
			coord_trans(pick,pBoltShoePlate->ucs,TRUE);
			coord_trans(pick,ucs,FALSE);
			baseAngleEdgeVertexArr[2]=pick;
		}
		//baseAngleEdgeVertexArr[1]=baseAngleEdgeVertexArr[2]+alongVertVec*thick;
		//由于存在斜材连接边高于主材连接边导致主材连接边不与主材垂直情况存在，以上代码改进如下 wht 15-07-06
		f3dPoint edge_vec=baseAngleEdgeVertexArr[0]-baseAngleEdgeVertexArr[2];
		normalize(edge_vec);
		double offset_len=thick,cosa=fabs(alongVertVec*edge_vec);
		if(cosa>0)
			offset_len=thick/cosa;
		baseAngleEdgeVertexArr[1]=baseAngleEdgeVertexArr[2]+edge_vec*offset_len;
	}
	//4.计算射线斜材的连接螺栓端形成的轮廓顶点
	f3dPoint linkAngleEdgeVertexArr[2];
	CLDSLineAngle* pLinkAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(hLinkXieAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(profile_side!=2&&pLinkAngle)
	{
		BOLTSET boltset;
		f3dPoint rayVertVec,rayVec=pLinkAngle->Start()-pLinkAngle->End();
		if(pLinkAngle->pStart&&pLinkAngle->pStart->handle==hBaseNode)
			rayVec*=-1.0;
		vector_trans(rayVec,ucs,FALSE);
		rayVec.z=0;
		normalize(rayVec);
		rayVertVec.Set(-rayVec.y,rayVec.x);
		CMaxDouble max_vert_dist,max_len_dist;
		CMinDouble min_vert_dist;
		f3dPoint prjpos,len_vec;
		double max_bolt_d=16;
		for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(pLinkAngle->FindLsByHandle(pLsRef->GetLsPtr()->handle))
			{
				boltset.append(pLsRef->GetLsPtr());
				prjpos=pLsRef->GetLsPtr()->ucs.origin;
				project_point(prjpos,pLinkAngle->Start(),pLsRef->GetLsPtr()->get_norm());
				len_vec=prjpos-ucs.origin;
				vector_trans(len_vec,ucs,FALSE);
				double len=len_vec*rayVec;
				double vert=len_vec*rayVertVec;
				max_len_dist.Update(len,pLsRef->GetLsPtr());
				min_vert_dist.Update(vert);
				max_vert_dist.Update(vert);
			}
		}
		if(max_len_dist.IsInited())
		{
			CLDSBolt* pEndBolt=(CLDSBolt*)max_len_dist.m_pRelaObj;
			if(pEndBolt)
				GetLsSpace(LsSpace,pEndBolt->get_d());
			else
				LsSpace.EndSpace=30;
			if(pLinkAngle->pStart&&pLinkAngle->pStart->handle==hBaseNode)
				len_vec=pLinkAngle->End()-ucs.origin;
			else
				len_vec=pLinkAngle->Start()-ucs.origin;
			vector_trans(len_vec,ucs,FALSE);
			double ber_vert_dist=len_vec*rayVertVec;
			if(max_vert_dist.number-ber_vert_dist>0)//fabs(min_vert_dist.number-ber_vert_dist))
			{
				linkAngleEdgeVertexArr[0]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(ber_vert_dist-IC)*rayVertVec;
				linkAngleEdgeVertexArr[1]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(ber_vert_dist+pLinkAngle->GetWidth()+ID)*rayVertVec;
			}
			else
			{
				linkAngleEdgeVertexArr[0]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(ber_vert_dist+IC)*rayVertVec;
				linkAngleEdgeVertexArr[1]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(ber_vert_dist-pLinkAngle->GetWidth()-ID)*rayVertVec;
			}
			//由于一般靴板要求完全覆盖隹连接的射线斜材，故不建议用以下四行代码 wjh-2015.4.21
			//if(max_vert_dist.number-ber_vert_dist>0)//fabs(min_vert_dist.number-ber_vert_dist))
			//	linkAngleEdgeVertexArr[1]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(max_vert_dist.number+LsSpace.EndSpace)*rayVertVec;
			//else
			//	linkAngleEdgeVertexArr[1]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(min_vert_dist.number-LsSpace.EndSpace)*rayVertVec;
			if(fabs(linkAngleEdgeVertexArr[1].y)<fabs(linkAngleEdgeVertexArr[0].y))
			{	//保证linkAngleEdgeVertexArr[0]为较靠近底板的顶点
				f3dPoint pt=linkAngleEdgeVertexArr[0];
				linkAngleEdgeVertexArr[0]=linkAngleEdgeVertexArr[1];
				linkAngleEdgeVertexArr[1]=pt;
			}
		}
		else	//无连接螺栓
			pLinkAngle=NULL;
	}
	f3dPoint horiLinkAngleEdgeVertexArr[2];
	CLDSLineAngle* pHoriLinkAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(hLinkHoriAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(profile_side!=2&&pHoriLinkAngle)
	{
		BOLTSET boltset;
		f3dPoint rayVertVec,rayVec=pHoriLinkAngle->Start()-pHoriLinkAngle->End();
		if(pHoriLinkAngle->pStart&&pHoriLinkAngle->pStart->handle==hBaseNode)
			rayVec*=-1.0;
		vector_trans(rayVec,ucs,FALSE);
		rayVec.z=0;
		normalize(rayVec);
		rayVertVec.Set(-rayVec.y,rayVec.x);
		CMaxDouble max_vert_dist,max_len_dist;
		CMinDouble min_vert_dist;
		f3dPoint prjpos,len_vec;
		double max_bolt_d=16;
		for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(pHoriLinkAngle->FindLsByHandle(pLsRef->GetLsPtr()->handle))
			{
				boltset.append(pLsRef->GetLsPtr());
				prjpos=pLsRef->GetLsPtr()->ucs.origin;
				project_point(prjpos,pHoriLinkAngle->Start(),pLsRef->GetLsPtr()->get_norm());
				len_vec=prjpos-ucs.origin;
				vector_trans(len_vec,ucs,FALSE);
				double len=len_vec*rayVec;
				double vert=len_vec*rayVertVec;
				max_len_dist.Update(len,pLsRef->GetLsPtr());
				min_vert_dist.Update(vert);
				max_vert_dist.Update(vert);
			}
		}
		if(max_len_dist.IsInited())
		{
			CLDSBolt* pEndBolt=(CLDSBolt*)max_len_dist.m_pRelaObj;
			if(pEndBolt)
				GetLsSpace(LsSpace,pEndBolt->get_d());
			else
				LsSpace.EndSpace=30;
			if(pHoriLinkAngle->pStart&&pHoriLinkAngle->pStart->handle==hBaseNode)
				len_vec=pHoriLinkAngle->End()-ucs.origin;
			else
				len_vec=pHoriLinkAngle->Start()-ucs.origin;
			vector_trans(len_vec,ucs,FALSE);
			double ber_vert_dist=len_vec*rayVertVec;
			horiLinkAngleEdgeVertexArr[0]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+ber_vert_dist*rayVertVec;
			if(max_vert_dist.number-ber_vert_dist>0)//fabs(min_vert_dist.number-ber_vert_dist))
				horiLinkAngleEdgeVertexArr[1]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(max_vert_dist.number+LsSpace.EndSpace)*rayVertVec;
			else
				horiLinkAngleEdgeVertexArr[1]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(min_vert_dist.number-LsSpace.EndSpace)*rayVertVec;
			if(fabs(horiLinkAngleEdgeVertexArr[1].y)<fabs(horiLinkAngleEdgeVertexArr[0].y))
			{	//保证linkAngleEdgeVertexArr[0]为较靠近底板的顶点
				f3dPoint pt=horiLinkAngleEdgeVertexArr[0];
				horiLinkAngleEdgeVertexArr[0]=horiLinkAngleEdgeVertexArr[1];
				horiLinkAngleEdgeVertexArr[1]=pt;
			}
		}
		else	//无连接螺栓
			pHoriLinkAngle=NULL;
	}
	f3dPoint vertex,offset_vec;
	WORD wKey=1;
	if(lenVec*pBasePlate->ucs.axis_z>0)
	{	//上侧靴板
		if (pBaseGroupAngle != NULL)
		{	//成都塔厂向刘伟反馈，单角钢时由于加工习惯以般以底面（不是靴板中性层，但组合角钢是中性层）为基准计算焊接边 wjh-2019.8.8
			xueban_btm_vertex[0]=MoveVertexByMidPlane(Thick*0.5,xueban_btm_vertex[0],ucs.axis_x,ucs.axis_z,pBasePlate->ucs.axis_z);
			xueban_btm_vertex[1]=MoveVertexByMidPlane(Thick*0.5,xueban_btm_vertex[1],ucs.axis_x,ucs.axis_z,pBasePlate->ucs.axis_z);
		}
		if(profile_side==0)		//外形类别:0.贯穿底板;1.肢翼单侧;2.楞线单侧
		{
			if(ciOldShemaWorkDirect==0)
			{	//X轴沿向靴板
				if(ucs.axis_x*outwardvec>0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;	//进行焊接
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(ciOldShemaWorkDirect==0&&A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(ciOldShemaWorkDirect==0&&A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;	//进行焊接
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
				}
			}
			else
			{	//Y轴沿向靴板
				if(ucs.axis_x*outwardvec<0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;	//进行焊接
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;	//进行焊接
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
			}
		}
		else if(profile_side==1)
		{	//肢翼侧截断板
			f3dPoint line_pick=baseAngleEdgeVertexArr[2];
			coord_trans(line_pick,ucs,TRUE);
			f3dPoint btm_fore_vertice;	//楞线侧(前侧)截断靴板底部顶点(与另一垂向靴板的贴合顶点)
			Int3dlf(btm_fore_vertice,line_pick,lenVec,datumPlaneOrg,pBasePlate->ucs.axis_z);
			coord_trans(btm_fore_vertice,ucs,FALSE);
			btm_fore_vertice.z=0;
			if (pBaseGroupAngle != NULL)
			{	//成都塔厂向刘伟反馈，单角钢时由于加工习惯以般以底面（不是靴板中性层，但组合角钢是中性层）为基准计算焊接边 wjh-2019.8.8
				btm_fore_vertice=MoveVertexByMidPlane(Thick*0.5,btm_fore_vertice,ucs.axis_x,ucs.axis_z,pBasePlate->ucs.axis_z);
			}
			double buttweld_offset_x=0;
			f3dPoint buttweld_offsetvec;
			if(bButtWeld)
			{
				buttweld_offset_x=pBaseAngle->GetWidth();
				buttweld_offsetvec=-buttweld_offset_x*alongVertVec;
				if(fabs(alongVertVec.x)>EPS)
					buttweld_offset_x/=-alongVertVec.x;
			}
			//TODO:ciOldShemaWorkDirect==0是不是应该改为work_direct=='X'判断 wjh-2018.1.25
			if(ciOldShemaWorkDirect==0)
			{	//X轴沿向靴板
				if(ucs.axis_x*outwardvec>0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
					}
				}
			}
			else
			{	//Y轴沿向靴板
				if(ucs.axis_x*outwardvec<0)
				{
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[1].x+buttweld_offset_x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
			}
		}
		else
		{	//楞线侧截断板
			f3dPoint btm_fore_vertice;	//楞线侧(前侧)截断靴板底部顶点(与另一垂向靴板的贴合顶点)
			f3dPoint line_pick=baseAngleEdgeVertexArr[1];
			if(bButtWeld)	//对接靴板以2号点为求交拾取点(不能重复考虑靴板厚度偏移）
				line_pick=baseAngleEdgeVertexArr[2];
			coord_trans(line_pick,ucs,TRUE);
			Int3dlf(btm_fore_vertice,line_pick,lenVec,datumPlaneOrg,pBasePlate->ucs.axis_z);
			coord_trans(btm_fore_vertice,ucs,FALSE);
			btm_fore_vertice.z=0;
			if (pBaseGroupAngle != NULL)
			{	//成都塔厂向刘伟反馈，单角钢时由于加工习惯以般以底面（不是靴板中性层，但组合角钢是中性层）为基准计算焊接边 wjh-2019.8.8
				btm_fore_vertice=MoveVertexByMidPlane(Thick*0.5,btm_fore_vertice,ucs.axis_x,ucs.axis_z,pBasePlate->ucs.axis_z);
			}
			if(ciOldShemaWorkDirect==0)
			{	//X轴沿向靴板
				if(ucs.axis_x*outwardvec>0)
				{
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(bButtWeld)
						pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					else
						pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
				}
			}
			else
			{	//Y轴沿向靴板
				if(ucs.axis_x*outwardvec<0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(bButtWeld)
						pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					else
						pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
					}
				}
			}
		}
	}
	else
	{	//下侧靴板
		if (pBaseGroupAngle != NULL)
		{	//成都塔厂向刘伟反馈，单角钢时由于加工习惯以般以底面（不是靴板中性层，但组合角钢是中性层）为基准计算焊接边 wjh-2019.8.8
			xueban_btm_vertex[0]=MoveVertexByMidPlane(Thick*0.5,xueban_btm_vertex[0],ucs.axis_x,ucs.axis_z,-pBasePlate->ucs.axis_z);
			xueban_btm_vertex[1]=MoveVertexByMidPlane(Thick*0.5,xueban_btm_vertex[1],ucs.axis_x,ucs.axis_z,-pBasePlate->ucs.axis_z);
		}
		if(profile_side==0)		//外形类别:0.贯穿底板;1.肢翼单侧;2.楞线单侧
		{
			if(ciOldShemaWorkDirect==0)
			{	//X轴沿向靴板
				if(ucs.axis_x*outwardvec<0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y-IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pHoriLinkAngle)
					{
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y-A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y-A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pHoriLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y-IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
			}
			else
			{	//Y轴沿向靴板
				if(ucs.axis_x*outwardvec>0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y-A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pHoriLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y-IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y-IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pHoriLinkAngle)
					{
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y-A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
				}
			}
		}
		else if(profile_side==1)
		{	//肢翼侧截断板
			f3dPoint line_pick=baseAngleEdgeVertexArr[2];
			coord_trans(line_pick,ucs,TRUE);
			f3dPoint btm_fore_vertice;	//楞线侧(前侧)截断靴板底部顶点(与另一垂向靴板的贴合顶点)
			Int3dlf(btm_fore_vertice,line_pick,lenVec,datumPlaneOrg,pBasePlate->ucs.axis_z);
			coord_trans(btm_fore_vertice,ucs,FALSE);
			btm_fore_vertice.z=0;
			if (pBaseGroupAngle != NULL)
			{	//成都塔厂向刘伟反馈，单角钢时由于加工习惯以般以底面（不是靴板中性层，但组合角钢是中性层）为基准计算焊接边 wjh-2019.8.8
				btm_fore_vertice=MoveVertexByMidPlane(Thick*0.5,btm_fore_vertice,ucs.axis_x,ucs.axis_z,-pBasePlate->ucs.axis_z);
			}
			double buttweld_offset_x=0;
			f3dPoint buttweld_offsetvec;
			if(bButtWeld)
			{
				buttweld_offset_x=pBaseAngle->GetWidth();
				buttweld_offsetvec=-buttweld_offset_x*alongVertVec;
				if(fabs(alongVertVec.x)>EPS)
					buttweld_offset_x/=-alongVertVec.x;
			}
			if(ciOldShemaWorkDirect==0)
			{	//X轴沿向靴板
				if(ucs.axis_x*outwardvec<0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y-IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pHoriLinkAngle)
					{
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW)
					{
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
				}
				else
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pHoriLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y-IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
			}
			else
			{	//Y轴沿向靴板
				if(ucs.axis_x*outwardvec>0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pHoriLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y-IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
				else
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y-IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pHoriLinkAngle)
					{
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW)
					{
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
				}
			}
		}
		else
		{	//楞线侧截断板
			f3dPoint btm_fore_vertice;	//楞线侧(前侧)截断靴板底部顶点(与另一垂向靴板的贴合顶点)
			f3dPoint line_pick=baseAngleEdgeVertexArr[1];
			coord_trans(line_pick,ucs,TRUE);
			Int3dlf(btm_fore_vertice,line_pick,lenVec,datumPlaneOrg,pBasePlate->ucs.axis_z);
			coord_trans(btm_fore_vertice,ucs,FALSE);
			btm_fore_vertice.z=0;
			if (pBaseGroupAngle != NULL)
			{	//成都塔厂向刘伟反馈，单角钢时由于加工习惯以般以底面（不是靴板中性层，但组合角钢是中性层）为基准计算焊接边 wjh-2019.8.8
				btm_fore_vertice=MoveVertexByMidPlane(Thick*0.5,btm_fore_vertice,ucs.axis_x,ucs.axis_z,-pBasePlate->ucs.axis_z);
			}
			if(ciOldShemaWorkDirect==0)
			{	//X轴沿向靴板
				if(ucs.axis_x*outwardvec<0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y-A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
				}
				else
				{
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y-A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
				}
			}
			else
			{	//Y轴沿向靴板
				if(ucs.axis_x*outwardvec>0)
				{
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y-A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
				}
				else
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y-A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
				}
			}
		}
	}
	//return TRUE;
	//为保证基板X、Y轴沿向靴板外形一致,去除凹点时以基板Y轴沿向靴板为基准，
	//X轴向靴板需翻转(链表逆序顶点坐标取反)去除凹点 wht 16-04-15
	ATOM_LIST<PROFILE_VERTEX> temp_vertex_list;
	//生成去除凹点所需顶点链表
	BOOL bReverse=fabs(ucs.axis_x*pBasePlate->ucs.axis_x)>fabs(ucs.axis_x*pBasePlate->ucs.axis_y);
	if(bReverse)
	{
		for(PROFILE_VERTEX* pVertex=vertex_list.GetTail();pVertex;pVertex=vertex_list.GetPrev())
		{
			pVertex->vertex.x*=-1;
			temp_vertex_list.append(*pVertex);
		}
	}
	else
	{
		for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			temp_vertex_list.append(*pVertex);
	}
	//依照无凹点原则重新修正靴板外形
	pPrevVertex=temp_vertex_list.GetTail();
	PROFILE_VERTEX* pFirstVertex=temp_vertex_list.GetFirst();
	for(PROFILE_VERTEX* pVertex=pFirstVertex;pVertex;pVertex=temp_vertex_list.GetNext())
	{
		BOOL pushed=temp_vertex_list.push_stack();
		PROFILE_VERTEX* pNextVertex=temp_vertex_list.GetNext();
		if(pNextVertex==NULL)
			pNextVertex=pFirstVertex;
		temp_vertex_list.pop_stack(pushed);
		if(DistOf2dPtLine(pNextVertex->vertex,pPrevVertex->vertex,pVertex->vertex)<EPS)
			temp_vertex_list.DeleteCursor();
		else
			pPrevVertex=pVertex;
	}
	temp_vertex_list.Clean();
	//恢复顶点链表
	vertex_list.Empty();
	if(bReverse)
	{
		for(PROFILE_VERTEX* pVertex=temp_vertex_list.GetTail();pVertex;pVertex=temp_vertex_list.GetPrev())
		{
			pVertex->vertex.x*=-1;
			vertex_list.append(*pVertex);
		}
	}
	else
	{
		for(PROFILE_VERTEX* pVertex=temp_vertex_list.GetFirst();pVertex;pVertex=temp_vertex_list.GetNext())
			vertex_list.append(*pVertex);
	}
	return TRUE;
}
BOOL CLDSParamPlate::DesignShoePlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)//设计靴板
{
	long iConnectStyle=0,iCutMode=0;
	long hConnectBasePlate=0,hConnectShoePlate=0,hDependentAngle=0,hLinkHoriAngle=0,hLinkXieAngle=0,work_direct=0,style_bolt0_weld1=0;
	long profile_side=0,A=0,B=0,bolt_layoutstyle=0,normOffDist=0,IA=0,IB=0,IC=0,ID=0;
	double fCutAngleW=0,fMargin=0,fAlongVecLen=0;
	//work_direct=='X'表示靴板在连接基准角钢的X肢，'Y'表示在Y肢
	if(!GetDesignItemValue('W',&work_direct,pItemHashTbl))	//靴板相对底板的坐标轴向0.X轴;1.Y轴
#ifdef AFX_TARG_ENU_ENGLISH
		Log2File()->Log("0X%X shoe plate lacks parameter of extending axis direction,defaults along X axis of base plate!",handle);
#else
		Log2File()->Log("0X%X靴板缺少延展轴向参数,默认沿底板X轴方向!",handle);
#endif
	if(work_direct=='X'||work_direct=='Y')
		return DesignShoePlate2(pItemHashTbl);
	GetDesignItemValue('A',&A,pItemHashTbl);//台口高
	GetDesignItemValue('B',&B,pItemHashTbl);//顶部余量
	GetDesignItemValue(KEY2C("IA"),&IA,pItemHashTbl);//内侧台口高
	GetDesignItemValue(KEY2C("IB"),&IB,pItemHashTbl);//顶部内侧余量
	GetDesignItemValue('M',&fMargin,pItemHashTbl);//靴板延伸至底板边缘时的间隙值
	GetDesignItemValue(KEY2C("AL"),&fAlongVecLen,pItemHashTbl);		//靴板延杆件长度方向延伸量(默认该延伸量根据螺栓计算)
	GetDesignItemValue(KEY2C("CM"),&iCutMode,pItemHashTbl);
	GetDesignItemValue('L',&fCutAngleW,pItemHashTbl);	//切角宽度
	GetDesignItemValue(KEY2C("IC"),&IC,pItemHashTbl);//斜材楞线侧延伸量
	GetDesignItemValue(KEY2C("ID"),&ID,pItemHashTbl);//斜材肢翼侧延伸量
	GetDesignItemValue(KEY2C("LX"),&hLinkXieAngle,pItemHashTbl);	//连接角钢
	GetDesignItemValue(KEY2C("LH"),&hLinkHoriAngle,pItemHashTbl);	//连接角钢
	GetDesignItemValue(KEY2C("CS"),&iConnectStyle,pItemHashTbl);	//与依赖角钢间的连接方式, 0.螺栓连接;1.对焊连接
	GetDesignItemValue(KEY2C("NL"),&normOffDist,pItemHashTbl);		//靴板沿法线方向偏移距离
	if(!GetDesignItemValue('C',&hConnectBasePlate,pItemHashTbl))//连接底板
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X shoe plate lacks parameter of connecting base plate,failed design!",handle);
#else
			log2file->Log("0X%X靴板缺少连接底板参数,设计失败!",handle);
#endif
		return FALSE;
	}
	if(!GetDesignItemValue('D',&hDependentAngle,pItemHashTbl))	//依赖角钢
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X parametric plate lacks parameter of reference angle,failed design!",handle);
#else
			log2file->Log("0X%X参数化钢板缺少依赖角钢参数,设计失败!",handle);
#endif
		return FALSE;
	}
	if(!GetDesignItemValue('F',&profile_side,pItemHashTbl)&&log2file)		//外形类别:0.贯穿底板;1.肢翼单侧;2.楞线单侧
#ifdef AFX_TARG_ENU_ENGLISH
		log2file->Log("0X%X shoe plate lacks parameter of shape type,defaults through base plate!",handle);
#else
		log2file->Log("0X%X靴板缺少外形类别参数,默认为贯穿底板的靴板!",handle);
#endif
	if(!GetDesignItemValue('G',&hConnectShoePlate,pItemHashTbl)&&profile_side==2&&iConnectStyle==0)		//连接靴板
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X shoe plate lacks parameter of connecting shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X靴板缺少靠接靴板参数,设计失败!",handle);
#endif
		return FALSE;
	}
	GetDesignItemValue('H',&bolt_layoutstyle,pItemHashTbl);	//螺栓布置型式:0.左高右低;1.左低右高
	CLDSGeneralPlate* pBasePlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(hConnectBasePlate,CLS_PLATE,CLS_PARAMPLATE);
	if(pBasePlate==NULL)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X missing the connecting base plate specified for shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X靴板的指定连接底板丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	CLDSGroupLineAngle* pBaseGroupAngle=NULL;
	CLDSLineAngle* pBaseAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(hDependentAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	DYN_ARRAY<CLDSLineAngle*> arrSonBaseAnglePtrs(4);
	arrSonBaseAnglePtrs.Clear();
	if(pBaseAngle&&pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		pBaseGroupAngle=(CLDSGroupLineAngle*)pBaseAngle;
		arrSonBaseAnglePtrs[0]=pBaseAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pBaseGroupAngle->son_jg_h[0]);
		for(int k=1;k<4;k++)
			arrSonBaseAnglePtrs[k]=(CLDSLineAngle*)BelongModel()->FromPartHandle(pBaseGroupAngle->son_jg_h[k]);
	}
	if(pBaseAngle==NULL)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X missing the reference angle specified for shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X靴板指定依赖角钢丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	//1.计算靴板装配坐标系
	long hBaseNode=pBasePlate->m_hBaseNode;
	f3dPoint lenVec;
	if(pBaseAngle->pStart&&pBaseAngle->pStart->handle==hBaseNode)
		lenVec=pBaseAngle->End()-pBaseAngle->Start();
	else if(pBaseAngle->pEnd&&pBaseAngle->pEnd->handle==hBaseNode)
		lenVec=pBaseAngle->Start()-pBaseAngle->End();
	else
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X without connection between reference angle specified for shoe and node that base plate depends,failed design!",handle);
#else
			log2file->Log("0X%X靴板指定依赖角钢与底板依赖节点无连接,设计失败!",handle);
#endif
		return FALSE;
	}
	normalize(lenVec);
	char cWorkWing='X';
	f3dPoint wing_vec_x=pBaseAngle->GetWingVecX();
	if(work_direct==0)
	{	//X轴沿向靴板
		if(fabs(wing_vec_x*pBasePlate->ucs.axis_x)>0.707)	//X肢为当前连接工作肢
			cWorkWing='X';
		else 	//Y肢为当前连接工作肢
			cWorkWing='Y';
	}
	else
	{	//Y轴沿向靴板
		if(fabs(wing_vec_x*pBasePlate->ucs.axis_y)>0.707)	//X肢为当前连接工作肢
			cWorkWing='X';
		else 	//Y肢为当前连接工作肢
			cWorkWing='Y';
	}
	ucs.axis_z=(cWorkWing=='X')?pBaseAngle->vxWingNorm:pBaseAngle->vyWingNorm;
	if(pBaseGroupAngle&&pBaseGroupAngle->wSectStyle!=1&&lenVec.z*ucs.axis_z.z<0)
	{	//异型组合角钢塔脚旋转或X/Y对称时很可能由于靴板的底面不同导致外形出现细微差异，实际应取中性层面，现暂时临时取法向与主材射向一致来避免 wjh-2017.6.6
		if(pBaseGroupAngle->wSectStyle==0)
			pBaseAngle=arrSonBaseAnglePtrs[1];
		else if(pBaseGroupAngle->wSectStyle==2)
			pBaseAngle=arrSonBaseAnglePtrs[2];
		if(pBaseAngle!=arrSonBaseAnglePtrs[0])
			ucs.axis_z*=-1.0;	//变换基准子角钢成功
	}
	ucs.axis_x=pBasePlate->ucs.axis_z^ucs.axis_z;
	normalize(ucs.axis_x);
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	if(ucs.axis_y*lenVec<0)
	{	//确保Y轴与依赖主材角钢射线方向一致
		ucs.axis_x*=-1.0;
		ucs.axis_y*=-1.0;
	}
	BYTE ciOldShemaWorkDirect=0;
	if( (cWorkWing=='X'&&(ucs.axis_x*pBaseAngle->vyWingNorm<0))||
		(cWorkWing=='Y'&&(ucs.axis_x*pBaseAngle->vxWingNorm<0)))
		ciOldShemaWorkDirect=1;
	f3dPoint datumPlaneOrg=pBasePlate->ucs.origin;
	if(lenVec.z<0)		//基准角钢在底板上侧
		datumPlaneOrg+=pBasePlate->ucs.axis_z*(pBasePlate->GetNormOffset()+pBasePlate->GetThick());
	else if(lenVec.z>0)	//基准角钢在底板下侧
		datumPlaneOrg+=pBasePlate->ucs.axis_z*pBasePlate->GetNormOffset();
	//计算由组合角钢间隙大于靴板厚带来的靴板法向位移偏差 wjh-2015.9.16
	double norm_offset=0;
	if(iConnectStyle==0&&pBaseAngle->group_father_jg_h>0X20)
		pBaseGroupAngle=(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(pBaseAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
	if(pBaseGroupAngle&&pBaseGroupAngle->jg_space!=GetThick())
		norm_offset=0.5*(pBaseGroupAngle->jg_space-GetThick());
	f3dPoint pick=pBaseAngle->Start();
	if(fabs(norm_offset)>EPS)
		pick+=ucs.axis_z*norm_offset;
	bool bButtWeld=false;	//靴板与依赖角钢肢实际以对接形式焊接
	if(iConnectStyle==1)
	{
		pick-=ucs.axis_z*thick;	//单角钢对焊连接
		bButtWeld=true;
	}
	if(bButtWeld && abs(normOffDist)>0)	//靴板沿法线方向偏移距离
		pick+=ucs.axis_z*normOffDist;
	Int3dlf(ucs.origin,pick,lenVec,datumPlaneOrg,pBasePlate->ucs.axis_z);
	/* 将来应以中性层为原则计算外形顶点　wjh-2017.6.6
	f3dPoint midorg,midpick=pick+ucs.axis_z*(thick*0.5);
	Int3dlf(midorg,midpick,lenVec,datumPlaneOrg,pBasePlate->ucs.axis_z);
	midorg=ucs.origin;*/
	//2.设计靴板外形
	f3dPoint origin=ucs.origin;
	f3dPoint axisx=ucs.axis_x;
	coord_trans(origin,pBasePlate->ucs,FALSE,TRUE);
	vector_trans(axisx,pBasePlate->ucs,FALSE,TRUE);
	origin.z=0;
	f3dArcLine arcline;
	GEPOINT inters,inters2;
	CMaxDouble left,right;
	PROFILE_VERTEX* pVertex=NULL,*pPrevVertex=pBasePlate->vertex_list.GetTail();
	for(pVertex=pBasePlate->vertex_list.GetFirst();pVertex;pVertex=pBasePlate->vertex_list.GetNext())
	{
		pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&pBasePlate->ucs);
		if(arcline.SectorAngle()==0)	//直线
		{
			if(Int2dll(f2dLine(pPrevVertex->vertex,pVertex->vertex),f2dLine(origin-axisx*10000,origin+axisx*10000),inters.x,inters.y)>0)
			{
				GEPOINT distvec(inters.x-origin.x,inters.y-origin.y);
				double dist =distvec*axisx;
				if(dist<0)
					left.Update(-dist);
				if(dist>0)
					right.Update(dist);
			}
		}
		else //if(arcline.ColumnNorm().IsZero()||fabs(arcline.ColumnNorm()*ucs.axis_z)>EPS_COS)
		{	//普通圆弧(外轮廓一般不会是椭圆故暂未特别处理椭圆)
			f2dArc arc;
			arc.centre=arcline.Center();
			arc.startPt=arcline.PositionInAngle(0);
			arc.angle=arcline.SectorAngle();
			int count=Int2dla(f2dLine(origin-axisx*10000,origin+axisx*10000),arc,inters.x,inters.y,inters2.x,inters2.y);
			if(count>0)
			{
				GEPOINT distvec(inters.x-origin.x,inters.y-origin.y),distvec2;
				if(count==2)
					distvec2.Set(inters2.x-origin.x,inters2.y-origin.y);	//第2个交点最近
				double dist =distvec*axisx;
				if(dist<0)
					left.Update(-dist);
				if(dist>0)
					right.Update(dist);
				dist =distvec2*axisx;
				if(dist<0)
					left.Update(-dist);
				if(dist>0)
					right.Update(dist);	
			}
		}
		pPrevVertex=pVertex;
	}
	bool find_vertex[2]={false,false};
	if(!(find_vertex[0]= left.IsInited()))
		left.Update(0);
	if(!(find_vertex[1]=right.IsInited()))
		right.Update(0);
	if(!find_vertex[0]||!find_vertex[1])
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X shoe plate,doesn't find the enough intersection point with base plate,failed design",handle);
#else
			log2file->Log("0X%X靴板,未找到与底板间的足够交点, 设计失败",handle);
#endif
		return false;
	}
	//靴板底板顶点
	f3dPoint xueban_btm_vertex[2];	//-X->+X或-Y->+Y顺序
	xueban_btm_vertex[0].Set(origin.x+axisx.x*-left.number,origin.y+axisx.y*-left.number);
	xueban_btm_vertex[1].Set(origin.x+axisx.x*right.number,origin.y+axisx.y*right.number);
	xueban_btm_vertex[0]+=axisx*fMargin;
	xueban_btm_vertex[1]-=axisx*fMargin;
	coord_trans(xueban_btm_vertex[0],pBasePlate->ucs,TRUE);
	coord_trans(xueban_btm_vertex[1],pBasePlate->ucs,TRUE);
	if(lenVec.z<0)		//基准角钢在底板上侧
	{
		xueban_btm_vertex[0]+=pBasePlate->ucs.axis_z*(pBasePlate->GetNormOffset()+pBasePlate->GetThick());
		xueban_btm_vertex[1]+=pBasePlate->ucs.axis_z*(pBasePlate->GetNormOffset()+pBasePlate->GetThick());
	}
	else if(lenVec.z>0)	//基准角钢在底板下侧
	{
		xueban_btm_vertex[0]+=pBasePlate->ucs.axis_z*pBasePlate->GetNormOffset();
		xueban_btm_vertex[1]+=pBasePlate->ucs.axis_z*pBasePlate->GetNormOffset();
	}
	coord_trans(xueban_btm_vertex[0],ucs,FALSE);
	coord_trans(xueban_btm_vertex[1],ucs,FALSE);
	//3.计算基准依赖角钢连接螺栓端形成的轮廓顶点
	CLDSGeneralPlate* pBoltShoePlate=this;	//获取一块与依赖角钢有螺栓连接靴板
	if(profile_side!=0&&iConnectStyle==0)
		pBoltShoePlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(abs(hConnectShoePlate),CLS_PLATE,CLS_PARAMPLATE);
	if(pBoltShoePlate==NULL)
		return FALSE;
	CLsRef* pLsRef,*pEndLsRef=NULL;
	CMaxDouble max_base_len;
	for(pLsRef=pBoltShoePlate->GetFirstLsRef();pLsRef;pLsRef=pBoltShoePlate->GetNextLsRef())
	{
		if( pBaseAngle->FindLsByHandle(pLsRef->GetLsPtr()->handle)||
			(arrSonBaseAnglePtrs[1]&&arrSonBaseAnglePtrs[1]->FindLsByHandle(pLsRef->GetLsPtr()->handle))||
			(arrSonBaseAnglePtrs[2]&&arrSonBaseAnglePtrs[2]->FindLsByHandle(pLsRef->GetLsPtr()->handle))||
			(arrSonBaseAnglePtrs[3]&&arrSonBaseAnglePtrs[3]->FindLsByHandle(pLsRef->GetLsPtr()->handle)))
		{
			f3dPoint len_vec=pLsRef->GetLsPtr()->ucs.origin-ucs.origin;
			max_base_len.Update(len_vec*lenVec,pLsRef->GetLsPtr());
		}
	}
	if(!max_base_len.IsInited())
		max_base_len.Update(0);	//靴板上无螺栓时，max_base_len需手动进行初始化
	//处理依赖主角钢上两肢错孔情况
	//TODO:未处理脚钉距靴板较近时的干扰 wjh-2015.5.21
	for(pLsRef=pBaseAngle->GetFirstLsRef();pLsRef;pLsRef=pBaseAngle->GetNextLsRef())
	{
		f3dPoint len_vec=pLsRef->GetLsPtr()->ucs.origin-ucs.origin;
		double len_dist=len_vec*lenVec;
		if(len_dist<max_base_len.number+60)
			max_base_len.Update(len_dist,pLsRef->GetLsPtr());
	}
	LSSPACE_STRU LsSpace;
	CLDSBolt* pEndBolt=(CLDSBolt*)max_base_len.m_pRelaObj;
	if(pEndBolt)
		GetLsSpace(LsSpace,pEndBolt->get_d());
	else
		LsSpace.EndSpace=30;
	max_base_len.number+=LsSpace.EndSpace;
	max_base_len.Update(fAlongVecLen);	//取设定长度与根据螺栓计算长度的较大值 wht 15-07-04
	//
	f3dPoint outwardvec;	//朝靴板外侧的验证方向
	if(cWorkWing=='X')
		outwardvec=pBaseAngle->vyWingNorm;
	else //if(cWorkWing=='Y')
		outwardvec=pBaseAngle->vxWingNorm;
	project_vector(outwardvec,pBasePlate->ucs.axis_z);
	f3dPoint alongVec=lenVec;	//依赖主材角钢的纵向延伸方向
	vector_trans(alongVec,ucs,FALSE);
	f3dPoint alongVertVec=lenVec;//垂直于主材纵向朝外的偏移方向(X+或Y-)
	RotateVectorAroundVector(alongVertVec,1,0,ucs.axis_z);
	if(alongVertVec*outwardvec<0)
		alongVertVec*=-1;
	vector_trans(alongVertVec,ucs,FALSE);
	alongVertVec.z=0;
	f3dPoint baseAngleEdgeVertexArr[4];	//依次为前侧顶点十字交叉焊点,角钢楞点,角钢肢翼顶点
	baseAngleEdgeVertexArr[2]=alongVec*(max_base_len.number);	//+LsSpace.EndSpace已考虑到max_base_len中 wht 15-07-02
	if(pBaseGroupAngle)
		B+=ftoi(pBaseGroupAngle->GetWidth()+pBaseGroupAngle->jg_space);
	IB+=ftoi(pBaseAngle->GetWidth());
	baseAngleEdgeVertexArr[1]=baseAngleEdgeVertexArr[2]+alongVertVec*(pBoltShoePlate->Thick+norm_offset);
	baseAngleEdgeVertexArr[0]=baseAngleEdgeVertexArr[2]+alongVertVec*B;
	baseAngleEdgeVertexArr[3]=baseAngleEdgeVertexArr[2]-alongVertVec*IB;
	baseAngleEdgeVertexArr[2]+=alongVertVec*norm_offset;
	//TODO:此处应根据另一肢靠接贯通靴板的轮廓进行修正
	if(pBoltShoePlate!=this)
	{
		f3dPoint localLenVec=lenVec;
		f3dPoint pick=baseAngleEdgeVertexArr[2];
		coord_trans(pick,ucs,TRUE);
		vector_trans(localLenVec,pBoltShoePlate->ucs,FALSE);
		coord_trans(pick,pBoltShoePlate->ucs,FALSE);
		f2dLine line(pick,pick+localLenVec*10000);
		double inters_x,inters_y;
		PROFILE_VERTEX* pPrvVertex=pBoltShoePlate->vertex_list.GetTail();
		for(pVertex=pBoltShoePlate->vertex_list.GetFirst();pVertex;pVertex=pBoltShoePlate->vertex_list.GetNext())
		{
			if(Int2dll(line,f2dLine(pPrvVertex->vertex,pVertex->vertex),inters_x,inters_y)>0)
				break;
			pPrvVertex=pVertex;
		}
		if(pVertex)
		{
			pick.Set(inters_x,inters_y,pick.z);
			coord_trans(pick,pBoltShoePlate->ucs,TRUE);
			coord_trans(pick,ucs,FALSE);
			baseAngleEdgeVertexArr[2]=pick;
		}
		//baseAngleEdgeVertexArr[1]=baseAngleEdgeVertexArr[2]+alongVertVec*thick;
		//由于存在斜材连接边高于主材连接边导致主材连接边不与主材垂直情况存在，以上代码改进如下 wht 15-07-06
		f3dPoint edge_vec=baseAngleEdgeVertexArr[0]-baseAngleEdgeVertexArr[2];
		normalize(edge_vec);
		double offset_len=thick,cosa=fabs(alongVertVec*edge_vec);
		if(cosa>0)
			offset_len=thick/cosa;
		baseAngleEdgeVertexArr[1]=baseAngleEdgeVertexArr[2]+edge_vec*offset_len;
	}
	//4.计算射线斜材的连接螺栓端形成的轮廓顶点
	f3dPoint linkAngleEdgeVertexArr[2];
	CLDSLineAngle* pLinkAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(hLinkXieAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(profile_side!=2&&pLinkAngle)
	{
		BOLTSET boltset;
		f3dPoint rayVertVec,rayVec=pLinkAngle->Start()-pLinkAngle->End();
		if(pLinkAngle->pStart&&pLinkAngle->pStart->handle==hBaseNode)
			rayVec*=-1.0;
		vector_trans(rayVec,ucs,FALSE);
		rayVec.z=0;
		normalize(rayVec);
		rayVertVec.Set(-rayVec.y,rayVec.x);
		CMaxDouble max_vert_dist,max_len_dist;
		CMinDouble min_vert_dist;
		f3dPoint prjpos,len_vec;
		double max_bolt_d=16;
		for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(pLinkAngle->FindLsByHandle(pLsRef->GetLsPtr()->handle))
			{
				boltset.append(pLsRef->GetLsPtr());
				prjpos=pLsRef->GetLsPtr()->ucs.origin;
				project_point(prjpos,pLinkAngle->Start(),pLsRef->GetLsPtr()->get_norm());
				len_vec=prjpos-ucs.origin;
				vector_trans(len_vec,ucs,FALSE);
				double len=len_vec*rayVec;
				double vert=len_vec*rayVertVec;
				max_len_dist.Update(len,pLsRef->GetLsPtr());
				min_vert_dist.Update(vert);
				max_vert_dist.Update(vert);
			}
		}
		if(max_len_dist.IsInited())
		{
			CLDSBolt* pEndBolt=(CLDSBolt*)max_len_dist.m_pRelaObj;
			if(pEndBolt)
				GetLsSpace(LsSpace,pEndBolt->get_d());
			else
				LsSpace.EndSpace=30;
			if(pLinkAngle->pStart&&pLinkAngle->pStart->handle==hBaseNode)
				len_vec=pLinkAngle->End()-ucs.origin;
			else
				len_vec=pLinkAngle->Start()-ucs.origin;
			vector_trans(len_vec,ucs,FALSE);
			double ber_vert_dist=len_vec*rayVertVec;
			if(max_vert_dist.number-ber_vert_dist>0)//fabs(min_vert_dist.number-ber_vert_dist))
			{
				linkAngleEdgeVertexArr[0]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(ber_vert_dist-IC)*rayVertVec;
				linkAngleEdgeVertexArr[1]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(ber_vert_dist+pLinkAngle->GetWidth()+ID)*rayVertVec;
			}
			else
			{
				linkAngleEdgeVertexArr[0]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(ber_vert_dist+IC)*rayVertVec;
				linkAngleEdgeVertexArr[1]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(ber_vert_dist-pLinkAngle->GetWidth()-ID)*rayVertVec;
			}
			//由于一般靴板要求完全覆盖隹连接的射线斜材，故不建议用以下四行代码 wjh-2015.4.21
			//if(max_vert_dist.number-ber_vert_dist>0)//fabs(min_vert_dist.number-ber_vert_dist))
			//	linkAngleEdgeVertexArr[1]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(max_vert_dist.number+LsSpace.EndSpace)*rayVertVec;
			//else
			//	linkAngleEdgeVertexArr[1]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(min_vert_dist.number-LsSpace.EndSpace)*rayVertVec;
			if(fabs(linkAngleEdgeVertexArr[1].y)<fabs(linkAngleEdgeVertexArr[0].y))
			{	//保证linkAngleEdgeVertexArr[0]为较靠近底板的顶点
				f3dPoint pt=linkAngleEdgeVertexArr[0];
				linkAngleEdgeVertexArr[0]=linkAngleEdgeVertexArr[1];
				linkAngleEdgeVertexArr[1]=pt;
			}
		}
		else	//无连接螺栓
			pLinkAngle=NULL;
	}
	f3dPoint horiLinkAngleEdgeVertexArr[2];
	CLDSLineAngle* pHoriLinkAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(hLinkHoriAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(profile_side!=2&&pHoriLinkAngle)
	{
		BOLTSET boltset;
		f3dPoint rayVertVec,rayVec=pHoriLinkAngle->Start()-pHoriLinkAngle->End();
		if(pHoriLinkAngle->pStart&&pHoriLinkAngle->pStart->handle==hBaseNode)
			rayVec*=-1.0;
		vector_trans(rayVec,ucs,FALSE);
		rayVec.z=0;
		normalize(rayVec);
		rayVertVec.Set(-rayVec.y,rayVec.x);
		CMaxDouble max_vert_dist,max_len_dist;
		CMinDouble min_vert_dist;
		f3dPoint prjpos,len_vec;
		double max_bolt_d=16;
		for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(pHoriLinkAngle->FindLsByHandle(pLsRef->GetLsPtr()->handle))
			{
				boltset.append(pLsRef->GetLsPtr());
				prjpos=pLsRef->GetLsPtr()->ucs.origin;
				project_point(prjpos,pHoriLinkAngle->Start(),pLsRef->GetLsPtr()->get_norm());
				len_vec=prjpos-ucs.origin;
				vector_trans(len_vec,ucs,FALSE);
				double len=len_vec*rayVec;
				double vert=len_vec*rayVertVec;
				max_len_dist.Update(len,pLsRef->GetLsPtr());
				min_vert_dist.Update(vert);
				max_vert_dist.Update(vert);
			}
		}
		if(max_len_dist.IsInited())
		{
			CLDSBolt* pEndBolt=(CLDSBolt*)max_len_dist.m_pRelaObj;
			if(pEndBolt)
				GetLsSpace(LsSpace,pEndBolt->get_d());
			else
				LsSpace.EndSpace=30;
			if(pHoriLinkAngle->pStart&&pHoriLinkAngle->pStart->handle==hBaseNode)
				len_vec=pHoriLinkAngle->End()-ucs.origin;
			else
				len_vec=pHoriLinkAngle->Start()-ucs.origin;
			vector_trans(len_vec,ucs,FALSE);
			double ber_vert_dist=len_vec*rayVertVec;
			horiLinkAngleEdgeVertexArr[0]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+ber_vert_dist*rayVertVec;
			if(max_vert_dist.number-ber_vert_dist>0)//fabs(min_vert_dist.number-ber_vert_dist))
				horiLinkAngleEdgeVertexArr[1]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(max_vert_dist.number+LsSpace.EndSpace)*rayVertVec;
			else
				horiLinkAngleEdgeVertexArr[1]=(max_len_dist.number+LsSpace.EndSpace)*rayVec+(min_vert_dist.number-LsSpace.EndSpace)*rayVertVec;
			if(fabs(horiLinkAngleEdgeVertexArr[1].y)<fabs(horiLinkAngleEdgeVertexArr[0].y))
			{	//保证linkAngleEdgeVertexArr[0]为较靠近底板的顶点
				f3dPoint pt=horiLinkAngleEdgeVertexArr[0];
				horiLinkAngleEdgeVertexArr[0]=horiLinkAngleEdgeVertexArr[1];
				horiLinkAngleEdgeVertexArr[1]=pt;
			}
		}
		else	//无连接螺栓
			pHoriLinkAngle=NULL;
	}
	f3dPoint vertex,offset_vec;
	WORD wKey=1;
	if(lenVec.z<0)
	{	//上侧靴板
		if (pBaseGroupAngle != NULL)
		{	//成都塔厂向刘伟反馈，单角钢时由于加工习惯以般以底面（不是靴板中性层，但组合角钢是中性层）为基准计算焊接边 wjh-2019.8.8
			xueban_btm_vertex[0]=MoveVertexByMidPlane(Thick*0.5,xueban_btm_vertex[0], ucs.axis_x, ucs.axis_z, pBasePlate->ucs.axis_z);
			xueban_btm_vertex[1]=MoveVertexByMidPlane(Thick*0.5,xueban_btm_vertex[1], ucs.axis_x, ucs.axis_z, pBasePlate->ucs.axis_z);
		}
		if(profile_side==0)		//外形类别:0.贯穿底板;1.肢翼单侧;2.楞线单侧
		{
			if(ciOldShemaWorkDirect==0)
			{	//X轴沿向靴板
				if(ucs.axis_x*outwardvec>0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;	//进行焊接
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(work_direct==0&&A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(work_direct==0&&A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;	//进行焊接
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
				}
			}
			else
			{	//Y轴沿向靴板
				if(ucs.axis_x*outwardvec<0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;	//进行焊接
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;	//进行焊接
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
			}
		}
		else if(profile_side==1)
		{	//肢翼侧截断板
			f3dPoint line_pick=baseAngleEdgeVertexArr[2];
			coord_trans(line_pick,ucs,TRUE);
			f3dPoint btm_fore_vertice;	//楞线侧(前侧)截断靴板底部顶点(与另一垂向靴板的贴合顶点)
			Int3dlf(btm_fore_vertice,line_pick,lenVec,datumPlaneOrg,pBasePlate->ucs.axis_z);
			coord_trans(btm_fore_vertice,ucs,FALSE);
			btm_fore_vertice.z=0;
			if (pBaseGroupAngle != NULL)
			{	//成都塔厂向刘伟反馈，单角钢时由于加工习惯以般以底面（不是靴板中性层，但组合角钢是中性层）为基准计算焊接边 wjh-2019.8.8
				btm_fore_vertice = MoveVertexByMidPlane(Thick*0.5, btm_fore_vertice, ucs.axis_x, ucs.axis_z, pBasePlate->ucs.axis_z);
			}
			double buttweld_offset_x=0;
			f3dPoint buttweld_offsetvec;
			if(bButtWeld)
			{
				buttweld_offset_x=pBaseAngle->GetWidth();
				buttweld_offsetvec=-buttweld_offset_x*alongVertVec;
				if(fabs(alongVertVec.x)>EPS)
					buttweld_offset_x/=-alongVertVec.x;
			}
			if(ciOldShemaWorkDirect==0)
			{	//X轴沿向靴板
				if(ucs.axis_x*outwardvec>0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
					}
				}
			}
			else
			{	//Y轴沿向靴板
				if(ucs.axis_x*outwardvec<0)
				{
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
			}
		}
		else
		{	//楞线侧截断板
			f3dPoint btm_fore_vertice;	//楞线侧(前侧)截断靴板底部顶点(与另一垂向靴板的贴合顶点)
			f3dPoint line_pick=baseAngleEdgeVertexArr[1];
			if(bButtWeld)	//对接靴板以2号点为求交拾取点(不能重复考虑靴板厚度偏移）
				line_pick=baseAngleEdgeVertexArr[2];
			coord_trans(line_pick,ucs,TRUE);
			Int3dlf(btm_fore_vertice,line_pick,lenVec,datumPlaneOrg,pBasePlate->ucs.axis_z);
			coord_trans(btm_fore_vertice,ucs,FALSE);
			btm_fore_vertice.z=0;
			if (pBaseGroupAngle != NULL)
			{	//成都塔厂向刘伟反馈，单角钢时由于加工习惯以般以底面（不是靴板中性层，但组合角钢是中性层）为基准计算焊接边 wjh-2019.8.8
				btm_fore_vertice=MoveVertexByMidPlane(Thick*0.5,btm_fore_vertice,ucs.axis_x,ucs.axis_z,pBasePlate->ucs.axis_z);
			}
			if(ciOldShemaWorkDirect==0)
			{	//X轴沿向靴板
				if(ucs.axis_x*outwardvec>0)
				{
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(bButtWeld)
						pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					else
						pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
				}
			}
			else
			{	//Y轴沿向靴板
				if(ucs.axis_x*outwardvec<0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(bButtWeld)
						pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					else
						pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
					}
				}
			}
		}
	}
	else
	{	//下侧靴板
		if (pBaseGroupAngle != NULL)
		{	//成都塔厂向刘伟反馈，单角钢时由于加工习惯以般以底面（不是靴板中性层，但组合角钢是中性层）为基准计算焊接边 wjh-2019.8.8
			xueban_btm_vertex[0]=MoveVertexByMidPlane(Thick*0.5,xueban_btm_vertex[0],ucs.axis_x,ucs.axis_z,-pBasePlate->ucs.axis_z);
			xueban_btm_vertex[1]=MoveVertexByMidPlane(Thick*0.5,xueban_btm_vertex[1],ucs.axis_x,ucs.axis_z,-pBasePlate->ucs.axis_z);
		}
		if(profile_side==0)		//外形类别:0.贯穿底板;1.肢翼单侧;2.楞线单侧
		{
			if(ciOldShemaWorkDirect==0)
			{	//X轴沿向靴板
				if(ucs.axis_x*outwardvec<0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pHoriLinkAngle)
					{
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pHoriLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
			}
			else
			{	//Y轴沿向靴板
				if(ucs.axis_x*outwardvec>0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pHoriLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
				else
				{	//组合角钢异型布置,1号角钢在外侧
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pHoriLinkAngle)
					{
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)	//X轴沿向靴板
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
				}
			}
		}
		else if(profile_side==1)
		{	//肢翼侧截断板
			f3dPoint line_pick=baseAngleEdgeVertexArr[2];
			coord_trans(line_pick,ucs,TRUE);
			f3dPoint btm_fore_vertice;	//楞线侧(前侧)截断靴板底部顶点(与另一垂向靴板的贴合顶点)
			Int3dlf(btm_fore_vertice,line_pick,lenVec,datumPlaneOrg,pBasePlate->ucs.axis_z);
			coord_trans(btm_fore_vertice,ucs,FALSE);
			btm_fore_vertice.z=0;
			if (pBaseGroupAngle != NULL)
			{	//成都塔厂向刘伟反馈，单角钢时由于加工习惯以般以底面（不是靴板中性层，但组合角钢是中性层）为基准计算焊接边 wjh-2019.8.8
				btm_fore_vertice = MoveVertexByMidPlane(Thick*0.5, btm_fore_vertice, ucs.axis_x, ucs.axis_z, -pBasePlate->ucs.axis_z);
			}
			double buttweld_offset_x = 0;
			f3dPoint buttweld_offsetvec;
			if(bButtWeld)
			{
				buttweld_offset_x=pBaseAngle->GetWidth();
				buttweld_offsetvec=-buttweld_offset_x*alongVertVec;
				if(fabs(alongVertVec.x)>EPS)
					buttweld_offset_x/=-alongVertVec.x;
			}
			if(ciOldShemaWorkDirect==0)
			{	//X轴沿向靴板
				if(ucs.axis_x*outwardvec<0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pHoriLinkAngle)
					{
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW)
					{
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
				}
				else
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pHoriLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
			}
			else
			{	//Y轴沿向靴板
				if(ucs.axis_x*outwardvec>0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pHoriLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
					}
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
				}
				else
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(IA>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+IA,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("IA"));
					}
					if(pHoriLinkAngle)
					{
						//该点往往与底板重合,如果非要加入可能会导致外形异常 wjh-2015.1.11
						//vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[0],0));
						pVertex=vertex_list.append(PROFILE_VERTEX(horiLinkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					if(pLinkAngle)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[0],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
						pVertex=vertex_list.append(PROFILE_VERTEX(linkAngleEdgeVertexArr[1],0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[3],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[2]+buttweld_offsetvec,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW)
					{
						offset_vec=baseAngleEdgeVertexArr[2]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=f3dPoint(xueban_btm_vertex[1].x+buttweld_offset_x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x+buttweld_offset_x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice+f3dPoint(buttweld_offset_x)));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
				}
			}
		}
		else
		{	//楞线侧截断板
			f3dPoint btm_fore_vertice;	//楞线侧(前侧)截断靴板底部顶点(与另一垂向靴板的贴合顶点)
			f3dPoint line_pick=baseAngleEdgeVertexArr[1];
			coord_trans(line_pick,ucs,TRUE);
			Int3dlf(btm_fore_vertice,line_pick,lenVec,datumPlaneOrg,pBasePlate->ucs.axis_z);
			coord_trans(btm_fore_vertice,ucs,FALSE);
			btm_fore_vertice.z=0;
			if (pBaseGroupAngle != NULL)
			{	//成都塔厂向刘伟反馈，单角钢时由于加工习惯以般以底面（不是靴板中性层，但组合角钢是中性层）为基准计算焊接边 wjh-2019.8.8
				btm_fore_vertice=MoveVertexByMidPlane(Thick*0.5,btm_fore_vertice,ucs.axis_x,ucs.axis_z,-pBasePlate->ucs.axis_z);
			}
			if(ciOldShemaWorkDirect==0)
			{	//X轴沿向靴板
				if(ucs.axis_x*outwardvec<0)
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
				}
				else
				{
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=f3dPoint(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[1].x,xueban_btm_vertex[1].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
				}
			}
			else
			{	//Y轴沿向靴板
				if(ucs.axis_x*outwardvec>0)
				{
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
				}
				else
				{
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[1],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					pVertex=vertex_list.append(PROFILE_VERTEX(baseAngleEdgeVertexArr[0],0));
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(A>0)
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y+A,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("A"));
					}
					pVertex=vertex_list.append(PROFILE_VERTEX(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0));
					pVertex->m_bWeldEdge=TRUE;
					pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					if(fCutAngleW>0)
					{
						offset_vec=f3dPoint(xueban_btm_vertex[0].x,xueban_btm_vertex[0].y,0)-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C("L"));
						if(iCutMode==1)
						{
							pVertex->type=2;//指定圆弧半径
							pVertex->radius=-fCutAngleW;
						}
						offset_vec=baseAngleEdgeVertexArr[1]-btm_fore_vertice;
						normalize(offset_vec);
						vertex=btm_fore_vertice+offset_vec*fCutAngleW;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertex.x,vertex.y,0));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
					else
					{
						pVertex=vertex_list.append(PROFILE_VERTEX(btm_fore_vertice));
						pVertex->m_bWeldEdge=TRUE;
						pVertex->SetPosDesignType(PROFILE_VERTEX::DT_PARAMETRIC,KEY2C(wKey++));
					}
				}
			}
		}
	}
	//return TRUE;
	//为保证基板X、Y轴沿向靴板外形一致,去除凹点时以基板Y轴沿向靴板为基准，
	//X轴向靴板需翻转(链表逆序顶点坐标取反)去除凹点 wht 16-04-15
	ATOM_LIST<PROFILE_VERTEX> temp_vertex_list;
	//生成去除凹点所需顶点链表
	BOOL bReverse=fabs(ucs.axis_x*pBasePlate->ucs.axis_x)>fabs(ucs.axis_x*pBasePlate->ucs.axis_y);
	if(bReverse)
	{
		for(PROFILE_VERTEX* pVertex=vertex_list.GetTail();pVertex;pVertex=vertex_list.GetPrev())
		{
			pVertex->vertex.x*=-1;
			temp_vertex_list.append(*pVertex);
		}
	}
	else
	{
		for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			temp_vertex_list.append(*pVertex);
	}
	//依照无凹点原则重新修正靴板外形
	pPrevVertex=temp_vertex_list.GetTail();
	PROFILE_VERTEX* pFirstVertex=temp_vertex_list.GetFirst();
	for(PROFILE_VERTEX* pVertex=pFirstVertex;pVertex;pVertex=temp_vertex_list.GetNext())
	{
		BOOL pushed=temp_vertex_list.push_stack();
		PROFILE_VERTEX* pNextVertex=temp_vertex_list.GetNext();
		if(pNextVertex==NULL)
			pNextVertex=pFirstVertex;
		temp_vertex_list.pop_stack(pushed);
		if(DistOf2dPtLine(pNextVertex->vertex,pPrevVertex->vertex,pVertex->vertex)<EPS)
			temp_vertex_list.DeleteCursor();
		else
			pPrevVertex=pVertex;
	}
	temp_vertex_list.Clean();
	//恢复顶点链表
	vertex_list.Empty();
	if(bReverse)
	{
		for(PROFILE_VERTEX* pVertex=temp_vertex_list.GetTail();pVertex;pVertex=temp_vertex_list.GetPrev())
		{
			pVertex->vertex.x*=-1;
			vertex_list.append(*pVertex);
		}
	}
	else
	{
		for(PROFILE_VERTEX* pVertex=temp_vertex_list.GetFirst();pVertex;pVertex=temp_vertex_list.GetNext())
			vertex_list.append(*pVertex);
	}
	return TRUE;
}
BOOL CLDSParamPlate::DesignPedalPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl /*=NULL*/)
{
	long hDatumPart = 0;
	GetDesignItemValue('A', &hDatumPart, pItemHashTbl);  //角钢句柄
	CLDSLineAngle* pDatumJg = (CLDSLineAngle*)BelongModel()->FromPartHandle(hDatumPart, CLS_LINEANGLE);
	if (pDatumJg == NULL)
	{
		if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X pedal plate lacks parameter of datum JG,failed design!", handle);
#else
			log2file->Log("0X%X脚踏板缺少基准角钢参数,设计失败!", handle);
#endif
		return FALSE;
	}
	GEPOINT line_vec = (pDatumJg->End() - pDatumJg->Start()).normalized();
	if (fabs(line_vec*f3dPoint(0, 0, 1)) < 0.5)
	{
		if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X pedal plate lacks parameter of datum JG,failed design!", handle);
#else
			log2file->Log("暂不支持在水平角钢上布置脚踏板,设计失败!", handle);
#endif
		return FALSE;
	}
	if (GetLsCount() <= 0)
	{
		if (log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X pedal plate lacks bolt,failed design!", handle);
#else
			log2file->Log("0X%X脚踏板缺少螺栓,设计失败!", handle);
#endif
		return FALSE;
	}
	//计算坐标系
	CLDSBolt* pOrgBolt = NULL;
	for (CLsRef *pLsRef = GetFirstLsRef(); pLsRef; pLsRef = GetNextLsRef())
	{
		CLDSBolt *pBolt = pLsRef->GetLsPtr();
		if (pOrgBolt == NULL)
			pOrgBolt = pBolt;
		else if (pOrgBolt->ucs.origin.z < pBolt->ucs.origin.z)
			pOrgBolt = pBolt;
	}
	double ddx = pDatumJg->vxWingNorm*pOrgBolt->ucs.axis_z;
	double ddy = pDatumJg->vyWingNorm*pOrgBolt->ucs.axis_z;
	double width = pDatumJg->GetWidth();
	BYTE ciWorkWing = (fabs(ddx) > fabs(ddy)) ? 0 : 1;
	BYTE ciLineS0E1 = (pDatumJg->xPosStart.z > pDatumJg->xPosEnd.z) ? 0 : 1;
	GEPOINT workNorm = (ciWorkWing == 0) ? pDatumJg->vxWingNorm : pDatumJg->vyWingNorm;
	GEPOINT wingVec = (ciWorkWing == 0) ? pDatumJg->GetWingVecX() : pDatumJg->GetWingVecY();
	GEPOINT viceVec = (ciLineS0E1 == 0) ? line_vec : (line_vec*-1);
	Int3dlf(ucs.origin, pOrgBolt->ucs.origin, pOrgBolt->ucs.axis_z, pDatumJg->Start(),workNorm);
	ucs.axis_z = workNorm;
	ucs.axis_y = f3dPoint(0,0,-1);// viceVec;
	ucs.axis_x = ucs.axis_y^ucs.axis_z;
	normalize(ucs.axis_x);
	ucs.axis_y = ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_y);
	if (!is_ucs_valid(ucs))
		return FALSE;
	//计算脚踏板的展开方向
	GEPOINT spreadVec, offVec(0, 0, 1);
	ddx = wingVec * f3dPoint(1, 0, 0);
	ddy = wingVec * f3dPoint(0, 1, 0);
	if (fabs(ddx) > fabs(ddy))
		spreadVec = (ddx > 0) ? f3dPoint(1, 0, 0) : f3dPoint(-1, 0, 0);
	else
		spreadVec = (ddy > 0) ? f3dPoint(0, 1, 0) : f3dPoint(0, -1, 0);
	project_vector(spreadVec, ucs.axis_z);
	project_vector(offVec, ucs.axis_z);
	normalize(spreadVec);
	normalize(offVec);
	//计算螺栓标准边距特征线
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace, pOrgBolt->get_d());
	GEPOINT base_pos = (ciLineS0E1 == 0) ? pDatumJg->xPosStart : pDatumJg->xPosEnd;
	CLDSBolt* pHeadLs = NULL, *pTailLs = NULL;
	double max_ber_dist = 0, min_ber_dist = 0;
	for (CLsRef *pLsRef = GetFirstLsRef(); pLsRef; pLsRef = GetNextLsRef())
	{
		CLDSBolt *pBolt = pLsRef->GetLsPtr();
		if (pDatumJg->FindLsByHandle(pBolt->handle) == NULL)
			continue;
		double dist = 0;
		f3dPoint ls_pos = pBolt->ucs.origin;
		project_point(ls_pos, base_pos, workNorm);
		SnapPerp(&ls_pos, f3dLine(pDatumJg->xPosStart, pDatumJg->xPosEnd), ls_pos, &dist);
		if (max_ber_dist==0)
			max_ber_dist = min_ber_dist = dist;
		else if (dist > max_ber_dist)
			max_ber_dist = dist;
		else if (dist < min_ber_dist)
			min_ber_dist = dist;
		//
		if (pHeadLs == NULL)
			pHeadLs = pTailLs = pBolt;
		else if (pBolt->ucs.origin.z > pHeadLs->ucs.origin.z)
			pHeadLs = pBolt;
		else if (pBolt->ucs.origin.z < pTailLs->ucs.origin.z)
			pTailLs = pBolt;
	}
	if (pHeadLs == NULL || pTailLs == NULL)
		return FALSE;
	f3dLine  head_line, tail_line, edge_line, ls_Space_line;
	GEPOINT pt = pHeadLs->ucs.origin;
	project_point(pt, base_pos, workNorm);
	pt += offVec * LsSpace.EndSpace;
	head_line.startPt = pt - spreadVec * width;
	head_line.endPt = pt + spreadVec * width;
	pt = pTailLs->ucs.origin;
	project_point(pt, base_pos, workNorm);
	pt -= offVec * LsSpace.EndSpace;
	tail_line.startPt = pt - spreadVec * width;
	tail_line.endPt = pt + spreadVec * width;
	//计算轮廓点
	double L = 0, D = 0, S = 0;
	long ciDirection = 0, C = 0;	
	GetDesignItemValue('V', &ciDirection, pItemHashTbl);//0.肢翼侧|1.楞线侧
	GetDesignItemValue('C', &C, pItemHashTbl);	//外形类型
	GetDesignItemValue('L', &L, pItemHashTbl);	//踏板长度
	GetDesignItemValue('D', &D, pItemHashTbl);	//台口高度
	GetDesignItemValue('S', &S, pItemHashTbl);	//连接宽度
	f3dPoint vertex, headPtO, headPtI, tailPtO, tailPtI, spreadPtL, spreadPtD;
	if (ciDirection == 0)
	{	//在肢翼侧进行卷边
		pt = base_pos + wingVec * (min_ber_dist - LsSpace.EndSpace);
		ls_Space_line.startPt = pt;
		ls_Space_line.endPt = pt + viceVec * pDatumJg->GetLength();
		edge_line.startPt = pDatumJg->xPosStart + wingVec * width;
		edge_line.endPt = pDatumJg->xPosEnd + wingVec * width;
	}
	else
	{	//在楞线侧进行卷边
		spreadVec *= -1;
		pt = base_pos + wingVec * (max_ber_dist + LsSpace.EndSpace);
		ls_Space_line.startPt = pt;
		ls_Space_line.endPt = pt + viceVec * pDatumJg->GetLength();
		edge_line.startPt = pDatumJg->xPosStart;
		edge_line.endPt = pDatumJg->xPosEnd;
	}
	Int3dll(ls_Space_line, head_line, headPtI);
	Int3dll(ls_Space_line, tail_line, tailPtI);
	if (S > 0)
	{	//指定连接边距
		headPtO = headPtI + spreadVec * S;
		tailPtO = tailPtI + spreadVec * S;
	}
	else
	{	//延伸到肢边
		Int3dll(f3dLine(tailPtI, tailPtI + spreadVec * width), edge_line, tailPtO);
		Int3dll(f3dLine(headPtI, headPtI + spreadVec * width), edge_line, headPtO);
	}
	spreadPtL = tailPtO + spreadVec * L;
	spreadPtD = spreadPtL + offVec * D;
	coord_trans(headPtI, ucs, FALSE);
	coord_trans(tailPtI, ucs, FALSE);
	coord_trans(headPtO, ucs, FALSE);
	coord_trans(tailPtO, ucs, FALSE);
	coord_trans(spreadPtL, ucs, FALSE);
	coord_trans(spreadPtD, ucs, FALSE);
	headPtI.z = tailPtI.z = headPtO.z = tailPtO.z = spreadPtL.z = spreadPtD.z = 0;
	//添加轮廓点
	//参数化钢板中，PROFILE_VERTEX::m_cPosDesignType不在buffer中存储,改用vertex.feature记录卷边信息
	if (C == 1)	
	{	//一刀切时，需根据螺栓边距重新计算headPtI
		GEPOINT center = pHeadLs->ucs.origin;
		coord_trans(center, ucs, FALSE);
		f2dCircle cir(LsSpace.EndSpace, center.x, center.y);
		f2dPoint basePt(spreadPtD.x, spreadPtD.y), retPt, pickPt(headPtI.x, headPtI.y);
		if (Tang2dpc(basePt, cir, pickPt, retPt) == 1)
			Int3dll(f3dLine(tailPtI, headPtI), f3dLine(f3dPoint(retPt.x, retPt.y), spreadPtD), headPtI);
		else
			Int3dll(f3dLine(tailPtI, headPtI), f3dLine(headPtO, spreadPtD), headPtI);
	}
	PROFILE_VERTEX* pVertex = NULL;
	if (ucs.axis_x*spreadVec < 0)
	{
		vertex_list.append(PROFILE_VERTEX(headPtI));
		vertex_list.append(PROFILE_VERTEX(tailPtI));
		pVertex = vertex_list.append(PROFILE_VERTEX(tailPtO));
		pVertex->vertex.feature = PROFILE_VERTEX::DT_BEND_DATUM_POINT;	//制弯基准点
		pVertex = vertex_list.append(PROFILE_VERTEX(spreadPtL));
		pVertex->vertex.feature = PROFILE_VERTEX::DT_BEND_EXTEND_POINT;	//制弯延伸点
		vertex_list.append(PROFILE_VERTEX(spreadPtD));
		if(C==0)
			vertex_list.append(PROFILE_VERTEX(headPtO));
	}
	else
	{
		vertex_list.append(PROFILE_VERTEX(tailPtI));
		vertex_list.append(PROFILE_VERTEX(headPtI));
		if(C==0)
			vertex_list.append(PROFILE_VERTEX(headPtO));
		vertex_list.append(PROFILE_VERTEX(spreadPtD));
		pVertex = vertex_list.append(PROFILE_VERTEX(spreadPtL));
		pVertex->vertex.feature = PROFILE_VERTEX::DT_BEND_EXTEND_POINT;	//制弯延伸点
		pVertex = vertex_list.append(PROFILE_VERTEX(tailPtO));
		pVertex->vertex.feature = PROFILE_VERTEX::DT_BEND_DATUM_POINT;	//制弯基准点
	}
	return TRUE;
}
BOOL CLDSParamPlate::DesignStationNodePlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	long hBaseNode=0,hBaseTube=0,hLinkJg=0;
	GetDesignItemValue('A',&hBaseNode,pItemHashTbl);
	CLDSNode* pBaseNode=BelongModel()->FromNodeHandle(hBaseNode);
	if(pBaseNode==NULL)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X missing the connecting base node specified for shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X立柱连接板的基准节点丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	GetDesignItemValue('B',&hBaseTube,pItemHashTbl);
	CLDSLineTube* pBaseTube=(CLDSLineTube*)BelongModel()->FromPartHandle(hBaseTube,CLS_LINETUBE);
	if(pBaseTube==NULL)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X missing the connecting base tube specified for shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X立柱连接板的基准钢管丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	GetDesignItemValue('C',&hLinkJg,pItemHashTbl);
	CLDSLineAngle* pLinkJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(hLinkJg,CLS_LINEANGLE);
	if(pLinkJg==NULL)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X missing the connecting base line-angle specified for shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X立柱连接板的连接角钢丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	//计算立板坐标系
	long iAxisZType=0;
	GetDesignItemValue('Z',&iAxisZType,pItemHashTbl);
	ucs.axis_z=CStdVector::GetVector(iAxisZType);
	f3dPoint tube_vec=(pBaseTube->End()-pBaseTube->Start()).normalized();
	BOOL bWorkWingX=FALSE;
	if(fabs(ucs.axis_z*pLinkJg->vxWingNorm)>fabs(ucs.axis_z*pLinkJg->vyWingNorm))
		bWorkWingX=TRUE;
	f3dPoint norm;
	bWorkWingX?(norm=pLinkJg->vxWingNorm):(norm=pLinkJg->vyWingNorm);
	if(ucs.axis_z*norm<0)
		ucs.axis_z*=-1;
	ucs.axis_y=tube_vec;
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	ucs.origin=pBaseNode->ActivePosition();
	//计算立柱连板在角钢上的外形顶点
	f3dLine ber_line(pLinkJg->Start(),pLinkJg->End());
	if(pLinkJg->pEnd->handle==hBaseNode)
	{
		ber_line.startPt=pLinkJg->End();
		ber_line.endPt=pLinkJg->Start();
	}
	f3dPoint ber_vec=(ber_line.endPt-ber_line.startPt).normalized();
	f3dPoint base_pos=ber_line.startPt;
	f3dPoint wing_vec=bWorkWingX?pLinkJg->GetWingVecX():pLinkJg->GetWingVecY();
	LSSPACE_STRU LsSpace;
	LsSpace.EndSpace=30;
	CMaxDouble max_base_len(0);
	for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		if(pLinkJg->FindLsByHandle(pLsRef->GetLsPtr()->handle)==NULL)
			continue;
		GetLsSpace(LsSpace,pLsRef->GetLsPtr()->get_d());
		f3dPoint ls_pos=pLsRef->GetLsPtr()->ucs.origin;
		project_point(ls_pos,base_pos,ucs.axis_z);
		SnapPerp(&ls_pos,ber_line,ls_pos);
		f3dPoint len_vec=ls_pos-base_pos;
		double len_dist=len_vec*ber_vec;
		if(len_dist<DISTANCE(ber_line.startPt,ber_line.endPt)*0.5)
			max_base_len.Update(len_dist);
	}
	max_base_len.number+=LsSpace.EndSpace;
	if(max_base_len.number<60)
		max_base_len.Update(120);
	f3dPoint vertex1=base_pos+ber_vec*max_base_len.number;
	f3dPoint vertex2=vertex1+wing_vec*pLinkJg->GetWidth();
	if((ucs.axis_z^ber_vec)*wing_vec<0)
	{
		f3dPoint vv=vertex1;
		vertex1=vertex2;
		vertex2=vv;
	}
	coord_trans(vertex1,ucs,FALSE);
	coord_trans(vertex2,ucs,FALSE);
	vertex1.z=vertex2.z=0;
	vertex_list.append(PROFILE_VERTEX(vertex1));
	vertex_list.append(PROFILE_VERTEX(vertex2));
	//计算立柱连板在钢管上的焊缝点
	pBaseTube->BuildUCS();
	double radius=pBaseTube->GetDiameter()*0.5;
	f3dPoint rod_vert_vec=pBaseTube->GetPlateVertSpreadVec(this);
	f3dPoint orig=ucs.origin+rod_vert_vec*radius;
	if((ucs.axis_z^ber_vec)*tube_vec<0)
		tube_vec*=-1;
	f3dLine spread_line(orig,orig+tube_vec*1000);
	for(int i=0;i<2;i++)
	{
		f3dPoint pt=(i==0)?vertex2:vertex1;
		coord_trans(pt,ucs,TRUE);
		f3dPoint intersPt1,intersPt2,weld_pt;
		Int3dll(f3dLine(pt,pt+ber_vec*100),spread_line,intersPt1);
		SnapPerp(&intersPt2,spread_line,pt);
		double dd1=DISTANCE(orig,intersPt1),dd2=DISTANCE(orig,intersPt2);
		if(i==0)
			weld_pt=orig+tube_vec*(dd1>dd2?dd1:dd2);
		else
			weld_pt=orig-tube_vec*(dd1>dd2?dd1:dd2);
		coord_trans(weld_pt,ucs,FALSE);
		vertex_list.append(PROFILE_VERTEX(f3dPoint(weld_pt.x,weld_pt.y)));
	}
	return TRUE;
}
BOOL CLDSParamPlate::DesignStationLinkPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	long hBaseNode=0,hBaseTube=0,hAssistJg1=0,hAssistJg2=0;
	GetDesignItemValue('B',&hBaseNode,pItemHashTbl);
	CLDSNode* pBaseNode=BelongModel()->FromNodeHandle(hBaseNode);
	if(pBaseNode==NULL)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X missing the connecting base node specified for shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X端撑连接板的基准节点丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	GetDesignItemValue('A',&hBaseTube,pItemHashTbl);
	CLDSLineTube* pBaseTube=(CLDSLineTube*)BelongModel()->FromPartHandle(hBaseTube,CLS_LINETUBE);
	if(pBaseTube==NULL)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X missing the connecting base tube specified for shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X端撑连接板的基准钢管丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	GetDesignItemValue('C',&hAssistJg1,pItemHashTbl);
	GetDesignItemValue('D',&hAssistJg2,pItemHashTbl);
	CLDSLineAngle* pAssistJg1=(CLDSLineAngle*)BelongModel()->FromPartHandle(hAssistJg1,CLS_LINEANGLE);
	CLDSLineAngle* pAssistJg2=(CLDSLineAngle*)BelongModel()->FromPartHandle(hAssistJg2,CLS_LINEANGLE);
	if(pAssistJg1==NULL||pAssistJg2==NULL)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X missing the connecting base line-angle specified for shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X端撑连接板的指定连接角钢丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	pBaseTube->BuildUCS();
	double radius=pBaseTube->GetDiameter()*0.5;
	f3dPoint ber_vec1=(pAssistJg1->End()-pAssistJg1->Start()).normalized();
	f3dPoint ber_vec2=(pAssistJg2->End()-pAssistJg2->Start()).normalized();
	(pAssistJg1->pStart->handle==hBaseNode)?ber_vec1*=1:ber_vec1*=-1;
	(pAssistJg2->pStart->handle==hBaseNode)?ber_vec2*=1:ber_vec2*=-1;
	f3dPoint extern_vec=ber_vec1+ber_vec2;
	normalize(extern_vec);
	//计算端撑连接板的坐标系
	long iAxisZType=0;
	GetDesignItemValue('Z',&iAxisZType,pItemHashTbl);
	ucs.axis_z=CStdVector::GetVector(iAxisZType);
	BOOL bWorkWingX=FALSE;
	if(fabs(ucs.axis_z*pAssistJg1->vxWingNorm)>fabs(ucs.axis_z*pAssistJg1->vyWingNorm))
		bWorkWingX=TRUE;
	f3dPoint norm;
	bWorkWingX?(norm=pAssistJg1->vxWingNorm):(norm=pAssistJg1->vyWingNorm);
	if(ucs.axis_z*norm<0)
		ucs.axis_z*=-1;
	ucs.axis_y=extern_vec;
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	ucs.origin=pBaseNode->ActivePosition();
	//计算端撑连接板在角钢上的外形顶点
	double wing_dist=0,ber_dist=0;
	GetDesignItemValue(KEY2C("OW"),&wing_dist,pItemHashTbl);
	GetDesignItemValue(KEY2C("OB"),&ber_dist,pItemHashTbl);
	vector_trans(ber_vec1,ucs,FALSE);
	vector_trans(ber_vec2,ucs,FALSE);
	ber_vec1.z=ber_vec2.z=0;
	double fAngle1=CalRotateAngle(f3dPoint(0,0,1),f3dPoint(1,0,0),ber_vec1);
	double fAngle2=CalRotateAngle(f3dPoint(0,0,1),f3dPoint(1,0,0),ber_vec2);
	if(fAngle1>fAngle2)
	{
		CLDSLineAngle* pTemJg=pAssistJg1;
		pAssistJg1=pAssistJg2;
		pAssistJg2=pTemJg;
	}
	f3dPoint pick_pt1,pick_pt2;
	for(int i=0;i<2;i++)
	{
		CLDSLineAngle* pAngle=(i==0)?pAssistJg1:pAssistJg2;
		if(pAngle==NULL)
			continue;
		f3dLine base_line(pAngle->Start(),pAngle->End());
		if(pAngle->pEnd->handle==hBaseNode)
		{
			base_line.startPt=pAngle->End();
			base_line.endPt=pAngle->Start();
		}
		f3dPoint base_pos=base_line.startPt;
		f3dPoint ber_vec=base_line.endPt-base_line.startPt;
		normalize(ber_vec);
		i==0?(ber_vec1=ber_vec):(ber_vec2=ber_vec);
		BYTE biInWingX0_Y1=0;
		if(fabs(ucs.axis_z*pAngle->vxWingNorm)<fabs(ucs.axis_z*pAngle->vyWingNorm))
			biInWingX0_Y1=1;
		f3dPoint wing_vec=biInWingX0_Y1==0?pAngle->GetWingVecX():pAngle->GetWingVecY();
		f3dPoint wing_norm=biInWingX0_Y1==0?pAngle->vxWingNorm:pAngle->vyWingNorm;
		LSSPACE_STRU LsSpace;
		LsSpace.EndSpace=30;
		CMaxDouble max_base_len(0);
		for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(pAngle->FindLsByHandle(pLsRef->GetLsPtr()->handle)==NULL)
				continue;
			GetLsSpace(LsSpace,pLsRef->GetLsPtr()->get_d());
			f3dPoint ls_pos=pLsRef->GetLsPtr()->ucs.origin;
			project_point(ls_pos,base_pos,wing_norm);
			SnapPerp(&ls_pos,base_line,ls_pos);
			f3dPoint len_vec=ls_pos-base_pos;
			double len_dist=len_vec*ber_vec;
			if(len_dist<DISTANCE(base_line.startPt,base_line.endPt)*0.5)
				max_base_len.Update(len_dist);
		}
		max_base_len.number+=LsSpace.EndSpace;
		if(max_base_len.number<60)
			max_base_len.Update(radius+120);
		f3dPoint vv=base_pos+ber_vec*max_base_len.number;
		f3dPoint vertice_up=vv+wing_vec*(pAngle->GetWidth()+wing_dist);
		f3dPoint vertice_dw=vv-wing_vec*ber_dist;
		if((ucs.axis_z^ber_vec)*wing_vec>0)
		{
			vv=vertice_up;
			vertice_up=vertice_dw;
			vertice_dw=vv;
		}
		coord_trans(vertice_up,ucs,FALSE);
		coord_trans(vertice_dw,ucs,FALSE);
		vertice_up.z=vertice_dw.z=0;
		vertex_list.append(PROFILE_VERTEX(vertice_up));
		vertex_list.append(PROFILE_VERTEX(vertice_dw));
		if(i==0)
			pick_pt1=vertice_up;
		else
			pick_pt2=vertice_dw;
	}
	//计算端撑连接板在钢管上的外形顶点(一段椭圆弧)
	vector_trans(ber_vec1,ucs,FALSE);
	vector_trans(ber_vec2,ucs,FALSE);
	ber_vec1.z=ber_vec2.z=0;
	f3dPoint vertex,inters1,inters2;
	coord_trans(pick_pt2,ucs,TRUE);
	vector_trans(ber_vec2,ucs,TRUE);
	Int3dlc(&inters1,&inters2,pick_pt2,ber_vec2,pBaseTube->Start(),pBaseTube->End(),radius,radius);
	if(DISTANCE(pick_pt2,inters1)<DISTANCE(pick_pt2,inters2))
		vertex=inters1;
	else
		vertex=inters2;
	coord_trans(vertex,ucs,FALSE);
	vertex.z=0;
	PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertex));
	pVertex->type=3;
	pVertex->center=ucs.origin;
	pVertex->radius=-radius;
	if(ucs.axis_z*pBaseTube->ucs.axis_z>0)
		pVertex->column_norm=pBaseTube->ucs.axis_z;
	else
		pVertex->column_norm=-pBaseTube->ucs.axis_z;
	//
	coord_trans(pick_pt1,ucs,TRUE);
	vector_trans(ber_vec1,ucs,TRUE);
	Int3dlc(&inters1,&inters2,pick_pt1,ber_vec1,pBaseTube->Start(),pBaseTube->End(),radius,radius);
	if(DISTANCE(pick_pt1,inters1)<DISTANCE(pick_pt1,inters2))
		vertex=inters1;
	else
		vertex=inters2;
	coord_trans(vertex,ucs,FALSE);
	vertex.z=0;
	vertex_list.append(PROFILE_VERTEX(vertex));
	return TRUE;
}
BOOL CLDSParamPlate::DesignStationConnPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	long hConnectBasePlate=0;
	long I=0;
	double H=0,C=0,r=0,R=0,offset_dist=0;
	GetDesignItemValue('H',&H,pItemHashTbl);
	GetDesignItemValue('C',&C,pItemHashTbl);
	GetDesignItemValue('r',&r,pItemHashTbl);
	GetDesignItemValue('R',&R,pItemHashTbl);
	GetDesignItemValue('I',&I,pItemHashTbl);
	GetDesignItemValue('D',&offset_dist,pItemHashTbl);
	if(!GetDesignItemValue(KEY2C("LP"),&hConnectBasePlate,pItemHashTbl))	//连接顶板
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X end plate lacks parameter of connecting base plate,failed design!",handle);
#else
			log2file->Log("0X%X端板缺少连接顶板参数,设计失败!",handle);
#endif
		return FALSE;
	}
	CLDSGeneralPlate* pBasePlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(hConnectBasePlate,CLS_PLATE,CLS_PARAMPLATE);
	if(pBasePlate==NULL)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X missing the connecting base plate specified for shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X端板的指定连接顶丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	//1.计算端板装配坐标系
	ucs.axis_y=pBasePlate->ucs.axis_z;
	ucs.origin=pBasePlate->ucs.origin+ucs.axis_y*pBasePlate->Thick;
	if(I==0)		//X+
		ucs.axis_z=pBasePlate->ucs.axis_x;
	else if(I==1)	//X-
		ucs.axis_z=-1*pBasePlate->ucs.axis_x;
	else if(I==2)	//Y+
		ucs.axis_z=pBasePlate->ucs.axis_y;
	else //if(I==3)	//Y-
		ucs.axis_z=-1*pBasePlate->ucs.axis_y;
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	ucs.origin+=ucs.axis_z*offset_dist;
	//2.计算端板轮廓点
	f3dPoint inters=ucs.origin+ucs.axis_z*Thick;
	f3dPoint axisx_line_start=inters-ucs.axis_x*10000;	//10000是随意取的一个肯定超出底板宽的大数
	f3dPoint axisx_line_end=inters+ucs.axis_x*10000;
	coord_trans(axisx_line_start,pBasePlate->ucs,FALSE);
	coord_trans(axisx_line_end,pBasePlate->ucs,FALSE);
	axisx_line_start.z=axisx_line_end.z=0;
	double Radius=0;
	((CLDSParamPlate*)pBasePlate)->GetDesignItemValue(KEY2C("R"),&Radius);
	double intersX1,intersY1,intersX2,intersY2;
	if(Int2dlc(f2dLine(axisx_line_start,axisx_line_end),f2dCircle(Radius,0,0),intersX1,intersY1,intersX2,intersY2)!=2)
		return FALSE;
	double W=DISTANCE(f3dPoint(intersX1,intersY1),f3dPoint(intersX2,intersY2));
	double half_w=W*0.5;
	f3dPoint btm_vertex[2],base_vertex[2],tang_vertex[2];
	btm_vertex[0].Set(half_w,0);
	btm_vertex[1].Set(-half_w,0);
	base_vertex[0]=btm_vertex[0];
	base_vertex[1]=btm_vertex[1];
	if(C>0)
	{
		base_vertex[0].Set(half_w,C);
		base_vertex[1].Set(-half_w,C);
	}
	f3dPoint pt,center;
	center.Set(0,H,0);
	for(int i=0;i<2;i++)
	{
		f3dPoint vec=center-base_vertex[i];
		normalize(vec);
		f3dPoint offset_vec(-vec.y,vec.x,0);
		if(i==0)
			pt=center-offset_vec*20;
		else
			pt=center+offset_vec*20;
		f2dPoint pick_pt,tang_pt,base_pt;
		pick_pt.Set(pt.x,pt.y);
		base_pt.Set(base_vertex[i].x,base_vertex[i].y);
		Tang2dpc(base_pt,f2dCircle(R,0,H),pick_pt,tang_pt);
		tang_vertex[i].Set(tang_pt.x,tang_pt.y,0);
	}
	if(tang_vertex[0].IsZero()||tang_vertex[1].IsZero())
		return FALSE;
	//3.生成轮廓顶点
	vertex_list.append(PROFILE_VERTEX(btm_vertex[0],0));
	if(C>0)
		vertex_list.append(PROFILE_VERTEX(base_vertex[0],0));
	PROFILE_VERTEX* pVertex=vertex_list.append(PROFILE_VERTEX(tang_vertex[0],2));
	pVertex->radius=R;
	vertex_list.append(PROFILE_VERTEX(tang_vertex[1],0));
	if(C>0)
		vertex_list.append(PROFILE_VERTEX(base_vertex[1],0));
	vertex_list.append(PROFILE_VERTEX(btm_vertex[1],0));
	return TRUE;
}
BOOL CLDSParamPlate::DesignStationCrossPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	long I=0,hConnectBasePlate=0,hAssitPlate=0;
	double H=0,offset_dist=0;
	GetDesignItemValue('H',&H,pItemHashTbl);	//插板高度
	GetDesignItemValue('I',&I,pItemHashTbl);	//插板方位类型
	GetDesignItemValue(KEY2C("LA"),&hAssitPlate,pItemHashTbl);
	if(!GetDesignItemValue(KEY2C("LP"),&hConnectBasePlate,pItemHashTbl))	//连接顶板
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X spile plate lacks parameter of connecting base plate,failed design!",handle);
#else
			log2file->Log("0X%X插板缺少连接顶板参数,设计失败!",handle);
#endif
		return FALSE;
	}
	CLDSGeneralPlate* pBasePlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(hConnectBasePlate,CLS_PLATE,CLS_PARAMPLATE);
	if(pBasePlate==NULL)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X missing the connecting base plate specified for spile plate,failed design!",handle);
#else
			log2file->Log("0X%X插板的指定连接顶丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	CLDSGeneralPlate* pAssistPlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(hAssitPlate,CLS_PLATE,CLS_PARAMPLATE);
	if(pAssistPlate)
		offset_dist=pAssistPlate->Thick*0.5;
	//1.计算插板装配坐标系
	ucs.axis_y=pBasePlate->ucs.axis_z*(-1);
	if(I<3)		//顶板X方向的插板
	{
		ucs.axis_x=pBasePlate->ucs.axis_x;
		if(I==2)	//X-方向
			ucs.axis_x*=-1;
	}
	else		//顶板Y方向的插板
	{
		ucs.axis_x=pBasePlate->ucs.axis_y;
		if(I==5)	//Y-方向
			ucs.axis_x*=-1;
	}
	ucs.axis_z=ucs.axis_x^ucs.axis_y;
	if(!is_ucs_valid(ucs))
		return FALSE;
	ucs.origin=pBasePlate->ucs.origin+ucs.axis_x*offset_dist;
	//2.计算插板轮廓点
	f3dPoint inters=pBasePlate->ucs.origin+ucs.axis_z*Thick*0.5;
	f3dPoint axisx_line_start=inters-ucs.axis_x*10000;	//10000是随意取的一个肯定超出底板宽的大数
	f3dPoint axisx_line_end=inters+ucs.axis_x*10000;
	coord_trans(axisx_line_start,pBasePlate->ucs,FALSE);
	coord_trans(axisx_line_end,pBasePlate->ucs,FALSE);
	axisx_line_start.z=axisx_line_end.z=0;
	double Radius=0;
	((CLDSParamPlate*)pBasePlate)->GetDesignItemValue(KEY2C("R"),&Radius);
	double intersX1,intersY1,intersX2,intersY2;
	if(Int2dlc(f2dLine(axisx_line_start,axisx_line_end),f2dCircle(Radius,0,0),intersX1,intersY1,intersX2,intersY2)!=2)
		return FALSE;
	double W=DISTANCE(f3dPoint(intersX1,intersY1),f3dPoint(intersX2,intersY2));
	double half_w=W*0.5;
	f3dPoint btm_vertex[2],base_vertex[2];
	if(I==0 || I==3)	//通板
	{
		btm_vertex[0].Set(-half_w,0,0);
		btm_vertex[1].Set(half_w,0,0);
	}
	else
	{	
		btm_vertex[0].Set(0,0,0);
		btm_vertex[1].Set(half_w-offset_dist,0,0);
	}
	
	base_vertex[0].Set(btm_vertex[0].x,H,0);
	base_vertex[1].Set(btm_vertex[1].x,H,0);
	//3.生成插板轮廓点
	vertex_list.append(PROFILE_VERTEX(btm_vertex[0],0));
	vertex_list.append(PROFILE_VERTEX(btm_vertex[1],0));
	vertex_list.append(PROFILE_VERTEX(base_vertex[1],0));
	vertex_list.append(PROFILE_VERTEX(base_vertex[0],0));
	return TRUE;
}
BOOL CLDSParamPlate::DesignStationShoePlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	long hBasePlate,hEndPlate;
	double R=0,fCutR=0,A=0,B=0;
	GetDesignItemValue('A',&A,pItemHashTbl);
	GetDesignItemValue('B',&B,pItemHashTbl);
	GetDesignItemValue(KEY2C("CR"),&fCutR,pItemHashTbl);
	if(!GetDesignItemValue(KEY2C("LB"),&hBasePlate,pItemHashTbl))	//连接顶板
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X plywood plate lacks parameter of connecting base plate,failed design!",handle);
#else
			log2file->Log("0X%X夹板缺少连接基板参数,设计失败!",handle);
#endif
		return FALSE;
	}
	CLDSGeneralPlate* pBasePlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(hBasePlate,CLS_PLATE,CLS_PARAMPLATE);
	if(pBasePlate==NULL)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("0X%X missing the connecting base plate specified for shoe plate,failed design!",handle);
#else
			log2file->Log("0X%X夹板的指定连基板丢失,设计失败!",handle);
#endif
		return FALSE;
	}
	//计算坐标系
	ucs.origin=pBasePlate->ucs.origin;
	ucs.axis_y=pBasePlate->ucs.axis_z;
	if(ucs.axis_y.z<0)
		ucs.axis_y*=-1;
	if(pBasePlate->ucs.axis_z.z>0)
		ucs.origin+=pBasePlate->ucs.axis_z*pBasePlate->GetThick();
	GetDesignItemValue(KEY2C("LE"),&hEndPlate,pItemHashTbl);
	CLDSParamPlate* pEndPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(hEndPlate,CLS_PARAMPLATE);
	if(pEndPlate&&pEndPlate->m_iParamType==TYPE_STATIONCONNPLATE)	
	{	//有辅助端板
		ucs.axis_z=pEndPlate->ucs.axis_z;
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
	}
	else
	{
		ucs.axis_x.Set(1,0,0);
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
	}
	if(!is_ucs_valid(ucs))
		return FALSE;
	//计算外形轮廓点
	f3dPoint intersPt[2],btm_vertex[2],base_vertex[2],tang_vertex1[2],tang_vertex2[2];
	f3dPoint axisx_line_start=pBasePlate->ucs.origin-ucs.axis_x*10000;	//10000是随意取的一个肯定超出底板宽的大数
	f3dPoint axisx_line_end=pBasePlate->ucs.origin+ucs.axis_x*10000;
	coord_trans(axisx_line_start,pBasePlate->ucs,FALSE);
	coord_trans(axisx_line_end,pBasePlate->ucs,FALSE);
	PROFILE_VERTEX* pPrevVertex=NULL,*pVertex=NULL;
	pPrevVertex=pBasePlate->vertex_list.GetTail();
	for(pVertex=pBasePlate->vertex_list.GetFirst();pVertex;pVertex=pBasePlate->vertex_list.GetNext())
	{
		double intersX,intersY;
		if(Int2dll(f2dLine(pPrevVertex->vertex,pVertex->vertex),f2dLine(axisx_line_start,axisx_line_end),intersX,intersY)>0)
		{
			if(intersPt[0].IsZero())
				intersPt[0].Set(intersX,intersY);
			else
				intersPt[1].Set(intersX,intersY);
			if(!intersPt[0].IsZero()&&!intersPt[1].IsZero())
				break;
		}
		pPrevVertex=pVertex;
	}
	if(intersPt[0].IsZero()&&intersPt[1].IsZero())
		return FALSE;
	coord_trans(intersPt[0],pBasePlate->ucs,TRUE);
	coord_trans(intersPt[1],pBasePlate->ucs,TRUE);
	coord_trans(intersPt[0],ucs,FALSE);
	coord_trans(intersPt[1],ucs,FALSE);
	if(intersPt[1].x>intersPt[0].x)
	{
		btm_vertex[0].Set(intersPt[1].x,0,0);
		btm_vertex[1].Set(intersPt[0].x,0,0);
	}
	else
	{
		btm_vertex[0].Set(intersPt[0].x,0,0);
		btm_vertex[1].Set(intersPt[1].x,0,0);
	}
	base_vertex[0]=btm_vertex[0];
	base_vertex[1]=btm_vertex[1];
	double fLeftR=0,fRightR=0,fCos=0,fSin=0;
	f3dPoint center1,center2,top_pt;
	if(pEndPlate&&pEndPlate->m_iParamType==TYPE_STATIONCONNPLATE)
	{
		double H=0,R=0;
		pEndPlate->GetDesignItemValue('H',&H);
		GetDesignItemValue('R',&R,pItemHashTbl);
		f3dPoint center(0,H,0),pt,vec,vec1,vec2;
		coord_trans(center,pEndPlate->ucs,TRUE);
		coord_trans(center,ucs,FALSE);
		center.z=0;
		top_pt.Set(0,center.y+R,0);
		if(center.x>0)
		{
			if(A>0)
				base_vertex[1].y=A;
			//X+圆心
			center1=center;
			fLeftR=R;
			//X-圆心
			pt.Set(-B,top_pt.y,0);
			vec1=top_pt-pt;
			normalize(vec1);
			vec2=base_vertex[1]-pt;
			normalize(vec2);
			vec=vec1+vec2;
			normalize(vec);
			fCos=vec*vec1;
			fSin=sqrt(1-SQR(fCos));
			center2=pt+vec*(fCutR/fSin);
			fRightR=fCutR;
		}
		else
		{
			if(A>0)
				base_vertex[0].y=A;
			//X+圆心
			pt.Set(B,top_pt.y,0);
			vec1=top_pt-pt;
			normalize(vec1);
			vec2=base_vertex[0]-pt;
			normalize(vec2);
			vec=vec1+vec2;
			normalize(vec);
			fCos=vec*vec1;
			fSin=sqrt(1-SQR(fCos));
			center1=pt+vec*(fCutR/fSin);
			fLeftR=fCutR;
			//X-圆心
			center2=center;
			fRightR=R;
		}
	}
	else
	{
		if(A>0)
		{
			base_vertex[0].y=A;
			base_vertex[1].y=A;
		}
		double H=0;
		GetDesignItemValue('H',&H);
		if(H==0)
			return FALSE;
		top_pt.Set(0,H,0);
		fLeftR=fRightR=fCutR;
		f3dPoint pt,vec,vec1,vec2;
		//X+圆心
		pt.Set(B,top_pt.y,0);
		vec1=top_pt-pt;
		normalize(vec1);
		vec2=base_vertex[0]-pt;
		normalize(vec2);
		vec=vec1+vec2;
		normalize(vec);
		fCos=vec*vec1;
		fSin=sqrt(1-SQR(fCos));
		center1=pt+vec*(fCutR/fSin);
		//X-圆心
		pt.Set(-B,top_pt.y,0);
		vec1=top_pt-pt;
		normalize(vec1);
		vec2=base_vertex[1]-pt;
		normalize(vec2);
		vec=vec1+vec2;
		normalize(vec);
		fCos=vec*vec1;
		fSin=sqrt(1-SQR(fCos));
		center2=pt+vec*(fCutR/fSin);
	}
	double radius=0;
	f3dPoint center,base_pt[2];
	for(int i=0;i<2;i++)
	{
		if(i==0)
		{
			radius=fLeftR;
			center=center1;
			base_pt[0]=base_vertex[0];
			base_pt[1]=top_pt;
		}
		else
		{	
			radius=fRightR;
			center=center2;
			base_pt[0]=top_pt;
			base_pt[1]=base_vertex[1];
		}
		for(int j=0;j<2;j++)
		{
			f3dPoint pt,vec=center-base_pt[j];
			normalize(vec);
			f3dPoint offset_vec(-vec.y,vec.x,0);
			if(j==0)
				pt=center-offset_vec*20;
			else
				pt=center+offset_vec*20;
			f2dPoint pick_pt,tang_pt,start_pt,orig_pt;
			pick_pt.Set(pt.x,pt.y);
			start_pt.Set(base_pt[j].x,base_pt[j].y);
			orig_pt.Set(center.x,center.y);
			Tang2dpc(start_pt,f2dCircle(radius,orig_pt),pick_pt,tang_pt);
			if(i==0)
				tang_vertex1[j].Set(tang_pt.x,tang_pt.y,0);
			else
				tang_vertex2[j].Set(tang_pt.x,tang_pt.y,0);
		}
	}
	//生成外形轮廓点
	vertex_list.append(PROFILE_VERTEX(btm_vertex[0],0));
	if(base_vertex[0].IsEqual(btm_vertex[0])==false)
		vertex_list.append(PROFILE_VERTEX(base_vertex[0],0));
	pVertex=vertex_list.append(PROFILE_VERTEX(tang_vertex1[0],2));
	pVertex->radius=fLeftR;
	vertex_list.append(PROFILE_VERTEX(tang_vertex1[1],0));
	pVertex=vertex_list.append(PROFILE_VERTEX(tang_vertex2[0],2));
	pVertex->radius=fRightR;
	vertex_list.append(PROFILE_VERTEX(tang_vertex2[1],0));
	if(base_vertex[1].IsEqual(btm_vertex[1])==false)
		vertex_list.append(PROFILE_VERTEX(base_vertex[1],0));
	vertex_list.append(PROFILE_VERTEX(btm_vertex[1],0));
	return TRUE;
}
//走道的加劲环板
BOOL CLDSParamPlate::DesignWalkWayCirPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	long hBaseTube=0,hBaseNode=0,hConnPlate=0;
	double angle=0;
	GetDesignItemValue('A',&hBaseTube,pItemHashTbl);
	GetDesignItemValue('B',&hBaseNode,pItemHashTbl);
	GetDesignItemValue('C',&hConnPlate,pItemHashTbl);
	GetDesignItemValue('D',&angle,pItemHashTbl);
	CLDSLineTube* pBaseTube=(CLDSLineTube*)BelongModel()->FromPartHandle(hBaseTube);
	if(pBaseTube==NULL)
	{
		if(log2file)
			log2file->Log("0X%X环板缺少基准钢管参数,设计失败!",handle);
		return FALSE;
	}
	CLDSNode* pBaseNode=BelongModel()->FromNodeHandle(hBaseNode);
	if(pBaseNode==NULL)
	{
		if(log2file)
			log2file->Log("0X%X环板缺少依赖节点参数,设计失败!",handle);
		return FALSE;
	}
	CLDSGeneralPlate* pConnPlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(hConnPlate);
	if(pConnPlate==NULL)
	{
		if(log2file)
			log2file->Log("0X%X环板缺少焊接钢板参数,设计失败!",handle);
		return FALSE;
	}
	//计算坐标系
	f3dPoint tube_vec=(pBaseTube->End()-pBaseTube->Start()).normalized();
	f3dPoint exten_vec=pConnPlate->ucs.axis_z;
	ucs.axis_z=tube_vec;
	ucs.axis_x=exten_vec^ucs.axis_z;
	normalize(ucs.axis_x);
	if(ucs.axis_x*f3dPoint(0,0,-1)<0)
		ucs.axis_x*=-1;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_y);
	ucs.origin=pBaseNode->Position();
	if(!is_ucs_valid(ucs))
		return FALSE;
	//计算外形轮廓点
	double tube_r=pBaseTube->GetDiameter()*0.5;
	f3dPoint arc_vertex[2],btm_vertex[2];
	//计算连接板的焊接点
	f3dPoint intersPt[2],pt;
	Int3dlf(pt,ucs.origin,exten_vec,pConnPlate->ucs.origin,exten_vec);
	f3dPoint axisx_ptS=pt+ucs.axis_x*10000;
	f3dPoint axisx_ptE=pt-ucs.axis_x*10000;
	coord_trans(axisx_ptS,pConnPlate->ucs,FALSE);
	coord_trans(axisx_ptE,pConnPlate->ucs,FALSE);
	PROFILE_VERTEX* pPrevVertex=NULL,*pVertex=NULL;
	pPrevVertex=pConnPlate->vertex_list.GetTail();
	for(pVertex=pConnPlate->vertex_list.GetFirst();pVertex;pVertex=pConnPlate->vertex_list.GetNext())
	{
		double intersX=0,intersY=0;
		if(Int2dll(f2dLine(pPrevVertex->vertex,pVertex->vertex),f2dLine(axisx_ptS,axisx_ptE),intersX,intersY)>0)
		{
			if(intersPt[0].IsZero())
				intersPt[0].Set(intersX,intersY);
			else
				intersPt[1].Set(intersX,intersY);
			if(!intersPt[0].IsZero()&&!intersPt[1].IsZero())
				break;
		}
		pPrevVertex=pVertex;
	}
	if(intersPt[0].IsZero()&&intersPt[1].IsZero())
		return FALSE;
	coord_trans(intersPt[0],pConnPlate->ucs,TRUE);
	coord_trans(intersPt[1],pConnPlate->ucs,TRUE);
	intersPt[0]+=pConnPlate->ucs.axis_z*pConnPlate->Thick;
	intersPt[1]+=pConnPlate->ucs.axis_z*pConnPlate->Thick;
	coord_trans(intersPt[0],ucs,FALSE);
	coord_trans(intersPt[1],ucs,FALSE);
	if(intersPt[1].x>intersPt[0].x)
	{
		btm_vertex[0].Set(intersPt[1].x,intersPt[1].y,0);
		btm_vertex[1].Set(intersPt[0].x,intersPt[0].y,0);
	}
	else
	{
		btm_vertex[0].Set(intersPt[0].x,intersPt[0].y,0);
		btm_vertex[1].Set(intersPt[1].x,intersPt[1].y,0);
	}
	//计算钢管上的焊接点
	if(angle==0)
		angle=45;
	f3dPoint off_vec(1,0,0);
	if(btm_vertex[0].y>0)
		RotateVectorAroundVector(off_vec,angle*RADTODEG_COEF,f3dPoint(0,0,1));
	else
		RotateVectorAroundVector(off_vec,-angle*RADTODEG_COEF,f3dPoint(0,0,1));
	arc_vertex[0].Set(tube_r,0,0);
	arc_vertex[1].Set(off_vec.x*tube_r,off_vec.y*tube_r,0);
	//生成钢板轮廓点
	if(btm_vertex[0].y>0)
	{
		vertex_list.append(PROFILE_VERTEX(arc_vertex[0]));
		vertex_list.append(PROFILE_VERTEX(btm_vertex[0].x,0,0));
		pVertex=vertex_list.append(PROFILE_VERTEX(btm_vertex[0]));
		pVertex->m_bWeldEdge=true;
		vertex_list.append(PROFILE_VERTEX(btm_vertex[1]));
		pVertex=vertex_list.append(PROFILE_VERTEX(arc_vertex[1]));
		pVertex->type=2;
		pVertex->radius=-tube_r;
		pVertex->m_bWeldEdge=true;
	}
	else
	{
		pVertex=vertex_list.append(PROFILE_VERTEX(arc_vertex[0]));
		pVertex->type=2;
		pVertex->radius=-tube_r;
		pVertex->m_bWeldEdge=true;
		vertex_list.append(PROFILE_VERTEX(arc_vertex[1]));
		pVertex=vertex_list.append(btm_vertex[1]);
		pVertex->m_bWeldEdge=true;
		vertex_list.append(btm_vertex[0]);
		vertex_list.append(PROFILE_VERTEX(btm_vertex[0].x,0,0));
	}
	return TRUE;
}
BOOL CLDSParamPlate::DesignTubeRibPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	long S=0,P=0,h=0;
	double W,H,R,D,N=0,Y=0,F=-1,A=0,manu_space=0;
	GetDesignItemValue('W',&W,pItemHashTbl);	//基准板1上的焊缝宽
	if(!GetDesignItemValue('N',&N,pItemHashTbl))//基准板2上的焊缝宽
		N=W;
	GetDesignItemValue('H',&H,pItemHashTbl);	//倒角高
	GetDesignItemValue('R',&R,pItemHashTbl);	//肋板的外圆半径(现应用D替代，D=Ro-Ri)
	GetDesignItemValue('D',&D,pItemHashTbl);	//肋板自节点的法向偏移距离(现改名为S)
	GetDesignItemValue('Y',&Y,pItemHashTbl);	//>=0肋板在基准板外侧;<0肋板在基准板内侧
	GetDesignItemValue('F',&F,pItemHashTbl);	//<0半圆型环向肋板;>=0整圆型环向肋板
	GetDesignItemValue('S',&S,pItemHashTbl);	//Ｘ轴定向基准钢管
	GetDesignItemValue('P',&P,pItemHashTbl);	//基准节点
	GetDesignItemValue('B',&h,pItemHashTbl);	//第一块定位基准钢板
	GetDesignItemValue('A',&A,pItemHashTbl);	//板自由边连接是否为圆弧连接
	GetDesignItemValue('V',&manu_space,pItemHashTbl);	//加工间隙 wht 10-08-28
	CLDSPlate *pDatumPlate1=(CLDSPlate*)BelongModel()->FromPartHandle(h,CLS_PLATE);
	h=0;
	GetDesignItemValue('C',&h,pItemHashTbl);		//第二块定位基准钢板
	CLDSPlate *pDatumPlate2=NULL;
	CLDSLineTube *pDatumLineTube2=NULL;
	CLDSPart *pPart=BelongModel()->FromPartHandle(h,CLS_PLATE,CLS_LINETUBE);
	if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
		pDatumLineTube2=(CLDSLineTube*)pPart;	//第二个基准构件为钢管
	else if(pPart&&pPart->GetClassTypeId()==CLS_PLATE)
		pDatumPlate2=(CLDSPlate*)pPart;			//第二个基准构件为钢板
	if(pDatumLineTube2)	//第二个基准构件为钢管时偏移量D不起作用在此处将D清零 wht 10-10-26
		D=0;
	if(!pDatumPlate1)//||!pDatumPlate2)
		return FALSE;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
	if(!pDatumTube)
	{
		//AfxMessageBox("钢管肋板无焊接父构件重新设计失败!");
		return FALSE;
	}
	//因实际生产过程中很少出现整圆型肋板，即使有也应合并到普通环向板中处理，因此此处暂注释掉整圆型环向肋板 wjh-2011.03.26
	//if(F<0)
	{	//半圆形钢管肋板
		pDatumTube->BuildUCS();
		//计算钢板Ｙ轴方向
		if(Y>=0)
			ucs.axis_y=pDatumPlate1->ucs.axis_z;
		else
			ucs.axis_y=-pDatumPlate1->ucs.axis_z;
		CLDSLinePart *pSetUpPart=(CLDSLinePart*)BelongModel()->FromPartHandle(S,CLS_LINETUBE,CLS_LINEANGLE);
		if(pDatumLineTube2||(pSetUpPart&&pSetUpPart->IsLinePart()))
		{	//指定Ｘ轴定位方向基准杆件
			//计算钢板Ｘ轴方向
			if(pDatumLineTube2)
				ucs.axis_x=pDatumLineTube2->End()-pDatumLineTube2->Start();	//由射线基准钢管确定装备坐标系X轴方向
			else 
				ucs.axis_x=pSetUpPart->End()-pSetUpPart->Start();	//由基准杆件确定装备坐标系X轴方向
			//计算钢板Ｚ轴方向
			ucs.axis_z=ucs.axis_x^ucs.axis_y;
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_x);
			normalize(ucs.axis_y);
			normalize(ucs.axis_z);
		} 
		else
		{	//未指定Ｘ轴定位方向基准杆件
			//计算钢板Ｚ轴方向（精确）
			f3dPoint axis_z=pDatumTube->End()-pDatumTube->Start();
			normalize(axis_z);
			//使用输入的Z轴方向为钢板Z轴方向	wht 10-01-21
			//原来仅仅是使用输入的Z轴方向调整Z轴的正负
			/*if(ucs.axis_z*axis_z>0)
				ucs.axis_z=axis_z;
			else
				ucs.axis_z=-axis_z;*/
			//计算钢板Ｘ轴方向
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_x);
			normalize(ucs.axis_y);
		}
		CLDSNode *pNode=BelongModel()->FromNodeHandle(P);
		//钢管肋板加工间隙 wht 10-08-28
		double radius=pDatumTube->GetDiameter()*0.5+manu_space;	//钢管半径
		double r=radius;
		double cosa=fabs(ucs.axis_z*pDatumTube->ucs.axis_z);
		if(pNode&&pDatumTube)
			SnapPerp(&ucs.origin,pDatumTube->Start(),pDatumTube->End(),pNode->Position(true));
		//钢管肋板在沿法线方向进行偏移时，要确保坐标原点在基准钢管心线上移动
		if(fabs(D)>EPS2&&pNode)
		{	//D!=0
			double tube_vec_dist=D/cosa;//转换成沿基准钢管方向移动的距离
			if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
				ucs.origin+=pDatumTube->ucs.axis_z*tube_vec_dist;
			else 
				ucs.origin-=pDatumTube->ucs.axis_z*tube_vec_dist;
		}
		f3dPoint vertice,inters,inters1;
		if(fabs(cosa)>EPS&&fabs(cosa)<EPS_COS&&pSetUpPart)	//椭圆弧
			r=radius/cosa;
		else if(fabs(cosa)<EPS)
			r=0; //为钢管轴向板 半径为0
		if(pDatumPlate1&&pDatumLineTube2)
		{	//第二个基准构件为钢管,此时原点只能在所选基点位置,无须考虑偏移量D。wht 10-10-26
			f3dPoint face_pick[2],line_pick[2],inters_vec[2],norm;
			if(Y>0||fabs(Y)<EPS)
				face_pick[0]=pDatumPlate1->ucs.origin+(pDatumPlate1->m_fNormOffset+pDatumPlate1->GetThick())*pDatumPlate1->ucs.axis_z;
			else
				face_pick[0]=pDatumPlate1->ucs.origin+pDatumPlate1->m_fNormOffset*pDatumPlate1->ucs.axis_z;
			Int3d2f(&line_pick[0],&inters_vec[0],ucs.origin,ucs.axis_z,face_pick[0],pDatumPlate1->ucs.axis_z);
			//修正两板面交线延伸方向的正负号
			f3dPoint vertice_vec;
			if(pDatumPlate1->vertex_list.GetNodeNum()>0)
			{
				vertice=pDatumPlate1->vertex_list.GetFirst()->vertex;
				coord_trans(vertice,pDatumPlate1->ucs,TRUE);
				vertice_vec=vertice-ucs.origin;
				//取钢板上第一点来纠正交线方向，会有不正确的地方,现修改为以下方式:
				//钢板的X轴方向一般都垂直于基准钢管轴线,此处先使用钢板上任意一点纠正钢板X轴方向,
				//然后使用X轴方向纠正交线方向。//wht 10-07-29
				f3dPoint axis_x1=pDatumPlate1->ucs.axis_x;	
				if(vertice_vec*axis_x1<0)
					axis_x1*=-1.0;
				if(axis_x1*inters_vec[0]<0)
					inters_vec[0]*=-1.0;
			}
			//射线基准钢管与基准钢管的交点以及延伸方向
			if(pDatumLineTube2->pStart->handle==pNode->handle)
			{
				inters_vec[1]=pDatumLineTube2->End()-pDatumLineTube2->Start();
				line_pick[1]=pDatumLineTube2->Start();
			}
			else //if(pDatumLineTube2->pEnd->handle==pNode->handle)
			{
				inters_vec[1]=pDatumLineTube2->Start()-pDatumLineTube2->End();
				line_pick[1]=pDatumLineTube2->End();
			}
			normalize(inters_vec[1]);
			f3dPoint offset_vec=inters_vec[0]^pDatumTube->ucs.axis_z;
			f3dPoint verify_vec;
			if(Y>0||fabs(Y)<EPS)	//钢板上平面添加肋板
				verify_vec=pDatumPlate1->ucs.axis_z;
			else					//钢板下平面添加肋板
				verify_vec=pDatumPlate1->ucs.axis_z*-1.0;
			if(offset_vec*verify_vec<0)
				offset_vec*=-1.0;
			//求射线钢管与基准钢管的交点
			line_pick[1]+=offset_vec*pDatumLineTube2->GetDiameter()*0.5;
			
			//计算钢管肋板与基准钢管相交内弧的起止点位置
			f3dPoint inner_arc_start,inner_arc_end;
			Int3dlc(&inner_arc_start,&vertice,line_pick[0],inters_vec[0],pDatumTube->Start(),pDatumTube->End(),radius,radius);
			vertice_vec=inner_arc_start-ucs.origin;
			if(vertice_vec*inters_vec[0]<0)
				inner_arc_start=vertice;
			Int3dlc(&inner_arc_end,&vertice,line_pick[1],inters_vec[1],pDatumTube->Start(),pDatumTube->End(),radius,radius);
			vertice_vec=inner_arc_end-ucs.origin;
			if(vertice_vec*inters_vec[1]<0)
				inner_arc_end=vertice;
			vector_trans(inters_vec[0],ucs,FALSE);
			vector_trans(inters_vec[1],ucs,FALSE);
			//顶部圆弧连接且H值为0表示外侧为圆弧,此时应保证由W，N生成的顶点位于半径为R的圆弧上 wht 10-07-29
			if(A>=0&&fabs(H)<EPS)
			{	//根据参数R重新计算W，N,保证在半径为R的圆弧上
				vertice=inner_arc_start;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				W=R-DISTANCE(vertice,f3dPoint(0,0,0));
				vertice=inner_arc_end;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				N=R-DISTANCE(vertice,f3dPoint(0,0,0));
				if(inters_vec[0].x>0)
				{
					double temp=W;
					W=N;
					N=W;
				}
			}
			if(inters_vec[0].x>0)
			{	//第一块板在右侧Ｘ轴正向
				//第一个顶点
				vertice=inner_arc_start;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=1;
				PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->m_bWeldEdge=TRUE;
				//第二个顶点
				vertice+=inters_vec[0]*N;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				if(H>0)
				{
					//第三个顶点
					vertice+=f3dPoint(-inters_vec[0].y,inters_vec[0].x)*H;
					vertice.feature=1;
					vertex_list.append(PROFILE_VERTEX(vertice,0));
					f3dPoint vertex2=vertice;
					if(A>=0)//板顶部为圆弧连接
					{
						//第四个顶点 为外圆弧起始点
						f2dPoint retPt,pickPt=vertice+f3dPoint(-inters_vec[0].y,inters_vec[0].x)*H;
						f2dCircle cir(R,f2dPoint(0,0));
						if(Tang2dpc(f2dPoint(vertex2.x,vertex2.y),cir,pickPt,retPt)>0)
							vertice.Set(retPt.x,retPt.y,0);
						else
							return FALSE;
						vertice.feature=1;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
						pVertex->type=2;
						pVertex->radius=R;
						//第五个顶点
						vertice=inner_arc_end;
						coord_trans(vertice,ucs,FALSE);
						vertice+=inters_vec[1]*W+f3dPoint(inters_vec[1].y,-inters_vec[1].x)*H;
						pickPt=vertice+f3dPoint(inters_vec[1].y,-inters_vec[1].x)*H;
						if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,pickPt,retPt))					
							vertice.Set(retPt.x,retPt.y,0);
						else
							return FALSE;
						vertice.feature=1;
						vertex_list.append(PROFILE_VERTEX(vertice,0));
					}
					//第六个顶点
					vertice=inner_arc_end;
					coord_trans(vertice,ucs,FALSE);
					vertice+=inters_vec[1]*W+f3dPoint(inters_vec[1].y,-inters_vec[1].x)*H;
					vertice.feature=1;
					vertex_list.append(PROFILE_VERTEX(vertice,0));
				}
				else if(A>=0&&fabs(H)<eps&&W>0)
				{	//顶部圆弧连接
					pVertex->type=2;
					pVertex->radius=R;
				}
				//第七个顶点
				vertice=inner_arc_end;
				coord_trans(vertice,ucs,FALSE);
				vertice+=inters_vec[1]*W;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->m_bWeldEdge=TRUE;
				//第八个顶点，为内圆弧起始点
				vertice=inner_arc_end;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				if(cosa>EPS_COS)	//圆弧
				{	//圆弧
					f3dPoint start=inner_arc_start;
					coord_trans(start,ucs,FALSE);
					coord_trans(vertice,ucs,FALSE);
					if(start.y*vertice.y<0)
					{
						pVertex->type=2;
						pVertex->radius=-pDatumTube->GetDiameter()*0.5;
						pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
						//大于180度的圆弧 分两段来生成
						vertice.Set(0,pDatumTube->GetDiameter()*0.5);
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
						pVertex->type=2;
						pVertex->radius=-pDatumTube->GetDiameter()*0.5;
					}
					else
					{	//指定圆弧半径 小于180°的圆弧
						pVertex->type=2;
						pVertex->radius=-pDatumTube->GetDiameter()*0.5;
					}
				}
				else
				{	//椭圆弧
					f3dPoint start=inner_arc_start;
					coord_trans(start,ucs,FALSE);
					coord_trans(vertice,ucs,FALSE);
					if(start.y*vertice.y<0)
					{
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=radius;
						pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
						//大于180度的圆弧 分两段来生成
						vertice.Set(0,pDatumTube->GetDiameter()*0.5);
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=radius;
					}
					else
					{	//指定圆弧半径 小于180°的圆弧
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=radius;
					}
				}
				pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
			}
			else
			{	//第一块板在左侧Ｘ轴负向
				//第一个顶点
				vertice=inner_arc_end;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=1;
				PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->m_bWeldEdge=TRUE;
				//第二个顶点
				vertice+=inters_vec[1]*N;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				if(H>0)
				{
					//第三个顶点
					vertice+=f3dPoint(-inters_vec[1].y,inters_vec[1].x)*H;
					vertice.feature=1;
					vertex_list.append(PROFILE_VERTEX(vertice,0));
					f3dPoint vertex2=vertice;
					if(A>=0)//板顶部为圆弧连接
					{
						//第四个顶点 为外圆弧起始点
						f2dPoint retPt,pickPt=vertice+f3dPoint(-inters_vec[1].y,inters_vec[1].x)*H;
						f2dCircle cir(R,f2dPoint(0,0));
						if(Tang2dpc(f2dPoint(vertex2.x,vertex2.y),cir,pickPt,retPt)>0)
							vertice.Set(retPt.x,retPt.y,0);
						else
							return FALSE;
						vertice.feature=1;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
						pVertex->type=2;
						pVertex->radius=R;
						//第五个顶点
						vertice=inner_arc_start;
						coord_trans(vertice,ucs,FALSE);
						vertice+=inters_vec[0]*W+f3dPoint(inters_vec[0].y,-inters_vec[0].x)*H;
						pickPt=vertice+f3dPoint(inters_vec[0].y,-inters_vec[0].x)*H;
						if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,pickPt,retPt))					
							vertice.Set(retPt.x,retPt.y,0);
						else
							return FALSE;
						vertice.feature=1;
						vertex_list.append(PROFILE_VERTEX(vertice,0));
					}
					//第六个顶点
					vertice=inner_arc_start;
					coord_trans(vertice,ucs,FALSE);
					vertice+=inters_vec[0]*W+f3dPoint(inters_vec[0].y,-inters_vec[0].x)*H;
					vertice.feature=1;
					vertex_list.append(PROFILE_VERTEX(vertice,0));
				}
				else if(A>=0&&fabs(H)<eps&&W>0)
				{	//顶部圆弧连接
					pVertex->type=2;
					pVertex->radius=R;
				}
				//第七个顶点
				vertice=inner_arc_start;
				coord_trans(vertice,ucs,FALSE);
				vertice+=inters_vec[0]*W;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->m_bWeldEdge=TRUE;
				//第八个顶点，为内圆弧起始点
				vertice=inner_arc_start;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
				if(cosa>EPS_COS)	//圆弧
				{	//圆弧
					f3dPoint end=inner_arc_end;
					coord_trans(end,ucs,FALSE);
					if(end.y*vertice.y<0)
					{
						pVertex->type=2;
						pVertex->radius=-pDatumTube->GetDiameter()*0.5;
						pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
						//大于180度的圆弧 分两段来生成
						vertice.Set(0,pDatumTube->GetDiameter()*0.5);
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
						pVertex->type=2;
						pVertex->radius=-pDatumTube->GetDiameter()*0.5;
					}
					else
					{	//指定圆弧半径 小于180°的圆弧
						pVertex->type=2;
						pVertex->radius=-pDatumTube->GetDiameter()*0.5;
					}
				}
				else
				{	//椭圆弧
					f3dPoint end=inner_arc_end;
					coord_trans(end,ucs,FALSE);
					if(end.y*vertice.y<0)
					{
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=radius;
						pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
						//大于180度的圆弧 分两段来生成
						vertice.Set(0,pDatumTube->GetDiameter()*0.5);
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=radius;
					}
					else
					{	//指定圆弧半径 小于180°的圆弧
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=radius;
					}
				}
				pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
			}
		}
		else if(pDatumPlate1&&pDatumPlate2)
		{	//有两块基准钢板的钢管肋板 
			f3dPoint face_pick[2],line_pick[2],inters_vec[2],norm;
			if(Y>0||fabs(Y)<EPS)
				face_pick[0]=pDatumPlate1->ucs.origin+(pDatumPlate1->m_fNormOffset+pDatumPlate1->GetThick())*pDatumPlate1->ucs.axis_z;
			else
				face_pick[0]=pDatumPlate1->ucs.origin+pDatumPlate1->m_fNormOffset*pDatumPlate1->ucs.axis_z;
			//法向偏移位置由下面的修正
			face_pick[1]=pDatumPlate2->ucs.origin+(pDatumPlate2->m_fNormOffset+pDatumPlate2->GetThick())*pDatumPlate2->ucs.axis_z;
			Int3d2f(&line_pick[0],&inters_vec[0],ucs.origin,ucs.axis_z,face_pick[0],pDatumPlate1->ucs.axis_z);
			Int3d2f(&line_pick[1],&inters_vec[1],ucs.origin,ucs.axis_z,face_pick[1],pDatumPlate2->ucs.axis_z);
			//修正两板面交线延伸方向的正负号
			f3dPoint vertice_vec;
			if(pDatumPlate1->vertex_list.GetNodeNum()>0)
			{
				vertice=pDatumPlate1->vertex_list.GetFirst()->vertex;
				coord_trans(vertice,pDatumPlate1->ucs,TRUE);
				vertice_vec=vertice-ucs.origin;
				//取钢板上第一点来纠正交线方向，会有不正确的地方,现修改为以下方式:
				//钢板的X轴方向一般都垂直于基准钢管轴线,此处先使用钢板上任意一点纠正钢板X轴方向,
				//然后使用X轴方向纠正交线方向。//wht 10-07-29
				f3dPoint axis_x1=pDatumPlate1->ucs.axis_x;	
				if(vertice_vec*axis_x1<0)
					axis_x1*=-1.0;
				if(axis_x1*inters_vec[0]<0)
					inters_vec[0]*=-1.0;
			}
			if(pDatumPlate2->vertex_list.GetNodeNum()>0)
			{
				vertice=pDatumPlate2->vertex_list.GetFirst()->vertex;
				coord_trans(vertice,pDatumPlate2->ucs,TRUE);
				vertice_vec=vertice-ucs.origin;
				//
				f3dPoint axis_x2=pDatumPlate2->ucs.axis_x;	
				if(vertice_vec*axis_x2<0)
					axis_x2*=-1.0;
				if(axis_x2*inters_vec[1]<0)
					inters_vec[1]*=-1.0;
			}
			f3dPoint N_vec2;
			vector_trans(inters_vec[0],ucs,FALSE);
			vector_trans(inters_vec[1],ucs,FALSE);
			if(inters_vec[0].x>0)	//第二块板在左侧Ｘ轴负方向
				N_vec2.Set(inters_vec[1].y,-inters_vec[1].x);
			else					//第二块板在右侧Ｘ轴正方向
				N_vec2.Set(-inters_vec[1].y,inters_vec[1].x);
			vector_trans(inters_vec[0],ucs,TRUE);
			vector_trans(inters_vec[1],ucs,TRUE);
			vector_trans(N_vec2,ucs,TRUE);
			if(N_vec2*pDatumPlate2->ucs.axis_z<0)
				line_pick[1]=pDatumPlate2->ucs.origin+pDatumPlate2->m_fNormOffset*pDatumPlate2->ucs.axis_z;
			
			//计算钢管肋板与基准钢管相交内弧的起止点位置
			f3dPoint inner_arc_start,inner_arc_end;
			Int3dlc(&inner_arc_start,&vertice,line_pick[0],inters_vec[0],pDatumTube->Start(),pDatumTube->End(),radius,radius);
			vertice_vec=inner_arc_start-ucs.origin;
			if(vertice_vec*inters_vec[0]<0)
				inner_arc_start=vertice;
			Int3dlc(&inner_arc_end,&vertice,line_pick[1],inters_vec[1],pDatumTube->Start(),pDatumTube->End(),radius,radius);
			vertice_vec=inner_arc_end-ucs.origin;
			if(vertice_vec*inters_vec[1]<0)
				inner_arc_end=vertice;
			vector_trans(inters_vec[0],ucs,FALSE);
			vector_trans(inters_vec[1],ucs,FALSE);
			//顶部圆弧连接且H值为0表示外侧为圆弧,此时应保证由W，N生成的顶点位于半径为R的圆弧上 wht 10-07-29
			if(A>=0&&fabs(H)<EPS)
			{	//根据参数R重新计算W，N,保证在半径为R的圆弧上
				vertice=inner_arc_start;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				W=R-DISTANCE(vertice,f3dPoint(0,0,0));
				vertice=inner_arc_end;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				N=R-DISTANCE(vertice,f3dPoint(0,0,0));
				if(inters_vec[0].x>0)
				{
					double temp=W;
					W=N;
					N=W;
				}
			}
			if(inters_vec[0].x>0)
			{	//第一块板在右侧Ｘ轴正向
				//第一个顶点
				vertice=inner_arc_start;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=1;
				PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->m_bWeldEdge=TRUE;
				//第二个顶点
				vertice+=inters_vec[0]*N;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				if(H>0)
				{
					//第三个顶点
					vertice+=f3dPoint(-inters_vec[0].y,inters_vec[0].x)*H;
					vertice.feature=1;
					vertex_list.append(PROFILE_VERTEX(vertice,0));
					f3dPoint vertex2=vertice;
					if(A>=0)//板顶部为圆弧连接
					{
						//第四个顶点 为外圆弧起始点
						f2dPoint retPt,pickPt=vertice+f3dPoint(-inters_vec[0].y,inters_vec[0].x)*H;
						f2dCircle cir(R,f2dPoint(0,0));
						if(Tang2dpc(f2dPoint(vertex2.x,vertex2.y),cir,pickPt,retPt)>0)
							vertice.Set(retPt.x,retPt.y,0);
						else
							return FALSE;
						vertice.feature=1;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
						pVertex->type=2;
						pVertex->radius=R;
						//第五个顶点
						vertice=inner_arc_end;
						coord_trans(vertice,ucs,FALSE);
						vertice+=inters_vec[1]*W+f3dPoint(inters_vec[1].y,-inters_vec[1].x)*H;
						pickPt=vertice+f3dPoint(inters_vec[1].y,-inters_vec[1].x)*H;
						if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,pickPt,retPt))					
							vertice.Set(retPt.x,retPt.y,0);
						else
							return FALSE;
						vertice.feature=1;
						vertex_list.append(PROFILE_VERTEX(vertice,0));
					}
					//第六个顶点
					vertice=inner_arc_end;
					coord_trans(vertice,ucs,FALSE);
					vertice+=inters_vec[1]*W+f3dPoint(inters_vec[1].y,-inters_vec[1].x)*H;
					vertice.feature=1;
					vertex_list.append(PROFILE_VERTEX(vertice,0));
				}
				else if(A>=0&&fabs(H)<eps&&W>0)
				{	//顶部圆弧连接
					pVertex->type=2;
					pVertex->radius=R;
				}
				//第七个顶点
				vertice=inner_arc_end;
				coord_trans(vertice,ucs,FALSE);
				vertice+=inters_vec[1]*W;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->m_bWeldEdge=TRUE;
				//第八个顶点，为内圆弧起始点
				vertice=inner_arc_end;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				if(cosa>EPS_COS)	//圆弧
				{	//圆弧
					f3dPoint start=inner_arc_start;
					coord_trans(start,ucs,FALSE);
					coord_trans(vertice,ucs,FALSE);
					if(start.y*vertice.y<0)
					{
						pVertex->type=2;
						pVertex->radius=-pDatumTube->GetDiameter()*0.5;
						pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
						//大于180度的圆弧 分两段来生成
						vertice.Set(0,pDatumTube->GetDiameter()*0.5);
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
						pVertex->type=2;
						pVertex->radius=-pDatumTube->GetDiameter()*0.5;
					}
					else
					{	//指定圆弧半径 小于180°的圆弧
						pVertex->type=2;
						pVertex->radius=-pDatumTube->GetDiameter()*0.5;
					}
				}
				else
				{	//椭圆弧
					f3dPoint start=inner_arc_start;
					coord_trans(start,ucs,FALSE);
					coord_trans(vertice,ucs,FALSE);
					if(start.y*vertice.y<0)
					{
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=radius;
						pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
						//大于180度的圆弧 分两段来生成
						vertice.Set(0,pDatumTube->GetDiameter()*0.5);
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=radius;
					}
					else
					{	//指定圆弧半径 小于180°的圆弧
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=radius;
					}
				}
				pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
			}
			else
			{	//第一块板在左侧Ｘ轴负向
				//第一个顶点
				vertice=inner_arc_end;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=1;
				PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->m_bWeldEdge=TRUE;
				//第二个顶点
				vertice+=inters_vec[1]*N;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				if(H>0)
				{
					//第三个顶点
					vertice+=f3dPoint(-inters_vec[1].y,inters_vec[1].x)*H;
					vertice.feature=1;
					vertex_list.append(PROFILE_VERTEX(vertice,0));
					f3dPoint vertex2=vertice;
					if(A>=0)//板顶部为圆弧连接
					{
						//第四个顶点 为外圆弧起始点
						f2dPoint retPt,pickPt=vertice+f3dPoint(-inters_vec[1].y,inters_vec[1].x)*H;
						f2dCircle cir(R,f2dPoint(0,0));
						if(Tang2dpc(f2dPoint(vertex2.x,vertex2.y),cir,pickPt,retPt)>0)
							vertice.Set(retPt.x,retPt.y,0);
						else
							return FALSE;
						vertice.feature=1;
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
						pVertex->type=2;
						pVertex->radius=R;
						//第五个顶点
						vertice=inner_arc_start;
						coord_trans(vertice,ucs,FALSE);
						vertice+=inters_vec[0]*W+f3dPoint(inters_vec[0].y,-inters_vec[0].x)*H;
						pickPt=vertice+f3dPoint(inters_vec[0].y,-inters_vec[0].x)*H;
						if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,pickPt,retPt))					
							vertice.Set(retPt.x,retPt.y,0);
						else
							return FALSE;
						vertice.feature=1;
						vertex_list.append(PROFILE_VERTEX(vertice,0));
					}
					//第六个顶点
					vertice=inner_arc_start;
					coord_trans(vertice,ucs,FALSE);
					vertice+=inters_vec[0]*W+f3dPoint(inters_vec[0].y,-inters_vec[0].x)*H;
					vertice.feature=1;
					vertex_list.append(PROFILE_VERTEX(vertice,0));
				}
				else if(A>=0&&fabs(H)<eps&&W>0)
				{	//顶部圆弧连接
					pVertex->type=2;
					pVertex->radius=R;
				}
				//第七个顶点
				vertice=inner_arc_start;
				coord_trans(vertice,ucs,FALSE);
				vertice+=inters_vec[0]*W;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->m_bWeldEdge=TRUE;
				//第八个顶点，为内圆弧起始点
				vertice=inner_arc_start;
				coord_trans(vertice,ucs,FALSE);
				vertice.z=0;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
				if(cosa>EPS_COS)	//圆弧
				{	//圆弧
					f3dPoint end=inner_arc_end;
					coord_trans(end,ucs,FALSE);
					if(end.y*vertice.y<0)
					{
						pVertex->type=2;
						pVertex->radius=-pDatumTube->GetDiameter()*0.5;
						pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
						//大于180度的圆弧 分两段来生成
						vertice.Set(0,pDatumTube->GetDiameter()*0.5);
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
						pVertex->type=2;
						pVertex->radius=-pDatumTube->GetDiameter()*0.5;
					}
					else
					{	//指定圆弧半径 小于180°的圆弧
						pVertex->type=2;
						pVertex->radius=-pDatumTube->GetDiameter()*0.5;
					}
				}
				else
				{	//椭圆弧
					f3dPoint end=inner_arc_end;
					coord_trans(end,ucs,FALSE);
					if(end.y*vertice.y<0)
					{
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=radius;
						pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
						//大于180度的圆弧 分两段来生成
						vertice.Set(0,pDatumTube->GetDiameter()*0.5);
						pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=radius;
					}
					else
					{	//指定圆弧半径 小于180°的圆弧
						pVertex->type=3;
						pVertex->center=ucs.origin;
						if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
							pVertex->column_norm=pDatumTube->ucs.axis_z;
						else
							pVertex->column_norm=-pDatumTube->ucs.axis_z;
						pVertex->radius=radius;
					}
				}
				pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
			}
		}
		else if(pDatumPlate1&&pDatumPlate2==NULL)	//仅有一块基准钢板的情况 即指定X轴方向的钢管肋板
		{
			f3dPoint face_pick,line_pick,inters_vec,norm;
			if(Y>=0)
				face_pick=pDatumPlate1->ucs.origin+(pDatumPlate1->m_fNormOffset+pDatumPlate1->GetThick())*pDatumPlate1->ucs.axis_z;
			else
				face_pick=pDatumPlate1->ucs.origin+pDatumPlate1->m_fNormOffset*pDatumPlate1->ucs.axis_z;
			Int3d2f(&line_pick,&inters_vec,ucs.origin,ucs.axis_z,face_pick,pDatumPlate1->ucs.axis_z);
			f3dPoint inner_arc_start,inner_arc_end;
			if(Int3dlc(&inner_arc_start,&inner_arc_end,line_pick,inters_vec,pDatumTube->Start(),pDatumTube->End(),radius,radius)!=2)
				return FALSE;
			/*
			//根据r以及cosa计算顶点坐标不准确，修改为直线与钢管求交 wht 10-07-09
			double offset_dist=-pDatumPlate1->m_fNormOffset;
			if(Y>0)
				offset_dist=pDatumPlate1->m_fNormOffset+pDatumPlate1->GetThick();
			*/
			coord_trans(inner_arc_start,ucs,FALSE);
			double offset_dist=inner_arc_start.y;
			r=fabs(inner_arc_start.x);
			//第一个顶点
			vertice.Set(r,offset_dist);
			vertice.feature=1;
			PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
			//第二个顶点
			vertice.Set(N+r,offset_dist);
			vertex_list.append(PROFILE_VERTEX(vertice,0));
			f3dPoint vertex2=vertice;
			//第三个顶点
			vertice.Set(N+r,H+offset_dist);
			vertice.feature=1;
			vertex_list.append(PROFILE_VERTEX(vertice,0));
			vertex2=vertice;
			if(A>=0)//板顶部为圆弧连接
			{
				//第四个顶点 为外圆弧起始点
				f2dPoint retPt,pickPt;
				f2dCircle cir(R,f2dPoint(0,0));
				pickPt.Set(vertice.x,vertice.y+H);
				if(Tang2dpc(f2dPoint(vertex2.x,vertex2.y),cir,pickPt,retPt))					
					vertice.Set(retPt.x,retPt.y,0);
				else
					return FALSE;
				vertice.feature=1;
				pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
				pVertex->type=2;
				pVertex->radius=R;
				//第五个顶点
				vertice.Set(-W-r,H,offset_dist);
				pickPt.Set(vertice.x,vertice.y+H);
				if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,pickPt,retPt))					
					vertice.Set(retPt.x,retPt.y,0);
				else
					return FALSE;
				vertice.feature=1;
				vertex_list.append(PROFILE_VERTEX(vertice,0));
			}
			//第六个顶点
			vertice.Set(-W-r,H+offset_dist);
			vertice.feature=1;
			vertex_list.append(PROFILE_VERTEX(vertice,0));
			//第七个顶点
			vertice.Set(-W-r,offset_dist,0);
			vertice.feature=1;
			vertex_list.append(PROFILE_VERTEX(vertice,0));
			//第八个顶点，为内圆弧起始点
			vertice.Set(-r,offset_dist,0);
			vertice.feature=1;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
			if(cosa>EPS_COS)	//圆弧
			{	//圆弧
				pVertex->type=2;
				pVertex->radius=-pDatumTube->GetDiameter()*0.5;
			}
			else
			{	//椭圆弧
				pVertex->type=3;
				pVertex->center=ucs.origin;
				if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
					pVertex->column_norm=pDatumTube->ucs.axis_z;
				else
					pVertex->column_norm=-pDatumTube->ucs.axis_z;
				pVertex->radius=-radius;//pDatumTube->GetDiameter()*0.5;
			}
			pVertex->m_bWeldEdge=TRUE;	//属于焊缝线起始点
		}
	}
	
	return TRUE;
}
/*else //if(F>=0)
{	//整圆形钢管肋板 即套在钢管上的肋板
	pDatumTube->BuildUCS();
	ucs.axis_y=pDatumPlate1->ucs.axis_z;	//Y轴同第一块基准钢板的Y轴同向
	CLDSLinePart *pSetUpPart=(CLDSLinePart*)BelongModel()->FromPartHandle(S,CLS_LINETUBE,CLS_LINEANGLE);
	if(pSetUpPart&&pSetUpPart->IsLinePart())
		ucs.axis_x=pSetUpPart->End()-pSetUpPart->Start();//由基准杆件确定装备坐标系X轴方向
	else
	{	//修正X轴方向 在未指定X轴定位方向基准杆件时 保证第一块基准钢板在X轴正方向一侧	
		ucs.axis_x=pDatumPlate1->ucs.axis_z^pDatumTube->ucs.axis_z;
		f3dPoint vertice_vec;
		if(pDatumPlate1->vertex_list.GetNodeNum()>0)
		{
			f3dPoint vertice=pDatumPlate1->vertex_list.GetFirst()->vertex;
			coord_trans(vertice,pDatumPlate1->ucs,TRUE);
			vertice_vec=vertice-ucs.origin;
			if(vertice_vec*ucs.axis_x<0)
				ucs.axis_x*=-1.0;
		}
	}
	ucs.axis_z=ucs.axis_x^ucs.axis_y;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;	//重新计算下Y轴确保坐标系为正交坐标系
	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	normalize(ucs.axis_z);

	CLDSNode *pNode=BelongModel()->FromNodeHandle(P);
	if(pNode&&pDatumTube)
		SnapPerp(&ucs.origin,pDatumTube->Start(),pDatumTube->End(),pNode->pos);
	if(D!=0&&pNode)
		ucs.origin+=ucs.axis_z*D;
	double radius=pDatumTube->GetDiameter()*0.5;	//钢管半径
	double r=radius;
	double cosa=fabs(ucs.axis_z*pDatumTube->ucs.axis_z);
	if(fabs(cosa)>EPS&&fabs(cosa)<EPS_COS&&pSetUpPart)	//椭圆弧
		r=radius/cosa;
	else if(fabs(cosa)<EPS)
		r=0; //为钢管轴向板 半径为0
	if(ftoi(N)==0&&ftoi(W)==0)
		return FALSE;	//N和W同时为0 则为法兰
	if(N>0)
		N+=pDatumTube->GetDiameter()*0.5;
	if(W>0)
		W+=pDatumTube->GetDiameter()*0.5;
	f3dPoint vertice;
	if(N>0)
	{	
		//第一个顶点
		vertice.Set(N,H);
		vertice.feature=1;
		PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
		//第二个顶点 为外圆弧起始点
		f2dPoint retPt,pickPt;
		f2dCircle cir(R,f2dPoint(0,0));
		pickPt.Set(vertice.x,vertice.y+H);
		if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,pickPt,retPt))					
			vertice.Set(retPt.x,retPt.y,0);
		else
			return FALSE;
		vertice.feature=1;
		if(W>0)
		{	pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
			pVertex->type=2;
			pVertex->center=ucs.origin;
			pVertex->radius=R;
		}
		else //if(W<=0)
		{
			double arc_angle,start_angle,end_angle;
			start_angle=Cal2dLineAng(0,0,retPt.x,retPt.y);
			end_angle=Cal2dLineAng(0,0,retPt.x,-retPt.y);
			arc_angle=end_angle-start_angle;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice,1));
			pVertex->sector_angle=arc_angle*0.5;
			pVertex->center=ucs.origin;
			//大于180度的圆弧 分两段来生成
			rotate_point_around_axis(vertice,arc_angle*0.5,f3dPoint(0,0,0),f3dPoint(0,0,100));
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice,1));
			pVertex->sector_angle=arc_angle*0.5;
			pVertex->center=ucs.origin;
		}
	}
	if(W>0)
	{
		//第三个顶点
		vertice.Set(-W,H);
		f2dPoint retPt,pickPt;
		f2dCircle cir(R,f2dPoint(0,0));
		pickPt.Set(vertice.x,vertice.y+H);
		if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,pickPt,retPt))					
			vertice.Set(retPt.x,retPt.y,0);
		else
			return FALSE;
		vertice.feature=1;
		PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
		//第四个顶点
		vertice.Set(-W,H);
		vertice.feature=1;
		vertex_list.append(PROFILE_VERTEX(vertice,0));
		//第五个顶点
		vertice.Set(-W,-H);
		vertice.feature=1;
		vertex_list.append(PROFILE_VERTEX(vertice,0));
		//第六个顶点 为外圆弧起始点
		f2dPoint retPt1,pickPt1;
		pickPt.Set(vertice.x,vertice.y-H);
		if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,pickPt,retPt))					
			vertice.Set(retPt.x,retPt.y,0);
		else
			return FALSE;
		vertice.feature=1;
		if(N>0)
		{
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice,0));
			pVertex->type=2;
			pVertex->center=ucs.origin;
			pVertex->radius=R;
		}
		else //if(N<=0)
		{
			double arc_angle,start_angle,end_angle;
			start_angle=Cal2dLineAng(0,0,retPt.x,retPt.y);
			end_angle=Cal2dLineAng(0,0,retPt.x,-retPt.y);
			arc_angle=2*Pi-(start_angle-end_angle);
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice,1));
			pVertex->sector_angle=arc_angle*0.5;
			pVertex->center=ucs.origin;
			//大于180度的圆弧 分两段来生成
			rotate_point_around_axis(vertice,arc_angle*0.5,f3dPoint(0,0,0),f3dPoint(0,0,100));
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice,1));
			pVertex->sector_angle=arc_angle*0.5;
			pVertex->center=ucs.origin;
		}
	}
	if(N>0)
	{
		//第七个顶点
		vertice.Set(N,-H);
		f2dPoint retPt,pickPt;
		f2dCircle cir(R,f2dPoint(0,0));
		pickPt.Set(vertice.x,vertice.y-H);
		if(Tang2dpc(f2dPoint(vertice.x,vertice.y),cir,pickPt,retPt))					
			vertice.Set(retPt.x,retPt.y,0);
		else
			return FALSE;
		vertice.feature=1;
		vertex_list.append(PROFILE_VERTEX(vertice,0));
		//第八个顶点
		vertice.Set(N,-H);
		vertice.feature=1;
		vertex_list.append(PROFILE_VERTEX(vertice,0));
	}
}*/
/*struct WELDCOVER_PLATE : public ICompareClass{
	CLDSGeneralPlate* pWeldPlate;
	double alfa;	//弧度制，
	double innerR;	//焊烧死角处工艺流水孔半径
	GEPOINT vxRayVec;
	WELDCOVER_PLATE(CLDSGeneralPlate* pRelaPlate=NULL,double _innerR=0){
		pWeldPlate=pRelaPlate;
		innerR=_innerR;
		alfa=0;	//需要后续计算
	}
	virtual int Compare(const ICompareClass *pCompareObj)const
	{
		WELDCOVER_PLATE* pCover2=(WELDCOVER_PLATE*)pCompareObj;
		if(alfa>pCover2->alfa)
			return -1;
		else if(alfa<pCover2->alfa)
			return  1;
		else
			return 0;
	}
};*/
int RetrievedWeldCoverPlates(CLDSParamPlate *pCoverPlate,PRESET_ARRAY8<WELDCOVER_PLATE> &arrWeldPlates,CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/) 
{
	CLDSGeneralPlate::DESIGN_PARAM::DUAL_VALUE coverplate1,coverplate2;
	CLDSGeneralPlate::DESIGN_PARAM itemparam;
	coverplate1.uiAttribute=coverplate2.uiAttribute=0;
	double BH=-1;
	pCoverPlate->GetDesignItemValue(KEY2C("BH"),&BH,pItemHashTbl);
	//GetDesignItemValue('F',&h,pItemHashTbl);	//第一块定位基准钢板
	if(!pCoverPlate->GetDesignItemValue('F',&itemparam,pItemHashTbl))
		coverplate1.hPart=0;
	else
	{
		if(itemparam.cValueType==3)	//句柄+数值
		{
			coverplate1.hPart=itemparam.value.hPart;
			coverplate1.uiAttribute=(BH==0)?itemparam.value.uiAttribute:0;
		}
		else //if(item_param.cValueType)
			coverplate1.hPart=itemparam.ToLong();
	}
	//GetDesignItemValue('G',&h,pItemHashTbl);	//第二块定位基准钢板
	if(!pCoverPlate->GetDesignItemValue('G',&itemparam,pItemHashTbl))
		coverplate2.hPart=0;
	else
	{
		if(itemparam.cValueType==3)	//句柄+数值
		{
			coverplate2.hPart=itemparam.value.hPart;
			coverplate2.uiAttribute=(BH==0)?itemparam.value.uiAttribute:0;
		}
		else //if(item_param.cValueType)
			coverplate2.hPart=itemparam.ToLong();
	}
	CLDSPlate *pDatumPlate1=(CLDSPlate*)pCoverPlate->BelongModel()->FromPartHandle(coverplate1.hPart,CLS_PLATE);
	CLDSPlate *pDatumPlate2=(CLDSPlate*)pCoverPlate->BelongModel()->FromPartHandle(coverplate2.hPart,CLS_PLATE);
	//
	long h=0,Y=0;
	pCoverPlate->GetDesignItemValue('Y',&Y,pItemHashTbl);
	pCoverPlate->GetDesignItemValue('C',&h,pItemHashTbl);	//定位基准钢管
	CLDSLineTube *pDatumTube=(CLDSLineTube*)pCoverPlate->BelongModel()->FromPartHandle(h,CLS_LINETUBE);
	if(!pDatumTube)	//如基准钢管为空 可能是基准钢管没当做参数存储时做的环向板
		pDatumTube=(CLDSLineTube*)pCoverPlate->BelongModel()->FromPartHandle(pCoverPlate->m_hPartWeldParent,CLS_LINETUBE);

	UINT i,j;
	double fInnerR=0;
	WELDCOVER_PLATE* pWeldCoverPlate=NULL;
	arrWeldPlates.ZeroPresetMemory();	//封板所盖住的其它关联钢板（可能没有定位关系，但有焊接需要定流水孔） wjh-2017.12.30
	if(pDatumPlate1!=NULL)
		arrWeldPlates.Append(WELDCOVER_PLATE(pDatumPlate1,coverplate1.uiAttribute*0.1));
	if(pDatumPlate2!=NULL)
		arrWeldPlates.Append(WELDCOVER_PLATE(pDatumPlate2,coverplate2.uiAttribute*0.1));
	for(i=0;i<4;i++)
	{
		CXhChar16 idstr("P%d",i);
		if(pCoverPlate->GetDesignItemValue(KEY2C(idstr),&itemparam,pItemHashTbl))
		{
			if(pDatumPlate1!=NULL&&pDatumPlate1->handle==itemparam.hValue)
				continue;
			else if(pDatumPlate2!=NULL&&pDatumPlate2->handle==itemparam.hValue)
				continue;
			fInnerR=0;
			if(itemparam.cValueType==3)	//句柄+数值
			{
				h=itemparam.value.hPart;
				fInnerR=itemparam.value.uiAttribute*0.1;
			}
			else //if(item_param.cValueType)
				h=itemparam.ToLong();

			for(j=0;j<arrWeldPlates.Count;j++)
			{
				pWeldCoverPlate=&arrWeldPlates.At(j);
				if(pWeldCoverPlate->pWeldPlate->handle==h)
				{
					pWeldCoverPlate->innerR=fInnerR;
					break;
				}
			}
			if(j==arrWeldPlates.Count)
			{
				CLDSGeneralPlate* pWeldPlate=(CLDSGeneralPlate*)pCoverPlate->BelongModel()->FromPartHandle(h,CLS_PLATE,CLS_PARAMPLATE);
				if(pWeldPlate)
				{
					pWeldCoverPlate=arrWeldPlates.Append(WELDCOVER_PLATE(pWeldPlate));
					pWeldCoverPlate->innerR=fInnerR;
				}
			}
		}
		else
			break;
	}
	int plateIndexArr[2]={0,1};//,nStartId=vertice.ID+1;
	double beta=180;
	pCoverPlate->GetDesignItemValue('B',&beta,pItemHashTbl);	//扇形夹角
	double fInitEdgeAngle=(-beta*0.5)*RADTODEG_COEF;
	for(i=0;i<arrWeldPlates.Count;i++)
	{
		WELDCOVER_PLATE* pWeldCover=&arrWeldPlates.At(i);
		GEPOINT vxRayVec=pDatumTube->GetPlateVertSpreadVec(pWeldCover->pWeldPlate);
		bool bTransSectPlate=false;
		if(vxRayVec.IsZero()&&Y==0&&arrWeldPlates.Count==1)	//贯穿板
		{
			GEPOINT vTubeAxisZ=pDatumTube->End()-pDatumTube->Start();
			normalize(vTubeAxisZ);
			vxRayVec=vTubeAxisZ^pWeldCover->pWeldPlate->ucs.axis_z;
			normalize(vxRayVec);
			bTransSectPlate=true;
		}
		vector_trans(vxRayVec,pCoverPlate->ucs,FALSE,TRUE);
		arrWeldPlates.At(i).vxRayVec=vxRayVec;
		double ray_alfa=Cal2dLineAng(0,0,vxRayVec.x,vxRayVec.y)-fInitEdgeAngle;
		pWeldCover->alfa=(ray_alfa>=Pi*2)?ray_alfa-Pi*2:ray_alfa;
		if(bTransSectPlate&&pWeldCover->alfa>beta*RADTODEG_COEF)
		{
			pWeldCover->alfa-=Pi;
			arrWeldPlates.At(i).vxRayVec=-vxRayVec;
		}
		if( i<2&&(pWeldCover->pWeldPlate==pDatumPlate1||pWeldCover->pWeldPlate==pDatumPlate2)&&
			pCoverPlate->GetDesignItemValue(KEY2C(CXhChar16("R%d",i+1)),&fInnerR,pItemHashTbl))
			pWeldCover->innerR=fInnerR;	//封板的两块定位基板之前都是以独立参数形式存储焊接死角的倒角内弧半径
	}
	CHeapSort<WELDCOVER_PLATE>::HeapSortClassic(arrWeldPlates.pPresetObjs,arrWeldPlates.Count);
	return arrWeldPlates.Size();
}
BOOL CLDSParamPlate::DesignCircularCoverPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{	//环向封板也应该修改为支持关联钢板为参数化板的情况，暂时未修改 wht 11-06-03
	f3dPoint vertice,axis_x1,axis_x2,vertex_start,vertice_vec,inters1,inters2;
	double alfa=0,beta=180;
	GetDesignItemValue('A',&alfa,pItemHashTbl);	//扇形旋转角
	GetDesignItemValue('B',&beta,pItemHashTbl);	//扇形夹角
	long h;
	GetDesignItemValue('E',&h,pItemHashTbl);	//定位基准节点
	DESIGN_PARAM::DUAL_VALUE coverplate1,coverplate2;
	DESIGN_PARAM itemparam;
	coverplate1.uiAttribute=coverplate2.uiAttribute=0;
	CLDSNode *pBaseNode=BelongModel()->FromNodeHandle(h);
	//GetDesignItemValue('F',&h,pItemHashTbl);	//第一块定位基准钢板
	if(!GetDesignItemValue('F',&itemparam,pItemHashTbl))
		coverplate1.hPart=h=0;
	else
	{
		if(itemparam.cValueType==3)	//句柄+数值
		{
			coverplate1.hPart=h=itemparam.value.hPart;
			coverplate1.uiAttribute=itemparam.value.uiAttribute;
		}
		else //if(item_param.cValueType)
			coverplate1.hPart=h=itemparam.ToLong();
	}
	CLDSGeneralPlate *pDatumPlate1=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(h,CLS_PLATE,CLS_PARAMPLATE);
	h=0;
	//GetDesignItemValue('G',&h,pItemHashTbl);	//第二块定位基准钢板
	if(!GetDesignItemValue('G',&itemparam,pItemHashTbl))
		coverplate2.hPart=h=0;
	else
	{
		if(itemparam.cValueType==3)	//句柄+数值
		{
			coverplate2.hPart=h=itemparam.value.hPart;
			coverplate2.uiAttribute=itemparam.value.uiAttribute;
		}
		else //if(item_param.cValueType)
			coverplate2.hPart=h=itemparam.ToLong();
	}
	CLDSPlate *pDatumPlate2=(CLDSPlate*)BelongModel()->FromPartHandle(h,CLS_PLATE);
	GetDesignItemValue('C',&h,pItemHashTbl);	//定位基准钢管
	CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(h,CLS_LINETUBE);
	if(!pDatumTube)	//如基准钢管为空 可能是基准钢管没当做参数存储时做的环向板
		pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
	if(pBaseNode==NULL||pDatumPlate1==NULL||pDatumTube==NULL)//||pDatumPlate2==NULL
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Log2File()->Log("0x%X Cicular cover plate{%s} missing datum object(node、relative plate or tube)，design failed!",handle,(char*)sPartNo);
#else
		Log2File()->Log("0x%X 环向封板{%s}缺少基准构件(节点、关联钢板或钢管)，设计失败!",handle,(char*)sPartNo);
#endif
		return FALSE;//第2块基准钢板可以为空 一块基准钢板也可确定环向封板位置(有些情况只有一块基准钢板)
	}
	double W=0,D=0,R=0,S=0,N=0,M=0;
	GetDesignItemValue('W',&W,pItemHashTbl);	//环形带宽W
	GetDesignItemValue('D',&D,pItemHashTbl);	//台口高度H
	GetDesignItemValue('R',&R,pItemHashTbl);	//环向板内圆弧半径(=钢管半径+加工间隙)
	GetDesignItemValue('S',&S,pItemHashTbl);	//-1:设计上封板 1:设计下封板
	GetDesignItemValue('M',&M,pItemHashTbl);	//M=-1时表示要设计贯通整板另一侧的封板
	GetDesignItemValue('N',&N,pItemHashTbl);	//简化封板与β对应侧边,距基准钢板的距离
	long Y=0; 
	GetDesignItemValue('Y',&Y,pItemHashTbl);	//封板类型
	//////////////////////////////////////////////////////////////////////////
	pDatumTube->BuildUCS();
	//设定环向封板坐标系
	ucs=pDatumTube->ucs;
	SnapPerp(&ucs.origin,pDatumTube->Start(),pDatumTube->End(),pBaseNode->Position(true));
	//计算上下封板的位置
	double up_len,down_len;
	BOOL bFirstVertex=TRUE;
	PROFILE_VERTEX *pVertex=NULL;
	//判断基准钢板Y轴是否与基准钢管延伸方向共线 wht 10-10-15
	BOOL bAdjustUcs=FALSE;
	f3dPoint tube_len_vec=pDatumTube->End()-pDatumTube->Start();
	normalize(tube_len_vec);
	UCS_STRU temp_ucs=pDatumPlate1->ucs;
	f3dPoint plate_axis_x1=pDatumPlate1->ucs.axis_x;
	if(fabs(tube_len_vec*pDatumPlate1->ucs.axis_y)<EPS_COS2)
	{
		bAdjustUcs=TRUE;
		//X,Y轴互换，Z轴反向
		temp_ucs.axis_y=pDatumPlate1->ucs.axis_x;
		temp_ucs.axis_x=pDatumPlate1->ucs.axis_y;
		temp_ucs.axis_z=-1.0*pDatumPlate1->ucs.axis_z;
		plate_axis_x1=temp_ucs.axis_x;
	}

	int sign=1,plate_style_left0_right1_mid2=-1;	//0:顶点在X轴负向（Y轴左侧）1:顶点在X轴正向（Y轴右侧）2:顶点分布在Y轴两侧
	for(pVertex=pDatumPlate1->vertex_list.GetFirst();pVertex;pVertex=pDatumPlate1->vertex_list.GetNext())
	{
		f3dPoint temp_vertex=pVertex->vertex;
		if(bAdjustUcs)
		{	//钢板Y轴与基准钢板延伸方向不一致
			coord_trans(temp_vertex,pDatumPlate1->ucs,TRUE);
			coord_trans(temp_vertex,temp_ucs,FALSE);
		}
		if(plate_style_left0_right1_mid2!=2)
		{
			if(temp_vertex.x>0&&plate_style_left0_right1_mid2==-1)
				plate_style_left0_right1_mid2=1;
			else if(temp_vertex.x>0&&plate_style_left0_right1_mid2==0)
				plate_style_left0_right1_mid2=2;
			else if(temp_vertex.x<0&&plate_style_left0_right1_mid2==-1)
				plate_style_left0_right1_mid2=0;
			else if(temp_vertex.x<0&&plate_style_left0_right1_mid2==1)
				plate_style_left0_right1_mid2=2;
		}
		vertice=pVertex->vertex;
		coord_trans(vertice,pDatumPlate1->ucs,TRUE);
		vertice_vec=vertice-ucs.origin;
		double dd=vertice_vec*ucs.axis_z;
		if(bFirstVertex)
		{
			up_len=down_len=dd;
			bFirstVertex=FALSE;
		}
		else if(dd<up_len)
			up_len=dd;
		else if(dd>down_len)
			down_len=dd;
	}
	if(plate_style_left0_right1_mid2==0)
		sign=-1;	//X轴向与板连接方向背离
	//根据第一块基准板的重心点计算板的延伸方向(与钢管Z轴垂直)
	axis_x1=pDatumPlate1->ucs.axis_z^ucs.axis_z;
	if(sign*(axis_x1*plate_axis_x1)<0)
		axis_x1*=-1.0;
	if(M==-1)	//设计贯通连板另外一侧的封板
		axis_x1*=-1.0;
	normalize(axis_x1);
	if(pDatumPlate2)
	{	//当用户指定第二块基准钢板时才需要执行以下代码
		bAdjustUcs=FALSE;
		plate_style_left0_right1_mid2=-1;
		temp_ucs=pDatumPlate2->ucs;
		f3dPoint plate_axis_x2=pDatumPlate2->ucs.axis_x;
		if(fabs(tube_len_vec*pDatumPlate2->ucs.axis_y)<EPS_COS2)
		{	//判断基准钢板Y轴是否与基准钢管延伸方向共线 wht 10-10-15
			bAdjustUcs=TRUE;
			//X,Y轴互换，Z轴方向
			temp_ucs.axis_y=pDatumPlate2->ucs.axis_x;
			temp_ucs.axis_x=pDatumPlate2->ucs.axis_y;
			temp_ucs.axis_z=-pDatumPlate2->ucs.axis_z;
			plate_axis_x2=temp_ucs.axis_x;
		}
		for(pVertex=pDatumPlate2->vertex_list.GetFirst();pVertex;pVertex=pDatumPlate2->vertex_list.GetNext())
		{
			f3dPoint temp_vertex=pVertex->vertex;
			if(bAdjustUcs)
			{	//钢板Y轴与基准钢板延伸方向不一致
				coord_trans(temp_vertex,pDatumPlate2->ucs,TRUE);
				coord_trans(temp_vertex,temp_ucs,FALSE);
			}
			if(plate_style_left0_right1_mid2!=2)
			{
				if(temp_vertex.x>0&&plate_style_left0_right1_mid2==-1)
					plate_style_left0_right1_mid2=1;
				else if(temp_vertex.x>0&&plate_style_left0_right1_mid2==0)
					plate_style_left0_right1_mid2=2;
				else if(temp_vertex.x<0&&plate_style_left0_right1_mid2==-1)
					plate_style_left0_right1_mid2=0;
				else if(temp_vertex.x<0&&plate_style_left0_right1_mid2==1)
					plate_style_left0_right1_mid2=2;
			}
			vertice=pVertex->vertex;
			coord_trans(vertice,pDatumPlate2->ucs,TRUE);
			vertice_vec=vertice-ucs.origin;
			double dd=vertice_vec*ucs.axis_z;
			if(bFirstVertex)
			{
				up_len=down_len=dd;
				bFirstVertex=FALSE;
			}
			else if(dd<up_len)
				up_len=dd;
			else if(dd>down_len)
				down_len=dd;
		}
		//根据第二块基准板的重心点计算板的延伸方向(与钢管Z轴垂直)
		if(plate_style_left0_right1_mid2==0)
			sign=-1;
		else
			sign=1;
		axis_x2=pDatumPlate2->ucs.axis_z^ucs.axis_z;
		if(sign*(axis_x2*plate_axis_x2)<0)
			axis_x2*=-1.0;
		normalize(axis_x2);
	}
	if(S<0)	//设计上封板
		ucs.origin+=(up_len-0.5*thick)*ucs.axis_z;
	else	//设计下封板
		ucs.origin+=(down_len+0.5*thick)*ucs.axis_z;
	if(axis_x2.IsZero())
		ucs.axis_x=axis_x1;
	else
	{
		ucs.axis_x=axis_x1+axis_x2;
		normalize(ucs.axis_x);
	}
	if(Y==0&&alfa!=0)	//生成普通封板时才需要根据α角调整坐标系
		RotateVectorAroundVector(ucs.axis_x,alfa*RADTODEG_COEF,ucs.axis_z);
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	if(Y==3)
	{	//直角封板，根据封板长度计算beta
		double fCosa=sqrt(SQR(R)-SQR(N*0.5))/R;
		beta=ACOS(fCosa)*2*DEGTORAD_COEF;	
		double angle_start=-beta*0.5*RADTODEG_COEF,angle_end=beta*0.5*RADTODEG_COEF;
		f3dPoint ptS(R*cos(angle_start),R*sin(angle_start));	//圆弧起点
		f3dPoint ptE(R*cos(angle_end),R*sin(angle_end));		//圆弧终点
		//第一个顶点
		vertice=ptS;
		vertex_start.feature=1;
		vertex_start.ID=1;
		vertex_list.append(PROFILE_VERTEX(vertice));
		//第二个顶点
		vertice.Set(ptS.x+W,ptS.y,0);
		vertice.feature=1;
		vertice.ID=2;
		vertex_list.append(PROFILE_VERTEX(vertice));
		//第三个顶点
		vertice.Set(ptE.x+W,ptE.y,0);
		vertice.feature=1;
		vertice.ID=3;
		vertex_list.append(PROFILE_VERTEX(vertice));
		//第四个顶点
		vertice=ptE;
		vertice.feature=1;
		vertice.ID=4;
		pVertex=vertex_list.append(PROFILE_VERTEX(vertice,2));
		pVertex->radius=-R;
		pVertex->m_bWeldEdge=TRUE; 
	}
	else if(Y>0)
	{	//Y>0表示主扇形角β在Y轴正方侧的简化封板
		if(beta>=180||D>W)
			return FALSE;	
		double angle_start=beta*RADTODEG_COEF;
		//第一个顶点
		f2dLine line2d;
		line2d.startPt.Set(R,-N);
		line2d.endPt.Set(R+W,-N);
		f2dArc arc;
		arc.startPt.Set(0,-R);
		arc.angle=0.5*Pi;
		vertice.z=0;
		Int2dla(line2d,arc,vertice.x,vertice.y,inters2.x,inters2.y);
		vertice.feature=1;
		vertice.ID=1;
		vertex_list.append(PROFILE_VERTEX(vertice));
		//第二个顶点 外圆弧起点
		arc.startPt.Set(0,-R-W);
		arc.angle=0.5*Pi;
		Int2dla(line2d,arc,vertice.x,vertice.y,inters2.x,inters2.y);
		vertice.feature=1;
		vertice.ID=2;
		pVertex=vertex_list.append(PROFILE_VERTEX(vertice,2));
		pVertex->radius=R+W;
		if(D<W)
		{	//第三个顶点 外圆弧终点
			f3dPoint next_vertice;
			next_vertice.Set((R+D)*cos(angle_start),(R+D)*sin(angle_start));
			line2d.startPt=next_vertice;
			line2d.endPt.Set(line2d.startPt.x+D*cos(angle_start-0.25*Pi),line2d.startPt.y+D*sin(angle_start-0.25*Pi));
			arc.startPt.Set(0,R+W);
			arc.angle=-0.5*Pi;
			Int2dla(line2d,arc,vertice.x,vertice.y,inters2.x,inters2.y);
			vertice.feature=1;
			vertice.ID=3;
			vertex_list.append(PROFILE_VERTEX(vertice));
			//第四个顶点
			next_vertice.feature=1;
			next_vertice.ID=4;
			vertex_list.append(PROFILE_VERTEX(next_vertice));
		}
		else 
		{	//第三个顶点 外圆弧终点
			vertice=f3dPoint((R+W)*cos(angle_start),(R+W)*sin(angle_start));
			vertice.feature=1;
			vertice.ID=3;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
		}	
		//第五个顶点	内圆弧起点
		vertice=f3dPoint(R*cos(angle_start),R*sin(angle_start));
		vertice.feature=1;
		if(D<W)
			vertice.ID=5;
		else  //第四个顶点
			vertice.ID=4;
		pVertex=vertex_list.append(PROFILE_VERTEX(vertice,2));
		pVertex->radius=-R;
		pVertex->m_bWeldEdge=TRUE;
	}
	else if(Y<0)
	{	//Y<0表示主扇形角β在Y轴负方向侧的简化封板
		if(beta>=180||D>W)
			return FALSE;
		double angle_start=beta*RADTODEG_COEF*-1.0;
		//第一个顶点
		vertice=f3dPoint(R*cos(angle_start),R*sin(angle_start));
		vertice.feature=1;
		vertice.ID=1;
		vertex_list.append(PROFILE_VERTEX(vertice));
		if(D<W)
		{	//第二个顶点
			vertice=f3dPoint((R+D)*cos(angle_start),(R+D)*sin(angle_start));
			vertice.feature=1;
			vertice.ID=2;
			vertex_list.append(PROFILE_VERTEX(vertice));
			//第三个顶点 外圆弧起点
			f2dLine line2d;
			line2d.startPt=vertice;
			line2d.endPt.Set(line2d.startPt.x+D*cos(angle_start+0.25*Pi),line2d.startPt.y+D*sin(angle_start+0.25*Pi));
			f2dArc arc;
			arc.startPt.Set(0,R+W);
			arc.angle=0.5*Pi;
			Int2dla(line2d,arc,vertice.x,vertice.y,inters2.x,inters2.y);
			vertice.feature=1;
			vertice.ID=3;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice,2));
			pVertex->radius=R+W;
		}
		else 
		{	//第二个顶点 外圆弧起点
			vertice=f3dPoint((R+W)*cos(angle_start),(R+W)*sin(angle_start));
			vertice.feature=1;
			vertice.ID=2;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
			pVertex->radius=R+W;
		}
		f2dLine line2d;
		line2d.startPt.Set(R,N);
		line2d.endPt.Set(R+W,N);
		//第三(四)个顶点
		f2dArc arc;
		arc.startPt.Set(0,R+W);
		arc.angle=-0.5*Pi;
		Int2dla(line2d,arc,vertice.x,vertice.y,inters2.x,inters2.y);
		vertice.feature=1;
		if(D<W)
			vertice.ID=4;
		else  
			vertice.ID=3;
		vertex_list.append(PROFILE_VERTEX(vertice));
		//第四(五)个顶点	内圆弧起点
		arc.startPt.Set(0,R);
		arc.angle=-0.5*Pi;
		Int2dla(line2d,arc,vertice.x,vertice.y,inters2.x,inters2.y);
		vertice.feature=1;
		if(D<W)
			vertice.ID=5;
		else  //第四个顶点
			vertice.ID=4;
		pVertex=vertex_list.append(PROFILE_VERTEX(vertice,2));
		pVertex->radius=-R;
		pVertex->m_bWeldEdge=TRUE;
	}
	else 
	{	//普通封板
		//double angle_start=(alfa-beta*0.5)*RADTODEG_COEF,angle_end=(alfa+beta*0.5)*RADTODEG_COEF;
		double angle_start=-beta*0.5*RADTODEG_COEF,angle_end=beta*0.5*RADTODEG_COEF;
		if(beta>=360||D>W)
			return FALSE;	//整环则为法兰
		//第一个顶点
		vertex_start.Set(R*cos(angle_start),R*sin(angle_start));
		vertex_start.feature=1;
		vertex_start.ID=1;
		vertex_list.append(PROFILE_VERTEX(vertex_start));
		if(D>0 && D<W)
		{
			//第二个顶点
			vertice=vertex_start+f3dPoint(D*cos(angle_start),D*sin(angle_start));
			vertice.feature=1;
			vertice.ID=2;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
			//第三个顶点	圆弧起点
			f2dLine line2d;
			line2d.startPt.Set(vertice.x,vertice.y);
			line2d.endPt.Set(vertice.x+D*cos(angle_start+0.25*Pi),vertice.y+D*sin(angle_start+0.25*Pi));
			f2dArc arc;
			arc.startPt.Set((R+W)*cos(angle_start),(R+W)*sin(angle_start));
			arc.angle=angle_end-angle_start;
			Int2dla(line2d,arc,vertice.x,vertice.y,inters2.x,inters2.y);
			vertice.feature=1;
			vertice.ID=3;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice,2));
			pVertex->radius=R+W;
			//第四个顶点
			line2d.startPt.Set((R+D)*cos(angle_end),(R+D)*sin(angle_end));
			line2d.endPt.Set(line2d.startPt.x+D*cos(angle_end-0.25*Pi),line2d.startPt.y+D*sin(angle_end-0.25*Pi));
			Int2dla(line2d,arc,vertice.x,vertice.y,inters2.x,inters2.y);
			vertice.feature=1;
			vertice.ID=4;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
			//第五个顶点
			vertice=f3dPoint((R+D)*cos(angle_end),(R+D)*sin(angle_end));
			vertice.feature=1;
			vertice.ID=5;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
		}
		else if(D<=0||fabs(D-W)<EPS)
		{
			if(D<=0)
				D=W;
			//第二个顶点	圆弧起点
			vertice=vertex_start+f3dPoint(D*cos(angle_start),D*sin(angle_start));
			vertice.feature=1;
			vertice.ID=2;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice,2));
			pVertex->radius=R+W;
			//第三个顶点
			vertice=f3dPoint((R+D)*cos(angle_end),(R+D)*sin(angle_end));
			vertice.feature=1;
			vertice.ID=3;
			pVertex=vertex_list.append(PROFILE_VERTEX(vertice));
		}	
		//第六个顶点	圆弧起点
		vertice=f3dPoint(R*cos(angle_end),R*sin(angle_end));
		vertice.feature=1;
		vertice.ID=6;
		if(fabs(D-W)<EPS) //第四个顶点
			vertice.ID=4;
		pVertex=vertex_list.append(PROFILE_VERTEX(vertice,2));
		pVertex->radius=-R;
		pVertex->m_bWeldEdge=TRUE; 
	}
	if(Y==0||Y==3)
	{
		//处理封板上分布于覆盖立板两侧的工艺流水孔（含半圆孔或圆孔两种类型）
		double BH=0,BG=0,BS=0;	//BH:流水孔直径(<0.无孔;0.半圆孔;1.圆孔);BG:流水孔到管壁距离;BS;流水孔到立板壁距离
		if(GetDesignItemValue(KEY2C("BH"),&BH,pItemHashTbl)&&fabs(BH)<EPS)
		{	//在T型焊缝角点上设流水半圆孔
			UINT i,j;
			double fInnerR=0;
			WELDCOVER_PLATE* pWeldCoverPlate=NULL;
			PRESET_ARRAY8<WELDCOVER_PLATE> arrWeldPlates;	//封板所盖住的其它关联钢板（可能没有定位关系，但有焊接需要定流水孔） wjh-2017.12.30
			if(pDatumPlate1!=NULL)
				arrWeldPlates.Append(WELDCOVER_PLATE(pDatumPlate1,coverplate1.uiAttribute*0.1));
			if(pDatumPlate2!=NULL)
				arrWeldPlates.Append(WELDCOVER_PLATE(pDatumPlate2,coverplate2.uiAttribute*0.1));
			for(i=0;i<4;i++)
			{
				CXhChar16 idstr("P%d",i);
				if(GetDesignItemValue(KEY2C(idstr),&itemparam,pItemHashTbl))
				{
					if(pDatumPlate1!=NULL&&pDatumPlate1->handle==itemparam.hValue)
						continue;
					else if(pDatumPlate2!=NULL&&pDatumPlate2->handle==itemparam.hValue)
						continue;
					fInnerR=0;
					if(itemparam.cValueType==3)	//句柄+数值
					{
						h=itemparam.value.hPart;
						fInnerR=itemparam.value.uiAttribute*0.1;
					}
					else //if(item_param.cValueType)
						h=itemparam.ToLong();

					for(j=0;j<arrWeldPlates.Count;j++)
					{
						pWeldCoverPlate=&arrWeldPlates.At(j);
						if(pWeldCoverPlate->pWeldPlate->handle==h)
						{
							pWeldCoverPlate->innerR=fInnerR;
							break;
						}
					}
					if(j==arrWeldPlates.Count)
					{
						CLDSGeneralPlate* pWeldPlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(h,CLS_PLATE,CLS_PARAMPLATE);
						if(pWeldPlate)
						{
							pWeldCoverPlate=arrWeldPlates.Append(WELDCOVER_PLATE(pWeldPlate));
							pWeldCoverPlate->innerR=fInnerR;
						}
					}
				}
				else
					break;
			}
			int nStartId=vertice.ID+1;
			int plateIndexArr[2]={0,1};//,nStartId=vertice.ID+1;
			double fInitEdgeAngle=(-beta*0.5)*RADTODEG_COEF;
			for(i=0;i<arrWeldPlates.Count;i++)
			{
				WELDCOVER_PLATE* pWeldCover=&arrWeldPlates.At(i);
				GEPOINT vxRayVec=pDatumTube->GetPlateVertSpreadVec(pWeldCover->pWeldPlate);
				bool bTransSectPlate=false;
				if(vxRayVec.IsZero()&&Y==0&&arrWeldPlates.Count==1)	//贯穿板
				{
					GEPOINT vTubeAxisZ=pDatumTube->End()-pDatumTube->Start();
					normalize(vTubeAxisZ);
					vxRayVec=vTubeAxisZ^pWeldCover->pWeldPlate->ucs.axis_z;
					normalize(vxRayVec);
					bTransSectPlate=true;
				}
				vector_trans(vxRayVec,ucs,FALSE,TRUE);
				arrWeldPlates.At(i).vxRayVec=vxRayVec;
				double ray_alfa=Cal2dLineAng(0,0,vxRayVec.x,vxRayVec.y)-fInitEdgeAngle;
				pWeldCover->alfa=(ray_alfa>=Pi*2)?ray_alfa-Pi*2:ray_alfa;
				if(bTransSectPlate&&pWeldCover->alfa>beta*RADTODEG_COEF)
				{
					pWeldCover->alfa-=Pi;
					arrWeldPlates.At(i).vxRayVec=-vxRayVec;
				}
				if( i<2&&(pWeldCover->pWeldPlate==pDatumPlate1||pWeldCover->pWeldPlate==pDatumPlate2)&&
					GetDesignItemValue(KEY2C(CXhChar16("R%d",i+1)),&fInnerR,pItemHashTbl))
					pWeldCover->innerR=fInnerR;	//封板的两块定位基板之前都是以独立参数形式存储焊接死角的倒角内弧半径
			}
			CHeapSort<WELDCOVER_PLATE>::HeapSortClassic(arrWeldPlates.pPresetObjs,arrWeldPlates.Count);
			double betaByRad=beta*RADTODEG_COEF;
			for(i=0;i<arrWeldPlates.Count;i++)
			{
				WELDCOVER_PLATE* pWeldCover=&arrWeldPlates.At(i);
				CLDSGeneralPlate *pPlate=pWeldCover->pWeldPlate;
				GEPOINT axis_x=pWeldCover->vxRayVec;
				double fInnerR=pWeldCover->innerR;
				if(pWeldCover->alfa>(betaByRad-RADTODEG_COEF)||pWeldCover->alfa<RADTODEG_COEF)
					continue;	//超出封板扇形角范围
				if(pPlate&&!axis_x.IsZero()&&fInnerR>0)
				{	//计算角度
					double fOffsetAngle=asin(fInnerR/R);
					//vector_trans(axis_x,ucs,FALSE);
					normalize(axis_x);
					axis_x.z=0;
					RotateVectorAroundVector(axis_x,fOffsetAngle,GEPOINT(0,0,1));
					//第七个顶点	流水孔的起点
					vertice=R*axis_x;
					vertice.feature=1;
					vertice.ID=nStartId++;
					pVertex=vertex_list.append(PROFILE_VERTEX(vertice,2));
					pVertex->radius=-fInnerR;
					//第八个顶点	流水孔的终点
					RotateVectorAroundVector(axis_x,-2*fOffsetAngle,GEPOINT(0,0,1));
					vertice=R*axis_x;
					vertice.feature=1;
					vertice.ID=nStartId++;
					pVertex=vertex_list.append(PROFILE_VERTEX(vertice,2));
					pVertex->radius=-R;
					pVertex->m_bWeldEdge=TRUE;
				}
			}
		}
	}
	return TRUE;
}

BOOL CLDSParamPlate::DesignStdPartUcs()
{
	if(!m_bStdPart)	//非标准构件
		return FALSE;
	CLDSLineTube *pLineTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
	if(pLineTube==NULL)
		return FALSE;
	pLineTube->BuildUCS();
	BOOL bStart=TRUE;
	if(pLineTube->m_tJointStart.hLinkObj==handle)
		bStart=TRUE;
	else if(pLineTube->m_tJointEnd.hLinkObj==handle)
		bStart=FALSE;
	else 
		return FALSE;
	SetModified();
	CLDSStdPartModel stdPartModel;
	if(!GetStdPartModel(&stdPartModel))
		return FALSE;
	if(m_iParamType==TYPE_FLG||m_iParamType==TYPE_FLR)
	{
		double fThick=stdPartModel.param.fl_param2.fThick;
		double D=0,H=stdPartModel.param.fl_param2.fH;
		GetDesignItemValue('X',&D);
		D=D*RADTODEG_COEF;
		if(bStart)
		{
			ucs.axis_z=pLineTube->Start()-pLineTube->End();
			normalize(ucs.axis_z);
			ucs.origin=pLineTube->Start()+(pLineTube->startOdd()+H-fThick)*ucs.axis_z;
		}
		else 
		{
			ucs.axis_z=pLineTube->End()-pLineTube->Start();
			normalize(ucs.axis_z);
			ucs.origin=pLineTube->End()+(pLineTube->endOdd()+H-fThick)*ucs.axis_z;
		}
		ucs.axis_x=pLineTube->ucs.axis_x;
		double rot_ang=0;
		long J=0;
		if(J!=0)		//以焊道为基准作为角度定位起始边	//m_bHasWeldRoad&&
			rot_ang=pLineTube->CalWeldRoadAngle();
		if((D+rot_ang)!=0)
			RotateVectorAroundVector(ucs.axis_x,D+rot_ang,ucs.axis_z);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_y);
	}
	else if(m_iParamType==TYPE_FLD||m_iParamType==TYPE_FLP)//对焊法兰/平焊法兰
	{
		double H,H1;
		H=stdPartModel.param.fl_param.H;
		H1=stdPartModel.param.fl_param.H1;
		if(m_iParamType==TYPE_FLP)		
			GetDesignItemValue('O',&H1);	//平焊法兰H1保存在参数'O'中 为钢管端头到法兰底面的距离
		if(bStart)
		{
			ucs.axis_z=pLineTube->End()-pLineTube->Start();
			normalize(ucs.axis_z);
			if(m_iParamType==TYPE_FLD)	//对焊法兰
				ucs.origin=pLineTube->Start()-(pLineTube->startOdd()+H)*ucs.axis_z;
			else
				ucs.origin=pLineTube->Start()-(pLineTube->startOdd()+H1)*ucs.axis_z;
		}
		else
		{
			ucs.axis_z=pLineTube->Start()-pLineTube->End();
			normalize(ucs.axis_z);
			if(m_iParamType==TYPE_FLD)	//对焊法兰
				ucs.origin=pLineTube->End()-(pLineTube->endOdd()+H)*ucs.axis_z;
			else
				ucs.origin=pLineTube->End()-(pLineTube->endOdd()+H1)*ucs.axis_z;
		}
		ucs.origin+=ucs.axis_z*m_fNormOffset;
		ucs.axis_x=pLineTube->ucs.axis_x;
		long J=0;
		double rot_ang=0,X=0;	//用X表示旋转角度 D与标准法兰相关参数同名 暂时如此处理 wht 10-01-21
		GetDesignItemValue('X',&X);
		GetDesignItemValue('J',&J);
		X=X*RADTODEG_COEF;
		if(J!=0)	//以焊道为基准作为角度定位起始边
			rot_ang=pLineTube->CalWeldRoadAngle();
		//调整标准法兰坐标系X轴指向塔心方向 主要目的使法兰中的一个螺栓在铁塔中心线上
		if((X+rot_ang)!=0)
			RotateVectorAroundVector(ucs.axis_x,X+rot_ang,pLineTube->ucs.axis_z);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_y);
	}
	else if(m_iParamType==TYPE_ROLLEND||m_iParamType==TYPE_UEND||m_iParamType==TYPE_XEND)
	{
		double M;
		M=stdPartModel.param.insert_plate_param.M;
		if(bStart)
		{
			ucs.axis_x=pLineTube->Start()-pLineTube->End();
			normalize(ucs.axis_x);
			ucs.origin=pLineTube->Start()+(M-pLineTube->startOdd())*pLineTube->ucs.axis_z;
		}
		else
		{
			ucs.axis_x=pLineTube->End()-pLineTube->Start();
			normalize(ucs.axis_x);
			ucs.origin=pLineTube->End()+(pLineTube->endOdd()-M)*pLineTube->ucs.axis_z;
		}
		if(m_iParamType==TYPE_XEND)
		{
			if(workNorm.UpdateVector(BelongModel()))
				ucs.axis_z=workNorm.vector;
			else
				ucs.axis_z=pLineTube->WorkPlaneNorm();
		}
		else
		{
			ucs.axis_z=pLineTube->WorkPlaneNorm();
			double rot_ang=0,X=0;	//用X表示旋转角度 D与标准法兰相关参数同名 暂时如此处理 wht 10-01-21
			GetDesignItemValue('X',&X);
			X=X*RADTODEG_COEF;
			if(X!=0)
				RotateVectorAroundVector(ucs.axis_z,X,pLineTube->ucs.axis_z);
		}
		normalize(ucs.axis_z);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		normalize(ucs.axis_y);
		ucs.origin+=ucs.axis_z*m_fNormOffset;
	}
	return TRUE;
}
bool CLDSParamPlate::GetStdPartModel(CLDSStdPartModel *pStdPartModel)
{
	if(!m_bStdPart||library==NULL||!library->GetStdPartModel(sPartNo,m_iParamType,pStdPartModel))
		return false;
	else
	{
		if(m_iParamType==TYPE_FLD)
		{	//查找对焊法兰所在钢管厚度
			CLDSLineTube *pLineTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
			if(pLineTube)
			{
				pStdPartModel->param.fl_param.t=pStdPartModel->m_fRelateTubeThick=pLineTube->GetThick();
				//新标准中法兰内径非独立自由参数 wht 12-06-15
				pStdPartModel->param.fl_param.B=pStdPartModel->param.fl_param.A-(pStdPartModel->param.fl_param.S+pStdPartModel->m_fRelateTubeThick)*2;	
				pStdPartModel->param.fl_param.UpdateCodeName();
				sPartNo.Copy(pStdPartModel->param.fl_param.codeName);
				SetModified();
				if(strchr(sPartNo,'?'))
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"the thickness %.0f of 0X%X relative weld flange %s connects main tube, can't find relative group in thickness type of standard flange library,maybe cause error statistics weight",pStdPartModel->param.fl_param.t,handle,(char*)sPartNo);
#else
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X对焊法兰%s连接主管的厚度值%.0f, 未能在标准法兰库的壁厚类别中找到对应分组,可能导致重量统计误差",handle,(char*)sPartNo,pStdPartModel->param.fl_param.t);
#endif
			}
		}
		return true;
	}
}

BOOL CLDSParamPlate::DesignWaterShieldedPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	long h=0;
	double A,S,W,H,K,O,L,E=-1;
	if(!GetDesignItemValue('A',&A,pItemHashTbl))	//两流水槽之间的夹角
		return FALSE;	
	if(!GetDesignItemValue('W',&W,pItemHashTbl))	//外环直径
		return FALSE;
	if(!GetDesignItemValue('H',&H,pItemHashTbl))	//流水槽深度
		return FALSE;
	if(!GetDesignItemValue('K',&K,pItemHashTbl))	//流水槽宽度
		return FALSE;
	if(!GetDesignItemValue('L',&L,pItemHashTbl))	//流水槽长度
		return FALSE;
	if(!GetDesignItemValue('S',&S,pItemHashTbl))	//遮水板相对与基准节点的偏移距离
		return FALSE;
	if(!GetDesignItemValue('E',&E,pItemHashTbl))	//定位基准边 0.基准边 1.焊道线
		return FALSE;
	if(!GetDesignItemValue('O',&O,pItemHashTbl))	//两流水槽角平分线与基准边的偏移角
		return FALSE;
	if(!GetDesignItemValue('B',&h,pItemHashTbl))	//基准节点句柄
		return FALSE;
	CLDSNode *pBaseNode=BelongModel()->FromNodeHandle(h);
	h=0;
	if(!GetDesignItemValue('C',&h,pItemHashTbl))	//基准钢管句柄
		return FALSE;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(h,CLS_LINETUBE);
	if(pBaseNode==NULL||pDatumTube==NULL)
		return FALSE;
	
	double offset_angle=O*RADTODEG_COEF;		//两流水槽角平分线与基准边之间的偏移角
	double water_slot_angle=A*RADTODEG_COEF*0.5;//两流水槽之间夹角的一半
	f3dPoint axis_z=pDatumTube->End()-pDatumTube->Start();
	//两流水槽角分线所在方向
	f3dPoint datum_axis=pDatumTube->ucs.axis_x;	
	if(E==1)	//以焊道线为基准
		offset_angle +=pDatumTube->CalWeldRoadAngle();
	RotateVectorAroundVector(datum_axis,offset_angle,axis_z);
	//计算坐标系
	UCS_STRU plate_ucs;
	f3dLine baseline(pDatumTube->pStart->Position(true),pDatumTube->pEnd->Position(true));
	if(pBaseNode==pDatumTube->pStart)
	{
		plate_ucs.axis_z=pDatumTube->Start()-pDatumTube->End();
		normalize(plate_ucs.axis_z);
		plate_ucs.origin=pDatumTube->Start();
	}
	else if(pBaseNode==pDatumTube->pEnd)
	{
		plate_ucs.axis_z=pDatumTube->End()-pDatumTube->Start();
		normalize(plate_ucs.axis_z);
		plate_ucs.origin=pDatumTube->End();
	}
	else
	{
		plate_ucs.axis_z=pDatumTube->End()-pDatumTube->Start();
		normalize(plate_ucs.axis_z);
		SnapPerp(&plate_ucs.origin,baseline,pBaseNode->Position(true));
	}
	//以两流水槽角平分线所在方向为X轴方向
	plate_ucs.axis_x=datum_axis;
	plate_ucs.axis_y=plate_ucs.axis_z^plate_ucs.axis_x;
	normalize(plate_ucs.axis_x);
	normalize(plate_ucs.axis_y);
	if(ucs.axis_z!=plate_ucs.axis_z)
	{
		f3dPoint rot_axis_vec=plate_ucs.axis_z^ucs.axis_z;
		double rot_ang=ACOS(plate_ucs.axis_z,ucs.axis_z);
		RotateVectorAroundVector(plate_ucs.axis_x,rot_ang,rot_axis_vec);
		RotateVectorAroundVector(plate_ucs.axis_y,rot_ang,rot_axis_vec);
		plate_ucs.axis_z=plate_ucs.axis_x^plate_ucs.axis_y;	//满足右手螺旋法则
	}
	plate_ucs.origin+=plate_ucs.axis_z*S;
	ucs=plate_ucs;
	//
	BOOL bHasWaterSolt=TRUE;
	if(fabs(A)<EPS||fabs(H)<EPS||fabs(K)<EPS||fabs(L)<EPS)
		bHasWaterSolt=FALSE;	//两流水槽夹角、长度、深度或宽度为0时表示无流水槽 wht 11-07-01
	if(bHasWaterSolt)
	{	//流水槽所在方向
		f3dPoint water_slot_axis1=datum_axis;	//流水槽1所在方向
		f3dPoint water_slot_axis2=datum_axis;	//流水槽2所在方向
		RotateVectorAroundVector(water_slot_axis1,water_slot_angle,axis_z);
		RotateVectorAroundVector(water_slot_axis2,-water_slot_angle,axis_z);
		//将流水槽所在方向转换到遮水板坐标系下的相对方向
		f3dPoint axis_up=water_slot_axis1,axis_down=water_slot_axis2;
		vector_trans(axis_up,ucs,FALSE);
		vector_trans(axis_down,ucs,FALSE);
		if(axis_up.y<axis_down.y)
		{	//确保axis_up在Y轴正方向一侧
			f3dPoint temp_axis=axis_up;
			axis_up=axis_down;
			axis_down=temp_axis;
		}
		double R=W*0.5;
		//两流水槽与遮水板外圆环的四个交点
		f3dPoint norm_up(-axis_up.y,axis_up.x);	//Y正方向流水槽法线
		f3dPoint norm_down(axis_down.y,-axis_down.x);//Y负方向流水槽法线
		f2dCircle cir(R,f2dPoint(0,0));	//遮水板外圆环
		f3dPoint vertex_up_up,vertex_up_down,vertex_down_up,vertex_down_down;
		f3dPoint line_pick;
		f2dLine line;
		//Y正方向流水槽外侧与遮水板外圆环的交点 
		line_pick=f3dPoint(0,0);
		line.startPt.Set(line_pick.x,line_pick.y);
		line_pick=line_pick+axis_up*W;
		line.endPt.Set(line_pick.x,line_pick.y);
		Int2dlc(line,cir,vertex_up_up.x,vertex_up_up.y,vertex_up_down.x,vertex_up_down.y);
		if(vertex_up_up.y<vertex_up_down.y)
			vertex_up_up=vertex_up_down;
		//Y正方向流水槽内侧与遮水板外圆环的交点
		line_pick=f3dPoint(0,0)-norm_up*K;
		line.startPt.Set(line_pick.x,line_pick.y);
		line_pick=line_pick+axis_up*W;
		line.endPt.Set(line_pick.x,line_pick.y);
		Int2dlc(line,cir,vertex_up_down.x,vertex_up_down.y,vertex_down_down.x,vertex_down_down.y);
		if(vertex_up_down.y<vertex_down_down.y)
			vertex_up_down=vertex_down_down;
		//Y负方向流水槽与遮水板的两个交点
		vertex_down_up.Set(vertex_up_down.x,-vertex_up_down.y);
		vertex_down_down.Set(vertex_up_up.x,-vertex_up_up.y);
		double slot_angle=atan(vertex_up_down.y/vertex_up_down.x);
		//if(slot_angle>water_slot_angle)
		//	return FALSE;	
		double arc_angle=(360-A)*RADTODEG_COEF;//遮水板均为整圆
		int i=0;
		if(slot_angle<0)
			slot_angle*=-1.0;
		arc_angle*=0.25;
		PROFILE_VERTEX vertex;
		vertex.type=1;
		vertex.center=ucs.origin;
		vertex.sector_angle = fabs(water_slot_angle-slot_angle);	
		vertex_list.Empty();
		vertex.vertex=vertex_up_down;
		vertex_list.append(vertex);
		for(i=0;i<=4;i++)
		{	//四等分不在两流水槽之间的圆弧
			vertex.sector_angle=arc_angle;
			if(i==4)
				vertex.sector_angle=water_slot_angle-slot_angle;
			vertex.vertex.Set(R*cos((i*arc_angle)+water_slot_angle),R*sin((i*arc_angle)+water_slot_angle),0);
			vertex_list.append(vertex);
		}
		vertex.sector_angle = slot_angle*2;	
		vertex.vertex=vertex_down_up;
		vertex_list.append(vertex);
	}
	else
	{
		double RO=W*0.5;
		PROFILE_VERTEX vertex;
		vertex.type=1;
		vertex.center=ucs.origin;
		vertex.sector_angle = Pi*0.5;	//四等分圆弧
		vertex_list.Empty();
		vertex.vertex.Set(RO,0,0);
		vertex_list.append(vertex);
		vertex.vertex.Set(0,RO,0);
		vertex_list.append(vertex);
		vertex.vertex.Set(-RO,0,0);
		vertex_list.append(vertex);
		vertex.vertex.Set(0,-RO,0);
		vertex_list.append(vertex);
	}
	return TRUE;
}

//肋板(角肋板，槽肋板，角钢肋板，钢板肋板，钢管肋板，钢管夹板) wht 11-03-21
//角钢肋板: A.角钢+基准节点
//钢管肋板:	B.钢管+基准节点
//钢板肋板: C.钢板+基准螺栓+方向构件
//  角肋板: D.钢板+辅助钢板+定位螺栓
//		    D.钢板+辅助钢板+相对坐标定位，相对方向
//			E.钢管+辅助钢板(旧:钢管肘板)
//			F.钢管+钢管(旧:钢管夹板)	
//  槽肋板: G.钢板+2个辅助钢板+定位螺栓
//			H.钢管+2个辅助钢板
//此函数计算具有内圆弧r的角钢内皮45°紧贴一厚为thick的钢板时，钢板边距离角钢内直角楞线的距离
static const double cos45=0.70710678118654752440084436210485;	//2的平方根的一半
static double CalJgROffset(double r,double thick)
{
	if(thick>=r*cos45*2)	//紧贴在内圆弧外的角钢内皮上
		return thick/2;
	else
		return r*(2*cos45-sqrt(1-thick*thick/(r*r*4)));
}
BOOL CLDSParamPlate::DesignRibPlate(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	long hDatumPart=0,hNode=0,hAssPart1=0,hAssPart2=0,work_norm_style=0;
	RIBPROFILE_PARA profile;
	profile.ClearZero();
	double fOffsetY=0;
	if(!GetDesignItemValue('A',&hDatumPart,pItemHashTbl))//基准构件
		return FALSE;
	GetDesignItemValue('B',&hNode,pItemHashTbl);		//基准节点
	GetDesignItemValue('C',&hAssPart1,pItemHashTbl);	//辅助构件1
	GetDesignItemValue('D',&hAssPart2,pItemHashTbl);	//辅助构件2
	CLDSNode *pDatumNode=NULL;
	CLDSPart *pDatumPart=NULL,*pAssistantPart1=NULL,*pAssistantPart2=NULL;
	pDatumNode=BelongModel()->FromNodeHandle(hNode);
	pDatumPart=BelongModel()->FromPartHandle(abs(hDatumPart),CLS_LINETUBE,CLS_PLATE,CLS_PARAMPLATE);
	pAssistantPart1=BelongModel()->FromPartHandle(abs(hAssPart1),CLS_PLATE,CLS_PARAMPLATE);
	pAssistantPart2=BelongModel()->FromPartHandle(hAssPart2,CLS_PLATE,CLS_PARAMPLATE);
	if(pDatumPart==NULL)
		return FALSE;
	if(!GetDesignItemValue('E',&profile.common.fBottomWidth,pItemHashTbl))	//底部宽度
		return FALSE;
	if(!GetDesignItemValue('F',&profile.common.fTopWidth,pItemHashTbl))	//顶部宽度
		return FALSE;
	if(!GetDesignItemValue('G',&profile.common.fInnerLen,pItemHashTbl))	//内侧长度
		return FALSE;
	if(!GetDesignItemValue('H',&profile.common.fOuterLen,pItemHashTbl))	//外侧长度
		return FALSE;
	if(!GetDesignItemValue('I',&profile.common.fCutAngleLen,pItemHashTbl))	//切角宽度
		profile.common.fCutAngleLen=0;

	if(pAssistantPart1&&pAssistantPart1->IsPlate()&&pAssistantPart2&&pAssistantPart2->IsPlate())
	{	//纠正坐标系Y轴方向,考虑法线偏移量 wht 11-05-24
		f3dPoint pt1=GetPlateDatumFacePickPt(pAssistantPart1,FALSE);
		f3dPoint pt2=GetPlateDatumFacePickPt(pAssistantPart2,FALSE);
		f3dPoint verfiy_vec=pt2-pt1;
		if(verfiy_vec*pAssistantPart1->ucs.axis_z>0)
			hAssPart1=abs(hAssPart1);	//肋板在辅助钢板顶面一侧
		else 
			hAssPart1=-abs(hAssPart1);	//肋板在辅助钢板底面一侧
		if(verfiy_vec*pAssistantPart2->ucs.axis_z<0)
			hAssPart2=abs(hAssPart2);
		else 
			hAssPart2=-abs(hAssPart2);
	}
	f3dPoint vertice;
	if(pDatumPart->GetClassTypeId()==CLS_LINETUBE)
	{	//基准构件为钢管
		double fOffsetAngle=0,fBottomOuterR=-1,fTopOuterR=-1;
		if(pDatumNode==NULL&&pAssistantPart1==NULL&&pAssistantPart2==NULL)
			return FALSE;
		if(!GetDesignItemValue('J',&work_norm_style,pItemHashTbl))	//0.基准边 1.焊道线 2.指定方向
			return FALSE;
		if(!GetDesignItemValue('K',&fOffsetAngle,pItemHashTbl))	//偏移角度
			return FALSE;
		if(!GetDesignItemValue('R',&fBottomOuterR,pItemHashTbl))//底部外环半径
			return FALSE;
		if(pAssistantPart2&&pAssistantPart2->IsPlate())
		{	//基准构件为钢管且存在第二块辅助钢板，此时存在顶部外环半径
			if(!GetDesignItemValue('r',&fTopOuterR,pItemHashTbl))//顶部外环半径
				return FALSE;
		}
		fOffsetAngle*=RADTODEG_COEF;	//偏移角度装换为弧度制
		CLDSLineTube *pDatumTube=(CLDSLineTube*)pDatumPart;
		pDatumTube->BuildUCS();
		f3dPoint line_vec=pDatumTube->ucs.axis_z;
		if(work_norm_style==2)
		{	//自定义工作法线方向
			if(!GetDesignItemValue(KEY2C("Nx"),&ucs.axis_z.x,pItemHashTbl))	//法线X分量
				return FALSE;
			if(!GetDesignItemValue(KEY2C("Ny"),&ucs.axis_z.y,pItemHashTbl))	//法线Y分量
				return FALSE;
			if(!GetDesignItemValue(KEY2C("Nz"),&ucs.axis_z.z,pItemHashTbl))	//法线Z分量
				return FALSE;
			if(!GetDesignItemValue(KEY2C("Wx"),&ucs.axis_x.x,pItemHashTbl))	//展开方向(X轴)X分量
				return FALSE;
			if(!GetDesignItemValue(KEY2C("Wy"),&ucs.axis_x.y,pItemHashTbl))	//展开方向(X轴)Y分量
				return FALSE;
			if(!GetDesignItemValue(KEY2C("Wz"),&ucs.axis_x.z,pItemHashTbl))	//展开方向(X轴)Z分量
				return FALSE;
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			ucs.axis_x=ucs.axis_y^ucs.axis_z;//通过Ｙ轴校正Ｘ轴与Ｚ轴的正交垂直
			normalize(ucs.axis_x);
			normalize(ucs.axis_y);
			normalize(ucs.axis_z);
		}
		else
		{	//通过基准边旋转角度确定工作法线(Y轴沿钢管轴向)
			double weldRoadAngle=0;
			if(pDatumTube->m_bHasWeldRoad&&work_norm_style==1)
				weldRoadAngle=pDatumTube->CalWeldRoadAngle();
			ucs.axis_x.Set(cos(weldRoadAngle),sin(weldRoadAngle));
			ucs.axis_z.Set(-ucs.axis_x.y,ucs.axis_x.x);
			vector_trans(ucs.axis_x,pDatumTube->ucs,TRUE);
			vector_trans(ucs.axis_z,pDatumTube->ucs,TRUE);
		}
		if(pAssistantPart1&&pAssistantPart1->IsPlate())
		{	
			f3dPoint tube_vec;
			double radiusRatio=0;
			BOOL bVerticalPlate=FALSE;	//肋板垂直与辅助钢板1且自动正位
			if(pAssistantPart2==NULL)
			{
				tube_vec=pDatumTube->Start()-pDatumTube->End();
				normalize(tube_vec);
				radiusRatio=fabs(tube_vec*pAssistantPart1->ucs.axis_z);
				GetDesignItemValue('S',(long*)&bVerticalPlate,pItemHashTbl);
				if(radiusRatio<EPS2||radiusRatio>EPS_COS)
					bVerticalPlate=FALSE;
			}
			if(bVerticalPlate)	//竖直肋板
				line_vec=pAssistantPart1->ucs.axis_z;
			double cosa=line_vec*pAssistantPart1->ucs.axis_z;	//钢管延伸方向与辅助钢板Z
			if((hAssPart1>0&&cosa<0)||(hAssPart1<0&&cosa>0))
				line_vec*=-1.0;
			//1、计算肋板坐标系方向
			f3dPoint inters_vec=ucs.axis_z^pAssistantPart1->ucs.axis_z;
			normalize(inters_vec);
			normalize(ucs.axis_x);
			if(ucs.axis_x*inters_vec<0)	//修正最终X轴方向的正负方向
				ucs.axis_x=-inters_vec;
			else						//将肘板最终X轴方向赋值给肘板装配坐标系
				ucs.axis_x=inters_vec;
			//根据偏移角度调整基准方向
			if(fabs(fOffsetAngle)>0)
				RotateVectorAroundVector(ucs.axis_x,fOffsetAngle,pAssistantPart1->ucs.axis_z);
			ucs.axis_z = ucs.axis_x^line_vec;
			normalize(ucs.axis_z);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
			//2、根据直线与圆柱的交点计算坐标系原点，保证原点在管壁上
			double fRadius=pDatumTube->GetDiameter()*0.5;
			f3dPoint inters1,inters2,line_pick,face_pick,temp_vec;
			face_pick=GetPlateDatumFacePickPt(pAssistantPart1,hAssPart1>0);
			Int3dlf(line_pick,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pAssistantPart1->ucs.axis_z);
			if(Int3dlc(&inters1,&inters2,line_pick,ucs.axis_x,pDatumTube->Start(),pDatumTube->End(),fRadius,fRadius)!=2)
			{
				logerr.Log("肋板坐标原点计算失败！");
				return FALSE;
			}
			temp_vec=inters1-inters2;
			normalize(temp_vec);
			if(temp_vec*ucs.axis_x>0)
				ucs.origin=inters1;
			else
				ucs.origin=inters2;
			//基准点，钢管轴线与辅助钢板所在平面的交点
			f3dPoint top_origin,top_datum_pos,bottom_datum_pos;
			Int3dlf(bottom_datum_pos,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pAssistantPart1->ucs.axis_z);
			if(fBottomOuterR>0)
				profile.common.fBottomWidth=fBottomOuterR-DISTANCE(ucs.origin,bottom_datum_pos);
			//
			if(pAssistantPart2&&pAssistantPart2->IsPlate())
			{	//存在两块辅助钢板时系统自动计算肋板长度以及顶部底部高度
				face_pick=GetPlateDatumFacePickPt(pAssistantPart2,ucs.axis_y*pAssistantPart2->ucs.axis_z<0);
				Int3dlf(top_origin,line_pick,pDatumTube->ucs.axis_z,face_pick,pAssistantPart2->ucs.axis_z);
				profile.common.fInnerLen=DISTANCE(ucs.origin,top_origin);
				Int3dlf(top_datum_pos,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pAssistantPart2->ucs.axis_z);
				if(fTopOuterR>0)
					profile.common.fTopWidth=fTopOuterR-DISTANCE(top_origin,top_datum_pos);
				//肋板基准面与辅助钢板2基准面求交线
				f3dPoint line_pick2,inters_vec2;
				Int3d2f(&line_pick2,&inters_vec2,ucs.origin,ucs.axis_z,face_pick,pAssistantPart2->ucs.axis_z);
				if(inters_vec2*ucs.axis_x<0)
					inters_vec2*=-1.0;
				normalize(inters_vec2);
				f3dPoint assistant_plate2_pt=top_origin+inters_vec2*profile.common.fTopWidth; //与关联钢板2相关的外侧顶点
				coord_trans(top_origin,ucs,FALSE);
				coord_trans(assistant_plate2_pt,ucs,FALSE);
				top_origin.z=assistant_plate2_pt.z=0;
				//
				vector_trans(line_vec,ucs,FALSE);
				double k=0;
				if(fabs(line_vec.y)>eps)	//防止line_vec.y=0 
					k=line_vec.x/line_vec.y;
				vertice.Set(profile.common.fCutAngleLen,0,0);
				PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE; 
				vertice.Set(profile.common.fBottomWidth,0,0);
				vertex_list.append(vertice);
				if(fabs(assistant_plate2_pt.x-profile.common.fBottomWidth)>EPS_COS)
				{
					if(assistant_plate2_pt.x>profile.common.fBottomWidth)		//顶部宽度大于底部宽度
					{
						f3dPoint axis_z=pAssistantPart2->ucs.axis_z;
						if(axis_z*ucs.axis_y>0)
							axis_z*=-1.0;
						vector_trans(axis_z,ucs,FALSE);
						axis_z.z=0;
						vertice=assistant_plate2_pt+profile.common.fOuterLen*axis_z;
					}
					else if(assistant_plate2_pt.x<profile.common.fBottomWidth)	//顶部宽度小于底部宽度
						vertice.Set(profile.common.fBottomWidth,profile.common.fOuterLen,0);
					vertex_list.append(vertice);
				}
				pVertex=vertex_list.append(assistant_plate2_pt);
				pVertex->m_bWeldEdge=TRUE;
				pVertex=vertex_list.append(top_origin);
				pVertex->m_bWeldEdge=TRUE;
				if(fabs(profile.common.fCutAngleLen)>eps)
				{
					vertice.Set(profile.common.fCutAngleLen*k,profile.common.fCutAngleLen,0);
					vertex_list.append(vertice);
				}
			}
			else if(bVerticalPlate) //钢管+辅助钢板
			{	//竖直且带自动正位的肘板	
				double radius=pDatumTube->GetDiameter()*0.5;
				f3dPoint datum_point,line_pick,inters1,inters2;
				line_pick.Set(0,profile.common.fInnerLen,0);
				coord_trans(line_pick,ucs,TRUE);
				line_vec=ucs.axis_x;
				if(Int3dlc(&inters1,&inters2,line_pick,line_vec,pDatumTube->Start(),pDatumTube->End(),radius,radius)>0)
				{
					f3dPoint axis_x=inters2-inters1;
					normalize(axis_x);
					if(axis_x*ucs.axis_x>0)
						datum_point=inters2;
					else 
						datum_point=inters1;
				}
				else //无交点时取直线的平行线与椭圆的切点作为交点	
				{	
					double majorAxisLen=radius/radiusRatio;
					UCS_STRU ellipse_ucs;	//椭圆坐标系
					f3dPoint face_pick=pAssistantPart1->ucs.origin;
					if(pAssistantPart1->GetClassTypeId()==CLS_PLATE)
						face_pick+=((CLDSPlate*)pAssistantPart1)->m_fNormOffset*pAssistantPart1->ucs.axis_z;
					Int3dlf(ellipse_ucs.origin,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pAssistantPart1->ucs.axis_z);
					if(tube_vec*ucs.axis_y<0)
						tube_vec*=-1.0;
					ellipse_ucs.origin+=(tube_vec*profile.common.fInnerLen/radiusRatio);
					ellipse_ucs.axis_z=pAssistantPart1->ucs.axis_z;
					ellipse_ucs.axis_y=tube_vec^pAssistantPart1->ucs.axis_z;
					normalize(ellipse_ucs.axis_y);
					ellipse_ucs.axis_x=ellipse_ucs.axis_y^ellipse_ucs.axis_z;
					normalize(ellipse_ucs.axis_x);
					f3dPoint tangPt1,tangPt2;
					f3dPoint vec=ucs.axis_x;
					vector_trans(vec,ellipse_ucs,FALSE);
					if(Tang2dle(vec,majorAxisLen,radius,tangPt1,tangPt2))
					{	//存在合理切点
						coord_trans(tangPt1,ellipse_ucs,TRUE);
						coord_trans(tangPt2,ellipse_ucs,TRUE);
						datum_point.Set(0,profile.common.fInnerLen,0);
						coord_trans(datum_point,ucs,TRUE);
						if(DISTANCE(datum_point,tangPt1)<DISTANCE(datum_point,tangPt2))
							datum_point=tangPt1;
						else 
							datum_point=tangPt2;
						//肘板坐标系需要调整 Y轴方向保持不变
						f3dPoint axis_y=datum_point-ucs.origin;
						normalize(axis_y);
						f3dPoint axis_z=axis_y^ucs.axis_y;
						if(ucs.axis_z*axis_z<0)
							ucs.axis_z=-1.0*axis_z;
						else
							ucs.axis_z=axis_z;
						normalize(ucs.axis_z);
						ucs.axis_x=ucs.axis_y^ucs.axis_z;
						normalize(ucs.axis_x);
					}
					else //不存在合理切点
					{
						datum_point.Set(0,profile.common.fInnerLen,0);
						coord_trans(datum_point,ucs,TRUE);
					}
				}
				coord_trans(datum_point,ucs,FALSE);
				datum_point.z=0;
				vertice.Set(profile.common.fCutAngleLen,0,0);
				PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE; 
				vertice.Set(profile.common.fBottomWidth,0,0);
				vertex_list.append(vertice);
				if((profile.common.fTopWidth+datum_point.x)<profile.common.fBottomWidth)	//避免出现凹形板
				{
					vertice.Set(profile.common.fBottomWidth,profile.common.fOuterLen,0);
					vertex_list.append(vertice);
				}
				vertice.Set(profile.common.fTopWidth+datum_point.x,profile.common.fInnerLen,0);
				vertex_list.append(vertice);
				
				f3dLine line(datum_point,f3dPoint(0,0,0));
				coord_trans(line.startPt,ucs,TRUE);
				coord_trans(line.endPt,ucs,TRUE);
				line_vec=line.startPt-line.endPt;
				normalize(line_vec);
				if(Int3dlc(&inters1,&inters2,line.startPt,line_vec,pDatumTube->Start(),pDatumTube->End(),radius,radius)>0)
				{	//圆弧边
					pVertex=vertex_list.append(datum_point);
					pVertex->type=3;
					pVertex->column_norm=pDatumTube->End()-pDatumTube->Start();
					normalize(pVertex->column_norm);
					pVertex->radius=-pDatumTube->GetDiameter()*0.5;
					pVertex->center=pDatumTube->Start();
					pVertex->m_bWeldEdge=TRUE;
					if(fabs(profile.common.fCutAngleLen)>eps)
					{
						f3dPoint e_vec(1,-1,0);
						f3dPoint startPt(profile.common.fCutAngleLen,0,0);
						coord_trans(startPt,ucs,TRUE);
						vector_trans(e_vec,ucs,TRUE);
						if(Int3dlc(&inters1,&inters2,startPt,e_vec,pDatumTube->Start(),pDatumTube->End(),radius,radius)>0)
						{
							f3dPoint inters_pos;
							if(DISTANCE(inters1,startPt)<DISTANCE(inters2,startPt))
								inters_pos=inters1;
							else
								inters_pos=inters2;
							coord_trans(inters_pos,ucs,FALSE);
							//应该确保Y坐标为E 暂时保证该点位于圆弧上
							//vertice.Set(E*datum_point.x/datum_point.y,E,0);
							vertice.Set(inters_pos.x,inters_pos.y);
							vertex_list.append(vertice);
						}
					}
				}
				else
				{	//无圆弧边
					vertice=datum_point;
					pVertex=vertex_list.append(vertice);
					pVertex->m_bWeldEdge=TRUE;
					if(fabs(profile.common.fCutAngleLen)>eps)
					{
						double k=0;
						if(fabs(datum_point.y)>eps)
							k=datum_point.x/datum_point.y;
						vertice.Set(profile.common.fCutAngleLen*k,profile.common.fCutAngleLen,0);
						vertex_list.append(vertice);
					}
				}
			}
			else
			{	//沿钢管方向的肘板
				vector_trans(line_vec,ucs,FALSE);
				double k=0;
				if(fabs(line_vec.y)>eps)	//防止line_vec.y=0 
					k=line_vec.x/line_vec.y;
				vertice.Set(profile.common.fCutAngleLen,0,0);
				PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE; 
				vertice.Set(profile.common.fBottomWidth,0,0);
				vertex_list.append(vertice);
				if(fabs(profile.common.fTopWidth-profile.common.fBottomWidth)>EPS)
				{
					if(profile.common.fTopWidth>profile.common.fBottomWidth)		//顶部宽度大于底部宽度
						vertice.Set(profile.common.fTopWidth,profile.common.fInnerLen-profile.common.fOuterLen,0);
					else if(profile.common.fTopWidth<profile.common.fBottomWidth)	//顶部宽度小于底部宽度
						vertice.Set(profile.common.fBottomWidth,profile.common.fOuterLen,0);
					vertex_list.append(vertice);
				}
				vertice.Set(profile.common.fTopWidth+profile.common.fInnerLen*k,profile.common.fInnerLen,0);
				vertex_list.append(vertice);
				vertice.Set(profile.common.fInnerLen*k,profile.common.fInnerLen,0);
				pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE;
				if(fabs(profile.common.fCutAngleLen)>eps)
				{
					vertice.Set(profile.common.fCutAngleLen*k,profile.common.fCutAngleLen,0);
					vertex_list.append(vertice);
				}
				double MF=0,MG=0,MH=0;
				GetDesignItemValue(KEY2C("MF"),&MF,pItemHashTbl);		//引流孔到底边距离参数F
				GetDesignItemValue(KEY2C("MG"),&MG,pItemHashTbl);		//引流孔到侧边距离参数G
				if(GetDesignItemValue(KEY2C("MH"),&MH,pItemHashTbl))	//引流孔孔径
				{
					CLsRef* pLsRef=GetFirstLsRef();
					CLDSBolt* pVirtualBolt=NULL;
					if(pLsRef==NULL)
						pVirtualBolt=(CLDSBolt*)BelongModel()->AppendPart(CLS_BOLT);
					else
						pVirtualBolt=pLsRef->GetLsPtr();
					pVirtualBolt->m_bVirtualPart=TRUE;
					pVirtualBolt->des_work_norm.norm_style=3;
					pVirtualBolt->des_work_norm.hVicePart=this->handle;
					pVirtualBolt->des_base_pos.cLocationStyle=1;	//LOCAL_COORD
					pVirtualBolt->des_base_pos.hPart=this->handle;
					pVirtualBolt->des_base_pos.R=0;
					pVirtualBolt->set_d(ftoi(MH));	//孔径
					pVirtualBolt->hole_d_increment=MH-pVirtualBolt->get_d();
					pVirtualBolt->des_base_pos.len_offset_dist=MF;	//ucs.origin.x
					//TODO:ucs.origin.y计算还有问题，应参照角钢楞点计算代码 wjh-2015.8.8
					pVirtualBolt->des_base_pos.wing_offset_dist=MG;	//ucs.origin.y
					pVirtualBolt->m_cFuncType=CLDSBolt::FUNC_WATERHOLE;
					if(pLsRef==NULL)
						pLsRef=AppendLsRef(pVirtualBolt->GetLsRef(),FALSE);
				}
			}
		}
		else if(pDatumNode)
		{	//钢管上无关联钢板的肋板以钢管始->终方向为X轴正向，翼展方向为Y轴正向(出于操作习惯考虑)
			SnapPerp(&ucs.origin,pDatumTube->Start(),pDatumTube->End(),pDatumNode->Position(true));
			//调换X和Y轴方向
			f3dPoint temp_vec=ucs.axis_x;
			ucs.axis_x=-ucs.axis_y;
			ucs.axis_y=temp_vec;
			if(work_norm_style==2)
			{
				double fOffsetM=0,fOffsetX=0,fMaxWidth=0;
				GetDesignItemValue(KEY2C("DM"),&fOffsetM,pItemHashTbl);		
				GetDesignItemValue(KEY2C("DX"),&fOffsetX,pItemHashTbl);		
				f3dPoint axis_vec=pDatumTube->End()-pDatumTube->Start();
				normalize(axis_vec);	//默认始->终为轴向偏移正方向
				ucs.origin+=axis_vec*fOffsetM;

				f3dPoint line_pick,inner_arc_start,inner_arc_end,arc_start,arc_end,origin;
				//原点坐标
				double radius = pDatumTube->GetDiameter()*0.5;
				if(fabs(fOffsetX)>0)
					ucs.origin+=ucs.axis_x*fOffsetX;
				if(Int3dlc(&inner_arc_start,&inner_arc_end,ucs.origin,ucs.axis_y,pDatumTube->Start(),pDatumTube->End(),radius,radius)!=2)
					return FALSE;
				double complement=(inner_arc_start-ucs.origin)*ucs.axis_y;	//高度补偿值
				if(complement>0)
					ucs.origin=inner_arc_start;
				else
				{
					complement=(inner_arc_end-ucs.origin)*ucs.axis_y;
					ucs.origin=inner_arc_end;
				}
				double cosa=fabs(ucs.axis_z*axis_vec);
				if(cosa>0)
					complement=radius/cosa-complement;
				else
					complement=0;
				arc_start=ucs.origin-ucs.axis_x*profile.common.fBottomWidth;
				if(Int3dlc(&inner_arc_start,&inner_arc_end,arc_start,ucs.axis_y,pDatumTube->Start(),pDatumTube->End(),radius,radius)!=2)
					return FALSE;
				if((inner_arc_start-arc_start)*ucs.axis_y>0)
					arc_start=inner_arc_start;
				else
					arc_start=inner_arc_end;
				//第一个顶点,圆弧终点（原点）
				vertice.feature=1;
				vertice.Set();
				vertex_list.append(vertice);
				//第二个顶点
				//vertice.x+=fInnerLen;
				vertice.Set(0,profile.common.fInnerLen+complement,0);
				vertex_list.append(vertice);
				//第三个顶点
				//vertice.y+=fBottomWidth;
				vertice.Set(-profile.common.fBottomWidth,profile.common.fInnerLen+complement,0);
				vertex_list.append(vertice);
				//第四个顶点,圆弧起点
				coord_trans(arc_start,ucs,FALSE);
				arc_start.z=0;
				arc_start.feature=1;
				PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(arc_start));
				if(fabs(ucs.axis_z*pDatumTube->ucs.axis_z)>EPS_COS)
				{	//圆弧
					pVertex->type=2;
					pVertex->center.Set(0,-100,0);	//给定圆弧拾取点，防止拾取点与圆弧的始终端点重合，导致无法确定圆心位置
					pVertex->radius=-radius;
				}
				else
				{	//椭圆弧
					pVertex->type=3;
					f3dPoint center;
					SnapPerp(&center,pDatumTube->Start(),pDatumTube->End(),ucs.origin);
					pVertex->center=center;
					if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
						pVertex->column_norm=pDatumTube->ucs.axis_z;
					else
						pVertex->column_norm=-pDatumTube->ucs.axis_z;
					pVertex->radius=-radius;
				}
				pVertex->m_bWeldEdge=TRUE;
			}
			else
			{
				ucs.origin+=ucs.axis_y*pDatumTube->GetDiameter()*0.5;
				/*ucs.axis_x=ucs.axis_y^ucs.axis_z;
				normalize(ucs.axis_x);
				ucs.axis_z=ucs.axis_x^ucs.axis_y;
				normalize(ucs.axis_z);*/
				if(hDatumPart<0)
				{	//X轴与钢管轴向相反
					ucs.axis_x*=-1.0;
					ucs.axis_z=ucs.axis_x^ucs.axis_y;
					normalize(ucs.axis_z);
				}
				double fOffsetM=0;
				GetDesignItemValue(KEY2C("DM"),&fOffsetM,pItemHashTbl);		
				ucs.origin+=line_vec*fOffsetM;	//Y方向偏移量
				//钢管+节点
				vertice.Set(0,0,0);
				vertex_list.append(vertice);
				vertice.Set(0,profile.common.fInnerLen,0);
				vertex_list.append(vertice);
				vertice.Set(-profile.common.fBottomWidth,profile.common.fInnerLen,0);
				vertex_list.append(vertice);
				vertice.Set(-profile.common.fBottomWidth,0,0);
				PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE;
			}
		}
	}
	else if(pDatumPart->IsPlate())
	{	//基准构件为钢板
		if(pAssistantPart1&&!pAssistantPart1->IsPlate())
		{
			if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("Ancillary part 1 must be plate！");
#else
				log2file->Log("辅助构件1必须为钢板！");
#endif
			return FALSE;	//辅助构件1不是钢板
		}
		if(pAssistantPart2&&!pAssistantPart2->IsPlate())
		{
			if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("Ancillary part 2 must be palte！");
#else
				log2file->Log("辅助构件2必须为钢板！");
#endif
			return FALSE;	//辅助构件2不是钢板
		}
		CLDSGeneralPlate* pDatumPlate =(CLDSGeneralPlate*)pDatumPart;
		CLDSGeneralPlate* pAssisPlate1=(CLDSGeneralPlate*)pAssistantPart1;
		CLDSGeneralPlate* pAssisPlate2=(CLDSGeneralPlate*)pAssistantPart2;
		long hBolt=0,hVecPart=0;
		GetDesignItemValue('O',&hBolt,pItemHashTbl);		//基点定位螺栓
		GetDesignItemValue('V',&hVecPart,pItemHashTbl);		//X轴或Y轴方向基准杆件
		double fOffsetX=0,fOffsetZ=0;
		GetDesignItemValue('X',&fOffsetX,pItemHashTbl);		//X方向偏移量
		GetDesignItemValue('Z',&fOffsetZ,pItemHashTbl);		//Z方向偏移量
		CLDSBolt *pBolt=(CLDSBolt*)BelongModel()->FromPartHandle(hBolt,CLS_BOLT);
		CLDSPart *pVecPart=(CLDSPart*)BelongModel()->FromPartHandle(abs(hVecPart));
		if(pVecPart&&!pVecPart->IsLinePart()&&pVecPart->GetClassTypeId()!=CLS_BOLT)
		{
			if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("Direction ancillary part must be rod or bolt(on datum plate or ancillary plate)！");
#else
				log2file->Log("方向辅助构件必须为杆件或(基板或辅助钢板上的)螺栓！");
#endif
			return FALSE;
		}
		BOOL bSpecOrg=true,bSpecAxisX=true,bSpecAxisY=false;
		//1.计算肋板坐标系原点位置
		double fRayAngle=0,fRayDist=0;	//射线角度为角度制
		BOOL bSpecRayAngle=GetDesignItemValue(KEY2C("Ra"),&fRayAngle,pItemHashTbl);
		BOOL bSpecRayDist=GetDesignItemValue(KEY2C("Rd"),&fRayDist,pItemHashTbl);
		f3dPoint ray_vec(1,0,0);				//底板坐标系下的相对方向
		if(bSpecRayAngle)
		{	//指定射线角
			RotateVectorAroundVector(ray_vec,fRayAngle*RADTODEG_COEF,f3dPoint(0,0,1));
			normalize(ray_vec);
			if(bSpecRayDist&&pAssisPlate1&&pAssisPlate2)
			{	//根据射线角、偏移距离计算肋板在辅助钢板内侧还是外侧
				f3dPoint verify_pt=ray_vec*fRayDist;
				coord_trans(verify_pt,pDatumPlate->ucs,TRUE);
				f3dPoint face1_pick=verify_pt,face2_pick=verify_pt;
				project_point(face1_pick,pAssisPlate1->ucs.origin,pAssisPlate1->ucs.axis_z);
				project_point(face2_pick,pAssisPlate2->ucs.origin,pAssisPlate2->ucs.axis_z);
				f3dPoint verify_vec=verify_pt-face1_pick;
				if(verify_vec*pAssisPlate1->ucs.axis_z>0)
					hAssPart1=abs(hAssPart1);
				else 
					hAssPart1=-abs(hAssPart1);
				verify_vec=verify_pt-face2_pick;
				if(verify_vec*pAssisPlate2->ucs.axis_z>0)
					hAssPart2=abs(hAssPart2);
				else 
					hAssPart2=-abs(hAssPart2);
			}
		}
		btc::RAYLINE cornerLine1,cornerLine2;
		btc::WORKPLANE datumPlane,assisPlane1,assisPlane2;
		datumPlane.pick=GetPlateDatumFacePickPt(pDatumPart,hDatumPart>0);
		if(pAssistantPart1)
		{
			assisPlane1.pick=GetPlateDatumFacePickPt(pAssistantPart1,hAssPart1>0);
			_Int3d2f(cornerLine1.pick,cornerLine1.vec,datumPlane.pick,pDatumPart->ucs.axis_z,assisPlane1.pick,pAssistantPart1->ucs.axis_z);
		}
		if(pAssistantPart2)
		{
			assisPlane2.pick=GetPlateDatumFacePickPt(pAssistantPart2,hAssPart2>0);
			_Int3d2f(cornerLine2.pick,cornerLine2.vec,datumPlane.pick,pDatumPart->ucs.axis_z,assisPlane2.pick,pAssistantPart2->ucs.axis_z);
		}
		double Ox=0,Oy=0;	//定位原点在底板上的相对坐标（与定位螺栓二选一）
		bSpecOrg=bSpecOrg&&GetDesignItemValue(KEY2C("Ox"),&Ox,pItemHashTbl);
		bSpecOrg=bSpecOrg&&GetDesignItemValue(KEY2C("Oy"),&Oy,pItemHashTbl);
		long hRelatedAngle=0;
		GetDesignItemValue(KEY2C("Pa"),&hRelatedAngle,pItemHashTbl);	//肋板关联角钢
		CLDSLineAngle *pRelatedAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(hRelatedAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(bSpecRayAngle)
		{	//指定射线角
			if(bSpecRayDist)	//指定射线距离，圆形塔脚肋板（射线为肋板Z轴方向）
			{
				ucs.origin=ray_vec*fRayDist;
				if(pAssisPlate1&&pAssisPlate2)
					profile.corner.C=profile.corner.A;	//圆形塔脚肋板台阶高度等于肋板高度
			}
			else
			{	//圆形塔脚对角肋板(射线方向为肋板X轴方向),坐标原点根据关联钢板、关联角钢确定	
				double origin_offset=0;
				if(pRelatedAngle)
				{
					double jg_space=pAssisPlate1->Thick,angle_r=FindJg_r(pRelatedAngle->GetWidth());
					if(pRelatedAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
						jg_space=((CLDSGroupLineAngle*)pRelatedAngle)->jg_space;
					origin_offset=(jg_space*0.5+pRelatedAngle->Thick)/cos45+CalJgROffset(angle_r,thick);
				}
				else 
					origin_offset=thick*0.5+pAssisPlate1->Thick/cos45*0.5;
				ucs.origin=ray_vec*origin_offset;
				if(pAssisPlate1&&pAssisPlate2)	//对角肋板延伸至塔脚板边缘
					profile.corner.B=pDatumPlate->GetWidth()*0.5-origin_offset;
			}
			coord_trans(ucs.origin,pDatumPart->ucs,TRUE);
		}
		else if(bSpecOrg)
		{
			ucs.origin.Set(Ox,Oy);
			coord_trans(ucs.origin,pDatumPart->ucs,TRUE);
		}
		else if(pBolt)
		{	//钢板肋板
			ucs.origin=pBolt->ucs.origin;
			if(pDatumPlate->FindLsByHandle(pBolt->handle)||fabs(pBolt->ucs.axis_z*pDatumPlate->ucs.axis_z)>EPS_COS)
				project_point(ucs.origin,datumPlane.pick,pBolt->ucs.axis_z);//螺栓坐标系原点向基准钢板基准面投影
			else if(pAssisPlate1&&pAssisPlate1->FindLsByHandle(pBolt->handle)||fabs(pBolt->ucs.axis_z*pAssisPlate1->ucs.axis_z)>EPS_COS)
				project_point(ucs.origin,assisPlane1.pick,pBolt->ucs.axis_z);//螺栓坐标系原点向基准钢板基准面投影
			else
			{
				if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("Datum bolt doesn't on datum base plate or ancillary palte！");
#else
					log2file->Log("基准螺栓不在基准底板或辅助钢板内！");
#endif
				ucs.origin=pBolt->ucs.origin;
			}
		}
		else if(pDatumNode)
			ucs.origin=pDatumNode->Position(true);
		else
			return FALSE;
		//2.计算肋板装配坐标系坐标轴方向
		//2.1计算Y轴初始默认方向
		ucs.axis_y=pDatumPart->ucs.axis_z;
		if(hDatumPart<0)
			ucs.axis_y*=-1.0;
		double VXx=0,VXy=0;	//X轴在底板上的相对坐标
		bSpecAxisX=bSpecAxisX&&GetDesignItemValue(KEY2C("Xx"),&VXx,pItemHashTbl);
		bSpecAxisX=bSpecAxisX&&GetDesignItemValue(KEY2C("Xy"),&VXy,pItemHashTbl);
		if(bSpecAxisX)
		{
			ucs.axis_x.Set(VXx,VXy);
			normalize(ucs.axis_x);
			vector_trans(ucs.axis_x,pDatumPart->ucs,TRUE);
			double fGradientDegAngle=90;	//与底板倾斜角度（即Y轴与底板间绕X轴的旋转角与方向基准杆件二选一）
			if(GetDesignItemValue(KEY2C("Ga"),&fGradientDegAngle,pItemHashTbl))
				bSpecAxisY=true;
			if(fGradientDegAngle!=90)
			{	//修正肋板有倾斜角时的Y轴方向
				double fRadAngle=(fGradientDegAngle-90)*RADTODEG_COEF;
				RotateVectorAroundVector(ucs.axis_y,fRadAngle,ucs.axis_x);
			}
		}
		if(pAssistantPart1==NULL&&pAssistantPart2==NULL)
		{	//钢板肋板
			if(!bSpecAxisX&&pVecPart==NULL)
				return FALSE;
			f3dPoint axis_x;
			if(pVecPart->IsLinePart()&&!bSpecAxisX)
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)pVecPart;
				axis_x=pLinePart->End()-pLinePart->Start();
				normalize(axis_x);
				if(hVecPart<0)	//用定位杆件句柄正负表示钢板延伸方向
					axis_x*=-1.0;
			}
			else if(pVecPart->GetClassTypeId()==CLS_BOLT&&!bSpecAxisX)
			{
				if(pBolt)
					axis_x=pVecPart->ucs.origin-pBolt->ucs.origin;
				else if(pDatumNode)
					axis_x=pVecPart->ucs.origin-pDatumNode->Position(true);
			}
			if(axis_x.IsZero())	//计算得到的X轴方向为空取基准钢板的X轴方向为默认X轴方向
				axis_x=pDatumPart->ucs.axis_x;
			ucs.axis_z=axis_x^ucs.axis_y;
			normalize(ucs.axis_z);
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
			normalize(ucs.axis_x);
			ucs.origin+=ucs.axis_x*fOffsetX;	//钢板肋板需要X轴方向偏移量
			ucs.origin+=ucs.axis_z*fOffsetZ;
			profile.corner.C=profile.corner.A;//fTopWidth=fBottomWidth;	//顶部宽度=底部宽度
			//添加钢板轮廓点
			vertice.Set(0,0,0);
			PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
			pVertex->m_bWeldEdge=TRUE;
			vertice.Set(profile.corner.B,0,0);
			vertex_list.append(vertice);
			vertice.Set(profile.corner.B,profile.corner.C,0);
			vertex_list.append(vertice);
			if(fabs(profile.corner.C-profile.corner.A)>EPS)
			{
				if(profile.corner.C>profile.corner.A)		//顶部宽度大于底部宽度
					vertice.Set(profile.corner.B-profile.corner.D,profile.corner.C,0);
				else if(profile.corner.C<profile.corner.A)	//顶部宽度小于底部宽度
					vertice.Set(profile.corner.D,profile.corner.A,0);
				vertex_list.append(vertice);
			}
			vertice.Set(0,profile.corner.A,0);
			vertex_list.append(vertice);
		}
		else if(pAssistantPart1)
		{	//角肋板或槽肋板坐标系计算
			if(bSpecRayAngle)
			{
				if(bSpecRayDist)	
				{	//指定射线距离，圆形塔脚肋板（射线为肋板Z轴方向）
					ucs.axis_z=ray_vec;
					vector_trans(ucs.axis_z,pDatumPart->ucs,TRUE);
					ucs.axis_x=ucs.axis_y^ucs.axis_z;
				}
				else
				{	//圆形塔脚对角肋板(射线方向为肋板X轴方向),坐标原点根据关联钢板、关联角钢确定
					ucs.axis_x=ray_vec;
					vector_trans(ucs.axis_x,pDatumPart->ucs,TRUE);
					ucs.axis_z=ucs.axis_x^ucs.axis_y;
				}
				normalize(ucs.axis_x);
				normalize(ucs.axis_z);
				ucs.axis_y=ucs.axis_z^ucs.axis_x;
				normalize(ucs.axis_y);
			}
			else if(!bSpecAxisX)
			{	//此种坐标轴方向定义能保证肋板与角焊缝线垂直 wjh-2015.1.21
				ucs.axis_x=pAssistantPart1->ucs.axis_z;
				if(hAssPart1<0)
					ucs.axis_x*=-1.0;
				//令X轴方向贴合基准钢板平面内 wjh-2015.1.29
				project_vector(ucs.axis_x,pDatumPart->ucs.axis_z);
			}
			ucs.axis_z=ucs.axis_x^ucs.axis_y;
			normalize(ucs.axis_z);
			if(!bSpecAxisY)
			{	//未通过倾斜角指定肋板Y轴方向时，需要进一步确认Y轴方向 wjh-2015.1.21
				if(bSpecRayAngle&&!bSpecRayDist)
				{
					f3dPoint axis_y=pAssisPlate1->ucs.axis_z^pAssisPlate2->ucs.axis_z;
					if(axis_y*ucs.axis_y<0)
						ucs.axis_y=-1*axis_y;
					else 
						ucs.axis_y=axis_y;
					ucs.axis_z=ucs.axis_x^ucs.axis_y;
					normalize(ucs.axis_z);
					ucs.axis_y=ucs.axis_z^ucs.axis_x;
				}
				else if(pVecPart&&pVecPart->IsLinePart())
				{	//钢板角肋板或槽肋板，存在Y方向定位杆件,纠正Y轴方向
					CLDSLinePart *pLinePart=(CLDSLinePart*)pVecPart;
					f3dPoint line_vec=pLinePart->End()-pLinePart->Start();
					if(line_vec*ucs.axis_y<0)
						ucs.axis_y=line_vec*-1.0;
					else 
						ucs.axis_y=line_vec;
					if(bSpecRayAngle&&bSpecRayDist)
					{
						ucs.axis_z=ray_vec;
						vector_trans(ucs.axis_z,pDatumPart->ucs,TRUE);
						ucs.axis_x=ucs.axis_y^ucs.axis_z;
						double dd=ucs.axis_x*pAssisPlate1->ucs.axis_z;
						if((hAssPart1>0&&dd<0)||(hAssPart1<0&&dd>0))
							ucs.axis_x*=-1;
						ucs.axis_z=ucs.axis_x^ucs.axis_y;
						normalize(ucs.axis_x);
						normalize(ucs.axis_z);
					}
					else 
					{
						ucs.axis_z=ucs.axis_x^ucs.axis_y;
						normalize(ucs.axis_z);
						ucs.axis_y=ucs.axis_z^ucs.axis_x;
					}
				}
				else	//未通过倾斜角指定肋板Y轴方向时，应根据X及Z轴向反向修正Y轴方向 wjh-2015.1.21
					ucs.axis_y=ucs.axis_z^ucs.axis_x;
				normalize(ucs.axis_y);
			}
			//肋板基准面与基准钢板基准面求交线
			f3dPoint along_axisvec=pAssistantPart1->ucs.axis_z^ucs.axis_z;	//沿1号辅板的角焊缝走向
			if(along_axisvec*ucs.axis_y<0)
				along_axisvec*=-1.0;
			//角肋板原点应位角焊缝线上
			if(bSpecRayAngle&&!bSpecRayDist)	//求交计算坐标原点位置
			{
				f3dPoint datumPlaneOrg=pDatumPlate->ucs.origin;
				if(hDatumPart>0)		//基准角钢在底板上侧
					datumPlaneOrg+=pDatumPlate->ucs.axis_z*(pDatumPlate->GetNormOffset()+pDatumPlate->GetThick());
				else 					//基准角钢在底板下侧
					datumPlaneOrg+=pDatumPlate->ucs.axis_z*pDatumPlate->GetNormOffset();
				Int3dlf(ucs.origin,ucs.origin,along_axisvec,datumPlaneOrg,pDatumPlate->ucs.axis_z);
				//根据两块辅助钢板计算肋板高度profile.corner.A
				CLDSGeneralPlate *platePtrArr[2]={pAssisPlate1,pAssisPlate2};
				for(int i=0;i<2;i++)
				{
					if(platePtrArr[i]==NULL)
						continue;
					f2dLine line;
					f3dPoint inters_pt;
					inters_pt=datumPlaneOrg+along_axisvec*50;
					coord_trans(inters_pt,platePtrArr[i]->ucs,FALSE);
					line.startPt.Set(inters_pt.x,inters_pt.y);
					inters_pt=datumPlaneOrg+along_axisvec*100000;
					coord_trans(inters_pt,platePtrArr[i]->ucs,FALSE);
					line.endPt.Set(inters_pt.x,inters_pt.y);
					PROFILE_VERTEX *pVertex=NULL,*pPrvVertex=platePtrArr[i]->vertex_list.GetTail();
					for(pVertex=platePtrArr[i]->vertex_list.GetFirst();pVertex;pVertex=platePtrArr[i]->vertex_list.GetNext())
					{
						if(Int2dll(line,f2dLine(pPrvVertex->vertex,pVertex->vertex),inters_pt.x,inters_pt.y)>0)
							break;	
						pPrvVertex=pVertex;
					}
					if(pVertex)
					{
						coord_trans(inters_pt,platePtrArr[i]->ucs,TRUE);
						profile.corner.A=DISTANCE(inters_pt,datumPlaneOrg);
						break;
					}
				}
			}
			else
				Int3dlf(ucs.origin,cornerLine1.pick,cornerLine1.vec,ucs.origin,ucs.axis_z);
			//添加钢板轮廓点
			//坐标原定沿aixs_z偏移
			ucs.origin+=ucs.axis_z*fOffsetZ;
			vector_trans(along_axisvec,ucs,FALSE);
			normalize(along_axisvec);	//沿1号辅板的角焊缝走向 wjh-2015.1.21
			//角肋板
			if(pAssistantPart2==NULL||(pAssistantPart1&&pAssistantPart2&&bSpecRayAngle&&!bSpecRayDist))
			{	//角肋板支持切角参数 wht 11-05-24
				vertice.Set(profile.corner.E,0,0);
				PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE;
				vertice.x=profile.corner.B;
				vertex_list.append(vertice);
				vertice.y+=profile.corner.C;	//
				vertex_list.append(vertice);
				vertice=profile.corner.A*along_axisvec;
				vertice.x+=profile.corner.D;
				vertex_list.append(vertice);
				vertice.x-=profile.corner.D;
				pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE;
				if(fabs(profile.corner.E)>eps)
				{
					vertice=profile.corner.E*along_axisvec;
					vertex_list.append(vertice);
				}
			}
			//槽型肋板
			else if(pAssistantPart2&&pAssistantPart2->IsPlate())
			{	//使用肋板与基准板的交线与基准钢板2基准面求交点
				f3dPoint inters_pt1,inters_pt2,inters_pt3;
				//以肋板工作平面与第二块辅助钢板的角焊缝线求交，更稳定 wjh-2015.1.29
				Int3dlf(inters_pt1,cornerLine2.pick,cornerLine2.vec,ucs.origin,ucs.axis_z);
				//沿2号辅板的角焊缝走向
				f3dPoint along_axisvec2=pAssistantPart2->ucs.axis_z^ucs.axis_z;	
				if(along_axisvec2*ucs.axis_y<0)
					along_axisvec2*=-1.0;
				normalize(along_axisvec2);
				inters_pt2=along_axisvec2*profile.corner.C+inters_pt1;
				inters_pt3=along_axisvec *profile.corner.A;
				coord_trans(inters_pt1,ucs,FALSE);
				coord_trans(inters_pt2,ucs,FALSE);
				inters_pt1.z=inters_pt2.z=0;
				//
				vertice.Set(0,0,0);
				PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE;
				pVertex=vertex_list.append(inters_pt1);
				pVertex->m_bWeldEdge=TRUE;
				vertex_list.append(inters_pt2);
				if(fabs(profile.corner.C-profile.corner.A)>EPS_COS)
				{
					f3dPoint axis_z;
					if(profile.corner.C>profile.corner.A)		//顶部宽度大于底部宽度
					{
						vertice=inters_pt2;
						axis_z=pAssistantPart2->ucs.axis_z;
						if(axis_z*ucs.axis_x>0)
							axis_z*=-1.0;
					}
					else if(profile.corner.C<profile.corner.A)
					{
						vertice=inters_pt3;
						axis_z=pAssistantPart1->ucs.axis_z;
						if(axis_z*ucs.axis_x<0)
							axis_z*=-1.0;
					}
					vector_trans(axis_z,ucs,FALSE);
					axis_z.z=0;
					vertice+=axis_z*profile.corner.D;
					vertex_list.append(vertice);
				}
				vertice=inters_pt3;
				pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE;
			}
		}
	}
	return TRUE;
}

double DESIGN_PLATE_ITEM::ToDouble()
{
	if(dataType==0)
		return value.fValue;
	else
		return value.iType;
}
long   DESIGN_PLATE_ITEM::ToLong()
{
	if(dataType==0)
		return ftoi(value.fValue);
	else
		return value.iType;
}
double DESIGN_PLATE_ITEM::SetItemValue(double val)
{
	return dataType==0?value.fValue=val:value.iType=(int)val;
}
void CLDSGeneralPlate::DESIGN_PARAM::Set(double fValue,bool exist/*=true*/)
{
	value.fValue=fValue;
	bExist=exist;
}
void CLDSGeneralPlate::DESIGN_PARAM::Set(long iValue,bool exist/*=true*/)
{
	value.iValue=iValue;
	bExist=exist;
}
double CLDSGeneralPlate::DESIGN_PARAM::ToDouble()
{
	if(cValueType==0)
		return value.fValue;
	else
		return value.iValue;
}
long   CLDSGeneralPlate::DESIGN_PARAM::ToLong()
{
	if(cValueType==0)
		return ftoi(value.fValue);
	else if(cValueType==3)	//句柄+数值
		return value.hPart;
	else
		return value.iValue;
}

void CLDSGeneralPlate::DESIGN_PARAM::InitBasicInfo(WORD wKey,BYTE cValType,BYTE cParamType/*=PARAM_SHAPE*/)
{
	this->wItemKey=wKey;
	this->bExist=false;
	this->cParamType=cParamType;
	this->cValueType=cValType;
}

bool CLDSParamPlate::IsElbowRibPlate()	//是否为钢管肘板型肋板，为统一肘板与肘板型肋板而设 wjh-2015.8.6
{
	if(_iParamType==TYPE_ELBOWPLATE)
		return true;
	if(_iParamType!=TYPE_RIBPLATE)
		return false;
	long hDatumPart,hAssPart1=0,hAssPart2=0;
	if(!GetDesignItemValue('A',&hDatumPart,NULL))//基准构件
		return false;
	if(!GetDesignItemValue('C',&hAssPart1,NULL))	//辅助构件1
		return false;
	GetDesignItemValue('D',&hAssPart2,NULL);	//辅助构件2
	CLDSPart* pDatumPart=BelongModel()->FromPartHandle(abs(hDatumPart),CLS_LINETUBE,CLS_PLATE,CLS_PARAMPLATE);
	CLDSPart* pAssistantPart1=BelongModel()->FromPartHandle(abs(hAssPart1),CLS_PLATE,CLS_PARAMPLATE);
	CLDSPart* pAssistantPart2=BelongModel()->FromPartHandle(hAssPart2,CLS_PLATE,CLS_PARAMPLATE);
	if(	pDatumPart&&pDatumPart->GetClassTypeId()==CLS_LINETUBE&&
		pAssistantPart1&&pAssistantPart1->IsPlate()&&pAssistantPart2==NULL)
		return true;
	else
		return false;
}
//旧版文件中会存在肘板类型(现已完全整合到角肋板中),为与角肋板统一显示界面,在显示属性时会临时将肘板参数转换为肋板 wjh-2017.1.17
void CLDSParamPlate::ShiftElbowAndRibPlateSchema(int iRibPlateSchema/*=TYPE_RIBPLATE*/)
{
	if(m_iParamType==iRibPlateSchema)
		return;
	CLDSParamPlate::RIBPLATE_ELBOW elbow;
	elbow.LoadParams(this,NULL);
	if(iRibPlateSchema==TYPE_RIBPLATE||iRibPlateSchema==TYPE_ELBOWPLATE)
		m_iParamType=iRibPlateSchema;
	EmptyDesignItems();
	if(m_iParamType==TYPE_RIBPLATE)
	{	//按照肋板模式填充设计参数
		SetDesignItemValue(KEY2C("A"),elbow.hDatumTube.ToDouble(),NULL,elbow.hDatumTube.cParamType,elbow.hDatumTube.cValueType);
		if(elbow.Z.ToDouble()>0)
			SetDesignItemValue(KEY2C("C"),elbow.hBtmPlate.ToDouble(),NULL,elbow.hBtmPlate.cParamType,elbow.hBtmPlate.cValueType);
		else
			SetDesignItemValue(KEY2C("C"),-1*elbow.hBtmPlate.ToDouble(),NULL,elbow.hBtmPlate.cParamType,elbow.hBtmPlate.cValueType);
		SetDesignItemValue(KEY2C("K"),elbow.rotAngle.ToDouble(),NULL,elbow.rotAngle.cParamType,elbow.rotAngle.cValueType);
		SetDesignItemValue(KEY2C("G"),elbow.A.ToDouble(),NULL,elbow.A.cParamType,elbow.A.cValueType);
		SetDesignItemValue(KEY2C("E"),elbow.B.ToDouble(),NULL,elbow.B.cParamType,elbow.B.cValueType);
		SetDesignItemValue(KEY2C("H"),elbow.C.ToDouble(),NULL,elbow.C.cParamType,elbow.C.cValueType);
		SetDesignItemValue(KEY2C("F"),elbow.D.ToDouble(),NULL,elbow.D.cParamType,elbow.D.cValueType);
		SetDesignItemValue(KEY2C("I"),elbow.E.ToDouble(),NULL,elbow.E.cParamType,elbow.E.cValueType);
		SetDesignItemValue(KEY2C("MF"),elbow.F.ToDouble(),NULL,elbow.F.cParamType,elbow.F.cValueType);
		SetDesignItemValue(KEY2C("MG"),elbow.G.ToDouble(),NULL,elbow.G.cParamType,elbow.G.cValueType);
		SetDesignItemValue(KEY2C("MH"),elbow.H.ToDouble(),NULL,elbow.H.cParamType,elbow.H.cValueType);
		if(elbow.V.bExist)
			SetDesignItemValue(KEY2C("S"),elbow.V.ToDouble(),NULL,elbow.V.cParamType,elbow.V.cValueType);
		else
			SetDesignItemValue(KEY2C("S"),0,NULL,PARAM_POS,2);
	}
	else if(m_iParamType==TYPE_ELBOWPLATE)
	{	//按照肘板模式填充设计参数
		SetDesignItemValue(KEY2C("K"),elbow.hDatumTube.ToDouble(),NULL,elbow.hDatumTube.cParamType,elbow.hDatumTube.cValueType);
		//肘板模式中elbow.hBtmPlate只能为正,肋板模式中的正负表示正反面信息由'Z'参数处理 wjh-2017.1.11
		SetDesignItemValue(KEY2C("L"),fabs(elbow.hBtmPlate.ToDouble()),NULL,elbow.hBtmPlate.cParamType,elbow.hBtmPlate.cValueType);
		SetDesignItemValue(KEY2C("Y"),elbow.rotAngle.ToDouble(),NULL,elbow.rotAngle.cParamType,elbow.rotAngle.cValueType);
		SetDesignItemValue(KEY2C("A"),elbow.A.ToDouble(),NULL,elbow.A.cParamType,elbow.A.cValueType);
		SetDesignItemValue(KEY2C("B"),elbow.B.ToDouble(),NULL,elbow.B.cParamType,elbow.B.cValueType);
		SetDesignItemValue(KEY2C("C"),elbow.C.ToDouble(),NULL,elbow.C.cParamType,elbow.C.cValueType);
		SetDesignItemValue(KEY2C("D"),elbow.D.ToDouble(),NULL,elbow.D.cParamType,elbow.D.cValueType);
		SetDesignItemValue(KEY2C("E"),elbow.E.ToDouble(),NULL,elbow.E.cParamType,elbow.E.cValueType);
		SetDesignItemValue(KEY2C("F"),elbow.F.ToDouble(),NULL,elbow.F.cParamType,elbow.F.cValueType);
		SetDesignItemValue(KEY2C("G"),elbow.G.ToDouble(),NULL,elbow.G.cParamType,elbow.G.cValueType);
		SetDesignItemValue(KEY2C("H"),elbow.H.ToDouble(),NULL,elbow.H.cParamType,elbow.H.cValueType);
		if(elbow.V.bExist)	//以前版本中没有该变量，肘板默认沿钢管方向
			SetDesignItemValue(KEY2C("V"),elbow.V.ToDouble(),NULL,elbow.V.cParamType,elbow.V.cValueType);
		else
			SetDesignItemValue(KEY2C("V"),0,NULL,PARAM_POS,2);
		if(elbow.Z.bExist)	//以前版本中没有该变量，默认在基准钢板反(底)面布置肘板
			SetDesignItemValue(KEY2C("Z"),elbow.Z.ToDouble(),NULL,elbow.Z.cParamType,elbow.Z.cValueType);
		else
			SetDesignItemValue(KEY2C("Z"),-1,NULL,PARAM_POS,2);
	}
	if(elbow.J.bExist)
		SetDesignItemValue(KEY2C("J"),elbow.J.ToDouble(),NULL,elbow.J.cParamType,elbow.J.cValueType);
	else
	{
		CLDSLineTube* pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(abs(elbow.hDatumTube.hValue),CLS_LINETUBE);
		CSuperSmartPtr<CLDSPart> pBottomPlate=BelongModel()->FromPartHandle(abs(elbow.hBtmPlate.hValue),CLS_PLATE,CLS_PARAMPLATE);
		elbow.J.value.iValue=0;
		if(pBottomPlate->GetClassTypeId()==CLS_PLATE)
			elbow.J.value.iValue=pBottomPlate.PlatePointer()->m_bAttachPartByWeldRoad;
		else if(pBottomPlate->GetClassTypeId()==CLS_PARAMPLATE)
		{
			long J=0;
			if(!pBottomPlate.ParamPlatePointer()->GetDesignItemValue('J',&J))
				elbow.J.value.iValue=pDatumTube->feature;
			else
				elbow.J.value.iValue=J;
		}
		SetDesignItemValue(KEY2C("J"),elbow.J.value.iValue,NULL,PARAM_POS,2);
	}
	if(elbow.b.bExist)
		SetDesignItemValue(KEY2C("b"),elbow.b.ToDouble(),NULL,elbow.b.cParamType,elbow.b.cValueType);
	else
		SetDesignItemValue(KEY2C("b"),1,NULL,PARAM_POS,2);
	SetDesignItemValue(KEY2C("R"),elbow.R.ToDouble(),NULL,elbow.R.cParamType,elbow.R.cValueType);
	if(elbow.e.bExist)
		SetDesignItemValue(KEY2C("e"),elbow.e.ToDouble(),NULL,elbow.e.cParamType,elbow.e.cValueType);
	else
		SetDesignItemValue(KEY2C("e"),1,NULL,PARAM_POS,2);
}
//初始肘板型肋板各参数的基础信息(数据键值、类型、属性名称及帮助提示）
void CLDSParamPlate::RIBPLATE_ELBOW::InitPropItemHashtable()
{
	//memset(this,0,sizeof(RIBPLATE_ELBOW));
	//hDatumTube.InitBasicInfo(KEY2C("A"),DESIGN_PARAM_ITEM::TYPE_HANDLE,PARAM_POS,"靠接钢管句柄","肋板侧面靠接钢管的标识句柄");
	////hBtmPlate>0:钢板底面布置肘板; hBtmPlate<0:钢板顶面布置肘板 与肘板参数'Z'对应
	//hBtmPlate.InitBasicInfo(KEY2C("C"),DESIGN_PARAM_ITEM::TYPE_HANDLE,PARAM_POS,"焊接钢板句柄","肋板底部焊接钢板的标识句柄");
	//J.InitBasicInfo(KEY2C('J'),DESIGN_PARAM_ITEM::TYPE_INTEGER,PARAM_POS,"转角基准边","肋板环向定位旋转角的起始基准线");
	//A.InitBasicInfo(KEY2C('G'),DESIGN_PARAM_ITEM::TYPE_DOUBLE,PARAM_SHAPE,"内侧高度","");	//内侧长度
	//B.InitBasicInfo(KEY2C('E'),DESIGN_PARAM_ITEM::TYPE_DOUBLE,PARAM_SHAPE,"底部宽度","");	//底部宽度
	//C.InitBasicInfo(KEY2C('H'),DESIGN_PARAM_ITEM::TYPE_DOUBLE,PARAM_SHAPE,"外侧长度","");	//外侧长度
	//D.InitBasicInfo(KEY2C('F'),DESIGN_PARAM_ITEM::TYPE_DOUBLE,PARAM_SHAPE,"底部宽度","");	//顶部宽度
	//E.InitBasicInfo(KEY2C('I'),DESIGN_PARAM_ITEM::TYPE_DOUBLE,PARAM_SHAPE,"内倒角宽度","肋板内侧焊接角点的切割宽度");
	//F.InitBasicInfo(KEY2C('MF'),DESIGN_PARAM_ITEM::TYPE_DOUBLE,PARAM_SHAPE,"孔间隙F","");	//孔间隙F
	//G.InitBasicInfo(KEY2C('MG'),DESIGN_PARAM_ITEM::TYPE_DOUBLE,PARAM_SHAPE,"孔间隙G","");	//孔间隙G
	//H.InitBasicInfo(KEY2C('M'),DESIGN_PARAM_ITEM::TYPE_DOUBLE,PARAM_SHAPE,"孔径","内侧通孔孔径");	//内侧通孔孔径
	//R.InitBasicInfo(KEY2C('R'),DESIGN_PARAM_ITEM::TYPE_DOUBLE,PARAM_SHAPE,"外环半径","助板底部外缘所在圆的半径");
	//rotAngle.InitBasicInfo(KEY2C('K'),DESIGN_PARAM_ITEM::TYPE_DOUBLE,PARAM_POS,"定位方向角","");
	//V.InitBasicInfo(KEY2C('S'),DESIGN_PARAM_ITEM::TYPE_INTEGER,PARAM_POS,"自动竖直正位","");	//0.沿钢管轴向肘板；1.竖直带自动正位的钢板
	//b.InitBasicInfo(KEY2C('b'),DESIGN_PARAM_ITEM::TYPE_INTEGER,PARAM_SHAPE);	//0.旧版数据; 1.2015年8月规范后新版数据 wjh-2015.8.08
}
void CLDSParamPlate::RIBPLATE_ELBOW::LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl)
{
	memset(this,0,sizeof(RIBPLATE_ELBOW));
	if(pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
	{
		pParamPlate->GetDesignItemValue(KEY2C("K"),&hDatumTube,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("L"),&hBtmPlate,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("Y"),&rotAngle,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("A"),&A,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("B"),&B,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("C"),&C,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("D"),&D,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("E"),&E,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("F"),&F,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("G"),&G,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("H"),&H,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("V"),&V,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("Z"),&Z,pHashTbl);
		if(V.cValueType==DESIGN_PARAM_ITEM::TYPE_DOUBLE)
			V.value.iValue=(long)V.value.fValue;
		V.cValueType=DESIGN_PARAM_ITEM::TYPE_INTEGER;
	}
	else if(pParamPlate->m_iParamType==TYPE_RIBPLATE)
	{
		pParamPlate->GetDesignItemValue(KEY2C("A"),&hDatumTube,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("C"),&hBtmPlate,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("K"),&rotAngle,pHashTbl);	//定位方向角
		pParamPlate->GetDesignItemValue(KEY2C("G"),&A,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("E"),&B,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("H"),&C,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("F"),&D,pHashTbl);
		pParamPlate->GetDesignItemValue(KEY2C("I"),&E,pHashTbl);	//内侧切角宽
		pParamPlate->GetDesignItemValue(KEY2C("MF"),&F,pHashTbl);	//孔间隙F
		pParamPlate->GetDesignItemValue(KEY2C("MG"),&G,pHashTbl);	//孔间隙G
		pParamPlate->GetDesignItemValue(KEY2C("MH"),&H,pHashTbl);	//孔径H
		pParamPlate->GetDesignItemValue(KEY2C("S"),&V,pHashTbl);	//自动竖直正位 0.沿钢管轴向肘板；1.竖直带自动正位的钢板
		Z.InitBasicInfo(KEY2C("Z"),DESIGN_PARAM_ITEM::TYPE_INTEGER,PARAM_POS);
		Z.bExist=true;
		Z.value.iValue=1;		//在钢板正面布置肘板
		if(hBtmPlate.value.iValue<0)
			Z.value.iValue=-1;	//在钢板反面布置肘板
		pParamPlate->GetDesignItemValue(KEY2C("O"),&hDatumBolt,pHashTbl);	//2.定位螺栓
		// 3.指定(全局)方向
		// 4.肘板底边(X轴)在底板上的延伸方向标识点的局部坐标
		pParamPlate->GetDesignItemValue(KEY2C("Xx"),&vAxisXVecX,pHashTbl);	//底边方向标定X分量
		pParamPlate->GetDesignItemValue(KEY2C("Xy"),&vAxisXVecY,pHashTbl);	//底边方向标定Y分量
		pParamPlate->GetDesignItemValue(KEY2C("Xz"),&vAxisXVecZ,pHashTbl);	//底边方向标定Z分量
		pParamPlate->GetDesignItemValue(KEY2C("mn"),&margin,pHashTbl);	//-1.默认指定底边长度;>=0.延伸到底板边缘(整数)
	}
	pParamPlate->GetDesignItemValue(KEY2C("J"),&J,pHashTbl);	//转角基准边","肋板环向定位旋转角的起始基准线
	pParamPlate->GetDesignItemValue(KEY2C("R"),&R,pHashTbl);	//"助板底部外缘所在圆的半径");
	pParamPlate->GetDesignItemValue(KEY2C("b"),&b,pHashTbl);	//0.旧版数据; 1.2015年8月规范后新版数据 wjh-2015.8.08
	pParamPlate->GetDesignItemValue(KEY2C("e"),&e,pHashTbl);	//倒角模式
}

void CLDSParamPlate::RIBPLATE_CORNER::LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl)
{
	memset(this,0,sizeof(RIBPLATE_CORNER));
	pParamPlate->GetDesignItemValue(KEY2C("A"),&hDatumPlate,pHashTbl);	//基准构件
	pParamPlate->GetDesignItemValue(KEY2C("B"),&hNode,pHashTbl);		//基点定位节点
	pParamPlate->GetDesignItemValue(KEY2C("C"),&hAssisPlate1,pHashTbl);	//辅助钢板1
	pParamPlate->GetDesignItemValue(KEY2C("D"),&hAssisPlate2,pHashTbl);	//辅助钢板1
	pParamPlate->GetDesignItemValue(KEY2C("O"),&hBolt,pHashTbl);		//基点定位螺栓
	pParamPlate->GetDesignItemValue(KEY2C("V"),&hVecPart,pHashTbl);		//X轴或Y轴方向基准杆件
	pParamPlate->GetDesignItemValue(KEY2C("X"),&fOffsetX,pHashTbl);		//X方向偏移量
	pParamPlate->GetDesignItemValue(KEY2C("Z"),&fOffsetZ,pHashTbl);		//Z方向偏移量
	pParamPlate->GetDesignItemValue(KEY2C("Ra"),&rayAngle,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Rd"),&rayDist,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Ox"),&Ox,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Oy"),&Oy,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Pa"),&hRelatedAngle,pHashTbl);	//肋板关联角钢
	pParamPlate->GetDesignItemValue(KEY2C("Xx"),&Xx,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Xy"),&Xy,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Ga"),&fGradientDegAngle,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("El"),&blSideEdgeLength,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("E"),&A,pHashTbl);	//底部宽度
	pParamPlate->GetDesignItemValue(KEY2C("F" ),&C,pHashTbl);	//顶部宽度
	pParamPlate->GetDesignItemValue(KEY2C("G" ),&B,pHashTbl);	//内侧长度
	pParamPlate->GetDesignItemValue(KEY2C("H" ),&D,pHashTbl);	//外侧长度
	pParamPlate->GetDesignItemValue(KEY2C("I" ),&E,pHashTbl);	//内侧切角宽度
	pParamPlate->GetDesignItemValue(KEY2C("MF"),&F,pHashTbl);	//孔间隙F
	pParamPlate->GetDesignItemValue(KEY2C("MG"),&G,pHashTbl);	//孔间隙G
	pParamPlate->GetDesignItemValue(KEY2C("MH"),&H,pHashTbl);	//内侧通孔孔径
	pParamPlate->GetDesignItemValue(KEY2C("e"),&e,pHashTbl);	//倒角模式
}
void CLDSParamPlate::RIBPLATE_TUBE::LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl)
{
	memset(this,0,sizeof(RIBPLATE_TUBE));
	pParamPlate->GetDesignItemValue(KEY2C("A"),&hDatumTube,pHashTbl);  //整数
	pParamPlate->GetDesignItemValue(KEY2C("B"),&hNode,pHashTbl);  //整数
	pParamPlate->GetDesignItemValue(KEY2C("K"),&rotAngle,pHashTbl);	//定位方向角
	pParamPlate->GetDesignItemValue(KEY2C("J"),&J,pHashTbl);  //整数
	pParamPlate->GetDesignItemValue(KEY2C("E"),&W,pHashTbl);  //浮点
	pParamPlate->GetDesignItemValue(KEY2C("G"),&H,pHashTbl);  //浮点
	pParamPlate->GetDesignItemValue(KEY2C("Nx"),&Nx,pHashTbl);  //浮点
	pParamPlate->GetDesignItemValue(KEY2C("Ny"),&Ny,pHashTbl);  //浮点
	pParamPlate->GetDesignItemValue(KEY2C("Nz"),&Nz,pHashTbl);  //浮点
	pParamPlate->GetDesignItemValue(KEY2C("Wx"),&Wx,pHashTbl);  //浮点
	pParamPlate->GetDesignItemValue(KEY2C("Wy"),&Wy,pHashTbl);  //浮点
	pParamPlate->GetDesignItemValue(KEY2C("Wz"),&Wz,pHashTbl);  //浮点
	pParamPlate->GetDesignItemValue(KEY2C("DM"),&DM,pHashTbl); //浮点
	pParamPlate->GetDesignItemValue(KEY2C("DX"),&DX,pHashTbl); //浮点
	pParamPlate->GetDesignItemValue(KEY2C("Lt"),&Lt,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Rt"),&Rt,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Lw"),&Lw,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Lh"),&Lh,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Rw"),&Rw,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Rh"),&Rh,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Nt"),&Nt,pHashTbl);	//整数
}
void CLDSParamPlate::RIBPLATE_STAY_WIRE::LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl)
{
	memset(this,0,sizeof(RIBPLATE_STAY_WIRE));
	pParamPlate->GetDesignItemValue(KEY2C("A"),&hDatumTube,pHashTbl);	//基准钢管
	pParamPlate->GetDesignItemValue(KEY2C("C"),&hAssisPlate,pHashTbl);  //依赖钢板
	pParamPlate->GetDesignItemValue(KEY2C("K"),&rotAngle,pHashTbl);		//定位方向角
	pParamPlate->GetDesignItemValue(KEY2C("J"),&J,pHashTbl);  //起始边
	pParamPlate->GetDesignItemValue(KEY2C("L"),&L,pHashTbl);  //板在钢管轴向长度
	pParamPlate->GetDesignItemValue(KEY2C("W"),&W,pHashTbl);  //板的宽度
	pParamPlate->GetDesignItemValue(KEY2C("H"),&H,pHashTbl);  //与板的焊接长度
	pParamPlate->GetDesignItemValue(KEY2C("R"),&R,pHashTbl);  //倒角半径
	pParamPlate->GetDesignItemValue(KEY2C("d"),&d,pHashTbl);  //拉线孔径
	pParamPlate->GetDesignItemValue(KEY2C("N"),&N,pHashTbl);  //拉线孔数
	pParamPlate->GetDesignItemValue(KEY2C("P"),&P,pHashTbl);  //孔间距
}
void CLDSParamPlate::RIBPLATE_PLATE::LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl)
{
	memset(this,0,sizeof(RIBPLATE_PLATE));
	pParamPlate->GetDesignItemValue(KEY2C("A"),&hDatumPlate,pHashTbl);  //整数
	pParamPlate->GetDesignItemValue(KEY2C("G"),&W,pHashTbl);  //浮点
	pParamPlate->GetDesignItemValue(KEY2C("E"),&H,pHashTbl);  //浮点
	pParamPlate->GetDesignItemValue(KEY2C("O"),&hStartBolt,pHashTbl);	//基点定位螺栓
	pParamPlate->GetDesignItemValue(KEY2C("V"),&hVecPart,pHashTbl);	//X轴或Y轴方向基准杆件
	pParamPlate->GetDesignItemValue(KEY2C("Ox"),&Ox,pHashTbl);			//定位点在基准钢板相对坐标系下的X分量
	pParamPlate->GetDesignItemValue(KEY2C("Oy"),&Oy,pHashTbl);			//定位点在基准钢板相对坐标系下的Y分量
	pParamPlate->GetDesignItemValue(KEY2C("Xx"),&Vx,pHashTbl);			//X轴延伸方向在基准钢板相对坐标系下的X分量
	pParamPlate->GetDesignItemValue(KEY2C("Xy"),&Vy,pHashTbl);			//X轴延伸方向在基准钢板相对坐标系下的Y分量
	pParamPlate->GetDesignItemValue(KEY2C("Ga"),&fGradientDegAngle,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("X"),&OffsetX,pHashTbl);		//X方向偏移量
	pParamPlate->GetDesignItemValue(KEY2C("Z"),&OffsetZ,pHashTbl);		//Z方向偏移量
	//扩展功能后的新增参数 wjh-2016.5.4
	pParamPlate->GetDesignItemValue(KEY2C("Ex"),&Ex,pHashTbl);		//O->E为肋板基准面的走向始->末定位基点
	pParamPlate->GetDesignItemValue(KEY2C("Ey"),&Ey,pHashTbl);		//O->E为肋板基准面的走向始->末定位基点
	pParamPlate->GetDesignItemValue(KEY2C("Bx"),&Bx,pHashTbl);		//B->T为肋板制弯面的走向始->末定位基点
	pParamPlate->GetDesignItemValue(KEY2C("By"),&By,pHashTbl);		//B->T为肋板制弯面的走向始->末定位基点
	pParamPlate->GetDesignItemValue(KEY2C("TB"),&hTipBolt,pHashTbl);//制弯面末端T点的基准螺栓
	pParamPlate->GetDesignItemValue(KEY2C("Tx"),&Tx,pHashTbl);		//制弯面末端T点的用户输入定义值
	pParamPlate->GetDesignItemValue(KEY2C("Ty"),&Ty,pHashTbl);		//制弯面末端T点的用户输入定义值
	pParamPlate->GetDesignItemValue(KEY2C("BV"),&hBendVecPart,pHashTbl);//'E'->T为带制弯肋板基面终端火曲点E'->制弯面末端T点
	pParamPlate->GetDesignItemValue(KEY2C("dx"),&TipOffsetX,pHashTbl);	//制弯面末段自定位基点的焊缝走向延伸量
	pParamPlate->GetDesignItemValue(KEY2C("dz"),&TipOffsetZ,pHashTbl);	//制弯面末段自定位基点的法向偏移量
}
void CLDSParamPlate::RIBPLATE_ANGLE::LoadParams(CLDSParamPlate* pParamPlate,CHashTable<DESIGN_PLATE_ITEM*>* pHashTbl)
{
	memset(this,0,sizeof(RIBPLATE_ANGLE));
	pParamPlate->GetDesignItemValue(KEY2C("A"),&A,pHashTbl);  //整数
	pParamPlate->GetDesignItemValue(KEY2C("B"),&B,pHashTbl);  //整数
	pParamPlate->GetDesignItemValue(KEY2C("C"),&C,pHashTbl);  //整数
	pParamPlate->GetDesignItemValue(KEY2C("I"),&I,pHashTbl);  //整数
	pParamPlate->GetDesignItemValue(KEY2C("L"),&L,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("W"),&W,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("S"),&S,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("E"),&E,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("D"),&D,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("H"),&H,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("R"),&R,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("d"),&d,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("P"),&P,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Ly"),&Ly,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Wy"),&Wy,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Hy"),&Hy,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("N"),&N,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Nx"),&Nx,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Ny"),&Ny,pHashTbl);
	pParamPlate->GetDesignItemValue(KEY2C("Nz"),&Nz,pHashTbl);
}
BOOL CLDSParamPlate::DesignRibPlateOnTube(CLDSLineTube* pDatumTube,CLDSNode* pDatumNode,CLDSParamPlate::RIBPLATE_TUBE& tuberib)
{
	//基准构件为钢管
	double fOffsetAngle=0,fBottomOuterR=-1,fTopOuterR=-1;
	int extend_vec_style=tuberib.J.value.iValue;	//0.基准边 1.焊道线 2.指定方向
	if(!tuberib.J.bExist)
		return FALSE;
	//if(!GetDesignItemValue('K',&fOffsetAngle,pItemHashTbl))	//偏移角度
	//	return FALSE;
	//fOffsetAngle*=RADTODEG_COEF;	//偏移角度装换为弧度制
	pDatumTube->BuildUCS();
	f3dPoint line_vec=pDatumTube->ucs.axis_z;
	int work_norm_style=0;
	if(tuberib.Nt.bExist)
	{	//延伸方向
		if(extend_vec_style==2)
		{
			if(!tuberib.Wx.bExist)	//展开方向(X轴)X分量
				return FALSE;
			if(!tuberib.Wy.bExist)	//展开方向(X轴)Y分量
				return FALSE;
			if(!tuberib.Wz.bExist)	//展开方向(X轴)Z分量
				return FALSE;
			ucs.axis_x.Set(tuberib.Wx.value.fValue,tuberib.Wy.value.fValue,tuberib.Wz.value.fValue);
			normalize(ucs.axis_x);
		}
		else
		{
			double rotAngle=tuberib.rotAngle.value.fValue*RADTODEG_COEF;
			if(pDatumTube->m_bHasWeldRoad&&extend_vec_style==1)
				rotAngle+=pDatumTube->CalWeldRoadAngle();
			ucs.axis_x.Set(cos(rotAngle),sin(rotAngle));
			vector_trans(ucs.axis_x,pDatumTube->ucs,TRUE);
		}
		//法线方向
		work_norm_style=tuberib.Nt.value.iValue;
		if(work_norm_style==2)
		{	//指定法向
			if(!tuberib.Nx.bExist)	//法线X分量
				return FALSE;
			if(!tuberib.Ny.bExist)	//法线Y分量
				return FALSE;
			if(!tuberib.Nz.bExist)	//法线Z分量
				return FALSE;
			ucs.axis_z.Set(tuberib.Nx.value.fValue,tuberib.Ny.value.fValue,tuberib.Nz.value.fValue);
			normalize(ucs.axis_z);
		}
		else if(work_norm_style==1)
		{	//钢管轴向
			ucs.axis_z=pDatumTube->ucs.axis_z;
		}
		else if(work_norm_style==0)
		{	//垂直与钢管延伸方向
			GEPOINT axis_x=ucs.axis_x;
			vector_trans(axis_x,pDatumTube->ucs,FALSE);
			ucs.axis_z.Set(-axis_x.y,axis_x.x);
			vector_trans(ucs.axis_z,pDatumTube->ucs,TRUE);
		}
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		normalize(ucs.axis_z);
	}
	else
	{
		if(extend_vec_style==2)
		{	//自定义工作法线方向
			work_norm_style=2;
			if(!tuberib.Nx.bExist)	//法线X分量
				return FALSE;
			if(!tuberib.Ny.bExist)	//法线Y分量
				return FALSE;
			if(!tuberib.Nz.bExist)	//法线Z分量
				return FALSE;
			if(!tuberib.Wx.bExist)	//展开方向(X轴)X分量
				return FALSE;
			if(!tuberib.Wy.bExist)	//展开方向(X轴)Y分量
				return FALSE;
			if(!tuberib.Wz.bExist)	//展开方向(X轴)Z分量
				return FALSE;
			ucs.axis_z.Set(tuberib.Nx.value.fValue,tuberib.Ny.value.fValue,tuberib.Nz.value.fValue);
			ucs.axis_x.Set(tuberib.Wx.value.fValue,tuberib.Wy.value.fValue,tuberib.Wz.value.fValue);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			ucs.axis_x=ucs.axis_y^ucs.axis_z;//通过Ｙ轴校正Ｘ轴与Ｚ轴的正交垂直
			normalize(ucs.axis_x);
			normalize(ucs.axis_y);
			normalize(ucs.axis_z);
		}
		else
		{	//通过基准边旋转角度确定工作法线(Y轴沿钢管轴向)
			double rotAngle=tuberib.rotAngle.value.fValue*RADTODEG_COEF;
			if(pDatumTube->m_bHasWeldRoad&&extend_vec_style==1)
				rotAngle+=pDatumTube->CalWeldRoadAngle();
			ucs.axis_x.Set(cos(rotAngle),sin(rotAngle));
			ucs.axis_z.Set(-ucs.axis_x.y,ucs.axis_x.x);
			vector_trans(ucs.axis_x,pDatumTube->ucs,TRUE);
			vector_trans(ucs.axis_z,pDatumTube->ucs,TRUE);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
		}
	}
	f3dPoint vertice;
	if(pDatumNode)
	{	//钢管上无关联钢板的肋板以钢管始->终方向为X轴正向，翼展方向为Y轴正向(出于操作习惯考虑)
		SnapPerp(&ucs.origin,pDatumTube->Start(),pDatumTube->End(),pDatumNode->Position(true));
		//调换X和Y轴方向
		f3dPoint temp_vec=ucs.axis_x;
		ucs.axis_x=-ucs.axis_y;
		ucs.axis_y=temp_vec;
		long Lt=tuberib.Lt.value.iValue,Rt=tuberib.Rt.value.iValue;
		double Lw=tuberib.Lw.value.fValue,Lh=tuberib.Lh.value.fValue;
		double Rw=tuberib.Rw.value.fValue,Rh=tuberib.Rh.value.fValue;
		if(Lt==2)
			Lh=Lw;
		if(Rt==2)
			Rh=Rw;
		PROFILE_VERTEX *pVertex=NULL;
		if(work_norm_style==2||work_norm_style==1)
		{
			double fOffsetM=tuberib.DM.value.fValue;
			double fOffsetX=tuberib.DX.value.fValue;
			double fMaxWidth=0;
			f3dPoint axis_vec=pDatumTube->End()-pDatumTube->Start();
			normalize(axis_vec);	//默认始->终为轴向偏移正方向
			ucs.origin+=axis_vec*fOffsetM;

			f3dPoint line_pick,inner_arc_start,inner_arc_end,arc_start,arc_end,origin;
			//原点坐标
			double radius = pDatumTube->GetDiameter()*0.5;
			if(fabs(fOffsetX)>0)
				ucs.origin+=ucs.axis_x*fOffsetX;
			if(Int3dlc(&inner_arc_start,&inner_arc_end,ucs.origin,ucs.axis_y,pDatumTube->Start(),pDatumTube->End(),radius,radius)!=2)
				return FALSE;
			double complement=(inner_arc_start-ucs.origin)*ucs.axis_y;	//高度补偿值
			if(complement>0)
				ucs.origin=inner_arc_start;
			else
			{
				complement=(inner_arc_end-ucs.origin)*ucs.axis_y;
				ucs.origin=inner_arc_end;
			}
			double cosa=fabs(ucs.axis_z*axis_vec);
			if(cosa>0)
				complement=radius/cosa-complement;
			else
				complement=0;
			arc_start=ucs.origin-ucs.axis_x*tuberib.W.value.fValue;//fBottomWidth;
			double fExtendHeight=pDatumTube->GetDiameter()*0.25;
			double fExtendWidth=20;
			int nRetCode=Int3dlc(&inner_arc_start,&inner_arc_end,arc_start,ucs.axis_y,pDatumTube->Start(),pDatumTube->End(),radius,radius);
			if(nRetCode==2)
			{
				if((inner_arc_start-arc_start)*ucs.axis_y>0)
					arc_start=inner_arc_start;
				else
					arc_start=inner_arc_end;
			}
			else if(nRetCode==1)
				arc_start=inner_arc_start;
			else 
			{
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->Log("The width of 0X%X plate is beyond the cylinder, design failure!",handle);
#else
				Log2File()->Log("0X%X肋板宽度已超出钢管柱面范围，设计失败!",handle);
#endif
				return FALSE;
			}
			//第一个顶点,圆弧终点（原点）
			vertice.feature=1;
			vertice.Set();
			vertex_list.append(vertice);
			if((Rt==1||Rt==2)&&Rw>0&&Rw<tuberib.W.value.fValue)
			{
				vertice.Set(0,tuberib.H.value.fValue+complement-Rh,0);
				PROFILE_VERTEX*pVertex = vertex_list.append(vertice);
				if(Rt==2)	//切弧，指定半径
				{
					pVertex->type=2;
					pVertex->radius=Rh;
				}
				vertice.Set(-Rw,tuberib.H.value.fValue+complement,0);
				vertex_list.append(vertice);
			}
			else
			{	//第二个顶点
				//vertice.x+=fInnerLen;
				vertice.Set(0,tuberib.H.value.fValue+complement,0);
				vertex_list.append(vertice);
			}
			if((Lt==1||Lt==2)&&Lw>0&&Lw<tuberib.W.value.fValue)
			{
				vertice.Set(-tuberib.W.value.fValue+Lw,tuberib.H.value.fValue+complement,0);
				pVertex = vertex_list.append(vertice);
				if(Lt==2)	//切弧，指定半径
				{
					pVertex->type=2;
					pVertex->radius=Lw;
				}
				vertice.Set(-tuberib.W.value.fValue,tuberib.H.value.fValue+complement-Lh,0);
				vertex_list.append(vertice);
			}
			else 
			{	//第三个顶点
				//vertice.y+=fBottomWidth;
				vertice.Set(-tuberib.W.value.fValue,tuberib.H.value.fValue+complement,0);
				vertex_list.append(vertice);
			}
			//第四个顶点,圆弧起点
			coord_trans(arc_start,ucs,FALSE);
			arc_start.z=0;
			arc_start.feature=1;
			pVertex=vertex_list.append(PROFILE_VERTEX(arc_start));
			if(fabs(ucs.axis_z*pDatumTube->ucs.axis_z)>EPS_COS)
			{	//圆弧
				pVertex->type=2;
				pVertex->center.Set(0,-100,0);	//给定圆弧拾取点，防止拾取点与圆弧的始终端点重合，导致无法确定圆心位置
				pVertex->radius=-radius;
			}
			else
			{	//椭圆弧
				pVertex->type=3;
				f3dPoint center;
				SnapPerp(&center,pDatumTube->Start(),pDatumTube->End(),ucs.origin);
				pVertex->center=center;
				if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
					pVertex->column_norm=pDatumTube->ucs.axis_z;
				else
					pVertex->column_norm=-pDatumTube->ucs.axis_z;
				pVertex->radius=-radius;
			}
			pVertex->m_bWeldEdge=TRUE;
		}
		else
		{
			ucs.origin+=ucs.axis_y*pDatumTube->GetDiameter()*0.5;
			/*ucs.axis_x=ucs.axis_y^ucs.axis_z;
			normalize(ucs.axis_x);
			ucs.axis_z=ucs.axis_x^ucs.axis_y;
			normalize(ucs.axis_z);*/
			if(tuberib.hDatumTube.hValue<0)
			{	//X轴与钢管轴向相反
				ucs.axis_x*=-1.0;
				ucs.axis_z=ucs.axis_x^ucs.axis_y;
				normalize(ucs.axis_z);
			}
			double fOffsetM=tuberib.DM.value.fValue;
			ucs.origin+=line_vec*fOffsetM;	//Y方向偏移量
			//钢管+节点
			vertice.Set(0,0,0);
			vertex_list.append(vertice);
			if((Rt==1||Rt==2)&&Rw>0&&Rw<tuberib.W.value.fValue)
			{
				vertice.Set(0,tuberib.H.value.fValue-Rh,0);
				pVertex = vertex_list.append(vertice);
				if(Rt==2)	//切弧,指定半径
				{
					pVertex->type=2;
					pVertex->radius=Rw;
				}
				vertice.Set(-Rw,tuberib.H.value.fValue,0);
				vertex_list.append(vertice);
			}
			else
			{
				vertice.Set(0,tuberib.H.value.fValue,0);
				vertex_list.append(vertice);
			}
			if((Lt==1||Lt==2)&&Lw>0&&Lw<tuberib.W.value.fValue)
			{
				vertice.Set(-tuberib.W.value.fValue+Lw,tuberib.H.value.fValue,0);
				pVertex = vertex_list.append(vertice);
				if(Lt==2)	//切弧,指定半径
				{
					pVertex->type=2;
					pVertex->radius=Lw;
				}
				vertice.Set(-tuberib.W.value.fValue,tuberib.H.value.fValue-Lh,0);
				vertex_list.append(vertice);
			}
			else 
			{
				vertice.Set(-tuberib.W.value.fValue,tuberib.H.value.fValue,0);
				vertex_list.append(vertice);
			}
			vertice.Set(-tuberib.W.value.fValue,0,0);
			pVertex=vertex_list.append(vertice);
			pVertex->m_bWeldEdge=TRUE;
		}
	}
	return TRUE;
}
BOOL CLDSParamPlate::DesignRibPlateOnAngle(CLDSLineAngle* pDatumAngle,CLDSParamPlate::RIBPLATE_ANGLE& anglerib)	//角钢肋板
{
	long A=anglerib.A.ToLong();
	long B=anglerib.B.ToLong();
	long I=anglerib.I.ToLong();
	long C=anglerib.C.ToLong();
	long N=anglerib.N.ToLong();
	long E=anglerib.E.ToLong();
	double L=anglerib.L.value.fValue;
	double W=anglerib.W.value.fValue;
	double S=anglerib.S.value.fValue;
	double D=anglerib.D.value.fValue;
	double H=anglerib.H.value.fValue;
	double R=anglerib.R.value.fValue;
	double d=anglerib.d.value.fValue;
	double P=anglerib.P.value.fValue;
	double Ly=anglerib.Ly.value.fValue;
	double Wy=anglerib.Wy.value.fValue;
	double Hy=anglerib.Hy.value.fValue;
	double Nx=anglerib.Nx.value.fValue;
	double Ny=anglerib.Ny.value.fValue;
	double Nz=anglerib.Nz.value.fValue;
	double wing_thick=0;
	if(!anglerib.B.bExist)
		return FALSE;
	//if(!anglerib.L.bExist)
	//	return FALSE;
	//if(!anglerib.W.bExist)
	//	return FALSE;
	//if(!anglerib.S.bExist)
	//	return FALSE;
	if(!anglerib.D.bExist)
		return FALSE;
	if(!anglerib.A.bExist)
		return FALSE;
	if(!anglerib.I.bExist) //肋板类型:0表示内侧肋板,1表示外侧肋板 wht 10-09-17
		I=0;	//没有该参数的肋板均为内侧肋板
	if(!anglerib.H.bExist&&I==1)
		return FALSE;	//外侧肋板与角钢相连侧的长度
	if(!anglerib.C.bExist&&I==0) //内侧肋板斜边类型:0表示直线,1表示圆弧
		return FALSE;	//内侧肋板斜边类型
	if(!anglerib.R.bExist&&I==0)
		return FALSE;	//内侧肋板斜边圆弧半径
	if(!anglerib.d.bExist&&I==0)
		return FALSE;	//内侧肋板中心孔直径
	if(!anglerib.P.bExist&&I==0)
		return FALSE;	//内侧肋板中心孔圆心距基准角钢肢的距离
	if(!anglerib.N.bExist)	//肋板法线计算：0.智能计算|1.用户指定
		N=0;	//没有该参数默认智能计算肋板法向
	if(!anglerib.Ly.bExist)
		Ly=L;
	if(!anglerib.Wy.bExist)
		Wy=W;
	if(!anglerib.Hy.bExist)
		Hy=H;
	CLDSLineAngle *pLineAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(A,CLS_LINEANGLE);
	CLDSNode *pDatumNode=BelongModel()->FromNodeHandle(B);
	CLDSBolt *pBolt = (CLDSBolt*)BelongModel()->FromPartHandle(B,CLS_BOLT);
	if((pDatumNode==NULL&&pBolt==NULL)||pLineAngle==NULL)
		return FALSE;
	wing_thick=pLineAngle->GetThick();
	pLineAngle->CalPosition();
	pLineAngle->cal_x_wing_norm();
	pLineAngle->cal_y_wing_norm();
	f3dPoint line_vec=pLineAngle->End()-pLineAngle->Start();
	normalize(line_vec);
	if(N==1&&anglerib.Nx.bExist&&anglerib.Ny.bExist&&anglerib.Nz.bExist)
	{
		ucs.axis_z.Set(Nx,Ny,Nz);
		normalize(ucs.axis_z);
	}
	else
		ucs.axis_z=line_vec;
	double justify=ucs.axis_z*line_vec;
	if(justify<0)
		ucs.axis_z*=-1;
	if (pDatumNode)
	{
		ucs.origin=pLineAngle->GetDatumPosBer(pDatumNode);
		if(I<2)
		{
			if(N==0)
				ucs.origin+=ucs.axis_z*D;
			else //if(N==1)	//指定平推面时，须保证节点位于钢板基准平面内 wjh-2017.11.15
			{
				GEPOINT xPlanePick=pDatumNode->xArcliftPos;
				if(D!=0)
					xPlanePick+=ucs.axis_z*D;
				Int3dlf(ucs.origin,ucs.origin,line_vec,xPlanePick,ucs.axis_z);
			}
		}
	}
	else //if (pBolt)
	{
		if(fabs(pLineAngle->get_norm_x_wing()*pBolt->get_norm())>fabs(pLineAngle->get_norm_y_wing()*pBolt->get_norm() ))
			Int3dlf(ucs.origin,pBolt->ucs.origin,pBolt->get_norm(),pLineAngle->Start(),pLineAngle->get_norm_x_wing());
		else
			Int3dlf(ucs.origin,pBolt->ucs.origin,pBolt->get_norm(),pLineAngle->Start(),pLineAngle->get_norm_y_wing());
		SnapPerp(&ucs.origin,pLineAngle->Start(),pLineAngle->End(),pBolt->ucs.origin);
		if(D!=0 && I<2)
			ucs.origin+=ucs.axis_z*D;
	}
	ucs.axis_x=-pLineAngle->get_norm_y_wing();
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	normalize(ucs.axis_y);
	normalize(ucs.axis_x);
	if(I==2)
	{	//纵向角钢肋板重新计算坐标系，原点落在肋板上
		f3dPoint y_extend_vec=pLineAngle->GetWingVecY();
		vector_trans(y_extend_vec,pLineAngle->ucs,FALSE);
		f3dPoint bi_vec(1+y_extend_vec.x,y_extend_vec.y,0);
		normalize(bi_vec);	//角平分方向
		double fDist=DISTANCE(f3dPoint(),f3dPoint(wing_thick*bi_vec.x/bi_vec.y,wing_thick));
		fDist+=Thick*0.5;
		vector_trans(bi_vec,pLineAngle->ucs,TRUE);
		//
		ucs.axis_x=(N==0)?line_vec:line_vec*(-1);
		ucs.axis_y=bi_vec;
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		ucs.origin+=ucs.axis_x*D+ucs.axis_y*fDist;
	}
	//
	f3dPoint vertice,x_wing_vec,y_wing_vec,norm_x_wing,norm_y_wing;
	x_wing_vec=pLineAngle->GetWingVecX();
	y_wing_vec=pLineAngle->GetWingVecY();
	norm_x_wing=pLineAngle->vxWingNorm;
	norm_y_wing=pLineAngle->vyWingNorm;
	if(fabs(justify)<EPS_COS)
	{
		GEPOINT vxWingVec=ucs.axis_z^pLineAngle->vxWingNorm;
		GEPOINT vyWingVec=ucs.axis_z^pLineAngle->vyWingNorm;
		x_wing_vec=(x_wing_vec*vxWingVec<0)?-vxWingVec:vxWingVec;
		y_wing_vec=(y_wing_vec*vyWingVec<0)?-vyWingVec:vyWingVec;
		norm_x_wing=x_wing_vec^ucs.axis_z;
		norm_y_wing=y_wing_vec^ucs.axis_z;
		if(norm_x_wing*pLineAngle->vxWingNorm<0)
			norm_x_wing*=-1.0;
		if(norm_y_wing*pLineAngle->vyWingNorm<0)
			norm_y_wing*=-1.0;
		normalize(x_wing_vec);
		normalize(y_wing_vec);
		normalize(norm_x_wing);
		normalize(norm_y_wing);
	}
	vector_trans(x_wing_vec,ucs,FALSE);
	vector_trans(y_wing_vec,ucs,FALSE);
	vector_trans(norm_x_wing,ucs,FALSE);
	vector_trans(norm_y_wing,ucs,FALSE);
	x_wing_vec.z=y_wing_vec.z=norm_x_wing.z=norm_y_wing.z=0;
	if(I==1)
	{	//外侧肋板 
		f3dLine line_firs,line_sec;
		line_firs.startPt=f3dPoint(0,0,0)+x_wing_vec*H+norm_x_wing*W;
		line_firs.endPt=line_firs.startPt-x_wing_vec*L;
		line_sec.startPt=f3dPoint(0,0,0)+y_wing_vec*Hy+norm_y_wing*Wy;
		line_sec.endPt=line_sec.startPt-y_wing_vec*Ly;
		line_firs.startPt.z=line_firs.endPt.z=line_sec.startPt.z=line_sec.endPt.z=0;
		f3dPoint inters_vertex;
		Int3dpl(line_firs,line_sec,inters_vertex);
		//
		vertice.Set();
		PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
		pVertex->m_bWeldEdge=TRUE;
		vertice=f3dPoint(0,0,0)+y_wing_vec*Hy;
		vertex_list.append(vertice);
		vertice=vertice+norm_y_wing*Wy;
		vertex_list.append(vertice);
		if(ftoi(S)==0)
			vertex_list.append(inters_vertex);	//交点
		else
		{	//倒角
			f3dPoint prev_vec=line_firs.startPt-line_firs.endPt;
			normalize(prev_vec);
			f3dPoint next_vec=line_sec.startPt-line_sec.endPt;
			normalize(next_vec);
			f3dPoint mid_vec=prev_vec+next_vec;
			normalize(mid_vec);
			double cosa=prev_vec*mid_vec;
			double sina=sqrt(1-cosa*cosa);
			if(fabs(sina)<EPS||fabs(cosa)<EPS)
				vertex_list.append(inters_vertex);	//交点
			else
			{
				double tana=sina/cosa;
				double offset_dist=S/tana;
				vertice=inters_vertex+y_wing_vec*offset_dist;
				pVertex=vertex_list.append(vertice);
				pVertex->type=2;	//指定半径圆弧,center为相对坐标 wht 12-06-15
				pVertex->center.Set();
				//pVertex->center=inters_vertex+mid_vec*(S/sina);
				pVertex->radius=S;
				vertice=inters_vertex+x_wing_vec*offset_dist;
				vertex_list.append(vertice);
			}
		}
		vertice=f3dPoint(0,0,0)+x_wing_vec*H+norm_x_wing*W;
		vertex_list.append(vertice);
		vertice=f3dPoint(0,0,0)+x_wing_vec*H;
		pVertex=vertex_list.append(vertice);
		pVertex->m_bWeldEdge=TRUE;
	}
	else if(I==0)
	{	//内侧肋板
		GEPOINT vMiddleVec=x_wing_vec+y_wing_vec;
		normalize(vMiddleVec);
		double dfBiCosa=vMiddleVec*x_wing_vec;
		double dfBiSina=SQRT(1-dfBiCosa*dfBiCosa);
		double dfBiLeng=dfBiSina>EPS?wing_thick/dfBiSina:wing_thick*1.414;	//dfBiSina<=EPS时就已经是出错了 wjh-2019.12.12
		GEPOINT xCornerPoint=dfBiLeng*vMiddleVec;
		//S+=wing_thick;	//此行代码影响已计入xCornerPoint中
		L+=wing_thick;
		Ly+=wing_thick;
		//W+=wing_thick;
		vertice=xCornerPoint+x_wing_vec*S;//-norm_x_wing*wing_thick;
		PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
		pVertex->m_bWeldEdge=TRUE;
		//TODO:未求公切点 wjh-2015.2.16
		if(C==1)
		{
			double alfa=pLineAngle->GetRadWingAngle();
			double ctana=(alfa<Pi-EPS&&alfa>EPS) ? tan((Pi-alfa)/2) : 1;
			norm_x_wing=pLineAngle->get_norm_x_wing();
			x_wing_vec=pLineAngle->GetWingVecX();
			vector_trans(norm_x_wing,ucs,FALSE);
			vector_trans(x_wing_vec,ucs,FALSE);
			//圆心
			f3dPoint center;
			if(R>0&&P>0)
			{	//根据特殊孔位置及圆弧半径计算轮廓点
				f3dPoint startVertice,endVertice;
				startVertice=f3dPoint(0,0,0)+x_wing_vec*L-norm_x_wing*wing_thick;
				endVertice=f3dPoint(0,0,0)+y_wing_vec*Ly-norm_y_wing*wing_thick;
				center = f3dPoint(0,0,0)-norm_x_wing*(P+wing_thick)+x_wing_vec*ctana*(P+wing_thick);

				PROFILE_VERTEX startTangPt,endTangPt;
				if(!CalStartAndEndTangentPt(startVertice,endVertice,R,center,startTangPt,endTangPt))
				{
					logerr.ShowToScreen();
					return FALSE;
				}
				vertex_list.append(startVertice);
				vertex_list.append(startTangPt);
				vertex_list.append(endTangPt);
				vertex_list.append(endVertice);                                                        
			}
			else if(R>0)
			{	//根据用户输入的圆环半径计算轮廓点
				f3dPoint pt=x_wing_vec*L+y_wing_vec*Ly;
				vertice=x_wing_vec*L-norm_x_wing*wing_thick;
				vertex_list.append(vertice);
				vertice=pt+norm_x_wing*R;
				pVertex=vertex_list.append(vertice);
				pVertex->type = 2;
				pVertex->radius=R;
				vertice=pt+norm_y_wing*R;
				vertex_list.append(vertice);
				vertice=y_wing_vec*Ly-norm_y_wing*wing_thick;
				pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE;
			}
			else
			{	//R==0时自动计算圆弧边
				f3dPoint pt_lx=f3dPoint(0,0,0)+x_wing_vec*L-norm_x_wing*wing_thick;
				f3dPoint pt_xw=pt_lx-norm_x_wing*W;
				f3dPoint pt_ly=f3dPoint(0,0,0)+y_wing_vec*Ly-norm_y_wing*wing_thick;
				f3dPoint pt_yw=pt_ly-norm_y_wing*Wy;
				vertex_list.append(pt_lx);
				if(Int3dll(pt_lx,pt_xw,pt_ly,pt_yw,center)>0)
				{	//两直线有交点,不存在圆弧
					vertex_list.append(center);
				}
				else
				{	//计算有效圆弧
					center=f3dPoint(0,0,0)-norm_x_wing*(wing_thick+W)+x_wing_vec*ctana*(wing_thick+W);
					double fRadius=DISTANCE(pt_xw,center);
					if(fRadius>2)
					{
						pVertex=vertex_list.append(pt_xw);
						pVertex->type=2;
						pVertex->radius=fRadius;
					}
					else
						vertex_list.append(pt_xw);
					vertex_list.append(pt_yw);
				}
				pVertex=vertex_list.append(pt_ly);
				pVertex->m_bWeldEdge=TRUE;				
			}
		}
		else
		{
			vertice=f3dPoint(0,0,0)+x_wing_vec*L-norm_x_wing*wing_thick;
			vertex_list.append(vertice);
			vertice=vertice-norm_x_wing*W;
			vertex_list.append(vertice);
			vertice=f3dPoint(0,0,0)+y_wing_vec*Ly-norm_y_wing*(Wy+wing_thick);
			vertex_list.append(vertice);
			vertice=f3dPoint(0,0,0)+y_wing_vec*Ly-norm_y_wing*wing_thick;
			pVertex=vertex_list.append(vertice);
			pVertex->m_bWeldEdge=TRUE;
		}
		if (S > 1)
		{
			vertice = xCornerPoint + y_wing_vec * S;//-norm_y_wing*wing_thick;
			pVertex = vertex_list.append(vertice);
			if (E==1)
			{
				pVertex->type = 2;
				pVertex->radius = -S;
				pVertex->center = xCornerPoint;
			}
		}
	}
	else if(I==2)
	{	//纵向肋板
		PROFILE_VERTEX *pVertex=vertex_list.append(f3dPoint(-L*0.5,0,0));
		pVertex->m_bWeldEdge=TRUE;
		vertex_list.append(f3dPoint(L*0.5,0,0));
		if(ftoi(R)==0||L-R<1||W-R<1)
		{
			vertex_list.append(f3dPoint(L*0.5,W,0));
			vertex_list.append(f3dPoint(-L*0.5,W,0));
		}
		else
		{	//进行圆弧倒角
			pVertex=vertex_list.append(f3dPoint(L*0.5,W-R,0));
			pVertex->type=2;
			pVertex->radius=R;
			pVertex->center.Set(L*0.5-R,W-R);
			vertex_list.append(f3dPoint(L*0.5-R,W,0));
			pVertex=vertex_list.append(f3dPoint(-L*0.5+R,W,0));
			pVertex->type=2;
			pVertex->radius=R;
			pVertex->center.Set(-L*0.5+R,W-R);
			vertex_list.append(f3dPoint(-L*0.5,W-R,0));
		}
	}
	return TRUE;
}
BOOL CLDSParamPlate::DesignTubeStayWirePlate(CLDSParamPlate::RIBPLATE_STAY_WIRE& stay_wire)
{	//钢管拉线板
	long hTube=stay_wire.hDatumTube.ToLong();
	long hPlate=stay_wire.hAssisPlate.ToLong();
	long nHole=stay_wire.N.ToLong();
	long J=0;
	if(stay_wire.J.bExist)
		J=stay_wire.J.ToLong();
	double rotAngle=stay_wire.rotAngle.ToDouble();
	rotAngle*=RADTODEG_COEF;
	double L=stay_wire.L.ToDouble();
	double W=stay_wire.W.ToDouble();
	double H=stay_wire.H.ToDouble();
	double R=stay_wire.R.ToDouble();
	double d=stay_wire.d.ToDouble();
	double P=stay_wire.P.ToDouble();
	if(!stay_wire.hDatumTube.bExist)
		return FALSE;
	if(!stay_wire.hAssisPlate.bExist)
		return FALSE;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(hTube,CLS_LINETUBE);
	CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(labs(hPlate),CLS_PLATE,CLS_PARAMPLATE);
	if(pPlate==NULL||pDatumTube==NULL)
		return FALSE;
	//计算坐标系
	pDatumTube->BuildUCS();
	f3dPoint line_vec=(pDatumTube->End()-pDatumTube->Start()).normalized();
	double fRadius=pDatumTube->GetDiameter()*0.5;
	double weldRoadAngle=rotAngle;
	if(pDatumTube->m_bHasWeldRoad&&J!=0)
		weldRoadAngle+=pDatumTube->CalWeldRoadAngle();
	ucs.axis_x.Set(cos(weldRoadAngle),sin(weldRoadAngle),0);
	ucs.axis_z.Set(-ucs.axis_x.y,ucs.axis_x.x);
	vector_trans(ucs.axis_x,pDatumTube->ucs,TRUE);
	vector_trans(ucs.axis_z,pDatumTube->ucs,TRUE);
	f3dPoint inters_vec=ucs.axis_z^pPlate->ucs.axis_z;
	normalize(inters_vec);
	normalize(ucs.axis_x);
	double cosa=ucs.axis_x*inters_vec;	//计算肘板初始及最终X轴方向间的余弦值
	if(ucs.axis_x*inters_vec<0)
		ucs.axis_x=-inters_vec;
	else
		ucs.axis_x=inters_vec;
	//根据偏移角度调整基准方向
	BOOL bTopFace=hPlate>0?TRUE:FALSE;
	f3dPoint face_pick=GetPlateDatumFacePickPt(pPlate,bTopFace);
	f3dPoint inters1,inters2,line_pick,temp_vec;
	Int3dlf(line_pick,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pPlate->ucs.axis_z);
	if(Int3dlc(&inters1,&inters2,line_pick,ucs.axis_x,pDatumTube->Start(),pDatumTube->End(),fRadius,fRadius)!=2)
	{
		logerr.Log("坐标原点计算失败！");
		return FALSE;
	}
	temp_vec=(inters1-inters2).normalized();
	if(temp_vec*ucs.axis_x>0)
		ucs.origin=inters1;
	else
		ucs.origin=inters2;
	//调整坐标系各方向轴
	ucs.axis_z = ucs.axis_x^line_vec;
	normalize(ucs.axis_z);
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_y);
	if((bTopFace&&pPlate->ucs.axis_z*ucs.axis_y<0)||(!bTopFace&&pPlate->ucs.axis_z*ucs.axis_y>0))
	{	//调整肘板坐标系
		ucs.axis_y*=-1.0;
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
	}
	//计算轮廓点
	f3dPoint pt_O,pt_L(0,L,0),pt_W(W,L,0),pt_H(H,0,0),tan_pt1,tan_pt2,pt_S,pt_E;
	if(R>0&&nHole>0&&d>0&&P>d)
	{
		f3dPoint center1,center2;
		center1.Set(W,L-R,0);
		center2.Set(W,L-R-(nHole-1)*P,0);
		f2dPoint tan_pt;
		if(Tang2dpc(f2dPoint(pt_L.x,pt_L.y),f2dCircle(R,center1.x,center1.y),f2dPoint(pt_W.x,pt_W.y),tan_pt))
			tan_pt1.Set(tan_pt.x,tan_pt.y,0);
		if(Tang2dpc(f2dPoint(pt_H.x,pt_H.y),f2dCircle(R,center2.x,center2.y),f2dPoint(pt_W.x+R,center2.y),tan_pt))
			tan_pt2.Set(tan_pt.x,tan_pt.y,0);
		if(nHole>1)
		{
			pt_S.Set(center1.x+R,center1.y,0);
			pt_E.Set(center2.x+R,center2.y,0);
		}
	}
	//初始化轮廓列表
	PROFILE_VERTEX* pVertex=vertex_list.append(pt_O);
	pVertex->m_bWeldEdge=true;
	pVertex=vertex_list.append(pt_H);
	if(!tan_pt1.IsZero())
	{
		pVertex=vertex_list.append(tan_pt2);
		pVertex->type=2;
		pVertex->radius=R;
		if(!pt_S.IsZero())
		{
			vertex_list.append(pt_E);
			pVertex=vertex_list.append(pt_S);
			pVertex->type=2;
			pVertex->radius=R;
		}
		vertex_list.append(tan_pt1);
	}
	else
		vertex_list.append(pt_W);
	pVertex=vertex_list.append(pt_L);
	pVertex->m_bWeldEdge=true;
	return TRUE;
}
BOOL CLDSParamPlate::DesignRibElbowPlate(CLDSLineTube* pDatumTube,CLDSGeneralPlate* pBtmPlate,CLDSParamPlate::RIBPLATE_ELBOW& elbow)
{	//肘板型肋板
	double A=elbow.A.ToDouble();
	double B=elbow.B.ToDouble();
	double C=elbow.C.ToDouble();
	double D=elbow.D.ToDouble();
	double E=elbow.E.ToDouble();
	double F=elbow.F.ToDouble();
	double G=elbow.G.ToDouble();
	double H=elbow.H.ToDouble();
	double Y=elbow.rotAngle.ToDouble();
	long V=elbow.V.ToLong();
	long K=elbow.hDatumTube.ToLong();
	long L=elbow.hBtmPlate.ToLong();
	long Z=elbow.Z.ToLong();
	long b=elbow.b.ToLong();
	long J=elbow.J.ToLong();
	long e=elbow.e.ToLong();
	if(!elbow.A.bExist)
		return FALSE;
	if(!elbow.B.bExist)
		return FALSE;
	if(!elbow.C.bExist)
		return FALSE;
	if(!elbow.D.bExist)
		return FALSE;
	if(!elbow.E.bExist)
		return FALSE;
	//if(!elbow.F.bExist)
	//	return FALSE;
	//if(!elbow.G.bExist)
	//	return FALSE;
	if(!elbow.H.bExist)
		H=0;
	if(!elbow.V.bExist)
		V=0;	//以前版本中没有该变量，肘板默认沿钢管方向
	if(!elbow.Z.bExist)
		Z=-1;	//以前版本中没有该变量，默认在基准钢板反(底)面布置肘板 wht 15-12-26
	if(!elbow.b.bExist)
		b=0;	//新增变量，用于处理肘板的偏移角度
	if(!elbow.e.bExist)
		e=0;	//新增变量，用于处理肘板的倒角模式
	long hBtmBolt =	elbow.hDatumBolt.bExist?elbow.hDatumBolt.ToLong():0;	//2.定位螺栓
	f3dPoint vGloblBtmEdgeVec,vLocalBtmEdgeVecPick;
	if(J==3)
	{	// 3.指定(全局)方向
		vGloblBtmEdgeVec.x=elbow.vAxisXVecX.ToDouble();	//底边方向标定X分量
		vGloblBtmEdgeVec.y=elbow.vAxisXVecY.ToDouble();	//底边方向标定Y分量
		vGloblBtmEdgeVec.z=elbow.vAxisXVecZ.ToDouble();	//底边方向标定Z分量
	}
	else if(J==4)
	{	// 4.肘板底边(X轴)在底板上的延伸方向标识点的局部坐标
		vLocalBtmEdgeVecPick.x=elbow.vAxisXVecX.ToDouble();	//底边方向标定点在底板局部坐标下的X分量
		vLocalBtmEdgeVecPick.y=elbow.vAxisXVecY.ToDouble();	//底边方向标定点在底板局部坐标下的Y分量
	}
	long margin = elbow.margin.bExist?elbow.margin.ToLong():-1;	//-1.默认指定底边长度;>=0.延伸到底板边缘(整数)
	if(!elbow.rotAngle.bExist&&J<=1)
		return FALSE;
	Y*=RADTODEG_COEF;	//定位方向角
	f3dPoint line_vec;
	CSuperSmartPtr<CLDSPart> pBottomPlate=pBtmPlate;
	if(pDatumTube!=NULL)
	{
		if(K>0)
			line_vec=pDatumTube->End()-pDatumTube->Start();
		else if(K<0)
			line_vec=pDatumTube->Start()-pDatumTube->End();
		if(V==1&&pBottomPlate.IsHasPtr())
			line_vec=pBottomPlate->ucs.axis_z*K;
	}
	else
	{
		GetDesignItemValue('x',&line_vec.x);//,&itemHashTbl);
		GetDesignItemValue('y',&line_vec.y);//,&itemHashTbl);
		GetDesignItemValue('z',&line_vec.z);//,&itemHashTbl);
	}
	normalize(line_vec);
	CLDSBolt* pDatumBolt=hBtmBolt>0 ? (CLDSBolt*)BelongModel()->FromPartHandle(hBtmBolt,CLS_BOLT) : NULL;
	if(m_bAutoCalParamPlateUcs)
	{	//计算肘板的装配坐标系
		if(pDatumTube&&pBottomPlate.IsHasPtr()&&J<=1)
		{	//参数完备自动计算
			pDatumTube->BuildUCS();
			double weldRoadAngle=0;
			if(elbow.J.bExist)	//旧版文件中elbow内无J参数，无参数时应继承旧有模式 wjh-2016.1.11
				J=elbow.J.ToLong();
			else if(pBottomPlate->GetClassTypeId()==CLS_PLATE)
				J=pBottomPlate.PlatePointer()->m_bAttachPartByWeldRoad;
			else if(pBottomPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				if(!pBottomPlate.ParamPlatePointer()->GetDesignItemValue('J',&J))
					J=pDatumTube->feature;	//读取钢管中以前变量m_bAttachPartByWeldRoad的值 现在该变量已去掉
			}
			if(pDatumTube->m_bHasWeldRoad&&J!=0)
				weldRoadAngle=pDatumTube->CalWeldRoadAngle();
			//-------------------------
			//优化环向布置肘板位置计算方法
			//旧:先计算出肘板偏移角度，根据角度计算出螺栓所在直线的
			//   方向(计算方向时坐标转换有问题),然后用钢管与钢板求交得到基准点
			//新:肘板偏移角度基准边方向应在基准钢管坐标系进行计算，但是偏移角度应该在钢板坐标系下计算
			//   先计算出基准边的方向然后绕钢板Z轴旋转得到肘板位置
			//-------------------------
			if(b==1)
				ucs.axis_x.Set(cos(weldRoadAngle),sin(weldRoadAngle),0);
			else
				ucs.axis_x.Set(cos(Y+weldRoadAngle),sin(Y+weldRoadAngle));
			ucs.axis_z.Set(-ucs.axis_x.y,ucs.axis_x.x);
			vector_trans(ucs.axis_x,pDatumTube->ucs,TRUE);
			vector_trans(ucs.axis_z,pDatumTube->ucs,TRUE);
			f3dPoint inters_vec=ucs.axis_z^pBottomPlate->ucs.axis_z;	//计算肘板与基准钢板间的相交边方向,即肘板最终X轴方向
			normalize(inters_vec);
			normalize(ucs.axis_x);
			double cosa=ucs.axis_x*inters_vec;	//计算肘板初始及最终X轴方向间的余弦值
			if(cosa<0)
			{	//修正最终X轴方向的正负方向
				ucs.axis_x=-inters_vec;
				cosa*=-1.0;
			}
			else
				ucs.axis_x=inters_vec;	//将肘板最终X轴方向赋值给肘板装配坐标系
			//根据偏移角度调整基准方向
			if(b==1)
				RotateVectorAroundVector(ucs.axis_x,Y,pBottomPlate->ucs.axis_z);
			f3dPoint face_pick=GetPlateDatumFacePickPt(pBottomPlate,Z==1);
			/*f3dPoint face_pick=pBottomPlate->ucs.origin;
			if(pBottomPlate->GetClassTypeId()==CLS_PLATE)
			{
				face_pick+=pBottomPlate.PlatePointer()->m_fNormOffset*pBottomPlate->ucs.axis_z;
				if(Z==1) //在钢板正面布置肘板
					face_pick+=pBottomPlate.PlatePointer()->GetThick()*pBottomPlate->ucs.axis_z;
			}*/
			//计算坐标系原点
			double fRadius=pDatumTube->GetDiameter()*0.5;
			f3dPoint inters1,inters2,line_pick,temp_vec;
			Int3dlf(line_pick,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pBottomPlate->ucs.axis_z);
			if(Int3dlc(&inters1,&inters2,line_pick,ucs.axis_x,pDatumTube->Start(),pDatumTube->End(),fRadius,fRadius)!=2)
			{
				logerr.Log("肘坐标原点计算失败！");
				return FALSE;
			}
			temp_vec=inters1-inters2;
			normalize(temp_vec);
			if(temp_vec*ucs.axis_x>0)
				ucs.origin=inters1;
			else
				ucs.origin=inters2;
			//调整坐标系各方向轴
			ucs.axis_z = ucs.axis_x^line_vec;
			normalize(ucs.axis_z);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
			if(pBottomPlate->IsPlate())
			{
				if((Z==1&&pBottomPlate->ucs.axis_z*ucs.axis_y<0)||
					(Z==-1&&pBottomPlate->ucs.axis_z*ucs.axis_y>0))
				{	//调整肘板坐标系
					ucs.axis_y*=-1.0;
					ucs.axis_z=ucs.axis_x^ucs.axis_y;
					normalize(ucs.axis_z);
				}
			}
		}
		else if(pDatumTube&&pBottomPlate.IsHasPtr()&&J==2&&pDatumBolt)
		{	//根据定位螺栓确定底边走向
			f3dPoint perp,inters,inters1,inters2;
			if(pBottomPlate->FindLsByHandle(hBtmBolt))
				pBottomPlate->GetBoltIntersPos(pDatumBolt,inters);
			else
				inters=pDatumBolt->ucs.origin;
			SnapPerp(&perp,pDatumTube->Start(),pDatumTube->End(),inters);
			ucs.axis_x=inters-perp;
			if(ucs.axis_x.IsZero())
				ucs.axis_x=inters_vec(line_vec);
			project_vector(ucs.axis_x,pBottomPlate->ucs.axis_z);	//保证X轴在底板平面上
			Standarized(ucs.axis_x);
			ucs.axis_z=ucs.axis_x^line_vec;
			Standarized(ucs.axis_z);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			//确定角肋板的角点即原点
			double radius=pDatumTube->GetDiameter()*0.5;
			f3dPoint line_pick,face_pick=GetPlateDatumFacePickPt(pBottomPlate,Z==1);
			Int3dlf(line_pick,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pBottomPlate->ucs.axis_z);
			if(Int3dlc(&inters1,&inters2,line_pick,ucs.axis_x,pDatumTube->Start(),pDatumTube->End(),radius,radius)!=2)
			{
				logerr.Log("肋板坐标原点计算失败！");
				return FALSE;
			}
			f3dPoint temp_vec=inters1-inters2;
			normalize(temp_vec);
			if(temp_vec*ucs.axis_x>0)
				ucs.origin=inters1;
			else
				ucs.origin=inters2;
		}
		else if(pDatumTube&&pBottomPlate.IsHasPtr()&&J==3)
		{	//根据指定的参照方向(全局坐标系)确定底边走向
			f3dPoint inters1,inters2;
			f3dPoint line_pick,face_pick=GetPlateDatumFacePickPt(pBottomPlate,Z==1);
			Int3dlf(line_pick,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pBottomPlate->ucs.axis_z);
			ucs.axis_x=vGloblBtmEdgeVec;
			project_vector(ucs.axis_x,pBottomPlate->ucs.axis_z);	//保证X轴在底板平面上
			if(ucs.axis_x.IsZero())
			{
				f3dPoint axisz=line_vec^pBottomPlate->ucs.axis_z;
				ucs.axis_x=axisz^pBottomPlate->ucs.axis_z;
			}
			Standarized(ucs.axis_x);
			ucs.axis_z=ucs.axis_x^line_vec;
			Standarized(ucs.axis_z);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			//确定角肋板的角点即原点
			double radius=pDatumTube->GetDiameter()*0.5;
			if(Int3dlc(&inters1,&inters2,line_pick,ucs.axis_x,pDatumTube->Start(),pDatumTube->End(),radius,radius)!=2)
			{
				logerr.Log("肋板坐标原点计算失败！");
				return FALSE;
			}
			f3dPoint temp_vec=inters1-inters2;
			normalize(temp_vec);
			if(temp_vec*ucs.axis_x>0)
				ucs.origin=inters1;
			else
				ucs.origin=inters2;
		}
		else if(pDatumTube&&pBottomPlate.IsHasPtr()&&J==4)
		{	//根据底边上的拾取点(底板上局部坐标定位点)确定底边走向
			//确定钢管中心轴线与底板当前焊接面的交点
			f3dPoint line_pick,face_pick=GetPlateDatumFacePickPt(pBottomPlate,Z==1);
			Int3dlf(line_pick,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pBottomPlate->ucs.axis_z);
			//确定底边走向(即X轴方向)
			coord_trans(vLocalBtmEdgeVecPick,pBtmPlate->ucs,TRUE,TRUE);
			ucs.axis_x=vLocalBtmEdgeVecPick-line_pick;
			project_vector(ucs.axis_x,pBottomPlate->ucs.axis_z);	//保证X轴在底板平面上
			if(ucs.axis_x.IsZero())
			{
				f3dPoint axisz=line_vec^pBottomPlate->ucs.axis_z;
				ucs.axis_x=axisz^pBottomPlate->ucs.axis_z;
			}
			Standarized(ucs.axis_x);
			ucs.axis_z=ucs.axis_x^line_vec;
			Standarized(ucs.axis_z);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			//确定角肋板的角点即原点
			f3dPoint inters1,inters2;
			double radius=pDatumTube->GetDiameter()*0.5;
			if(Int3dlc(&inters1,&inters2,line_pick,ucs.axis_x,pDatumTube->Start(),pDatumTube->End(),radius,radius)!=2)
			{
				logerr.Log("肋板坐标原点计算失败！");
				return FALSE;
			}
			f3dPoint temp_vec=inters1-inters2;
			normalize(temp_vec);
			if(temp_vec*ucs.axis_x>0)
				ucs.origin=inters1;
			else
				ucs.origin=inters2;
		}
		else
		{	//参数不完备以原有Ｘ轴方向及原点位置为基准
			ucs.axis_z=ucs.axis_x^line_vec;
			normalize(ucs.axis_z);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
		}
	}
	f3dPoint tube_vec,vertice;
	double radiusRatio=0;
	if(pDatumTube&&pBottomPlate.IsHasPtr())
	{	
		tube_vec=pDatumTube->Start()-pDatumTube->End();
		normalize(tube_vec);
		radiusRatio=fabs(tube_vec*pBottomPlate->ucs.axis_z);
	}
	if(margin>=0&&pBottomPlate.IsHasPtr())
	{	//自动延伸到边
		f3dPoint origin=ucs.origin;
		f3dPoint axisx=ucs.axis_x;
		coord_trans(origin,pBottomPlate->ucs,FALSE,TRUE);
		vector_trans(axisx,pBottomPlate->ucs,FALSE,TRUE);
		CLDSGeneralPlate* pBtmPlate=pBottomPlate.GeneralPlatePointer();
		f3dArcLine arcline;
		GEPOINT inters,inters2;
		f2dRay axisxray(origin,axisx);
		f2dLine axisxline(origin,origin+axisx*10000);
		double inner_r=0,outer_r=0,tube_r=pDatumTube->GetDiameter()*0.5;
		PROFILE_VERTEX* pVertex=NULL,*pPrevVertex=pBtmPlate->vertex_list.GetTail();
		for(pVertex=pBtmPlate->vertex_list.GetFirst();pVertex;pVertex=pBtmPlate->vertex_list.GetNext())
		{
			if(pPrevVertex->type==0)
			{
				pPrevVertex=pVertex;
				continue;
			}
			double cur_r=0;
			if(pPrevVertex->type==1)
			{	//指定圆弧夹角
				int sign=1;
				f3dArcLine arcline;
				if(pPrevVertex->sector_angle<0)
					sign=-1;
				arcline.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPrevVertex->sector_angle);
				cur_r=arcline.Radius();
			}
			else 
				cur_r=fabs(pPrevVertex->radius);
			if(fabs(cur_r-tube_r)<10)	//10为最大的加工间隙值
				inner_r=cur_r;
			else 
				outer_r=max(outer_r,cur_r);
			pPrevVertex=pVertex;
		}
		if(outer_r>tube_r)
		{	//外圆弧
			if(inner_r>tube_r)
				B=outer_r-inner_r;
			else
				B=outer_r-tube_r;
		}
	}
	if(V==1&&pDatumTube&&pBottomPlate.IsHasPtr()&&radiusRatio>EPS2&&radiusRatio<EPS_COS)
	{	//竖直且带自动正位的肘板	
		double radius=pDatumTube->GetDiameter()*0.5;
		f3dPoint datum_point,line_pick,inters1,inters2;
		line_pick.Set(0,A,0);
		coord_trans(line_pick,ucs,TRUE);
		line_vec=ucs.axis_x;
		if(Int3dlc(&inters1,&inters2,line_pick,line_vec,pDatumTube->Start(),pDatumTube->End(),radius,radius)>0)
		{
			f3dPoint axis_x=inters2-inters1;
			normalize(axis_x);
			if(axis_x*ucs.axis_x>0)
				datum_point=inters2;
			else 
				datum_point=inters1;
		}
		else //无交点时取直线的平行线与椭圆的切点作为交点	
		{	
			double majorAxisLen=radius/radiusRatio;
			UCS_STRU ellipse_ucs;	//椭圆坐标系
			f3dPoint face_pick=pBottomPlate->ucs.origin;
			if(pBottomPlate->GetClassTypeId()==CLS_PLATE)
				face_pick+=pBottomPlate.PlatePointer()->m_fNormOffset*pBottomPlate->ucs.axis_z;
			Int3dlf(ellipse_ucs.origin,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pBottomPlate->ucs.axis_z);
			if(tube_vec*ucs.axis_y<0)
				tube_vec*=-1.0;
			ellipse_ucs.origin+=(tube_vec*A/radiusRatio);
			ellipse_ucs.axis_z=pBottomPlate->ucs.axis_z;
			ellipse_ucs.axis_y=tube_vec^pBottomPlate->ucs.axis_z;
			normalize(ellipse_ucs.axis_y);
			ellipse_ucs.axis_x=ellipse_ucs.axis_y^ellipse_ucs.axis_z;
			normalize(ellipse_ucs.axis_x);
			f3dPoint tangPt1,tangPt2;
			f3dPoint vec=ucs.axis_x;
			vector_trans(vec,ellipse_ucs,FALSE);
			if(Tang2dle(vec,majorAxisLen,radius,tangPt1,tangPt2))
			{	//存在合理切点
				coord_trans(tangPt1,ellipse_ucs,TRUE);
				coord_trans(tangPt2,ellipse_ucs,TRUE);
				datum_point.Set(0,A,0);
				coord_trans(datum_point,ucs,TRUE);
				if(DISTANCE(datum_point,tangPt1)<DISTANCE(datum_point,tangPt2))
					datum_point=tangPt1;
				else 
					datum_point=tangPt2;
				//肘板坐标系需要调整 Y轴方向保持不变
				f3dPoint axis_y=datum_point-ucs.origin;
				normalize(axis_y);
				f3dPoint axis_z=axis_y^ucs.axis_y;
				if(ucs.axis_z*axis_z<0)
					ucs.axis_z=-1.0*axis_z;
				else
					ucs.axis_z=axis_z;
				normalize(ucs.axis_z);
				ucs.axis_x=ucs.axis_y^ucs.axis_z;
				normalize(ucs.axis_x);
			}
			else //不存在合理切点
			{
				datum_point.Set(0,A,0);
				coord_trans(datum_point,ucs,TRUE);
			}
		}
		coord_trans(datum_point,ucs,FALSE);
		datum_point.z=0;
		vertice.Set(E,0,0);
		PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
		pVertex->m_bWeldEdge=TRUE; 
		vertice.Set(B,0,0);
		vertex_list.append(vertice);
		if((D+datum_point.x)<B)	//避免出现凹形板
		{
			vertice.Set(B,C,0);
			vertex_list.append(vertice);
		}
		vertice.Set(D+datum_point.x,A,0);
		vertex_list.append(vertice);

		f3dLine line(datum_point,f3dPoint(0,0,0));
		coord_trans(line.startPt,ucs,TRUE);
		coord_trans(line.endPt,ucs,TRUE);
		line_vec=line.startPt-line.endPt;
		normalize(line_vec);
		if(Int3dlc(&inters1,&inters2,line.startPt,line_vec,pDatumTube->Start(),pDatumTube->End(),radius,radius)>0)
		{	//圆弧边
			pVertex=vertex_list.append(datum_point);
			pVertex->type=3;
			pVertex->column_norm=pDatumTube->End()-pDatumTube->Start();
			normalize(pVertex->column_norm);
			pVertex->radius=-pDatumTube->GetDiameter()*0.5;	//肘板上圆弧一定是内凹椭圆弧，故应给负值
			pVertex->center=pDatumTube->Start();
			pVertex->m_bWeldEdge=TRUE;
			if(fabs(E)>eps)
			{
				f3dPoint e_vec(1,-1,0);
				f3dPoint startPt(E,0,0);
				coord_trans(startPt,ucs,TRUE);
				vector_trans(e_vec,ucs,TRUE);
				if(Int3dlc(&inters1,&inters2,startPt,e_vec,pDatumTube->Start(),pDatumTube->End(),radius,radius)>0)
				{
					f3dPoint inters_pos;
					if(DISTANCE(inters1,startPt)<DISTANCE(inters2,startPt))
						inters_pos=inters1;
					else
						inters_pos=inters2;
					coord_trans(inters_pos,ucs,FALSE);
					//应该确保Y坐标为E 暂时保证该点位于圆弧上
					//vertice.Set(E*datum_point.x/datum_point.y,E,0);
					vertice.Set(inters_pos.x,inters_pos.y);
					vertex_list.append(vertice);
				}
			}
		}
		else
		{	//无圆弧边
			vertice=datum_point;
			pVertex=vertex_list.append(vertice);
			pVertex->m_bWeldEdge=TRUE;
			if(fabs(E)>eps)
			{
				double k=0;
				if(fabs(datum_point.y)>eps)
					k=datum_point.x/datum_point.y;
				vertice.Set(E*k,E,0);
				pVertex=vertex_list.append(vertice);
				if(e==1)
				{
					pVertex->type=2;//指定圆弧半径
					pVertex->radius=-E;
				}
			}
		}
	}
	else
	{	//沿钢管方向的肘板
		vector_trans(line_vec,ucs,FALSE);
		double k=0;
		if(fabs(line_vec.y)>eps)	//防止line_vec.y=0 
			k=line_vec.x/line_vec.y;
		vertice.Set(E,0,0);
		PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
		pVertex->m_bWeldEdge=TRUE; 
		vertice.Set(B,0,0);
		vertex_list.append(vertice);
		vertice.Set(B,C,0);
		vertex_list.append(vertice);
		vertice.Set(D+A*k,A,0);
		vertex_list.append(vertice);
		vertice.Set(A*k,A,0);
		pVertex=vertex_list.append(vertice);
		pVertex->m_bWeldEdge=TRUE;
		if(fabs(E)>eps)
		{
			vertice.Set(E*k,E,0);
			pVertex=vertex_list.append(vertice);
			if(e==1)
			{
				pVertex->type=2;//指定圆弧半径
				pVertex->radius=-E;
			}
		}
	}
	return TRUE;
}
BOOL CLDSParamPlate::DesignRibCornerOrSlotPlate(CLDSGeneralPlate* pDatumPlate,CLDSParamPlate::RIBPLATE_CORNER& corner)
{	//普通钢板夹角处的角肋板或三块钢板间组成的槽内肋板
	CLDSNode *pDatumNode=BelongModel()->FromNodeHandle(corner.hNode.hValue);
	//pDatumPlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(abs(corner.hDatumPlate.hValue),CLS_LINETUBE,CLS_PLATE,CLS_PARAMPLATE);
	CLDSGeneralPlate* pAssistantPart1=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(abs(corner.hAssisPlate1.hValue),CLS_PLATE,CLS_PARAMPLATE);
	CLDSGeneralPlate* pAssistantPart2=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(abs(corner.hAssisPlate2.hValue),CLS_PLATE,CLS_PARAMPLATE);
	long hAssPart1=corner.hAssisPlate1.hValue;
	long hAssPart2=corner.hAssisPlate2.hValue;
	bool blSideEdgeLengthByHeight=false;
	if(corner.blSideEdgeLength.bExist)
		blSideEdgeLengthByHeight=(corner.blSideEdgeLength.iValue!=0);
	if(pAssistantPart1&&pAssistantPart1->IsPlate()&&pAssistantPart2&&pAssistantPart2->IsPlate())
	{	//纠正坐标系Y轴方向,考虑法线偏移量 wht 11-05-24
		f3dPoint pt1=GetPlateDatumFacePickPt(pAssistantPart1,FALSE);
		f3dPoint pt2=GetPlateDatumFacePickPt(pAssistantPart2,FALSE);
		f3dPoint verfiy_vec=pt2-pt1;
		if(verfiy_vec*pAssistantPart1->ucs.axis_z>0)
			hAssPart1=abs(hAssPart1);	//肋板在辅助钢板顶面一侧
		else 
			hAssPart1=-abs(hAssPart1);	//肋板在辅助钢板底面一侧
		if(verfiy_vec*pAssistantPart2->ucs.axis_z<0)
			hAssPart2=abs(hAssPart2);
		else 
			hAssPart2=-abs(hAssPart2);
	}
	f3dPoint vertice;
	//基准构件为钢板
	if(pAssistantPart1&&!pAssistantPart1->IsPlate())
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("Ancillary part 1 must be plate！");
#else
			log2file->Log("辅助构件1必须为钢板！");
#endif
		return FALSE;	//辅助构件1不是钢板
	}
	if(pAssistantPart2&&!pAssistantPart2->IsPlate())
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("Ancillary part 2 must be palte！");
#else
			log2file->Log("辅助构件2必须为钢板！");
#endif
		return FALSE;	//辅助构件2不是钢板
	}
	CLDSGeneralPlate* pAssisPlate1=(CLDSGeneralPlate*)pAssistantPart1;
	CLDSGeneralPlate* pAssisPlate2=(CLDSGeneralPlate*)pAssistantPart2;
	long hBolt=corner.hBolt.ToLong();			//基点定位螺栓
	long hVecPart=corner.hVecPart.ToLong();		//X轴或Y轴方向基准杆件
	double fOffsetX=corner.fOffsetX.value.fValue;	//X方向偏移量
	double fOffsetZ=corner.fOffsetZ.value.fValue;	//Z方向偏移量
	CLDSBolt *pBolt=(CLDSBolt*)BelongModel()->FromPartHandle(hBolt,CLS_BOLT);
	CLDSPart *pVecPart=(CLDSPart*)BelongModel()->FromPartHandle(abs(hVecPart));
	if(pVecPart&&!pVecPart->IsLinePart()&&pVecPart->GetClassTypeId()!=CLS_BOLT)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("Direction ancillary part must be rod or bolt(on datum plate or ancillary plate)！");
#else
			log2file->Log("方向辅助构件必须为杆件或(基板或辅助钢板上的)螺栓！");
#endif
		return FALSE;
	}
	BOOL bSpecOrg=true,bSpecAxisX=true,bSpecAxisY=false;
	//1.计算肋板坐标系原点位置
	BOOL bSpecRayAngle=corner.rayAngle.bExist;
	BOOL bSpecRayDist=corner.rayDist.bExist;;
	double fRayAngle=corner.rayAngle.value.fValue;
	double fRayDist=corner.rayDist.value.fValue;	//射线角度为角度制
	f3dPoint ray_vec(1,0,0);				//底板坐标系下的相对方向
	if(bSpecRayAngle)
	{	//指定射线角
		RotateVectorAroundVector(ray_vec,fRayAngle*RADTODEG_COEF,f3dPoint(0,0,1));
		normalize(ray_vec);
		if(bSpecRayDist&&pAssisPlate1&&pAssisPlate2)
		{	//根据射线角、偏移距离计算肋板在辅助钢板内侧还是外侧
			f3dPoint verify_pt=ray_vec*fRayDist;
			coord_trans(verify_pt,pDatumPlate->ucs,TRUE);
			f3dPoint face1_pick=verify_pt,face2_pick=verify_pt;
			project_point(face1_pick,pAssisPlate1->ucs.origin,pAssisPlate1->ucs.axis_z);
			project_point(face2_pick,pAssisPlate2->ucs.origin,pAssisPlate2->ucs.axis_z);
			f3dPoint verify_vec=verify_pt-face1_pick;
			if(verify_vec*pAssisPlate1->ucs.axis_z>0)
				hAssPart1=abs(hAssPart1);
			else 
				hAssPart1=-abs(hAssPart1);
			verify_vec=verify_pt-face2_pick;
			if(verify_vec*pAssisPlate2->ucs.axis_z>0)
				hAssPart2=abs(hAssPart2);
			else 
				hAssPart2=-abs(hAssPart2);
		}
	}
	btc::RAYLINE cornerLine1,cornerLine2;
	btc::WORKPLANE datumPlane,assisPlane1,assisPlane2;
	datumPlane.pick=GetPlateDatumFacePickPt(pDatumPlate,corner.hDatumPlate.hValue>0);
	if(pAssistantPart1)
	{
		assisPlane1.pick=GetPlateDatumFacePickPt(pAssistantPart1,hAssPart1>0);
		_Int3d2f(cornerLine1.pick,cornerLine1.vec,datumPlane.pick,pDatumPlate->ucs.axis_z,assisPlane1.pick,pAssistantPart1->ucs.axis_z);
	}
	if(pAssistantPart2)
	{
		assisPlane2.pick=GetPlateDatumFacePickPt(pAssistantPart2,hAssPart2>0);
		_Int3d2f(cornerLine2.pick,cornerLine2.vec,datumPlane.pick,pDatumPlate->ucs.axis_z,assisPlane2.pick,pAssistantPart2->ucs.axis_z);
	}
	double Ox=corner.Ox.value.fValue,Oy=corner.Oy.value.fValue;	//定位原点在底板上的相对坐标（与定位螺栓二选一）
	bSpecOrg=corner.Ox.bExist&&corner.Oy.bExist;
	long hRelatedAngle=corner.hRelatedAngle.hValue;	//肋板关联角钢
	CLDSLineAngle *pRelatedAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(hRelatedAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(bSpecRayAngle)
	{	//指定射线角
		if(bSpecRayDist)	//指定射线距离，圆形塔脚肋板（射线为肋板Z轴方向）
		{
			ucs.origin=ray_vec*fRayDist;
			if(pAssisPlate1&&pAssisPlate2)
				corner.C.value=corner.A.value;	//圆形塔脚肋板台阶高度等于肋板高度
		}
		else
		{	//圆形塔脚对角肋板(射线方向为肋板X轴方向),坐标原点根据关联钢板、关联角钢确定	
			double origin_offset=0;
			if(pRelatedAngle)
			{
				double jg_space=pAssisPlate1->Thick,angle_r=FindJg_r(pRelatedAngle->GetWidth());
				if(pRelatedAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
					jg_space=((CLDSGroupLineAngle*)pRelatedAngle)->jg_space;
				origin_offset=(jg_space*0.5+pRelatedAngle->Thick)/cos45+CalJgROffset(angle_r,thick);
			}
			else 
				origin_offset=thick*0.5+pAssisPlate1->Thick/cos45*0.5;
			ucs.origin=ray_vec*origin_offset;
			if(pAssisPlate1&&pAssisPlate2)	//对角肋板延伸至塔脚板边缘
				corner.B.value.fValue=pDatumPlate->GetWidth()*0.5-origin_offset;
		}
		coord_trans(ucs.origin,pDatumPlate->ucs,TRUE);
		project_point(ucs.origin,datumPlane.pick,pDatumPlate->ucs.axis_z);//螺栓坐标系原点向基准钢板基准面投影
	}
	else if(bSpecOrg)
	{
		ucs.origin.Set(Ox,Oy);
		coord_trans(ucs.origin,pDatumPlate->ucs,TRUE);
		project_point(ucs.origin,datumPlane.pick,pDatumPlate->ucs.axis_z);//螺栓坐标系原点向基准钢板基准面投影
	}
	else if(pBolt)
	{	//钢板肋板
		ucs.origin=pBolt->ucs.origin;
		if(pDatumPlate->FindLsByHandle(pBolt->handle)||fabs(pBolt->ucs.axis_z*pDatumPlate->ucs.axis_z)>EPS_COS)
			project_point(ucs.origin,datumPlane.pick,pBolt->ucs.axis_z);//螺栓坐标系原点向基准钢板基准面投影
		else if(pAssisPlate1&&pAssisPlate1->FindLsByHandle(pBolt->handle)||fabs(pBolt->ucs.axis_z*pAssisPlate1->ucs.axis_z)>EPS_COS)
			project_point(ucs.origin,assisPlane1.pick,pBolt->ucs.axis_z);//螺栓坐标系原点向基准钢板基准面投影
		else
		{
			if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("Datum bolt doesn't on datum base plate or ancillary palte！");
#else
				log2file->Log("基准螺栓不在基准底板或辅助钢板内！");
#endif
			ucs.origin=pBolt->ucs.origin;
		}
	}
	else if(pDatumNode)
	{
		ucs.origin=pDatumNode->Position(true);
		project_point(ucs.origin,datumPlane.pick,pDatumPlate->ucs.axis_z);//螺栓坐标系原点向基准钢板基准面投影
	}
	else
		return FALSE;
	//2.计算肋板装配坐标系坐标轴方向
	//2.1计算Y轴初始默认方向
	ucs.axis_y=pDatumPlate->ucs.axis_z;
	if(corner.hDatumPlate.hValue<0)
		ucs.axis_y*=-1.0;
	double VXx=corner.Xx.value.fValue;
	double VXy=corner.Xy.value.fValue;	//X轴在底板上的相对坐标
	bSpecAxisX=corner.Xx.bExist&&corner.Xy.bExist;
	if(bSpecAxisX)
	{
		ucs.axis_x.Set(VXx,VXy);
		normalize(ucs.axis_x);
		vector_trans(ucs.axis_x,pDatumPlate->ucs,TRUE);
	}
	double fGradientDegAngle=corner.fGradientDegAngle.value.fValue;	//与底板倾斜角度（即Y轴与底板间绕X轴的旋转角与方向基准杆件二选一）
	if(corner.fGradientDegAngle.bExist)
	{
		bSpecAxisY=true;
		if(fGradientDegAngle!=90)
		{	//修正肋板有倾斜角时的Y轴方向
			double fRadAngle=(fGradientDegAngle-90)*RADTODEG_COEF;
			RotateVectorAroundVector(ucs.axis_y,fRadAngle,ucs.axis_x);
		}
	}
	if(pAssistantPart1==NULL&&pAssistantPart2==NULL)
	{	//钢板肋板
		if(!bSpecAxisX&&pVecPart==NULL)
			return FALSE;
		f3dPoint axis_x;
		if(pVecPart->IsLinePart()&&!bSpecAxisX)
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)pVecPart;
			axis_x=pLinePart->End()-pLinePart->Start();
			normalize(axis_x);
			if(hVecPart<0)	//用定位杆件句柄正负表示钢板延伸方向
				axis_x*=-1.0;
		}
		else if(pVecPart->GetClassTypeId()==CLS_BOLT&&!bSpecAxisX)
		{
			if(pBolt)
				axis_x=pVecPart->ucs.origin-pBolt->ucs.origin;
			else if(pDatumNode)
				axis_x=pVecPart->ucs.origin-pDatumNode->Position(true);
		}
		if(axis_x.IsZero())	//计算得到的X轴方向为空取基准钢板的X轴方向为默认X轴方向
			axis_x=pDatumPlate->ucs.axis_x;
		ucs.axis_z=axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		ucs.origin+=ucs.axis_x*fOffsetX;	//钢板肋板需要X轴方向偏移量
		ucs.origin+=ucs.axis_z*fOffsetZ;
		corner.C.value=corner.A.value;//fTopWidth=fBottomWidth;	//顶部宽度=底部宽度
		//添加钢板轮廓点
		vertice.Set(0,0,0);
		PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
		pVertex->m_bWeldEdge=TRUE;
		vertice.Set(corner.B.value.fValue,0,0);
		vertex_list.append(vertice);
		vertice.Set(corner.B.value.fValue,corner.C.value.fValue,0);
		vertex_list.append(vertice);
		if(fabs(corner.C.value.fValue-corner.A.value.fValue)>EPS_COS)
		{
			if(corner.C.value.fValue>corner.A.value.fValue)		//顶部宽度大于底部宽度
				vertice.Set(corner.B.value.fValue-corner.D.value.fValue,corner.C.value.fValue,0);
			else if(corner.C.value.fValue<corner.A.value.fValue)	//顶部宽度小于底部宽度
				vertice.Set(corner.D.value.fValue,corner.A.value.fValue,0);
			vertex_list.append(vertice);
		}
		vertice.Set(0,corner.A.value.fValue,0);
		vertex_list.append(vertice);
	}
	else if(pAssistantPart1)
	{	//角肋板或槽肋板坐标系计算
		if(bSpecRayAngle)
		{
			if(bSpecRayDist)	
			{	//指定射线距离，圆形塔脚肋板（射线为肋板Z轴方向）
				ucs.axis_z=ray_vec;
				vector_trans(ucs.axis_z,pDatumPlate->ucs,TRUE);
				ucs.axis_x=ucs.axis_y^ucs.axis_z;
			}
			else
			{	//圆形塔脚对角肋板(射线方向为肋板X轴方向),坐标原点根据关联钢板、关联角钢确定
				ucs.axis_x=ray_vec;
				vector_trans(ucs.axis_x,pDatumPlate->ucs,TRUE);
				ucs.axis_z=ucs.axis_x^ucs.axis_y;
			}
			normalize(ucs.axis_x);
			normalize(ucs.axis_z);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
		}
		else if(!bSpecAxisX)
		{	//此种坐标轴方向定义能保证肋板与角焊缝线垂直 wjh-2015.1.21
			ucs.axis_x=pAssistantPart1->ucs.axis_z;
			if(hAssPart1<0)
				ucs.axis_x*=-1.0;
			//令X轴方向贴合基准钢板平面内 wjh-2015.1.29
			project_vector(ucs.axis_x,pDatumPlate->ucs.axis_z);
		}
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		if(!bSpecAxisY)
		{	//未通过倾斜角指定肋板Y轴方向时，需要进一步确认Y轴方向 wjh-2015.1.21
			if(bSpecRayAngle&&!bSpecRayDist)
			{
				f3dPoint axis_y=pAssisPlate1->ucs.axis_z^pAssisPlate2->ucs.axis_z;
				if(axis_y*ucs.axis_y<0)
					ucs.axis_y=-1*axis_y;
				else 
					ucs.axis_y=axis_y;
				ucs.axis_z=ucs.axis_x^ucs.axis_y;
				normalize(ucs.axis_z);
				ucs.axis_y=ucs.axis_z^ucs.axis_x;
			}
			else if(pVecPart&&pVecPart->IsLinePart())
			{	//钢板角肋板或槽肋板，存在Y方向定位杆件,纠正Y轴方向
				CLDSLinePart *pLinePart=(CLDSLinePart*)pVecPart;
				f3dPoint line_vec=pLinePart->End()-pLinePart->Start();
				if(line_vec*ucs.axis_y<0)
					ucs.axis_y=line_vec*-1.0;
				else 
					ucs.axis_y=line_vec;
				normalize(ucs.axis_y);
				if(bSpecRayAngle&&bSpecRayDist)
				{
					ucs.axis_z=ray_vec;
					vector_trans(ucs.axis_z,pDatumPlate->ucs,TRUE);
					ucs.axis_x=ucs.axis_y^ucs.axis_z;
					double dd=ucs.axis_x*pAssisPlate1->ucs.axis_z;
					if((hAssPart1>0&&dd<0)||(hAssPart1<0&&dd>0))
						ucs.axis_x*=-1;
					ucs.axis_z=ucs.axis_x^ucs.axis_y;
					normalize(ucs.axis_x);
					normalize(ucs.axis_z);
				}
				else 
				{
					ucs.axis_z=ucs.axis_x^ucs.axis_y;
					normalize(ucs.axis_z);
					ucs.axis_y=ucs.axis_z^ucs.axis_x;
				}
			}
			else	//未通过倾斜角指定肋板Y轴方向时，应根据X及Z轴向反向修正Y轴方向 wjh-2015.1.21
				ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
		}
		//肋板基准面与基准钢板基准面求交线
		f3dPoint along_axisvec=pAssistantPart1->ucs.axis_z^ucs.axis_z;	//沿1号辅板的角焊缝走向
		if(along_axisvec*ucs.axis_y<0)
			along_axisvec*=-1.0;
		//角肋板原点应位角焊缝线上
		if(bSpecRayAngle&&!bSpecRayDist)	//求交计算坐标原点位置
		{
			f3dPoint datumPlaneOrg=pDatumPlate->ucs.origin;
			if(corner.hDatumPlate.hValue>0)		//基准角钢在底板上侧
				datumPlaneOrg+=pDatumPlate->ucs.axis_z*(pDatumPlate->GetNormOffset()+pDatumPlate->GetThick());
			else 					//基准角钢在底板下侧
				datumPlaneOrg+=pDatumPlate->ucs.axis_z*pDatumPlate->GetNormOffset();
			Int3dlf(ucs.origin,ucs.origin,along_axisvec,datumPlaneOrg,pDatumPlate->ucs.axis_z);
			//根据两块辅助钢板计算肋板高度profile.corner.A
			CLDSGeneralPlate *platePtrArr[2]={pAssisPlate1,pAssisPlate2};
			for(int i=0;i<2;i++)
			{
				if(platePtrArr[i]==NULL)
					continue;
				f2dLine line;
				f3dPoint inters_pt;
				inters_pt=datumPlaneOrg+along_axisvec*50;
				coord_trans(inters_pt,platePtrArr[i]->ucs,FALSE);
				line.startPt.Set(inters_pt.x,inters_pt.y);
				inters_pt=datumPlaneOrg+along_axisvec*100000;
				coord_trans(inters_pt,platePtrArr[i]->ucs,FALSE);
				line.endPt.Set(inters_pt.x,inters_pt.y);
				PROFILE_VERTEX *pVertex=NULL,*pPrvVertex=platePtrArr[i]->vertex_list.GetTail();
				for(pVertex=platePtrArr[i]->vertex_list.GetFirst();pVertex;pVertex=platePtrArr[i]->vertex_list.GetNext())
				{
					if(Int2dll(line,f2dLine(pPrvVertex->vertex,pVertex->vertex),inters_pt.x,inters_pt.y)>0)
						break;	
					pPrvVertex=pVertex;
				}
				if(pVertex)
				{
					coord_trans(inters_pt,platePtrArr[i]->ucs,TRUE);
					corner.A.value.fValue=DISTANCE(inters_pt,datumPlaneOrg);
					break;
				}
			}
		}
		else
			Int3dlf(ucs.origin,cornerLine1.pick,cornerLine1.vec,ucs.origin,ucs.axis_z);
		//添加钢板轮廓点
		//坐标原定沿aixs_z偏移
		ucs.origin+=ucs.axis_z*fOffsetZ;
		vector_trans(along_axisvec,ucs,FALSE);
		normalize(along_axisvec);	//沿1号辅板的角焊缝走向 wjh-2015.1.21
		//角肋板
		if(pAssistantPart2==NULL||(pAssistantPart1&&pAssistantPart2&&bSpecRayAngle&&!bSpecRayDist))
		{	//角肋板支持切角参数 wht 11-05-24
			PROFILE_VERTEX *pVertex=NULL;
			f3dPoint pt0,pt1,pt2,pt3,pt4;
			pt0.Set(corner.E.ToDouble(),0,0);
			pVertex=vertex_list.append(pt0);
			pVertex->m_bWeldEdge=TRUE;
			pt1.Set(corner.B.value.fValue,0,0);
			vertex_list.append(pt1);
			pt2.Set(pt1.x,pt1.y+corner.C.value.fValue);
			vertex_list.append(pt2);
			double dfEdgeLength=corner.A.value.fValue;
			if (blSideEdgeLengthByHeight&&fabs(along_axisvec.y)>EPS)	//是否根据高度计算侧边长度
				dfEdgeLength=corner.A.value.fValue/fabs(along_axisvec.y);
			pt4=dfEdgeLength*along_axisvec;
			pt3.Set(pt4.x+corner.D.value.fValue,pt4.y);
			if(!pt2.IsEqual(pt3))
				vertex_list.append(pt3);
			pVertex=vertex_list.append(pt4);
			pVertex->m_bWeldEdge=TRUE;
			if(fabs(corner.E.ToDouble())>eps)
			{
				vertice=corner.E.ToDouble()*along_axisvec;
				pVertex=vertex_list.append(vertice);
				if(corner.e.value.iValue>0)
				{
					pVertex->type=2;
					pVertex->radius=-corner.E.ToDouble();
				}
			}
		}
		//槽型肋板
		else if(pAssistantPart2&&pAssistantPart2->IsPlate())
		{	//使用肋板与基准板的交线与基准钢板2基准面求交点
			double fCutAngleLen2=0;
			if(!GetDesignItemValue(KEY2C("I2"),&fCutAngleLen2))	//切角宽度2
				fCutAngleLen2=0;
			f3dPoint inters_pt1,inters_pt2,inters_pt3,inters_pt4;
			//以肋板工作平面与第二块辅助钢板的角焊缝线求交，更稳定 wjh-2015.1.29
			Int3dlf(inters_pt1,cornerLine2.pick,cornerLine2.vec,ucs.origin,ucs.axis_z);
			//沿2号辅板的角焊缝走向
			f3dPoint along_axisvec2=pAssistantPart2->ucs.axis_z^ucs.axis_z;	
			if(along_axisvec2*ucs.axis_y<0)
				along_axisvec2*=-1.0;
			normalize(along_axisvec2);
			inters_pt2=along_axisvec2*corner.C.value.fValue+inters_pt1;
			inters_pt4=along_axisvec2*fCutAngleLen2+inters_pt1;
			inters_pt3=along_axisvec *corner.A.value.fValue;
			coord_trans(inters_pt1,ucs,FALSE);
			coord_trans(inters_pt2,ucs,FALSE);
			coord_trans(inters_pt4,ucs,FALSE);
			inters_pt1.z=inters_pt2.z=inters_pt4.z=0;
			//
			vertice.Set(corner.E.ToDouble(),0,0);
			PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
			pVertex->m_bWeldEdge=TRUE;
			if(fabs(fCutAngleLen2)>0)
			{
				vertice.Set(inters_pt1.x-fCutAngleLen2,0,0);
				pVertex=vertex_list.append(vertice);
				if(corner.e.value.iValue>0)
				{
					pVertex->type=2;
					pVertex->radius=-fCutAngleLen2;
					pVertex->center=inters_pt1;
				}
			}
			pVertex=vertex_list.append(inters_pt4);
			pVertex->m_bWeldEdge=TRUE;
			vertex_list.append(inters_pt2);
			if(fabs(corner.C.value.fValue-corner.A.value.fValue)>EPS_COS)
			{
				f3dPoint axis_z;
				if(corner.C.value.fValue>corner.A.value.fValue)		//顶部宽度大于底部宽度
				{
					vertice=inters_pt2;
					axis_z=pAssistantPart2->ucs.axis_z;
					if(axis_z*ucs.axis_x>0)
						axis_z*=-1.0;
				}
				else if(corner.C.value.fValue<corner.A.value.fValue)
				{
					vertice=inters_pt3;
					axis_z=pAssistantPart1->ucs.axis_z;
					if(axis_z*ucs.axis_x<0)
						axis_z*=-1.0;
				}
				vector_trans(axis_z,ucs,FALSE);
				axis_z.z=0;
				vertice+=axis_z*corner.D.value.fValue;
				vertex_list.append(vertice);
			}
			vertice=inters_pt3;
			pVertex=vertex_list.append(vertice);
			pVertex->m_bWeldEdge=TRUE;
			if(fabs(corner.E.ToDouble())>eps)
			{
				vertice=corner.E.ToDouble()*along_axisvec;
				pVertex=vertex_list.append(vertice);
				if(corner.e.value.iValue>0)
				{
					pVertex->type=2;
					pVertex->radius=-corner.E.ToDouble();
				}
			}
		}
	}
	return TRUE;
}
//根据设计参数预计算参数化板的坐标轴方向及原点位置
BOOL CLDSParamPlate::PreCalPlateCS(double* axis_x/*=NULL*/,double* axis_y/*=NULL*/,double* axis_z/*=NULL*/,double* origin/*=NULL*/)
{
	if(m_iParamType==TYPE_RIBPLATE)
	{
		long hDatumPart=0,hAssPart1=0,hAssPart2=0;
		DESIGN_PLATE_ITEM* pItem=NULL;
		if(!GetDesignItemValue('A',&hDatumPart))//基准构件
			return FALSE;
		CSuperSmartPtr<CLDSPart> pDatumPart=BelongModel()->FromPartHandle(hDatumPart);
		if(pDatumPart.IsNULL())
			return FALSE;
		//GetDesignItemValue('B',&hNode,pItemHashTbl);		//基准节点
		GetDesignItemValue('C',&hAssPart1);	//辅助构件1
		GetDesignItemValue('D',&hAssPart2);	//辅助构件2
		//CLDSNode *pDatumNode=NULL;
		CLDSPart *pAssistantPart1=hAssPart1>0?BelongModel()->FromPartHandle(hAssPart1):NULL;
		CLDSPart *pAssistantPart2=hAssPart2>0?BelongModel()->FromPartHandle(hAssPart2):NULL;
		if( pDatumPart->IsPlate()&&pAssistantPart1==NULL&&pAssistantPart2==NULL)
		{	//钢板上的普通矩形肋板
			CLDSGeneralPlate* pDatumPlate=pDatumPart.GeneralPlatePointer();
			CLDSParamPlate::RIBPLATE_PLATE platerib;
			platerib.LoadParams(this);
			long hStartBolt=platerib.hStartBolt.hValue;			//基点定位螺栓
			long hVecPart=platerib.hVecPart.hValue;	//X轴或Y轴方向基准杆件
			CLDSBolt *pBolt=(CLDSBolt*)BelongModel()->FromPartHandle(hStartBolt,CLS_BOLT);
			CLDSPart *pVecPart=(CLDSPart*)BelongModel()->FromPartHandle(abs(hVecPart));
			if(pVecPart&&!pVecPart->IsLinePart()&&pVecPart->GetClassTypeId()!=CLS_BOLT)
			{
		#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->Log("Direction ancillary part must be rod or bolt(on datum plate or ancillary plate)！");
		#else
				Log2File()->Log("方向辅助构件必须为杆件或(基板或辅助钢板上的)螺栓！");
		#endif
				return FALSE;
			}
			//1.计算肋板坐标系原点位置
			//f3dPoint vertice;
			GEPOINT vOrigin,vAxisX,vAxisY,vAxisZ;
			BASERIBPLANE baseplane;//baseplane为Z向偏移前数据
			//由于后续X/Y轴方向计算需要原点,故无论如何都需要计算 wjh-2017.1.18
			double Ox=platerib.Ox.value.fValue;
			double Oy=platerib.Oy.value.fValue;	//定位原点在底板上的相对坐标（与定位螺栓二选一）
			bool bSpecOrg=platerib.Ox.bExist&&platerib.Oy.bExist;
			if(bSpecOrg)
				vOrigin.Set(Ox,Oy,pDatumPlate->GetNormOffset());
			else if(pBolt)
			{	//钢板肋板
				vOrigin=pBolt->ucs.origin;
				if(pDatumPlate->FindLsByHandle(pBolt->handle)||fabs(pBolt->ucs.axis_z*pDatumPlate->ucs.axis_z)>EPS_COS)
				{
					int iIntersPlane=platerib.hDatumPlate.hValue<=0 ? CLDSPart::DATUM_PLANE : CLDSGeneralPlate::TOP_PLANE;
					pDatumPlate->CalBoltIntersPos(pBolt,vOrigin,iIntersPlane,&baseplane.iResidePlane);
				}
				else
				{
#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("Datum bolt doesn't on datum base plate！");
#else
					Log2File()->Log("基准螺栓不在基准底板内！");
#endif
					vOrigin=pBolt->ucs.origin;
				}
			}
			else
				return FALSE;
			if(origin!=NULL)
			{
				origin[0]=vOrigin.x;
				origin[1]=vOrigin.y;
				origin[2]=vOrigin.z;
			}
			if(axis_y!=NULL)
			{
				PROFILE_VERTEX* pBendlineStart[2]={NULL},*pBendlineEnd[2]={NULL};
				if(pDatumPlate->GetFaceN()>=2&&baseplane.iResidePlane==-1)
					pDatumPlate->GetBendlineVertexies(&pBendlineStart[0],&pBendlineEnd[0],&pBendlineStart[1],&pBendlineEnd[1]);
				bool middlePick=true;	//暂时先不启用
				if(baseplane.iResidePlane==-1&&pBendlineStart[0]&&pBendlineEnd[0])//&&bSpecOrg)
				{
					double left_or_right=DistOf2dPtLine(f2dPoint(vOrigin.x,vOrigin.y),pBendlineStart[0]->vertex,pBendlineEnd[0]->vertex);
					//middlePick=(vpTipPoint-pBendlineStart[0]->vertex)*(vpTipPoint-pBendlineEnd[0]->vertex)<0;
					if(left_or_right<0&&middlePick)
						baseplane.iResidePlane=1;
				}
				if(baseplane.iResidePlane==-1&&pBendlineStart[1]!=NULL&&pBendlineEnd[1]!=NULL)
				{
					double left_or_right=DistOf2dPtLine(f2dPoint(vOrigin.x,vOrigin.y),pBendlineStart[1]->vertex,pBendlineEnd[1]->vertex);
					//middlePick=(vpTipPoint-pBendlineStart[1]->vertex)*(vpTipPoint-pBendlineEnd[1]->vertex)<0;
					if(left_or_right<0&&middlePick)
						baseplane.iResidePlane=2;
				}
				baseplane.iResidePlane=max(0,baseplane.iResidePlane);
				//2.计算肋板装配坐标系坐标轴方向
				//2.1计算Y轴初始默认方向
				vAxisY=baseplane.iResidePlane==0?pDatumPlate->ucs.axis_z:pDatumPlate->GetHuoquFaceNorm(baseplane.iResidePlane-1);
				if(platerib.hDatumPlate.hValue<0)
					vAxisY*=-1.0;
				axis_y[0]=vAxisY.x;
				axis_y[1]=vAxisY.y;
				axis_y[2]=vAxisY.z;
				//if(bSpecOrg)
				//{
				//	vOrigin=pDatumPlate->TransToActual(ucs.origin,baseplane.iResidePlane+1,FROM_ACS|TO_MCS|IS_VERTEX);
				//	if(platerib.hDatumPlate.hValue>0)
				//		vOrigin+=pDatumPlate->GetThick()*vAxisY;
				//}
			}
			if(axis_x!=NULL)
			{
				double VXx=platerib.Vx.value.fValue;
				double VXy=platerib.Vy.value.fValue;	//X轴在底板上的相对坐标
				bool bSpecAxisX=platerib.Vx.bExist&&platerib.Vy.bExist;
				double Ex=platerib.Ex.value.fValue;
				double Ey=platerib.Ey.value.fValue;
				bool bSpecEP=platerib.Ex.bExist&&platerib.Ey.bExist;
				if(bSpecAxisX)
				{	//直接指定基面走向
					vAxisX.Set(VXx,VXy);
					normalize(vAxisX);
					vAxisX=pDatumPlate->TransToActual(vAxisX,baseplane.iResidePlane+1,FROM_ACS|TO_MCS|IS_VECTOR);
					//vector_trans(ucs.axis_x,pDatumPlate->ucs,TRUE);
				}
				else if(bSpecEP)
				{	//指定基面走向终点
					GEPOINT vpEndPoint=pDatumPlate->TransToActual(GEPOINT(Ex,Ey,pDatumPlate->GetNormOffset()),baseplane.iResidePlane+1,FROM_ACS|TO_MCS|IS_VERTEX);
					if(platerib.hDatumPlate.hValue>0)
						vpEndPoint+=pDatumPlate->GetThick()*ucs.axis_y;
					vAxisX=vpEndPoint-vOrigin;
					//normalize(ucs.axis_x);
				}
				else if(pVecPart==NULL)
					return FALSE;
				else if(pVecPart->IsLinePart())
				{	//指定基面走向基准杆件
					CLDSLinePart *pLinePart=(CLDSLinePart*)pVecPart;
					vAxisX=pLinePart->End()-pLinePart->Start();
					normalize(vAxisX);
					if(hVecPart<0)	//用定位杆件句柄正负表示钢板延伸方向
						vAxisX*=-1.0;
				}
				else if(pVecPart->GetClassTypeId()==CLS_BOLT)
				{	//指定基面走向终端基准螺栓
					GEPOINT location=pVecPart->ucs.origin;
					int iIntersPlane=platerib.hDatumPlate.hValue<=0 ? CLDSPart::DATUM_PLANE : CLDSGeneralPlate::TOP_PLANE;
					pDatumPlate->CalBoltIntersPos((CLDSBolt*)pVecPart,location,iIntersPlane);
					vAxisX=location-GEPOINT(vOrigin);
				}
				if(vAxisX.IsZero())	//计算得到的X轴方向为空取基准钢板的X轴方向为默认X轴方向
					vAxisX=pDatumPlate->ucs.axis_x;
				axis_x[0]=vAxisX.x;
				axis_x[1]=vAxisX.y;
				axis_x[2]=vAxisX.z;
				//if(m_cReverseAxisVec=='X')
				//	vAxisX*=-1.0;
				//ucs.axis_z=ucs.axis_x^ucs.axis_y;
				//normalize(ucs.axis_z);
				//ucs.axis_x=ucs.axis_y^ucs.axis_z;
				//normalize(ucs.axis_x);
				//baseplane.origin=ucs.origin;
			}
		}
	}
	return FALSE;
}
BOOL CLDSParamPlate::DesignRibPlateOnPlate(CLDSGeneralPlate* pDatumPlate,CLDSParamPlate::RIBPLATE_PLATE& platerib)	//角钢肋板
{	//钢板上焊接的矩形肋板
	long hStartBolt=platerib.hStartBolt.hValue;			//基点定位螺栓
	long hVecPart=platerib.hVecPart.hValue;	//X轴或Y轴方向基准杆件
	double fOffsetX=platerib.OffsetX.value.fValue;	//X方向偏移量
	double fOffsetZ=platerib.OffsetZ.value.fValue;	//Z方向偏移量
	CLDSBolt *pBolt=(CLDSBolt*)BelongModel()->FromPartHandle(hStartBolt,CLS_BOLT);
	CLDSPart *pVecPart=(CLDSPart*)BelongModel()->FromPartHandle(abs(hVecPart));
	if(pVecPart&&!pVecPart->IsLinePart()&&pVecPart->GetClassTypeId()!=CLS_BOLT)
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->Log("Direction ancillary part must be rod or bolt(on datum plate or ancillary plate)！");
#else
			log2file->Log("方向辅助构件必须为杆件或(基板或辅助钢板上的)螺栓！");
#endif
		return FALSE;
	}
	//1.计算肋板坐标系原点位置
	f3dPoint vertice;
	BASERIBPLANE baseplane,baseplane2;//baseplane为Z向偏移前数据，baseplane2为Z向偏移后数据
	double Ox=platerib.Ox.value.fValue;
	double Oy=platerib.Oy.value.fValue;	//定位原点在底板上的相对坐标（与定位螺栓二选一）
	bool bSpecOrg=platerib.Ox.bExist&&platerib.Oy.bExist;
	if(bSpecOrg)
		ucs.origin.Set(Ox,Oy,pDatumPlate->GetNormOffset());
	else if(pBolt)
	{	//钢板肋板
		ucs.origin=pBolt->ucs.origin;
		if(pDatumPlate->FindLsByHandle(pBolt->handle)||fabs(pBolt->ucs.axis_z*pDatumPlate->ucs.axis_z)>EPS_COS)
		{
			int iIntersPlane=platerib.hDatumPlate.hValue<=0 ? CLDSPart::DATUM_PLANE : CLDSGeneralPlate::TOP_PLANE;
			pDatumPlate->CalBoltIntersPos(pBolt,ucs.origin,iIntersPlane,&baseplane.iResidePlane);
		}
		else
		{
			if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("Datum bolt doesn't on datum base plate！");
#else
				log2file->Log("基准螺栓不在基准底板内！");
#endif
			ucs.origin=pBolt->ucs.origin;
		}
	}
	else
		return FALSE;
	PROFILE_VERTEX* pBendlineStart[2]={NULL},*pBendlineEnd[2]={NULL};
	if(pDatumPlate->GetFaceN()>=2&&baseplane.iResidePlane==-1)
		pDatumPlate->GetBendlineVertexies(&pBendlineStart[0],&pBendlineEnd[0],&pBendlineStart[1],&pBendlineEnd[1]);
	bool middlePick=true;	//暂时先不启用
	if(baseplane.iResidePlane==-1&&pBendlineStart[0]&&pBendlineEnd[0]&&bSpecOrg)
	{
		double left_or_right=DistOf2dPtLine(f2dPoint(Ox,Oy),pBendlineStart[0]->vertex,pBendlineEnd[0]->vertex);
		//middlePick=(vpTipPoint-pBendlineStart[0]->vertex)*(vpTipPoint-pBendlineEnd[0]->vertex)<0;
		if(left_or_right<0&&middlePick)
			baseplane.iResidePlane=1;
	}
	if(baseplane.iResidePlane==-1&&pBendlineStart[1]!=NULL&&pBendlineEnd[1]!=NULL)
	{
		double left_or_right=DistOf2dPtLine(f2dPoint(Ox,Oy),pBendlineStart[1]->vertex,pBendlineEnd[1]->vertex);
		//middlePick=(vpTipPoint-pBendlineStart[1]->vertex)*(vpTipPoint-pBendlineEnd[1]->vertex)<0;
		if(left_or_right<0&&middlePick)
			baseplane.iResidePlane=2;
	}
	baseplane.iResidePlane=max(0,baseplane.iResidePlane);
	//2.计算肋板装配坐标系坐标轴方向
	//2.1计算Y轴初始默认方向
	ucs.axis_y=baseplane.iResidePlane==0?pDatumPlate->ucs.axis_z:pDatumPlate->GetHuoquFaceNorm(baseplane.iResidePlane-1);
	if(platerib.hDatumPlate.hValue<0)
		ucs.axis_y*=-1.0;
	if(bSpecOrg)
	{
		ucs.origin=pDatumPlate->TransToActual(ucs.origin,baseplane.iResidePlane+1,FROM_ACS|TO_MCS|IS_VERTEX);
		if(platerib.hDatumPlate.hValue>0)
			ucs.origin+=pDatumPlate->GetThick()*ucs.axis_y;
	}
	double VXx=platerib.Vx.value.fValue;
	double VXy=platerib.Vy.value.fValue;	//X轴在底板上的相对坐标
	bool bSpecAxisX=platerib.Vx.bExist&&platerib.Vy.bExist;
	double Ex=platerib.Ex.value.fValue;
	double Ey=platerib.Ey.value.fValue;
	bool bSpecEP=platerib.Ex.bExist&&platerib.Ey.bExist;
	if(bSpecAxisX)
	{	//直接指定基面走向
		ucs.axis_x.Set(VXx,VXy);
		normalize(ucs.axis_x);
		ucs.axis_x=pDatumPlate->TransToActual(ucs.axis_x,baseplane.iResidePlane+1,FROM_ACS|TO_MCS|IS_VECTOR);
		//vector_trans(ucs.axis_x,pDatumPlate->ucs,TRUE);
	}
	else if(bSpecEP)
	{	//指定基面走向终点
		f3dPoint vpEndPoint=pDatumPlate->TransToActual(GEPOINT(Ex,Ey,pDatumPlate->GetNormOffset()),baseplane.iResidePlane+1,FROM_ACS|TO_MCS|IS_VERTEX);
		if(platerib.hDatumPlate.hValue>0)
			vpEndPoint+=pDatumPlate->GetThick()*ucs.axis_y;
		ucs.axis_x=f3dPoint(vpEndPoint)-ucs.origin;
		//normalize(ucs.axis_x);
	}
	else if(pVecPart==NULL)
		return FALSE;
	else if(pVecPart->IsLinePart())
	{	//指定基面走向基准杆件
		CLDSLinePart *pLinePart=(CLDSLinePart*)pVecPart;
		ucs.axis_x=pLinePart->End()-pLinePart->Start();
		normalize(ucs.axis_x);
		if(hVecPart<0)	//用定位杆件句柄正负表示钢板延伸方向
			ucs.axis_x*=-1.0;
	}
	else if(pVecPart->GetClassTypeId()==CLS_BOLT)
	{	//指定基面走向终端基准螺栓
		GEPOINT location=pVecPart->ucs.origin;
		int iIntersPlane=platerib.hDatumPlate.hValue<=0 ? CLDSPart::DATUM_PLANE : CLDSGeneralPlate::TOP_PLANE;
		pDatumPlate->CalBoltIntersPos((CLDSBolt*)pVecPart,location,iIntersPlane);
		ucs.axis_x=location-GEPOINT(ucs.origin);
	}
	if(ucs.axis_x.IsZero())	//计算得到的X轴方向为空取基准钢板的X轴方向为默认X轴方向
		ucs.axis_x=pDatumPlate->ucs.axis_x;
	if(m_cReverseAxisVec=='X')
		ucs.axis_x*=-1.0;
	ucs.axis_z=ucs.axis_x^ucs.axis_y;
	normalize(ucs.axis_z);
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	normalize(ucs.axis_x);
	baseplane.origin=ucs.origin;
	baseplane.direction=ucs.axis_x;
	baseplane.norm=ucs.axis_z;
	//计算Z向偏移后数据
	baseplane2=baseplane;
	baseplane2.origin=baseplane.origin+baseplane.direction*fOffsetX+baseplane2.norm*fOffsetZ;
	//自此行开始baseplane中所有点位(baseplane.vpBendPoint未初始化除外)及向量均为模型坐标系下全局坐标
	bool hasBendPlane=true;
	try
	{
		BENDRIBPLANE bendplane,bendplane2;//bendplane为Z向偏移前数据，bendplane2为Z向偏移后数据
		CLDSBolt* pBendTipBolt=NULL;
		if(platerib.hTipBolt.bExist)
		{
			pBendTipBolt=(CLDSBolt*)BelongModel()->FromPartHandle(platerib.hTipBolt.hValue,CLS_BOLT);
			if(pBendTipBolt==NULL)
				throw CXhChar50("0X%X助板的制弯面末端定位螺栓未找到!",platerib.hTipBolt.hValue);
			if(!pDatumPlate->CalBoltIntersPos(pBendTipBolt,bendplane.vpTipPoint,CLDSPlate::TOP_PLANE,&bendplane.iResidePlane))
			{	//出错啦 wjh-2016.5.7
				int iIntersPlane=platerib.hDatumPlate.hValue<=0 ? CLDSPart::DATUM_PLANE : CLDSGeneralPlate::TOP_PLANE;
				pDatumPlate->CalBoltIntersPos(pBendTipBolt,bendplane.vpTipPoint,iIntersPlane);
				//vpTipPoint=bendplane.tipPoint;
				coord_trans(bendplane.vpTipPoint,pDatumPlate->ucs,FALSE,TRUE);
				throw CXhChar50("0X%X助板的制弯面末端定位螺栓与底板交点计算失败!",platerib.hTipBolt.hValue);
			}
		}
		else if(platerib.Tx.bExist&&platerib.Ty.bExist)
		{
			//自此行开始bendplane.vpTipPoint点为基准钢板pDatumPlate装配坐标系下局部展开坐标 wjh-2016.5.7
			if(pDatumPlate->GetFaceN()>=2&&pBendlineStart[0]==NULL&&bendplane.iResidePlane==-1)
			{
				if(!pDatumPlate->GetBendlineVertexies(&pBendlineStart[0],&pBendlineEnd[0],&pBendlineStart[1],&pBendlineEnd[1]))
#ifdef AFX_TARG_ENU_ENGLISH
					throw CXhChar50(" Find Plate HuoQu-Line Failed!");
#else
					throw CXhChar50("钢板火曲线获取失败!");
#endif
			}
			bool middlePick=true;
			GEPOINT vpLocalTipPoint(platerib.Tx.value.fValue,platerib.Ty.value.fValue,pDatumPlate->GetNormOffset());
			if(bendplane.iResidePlane==-1&&pBendlineStart[0]&&pBendlineEnd[0])
			{
				double left_or_right=DistOf2dPtLine(vpLocalTipPoint,pBendlineStart[0]->vertex,pBendlineEnd[0]->vertex);
				//middlePick=(vpTipPoint-pBendlineStart[0]->vertex)*(vpTipPoint-pBendlineEnd[0]->vertex)<0;
				if(left_or_right<0&&middlePick)
					bendplane.iResidePlane=1;
			}
			if(bendplane.iResidePlane==-1&&bendplane.iResidePlane==0&&pBendlineStart[1]!=NULL&&pBendlineEnd[1]!=NULL)
			{
				double left_or_right=DistOf2dPtLine(vpLocalTipPoint,pBendlineStart[1]->vertex,pBendlineEnd[1]->vertex);
				//middlePick=(vpTipPoint-pBendlineStart[1]->vertex)*(vpTipPoint-pBendlineEnd[1]->vertex)<0;
				if(left_or_right<0&&middlePick)
					bendplane.iResidePlane=2;
			}
			bendplane.iResidePlane=max(0,bendplane.iResidePlane);
			bendplane.vpTipPoint=pDatumPlate->TransToActual(vpLocalTipPoint,bendplane.iResidePlane+1,FROM_ACS|TO_MCS|IS_VERTEX);
			if(platerib.hDatumPlate.hValue>0)
			{
				GEPOINT bendplanenorm=bendplane.iResidePlane==0?pDatumPlate->ucs.axis_z:pDatumPlate->GetHuoquFaceNorm(bendplane.iResidePlane-1);
				bendplane.vpTipPoint+=pDatumPlate->GetThick()*bendplanenorm;
			}
		}
		else	//正常单面肋板，非制弯肋板，不输出警示信息
			throw CXhChar50("");
		if(platerib.hBendVecPart.bExist)
		{
			CLDSPart* pBendDirectPart=BelongModel()->FromPartHandle(platerib.hBendVecPart.hValue,CLS_BOLT,CLS_LINEPART);
			if(pBendDirectPart&&pBendDirectPart->GetClassTypeId()==CLS_BOLT)
			{
				int iIntersPlane=platerib.hDatumPlate.hValue<=0 ? CLDSPart::DATUM_PLANE : CLDSGeneralPlate::TOP_PLANE;
				pDatumPlate->CalBoltIntersPos((CLDSBolt*)pBendDirectPart,bendplane.pickPoint,iIntersPlane);
				bendplane.direction=bendplane.vpTipPoint-bendplane.pickPoint;
			}
			else if(pBendDirectPart&&pBendDirectPart->IsLinePart())
				bendplane.direction=((CLDSLinePart*)pBendDirectPart)->End()-((CLDSLinePart*)pBendDirectPart)->Start();
			else
				Log2File()->Log("0X%X助板的制弯面辅助定向构件未找到!",platerib.hBendVecPart.hValue);
		}
		else if(platerib.Bx.bExist&&platerib.By.bExist)
		{
			bendplane.pickPoint=pDatumPlate->TransToActual(GEPOINT(platerib.Bx.value.fValue,platerib.By.value.fValue),bendplane.iResidePlane+1,FROM_ACS|TO_MCS|IS_VERTEX);
			bendplane.direction=bendplane.vpTipPoint-bendplane.pickPoint;
		}
		if(bendplane.direction.IsZero())//&&ciOnBendlineRightSide==0)
		{	//智能计算制弯面走向
			CLDSLinePart* pRod=pBendTipBolt?BelongModel()->FromRodHandle(pBendTipBolt->des_base_pos.hPart):NULL;
			if(pRod)
				bendplane.direction=pRod->End()-pRod->Start();
		}
		//计算初始（Z向偏移前）的基准面走向及制弯面走向
		if(pDatumPlate->GetFaceN()>=2&&pBendlineStart[0]==NULL)
		{
			if(!pDatumPlate->GetBendlineVertexies(&pBendlineStart[0],&pBendlineEnd[0],&pBendlineStart[1],&pBendlineEnd[1]))
#ifdef AFX_TARG_ENU_ENGLISH
				throw CXhChar50(" Find Plate HuoQu-Line Failed!");
#else
				throw CXhChar50("钢板火曲线获取失败!");
#endif
		}
		bool bPriorBendPoint=bendplane.direction.IsZero();
		if(!bPriorBendPoint)
		{
			Int3dlf(baseplane.vpBendPoint,bendplane.vpTipPoint,bendplane.direction,baseplane.origin,baseplane.norm);
			if(bendplane.direction*(bendplane.vpTipPoint-baseplane.vpBendPoint)<0)
				bendplane.direction*=-1.0;
		}
		else if(bPriorBendPoint)
		{
			if(baseplane.iResidePlane!=bendplane.iResidePlane&&min(baseplane.iResidePlane,bendplane.iResidePlane)==0)
			{	//肋板跨越基准底板火曲线时，应强制以基准底板火曲线为肋板制弯点
				int iResidePlane=max(baseplane.iResidePlane,bendplane.iResidePlane);
				f3dLine bendline(pBendlineStart[iResidePlane-1]->vertex,pBendlineEnd[iResidePlane-1]->vertex);
				bendline.startPt.z=bendline.endPt.z=pDatumPlate->GetNormOffset();
				coord_trans(bendline.startPt,pDatumPlate->ucs,TRUE,TRUE);
				coord_trans(bendline.endPt,pDatumPlate->ucs,TRUE,TRUE);
				if(platerib.hDatumPlate.hValue>0)
				{	//正面肋板应考虑板厚等距线交叉点影响
					GEPOINT bivec=pDatumPlate->ucs.axis_z+pDatumPlate->GetHuoquFaceNorm(iResidePlane-1);
					normalize(bivec);
					double cosa=bivec*pDatumPlate->ucs.axis_z;
					vector_trans(bivec,pDatumPlate->ucs,FALSE,TRUE);
					if(fabs(cosa)>EPS)
					{
						GEPOINT offset=bivec*(pDatumPlate->GetThick()/cosa);
						bendline.startPt+=offset;
						bendline.endPt+=offset;
					}
				}
				GEPOINT bendlinevec=bendline.endPt-bendline.startPt;
				normalize(bendlinevec);
				double justify1=fabs(bendlinevec*baseplane.norm);
				double justify2=fabs(bendlinevec*bendplane.norm);
				bool finished=false;
				if(justify1>0.25)
					finished=Int3dlf(baseplane.vpBendPoint,bendline,baseplane.origin,baseplane.norm)==1;
				if(!finished&&justify2>0.25)
					finished=Int3dlf(baseplane.vpBendPoint,bendline,bendplane.vpTipPoint,baseplane.norm)==1;
				if(!finished)
#ifdef AFX_TARG_ENU_ENGLISH
					throw CXhChar50("Bending point calculation encounter a problem!");
#else
					throw CXhChar50("火曲点计算错误");
#endif
			}
			else if(baseplane.iResidePlane==bendplane.iResidePlane)
				baseplane.vpBendPoint=baseplane.origin+baseplane.direction*(platerib.W.value.fValue-fOffsetX);
			else
#ifdef AFX_TARG_ENU_ENGLISH
				throw CXhChar50("Rib plate across the two bending plane is not allowed!");
#else
				throw CXhChar50("不允许肋板直接跨越两个制弯面");
#endif
			bendplane.direction=bendplane.vpTipPoint-baseplane.vpBendPoint;
		}
		GEPOINT vCurrNorm=bendplane.iResidePlane==0?pDatumPlate->ucs.axis_z:pDatumPlate->GetHuoquFaceNorm(bendplane.iResidePlane-1);
		bendplane.direction-=(bendplane.direction*vCurrNorm)*vCurrNorm;	//保证制弯面走向一定与钢板工作平面垂直
		normalize(bendplane.direction);
		bendplane.norm=bendplane.direction;
		if(platerib.hDatumPlate.hValue>0)
			RotateVectorAroundVector(bendplane.norm,-1,0,vCurrNorm);
		else
			RotateVectorAroundVector(bendplane.norm, 1,0,vCurrNorm);
		if(m_cReverseAxisVec=='X')
		{
			bendplane.direction*=-1;
			bendplane.norm*=-1;
		}
		//计算Z向偏移后数据
		double fTipOffsetX=!platerib.TipOffsetX.bExist?0:platerib.TipOffsetX.value.fValue;
		double fTipOffsetZ=!platerib.TipOffsetZ.bExist?0:platerib.TipOffsetZ.value.fValue;
		bendplane2=bendplane;
		bendplane2.vpTipPoint=bendplane.vpTipPoint+bendplane.direction*fTipOffsetX+bendplane.norm*fTipOffsetZ;
		//计算真实的制弯点
		if(baseplane.iResidePlane!=bendplane.iResidePlane&&min(baseplane.iResidePlane,bendplane.iResidePlane)==0)
		{	//肋板跨越基准底板火曲线时，应强制以基准底板火曲线为肋板制弯点
			int iResidePlane=max(baseplane.iResidePlane,bendplane.iResidePlane);
			f3dLine bendline(pBendlineStart[iResidePlane-1]->vertex,pBendlineEnd[iResidePlane-1]->vertex);
			bendline.startPt.z=bendline.endPt.z=pDatumPlate->GetNormOffset();
			coord_trans(bendline.startPt,pDatumPlate->ucs,TRUE,TRUE);
			coord_trans(bendline.endPt,pDatumPlate->ucs,TRUE,TRUE);
			if(platerib.hDatumPlate.hValue>0)
			{	//正面肋板应考虑板厚等距线交叉点影响
				GEPOINT bivec=pDatumPlate->ucs.axis_z+pDatumPlate->GetHuoquFaceNorm(iResidePlane-1);
				normalize(bivec);
				double cosa=bivec*pDatumPlate->ucs.axis_z;
				//vector_trans(bivec,pDatumPlate->ucs,FALSE,TRUE);
				if(fabs(cosa)>EPS)
				{
					GEPOINT offset=bivec*(pDatumPlate->GetThick()/cosa);
					bendline.startPt+=offset;
					bendline.endPt+=offset;
				}
			}
			GEPOINT bendlinevec=bendline.endPt-bendline.startPt;
			normalize(bendlinevec);
			double justify1=fabs(bendlinevec*baseplane2.norm);
			double justify2=fabs(bendlinevec*bendplane2.norm);
			bool finished=false;
			if(justify1>0.25)
				finished=Int3dlf(baseplane2.vpBendPoint,bendline,baseplane2.origin,baseplane2.norm)==1;
			if(!finished&&justify2>0.25)
				finished=Int3dlf(baseplane2.vpBendPoint,bendline,bendplane2.vpTipPoint,bendplane2.norm)==1;
			if(!finished)
#ifdef AFX_TARG_ENU_ENGLISH
				throw CXhChar50("Bending point calculation encounter a problem!");
#else
				throw CXhChar50("火曲点计算错误");
#endif
		}
		else if(bPriorBendPoint&&baseplane.iResidePlane==bendplane.iResidePlane)
			baseplane2.vpBendPoint=baseplane2.origin+baseplane2.direction*(platerib.W.value.fValue-fOffsetX);
		else if(!bPriorBendPoint&&baseplane.iResidePlane==bendplane.iResidePlane)
		{	//根据制弯面走向，反算制弯点
			Int3dlf(baseplane2.vpBendPoint,baseplane2.origin,baseplane2.direction,bendplane2.vpTipPoint,bendplane2.norm);
		}
		else
			throw CXhChar50("Encounter an unexpected error");
		bendplane2.direction=bendplane2.vpTipPoint-baseplane2.vpBendPoint;
		normalize(bendplane2.direction);
		bendplane2.norm=bendplane2.direction;
		if(platerib.hDatumPlate.hValue>0)
			RotateVectorAroundVector(bendplane2.norm,-1,0,vCurrNorm);
		else
			RotateVectorAroundVector(bendplane2.norm, 1,0,vCurrNorm);
		if(m_cReverseAxisVec=='X')
		{
			bendplane2.direction*=-1;
			bendplane2.norm*=-1;
		}
		keyvecset.huoqufacenorm=bendplane2.norm;
		keyvecset.huoquaxis_vec=baseplane.norm^bendplane2.norm;
		f3dPoint verifynorm=baseplane.direction^keyvecset.huoquaxis_vec;
		if(verifynorm*baseplane.norm<0)
			keyvecset.huoquaxis_vec*=-1;
		normalize(keyvecset.huoquaxis_vec);
		//计算火曲面夹角
		GECS huoquucs;
		huoquucs.axis_z=keyvecset.huoquaxis_vec;
		huoquucs.axis_y=-ucs.axis_z;
		huoquucs.axis_x=huoquucs.axis_y^huoquucs.axis_z;
		GEPOINT benddirect=huoquucs.TransVToCS(bendplane2.direction);
		double huoquangle=Cal2dLineAng(0,0,benddirect.x,benddirect.y);
		if(huoquangle>Pi)
			huoquangle-=2*Pi;
		//计算顶部火曲点
		GEPOINT vpTopBendPoint,vUpwardVec=bendplane.direction;
		RotateVectorAroundVector(vUpwardVec,1,0,bendplane.norm);
		double fCosa=fabs(baseplane.norm*bendplane.norm);
		BOOL bHuoquRib=fCosa>0.99?FALSE:TRUE;	//两面法线无限接近，认为是单面板
		if(fCosa<EPS_COS2)
			Int3dlf(vpTopBendPoint,baseplane2.origin+ucs.axis_y*platerib.H.value.fValue,ucs.axis_x,bendplane2.vpTipPoint,bendplane2.norm);
		else
		{	//两面法线接近时再用线面求交误差比较大 wjh-2017.11.27
			GEPOINT vBiVec=(ucs.axis_y+vUpwardVec);
			vBiVec-=(vBiVec*ucs.axis_z)*ucs.axis_z;
			normalize(vBiVec);
			vpTopBendPoint=baseplane2.vpBendPoint+vBiVec*platerib.H.value.fValue;
			//vpTopBendPoint=baseplane2.vpBendPoint+vBiVec*(platerib.H.value.fValue/vBiVec.y);
		}
		GEPOINT vpBendPoint=baseplane2.vpBendPoint;
		GEPOINT vpTipPoint=bendplane2.vpTipPoint;
		GEPOINT vpTopTipPoint=bendplane2.vpTipPoint+vUpwardVec*platerib.H.value.fValue;
		f3dLine rotaxis(baseplane2.vpBendPoint,baseplane2.vpBendPoint+keyvecset.huoquaxis_vec);
		//将全部坐标转换至当前参数化钢板的局部坐标系
		ucs.origin=baseplane2.origin;
		coord_trans(vpBendPoint,ucs,FALSE,TRUE);
		coord_trans(vpTopBendPoint,ucs,FALSE,TRUE);
		if(fabs(vpBendPoint.z)>EPS)
		{	//肋板的基准面与制弯面交点不在制弯线上，此时需要修正制弯点到基准面上 wjh-2016.6.18
			GEPOINT vpBendRoadVec=bendplane2.vpTipPoint-baseplane2.vpBendPoint;
			Int3dlf(vpBendPoint,bendplane2.vpTipPoint,vpBendRoadVec,ucs.origin,ucs.axis_z);
			rotaxis=f3dLine(vpBendPoint,vpBendPoint+keyvecset.huoquaxis_vec);
			coord_trans(vpBendPoint,ucs,FALSE,TRUE);
		}
		//根据修正后的制弯点将制弯面末端顶点展开并转换至局部坐标系下
		rotate_point_around_axis(vpTipPoint,-huoquangle,rotaxis);
		rotate_point_around_axis(vpTopTipPoint,-huoquangle,rotaxis);
		coord_trans(vpTipPoint,ucs,FALSE,TRUE);
		coord_trans(vpTopTipPoint,ucs,FALSE,TRUE);
		//添加钢板轮廓点
		if(m_cReverseAxisVec=='X')
		{	//反转轮廓点
			vertice.Set(0,0,0);
			PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
			pVertex->vertex.feature=1;
			vertice.Set(0,platerib.H.value.fValue,0);
			pVertex=vertex_list.append(vertice);
			pVertex->vertex.feature=1;
			vertice.Set(vpTopBendPoint.x,vpTopBendPoint.y);
			pVertex=vertex_list.append(vertice);
			pVertex->vertex.feature=bHuoquRib?12:1;
			vertice.Set(vpTopTipPoint.x,vpTopTipPoint.y);
			pVertex=vertex_list.append(vertice);
			pVertex->vertex.feature=bHuoquRib?2:1;
			vertice.Set(vpTipPoint.x,vpTipPoint.y);
			pVertex=vertex_list.append(vertice);
			pVertex->vertex.feature=bHuoquRib?2:1;
			pVertex->m_bWeldEdge=TRUE;
			vertice.Set(vpBendPoint.x,0,0);
			pVertex=vertex_list.append(vertice);
			pVertex->vertex.feature=bHuoquRib?12:1;
			pVertex->m_bWeldEdge=TRUE;
		}
		else
		{
			vertice.Set(0,0,0);
			vertice.feature=1;
			PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
			pVertex->m_bWeldEdge=TRUE;
			vertice.Set(vpBendPoint.x,0,0);
			pVertex=vertex_list.append(vertice);
			if(fabs(vpBendPoint.y)<EPS2)
			{
				pVertex->vertex.feature=bHuoquRib?12:1;
				pVertex->m_bWeldEdge=TRUE;
			}
			else
			{
				pVertex->vertex.feature=1;
				pVertex->m_bWeldEdge=FALSE;

				vertice.Set(vpBendPoint.x,vpBendPoint.y,0);
				pVertex=vertex_list.append(vertice);
				pVertex->vertex.feature=bHuoquRib?12:1;
				pVertex->m_bWeldEdge=TRUE;
				if(fabs(vpBendPoint.y)>0.5)
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.Log(
					"The angle between rib plate's datum plane with the bendline of datum plate."
					"There may be a large error(%.1fmm) in the calculation of the bending point!"
					,fabs(vpBendPoint.y));
#else
					logerr.Log("待创建的制弯肋板的的基准面与底板的火曲线夹角过小，制弯点计算可能存在误差(%.1fmm)!",fabs(vpBendPoint.y));
#endif
			}
			vertice.Set(vpTipPoint.x,vpTipPoint.y);
			pVertex=vertex_list.append(vertice);
			pVertex->vertex.feature=bHuoquRib?2:1;
			vertice.Set(vpTopTipPoint.x,vpTopTipPoint.y);
			pVertex=vertex_list.append(vertice);
			pVertex->vertex.feature=bHuoquRib?2:1;
			vertice.Set(vpTopBendPoint.x,vpTopBendPoint.y);
			pVertex=vertex_list.append(vertice);
			pVertex->vertex.feature=bHuoquRib?12:1;

			vertice.Set(0,platerib.H.value.fValue,0);
			vertex_list.append(vertice);
		}
	}
	catch(CXhChar50 error)
	{
		if(error.GetLength()>0)
			Log2File()->Log(error);
		hasBendPlane=false;
	}
	if(!hasBendPlane)
	{
		ucs.origin+=ucs.axis_x*fOffsetX;	//钢板肋板需要X轴方向偏移量
		ucs.origin+=ucs.axis_z*fOffsetZ;
		/////////////////////////////////////////////////////////////////
		//与底板倾斜角度 目前这部分代码还不够严谨，仅时用于临时满足倾斜肋板处理(且不支持制弯型肋板) wjh-2019.7.30
		if(platerib.fGradientDegAngle.bExist&&platerib.fGradientDegAngle.iValue!=90)
		{	//修正肋板有倾斜角时的Y轴方向(bSpecAxisY=true)
			double fRadAngle=(platerib.fGradientDegAngle.fValue-90)*RADTODEG_COEF;
			RotateVectorAroundVector(ucs.axis_y,fRadAngle,ucs.axis_x);
			ucs.axis_z=ucs.axis_x^ucs.axis_y;
			normalize(ucs.axis_z);
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
			normalize(ucs.axis_x);
		}
		/////////////////////////////////////////////////////////////////
		//添加钢板轮廓点
		if(m_cReverseAxisVec=='X')
		{	//反转轮廓点
			vertice.Set(0,0,0);
			vertice.feature=1;
			vertex_list.append(vertice);
			vertice.Set(0,platerib.H.value.fValue,0);
			vertex_list.append(vertice);
			vertice.Set(-platerib.W.value.fValue,platerib.H.value.fValue,0);
			vertex_list.append(vertice);
			vertice.Set(-platerib.W.value.fValue,0,0);
			PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
			pVertex->m_bWeldEdge=TRUE;
		}
		else
		{
			vertice.Set(0,0,0);
			vertice.feature=1;
			PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
			pVertex->m_bWeldEdge=TRUE;
			vertice.Set(platerib.W.value.fValue,0,0);
			vertex_list.append(vertice);
			vertice.Set(platerib.W.value.fValue,platerib.H.value.fValue,0);
			vertex_list.append(vertice);
			vertice.Set(0,platerib.H.value.fValue,0);
			vertex_list.append(vertice);
		}
	}

	return TRUE;
}
BOOL CLDSParamPlate::DesignRibPlate2(CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	long hDatumPart=0,hNode=0,hAssPart1=0,hAssPart2=0,work_norm_style=0;
	RIBPROFILE_PARA profile;
	profile.ClearZero();
	double fOffsetY=0;
	if(!GetDesignItemValue('A',&hDatumPart,pItemHashTbl))//基准构件
		return FALSE;
	GetDesignItemValue('B',&hNode,pItemHashTbl);		//基准节点
	GetDesignItemValue('C',&hAssPart1,pItemHashTbl);	//辅助构件1
	GetDesignItemValue('D',&hAssPart2,pItemHashTbl);	//辅助构件2
	CLDSNode *pDatumNode=NULL;
	CLDSPart *pDatumPart=NULL,*pAssistantPart1=NULL,*pAssistantPart2=NULL;
	pDatumNode=BelongModel()->FromNodeHandle(hNode);
	pDatumPart=BelongModel()->FromPartHandle(abs(hDatumPart),CLS_LINETUBE,CLS_PLATE,CLS_PARAMPLATE);
	pAssistantPart1=BelongModel()->FromPartHandle(abs(hAssPart1),CLS_PLATE,CLS_PARAMPLATE);
	pAssistantPart2=BelongModel()->FromPartHandle(hAssPart2,CLS_PLATE,CLS_PARAMPLATE);
	if( pDatumPart!=NULL&&pAssistantPart1!=NULL&&pAssistantPart2==NULL&&
		pDatumPart->GetClassTypeId()==CLS_LINETUBE&&pAssistantPart1->IsPlate())
	{	//肘板型肋板
		CLDSParamPlate::RIBPLATE_ELBOW elbow;
		elbow.LoadParams(this,pItemHashTbl);
		return DesignRibElbowPlate((CLDSLineTube*)pDatumPart,(CLDSGeneralPlate*)pAssistantPart1,elbow);
	}
	if( pDatumPart!=NULL&&pDatumPart->IsPlate()&&
		pAssistantPart1==NULL&&pAssistantPart2==NULL)
	{	//钢板上的普通矩形肋板
		CLDSParamPlate::RIBPLATE_PLATE platerib;
		platerib.LoadParams(this,pItemHashTbl);
		return DesignRibPlateOnPlate((CLDSGeneralPlate*)pDatumPart,platerib);
	}
	if( pDatumPart!=NULL&&pDatumPart->GetClassTypeId()==CLS_LINETUBE&&
		pAssistantPart1==NULL&&pAssistantPart2==NULL)
	{	//钢管上的普通矩形肋板
		CLDSParamPlate::RIBPLATE_TUBE tuberib;
		tuberib.LoadParams(this,pItemHashTbl);
		return DesignRibPlateOnTube((CLDSLineTube*)pDatumPart,pDatumNode,tuberib);
	}
	if(pDatumPart!=NULL&&pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSParamPlate::RIBPLATE_ANGLE anglerib;
		anglerib.LoadParams(this,pItemHashTbl);
		return DesignRibPlateOnAngle((CLDSLineAngle*)pDatumPart,anglerib);
	}
	if( pDatumPart!=NULL&&pDatumPart->IsPlate()&&(pAssistantPart1==NULL||
		(pAssistantPart1!=NULL&&pAssistantPart1->IsPlate())))
	{
		CLDSParamPlate::RIBPLATE_CORNER corner;
		corner.LoadParams(this,pItemHashTbl);
		return DesignRibCornerOrSlotPlate((CLDSGeneralPlate*)pDatumPart,corner);
	}
	if(pDatumPart==NULL)
		return FALSE;
	if(!GetDesignItemValue('E',&profile.common.fBottomWidth,pItemHashTbl))	//底部宽度
		return FALSE;
	//else
	//	raw.E.Set(profile.common.fBottomWidth);
	if(!GetDesignItemValue('F',&profile.common.fTopWidth,pItemHashTbl))	//顶部宽度
		return FALSE;
	//else
	//	raw.F.Set(profile.common.fTopWidth);
	if(!GetDesignItemValue('G',&profile.common.fInnerLen,pItemHashTbl))	//内侧长度
		return FALSE;
	//else
	//	raw.G.Set(profile.common.fInnerLen);
	if(!GetDesignItemValue('H',&profile.common.fOuterLen,pItemHashTbl))	//外侧长度
		return FALSE;
	//else
	//	raw.H.Set(profile.common.fOuterLen);
	if(!GetDesignItemValue('I',&profile.common.fCutAngleLen,pItemHashTbl))	//切角宽度
		profile.common.fCutAngleLen=0;
	//else
	//	raw.I.Set(profile.common.fCutAngleLen);
	long iCutMode=0;
	if(!GetDesignItemValue('e',&iCutMode,pItemHashTbl))	//切角模式
		iCutMode=0;
	double fCutAngleLen2=0;
	if(!GetDesignItemValue(KEY2C("I2"),&fCutAngleLen2,pItemHashTbl))	//切角宽度2
		fCutAngleLen2=0;
	if(pAssistantPart1&&pAssistantPart1->IsPlate()&&pAssistantPart2&&pAssistantPart2->IsPlate())
	{	//纠正坐标系Y轴方向,考虑法线偏移量 wht 11-05-24
		f3dPoint pt1=GetPlateDatumFacePickPt(pAssistantPart1,FALSE);
		f3dPoint pt2=GetPlateDatumFacePickPt(pAssistantPart2,FALSE);
		f3dPoint verfiy_vec=pt2-pt1;
		if(verfiy_vec*pAssistantPart1->ucs.axis_z>0)
			hAssPart1=abs(hAssPart1);	//肋板在辅助钢板顶面一侧
		else 
			hAssPart1=-abs(hAssPart1);	//肋板在辅助钢板底面一侧
		if(verfiy_vec*pAssistantPart2->ucs.axis_z<0)
			hAssPart2=abs(hAssPart2);
		else 
			hAssPart2=-abs(hAssPart2);
	}
	f3dPoint vertice;
	if(pDatumPart->GetClassTypeId()==CLS_LINETUBE)
	{	//基准构件为钢管
		double fOffsetAngle=0,fBottomOuterR=-1,fTopOuterR=-1;
		if(pDatumNode==NULL&&pAssistantPart1==NULL&&pAssistantPart2==NULL)
			return FALSE;
		if(!GetDesignItemValue('J',&work_norm_style,pItemHashTbl))	//0.基准边 1.焊道线 2.指定方向
			return FALSE;
		if(!GetDesignItemValue('K',&fOffsetAngle,pItemHashTbl))	//偏移角度
			return FALSE;
		if(!GetDesignItemValue('R',&fBottomOuterR,pItemHashTbl))//底部外环半径
			return FALSE;
		if(pAssistantPart2&&pAssistantPart2->IsPlate())
		{	//基准构件为钢管且存在第二块辅助钢板，此时存在顶部外环半径
			if(!GetDesignItemValue('r',&fTopOuterR,pItemHashTbl))//顶部外环半径
				return FALSE;
		}
		fOffsetAngle*=RADTODEG_COEF;	//偏移角度装换为弧度制
		CLDSLineTube *pDatumTube=(CLDSLineTube*)pDatumPart;
		pDatumTube->BuildUCS();
		f3dPoint line_vec=pDatumTube->ucs.axis_z;
		if(work_norm_style==2)
		{	//自定义工作法线方向
			if(!GetDesignItemValue(KEY2C("Nx"),&ucs.axis_z.x,pItemHashTbl))	//法线X分量
				return FALSE;
			if(!GetDesignItemValue(KEY2C("Ny"),&ucs.axis_z.y,pItemHashTbl))	//法线Y分量
				return FALSE;
			if(!GetDesignItemValue(KEY2C("Nz"),&ucs.axis_z.z,pItemHashTbl))	//法线Z分量
				return FALSE;
			if(!GetDesignItemValue(KEY2C("Wx"),&ucs.axis_x.x,pItemHashTbl))	//展开方向(X轴)X分量
				return FALSE;
			if(!GetDesignItemValue(KEY2C("Wy"),&ucs.axis_x.y,pItemHashTbl))	//展开方向(X轴)Y分量
				return FALSE;
			if(!GetDesignItemValue(KEY2C("Wz"),&ucs.axis_x.z,pItemHashTbl))	//展开方向(X轴)Z分量
				return FALSE;
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			ucs.axis_x=ucs.axis_y^ucs.axis_z;//通过Ｙ轴校正Ｘ轴与Ｚ轴的正交垂直
			normalize(ucs.axis_x);
			normalize(ucs.axis_y);
			normalize(ucs.axis_z);
		}
		else
		{	//通过基准边旋转角度确定工作法线(Y轴沿钢管轴向)
			double weldRoadAngle=0;
			if(pDatumTube->m_bHasWeldRoad&&work_norm_style==1)
				weldRoadAngle=pDatumTube->CalWeldRoadAngle();
			ucs.axis_x.Set(cos(weldRoadAngle),sin(weldRoadAngle));
			ucs.axis_z.Set(-ucs.axis_x.y,ucs.axis_x.x);
			vector_trans(ucs.axis_x,pDatumTube->ucs,TRUE);
			vector_trans(ucs.axis_z,pDatumTube->ucs,TRUE);
		}
		if(pAssistantPart1&&pAssistantPart1->IsPlate())
		{	
			f3dPoint tube_vec;
			double radiusRatio=0;
			BOOL bVerticalPlate=FALSE;	//肋板垂直与辅助钢板1且自动正位
			if(pAssistantPart2==NULL)
			{
				tube_vec=pDatumTube->Start()-pDatumTube->End();
				normalize(tube_vec);
				radiusRatio=fabs(tube_vec*pAssistantPart1->ucs.axis_z);
				GetDesignItemValue('S',(long*)&bVerticalPlate,pItemHashTbl);
				if(radiusRatio<EPS2||radiusRatio>EPS_COS)
					bVerticalPlate=FALSE;
			}
			if(bVerticalPlate)	//竖直肋板
				line_vec=pAssistantPart1->ucs.axis_z;
			double cosa=line_vec*pAssistantPart1->ucs.axis_z;	//钢管延伸方向与辅助钢板Z
			if((hAssPart1>0&&cosa<0)||(hAssPart1<0&&cosa>0))
				line_vec*=-1.0;
			//1、计算肋板坐标系方向
			f3dPoint inters_vec=ucs.axis_z^pAssistantPart1->ucs.axis_z;
			normalize(inters_vec);
			normalize(ucs.axis_x);
			if(ucs.axis_x*inters_vec<0)	//修正最终X轴方向的正负方向
				ucs.axis_x=-inters_vec;
			else						//将肘板最终X轴方向赋值给肘板装配坐标系
				ucs.axis_x=inters_vec;
			//根据偏移角度调整基准方向
			if(fabs(fOffsetAngle)>0)
				RotateVectorAroundVector(ucs.axis_x,fOffsetAngle,pAssistantPart1->ucs.axis_z);
			ucs.axis_z = ucs.axis_x^line_vec;
			normalize(ucs.axis_z);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
			//2、根据直线与圆柱的交点计算坐标系原点，保证原点在管壁上
			double fRadius=pDatumTube->GetDiameter()*0.5;
			f3dPoint inters1,inters2,line_pick,face_pick,temp_vec;
			face_pick=GetPlateDatumFacePickPt(pAssistantPart1,hAssPart1>0);
			Int3dlf(line_pick,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pAssistantPart1->ucs.axis_z);
			if(Int3dlc(&inters1,&inters2,line_pick,ucs.axis_x,pDatumTube->Start(),pDatumTube->End(),fRadius,fRadius)!=2)
			{
				logerr.Log("肋板坐标原点计算失败！");
				return FALSE;
			}
			temp_vec=inters1-inters2;
			normalize(temp_vec);
			if(temp_vec*ucs.axis_x>0)
				ucs.origin=inters1;
			else
				ucs.origin=inters2;
			//基准点，钢管轴线与辅助钢板所在平面的交点
			f3dPoint top_origin,top_datum_pos,bottom_datum_pos;
			Int3dlf(bottom_datum_pos,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pAssistantPart1->ucs.axis_z);
			if(fBottomOuterR>0)
				profile.common.fBottomWidth=fBottomOuterR-DISTANCE(ucs.origin,bottom_datum_pos);
			//
			if(pAssistantPart2&&pAssistantPart2->IsPlate())
			{	//存在两块辅助钢板时系统自动计算肋板长度以及顶部底部高度
				face_pick=GetPlateDatumFacePickPt(pAssistantPart2,ucs.axis_y*pAssistantPart2->ucs.axis_z<0);
				Int3dlf(top_origin,ucs.origin,pDatumTube->ucs.axis_z,face_pick,pAssistantPart2->ucs.axis_z);
				profile.common.fInnerLen=DISTANCE(ucs.origin,top_origin);
				Int3dlf(top_datum_pos,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pAssistantPart2->ucs.axis_z);
				if(fTopOuterR>0)
					profile.common.fTopWidth=fTopOuterR-DISTANCE(top_origin,top_datum_pos);
				//肋板基准面与辅助钢板2基准面求交线
				f3dPoint line_pick2,inters_vec2;
				Int3d2f(&line_pick2,&inters_vec2,ucs.origin,ucs.axis_z,face_pick,pAssistantPart2->ucs.axis_z);
				if(inters_vec2*ucs.axis_x<0)
					inters_vec2*=-1.0;
				normalize(inters_vec2);
				f3dPoint assistant_plate2_pt=top_origin+inters_vec2*profile.common.fTopWidth; //与关联钢板2相关的外侧顶点
				coord_trans(top_origin,ucs,FALSE);
				coord_trans(assistant_plate2_pt,ucs,FALSE);
				top_origin.z=assistant_plate2_pt.z=0;
				//
				vector_trans(line_vec,ucs,FALSE);
				double k=0;
				if(fabs(line_vec.y)>eps)	//防止line_vec.y=0 
					k=line_vec.x/line_vec.y;
				vertice.Set(profile.common.fCutAngleLen,0,0);
				PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE; 
				vertice.Set(profile.common.fBottomWidth,0,0);
				vertex_list.append(vertice);
				if(fabs(assistant_plate2_pt.x-profile.common.fBottomWidth)>EPS_COS)
				{
					if(assistant_plate2_pt.x>profile.common.fBottomWidth)		//顶部宽度大于底部宽度
					{
						f3dPoint axis_z=pAssistantPart2->ucs.axis_z;
						if(axis_z*ucs.axis_y>0)
							axis_z*=-1.0;
						vector_trans(axis_z,ucs,FALSE);
						axis_z.z=0;
						vertice=assistant_plate2_pt+profile.common.fOuterLen*axis_z;
					}
					else if(assistant_plate2_pt.x<profile.common.fBottomWidth)	//顶部宽度小于底部宽度
						vertice.Set(profile.common.fBottomWidth,profile.common.fOuterLen,0);
					vertex_list.append(vertice);
				}
				pVertex=vertex_list.append(assistant_plate2_pt);
				pVertex->m_bWeldEdge=TRUE;
				if(fabs(fCutAngleLen2)>eps)
				{
					f3dPoint axis_x2=(assistant_plate2_pt-top_origin).normalized();
					vertice=top_origin+axis_x2*fCutAngleLen2;
					pVertex=vertex_list.append(vertice);
					if(iCutMode>0)
					{
						pVertex->type=2;
						pVertex->radius=-fCutAngleLen2;
						pVertex->center=top_origin;
					}
					//
					if(line_vec.y>0)
						vertice=top_origin-line_vec*fCutAngleLen2;
					else 
						vertice=top_origin+line_vec*fCutAngleLen2;
					pVertex=vertex_list.append(vertice);
					pVertex->m_bWeldEdge=TRUE;
				}
				else
				{
					pVertex=vertex_list.append(top_origin);
					pVertex->m_bWeldEdge=TRUE;
				}
				if(fabs(profile.common.fCutAngleLen)>eps)
				{
					vertice.Set(profile.common.fCutAngleLen*k,profile.common.fCutAngleLen,0);
					pVertex=vertex_list.append(vertice);
					if(iCutMode>0)
					{
						pVertex->type=2;
						pVertex->radius=-profile.common.fCutAngleLen;
					}
				}
			}
			else if(bVerticalPlate) //钢管+辅助钢板
			{	//竖直且带自动正位的肘板	
				double radius=pDatumTube->GetDiameter()*0.5;
				f3dPoint datum_point,line_pick,inters1,inters2;
				line_pick.Set(0,profile.common.fInnerLen,0);
				coord_trans(line_pick,ucs,TRUE);
				line_vec=ucs.axis_x;
				if(Int3dlc(&inters1,&inters2,line_pick,line_vec,pDatumTube->Start(),pDatumTube->End(),radius,radius)>0)
				{
					f3dPoint axis_x=inters2-inters1;
					normalize(axis_x);
					if(axis_x*ucs.axis_x>0)
						datum_point=inters2;
					else 
						datum_point=inters1;
				}
				else //无交点时取直线的平行线与椭圆的切点作为交点	
				{	
					double majorAxisLen=radius/radiusRatio;
					UCS_STRU ellipse_ucs;	//椭圆坐标系
					f3dPoint face_pick=pAssistantPart1->ucs.origin;
					if(pAssistantPart1->GetClassTypeId()==CLS_PLATE)
						face_pick+=((CLDSPlate*)pAssistantPart1)->m_fNormOffset*pAssistantPart1->ucs.axis_z;
					Int3dlf(ellipse_ucs.origin,pDatumTube->Start(),pDatumTube->ucs.axis_z,face_pick,pAssistantPart1->ucs.axis_z);
					if(tube_vec*ucs.axis_y<0)
						tube_vec*=-1.0;
					ellipse_ucs.origin+=(tube_vec*profile.common.fInnerLen/radiusRatio);
					ellipse_ucs.axis_z=pAssistantPart1->ucs.axis_z;
					ellipse_ucs.axis_y=tube_vec^pAssistantPart1->ucs.axis_z;
					normalize(ellipse_ucs.axis_y);
					ellipse_ucs.axis_x=ellipse_ucs.axis_y^ellipse_ucs.axis_z;
					normalize(ellipse_ucs.axis_x);
					f3dPoint tangPt1,tangPt2;
					f3dPoint vec=ucs.axis_x;
					vector_trans(vec,ellipse_ucs,FALSE);
					if(Tang2dle(vec,majorAxisLen,radius,tangPt1,tangPt2))
					{	//存在合理切点
						coord_trans(tangPt1,ellipse_ucs,TRUE);
						coord_trans(tangPt2,ellipse_ucs,TRUE);
						datum_point.Set(0,profile.common.fInnerLen,0);
						coord_trans(datum_point,ucs,TRUE);
						if(DISTANCE(datum_point,tangPt1)<DISTANCE(datum_point,tangPt2))
							datum_point=tangPt1;
						else 
							datum_point=tangPt2;
						//肘板坐标系需要调整 Y轴方向保持不变
						f3dPoint axis_y=datum_point-ucs.origin;
						normalize(axis_y);
						f3dPoint axis_z=axis_y^ucs.axis_y;
						if(ucs.axis_z*axis_z<0)
							ucs.axis_z=-1.0*axis_z;
						else
							ucs.axis_z=axis_z;
						normalize(ucs.axis_z);
						ucs.axis_x=ucs.axis_y^ucs.axis_z;
						normalize(ucs.axis_x);
					}
					else //不存在合理切点
					{
						datum_point.Set(0,profile.common.fInnerLen,0);
						coord_trans(datum_point,ucs,TRUE);
					}
				}
				coord_trans(datum_point,ucs,FALSE);
				datum_point.z=0;
				vertice.Set(profile.common.fCutAngleLen,0,0);
				PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE; 
				vertice.Set(profile.common.fBottomWidth,0,0);
				vertex_list.append(vertice);
				if((profile.common.fTopWidth+datum_point.x)<profile.common.fBottomWidth)	//避免出现凹形板
				{
					vertice.Set(profile.common.fBottomWidth,profile.common.fOuterLen,0);
					vertex_list.append(vertice);
				}
				vertice.Set(profile.common.fTopWidth+datum_point.x,profile.common.fInnerLen,0);
				vertex_list.append(vertice);
				
				f3dLine line(datum_point,f3dPoint(0,0,0));
				coord_trans(line.startPt,ucs,TRUE);
				coord_trans(line.endPt,ucs,TRUE);
				line_vec=line.startPt-line.endPt;
				normalize(line_vec);
				if(Int3dlc(&inters1,&inters2,line.startPt,line_vec,pDatumTube->Start(),pDatumTube->End(),radius,radius)>0)
				{	//圆弧边
					pVertex=vertex_list.append(datum_point);
					pVertex->type=3;
					pVertex->column_norm=pDatumTube->End()-pDatumTube->Start();
					normalize(pVertex->column_norm);
					pVertex->radius=-pDatumTube->GetDiameter()*0.5;
					pVertex->center=pDatumTube->Start();
					pVertex->m_bWeldEdge=TRUE;
					if(fabs(profile.common.fCutAngleLen)>eps)
					{
						f3dPoint e_vec(1,-1,0);
						f3dPoint startPt(profile.common.fCutAngleLen,0,0);
						coord_trans(startPt,ucs,TRUE);
						vector_trans(e_vec,ucs,TRUE);
						if(Int3dlc(&inters1,&inters2,startPt,e_vec,pDatumTube->Start(),pDatumTube->End(),radius,radius)>0)
						{
							f3dPoint inters_pos;
							if(DISTANCE(inters1,startPt)<DISTANCE(inters2,startPt))
								inters_pos=inters1;
							else
								inters_pos=inters2;
							coord_trans(inters_pos,ucs,FALSE);
							//应该确保Y坐标为E 暂时保证该点位于圆弧上
							//vertice.Set(E*datum_point.x/datum_point.y,E,0);
							vertice.Set(inters_pos.x,inters_pos.y);
							vertex_list.append(vertice);
						}
					}
				}
				else
				{	//无圆弧边
					vertice=datum_point;
					pVertex=vertex_list.append(vertice);
					pVertex->m_bWeldEdge=TRUE;
					if(fabs(profile.common.fCutAngleLen)>eps)
					{
						double k=0;
						if(fabs(datum_point.y)>eps)
							k=datum_point.x/datum_point.y;
						vertice.Set(profile.common.fCutAngleLen*k,profile.common.fCutAngleLen,0);
						pVertex=vertex_list.append(vertice);
						if(iCutMode>0)
						{
							pVertex->type=2;
							pVertex->radius=-profile.common.fCutAngleLen;
						}
					}
				}
			}
			else
			{	//沿钢管方向的肘板
				RIBPLATE_ELBOW elbow;
				elbow.LoadParams(this,pItemHashTbl);
				vector_trans(line_vec,ucs,FALSE);
				double k=0;
				if(fabs(line_vec.y)>eps)	//防止line_vec.y=0 
					k=line_vec.x/line_vec.y;
				vertice.Set(profile.common.fCutAngleLen,0,0);
				PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE; 
				vertice.Set(profile.common.fBottomWidth,0,0);
				vertex_list.append(vertice);
				if(fabs(profile.common.fTopWidth-profile.common.fBottomWidth)>EPS)
				{
					if(profile.common.fTopWidth>profile.common.fBottomWidth)		//顶部宽度大于底部宽度
						vertice.Set(profile.common.fTopWidth,profile.common.fInnerLen-profile.common.fOuterLen,0);
					else if(profile.common.fTopWidth<profile.common.fBottomWidth)	//顶部宽度小于底部宽度
						vertice.Set(profile.common.fBottomWidth,profile.common.fOuterLen,0);
					vertex_list.append(vertice);
				}
				vertice.Set(profile.common.fTopWidth+profile.common.fInnerLen*k,profile.common.fInnerLen,0);
				vertex_list.append(vertice);
				vertice.Set(profile.common.fInnerLen*k,profile.common.fInnerLen,0);
				pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE;
				if(fabs(profile.common.fCutAngleLen)>eps)
				{
					vertice.Set(profile.common.fCutAngleLen*k,profile.common.fCutAngleLen,0);
					pVertex=vertex_list.append(vertice);
					if(iCutMode>0)
					{
						pVertex->type=2;
						pVertex->radius=-profile.common.fCutAngleLen;
					}
				}
				CLsRef* pLsRef=GetFirstLsRef();
				CLDSBolt* pVirtualBolt=NULL;
				if(pLsRef==NULL)
					pVirtualBolt=(CLDSBolt*)BelongModel()->AppendPart(CLS_BOLT);
				else
					pVirtualBolt=pLsRef->GetLsPtr();
				pVirtualBolt->m_bVirtualPart=TRUE;
				pVirtualBolt->des_work_norm.norm_style=3;
				pVirtualBolt->des_work_norm.hVicePart=this->handle;
				pVirtualBolt->des_base_pos.cLocationStyle=1;	//LOCAL_COORD
				pVirtualBolt->des_base_pos.hPart=this->handle;
				pVirtualBolt->des_base_pos.R=0;
				pVirtualBolt->set_d(ftoi(elbow.H.value.fValue));	//孔径
				pVirtualBolt->hole_d_increment=elbow.H.value.fValue-pVirtualBolt->get_d();
				pVirtualBolt->des_base_pos.len_offset_dist=elbow.F.value.fValue;	//ucs.origin.x
				//TODO:ucs.origin.y计算还有问题，应参照角钢楞点计算代码 wjh-2015.8.8
				pVirtualBolt->des_base_pos.wing_offset_dist=elbow.G.value.fValue;	//ucs.origin.y
				pVirtualBolt->m_cFuncType=CLDSBolt::FUNC_WATERHOLE;
				if(pLsRef==NULL)
					pLsRef=AppendLsRef(pVirtualBolt->GetLsRef(),FALSE);
			}
		}
		else if(pDatumNode)
		{	//钢管上无关联钢板的肋板以钢管始->终方向为X轴正向，翼展方向为Y轴正向(出于操作习惯考虑)
			SnapPerp(&ucs.origin,pDatumTube->Start(),pDatumTube->End(),pDatumNode->Position(true));
			//调换X和Y轴方向
			f3dPoint temp_vec=ucs.axis_x;
			ucs.axis_x=-ucs.axis_y;
			ucs.axis_y=temp_vec;
			if(work_norm_style==2)
			{
				double fOffsetM=0,fOffsetX=0,fMaxWidth=0;
				GetDesignItemValue(KEY2C("DM"),&fOffsetM,pItemHashTbl);		
				GetDesignItemValue(KEY2C("DX"),&fOffsetX,pItemHashTbl);		
				f3dPoint axis_vec=pDatumTube->End()-pDatumTube->Start();
				normalize(axis_vec);	//默认始->终为轴向偏移正方向
				ucs.origin+=axis_vec*fOffsetM;

				f3dPoint line_pick,inner_arc_start,inner_arc_end,arc_start,arc_end,origin;
				//原点坐标
				double radius = pDatumTube->GetDiameter()*0.5;
				if(fabs(fOffsetX)>0)
					ucs.origin+=ucs.axis_x*fOffsetX;
				if(Int3dlc(&inner_arc_start,&inner_arc_end,ucs.origin,ucs.axis_y,pDatumTube->Start(),pDatumTube->End(),radius,radius)!=2)
					return FALSE;
				double complement=(inner_arc_start-ucs.origin)*ucs.axis_y;	//高度补偿值
				if(complement>0)
					ucs.origin=inner_arc_start;
				else
				{
					complement=(inner_arc_end-ucs.origin)*ucs.axis_y;
					ucs.origin=inner_arc_end;
				}
				double cosa=fabs(ucs.axis_z*axis_vec);
				if(cosa>0)
					complement=radius/cosa-complement;
				else
					complement=0;
				arc_start=ucs.origin-ucs.axis_x*profile.common.fBottomWidth;
				if(Int3dlc(&inner_arc_start,&inner_arc_end,arc_start,ucs.axis_y,pDatumTube->Start(),pDatumTube->End(),radius,radius)!=2)
					return FALSE;
				if((inner_arc_start-arc_start)*ucs.axis_y>0)
					arc_start=inner_arc_start;
				else
					arc_start=inner_arc_end;
				//第一个顶点,圆弧终点（原点）
				vertice.feature=1;
				vertice.Set();
				vertex_list.append(vertice);
				//第二个顶点
				//vertice.x+=fInnerLen;
				vertice.Set(0,profile.common.fInnerLen+complement,0);
				vertex_list.append(vertice);
				//第三个顶点
				//vertice.y+=fBottomWidth;
				vertice.Set(-profile.common.fBottomWidth,profile.common.fInnerLen+complement,0);
				vertex_list.append(vertice);
				//第四个顶点,圆弧起点
				coord_trans(arc_start,ucs,FALSE);
				arc_start.z=0;
				arc_start.feature=1;
				PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(arc_start));
				if(fabs(ucs.axis_z*pDatumTube->ucs.axis_z)>EPS_COS)
				{	//圆弧
					pVertex->type=2;
					pVertex->center.Set(0,-100,0);	//给定圆弧拾取点，防止拾取点与圆弧的始终端点重合，导致无法确定圆心位置
					pVertex->radius=-radius;
				}
				else
				{	//椭圆弧
					pVertex->type=3;
					f3dPoint center;
					SnapPerp(&center,pDatumTube->Start(),pDatumTube->End(),ucs.origin);
					pVertex->center=center;
					if(ucs.axis_z*pDatumTube->ucs.axis_z>0)
						pVertex->column_norm=pDatumTube->ucs.axis_z;
					else
						pVertex->column_norm=-pDatumTube->ucs.axis_z;
					pVertex->radius=-radius;
				}
				pVertex->m_bWeldEdge=TRUE;
			}
			else
			{
				ucs.origin+=ucs.axis_y*pDatumTube->GetDiameter()*0.5;
				/*ucs.axis_x=ucs.axis_y^ucs.axis_z;
				normalize(ucs.axis_x);
				ucs.axis_z=ucs.axis_x^ucs.axis_y;
				normalize(ucs.axis_z);*/
				if(hDatumPart<0)
				{	//X轴与钢管轴向相反
					ucs.axis_x*=-1.0;
					ucs.axis_z=ucs.axis_x^ucs.axis_y;
					normalize(ucs.axis_z);
				}
				double fOffsetM=0;
				GetDesignItemValue(KEY2C("DM"),&fOffsetM,pItemHashTbl);		
				ucs.origin+=line_vec*fOffsetM;	//Y方向偏移量
				//钢管+节点
				vertice.Set(0,0,0);
				vertex_list.append(vertice);
				vertice.Set(0,profile.common.fInnerLen,0);
				vertex_list.append(vertice);
				vertice.Set(-profile.common.fBottomWidth,profile.common.fInnerLen,0);
				vertex_list.append(vertice);
				vertice.Set(-profile.common.fBottomWidth,0,0);
				PROFILE_VERTEX *pVertex=vertex_list.append(vertice);
				pVertex->m_bWeldEdge=TRUE;
			}
		}
	}
	
	return TRUE;
}
BOOL CLDSParamPlate::GetStdPartParamValue(WORD wKey, double *pfValue)
{
	if(!m_bStdPart)
		return FALSE;
	if(pfValue==NULL)
		return FALSE;
	//使用该函数时如果连续查询多个参数值，会造成重复查询  wht 10-04-06
	CLDSStdPartModel stdPartModel;
	if(!GetStdPartModel(&stdPartModel))
		return FALSE;
	if(m_iParamType==TYPE_UEND||m_iParamType==TYPE_ROLLEND||m_iParamType==TYPE_XEND)
	{
		if(wKey=='A')
		{
			*pfValue=stdPartModel.param.insert_plate_param.A;
			return TRUE;
		}
		else if(wKey=='d')
		{
			*pfValue=stdPartModel.param.insert_plate_param.d;
			return TRUE;
		}
		else if(wKey=='t')
		{
			*pfValue=stdPartModel.param.insert_plate_param.t;
			return TRUE;
		}
		else if(wKey=='H')
		{
			*pfValue=stdPartModel.param.insert_plate_param.H;
			return TRUE;
		}
		else if(wKey=='L')
		{
			*pfValue=stdPartModel.param.insert_plate_param.L;
			return TRUE;
		}
		else if(wKey=='M')
		{
			*pfValue=stdPartModel.param.insert_plate_param.M;
			return TRUE;
		}
		else if(wKey=='N')
		{
			*pfValue=stdPartModel.param.insert_plate_param.N;
			return TRUE;
		}
		else if(wKey=='R')
		{
			*pfValue=stdPartModel.param.insert_plate_param.R;
			return TRUE;
		}
		else if(wKey=='S')
		{
			*pfValue=stdPartModel.param.insert_plate_param.S;
			return TRUE;
		}
		else if(wKey=='T')
		{
			*pfValue=stdPartModel.param.insert_plate_param.Thick;
			return TRUE;
		}
		else if(wKey=='W')
		{
			*pfValue=stdPartModel.param.insert_plate_param.W;
			return TRUE;
		}
	}
	else if(m_iParamType==TYPE_FLP||m_iParamType==TYPE_FLD)
	{
		if(wKey=='A')
		{
			*pfValue=stdPartModel.param.fl_param.A;
			return TRUE;
		}
		else if(wKey=='B')
		{
			*pfValue=stdPartModel.param.fl_param.B;
			return TRUE;
		}
		else if(wKey=='C')
		{
			*pfValue=stdPartModel.param.fl_param.C;
			return TRUE;
		}
		else if(wKey=='D')
		{
			*pfValue=stdPartModel.param.fl_param.D;
			return TRUE;
		}
		else if(wKey=='H')
		{
			*pfValue=stdPartModel.param.fl_param.H;
			return TRUE;
		}
		else if(wKey=='O')
		{
			*pfValue=stdPartModel.param.fl_param.H1;
			return TRUE;
		}
		else if(wKey=='K')
		{
			*pfValue=stdPartModel.param.fl_param.K;
			return TRUE;
		}
		else if(wKey=='L')
		{
			*pfValue=stdPartModel.param.fl_param.L;
			return TRUE;
		}
		else if(wKey=='N')
		{
			*pfValue=stdPartModel.param.fl_param.N;
			return TRUE;
		}
		else if(wKey=='R')
		{
			*pfValue=stdPartModel.param.fl_param.R;
			return TRUE;
		}
		else if(wKey=='S')
		{
			*pfValue=stdPartModel.param.fl_param.S;
			if(m_iParamType==TYPE_FLD)
				*pfValue+=stdPartModel.m_fRelateTubeThick;
			return TRUE;
		}
		else if(wKey=='d')
		{
			*pfValue=stdPartModel.param.fl_param.d;
			return TRUE;
		}
		else if(wKey=='t')
		{
			*pfValue=stdPartModel.param.fl_param.t;
			return TRUE;
		}
	}
	else if(m_iParamType==TYPE_FLG||m_iParamType==TYPE_FLR)
	{	//为统一查询此处与TYPE_FL使用同样的标示字符
		if(wKey=='A')
		{	//法兰圆弧角，此处返回360
			*pfValue=360;
			return TRUE;
		}
		if(wKey=='W')
		{	
			*pfValue=stdPartModel.param.fl_param2.fOuterD;
			return TRUE;
		}
		else if(wKey=='R')
		{
			*pfValue=stdPartModel.param.fl_param2.fInnerD*0.5;
			return TRUE;
		}
		else if(wKey=='T')
		{
			*pfValue=stdPartModel.param.fl_param2.fThick;
			return TRUE;
		}
		else if(wKey=='H')
		{
			*pfValue=stdPartModel.param.fl_param2.fH;
			return TRUE;
		}
		else if(wKey=='K')
		{
			*pfValue=stdPartModel.param.fl_param2.fBoltCirD;
			return TRUE;
		}
	}
	return FALSE;
}

//返回参数化钢板的基准节点
CLDSNode *CLDSParamPlate::GetBaseNode(bool bClearLocks/*=true*/)
{
	CDepthCounter<BYTE> visiter(NULL);//避免组焊件焊接父构件导致的死循环
	if(!bClearLocks&&IsLocked())
		return NULL;	//内部调用时发现该板已在之前分步中调用过，为防止死循环直接返回NULL wjh-2016.10.24
	else if(!IsLocked())
		visiter.AttachCounter(&m_bLocked);
	//此刻已可以保证m_bLocked=true
	long hBaseNode=0;
	CLDSNode *pBaseNode=NULL;
	if(IsFL()||m_iParamType==TYPE_UEND||m_iParamType==TYPE_ROLLEND||m_iParamType==TYPE_XEND)
	{	//钢管端连接
		CLDSLineTube *pLineTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
		if(pLineTube)
		{
			if(pLineTube->m_tJointStart.hLinkObj==handle&&pLineTube->pStart)
				return pLineTube->pStart;
			else if(pLineTube->m_tJointEnd.hLinkObj==handle&&pLineTube->pEnd)
				return pLineTube->pEnd;
		}
	}	
	else if(m_iParamType==TYPE_CIRCOVERPLATE)	//环向封板
		GetDesignItemValue('E',&hBaseNode);
	else if(m_iParamType==TYPE_CIRRIBPLATE)		//环向肋板
		GetDesignItemValue('P',&hBaseNode);		//'A'改为'P' wht 11-05-20
	else if(m_iParamType==TYPE_TUBERIBPLATE)	//跨越式钢管连板
		GetDesignItemValue('P',&hBaseNode);
	else if(m_iParamType==TYPE_LINEANGLERIBPLATE//角钢肋板
		||m_iParamType==TYPE_WATERSHIELDEDPLATE)//遮水板
		GetDesignItemValue('B',&hBaseNode);
	else if(m_iParamType==TYPE_ELBOWPLATE)		//肘板
	{
		long hPlate=0;
		GetDesignItemValue('L',&hPlate);
		CSuperSmartPtr<CLDSPart> pPlate;
		pPlate=BelongModel()->FromPartHandle(hPlate,CLS_PLATE,CLS_PARAMPLATE);
		if(pPlate.IsHasPtr())
		{
			if(pPlate->GetClassTypeId()==CLS_PLATE)
				return pPlate.PlatePointer()->GetBaseNode(false);
			else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
				return pPlate.ParamPlatePointer()->GetBaseNode(false);
		}
	}
	else if(m_iParamType==TYPE_RIBPLATE)
	{	//肋板 wht 11-03-24
		if(!GetDesignItemValue('B',&hBaseNode))
		{
			long hPlate=0;
			GetDesignItemValue('C',&hPlate);
			CSuperSmartPtr<CLDSPart> pPlate;
			pPlate=BelongModel()->FromPartHandle(hPlate,CLS_PLATE,CLS_PARAMPLATE);
			if(pPlate.IsHasPtr())
			{
				if(pPlate->GetClassTypeId()==CLS_PLATE)
					return pPlate.PlatePointer()->GetBaseNode(false);
				else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
					return pPlate.ParamPlatePointer()->GetBaseNode(false);
			}
		}
	}
	else if(m_iParamType==TYPE_BASEPLATE)
		GetDesignItemValue('A',&hBaseNode);	
	else if(m_iParamType==TYPE_SHOEPLATE)
	{
		long hBasePlate=0;
		GetDesignItemValue('C',&hBasePlate);
		CSuperSmartPtr<CLDSPart> pBasePlate;
		pBasePlate=BelongModel()->FromPartHandle(hBasePlate,CLS_PLATE,CLS_PARAMPLATE);
		if(pBasePlate.IsHasPtr())
		{
			if(pBasePlate->GetClassTypeId()==CLS_PLATE)
				return pBasePlate.PlatePointer()->GetBaseNode(false);
			else if(pBasePlate->GetClassTypeId()==CLS_PARAMPLATE)
				return pBasePlate.ParamPlatePointer()->GetBaseNode(false);
		}
	}
	else if(m_iParamType==TYPE_TUBESTAYWIREPLATE)
	{
		long hBasePlate=0;
		GetDesignItemValue('C',&hBasePlate);
		CSuperSmartPtr<CLDSPart> pBasePlate;
		pBasePlate=BelongModel()->FromPartHandle(hBasePlate,CLS_PLATE,CLS_PARAMPLATE);
		if(pBasePlate.IsHasPtr())
		{
			if(pBasePlate->GetClassTypeId()==CLS_PLATE)
				return pBasePlate.PlatePointer()->GetBaseNode(false);
			else if(pBasePlate->GetClassTypeId()==CLS_PARAMPLATE)
				return pBasePlate.ParamPlatePointer()->GetBaseNode(false);
		}
	}
	//垂向加劲板以及封头板为旧版本中使用 此处不用判断
	//else if(m_iParamType==TYPE_VERTPLATE||m_iParamType==TYPE_ENDPLATE)
	if(hBaseNode<0X20)
	{	//根据焊接父构件查找基准节点
		CSuperSmartPtr<CLDSPart> pPlate;
		pPlate=BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_PLATE,CLS_PARAMPLATE);
		if(pPlate.IsHasPtr())
		{
			if(pPlate->GetClassTypeId()==CLS_PLATE)
				return pPlate.PlatePointer()->GetBaseNode(false);
			else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
				return pPlate.ParamPlatePointer()->GetBaseNode(false);
		}
	}
	else
		pBaseNode=BelongModel()->FromNodeHandle(hBaseNode);
	return pBaseNode;
}

//克隆参数化钢板
void CLDSParamPlate::CloneTo(CLDSParamPlate &plate)
{
	CBuffer buffer;
	ToBuffer(buffer);
	buffer.SeekToBegin();
	plate.FromBuffer(buffer);
}

bool CLDSParamPlate::ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType/*=0*/)
{
	bool replaced=replacer.VerifyAndReplace(&handle,true);
	replacer.SelectPerfectPartHandle(ucs.origin);
	replaced|=replacer.VerifyAndReplace(&_hPartWeldParent);
	for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
		replaced|=replacer.VerifyAndReplace(&pRelaObj->hObj);
	//连接方向列表 wht 11-07-08
	CONNECT_DIRECTION *pConVec=NULL;
	for(pConVec=ConnDirectList.GetFirst();pConVec;pConVec=ConnDirectList.GetNext())
		replaced|=pConVec->direct.ReplaceReferenceHandle(replacer);
	//设计参数 
	DESIGN_PLATE_ITEM *pItem=NULL;
	for(pItem=params.GetFirst();pItem;pItem=params.GetNext())
	{
		long hLegacy=abs(pItem->value.hPart);
		if(pItem->dataType==1&&replacer.VerifyAndReplace(&hLegacy))
		{
			replaced=true;
			if(pItem->value.hPart<0)
				pItem->value.hPart=-1*hLegacy;
			else 
				pItem->value.hPart=hLegacy;
		}
	}
	if(replaced)
		SetModified();
	return replaced;
}
long CLDSParamPlate::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	UINT nInitCount=pListHandlers->Count;
	pListHandlers->Append(HANDLE_ADDR(&handle));
	pListHandlers->Append(HANDLE_ADDR(&_hPartWeldParent));
	for (RELATIVE_OBJECT *pRelaObj = relativeObjs.GetFirst(); pRelaObj; pRelaObj = relativeObjs.GetNext())
		pListHandlers->Append(HANDLE_ADDR(&pRelaObj->hObj));
	//连接方向列表 wht 11-07-08
	CONNECT_DIRECTION *pConVec = NULL;
	for (pConVec = ConnDirectList.GetFirst(); pConVec; pConVec = ConnDirectList.GetNext())
		pConVec->direct.StatObjHandleReference(pListHandlers);
	//设计参数 
	DESIGN_PLATE_ITEM *pItem = NULL;
	for (pItem = params.GetFirst(); pItem; pItem = params.GetNext())
	{
		long hLegacy = abs(pItem->value.hPart);
		if (pItem->dataType == 1)
			pListHandlers->Append(HANDLE_ADDR(&pItem->value.hPart));
	}
	return pListHandlers->Count- nInitCount;
}

bool CLDSParamPlate::HasPreferShadowDirection(double* vShadowDirection)
{
	return false;
}
CLDSPart* CLDSParamPlate::NewCopy(bool bByShadowMode/*=false*/)
{
	if(bByShadowMode)
		return CLDSPart::NewCopy(true);
	return NULL;
}
CLDSGeneralPlate* CLDSParamPlate::CloneCopy(double offset,double* vUdfDirection)
{
	return NULL;
}
CLDSGeneralPlate* CLDSParamPlate::ShadowCopy(double offset,double* vUdfDirection)
{
	return NULL;
}
CLDSGeneralPlate* CLDSParamPlate::CloneCopy(GEPOINT vOffset,bool localAcsCoord/*=true*/)
{
	return NULL;
}
CLDSGeneralPlate* CLDSParamPlate::ShadowCopy(double offset,bool localAcsCoord/*=true*/)
{
	return NULL;
}

double CLDSParamPlate::GetOutterPlaneOffset(const f3dPoint &outter_vec)
{
	if( m_iParamType==TYPE_FL  ||		//法兰盘
		m_iParamType==TYPE_FLG||	//刚性法兰
		m_iParamType==TYPE_FLR||//柔性法兰
		m_iParamType==TYPE_ENDPLATE||	//封头板
		m_iParamType==TYPE_WATERSHIELDEDPLATE)
	{
		if(outter_vec*ucs.axis_z>0)
			return m_fNormOffset+GetThick();
		else
			return m_fNormOffset;
	}
	else if(m_iParamType==TYPE_ROLLEND||	//槽型插板
			m_iParamType==TYPE_ELBOWPLATE||
			m_iParamType==TYPE_CIRRIBPLATE||
			m_iParamType==TYPE_CIRCOVERPLATE||
			m_iParamType==TYPE_TUBERIBPLATE||
			m_iParamType==TYPE_VERTPLATE||	//垂向加劲板
			m_iParamType==TYPE_LINEANGLERIBPLATE||
			m_iParamType==TYPE_XEND||
			m_iParamType==TYPE_RIBPLATE)	//肋板
	{
		if(outter_vec*ucs.axis_z>0)
			return 0.5*GetThick();
		else
			return -0.5*GetThick();
	}
	else
		return 0;
}

//得到十字插板上下两块立板 wht 10-07-06
typedef CLDSParamPlate* PLATEPTR;
BOOL CLDSParamPlate::GetVertPlateInXEndPlate(CLDSPlate **pUpPlate,CLDSPlate **pDownPlate)
{
	if(m_iParamType!=TYPE_XEND)
		return FALSE;
	ATTACHPART_ITEM *pItem=NULL;
	double M=0;
	GetDesignItemValue('M',&M);
	for(pItem=GetFirstAttachPart();pItem;pItem=GetNextAttachPart())
	{
		CLDSPlate *pPlate=(CLDSPlate*)m_pModel->FromPartHandle(pItem->h,CLS_PLATE);
		if(pPlate==NULL)
			continue;
		f3dPoint plate_pos=pPlate->ucs.origin;
		//转换到十字插板坐标系下
		coord_trans(plate_pos,ucs,FALSE);
		if(plate_pos.x>M)
			continue;
		if(plate_pos.x<EPS2&&plate_pos.z>0)
			*pUpPlate=pPlate;
		else if(plate_pos.x<EPS2&&plate_pos.z<0)
			*pDownPlate=pPlate;
	}
	return TRUE;
}
//检查插板外形是否合法，目前主要检查边距 暂未使用 wht 11-04-16
BOOL CLDSParamPlate::IsValidProfile(double epsilon/*=1.0*/)
{
	if(vertex_list.GetNodeNum()<3)
	{
		DesignPlate();
		if(vertex_list.GetNodeNum()<3)
			return TRUE;
	}
	if(m_iParamType!=TYPE_ROLLEND&&m_iParamType!=TYPE_XEND&&m_iParamType!=TYPE_UEND)
		return TRUE;
	int i=0,n=vertex_list.GetNodeNum();
	CArray<PROFILE_VERTEX,PROFILE_VERTEX&>vertexArr;
	vertexArr.SetSize(n);
	f3dPoint *pnt_arr=new f3dPoint[n];
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		pnt_arr[i]=pVertex->vertex;
		vertexArr[i]=*pVertex;
		i++;
	}
	//
	fPolygonRgn rgn;
	if(rgn.CreatePolygonRgn(pnt_arr,n))
	{
		f3dPoint ls_pos,perp;
		double dist,SpaceLim;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if((*pLsRef)->get_d()==12)
				SpaceLim=20;
			else if((*pLsRef)->get_d()==16)
				SpaceLim=25;
			else if((*pLsRef)->get_d()==20)
				SpaceLim=30;
			else// if((*pLsRef)->get_d()==24)
				SpaceLim=40;
			//
			f3dPoint ls_pos=(*pLsRef)->ucs.origin;
			project_point(ls_pos,ucs.origin,ucs.axis_z);
			coord_trans(ls_pos,ucs,FALSE);
			if(rgn.PtInRgn(ls_pos)!=1)
			{
				delete []pnt_arr;
				return FALSE;
			}
			for(i=0;i<n;i++)
			{
				if(vertexArr[i].type==0)	//指定夹角
				{
					f3dLine line(pnt_arr[i],pnt_arr[(i+1)%n]);
					if(SnapPerp(&perp,line,ls_pos,&dist))
					{
						int retCode=line.PtInLine(perp);
						if(retCode>0&&dist<SpaceLim-epsilon)	//垂足点在线段上
						{
							delete []pnt_arr;
							return FALSE;
						}
						else if(retCode==-1&&DISTANCE(ls_pos,line.endPt)<SpaceLim-epsilon)
						{
							delete []pnt_arr;
							return FALSE;
						}
						else if(retCode==-2&&DISTANCE(ls_pos,line.startPt)<SpaceLim-epsilon)
						{
							delete []pnt_arr;
							return FALSE;
						}
					}
					/*else	//技术还不成熟不足以判断螺栓到圆弧边的距离是否满足
					{
					f3dArcLine arcline;
					vertexArr[i].RetrieveArcLine(arcline,vertexArr[(i+1)%vertexArr.GetSize()].vertex,&ucs);
					dist=DISTANCE(arcline.Center(),ls_pos);
					if(dist-arcline.Radius()
					}*/
				}
			}
		}
	}
	else
	{
		delete []pnt_arr;
		return FALSE;
	}
	delete []pnt_arr;
	return TRUE;	//边距合法
}

CONNECT_DIRECTION *CLDSParamPlate::GetRelaConnInfo(int serial)
{
	CONNECT_DIRECTION *pConnectDir=NULL;
	for(pConnectDir=ConnDirectList.GetFirst();pConnectDir;pConnectDir=ConnDirectList.GetNext())
	{
		if(pConnectDir->serial==serial)
			break;
	}
	return pConnectDir;
}

//<DEVELOP_PROCESS_MARK nameId="CLDSParamPlate::GetRelaConnVec">
//返回指定关联面的连接方向(焊缝走向)
f3dPoint CLDSParamPlate::GetRelaConnVec(int serial,CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	f3dPoint conn_vec;
	long h=0;
	CLDSPart *pRelaPart=NULL;	//支持关联钢板为参数化钢板的情况 wht 11-06-03
	CLDSLineTube *pRelaTube=NULL;
	if(serial==1&&GetDesignItemValue('E',&h,pItemHashTbl))
		pRelaPart=m_pModel->FromPartHandle(h,CLS_PLATE,CLS_PARAMPLATE);
	else if(serial==2&&GetDesignItemValue('F',&h,pItemHashTbl))
		pRelaPart=m_pModel->FromPartHandle(h,CLS_PLATE,CLS_PARAMPLATE);
	if(serial==2&&pRelaPart==NULL)
	{
		pRelaTube=(CLDSLineTube*)m_pModel->FromPartHandle(h,CLS_LINETUBE);
		if(pRelaTube==NULL)
		{	//第二个关联构件为空且关联基板为贯通钢板,第二个连接方向为第一个连接方向的负方向 wht 11-05-20
			GetDesignItemValue('E',&h,pItemHashTbl);
			CLDSPlate *pFirstRelaPlate=(CLDSPlate*)m_pModel->FromPartHandle(h,CLS_PLATE);
			if(pFirstRelaPlate&&pFirstRelaPlate->IsTransPlate())
				return -1.0*GetRelaConnVec(1,pItemHashTbl);
		}
	}
	if(pRelaPart==NULL&&pRelaTube==NULL)
		return conn_vec;
	if(pRelaTube)
	{
		if(pRelaTube->pStart&&GetDesignItemValue('P',&h,pItemHashTbl)&&pRelaTube->pStart->handle==h)
			conn_vec=pRelaTube->End()-pRelaTube->Start();
		else
			conn_vec=pRelaTube->Start()-pRelaTube->End();
	}
	else
	{
		CONNECT_DIRECTION *pConnectDir=GetRelaConnInfo(serial);
		if(pConnectDir)
		{
			pConnectDir->direct.UpdateVector(BelongModel());	//根据参数计算方向
			if(pConnectDir->style==0)
			{
				conn_vec=pConnectDir->direct.vector;
				project_vector(conn_vec,pRelaPart->ucs.axis_z);
			}
			else
				conn_vec=pRelaPart->ucs.axis_z^pConnectDir->direct.vector;
		}
		if(conn_vec.IsZero())	//焊缝走向为默认法向的环向板与关联板的两面交线
			conn_vec=ucs.axis_z^pRelaPart->ucs.axis_z;
	}
	//修正射线方向的正负号
	//判断基准钢板是否为贯通连板 在钢板X轴一侧生成肋板 wht 10-03-22
	f3dPoint vertice_vec;
	CLDSPlate *pPlate=NULL;
	if(pRelaPart&&pRelaPart->GetClassTypeId()==CLS_PLATE)
		pPlate=(CLDSPlate*)pRelaPart;
	if(pPlate&&pPlate->IsTransPlate())	//贯通钢管的连板，可能会出现_/_|_\_情况的环向肋板 wjh-2011.3.31
	{	//对于贯通关联板为第一块时取X轴正向为连接方向的校验方向，否则取X轴负向为校验方向
		if(serial==1)
			vertice_vec= pPlate->ucs.axis_x;
		else
			vertice_vec=-pPlate->ucs.axis_x;
	}
	else if(pPlate)
	{
		f3dPoint vertice=CLDSPlate::GetGeomCenter(pPlate);
		coord_trans(vertice,pPlate->ucs,TRUE);
		vertice_vec=vertice-ucs.origin;
	}
	if(vertice_vec*conn_vec<0)
		conn_vec*=-1.0;
	normalize(conn_vec);
	return conn_vec;
}
//</DEVELOP_PROCESS_MARK>

//返回指定关联面的工作法向
f3dPoint CLDSParamPlate::GetRelaConnPlaneNorm(int serial,CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/)
{
	CONNECT_DIRECTION *pConnectDir=NULL;
	for(pConnectDir=ConnDirectList.GetFirst();pConnectDir;pConnectDir=ConnDirectList.GetNext())
	{
		if(pConnectDir->serial==serial)
			break;
	}
	long h;
	f3dPoint norm;
	CLDSLineTube *pDatumTube=NULL,*pRelaTube=NULL;
	if(GetDesignItemValue('C',&h,pItemHashTbl))
		pDatumTube=(CLDSLineTube*)m_pModel->FromPartHandle(h,CLS_LINETUBE);
	if(pConnectDir)
	{
		pConnectDir->direct.UpdateVector(BelongModel()); //根据参数计算方向
		if(pConnectDir->style==1)
			norm=pConnectDir->direct.vector;
		else if(pConnectDir->style==0)
		{
			CLDSPart *pRelaPart=NULL;	//支持关联钢板为参数化钢板的情况 wht 11-06-03
			if(serial==1&&GetDesignItemValue('E',&h,pItemHashTbl))
				pRelaPart=m_pModel->FromPartHandle(h,CLS_PLATE,CLS_PARAMPLATE);
			else if(serial==2&&GetDesignItemValue('F',&h,pItemHashTbl))
				pRelaPart=(CLDSPlate*)m_pModel->FromPartHandle(h,CLS_PLATE,CLS_PARAMPLATE);
			if(serial==2&&pRelaPart==NULL)
				pRelaTube=(CLDSLineTube*)m_pModel->FromPartHandle(h,CLS_LINETUBE);
			if(pRelaPart)
				norm=pRelaPart->ucs.axis_z^pConnectDir->direct.vector;
			else if(pRelaTube&&pDatumTube)
			{
				pDatumTube->BuildUCS();
				pRelaTube->BuildUCS();
				f3dPoint axis_z=pRelaTube->ucs.axis_z^pDatumTube->ucs.axis_z;
				normalize(axis_z);
				norm=axis_z^pConnectDir->direct.vector;
			}
		}
	}
	//钢板基准面法线无关联方向时默认为钢管延伸方向 wht 11-05-13
	else if(pDatumTube&&serial==1)
		norm=pDatumTube->End()-pDatumTube->Start();
	normalize(norm);
	return norm;
}
long CLDSParamPlate::GetRelaConnVecLinePart(int serial)
{
	long hPart=0;
	CONNECT_DIRECTION *pConnectDir=NULL;
	for(pConnectDir=ConnDirectList.GetFirst();pConnectDir;pConnectDir=ConnDirectList.GetNext())
	{
		if(pConnectDir->serial==serial)
		{
			if(pConnectDir->direct.norm_style>0&&pConnectDir->direct.norm_style<6)
				hPart=pConnectDir->direct.hVicePart;
			break;
		}
	}
	return hPart;
}

//根据序列号删除关联方向 wht 11-05-18
void CLDSParamPlate::DeleteRelaConnVec(int serial)
{
	CONNECT_DIRECTION *pVec=NULL;
	BOOL bPush=ConnDirectList.push_stack();
	for(pVec=ConnDirectList.GetFirst();pVec;pVec=ConnDirectList.GetNext())
	{
		if(pVec->serial==serial)
		{
			ConnDirectList.DeleteCursor(FALSE);
			break;
		}
	}
	ConnDirectList.Clean();
	if(bPush)
		ConnDirectList.pop_stack();
}

//要求输入的顶点必须是此连接板上的顶点,否则
//可能不进行转换或转换错误(由展开相对坐标-->实际相对坐标)
//bSpreadVertex=true表示展开顶点false表示展开矢量方向
f3dPoint CLDSParamPlate::GetRealVertex(f3dPoint vertice,bool bCalVertex/*=true*/)
{
	BYTE cbFlag=FROM_ACS|TO_ACS;
	if(bCalVertex)
		cbFlag|=IS_VERTEX;
	else
		cbFlag|=IS_VECTOR;
	f3dPoint real_vertice=TransToActual(vertice,vertice.feature,cbFlag);
	real_vertice.feature=vertice.feature;
	return real_vertice;
}
GEPOINT CLDSParamPlate::TransToActual(const double* coord3d,int iResidePlane/*=1*/,BYTE cbFlag/*=0*/)
{
	GEPOINT vertice(coord3d);
	bool bCalVertex = ((cbFlag&IS_VECTOR)==0);	//因IS_VERTEX为0, 故不能为if(cbFlag&IS_VERTEX)
	if(IsZhiWan()&&(iResidePlane==2||iResidePlane==3))
	{
		f3dLine huoqu_line;
		double huoqu_angle=GetHuoQuAngle(iResidePlane-2,&huoqu_line);	
		if(bCalVertex)
			rotate_point_around_axis(vertice,huoqu_angle,huoqu_line);
		else
			RotateVectorAroundVector(vertice,huoqu_angle,huoqu_line.endPt-huoqu_line.startPt);
	}
	if(cbFlag&TO_MCS)
	{
		if(bCalVertex)
			coord_trans(vertice,ucs,TRUE,TRUE);
		else
			vector_trans(vertice,ucs,TRUE,TRUE);
	}
	return vertice;	
}
f3dPoint CLDSParamPlate::GetSpreadVertex(const double* V,int face_i,bool bSpreadVertex/*=true*/)
{
	if(face_i==2)
		return SpreadTransV(V,bSpreadVertex);
	else 
		return V;
}
//将制弯面上的相对坐标（方向）-->展开至钢板相对（装配）坐标系;
//bSpreadVertex=true表示展开顶点false表示展开矢量方向
f3dPoint CLDSParamPlate::SpreadTransV(f3dPoint V,bool bSpreadVertex/*=true*/)
{
	if(keyvecset.huoqufacenorm.IsZero())
		return V;	//无火曲面法线，可能不是制弯板不需要展开转换
	/*double cosa=keyvecset.huoqufacenorm*ucs.axis_z;
	if(cosa>1.0)
		cosa=1.0;
	if(cosa<-1.0)
		cosa=-1.0;
	if(cosa>EPS_COS)
		return V;	//不是制弯板不需要展开转换
	double alfa=ACOS(cosa);*/
	f3dLine huoqu_line;
	double alfa=GetHuoQuAngle(0,&huoqu_line);
	if(bSpreadVertex)
		rotate_point_around_axis(V,-alfa,huoqu_line);
	else
		RotateVectorAroundVector(V,-alfa,huoqu_line.endPt-huoqu_line.startPt);
	return V;
}
f3dPoint CLDSParamPlate::HuoquDirection()
{
	f3dPoint direction;
	if(keyvecset.huoqufacenorm.IsZero())
		return direction;
	direction=keyvecset.huoquaxis_vec;
	RotateVectorAroundVector(direction,-0.5*Pi,keyvecset.huoqufacenorm);
	return direction;
}
//获取环向板顶点集合,主要在出图时使用 wht 11-05-30
CIRCULAR_PLATE_VERTEXSET CLDSParamPlate::GetCirPlateVertexSet()
{
	if(vertexset.inner_arc_start.IsZero()&&vertexset.inner_arc_end.IsZero())
		DesignPlate(false);	//内圆弧起始点为0时需要重新设计 
	return vertexset;
}
//返回值表示是否存在内圆,如果存在r表示内圆半径,column_norm表示圆柱法线 wht 11-05-27
BOOL CLDSParamPlate::GetInnerCirRadius(double &r,double* column_norm)
{	//该函数目前仅用于判断环向板是否存在内圆
	
	/*if(m_iParamType==TYPE_TUBERIBPLATE)
	{	//旧版本文件,跨越连板F==1表示存在内圆,目前已去掉该类钢板的生成
		double F=-1;
		GetDesignItemValue('F',&F);
	}
	else */
	if(m_iParamType!=TYPE_CIRRIBPLATE)
		return FALSE;
	long E=0;
	GetDesignItemValue('E',&E);		//第一块定位基准钢板
	CLDSPart *pRelaPlate1=BelongModel()->FromPartHandle(E,CLS_PLATE,CLS_PARAMPLATE);
	if(pRelaPlate1)	
		return FALSE;	//存在第一块基准钢板不可能有内圆
	double B=0;
	if(GetDesignItemValue('B',&B)&&B<360)
		return FALSE;	//非整圆,不必生成内圆弧
	r=-1;	//默认为根据钢管生成内圆
	//r:环向板内圆半径(异形法兰或360的环向板使用 
	//r<0根据钢管生成内圆 r=0无内圆 r>0按指定半径生成内圆) wht 11-05-27
	GetDesignItemValue('r',&r);
	if(fabs(r)<EPS)	//1.无内圆
		return FALSE;
	else if(r>0)	//2.按指定半径生成内圆
	{
		if(column_norm)
			memcpy(column_norm,(double*)ucs.axis_z,24);
		return TRUE;
	}
	//3.根据钢管生成内圆
	long C=0;
	double V=0;
	GetDesignItemValue('V',&V);	//V:加工间隙(基准钢管外壁到环向板内环的预留间隙)
	GetDesignItemValue('C',&C);	//C:定位基准钢管(球体)句柄(定位参数)
	CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(C,CLS_LINETUBE);
	if(pDatumTube==NULL)
		return FALSE;
	r=pDatumTube->GetDiameter()*0.5+V;
	if(column_norm)
	{
		f3dPoint vColumnNorm=pDatumTube->End()-pDatumTube->Start();
		normalize(vColumnNorm);
		memcpy(column_norm,(double*)vColumnNorm,24);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//CConnectDirection
//根据基准点、Y轴方向以及当前连接方向获取连接方向关联螺栓
int CONNECT_DIRECTION::GetRelaLsRefList(CLsRefList &boltList,IModel *pTower,
										f3dPoint axis_y,f3dPoint origin)
{
	int nBoltNum=0;
	long hPart=0;
	//double ;	//自动搜索螺栓时的范围
	CLsRef *pLsRef=NULL;
	if(direct.norm_style>0&&direct.norm_style<6)
		hPart=direct.hVicePart;
	CLDSLinePart *pLinePart=(CLDSLinePart*)pTower->FromPartHandle(hPart,CLS_LINEPART);
	UCS_STRU ucs;
	ucs.origin=origin;
	ucs.axis_y=axis_y;
	direct.UpdateVector(pTower);
	//
	double x_len=200,y_len=200,z_len=100;
	f3dPoint vertex_arr[4];
	SCOPE_STRU scope;
	scope.ClearScope();
	if(style==0)
	{	//焊缝走向
		ucs.axis_x=direct.vector;
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_y);
		//构件螺栓搜索区域
		vertex_arr[0]=ucs.origin;
		vertex_arr[1]=ucs.origin+ucs.axis_x*x_len;
		vertex_arr[2]=ucs.origin+ucs.axis_y*y_len;
		vertex_arr[3]=ucs.origin+ucs.axis_x*x_len+ucs.axis_y*y_len;
		for(int i=0;i<4;i++)
		{
			scope.VerifyVertex(vertex_arr[i]+ucs.axis_z*z_len*0.5);
			scope.VerifyVertex(vertex_arr[i]-ucs.axis_z*z_len*0.5);
		}
		if(pLinePart&&pLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{	//方向杆件为角钢
			for(pLsRef=pLinePart->GetFirstLsRef();pLsRef;pLsRef=pLinePart->GetNextLsRef())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				if(fabs(pBolt->ucs.axis_z*ucs.axis_z)<EPS_COS2)
					continue;	//非当前面螺栓
				if(!scope.IsIncludePoint(pBolt->ucs.origin))
					continue;
				boltList.append(pBolt->GetLsRef());
				nBoltNum++;
			}
		}
		else if(pLinePart&&pLinePart->GetClassTypeId()==CLS_LINETUBE)
		{	//方向杆件为钢管
			CLDSLineTube *pLineTube=(CLDSLineTube*)pLinePart;
			CLDSParamPlate *pParamPlate=NULL;
			double start_offset=DISTANCE(pLinePart->Start(),origin);
			double end_offset=DISTANCE(pLinePart->End(),origin);
			pParamPlate=(CLDSParamPlate*)pLineTube->GetParamPlate(start_offset>end_offset);
			if(pParamPlate==NULL)
				return 0;
			for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				/*if(fabs(pBolt->ucs.axis_z*ucs.axis_z)<EPS_COS2)
					continue;	//非当前面螺栓
				f3dPoint bolt_pos=pBolt->ucs.origin;
				coord_trans(bolt_pos,ucs,FALSE);
				if(bolt_pos.y<0)
					continue;*/
				boltList.append(pBolt->GetLsRef());
				nBoltNum++;
			}
		}
	}
	else if(style==1)
	{	//焊接面工作法线
		ucs.axis_z=direct.vector;
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		//构件螺栓搜索区域
		vertex_arr[0]=ucs.origin;
		vertex_arr[1]=ucs.origin+ucs.axis_x*x_len;
		vertex_arr[2]=ucs.origin+ucs.axis_y*y_len;
		vertex_arr[3]=ucs.origin+ucs.axis_x*x_len+ucs.axis_y*y_len;
		for(int i=0;i<4;i++)
		{
			scope.VerifyVertex(vertex_arr[i]+ucs.axis_z*z_len*0.5);
			scope.VerifyVertex(vertex_arr[i]-ucs.axis_z*z_len*0.5);
		}
		CLDSBolt *pBolt=NULL;
		BOOL pushed=pTower->PushPartStack(); //遍历构件列表前应先压栈 wht 12-03-05
		for(pBolt=(CLDSBolt*)pTower->EnumPartFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)pTower->EnumPartNext(CLS_BOLT))
		{
			if(!scope.IsIncludePoint(pBolt->ucs.origin))
				continue;
			if(fabs(pBolt->ucs.axis_z*ucs.axis_z)<EPS_COS2)
				continue;	//非当前面螺栓
			boltList.append(pBolt->GetLsRef());
			nBoltNum++;
		}
		pTower->PopPartStack(pushed);
	}
	return nBoltNum;
}

void CLDSParamPlate::SetPartNo(const char *part_no,bool bIncStdPart/*=false*/)
{	//自动编号时可统一设置编号，不会错误修改标准构件编号
	if(!_bSyncShadowPartNo)
	{
		sPartNo.Copy(part_no);
		SetModified(TRUE,FALSE);
		return;
	}
	CLDSPart* pMotherPart=MotherPart();
	if(!m_bStdPart||bIncStdPart)
		sPartNo.Copy(part_no);
	if(pMotherPart!=this)//if(pMotherPart->iSeg==iSeg)
		pMotherPart->SetPartNo(part_no,bIncStdPart);
	SetModified();
}
//输出最小包络矩形范围及面积
f2dRect CLDSParamPlate::GetMinWrapRect(UCS_STRU *Rectucs/*=NULL*/,double offset/*=0*/,BOOL bExportDeformedProfile/*=FALSE*/)
{
	f2dRect rect2d=CLDSGeneralPlate::GetMinWrapRect(Rectucs,offset,bExportDeformedProfile);
	//TODO:根据不同的参数化板进行特例化编码
	if(m_iParamType!=TYPE_FL && m_iParamType!=TYPE_CIRCOVERPLATE)
		return rect2d;
	SCOPE_STRU min_scope;
	double radius=0,angle=0;
	if(m_iParamType==TYPE_FL)
	{	//法兰盘
		double A,W;
		GetDesignItemValue('A',&A);
		GetDesignItemValue('W',&W);
		angle=A;
		radius=W*0.5;
	}
	else if(m_iParamType==TYPE_CIRCOVERPLATE)
	{	//环向封板
		double B,R,W;
		GetDesignItemValue('B',&B);
		GetDesignItemValue('R',&R);
		GetDesignItemValue('W',&W);
		angle=B;
		radius=R+W;
	}
	if(angle<0)
		angle+=360;
	angle*=RADTODEG_COEF;
	if(angle<=Pi/2)
	{
		min_scope.VerifyVertex(f3dPoint(radius,0,0));
		double y=sin(angle)*radius;
		min_scope.VerifyVertex(f3dPoint(radius,y,0));
		min_scope.VerifyVertex(f3dPoint(0,y,0));
		min_scope.VerifyVertex(f3dPoint(0,0,0));
	}
	else if(angle<=Pi)
	{
		min_scope.VerifyVertex(f3dPoint(radius,0,0));
		min_scope.VerifyVertex(f3dPoint(radius,radius,0));
		double x=cos(angle)*radius;
		min_scope.VerifyVertex(f3dPoint(x,radius,0));
		min_scope.VerifyVertex(f3dPoint(x,0,0));
	}
	else if(angle<=1.5*Pi)
	{
		min_scope.VerifyVertex(f3dPoint(radius,radius,0));
		min_scope.VerifyVertex(f3dPoint(-radius,radius,0));
		double y=sin(angle)*radius;
		min_scope.VerifyVertex(f3dPoint(-radius,y,0));
		min_scope.VerifyVertex(f3dPoint(radius,y,0));
	}
	else
	{
		min_scope.VerifyVertex(f3dPoint(radius,radius,0));
		min_scope.VerifyVertex(f3dPoint(-radius,radius,0));
		min_scope.VerifyVertex(f3dPoint(-radius,-radius,0));
		min_scope.VerifyVertex(f3dPoint(radius,-radius,0));
	}
	//
	rect2d.topLeft.Set(min_scope.fMinX,min_scope.fMaxY);
	rect2d.bottomRight.Set(min_scope.fMaxX,min_scope.fMinY);
	return rect2d;
}
bool CLDSParamPlate::ShadowDesign()
{
	if(!IsShadowObject())
		return false;
	CLDSParamPlate *pMotherPlate=(CLDSParamPlate*)MotherObject(false);
	if (shadow.COMMON_OBJ.m_hMotherObj!=pMotherPlate->handle)
		pMotherPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(shadow.COMMON_OBJ.m_hMotherObj);
	thick=(int)pMotherPlate->GetThick();
	cMaterial=pMotherPlate->cMaterial;
	m_iParamType = pMotherPlate->m_iParamType;
	m_bStdPart = pMotherPlate->m_bStdPart;
	strcpy(sNotes,pMotherPlate->sNotes);

	if(m_iShadowObjType==SHADOW_ISOLATED)
	{
		CParaCS *pAcs=m_pModel->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
		if(pAcs&&pAcs->GetDefStyle()==CParaCS::CS_MIRTRANS)
		{
			CParaCS::PARAMETRIC_INFO param = pAcs->GetParamInfo();
			if(param.m_xMirInfo.axis_flag&0x01||param.m_xMirInfo.axis_flag&0x02||param.m_xMirInfo.axis_flag&0x10)
				m_fNormOffset=-thick-pMotherPlate->m_fNormOffset;
		}
		else
			m_fNormOffset=pMotherPlate->m_fNormOffset;
	}
	else if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
		m_fNormOffset=pMotherPlate->m_fNormOffset;
	//外形信息
	vertex_list.Empty();
	for(PROFILE_VERTEX *pVertex=pMotherPlate->vertex_list.GetFirst();pVertex;pVertex=pMotherPlate->vertex_list.GetNext())
		vertex_list.append(*pVertex);
	SetModified();
	return CLDSPart::ShadowDesign();
}
int CLDSParamPlate::GetFaceN()
{ 
	double huoqu_cosa=1;
	if(!keyvecset.huoqufacenorm.IsZero())
		huoqu_cosa=ucs.axis_z*keyvecset.huoqufacenorm;
	if(fabs(huoqu_cosa)<EPS_COS)
		return 2;
	else
		return 1;
}
//获取指火曲线(0.第1、2面的火曲线 1.第2、3面的获取线)，为板坐标系下的相对坐标
f3dLine CLDSParamPlate::GetHuoquLine(int index)
{
	f3dLine huoqu_line;
	if(index==0)
	{
		if(m_iParamType==TYPE_RIBPLATE||m_iParamType==TYPE_CIRRIBPLATE||m_iParamType==TYPE_TUBERIBPLATE)
		{
			f3dPoint face_pick;
			bool planePickFinded=false,bendlineStartFinded=false,bendlineEndFinded=false;
			int nPush=vertex_list.push_stack();
			for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				if(pVertex->vertex.feature==2)
				{
					face_pick=pVertex->vertex;
					planePickFinded=true;
				}
				if(pVertex->vertex.feature==12)
				{
					if(!bendlineStartFinded)
					{
						huoqu_line.startPt=pVertex->vertex;
						bendlineStartFinded=true;
					}
					else
					{
						huoqu_line.endPt=pVertex->vertex;
						bendlineEndFinded=true;
					}
				}
				if(planePickFinded&&bendlineEndFinded)
					break;
			}
			vertex_list.pop_stack(nPush);
			if(!huoqu_line.startPt.IsZero()&&!huoqu_line.endPt.IsZero())
			{
				double justify=DistOf2dPtLine(face_pick,huoqu_line.startPt,huoqu_line.endPt);
				if(justify>0)
				{
					f3dPoint point=huoqu_line.startPt;
					huoqu_line.startPt=huoqu_line.endPt;
					huoqu_line.endPt=point;
				}
			}
		}
		else
		{
			huoqu_line.startPt.Set();
			f3dPoint huoquaxis_vec=keyvecset.huoquaxis_vec;
			vector_trans(huoquaxis_vec,ucs,FALSE);
			huoqu_line.endPt = huoqu_line.startPt+huoquaxis_vec*100;
		}
	}
	return huoqu_line;
}

double CLDSParamPlate::GetHuoQuAngle(int iFace,f3dLine *pHuoQuLine/*=NULL*/)
{
	if(!IsZhiWan())
		return 0;
	f3dLine huoqu_line=GetHuoquLine(0);
	f3dPoint huoquAxisVec=huoqu_line.endPt-huoqu_line.startPt;
	huoquAxisVec.z=0;
	normalize(huoquAxisVec);
	GECS huoqucs;
	huoqucs.axis_x.Set(huoquAxisVec.y,-huoquAxisVec.x);
	huoqucs.axis_z=huoquAxisVec;
	huoqucs.axis_y.Set(0,0,-1);
	GEPOINT huoquFaceNorm=keyvecset.huoqufacenorm;
	vector_trans(huoquFaceNorm,ucs,FALSE);
	normalize(huoquFaceNorm);
	GEPOINT extend_vec=huoqucs.TransVToCS(huoquFaceNorm);
	RotateVectorAroundVector(extend_vec,1,0,GEPOINT(0,0,1));
	double huoqu_angle=Cal2dLineAng(0,0,extend_vec.x,extend_vec.y);
	if(huoqu_angle>Pi)
		huoqu_angle-=2*Pi;
	if(pHuoQuLine)
		*pHuoQuLine=huoqu_line;
	return huoqu_angle;
}
//获取指火曲面的法线，为绝对坐标
f3dPoint CLDSParamPlate::GetHuoquFaceNorm(int faceI)
{
	if(faceI==0)
		return keyvecset.huoqufacenorm;
	else 
		return f3dPoint();
}
//iResidePlane=0,1,2,分别表示基准面、１火曲面、２火曲面
bool CLDSParamPlate::GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* iResidePlane/*=NULL*/,int iIntersPlane/*=0*/)
{
	f3dPoint xPlateOrg=ucs.origin+ucs.axis_z*GetNormOffset();
	int retcode;
	f3dPoint worknorm=ucs.axis_z;
	f3dPoint boltnorm=pBolt->ucs.axis_z;
	if(pBolt->m_dwFlag.IsHasFlag(CLDSBolt::REVERSE_NORM))
		boltnorm*=-1.0;
	int iBoltResidePlane=iBoltResidePlane=GetNormFaceI(pBolt);
	if(iBoltResidePlane==1&&!keyvecset.huoqufacenorm.IsZero())
	{
		retcode=Int3dlf(inters_coords,pBolt->ucs.origin,boltnorm,xPlateOrg,keyvecset.huoqufacenorm);
		worknorm==keyvecset.huoqufacenorm;
		if(iResidePlane)
			*iResidePlane=1;
	}
	else
		retcode=Int3dlf(inters_coords,pBolt->ucs.origin,boltnorm,xPlateOrg,ucs.axis_z);
	f3dPoint inters(inters_coords);
	if(iIntersPlane==TOP_PLANE||iIntersPlane==MID_PLANE)
	{
		double dfOffset=(iIntersPlane==TOP_PLANE)?_thick:_thick*0.5;
		if(iBoltResidePlane==0)
			inters+=ucs.axis_z*dfOffset;
		else
			inters+=GetHuoquFaceNorm(iBoltResidePlane-1)*dfOffset;
	}
	else if(iIntersPlane==CLDSPart::BOLTBTM_PLANE && boltnorm*worknorm<0)
		inters+=worknorm*GetThick();
	memcpy(inters_coords,(double*)inters,24);
	return retcode==1;
}
//cbFlag由FROM_MCS、FROM_ACS、IS_VECTOR与IS_VERTEX进行组合，默认为FROM_MCS|IS_VERTEX wjh-2015.2.27
//iResidePlane取值:1、2、3表示待转换点或矢量所驻存的面号 wjh-2015.2.27
//static const BYTE FROM_MCS =0x00;	//coord3d为模型坐标
//static const BYTE FROM_ACS =0x01;	//coord3d为当前钢板下装配坐标(即相对坐标)
//static const BYTE IS_VERTEX=0x00;	//coord3d为位置点, 执行位置坐标转换
//static const BYTE IS_VECTOR=0x02;	//coord3d为方向量, 执行矢量坐标转换
f3dPoint CLDSParamPlate::SpreadToACS(const double* coord3d,int iResidePlane/*=1*/,BYTE cbFlag/*=0*/)
{
	f3dPoint V(coord3d);
	bool isVector=(cbFlag&IS_VECTOR)>0;
	bool isVertex=!isVector;
	bool fromAcs=(cbFlag&FROM_ACS)>0;
	bool fromMcs=!fromAcs;
	//统一转换到钢板装配坐标系下
	if(fromMcs&&isVertex)
		coord_trans(V,ucs,FALSE);
	else if(fromMcs&&isVector)
		vector_trans(V,ucs,FALSE);
	if(!keyvecset.huoqufacenorm.IsZero())
	{	
		double cosa=keyvecset.huoqufacenorm*ucs.axis_z;
		if(cosa>1.0)
			cosa=1.0;
		if(cosa<-1.0)
			cosa=-1.0;
		if(cosa<EPS_COS)
		{
			double alfa=ACOS(cosa);
			if(isVertex)
				rotate_point_around_axis(V,-alfa,ucs.origin,ucs.origin+keyvecset.huoquaxis_vec);
			else
				RotateVectorAroundVector(V,-alfa,keyvecset.huoquaxis_vec);
		}
	}
	if(isVertex)
		V.feature=iResidePlane;
	return V;
}
BOOL CLDSParamPlate::SpreadLsPos(CLDSBolt *pLs,double* lspos)
{
	if(pLs==NULL)
		return FALSE;

	GEPOINT ls_pos = pLs->ucs.origin;
	ls_pos -= pLs->des_base_pos.norm_offset.Thick(BelongModel())*pLs->get_norm();
	coord_trans(ls_pos,ucs,FALSE);
	double huoqu_cosa=1;
	if(!keyvecset.huoqufacenorm.IsZero())
	{
		huoqu_cosa=ucs.axis_z*keyvecset.huoqufacenorm;
		if(fabs(huoqu_cosa)<EPS_COS)
		{
			if(fabs(keyvecset.huoqufacenorm*pLs->ucs.axis_z)>EPS_COS)
				ls_pos=SpreadTransV(ls_pos,true);	//制弯面上的螺栓
		}
	}
	if(lspos)
	{
		lspos[0]=ls_pos.x;
		lspos[1]=ls_pos.y;
		lspos[2]=ls_pos.z;
	}
	return TRUE;
}
bool CLDSParamPlate::IsNeedConvertToPlate(int doc_type,int nVersion)
{
	if((m_iParamType==TYPE_LINEANGLERIBPLATE||m_iParamType==TYPE_BASEPLATE||m_iParamType==TYPE_SHOEPLATE||m_iParamType==TYPE_RIBPLATE)&&
	   ((doc_type==1&&nVersion>0&&nVersion<4020006)||	//TMA
		(doc_type==2&&nVersion>0&&nVersion<1020006)||	//LMA
		(doc_type==3&&nVersion>0&&nVersion<2000006)||	//TSA
		(doc_type==4&&nVersion>0&&nVersion<1020006)||	//LDS
		(doc_type==5&&nVersion>0&&nVersion<1020006)))	//TDA)
		return true;	//因晓川暂还未写好ConvertToPlate函数，此处暂返回false;
	else
		return false;
}
static CLDSBolt* CreateVirtualBolt(IModel *pModel,CLDSGeneralPlate *pPlate,f3dPoint hole_pos,double hole_d)
{
	int nPush=pModel->PushPartStack();
	CLDSBolt *pBolt=(CLDSBolt*)pModel->AppendPart(CLS_BOLT);
	pBolt->m_bVirtualPart=TRUE;
	pBolt->CopyModuleInstanceInfo(pPlate);
	pBolt->ucs.axis_x.Set();
	pBolt->ucs.axis_y.Set();
	pBolt->ucs.origin=hole_pos;
	pModel->PopPartStack(nPush);
	//模糊确定螺栓直径
	long ls_d=int(hole_d-1.5);
	if(ls_d<12||abs(ls_d-12)<=2)
		ls_d=12;
	else if(abs(ls_d-16)<=2)
		ls_d=16;
	else if(abs(ls_d-20)<=2)
		ls_d=20;
	else if(abs(ls_d-24)<=2)
		ls_d=24;
	else if(abs(ls_d-27)<=2)
		ls_d=27;
	else if(abs(ls_d-30)<=2)
		ls_d=30;
	else 
		ls_d=36;
	pBolt->set_d(ls_d);
	pBolt->hole_d_increment=hole_d-ls_d;
	pPlate->AppendLsRef(pBolt->GetLsRef());
	pBolt->AddL0Thick(pPlate->handle,TRUE,TRUE);
	pBolt->set_norm(pPlate->ucs.axis_z);
	//新生成虚拟孔一律按钢板上相对坐标进行定位
	coord_trans(hole_pos,pPlate->ucs,FALSE,TRUE);
	pBolt->des_base_pos.cLocationStyle=DESIGN_LS_POS::LOCAL_COORD;
	pBolt->des_base_pos.hPart=pPlate->handle;
	pBolt->des_base_pos.len_offset_dist=hole_pos.x;
	pBolt->des_base_pos.wing_offset_dist=hole_pos.y;
	pBolt->des_work_norm.norm_style=4;
	pBolt->des_work_norm.hVicePart=pPlate->handle;
	pBolt->des_work_norm.vector=pBolt->ucs.axis_z;
	pBolt->des_work_norm.nearVector.Set(0,0,1);
	return pBolt;
}
BOOL CLDSParamPlate::ConvertToPlate(CLDSPlate* pPlate)
{
	pPlate->Thick=Thick;
	pPlate->face_N=1;					
	pPlate->cMaterial=cMaterial;
	pPlate->cfgword=cfgword;
	pPlate->SetLayer(layer());
	pPlate->iSeg=iSeg;
	pPlate->m_uStatMatCoef=m_uStatMatCoef;
	pPlate->m_bDisplayHole=m_bDisplayHole;
	pPlate->SetPartNo(GetPartNo());
	strcpy(pPlate->sNotes,sNotes);
	pPlate->m_hPartWeldParent=m_hPartWeldParent;
	if(m_hPartWeldParent>0X20)
		pPlate->m_bNeedFillet=TRUE;	//初始化坡口工艺，仅有焊接父构件的钢板需要坡口工艺 wxc-2017.3.7
	pPlate->ucs=ucs;
	pPlate->m_bWeldPart=TRUE;//焊接件
	pPlate->m_fNormOffset=GetNormOffset();
	//填充顶点信息
	if(vertex_list.GetNodeNum()<3)
		DesignPlate();
	BOOL bIsZhiWan=IsZhiWan();
	BOOL bFindHuoquStart=FALSE,bFindHuoquEnd=FALSE;
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		pVertex=pPlate->vertex_list.append(*pVertex);
		if(!bIsZhiWan)
			pVertex->vertex.feature=1;	//第一面轮廓点,此处必须赋值否则转换为普通钢板后无法正常显示 wht 11-06-03
		if(bIsZhiWan&&pVertex->vertex.feature>10)
		{	//查找制弯钢板获取线 wht 11-05-30
			if(!bFindHuoquEnd)
			{
				pPlate->HuoQuLine[0].endPt=pVertex->vertex;
				bFindHuoquEnd=TRUE;
			}
			else if(!bFindHuoquStart)
			{
				pPlate->HuoQuLine[0].startPt=pVertex->vertex;
				bFindHuoquStart=TRUE;
			}
		}
	}
	if(IsZhiWan())
	{	//制弯的参数化板转换为普通板
		pPlate->face_N=2;
		pPlate->huoqufacenorm[0]=HuoquFaceNorm();
		pPlate->designInfo.huoqufacenorm[0].norm_style=0;
		pPlate->designInfo.huoqufacenorm[0].vector=HuoquFaceNorm();
		pPlate->designInfo.m_bEnableFlexibleDesign=false;
	}
	//填充设计信息
	pPlate->designInfo.m_bEnableFlexibleDesign=FALSE;	//启动柔性设计
	//pPlate->designInfo.iProfileStyle0123=3;			//不进行外形设计
	pPlate->designInfo.m_hBaseNode=m_hBaseNode;
	pPlate->designInfo.origin.datum_pos_style=0;
	pPlate->designInfo.origin.SetPosition(pPlate->ucs.origin);
	if(m_iParamType==TYPE_CIRRIBPLATE)
	{	//环向肋板
		long h=0;
		double S=0;
		GetDesignItemValue('S',&S);	//环向板到定位点的偏移距离
		GetDesignItemValue('P',&h);	//定位基准节点 wht 11-05-12
		CLDSNode *pBaseNode=BelongModel()->FromNodeHandle(h);
		CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
		if(pBaseNode==NULL||pDatumTube==NULL)	//发生异常时应清理冗余数据
			return FALSE;
		pPlate->designInfo.m_hBasePart=pDatumTube->handle;
		pPlate->designInfo.origin.datum_pos_style=2;
		pPlate->designInfo.origin.des_para.RODNODE.hRod=pDatumTube->handle;
		pPlate->designInfo.origin.des_para.RODNODE.hNode=pBaseNode->handle;
		f3dPoint len_vec=pDatumTube->End()-pDatumTube->Start();
		if(pPlate->ucs.axis_z*len_vec<0)
			pPlate->designInfo.origin.des_para.RODNODE.direction=1;
		else
			pPlate->designInfo.origin.des_para.RODNODE.direction=0;
		pPlate->designInfo.origin.des_para.RODNODE.len_offset_dist=S;
		//环向肋板转换为普通钢板后需调整法向偏移量
		pPlate->m_fNormOffset=m_fNormOffset-0.5*GetThick();
		//
		double inner_r=0;	//内圆半径
		f3dPoint col_axis;	//圆柱法线方向
		if(GetInnerCirRadius(inner_r,col_axis))
		{	//将环向板(异形法兰)内圆转换为普通钢板的内圆 wht 11-05-27
			pPlate->m_fInnerRadius=inner_r;
			pPlate->m_tInnerColumnNorm=col_axis;
		}
	}
	else if(m_iParamType==TYPE_TUBERIBPLATE)
	{
		pPlate->m_fNormOffset=-0.5*pPlate->GetThick();
	}
	else if(m_iParamType==TYPE_FL)
	{
		double A=0,W=0,R=0;
		pPlate->m_iLsArrayStyle=1;	//环向布置螺栓
		GetDesignItemValue('W',&W);
		GetDesignItemValue('R',&R);
		GetDesignItemValue('A',&A);
		if(A!=360&&A!=0)	//暂时只处理整圆的情况
		{	//发生异常时应清理冗余数据
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("0x%X flange's design parameter A is %s°,now only consider whole circular flange ",handle,(char*)CXhChar16(A));
#else
			logerr.Log("0x%X法兰的设计参数A值为%s°,暂时只处理整圆法兰的情况 ",handle,(char*)CXhChar16(A));
#endif
			return FALSE;
		}
		pPlate->m_fInnerRadius=R;
		pPlate->m_tInnerColumnNorm=pPlate->ucs.axis_z;
	}
	else if(m_iParamType==TYPE_ELBOWPLATE)
	{
		pPlate->designInfo.origin.datum_pos_style=0;
		pPlate->designInfo.origin.SetPosition(pPlate->ucs.origin);
		//肋板转换为普通钢板后需调整法向偏移量
		pPlate->m_fNormOffset=m_fNormOffset-0.5*GetThick();
	}
	else if(m_iParamType==TYPE_BASEPLATE||m_iParamType==TYPE_SHOEPLATE)
	{
		pPlate->designInfo.origin=design.origin;
		CLDSNode* pBaseNode=GetBaseNode();
		if(pBaseNode)
			pPlate->designInfo.m_hBaseNode=pBaseNode->handle;
		long hBaseAngle=0;
		if(m_iParamType==TYPE_BASEPLATE)
			GetDesignItemValue(KEY2C('L'),&hBaseAngle);
		else if(m_iParamType==TYPE_SHOEPLATE)
			GetDesignItemValue('D',&hBaseAngle);
		pPlate->designInfo.m_hBasePart=hBaseAngle;
		pPlate->designInfo.m_bEnableFlexibleDesign=FALSE;
	}
	else if(m_iParamType==TYPE_LINEANGLERIBPLATE)
	{	
		pPlate->m_fNormOffset=-0.5*pPlate->GetThick();
		//定义中心孔
		double d=0,P=0;	
		if (GetDesignItemValue('d',&d)&&GetDesignItemValue('P',&P)&&P>0&&d>0)
		{
			long A;
			GetDesignItemValue('A',&A);
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(A,CLS_LINEANGLE);
			if(pLineAngle)
				pPlate->designInfo.m_hBasePart=pLineAngle->handle;
		}
	}
	else if(m_iParamType==TYPE_RIBPLATE)
	{
		pPlate->m_fNormOffset=-0.5*pPlate->GetThick();
		long A=0;
		GetDesignItemValue(KEY2C("A"),&A);
		CLDSPart *pPart=BelongModel()->FromPartHandle(A);
		if(pPart)
			pPlate->designInfo.m_hBasePart=pPart->handle;
	}
	else if(m_iParamType==TYPE_CIRCOVERPLATE)
		pPlate->m_fNormOffset=GetNormOffset();//-0.5*pParamPlate->GetThick();
	//螺栓引用
	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		pPlate->AppendLsRef(*pLsRef);
	//添加虚拟孔
	CXhSimpleList<HOLE> listWaterleakHoles;
	AddMiscellaneousHoles(&listWaterleakHoles);
	for (HOLE* pWaterHole = listWaterleakHoles.EnumObjectFirst(); pWaterHole; pWaterHole = listWaterleakHoles.EnumObjectNext())
	{
		f3dPoint hole_pos= pWaterHole->xMcsPosition;
		if (hole_pos.IsZero())
		{
			hole_pos = pWaterHole->xLocalPosition;
			coord_trans(hole_pos, ucs, TRUE);
		}
		CreateVirtualBolt(BelongModel(), pPlate, hole_pos, pWaterHole->fHoleD);
	}
	return TRUE;
}
bool CLDSParamPlate::SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL)
		return false;
	return CLDSGeneralPlate::SyncMirPropTo(pDestObj,propKeyStr,dwPropFilterFlag);
}
bool CLDSParamPlate::SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL)
		return false;
	CLDSGeneralPlate::SyncMirPropTo(pDestObj,mirmsg,propKeyStr,dwPropFilterFlag);
	CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pDestObj;
	if(propKeyStr==NULL||strcmp(propKeyStr,"params")==0)
	{
		return ((CLDSParamPlate*)pDestObj)->MirDesignItemsFrom(this,mirmsg);
	}
	/*if((propKeyStr==NULL||strcmp(propKeyStr,"layer")==0)&&IsSyncMirPropWith(pDestObj,"layer"))
	{	//图层名同步对称
		if(hMirAcs<=3)
			MirObjLayer(layer(),pPlate->layer(),mirmsg);
	}*/
	return false;
}
//返回弧度值角度
double CalPrjPlateRotAngle(CLDSGeneralPlate* pBtmPlate,CLDSLineTube* pRotTube,int fromEdgeDatum0Weld1,const double* axis_vec)
{
	f3dPoint vStartEdgeDirect=pRotTube->ucs.axis_x;
	if(fromEdgeDatum0Weld1==1&&pRotTube->m_bHasWeldRoad)
	{
		double weldRoadAngle=pRotTube->CalWeldRoadAngle();
		RotateVectorAroundVector(vStartEdgeDirect,weldRoadAngle,pRotTube->ucs.axis_z);
	}
	//计算vStartEdgeDirect沿钢管轴向向底板的投影方向
	GEPOINT vPrjPlaneNormal=pRotTube->ucs.axis_z^vStartEdgeDirect;
	GEPOINT vPrjStartEdgeDirect=vPrjPlaneNormal^pBtmPlate->ucs.axis_z;
	if(vPrjStartEdgeDirect*vStartEdgeDirect<0)
		vPrjStartEdgeDirect*=-1.0;
	normalize(vPrjStartEdgeDirect);
	//计算对称肘板在对称基准钢管上的定位角度
	GEPOINT vAxisVec(axis_vec);	//待计算旋转角度的射线方向
	return  CalRotateAngle(pBtmPlate->ucs.axis_z,vPrjStartEdgeDirect,vAxisVec);
}
bool CLDSParamPlate::MirDesignItemsFrom(CLDSParamPlate* pSrcParamPlate,MIRMSG mirmsg)
{
	if(m_iParamType!=pSrcParamPlate->m_iParamType)
		return false;
	if(pSrcParamPlate->m_iParamType==TYPE_ELBOWPLATE)
	{
		long K,L;
		if(pSrcParamPlate->GetDesignItemValue('K',&K))
		{
			CLDSLineTube *pDatumTube=NULL,*pMirDatumTube=NULL;
			CSuperSmartPtr<CLDSPart> pBottomPlate=NULL,pMirBottomPlate;
			pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(abs(K),CLS_LINETUBE);
			if(pSrcParamPlate->GetDesignItemValue('L',&L))//获得基准底板
				pBottomPlate=BelongModel()->FromPartHandle(L,CLS_PLATE,CLS_PARAMPLATE);
			if(pDatumTube&&pBottomPlate.IsHasPtr())
			{
				pMirDatumTube = (CLDSLineTube*)pDatumTube->GetMirPart(mirmsg);
				pMirBottomPlate = pBottomPlate->GetMirPart(mirmsg);
				if(pMirDatumTube&&pMirBottomPlate.IsHasPtr())
				{
					if(pMirBottomPlate->ucs.axis_z*pMirDatumTube->ucs.axis_z>0)
#ifdef AFX_TARG_ENU_ENGLISH
						SetDesignItemValue('K',-pMirDatumTube->handle,"datum tube that rib depend on",PARAM_POS,1);
					else
						SetDesignItemValue('K',pMirDatumTube->handle,"datum tube that rib depend on",PARAM_POS,1);
					SetDesignItemValue('L',pMirBottomPlate->handle,"datum bottom plate",PARAM_POS,1);
#else
						SetDesignItemValue('K',-pMirDatumTube->handle,"肘板所依靠的基准钢管",PARAM_POS,1);
					else
						SetDesignItemValue('K',pMirDatumTube->handle,"肘板所依靠的基准钢管",PARAM_POS,1);
					SetDesignItemValue('L',pMirBottomPlate->handle,"基准底板",PARAM_POS,1);
#endif
					long J=0;
					if(pMirBottomPlate->GetClassTypeId()==CLS_PLATE)
						J=pMirBottomPlate.PlatePointer()->m_bAttachPartByWeldRoad=pBottomPlate.PlatePointer()->m_bAttachPartByWeldRoad;
					else if(pMirBottomPlate->GetClassTypeId()==CLS_PARAMPLATE)
					{
						if(pBottomPlate.ParamPlatePointer()->GetDesignItemValue('J',&J))
							pMirBottomPlate.ParamPlatePointer()->SetDesignItemValue('J',J,NULL,PARAM_POS,1);
						else
							J=pMirDatumTube->feature=pBottomPlate->feature;
					}
					f3dPoint axis_mir_x=GetMirVector(pSrcParamPlate->ucs.axis_x,mirmsg);	//对称肘板的X轴方向
					//计算对称肘板在对称基准钢管上的定位角度
					double angle=CalPrjPlateRotAngle(pMirBottomPlate.pBasicPlate,pMirDatumTube,J,axis_mir_x);
					angle*=DEGTORAD_COEF;
					angle=ftoi(angle*100)/100;
#ifdef AFX_TARG_ENU_ENGLISH
					SetDesignItemValue('Y',angle,"rib plate positioning direction angle",PARAM_POS);
#else
					SetDesignItemValue('Y',angle,"肘板定位方向角",PARAM_POS);
#endif
				}
				else
				{
					if(pMirDatumTube==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
						logerr.Log("0X%X tube's symmetrical tube not found!",abs(K));
					if(!pMirBottomPlate.IsHasPtr())
						logerr.Log("0X%X tube's symmetrical tube not found!",L);
#else
						logerr.Log("0X%X钢管的对称钢管未找到!",abs(K));
					if(!pMirBottomPlate.IsHasPtr())
						logerr.Log("0X%X钢板的对称钢板未找到!",L);
#endif
					return false;
					//TODO:需要处理后续板删除工作 wjh-2016.4.13
					//console.DeletePart(pMirPlate->handle);
					//return NULL;
				}
				//将肘板存放在法兰盘的依附构件链表里	
				if(pMirBottomPlate->GetClassTypeId()==CLS_PARAMPLATE)
					pMirBottomPlate.ParamPlatePointer()->AppendAttachPart(this);
			}
		}
		else
		{
			f3dPoint columnnorm;
			pSrcParamPlate->GetDesignItemValue('x', &columnnorm.x);
			pSrcParamPlate->GetDesignItemValue('y', &columnnorm.y);
			pSrcParamPlate->GetDesignItemValue('z', &columnnorm.z);
			columnnorm=GetMirVector(columnnorm,mirmsg);
#ifdef AFX_TARG_ENU_ENGLISH
			SetDesignItemValue('x',columnnorm.x,"X component in tube direction");
			SetDesignItemValue('y',columnnorm.y,"Y component in tube direction");
			SetDesignItemValue('z',columnnorm.z,"Z component in tube direction");
#else
			SetDesignItemValue('x',columnnorm.x,"钢管方向X分量");
			SetDesignItemValue('y',columnnorm.y,"钢管方向Y分量");
			SetDesignItemValue('z',columnnorm.z,"钢管方向Z分量");
#endif
		}
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{
		long C,E,G,F,c,e,g,f;
		pSrcParamPlate->GetDesignItemValue('E',&E);
		pSrcParamPlate->GetDesignItemValue('F',&F);
		pSrcParamPlate->GetDesignItemValue('G',&G);
		pSrcParamPlate->GetDesignItemValue('C',&C);
		MirPartHandle(pSrcParamPlate->BelongModel(),&e,E,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&f,F,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&g,G,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&c,C,mirmsg);
		if(c==0||e==0||f==0)//||g==0
		{
			//console.DispPartSet.DeleteNode(handle);
			//console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("0X%X fail to meet symmetric requirements, can not finish symmtry!");
			return false;
		}
		SetDesignItemValue('C',c,"positioning datum tube",PARAM_POS,1);
		SetDesignItemValue('E',e,"positioning datum node",PARAM_POS,1);
		SetDesignItemValue('F',f,"the first datum plate",PARAM_POS,1);
		SetDesignItemValue('G',g,"the second datum plate",PARAM_POS,1);
#else
			Log2File()->Log("0X%X钢板不满足对称条件，对称失败!",pSrcParamPlate->handle);
			return false;
		}
		SetDesignItemValue('C',c,"定位基准钢管",PARAM_POS,1);
		SetDesignItemValue('E',e,"定位基准节点",PARAM_POS,1);
		SetDesignItemValue('F',f,"第一块基准钢板",PARAM_POS,1);
		SetDesignItemValue('G',g,"第二块基准钢板",PARAM_POS,1);
#endif
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
	{
		long E=0,F=0,P=0,C=0,e=0,f=0,p=0,c=0;
		pSrcParamPlate->GetDesignItemValue('P',&P);	//'A'改为'P' wht 11-05-17
		pSrcParamPlate->GetDesignItemValue('E',&E);	//'B'改为'E'
		pSrcParamPlate->GetDesignItemValue('F',&F);	//'C'改为'F'
		pSrcParamPlate->GetDesignItemValue('C',&C);	//基准钢管
		MirPartHandle(pSrcParamPlate->BelongModel(),&p,P,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&e,E,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&f,F,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&c,C,mirmsg);
		if(c==0||p==0||(E>0x20&&e==0))//||(F>0x20&&f==0)
		{
			//console.DispPartSet.DeleteNode(handle);
			//console.DeletePart(handle);
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("0X%X fail to meet symmetric requirements, can not finish symmtry!");
			return false;
		}
		SetDesignItemValue('C',c,"positioning datum tube",PARAM_POS,1);
		SetDesignItemValue('P',p,"positioning datum node",PARAM_POS,1);
		SetDesignItemValue('E',e,"the first datum plate",PARAM_POS,1);
		SetDesignItemValue('F',f,"the second datum plate",PARAM_POS,1);
#else
			Log2File()->Log("0X%X钢板不满足对称条件，对称失败!",pSrcParamPlate->handle);
			return false;
		}
		SetDesignItemValue('C',c,"定位基准钢管",PARAM_POS,1);
		SetDesignItemValue('P',p,"定位基准节点",PARAM_POS,1);
		SetDesignItemValue('E',e,"第一块基准钢板",PARAM_POS,1);
		SetDesignItemValue('F',f,"第二块基准钢板",PARAM_POS,1);
#endif
		//肋板连接方向(环向肋板，普通环向板和异形法兰) wht 11-05-17
		ConnDirectList.Empty();
		CONNECT_DIRECTION *pVec=NULL;
		for(pVec=pSrcParamPlate->ConnDirectList.GetFirst();pVec;pVec=pSrcParamPlate->ConnDirectList.GetNext())
		{
			CONNECT_DIRECTION *pMirVec=ConnDirectList.SetValue(pVec->serial,*pVec);
			pVec->direct.MirToVector(BelongModel(),pMirVec->direct,mirmsg);
			//MirDatumVector(&Ta,pVec->direct,pMirVec->direct,msg);
		}
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
	{
		long C=0,P=0,E=0,F=0,c=0,p=0,e=0,f=0;
		pSrcParamPlate->GetDesignItemValue('C',&C);
		pSrcParamPlate->GetDesignItemValue('P',&P);
		pSrcParamPlate->GetDesignItemValue('E',&E);
		pSrcParamPlate->GetDesignItemValue('F',&F);
		MirPartHandle(pSrcParamPlate->BelongModel(),&c,C,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&p,P,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&e,E,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&f,F,mirmsg);
		if(c==0||p==0||e==0)
		{
			//console.DispPartSet.DeleteNode(pMirPlate->handle);
			//console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("0X%X fail to meet symmetric requirements, can not finish symmtry!");
			return false;
		}
		SetDesignItemValue('C',c,"positioning datum tube",PARAM_POS,1);
		SetDesignItemValue('P',p,"positioning datum node",PARAM_POS,1);
		SetDesignItemValue('E',e,"the first datum plate",PARAM_POS,1);
		SetDesignItemValue('F',f,"the second datum plate",PARAM_POS,1);
#else
			Log2File()->Log("0X%X钢板不满足对称条件，对称失败!",pSrcParamPlate->handle);
			return false;
		}
		SetDesignItemValue('C',c,"定位基准钢管",PARAM_POS,1);
		SetDesignItemValue('P',p,"定位基准节点",PARAM_POS,1);
		SetDesignItemValue('E',e,"第一块基准钢板",PARAM_POS,1);
		SetDesignItemValue('F',f,"第二块基准钢板",PARAM_POS,1);
#endif
		//不知道王海涛当初是何意思了，现在感觉不需要调整W,N,及Y参数，否则十字插板中跨越连板Z轴对称错误 wjh 2011-11-10
		/*CLDSPlate *pDatumPlate=(CLDSPlate*)Ta.Parts.FromHandle(E,CLS_PLATE);
		CLDSPlate *pMirDatumPlate=(CLDSPlate*)Ta.Parts.FromHandle(e,CLS_PLATE);
		if(pDatumPlate&&pMirDatumPlate)
		{
			if(pDatumPlate->ucs.axis_z*pMirDatumPlate->ucs.axis_z<0&&(msg.axis_flag==0x01||msg.axis_flag==0x02))
			{	//X/Y轴对称时，Z轴方向相反时W，N值需要互换
				double W=0,N=0;
				pSrcParamPlate->GetDesignItemValue('W',&W);
				pSrcParamPlate->GetDesignItemValue('N',&N);
				pMirPlate->SetDesignItemValue('W',N,"确定板外形的参数W",PARAM_SHAPE);
				pMirPlate->SetDesignItemValue('N',W,"确定板外形的参数N",PARAM_SHAPE);
				//调整内外侧 wht 11-05-17 
				double Y=0;
				pMirPlate->GetDesignItemValue('Y',&Y);
				if(Y>=0)
					Y=-1;
				else
					Y=1;
				pMirPlate->SetDesignItemValue('Y',Y,"半圆型环向板的位置(>0基准板外侧;=0自动;<0基准板内侧)",PARAM_SHAPE);
			}
		}*/
		//跨越连板的连接方向 wht 11-05-17
		ConnDirectList.Empty();
		CONNECT_DIRECTION *pVec=NULL;
		for(pVec=pSrcParamPlate->ConnDirectList.GetFirst();pVec;pVec=pSrcParamPlate->ConnDirectList.GetNext())
		{
			CONNECT_DIRECTION *pMirVec=ConnDirectList.SetValue(pVec->serial,*pVec);
			pVec->direct.MirToVector(BelongModel(),pMirVec->direct,mirmsg);
			//MirDatumVector(&Ta,pVec->direct,pMirVec->direct,msg);
		}
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
	{	//角钢肋板
		long A,B,a,b;
		pSrcParamPlate->GetDesignItemValue('A',&A);
		pSrcParamPlate->GetDesignItemValue('B',&B);
		MirPartHandle(pSrcParamPlate->BelongModel(),&a,A,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&b,B,mirmsg);
		if(a==0||b==0)
		{
			//console.DispPartSet.DeleteNode(pMirPlate->handle);
			//console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("0X%X fail to meet symmetric requirements, can not finish symmtry!");
			return false;
		}
		SetDesignItemValue('A',a,"datum angle",PARAM_POS,1);
		SetDesignItemValue('B',b,"datum node",PARAM_POS,1);
#else
			Log2File()->Log("0X%X钢板不满足对称条件，对称失败!",pSrcParamPlate->handle);
			return false;
		}
		SetDesignItemValue('A',a,"基准角钢",PARAM_POS,1);
		SetDesignItemValue('B',b,"基准节点",PARAM_POS,1);
#endif
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
	{	//遮水板
		long C,B,c,b;
		pSrcParamPlate->GetDesignItemValue('C',&C);
		pSrcParamPlate->GetDesignItemValue('B',&B);
		MirPartHandle(pSrcParamPlate->BelongModel(),&c,C,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&b,B,mirmsg);
		if(c==0||b==0)
		{
			//console.DispPartSet.DeleteNode(pMirPlate->handle);
			//console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("0X%X fail to meet symmetric requirements, can not finish symmtry!");
			return false;
		}
		SetDesignItemValue('C',c,"datum tube",PARAM_POS,1);
		SetDesignItemValue('B',b,"datum node",PARAM_POS,1);
#else
			Log2File()->Log("0X%X钢板不满足对称条件，对称失败!",pSrcParamPlate->handle);
			return false;
		}
		SetDesignItemValue('C',c,"基准钢管",PARAM_POS,1);
		SetDesignItemValue('B',b,"基准节点",PARAM_POS,1);
#endif
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_RIBPLATE)
	{	//肋板
		long hDatumPart=0,hNode=0,hRelaPart1=0,hRelaPart2=0,hBolt=0,hVecPart=0,hTipBolt=0,hBendVecPart=0;
		long A=0,B=0,C=0,D=0,O=0,V=0,TB=0,BV=0;
		DESIGN_PARAM Ox,Oy,Xx,Xy,Ex,Ey,Tx,Ty,Bx,By;
		memset(&Xy,0,sizeof(DESIGN_PARAM));
		Ox=Oy=Xx=Ex=Ey=Tx=Ty=Bx=By=Xy;
		pSrcParamPlate->GetDesignItemValue('A',&hDatumPart);
		pSrcParamPlate->GetDesignItemValue('B',&hNode);
		pSrcParamPlate->GetDesignItemValue('C',&hRelaPart1);
		pSrcParamPlate->GetDesignItemValue('D',&hRelaPart2);
		pSrcParamPlate->GetDesignItemValue('V',&hVecPart);
		pSrcParamPlate->GetDesignItemValue('O',&hBolt);
		pSrcParamPlate->GetDesignItemValue(KEY2C("TB"),&hTipBolt);
		pSrcParamPlate->GetDesignItemValue(KEY2C("BV"),&hBendVecPart);
		pSrcParamPlate->GetDesignItemValue(KEY2C("Ox"),&Ox);
		pSrcParamPlate->GetDesignItemValue(KEY2C("Oy"),&Oy);
		pSrcParamPlate->GetDesignItemValue(KEY2C("Xx"),&Xx);
		pSrcParamPlate->GetDesignItemValue(KEY2C("Xy"),&Xy);
		pSrcParamPlate->GetDesignItemValue(KEY2C("Ex"),&Ex);
		pSrcParamPlate->GetDesignItemValue(KEY2C("Ey"),&Ey);
		pSrcParamPlate->GetDesignItemValue(KEY2C("Bx"),&Bx);
		pSrcParamPlate->GetDesignItemValue(KEY2C("By"),&By);
		pSrcParamPlate->GetDesignItemValue(KEY2C("Tx"),&Tx);	//制弯面末端T点的用户输入定义值
		pSrcParamPlate->GetDesignItemValue(KEY2C("Ty"),&Ty);	//制弯面末端T点的用户输入定义值
		MirPartHandle(pSrcParamPlate->BelongModel(),&A,abs(hDatumPart),mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&B,hNode,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&C,abs(hRelaPart1),mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&D,abs(hRelaPart2),mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&O,abs(hBolt),mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&V,abs(hVecPart),mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&TB,abs(hTipBolt),mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&BV,abs(hBendVecPart),mirmsg);
		CSuperSmartPtr<CLDSPart> pDatumPart=BelongModel()->FromPartHandle(abs(hDatumPart));
		CSuperSmartPtr<CLDSPart> pRelaPart1=BelongModel()->FromPartHandle(abs(hRelaPart1));
		CSuperSmartPtr<CLDSPart> pRelaPart2=BelongModel()->FromPartHandle(abs(hRelaPart2));
		CSuperSmartPtr<CLDSPart> pMirDatumPart=BelongModel()->FromPartHandle(abs(A));
		CSuperSmartPtr<CLDSPart> pMirRelaPart1=BelongModel()->FromPartHandle(abs(hRelaPart1));
		CSuperSmartPtr<CLDSPart> pMirRelaPart2=BelongModel()->FromPartHandle(abs(hRelaPart2));
		GEPOINT Op(Ox.ToDouble(),Oy.ToDouble()),Xv(Xx.ToDouble(),Xy.ToDouble());
		GEPOINT ptE(Ex.ToDouble(),Ey.ToDouble()),ptB(Bx.ToDouble(),By.ToDouble()),ptT(Tx.ToDouble(),Ty.ToDouble());
		if((mirmsg.axis_flag==0x01||mirmsg.axis_flag==0x02))
		{
			if(pDatumPart.IsHasPtr()&&pDatumPart->IsPlate()&&pRelaPart1.IsNULL()&&pMirDatumPart.IsHasPtr())
			{	//底板为普通钢板的普通肋板
				CSuperSmartPtr<CLDSPart> pVecPart=BelongModel()->FromPartHandle(abs(hVecPart));
				CSuperSmartPtr<CLDSPart> pMirVecPart=BelongModel()->FromPartHandle(V);
				if(pVecPart.IsHasPtr()&&pMirVecPart.IsHasPtr()&&pVecPart->IsLinePart())
				{
					GEPOINT vec,mirvec,destmirvec;
					vec=pVecPart.LinePartPointer()->End()-pVecPart.LinePartPointer()->Start();
					normalize(vec);
					if(hVecPart<0)
						vec*=-1;
					mirvec=pMirVecPart.LinePartPointer()->End()-pMirVecPart.LinePartPointer()->Start();//pMirRod->End()-pMirRod->Start();
					normalize(mirvec);
					destmirvec=GetMirVector(vec,mirmsg);
					if(mirvec*destmirvec<-EPS_COS)
						V*=-1;
				}
				else if(pVecPart.IsHasPtr()&&pMirVecPart.IsHasPtr()&&pVecPart->GetClassTypeId()==CLS_BOLT)
				{	//根据螺栓确定钢板肋板的X轴走向,判断对称肋板是否反转X轴 
					GEPOINT vec,norm,mirnorm,destmirnorm;
					vec=pVecPart->ucs.origin-pDatumPart->ucs.origin;
					normalize(vec);
					norm=vec^pDatumPart->ucs.axis_z;
					vec=pMirVecPart->ucs.origin-pMirDatumPart->ucs.origin;
					normalize(vec);
					mirnorm=vec^pMirDatumPart->ucs.axis_z;
					destmirnorm=GetMirVector(norm,mirmsg);
					double fCos=mirnorm*destmirnorm;
					if(mirnorm*destmirnorm<0)	//反向
						m_cReverseAxisVec='X';
				}
			//}
			//else if(pDatumPart.IsHasPtr()&&pDatumPart->IsPlate()&&pRelaPart1.IsHasPtr()&&pRelaPart1->IsPlate()&&
			//	pMirDatumPart.IsHasPtr()&&pMirRelaPart1.IsHasPtr())	//角肋板
			//{
				coord_trans(Op,pDatumPart->ucs,TRUE,TRUE);
				vector_trans(Xv,pDatumPart->ucs,TRUE,TRUE);
				coord_trans(ptB,pDatumPart->ucs,TRUE,TRUE);
				coord_trans(ptE,pDatumPart->ucs,TRUE,TRUE);
				coord_trans(ptT,pDatumPart->ucs,TRUE,TRUE);
				Op=GetMirPos(Op,mirmsg);
				Xv=GetMirVector(Xv,mirmsg);
				ptB=GetMirPos(ptB,mirmsg);
				ptE=GetMirPos(ptE,mirmsg);
				ptT=GetMirPos(ptT,mirmsg);
				coord_trans(Op,pMirDatumPart->ucs,FALSE,TRUE);
				vector_trans(Xv,pMirDatumPart->ucs,FALSE,TRUE);
				coord_trans(ptB,pMirDatumPart->ucs,FALSE,TRUE);
				coord_trans(ptE,pMirDatumPart->ucs,FALSE,TRUE);
				coord_trans(ptT,pMirDatumPart->ucs,FALSE,TRUE);
			}
		}
		if(Ox.bExist)
			SetDesignItemValue(KEY2C("Ox"),Op.x,"肋板坐标系原点X坐标分量(基准钢板下的相对坐标",PARAM_POS,0);
		if(Oy.bExist)
			SetDesignItemValue(KEY2C("Oy"),Op.y,"肋板坐标系原点Y坐标分量(基准钢板下的相对坐标)",PARAM_POS,0);
		if(Xx.bExist)
			SetDesignItemValue(KEY2C("Xx"),Xv.x,"肋板坐标系X轴X坐标分量(基准钢板下的相对坐标)",PARAM_POS,0);
		if(Xy.bExist)
			SetDesignItemValue(KEY2C("Xy"),Xv.y,"肋板坐标系X轴Y坐标分量(基准钢板下的相对坐标)",PARAM_POS,0);
		if(Bx.bExist)
			SetDesignItemValue(KEY2C("Bx"),ptB.x,"",PARAM_POS,0);
		if(By.bExist)
			SetDesignItemValue(KEY2C("By"),ptB.y,"",PARAM_POS,0);
		if(Ex.bExist)
			SetDesignItemValue(KEY2C("Ex"),ptE.x,"",PARAM_POS,0);
		if(Ey.bExist)
			SetDesignItemValue(KEY2C("Ey"),ptE.y,"",PARAM_POS,0);
		if(Tx.bExist)
			SetDesignItemValue(KEY2C("Tx"),ptT.x,"",PARAM_POS,0);
		if(Ty.bExist)
			SetDesignItemValue(KEY2C("Ty"),ptT.y,"",PARAM_POS,0);
		if((A==0&&abs(hDatumPart)>0x20)||(B==0&&hNode>0x20)
			||(C==0&&abs(hRelaPart1)>0x20)||(D==0&&abs(hRelaPart2)>0x20)
			||(O==0&&abs(hBolt)>0x20)||(V==0&&abs(hVecPart)>0x20)
			||(TB==0&&abs(hTipBolt)>0x20)||(BV==0&&abs(hBendVecPart)>0x20))
		{
			//console.DispPartSet.DeleteNode(pMirPlate->handle);
			//console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("0X%X fail to meet symmetric requirements, can not finish symmtry!");
#else
			Log2File()->Log("0X%X钢板不满足对称条件，对称失败!",pSrcParamPlate->handle);
#endif
			return false;
		}
		if(A>0x20)
#ifdef AFX_TARG_ENU_ENGLISH
			SetDesignItemValue('A',A*(hDatumPart/abs(hDatumPart)),"datum part",PARAM_POS,1);
		if(B>0x20)
			SetDesignItemValue('B',B,"datum node",PARAM_POS,1);
		if(C>0x20)
			SetDesignItemValue('C',C*(hRelaPart1/abs(hRelaPart1)),"rib plate positioning auxiliary part 1",PARAM_POS,1);
		if(D>0x20)
			SetDesignItemValue('D',D*(hRelaPart2/abs(hRelaPart2)),"rib plate positioning auxiliary part 2",PARAM_POS,1);
		if(V>0x20)
			SetDesignItemValue('V',V*(hVecPart/abs(hVecPart)),"rib plate X axis direction positioning part (rod or bolt)",PARAM_POS,1);
		if(O>0x20)
			SetDesignItemValue('O',O*(hBolt/abs(hBolt)),"rib plate positioning datum bolt",PARAM_POS,1);
#else
			SetDesignItemValue('A',A*(hDatumPart/abs(hDatumPart)),"基准构件",PARAM_POS,1);
		if(B>0x20)
			SetDesignItemValue('B',B,"基准节点",PARAM_POS,1);
		if(C>0x20)
			SetDesignItemValue('C',C*(hRelaPart1/abs(hRelaPart1)),"肋板定位辅助构件1",PARAM_POS,1);
		if(D>0x20)
			SetDesignItemValue('D',D*(hRelaPart2/abs(hRelaPart2)),"肋板定位辅助构件2",PARAM_POS,1);
		if(abs(V)>0x20)
			SetDesignItemValue('V',V,"肋板X轴方向定位构件(杆件或螺栓)",PARAM_POS,1);
		if(O>0x20)
			SetDesignItemValue('O',O*(hBolt/abs(hBolt)),"肋板定位基准螺栓",PARAM_POS,1);
#endif
		if(TB>0x20)
			SetDesignItemValue(KEY2C("TB"),TB*(hTipBolt/abs(hTipBolt)),"",PARAM_POS,1);
		if(BV>0x20)
			SetDesignItemValue(KEY2C("BV"),BV*(hBendVecPart/abs(hBendVecPart)),"",PARAM_POS,1);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsPlate()&&pRelaPart1.IsNULL()&&pRelaPart2.IsNULL())
		{	//以下六个参数需要在确认X轴是否与对称方向反向后再赋值(即考虑翻转X轴时带来的'X''dx'参数的反号)
			DESIGN_PLATE_ITEM* pSrcItem;
			if((pSrcItem=pSrcParamPlate->GetDesignItem('G'))!=NULL)
				params.SetValue('G',*pSrcItem);
			if((pSrcItem=pSrcParamPlate->GetDesignItem('E'))!=NULL)
				params.SetValue('E',*pSrcItem);
			if((pSrcItem=pSrcParamPlate->GetDesignItem('Z'))!=NULL)
				params.SetValue('Z',*pSrcItem);			//Z方向偏移量
			if((pSrcItem=pSrcParamPlate->GetDesignItem(KEY2C("dz")))!=NULL)
				params.SetValue(KEY2C("dz"),*pSrcItem);	//制弯面末段自定位基点的法向偏移量
			GEPOINT vAxisX=ucs.axis_x;
			PreCalPlateCS(vAxisX);
			//根据对称得到的X轴方向
			f3dPoint axis_x=GetMirVector(pSrcParamPlate->ucs.axis_x,mirmsg);
			//f3dPoint axis_z=GetMirVector(pSrcParamPlate->ucs.axis_z,msg);
			double X=0,dx=0,Z=0,dz=0;
			GetDesignItemValue('X',&X);	//X方向偏移量
			GetDesignItemValue(KEY2C("dx"),&dx);	//制弯面末段自定位基点的法向偏移量
			if(vAxisX*axis_x<0&&fabs(X)>0)
			{
				SetDesignItemValue('X',-1.0*X,NULL,PARAM_POS,0);
				SetDesignItemValue(KEY2C("dx"),-1.0*dx,NULL,PARAM_POS,0);
				m_cReverseAxisVec='X';
			}
			else
				m_cReverseAxisVec=0;
		}
		//得新提取更新肘板型肋板的旋转定位角　wjh-2017.6.27
		if( pDatumPart.IsHasPtr()&&pRelaPart1.IsHasPtr()&&pRelaPart2.IsNULL()&&
			pDatumPart->GetClassTypeId()==CLS_LINETUBE&&pRelaPart1->IsPlate())
		{	//肘板型肋板，由于环向角基准起始边可能不同，需要重新计算初始旋转角　wjh-2017.6.27
			CLDSLineTube *pDatumTube=pDatumPart.pTube;
			CSuperSmartPtr<CLDSGeneralPlate> pBottomPlate=pRelaPart1.pBasicPlate;//基准底板
			CLDSLineTube *pMirDatumTube = (CLDSLineTube*)pDatumTube->GetMirPart(mirmsg);
			CSuperSmartPtr<CLDSGeneralPlate> pMirBottomPlate = (CLDSGeneralPlate*)pBottomPlate->GetMirPart(mirmsg);
			if(pMirDatumTube&&pMirBottomPlate.IsHasPtr())
			{
				//更新肘板的定位方向角 wxc-2015.3.8
				long J=0;	//0.基准边;1.焊道线
				if(!GetDesignItemValue('J',&J))
				{
					if(pMirBottomPlate->GetClassTypeId()==CLS_PLATE)
						J=pMirBottomPlate.PlatePointer()->m_bAttachPartByWeldRoad=pBottomPlate.PlatePointer()->m_bAttachPartByWeldRoad;
					else if(pMirBottomPlate->GetClassTypeId()==CLS_PARAMPLATE)
					{
						if(pBottomPlate.ParamPlatePointer()->GetDesignItemValue('J',&J))
							pMirBottomPlate.ParamPlatePointer()->SetDesignItemValue('J',J,NULL,PARAM_POS,1);
					}
				}
				f3dPoint axis_mir_x=GetMirVector(pSrcParamPlate->ucs.axis_x,mirmsg);	//对称肘板的X轴方向
				//计算对称肘板在对称基准钢管上的定位角度
				double angle=CalPrjPlateRotAngle(pMirBottomPlate,pMirDatumTube,J,axis_mir_x);
				angle*=DEGTORAD_COEF;
				angle=ftoi(angle*100)/100;
#ifdef AFX_TARG_ENU_ENGLISH
				SetDesignItemValue('K',angle,"rib plate positioning direction angle",PARAM_POS);
#else
				SetDesignItemValue('K',angle,"肘板定位方向角",PARAM_POS);
#endif
			}
			else
			{
				if(pMirDatumTube==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.Log("tube's symmetrical tube not found!");
				if(!pMirBottomPlate.IsHasPtr())
					logerr.Log("tube's symmetrical tube not found!");
#else
					logerr.Log("钢管的对称钢管未找到!");
				if(!pMirBottomPlate.IsHasPtr())
					logerr.Log("钢板的对称钢板未找到!");
#endif
				//console.DispPartSet.DeleteNode(pMirPlate->handle);
				//console.DeletePart(pMirPlate->handle);
				return false;
			}
			//将肘板存放在法兰盘的依附构件链表里	
			if(pMirBottomPlate->GetClassTypeId()==CLS_PARAMPLATE)
				pMirBottomPlate.ParamPlatePointer()->AppendAttachPart(this);
		}
	}
	if(pSrcParamPlate->m_iParamType==TYPE_ENDPLATE||pSrcParamPlate->m_iParamType==TYPE_VERTPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(pSrcParamPlate->m_hPartWeldParent,CLS_PARAMPLATE);
		if(!pParamPlate)
		{
			//console.DispPartSet.DeleteNode(pMirPlate->handle);
			//console.DeletePart(pMirPlate->handle);
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("0X%X fail to meet symmetric requirements, can not finish symmtry!");
#else
			Log2File()->Log("0X%X钢板不满足对称条件，对称失败!",pSrcParamPlate->handle);
#endif
			return false;
		}
		CLDSParamPlate *pMirParamPlate=(CLDSParamPlate*)pParamPlate->GetMirPart(mirmsg);
		if(pMirParamPlate)
			m_hPartWeldParent=pMirParamPlate->handle;
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_BASEPLATE)
	{
		long hBaseNode,A,hBaseAngle,L;
		pSrcParamPlate->GetDesignItemValue('A',&hBaseNode);
		pSrcParamPlate->GetDesignItemValue('L',&hBaseAngle);
		MirPartHandle(pSrcParamPlate->BelongModel(),&A,hBaseNode,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&L,hBaseAngle,mirmsg);
		if(A>0x20)
			SetDesignItemValue('A',A,NULL,PARAM_POS,1);
		if(L>0x20)
			SetDesignItemValue('L',L,NULL,PARAM_POS,1);
	}
	else if(pSrcParamPlate->m_iParamType==TYPE_SHOEPLATE)
	{
		//获取母板上的依赖角钢集合，更新对称角钢的关联属性信息(正负头、切角)
		ATOM_LIST<THANDLE> angleHandleList;
		long hBaseNode=0,hBasePlate=0,hBaseAngle=0,hConnectShoePlate=0,hRayAngleH=0,hRayAngleX=0;	//不赋初始值，后续代码会误判存在有效值 wjh-2015.9.21
		if(pSrcParamPlate->GetDesignItemValue('D',&hBaseAngle) && hBaseAngle>0X20)
			angleHandleList.append(hBaseAngle);
		if(pSrcParamPlate->GetDesignItemValue(KEY2C("LH"),&hRayAngleH) && hRayAngleH>0X20)
			angleHandleList.append(hRayAngleH);
		if(pSrcParamPlate->GetDesignItemValue(KEY2C("LX"),&hRayAngleX) && hRayAngleX>0X20)
			angleHandleList.append(hRayAngleX);
		pSrcParamPlate->GetDesignItemValue('C',&hBasePlate);
		CLDSGeneralPlate* pBasePlate=(CLDSGeneralPlate*)pSrcParamPlate->BelongModel()->FromPartHandle(hBasePlate);
		hBaseNode=pBasePlate->m_hBaseNode;
		if(mirmsg.axis_flag==0x01||mirmsg.axis_flag==0x02)
		{	//靴板中的'W'参数表示靴板在底板的沿向坐标轴，在进行X/Y对称时，应X/Y调换 wjh-2015.9.21
			CLDSParamPlate::DESIGN_PARAM param;
			if(pSrcParamPlate->GetDesignItemValue(KEY2C("W"),&param) && (param.ToLong()==0||param.ToLong()==1))
				SetDesignItemValue(KEY2C("W"),1-param.ToLong(),NULL,param.cParamType,param.cValueType);
		}
		CLDSLineAngle *pLineAngle=NULL,*pDestAngle=NULL;
		for(THANDLE* pHandle=angleHandleList.GetFirst();pHandle;pHandle=angleHandleList.GetNext())
		{
			long hDestAngle=0;
			pLineAngle=(CLDSLineAngle*)pSrcParamPlate->BelongModel()->FromPartHandle(*pHandle,CLS_LINEANGLE);
			MirPartHandle(pSrcParamPlate->BelongModel(),&hDestAngle,*pHandle,mirmsg);
			pDestAngle=(CLDSLineAngle*)pSrcParamPlate->BelongModel()->FromPartHandle(hDestAngle,CLS_LINEANGLE);
			if(pLineAngle==NULL || pDestAngle==NULL)
				continue;
			CXhChar50 sKeyStr;
			if(pLineAngle->pStart->handle==hBaseNode)
				sKeyStr.Copy("oddStart");
			else
				sKeyStr.Copy("oddEnd");	
			PROPLIST_TYPE* pListProps=pLineAngle->GetSyncPropList();
			PROPLIST_ITEM* pItem=pListProps->GetPropItem(sKeyStr);
			MEMORY_BLOCK mpcl_copy1=pLineAngle->mpcl;
			MEMORY_BLOCK mpcl_copy2=pDestAngle->mpcl;
			pLineAngle->mpcl.SetBitState((WORD)(pItem->id-1),true);
			pDestAngle->mpcl.SetBitState((WORD)(pItem->id-1),true);
			pLineAngle->SyncMirPropTo(pDestAngle,mirmsg,sKeyStr);
			pLineAngle->mpcl=mpcl_copy1;
			pDestAngle->mpcl=mpcl_copy2;
		}
		//更新对称靴板的依赖构件信息
		long C,D,G,LH,LX;
		pSrcParamPlate->GetDesignItemValue('G',&hConnectShoePlate);
		MirPartHandle(pSrcParamPlate->BelongModel(),&C,hBasePlate,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&D,hBaseAngle,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&G,hConnectShoePlate,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&LH,hRayAngleH,mirmsg);
		MirPartHandle(pSrcParamPlate->BelongModel(),&LX,hRayAngleX,mirmsg);
		SetDesignItemValue('C',C,NULL,PARAM_POS,1);
		SetDesignItemValue('D',D,NULL,PARAM_POS,1);
		if(G>0x20)
			SetDesignItemValue('G',G,NULL,PARAM_POS,1);
		if(LH>0x20)
			SetDesignItemValue(KEY2C("LH"),LH,NULL,PARAM_POS,1);
		if(LX>0x20)
			SetDesignItemValue(KEY2C("LX"),LX,NULL,PARAM_POS,1);
	}
	return true;
}
//获取肋板子类型
int CLDSParamPlate::GetRibPlateSubType()
{
	if(m_iParamType==TYPE_ELBOWPLATE)
		return TYPE_RIB_ELBOW_PLATE;
	else if(m_iParamType!=TYPE_RIBPLATE)
		return 0;
	long hDatumPart=0,hAssPart1=0,hAssPart2=0;
	GetDesignItemValue('A',&hDatumPart);	//基准构件
	GetDesignItemValue('C',&hAssPart1);		//辅助构件1
	GetDesignItemValue('D',&hAssPart2);		//辅助构件2
	CLDSPart *pDatumPart=BelongModel()->FromPartHandle(abs(hDatumPart),CLS_LINETUBE,CLS_PLATE,CLS_PARAMPLATE);
	CLDSPart *pAssistantPart1=BelongModel()->FromPartHandle(abs(hAssPart1),CLS_PLATE,CLS_PARAMPLATE);
	CLDSPart *pAssistantPart2=BelongModel()->FromPartHandle(hAssPart2,CLS_PLATE,CLS_PARAMPLATE);
	if(pDatumPart==NULL)
		return 0;
	//根据依赖构件，智能判断肋板子类型
	int iRibPlateSubType=0;		//RIB_PLATE_SUB_TYPE
	if(pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
		iRibPlateSubType=TYPE_RIB_PLATE_ON_ANGLE;		//角钢肋板
	else if(pDatumPart->GetClassTypeId()==CLS_LINETUBE&&pAssistantPart1!=NULL&&pAssistantPart2==NULL&&pAssistantPart1->IsPlate())
		iRibPlateSubType=TYPE_RIB_ELBOW_PLATE;			//肘板型肋板
	else if(pDatumPart->GetClassTypeId()==CLS_LINETUBE&&pAssistantPart1==NULL&&pAssistantPart2==NULL)
		iRibPlateSubType=TYPE_RIB_PLATE_ON_TUBE;		//钢管上的普通矩形肋板
	else if(pDatumPart->IsPlate()&&pAssistantPart1==NULL&&pAssistantPart2==NULL)
		iRibPlateSubType=TYPE_RIB_PLATE_ON_PLATE;		//钢板上的普通矩形肋板
	else if(pDatumPart->IsPlate()&&pAssistantPart1!=NULL&&pAssistantPart1->IsPlate()&&pAssistantPart2==NULL)
		iRibPlateSubType=TYPE_RIB_CORNER_PLATE;			//角肋板
	else if(pAssistantPart1!=NULL&&pAssistantPart1->IsPlate()&&pAssistantPart2&&pAssistantPart2->IsPlate())
		iRibPlateSubType=TYPE_RIB_SLOT_PLATE;			//槽肋板
	return iRibPlateSubType;
}
//获取环板子类型
int CLDSParamPlate::GetCircularPlateSubType()
{
	if(m_iParamType!=TYPE_CIRRIBPLATE)
		return 0;
	long hDatumPart=0,hAssPart1=0,hAssPart2=0;
	GetDesignItemValue('C',&hDatumPart);	//基准构件
	GetDesignItemValue('E',&hAssPart1);		//辅助构件1
	GetDesignItemValue('F',&hAssPart2);		//辅助构件2
	CLDSPart *pDatumPart=BelongModel()->FromPartHandle(abs(hDatumPart),CLS_LINETUBE,CLS_SPHERE);
	CLDSPart *pAssistantPart1=BelongModel()->FromPartHandle(hAssPart1,CLS_PLATE,CLS_PARAMPLATE,CLS_LINEANGLE);
	CLDSPart *pAssistantPart2=BelongModel()->FromPartHandle(hAssPart2,CLS_PLATE,CLS_PARAMPLATE,CLS_LINETUBE);
	if(pDatumPart==NULL)
		return 0;
	//根据依赖构件，智能判断肋板子类型
	int iCirPlateSubType=0;
	double edge_type=0;
	GetDesignItemValue('G',&edge_type);	//自由边类型
	if(pAssistantPart1==NULL && pAssistantPart2==NULL)
	{
		int nPoleConnect=0;
		for(CONNECT_DIRECTION* pConnect=ConnDirectList.GetFirst();pConnect;pConnect=ConnDirectList.GetNext())
		{
			if(pConnect->IsProfilePole())
				nPoleConnect++;
		}
		if(nPoleConnect>0)
			iCirPlateSubType=TYPE_SPECIAL_CIR_PLATE;
		else
			iCirPlateSubType=TYPE_COMMON_CIR_PLATE;
	}
	else if(pAssistantPart1 && pAssistantPart2==NULL)
	{
		if(pAssistantPart1->IsAngle())
			iCirPlateSubType=TYPE_CIR_AISLE_PLATE;
		else if(edge_type>0)
			iCirPlateSubType=TYPE_2WL_CIR_PLATE;
		else
			iCirPlateSubType=TYPE_2WA_CIR_PLATE;
	}
	else if(pAssistantPart1 && pAssistantPart2)
	{
		if(edge_type>0)
			iCirPlateSubType=TYPE_3WL_CIR_PLATE;
		else
			iCirPlateSubType=TYPE_3WA_CIR_PLATE;
	}
	return iCirPlateSubType;
}
double CLDSParamPlate::CalWeldEdgeLength()	//计算钢板焊缝边长,mm
{
	double weldlength=CLDSGeneralPlate::CalWeldEdgeLength();
	//钢管端头插板
	if(IsInsertPlate())
	{
		double M=0;
		if(!m_bStdPart)
			GetDesignItemValue('M',&M);
		else 
			GetStdPartParamValue('M',&M);
		weldlength+=M*2;
	}
	return weldlength;
}
#endif
