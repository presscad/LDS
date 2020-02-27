// Bolt.cpp: implementation of the C3dLs class.
//wbt translation
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Hashtable.h"
#include "PropListItem.h"
#include "PartLib.h"
#include "FoundationParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLDSFoundation
//////////////////////////////////////////////////////////////////////////
// FOUNDATION_PARA
IMPLEMENT_PROP_FUNC(FOUNDATION_PARA)

FOUNDATION_PARA::FOUNDATION_PARA()
{
	biBasePlateType=1;		//0.交于上平面 1.交于下平面 2.交于中间 
	wiBasePlateThick=40;	//基板厚度
	wiBasePlateWidth=800;	//基板宽度
	wiBasePlateHoleD=40;	//基板孔径
	biLayoutType=0;			//地脚螺栓布置类型：0.4螺栓 1.8螺栓
	wiBoltD=42;				//地脚螺栓直径
	wiBoltSapce=S2=200;		//螺栓间距
	wiFoundationWidth=1200;	//基础宽度
	//塔脚板靴板/肋板设计参数
	cnMainAngleBoltNum = 5;
	cnMaxSingularRowBolts = 4;
	cnRows=0;	//主材连接螺栓数及布置排数,0.表示根据螺栓数及cnMaxSingularRowBolts判断单双排;1.单排;2.双排
	wiShoeThick = 16;
	wiRibThick = 10;
	wiRibMargin = 25;				//底板厚度、靴板厚度，肋板厚度、边距
	wRibWidth = 200;		//肋板宽度(底板焊接边)
	wRibOuterHeight = 200;	//肋板外侧高度
	wRibInnerHeight = 200;	//肋板内侧高度
}
void FOUNDATION_PARA::InitAnchorsLocation()
{
	if(biLayoutType==0)
	{	//四螺栓布置
		anchors.Append(LOCATION( wiBoltSapce, wiBoltSapce));
		anchors.Append(LOCATION(-wiBoltSapce, wiBoltSapce));
		anchors.Append(LOCATION(-wiBoltSapce,-wiBoltSapce));
		anchors.Append(LOCATION( wiBoltSapce,-wiBoltSapce));
	}
	else if(biLayoutType==1)
	{	//八螺栓布置
		anchors.Append(LOCATION( wiBoltSapce+S2, wiBoltSapce));
		anchors.Append(LOCATION( wiBoltSapce   , wiBoltSapce+S2));
		anchors.Append(LOCATION(-wiBoltSapce   , wiBoltSapce+S2));
		anchors.Append(LOCATION(-wiBoltSapce-S2, wiBoltSapce));
		anchors.Append(LOCATION(-wiBoltSapce-S2,-wiBoltSapce));
		anchors.Append(LOCATION(-wiBoltSapce   ,-wiBoltSapce-S2));
		anchors.Append(LOCATION( wiBoltSapce   ,-wiBoltSapce-S2));
		anchors.Append(LOCATION( wiBoltSapce+S2,-wiBoltSapce));
	}
}
CXhChar200 GetAnchorBoltCmbSizeStr()
{
	CXhChar200 szSizeList;
	int i,count=ANCHOR_BOLT::GetCount();
	for(i=0;i<count;i++)
	{
		const ANCHOR_BOLT* pAnchorBolt=ANCHOR_BOLT::RecordAt(i);
		szSizeList.Append(CXhChar16("%d",pAnchorBolt->d),'|');
	}
	return szSizeList;
}
void FOUNDATION_PARA::InitPropHashtable()
{
	int id = 1;
	const DWORD HASHTABLESIZE = 500;
	const DWORD STATUSHASHTABLESIZE = 50;
	propHashtable.Empty();
	propStatusHashtable.Empty();
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	AddPropItem("PrimaryAngle",PROPLIST_ITEM(id++,"(上)连接主材"));
	AddPropItem("ConnBoltCountOfPrimaryAngle",PROPLIST_ITEM(id++,"连接螺栓数","主材的连接螺栓数",""));
	AddPropItem("ConnBoltRowsOfPrimaryAngle",PROPLIST_ITEM(id++,"螺栓排数","主材连接螺栓布置排数","1.单排|2.双排"));
	AddPropItem("BasePlate",PROPLIST_ITEM(id++,"塔脚板"));
	AddPropItem("biBasePlateType",PROPLIST_ITEM(id++,"底板类型","底板类型","0.交于上平面|1.交于下平面|2.交于中间|"));
	AddPropItem("wiBasePlateHoleD",PROPLIST_ITEM(id++,"底板孔径"));
	AddPropItem("wiBasePlateThick",PROPLIST_ITEM(id++,"底板厚度"));
	AddPropItem("wiBasePlateWidth",PROPLIST_ITEM(id++,"底板宽度"));
	AddPropItem("wiShoePlateThick",PROPLIST_ITEM(id++,"靴板厚度"));
	AddPropItem("wiShoePlateWidth",PROPLIST_ITEM(id++,"靴板宽度"));
	AddPropItem("wiRibPlateThick",PROPLIST_ITEM(id++,"肋板厚度"));
	AddPropItem("wiRibPlateWidth",PROPLIST_ITEM(id++,"肋板宽度","底板焊接边宽度"));
	AddPropItem("wiRibPlateHeightOuter",PROPLIST_ITEM(id++,"肋板(外)高度","肋板高度"));
	AddPropItem("wiRibPlateHeightInner",PROPLIST_ITEM(id++,"肋板(内)高度","肋板高度"));
	AddPropItem("wiRibPlateMargin",PROPLIST_ITEM(id++,"肋板边距","肋板距底板外缘边距"));
	AddPropItem("FoundationPara",PROPLIST_ITEM(id++,"基础参数"));
	AddPropItem("biLayoutType",PROPLIST_ITEM(id++,"地脚螺栓布置",NULL,"4地脚螺栓|8地脚螺栓"));
	AddPropItem("wiBoltD",PROPLIST_ITEM(id++,"地脚螺栓直径M",NULL,GetAnchorBoltCmbSizeStr()));
	AddPropItem("ciAnchorBoltType",PROPLIST_ITEM(id++,"地脚螺栓类型:", NULL, "普通直柱|带弯勾"));
	AddPropItem("wiBoltSapce",PROPLIST_ITEM(id++,"螺栓半间距S1"));
	AddPropItem("wiFoundationWidth",PROPLIST_ITEM(id++,"基础直径"));
	AddPropItem("wiFoundationDepth",PROPLIST_ITEM(id++,"基础深度"));
}

int FOUNDATION_PARA::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("biBasePlateType")==id)
	{
		if(biBasePlateType==TYPE_BTM)
			sText.Copy("1.交于下平面");
		else if(biBasePlateType==TYPE_MID)
			sText.Copy("2.交于中间");
		else
			sText.Copy("0.交于上平面");
	}
	else if(GetPropID("wiBasePlateThick")==id)
		sText.Printf("%d",wiBasePlateThick);
	else if(GetPropID("wiBasePlateWidth")==id)
		sText.Printf("%d",wiBasePlateWidth);
	else if(GetPropID("wiBasePlateHoleD")==id)
		sText.Printf("%d",wiBasePlateHoleD);
	else if(GetPropID("wiBoltD")==id)
		sText.Printf("%d",this->wiBoltD);
	else if(GetPropID("biLayoutType")==id)
	{
		if(biLayoutType==1)
			sText.Copy("8地脚螺栓");
		else
			sText.Copy("4地脚螺栓");
	}
	else if(GetPropID("wiBoltSapce")==id)
		sText.Printf("%d",wiBoltSapce);
	else if(GetPropID("wiFoundationWidth")==id)
		sText.Printf("%d",wiFoundationWidth);
	else if (GetPropID("ConnBoltCountOfPrimaryAngle") == id)
		sText.Printf("%d", cnMainAngleBoltNum);
	else if (GetPropID("ConnBoltRowsOfPrimaryAngle") == id)
	{
		if (cnRows == 2)
			sText.Copy("2.双排");
		else
			sText.Copy("1.单排");
	}
	else if (GetPropID("wiShoePlateThick") == id)
		sText.Printf("%d", wiShoeThick);
	else if (GetPropID("wiRibPlateThick") == id)
		sText.Printf("%d", wiRibThick);
	else if (GetPropID("wiRibPlateWidth") == id)
		sText.Printf("%d", wRibWidth);
	else if (GetPropID("wiRibPlateHeightOuter") == id)
		sText.Printf("%d", wRibOuterHeight);
	else if (GetPropID("wiRibPlateHeightInner") == id)
		sText.Printf("%d", wRibInnerHeight);
	else if (GetPropID("wiRibPlateMargin") == id)
		sText.Printf("%d", wiRibMargin);
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
