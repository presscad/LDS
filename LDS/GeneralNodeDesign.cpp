// DesignWireHole.cpp: implementation of the CDesignWireHole class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "LDS.h"
#include "GlobalFunc.h"
#include "tower.h"
#include "env_def.h"
#include "SortFunc.h"
#include "LogFile.h"
#include "GeneralNodeDesign.h"
#include "CommonLjSmartDesign.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void GENERAL_NODEDESIGN_RULE::InitDefaultParams()
{
	wiSerial=0;
	blHasPlate=true;		//是否需要实体板连接
	cnBoltDirectConn=2;		//允许端(直接)连接螺栓数量
	cnBaseAngleBoltRows=2;	//基准材最多允许布置的螺栓排数
	blPriorMiddleEndLj=true;//这个属性应该被替换掉
}

INTELLI_MODEL_GUIDE gxModelGuide;
IMPLEMENT_PROP_FUNC(INTELLI_MODEL_GUIDE);
void INTELLI_MODEL_GUIDE::InitPropHashtable()
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(50);//CreateHashTable(HASHTABLESIZE);
	//通用连接设计规则
	AddPropItem("ciProcessLegacyConnections", PROPLIST_ITEM(id++, "已有连处理方式", "", "0.重新设计|1.跳过不设计"));
	AddPropItem("ciPlateProfileStyle", PROPLIST_ITEM(id++, "连板外形", "", "0.标准外形|1.定制外形"));
	//塔头横担连接设计
	AddPropItem("blIncArmHeadConnPlates", PROPLIST_ITEM(id++, "连接板设计", "包含塔头横担连接板设计","是|否"));
	AddPropItem("blAddWireAngle", PROPLIST_ITEM(id++, "挂线角钢设计", "需要时添加外贴挂线角钢", "是|否"));
	AddPropItem("blAddWirePoint", PROPLIST_ITEM(id++, "挂点连接设计", "添加挂点连接设计", "是|否"));
	//多呼高塔身连接设计
	AddPropItem("blIncSlopeTransitPlates", PROPLIST_ITEM(id++, "变坡板", "包含变坡板,塔头瓶口板","是|否"));
	AddPropItem("blIncArmConnPlates", PROPLIST_ITEM(id++, "塔身横担连接板", "包含塔身横担连接板", "是|否"));
	AddPropItem("blSectionCornerPlates", PROPLIST_ITEM(id++, "塔身隔面角连板", "塔身水平隔面连接角板", "是|否"));
	//多接腿设计
	AddPropItem("VPlate.ciProfileSchema", PROPLIST_ITEM(id++, "V面板外形类别", "V面板外形类别", "0.标准外形|1.定制外形|2.矩形化"));
	AddPropItem("VPlate.blVPlateInside", PROPLIST_ITEM(id++, "V面板搭接位置", "V面板相对角钢肢的位置", "贴于里皮|贴于外皮"));
	AddPropItem("VPlate.siSpecThick", PROPLIST_ITEM(id++, "V面板厚度", "0.表示自动计算板厚;>0.表示统一指定板厚"));
	AddPropItem("VPlate.siBendLineSpace", PROPLIST_ITEM(id++, "V面板火曲间隙", "V面板火曲间隙值"));
	AddPropItem("VPlate.ciTouchLineType", PROPLIST_ITEM(id++, "射线材交接方式", "V面板斜材交接方式", "0.火曲线|1.偏移准线"));
	AddPropItem("VPlate.siBendLineG", PROPLIST_ITEM(id++, "偏移准距"));
	AddPropItem("VPlate.cnMinBaseAngleBolts", PROPLIST_ITEM(id++, "基准材最少螺栓数", "V面板基准连接材上的最小螺栓数"));
}
int INTELLI_MODEL_GUIDE::GetPropValueStr(long id, char *valueStr, UINT nMaxStrBufLen/* = 100*/)
{	//根据属性ID得到属性值
	CXhChar100 szText;
	if (GetPropID("ciProcessLegacyConnections") == id)
	{
		if (this->ciProcessLegacyConnections == 0)
			szText.Copy("0.重新设计");
		else
			szText.Copy("1.跳过不设计");
	}
	else if (GetPropID("ciPlateProfileStyle") == id)
	{
		if (this->ciPlateProfileStyle == 0)
			szText.Copy("0.标准外形");
		else
			szText.Copy("1.定制外形");
	}
	//塔头横担连接设计
	else if (GetPropID("blIncArmHeadConnPlates") == id)
	{	//包含塔头横担连接板设计
		szText = (this->blIncArmHeadConnPlates) ? "是" : "否";
	}
	else if (GetPropID("blAddWireAngle") == id)
	{	//需要时添加外贴挂线角钢
		szText = (this->blAddWireAngle) ? "是" : "否";
	}
	else if (GetPropID("blAddWirePoint") == id)
	{	//添加挂点连接设计
		szText = (this->blAddWirePoint) ? "是" : "否";
	}
	//多呼高塔身连接设计
	else if (GetPropID("blIncSlopeTransitPlates") == id)
	{	//包含变坡板,塔头瓶口板
		szText = (this->blIncSlopeTransitPlates) ? "是" : "否";
	}
	else if (GetPropID("blIncArmConnPlates") == id)
	{	//包含塔身横担连接板
		szText = (this->blIncArmConnPlates) ? "是" : "否";
	}
	else if (GetPropID("blSectionCornerPlates") == id)
	{	//塔身水平隔面连接角板
		szText = (this->blSectionCornerPlates) ? "是" : "否";
	}
	//多接腿设计
	else if (GetPropID("VPlate.blVPlateInside") == id)
	{	//V面板相对角钢肢的位置
		szText = (multileg.blVPlateInside) ? "贴于里皮" : "贴于外皮";
	}
	else if (GetPropID("VPlate.cnMinBaseAngleBolts") == id)
	{	//V面板基准连接材上的最小螺栓数
		szText.Printf("%d", multileg.snMinBaseAngleBolts);
	}
	else if (GetPropID("VPlate.siBendLineSpace") == id)
	{	//V面板火曲间隙
		szText.Printf("%d", multileg.siBendLineSpace);
	}
	else if (GetPropID("VPlate.siBendLineG") == id)
	{	//V面板偏移准距
		szText.Printf("%d", multileg.siBendLineG);
	}
	else if (GetPropID("VPlate.ciProfileSchema") == id)
	{
		if (multileg.ciProfileSchema == 0)
			szText.Copy("0.标准外形");
		else if (multileg.ciProfileSchema == 1)
			szText.Copy("1.定制外形");
		else if (multileg.ciProfileSchema == 2)
			szText.Copy("2.矩形化");
	}
	else if (GetPropID("VPlate.siSpecThick") == id)
	{
		szText.Printf("%d", multileg.siSpecThick);
	}
	else if (GetPropID("VPlate.ciTouchLineType") == id)
	{
		if (multileg.ciTouchLineType == 0)
			szText.Copy("0.火曲线");
		else if (multileg.ciTouchLineType == 1)
			szText.Copy("1.偏移准线");
	}
	if (valueStr)
		StrCopy(valueStr, szText, nMaxStrBufLen);
	return szText.Length;
}
INTELLI_MODEL_GUIDE::INTELLI_MODEL_GUIDE()
{
	m_nMaxNodeCountOfManualDesign=5;
	ciProcessLegacyConnections = 1;
	ciPlateProfileStyle = 1;
	wiInitBranchNodeSpace=60;			//端连接节点拆分初始间距
	wiBranchNodeSpaceIncrementStep=20;	//端连接节点拆分间距增长步长
	blIncArmHeadConnPlates = blAddWireAngle = blAddWirePoint = true;
	blIncSlopeTransitPlates = blIncArmConnPlates = blSectionCornerPlates = true;
	multileg.snMinBaseAngleBolts = 2;
	multileg.blVPlateInside = true;
	multileg.ciProfileSchema = 1;
	multileg.ciTouchLineType = 0;
	multileg.siBendLineSpace = 10;
	multileg.siBendLineG = 0;
	multileg.siSpecThick = 0;
}
void INTELLI_MODEL_GUIDE::InitNodeDesignRule()
{
	GENERAL_NODEDESIGN_RULE xarrRules1[5]={
		{1,"L_1LB1",false,1,1,true},
		{2,"L_1LB2",false,1,1,true},
		{3,"L_1LE1",false,1,1,true},
		{4,"L_1LE2",false,1,1,true},
		{5,"L_1LO1",false,1,1,true},
	};
	GENERAL_NODEDESIGN_RULE xarrRules2[9]={
		{1,"L_2LB1",false,1,1,true},
		{2,"L_2LB2",false,1,1,true},
		{3,"L_2LB3",false,1,1,true},
		{4,"L_2LB4",false,1,1,true},
		{5,"L_2LE1",false,1,1,true},
		{6,"L_2LE2",false,1,1,true},
		{7,"L_2LC1",false,1,1,true},
		{8,"L_2LC2",false,1,1,true},
		{9,"L_2LO1",false,1,1,true},
	};
	GENERAL_NODEDESIGN_RULE xarrRules3[6]={
		{1,"L_3LB1",false,1,1,true},
		{2,"L_3LB2",false,1,1,true},
		{3,"L_3LC1",false,1,1,true},
		{4,"L_3LC2",false,1,1,true},
		{5,"L_3LC3",false,1,1,true},
		{6,"L_3LO1",false,1,1,true},
	};
}
void INTELLI_MODEL_GUIDE::CopyFrom(INTELLI_MODEL_GUIDE* pModelGuide)
{
	ciProcessLegacyConnections = pModelGuide->ciProcessLegacyConnections;
	m_nMaxNodeCountOfManualDesign = pModelGuide->m_nMaxNodeCountOfManualDesign;
	ciPlateProfileStyle = pModelGuide->ciPlateProfileStyle;
	blIncArmHeadConnPlates = pModelGuide->blIncArmHeadConnPlates;
	blAddWireAngle = pModelGuide->blAddWireAngle;
	blAddWirePoint = pModelGuide ->blAddWirePoint;
	blIncSlopeTransitPlates = pModelGuide->blIncSlopeTransitPlates;
	blIncArmConnPlates = pModelGuide->blIncArmConnPlates;
	blSectionCornerPlates = pModelGuide ->blSectionCornerPlates;
	multileg.snMinBaseAngleBolts = pModelGuide ->multileg.snMinBaseAngleBolts;
	multileg.blVPlateInside = pModelGuide ->multileg.blVPlateInside;
	multileg.ciProfileSchema = pModelGuide->multileg.ciProfileSchema;
	multileg.ciTouchLineType = pModelGuide->multileg.ciTouchLineType;
	multileg.siBendLineSpace = pModelGuide->multileg.siBendLineSpace;
	multileg.siBendLineG = pModelGuide->multileg.siBendLineG;
	multileg.siSpecThick = pModelGuide->multileg.siSpecThick;
}
void INTELLI_MODEL_GUIDE::CopyTo(INTELLI_MODEL_GUIDE* pModelGuide)
{
	pModelGuide->ciProcessLegacyConnections = ciProcessLegacyConnections;
	pModelGuide->m_nMaxNodeCountOfManualDesign = m_nMaxNodeCountOfManualDesign;
	pModelGuide->ciPlateProfileStyle = ciPlateProfileStyle;
	pModelGuide->blIncArmHeadConnPlates = blIncArmHeadConnPlates;
	pModelGuide->blAddWireAngle = blAddWireAngle;
	pModelGuide->blAddWirePoint = blAddWirePoint;
	pModelGuide->blIncSlopeTransitPlates = blIncSlopeTransitPlates;
	pModelGuide->blIncArmConnPlates = blIncArmConnPlates;
	pModelGuide->blSectionCornerPlates = blSectionCornerPlates;
	pModelGuide->multileg.snMinBaseAngleBolts = multileg.snMinBaseAngleBolts;
	pModelGuide->multileg.blVPlateInside = multileg.blVPlateInside;
	pModelGuide->multileg.ciProfileSchema = multileg.ciProfileSchema;
	pModelGuide->multileg.ciTouchLineType = multileg.ciTouchLineType;
	pModelGuide->multileg.siBendLineSpace = multileg.siBendLineSpace;
	pModelGuide->multileg.siBendLineG = multileg.siBendLineG;
	pModelGuide->multileg.siSpecThick = multileg.siSpecThick;
}
